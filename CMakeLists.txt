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

cmake_minimum_required(VERSION 3.27)

include(FetchContent)
include(ExternalProject)

set(FETCHCONTENT_QUIET OFF)
cmake_policy(SET CMP0135 NEW)

option(BUILD_TESTING "Build testing binaries" OFF)
option(NO_WEBRTC "Build with no WebRTC support" ON)

if(NO_WEBRTC)
  add_compile_definitions("NO_WEBRTC")
endif()

# Prevents GLOG adding it's own GFLAGS to it's export target, resulting in an error with CMake
set(WITH_GFLAGS OFF CACHE BOOL "Disables building of GFlags")
set(GFLAGS_IS_SUBPROJECT TRUE)
set(GTEST_IS_SUBPROJECT TRUE)

set(INSTALL_GTEST OFF CACHE BOOL "Turns off installing GTest because it's being embedded")
set(BUILD_GMOCK OFF CACHE BOOL "Disable building of GMock in Google Test")
set(GTEST_HAS ON CACHE BOOL "...")
set(ABSL_ENABLE_INSTALL ON CACHE BOOL "...")
set(protobuf_BUILD_TESTS OFF CACHE BOOL "...")

fetchcontent_declare(
  absl
  GIT_REPOSITORY "https://github.com/abseil/abseil-cpp.git"
  GIT_TAG "20230802.0"
  GIT_PROGRESS TRUE
  OVERRIDE_FIND_PACKAGE
)

fetchcontent_declare(
  protobuf
  GIT_REPOSITORY "https://github.com/protocolbuffers/protobuf.git"
  GIT_TAG "v3.22.5"
  GIT_PROGRESS TRUE
  OVERRIDE_FIND_PACKAGE
)

fetchcontent_declare(
  smhasher
  GIT_REPOSITORY "https://github.com/aappleby/smhasher.git"
  GIT_PROGRESS TRUE
  SOURCE_SUBDIR "src"
  OVERRIDE_FIND_PACKAGE
)

fetchcontent_declare(
  nlohmann_json
  GIT_REPOSITORY "https://github.com/nlohmann/json.git"
  GIT_TAG "v3.10.5"
  GIT_PROGRESS TRUE
  OVERRIDE_FIND_PACKAGE
)

fetchcontent_declare(
  GTest
  GIT_REPOSITORY "https://github.com/google/googletest.git"
  GIT_TAG "main"
  GIT_PROGRESS TRUE
  OVERRIDE_FIND_PACKAGE
)

fetchcontent_declare(
  gflags
  GIT_REPOSITORY "https://github.com/gflags/gflags.git"
  GIT_TAG "v2.2.2"
  GIT_PROGRESS TRUE
  OVERRIDE_FIND_PACKAGE
)

fetchcontent_declare(
  glog
  GIT_REPOSITORY "https://github.com/google/glog.git"
  GIT_TAG "v0.4.0"
  GIT_PROGRESS TRUE
  OVERRIDE_FIND_PACKAGE
)

fetchcontent_declare(
  boringssl
  GIT_REPOSITORY "https://boringssl.googlesource.com/boringssl.git"
  GIT_PROGRESS TRUE
  OVERRIDE_FIND_PACKAGE
)

fetchcontent_declare(
  protobuf_matchers
  GIT_REPOSITORY "https://github.com/inazarenko/protobuf-matchers.git"
  GIT_PROGRESS TRUE
  OVERRIDE_FIND_PACKAGE
)

fetchcontent_declare(
  re2
  GIT_REPOSITORY "https://github.com/google/re2.git"
  GIT_TAG "2021-06-01"
  GIT_PROGRESS TRUE
  OVERRIDE_FIND_PACKAGE
)

fetchcontent_declare(
  ukey2
  GIT_REPOSITORY "https://github.com/google/ukey2.git"
  GIT_TAG "c2436e55116964d88532080784f6ed496b0d11f9"
  GIT_PROGRESS TRUE
  OVERRIDE_FIND_PACKAGE
)
fetchcontent_declare(
  securemessage
  GIT_REPOSITORY "https://github.com/google/securemessage"
  GIT_PROGRESS TRUE
  OVERRIDE_FIND_PACKAGE
)

fetchcontent_makeavailable(
  absl
  protobuf
  glog
  smhasher
  nlohmann_json
  GTest
  gflags
  boringssl
  #protobuf_matchers
  re2
)
set(Protobuf_LIBRARIES protobuf::libprotobuf)

# Include the CMake file that adds the function protobuf_generate into our project
fetchcontent_getproperties(protobuf SOURCE_DIR protobuf_SOURCE_DIR)
include(${protobuf_SOURCE_DIR}/cmake/protobuf-generate.cmake)

# UKey2 and securemessage depends on the functions included in protobuf-generate.cmake
# We make this avaliable after we include that
fetchcontent_makeavailable(securemessage)
fetchcontent_makeavailable(ukey2)
add_library(ukey2::ukey2 ALIAS ukey2)


# For BoringSSL, it throws all warnings as errors, and this gets thrown on the current "master" branch
target_compile_options(crypto INTERFACE "-Wno-error=ignored-attributes")
add_library(boringssl::crypto ALIAS crypto)

# Find packages
include(FindGTest)

# Protobuf targets for UKey2 and SecureMessage don't link to protobuf::libprotobuf
# thus throwing a `#error` that the protobuf version isn't correct.

# For aappleby_smhasher it does not add an include directory to it's target
fetchcontent_getproperties(smhasher
  SOURCE_DIR
    smhasher_SOURCE_DIR
)
set_property(
  TARGET
    SMHasherSupport
  APPEND
  PROPERTY
    INCLUDE_DIRECTORIES
      ${smhasher_SOURCE_DIR}
)
set_property(
  TARGET
    SMHasherSupport
  APPEND
  PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES
      ${smhasher_SOURCE_DIR}
)

# aappleby_smhasher also does not have export and install targets, so no alias target is made
add_library(smhasher::SMHasherSupport ALIAS SMHasherSupport)

# Projects that don't have CMake support
# WebRTC
# These will need to be implemented here with ExternalProject_Add()

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_STATIC_LIBS "Build static libraries" OFF)

project(Nearby
DESCRIPTION "A collection of projects focused on connectivity that enable building cross-device experiences."
  HOMEPAGE_URL "https://github.com/google/nearby"
  LANGUAGES 
    C
    CXX
    #OBJC
    #OBJCPP
    #Swift
)

# Targets
add_subdirectory("proto")
add_subdirectory("internal/analytics")
add_subdirectory("internal/proto")
add_subdirectory("internal/crypto")
add_subdirectory("internal/flags")
add_subdirectory("internal/base")
add_subdirectory("internal/interop")
add_subdirectory("internal/network")
add_subdirectory("internal/platform")
add_subdirectory("internal/test")
add_subdirectory("internal/weave")
add_subdirectory("connections")
