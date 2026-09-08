#pragma once

#include <vector>
#include <type_traits>

namespace OGLCore {

/// Scalar component type of a vertex attribute.
///
/// Deliberately not a GL enum: keeping this an OGLCore type is what allows
/// this header to stay free of any GL include.
enum class VertexAttribType
{
	Float,
	UInt,
	UByte
};

/// Size in bytes of a single component of @p type.
unsigned int SizeOfAttribType(VertexAttribType type);

/// One attribute within a vertex.
struct VertexBufferElement
{
	VertexAttribType type;
	unsigned int     count;
	bool             normalized;
};

/// Describes the memory layout of a single vertex, in attribute order.
///
/// Attributes are declared in the order they appear in the buffer; stride and
/// per-attribute offsets are derived automatically.
///
/// @code
///   VertexBufferLayout layout;
///   layout.Push<float>(2);   // vec2 position
///   layout.Push<float>(2);   // vec2 texcoord
/// @endcode
class VertexBufferLayout
{
public:
	/// Appends an attribute of @p count components of type @p T.
	/// Supported types: float, unsigned int, unsigned char (normalised).
	template<typename T>
	void Push(unsigned int count)
	{
		if constexpr (std::is_same_v<T, float>)
			Add(VertexAttribType::Float, count, false);
		else if constexpr (std::is_same_v<T, unsigned int>)
			Add(VertexAttribType::UInt, count, false);
		else if constexpr (std::is_same_v<T, unsigned char>)
			Add(VertexAttribType::UByte, count, true);
		else
			static_assert(sizeof(T) == 0,
			              "VertexBufferLayout::Push: unsupported attribute type");
	}

	const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }

	/// Total size in bytes of one vertex.
	unsigned int GetStride() const { return m_Stride; }

private:
	void Add(VertexAttribType type, unsigned int count, bool normalized);

	std::vector<VertexBufferElement> m_Elements;
	unsigned int                     m_Stride = 0;
};

} // namespace OGLCore
