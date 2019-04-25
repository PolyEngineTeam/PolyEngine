#pragma once

#include <Defines.hpp>

namespace Poly {
    template <typename IT>
    class Range final : public BaseObjectLiteralType<>
	{
	public:
        using Iterator_t = IT;

        Range(Iterator_t begin, Iterator_t end)
            : BeginIt(std::move(begin)), EndIt(std::move(end))
        {}

        template <typename C>
        Range(C& collection)
            : Range(begin(collection), end(collection))
        {}

        template <typename C>
        Range(const C& collection)
            : Range(cbegin(collection), cend(collection))
        {}

        Iterator_t Begin() { return BeginIt; }
        Iterator_t End() { return EndIt; }
    private:
        Iterator_t BeginIt;
        Iterator_t EndIt;
	};

    template <typename IT> typename Poly::Range<IT>::Iterator_t begin(Poly::Range<IT>& rhs) { return rhs.Begin(); }
	template <typename IT> typename Poly::Range<IT>::Iterator_t end(Poly::Range<IT>& rhs) { return rhs.End(); }

    template <typename C> auto MakeRange(C& container) { return Range<decltype( begin(std::declval<C>()) )>(container); }
    template <typename C> auto MakeRange(const C& container) { return Range<decltype( cbegin(std::declval<C>()) )>(container); }
}