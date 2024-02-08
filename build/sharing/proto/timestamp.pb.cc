// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: sharing/proto/timestamp.proto

#include "sharing/proto/timestamp.pb.h"

#include <algorithm>
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/extension_set.h"
#include "google/protobuf/wire_format_lite.h"
#include "google/protobuf/io/zero_copy_stream_impl_lite.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"
PROTOBUF_PRAGMA_INIT_SEG
namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = ::PROTOBUF_NAMESPACE_ID::internal;
namespace nearby {
namespace sharing {
namespace proto {
PROTOBUF_CONSTEXPR Timestamp::Timestamp(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.seconds_)*/ ::int64_t{0}

  , /*decltype(_impl_.nanos_)*/ 0

  , /*decltype(_impl_._cached_size_)*/{}} {}
struct TimestampDefaultTypeInternal {
  PROTOBUF_CONSTEXPR TimestampDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~TimestampDefaultTypeInternal() {}
  union {
    Timestamp _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 TimestampDefaultTypeInternal _Timestamp_default_instance_;
}  // namespace proto
}  // namespace sharing
}  // namespace nearby
namespace nearby {
namespace sharing {
namespace proto {
// ===================================================================

class Timestamp::_Internal {
 public:
};

Timestamp::Timestamp(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:nearby.sharing.proto.Timestamp)
}
Timestamp::Timestamp(const Timestamp& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(), _impl_(from._impl_) {
  _internal_metadata_.MergeFrom<std::string>(
      from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:nearby.sharing.proto.Timestamp)
}

inline void Timestamp::SharedCtor(::_pb::Arena* arena) {
  (void)arena;
  new (&_impl_) Impl_{
      decltype(_impl_.seconds_) { ::int64_t{0} }

    , decltype(_impl_.nanos_) { 0 }

    , /*decltype(_impl_._cached_size_)*/{}
  };
}

Timestamp::~Timestamp() {
  // @@protoc_insertion_point(destructor:nearby.sharing.proto.Timestamp)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<std::string>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void Timestamp::SharedDtor() {
  ABSL_DCHECK(GetArenaForAllocation() == nullptr);
}

void Timestamp::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void Timestamp::Clear() {
// @@protoc_insertion_point(message_clear_start:nearby.sharing.proto.Timestamp)
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&_impl_.seconds_, 0, static_cast<::size_t>(
      reinterpret_cast<char*>(&_impl_.nanos_) -
      reinterpret_cast<char*>(&_impl_.seconds_)) + sizeof(_impl_.nanos_));
  _internal_metadata_.Clear<std::string>();
}

const char* Timestamp::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // int64 seconds = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::uint8_t>(tag) == 8)) {
          _impl_.seconds_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else {
          goto handle_unusual;
        }
        continue;
      // int32 nanos = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::uint8_t>(tag) == 16)) {
          _impl_.nanos_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else {
          goto handle_unusual;
        }
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<std::string>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

::uint8_t* Timestamp::_InternalSerialize(
    ::uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:nearby.sharing.proto.Timestamp)
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // int64 seconds = 1;
  if (this->_internal_seconds() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteInt64ToArray(
        1, this->_internal_seconds(), target);
  }

  // int32 nanos = 2;
  if (this->_internal_nanos() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteInt32ToArray(
        2, this->_internal_nanos(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:nearby.sharing.proto.Timestamp)
  return target;
}

::size_t Timestamp::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:nearby.sharing.proto.Timestamp)
  ::size_t total_size = 0;

  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // int64 seconds = 1;
  if (this->_internal_seconds() != 0) {
    total_size += ::_pbi::WireFormatLite::Int64SizePlusOne(
        this->_internal_seconds());
  }

  // int32 nanos = 2;
  if (this->_internal_nanos() != 0) {
    total_size += ::_pbi::WireFormatLite::Int32SizePlusOne(
        this->_internal_nanos());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::_pbi::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void Timestamp::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::_pbi::DownCast<const Timestamp*>(
      &from));
}

void Timestamp::MergeFrom(const Timestamp& from) {
  Timestamp* const _this = this;
  // @@protoc_insertion_point(class_specific_merge_from_start:nearby.sharing.proto.Timestamp)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_seconds() != 0) {
    _this->_internal_set_seconds(from._internal_seconds());
  }
  if (from._internal_nanos() != 0) {
    _this->_internal_set_nanos(from._internal_nanos());
  }
  _this->_internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
}

void Timestamp::CopyFrom(const Timestamp& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:nearby.sharing.proto.Timestamp)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Timestamp::IsInitialized() const {
  return true;
}

void Timestamp::InternalSwap(Timestamp* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(Timestamp, _impl_.nanos_)
      + sizeof(Timestamp::_impl_.nanos_)
      - PROTOBUF_FIELD_OFFSET(Timestamp, _impl_.seconds_)>(
          reinterpret_cast<char*>(&_impl_.seconds_),
          reinterpret_cast<char*>(&other->_impl_.seconds_));
}

std::string Timestamp::GetTypeName() const {
  return "nearby.sharing.proto.Timestamp";
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace proto
}  // namespace sharing
}  // namespace nearby
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::nearby::sharing::proto::Timestamp*
Arena::CreateMaybeMessage< ::nearby::sharing::proto::Timestamp >(Arena* arena) {
  return Arena::CreateMessageInternal< ::nearby::sharing::proto::Timestamp >(arena);
}
PROTOBUF_NAMESPACE_CLOSE
// @@protoc_insertion_point(global_scope)
#include "google/protobuf/port_undef.inc"
