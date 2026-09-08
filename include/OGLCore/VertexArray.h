#pragma once

namespace OGLCore {

class VertexBuffer;
class VertexBufferLayout;

/// Owns a GL vertex array object: the binding between buffers and the
/// attribute slots a shader reads. Move-only.
///
/// Attribute locations are handed out consecutively across calls, so a vertex
/// buffer added first occupies locations 0..n and an instance buffer added
/// afterwards continues from n+1.
class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	VertexArray(const VertexArray&)            = delete;
	VertexArray& operator=(const VertexArray&) = delete;
	VertexArray(VertexArray&& other) noexcept;
	VertexArray& operator=(VertexArray&& other) noexcept;

	/// Registers @p vb as a per-vertex attribute source: attributes advance
	/// once per vertex.
	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	/// Registers @p vb as a per-instance attribute source: attributes advance
	/// once per instance (divisor 1). Draw with Renderer::DrawInstanced.
	void AddInstancedBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	void Bind() const;
	void Unbind() const;

	/// Number of attribute locations consumed so far.
	unsigned int GetAttributeCount() const { return m_NextAttribIndex; }

private:
	void AddBufferInternal(const VertexBuffer& vb,
	                       const VertexBufferLayout& layout,
	                       unsigned int divisor);

	unsigned int m_RendererID      = 0;
	unsigned int m_NextAttribIndex = 0;
};

} // namespace OGLCore
