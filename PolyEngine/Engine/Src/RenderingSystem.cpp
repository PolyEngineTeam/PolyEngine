#include "EnginePCH.hpp"

#include "RenderingSystem.hpp"
#include "ShaderProgram.hpp"

#include <gl/glew.h>


using namespace Poly;

void TestDraw() {
	static ShaderProgram testProgram("Res/test.vsh", "Res/test.fsh");
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,
	};
	static bool Once = true;
	static GLuint vertexbuffer = 0;
	static GLuint VertexArrayID = 0;
	if (Once)
	{
		Once = false;
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		// Generate 1 buffer, put the resulting identifier in vertexbuffer
		glGenBuffers(1, &vertexbuffer);
		// The following commands will talk about our 'vertexbuffer' buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		// Give our vertices to OpenGL.
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	}

	testProgram.BindProgram();

	// Test Draw
	glBindVertexArray(VertexArrayID);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);
}

void RenderingSystem::RenderingPhase(World* world)
{	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	for (auto& kv : world->GetViewportWorldComponent().GetViewports())
	{
		glClear(GL_DEPTH_BUFFER_BIT);



		//glViewport()

	}

	
	// get camera projections
	auto allocator1 = world->GetComponentAllocator<BaseCameraComponent>();
	Matrix mvp;
	Matrix modelView;
	for (BaseCameraComponent& baseCameraCmp : *allocator1)
	{
		mvp = baseCameraCmp.GetMVP();
		modelView = baseCameraCmp.GetModelViewMatrix();
	}
	
	// clear framebuffer
	glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//TODO remove this
	TestDraw();

	// render objects
	auto allocator2 = world->GetComponentAllocator<MeshRenderingComponent>();
	for (MeshRenderingComponent& meshRenderingCmp : *allocator2)
	{
		Matrix transformation;
		TransformComponent* transformCmp = meshRenderingCmp.GetSibling<TransformComponent>();
		if (transformCmp)
		{
			transformation = transformCmp->GetGlobalTransformationMatrix();
		}
		else
			gConsole.LogError("Entity has mesh component but no transform component!");

		//TODO render meshes
	}

	world->GetEngine()->GetRenderingContext()->EndFrame();
}
