#include "GLMeshDeviceProxy.hpp"
#include "Vector3f.hpp"
#include "GLUtils.hpp"
#include "GLParticleDeviceProxy.hpp"
#include "ParticleEmitter.hpp"
#include <algorithm>

using namespace Poly;

//---------------------------------------------------------------
GLParticleDeviceProxy::GLParticleDeviceProxy()
{
	gConsole.LogInfo("GLParticleDeviceProxy::GLParticleDeviceProxy");
	
	// quad with uv mapping
	static const float vertices[] = {
		// x,y,z, u,v
		-1.f,  1.f,  0.f,  0.f,  1.f,
		1.f, -1.f,  0.f,  1.f,  0.f,
		1.f,  1.f,  0.f,  1.f,  1.f,

		-1.f,  1.f,  0.f,  0.f,  1.f,
		-1.f, -1.f,  0.f,  0.f,  0.f,
		1.f, -1.f,  0.f,  1.f,  0.f
	};

	glGenVertexArrays(1, &VAO);
	ASSERTE(VAO > 0, "PostprocessQuad VAO creation failed!");
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	ASSERTE(VBO > 0, "PostprocessQuad VBO creation failed!");
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(float)));

	// gConsole.LogInfo("InstancedMeshRenderingPass::Ctor sizeof(Matrix): {}, sizeof(GLfloat): {}", sizeof(Matrix), sizeof(GLfloat));

	instancesTransform.Resize(16 * instancesLen);
	
	// fill array with zeros
	for (int i = 0; i < instancesTransform.GetSize(); ++i)
	{
		instancesTransform[i] = 0.0f;
	}
	
	int index = 0;
	for (int i = 0; i < instancesLen; ++i)
	{
		// identity
		instancesTransform[index + 0] = 1.0f;
		instancesTransform[index + 5] = 1.0f;
		instancesTransform[index + 10] = 1.0f;
		instancesTransform[index + 15] = 1.0f;
		// translation
		instancesTransform[index + 12] = 5.0f * Random(-1.0, 1.0);
		instancesTransform[index + 13] = 5.0f * Random(-1.0, 1.0);
		instancesTransform[index + 14] = 5.0f * Random(-1.0, 1.0);
		index += 16;
	}

	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * instancesTransform.GetSize(), instancesTransform.GetData(), GL_STATIC_DRAW);
	// glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// http://sol.gfxile.net/instancing.html
	// int pos = glGetAttribLocation((GLint)GetProgram().GetProgramHandle(), "aOffset");
	int pos = 3;
	int pos1 = pos + 0;
	int pos2 = pos + 1;
	int pos3 = pos + 2;
	int pos4 = pos + 3;
	glEnableVertexAttribArray(pos1);
	glEnableVertexAttribArray(pos2);
	glEnableVertexAttribArray(pos3);
	glEnableVertexAttribArray(pos4);
	// glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glVertexAttribPointer(pos1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
	glVertexAttribPointer(pos2, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(pos3, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
	glVertexAttribPointer(pos4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));
	glVertexAttribDivisor(pos1, 1);
	glVertexAttribDivisor(pos2, 1);
	glVertexAttribDivisor(pos3, 1);
	glVertexAttribDivisor(pos4, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	CHECK_GL_ERR();
}

//---------------------------------------------------------------
GLParticleDeviceProxy::~GLParticleDeviceProxy()
{
	if (VBO)
		glDeleteBuffers(1, &VBO);
	if (VAO)
		glDeleteVertexArrays(1, &VAO);
}

void GLParticleDeviceProxy::SetContent(const ParticleEmitter& particles)
{
	gConsole.LogInfo("GLParticleDeviceProxy::SetContent new: {}",
		// instancesTransformBuffer.GetSize() / 16,
		particles.GetInstances().GetSize() / 16
	);
	
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particles.GetInstances().GetSize(), particles.GetInstances().GetData(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

float GLParticleDeviceProxy::Random() const
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

float GLParticleDeviceProxy::Random(float min, float max) const
{
	float rnd = Random();
	return Lerp(min, max, rnd);
}