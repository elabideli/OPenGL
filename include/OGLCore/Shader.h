#pragma once

#include <string>
#include <unordered_map>

namespace OGLCore {

/// Vertex and fragment stages parsed out of a single .shader file.
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

/// Owns a linked GL program. Move-only.
///
/// Source is a single file containing both stages, delimited by
/// `#shader vertex` and `#shader fragment` lines.
class Shader
{
public:
	explicit Shader(const std::string& filepath);
	~Shader();

	Shader(const Shader&)            = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;

	void Bind() const;
	void Unbind() const;

	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform2f(const std::string& name, float v0, float v1);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

	/// False if the file was missing or a stage failed to compile or link.
	bool IsValid() const { return m_RendererID != 0; }

	const std::string& GetFilePath() const { return m_FilePath; }

private:
	static ShaderProgramSource ParseShader(const std::string& filepath);
	static unsigned int CompileShader(unsigned int type, const std::string& source);
	static unsigned int CreateShader(const std::string& vertexShader,
	                                 const std::string& fragmentShader);

	int GetUniformLocation(const std::string& name);

	std::string                          m_FilePath;
	unsigned int                         m_RendererID = 0;
	std::unordered_map<std::string, int> m_UniformLocationCache;
};

} // namespace OGLCore
