#pragma once

#include <Defines.hpp>
#include <Memory/SafePtr.hpp>
#include <ECS/DeferredTaskSystem.hpp>
#include <ECS/DeferredTaskBase.hpp>
#include <ECS/Scene.hpp>

namespace Poly
{
	//Helper structs. Used in AddComponentDeferredTask.
	//These are needed to store 'args...' in std::tuple for later use (in Execute(...) method)
	template <std::size_t... T>
	struct index {};

	template <std::size_t N, std::size_t... T>
	struct gen_seq : gen_seq<N - 1, N - 1, T...> {};

	template <std::size_t... T>
	struct gen_seq<0, T...> : index<T...> {};
	//---------------------------------------------------------------
	class DestroyEntityDeferredTask : public DeferredTaskBase
	{
	public:
		DestroyEntityDeferredTask(Entity* entity) : Ent(entity) {}

		virtual void Execute(Scene* w) { if(Ent) DeferredTaskSystem::DestroyEntityImmediate(w, Ent.Get()); }

		virtual const char* GetDescription() const { return "Destroy entity"; }
	private:
		SafePtr<Entity> Ent;
	};

	//---------------------------------------------------------------
	template<typename T, typename... Args>
	class AddComponentDeferredTask : public DeferredTaskBase
	{
	public:
		AddComponentDeferredTask(Entity* entity, Args&&... args) : Ent(entity), arguments(std::forward<Args>(args)...) {}

		virtual void Execute(Scene* w) { func(w, arguments); }

		virtual const char* GetDescription() const { return "Add component"; }

		template <typename... ARG, std::size_t... Is> void func(Scene* w, std::tuple<ARG...>& tup, index<Is...>) { if(Ent) DeferredTaskSystem::AddComponentImmediate<T>(w, Ent.Get(), std::get<Is>(tup)...); }
		template <typename... ARG> void func(Scene* w, std::tuple<ARG...>& tup) { func(w, tup, gen_seq<sizeof...(ARG)>{}); }
	private:
		SafePtr<Entity> Ent;
		std::tuple<Args...> arguments;
	};

	//---------------------------------------------------------------
	template<typename T>
	class RemoveComponentDeferredTask : public DeferredTaskBase
	{
	public:
		RemoveComponentDeferredTask(Entity* entity) : Ent(entity) {}

		virtual void Execute(Scene* w) { if(Ent) w->RemoveComponent<T>(Ent.Get()); }

		virtual const char* GetDescription() const { return "Remove component"; }
	private:
		SafePtr<Entity> Ent;
	};
}
