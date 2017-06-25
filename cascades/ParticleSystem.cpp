#include "ParticleSystem.h"



ParticleSystem::ParticleSystem()
	:ParticleSystem(10000, 10)
{
}

ParticleSystem::ParticleSystem(GLuint maxParticles, GLuint maxEmitters)
	:m_activeEmitters(0),
	m_maxLifetime(5.f),
	m_maxParticles(maxParticles),
	m_maxEmitters(maxEmitters)
{
	compileShader();

	m_particles = new GLfloat[m_maxParticles * DataInParticle]{ 0.f };
	m_emitters = new GLfloat[m_maxEmitters * DataInEmitter];

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * SizeOfParticle, m_particles, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, SizeOfParticle, (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, SizeOfParticle, (GLvoid*)(3 * sizeof(GLfloat)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, SizeOfParticle, (GLvoid*)(6 * sizeof(GLfloat)));

	//CLEANUP
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//transform feedback
	glGenBuffers(1, &m_feedbackBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_feedbackBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * SizeOfParticle, nullptr, GL_STATIC_READ);

	glEnable(GL_PROGRAM_POINT_SIZE);

	glGenTextures(1, &m_smokeTexture);
	glBindTexture(GL_TEXTURE_2D, m_smokeTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps
	int width, height;
	unsigned char* image = SOIL_load_image("smoke_alpha2.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glHandleError(__FUNCTION__);
}

ParticleSystem::~ParticleSystem()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_feedbackBuffer);

	delete[] m_particles;
	delete[] m_emitters;
}

void ParticleSystem::update(float dt)
{
	/*GLuint query;
	glGenQueries(1, &query);*/

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, SizeOfParticle * m_maxParticles, m_particles);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glHandleError(__FUNCTION__, __LINE__);

	glEnable(GL_RASTERIZER_DISCARD);

	m_computeShader.Use();
	GLuint shaderId = m_computeShader.getGLProgramID();

	glUniform1f(glGetUniformLocation(shaderId, "dt"), dt);
	glUniform1f(glGetUniformLocation(shaderId, "maxLifetime"), m_maxLifetime);
	glUniform1fv(glGetUniformLocation(shaderId, "emitters"), DataInEmitter * m_maxEmitters, m_emitters);
	glUniform1ui(glGetUniformLocation(shaderId, "activeEmitters"), m_activeEmitters);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_smokeTexture);
	glUniform1i(glGetUniformLocation(shaderId, "smokeTexture"), m_smokeTexture);
	glHandleError(__FUNCTION__, __LINE__);

	//glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_feedbackBuffer);
	glBeginTransformFeedback(GL_POINTS);
	glHandleError(__FUNCTION__, __LINE__);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_POINTS, 0, m_maxParticles);
	glBindVertexArray(0);
	glHandleError(__FUNCTION__, __LINE__);
	glEndTransformFeedback();
	glFlush();

	//glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	glHandleError(__FUNCTION__, __LINE__);

	//GLuint primitives;
	//glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitives);
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, SizeOfParticle * m_maxParticles, m_particles);

	glHandleError(__FUNCTION__, __LINE__);

	glDisable(GL_RASTERIZER_DISCARD);
	//glDeleteQueries(1, &query);
}

void ParticleSystem::draw(glm::mat4 projection, glm::mat4 view)
{
	//glDisable(GL_DEPTH_TEST);

	m_drawShader.Use();
	GLuint shaderId = m_drawShader.getGLProgramID();
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniform1f(glGetUniformLocation(shaderId, "maxLifetime"), m_maxLifetime);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_POINTS, 0, m_maxParticles);
	glBindVertexArray(0);

	//glEnable(GL_DEPTH_TEST);

	glHandleError(__FUNCTION__, __LINE__);
}

void ParticleSystem::addEmitter(glm::vec3 position, glm::vec3 normal)
{
	memcpy(m_emitters + (DataInEmitter * m_activeEmitters), &position, 3 * sizeof(GLfloat));
	memcpy(m_emitters + (DataInEmitter * m_activeEmitters) + 3, &normal, 3 * sizeof(GLfloat));

	/*m_particles[m_activeEmitters * 4] = position.x;
	m_particles[m_activeEmitters * 4 + 1] = position.y;
	m_particles[m_activeEmitters * 4 + 2] = position.z;
	m_particles[m_activeEmitters * 4 + 3] = 0.f;*/
	m_activeEmitters++;

	if (m_activeEmitters >= m_maxEmitters)
		m_activeEmitters = 0;
}

void ParticleSystem::compileShader()
{
	m_computeShader = Shader();
	m_computeShader.AttachShaderToProgram("particle_compute.glsl", GL_VERTEX_SHADER);
	const GLchar* feedbackVaryings[] = { "feedbackBlock.position", "feedbackBlock.velocity", "feedbackBlock.lifetime" };
	glTransformFeedbackVaryings(m_computeShader.getGLProgramID(), 3, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
	m_computeShader.LinkShader();

	m_drawShader = Shader();
	m_drawShader.AttachShaderToProgram("particle_vs.glsl", GL_VERTEX_SHADER);
	m_drawShader.AttachShaderToProgram("particle_gs.glsl", GL_GEOMETRY_SHADER);
	m_drawShader.AttachShaderToProgram("particle_fs.glsl", GL_FRAGMENT_SHADER);
	m_drawShader.LinkShader();
}

GLuint ParticleSystem::getVAO()
{
	return m_VAO;
}
