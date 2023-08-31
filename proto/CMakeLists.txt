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

# target connections_enums_cc_proto
add_library(connections_enums_cc_proto
  OBJECT
    "connections_enums.proto"
)

protobuf_generate(
  LANGUAGE
    cpp
  TARGET
    connections_enums_cc_proto
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
)


target_include_directories(connections_enums_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(connections_enums_cc_proto PUBLIC protobuf::libprotobuf)
set_property(TARGET connections_enums_cc_proto PROPERTY POSITION_INDEPENDENT_CODE ON)

# target fast_pair_enums_cc_proto
add_library(fast_pair_enums_cc_proto
  OBJECT
    "fast_pair_enums.proto"
)

protobuf_generate(
  LANGUAGE
    cpp
  TARGET
    fast_pair_enums_cc_proto
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
)

target_include_directories(fast_pair_enums_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(fast_pair_enums_cc_proto PUBLIC protobuf::libprotobuf)
set_property(TARGET fast_pair_enums_cc_proto PROPERTY POSITION_INDEPENDENT_CODE ON)

# target sharing_enums_cc_proto
add_library(sharing_enums_cc_proto
  OBJECT
    "sharing_enums.proto"
)

protobuf_generate(
  LANGUAGE
    cpp
  TARGET
    sharing_enums_cc_proto
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
  PROTOC_OUT_DIR
    ${CMAKE_BINARY_DIR}
)

target_include_directories(sharing_enums_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(sharing_enums_cc_proto PUBLIC protobuf::libprotobuf)
set_property(TARGET sharing_enums_cc_proto PROPERTY POSITION_INDEPENDENT_CODE ON)

add_subdirectory("errorcode")
add_subdirectory("mediums")
