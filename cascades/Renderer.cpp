#include "Renderer.h"



Renderer::Renderer()
	:m_debug(true),
	m_densityThreshold(4.f),
	m_transformFeedbackSwitch(true),
	m_saveTransformFeedback(true),
	m_shadowMode(0),
	m_drawMode(GL_PATCHES)
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
			m_transformFeedbackSwitch = true;
			break;
		case GLFW_KEY_C:
		{
			m_shaderManager.clearShader(0);
			m_shaderManager.attachShaderToProgram(0, "terrain_vs.glsl", GL_VERTEX_SHADER);
			m_shaderManager.attachShaderToProgram(0, "terrain_gs.glsl", GL_GEOMETRY_SHADER);
			const GLchar* feedbackVaryings[] = { "feedbackBlock.position", "feedbackBlock.normals" };
			glTransformFeedbackVaryings(m_shaderManager.getGLIdById(0), 2, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
			m_shaderManager.attachShaderToProgram(0, "terrain_fs.glsl", GL_FRAGMENT_SHADER);
			m_shaderManager.LinkShader(0);

			m_particleSystem->compileShader();

			m_shaderManager.clearShader(1);
			/*m_shaderManager.attachShaderToProgram(1, "simple_vs.glsl", GL_VERTEX_SHADER);
			m_shaderManager.attachShaderToProgram(1, "simple_tes.glsl", GL_TESS_EVALUATION_SHADER);
			m_shaderManager.attachShaderToProgram(1, "simple_fs.glsl", GL_FRAGMENT_SHADER);*/
			m_shaderManager.attachShaderToProgram(1, "tessellation_vs.glsl", GL_VERTEX_SHADER);
			m_shaderManager.attachShaderToProgram(1, "tessellation_tcs.glsl", GL_TESS_CONTROL_SHADER);
			m_shaderManager.attachShaderToProgram(1, "tessellation_tes.glsl", GL_TESS_EVALUATION_SHADER);
			m_shaderManager.attachShaderToProgram(1, "tessellation_fs.glsl", GL_FRAGMENT_SHADER);
			m_shaderManager.LinkShader(1);
		}
			break;
		case GLFW_KEY_R:
			m_transformFeedbackSwitch = true;
			break;
		case GLFW_KEY_L:
			m_light.position = m_camera.position;
			break;
		case GLFW_KEY_P:
			m_particlesOn = !m_particlesOn;
			break;
		case GLFW_KEY_1:
			m_shadowMode = 0;
			break;
		case GLFW_KEY_2:
			m_shadowMode = 1;
			break;
		case GLFW_KEY_3:
			m_shadowMode = 2;
			break;
		case GLFW_KEY_M:
			if (m_drawMode == GL_TRIANGLES)
				m_drawMode = GL_LINES;
			else
				m_drawMode = GL_TRIANGLES;
			break;
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			break;
		}
	}

	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_Q:
		case GLFW_KEY_E:
		case GLFW_KEY_T:
		case GLFW_KEY_R:
			m_saveTransformFeedback = true;
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

void Renderer::mouse_button_callback(int button, int action)
{
	if (action == GLFW_PRESS)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "click!\n";

			std::cout << "Camera Position: " << m_camera.position.x << "|" << m_camera.position.y << "|" << m_camera.position.z << std::endl;
			std::cout << m_camera.rotation.y << "|" << m_camera.rotation.y << "|" << m_camera.rotation.z << std::endl;
			glm::vec3 rayRay = glm::vec3(0, 0, -1.f) * m_camera.rotation;
			//addLine(m_camera.position, rayRay);

			GLfloat* data = m_terrainCreator->feedbackDataPtr();
			GLuint dataSize = m_terrainCreator->getVAOSize();

			int closestT = -1;
			float closest = HUGE_VALF;
			glm::vec3 triangle[3];
			for (int i = 0; i < dataSize; i += 18)
			{
				for (int j = 0; j < 3; ++j)
				{
					triangle[j].x = data[(j * 6) + i];
					triangle[j].y = data[(j * 6) + i + 1];
					triangle[j].z = data[(j * 6) + i + 2];
				}
				
				float hit = triangleHit(triangle, m_camera.position, rayRay, 100.f);
				if (hit > 0.f)
				{
					if (hit < closest)
					{
						closest = hit;
						closestT = i;
					}
				}
			}

			if (closestT > 0)
			{
				//addHit(m_camera.position + (rayRay * closest), .1f);
				//addLine(m_camera.position, rayRay, closest);
				std::cout << "HIT@" << closest << "Index: " << closestT << std::endl;
				glm::vec3 pos = glm::vec3(data[closestT], data[closestT + 1], data[closestT + 2]);
				glm::vec3 normal = glm::vec3(data[closestT + 3], data[closestT + 4], data[closestT + 5]);
				m_particleSystem->addEmitter(pos, normal);
			}
			else
				std::cout << "MISS\n";
			break;
		}
	}
}

