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

# target internal_platform_implementation_shared_posix_mutex
add_library(internal_platform_implementation_shared_posix_mutex
    "posix_mutex.cc"
    "posix_mutex.h"
)

target_link_libraries(internal_platform_implementation_shared_posix_mutex
  PUBLIC
    internal_platform_implementation_types
)

target_include_directories(internal_platform_implementation_shared_posix_mutex PRIVATE ${CMAKE_SOURCE_DIR})

# target internal_platform_implementation_shared_posix_condition_variable
add_library(internal_platform_implementation_shared_posix_condition_variable
    "posix_condition_variable.cc"
    "posix_condition_variable.h"
)

target_link_libraries(internal_platform_implementation_shared_posix_condition_variable
  PUBLIC
    internal_platform_implementation_shared_posix_mutex
    internal_platform_implementation_types
)

target_include_directories(internal_platform_implementation_shared_posix_condition_variable PRIVATE ${CMAKE_SOURCE_DIR})

# target internal_platform_implementation_shared_file
add_library(internal_platform_implementation_shared_file
    "file.cc"
    "file.h"
)

target_link_libraries(internal_platform_implementation_shared_file
  PUBLIC
    internal_platform_base
    internal_platform_implementation_types
    absl::memory
    absl::strings
)

target_include_directories(internal_platform_implementation_shared_file PRIVATE ${CMAKE_SOURCE_DIR})

# target internal_platform_implementation_shared_count_down_latch
add_library(internal_platform_implementation_shared_count_down_latch
    "count_down_latch.cc"
    "count_down_latch.h"
)

target_link_libraries(internal_platform_implementation_shared_count_down_latch
  PUBLIC
    internal_platform_implementation_types
    absl::core_headers
    absl::strings
    absl::synchronization
    absl::time
)

# target internal_platform_implementation_shared_file_test
