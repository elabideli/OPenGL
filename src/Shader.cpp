#include "OGLCore/Shader.h"

#include "GLDebug.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

namespace OGLCore {

namespace {

/// The #version directive and any precision qualifier the target needs.
///
/// Shader files carry no version line of their own: desktop GL wants
/// "#version 330 core" and WebGL 2 wants "#version 300 es", and nothing
/// translates between them. Everything else in a shader - layout
/// qualifiers, instanced attributes, the built-in functions - is common to
/// both, so injecting the header lets one shader body serve either target.
const char* VersionHeader(unsigned int type)
{
#if defined(__EMSCRIPTEN__)
	// ES requires an explicit float precision in the fragment stage.
	return (type == GL_FRAGMENT_SHADER)
		? "#version 300 es\nprecision highp float;\n"
		: "#version 300 es\n";
#else
	(void)type;
	return "#version 330 core\n";
#endif
}

} // namespace

Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath)
{
	const ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	if (m_RendererID)
		GLCall(glDeleteProgram(m_RendererID));
}

Shader::Shader(Shader&& other) noexcept
	: m_FilePath(std::move(other.m_FilePath))
	, m_RendererID(other.m_RendererID)
	, m_UniformLocationCache(std::move(other.m_UniformLocationCache))
{
	other.m_RendererID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
	if (this != &other) {
		if (m_RendererID)
			GLCall(glDeleteProgram(m_RendererID));
		m_FilePath             = std::move(other.m_FilePath);
		m_RendererID           = other.m_RendererID;
		m_UniformLocationCache = std::move(other.m_UniformLocationCache);
		other.m_RendererID     = 0;
	}
	return *this;
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);
	if (!stream) {
		std::cerr << "[OGLCore] Shader: cannot open '" << filepath << "'" << std::endl;
		return {};
	}

	enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

	std::string       line;
	std::stringstream ss[2];
	ShaderType        type = ShaderType::NONE;

	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		} else if (line.rfind("#version", 0) == 0) {
			// Dropped rather than passed through: the version is chosen
			// per target in VersionHeader. Tolerated so shader files
			// written for the desktop-only library still work unchanged.
			continue;
		} else if (type != ShaderType::NONE) {
			ss[static_cast<int>(type)] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	if (source.empty())
		return 0;

	const std::string  full = VersionHeader(type) + source;
	const unsigned int id   = glCreateShader(type);
	const char*        src  = full.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	int result = GL_FALSE;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

	if (result == GL_FALSE) {
		int length = 0;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));

		// std::vector rather than alloca: alloca lives in a different header on
		// every platform, and this is not a hot path.
		std::vector<char> message(static_cast<std::size_t>(length > 0 ? length : 1));
		GLCall(glGetShaderInfoLog(id, length, &length, message.data()));

		std::cerr << "[OGLCore] Failed to compile "
		          << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader\n"
		          << message.data() << std::endl;

		GLCall(glDeleteShader(id));
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader,
                                  const std::string& fragmentShader)
{
	const unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	const unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	if (vs == 0 || fs == 0) {
		if (vs) GLCall(glDeleteShader(vs));
		if (fs) GLCall(glDeleteShader(fs));
		return 0;
	}

	const unsigned int program = glCreateProgram();
	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));

	int linked = GL_FALSE;
	GLCall(glGetProgramiv(program, GL_LINK_STATUS, &linked));
	if (linked == GL_FALSE) {
		int length = 0;
		GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
		std::vector<char> message(static_cast<std::size_t>(length > 0 ? length : 1));
		GLCall(glGetProgramInfoLog(program, length, &length, message.data()));
		std::cerr << "[OGLCore] Failed to link program\n" << message.data() << std::endl;

		GLCall(glDeleteProgram(program));
		GLCall(glDeleteShader(vs));
		GLCall(glDeleteShader(fs));
		return 0;
	}

	GLCall(glDetachShader(program, vs));
	GLCall(glDetachShader(program, fs));
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

void Shader::Bind() const   { GLCall(glUseProgram(m_RendererID)); }
void Shader::Unbind() const { GLCall(glUseProgram(0)); }

void Shader::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
	GLCall(glUniform2f(GetUniformLocation(name), v0, v1));
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

int Shader::GetUniformLocation(const std::string& name)
{
	const auto it = m_UniformLocationCache.find(name);
	if (it != m_UniformLocationCache.end())
		return it->second;

	// Declared outside the GLCall: the macro wraps its argument in a
	// do/while block, so a declaration inside would not escape it.
	int location = -1;
	GLCall(location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
		std::cerr << "[OGLCore] Warning: uniform '" << name << "' not found in "
		          << m_FilePath << std::endl;

	m_UniformLocationCache[name] = location;
	return location;
}

} // namespace OGLCore
