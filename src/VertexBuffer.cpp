#include "OGLCore/VertexBuffer.h"

#include "GLDebug.h"

namespace OGLCore {

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
	: m_Capacity(size)
{
	GLCall(glGenBuffers(1, &m_RendererID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW));
}

VertexBuffer::VertexBuffer(unsigned int size)
	: m_Capacity(size)
{
	GLCall(glGenBuffers(1, &m_RendererID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_DYNAMIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
	if (m_RendererID)
		GLCall(glDeleteBuffers(1, &m_RendererID));
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
	: m_RendererID(other.m_RendererID), m_Capacity(other.m_Capacity)
{
	other.m_RendererID = 0;
	other.m_Capacity   = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
{
	if (this != &other) {
		if (m_RendererID)
			GLCall(glDeleteBuffers(1, &m_RendererID));
		m_RendererID       = other.m_RendererID;
		m_Capacity         = other.m_Capacity;
		other.m_RendererID = 0;
		other.m_Capacity   = 0;
	}
	return *this;
}

void VertexBuffer::SetData(const void* data, unsigned int size)
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));

	if (size > m_Capacity) {
		// Reallocate rather than overrun. Keeps streaming callers correct when
		// the frame's instance count exceeds the initial estimate.
		GLCall(glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, GL_DYNAMIC_DRAW));
		m_Capacity = size;
		return;
	}

	GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), data));
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

} // namespace OGLCore
