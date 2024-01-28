# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/D/Work/GNearbyCmake/GNearby/build/_deps/boringssl-src"
  "C:/D/Work/GNearbyCmake/GNearby/build/_deps/boringssl-build"
  "C:/D/Work/GNearbyCmake/GNearby/build/_deps/boringssl-subbuild/boringssl-populate-prefix"
  "C:/D/Work/GNearbyCmake/GNearby/build/_deps/boringssl-subbuild/boringssl-populate-prefix/tmp"
  "C:/D/Work/GNearbyCmake/GNearby/build/_deps/boringssl-subbuild/boringssl-populate-prefix/src/boringssl-populate-stamp"
  "C:/D/Work/GNearbyCmake/GNearby/build/_deps/boringssl-subbuild/boringssl-populate-prefix/src"
  "C:/D/Work/GNearbyCmake/GNearby/build/_deps/boringssl-subbuild/boringssl-populate-prefix/src/boringssl-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/D/Work/GNearbyCmake/GNearby/build/_deps/boringssl-subbuild/boringssl-populate-prefix/src/boringssl-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/D/Work/GNearbyCmake/GNearby/build/_deps/boringssl-subbuild/boringssl-populate-prefix/src/boringssl-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
