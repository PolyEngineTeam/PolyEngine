#include "DebugRenderingPass.hpp"

#include "DebugRenderingBuffers.hpp"

#include <World.hpp>
#include <CameraComponent.hpp>
#include <DebugRenderingComponent.hpp>

using namespace Poly;

DebugRenderingPass::DebugRenderingPass()
	: RenderingPassBase("Shaders/debugVert.shader", "Shaders/debugFrag.shader")
{
	GetProgram().RegisterUniform("mat4", "MVP");
}

void DebugRenderingPass::OnRun(World* world, const CameraComponent* camera, const AARect& /*rect*/, ePassType passType)
{
	GetProgram().BindProgram();
	const Matrix& MVP = camera->GetMVP();

	// Render Lines
	{
		auto debugLinesComponent = world->GetWorldComponent<DebugRenderingLinesComponent>();
		auto & debugLines = debugLinesComponent->DebugLines;
		auto & debugLinesColors = debugLinesComponent->DebugLinesColors;

		DebugRenderingBuffers debugLinesBuffers; // vertex and color buffer

		// set up buffer
		glBindVertexArray(debugLinesBuffers.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, debugLinesBuffers.VBO);
		glBufferData(GL_ARRAY_BUFFER, debugLines.GetSize() * sizeof(DebugRenderingLinesComponent::DebugLine)
									+ debugLinesColors.GetSize() * sizeof(DebugRenderingLinesComponent::DebugLine), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, debugLines.GetSize() * sizeof(DebugRenderingLinesComponent::DebugLine), (GLvoid*)debugLines.GetData());
		glBufferSubData(GL_ARRAY_BUFFER, debugLines.GetSize() * sizeof(DebugRenderingLinesComponent::DebugLine), debugLinesColors.GetSize() * sizeof(DebugRenderingLinesComponent::DebugLine), (GLvoid*)debugLinesColors.GetData());
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vector3D), NULL);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vector3D), (GLvoid*)(debugLines.GetSize() * sizeof(DebugRenderingLinesComponent::DebugLine)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GetProgram().SetUniform("MVP", MVP);

		glDrawArrays(GL_LINES, 0, (GLsizei)debugLines.GetSize() * 2);
		glBindVertexArray(0);

		debugLines.Clear();
		debugLinesColors.Clear();
	}
}
