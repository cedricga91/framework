#
# Find the FLEXLM includes and library (FlexlmAPI)
#
# This module defines
# FLEXLM_INCLUDE_DIR, where to find headers to use RLM (FlexlmAPI)
# FLEXLM_LIBRARIES, the libraries to link against to use RLM (FlexlmAPI)
# FLEXLM_LIBRARY_DIRS, the library path to link against to use RLM (FlexlmAPI)
# FLEXLM_FOUND, if false, do not try to use RLM (FlexlmAPI)
# PROTECTION_NAME, return the protection name : RLM

if(NOT RLM_ROOT)
  set(RLM_ROOT $ENV{RLM_ROOT})
endif()

# Replace \ by / in RLM_ROOT
if(RLM_ROOT)
  string(REPLACE "\\" "/" RLM_ROOT ${RLM_ROOT})
endif()

# HINTS can be removed when using find_package for FLEXLM
FIND_PATH(FLEXLM_INCLUDE_DIR FlexlmAPI.h HINTS ${RLM_ROOT}/include)


SET(FLEXLM_LIBRARY)
SET(FLEXLM_LIBRARY_FAILED)

IF(WIN32)
  FOREACH(WANTED_LIB licenseapi rlm)
    FIND_LIBRARY(FLEXLM_SUB_LIBRARY_${WANTED_LIB} ${WANTED_LIB} HINTS ${RLM_ROOT}/lib)
    MESSAGE(STATUS "Look for RLM lib ${WANTED_LIB} : ${FLEXLM_SUB_LIBRARY_${WANTED_LIB}}")
    IF(FLEXLM_SUB_LIBRARY_${WANTED_LIB})
      SET(FLEXLM_LIBRARY ${FLEXLM_LIBRARY} ${FLEXLM_SUB_LIBRARY_${WANTED_LIB}})
      GET_FILENAME_COMPONENT(FLEXLM_SUB_PATHLIB_${WANTED_LIB} ${FLEXLM_SUB_LIBRARY_${WANTED_LIB}} PATH)
      LIST(APPEND FLEXLM_LIBRARY_DIRS ${FLEXLM_LIBRARY_DIRS} ${FLEXLM_SUB_PATHLIB_${WANTED_LIB}})
    ELSE(FLEXLM_SUB_LIBRARY_${WANTED_LIB})
      SET(FLEXLM_LIBRARY_FAILED "YES")
    ENDIF(FLEXLM_SUB_LIBRARY_${WANTED_LIB})
  ENDFOREACH(WANTED_LIB)
ELSE(WIN32)
  FOREACH(WANTED_LIB licenseapi rlm)
    FIND_LIBRARY(FLEXLM_SUB_LIBRARY_${WANTED_LIB} ${WANTED_LIB} HINTS ${RLM_ROOT}/lib)
    MESSAGE(STATUS "Look for RLM lib ${WANTED_LIB} : ${FLEXLM_SUB_LIBRARY_${WANTED_LIB}}")
    IF(FLEXLM_SUB_LIBRARY_${WANTED_LIB})
      GET_FILENAME_COMPONENT(FLEXLM_SUB_NAMELIB_${WANTED_LIB} ${FLEXLM_SUB_LIBRARY_${WANTED_LIB}} NAME_WE)
      STRING(REGEX REPLACE "^lib" "" FLEXLM_SUB_NAMELIB_${WANTED_LIB} ${FLEXLM_SUB_NAMELIB_${WANTED_LIB}})
      GET_FILENAME_COMPONENT(FLEXLM_SUB_PATHLIB_${WANTED_LIB} ${FLEXLM_SUB_LIBRARY_${WANTED_LIB}} PATH)
      # SET(FLEXLM_LIBRARY ${FLEXLM_LIBRARY} ${FLEXLM_SUB_LIBRARY_${WANTED_LIB}})
      SET(FLEXLM_LIBRARY ${FLEXLM_LIBRARY} ${FLEXLM_SUB_NAMELIB_${WANTED_LIB}})
      LIST(APPEND FLEXLM_LIBRARY_DIRS ${FLEXLM_LIBRARY_DIRS} ${FLEXLM_SUB_PATHLIB_${WANTED_LIB}})
    ELSE(FLEXLM_SUB_LIBRARY_${WANTED_LIB})
      SET(FLEXLM_LIBRARY_FAILED "YES")
    ENDIF(FLEXLM_SUB_LIBRARY_${WANTED_LIB})
  ENDFOREACH(WANTED_LIB)
ENDIF(WIN32)

SET(FLEXLM_FOUND "NO")
IF(FLEXLM_INCLUDE_DIR)
  IF(FLEXLM_LIBRARY_FAILED)
    # erreur dans une recherche de lib
  ELSE(FLEXLM_LIBRARY_FAILED)
    SET(FLEXLM_FOUND "YES")
    SET(PROTECTION_NAME "RLM")
    # Biblioth�ques syst�mes suppl�mentaires
    if(WIN32)
      SET(FLEXLM_LIBRARIES ${FLEXLM_LIBRARY} oldnames.lib kernel32.lib user32.lib netapi32.lib
        advapi32.lib gdi32.lib comdlg32.lib comctl32.lib wsock32.lib shell32.lib
        Rpcrt4.lib oleaut32.lib Ole32.lib Wbemuuid.lib wintrust.lib crypt32.lib Ws2_32.lib psapi.lib Shlwapi.lib dhcpcsvc.lib
        userenv.lib legacy_stdio_definitions.lib vcruntime.lib ucrt.lib legacy_stdio_wide_specifiers.lib libvcruntime.lib)
    else(WIN32)
      SET(FLEXLM_LIBRARIES ${FLEXLM_LIBRARY} pthread)
    endif(WIN32)
    SET(FLEXLM_INCLUDE_DIRS ${FLEXLM_INCLUDE_DIR})
    LIST(REMOVE_DUPLICATES FLEXLM_LIBRARY_DIRS)
  ENDIF(FLEXLM_LIBRARY_FAILED)
ENDIF(FLEXLM_INCLUDE_DIR)