void Renderer::Run()
{
	float lastframe = static_cast<float>(glfwGetTime());
	m_dt = 0.014f;

	glGenTextures(1, &m_vertTable);
	glBindTexture(GL_TEXTURE_2D, m_vertTable);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, 16, 256, 0, GL_RED_INTEGER, GL_INT, vertTable);
	glHandleError(__FILE__, __LINE__);
	glBindTexture(GL_TEXTURE_2D, 0);

	/*glGenTextures(1, &m_edgeTable);
	glBindTexture(GL_TEXTURE_1D, m_edgeTable);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R32I, 256, 0, GL_RED_INTEGER, GL_INT, edgeTable);
	glHandleError("post edgeTable");
	glBindTexture(GL_TEXTURE_1D, 0);*/

	// Load and create a texture 
	// ====================
	// Texture 1
	// ====================
	glGenTextures(1, &m_diffuseTexture);
	glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps
	int width, height;
	unsigned char* image = SOIL_load_image("soil.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	i_generateNewFrameBuffer();
	i_initShadow();

	m_particlesOn = true;

	//transform feedback
	m_terrainCreator->initFeedback();

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
		{
			m_densityThreshold += (1.0f * m_dt);
			m_transformFeedbackSwitch = true;
		}
		else if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS)
		{
			m_densityThreshold -= (1.0f * m_dt);
			m_transformFeedbackSwitch = true;
		}

		glm::clamp(m_densityThreshold, 0.f, 100.f);

		//glm::mat4 translate = glm::mat4(1.0f);
		m_view = glm::translate(m_view, -m_camera.position);
		m_view = glm::mat4_cast(m_camera.rotation) * m_view;
		//m_light.position = m_camera.position;

		//keyboard events
		glfwPollEvents();

		/*if (m_lightCam)
			m_light.position = m_camera.position;*/

		//color render
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
		glClearColor(0.69f, 0.69f, 0.69f, 1.0f);
		//glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(m_particlesOn)
			m_particleSystem->update(m_dt);

		GLuint query;
		glGenQueries(1, &query);

		glCullFace(GL_BACK);

		if (m_transformFeedbackSwitch)
		{
			m_shaderManager.UseShader(0);
			if (m_saveTransformFeedback)
			{
				glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
				glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_terrainCreator->getFeedbackBuffer());
				glBeginTransformFeedback(GL_TRIANGLES);
				glHandleError("post begin tranform feedback");
			}

			//i_renderScene(m_scene.data(), m_scene.size());
			i_renderArray(m_terrainCreator->getCubeBuffer(), m_terrainCreator->getNumberOfVertices(true) * 3, GL_POINTS, 0);

			if (m_saveTransformFeedback)
			{
				glEndTransformFeedback();
				glHandleError("post end tranform feedback");
				glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
			}
		}
		else
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			m_shaderManager.UseShader(2);
			glViewport(0, 0, m_shadow.size, m_shadow.size);
			glBindFramebuffer(GL_FRAMEBUFFER, m_shadow.depthFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			i_renderArray(m_terrainCreator->getFeedbackVAO(), m_terrainCreator->getVAOSize(), GL_TRIANGLES, 2);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, m_size.x, m_size.y); //reset viewport
			glDisable(GL_CULL_FACE);

			glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
			glClearColor(0.69f, 0.69f, 0.69f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_shaderManager.UseShader(1);
			//glPatchParameteri(GL_PATCH_VERTICES, 3);
			/*float tessLevel[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, tessLevel);
			glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, tessLevel);*/
			glHandleError(__FUNCTION__, __LINE__);
			i_renderArray(m_terrainCreator->getFeedbackVAO(), m_terrainCreator->getVAOSize(), m_drawMode, 1);
		}

		if (m_particlesOn)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			m_particleSystem->draw(m_projection, m_view);
			glDisable(GL_BLEND);
		}

		//render debug scene
		/*if (m_debug)
			i_renderScene(m_debugScene, m_view);
*/
		//if(m_drawAnimation && false)
		//	drawAnimation(m_view);
	
		// 2. Now blit multisampled buffer(s) to default framebuffers
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, m_size.x, m_size.y, 0, 0, m_size.x, m_size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//swap buffers
		glfwSwapBuffers(m_window);
		glHandleError("post swap buffers");

		if (m_transformFeedbackSwitch)
		{
			m_transformFeedbackSwitch = false;

			if (m_saveTransformFeedback)
			{
				GLuint primitives;
				glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitives);
				glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_terrainCreator->getFeedbackSize(), m_terrainCreator->feedbackDataPtr());
				m_terrainCreator->smoothFeedbackData(primitives);
				m_terrainCreator->feedbackToVAO(primitives);
				//m_transformFeedbackSwitch = false;
				std::cout << primitives << " primitives captured\n";
				glHandleError("post tranform feedback read");
				m_saveTransformFeedback = false;
			}
		}

		glDeleteQueries(1, &query);
	}
}

