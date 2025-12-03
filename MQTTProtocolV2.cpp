#include "MQTTProtocolV2.h"
#include "esp_idf_version.h" // check IDF version
#include <ArduinoJson.h>
#include <map>
String server = "mqtt://192.168.2.102:1883";

char *subscribeTopic = "usr/response";
char *publishTopic = "usr/cmd";

static String host = "192.168.2.102";
static uint16_t port = 1883;
static String username = "huskylens";
static String password = "dfrobot";

static volatile bool responseReady;
static std::string responsePayload;
static bool volatile connected = false;

ESP32MQTTClient mqttClient;
void subscribeCB(const std::string &topic, const std::string &payload) {
  log_i("Global callback: %s: %s", topic.c_str(), payload.c_str());
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(payload.c_str());
  if (!root.success()) {
    log_e("deserializeJson() failed");
    return;
  }
  const char *cmd = root["cmd"];
  Serial.println(cmd);
}

MQTTProtocolV2::MQTTProtocolV2() {};
void gbegin(String _host, uint16_t _port, String _username, String _password) {
  host = _host;
  port = _port;
  username = _username;
  password = _password;
  server = "mqtt://" + host + ":" + String(port);
  log_i("%s(): %d host: %s, port: %d, username: %s, password: %s", __func__,
        __LINE__, host.c_str(), port, username.c_str(), password.c_str());
  mqttClient.enableDebuggingMessages();
  mqttClient.setURI(server.c_str(), username.c_str(), password.c_str());
  mqttClient.enableLastWillMessage("lwt", "I am going offline");
  mqttClient.setKeepAlive(30);
  mqttClient.setOnMessageCallback(
      [](const std::string &topic, const std::string &payload) {
        responsePayload = payload;
        responseReady = true;
      });
  log_i("%s(): %d", __func__, __LINE__);
  mqttClient.loopStart();
  log_i("%s(): %d", __func__, __LINE__);
}

void playMusic(String filename, int16_t volume) {
  log_i("%s(): %d", __func__, __LINE__);
  std::string msg = R"({
        "cmd": "play_music",
        "algorithm": 0,
        "volume": 50,
        "filename": "abc.mp3",
        "correlation_id": "1234"
    })";
  mqttClient.publish(publishTopic, msg, 0, false);
}

bool MQTTProtocolV2::begin(String &_host, uint16_t _port, String &_username,
                           String &_password) {
  gbegin(_host, _port, _username, _password);
  uint32_t start = millis();
  while (!connected) {
    if (millis() - start > 5000) {
      return false; // 超时
    }
    delay(1);
  }
  return true;
}
void MQTTProtocolV2::timerBegin() {}
bool MQTTProtocolV2::timerAvailable() {}
bool MQTTProtocolV2::protocolAvailable() {}
bool MQTTProtocolV2::wait(String command) {}
bool MQTTProtocolV2::knock(void) { return true; }
int8_t MQTTProtocolV2::getResult(eAlgorithm_t algo) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "get_result";
  root["algorithm"] = (uint8_t)algo;
  root["correlation_id"] = "1234";
  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);
  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());
    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return false;
    }
    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    const char *cmd = resp["cmd"];
    log_i("cmd: %s", cmd);
    if (strcmp(pret, "success") == 0) {
      JsonArray &results = resp["results"];
      for (uint8_t i = 0; i < results.size(); i++) {
        JsonObject &item = results[i];
        int algorithm = item["algorithm"];
        if (algorithm == ALGORITHM_FACE_RECOGNITION) {
          result_tmp[i] = new Result(item);
        } else if (algorithm == ALGORITHM_HAND_RECOGNITION) {
          result_tmp[i] = new Result(item);
        } else if (algorithm == ALGORITHM_POSE_RECOGNITION) {
          result_tmp[i] = new Result(item);
        } else {
          result_tmp[i] = new Result(item);
        }
      }
      return true;
    }
  }
  return false;
}
uint8_t MQTTProtocolV2::learn(eAlgorithm_t algo) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "learn";
  root["algorithm"] = (uint8_t)algo;
  root["correlation_id"] = "1234";
  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);
  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());
    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return false;
    }
    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}
