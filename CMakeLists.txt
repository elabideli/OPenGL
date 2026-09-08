cmake_minimum_required(VERSION 3.21)
project(OGLCore VERSION 0.1.0 LANGUAGES C CXX)

option(OGLCORE_BUILD_SANDBOX "Build the OGLCore sandbox demo" ${PROJECT_IS_TOP_LEVEL})
option(OGLCORE_INSTALL       "Generate install and export rules" ${PROJECT_IS_TOP_LEVEL})

find_package(OpenGL REQUIRED)

# ---------------------------------------------------------------------------
# The library
# ---------------------------------------------------------------------------
add_library(OGLCore STATIC
    src/Core.cpp
    src/Renderer.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/VertexArray.cpp
    src/VertexBuffer.cpp
    src/VertexBufferLayout.cpp
    src/IndexBuffer.cpp
    vendor/glad/src/gl.c
    vendor/stb_image/stb_image.cpp)

add_library(OGLCore::OGLCore ALIAS OGLCore)

# glad, stb_image and src/ are PRIVATE on purpose: if a public header ever
# reaches for one of them, a consumer's build breaks immediately.
target_include_directories(OGLCore
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include)

target_compile_features(OGLCore PUBLIC cxx_std_17)
target_link_libraries(OGLCore PUBLIC OpenGL::GL PRIVATE ${CMAKE_DL_LIBS})

if(MSVC)
    target_compile_options(OGLCore PRIVATE /W4 /permissive-)
else()
    target_compile_options(OGLCore PRIVATE -Wall -Wextra)
endif()

set_target_properties(OGLCore PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    C_STANDARD 11)

# ---------------------------------------------------------------------------
# Sandbox demo - also the smoke test that the public API is usable standalone
# ---------------------------------------------------------------------------
if(OGLCORE_BUILD_SANDBOX)
    include(FetchContent)
    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
        GIT_SHALLOW    TRUE)
    FetchContent_MakeAvailable(glfw)

    add_executable(OGLCoreSandbox sandbox/Application.cpp)
    target_link_libraries(OGLCoreSandbox PRIVATE OGLCore::OGLCore glfw)

    add_custom_command(TARGET OGLCoreSandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/sandbox/Ressources
                $<TARGET_FILE_DIR:OGLCoreSandbox>/Ressources
        COMMENT "Copying sandbox resources next to the executable")
endif()

# ---------------------------------------------------------------------------
# Install / export - lets consumers use find_package(OGLCore) as well as
# FetchContent
# ---------------------------------------------------------------------------
if(OGLCORE_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(TARGETS OGLCore EXPORT OGLCoreTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(EXPORT OGLCoreTargets
        FILE        OGLCoreTargets.cmake
        NAMESPACE   OGLCore::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/OGLCore)

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/OGLCoreConfigVersion.cmake
        VERSION       ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/OGLCoreConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/OGLCoreConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/OGLCore)

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/OGLCoreConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/OGLCoreConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/OGLCore)
endif()
