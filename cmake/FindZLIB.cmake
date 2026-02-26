if(NOT ZLIB_FOUND)
  # Try to find the system-installed zlib
  find_path(ZLIB_INCLUDE_DIR NAMES zlib.h)
  find_library(ZLIB_LIBRARY NAMES z zlib)

  if(ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)
    if(NOT TARGET ZLIB::ZLIB)
      add_library(ZLIB::ZLIB UNKNOWN IMPORTED)
      set_target_properties(ZLIB::ZLIB PROPERTIES
        IMPORTED_LOCATION "${ZLIB_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ZLIB_INCLUDE_DIR}")
    endif()
    set(ZLIB_FOUND ON)
  else()
    message(FATAL_ERROR "Could not find zlib. Please install zlib development package.")
  endif()
endif()
