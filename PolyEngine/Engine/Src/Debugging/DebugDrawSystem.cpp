#include <EnginePCH.hpp>

#include <Debugging/DebugDrawComponents.hpp>
#include <Debugging/DebugDrawSystem.hpp>
#include <Rendering/ViewportWorldComponent.hpp>
#include <Rendering/Camera/CameraComponent.hpp>
#include <Rendering/MeshRenderingComponent.hpp>
#include <Rendering/Lighting/LightSourceComponent.hpp>
#include <ECS/Scene.hpp>
#include <ECS/DeferredTaskSystem.hpp>
#include <Movement/MovementSystem.hpp>
#include <UI/ScreenSpaceTextComponent.hpp>
#include <Physics2D/Rigidbody2DComponent.hpp>

using namespace Poly;

namespace Util
{
	/// <summary>Get vector with minimum/maximum/else (depends on comparison function) values
	/// on each axis in given set of vectors.</summary>
	Vector3f FindExtremum(const std::vector<Vector3f>& dynarray, std::function< bool(const float, const float) > comp)
	{
		ASSERTE(dynarray.size() > 0, "SubMesh vertex data is empty.");

		Vector3f result = dynarray[0];
		for (auto const & vec : dynarray)
		{
			if (comp(vec.X, result.X))
				result.X = vec.X;
			if (comp(vec.Y, result.Y))
				result.Y = vec.Y;
			if (comp(vec.Z, result.Z))
				result.Z = vec.Z;
		}

		return result;
	}

	/// <summary>Produces two orthonormal vectors from a single normal vector.</summary>
	/// Building an Orthonormal Basis, Revisited; http://jcgt.org/published/0006/01/01/
	void BranchlessONB(const Vector & n, Vector & b1, Vector & b2)
	{
		float sign = std::copysignf(1.0f, n.Z);
		const float a = -1.0f / (sign + n.Z);
		const float b = n.X * n.Y * a;
		b1 = Vector(1.0f + sign * n.X * n.X * a, sign * b, -sign * n.X);
		b2 = Vector(b, sign + n.Y * n.Y * a, -n.Y);
	}
}

