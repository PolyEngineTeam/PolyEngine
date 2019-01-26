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
	Components.Resize(MAX_COMPONENTS_COUNT);
	std::fill(Components.Begin(), Components.End(), nullptr);

	if (parent)
		SetParent(parent);
}

void Poly::Entity::SetBBoxDirty()
{
	for (eEntityBoundingChannel channel : IterateEnum<eEntityBoundingChannel>())
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
		Parent->Children.Remove([](const EntityUniquePtr& p) { return p.get() == nullptr; });
		Parent = nullptr;
		Transform.UpdateParentTransform();
	}
}

Poly::Entity::Entity()
 : Transform(this)
{
	Components.Resize(MAX_COMPONENTS_COUNT);
	std::fill(Components.Begin(), Components.End(), nullptr);
}

Poly::Entity::~Entity()
{
	//ReleaseFromParent();
	Children.Clear();
	Components.Clear();
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
	const size_t id = ComponentManager::Get().GetComponentID(t).Value();
	gConsole.LogInfo("Allocationg component: {} with ID {} and typeID {}", t.GetTypeName(), id, t);
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
	Parent->Children.PushBack(EntityUniquePtr(this, EntityScene->GetEntityDeleter()));
	Parent->SetBBoxDirty();

	Transform.UpdateParentTransform();
}

bool Poly::Entity::ContainsChildRecursive(Entity* child) const
{
	if (Children.Contains([child](const EntityUniquePtr& p) { return p.get() == child; }))
		return true;

	for (const EntityUniquePtr& myChild : Children)
		if (myChild->ContainsChildRecursive(child))
			return true;

	return false;
}

const AABox& Poly::Entity::GetLocalBoundingBox(eEntityBoundingChannel channel) const
{
	if (BBoxDirty[channel])
	{
		LocalBBox[channel].SetMin(Vector::ZERO);
		LocalBBox[channel].SetSize(Vector::ZERO);

		// Update bounding box by children boxes
		for (auto& child : Children)
		{
			AABox childBox = child->GetLocalBoundingBox(channel);
			LocalBBox[channel].Expand(childBox.GetTransformed(child->GetTransform().GetParentFromModel()));
		}

		// Components that affect bounding box
		for (auto& component : Components)
		{
			if (!component)
				continue;

			auto bboxOpt = component->GetBoundingBox(channel);
			if (bboxOpt.HasValue())
				LocalBBox[channel].Expand(bboxOpt.Value());
		}
		BBoxDirty[channel] = false;
	}
	return LocalBBox[channel];
}

AABox Poly::Entity::GetGlobalBoundingBox(eEntityBoundingChannel channel) const
{
	return GetLocalBoundingBox(channel).GetTransformed(GetTransform().GetWorldFromModel());
}

bool Entity::HasComponent(size_t ID) const
{
	HEAVY_ASSERTE(ID < MAX_COMPONENTS_COUNT, "Invalid component ID - greater than MAX_COMPONENTS_COUNT.");
	return ComponentPosessionFlags[ID];
}
