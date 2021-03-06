// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <stl2/detail/algorithm/partial_sort.hpp>
#include <cassert>
#include <memory>
#include <random>
#include <algorithm>
#include <vector>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

namespace stl2 = __stl2;

namespace { std::mt19937 gen; }

struct indirect_less
{
	template <class P>
	bool operator()(const P& x, const P& y) const { return *x < *y; }
};

void
test_larger_sorts(int N, int M)
{
	assert(N > 0);
	assert(M >= 0 && M <= N);
	int* array = new int[N];
	for(int i = 0; i < N; ++i)
		array[i] = i;

	using I = random_access_iterator<int*>;
	using S = sentinel<int*>;

	std::shuffle(array, array+N, gen);
	int *res = stl2::partial_sort(array, array+M, array+N);
	CHECK(res == array+N);
	for(int i = 0; i < M; ++i)
		CHECK(array[i] == i);

	std::shuffle(array, array+N, gen);
	I res2 = stl2::partial_sort(I{array}, I{array+M}, S{array+N});
	CHECK(res2.base() == array+N);
	for(int i = 0; i < M; ++i)
		CHECK(array[i] == i);

	std::shuffle(array, array+N, gen);
	res = stl2::partial_sort(::as_lvalue(stl2::ext::make_range(array, array+N)), array+M);
	CHECK(res == array+N);
	for(int i = 0; i < M; ++i)
		CHECK(array[i] == i);

	std::shuffle(array, array+N, gen);
	res2 = stl2::partial_sort(::as_lvalue(stl2::ext::make_range(I{array}, S{array+N})), I{array+M});
	CHECK(res2.base() == array+N);
	for(int i = 0; i < M; ++i)
		CHECK(array[i] == i);

	std::shuffle(array, array+N, gen);
	auto res3 = stl2::partial_sort(stl2::ext::make_range(array, array+N), array+M);
	CHECK(res3.get_unsafe() == array+N);
	for(int i = 0; i < M; ++i)
		CHECK(array[i] == i);

	std::shuffle(array, array+N, gen);
	auto res4 = stl2::partial_sort(stl2::ext::make_range(I{array}, S{array+N}), I{array+M});
	CHECK(res4.get_unsafe().base() == array+N);
	for(int i = 0; i < M; ++i)
		CHECK(array[i] == i);

	std::shuffle(array, array+N, gen);
	res = stl2::partial_sort(array, array+M, array+N, std::greater<int>());
	CHECK(res == array+N);
	for(int i = 0; i < M; ++i)
		CHECK(array[i] == N-i-1);

	std::shuffle(array, array+N, gen);
	res2 = stl2::partial_sort(I{array}, I{array+M}, S{array+N}, std::greater<int>());
	CHECK(res2.base() == array+N);
	for(int i = 0; i < M; ++i)
		CHECK(array[i] == N-i-1);

	std::shuffle(array, array+N, gen);
	res = stl2::partial_sort(::as_lvalue(stl2::ext::make_range(array, array+N)), array+M, std::greater<int>());
	CHECK(res == array+N);
	for(int i = 0; i < M; ++i)
		CHECK(array[i] == N-i-1);

	std::shuffle(array, array+N, gen);
	res2 = stl2::partial_sort(::as_lvalue(stl2::ext::make_range(I{array}, S{array+N})), I{array+M}, std::greater<int>());
	CHECK(res2.base() == array+N);
	for(int i = 0; i < M; ++i)
		CHECK(array[i] == N-i-1);

	delete [] array;
}

void
test_larger_sorts(int N)
{
	test_larger_sorts(N, 0);
	test_larger_sorts(N, 1);
	test_larger_sorts(N, 2);
	test_larger_sorts(N, 3);
	test_larger_sorts(N, N/2-1);
	test_larger_sorts(N, N/2);
	test_larger_sorts(N, N/2+1);
	test_larger_sorts(N, N-2);
	test_larger_sorts(N, N-1);
	test_larger_sorts(N, N);
}

struct S
{
	int i, j;
};

int main()
{
	int i = 0;
	int * res = stl2::partial_sort(&i, &i, &i);
	CHECK(i == 0);
	CHECK(res == &i);
	test_larger_sorts(10);
	test_larger_sorts(256);
	test_larger_sorts(257);
	test_larger_sorts(499);
	test_larger_sorts(500);
	test_larger_sorts(997);
	test_larger_sorts(1000);
	test_larger_sorts(1009);

	// Check move-only types
	{
		std::vector<std::unique_ptr<int> > v(1000);
		for(int i = 0; (std::size_t)i < v.size(); ++i)
			v[i].reset(new int(v.size() - i - 1));
		stl2::partial_sort(v, v.begin() + v.size()/2, indirect_less());
		for(int i = 0; (std::size_t)i < v.size()/2; ++i)
			CHECK(*v[i] == i);
	}

	// Check projections
	{
		std::vector<S> v(1000, S{});
		for(int i = 0; (std::size_t)i < v.size(); ++i)
		{
			v[i].i = v.size() - i - 1;
			v[i].j = i;
		}
		stl2::partial_sort(v, v.begin() + v.size()/2, std::less<int>{}, &S::i);
		for(int i = 0; (std::size_t)i < v.size()/2; ++i)
		{
			CHECK(v[i].i == i);
			CHECK((std::size_t)v[i].j == v.size() - i - 1);
		}
	}

	return ::test_result();
}
