#include <Result.h>

Result::Result(JsonObject &obj) {
  if (obj.containsKey("id")) {
    ID = obj["id"];
  } else if (obj.containsKey("maxID")) {
    maxID = obj["maxID"];
  } else if (obj.containsKey("resolution")) {
    resolution = obj["resolution"];
  } else if (obj.containsKey("boardType")) {
    boardType = obj["boardType"];
  } else if (obj.containsKey("multiAlgoNum")) {
    multiAlgoNum = obj["multiAlgoNum"];
  }

  if (obj.containsKey("level")) {
    level = obj["level"];
  } else if (obj.containsKey("mediaType")) {
    mediaType = obj["mediaType"];
  } else if (obj.containsKey("retValue")) {
    retValue = obj["retValue"];
  } else if (obj.containsKey("retValue")) {
    lineWidth = obj["lineWidth"];
  } else if (obj.containsKey("confidence")) {
    confidence = obj["confidence"];
  }

  if (obj.containsKey("xCenter")) {
    xCenter = obj["xCenter"];
  } else if (obj.containsKey("xTarget")) {
    xTarget = obj["xTarget"];
  } else if (obj.containsKey("duration")) {
    duration = obj["duration"];
  } else if (obj.containsKey("duration")) {
    algorithmType = obj["algorithmType"];
  } else if (obj.containsKey("classID")) {
    classID = obj["classID"];
  } else if (obj.containsKey("total_results")) {
    total_results = obj["total_results"];
  } else if (obj.containsKey("pitch")) {
    pitch = obj["pitch"];
  }

  if (obj.containsKey("yCenter")) {
    yCenter = obj["yCenter"];
  } else if (obj.containsKey("yTarget")) {
    yTarget = obj["yTarget"];
  } else if (obj.containsKey("total_results_learned")) {
    total_results_learned = obj["total_results_learned"];
  } else if (obj.containsKey("yaw")) {
    yaw = obj["yaw"];
  }

  if (obj.containsKey("width")) {
    width = obj["width"];
  } else if (obj.containsKey("angle")) {
    angle = obj["angle"];
  } else if (obj.containsKey("azimuth")) {
    azimuth = obj["azimuth"];
  } else if (obj.containsKey("total_blocks")) {
    total_blocks = obj["total_blocks"];
  } else if (obj.containsKey("roll")) {
    roll = obj["roll"];
  }

  if (obj.containsKey("height")) {
    height = obj["height"];
  } else if (obj.containsKey("length")) {
    length = obj["length"];
  } else if (obj.containsKey("total_blocks_learned")) {
    total_blocks_learned = obj["total_blocks_learned"];
  }

  name = obj["name"].as<String>();
  content = obj["content"].as<String>();
  used = 0;

  DBG_KV("ID=", ID);
  DBG_KV("xCenter=", xCenter);
  DBG_KV("yCenter=", yCenter);
  DBG_KV("width=", width);

  DBG_KV("height=", height);
  DBG_KV("name=", name);
  DBG_KV("content=", content);
}
Result::~Result() {}

#define READ_INT16(p)                                                          \
  ({                                                                           \
    int16_t v;                                                                 \
    memcpy(&v, p, sizeof(v));                                                  \
    p++;                                                                       \
    v;                                                                         \
  })
#if 0
FaceResult::FaceResult(const void *buf) : Result(buf) {
  DBG("\n");
  PacketHead_t *head = (PacketHead_t *)buf;
  PacketData_t *packet = (PacketData_t *)head->data;
  String_t *pname = (String_t *)packet->payload;
  String_t *pcontent =
      (String_t *)(packet->payload + sizeof(String_t) + pname->length);
  if (head->data_length == (uint8_t)((uint32_t)pname - (uint32_t)packet)) {
    DBG("no string data");
    return;
  }
  int16_t *face_data = (int16_t *)(packet->payload + pname->length +
                                   pcontent->length + sizeof(String_t) * 2);
  if (head->data_length == (uint8_t)((uint32_t)face_data - (uint32_t)packet)) {
    DBG("no externed data");
    return;
  }

  leye_x = READ_INT16(face_data);
  leye_y = READ_INT16(face_data);
  reye_x = READ_INT16(face_data);
  reye_y = READ_INT16(face_data);
  nose_x = READ_INT16(face_data);
  nose_y = READ_INT16(face_data);
  lmouth_x = READ_INT16(face_data);
  lmouth_y = READ_INT16(face_data);
  rmouth_x = READ_INT16(face_data);
  rmouth_y = READ_INT16(face_data);
}

