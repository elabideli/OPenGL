#include "OGLCore/Core.h"

#include "GLDebug.h"

#include <iostream>

namespace OGLCore {

namespace {
	bool s_Initialized = false;

	const char* QueryString(GLenum name)
	{
		if (!s_Initialized)
			return "";
		const GLubyte* value = glGetString(name);
		return value ? reinterpret_cast<const char*>(value) : "";
	}
}

bool Init(GLProcLoader loader)
{
	if (s_Initialized)
		return true;

	if (!loader) {
		std::cerr << "[OGLCore] Init: null loader" << std::endl;
		return false;
	}

	if (gladLoadGL(reinterpret_cast<GLADloadfunc>(loader)) == 0) {
		std::cerr << "[OGLCore] Init: failed to load OpenGL 3.3 core" << std::endl;
		return false;
	}

	s_Initialized = true;

	// Context creation can leave an error queued on some drivers; clear it so
	// the first real GLCall does not trip the assert on someone else's fault.
	GLClearError();
	return true;
}

bool IsInitialized()      { return s_Initialized; }
const char* VersionString()  { return QueryString(GL_VERSION); }
const char* RendererString() { return QueryString(GL_RENDERER); }

} // namespace OGLCore
