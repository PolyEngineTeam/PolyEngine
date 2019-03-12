#include <PolyRenderingDeviceGLPCH.hpp>

#include <Pipeline/Text2DRenderingPass.hpp>
#include <Proxy/GLTextFieldBufferDeviceProxy.hpp>
#include <Proxy/GLTextureDeviceProxy.hpp>
#include <Proxy/GLMeshDeviceProxy.hpp>
#include <Common/GLUtils.hpp>

using namespace Poly;

Text2DRenderingPass::Text2DRenderingPass(const GLRenderingDevice* rdi)
: RenderingPassBase(rdi, "Shaders/text2D.vert.glsl", "Shaders/text2D.frag.glsl")
{

}

void Text2DRenderingPass::OnRun(Scene* world, const CameraComponent* /*camera*/, const AARect& rect, ePassType /*passType = ePassType::GLOBAL*/ )
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