void DebugDrawSystem::DebugRenderingUpdatePhase(Scene* scene)
{
	gDebugConfig.DebugRender = false;
	for (auto& kv : scene->GetWorldComponent<ViewportWorldComponent>()->GetViewports())
	{
		CameraComponent* cameraCmp = kv.second.GetCamera();
		if (cameraCmp->GetRenderingMode() == eRenderingModeType::IMMEDIATE_DEBUG)
			gDebugConfig.DebugRender = true;
	}

	auto debugDrawWorldCmp = scene->GetWorldComponent<DebugDrawStateWorldComponent>();
	if (!gDebugConfig.DebugRender)
	{
		debugDrawWorldCmp->Clear();
		return;
	}

	// Prepare text2D to render
	{
		size_t usedTextEntites = 0;
		for (const auto& text2D : debugDrawWorldCmp->DebugTexts2D)
		{
			Entity* entToUse = nullptr;
			while (usedTextEntites < debugDrawWorldCmp->Text2DEntityPool.size())
			{
				if (debugDrawWorldCmp->Text2DEntityPool[usedTextEntites])
				{
					entToUse = debugDrawWorldCmp->Text2DEntityPool[usedTextEntites].Get();
					++usedTextEntites;
					break;
				}
				else
					debugDrawWorldCmp->Text2DEntityPool.erase(debugDrawWorldCmp->Text2DEntityPool.begin() + usedTextEntites);
			}

			if (!entToUse)
			{
				entToUse = DeferredTaskSystem::SpawnEntityImmediate(scene);
				DeferredTaskSystem::AddComponentImmediate<ScreenSpaceTextComponent>(scene, entToUse, Vector2i::ZERO, "Fonts/Raleway/Raleway-Regular.ttf", eResourceSource::ENGINE, 0);
				debugDrawWorldCmp->Text2DEntityPool.push_back(entToUse);
				++usedTextEntites;
			}

			ScreenSpaceTextComponent* textCmp = entToUse->GetComponent<ScreenSpaceTextComponent>();
			textCmp->SetText(text2D.Text);
			textCmp->GetText().SetFontSize(text2D.FontSize);
			textCmp->GetText().SetFontColor(text2D.FontColor);
			textCmp->SetScreenPosition(text2D.Position);
		}


		// Clear rest of the text entities
		for (size_t i = usedTextEntites; i < debugDrawWorldCmp->Text2DEntityPool.size(); ++i)
		{
			Entity* ent = debugDrawWorldCmp->Text2DEntityPool[i].Get();
			if (ent)
			{
				ScreenSpaceTextComponent* textCmp = ent->GetComponent<ScreenSpaceTextComponent>();
				textCmp->SetText("");
				textCmp->GetText().SetFontSize(0);
				textCmp->GetText().SetFontColor(Color::WHITE);
				textCmp->SetScreenPosition(Vector2i::ZERO);
			}
			else
			{
				debugDrawWorldCmp->Text2DEntityPool.erase(debugDrawWorldCmp->Text2DEntityPool.begin() + i);
				--i;
			}
		}

		debugDrawWorldCmp->DebugTexts2D.clear();
	}

	enum class RenderMode : int { /*POINT = 1,*/ LINE = 1, /*STRING,*/ _COUNT };

	// iterate RenderMode::_COUNT times to create shapes defining debug primitives
	for(int renderMode = static_cast<int>(RenderMode::LINE); renderMode < static_cast<int>(RenderMode::_COUNT); ++renderMode)
	{
		for(auto componentsTuple : scene->IterateComponents<MeshRenderingComponent, DebugDrawableComponent>())
		{
			const auto ddrawCmp = std::get<DebugDrawableComponent*>(componentsTuple);
			const auto meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
			const auto& transform = meshCmp->GetTransform();

			if(ddrawCmp == nullptr)
				continue;

			if(!gDebugConfig.DebugDrawPresets.IsSet(ddrawCmp->entityPreset))
				continue;

			const Matrix& objTransform = transform.GetWorldFromModel();

			// spawn a box for every mesh, in correct size
			for(const auto subMesh : meshCmp->GetMesh()->GetSubMeshes())
			{
				AABox box = subMesh->GetAABox();
				Vector minVector = objTransform * box.GetMin();
				Vector maxVector = objTransform * box.GetMax();

				const auto boundingOffset = Vector(0.04f, 0.04f, 0.04f);
				// move vector away a little bit from a mesh
				minVector -= boundingOffset;
				maxVector += boundingOffset;
				
				DrawBox(scene, minVector, maxVector, Color::GREEN);
			}
		}

		// @fixme
		// move iteration over RigidBody2DComponent to different debug-system (f.ex. PhysicsDebugDrawSystem)
		for(auto componentsTuple : scene->IterateComponents<RigidBody2DComponent, DebugDrawableComponent>())
		{
			const auto rigidbodyCmp = std::get<RigidBody2DComponent*>(componentsTuple);
			const auto ddrawCmp = std::get<DebugDrawableComponent*>(componentsTuple);
			const auto& transform = rigidbodyCmp->GetTransform();

			if(ddrawCmp == nullptr)
				continue;

			if(!gDebugConfig.DebugDrawPresets.IsSet(ddrawCmp->entityPreset))
				continue;

			auto localTrans = transform.GetLocalTranslation();
			auto velocity = rigidbodyCmp->GetLinearVelocity();

			if(velocity.LengthSquared() == 0.0f)
				continue;

			DrawArrow(scene, localTrans, velocity, Color::RED);
		}


		for(auto componentsTuple : scene->IterateComponents<DirectionalLightComponent, DebugDrawableComponent>())
		{
			const auto dirLightCmp = std::get<DirectionalLightComponent*>(componentsTuple);
			const auto ddrawCmp = std::get<DebugDrawableComponent*>(componentsTuple);
			const auto transform = dirLightCmp->GetTransform();

			if(ddrawCmp == nullptr)
				continue;

			if(!gDebugConfig.DebugDrawPresets.IsSet(ddrawCmp->entityPreset))
				continue;

			auto localTrans = transform.GetLocalTranslation();
			//auto lightDirection = Vector(1.0f, 1.0f, 1.0f);
			auto directionRotation = transform.GetGlobalForward();
			auto lightMagnitude = dirLightCmp->GetIntensity() * 2.0f;

			DrawArrow(scene, localTrans, directionRotation*lightMagnitude, Color::WHITE);
		}
	}
}

