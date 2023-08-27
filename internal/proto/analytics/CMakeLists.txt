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

# target connections_log_cc_proto
add_library(connections_log_cc_proto
  OBJECT
    "${CMAKE_CURRENT_LIST_DIR}/connections_log.proto"
)

protobuf_generate(
  LANGUAGE
    cpp
  TARGET
    connections_log_cc_proto
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
)

target_include_directories(connections_log_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(connections_log_cc_proto PUBLIC protobuf::libprotobuf)

# target fast_pair_log_cc_proto
add_library(fast_pair_log_cc_proto
  OBJECT
    "${CMAKE_CURRENT_LIST_DIR}/fast_pair_log.proto"
)

protobuf_generate(
  LANGUAGE
    cpp
  TARGET
    fast_pair_log_cc_proto
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
)

target_include_directories(fast_pair_log_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(fast_pair_log_cc_proto PUBLIC protobuf::libprotobuf)
