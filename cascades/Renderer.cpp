#include "Renderer.h"



Renderer::Renderer()
	:m_debug(true),
	m_densityThreshold(4.f)
{
	m_camera.nearPlane = 0.1f;
	m_camera.farPlane = 1000.0f;
	m_camera.position = glm::vec3(-14, 10, 28);
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
		case GLFW_KEY_H:
			std::cout << "cameraPos: " << m_camera.position.x << "|" << m_camera.position.y << "|" << m_camera.position.z << "\n";
			std::cout << "cameraRot: " << m_camera.rotation.x << "|" << m_camera.rotation.y << "|" << m_camera.rotation.z << "\n";
			std::cout << "densityThreshold = " << m_densityThreshold << std::endl;
			break;
		case GLFW_KEY_T:
			m_terrainCreator->createTerrain();
			glBindTexture(GL_TEXTURE_3D, m_terrainTexture);
			glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, m_terrainCreator->getDimensions().x, m_terrainCreator->getDimensions().y, m_terrainCreator->getDimensions().z, 0, GL_RED, GL_FLOAT, m_terrainCreator->getTerrainData());
			glBindTexture(GL_TEXTURE_3D, 0);
			break;
		case GLFW_KEY_C:
			m_shaderManager.clearShader(0);
			m_shaderManager.attachShaderToProgram(0, "simple_vs.hlsl", GL_VERTEX_SHADER);
			m_shaderManager.attachShaderToProgram(0, "simple_fs.hlsl", GL_FRAGMENT_SHADER);
			m_shaderManager.attachShaderToProgram(0, "simple_gs.hlsl", GL_GEOMETRY_SHADER);
			break;
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
	glm::quat yaw = glm::quat(glm::vec3(diff.y * m_dt * sens, 0.0f, 0.0f));
	glm::quat pitch = glm::quat(glm::vec3(0.0f, diff.x * m_dt * sens, 0.0f));
	m_camera.rotation = glm::normalize(yaw * m_camera.rotation * pitch);
	m_mouse.x = xpos;
	m_mouse.y = ypos;
}

void Renderer::Run()
{
	float lastframe = static_cast<float>(glfwGetTime());
	m_dt = 0.014f;

	glHandleError("pre edgeTable");

	glGenTextures(1, &m_vertTable);
	glBindTexture(GL_TEXTURE_2D, m_vertTable);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, 16, 256, 0, GL_RED_INTEGER, GL_INT, vertTable);
	glHandleError("post vertable");
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &m_edgeTable);
	glBindTexture(GL_TEXTURE_1D, m_edgeTable);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R32I, 256, 0, GL_RED_INTEGER, GL_INT, edgeTable);
	glHandleError("post edgeTable");
	glBindTexture(GL_TEXTURE_1D, 0);

	mouse_callback(m_size.x/2, m_size.y/2);

	while (!glfwWindowShouldClose(m_window))
	{
		glGetError();
		glEnable(GL_DEPTH_TEST);

		m_dt = static_cast<float>(glfwGetTime()) - lastframe;
		lastframe = static_cast<float>(glfwGetTime());
		float fps = 1.f / m_dt;

		m_view = glm::mat4();
		float pi = 3.1415f;
		float moveSpeed = 5.f;

		if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			moveSpeed *= 0.1f;

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

		if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS)
			m_densityThreshold += (1.0f * m_dt);
		else if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS)
			m_densityThreshold -= (1.0f * m_dt);

		glm::clamp(m_densityThreshold, 0.f, 100.f);

		//glm::mat4 translate = glm::mat4(1.0f);
		m_view = glm::translate(m_view, -m_camera.position);
		m_view = glm::mat4_cast(m_camera.rotation) * m_view;

		//keyboard events
		glfwPollEvents();

		/*if (m_lightCam)
			m_light.position = m_camera.position;*/

		//color render
		//glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
		//glClearColor(0.69f, 0.69f, 0.69f, 1.0f);
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		i_renderScene(m_scene.data(), m_scene.size());

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
	
		//swap buffers
		glfwSwapBuffers(m_window);

		glHandleError("post swap buffers");
	}
}

GLFWwindow * Renderer::createWindow(int width, int height)
{
	m_size = glm::vec2(width, height);

	glfwInit();
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

	glewExperimental = GL_TRUE;
	glewInit();
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
	glfwSwapInterval(1);

	//i_generateNewFrameBuffer();

	return m_window;
}

size_t Renderer::addObjectToScene(GLfloat * vertices, int vSize, GLuint* vao)
{
	GLuint vbo;

	glGenVertexArrays(1, vao);
	glBindVertexArray(*vao);

	//VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vSize * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	//CLEANUP
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//m_vao.push_back(vao);
	//m_vbo.push_back(vbo);

	Sceneobj tmpObj;
	tmpObj.VAO = *vao;
	tmpObj.iCount = vSize;

	m_scene.push_back(tmpObj);

	return m_scene.size() - 1;
}

Sceneobj * Renderer::getObjectById(size_t id)
{
	if (id >= m_scene.size())
		return nullptr;
	return &(m_scene[id]);
}

void Renderer::setTerrainCreatorPtr(TerrainCreator* tcPtr, GLuint textureId)
{
	m_terrainTexture = textureId;
	m_terrainCreator = tcPtr;
}

ShaderManager * Renderer::getShaderManager()
{
	return &m_shaderManager;
}

void Renderer::i_renderScene(Sceneobj* scene, size_t size)
{
	m_shaderManager.UseShader(scene[0].shader);
	GLuint shaderId = m_shaderManager.getGLIdById(scene[0].shader);
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "view"), 1, GL_FALSE, glm::value_ptr(m_view));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_vertTable);
	glUniform1i(glGetUniformLocation(shaderId, "vertTable"), 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_1D, m_edgeTable);
	glUniform1i(glGetUniformLocation(shaderId, "edgeTable"), 2);

	glUniform1f(glGetUniformLocation(shaderId, "densityThreshold"), m_densityThreshold);

	for (int i = 0; i < size; ++i)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, scene[i].texture);
		glUniform1i(glGetUniformLocation(shaderId, "densityMap"), 1);

		glBindVertexArray(scene[i].VAO);
		glDrawArrays(GL_POINTS, 0, scene[i].iCount);
		glBindVertexArray(0);
	}
}

void Renderer::setPerspective(float fovy, float aspect, float near, float far)
{
	m_projection = glm::perspective(fovy, aspect, near, far);
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
