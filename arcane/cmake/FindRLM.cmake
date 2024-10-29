#
# Find the FlexlmAPI includes and library
#
# This module defines
# RLM_INCLUDE_DIR, where to find headers,
# RLM_LIBRARIES, the libraries to link against to use FlexlmAPI.
# RLM_LIBRARY_DIRS, the library path to link against to use FlexlmAPI.
# RLM_FOUND, If false, do not try to use FlexlmAPI.

if(NOT RLM_ROOT)
  set(RLM_ROOT $ENV{RLM_ROOT})
endif()

# Replace \ by / in RLM_ROOT
if(RLM_ROOT)
  string(REPLACE "\\" "/" RLM_ROOT ${RLM_ROOT})
endif()

# HINTS can be removed when using find_package for flexlm
FIND_PATH(RLM_INCLUDE_DIR FlexlmAPI.h HINTS ${RLM_ROOT}/include)


SET(RLM_LIBRARY)
SET(RLM_LIBRARY_FAILED)

# par l'inclusion de la lib noact, nous ne visons ici que FlexNet v11 et +
IF(WIN32)
  FOREACH(WANTED_LIB licenseapi)
    FIND_LIBRARY(RLM_SUB_LIBRARY_${WANTED_LIB} ${WANTED_LIB})
    # MESSAGE(STATUS "Look for FlexNet lib ${WANTED_LIB} : ${RLM_SUB_LIBRARY_${WANTED_LIB}}")
    IF(RLM_SUB_LIBRARY_${WANTED_LIB})
      SET(RLM_LIBRARY ${RLM_LIBRARY} ${RLM_SUB_LIBRARY_${WANTED_LIB}})
      GET_FILENAME_COMPONENT(RLM_SUB_PATHLIB_${WANTED_LIB} ${RLM_SUB_LIBRARY_${WANTED_LIB}} PATH)
      LIST(APPEND RLM_LIBRARY_DIRS ${RLM_LIBRARY_DIRS} ${RLM_SUB_PATHLIB_${WANTED_LIB}})
    ELSE(RLM_SUB_LIBRARY_${WANTED_LIB})
      SET(RLM_LIBRARY_FAILED "YES")
    ENDIF(RLM_SUB_LIBRARY_${WANTED_LIB})
  ENDFOREACH(WANTED_LIB)
ELSE(WIN32)
  FOREACH(WANTED_LIB licenseapi)
    FIND_LIBRARY(RLM_SUB_LIBRARY_${WANTED_LIB} ${WANTED_LIB} HINTS ${RLM_ROOT}/lib)
    MESSAGE(STATUS "Look for FlexNet lib ${WANTED_LIB} : ${RLM_SUB_LIBRARY_${WANTED_LIB}}")
    IF(RLM_SUB_LIBRARY_${WANTED_LIB})
      GET_FILENAME_COMPONENT(RLM_SUB_NAMELIB_${WANTED_LIB} ${RLM_SUB_LIBRARY_${WANTED_LIB}} NAME_WE)
      STRING(REGEX REPLACE "^lib" "" RLM_SUB_NAMELIB_${WANTED_LIB} ${RLM_SUB_NAMELIB_${WANTED_LIB}})
      GET_FILENAME_COMPONENT(RLM_SUB_PATHLIB_${WANTED_LIB} ${RLM_SUB_LIBRARY_${WANTED_LIB}} PATH)
      # SET(RLM_LIBRARY ${RLM_LIBRARY} ${RLM_SUB_LIBRARY_${WANTED_LIB}})
      SET(RLM_LIBRARY ${RLM_LIBRARY} ${RLM_SUB_NAMELIB_${WANTED_LIB}})
      LIST(APPEND RLM_LIBRARY_DIRS ${RLM_LIBRARY_DIRS} ${RLM_SUB_PATHLIB_${WANTED_LIB}})
    ELSE(RLM_SUB_LIBRARY_${WANTED_LIB})
      SET(RLM_LIBRARY_FAILED "YES")
    ENDIF(RLM_SUB_LIBRARY_${WANTED_LIB})
  ENDFOREACH(WANTED_LIB)
ENDIF(WIN32)

SET(RLM_FOUND "NO")
IF(RLM_INCLUDE_DIR)
  IF(RLM_LIBRARY_FAILED)
    # erreur dans une recherche de lib
  ELSE(RLM_LIBRARY_FAILED)
    SET(RLM_FOUND "YES")
    # Biblioth�ques syst�mes suppl�mentaires
    if(WIN32)
      SET(RLM_LIBRARIES ${RLM_LIBRARY} oldnames.lib kernel32.lib user32.lib netapi32.lib
        advapi32.lib gdi32.lib comdlg32.lib comctl32.lib wsock32.lib shell32.lib
        Rpcrt4.lib oleaut32.lib Ole32.lib Wbemuuid.lib wintrust.lib crypt32.lib Ws2_32.lib psapi.lib Shlwapi.lib dhcpcsvc.lib
        userenv.lib legacy_stdio_definitions.lib vcruntime.lib ucrt.lib legacy_stdio_wide_specifiers.lib libvcruntime.lib)
    else(WIN32)
      SET(RLM_LIBRARIES ${RLM_LIBRARY} pthread)
    endif(WIN32)
    SET(RLM_INCLUDE_DIRS ${RLM_INCLUDE_DIR})
    LIST(REMOVE_DUPLICATES RLM_LIBRARY_DIRS)
  ENDIF(RLM_LIBRARY_FAILED)
ENDIF(RLM_INCLUDE_DIR)
