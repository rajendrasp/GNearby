# Copyright 2023 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# target credential_cc_proto
add_library(credential_cc_proto
  OBJECT
    "credential.proto"
)

protobuf_generate(
  LANGUAGE
    cpp
  TARGET
    credential_cc_proto
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
)

target_include_directories(credential_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(credential_cc_proto PUBLIC protobuf::libprotobuf)
set_property(TARGET credential_cc_proto PROPERTY POSITION_INDEPENDENT_CODE ON)

# target local_credential_cc_proto
add_library(local_credential_cc_proto
  OBJECT
    "local_credential.proto"
)

protobuf_generate(
  LANGUAGE
    cpp
  TARGET
    local_credential_cc_proto
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
)

target_include_directories(local_credential_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(local_credential_cc_proto PUBLIC protobuf::libprotobuf)
set_property(TARGET local_credential_cc_proto PROPERTY POSITION_INDEPENDENT_CODE ON)

# target metadata_cc_proto
add_library(metadata_cc_proto
  OBJECT
    "metadata.proto"
)

protobuf_generate(
  LANGUAGE
    cpp
  TARGET
    metadata_cc_proto
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
)

target_include_directories(metadata_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(metadata_cc_proto PUBLIC protobuf::libprotobuf)
set_property(TARGET metadata_cc_proto PROPERTY POSITION_INDEPENDENT_CODE ON)

#[[
# target tachyon_cc_proto
add_library(tachyon_cc_proto
  OBJECT
    "${CMAKE_CURRENT_LIST_DIR}/duration.proto"
    "${CMAKE_CURRENT_LIST_DIR}/ice.proto"
    "${CMAKE_CURRENT_LIST_DIR}/tachyon.proto"
    "${CMAKE_CURRENT_LIST_DIR}/tachyon_common.proto"
    "${CMAKE_CURRENT_LIST_DIR}/tachyon_enums.proto"
)

protobuf_generate(
  LANGUAGE
    cpp
  TARGET
    tachyon_cc_proto
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
)

target_include_directories(tachyon_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(tachyon_cc_proto PUBLIC protobuf::libprotobuf)
#]]

add_subdirectory("analytics")
