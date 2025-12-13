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

# Main executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/VertexBuffer.cpp
    src/IndexBuffer.cpp
    src/VertexArray.cpp
    src/Shader.cpp
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glad
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${GLAD_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/Vendor/glfw/include
)

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