void Poly::DebugDrawSystem::DrawLine(Scene* scene, const Vector& begin, const Vector& end, const Color& color)
{
	if (!gDebugConfig.DebugRender)
		return;

	Vector3f meshvecBegin(begin.X, begin.Y, begin.Z), meshvecEnd(end.X, end.Y, end.Z);
	auto debugLinesComponent = scene->GetWorldComponent<DebugDrawStateWorldComponent>();
	debugLinesComponent->DebugLines.push_back(DebugDrawStateWorldComponent::DebugLine{ meshvecBegin, meshvecEnd });
	debugLinesComponent->DebugLinesColors.push_back(DebugDrawStateWorldComponent::DebugLineColor{ color, color });
}

void Poly::DebugDrawSystem::DrawBox(Scene* scene, const Vector& mins, const Vector& maxs, const Color& color)
{
	if (!gDebugConfig.DebugRender)
		return;

	std::array<Vector, 8> points;
	std::array<Vector, 2> minmaxVector = { {mins, maxs} };

	for (unsigned int i = 0; i < points.size(); ++i)
	{
		points[i].X = minmaxVector[(i ^ (i >> 1)) & 1].X;
		points[i].Y = minmaxVector[(i >> 1) & 1].Y;
		points[i].Z = minmaxVector[(i >> 2) & 1].Z;
	}
	// i: X Y Z
	// 0: 0 0 0
	// 1: 1 0 0
	// 2: 1 1 0
	// 3: 0 1 0
	// 4: 0 0 1
	// 5: 1 0 1
	// 6: 1 1 1
	// 7: 0 1 1

	// bottom
	DrawLine(scene, points[0], points[4], color);
	DrawLine(scene, points[0], points[1], color);
	DrawLine(scene, points[5], points[4], color);
	DrawLine(scene, points[5], points[1], color);

	// top
	DrawLine(scene, points[3], points[7], color);
	DrawLine(scene, points[3], points[2], color);
	DrawLine(scene, points[6], points[7], color);
	DrawLine(scene, points[6], points[2], color);

	// top-bottom lines
	DrawLine(scene, points[0], points[3], color);
	DrawLine(scene, points[1], points[2], color);
	DrawLine(scene, points[4], points[7], color);
	DrawLine(scene, points[5], points[6], color);
}

