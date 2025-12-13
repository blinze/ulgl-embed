cmake_minimum_required(VERSION 3.16)

project(ULGL-Embed VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(PYTHON_EXECUTABLE python python3)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_LIBRARY_TYPE STATIC CACHE STRING "" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/Vendor/glfw)

set(GLAD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Vendor/glad/include)
set(GLAD_SRC_DIR ${CMAKE_SOURCE_DIR}/Vendor/glad/src)

add_library(glad STATIC
    ${GLAD_SRC_DIR}/glad.c
)

target_include_directories(glad PUBLIC
    ${GLAD_INCLUDE_DIR}
)

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

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/VertexBuffer.cpp
    src/IndexBuffer.cpp
    src/VertexArray.cpp
    src/Shader.cpp
    src/Texture.cpp
)

if(ULTRALIGHT_FOUND)
    target_sources(${PROJECT_NAME} PRIVATE
        src/UltralightRenderer.cpp
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glad
)

if(ULTRALIGHT_FOUND)
    if(CMAKE_SYSTEM_NAME MATCHES "Windows")
        target_link_libraries(${PROJECT_NAME} PRIVATE
            UltralightCore
            Ultralight
            WebCore
            AppCore
        )
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE
            UltralightCore
            Ultralight
            WebCore
            AppCore
        )
    endif()
    
    if(PYTHON_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Copying Ultralight binaries from ${ULTRALIGHT_BINARY_DIR} to $<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/cmake_copy_dir.py"
                "${ULTRALIGHT_BINARY_DIR}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E echo "Ultralight binaries copied"
        )
        
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/cmake_copy_dir.py"
                "${ULTRALIGHT_RESOURCES_DIR}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
        )
    else()
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Copying Ultralight binaries from ${ULTRALIGHT_BINARY_DIR} to $<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ULTRALIGHT_BINARY_DIR}"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND ${CMAKE_COMMAND} -E echo "Ultralight binaries copied"
        )
        
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ULTRALIGHT_RESOURCES_DIR}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
        )
    endif()
endif()

if(PYTHON_EXECUTABLE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/cmake_copy_dir.py"
            "${CMAKE_SOURCE_DIR}/app"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/app"
    )
else()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/app"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/app"
    )
endif()

set(DEPLOY_DIR "${CMAKE_BINARY_DIR}/deploy")
add_custom_target(Deploy ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPLOY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "Creating deployment directory at ${DEPLOY_DIR}..."
    DEPENDS ${PROJECT_NAME}
)

add_custom_command(TARGET Deploy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:${PROJECT_NAME}>
    "${DEPLOY_DIR}/"
)

if(UNIX AND NOT APPLE)
    add_custom_command(TARGET Deploy POST_BUILD
        COMMAND chmod +x "${DEPLOY_DIR}/${PROJECT_NAME}"
        COMMENT "Setting execute permissions on ${PROJECT_NAME}"
    )
elseif(APPLE)
    add_custom_command(TARGET Deploy POST_BUILD
        COMMAND chmod +x "${DEPLOY_DIR}/${PROJECT_NAME}"
        COMMENT "Setting execute permissions on ${PROJECT_NAME}"
    )
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/Assets")
    if(PYTHON_EXECUTABLE)
        add_custom_command(TARGET Deploy POST_BUILD
            COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/cmake_copy_dir.py"
                "${CMAKE_SOURCE_DIR}/Assets"
                "${DEPLOY_DIR}/Assets"
        )
    else()
        add_custom_command(TARGET Deploy POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/Assets"
            "${DEPLOY_DIR}/Assets"
        )
    endif()
endif()

if(PYTHON_EXECUTABLE)
    add_custom_command(TARGET Deploy POST_BUILD
        COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/cmake_copy_dir.py"
            "${CMAKE_SOURCE_DIR}/app"
            "${DEPLOY_DIR}/app"
    )
else()
    add_custom_command(TARGET Deploy POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/app"
        "${DEPLOY_DIR}/app"
    )
endif()

if(ULTRALIGHT_FOUND)
    if(PYTHON_EXECUTABLE)
        add_custom_command(TARGET Deploy POST_BUILD
            COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/cmake_copy_dir.py"
                "${ULTRALIGHT_BINARY_DIR}"
                "${DEPLOY_DIR}"
        )
        
        add_custom_command(TARGET Deploy POST_BUILD
            COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/cmake_copy_dir.py"
                "${ULTRALIGHT_RESOURCES_DIR}"
                "${DEPLOY_DIR}/resources"
        )
    else()
        add_custom_command(TARGET Deploy POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ULTRALIGHT_BINARY_DIR}"
            "${DEPLOY_DIR}"
        )
        
        add_custom_command(TARGET Deploy POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ULTRALIGHT_RESOURCES_DIR}"
            "${DEPLOY_DIR}/resources"
        )
    endif()
    
    message(STATUS "")
    message(STATUS "=== Deployment Setup ===")
    message(STATUS "Deployment directory: ${DEPLOY_DIR}")
    message(STATUS "  This directory contains:")
    message(STATUS "    - ULGL-Embed.exe")
    message(STATUS "    - All Ultralight DLLs")
    message(STATUS "    - resources/ folder")
    message(STATUS "    - Assets/ folder")
    message(STATUS "    - app/ folder (React app)")
    message(STATUS "")
    message(STATUS "To run: Use 'cmake --build . --target Run'")
    message(STATUS "")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(EXECUTABLE_NAME "${PROJECT_NAME}.exe")
    add_custom_target(Run
        COMMAND ${CMAKE_COMMAND} -E echo "Running ${PROJECT_NAME} from deployment directory..."
        COMMAND ${CMAKE_COMMAND} -E chdir "${DEPLOY_DIR}" "${EXECUTABLE_NAME}"
        DEPENDS Deploy
        COMMENT "Running ${PROJECT_NAME} from deployment directory (CMake will wait for the window to close)"
        USES_TERMINAL
    )
else()
    set(EXECUTABLE_NAME "${PROJECT_NAME}")
    add_custom_target(Run
        COMMAND ${CMAKE_COMMAND} -E echo "Running ${PROJECT_NAME} from deployment directory..."
        COMMAND ${CMAKE_COMMAND} -E chdir "${DEPLOY_DIR}" ./${EXECUTABLE_NAME}
        DEPENDS Deploy
        COMMENT "Running ${PROJECT_NAME} from deployment directory (CMake will wait for the window to close)"
        USES_TERMINAL
    )
endif()

add_custom_target(RunBin
    COMMAND ${CMAKE_COMMAND} -E echo "Running ${PROJECT_NAME} from bin directory..."
    COMMAND $<TARGET_FILE:${PROJECT_NAME}>
    DEPENDS ${PROJECT_NAME}
    COMMENT "Running ${PROJECT_NAME} from bin directory"
    USES_TERMINAL
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${GLAD_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/Vendor/glfw/include
)

if(ULTRALIGHT_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${ULTRALIGHT_INCLUDE_DIR}
    )
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        opengl32
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
elseif(APPLE)
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(COREVIDEO_FRAMEWORK CoreVideo REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COCOA_FRAMEWORK}
        ${IOKIT_FRAMEWORK}
        ${COREVIDEO_FRAMEWORK}
    )
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        GL
        X11
        pthread
        dl
    )
endif()

