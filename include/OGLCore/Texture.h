#pragma once

#include <string>

namespace OGLCore {

/// Owns a GL 2D texture loaded from an image file via stb_image. Move-only.
class Texture
{
public:
	explicit Texture(const std::string& path);
	~Texture();

	Texture(const Texture&)            = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&& other) noexcept;
	Texture& operator=(Texture&& other) noexcept;

	/// Binds to texture unit @p slot; pass the same slot to the sampler uniform.
	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	int GetWidth()  const { return m_Width; }
	int GetHeight() const { return m_Height; }
	int GetBPP()    const { return m_BPP; }

	/// False if the image could not be loaded.
	bool IsValid() const { return m_RendererID != 0; }

private:
	unsigned int m_RendererID = 0;
	std::string  m_FilePath;
	int          m_Width  = 0;
	int          m_Height = 0;
	int          m_BPP    = 0;
};

} // namespace OGLCore
