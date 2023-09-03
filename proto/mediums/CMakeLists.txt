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

# target ble_frames_cc_proto
protobuf_generate(
  LANGUAGE cpp
  OUT_VAR ble_frames_cc_PROTO_GENERATED
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
  PROTOS
    "ble_frames.proto"
)

add_library(ble_frames_cc_proto
    ${ble_frames_cc_PROTO_GENERATED}
)

target_include_directories(ble_frames_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(ble_frames_cc_proto PUBLIC protobuf::libprotobuf)

# target multiplex_frames_cc_proto
protobuf_generate(
  LANGUAGE cpp
  OUT_VAR multiplex_frames_cc_PROTO_GENERATED
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
  PROTOS
    "multiplex_frames.proto"
)

add_library(multiplex_frames_cc_proto
    ${multiplex_frames_cc_PROTO_GENERATED}
)

target_include_directories(multiplex_frames_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(multiplex_frames_cc_proto PUBLIC protobuf::libprotobuf)

# target nfc_frames_cc_proto
protobuf_generate(
  LANGUAGE cpp
  OUT_VAR nfc_frames_cc_PROTO_GENERATED
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
  PROTOS
    "nfc_frames.proto"
)

add_library(nfc_frames_cc_proto
    ${nfc_frames_cc_PROTO_GENERATED}
)

target_include_directories(nfc_frames_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(nfc_frames_cc_proto PUBLIC protobuf::libprotobuf)

# target web_rtc_signaling_frames_cc_proto
protobuf_generate(
  LANGUAGE cpp
  OUT_VAR web_rtc_signaling_frames_cc_PROTO_GENERATED
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
  PROTOS
    "web_rtc_signaling_frames.proto"
)

add_library(web_rtc_signaling_frames_cc_proto
    ${web_rtc_signaling_frames_cc_PROTO_GENERATED}
)

target_include_directories(web_rtc_signaling_frames_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(web_rtc_signaling_frames_cc_proto PUBLIC protobuf::libprotobuf)
