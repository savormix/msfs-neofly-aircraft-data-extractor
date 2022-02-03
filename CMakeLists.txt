cmake_minimum_required(VERSION 3.20)
project(msfs_neofly_aircraft_data_extractor
        VERSION 1.1.3
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(VERSION_SUFFIX)
if(${VERSION_SUFFIX} MATCHES .*verbose.*)
    set(VERBOSE_OUTPUT 1)
else()
    set(VERBOSE_OUTPUT 0)
endif()
configure_file(cond_comp.h.in cond_comp.h)
configure_file(version.h.in version.h)
add_executable(${PROJECT_NAME}
        main.cpp
        res/version.rc
        src/neofly/neofly_aircraft_data.cpp
        src/neofly/neofly_aircraft_data.h
        src/simconnect/simconnect_connection.cpp
        src/simconnect/simconnect_connection.h
        src/msfs/msfs_package_reader.cpp
        src/msfs/msfs_package_reader.h)

# dependencies
find_path(
        SIMCONNECT_SDK_INCLUDES
        NAMES SimConnect.h
        HINTS "$ENV{MSFS_SDK}/SimConnect SDK/include"
)
find_path(
        SIMCONNECT_SDK_LIBS
        NAMES SimConnect.dll SimConnect.lib
        HINTS "$ENV{MSFS_SDK}/SimConnect SDK/lib"
)
find_library(
        SIMCONNECT_SDK_RELEASE
        NAMES SimConnect.dll SimConnect.lib
        HINTS ${SIMCONNECT_SDK_LIBS}
        REQUIRED
)
# system dependencies
find_library(
        SHLWAPI
        NAMES shlwapi.lib
        REQUIRED
)
find_library(
        USER32
        NAMES user32.lib
        REQUIRED
)
find_library(
        WINSOCK2
        NAMES Ws2_32.lib
        REQUIRED
)

# search paths
target_include_directories(${PROJECT_NAME}
        PUBLIC "${PROJECT_BINARY_DIR}"
        PRIVATE ${SIMCONNECT_SDK_INCLUDES})
target_link_directories(${PROJECT_NAME}
        PRIVATE ${SIMCONNECT_SDK_LIBS})

# compile/link options
target_compile_options(${PROJECT_NAME}
        PRIVATE /sdl
        PRIVATE /W3
        PRIVATE /permissive-)
target_link_libraries(${PROJECT_NAME} SimConnect.lib shlwapi.lib user32.lib Ws2_32.lib)
target_link_options(${PROJECT_NAME}
        PRIVATE "/MANIFESTUAC:level='asInvoker' uiAccess='false'")
# file(COPY_FILE ${SIMCONNECT_SDK_LIBS}/SimConnect.dll SimConnect.dll ONLY_IF_DIFFERENT)
configure_file(${SIMCONNECT_SDK_LIBS}/SimConnect.dll SimConnect.dll COPYONLY)
configure_file(LICENSE LICENSE COPYONLY)
configure_file(README.md README.md COPYONLY)

if (${CREATE_VERSION_FILE})
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/version.txt ${PROJECT_VERSION}${VERSION_SUFFIX})
endif()

if(NOT DEFINED MANUAL_PACKAGING OR NOT ${MANUAL_PACKAGING})
    add_custom_command(TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND certutil -hashfile ${PROJECT_NAME}.exe SHA256 | findstr /v /b SHA | findstr /v /b CertUtil > ${PROJECT_NAME}.exe.sha256
            COMMAND certutil -hashfile SimConnect.dll SHA256 | findstr /v /b SHA | findstr /v /b CertUtil > SimConnect.dll.sha256
            COMMAND del ${PROJECT_NAME}-${PROJECT_VERSION}${VERSION_SUFFIX}.zip
            COMMAND 7z a -bd -sse -stl -tzip ${PROJECT_NAME}-${PROJECT_VERSION}${VERSION_SUFFIX}.zip ${PROJECT_NAME}.exe ${PROJECT_NAME}.exe.sha256 SimConnect.dll SimConnect.dll.sha256 LICENSE README.md
            BYPRODUCTS ${PROJECT_NAME}.exe.sha256 SimConnect.dll.sha256 ${PROJECT_NAME}-${PROJECT_VERSION}${VERSION_SUFFIX}.zip
            COMMENT Create SHA256 hashes and release archive
            VERBATIM USES_TERMINAL)
endif()