uint8_t MQTTProtocolV2::learnBlock(eAlgorithm_t algo, int16_t x, int16_t y,
                                   int16_t width, int16_t height) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "learn_block";
  root["algorithm"] = (uint8_t)algo;
  root["x"] = x;
  root["y"] = y;
  root["width"] = width;
  root["height"] = height;
  root["correlation_id"] = "1234";
  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);
  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());
    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return false;
    }
    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}
bool MQTTProtocolV2::forgot(eAlgorithm_t algo) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "forget";
  root["algorithm"] = (uint8_t)algo;
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);

  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());
    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return false;
    }
    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}
bool MQTTProtocolV2::doSwitchAlgorithm(eAlgorithm_t algo) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "switch_algorithm";
  root["algorithm"] = (uint8_t)algo;
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);

  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());
    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return false;
    }
    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}

bool MQTTProtocolV2::drawUniqueRect(uint32_t color, uint8_t lineWidth,
                                    int16_t x, int16_t y, int16_t width,
                                    int16_t height) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "draw_unique_rect";
  JsonArray &arr = root.createNestedArray("color");
  arr.add((color >> 16) & 0xFF);
  arr.add((color >> 8) & 0xFF);
  arr.add(color & 0xFF);
  root["line_width"] = lineWidth;
  root["x"] = x;
  root["y"] = y;
  root["width"] = width;
  root["height"] = height;
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);

  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());
    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return false;
    }
    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}
bool MQTTProtocolV2::drawRect(uint32_t color, uint8_t lineWidth, int16_t x,
                              int16_t y, int16_t width, int16_t height) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "draw_rect";
  JsonArray &arr = root.createNestedArray("color");
  arr.add((color >> 16) & 0xFF);
  arr.add((color >> 8) & 0xFF);
  arr.add(color & 0xFF);
  root["line_width"] = lineWidth;
  root["x"] = x;
  root["y"] = y;
  root["width"] = width;
  root["height"] = height;
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);

  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());
    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return false;
    }
    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}
bool MQTTProtocolV2::clearRect() {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "clear_rect";
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);

  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());
    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return false;
    }
    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}
bool MQTTProtocolV2::drawText(uint32_t color, uint8_t fontSize, int16_t x,
                              int16_t y, String text) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "draw_text";
  root["font_size"] = fontSize;
  root["text"] = text;

  JsonArray &arr = root.createNestedArray("color");
  arr.add((color >> 16) & 0xFF);
  arr.add((color >> 8) & 0xFF);
  arr.add(color & 0xFF);

  root["x"] = x;
  root["y"] = y;
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);

  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());
    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return false;
    }
    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}
bool MQTTProtocolV2::clearText() {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "clear_text";
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);

  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());
    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return false;
    }
    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}
bool MQTTProtocolV2::saveKnowledges(eAlgorithm_t algo, uint8_t knowledgeID) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "save_knowledges";
  root["algorithm"] = (uint8_t)algo;
  root["knowledge_id"] = knowledgeID;
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);

  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);
  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());
    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return false;
    }
    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}
bool MQTTProtocolV2::loadKnowledges(eAlgorithm_t algo, uint8_t knowledgeID) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "load_knowledges";
  root["algorithm"] = (uint8_t)algo;
  root["knowledge_id"] = knowledgeID;
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);

  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);
  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());
    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return false;
    }
    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}

