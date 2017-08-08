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
			~OptionalBase() { if (this->HasValue()) { this->Value().~V(); } }

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
			bool HasValue() const { return this->initialized; }
			operator bool() const { return this->HasValue(); }

		public:
			const V&  Value() const& { ASSERTE(this->HasValue(), "Attempting to get a value from an empty optional"); return this->value_storage; }
			      V&  Value() &      { ASSERTE(this->HasValue(), "Attempting to get a value from an empty optional"); return this->value_storage; }
			      V&& Value() &&     { ASSERTE(this->HasValue(), "Attempting to get a value from an empty optional"); return std::move(this->value_storage); }

		public:
			V TakeValue() {
				ASSERTE(this->HasValue(), "Attempting to take a value from an empty optional");

				V ret = std::move(this->value_storage);

				this->initialized = false;
				this->value_storage.~V();

				return ret;
			}

		public:
			V ValueOr(V&& fallback) const &  { if (this->HasValue()) { return this->Value(); } return fallback; } //todo(vuko): do not copy when possible
			V ValueOr(V&& fallback) const && { if (this->HasValue()) { return std::move(this->Value()); } return fallback; }

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
		bool HasValue() const  { return static_cast<bool>(this->value_storage); }
		operator bool() const { return this->HasValue(); }

	public:
		const V&  Value() const& { ASSERTE(this->HasValue(), "Attempting to get a value from an empty optional"); return *this->value_storage; }
		      V&  Value() &      { ASSERTE(this->HasValue(), "Attempting to get a value from an empty optional"); return *this->value_storage; }

	public:
		V& TakeValue() { ASSERTE(this->HasValue(), "Attempting to take a value from an empty optional"); V& ret = *this->Value(); this->value_storage = nullptr; return ret; }

	public:
		V ValueOr(V&& fallback) const &  { if (this->HasValue()) { return *this->Value(); } return fallback; }
		V ValueOr(V&& fallback) const && { if (this->HasValue()) { return std::move(*this->Value()); } return fallback; }

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
