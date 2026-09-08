#pragma once

namespace OGLCore {

/// Owns a GL element array buffer of 32-bit indices. Move-only.
class IndexBuffer
{
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	IndexBuffer(const IndexBuffer&)            = delete;
	IndexBuffer& operator=(const IndexBuffer&) = delete;
	IndexBuffer(IndexBuffer&& other) noexcept;
	IndexBuffer& operator=(IndexBuffer&& other) noexcept;

	void Bind() const;
	void Unbind() const;

	/// Number of indices, not bytes.
	unsigned int GetCount() const { return m_Count; }

private:
	unsigned int m_RendererID = 0;
	unsigned int m_Count      = 0;
};

} // namespace OGLCore
