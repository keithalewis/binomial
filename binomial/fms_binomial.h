// fms_binomial.h - Binomial distribution.
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <cmath>
#include <compare>
#include <functional>
#include <iterator>
#include <limits>
#include <numeric>
#define _USE_MATH_DEFINES
#include <math.h>
//#include "fms_iterable.h"

namespace fms::binomial {

	// P(V_n = k) using (2 V_n - n)/sqrt(n) ~ N(0,1)
	inline double P(long n, long k)
	{
		double x = (2 * k - n) / sqrt(n);
		// !!! adjust so sum 0 <= k <= n is exactly 1

		return exp(-x * x / 2) * 2 / sqrt(2 * M_PI * n);
	}

	// E[f(V_N) | V_n = k]
	template<class F>
	inline auto E(long N, const F& f)
	{
		return [N, &f](long n, long k) {
			double e = 0;

			for (long i = k; i <= k + N - n; ++i) {
				e += f(i) * P(N, i);
			}

			return e;
			};
	}
	struct atom {
		long n, k;
		constexpr atom(long n, long k)
			: n(n), k(k)
		{ }

		constexpr auto operator<=>(const atom & a) const = default;

		constexpr operator double() const
		{
			return k;
		}
		double operator()() const
		{
			return P(n, k);
		}
		atom& operator++()
		{
			if (k <= n) {
				++k;
			}

			return *this;
		}
	};
	template<class F>
	inline auto E(const std::function<bool(long,long)>& tau, const F& f)
	{
		return [&tau, &f](long n, long k) {
			if (tau(n, k)) {
				return f(n, k);
			}
			else {
				return (E(tau, f)(n + 1, k) + E(tau, f)(n + 1, k + 1)) / 2;
			}
		};
	}
	// W_n = k, 0 <= k <= n.
	struct W {
		long n, k;
		W(long n, long k)
			: n(n), k(k)
		{ }
		constexpr double operator()(long n, long k) const
		{
			return 2 * k - n;
		}
	};

#ifdef _DEBUG
	inline int E_test()
	{
		{
			constexpr double eps = std::numeric_limits<double>::epsilon();
			const auto id = [](double x) { return 2 * x - 10; };
			const auto Ef = E(10, id);
			long n = 5;
			for (long k = 0; k <= n; ++k) {
				double x = Ef(n, k);
				double x_ = Ef(n, n - k);
				double dx = x + x_;
				assert(fabs(dx) < eps);
			}
		}

		return 0;
	}
#endif // _DEBUG

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
		constexpr iota& operator++()
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
		return m ? f(*m) * m() + integral(f, ++m) : 0.;
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
			return 1. / n;
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
				constexpr double err = Iu - 0.5 + 0.5 / 100;
				static_assert(0 < err);
				static_assert(err < std::numeric_limits<double>::epsilon());
			}
		}
#endif // _DEBUG
	};
#endif // 0
}
