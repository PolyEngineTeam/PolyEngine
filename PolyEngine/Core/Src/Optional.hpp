#pragma once

#include "Defines.hpp"

namespace Poly {
	template<typename V>
	class Optional {
	public:
		using ValueType = V;
	public:
		Optional() : initialized(false), dummy{} {}
		Optional(const V&  value) : initialized(true), value_storage(value) {}
		Optional(      V&& value) : initialized(true), value_storage(std::move(value)) {}
		~Optional() { if (this->HasValue()) { this->Value().~V(); } }

	public:
		Optional(Optional&& other) {
			if (other.HasValue()) {
				this->initialized = true;
				::new(&this->Value()) V(std::move(other.Value()));

				other.Value().~V();
				other.initialized = false;
			} else {
				this->initialized = false;
			}
		}
		Optional& operator=(Optional&& other) {
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

	//todo(vuko): this is pretty much copy-pasted from the main implementation (with the copying constructor removed); can we somehow make an alias?
	//always moves in the contents
	template<typename V>
	class Optional<V&&> {
	public:
		using ValueType = V&&;
	public:
		Optional() : initialized(false), dummy{} {}
		Optional(      V&& value) : initialized(true), value_storage(std::move(value)) {}
		~Optional() { if (this->HasValue()) { this->Value().~V(); } }

	public:
		Optional(Optional&& other) {
			if (other.HasValue()) {
				this->initialized = true;
				::new(&this->Value()) V(std::move(other.Value()));

				other.Value().~V();
				other.initialized = false;
			} else {
				this->initialized = false;
			}
		}
		Optional& operator=(Optional&& other) {
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
		V ValueOr(V&& fallback) const &  { if (this->HasValue()) { return this->Value(); } return fallback; }
		V ValueOr(V&& fallback) const && { if (this->HasValue()) { return std::move(this->Value()); } return fallback; }

	private:
		bool initialized;
		union {
			V value_storage;
			char dummy;
		};
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
