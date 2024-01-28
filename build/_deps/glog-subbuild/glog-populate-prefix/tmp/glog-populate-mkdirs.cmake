# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/D/Work/GNearbyCmake/GNearby/build/_deps/glog-src"
  "C:/D/Work/GNearbyCmake/GNearby/build/_deps/glog-build"
  "C:/D/Work/GNearbyCmake/GNearby/build/_deps/glog-subbuild/glog-populate-prefix"
  "C:/D/Work/GNearbyCmake/GNearby/build/_deps/glog-subbuild/glog-populate-prefix/tmp"
  "C:/D/Work/GNearbyCmake/GNearby/build/_deps/glog-subbuild/glog-populate-prefix/src/glog-populate-stamp"
  "C:/D/Work/GNearbyCmake/GNearby/build/_deps/glog-subbuild/glog-populate-prefix/src"
  "C:/D/Work/GNearbyCmake/GNearby/build/_deps/glog-subbuild/glog-populate-prefix/src/glog-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/D/Work/GNearbyCmake/GNearby/build/_deps/glog-subbuild/glog-populate-prefix/src/glog-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/D/Work/GNearbyCmake/GNearby/build/_deps/glog-subbuild/glog-populate-prefix/src/glog-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
