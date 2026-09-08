#include "OGLCore/Texture.h"

#include "GLDebug.h"
#include "stb_image/stb_image.h"

#include <iostream>
#include <utility>

namespace OGLCore {

Texture::Texture(const std::string& path)
	: m_FilePath(path)
{
	stbi_set_flip_vertically_on_load(1);

	// Local, not a member: the pixels are only needed until they reach the GPU.
	unsigned char* pixels = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);
	if (!pixels) {
		std::cerr << "[OGLCore] Texture: failed to load '" << path << "'" << std::endl;
		return;
	}

	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0,
	                    GL_RGBA, GL_UNSIGNED_BYTE, pixels));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	stbi_image_free(pixels);
}

Texture::~Texture()
{
	if (m_RendererID)
		GLCall(glDeleteTextures(1, &m_RendererID));
}

Texture::Texture(Texture&& other) noexcept
	: m_RendererID(other.m_RendererID)
	, m_FilePath(std::move(other.m_FilePath))
	, m_Width(other.m_Width)
	, m_Height(other.m_Height)
	, m_BPP(other.m_BPP)
{
	other.m_RendererID = 0;
	other.m_Width = other.m_Height = other.m_BPP = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
	if (this != &other) {
		if (m_RendererID)
			GLCall(glDeleteTextures(1, &m_RendererID));
		m_RendererID = other.m_RendererID;
		m_FilePath   = std::move(other.m_FilePath);
		m_Width      = other.m_Width;
		m_Height     = other.m_Height;
		m_BPP        = other.m_BPP;

		other.m_RendererID = 0;
		other.m_Width = other.m_Height = other.m_BPP = 0;
	}
	return *this;
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

} // namespace OGLCore
