#include <EnginePCH.hpp>

#include <ECS/Entity.hpp>
#include <ECS/EntityTransform.hpp>
#include <ECS/Scene.hpp>

using namespace Poly;

RTTI_DEFINE_TYPE(::Poly::Entity);

//------------------------------------------------------------------------------
void Poly::EntityDeleter::operator()(Entity* e)
{
	Scene* scene = e->GetEntityScene();
	e->~Entity();
	scene->EntitiesAllocator.Free(e);
}

//------------------------------------------------------------------------------
void Poly::ComponentDeleter::operator()(ComponentBase* c)
{
	const size_t componentID = c->GetComponentID();
	Scene* scene = c->GetOwner()->GetEntityScene();
	c->~ComponentBase();
	scene->ComponentAllocators[componentID]->GenericFree(c);
}


Entity::Entity(Scene* world, Entity* parent)
	: Transform(this), EntityScene(world), ComponentPosessionFlags(0)
{
	Components.resize(MAX_COMPONENTS_COUNT);
	std::fill(Components.begin(), Components.end(), nullptr);

	if (parent)
		SetParent(parent);
}

void Poly::Entity::SetBBoxDirty()
{
	for (eEntityBoundingChannel channel : core::utils::IterateEnum<eEntityBoundingChannel>())
		BBoxDirty[channel] = true;
	if (Parent)
		Parent->SetBBoxDirty();
}

void Poly::Entity::ReleaseFromParent()
{
	if (Parent != nullptr)
	{
		for (auto& child : Parent->Children)
		{
			if (child.get() == this)
				child.release();
		}
		DISCARD std::remove_if(Parent->Children.begin(), Parent->Children.end(), [](const EntityUniquePtr& p) { return p.get() == nullptr; });
		Parent = nullptr;
		Transform.UpdateParentTransform();
	}
}

Poly::Entity::Entity()
 : Transform(this)
{
	Components.resize(MAX_COMPONENTS_COUNT);
	std::fill(Components.begin(), Components.end(), nullptr);
}

Poly::Entity::~Entity()
{
	//ReleaseFromParent();
	Children.clear();
	Components.clear();
}

void* Poly::Entity::AllocateEntity(RTTI::TypeInfo t)
{
	Scene* s = gEngine->GetCurrentlySerializedScene();
	Entity* ent = s->GetEntityAllocator().Alloc();
	::new(ent) Entity();
	return ent;
}

void* Poly::Entity::AllocateComponent(RTTI::TypeInfo t)
{
	Scene* s = gEngine->GetCurrentlySerializedScene();
	const size_t id = ComponentManager::Get().GetComponentID(t).value();
	core::utils::gConsole.LogInfo("Allocationg component: {} with ID {} and typeID {}", t.GetTypeName(), id, t);
	void* ptr = s->GetComponentAllocator(id)->GenericAlloc();
	t.CreateInstanceInPlace(ptr);
	return ptr;
}

void Poly::Entity::SetParent(Entity* parent)
{
	ASSERTE(parent, "New parent cannot be null");
	ASSERTE(parent != this, "Cannot parent myself!");
	HEAVY_ASSERTE(!ContainsChildRecursive(parent), "Detected parenting cycle!");

	ReleaseFromParent();

	Parent = parent;
	Parent->Children.push_back(EntityUniquePtr(this, EntityScene->GetEntityDeleter()));
	Parent->SetBBoxDirty();

	Transform.UpdateParentTransform();
}

bool Poly::Entity::ContainsChildRecursive(Entity* child) const
{
	if (std::find_if(Children.begin(), Children.end(), [child](const EntityUniquePtr& p) { return p.get() == child; }) != Children.end())
		return true;

	for (const EntityUniquePtr& myChild : Children)
		if (myChild->ContainsChildRecursive(child))
			return true;

	return false;
}

const core::math::AABox& Poly::Entity::GetLocalBoundingBox(eEntityBoundingChannel channel) const
{
	if (BBoxDirty[channel])
	{
		LocalBBox[channel].SetMin(core::math::Vector::ZERO);
		LocalBBox[channel].SetSize(core::math::Vector::ZERO);

		// Update bounding box by children boxes
		for (auto& child : Children)
		{
			core::math::AABox childBox = child->GetLocalBoundingBox(channel);
			LocalBBox[channel].Expand(childBox.GetTransformed(child->GetTransform().GetParentFromModel()));
		}

		// Components that affect bounding box
		for (auto& component : Components)
		{
			if (!component)
				continue;

			auto bboxOpt = component->GetBoundingBox(channel);
			if (bboxOpt.has_value())
				LocalBBox[channel].Expand(bboxOpt.value());
		}
		BBoxDirty[channel] = false;
	}
	return LocalBBox[channel];
}

core::math::AABox Poly::Entity::GetGlobalBoundingBox(eEntityBoundingChannel channel) const
{
	return GetLocalBoundingBox(channel).GetTransformed(GetTransform().GetWorldFromModel());
}

bool Entity::HasComponent(size_t ID) const
{
	HEAVY_ASSERTE(ID < MAX_COMPONENTS_COUNT, "Invalid component ID - greater than MAX_COMPONENTS_COUNT.");
	return ComponentPosessionFlags[ID];
}
