// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_C2SCHATECHOREQ_H_
#define FLATBUFFERS_GENERATED_C2SCHATECHOREQ_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 22 &&
              FLATBUFFERS_VERSION_MINOR == 12 &&
              FLATBUFFERS_VERSION_REVISION == 6,
             "Non-compatible flatbuffers version included");

struct C2S_CHATECHO_REQ;
struct C2S_CHATECHO_REQBuilder;

struct C2S_CHATECHO_REQ FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef C2S_CHATECHO_REQBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_SIZE = 4,
    VT_CODE = 6,
    VT_USERIDX = 8,
    VT_STRINGLENGTH = 10,
    VT_MSG = 12
  };
  int32_t size() const {
    return GetField<int32_t>(VT_SIZE, 0);
  }
  int32_t code() const {
    return GetField<int32_t>(VT_CODE, 0);
  }
  int32_t userIdx() const {
    return GetField<int32_t>(VT_USERIDX, 0);
  }
  int32_t stringLength() const {
    return GetField<int32_t>(VT_STRINGLENGTH, 0);
  }
  const flatbuffers::String *msg() const {
    return GetPointer<const flatbuffers::String *>(VT_MSG);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_SIZE, 4) &&
           VerifyField<int32_t>(verifier, VT_CODE, 4) &&
           VerifyField<int32_t>(verifier, VT_USERIDX, 4) &&
           VerifyField<int32_t>(verifier, VT_STRINGLENGTH, 4) &&
           VerifyOffset(verifier, VT_MSG) &&
           verifier.VerifyString(msg()) &&
           verifier.EndTable();
  }
};

struct C2S_CHATECHO_REQBuilder {
  typedef C2S_CHATECHO_REQ Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_size(int32_t size) {
    fbb_.AddElement<int32_t>(C2S_CHATECHO_REQ::VT_SIZE, size, 0);
  }
  void add_code(int32_t code) {
    fbb_.AddElement<int32_t>(C2S_CHATECHO_REQ::VT_CODE, code, 0);
  }
  void add_userIdx(int32_t userIdx) {
    fbb_.AddElement<int32_t>(C2S_CHATECHO_REQ::VT_USERIDX, userIdx, 0);
  }
  void add_stringLength(int32_t stringLength) {
    fbb_.AddElement<int32_t>(C2S_CHATECHO_REQ::VT_STRINGLENGTH, stringLength, 0);
  }
  void add_msg(flatbuffers::Offset<flatbuffers::String> msg) {
    fbb_.AddOffset(C2S_CHATECHO_REQ::VT_MSG, msg);
  }
  explicit C2S_CHATECHO_REQBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<C2S_CHATECHO_REQ> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<C2S_CHATECHO_REQ>(end);
    return o;
  }
};

inline flatbuffers::Offset<C2S_CHATECHO_REQ> CreateC2S_CHATECHO_REQ(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t size = 0,
    int32_t code = 0,
    int32_t userIdx = 0,
    int32_t stringLength = 0,
    flatbuffers::Offset<flatbuffers::String> msg = 0) {
  C2S_CHATECHO_REQBuilder builder_(_fbb);
  builder_.add_msg(msg);
  builder_.add_stringLength(stringLength);
  builder_.add_userIdx(userIdx);
  builder_.add_code(code);
  builder_.add_size(size);
  return builder_.Finish();
}

inline flatbuffers::Offset<C2S_CHATECHO_REQ> CreateC2S_CHATECHO_REQDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t size = 0,
    int32_t code = 0,
    int32_t userIdx = 0,
    int32_t stringLength = 0,
    const char *msg = nullptr) {
  auto msg__ = msg ? _fbb.CreateString(msg) : 0;
  return CreateC2S_CHATECHO_REQ(
      _fbb,
      size,
      code,
      userIdx,
      stringLength,
      msg__);
}

inline const C2S_CHATECHO_REQ *GetC2S_CHATECHO_REQ(const void *buf) {
  return flatbuffers::GetRoot<C2S_CHATECHO_REQ>(buf);
}

inline const C2S_CHATECHO_REQ *GetSizePrefixedC2S_CHATECHO_REQ(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<C2S_CHATECHO_REQ>(buf);
}

inline bool VerifyC2S_CHATECHO_REQBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<C2S_CHATECHO_REQ>(nullptr);
}

inline bool VerifySizePrefixedC2S_CHATECHO_REQBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<C2S_CHATECHO_REQ>(nullptr);
}

inline void FinishC2S_CHATECHO_REQBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<C2S_CHATECHO_REQ> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedC2S_CHATECHO_REQBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<C2S_CHATECHO_REQ> root) {
  fbb.FinishSizePrefixed(root);
}

#endif  // FLATBUFFERS_GENERATED_C2SCHATECHOREQ_H_
