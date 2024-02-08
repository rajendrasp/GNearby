// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: sharing/proto/contact_rpc.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_sharing_2fproto_2fcontact_5frpc_2eproto_2epb_2eh
#define GOOGLE_PROTOBUF_INCLUDED_sharing_2fproto_2fcontact_5frpc_2eproto_2epb_2eh

#include <limits>
#include <string>
#include <type_traits>

#include "google/protobuf/port_def.inc"
#if PROTOBUF_VERSION < 4023000
#error "This file was generated by a newer version of protoc which is"
#error "incompatible with your Protocol Buffer headers. Please update"
#error "your headers."
#endif  // PROTOBUF_VERSION

#if 4023000 < PROTOBUF_MIN_PROTOC_VERSION
#error "This file was generated by an older version of protoc which is"
#error "incompatible with your Protocol Buffer headers. Please"
#error "regenerate this file with a newer version of protoc."
#endif  // PROTOBUF_MIN_PROTOC_VERSION
#include "google/protobuf/port_undef.inc"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/arena.h"
#include "google/protobuf/arenastring.h"
#include "google/protobuf/generated_message_util.h"
#include "google/protobuf/metadata_lite.h"
#include "google/protobuf/message_lite.h"
#include "google/protobuf/repeated_field.h"  // IWYU pragma: export
#include "google/protobuf/extension_set.h"  // IWYU pragma: export
#include "sharing/proto/rpc_resources.pb.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"

#define PROTOBUF_INTERNAL_EXPORT_sharing_2fproto_2fcontact_5frpc_2eproto

PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_sharing_2fproto_2fcontact_5frpc_2eproto {
  static const ::uint32_t offsets[];
};
namespace nearby {
namespace sharing {
namespace proto {
class ListContactPeopleRequest;
struct ListContactPeopleRequestDefaultTypeInternal;
extern ListContactPeopleRequestDefaultTypeInternal _ListContactPeopleRequest_default_instance_;
class ListContactPeopleResponse;
struct ListContactPeopleResponseDefaultTypeInternal;
extern ListContactPeopleResponseDefaultTypeInternal _ListContactPeopleResponse_default_instance_;
}  // namespace proto
}  // namespace sharing
}  // namespace nearby
PROTOBUF_NAMESPACE_OPEN
template <>
::nearby::sharing::proto::ListContactPeopleRequest* Arena::CreateMaybeMessage<::nearby::sharing::proto::ListContactPeopleRequest>(Arena*);
template <>
::nearby::sharing::proto::ListContactPeopleResponse* Arena::CreateMaybeMessage<::nearby::sharing::proto::ListContactPeopleResponse>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

