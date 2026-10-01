cmake_minimum_required(VERSION 3.15)
project(EaglePatchPlus LANGUAGES CXX)

# Enforce 32-bit x86 architecture on Windows MSVC, as these ASI plugins are built for 32-bit games.
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  if(MSVC)
    message(FATAL_ERROR "EaglePatch+ must be compiled as a 32-bit (x86) target. Please configure CMake with -A Win32.")
  else()
    message(FATAL_ERROR "EaglePatch+ must be compiled as a 32-bit (x86) target. Please configure your toolchain for 32-bit (e.g. gcc -m32).")
  endif()
endif()

# Set output directories matching the original layout
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/EaglePatch/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/EaglePatch/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/EaglePatch/bin")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/EaglePatch/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/EaglePatch/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/EaglePatch/bin")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/EaglePatch/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/EaglePatch/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/EaglePatch/bin")

# Set MSVC runtime library selection to static (corresponds to staticruntime "on" -> /MT or /MTd)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Common compile options
add_compile_options(
    $<$<CXX_COMPILER_ID:MSVC>:/Zc:sizedDealloc->
    $<$<CXX_COMPILER_ID:MSVC>:/fp:fast>
)

# Common sources and headers
set(SHARED_SOURCES
    patcher/patcher.cpp
    patcher/patcher.h
    EaglePatch/shared/console.h
    EaglePatch/shared/ini_reader.h
    EaglePatch/shared/utils.h
    EaglePatch/shared/logger.h
    EaglePatch/shared/console.cpp
    EaglePatch/shared/ini_reader.cpp
    EaglePatch/shared/utils.cpp
    EaglePatch/shared/logger.cpp
)

# Common include directories
set(SHARED_INCLUDE_DIRS
    patcher
)

# ------------------------------------------------------------------------------
# Target: EaglePatchPlusAC1
# ------------------------------------------------------------------------------
add_library(EaglePatchPlusAC1 SHARED
    ${SHARED_SOURCES}
    EaglePatch/src/ac1.cpp
)

target_include_directories(EaglePatchPlusAC1 PRIVATE ${SHARED_INCLUDE_DIRS})

target_compile_definitions(EaglePatchPlusAC1 PRIVATE
    WIN32_LEAN_AND_MEAN
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_DEPRECATE
    _USE_32BIT_TIME_T
    NOMINMAX
    _MBCS
    DLL_NAME="EaglePatchAC1"
)

# MSVC-specific link requirements and settings
target_link_libraries(EaglePatchPlusAC1 PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:legacy_stdio_definitions>
)

target_link_options(EaglePatchPlusAC1 PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/SAFESEH:NO>
    $<$<CXX_COMPILER_ID:MSVC>:/IGNORE:4222>
)

target_compile_options(EaglePatchPlusAC1 PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/O2>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Od>
)

# Enable Link Time Optimization (LTCG) for Release, and change output suffix to .asi
set_target_properties(EaglePatchPlusAC1 PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
    OUTPUT_NAME "EaglePatchAC1"
    SUFFIX ".asi"
)

target_link_options(EaglePatchPlusAC1 PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/OPT:NOICF>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/INCREMENTAL:NO>
)

# ------------------------------------------------------------------------------
# Target: EaglePatchPlusAC2
# ------------------------------------------------------------------------------
add_library(EaglePatchPlusAC2 SHARED
    ${SHARED_SOURCES}
    EaglePatch/src/ac2.cpp
)

target_include_directories(EaglePatchPlusAC2 PRIVATE ${SHARED_INCLUDE_DIRS})

target_compile_definitions(EaglePatchPlusAC2 PRIVATE
    WIN32_LEAN_AND_MEAN
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_DEPRECATE
    _USE_32BIT_TIME_T
    NOMINMAX
    _MBCS
    DLL_NAME="EaglePatchAC2"
)

# MSVC-specific link requirements and settings
target_link_libraries(EaglePatchPlusAC2 PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:legacy_stdio_definitions>
)

target_link_options(EaglePatchPlusAC2 PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/SAFESEH:NO>
    $<$<CXX_COMPILER_ID:MSVC>:/IGNORE:4222>
)

target_compile_options(EaglePatchPlusAC2 PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/O2>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Od>
)

# Enable Link Time Optimization (LTCG) for Release, and change output suffix to .asi
set_target_properties(EaglePatchPlusAC2 PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
    OUTPUT_NAME "EaglePatchAC2"
    SUFFIX ".asi"
)

target_link_options(EaglePatchPlusAC2 PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/OPT:NOICF>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/INCREMENTAL:NO>
)

# ------------------------------------------------------------------------------
# Target: Unit Tests
# ------------------------------------------------------------------------------
option(BUILD_TESTING "Build unit tests" ON)
if(BUILD_TESTING)
    enable_testing()
    add_executable(EaglePatchTests
        tests/test_ini_reader.cpp
        EaglePatch/shared/ini_reader.cpp
        EaglePatch/shared/utils.cpp
        EaglePatch/shared/logger.cpp
    )
    target_include_directories(EaglePatchTests PRIVATE EaglePatch/shared patcher)
    target_compile_definitions(EaglePatchTests PRIVATE WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS NOMINMAX)
    add_test(NAME EaglePatchTests COMMAND EaglePatchTests)
endif()
# ------------------------------------------------------------------------------
# Target: Package Release
# ------------------------------------------------------------------------------
add_custom_target(package_release
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/dist/EaglePatch+AC1/scripts"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/dist/EaglePatch+AC2/scripts"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:EaglePatchPlusAC1>" "${CMAKE_SOURCE_DIR}/dist/EaglePatch+AC1/scripts/EaglePatchAC1.asi"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/EaglePatch/EaglePatchAC1.ini" "${CMAKE_SOURCE_DIR}/dist/EaglePatch+AC1/scripts/EaglePatchAC1.ini"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/EaglePatch/Readme - EaglePatch+AC1.txt" "${CMAKE_SOURCE_DIR}/dist/EaglePatch+AC1/"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:EaglePatchPlusAC2>" "${CMAKE_SOURCE_DIR}/dist/EaglePatch+AC2/scripts/EaglePatchAC2.asi"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/EaglePatch/EaglePatchAC2.ini" "${CMAKE_SOURCE_DIR}/dist/EaglePatch+AC2/scripts/EaglePatchAC2.ini"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/EaglePatch/Readme - EaglePatch+AC2.txt" "${CMAKE_SOURCE_DIR}/dist/EaglePatch+AC2/"
    DEPENDS EaglePatchPlusAC1 EaglePatchPlusAC2
    COMMENT "Packaging EaglePatch+ release distribution to dist/..."
)

