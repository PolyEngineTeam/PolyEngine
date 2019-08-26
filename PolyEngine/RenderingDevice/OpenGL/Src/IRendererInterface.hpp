#pragma once

#include <pe/Defines.hpp>
#include <Rendering/Viewport.hpp>
#include <Rendering/Lighting/LightSourceComponent.hpp>
#include <Rendering/RenderingSettingsComponent.hpp>

// TODO: inherit from BaseRenderPass - make multipass RenderPass


namespace pe::core::math
{
	class AARect;
}

namespace Poly {

	class GLRenderingDevice;

	struct ScreenSize;
	class Scene;
	class CameraComponent;
	class MeshRenderingComponent;

	struct SceneView : public ::pe::core::BaseObject<>
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
			DistanceToCameraComparator(::pe::core::math::Vector cameraPosition, eSortOrderType sortOrder = eSortOrderType::FRONT_TO_BACK)
				: CameraPosition(cameraPosition), SortOrder(sortOrder)
			{
			}

			::pe::core::math::Vector CameraPosition;
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

		SceneView(Scene* s, Viewport& v)
			: SceneData(s), ViewportData(v), Rect(v.GetRect()), CameraCmp(v.GetCamera()),
			DirShadowCastersQueue(DistanceToCameraComparator(::pe::core::math::Vector::ZERO,										eSortOrderType::FRONT_TO_BACK), 0), // filled by GLRenderingDevice::CullShadowCasters
			OpaqueQueue(DistanceToCameraComparator(v.GetCamera()->GetTransform().GetGlobalTranslation(),		eSortOrderType::FRONT_TO_BACK), 0),
			TranslucentQueue(DistanceToCameraComparator(v.GetCamera()->GetTransform().GetGlobalTranslation(),	eSortOrderType::BACK_TO_FRONT), 0),
			ParticleQueue(DistanceToCameraComparator(v.GetCamera()->GetTransform().GetGlobalTranslation(),		eSortOrderType::BACK_TO_FRONT), 0)
		{
			SettingsCmp = s->GetWorldComponent<RenderingSettingsComponent>();
		};

		Scene* SceneData;
		const Viewport& ViewportData;
		const ::pe::core::math::AARect& Rect;

		const CameraComponent* CameraCmp;
		const RenderingSettingsComponent* SettingsCmp;
		
		::pe::core::storage::PriorityQueue<const MeshRenderingComponent*, DistanceToCameraComparator> DirShadowCastersQueue;
		::pe::core::storage::PriorityQueue<const MeshRenderingComponent*, DistanceToCameraComparator> OpaqueQueue;
		::pe::core::storage::PriorityQueue<const MeshRenderingComponent*, DistanceToCameraComparator> TranslucentQueue;
		::pe::core::storage::PriorityQueue<const ParticleComponent*, DistanceToCameraComparator> ParticleQueue; // TODO: make translucent and particles one queue with common priority

		::pe::core::math::AABox DirShadowAABBInLS;
		std::vector<const DirectionalLightComponent*> DirectionalLightList;
		std::vector<const PointLightComponent*> PointLightList;
	};

	class IRendererInterface : public ::pe::core::BaseObject<>
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