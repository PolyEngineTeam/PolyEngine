#pragma once

#include "HeavyTaskBase.hpp"
#include "World.hpp"

namespace HeavyTaskHelper
{
	template <std::size_t... T>
	struct index {};

	template <std::size_t N, std::size_t... T>
	struct gen_seq : gen_seq<N - 1, N - 1, T...> {};

	template <std::size_t... T>
	struct gen_seq<0, T...> : index<T...> {};
}

namespace Poly
{
	class DestroyEntityHeavyTask : public HeavyTaskBase
	{
	private:
		const UniqueID Id;
	public:
		DestroyEntityHeavyTask(const UniqueID &entityID) : Id(entityID) {}

		virtual void Execute(World* w)
		{
			w->DestroyEntity(Id);
		}

		virtual String GetDescription() const { return String("Destroy entity"); }
	};
	//---------------------------------------------------------------
	template<typename T>
	class RemoveComponentHeavyTask : public HeavyTaskBase
	{
	private:
		const UniqueID Id;
	public:
		RemoveComponentHeavyTask(const UniqueID &entityID) : Id(entityID) {}

		virtual void Execute(World* w)
		{
			w->RemoveComponent<T>(Id);
		}
		virtual String GetDescription() const { return String("Remove component"); }
	};

	template<typename T, typename... Args>
	class AddComponentHeavyTask : public HeavyTaskBase
	{
	private:
		const UniqueID Id;
		std::tuple<Args...> arguments;
	public:
		AddComponentHeavyTask(const UniqueID &entityID, Args&&... args) : arguments(std::make_tuple(std::forward<Args>(args)...)), Id(entityID) {}

		virtual void Execute(World* w)
		{
			func(w, arguments);
		}

		template <typename... ARG, std::size_t... Is>
		void func(World* w, std::tuple<ARG...>& tup, HeavyTaskHelper::index<Is...>)
		{
			w->AddComponent<T>(Id, std::get<Is>(tup)...);
		}

		template <typename... ARG>
		void func(World* w, std::tuple<ARG...>& tup)
		{
			func(w, tup, HeavyTaskHelper::gen_seq<sizeof...(ARG)>{});
		}
		virtual String GetDescription() const { return String("Add component"); }
	};
}
