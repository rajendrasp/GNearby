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

# target internal_data_data_manager

add_library(internal_data_data_manager
  INTERFACE
    "data_manager.h"
    "data_set.h"
    "leveldb_data_set.h"
    "memory_data_set.h"
)

target_link_libraries(internal_data_data_manager
  INTERFACE
    internal::platform::types
    leveldb::leveldb
    protobuf::protobuf_lite
    absl::flat_hash_map
    absl::strings
    absl::synchronization
)

target_include_directories(internal_data_data_manager INTERFACE ${CMAKE_SOURCE_DIR})

add_library(internal::data::data_manager ALIAS internal_data_data_manager)

# target leveldb_data_set_test_cc_proto

# target internal_data_data_manager_test
