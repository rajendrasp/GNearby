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
add_library(ble_frames_cc_proto
  OBJECT
    "${CMAKE_CURRENT_LIST_DIR}/ble_frames.proto"
)
protobuf_generate(
  LANGUAGE
    cpp
  TARGET
    ble_frames_cc_proto
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
)

target_include_directories(ble_frames_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(ble_frames_cc_proto PUBLIC protobuf::libprotobuf)

# target multiplex_frames_cc_proto
add_library(multiplex_frames_cc_proto
  OBJECT
    "${CMAKE_CURRENT_LIST_DIR}/multiplex_frames.proto"
)

protobuf_generate(
  LANGUAGE
    cpp
  TARGET
    multiplex_frames_cc_proto
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
)

target_include_directories(multiplex_frames_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(multiplex_frames_cc_proto PUBLIC protobuf::libprotobuf)

# target nfc_frames_cc_proto
add_library(nfc_frames_cc_proto
  OBJECT
    "${CMAKE_CURRENT_LIST_DIR}/nfc_frames.proto"
)
protobuf_generate(
  LANGUAGE
    cpp
  TARGET
    nfc_frames_cc_proto
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
)

target_include_directories(nfc_frames_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(nfc_frames_cc_proto PUBLIC protobuf::libprotobuf)

# target web_rtc_signaling_frames_cc_proto
add_library(web_rtc_signaling_frames_cc_proto
  OBJECT
    "${CMAKE_CURRENT_LIST_DIR}/web_rtc_signaling_frames.proto"
)

protobuf_generate(
  LANGUAGE
    cpp
  TARGET
    web_rtc_signaling_frames_cc_proto
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
)

target_include_directories(web_rtc_signaling_frames_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(web_rtc_signaling_frames_cc_proto PUBLIC protobuf::libprotobuf)
