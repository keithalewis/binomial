// fms_binomial.h - Binomial distribution.
#pragma once
#include <cmath>
#include <iterator>
#include <limits>
#include <numeric>
#include "fms_iterable.h"

namespace fms {

	class binomial {
		size_t n;
		size_t k;
	public:
		binomial(size_t n, size_t k = 0)
			: n(n), k(k)
		{ }
		constexpr bool operator==(const binomial& b) const
		{
			return n == b.n && k == b.k;
		}
		binomial begin() const
		{
			return binomial(n, k);
		}
		binomial end() const
		{
			return binomial(n, n + 1);
		}
		constexpr operator bool() const
		{
			return k <= n;
		}
		// C(n, k)/2^n
		constexpr double operator()() const
		{
			return k == 0 || k == n ? 1 : binomial(n, k - 1)()*(n - k + 1)/(2*k);
		}
		// random walk 0, 1, ..., n
		constexpr double operator*() const
		{
			return k * 1. / n;
		}
		binomial& operator++()
		{
			if (operator bool()) {
				++k;
			}

			return *this;
		}
	};
#if 0
	template<class T>
	class iota {
		T t;
	public:
		using value_type = T;
		using iterator_category = std::forward_iterator_tag;
		
		constexpr iota(T t = 0)
			: t(t)
		{ }
		constexpr operator bool() const
		{
			return true;
		}
		constexpr bool operator==(const iota& i) const
		{
			return t == i.t;
		}
		constexpr T operator*() const
		{
			return t;
		}
		constexpr iota & operator++()
		{
			++t;

			return *this;
		}
	};
	/*
	template<class I>
	class take : public I {
		size_t n;
	public:
		take(size_t n, I i)
			: I(i), n(n)
		{ }
		constexpr operator bool() const
		{
			return I::operator bool() && n > 0;
		}
	};
#ifdef _DEBUG
	//static_assert(take(0, sum(iota<int>(0), 0)) == 0);
#endif // _DEBUG
	*/

	// integral f dm
	template<class F, class M>
	constexpr double integral(const F& f, M m)
	{
		return m ? f(*m)*m() + integral(f, ++m) : 0.;
	}

	class uniform {
		size_t n;
		size_t k;
public:
	constexpr uniform(size_t n = 1, size_t k = 0)
			: n(n), k(0)
		{ }
		constexpr double operator()() const
		{
			return 1./n;
		}
		constexpr operator bool() const
		{
			return k < n;
		}
		constexpr double operator*() const
		{
			return 1. * k / n;
		}
		constexpr uniform& operator++()
		{
			if (operator bool()) {
				++k;
			}

			return *this;
		}
#ifdef _DEBUG
		static constexpr void test()
		{
			{
				constexpr uniform u(2);
				static_assert(u);
				static_assert(*u == 0);
				constexpr auto id = [](double x) constexpr { return 1.; };
				constexpr auto Iu = integral(id, u);
				static_assert(Iu == 1.);
			}
			{
				constexpr uniform u(2);
				static_assert(u);
				static_assert(*u == 0);
				constexpr auto id = [](double x) constexpr { return x; };
				constexpr auto Iu = integral(id, u);
				static_assert(Iu == 0.25);
			}
			{
				constexpr uniform u(100);
				static_assert(u);
				static_assert(*u == 0);
				constexpr auto id = [](double x) constexpr { return x; };
				constexpr auto Iu = integral(id, u);
				constexpr double err = Iu - 0.5 + 0.5/100;
				static_assert(0 < err);
				static_assert(err < std::numeric_limits<double>::epsilon());
			}
		}
#endif // _DEBUG
	};
#endif // 0
}
