#pragma once
#include <type_traits>

namespace proccessor::cncpt {

	template <typename T, typename ...Ts>
	struct has_now {

		template <typename U, typename ...Us>
		static constexpr
			decltype(std::declval<U>().now(Us()...), bool())
			test_now(int()) { return true; };

		template <typename U>
		static constexpr bool test_now(...) { return false; };

		static constexpr bool value = test_now<T, Ts...>(int());
	};

	template<typename T, typename ...Ts>
	inline constexpr bool has_now_v = has_now<T, Ts...>::value;

	template <typename T>
	struct has_io {

		template <typename U>
		static constexpr
			decltype(std::declval<U>().io(), bool())
			test_io(int()) { return true; };

		template <typename U>
		static constexpr bool test_io(...) { return false; };

		static constexpr bool value = test_io<T>(int());
	};

	template<typename T>
	inline constexpr bool has_io_v = has_io<T>::value;
}