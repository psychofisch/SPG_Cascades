#include "ParticleSystem.h"



ParticleSystem::ParticleSystem()
	:ParticleSystem(100)
{
}

ParticleSystem::ParticleSystem(GLuint maxParticles)
	:m_activeEmitters(0),
	m_emitterParticles(nullptr),
	m_maxLifetime(5.f)
{
	m_computeShader.AttachShaderToProgram("particle_compute.glsl", GL_VERTEX_SHADER);
	const GLchar* feedbackVaryings[] = { "feedbackBlock.position", "feedbackBlock.lifetime" };
	glTransformFeedbackVaryings(m_computeShader.getGLProgramID(), 2, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
	//m_shader.AttachShaderToProgram("particle_fs.glsl", GL_FRAGMENT_SHADER);
	m_computeShader.LinkShader();

	m_drawShader.AttachShaderToProgram("particle_vs.glsl", GL_VERTEX_SHADER);
	m_drawShader.AttachShaderToProgram("particle_fs.glsl", GL_FRAGMENT_SHADER);
	m_drawShader.LinkShader();

	m_noOfParticles = maxParticles;
	//m_particles = new Particle[maxParticles];
	m_particles = new GLfloat[maxParticles * 4](); //position + lifetime

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_noOfParticles * 4 * sizeof(GLfloat), m_particles, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	//CLEANUP
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//transform feedback
	glGenBuffers(1, &m_feedbackBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_feedbackBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_noOfParticles * 4 * sizeof(GLfloat), nullptr, GL_STATIC_READ);

	glEnable(GL_PROGRAM_POINT_SIZE);

	glHandleError(__FUNCTION__);
}

ParticleSystem::~ParticleSystem()
{
	glDeleteBuffers(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);

	delete[] m_particles;
	delete[] m_emitterParticles;
}

void ParticleSystem::update(float dt)
{
	/*GLuint activeParticles = 0;

	for (int i = 0; i < m_noOfParticles; ++i)
	{
		if(m_particles[i].lifetime > m_ttl)
	}*/

	/*GLuint query;
	glGenQueries(1, &query);*/

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(GLfloat) * m_noOfParticles, m_particles);

	glEnable(GL_RASTERIZER_DISCARD);

	m_computeShader.Use();
	GLuint shaderId = m_computeShader.getGLProgramID();

	glUniform1f(glGetUniformLocation(shaderId, "dt"), dt);
	glUniform1f(glGetUniformLocation(shaderId, "maxLifetime"), m_maxLifetime);
	glHandleError(__FUNCTION__, __LINE__);

	//glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_feedbackBuffer);
	glBeginTransformFeedback(GL_POINTS);
	glHandleError(__FUNCTION__, __LINE__);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_POINTS, 0, m_noOfParticles);
	glBindVertexArray(0);
	glHandleError(__FUNCTION__, __LINE__);
	glFlush();

	glEndTransformFeedback();
	//glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	glHandleError(__FUNCTION__, __LINE__);

	//GLuint primitives;
	//glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitives);
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 4 * sizeof(GLfloat) * m_noOfParticles, m_particles);

	glHandleError(__FUNCTION__, __LINE__);

	glDisable(GL_RASTERIZER_DISCARD);

	//glDeleteQueries(1, &query);
}

void ParticleSystem::draw(glm::mat4 projection, glm::mat4 view)
{
	m_drawShader.Use();
	GLuint shaderId = m_drawShader.getGLProgramID();
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniform1f(glGetUniformLocation(shaderId, "maxLifetime"), m_maxLifetime);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_POINTS, 0, m_noOfParticles);
	glBindVertexArray(0);

	glHandleError(__FUNCTION__, __LINE__);
}

void ParticleSystem::addEmitter(GLfloat x, GLfloat y, GLfloat z)
{
	m_particles[m_activeEmitters * 4] = x;
	m_particles[m_activeEmitters * 4 + 1] = y;
	m_particles[m_activeEmitters * 4 + 2] = z;
	m_particles[m_activeEmitters * 4 + 3] = 0.f;
	m_activeEmitters++;
}

GLuint ParticleSystem::getVAO()
{
	return m_VAO;
}
