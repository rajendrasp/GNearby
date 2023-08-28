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

# target error_code_enums_cc_proto
add_library(error_code_enums_cc_proto
  OBJECT
    "error_code_enums.proto"
)

protobuf_generate(
  LANGUAGE
    cpp
  TARGET
    error_code_enums_cc_proto
  IMPORT_DIRS
    ${CMAKE_SOURCE_DIR}
)

target_include_directories(error_code_enums_cc_proto PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(error_code_enums_cc_proto PUBLIC protobuf::libprotobuf)
set_property(TARGET error_code_enums_cc_proto PROPERTY POSITION_INDEPENDENT_CODE ON)
