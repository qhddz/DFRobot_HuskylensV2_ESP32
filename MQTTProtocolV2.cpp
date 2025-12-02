#include "MQTTProtocolV2.h"
#include "esp_idf_version.h" // check IDF version
#include <ArduinoJson.h>
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
int8_t MQTTProtocolV2::getResult(eAlgorithm_t algo) {}
uint8_t MQTTProtocolV2::learn(eAlgorithm_t algo) {}
uint8_t MQTTProtocolV2::learnBlock(eAlgorithm_t algo, int16_t x, int16_t y,
                                   int16_t width, int16_t height) {}
bool MQTTProtocolV2::forgot(eAlgorithm_t algo) { return true; }
bool MQTTProtocolV2::switchAlgorithm(eAlgorithm_t algo) { return true; }
String MQTTProtocolV2::takePhoto(eResolution_t resolution) { return String(); }
String MQTTProtocolV2::takeScreenshot() { return String(); }

bool MQTTProtocolV2::drawUniqueRect(uint32_t color, uint8_t lineWidth,
                                    int16_t x, int16_t y, int16_t width,
                                    int16_t height) {
  return true;
}
bool MQTTProtocolV2::drawRect(uint32_t color, uint8_t lineWidth, int16_t x,
                              int16_t y, int16_t width, int16_t height) {
  return true;
}
bool MQTTProtocolV2::clearRect() { return true; }
bool MQTTProtocolV2::drawText(uint32_t color, uint8_t fontSize, int16_t x,
                              int16_t y, String text) {
  return true;
}
bool MQTTProtocolV2::clearText() { return true; }
bool MQTTProtocolV2::saveKnowledges(eAlgorithm_t algo, uint8_t knowledgeID) {
  return true;
}
bool MQTTProtocolV2::loadKnowledges(eAlgorithm_t algo, uint8_t knowledgeID) {
  return true;
}

bool MQTTProtocolV2::playMusic(String name, int16_t volume) {
  log_i("%s(): %d", __func__, __LINE__);
  std::string msg = R"({
        "cmd": "play_music",
        "algorithm": 0,
        "volume": 50,
        "filename": "abc.mp3",
        "correlation_id": "1234"
    })";
  bool ret = sendAndWait(msg);
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
    const char *cmd = root["cmd"];
    log_i("cmd: %s", cmd);
    if (strcmp(pret, "success") == 0) {
      return true;
    }
  }
  return false;
}
bool MQTTProtocolV2::sendAndWait(std::string &command) {
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
  return true;
}

bool MQTTProtocolV2::doSetMultiAlgorithm(eAlgorithm_t algo0, eAlgorithm_t algo1,
                                         eAlgorithm_t algo2) {
  return true;
}
bool MQTTProtocolV2::setMultiAlgorithmRatio(int8_t ratio0, int8_t ratio1,
                                            int8_t ratio2) {
  return true;
}

bool MQTTProtocolV2::getAlgoParamBool(eAlgorithm_t algo, String key) {
  return true;
}
float MQTTProtocolV2::getAlgoParamFloat(eAlgorithm_t algo, String key) {
  return true;
}
String MQTTProtocolV2::getAlgoParamString(eAlgorithm_t algo, String key) {
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
  return true;
}
bool MQTTProtocolV2::stopRecording(eMediaType_t mediaType) { return true; }

void onMqttConnect(esp_mqtt_client_handle_t client) {
  log_i("%s(): %d", __func__, __LINE__);
  if (mqttClient.isMyTurn(client)) // can be omitted if only one client
  {
    connected = true;
    log_i("%s(): %d", __func__, __LINE__);
    mqttClient.subscribe(subscribeTopic, [](const std::string &payload) {
      log_i("%s: %s", subscribeTopic, payload.c_str());
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