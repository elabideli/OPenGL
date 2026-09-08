#include "OGLCore/VertexBufferLayout.h"

// Deliberately no GL include: the layout description is pure data. The
// mapping to GL enums happens in VertexArray.cpp, where it is actually needed.

namespace OGLCore {

unsigned int SizeOfAttribType(VertexAttribType type)
{
	switch (type) {
		case VertexAttribType::Float: return 4;
		case VertexAttribType::UInt:  return 4;
		case VertexAttribType::UByte: return 1;
	}
	return 0;
}

void VertexBufferLayout::Add(VertexAttribType type, unsigned int count, bool normalized)
{
	m_Elements.push_back({ type, count, normalized });
	m_Stride += count * SizeOfAttribType(type);
}

} // namespace OGLCore
