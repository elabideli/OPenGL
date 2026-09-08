#pragma once

namespace OGLCore {

/// Owns a GL vertex buffer object.
///
/// Move-only: the buffer name is a unique resource, so copying is deleted to
/// prevent two objects deleting the same GL name.
class VertexBuffer
{
public:
	/// Buffer initialised from @p data and not expected to change (GL_STATIC_DRAW).
	VertexBuffer(const void* data, unsigned int size);

	/// Empty buffer of @p size bytes intended for per-frame updates
	/// (GL_DYNAMIC_DRAW). Fill it with SetData().
	explicit VertexBuffer(unsigned int size);

	~VertexBuffer();

	VertexBuffer(const VertexBuffer&)            = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;
	VertexBuffer(VertexBuffer&& other) noexcept;
	VertexBuffer& operator=(VertexBuffer&& other) noexcept;

	/// Replaces the first @p size bytes of the buffer. Grows the allocation if
	/// @p size exceeds the current capacity.
	void SetData(const void* data, unsigned int size);

	void Bind() const;
	void Unbind() const;

	unsigned int GetCapacity() const { return m_Capacity; }

private:
	unsigned int m_RendererID = 0;
	unsigned int m_Capacity   = 0;
};

} // namespace OGLCore