GLFWwindow * Renderer::createWindow(int width, int height)
{
	m_size = glm::vec2(width, height);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
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
	glHandleError(__FILE__, __LINE__);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		std::cin.ignore();
		exit(1);
	}

	//glfwGetFramebufferSize(m_window, &m_size.x, &m_size.y);

	glViewport(0, 0, m_size.x, m_size.y);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSwapInterval(1);

	//i_generateNewFrameBuffer();
	glHandleError(__FILE__, __LINE__);

	m_particleSystem = new ParticleSystem();

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
	glHandleError(__FILE__, __LINE__);

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
	//m_shaderManager.UseShader(scene[0].shader);
	//glHandleError(__FILE__, __LINE__);

	GLuint shaderId = m_shaderManager.getGLIdById(scene[0].shader);
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "view"), 1, GL_FALSE, glm::value_ptr(m_view));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_vertTable);
	glUniform1i(glGetUniformLocation(shaderId, "vertTable"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);
	glUniform1i(glGetUniformLocation(shaderId, "diffuseTexture"), 1);

	/*glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_1D, m_edgeTable);
	glUniform1i(glGetUniformLocation(shaderId, "edgeTable"), 2);*/
	//glUniform1iv(glGetUniformLocation(shaderId, "edgeTable"), 256, edgeTable);

	glUniform1f(glGetUniformLocation(shaderId, "densityThreshold"), m_densityThreshold);

	glUniform3f(glGetUniformLocation(shaderId, "lightPos"), m_light.position.x, m_light.position.y, m_light.position.z);
	//glUniform3f(glGetUniformLocation(shaderId, "lightPos"), 0, 0, 0);
	glUniform3f(glGetUniformLocation(shaderId, "cameraPos"), m_camera.position.x, m_camera.position.y, m_camera.position.z);

	for (int i = 0; i < size; ++i)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, m_terrainTexture);
		glUniform1i(glGetUniformLocation(shaderId, "densityMap"), 1);

		glBindVertexArray(scene[i].VAO);
		glDrawArrays(GL_POINTS, 0, scene[i].iCount);
		glBindVertexArray(0);
	}

	glHandleError(__FILE__, __LINE__);
}

