#pragma once

#include "HeavyTaskBase.hpp"
#include "World.hpp"


namespace Poly
{
	//Helper structs. Used in AddComponentHeavyTask.
	//These are needed to store 'args...' in std::tuple for later use (in Execute(...) method)
	template <std::size_t... T>
	struct index {};

	template <std::size_t N, std::size_t... T>
	struct gen_seq : gen_seq<N - 1, N - 1, T...> {};

	template <std::size_t... T>
	struct gen_seq<0, T...> : index<T...> {};
	//---------------------------------------------------------------
	class SpawnEntityHeavyTask : public HeavyTaskBase
	{
	public:
		SpawnEntityHeavyTask() {}

		virtual void Execute(World* w) { Id = w->SpawnEntity(); }

		virtual String GetDescription() const { return String("Spawn entity"); }

		UniqueID GetID() { return Id; }
	private:
		UniqueID Id;
	};

	class DestroyEntityHeavyTask : public HeavyTaskBase
	{
	public:
		DestroyEntityHeavyTask(const UniqueID &entityID) : Id(entityID) {}

		virtual void Execute(World* w) { w->DestroyEntity(Id); }

		virtual String GetDescription() const { return String("Destroy entity"); }
	private:
		const UniqueID Id;
	};
	
	template<typename T, typename... Args>
	class AddComponentHeavyTask : public HeavyTaskBase
	{
	public:
		AddComponentHeavyTask(const UniqueID &entityID, Args&&... args) : arguments(std::make_tuple(std::forward<Args>(args)...)), Id(entityID) {}

		virtual void Execute(World* w) { func(w, arguments); }

		virtual String GetDescription() const { return String("Add component"); }

		template <typename... ARG, std::size_t... Is> void func(World* w, std::tuple<ARG...>& tup, index<Is...>) { w->AddComponent<T>(Id, std::get<Is>(tup)...); }
		template <typename... ARG> void func(World* w, std::tuple<ARG...>& tup) { func(w, tup, gen_seq<sizeof...(ARG)>{}); }
	private:
		const UniqueID Id;
		std::tuple<Args...> arguments;
	};

	template<typename T>
	class RemoveComponentHeavyTask : public HeavyTaskBase
	{
	public:
		RemoveComponentHeavyTask(const UniqueID &entityID) : Id(entityID) {}

		virtual void Execute(World* w) { w->RemoveComponent<T>(Id); }

		virtual String GetDescription() const { return String("Remove component"); }
	private:
		const UniqueID Id;
	};
}