HandResult::HandResult(const void *buf) : Result(buf) {
  DBG("\n");
  PacketHead_t *head = (PacketHead_t *)buf;
  PacketData_t *packet = (PacketData_t *)head->data;
  String_t *pname = (String_t *)packet->payload;
  String_t *pcontent =
      (String_t *)(packet->payload + sizeof(String_t) + pname->length);

  if (head->data_length == (uint8_t)((uint32_t)pname - (uint32_t)packet)) {
    DBG("no string data");
    return;
  }
  int16_t *hand_data = (int16_t *)(packet->payload + pname->length +
                                   pcontent->length + sizeof(String_t) * 2);
  if (head->data_length <= (uint8_t)((uint32_t)hand_data - (uint32_t)packet)) {
    DBG("no externed data");
    return;
  }

  wrist_x = READ_INT16(hand_data); // 手腕
  wrist_y = READ_INT16(hand_data);
  DBG_HEX(wrist_x);
  DBG_HEX(wrist_y);
  thumb_cmc_x = READ_INT16(hand_data);
  thumb_cmc_y = READ_INT16(hand_data); // 拇指根部
  thumb_mcp_x = READ_INT16(hand_data);
  thumb_mcp_y = READ_INT16(hand_data); // 拇指中间关节
  thumb_ip_x = READ_INT16(hand_data);
  thumb_ip_y = READ_INT16(hand_data); // 拇指第二关节
  thumb_tip_x = READ_INT16(hand_data);
  thumb_tip_y = READ_INT16(hand_data); // 拇指指尖

  index_finger_mcp_x = READ_INT16(hand_data);
  index_finger_mcp_y = READ_INT16(hand_data); // 食指根部
  index_finger_pip_x = READ_INT16(hand_data);
  index_finger_pip_y = READ_INT16(hand_data); // 食指第一关节
  index_finger_dip_x = READ_INT16(hand_data);
  index_finger_dip_y = READ_INT16(hand_data); // 食指第二关节
  index_finger_tip_x = READ_INT16(hand_data);
  index_finger_tip_y = READ_INT16(hand_data); // 食指指尖
  middle_finger_mcp_x = READ_INT16(hand_data);
  middle_finger_mcp_y = READ_INT16(hand_data); // 中指根部

  middle_finger_pip_x = READ_INT16(hand_data);
  middle_finger_pip_y = READ_INT16(hand_data); // 中指第一关节
  middle_finger_dip_x = READ_INT16(hand_data);
  middle_finger_dip_y = READ_INT16(hand_data); // 中指第二关节
  middle_finger_tip_x = READ_INT16(hand_data);
  middle_finger_tip_y = READ_INT16(hand_data); // 中指指尖
  ring_finger_mcp_x = READ_INT16(hand_data);
  ring_finger_mcp_y = READ_INT16(hand_data); // 无名指根部
  ring_finger_pip_x = READ_INT16(hand_data);
  ring_finger_pip_y = READ_INT16(hand_data); // 无名指第一关节

  ring_finger_dip_x = READ_INT16(hand_data);
  ring_finger_dip_y = READ_INT16(hand_data); // 无名指第二关节
  ring_finger_tip_x = READ_INT16(hand_data);
  ring_finger_tip_y = READ_INT16(hand_data); // 无名指指尖
  pinky_finger_mcp_x = READ_INT16(hand_data);
  pinky_finger_mcp_y = READ_INT16(hand_data); // 小指根部
  pinky_finger_pip_x = READ_INT16(hand_data);
  pinky_finger_pip_y = READ_INT16(hand_data); // 小指第一关节
  pinky_finger_dip_x = READ_INT16(hand_data);
  pinky_finger_dip_y = READ_INT16(hand_data); // 小指第二关节
  DBG_HEX(pinky_finger_dip_x);
  DBG_HEX(pinky_finger_dip_y);
  pinky_finger_tip_x = READ_INT16(hand_data);
  pinky_finger_tip_y = READ_INT16(hand_data); // 小指指尖
  DBG_HEX(pinky_finger_tip_x);
  DBG_HEX(pinky_finger_tip_y);
}

