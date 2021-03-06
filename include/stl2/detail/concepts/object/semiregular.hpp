// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_CONCEPTS_OBJECT_SEMIREGULAR_HPP
#define STL2_DETAIL_CONCEPTS_OBJECT_SEMIREGULAR_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object/assignable.hpp>
#include <stl2/detail/concepts/object/movable.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// CopyConstructible [concepts.lib.object.copyconstructible]
	//
	template <class T>
	concept bool CopyConstructible() {
		return MoveConstructible<T>() &&
			Constructible<T, const remove_cv_t<T>&>() &&
			ConvertibleTo<remove_cv_t<T>&, T>() &&
			ConvertibleTo<const remove_cv_t<T>&, T>() &&
			ConvertibleTo<const remove_cv_t<T>&&, T>();
	}

	namespace models {
		template <class>
		constexpr bool CopyConstructible = false;
		__stl2::CopyConstructible{T}
		constexpr bool CopyConstructible<T> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Copyable [concepts.lib.object.copyable]
	//
	template <class T>
	concept bool Copyable() {
		return CopyConstructible<T>() &&
			Movable<T>() && Assignable<T&, const T&>();
	}

	namespace models {
		template <class>
		constexpr bool Copyable = false;
		__stl2::Copyable{T}
		constexpr bool Copyable<T> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Semiregular [concepts.lib.object.semiregular]
	//
	template <class T>
	concept bool Semiregular() {
		return Copyable<T>() && DefaultConstructible<T>();
	}

	namespace models {
		template <class>
		constexpr bool Semiregular = false;
		__stl2::Semiregular{T}
		constexpr bool Semiregular<T> = true;
	}
} STL2_CLOSE_NAMESPACE

#endif