void DebugDrawSystem::DrawBox(Scene* scene, const Vector& mins, const Vector& maxs, const Matrix& worldFromSpace, const Color& color)
{
	if (!gDebugConfig.DebugRender)
		return;

	std::array<Vector, 8> points;
	std::array<Vector, 2> minmaxVector = { { mins, maxs } };

	for (unsigned int i = 0; i < points.size(); ++i)
	{
		points[i].X = minmaxVector[(i ^ (i >> 1)) & 1].X;
		points[i].Y = minmaxVector[(i >> 1) & 1].Y;
		points[i].Z = minmaxVector[(i >> 2) & 1].Z;
	}
	// i: X Y Z
	// 0: 0 0 0
	// 1: 1 0 0
	// 2: 1 1 0
	// 3: 0 1 0
	// 4: 0 0 1
	// 5: 1 0 1
	// 6: 1 1 1
	// 7: 0 1 1

	// bottom
	DebugDrawSystem::DrawLine(scene, worldFromSpace * points[0], worldFromSpace * points[4], color);
	DebugDrawSystem::DrawLine(scene, worldFromSpace * points[0], worldFromSpace * points[1], color);
	DebugDrawSystem::DrawLine(scene, worldFromSpace * points[5], worldFromSpace * points[4], color);
	DebugDrawSystem::DrawLine(scene, worldFromSpace * points[5], worldFromSpace * points[1], color);

	// top
	DebugDrawSystem::DrawLine(scene, worldFromSpace * points[3], worldFromSpace * points[2], color);
	DebugDrawSystem::DrawLine(scene, worldFromSpace * points[3], worldFromSpace * points[7], color);
	DebugDrawSystem::DrawLine(scene, worldFromSpace * points[6], worldFromSpace * points[7], color);
	DebugDrawSystem::DrawLine(scene, worldFromSpace * points[6], worldFromSpace * points[2], color);

	// top-bottom lines
	DebugDrawSystem::DrawLine(scene, worldFromSpace * points[0], worldFromSpace * points[3], color);
	DebugDrawSystem::DrawLine(scene, worldFromSpace * points[1], worldFromSpace * points[2], color);
	DebugDrawSystem::DrawLine(scene, worldFromSpace * points[4], worldFromSpace * points[7], color);
	DebugDrawSystem::DrawLine(scene, worldFromSpace * points[5], worldFromSpace * points[6], color);
}

void Poly::DebugDrawSystem::DrawBox(Scene* scene, const AABox & box, const Color & color)
{
	DrawBox(scene, box.GetMin(), box.GetMax(), color);
}

void Poly::DebugDrawSystem::DrawFrustum(Scene* scene, const Frustum& frustum, const Matrix& viewFromWorld, const Color color)
{
	Matrix clipFromView;
	clipFromView.SetPerspective(frustum.GetFov(), frustum.GetAspect(), frustum.GetZNear(), frustum.GetZFar());
	Matrix clipFromWorld = clipFromView * viewFromWorld;

	// Transform frustum corners to DirLightSpace
	std::vector<Vector> cornersInNDC{
		Vector(-1.0f,  1.0f, -1.0f), // back  left	top
		Vector( 1.0f,  1.0f, -1.0f), // back  right top
		Vector(-1.0f, -1.0f, -1.0f), // back  left  bot
		Vector( 1.0f, -1.0f, -1.0f), // back  right bot
		Vector(-1.0f,  1.0f,  1.0f), // front left	top
		Vector( 1.0f,  1.0f,  1.0f), // front right top
		Vector(-1.0f, -1.0f,  1.0f), // front left  bot
		Vector( 1.0f, -1.0f,  1.0f)  // front right bot
	};

	// Transform frustum corners from NDC to World
	// could be done in one iteration but we need to do perspective division by W
	Matrix worldFromClip = clipFromWorld.GetInversed();
	std::vector<Vector> cornersInWS;
	for (Vector posInClip : cornersInNDC)
	{
		Vector posInWS = worldFromClip * posInClip;
		posInWS.X /= posInWS.W;
		posInWS.Y /= posInWS.W;
		posInWS.Z /= posInWS.W;
		cornersInWS.push_back(posInWS);
	}

	DrawFrustumPoints(scene, cornersInWS, color);
}

void Poly::DebugDrawSystem::DrawFrustumPoints(Scene* scene, const std::vector<Vector>& cornersInWorld, const Color color)
{
	// Vertives should be in order:
	// back  left  top
	// back  right top
	// back  left  bot
	// back  right bot
	// front left  top
	// front right top
	// front left  bot
	// front right bot

	ASSERTE(cornersInWorld.size() == 8, "Wrong number of frustum corners to draw");

	for (size_t i = 0; i < 4; ++i)
		DebugDrawSystem::DrawLine(scene, cornersInWorld[i], cornersInWorld[i + 4], color);

	for (size_t i = 0; i < 2; ++i)
	{
		DebugDrawSystem::DrawLine(scene, cornersInWorld[0 + i * 4], cornersInWorld[1 + i * 4], color);
		DebugDrawSystem::DrawLine(scene, cornersInWorld[2 + i * 4], cornersInWorld[3 + i * 4], color);
		DebugDrawSystem::DrawLine(scene, cornersInWorld[0 + i * 4], cornersInWorld[2 + i * 4], color);
		DebugDrawSystem::DrawLine(scene, cornersInWorld[1 + i * 4], cornersInWorld[3 + i * 4], color);
	}
}

