// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_MESSAGE_TURKEY_H_
#define FLATBUFFERS_GENERATED_MESSAGE_TURKEY_H_

#include "flatbuffers/flatbuffers.h"

namespace Turkey {

struct Message;

enum MessageType {
  MessageType_Start = 0,
  MessageType_Stop = 1,
  MessageType_Update = 2,
  MessageType_MIN = MessageType_Start,
  MessageType_MAX = MessageType_Update
};

inline MessageType (&EnumValuesMessageType())[3] {
  static MessageType values[] = {
    MessageType_Start,
    MessageType_Stop,
    MessageType_Update
  };
  return values;
}

inline const char **EnumNamesMessageType() {
  static const char *names[] = {
    "Start",
    "Stop",
    "Update",
    nullptr
  };
  return names;
}

inline const char *EnumNameMessageType(MessageType e) {
  const size_t index = static_cast<int>(e);
  return EnumNamesMessageType()[index];
}

struct Message FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_TYPE = 4,
    VT_PID = 6,
    VT_DATA = 8
  };
  MessageType type() const {
    return static_cast<MessageType>(GetField<int8_t>(VT_TYPE, 0));
  }
  bool mutate_type(MessageType _type) {
    return SetField<int8_t>(VT_TYPE, static_cast<int8_t>(_type), 0);
  }
  int32_t pid() const {
    return GetField<int32_t>(VT_PID, 0);
  }
  bool mutate_pid(int32_t _pid) {
    return SetField<int32_t>(VT_PID, _pid, 0);
  }
  int32_t data() const {
    return GetField<int32_t>(VT_DATA, 0);
  }
  bool mutate_data(int32_t _data) {
    return SetField<int32_t>(VT_DATA, _data, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int8_t>(verifier, VT_TYPE) &&
           VerifyField<int32_t>(verifier, VT_PID) &&
           VerifyField<int32_t>(verifier, VT_DATA) &&
           verifier.EndTable();
  }
};

struct MessageBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_type(MessageType type) {
    fbb_.AddElement<int8_t>(Message::VT_TYPE, static_cast<int8_t>(type), 0);
  }
  void add_pid(int32_t pid) {
    fbb_.AddElement<int32_t>(Message::VT_PID, pid, 0);
  }
  void add_data(int32_t data) {
    fbb_.AddElement<int32_t>(Message::VT_DATA, data, 0);
  }
  MessageBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  MessageBuilder &operator=(const MessageBuilder &);
  flatbuffers::Offset<Message> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Message>(end);
    return o;
  }
};

inline flatbuffers::Offset<Message> CreateMessage(
    flatbuffers::FlatBufferBuilder &_fbb,
    MessageType type = MessageType_Start,
    int32_t pid = 0,
    int32_t data = 0) {
  MessageBuilder builder_(_fbb);
  builder_.add_data(data);
  builder_.add_pid(pid);
  builder_.add_type(type);
  return builder_.Finish();
}

inline const Turkey::Message *GetMessage(const void *buf) {
  return flatbuffers::GetRoot<Turkey::Message>(buf);
}

inline Message *GetMutableMessage(void *buf) {
  return flatbuffers::GetMutableRoot<Message>(buf);
}

inline bool VerifyMessageBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<Turkey::Message>(nullptr);
}

inline void FinishMessageBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<Turkey::Message> root) {
  fbb.Finish(root);
}

}  // namespace Turkey

#endif  // FLATBUFFERS_GENERATED_MESSAGE_TURKEY_H_
