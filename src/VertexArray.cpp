#include "OGLCore/VertexArray.h"

#include "OGLCore/VertexBuffer.h"
#include "OGLCore/VertexBufferLayout.h"
#include "GLDebug.h"

#include <cstdint>

namespace OGLCore {

namespace {
	GLenum ToGLType(VertexAttribType type)
	{
		switch (type) {
			case VertexAttribType::Float: return GL_FLOAT;
			case VertexAttribType::UInt:  return GL_UNSIGNED_INT;
			case VertexAttribType::UByte: return GL_UNSIGNED_BYTE;
		}
		return GL_FLOAT;
	}
}

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
	GLCall(glBindVertexArray(m_RendererID));
}

VertexArray::~VertexArray()
{
	if (m_RendererID)
		GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

VertexArray::VertexArray(VertexArray&& other) noexcept
	: m_RendererID(other.m_RendererID), m_NextAttribIndex(other.m_NextAttribIndex)
{
	other.m_RendererID      = 0;
	other.m_NextAttribIndex = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
	if (this != &other) {
		if (m_RendererID)
			GLCall(glDeleteVertexArrays(1, &m_RendererID));
		m_RendererID            = other.m_RendererID;
		m_NextAttribIndex       = other.m_NextAttribIndex;
		other.m_RendererID      = 0;
		other.m_NextAttribIndex = 0;
	}
	return *this;
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	AddBufferInternal(vb, layout, 0);
}

void VertexArray::AddInstancedBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	AddBufferInternal(vb, layout, 1);
}

void VertexArray::AddBufferInternal(const VertexBuffer& vb,
                                    const VertexBufferLayout& layout,
                                    unsigned int divisor)
{
	Bind();
	vb.Bind();

	const auto& elements = layout.GetElements();
	std::uintptr_t offset = 0;

	for (const auto& element : elements) {
		// Locations continue from where the previous buffer left off, so a
		// per-vertex buffer and a per-instance buffer can coexist in one VAO.
		const unsigned int index = m_NextAttribIndex++;

		GLCall(glEnableVertexAttribArray(index));
		GLCall(glVertexAttribPointer(index,
		                             static_cast<GLint>(element.count),
		                             ToGLType(element.type),
		                             element.normalized ? GL_TRUE : GL_FALSE,
		                             static_cast<GLsizei>(layout.GetStride()),
		                             reinterpret_cast<const void*>(offset)));

		if (divisor != 0) {
			GLCall(glVertexAttribDivisor(index, divisor));
		}

		offset += element.count * SizeOfAttribType(element.type);
	}
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}

} // namespace OGLCore