void Poly::DebugDrawSystem::DrawCircle(Scene* scene, const Vector& position, float radius, Vector orientation, const Color& color)
{
	if (!gDebugConfig.DebugRender)
		return;

	const auto circleSegmentStep = Angle::FromDegrees(15.0f);
	orientation.Normalize();

	Quaternion rotAroundCircle;
	rotAroundCircle.SetRotation(orientation, circleSegmentStep);

	// get a vector perpendicular to orientation
	Vector right, forward; // forward is unused
	Util::BranchlessONB(orientation, right, forward);

	right *= radius;

	for(float degrees = 0.0f; degrees < 360.0f; degrees += circleSegmentStep.AsDegrees())
	{
		Vector previousSegmentPoint = right;
		right = rotAroundCircle*right;
		DrawLine(scene, position + previousSegmentPoint, position + right, color);
	}

	DrawArrow(scene, position, right, color);
}

void Poly::DebugDrawSystem::DrawSphere(Scene* scene, const Vector& position, float radius, const Color& color)
{
	if (!gDebugConfig.DebugRender)
		return;

	DrawCircle(scene, position, radius, Vector(1.0f, 0.0f, 0.0f), color);
	DrawCircle(scene, position, radius, Vector(0.0f, 1.0f, 0.0f), color);
	DrawCircle(scene, position, radius, Vector(0.0f, 0.0f, 1.0f), color);
}

void Poly::DebugDrawSystem::DrawArrow(Scene* scene, Vector position, Vector directionVector, const Color& color)
{
	if (!gDebugConfig.DebugRender)
		return;

	constexpr float arrowLengthScale = 0.5f;
	constexpr float arrowheadScale = 0.2f;
	
	// body line
	auto arrowTip = position + directionVector*arrowLengthScale;
	DrawLine(scene, position, arrowTip, color);
	directionVector.Normalize();

	// arrowhead
	const auto rotationStep = Angle::FromDegrees(60.0f);
	Quaternion rotAroundDirectionVector;
	rotAroundDirectionVector.SetRotation(directionVector, rotationStep);

	if(position.X == 0.0f && position.Y == 0.0f && position.Z == 0.0f) {
		position.X += 0.01f; position.Y += 0.01f; position.Z += 0.01f;
	}

	// calculate point which sets edge points around the arrowhead
	Vector perp[2];
	perp[0] = Vector(directionVector.Z, directionVector.Z, -directionVector.X - directionVector.Y);
	perp[1] = Vector(-directionVector.Y - directionVector.Z, directionVector.X, directionVector.X);
	int selectIndex = ((directionVector.Z != 0) && (-directionVector.X != directionVector.Y)); // this is not a branch
	Vector perpendicularVector = perp[selectIndex];

	perpendicularVector.Normalize();
	perpendicularVector *= arrowheadScale; // scale the arrowhead (cone base diameter)

	for (float degrees = 0.0f; degrees < 360.0f; degrees += rotationStep.AsDegrees())
	{
		DrawLine(scene, arrowTip, arrowTip + perpendicularVector, color);
		DrawLine(scene, arrowTip + perpendicularVector, arrowTip + directionVector*arrowheadScale, color);
		perpendicularVector = rotAroundDirectionVector* perpendicularVector;
	}
}

void Poly::DebugDrawSystem::DrawText2D(Scene* scene, const Vector2i& screenPosition, String text, size_t fontSize, const Color& color)
{
	if (!gDebugConfig.DebugRender)
		return;

	auto debugLinesComponent = scene->GetWorldComponent<DebugDrawStateWorldComponent>();
	debugLinesComponent->DebugTexts2D.push_back({ std::move(text), screenPosition , fontSize, color });
}
