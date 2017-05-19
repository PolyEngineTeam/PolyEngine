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
		const UniqueID *Id;
	public:
		DestroyEntityHeavyTask(const UniqueID &entityID)
		{
			Id = new UniqueID(entityID);
		}
		~DestroyEntityHeavyTask()
		{
			if (Id) delete Id;
		}

		virtual void Execute(World* w)
		{
			w->DestroyEntity(*Id);
		}
	};
	//---------------------------------------------------------------
	template<typename T>
	class RemoveComponentHeavyTask : public HeavyTaskBase
	{
	private:
		const UniqueID *Id;
	public:
		RemoveComponentHeavyTask(const UniqueID &entityID)
		{
			Id = new UniqueID(entityID);
		}
		~RemoveComponentHeavyTask()
		{
			if (Id) delete Id;
		}

		virtual void Execute(World* w)
		{
			w->RemoveComponent<T>(*Id);
		}
	};

	template<typename T, typename... Args>
	class AddComponentHeavyTask : public HeavyTaskBase
	{
	private:
		const UniqueID *Id;
		std::tuple<Args...> arguments;
	public:
		AddComponentHeavyTask(const UniqueID &entityID, Args&&... args)
		{
			Id = new UniqueID(entityID);
			arguments = (std::make_tuple(std::forward<Args>(args)...));
		}
		~AddComponentHeavyTask()
		{
			if (Id) delete Id;
		}

		virtual void Execute(World* w)
		{
			func(w, arguments);
		}

		template <typename... ARG, std::size_t... Is>
		void func(World* w, std::tuple<ARG...>& tup, HeavyTaskHelper::index<Is...>)
		{
			w->AddComponent<T>(*Id, std::get<Is>(tup)...);
		}

		template <typename... ARG>
		void func(World* w, std::tuple<ARG...>& tup)
		{
			func(w, tup, HeavyTaskHelper::gen_seq<sizeof...(ARG)>{});
		}
	};
}