void Renderer::i_renderArray(GLuint VAO, GLuint arraySize, int glDrawMode, size_t shaderManagerId)
{
	GLuint shaderId = m_shaderManager.getGLIdById(shaderManagerId);

	if (shaderManagerId == 0) //feedback Shader
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_vertTable);
		glUniform1i(glGetUniformLocation(shaderId, "vertTable"), 0);

		glUniform1f(glGetUniformLocation(shaderId, "densityThreshold"), m_densityThreshold);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, m_terrainTexture);
		glUniform1i(glGetUniformLocation(shaderId, "densityMap"), 1);

		glHandleError(__FUNCTION__, __LINE__);
	}

	GLfloat near_plane = 1.0f, far_plane = 100.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(glm::vec3(m_light.position.x, m_light.position.y, m_light.position.z),
		glm::vec3(0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightMatrix = lightProjection * lightView;
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "lightMatrix"), 1, GL_FALSE, glm::value_ptr(lightMatrix));

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_shadow.depthTex);
	glUniform1i(glGetUniformLocation(shaderId, "depthMap"), 2);

	glUniformMatrix4fv(glGetUniformLocation(shaderId, "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "view"), 1, GL_FALSE, glm::value_ptr(m_view));

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);
	glUniform1i(glGetUniformLocation(shaderId, "diffuseTexture"), 1);

	/*glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_1D, m_edgeTable);
	glUniform1i(glGetUniformLocation(shaderId, "edgeTable"), 2);*/
	//glUniform1iv(glGetUniformLocation(shaderId, "edgeTable"), 256, edgeTable);

	glUniform3f(glGetUniformLocation(shaderId, "lightPos"), m_light.position.x, m_light.position.y, m_light.position.z);
	//glUniform3fv(glGetUniformLocation(shaderId, "lightPos"), 3, glm::value_ptr(m_light.position));
	//glUniform3f(glGetUniformLocation(shaderId, "lightPos"), 0, 0, 0);
	glUniform3f(glGetUniformLocation(shaderId, "cameraPos"), m_camera.position.x, m_camera.position.y, m_camera.position.z);

	glUniform1i(glGetUniformLocation(shaderId, "shadowMode"), m_shadowMode);

	glHandleError(__FUNCTION__, __LINE__);

	glBindVertexArray(VAO);
	glDrawArrays(glDrawMode, 0, arraySize);
	glBindVertexArray(0);

	glHandleError(__FUNCTION__, __LINE__);
}

void Renderer::i_generateNewFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glDeleteFramebuffers(1, &m_framebuffer);
	glDeleteRenderbuffers(1, &m_rbo);

	//FBO
	glGenFramebuffers(1, &m_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	// Create a multisampled color attachment texture
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_size.x, m_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	// Create a renderbuffer object for depth and stencil attachments
	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_size.x, m_size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::setPerspective(float fovy, float aspect, float near, float far)
{
	m_projection = glm::perspective(fovy, aspect, near, far);
}

void Renderer::i_initShadow()
{
	glGenFramebuffers(1, &m_shadow.depthFBO);

	m_shadow.size = 2048;

	glGenTextures(1, &m_shadow.depthTex);
	glBindTexture(GL_TEXTURE_2D, m_shadow.depthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, m_shadow.size, m_shadow.size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glHandleError(__FUNCTION__, __LINE__);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_shadow.depthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadow.depthTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glHandleError(__FUNCTION__, __LINE__);
}

//statics
float Renderer::triangleHit(glm::vec3* triangle, glm::vec3 rOrigin, glm::vec3 rDirection, float length)
{
	glm::vec3 e1, e2, h, s, q;
	float a, f, u, v, t;

	e1 = triangle[1] - triangle[0];
	e2 = triangle[2] - triangle[0];

	h = glm::cross(rDirection, e2);
	a = glm::dot(e1, h);

	if (a > -0.00001 && a < 0.00001)
		return(false);

	f = 1 / a;
	s = rOrigin - triangle[0];
	u = f * (glm::dot(s, h));

	if (u < 0.0 || u > 1.0)
		return(false);

	q = glm::cross(s, e1);
	v = f * glm::dot(rDirection, q);

	if (v < 0.0 || u + v > 1.0)
		return(false);

	// at this stage we can compute t to find out where
	// the intersection point is on the line
	t = f * glm::dot(e2, q);

	if (t > 0.f && t < length)
	{
		return t;
	}

	return -1.f;
}
