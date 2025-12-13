cmake_minimum_required(VERSION 3.16)

project(ULGL-Embed VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# GLFW Configuration - Build as static library
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_LIBRARY_TYPE STATIC CACHE STRING "" FORCE)

# Add GLFW subdirectory
add_subdirectory(${CMAKE_SOURCE_DIR}/Vendor/glfw)

# GLAD - Static library
set(GLAD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Vendor/glad/include)
set(GLAD_SRC_DIR ${CMAKE_SOURCE_DIR}/Vendor/glad/src)

add_library(glad STATIC
    ${GLAD_SRC_DIR}/glad.c
)

target_include_directories(glad PUBLIC
    ${GLAD_INCLUDE_DIR}
)

# Ultralight SDK Configuration
# Set ULTRALIGHT_SDK_PATH to point to your Ultralight SDK directory
# You can download it from https://ultralig.ht
# Check multiple possible locations
set(ULTRALIGHT_SDK_PATH "${CMAKE_SOURCE_DIR}/Vendor/Ultralight" CACHE PATH "Path to Ultralight SDK")

if(EXISTS "${ULTRALIGHT_SDK_PATH}/include")
    message(STATUS "Found Ultralight SDK at: ${ULTRALIGHT_SDK_PATH}")
    message(STATUS "  - Include directory: ${ULTRALIGHT_SDK_PATH}/include")
    message(STATUS "  - Binary directory: ${ULTRALIGHT_SDK_PATH}/bin")
    message(STATUS "  - Library directory: ${ULTRALIGHT_SDK_PATH}/lib")
    
    set(ULTRALIGHT_INCLUDE_DIR "${ULTRALIGHT_SDK_PATH}/include")
    set(ULTRALIGHT_BINARY_DIR "${ULTRALIGHT_SDK_PATH}/bin")
    set(ULTRALIGHT_RESOURCES_DIR "${ULTRALIGHT_SDK_PATH}/resources")
    
    if(CMAKE_SYSTEM_NAME MATCHES "Windows")
        set(ULTRALIGHT_LIBRARY_DIR "${ULTRALIGHT_SDK_PATH}/lib")
    else()
        set(ULTRALIGHT_LIBRARY_DIR "${ULTRALIGHT_SDK_PATH}/bin")
    endif()
    
    # Add Ultralight include directories
    include_directories("${ULTRALIGHT_INCLUDE_DIR}")
    link_directories("${ULTRALIGHT_LIBRARY_DIR}")
    
    set(ULTRALIGHT_FOUND TRUE)
    add_definitions(-DULTRALIGHT_FOUND)
else()
    message(WARNING "Ultralight SDK not found at ${ULTRALIGHT_SDK_PATH}")
    message(STATUS "Please download the Ultralight SDK from https://ultralig.ht")
    message(STATUS "Extract it to: ${ULTRALIGHT_SDK_PATH}")
    set(ULTRALIGHT_FOUND FALSE)
endif()

# Main executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/VertexBuffer.cpp
    src/IndexBuffer.cpp
    src/VertexArray.cpp
    src/Shader.cpp
)

# Add Ultralight wrapper if SDK is found
if(ULTRALIGHT_FOUND)
    target_sources(${PROJECT_NAME} PRIVATE
        src/UltralightRenderer.cpp
    )
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glad
)

# Link Ultralight libraries if found
if(ULTRALIGHT_FOUND)
    if(CMAKE_SYSTEM_NAME MATCHES "Windows")
        target_link_libraries(${PROJECT_NAME} PRIVATE
            UltralightCore
            Ultralight
            WebCore
            AppCore
        )
    else()
        # On Unix-like systems, libraries might have different names
        target_link_libraries(${PROJECT_NAME} PRIVATE
            UltralightCore
            Ultralight
            WebCore
            AppCore
        )
    endif()
    
    # Copy Ultralight binaries to output directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ULTRALIGHT_BINARY_DIR}"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
    
    # Copy Ultralight resources to output directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ULTRALIGHT_RESOURCES_DIR}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
    )
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${GLAD_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/Vendor/glfw/include
)

# Add Ultralight include directory if found
if(ULTRALIGHT_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${ULTRALIGHT_INCLUDE_DIR}
    )
endif()

# Platform-specific linking
if(WIN32)
    # Windows-specific libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE
        opengl32
    )
elseif(APPLE)
    # macOS-specific libraries
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(COREVIDEO_FRAMEWORK CoreVideo REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COCOA_FRAMEWORK}
        ${IOKIT_FRAMEWORK}
        ${COREVIDEO_FRAMEWORK}
    )
elseif(UNIX)
    # Linux-specific libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE
        GL
        X11
        pthread
        dl
    )
endif()

