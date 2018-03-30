#include "Text2DRenderingPass.hpp"

#include "GLMeshDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"

#include <ECS/World.hpp>
#include <Rendering/Camera/CameraComponent.hpp>
#include <Rendering/MeshRenderingComponent.hpp>

#include <ECS/World.hpp>
#include <Configs/AssetsPathConfig.hpp>
#include <Rendering/Viewport.hpp>

#include <Rendering/Camera/CameraComponent.hpp>
#include <Rendering/MeshRenderingComponent.hpp>
#include <UI/ScreenSpaceTextComponent.hpp>
#include <Rendering/ViewportWorldComponent.hpp>

#include "GLTextFieldBufferDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"
#include "GLMeshDeviceProxy.hpp"
#include "GLUtils.hpp"

using namespace Poly;

Text2DRenderingPass::Text2DRenderingPass()
: RenderingPassBase("Shaders/text2DVert.shader", "Shaders/text2DFrag.shader")
{

}

void Text2DRenderingPass::OnRun(World* world, const CameraComponent* /*camera*/, const AARect& rect, ePassType /*passType = ePassType::GLOBAL*/ )
{
	// Text drawing
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ScreenSize screen = gEngine->GetRenderingDevice()->GetScreenSize();

	Matrix ortho;
	ortho.SetOrthographic(rect.GetMin().Y * screen.Height, rect.GetMax().Y * screen.Height, rect.GetMin().X * screen.Width, rect.GetMax().X * screen.Width, -1, 1);
	GetProgram().BindProgram();
	GetProgram().SetUniform("u_projection", ortho);

	for (auto componentsTuple : world->IterateComponents<ScreenSpaceTextComponent>())
	{
		ScreenSpaceTextComponent* textCmp = std::get<ScreenSpaceTextComponent*>(componentsTuple);
		Text2D& text = textCmp->GetText();
		GetProgram().SetUniform("u_textColor", text.GetFontColor());
		GetProgram().SetUniform("u_position", Vector((float)textCmp->GetScreenPosition().X, (float)textCmp->GetScreenPosition().Y, 0));
		text.UpdateDeviceBuffers();

		const GLTextFieldBufferDeviceProxy* textFieldBuffer = static_cast<const GLTextFieldBufferDeviceProxy*>(text.GetTextFieldBuffer());

		glBindVertexArray(textFieldBuffer->GetVAO());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, static_cast<const GLTextureDeviceProxy*>(text.GetFontTextureProxy())->GetTextureID());

		// Render glyph texture over quad
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(6 * textFieldBuffer->GetSize()));

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
	CHECK_GL_ERR();

	glDisable(GL_BLEND);
}