PoseResult::PoseResult(const void *buf) : Result(buf) {
  DBG("begin parse\n");
  PacketHead_t *head = (PacketHead_t *)buf;
  PacketData_t *packet = (PacketData_t *)head->data;
  String_t *pname = (String_t *)packet->payload;
  String_t *pcontent =
      (String_t *)(packet->payload + sizeof(String_t) + pname->length);
  int16_t *pose_data = (int16_t *)(packet->payload + pname->length +
                                   pcontent->length + sizeof(String_t) * 2);
  if (head->data_length == (uint8_t)((uint32_t)pname - (uint32_t)packet)) {
    DBG("no string data");
    return;
  }
  if (head->data_length == (uint8_t)((uint32_t)pose_data - (uint32_t)packet)) {
    DBG("no externed data");
    return;
  }
  DBG("hax externed data");
  DBG_PRINTLN(head->data_length -
              (uint8_t)((uint32_t)pose_data - (uint32_t)packet));
  nose_x = READ_INT16(pose_data);
  nose_y = READ_INT16(pose_data);
  leye_x = READ_INT16(pose_data);
  leye_y = READ_INT16(pose_data);
  reye_x = READ_INT16(pose_data);
  reye_y = READ_INT16(pose_data);
  lear_x = READ_INT16(pose_data);
  lear_y = READ_INT16(pose_data);
  rear_x = READ_INT16(pose_data);
  rear_y = READ_INT16(pose_data);
  lshoulder_x = READ_INT16(pose_data);
  lshoulder_y = READ_INT16(pose_data);
  rshoulder_x = READ_INT16(pose_data);
  rshoulder_y = READ_INT16(pose_data);
  lelbow_x = READ_INT16(pose_data);
  lelbow_y = READ_INT16(pose_data);
  relbow_x = READ_INT16(pose_data);
  relbow_y = READ_INT16(pose_data);
  lwrist_x = READ_INT16(pose_data);
  lwrist_y = READ_INT16(pose_data);
  rwrist_x = READ_INT16(pose_data);
  rwrist_y = READ_INT16(pose_data);
  lhip_x = READ_INT16(pose_data);
  lhip_y = READ_INT16(pose_data);
  rhip_x = READ_INT16(pose_data);
  rhip_y = READ_INT16(pose_data);
  lknee_x = READ_INT16(pose_data);
  lknee_y = READ_INT16(pose_data);
  rknee_x = READ_INT16(pose_data);
  rknee_y = READ_INT16(pose_data);
  lankle_x = READ_INT16(pose_data);
  lankle_y = READ_INT16(pose_data);
  rankle_x = READ_INT16(pose_data);
  rankle_y = READ_INT16(pose_data);
  DBG("end parse\n");
}
#endif

Result *result[ALGORITHM_COUNT][MAX_RESULT_NUM];
uint8_t customId[3];
uint8_t customAlgoNum;
int16_t maxID[ALGORITHM_BUILTIN_COUNT];

eAlgorithm_t toRealID(uint8_t id) {
  eAlgorithm_t algo = ALGORITHM_ANY;
  if (id >= ALGORITHM_CUSTOM_BEGIN) {
    for (uint8_t i = 0; i < CUSTOM_ALGORITHM_COUNT; i++)
      if (customId[i] == algo) {
        algo = (eAlgorithm_t)((int)ALGORITHM_CUSTOM0 + i);
        break;
      }
  }
  return algo;
}