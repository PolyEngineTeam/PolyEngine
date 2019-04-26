#pragma once

#include <Defines.hpp>

namespace Poly {

	/// Immutable iterable class that represents a range of objects
	/// starting at the begin iterator and finishing at the end iterator.
    template <typename IT>
    class Range final : public BaseObjectLiteralType<>
	{
	public:
        using Iterator_t = IT;

        /// @brief Basic range constructor. Takes two iterators that form a range.
        /// @param[in] begin Iterator pointing to the begining of the range
        /// @param[in] end Iterator pointing to the end of the range
        Range(Iterator_t begin, Iterator_t end)
            : BeginIt(std::move(begin)), EndIt(std::move(end))
        {}

		/// @brief Returns const reference to the iterator pointing to the begining of the range.
		/// @return Iterator pointing to the begining of the range.
        const Iterator_t& Begin() const noexcept { return BeginIt; }

		/// @brief Returns const reference to the iterator pointing to the end of the range.
		/// @return Iterator pointing to the end of the range.
        const Iterator_t& End() const noexcept { return EndIt; }

		inline bool operator==(const Range& rhs) const { return BeginIt == rhs.BeginIt && EndIt == rhs.EndIt; }
		inline bool operator!=(const Range& rhs) const { return !(*this == rhs); }
    private:
        const Iterator_t BeginIt;
        const Iterator_t EndIt;
	};

    template <typename IT> const typename Poly::Range<IT>::Iterator_t& begin(const Poly::Range<IT>& rhs) noexcept { return rhs.Begin(); }
	template <typename IT> const typename Poly::Range<IT>::Iterator_t& end(const Poly::Range<IT>& rhs) noexcept { return rhs.End(); }

	/// @brief Create range from iterable object.
	/// @param[in] iterable Reference to the iterable object
	/// @return Range representing whole span of the iterable object.
	/// @note Calls begin() and end() methods of the iterable object.
	/// @note Constness of the iterator is dictated by the constness of the iterable object.
    template <typename T> auto MakeRange(T& iterable) { return Range(iterable.begin(), iterable.end()); }

	/// @brief Creates immutable range from iterable object.
	/// @param[in] iterable Const reference to the iterable object
	/// @return Range representing whole span of the iterable object.
	/// @note Calls cbegin() and cend() methods of the iterable object.
    template <typename T> auto MakeRangeConst(const T& iterable) { return Range(iterable.cbegin(), iterable.cend()); }

	/// @brief Creates reverse range from iterable object.
	/// @param[in] iterable Reference to the iterable object
	/// @return Range representing whole span of the iterable object.
	/// @note Calls rbegin() and rend() methods of the iterable object.
	/// @note Constness of the iterator is dictated by the constness of the iterable object.
    template <typename T> auto MakeRangeReversed(T& iterable) { return Range(iterable.rbegin(), iterable.rend()); }

	/// @brief Creates immutable reverse range from iterable object.
	/// @param[in] iterable Const reference to the iterable object
	/// @return Range representing whole span of the iterable object.
	/// @note Calls crbegin() and crend() methods of the iterable object.
    template <typename T> auto MakeRangeReversedConst(const T& iterable) { return Range(iterable.crbegin(), iterable.crend()); }
    
}