bool MQTTProtocolV2::playMusic(String name, int16_t volume) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "play_music";
  root["algorithm"] = 0;
  root["volume"] = volume;
  root["filename"] = name;
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);

  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(responsePayload.c_str());

    if (!root.success()) {
      log_e("deserializeJson() failed");
      return false;
    }

    const char *pret = root["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}
bool MQTTProtocolV2::sendAndWait(String &command) {
  log_i("%s(): %d command: %s", __func__, __LINE__, command.c_str());
  responseReady = false;
  responsePayload = "";
  mqttClient.publish(publishTopic, command.c_str(), 0, false);
  uint32_t start = millis();
  while (!responseReady) {
    if (millis() - start > 5000) {
      return false; // 超时
    }
    delay(1);
  }
  return true;
}
bool MQTTProtocolV2::stopMusic() { return true; }
bool MQTTProtocolV2::setNameByID(eAlgorithm_t algo, uint8_t id, String name) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "set_name_by_id";
  root["algorithm"] = (uint8_t)algo;
  root["id"] = id;
  root["name"] = name;
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);

  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(responsePayload.c_str());

    if (!root.success()) {
      log_e("deserializeJson() failed");
      return false;
    }

    const char *pret = root["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}

bool MQTTProtocolV2::doSetMultiAlgorithm(eAlgorithm_t algo0, eAlgorithm_t algo1,
                                         eAlgorithm_t algo2) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "set_multi_algorithm";
  JsonArray &arr = root.createNestedArray("algorithms");
  if (algo0 != ALGORITHM_ANY)
    arr.add((uint8_t)algo0);
  if (algo1 != ALGORITHM_ANY)
    arr.add((uint8_t)algo1);
  if (algo2 != ALGORITHM_ANY)
    arr.add((uint8_t)algo2);
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);

  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(responsePayload.c_str());

    if (!root.success()) {
      log_e("deserializeJson() failed");
      return false;
    }

    const char *pret = root["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}
bool MQTTProtocolV2::setMultiAlgorithmRatio(int8_t ratio0, int8_t ratio1,
                                            int8_t ratio2) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "set_multi_ratio";
  JsonArray &arr = root.createNestedArray("ratios");
  if (ratio0 != -1)
    arr.add(ratio0);
  if (ratio1 != -1)
    arr.add(ratio1);
  if (ratio2 != -1)
    arr.add(ratio2);
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);

  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(responsePayload.c_str());

    if (!root.success()) {
      log_e("deserializeJson() failed");
      return false;
    }

    const char *pret = root["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}

bool MQTTProtocolV2::getAlgoParamBool(eAlgorithm_t algo, String key) {
  log_i("%s(): %d", __func__, __LINE__);
  bool ret = false;
  {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["cmd"] = "get_algorithm_params";
    root["algorithm"] = (uint8_t)algo;
    root["correlation_id"] = "1234";

    String jsonStr;
    root.printTo(jsonStr);
    ret = sendAndWait(jsonStr);
    log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);
  }
  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());

    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return false;
    }
    if (!resp.containsKey(key.c_str()) || !resp[key.c_str()].is<bool>()) {
      log_e("params %s not found or not bool type", key.c_str());
      return false;
    }

    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      responsePayload = "";
      return resp[key.c_str()]["value"].as<bool>();
    }
  }
  return false;
}
float MQTTProtocolV2::getAlgoParamFloat(eAlgorithm_t algo, String key) {
  log_i("%s(): %d", __func__, __LINE__);
  bool ret;
  {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["cmd"] = "get_algorithm_params";
    root["algorithm"] = (uint8_t)algo;
    root["correlation_id"] = "1234";

    String jsonStr;
    root.printTo(jsonStr);
    ret = sendAndWait(jsonStr);
    log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);
  }
  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());

    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return false;
    }
    if (!resp.containsKey(key.c_str()) || !resp[key.c_str()].is<float>()) {
      log_e("params %s not found or not float type", key.c_str());
      return false;
    }

    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return resp[key.c_str()]["value"].as<float>();
    }
  }
  return 0.0;
}
String MQTTProtocolV2::getAlgoParamString(eAlgorithm_t algo, String key) {
  log_i("%s(): %d", __func__, __LINE__);
  bool ret;
  {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["cmd"] = "get_algorithm_params";
    root["algorithm"] = (uint8_t)algo;
    root["correlation_id"] = "1234";
    String jsonStr;
    root.printTo(jsonStr);
    ret = sendAndWait(jsonStr);
  }
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);
  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());

    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return String();
    }
    if (!resp.containsKey(key.c_str()) || !resp[key.c_str()].is<String>()) {
      log_e("params %s not found or not string type", key.c_str());
      return String();
    }

    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return resp[key.c_str()]["value"].as<String>();
    }
  }
  return String();
}

