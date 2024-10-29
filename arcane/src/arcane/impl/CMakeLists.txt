# FlexLM
add_include_directories_if_found(FLEXLM)
# RLM
add_include_directories_if_found(RLM)

include(srcs.cmake)

# FlexLM
set(PKGS FLEXLM)
if(FLEXLM_FOUND)
  set(ARCANE_SOURCES ${ARCANE_SOURCES} FlexLMTools.cc)
endif()

# RLM
set(PKGS RLM)
if(RLM_FOUND)
  set(ARCANE_SOURCES ${ARCANE_SOURCES} FlexLMTools.cc)
endif()

arcane_add_library(arcane_impl
  INPUT_PATH ${Arcane_SOURCE_DIR}/src
  RELATIVE_PATH arcane/impl
  AXL_FILES ${AXL_FILES}
  FILES ${ARCANE_SOURCES}
  )

arcane_add_arccon_packages(arcane_impl PRIVATE ${PKGS})

target_compile_definitions(arcane_impl PRIVATE ARCANE_COMPONENT_arcane_impl)

target_link_libraries(arcane_impl PUBLIC arcane_core)

# Avec les versions statiques de la glib, cela est nécessaire
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(STATUS "Add library 'dl' to target 'arcane_impl'")
  target_link_libraries(arcane_impl PUBLIC dl)
endif()

# FlexLM
target_link_directories(arcane_utils PUBLIC ${FLEXLM_LIBRARY_DIRS})
target_link_libraries(arcane_utils PUBLIC ${FLEXLM_LIBRARIES})

# RLM
target_link_directories(arcane_utils PUBLIC ${RLM_LIBRARY_DIRS})
target_link_libraries(arcane_utils PUBLIC ${RLM_LIBRARIES})

arcane_register_library(arcane_impl)
