// fms_iterable.h - Iterators with `operator bool() const` to detect end.
#pragma once
#ifdef _DEBUG
#include <cassert>
#include <vector>
#endif // _DEBUG
#include <algorithm>
#include <iterator>
#include <numeric>
#include <stdexcept>

#if 0
namespace fms::iterable {

	//
	// Generic iterable routines.
	//

	// Add elements of iterable.
	template<class I>
	constexpr auto accumulate(I i, auto s = 0) -> decltype(s + *i)
	{
		return std::accumulate(i.begin(), i.end(), s);
	}


	template<class I, class P>
	constexpr bool any(I i, P p = [](auto i) { return *i; })
	{
		return std::any_of(i.begin(), i.end(), p);
	}
	template<class I, class P>
	constexpr bool all(I i, P p = [](auto i) { return *i; })
	{
		return std::all_of(i.begin(), i.end(), p);
	}
	template<class I, class P>
	constexpr bool none(I i, P p = [](auto i) { return *i; })
	{
		return std::none_of(i.begin(), i.end(), p);
	}
#ifdef _DEBUG
	inline int logical_test()
	{
		{
			std::vector i{ 1,2,3 };
			assert(any(i));
			//assert(all(i));
			//assert(!none(i));
		}

		return 0;
	}
#endif // _DEBUG

	// Drop at most n elements from iterable.
	template<class I>
	constexpr I drop(size_t n, I i)
	{
		while (i && n--) {
			++i;
		}

		return i;
	}

	// Integral of function f at points in iterable.
	// integral(f, i, x) = integral(f, i, x) + f(x)
	template<class F, class I>
	constexpr auto integral(F f, I i, auto x = 0) -> decltype(f(x))
	{
		return i ? (integral(f, ++i, x) + f(x), x = *i) : f(x);
	}

	// Number of elements in iterable.
	// size(i, size(j)) = size(i) + (size(j))
	template<class I>
	constexpr size_t size(I i, size_t n = 0)
	{
		while (i) {
			++i;
			++n;
		}
		
		return n;
	}

	// Unsafe iterable from pointer.
	template<class T>
	class ptr {
		T* p;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using reference_type = T&;
		using pointer = T*;
		using difference_type = std::ptrdiff_t;

		constexpr ptr(T* p = nullptr) noexcept
			: p(p)
		{ }
		
		constexpr bool operator==(const ptr& i) const = default;
		
		constexpr ptr begin() const
		{
			return ptr(p);
		}
		constexpr ptr end() const
		{
			throw std::runtime_error(__FUNCTION__ ": unsafe");
		}
		
		constexpr operator bool() const
		{
			return p != nullptr;
		}
		constexpr value_type operator*() const
		{
			return *p;
		}
		constexpr ptr& operator++()
		{
			operator bool() && ++p;

			return *this;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				int i[] = { 1,2,3 };
				auto p = ptr(i);
				assert(p);
				auto p2{ p };
				assert(p2);
				assert(p == p2);
				p = p2;
				assert(!(p2 != p));

				assert(p == p.begin());
				assert(p != p.end());
				
				assert(*p == 1);
				++p;
				assert(p);
				assert(*p == 2);
				assert(*++p == 3);
			}

			return 0;
		}
#endif // _DEBUG
	};

	template<class I, class T = typename std::iterator_traits<I>::value_type>
	class counted {
		I i;
		size_t n;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using reference_type = T&;
		using pointer = T*;
		//using difference_type = I::difference_type;

		constexpr counted(I i, size_t n) noexcept
			: i(i), n(n)
		{ }
		template<size_t N>
		constexpr counted(T(&i)[N]) noexcept
			: i(i), n(N)
		{ }
		
		constexpr bool operator==(const counted& i) const = default;

		constexpr counted begin() const
		{
			return counted(i, n);
		}
		constexpr counted end() const
		{
			return counted(i + n, 0);
		}
		
		constexpr operator bool() const
		{
			return n > 0;
		}
		constexpr value_type operator*() const
		{
			return *i;
		}
		constexpr counted& operator++()
		{
			if (operator bool()) {
				++i;
				--n;
			}

			return *this;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				int i[] = { 1,2,3 };
				auto c = counted(i);
				assert(c);
				auto c2{ c };
				assert(c2 == c);
				c = c2;
				assert(!(c != c2));

				assert(*c == 1);
				++c;
				assert(c);
				assert(*c == 2);
				assert(*++c == 3);
				++c;
				assert(!c);
				++c;
				assert(!c);
			}
			{
				int i[] = { 1,2,3 };
				int s = 0;
				for (auto si : counted(i)) {
					s += si;
				}
				assert(s == accumulate(counted(i), 0));
				assert(size(counted(i)) == 3);
			}

			return 0;
		}
#endif // _DEBUG
	};
	template<class I>
	constexpr counted<I> take(size_t n, I i)
	{
		return counted<I>(i, n);
	}

	template<class I, class T = typename std::iterator_traits<I>::value_type>
	class span {
		I b;
		I e;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using reference_type = T&;
		using pointer = T*;
		using difference_type = std::iterator_traits<I>::difference_type;

		constexpr span(I b, I e) noexcept
			: b(b), e(e)
		{ }

		constexpr bool operator==(const span& s) const = default;

		constexpr span begin() const
		{
			return span(b, e);
		}
		constexpr span end() const
		{
			return span(e, e);
		}
		
		constexpr operator bool() const
		{
			return b != e;
		}
		value_type operator*() const
		{
			return *b;
		}
		constexpr span& operator++()
		{
			if (operator bool()) {
				++b;
			}

			return *this;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				int i[] = { 1,2,3 };
				auto s = span(i, i + 3);
				auto s2{ s };
				assert(s2 == s);
				s = s2;
				assert(!(s != s2));

				assert(s);
				assert(*s == 1);
				assert(*++s == 2);
				assert(*++s == 3);
				assert(!++s);
			}

			return 0;
		}
#endif // _DEBUG
	};

}
#endif // 0