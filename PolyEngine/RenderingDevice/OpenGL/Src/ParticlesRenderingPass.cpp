#include "ParticlesRenderingPass.hpp"

#include "GLMeshDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"

#include <World.hpp>
#include <TimeSystem.hpp>
#include <CameraComponent.hpp>
#include <ParticleComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <MovementSystem.hpp>

using namespace Poly;

ParticlesRenderingPass::ParticlesRenderingPass()
	: RenderingPassBase("Shaders/instancedVert.shader", "Shaders/instancedFrag.shader")
{
	gConsole.LogInfo("ParticlesRenderingPass::ParticlesRenderingPass");

	GetProgram().RegisterUniform("float", "uTime");
	GetProgram().RegisterUniform("mat4", "uMVP");

	float quadVertices[] = {
		// positions			// uv
		-1.0f,  1.0f, 0.0f,		1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,		0.0f, 1.0f,
		 1.0f,  1.0f, 0.0f,		0.0f, 0.0f,
					  
		-1.0f,  1.0f, 0.0f,		1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,		0.0f, 1.0f,
	 	 1.0f, -1.0f, 0.0f,		0.0f, 1.0f
	};
	
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	// gConsole.LogInfo("InstancedMeshRenderingPass::Ctor sizeof(Matrix): {}, sizeof(GLfloat): {}", sizeof(Matrix), sizeof(GLfloat));

	instancesTransform.Resize(16 * instancesLen);

	// fill array with zeros
	for (int i = 0; i < 16 * instancesLen; ++i)
	{
		instancesTransform[i] = 0.0f;
	}

	srand(42);

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

void ParticlesRenderingPass::OnRun(World* world, const CameraComponent* camera, const AARect& /*rect*/, ePassType passType = ePassType::GLOBAL)
{
	gConsole.LogInfo("ParticlesRenderingPass::OnRun");

	float Time = (float)TimeSystem::GetTimerElapsedTime(world, eEngineTimer::GAMEPLAY);
	const Matrix& mvp = camera->GetMVP();

	glDisable(GL_CULL_FACE);

	GetProgram().BindProgram();
	GetProgram().SetUniform("uTime", Time);
	GetProgram().SetUniform("uMVP", mvp);

	// UpdateInstanceVBO();
	glBindVertexArray(quadVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instancesTransform.GetSize()/16);
	glBindVertexArray(0);
	CHECK_GL_ERR();

	// Render meshes
	for (auto componentsTuple : world->IterateComponents<ParticleComponent>())
	{
		const ParticleComponent* particleCmp = std::get<ParticleComponent*>(componentsTuple);
		const EntityTransform& transform = particleCmp->GetTransform();
		const Matrix& objTransform = transform.GetGlobalTransformationMatrix();
		Matrix screenTransform = mvp * objTransform;
		GetProgram().SetUniform("uMVP", screenTransform);

		int partileLen = particleCmp->Emitter->GetInstances().GetSize() / 16;
		const GLParticleDeviceProxy* particleProxy = static_cast<const GLParticleDeviceProxy*>(particleCmp->Emitter->GetParticleProxy());
		GLuint particleVAO = particleProxy->GetVAO();

		gConsole.LogInfo("ParticlesRenderingPass::OnRun VAO: {}, found particles: {}",
			particleVAO, partileLen);

		glBindVertexArray(particleVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, partileLen);
		glBindVertexArray(0);
		CHECK_GL_ERR();
	}

	glEnable(GL_CULL_FACE);
}

void Poly::ParticlesRenderingPass::UpdateInstanceVBO()
{
	// fill array with zeros
	for (int i = 0; i < 16 * instancesLen; ++i)
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

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * instancesTransform.GetSize(), instancesTransform.GetData(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

float ParticlesRenderingPass::Random() const
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

float ParticlesRenderingPass::Random(float min, float max) const
{
	float rnd = Random();
	return Lerp(min, max, rnd);
}
