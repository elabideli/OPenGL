// OGLCore sandbox: the smallest program that exercises the public API.
//
// It doubles as the library's smoke test - if this draws, the encapsulation
// is intact, because this file has no access to glad whatsoever.

#include <GLFW/glfw3.h>

#include <OGLCore/Core.h>
#include <OGLCore/IndexBuffer.h>
#include <OGLCore/Renderer.h>
#include <OGLCore/Shader.h>
#include <OGLCore/Texture.h>
#include <OGLCore/VertexArray.h>
#include <OGLCore/VertexBuffer.h>
#include <OGLCore/VertexBufferLayout.h>

#include <iostream>

namespace {

const OGLCore::Renderer g_renderer;

void OnFramebufferResize(GLFWwindow*, int width, int height)
{
	g_renderer.SetViewport(0, 0, width, height);
}

} // namespace

int main()
{
	if (!glfwInit()) {
		std::cerr << "Failed to initialise GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(640, 480, "OGLCore Sandbox", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create a GL 3.3 core window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetFramebufferSizeCallback(window, OnFramebufferResize);

	// glfwGetProcAddress matches OGLCore::GLProcLoader exactly, so it passes
	// through with no cast at all.
	if (!OGLCore::Init(glfwGetProcAddress)) {
		glfwTerminate();
		return -1;
	}

	std::cout << "OpenGL " << OGLCore::VersionString()
	          << " on " << OGLCore::RendererString() << std::endl;

	{
		const float positions[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, // 0
			 0.5f, -0.5f, 1.0f, 0.0f, // 1
			 0.5f,  0.5f, 1.0f, 1.0f, // 2
			-0.5f,  0.5f, 0.0f, 1.0f  // 3
		};

		const unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		g_renderer.SetBlending(true);
		g_renderer.SetClearColor(0.10f, 0.10f, 0.12f, 1.0f);

		OGLCore::VertexArray  va;
		OGLCore::VertexBuffer vb(positions, sizeof(positions));

		OGLCore::VertexBufferLayout layout;
		layout.Push<float>(2); // position
		layout.Push<float>(2); // texcoord
		va.AddBuffer(vb, layout);

		OGLCore::IndexBuffer ib(indices, 6);

		OGLCore::Shader shader("Ressources/Shaders/Basic.shader");
		if (!shader.IsValid()) {
			std::cerr << "Shader failed to build - is the working directory correct?" << std::endl;
			glfwTerminate();
			return -1;
		}

		OGLCore::Texture texture("Ressources/Textures/bg.jpeg");
		texture.Bind(0);
		shader.Bind();
		shader.SetUniform1i("u_Texture", 0);

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		while (!glfwWindowShouldClose(window)) {
			g_renderer.Clear();
			g_renderer.Draw(va, ib, shader);

			glfwSwapBuffers(window);
			glfwPollEvents();

			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	} // GL objects destruct here, while the context is still current

	glfwTerminate();
	return 0;
}
