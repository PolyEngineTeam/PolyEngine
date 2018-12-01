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

	struct SceneView : public BaseObject<>
	{
		enum class eSortOrderType
		{
			BACK_TO_FRONT,
			FRONT_TO_BACK,
			_COUNT
		};
		
		// Sorts geometry based on distance to camera and sort order type
		struct DistanceToCameraComparator
		{
			DistanceToCameraComparator(Vector cameraPosition, eSortOrderType sortOrder = eSortOrderType::FRONT_TO_BACK) 
				: CameraPosition(cameraPosition), SortOrder(sortOrder)
			{
			}

			Vector CameraPosition;
			eSortOrderType SortOrder;

			bool operator()(const ComponentBase* a, const ComponentBase* b) const
			{
				float aDistToCamera = (a->GetTransform().GetGlobalTranslation() - CameraPosition).LengthSquared();
				float bDistToCamera = (b->GetTransform().GetGlobalTranslation() - CameraPosition).LengthSquared();
				return SortOrder == eSortOrderType::FRONT_TO_BACK
					? aDistToCamera < bDistToCamera
					: aDistToCamera > bDistToCamera;
			}
		};

		SceneView(Scene* s, const Viewport& v)
				: SceneData(s), ViewportData(v), Rect(v.GetRect()), CameraCmp(v.GetCamera()),
				ParticleQueue(DistanceToCameraComparator(v.GetCamera()->GetTransform().GetGlobalTranslation(), eSortOrderType::BACK_TO_FRONT), 0),
				TranslucentQueue(DistanceToCameraComparator(v.GetCamera()->GetTransform().GetGlobalTranslation(), eSortOrderType::BACK_TO_FRONT), 0),
				OpaqueQueue(DistanceToCameraComparator(v.GetCamera()->GetTransform().GetGlobalTranslation(), eSortOrderType::FRONT_TO_BACK), 0)
			{};

		Scene* SceneData;
		const Viewport& ViewportData;
		const AARect& Rect;
		const CameraComponent* CameraCmp;

		
		Dynarray<const MeshRenderingComponent*> DirShadowOpaqueList;
		PriorityQueue<const MeshRenderingComponent*, DistanceToCameraComparator> OpaqueQueue;
		PriorityQueue<const MeshRenderingComponent*, DistanceToCameraComparator> TranslucentQueue;
		PriorityQueue<const ParticleComponent*, DistanceToCameraComparator> ParticleQueue; // TODO: make translucent and particles one queue with common priority

		Dynarray<const DirectionalLightComponent*> DirectionalLights;
		Dynarray<const PointLightComponent*> PointLightList;
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