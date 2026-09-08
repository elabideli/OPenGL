#pragma once

// No GL header here on purpose: glad is an implementation detail of the
// library and stays out of the public interface. Draw targets are taken by
// const reference, so forward declarations are sufficient.

namespace OGLCore {

class VertexArray;
class IndexBuffer;
class Shader;

/// Issues draw and framebuffer commands against the current GL context.
///
/// Holds no state of its own and owns no GL objects, so it is cheap to copy
/// and safe to construct wherever it is needed.
class Renderer
{
public:
	/// Clears the colour buffer to the colour set by SetClearColor.
	void Clear() const;

	/// @param r,g,b,a  clear colour, each in [0, 1]
	void SetClearColor(float r, float g, float b, float a) const;

	/// Resizes the drawing region. Call on framebuffer resize.
	void SetViewport(int x, int y, int width, int height) const;

	/// Enables or disables alpha blending (src-alpha / one-minus-src-alpha).
	void SetBlending(bool enabled) const;

	/// Draws the indexed geometry in @p va once.
	void Draw(const VertexArray& va,
	          const IndexBuffer& ib,
	          const Shader& shader) const;

	/// Draws @p instanceCount copies of the indexed geometry in @p va.
	///
	/// Per-instance attributes come from a buffer registered through
	/// VertexArray::AddInstancedBuffer; without one, every instance renders
	/// identically at the same position.
	void DrawInstanced(const VertexArray& va,
	                   const IndexBuffer& ib,
	                   const Shader& shader,
	                   unsigned int instanceCount) const;
};

} // namespace OGLCore
