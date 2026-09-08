#include "OGLCore/Renderer.h"

#include "OGLCore/VertexArray.h"
#include "OGLCore/IndexBuffer.h"
#include "OGLCore/Shader.h"
#include "GLDebug.h"

#include <iostream>

namespace OGLCore {

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR)
		;
}

bool GLLogCall(const char* function, const char* file, int line)
{
	bool ok = true;
	while (GLenum error = glGetError()) {
		std::cerr << "[OpenGL_ERROR] (" << error << "): " << function << " "
		          << file << " : " << line << std::endl;
		ok = false;
	}
	return ok;
}

void Renderer::Clear() const
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::SetClearColor(float r, float g, float b, float a) const
{
	GLCall(glClearColor(r, g, b, a));
}

void Renderer::SetViewport(int x, int y, int width, int height) const
{
	GLCall(glViewport(x, y, width, height));
}

void Renderer::SetBlending(bool enabled) const
{
	if (enabled) {
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	} else {
		GLCall(glDisable(GL_BLEND));
	}
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(ib.GetCount()),
	                      GL_UNSIGNED_INT, nullptr));
}

void Renderer::DrawInstanced(const VertexArray& va, const IndexBuffer& ib,
                             const Shader& shader, unsigned int instanceCount) const
{
	if (instanceCount == 0)
		return;

	shader.Bind();
	va.Bind();
	ib.Bind();
	GLCall(glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(ib.GetCount()),
	                               GL_UNSIGNED_INT, nullptr,
	                               static_cast<GLsizei>(instanceCount)));
}

} // namespace OGLCore