bool MQTTProtocolV2::setAlgoParamBool(eAlgorithm_t algo, String key,
                                      bool value) {
  return true;
}
bool MQTTProtocolV2::setAlgoParamFloat(eAlgorithm_t algo, String key,
                                       float value) {
  return true;
}
bool MQTTProtocolV2::setAlgoParamString(eAlgorithm_t algo, String key,
                                        String value) {
  return true;
}
bool MQTTProtocolV2::updateAlgoParams(eAlgorithm_t algo) { return true; }
bool MQTTProtocolV2::startRecording(eMediaType_t mediaType, int16_t duration,
                                    String filename, eResolution_t resolution) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["filename"] = filename;
  if (mediaType == MEDIA_TYPE_AUDIO) {
    root["cmd"] = "start_recording_audio";
  } else if (mediaType == MEDIA_TYPE_VIDEO) {
    root["cmd"] = "start_recording_video";
    root["resolution"] = "default";
  }
  root["duration"] = duration;
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);

  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(responsePayload.c_str());

    if (!root.success()) {
      log_e("deserializeJson() failed");
      return false;
    }

    const char *pret = root["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}
bool MQTTProtocolV2::stopRecording(eMediaType_t mediaType) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (mediaType == MEDIA_TYPE_AUDIO) {
    root["cmd"] = "stop_recording_audio";
  } else if (mediaType == MEDIA_TYPE_VIDEO) {
    root["cmd"] = "stop_recording_video";
  }
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);

  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(responsePayload.c_str());

    if (!root.success()) {
      log_e("deserializeJson() failed");
      return false;
    }

    const char *pret = root["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}

String MQTTProtocolV2::takePhoto(eResolution_t resolution) {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();

  std::map<eResolution_t, String> resolutionMap = {
      {RESOLUTION_DEFAULT, "640x480"},
      {RESOLUTION_640x480, "640x480"},
      {RESOLUTION_1280x720, "1280x720"},
      {RESOLUTION_1920x1080, "1920x1080"},
  };
  root["cmd"] = "take_photo";
  root["resolution"] = resolutionMap[resolution];
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);

  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());

    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return String();
    }

    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return resp["filename"].as<String>();
    }
  }
  return String();
}

String MQTTProtocolV2::takeScreenshot() {
  log_i("%s(): %d", __func__, __LINE__);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["cmd"] = "take_screenshot";
  root["correlation_id"] = "1234";

  String jsonStr;
  root.printTo(jsonStr);
  bool ret = sendAndWait(jsonStr);
  log_i("%s(): %d ret: %d", __func__, __LINE__, (int)ret);

  if (ret) {
    log_i("%s(): %d response: %s", __func__, __LINE__, responsePayload.c_str());
    DynamicJsonBuffer jsonBuffer;
    JsonObject &resp = jsonBuffer.parseObject(responsePayload.c_str());

    if (!resp.success()) {
      log_e("deserializeJson() failed");
      return String();
    }

    const char *pret = resp["ret"];
    log_i("ret: %s", pret);
    if (strcmp(pret, "success") == 0) {
      return resp["filename"].as<String>();
    }
  }
  return String();
}

void onMqttConnect(esp_mqtt_client_handle_t client) {
  log_i("%s(): %d", __func__, __LINE__);
  if (mqttClient.isMyTurn(client)) // can be omitted if only one client
  {
    connected = true;
    log_i("%s(): %d", __func__, __LINE__);
    mqttClient.subscribe(subscribeTopic, [](const std::string &payload) {
      log_i("payload length = %d", payload.length());
    });
  }
}

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
esp_err_t handleMQTT(esp_mqtt_event_handle_t event) {
  mqttClient.onEventCallback(event);
  return ESP_OK;
}
#else  // IDF CHECK
void handleMQTT(void *handler_args, esp_event_base_t base, int32_t event_id,
                void *event_data) {
  auto *event = static_cast<esp_mqtt_event_handle_t>(event_data);
  mqttClient.onEventCallback(event);
  if (event->event_id == MQTT_EVENT_CONNECTED) {
    connected = true;
  } else if (event->event_id == MQTT_EVENT_DISCONNECTED) {
    connected = false;
  }
}
#endif // // IDF CHECK