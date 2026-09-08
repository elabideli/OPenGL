#include "OGLCore/IndexBuffer.h"

#include "GLDebug.h"

namespace OGLCore {

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
	: m_Count(count)
{
	GLCall(glGenBuffers(1, &m_RendererID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	                    static_cast<GLsizeiptr>(count * sizeof(unsigned int)),
	                    data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
	if (m_RendererID)
		GLCall(glDeleteBuffers(1, &m_RendererID));
}

IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
	: m_RendererID(other.m_RendererID), m_Count(other.m_Count)
{
	other.m_RendererID = 0;
	other.m_Count      = 0;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept
{
	if (this != &other) {
		if (m_RendererID)
			GLCall(glDeleteBuffers(1, &m_RendererID));
		m_RendererID       = other.m_RendererID;
		m_Count            = other.m_Count;
		other.m_RendererID = 0;
		other.m_Count      = 0;
	}
	return *this;
}

void IndexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}

void IndexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

} // namespace OGLCore
