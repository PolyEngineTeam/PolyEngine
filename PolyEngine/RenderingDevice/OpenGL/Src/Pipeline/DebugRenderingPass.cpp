#include "PolyRenderingDeviceGLPCH.hpp"

#include "Pipeline/DebugRenderingPass.hpp"
#include "Common/DebugRenderingBuffers.hpp"

using namespace Poly;

DebugRenderingPass::DebugRenderingPass(const GLRenderingDevice* rdi)
	: RenderingPassBase(rdi, "Shaders/debug.vert.glsl", "Shaders/debug.frag.glsl")
{
	GetProgram().RegisterUniform("mat4", "uMVP");
}

void DebugRenderingPass::OnRun(Scene* world, const CameraComponent* camera, const AARect& /*rect*/, ePassType /*passType*/)
{
	GetProgram().BindProgram();
	const Matrix& MVP = camera->GetClipFromWorld();

	// Render Lines
	{
		auto debugLinesComponent = world->GetWorldComponent<DebugDrawStateWorldComponent>();
		auto& debugLines = debugLinesComponent->DebugLines;
		auto& debugLinesColors = debugLinesComponent->DebugLinesColors;

		DebugRenderingBuffers debugLinesBuffers; // vertex and color buffer

		// set up buffer
		glBindVertexArray(debugLinesBuffers.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, debugLinesBuffers.VBO);
		glBufferData(GL_ARRAY_BUFFER, debugLines.GetSize() * sizeof(DebugDrawStateWorldComponent::DebugLine)
									+ debugLinesColors.GetSize() * sizeof(DebugDrawStateWorldComponent::DebugLineColor), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, debugLines.GetSize() * sizeof(DebugDrawStateWorldComponent::DebugLine), (GLvoid*)debugLines.GetData());
		glBufferSubData(GL_ARRAY_BUFFER, debugLines.GetSize() * sizeof(DebugDrawStateWorldComponent::DebugLine), debugLinesColors.GetSize() * sizeof(DebugDrawStateWorldComponent::DebugLineColor), (GLvoid*)debugLinesColors.GetData());
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), NULL);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Color), (GLvoid*)(debugLines.GetSize() * sizeof(DebugDrawStateWorldComponent::DebugLine)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GetProgram().SetUniform("uMVP", MVP);

		glDrawArrays(GL_LINES, 0, (GLsizei)debugLines.GetSize() * 2);
		glBindVertexArray(0);

		debugLines.Clear();
		debugLinesColors.Clear();
	}
}
