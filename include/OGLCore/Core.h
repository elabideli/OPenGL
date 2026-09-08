#pragma once

// OGLCore - a small OpenGL 3.3 core-profile rendering library.
//
// The library is deliberately windowing-agnostic: it never includes GLFW,
// SDL or any other platform header. The consumer creates the window and the
// GL context, then hands OGLCore a loader function so it can resolve the
// OpenGL entry points it needs.

namespace OGLCore {

/// Pointer to an OpenGL entry point.
/// Layout-compatible with GLFWglproc, SDL_FunctionPointer and GLADapiproc.
using GLProc = void (*)();

/// Loader supplied by the windowing layer. GLFW's glfwGetProcAddress and
/// SDL's SDL_GL_GetProcAddress both match this signature exactly, so they
/// can be passed without a cast.
using GLProcLoader = GLProc (*)(const char* name);

/// Resolves the OpenGL 3.3 core entry points.
///
/// Must be called exactly once, after a GL context is current on the calling
/// thread and before any other OGLCore object is constructed. Constructing a
/// VertexBuffer, Shader or Texture beforehand is undefined behaviour.
///
/// @param loader  entry-point resolver, e.g. glfwGetProcAddress
/// @return        true if a 3.3 core context was loaded successfully
bool Init(GLProcLoader loader);

/// True once Init() has returned true.
bool IsInitialized();

/// GL_VERSION of the current context, or "" before a successful Init().
/// The returned string is owned by the GL driver and outlives the context.
const char* VersionString();

/// GL_RENDERER of the current context, or "" before a successful Init().
const char* RendererString();

} // namespace OGLCore
