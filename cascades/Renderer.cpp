#include "Renderer.h"



Renderer::Renderer()
	:m_debug(true)
{
	glfwInit();
}


Renderer::~Renderer()
{
}

void Renderer::key_callback(int key, int action)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			break;
		}
	}
}

void Renderer::mouse_callback(double xpos, double ypos)
{
	const GLfloat sens = 0.3f;
	glm::vec2 diff = glm::vec2(xpos, ypos) - m_mouse;
	glm::quat yaw = glm::quat(glm::vec3(diff.y * dt * sens, 0.0f, 0.0f));
	glm::quat pitch = glm::quat(glm::vec3(0.0f, diff.x * dt * sens, 0.0f));
	m_camera.rotation = glm::normalize(yaw * m_camera.rotation * pitch);
	m_mouse.x = xpos;
	m_mouse.y = ypos;
}

void Renderer::Run()
{
	float lastframe = static_cast<float>(glfwGetTime());
	dt = 0.014f;

	while (!glfwWindowShouldClose(m_window))
	{
		glGetError();
		glEnable(GL_DEPTH_TEST);
		glm::mat4 m_view;

		dt = static_cast<float>(glfwGetTime()) - lastframe;
		lastframe = static_cast<float>(glfwGetTime());
		fps = 1.f / dt;

		float pi = 3.1415f;
		float moveSpeed = 0.1f;
		glm::vec3 forwardVec(0, 0, moveSpeed),
			sideVec(moveSpeed, 0, 0);

		if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
		{
			m_camera.position -= forwardVec * m_camera.rotation;
		}
		else if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
		{
			m_camera.position += forwardVec * m_camera.rotation;
		}

		if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
			m_camera.position -= sideVec * m_camera.rotation;
		else if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
			m_camera.position += sideVec * m_camera.rotation;

		glm::mat4 translate = glm::mat4(1.0f);
		m_view = glm::translate(m_view, -m_camera.position);
		m_view = glm::mat4_cast(m_camera.rotation) * m_view;

		//keyboard events
		glfwPollEvents();

		/*if (m_lightCam)
			m_light.position = m_camera.position;*/

		//color render
		//glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
		glClearColor(0.69f, 0.69f, 0.69f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//i_renderScene(m_scene, m_view);

		//render debug scene
		/*if (m_debug)
			i_renderScene(m_debugScene, m_view);
*/
		//if(m_drawAnimation && false)
		//	drawAnimation(m_view);

		// 2. Now blit multisampled buffer(s) to default framebuffers
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebuffer);
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		//glBlitFramebuffer(0, 0, m_size.x, m_size.y, 0, 0, m_size.x, m_size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glHandleError("post swap buffers");

		//swap buffers
		glfwSwapBuffers(m_window);
	}
}

GLFWwindow * Renderer::createWindow(int width, int height)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	m_window = glfwCreateWindow(width, height, "SPG Cascades", nullptr, nullptr);
	if (m_window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(m_window);
	glGetError();

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		std::cin.ignore();
		exit(1);
	}

	//glfwGetFramebufferSize(m_window, &m_size.x, &m_size.y);

	glViewport(0, 0, m_size.x, m_size.y);

	glEnable(GL_DEPTH_TEST);

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//i_generateNewFrameBuffer();

	return m_window;
}

void glHandleError(const char* info)
{
	GLenum err = glGetError();

	if (err != GL_NO_ERROR)
	{
		char* fill = "";
		if (strlen(info) > 0)
		{
			std::cout << info << ":\n";
			fill = "\t";
		}

		while (err != GL_NO_ERROR)
		{
			std::cout << fill;
			switch (err)
			{
			case GL_INVALID_ENUM: std::cout << fill << "GL_INVALID_ENUM\n";
				break;
			case GL_INVALID_VALUE: std::cout << "GL_INVALID_VALUE\n";
				break;
			case GL_INVALID_OPERATION: std::cout << "GL_INVALID_OPERATION\n";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION\n";
				break;
			case GL_OUT_OF_MEMORY: std::cout << "GL_OUT_OF_MEMORY\n";
				break;
			case GL_STACK_UNDERFLOW: std::cout << "GL_STACK_UNDERFLOW\n";
				break;
			case GL_STACK_OVERFLOW: std::cout << "GL_STACK_OVERFLOW\n";
				break;
			}
			err = glGetError();
		}
	}
}
