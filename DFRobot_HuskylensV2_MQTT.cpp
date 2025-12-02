#include <DFRobot_HuskylensV2_MQTT.h>

HuskylensV2_MQTT::HuskylensV2_MQTT() {}
HuskylensV2_MQTT::~HuskylensV2_MQTT() {}

bool HuskylensV2_MQTT::begin(String host, uint16_t port, String username,
                             String password) {
  DBG("\n");
  MQTTProtocolV2::begin(host, port, username, password);
  return knock();
}

int8_t HuskylensV2_MQTT::getResult(eAlgorithm_t algo) {
  return getResult(algo);
}

bool HuskylensV2_MQTT::available(eAlgorithm_t algo) {
  bool ret = false;
  algo = toRealID(algo);

  for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i] != NULL)
      if (!result[algo][i]->used) {
        DBG(i);
        ret = true;
        break;
      }
  }

  return ret;
}

Result *HuskylensV2_MQTT::popCachedResult(eAlgorithm_t algo) {
  DBG("\n");
  algo = toRealID(algo);

  for (int8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i]) {
      if (result[algo][i]->used) {
        continue;
      }

      result[algo][i]->used = 1;
      DBG_PRINT("return ");
      DBG_PRINTLN((unsigned long)result[algo][i]);
      DBG(i);
      return result[algo][i];
    }
  }

  return NULL;
}

Result *HuskylensV2_MQTT::getCachedCenterResult(eAlgorithm_t algo) {
  DBG("\n");
  algo = toRealID(algo);

  int8_t centerIndex = -1;
  int32_t minLen = 0x0FFFFFFF;
  for (int8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i]) {
      int32_t len = SQUARE(result[algo][i]->xCenter - LCD_WIDTH / 2) +
                    SQUARE(result[algo][i]->yCenter - LCD_HEIGHT / 2);
      if (len < minLen) {
        minLen = len;
        centerIndex = i;
      }
    }
  }
  if (centerIndex != -1) {
    return result[algo][centerIndex];
  }
  return NULL;
}

Result *HuskylensV2_MQTT::getCachedResultByIndex(eAlgorithm_t algo,
                                                 int16_t index) {
  DBG("\n");
  algo = toRealID(algo);

  if (index >= MAX_RESULT_NUM) {
    return NULL;
  }
  return result[algo][index];
}

Result *HuskylensV2_MQTT::getCachedResultByID(eAlgorithm_t algo, int16_t ID) {
  DBG("\n");
  algo = toRealID(algo);

  for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i] == NULL) {
      continue;
    }
    if (result[algo][i]->ID == ID) {
      return result[algo][i];
    }
  }
  return NULL;
}

int16_t HuskylensV2_MQTT::getCachedResultNum(eAlgorithm_t algo) {
  DBG("\n");
  int16_t count = 0;
  algo = toRealID(algo);

  for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i]) {
      count++;
    }
  }
  return count;
}

int16_t HuskylensV2_MQTT::getCachedResultLearnedNum(eAlgorithm_t algo) {
  DBG("\n");
  int16_t count = 0;
  algo = toRealID(algo);

  for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i] && result[algo][i]->ID) {
      count++;
    }
  }
  return count;
}

int16_t HuskylensV2_MQTT::getCachedResultNumByID(eAlgorithm_t algo,
                                                 uint8_t id) {
  DBG("\n");
  int16_t count = 0;
  algo = toRealID(algo);

  for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i] && (id == result[algo][i]->ID)) {
      count++;
    }
  }
  return count;
}

Result *HuskylensV2_MQTT::getCachedIndexResultByID(eAlgorithm_t algo,
                                                   uint8_t id, uint8_t index) {
  DBG("\n");
  Result *rlt = NULL;
  uint8_t _index = 0;
  algo = toRealID(algo);

  for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i] && (id == result[algo][i]->ID)) {
      if (_index == index) {
        return result[algo][i];
      }
      _index++;
    }
  }
  return rlt;
}

int16_t HuskylensV2_MQTT::getCachedResultMaxID(eAlgorithm_t algo) {
  return maxID[toRealID(algo)];
}

Result *HuskylensV2_MQTT::getCurrentBranch(eAlgorithm_t algo) {
  DBG("\n");
  Result *rlt = NULL;
  algo = toRealID(algo);

  if (result[algo][0] && (result[algo][0]->level == 1)) {
    return result[algo][0];
  }
}

int8_t HuskylensV2_MQTT::getUpcomingBranchCount(eAlgorithm_t algo) {
  DBG("\n");
  int8_t count = 0;
  algo = toRealID(algo);

  for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i]) {
      count++;
    }
  }
  return count > 0 ? count - 1 : 0;
}

Result *HuskylensV2_MQTT::getBranch(eAlgorithm_t algo, int16_t index) {
  DBG("\n");
  Result *rlt = NULL;
  index++;
  algo = toRealID(algo);

  for (uint8_t i = 1; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i]) {
      if (i == index) {
        rlt = result[algo][i];
        break;
      }
    }
  }
  return rlt;
}

bool HuskylensV2_MQTT::switchAlgorithm(eAlgorithm_t algo) {
  DBG("\n");
  bool ret;
  customAlgoNum = 0;
  customId[0] = 0;
  customId[1] = 0;
  customId[2] = 0;

  ret = switchAlgorithm(algo);
  if (ret) {
    if (algo >= ALGORITHM_CUSTOM_BEGIN) {
      customAlgoNum = 1;
      customId[customAlgoNum++] = algo;
    }
  }
  return ret;
}

