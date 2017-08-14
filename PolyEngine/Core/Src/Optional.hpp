#pragma once

#include "Defines.hpp"

namespace Poly {
	namespace Impl {

		template<typename V>
		class OptionalBase {
		public:
			using ValueType = V;
		public:
			OptionalBase() : initialized(false), dummy{} {}
			OptionalBase(const V&  value) : initialized(true), value_storage(value) {}
			OptionalBase(      V&& value) : initialized(true), value_storage(std::move(value)) {}
			~OptionalBase() { if (HasValue()) { Value().~V(); } }

		public:
			OptionalBase(OptionalBase&& other) {
				if (other.HasValue()) {
					this->initialized = true;
					::new(&this->Value()) V(std::move(other.Value()));

					other.Value().~V();
					other.initialized = false;
				} else {
					this->initialized = false;
				}
			}
			OptionalBase& operator=(OptionalBase&& other) {
				if (other.HasValue()) {
					this->initialized = true;
					this->Value() = std::move(other.Value());

					other.Value().~V();
					other.initialized = false;
				} else if (this->HasValue()) {
					this->Value().~V();
					this->initialized = false;
				}
				return *this;
			}

		public:
			bool HasValue() const { return initialized; }
			operator bool() const { return HasValue(); }

		public:
			const V&  Value() const& { ASSERTE(HasValue(), "Attempting to get a value from an empty optional"); return value_storage; }
			      V&  Value() &      { ASSERTE(HasValue(), "Attempting to get a value from an empty optional"); return value_storage; }
			      V&& Value() &&     { ASSERTE(HasValue(), "Attempting to get a value from an empty optional"); return std::move(value_storage); }

		public:
			V TakeValue() {
				ASSERTE(HasValue(), "Attempting to take a value from an empty optional");

				V ret = std::move(value_storage);

				initialized = false;
				value_storage.~V();

				return ret;
			}

		public:
			V ValueOr(V&& fallback) const &  { if (HasValue()) { return Value(); } return fallback; } //todo(vuko): do not copy when possible
			V ValueOr(V&& fallback) const && { if (HasValue()) { return std::move(Value()); } return fallback; }

		private:
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
		using Impl::OptionalBase<V>::OptionalBase;
	};

	template<typename V>
	class Optional<V&&> : public Impl::OptionalBase<V> {
	public:
		using Impl::OptionalBase<V>::OptionalBase;
		Optional(const V&) = delete; //note(vuko): prevent copying
	};

	template<typename V>
	class Optional<V&> {
	public:
		using ValueType = V&;
	public:
		Optional() : value_storage(nullptr) {}
		Optional(V& value) : value_storage(&value) {}
		~Optional() = default;

	public:
		Optional(Optional&& other) { this->value_storage = other.value_storage; other.value_storage = nullptr; }
		Optional& operator=(Optional&& other) { this->value_storage = other.value_storage; other.value_storage = nullptr; return *this; }

	public:
		bool HasValue() const  { return static_cast<bool>(value_storage); }
		operator bool() const { return HasValue(); }

	public:
		const V&  Value() const& { ASSERTE(HasValue(), "Attempting to get a value from an empty optional"); return *value_storage; }
		      V&  Value() &      { ASSERTE(HasValue(), "Attempting to get a value from an empty optional"); return *value_storage; }

	public:
		V& TakeValue() { ASSERTE(HasValue(), "Attempting to take a value from an empty optional"); V& ret = *value_storage; value_storage = nullptr; return ret; }

	public:
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