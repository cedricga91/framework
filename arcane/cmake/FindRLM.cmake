#
# Find the RLM includes and library
#
# This module defines
# LICENSE_SYSTEM_INCLUDE_DIR, where to find headers,
# LICENSE_SYSTEM_LIBRARIES, the libraries to link against to use RLM.
# LICENSE_SYSTEM_LIBRARY_DIRS, the library path to link against to use RLM.
# LICENSE_SYSTEM_FOUND, If false, do not try to use RLM.
# LICENSE_SYSTEM_NAME, RLM.

if(NOT RLM_ROOT)
  set(RLM_ROOT $ENV{RLM_ROOT})
endif()

# Replace \ by / in RLM_ROOT
if(RLM_ROOT)
  string(REPLACE "\\" "/" RLM_ROOT ${RLM_ROOT})
endif()

# HINTS can be removed when using find_package for RLM
FIND_PATH(LICENSE_SYSTEM_INCLUDE_DIR FlexlmAPI.h HINTS ${RLM_ROOT}/include)


SET(LICENSE_SYSTEM_LIBRARY)
SET(LICENSE_SYSTEM_LIBRARY_FAILED)

IF(WIN32)
  FOREACH(WANTED_LIB licenseapi)
    FIND_LIBRARY(LICENSE_SYSTEM_SUB_LIBRARY_${WANTED_LIB} ${WANTED_LIB})
    # MESSAGE(STATUS "Look for FlexNet lib ${WANTED_LIB} : ${LICENSE_SYSTEM_SUB_LIBRARY_${WANTED_LIB}}")
    IF(LICENSE_SYSTEM_SUB_LIBRARY_${WANTED_LIB})
      SET(LICENSE_SYSTEM_LIBRARY ${LICENSE_SYSTEM_LIBRARY} ${LICENSE_SYSTEM_SUB_LIBRARY_${WANTED_LIB}})
      GET_FILENAME_COMPONENT(LICENSE_SYSTEM_SUB_PATHLIB_${WANTED_LIB} ${LICENSE_SYSTEM_SUB_LIBRARY_${WANTED_LIB}} PATH)
      LIST(APPEND LICENSE_SYSTEM_LIBRARY_DIRS ${LICENSE_SYSTEM_LIBRARY_DIRS} ${LICENSE_SYSTEM_SUB_PATHLIB_${WANTED_LIB}})
    ELSE(LICENSE_SYSTEM_SUB_LIBRARY_${WANTED_LIB})
      SET(LICENSE_SYSTEM_LIBRARY_FAILED "YES")
    ENDIF(LICENSE_SYSTEM_SUB_LIBRARY_${WANTED_LIB})
  ENDFOREACH(WANTED_LIB)
ELSE(WIN32)
  FOREACH(WANTED_LIB licenseapi)
    FIND_LIBRARY(LICENSE_SYSTEM_SUB_LIBRARY_${WANTED_LIB} ${WANTED_LIB} HINTS ${RLM_ROOT}/lib)
    MESSAGE(STATUS "Look for FlexNet lib ${WANTED_LIB} : ${LICENSE_SYSTEM_SUB_LIBRARY_${WANTED_LIB}}")
    IF(LICENSE_SYSTEM_SUB_LIBRARY_${WANTED_LIB})
      GET_FILENAME_COMPONENT(LICENSE_SYSTEM_SUB_NAMELIB_${WANTED_LIB} ${LICENSE_SYSTEM_SUB_LIBRARY_${WANTED_LIB}} NAME_WE)
      STRING(REGEX REPLACE "^lib" "" LICENSE_SYSTEM_SUB_NAMELIB_${WANTED_LIB} ${LICENSE_SYSTEM_SUB_NAMELIB_${WANTED_LIB}})
      GET_FILENAME_COMPONENT(LICENSE_SYSTEM_SUB_PATHLIB_${WANTED_LIB} ${LICENSE_SYSTEM_SUB_LIBRARY_${WANTED_LIB}} PATH)
      # SET(LICENSE_SYSTEM_LIBRARY ${LICENSE_SYSTEM_LIBRARY} ${LICENSE_SYSTEM_SUB_LIBRARY_${WANTED_LIB}})
      SET(LICENSE_SYSTEM_LIBRARY ${LICENSE_SYSTEM_LIBRARY} ${LICENSE_SYSTEM_SUB_NAMELIB_${WANTED_LIB}})
      LIST(APPEND LICENSE_SYSTEM_LIBRARY_DIRS ${LICENSE_SYSTEM_LIBRARY_DIRS} ${LICENSE_SYSTEM_SUB_PATHLIB_${WANTED_LIB}})
    ELSE(LICENSE_SYSTEM_SUB_LIBRARY_${WANTED_LIB})
      SET(LICENSE_SYSTEM_LIBRARY_FAILED "YES")
    ENDIF(LICENSE_SYSTEM_SUB_LIBRARY_${WANTED_LIB})
  ENDFOREACH(WANTED_LIB)
ENDIF(WIN32)

SET(LICENSE_SYSTEM_FOUND "NO")
IF(LICENSE_SYSTEM_INCLUDE_DIR)
  IF(LICENSE_SYSTEM_LIBRARY_FAILED)
    # erreur dans une recherche de lib
  ELSE(LICENSE_SYSTEM_LIBRARY_FAILED)
    SET(LICENSE_SYSTEM_FOUND "YES")
    SET(LICENSE_SYSTEM_NAME "RLM")
    # Bibliothèques systèmes supplémentaires
    if(WIN32)
      SET(LICENSE_SYSTEM_LIBRARIES ${LICENSE_SYSTEM_LIBRARY} oldnames.lib kernel32.lib user32.lib netapi32.lib
        advapi32.lib gdi32.lib comdlg32.lib comctl32.lib wsock32.lib shell32.lib
        Rpcrt4.lib oleaut32.lib Ole32.lib Wbemuuid.lib wintrust.lib crypt32.lib Ws2_32.lib psapi.lib Shlwapi.lib dhcpcsvc.lib
        userenv.lib legacy_stdio_definitions.lib vcruntime.lib ucrt.lib legacy_stdio_wide_specifiers.lib libvcruntime.lib)
    else(WIN32)
      SET(LICENSE_SYSTEM_LIBRARIES ${LICENSE_SYSTEM_LIBRARY} pthread)
    endif(WIN32)
    SET(LICENSE_SYSTEM_INCLUDE_DIRS ${LICENSE_SYSTEM_INCLUDE_DIR})
    LIST(REMOVE_DUPLICATES LICENSE_SYSTEM_LIBRARY_DIRS)
  ENDIF(LICENSE_SYSTEM_LIBRARY_FAILED)
ENDIF(LICENSE_SYSTEM_INCLUDE_DIR)
