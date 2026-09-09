#pragma once

// Internal to OGLCore. Never installed, never included by a public header:
// this is the only place the glad header and the debug macros live, which is
// what keeps them off the consumer's plate.

// The browser links the WebGL entry points directly, so there is nothing for
// a loader to resolve and glad is neither needed nor buildable there.
#if defined(__EMSCRIPTEN__)
	#include <GLES3/gl3.h>
#else
	#include <glad/gl.h>
#endif

namespace OGLCore {

/// Drains the GL error queue.
void GLClearError();

/// Reports any queued GL error against the given call site.
/// @return true if the queue was empty.
bool GLLogCall(const char* function, const char* file, int line);

} // namespace OGLCore

#if defined(_MSC_VER)
	#define OGLCORE_DEBUG_BREAK() __debugbreak()
#elif defined(__has_builtin)
	#if __has_builtin(__builtin_debugtrap)
		#define OGLCORE_DEBUG_BREAK() __builtin_debugtrap()
	#else
		#include <csignal>
		#define OGLCORE_DEBUG_BREAK() raise(SIGTRAP)
	#endif
#else
	#include <csignal>
	#define OGLCORE_DEBUG_BREAK() raise(SIGTRAP)
#endif

// In release builds the call is issued directly: glGetError() forces a
// pipeline sync, so checking after every call is a debug-only luxury.
#ifdef NDEBUG
	#define GLCall(x) x
#else
	#define GLCall(x)                                                        \
		do {                                                                 \
			::OGLCore::GLClearError();                                       \
			x;                                                               \
			if (!::OGLCore::GLLogCall(#x, __FILE__, __LINE__))               \
				OGLCORE_DEBUG_BREAK();                                       \
		} while (0)
#endif
