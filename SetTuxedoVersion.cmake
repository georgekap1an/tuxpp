macro(set_tuxedo_version)
   # determine Tuxedo version in use
   # assume at least 12.1.1
   set(TUXEDO_VERSION 1211)

   # check for specific features
   find_library(FML32_LIB "fml32" PATHS "$ENV{TUXDIR}/lib" NO_DEFAULT_PATH)
   find_library(TUX_LIB "tux" PATHS "$ENV{TUXDIR}/lib" NO_DEFAULT_PATH)
   set(CMAKE_REQUIRED_INCLUDES "$ENV{TUXDIR}/include") 
   set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_DL_LIBS} ${TUX_LIB} ${FML32_LIB}) 
   set(CMAKE_EXTRA_INCLUDE_FILES fml32.h)
   check_type_size("RECORD" RECORD_STRUCT LANGUAGE CXX)
   if(${HAVE_RECORD_STRUCT})
      set(TUXEDO_VERSION 1213)
   endif()

   check_cxx_symbol_exists("tpadvertisex" atmi.h HAS_TPADVERTISEX)
   if(${HAS_TPADVERTISEX})
      set(TUXEDO_VERSION 1222)
   endif()
   add_definitions(-DTUXEDO_VERSION=${TUXEDO_VERSION} )
   message("Setting TUXEDO_VERSION = " ${TUXEDO_VERSION})
endmacro() 