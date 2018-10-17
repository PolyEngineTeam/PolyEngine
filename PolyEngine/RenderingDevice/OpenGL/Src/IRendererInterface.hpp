#pragma once

#include <Defines.hpp>
#include <Rendering/Viewport.hpp>
#include <Rendering/Lighting/LightSourceComponent.hpp>

// TODO: inherit from BaseRenderPass - make multipass RenderPass

namespace Poly {

	class GLRenderingDevice;

	struct ScreenSize;
	class Scene;
	class AARect;
	class CameraComponent;
	class MeshRenderingComponent;


	struct SceneView : public BaseObject<> {
		SceneView(Scene* w, Viewport v)
			: SceneData(w), ViewportData(v), Rect(v.GetRect()), CameraCmp(v.GetCamera()),
			ParticleQueue(TranslucentComparator(v.GetCamera()->GetTransform().GetGlobalTranslation()), 0)
		{
		};

		/// Sorts geometry from far to near based on component pivot and camera postion
		struct TranslucentComparator
		{
			TranslucentComparator(Vector cameraPosition) 
				: CameraPosition(cameraPosition)
			{
			}

			Vector CameraPosition = Vector::ZERO;

			bool operator()(const ParticleComponent* a, const ParticleComponent* b) const
			{
				float aDistToCamera = (a->GetTransform().GetGlobalTranslation() - CameraPosition).LengthSquared();
				float bDistToCamera = (b->GetTransform().GetGlobalTranslation() - CameraPosition).LengthSquared();
				return aDistToCamera > bDistToCamera;
			}
		};

		Scene* SceneData;
		Viewport ViewportData;
		AARect Rect;
		const CameraComponent* CameraCmp;
		
		Dynarray<const MeshRenderingComponent*> DirShadowOpaqueQueue;
		Dynarray<const MeshRenderingComponent*> OpaqueQueue;
		Dynarray<const MeshRenderingComponent*> TranslucentQueue; // TODO: make translucent and particles one queue with common priority
		PriorityQueue<const ParticleComponent*, TranslucentComparator> ParticleQueue;

		Dynarray<const DirectionalLightComponent*> DirectionalLights;
		Dynarray<const PointLightComponent*> PointLights;
	};

	class IRendererInterface : public BaseObject<>
	{
	public:
		IRendererInterface(GLRenderingDevice* RenderingDeviceInterface);

		virtual void Init() = 0;		
		virtual void Resize(const ScreenSize& size) = 0;
		virtual void Render(const SceneView& sceneView) = 0;
		virtual void Deinit() = 0;

	protected:
		GLRenderingDevice* RDI;
	};
}