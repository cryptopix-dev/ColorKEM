#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ColorKEM::clwe" for configuration "Release"
set_property(TARGET ColorKEM::clwe APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ColorKEM::clwe PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libclwe.a"
  )

list(APPEND _cmake_import_check_targets ColorKEM::clwe )
list(APPEND _cmake_import_check_files_for_ColorKEM::clwe "${_IMPORT_PREFIX}/lib/libclwe.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
