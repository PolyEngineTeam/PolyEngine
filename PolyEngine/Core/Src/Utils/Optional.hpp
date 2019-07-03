#pragma once

#include <Defines.hpp>
#include <Memory/ObjectLifetimeHelpers.hpp>

namespace Poly {
	namespace Impl {
		namespace Object = ObjectLifetimeHelper;

		template<typename V>
		class OptionalBase : public BaseObject<> {
		public:
			using ValueType = V;

			OptionalBase() : initialized(false), dummy{} {}
			OptionalBase(const V&  value) : initialized(true), value_storage(value) {}
			OptionalBase(      V&& value) : initialized(true), value_storage(std::move(value)) {}
			~OptionalBase() { if (HasValue()) { Object::Destroy(&Value()); } }

			OptionalBase(OptionalBase&& other) {
				if (other.HasValue()) {
					this->initialized = true;
					Object::MoveCreate(&this->Value(), std::move(other.Value()));

					Object::Destroy(&other.Value());
					other.initialized = false;
				} else {
					this->initialized = false;
				}
			}
			OptionalBase& operator=(OptionalBase&& other) {
				if (other.HasValue()) {
					this->initialized = true;
					this->Value() = std::move(other.Value());

					Object::Destroy(&other.Value());
					other.initialized = false;
				} else if (this->HasValue()) {
					Object::Destroy(&this->Value());
					this->initialized = false;
				}
				return *this;
			}

			bool HasValue() const { return initialized; }
			operator bool() const { return HasValue(); }

			const V&  Value() const& { ASSERTE(HasValue(), "Attempting to get a value from an empty optional"); return value_storage; }
			      V&  Value() &      { ASSERTE(HasValue(), "Attempting to get a value from an empty optional"); return value_storage; }
			      V&& Value() &&     { ASSERTE(HasValue(), "Attempting to get a value from an empty optional"); return std::move(value_storage); }

			V TakeValue() {
				ASSERTE(HasValue(), "Attempting to take a value from an empty optional");

				V ret = std::move(value_storage);

				initialized = false;
				Object::Destroy(&value_storage);

				return ret;
			}

			V ValueOr(V&& fallback) const &  { if (HasValue()) { return Value(); } return std::move(fallback); }
			V ValueOr(V&& fallback) const && { if (HasValue()) { return std::move(Value()); } return std::move(fallback); }

		protected:
			bool initialized;
			union {
				V value_storage;
				char dummy;
			};
		};
	}

	template<typename V>
	class Optional : public Impl::OptionalBase<V> {
	public:
		Optional() : Impl::OptionalBase<V>() {} //note(vuko): work around older compilers not importing default constructor from the base class
		using Impl::OptionalBase<V>::OptionalBase;
		using Impl::OptionalBase<V>::initialized;

		Optional(Optional&& other) : Impl::OptionalBase<V>(std::move(other)) {} //note(vuko): another workaround

		Optional(const Optional& other) : Impl::OptionalBase<V>() {
			namespace Object = ObjectLifetimeHelper;
			if (other.HasValue()) {
				this->initialized = true;
				Object::CopyCreate(&this->Value(), other.Value());
			} else {
				this->initialized = false;
			}
		}
		Optional& operator=(const Optional& other) {
			namespace Object = ObjectLifetimeHelper;
			if (other.HasValue()) {
				this->initialized = true;
				this->Value() = other.Value();
			} else if (this->HasValue()) {
				Object::Destroy(&this->Value());
				this->initialized = false;
			}
			return *this;
		}
	};

	template<typename V>
	class Optional<V&&> : public Impl::OptionalBase<V> {
	public:
		Optional() : Impl::OptionalBase<V>() {} //note(vuko): see above
		using Impl::OptionalBase<V>::OptionalBase;
		Optional(const V&) = delete; //note(vuko): prevent copying value in
	};

	template<typename V>
	class Optional<V&> {
	public:
		using ValueType = V&;

		Optional() : value_storage(nullptr) {}
		Optional(V& value) : value_storage(&value) {}
		~Optional() = default;

		Optional(Optional&& other) { this->value_storage = other.value_storage; other.value_storage = nullptr; }
		Optional& operator=(Optional&& other) { this->value_storage = other.value_storage; other.value_storage = nullptr; return *this; }

		Optional(const Optional& other) { this->value_storage = other.value_storage; }
		Optional& operator=(const Optional& other) { this->value_storage = other.value_storage; return *this; }

		bool HasValue() const  { return value_storage != nullptr; }
		explicit operator bool() const { return HasValue(); }

		const V&  Value() const { ASSERTE(HasValue(), "Attempting to get a value from an empty optional"); return *value_storage; }
		      V&  Value()       { ASSERTE(HasValue(), "Attempting to get a value from an empty optional"); return *value_storage; }

		V& TakeValue() { ASSERTE(HasValue(), "Attempting to take a value from an empty optional"); V& ret = *value_storage; value_storage = nullptr; return ret; }

		V ValueOr(V&& fallback) const &  { if (HasValue()) { return *Value(); } return fallback; }
		V ValueOr(V&& fallback) const && { if (HasValue()) { return std::move(*Value()); } return fallback; }

	private:
		V* value_storage;
	};

	//todo(vuko): Optional-creating utility functions
	//template<typename V> Optional WithValue(V&&);
	//template<typename V> Optional Valueless(V*);
	//template<typename V> Optional Valueless();
}

//todo(vuko): swap() impl for Optional
//void swap(optional<T>& other) {
// 1;0 | 0;1 -> set one, clear another
// 1;1 -> using std::swap; swap(this->internals, other.internals);
// specialize for references
//}
