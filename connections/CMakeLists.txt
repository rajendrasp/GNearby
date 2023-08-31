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

# target connections_core
add_library(connections_core
    "core.cc"
    "core.h"
)

target_link_libraries(connections_core
  PUBLIC
    connections_core_types
    connections_implementation_internal
    connections_v3_v3_types
    internal_analytics_event_logger
    internal_interop_device
    internal_platform_base
    internal_platform_types
    absl::strings
    absl::time
    absl::span
)

target_compile_definitions(connections_core PRIVATE "CORE_ADAPTER_DLL")
target_include_directories(connections_core PRIVATE ${CMAKE_SOURCE_DIR})

# target connections_core_types
add_library(connections_core_types
    "advertising_options.cc"
    "connection_options.cc"
    "discovery_options.cc"
    "payload.cc"
    "status.cc"
    "strategy.cc"
    "advertising_options.h"
    "connection_options.h"
    "discovery_options.h"
    "listeners.h"
    "medium_selector.h"
    "options_base.h"
    "out_of_band_connection_metadata.h"
    "params.h"
    "payload.h"
    "payload_type.h"
    "power_level.h"
    "status.h"
    "strategy.h"
)

target_link_libraries(connections_core_types
  PUBLIC
    internal_platform_base
    internal_platform_types
    internal_platform_util
    connections_enums_cc_proto
    absl::any_invocable
    absl::variant
)

target_compile_definitions(connections_core_types PRIVATE "CORE_ADAPTER_DLL")
target_include_directories(connections_core_types PRIVATE ${CMAKE_SOURCE_DIR})

# target connections_core_test

add_subdirectory("implementation")
add_subdirectory("v3")

if(UNIX AND NOT APPLE)
  add_subdirectory("connectionsd")
endif()
