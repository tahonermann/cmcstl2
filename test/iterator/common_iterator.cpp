// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2015
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <algorithm>
#include <numeric>
#include <stl2/iterator.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

namespace ranges = __stl2;
namespace models = ranges::models;

namespace {
	struct silly_arrow_cursor {
		int read() const;
		void next();
		int arrow() const { return 42; }
	};

	int forty_two = 42;
	struct lvalue_iterator {
		using iterator_category = ranges::input_iterator_tag;
		using difference_type = int;
		using value_type = int;
		int& operator*() const { return forty_two; }
		lvalue_iterator& operator++() &;
		lvalue_iterator operator++(int) &;
	};
	struct xvalue_iterator : lvalue_iterator {
		int&& operator*() const { return std::move(forty_two); }
		xvalue_iterator& operator++() &;
		xvalue_iterator operator++(int) &;
	};

	struct proxy_cursor {
		int read() const { return 42; }
		void next();
	};

	void test_operator_arrow() {
		// I is a pointer type
		{
			int i = 42;
			auto ci = ranges::common_iterator<int*, ranges::unreachable>{&i};
			static_assert(models::Same<int*, decltype(ci.operator->())>);
			CHECK(ci.operator->() == &i);
		}
		// the expression i.operator->() is well-formed
		{
			using I = ranges::basic_iterator<silly_arrow_cursor>;
			auto ci = ranges::common_iterator<I, ranges::unreachable>{};
			static_assert(models::Same<I, decltype(ci.operator->())>);
			CHECK(ci.operator->().operator->() == 42);
		}
		// the expression *i is a glvalue [lvalue case]
		{
			auto ci = ranges::common_iterator<lvalue_iterator, ranges::unreachable>{};
			static_assert(models::Same<int*, decltype(ci.operator->())>);
			CHECK(ci.operator->() == &forty_two);
		}
		// the expression *i is a glvalue [xvalue case]
		{
			auto ci = ranges::common_iterator<xvalue_iterator, ranges::unreachable>{};
			static_assert(models::Same<int*, decltype(ci.operator->())>);
			CHECK(ci.operator->() == &forty_two);
		}
		// Otherwise, returns a proxy object
		{
			using I = ranges::basic_iterator<proxy_cursor>;
			auto ci = ranges::common_iterator<I, ranges::unreachable>{};
			using A = decltype(ci.operator->());
			static_assert(std::is_class<A>::value);
			static_assert(!std::is_same<I, A>::value);
			CHECK(*ci.operator->().operator->() == 42);
		}
	}
}

int main() {
	{
		namespace models = ::__stl2::models;
		static_assert(
			models::ForwardIterator<
				__stl2::common_iterator<
					bidirectional_iterator<const char *>,
					sentinel<const char *>>>);
		static_assert(
			!models::BidirectionalIterator<
				__stl2::common_iterator<
					bidirectional_iterator<const char *>,
					sentinel<const char *>>>);
		static_assert(
			std::is_same<
				__stl2::common_reference<
					__stl2::common_iterator<
						bidirectional_iterator<const char *>,
						sentinel<const char *>
					>&,
					__stl2::common_iterator<
						bidirectional_iterator<const char *>,
						sentinel<const char *>
					>
				>::type,
				__stl2::common_iterator<
					bidirectional_iterator<const char *>,
					sentinel<const char *>
				>
			>::value);
		// Sized iterator range tests
		static_assert(
			!models::SizedSentinel<
				__stl2::common_iterator<
					forward_iterator<int*>,
					sentinel<int*, true> >,
				__stl2::common_iterator<
					forward_iterator<int*>,
					sentinel<int*, true> > >);
		static_assert(
			models::SizedSentinel<
				__stl2::common_iterator<
					random_access_iterator<int*>,
					sentinel<int*, true> >,
				__stl2::common_iterator<
					random_access_iterator<int*>,
					sentinel<int*, true> > >);
		static_assert(
			!models::SizedSentinel<
				__stl2::common_iterator<
					random_access_iterator<int*>,
					sentinel<int*, false> >,
				__stl2::common_iterator<
					random_access_iterator<int*>,
					sentinel<int*, false> > >);
	}
	{
		int rgi[] {0,1,2,3,4,5,6,7,8,9};
		using CI = __stl2::common_iterator<
			random_access_iterator<int*>,
			sentinel<int*>>;
		CI first{random_access_iterator<int*>{rgi}};
		CI last{sentinel<int*>{rgi+10}};
		CHECK(std::accumulate(first, last, 0, std::plus<int>{}) == 45);
	}

	test_operator_arrow();

	return test_result();
}