bool HuskylensV2_MQTT::setMultiAlgorithm(eAlgorithm_t algo0, eAlgorithm_t algo1,
                                         eAlgorithm_t algo2) {
  DBG("\n");
  customAlgoNum = 0;
  memset(customId, 0, sizeof(customId));
  if (algo0 >= ALGORITHM_CUSTOM_BEGIN) {
    customId[customAlgoNum++] = algo0;
  }
  if (algo1 >= ALGORITHM_CUSTOM_BEGIN) {
    customId[customAlgoNum++] = algo1;
  }
  if (algo2 >= ALGORITHM_CUSTOM_BEGIN) {
    customId[customAlgoNum++] = algo2;
  }

  return doSetMultiAlgorithm(algo0, algo1, algo2);
}

uint8_t HuskylensV2_MQTT::learn(eAlgorithm_t algo) {
  DBG("\n");
  return learn(algo);
}
uint8_t HuskylensV2_MQTT::learnBlock(eAlgorithm_t algo, int16_t x, int16_t y,
                                     int16_t width, int16_t height) {
  DBG("\n");
  return learnBlock(algo, x, y, width, height);
}
bool HuskylensV2_MQTT::forgot(eAlgorithm_t algo) {
  DBG("\n");
  return forgot(algo);
}
String HuskylensV2_MQTT::takePhoto(eResolution_t resolution) {
  DBG("\n");
  return takePhoto(resolution);
}
String HuskylensV2_MQTT::takeScreenshot() {
  DBG("\n");
  return takeScreenshot();
}

bool HuskylensV2_MQTT::drawUniqueRect(uint32_t color, uint8_t lineWidth,
                                      int16_t x, int16_t y, int16_t width,
                                      int16_t height) {
  DBG("\n");
  return drawUniqueRect(color, lineWidth, x, y, width, height);
}
bool HuskylensV2_MQTT::drawRect(uint32_t color, uint8_t lineWidth, int16_t x,
                                int16_t y, int16_t width, int16_t height) {
  DBG("\n");
  return drawRect(color, lineWidth, x, y, width, height);
}
bool HuskylensV2_MQTT::clearRect() {
  DBG("\n");
  return clearRect();
}
bool HuskylensV2_MQTT::drawText(uint32_t color, uint8_t fontSize, int16_t x,
                                int16_t y, String text) {
  DBG("\n");
  return drawText(color, fontSize, x, y, text);
}
bool HuskylensV2_MQTT::clearText() {
  DBG("\n");
  return clearText();
}
bool HuskylensV2_MQTT::saveKnowledges(eAlgorithm_t algo, uint8_t knowledgeID) {
  DBG("\n");
  return saveKnowledges(algo, knowledgeID);
}
bool HuskylensV2_MQTT::loadKnowledges(eAlgorithm_t algo, uint8_t knowledgeID) {
  DBG("\n");
  return loadKnowledges(algo, knowledgeID);
}
bool HuskylensV2_MQTT::setNameByID(eAlgorithm_t algo, uint8_t id, String name) {
  DBG("\n");
  return setNameByID(algo, id, name);
}

bool HuskylensV2_MQTT::doSetMultiAlgorithm(eAlgorithm_t algo0,
                                           eAlgorithm_t algo1,
                                           eAlgorithm_t algo2) {
  DBG("\n");
  return doSetMultiAlgorithm(algo0, algo1, algo2);
}
bool HuskylensV2_MQTT::setMultiAlgorithmRatio(int8_t ratio0, int8_t ratio1,
                                              int8_t ratio2) {
  DBG("\n");
  return setMultiAlgorithmRatio(ratio0, ratio1, ratio2);
}

bool HuskylensV2_MQTT::getAlgoParamBool(eAlgorithm_t algo, String key) {
  DBG("\n");
  return getAlgoParamBool(algo, key);
}
float HuskylensV2_MQTT::getAlgoParamFloat(eAlgorithm_t algo, String key) {
  DBG("\n");
  return getAlgoParamFloat(algo, key);
}
String HuskylensV2_MQTT::getAlgoParamString(eAlgorithm_t algo, String key) {
  DBG("\n");
  return getAlgoParamString(algo, key);
}

bool HuskylensV2_MQTT::setAlgoParamBool(eAlgorithm_t algo, String key,
                                        bool value) {
  DBG("\n");
  return setAlgoParamBool(algo, key, value);
}
bool HuskylensV2_MQTT::setAlgoParamFloat(eAlgorithm_t algo, String key,
                                         float value) {
  DBG("\n");
  return setAlgoParamFloat(algo, key, value);
}
bool HuskylensV2_MQTT::setAlgoParamString(eAlgorithm_t algo, String key,
                                          String value) {
  DBG("\n");
  return setAlgoParamString(algo, key, value);
}
bool HuskylensV2_MQTT::updateAlgoParams(eAlgorithm_t algo) {
  DBG("\n");
  return updateAlgoParams(algo);
}
bool HuskylensV2_MQTT::startRecording(eMediaType_t mediaType, int16_t duration,
                                      String filename,
                                      eResolution_t resolution) {
  DBG("\n");
  return startRecording(mediaType, duration, filename, resolution);
}
bool HuskylensV2_MQTT::stopRecording(eMediaType_t mediaType) {
  DBG("\n");
  return stopRecording(mediaType);
}
