#include "ParticleSystem.h"



ParticleSystem::ParticleSystem(GLuint maxParticles)
	:m_activeEmitters(0),
	m_emitterParticles(nullptr)
{
	m_shader.AttachShaderToProgram("particle_vs.glsl", GL_VERTEX_SHADER);
	m_shader.AttachShaderToProgram("particle_gs.glsl", GL_GEOMETRY_SHADER);
	m_shader.LinkShader();

	m_noOfParticles = maxParticles;
	m_particles = new Particle[maxParticles];

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	//CLEANUP
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

	m_shader.Use();
	GLuint shaderId = m_shader.getGLProgramID();

	glUniform1f(glGetUniformLocation(shaderId, "dt"), dt);
	glUniform1f(glGetUniformLocation(shaderId, "ttl"), m_ttl);

	glDrawArrays(GL_POINTS, 0, m_noOfParticles);
	glFlush();
}

GLuint ParticleSystem::getVAO()
{
	return m_VAO;
}