namespace nearby {
namespace sharing {
namespace proto {

// ===================================================================


// -------------------------------------------------------------------

class ListContactPeopleRequest final :
    public ::PROTOBUF_NAMESPACE_ID::MessageLite /* @@protoc_insertion_point(class_definition:nearby.sharing.proto.ListContactPeopleRequest) */ {
 public:
  inline ListContactPeopleRequest() : ListContactPeopleRequest(nullptr) {}
  ~ListContactPeopleRequest() override;
  explicit PROTOBUF_CONSTEXPR ListContactPeopleRequest(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  ListContactPeopleRequest(const ListContactPeopleRequest& from);
  ListContactPeopleRequest(ListContactPeopleRequest&& from) noexcept
    : ListContactPeopleRequest() {
    *this = ::std::move(from);
  }

  inline ListContactPeopleRequest& operator=(const ListContactPeopleRequest& from) {
    CopyFrom(from);
    return *this;
  }
  inline ListContactPeopleRequest& operator=(ListContactPeopleRequest&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  inline const std::string& unknown_fields() const {
    return _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString);
  }
  inline std::string* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields<std::string>();
  }

  static const ListContactPeopleRequest& default_instance() {
    return *internal_default_instance();
  }
  static inline const ListContactPeopleRequest* internal_default_instance() {
    return reinterpret_cast<const ListContactPeopleRequest*>(
               &_ListContactPeopleRequest_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(ListContactPeopleRequest& a, ListContactPeopleRequest& b) {
    a.Swap(&b);
  }
  inline void Swap(ListContactPeopleRequest* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(ListContactPeopleRequest* other) {
    if (other == this) return;
    ABSL_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  ListContactPeopleRequest* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<ListContactPeopleRequest>(arena);
  }
  void CheckTypeAndMergeFrom(const ::PROTOBUF_NAMESPACE_ID::MessageLite& from)  final;
  void CopyFrom(const ListContactPeopleRequest& from);
  void MergeFrom(const ListContactPeopleRequest& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  ::size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::uint8_t* _InternalSerialize(
      ::uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(ListContactPeopleRequest* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::absl::string_view FullMessageName() {
    return "nearby.sharing.proto.ListContactPeopleRequest";
  }
  protected:
  explicit ListContactPeopleRequest(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  std::string GetTypeName() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kPageTokenFieldNumber = 2,
    kPageSizeFieldNumber = 1,
  };
  // string page_token = 2;
  void clear_page_token() ;
  const std::string& page_token() const;




  template <typename Arg_ = const std::string&, typename... Args_>
  void set_page_token(Arg_&& arg, Args_... args);
  std::string* mutable_page_token();
  PROTOBUF_NODISCARD std::string* release_page_token();
  void set_allocated_page_token(std::string* ptr);

  private:
  const std::string& _internal_page_token() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_page_token(
      const std::string& value);
  std::string* _internal_mutable_page_token();

  public:
  // int32 page_size = 1;
  void clear_page_size() ;
  ::int32_t page_size() const;
  void set_page_size(::int32_t value);

  private:
  ::int32_t _internal_page_size() const;
  void _internal_set_page_size(::int32_t value);

  public:
  // @@protoc_insertion_point(class_scope:nearby.sharing.proto.ListContactPeopleRequest)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr page_token_;
    ::int32_t page_size_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_sharing_2fproto_2fcontact_5frpc_2eproto;
};// -------------------------------------------------------------------

class ListContactPeopleResponse final :
    public ::PROTOBUF_NAMESPACE_ID::MessageLite /* @@protoc_insertion_point(class_definition:nearby.sharing.proto.ListContactPeopleResponse) */ {
 public:
  inline ListContactPeopleResponse() : ListContactPeopleResponse(nullptr) {}
  ~ListContactPeopleResponse() override;
  explicit PROTOBUF_CONSTEXPR ListContactPeopleResponse(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  ListContactPeopleResponse(const ListContactPeopleResponse& from);
  ListContactPeopleResponse(ListContactPeopleResponse&& from) noexcept
    : ListContactPeopleResponse() {
    *this = ::std::move(from);
  }

  inline ListContactPeopleResponse& operator=(const ListContactPeopleResponse& from) {
    CopyFrom(from);
    return *this;
  }
  inline ListContactPeopleResponse& operator=(ListContactPeopleResponse&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  inline const std::string& unknown_fields() const {
    return _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString);
  }
  inline std::string* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields<std::string>();
  }

  static const ListContactPeopleResponse& default_instance() {
    return *internal_default_instance();
  }
  static inline const ListContactPeopleResponse* internal_default_instance() {
    return reinterpret_cast<const ListContactPeopleResponse*>(
               &_ListContactPeopleResponse_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(ListContactPeopleResponse& a, ListContactPeopleResponse& b) {
    a.Swap(&b);
  }
  inline void Swap(ListContactPeopleResponse* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(ListContactPeopleResponse* other) {
    if (other == this) return;
    ABSL_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  ListContactPeopleResponse* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<ListContactPeopleResponse>(arena);
  }
  void CheckTypeAndMergeFrom(const ::PROTOBUF_NAMESPACE_ID::MessageLite& from)  final;
  void CopyFrom(const ListContactPeopleResponse& from);
  void MergeFrom(const ListContactPeopleResponse& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  ::size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::uint8_t* _InternalSerialize(
      ::uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(ListContactPeopleResponse* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::absl::string_view FullMessageName() {
    return "nearby.sharing.proto.ListContactPeopleResponse";
  }
  protected:
  explicit ListContactPeopleResponse(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  std::string GetTypeName() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kContactRecordsFieldNumber = 1,
    kNextPageTokenFieldNumber = 2,
  };
  // repeated .nearby.sharing.proto.ContactRecord contact_records = 1;
  int contact_records_size() const;
  private:
  int _internal_contact_records_size() const;

  public:
  void clear_contact_records() ;
  ::nearby::sharing::proto::ContactRecord* mutable_contact_records(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::nearby::sharing::proto::ContactRecord >*
      mutable_contact_records();
  private:
  const ::nearby::sharing::proto::ContactRecord& _internal_contact_records(int index) const;
  ::nearby::sharing::proto::ContactRecord* _internal_add_contact_records();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<::nearby::sharing::proto::ContactRecord>& _internal_contact_records() const;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<::nearby::sharing::proto::ContactRecord>* _internal_mutable_contact_records();
  public:
  const ::nearby::sharing::proto::ContactRecord& contact_records(int index) const;
  ::nearby::sharing::proto::ContactRecord* add_contact_records();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::nearby::sharing::proto::ContactRecord >&
      contact_records() const;
  // string next_page_token = 2;
  void clear_next_page_token() ;
  const std::string& next_page_token() const;




  template <typename Arg_ = const std::string&, typename... Args_>
  void set_next_page_token(Arg_&& arg, Args_... args);
  std::string* mutable_next_page_token();
  PROTOBUF_NODISCARD std::string* release_next_page_token();
  void set_allocated_next_page_token(std::string* ptr);

  private:
  const std::string& _internal_next_page_token() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_next_page_token(
      const std::string& value);
  std::string* _internal_mutable_next_page_token();

  public:
  // @@protoc_insertion_point(class_scope:nearby.sharing.proto.ListContactPeopleResponse)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::nearby::sharing::proto::ContactRecord > contact_records_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr next_page_token_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_sharing_2fproto_2fcontact_5frpc_2eproto;
};

// ===================================================================




// ===================================================================


#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// -------------------------------------------------------------------

// ListContactPeopleRequest

// int32 page_size = 1;
inline void ListContactPeopleRequest::clear_page_size() {
  _impl_.page_size_ = 0;
}
inline ::int32_t ListContactPeopleRequest::page_size() const {
  // @@protoc_insertion_point(field_get:nearby.sharing.proto.ListContactPeopleRequest.page_size)
  return _internal_page_size();
}
inline void ListContactPeopleRequest::set_page_size(::int32_t value) {
  _internal_set_page_size(value);
  // @@protoc_insertion_point(field_set:nearby.sharing.proto.ListContactPeopleRequest.page_size)
}
inline ::int32_t ListContactPeopleRequest::_internal_page_size() const {
  return _impl_.page_size_;
}
inline void ListContactPeopleRequest::_internal_set_page_size(::int32_t value) {
  ;
  _impl_.page_size_ = value;
}

// string page_token = 2;
inline void ListContactPeopleRequest::clear_page_token() {
  _impl_.page_token_.ClearToEmpty();
}
inline const std::string& ListContactPeopleRequest::page_token() const {
  // @@protoc_insertion_point(field_get:nearby.sharing.proto.ListContactPeopleRequest.page_token)
  return _internal_page_token();
}
template <typename Arg_, typename... Args_>
inline PROTOBUF_ALWAYS_INLINE void ListContactPeopleRequest::set_page_token(Arg_&& arg,
                                                     Args_... args) {
  ;
  _impl_.page_token_.Set(static_cast<Arg_&&>(arg), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:nearby.sharing.proto.ListContactPeopleRequest.page_token)
}
inline std::string* ListContactPeopleRequest::mutable_page_token() {
  std::string* _s = _internal_mutable_page_token();
  // @@protoc_insertion_point(field_mutable:nearby.sharing.proto.ListContactPeopleRequest.page_token)
  return _s;
}
inline const std::string& ListContactPeopleRequest::_internal_page_token() const {
  return _impl_.page_token_.Get();
}
inline void ListContactPeopleRequest::_internal_set_page_token(const std::string& value) {
  ;


  _impl_.page_token_.Set(value, GetArenaForAllocation());
}
inline std::string* ListContactPeopleRequest::_internal_mutable_page_token() {
  ;
  return _impl_.page_token_.Mutable( GetArenaForAllocation());
}
inline std::string* ListContactPeopleRequest::release_page_token() {
  // @@protoc_insertion_point(field_release:nearby.sharing.proto.ListContactPeopleRequest.page_token)
  return _impl_.page_token_.Release();
}
inline void ListContactPeopleRequest::set_allocated_page_token(std::string* value) {
  _impl_.page_token_.SetAllocated(value, GetArenaForAllocation());
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
        if (_impl_.page_token_.IsDefault()) {
          _impl_.page_token_.Set("", GetArenaForAllocation());
        }
  #endif  // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:nearby.sharing.proto.ListContactPeopleRequest.page_token)
}

// -------------------------------------------------------------------

// ListContactPeopleResponse

// repeated .nearby.sharing.proto.ContactRecord contact_records = 1;
inline int ListContactPeopleResponse::_internal_contact_records_size() const {
  return _impl_.contact_records_.size();
}
inline int ListContactPeopleResponse::contact_records_size() const {
  return _internal_contact_records_size();
}
inline ::nearby::sharing::proto::ContactRecord* ListContactPeopleResponse::mutable_contact_records(int index) {
  // @@protoc_insertion_point(field_mutable:nearby.sharing.proto.ListContactPeopleResponse.contact_records)
  return _internal_mutable_contact_records()->Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::nearby::sharing::proto::ContactRecord >*
ListContactPeopleResponse::mutable_contact_records() {
  // @@protoc_insertion_point(field_mutable_list:nearby.sharing.proto.ListContactPeopleResponse.contact_records)
  return _internal_mutable_contact_records();
}
inline const ::nearby::sharing::proto::ContactRecord& ListContactPeopleResponse::_internal_contact_records(int index) const {
  return _internal_contact_records().Get(index);
}
inline const ::nearby::sharing::proto::ContactRecord& ListContactPeopleResponse::contact_records(int index) const {
  // @@protoc_insertion_point(field_get:nearby.sharing.proto.ListContactPeopleResponse.contact_records)
  return _internal_contact_records(index);
}
inline ::nearby::sharing::proto::ContactRecord* ListContactPeopleResponse::_internal_add_contact_records() {
  return _internal_mutable_contact_records()->Add();
}
inline ::nearby::sharing::proto::ContactRecord* ListContactPeopleResponse::add_contact_records() {
  ::nearby::sharing::proto::ContactRecord* _add = _internal_add_contact_records();
  // @@protoc_insertion_point(field_add:nearby.sharing.proto.ListContactPeopleResponse.contact_records)
  return _add;
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::nearby::sharing::proto::ContactRecord >&
ListContactPeopleResponse::contact_records() const {
  // @@protoc_insertion_point(field_list:nearby.sharing.proto.ListContactPeopleResponse.contact_records)
  return _internal_contact_records();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<::nearby::sharing::proto::ContactRecord>&
ListContactPeopleResponse::_internal_contact_records() const {
  return _impl_.contact_records_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<::nearby::sharing::proto::ContactRecord>*
ListContactPeopleResponse::_internal_mutable_contact_records() {
  return &_impl_.contact_records_;
}

// string next_page_token = 2;
inline void ListContactPeopleResponse::clear_next_page_token() {
  _impl_.next_page_token_.ClearToEmpty();
}
inline const std::string& ListContactPeopleResponse::next_page_token() const {
  // @@protoc_insertion_point(field_get:nearby.sharing.proto.ListContactPeopleResponse.next_page_token)
  return _internal_next_page_token();
}
template <typename Arg_, typename... Args_>
inline PROTOBUF_ALWAYS_INLINE void ListContactPeopleResponse::set_next_page_token(Arg_&& arg,
                                                     Args_... args) {
  ;
  _impl_.next_page_token_.Set(static_cast<Arg_&&>(arg), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:nearby.sharing.proto.ListContactPeopleResponse.next_page_token)
}
inline std::string* ListContactPeopleResponse::mutable_next_page_token() {
  std::string* _s = _internal_mutable_next_page_token();
  // @@protoc_insertion_point(field_mutable:nearby.sharing.proto.ListContactPeopleResponse.next_page_token)
  return _s;
}
inline const std::string& ListContactPeopleResponse::_internal_next_page_token() const {
  return _impl_.next_page_token_.Get();
}
inline void ListContactPeopleResponse::_internal_set_next_page_token(const std::string& value) {
  ;


  _impl_.next_page_token_.Set(value, GetArenaForAllocation());
}
inline std::string* ListContactPeopleResponse::_internal_mutable_next_page_token() {
  ;
  return _impl_.next_page_token_.Mutable( GetArenaForAllocation());
}
inline std::string* ListContactPeopleResponse::release_next_page_token() {
  // @@protoc_insertion_point(field_release:nearby.sharing.proto.ListContactPeopleResponse.next_page_token)
  return _impl_.next_page_token_.Release();
}
inline void ListContactPeopleResponse::set_allocated_next_page_token(std::string* value) {
  _impl_.next_page_token_.SetAllocated(value, GetArenaForAllocation());
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
        if (_impl_.next_page_token_.IsDefault()) {
          _impl_.next_page_token_.Set("", GetArenaForAllocation());
        }
  #endif  // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:nearby.sharing.proto.ListContactPeopleResponse.next_page_token)
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)
}  // namespace proto
}  // namespace sharing
}  // namespace nearby


// @@protoc_insertion_point(global_scope)

#include "google/protobuf/port_undef.inc"

#endif  // GOOGLE_PROTOBUF_INCLUDED_sharing_2fproto_2fcontact_5frpc_2eproto_2epb_2eh
