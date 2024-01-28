#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "OpenSSL::Crypto" for configuration "MinSizeRel"
set_property(TARGET OpenSSL::Crypto APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(OpenSSL::Crypto PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "ASM_NASM;C"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/crypto.lib"
  )

list(APPEND _cmake_import_check_targets OpenSSL::Crypto )
list(APPEND _cmake_import_check_files_for_OpenSSL::Crypto "${_IMPORT_PREFIX}/lib/crypto.lib" )

# Import target "OpenSSL::SSL" for configuration "MinSizeRel"
set_property(TARGET OpenSSL::SSL APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(OpenSSL::SSL PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/ssl.lib"
  )

list(APPEND _cmake_import_check_targets OpenSSL::SSL )
list(APPEND _cmake_import_check_files_for_OpenSSL::SSL "${_IMPORT_PREFIX}/lib/ssl.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
