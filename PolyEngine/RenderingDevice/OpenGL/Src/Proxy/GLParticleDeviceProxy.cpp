#include "Proxy/GLMeshDeviceProxy.hpp"
#include "Math/Vector3f.hpp"
#include "Proxy/GLUtils.hpp"
#include "Proxy/GLParticleDeviceProxy.hpp"
#include "Rendering/Particles/ParticleEmitter.hpp"
#include <algorithm>

using namespace Poly;

//---------------------------------------------------------------
GLParticleDeviceProxy::GLParticleDeviceProxy()
{
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


	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
	
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

void GLParticleDeviceProxy::SetContent(const ParticleEmitter& emitter)
{
	InstancesTransform.Clear();
	InstancesTransform.Resize(16 * emitter.GetInstancesCount());
	
	for (size_t i = 0; i < InstancesTransform.GetSize(); ++i)
	{
		InstancesTransform[i] = 0.0f;
	}

	int transIndx = 0;
	for (const ParticleEmitter::Particle& p : emitter.GetParticlesPool())
	{
		// Scale
		InstancesTransform[transIndx + 0] = p.Scale.X;
		InstancesTransform[transIndx + 5] = p.Scale.Y;
		InstancesTransform[transIndx + 10] = p.Scale.Z;
		InstancesTransform[transIndx + 15] = 1.0f;
		// translation
		InstancesTransform[transIndx + 12] = p.Position.X;
		InstancesTransform[transIndx + 13] = p.Position.Y;
		InstancesTransform[transIndx + 14] = p.Position.Z;
		transIndx += 16;
	}

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * InstancesTransform.GetSize(), InstancesTransform.GetData(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
