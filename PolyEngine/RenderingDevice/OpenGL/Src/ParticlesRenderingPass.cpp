#include "ParticlesRenderingPass.hpp"

#include "GLMeshDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"

#include <World.hpp>
#include <TimeSystem.hpp>
#include <CameraComponent.hpp>
#include <ParticlesComponent.hpp>
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16 * instancesLen, instancesTransform.GetData(), GL_STATIC_DRAW);
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
}

void ParticlesRenderingPass::OnRun(World* world, const CameraComponent* camera, const AARect& /*rect*/, ePassType passType = ePassType::GLOBAL)
{
	gConsole.LogInfo("ParticlesRenderingPass::OnRun");
	
	float Time = (float)TimeSystem::GetTimerElapsedTime(world, eEngineTimer::GAMEPLAY);

	GetProgram().BindProgram();
	const Matrix& mvp = camera->GetMVP();
	
	// gConsole.LogInfo("InstancedMeshRenderingPass::OnRun MVP: {}, InstTrans: {}", objTransform, inst);

	GetProgram().BindProgram();
	GetProgram().SetUniform("uTime", Time);

	// Render meshes
	for (auto componentsTuple : world->IterateComponents<ParticlesComponent>())
	{
		gConsole.LogInfo("ParticlesRenderingPass::OnRun found particles");

		const ParticlesComponent* meshCmp = std::get<ParticlesComponent*>(componentsTuple);
		const EntityTransform& transform = meshCmp->GetTransform();
		
		const Matrix& objTransform = transform.GetGlobalTransformationMatrix();
		Matrix screenTransform = mvp * objTransform;
		GetProgram().SetUniform("uMVP", screenTransform);

		glBindVertexArray(quadVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instancesLen);
		glBindVertexArray(0);
	}
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
