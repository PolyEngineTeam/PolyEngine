#include "EnginePCH.hpp"

#include "ECS/Entity.hpp"
#include "ECS/EntityTransform.hpp"
#include "ECS/Scene.hpp"

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
	scene->ComponentAllocators[componentID]->Free(c);
}


Entity::Entity(Scene* world, Entity* parent)
	: Transform(this), EntityScene(world), ComponentPosessionFlags(0)
{
	Components.Resize(MAX_COMPONENTS_COUNT);
	std::fill(Components.Begin(), Components.End(), nullptr);

	if (parent)
		SetParent(parent);
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

bool Entity::HasComponent(size_t ID) const
{
	HEAVY_ASSERTE(ID < MAX_COMPONENTS_COUNT, "Invalid component ID - greater than MAX_COMPONENTS_COUNT.");
	return ComponentPosessionFlags[ID];
}
