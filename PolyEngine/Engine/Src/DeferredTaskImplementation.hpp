#pragma once

#include "DeferredTaskBase.hpp"
#include "World.hpp"

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
		DestroyEntityDeferredTask(const UniqueID &entityID) : Id(entityID) {}

		virtual void Execute(World* w) { DeferredTaskSystem::DestroyEntityImmediate(w, Id); }

		virtual const char* GetDescription() const { return "Destroy entity"; }
	private:
		const UniqueID Id;
	};

	//---------------------------------------------------------------
	template<typename T, typename... Args>
	class AddComponentDeferredTask : public DeferredTaskBase
	{
	public:
		AddComponentDeferredTask(const UniqueID &entityID, Args&&... args) : arguments(std::forward<Args>(args)...), Id(entityID) {}

		virtual void Execute(World* w) { func(w, arguments); }

		virtual const char* GetDescription() const { return "Add component"; }

		template <typename... ARG, std::size_t... Is> void func(World* w, std::tuple<ARG...>& tup, index<Is...>) { DeferredTaskSystem::AddComponentImmediate<T>(w, Id, std::get<Is>(tup)...); }
		template <typename... ARG> void func(World* w, std::tuple<ARG...>& tup) { func(w, tup, gen_seq<sizeof...(ARG)>{}); }
	private:
		const UniqueID Id;
		std::tuple<Args...> arguments;
	};

	//---------------------------------------------------------------
	template<typename T>
	class RemoveComponentDeferredTask : public DeferredTaskBase
	{
	public:
		RemoveComponentDeferredTask(const UniqueID &entityID) : Id(entityID) {}

		virtual void Execute(World* w) { w->RemoveComponent<T>(Id); }

		virtual const char* GetDescription() const { return "Remove component"; }
	private:
		const UniqueID Id;
	};
}
