#pragma once
#include <numeric>
#include <tuple>
#include <utility>
#include<limits>

namespace proccessor::utilities {

	template<typename... Args> void whatis();

	/* Naive is_same */

	template <typename A, typename B>
	struct is_same {
		static constexpr bool value = false;
	};

	template <typename A>
	struct is_same<A, A> {
		static constexpr bool value = true;
	};

	template<typename A, typename B>
	inline constexpr bool is_same_v = is_same<A, B>::value;

	/* Naive min/max, min_element / max_element (for types) */

	template <typename A, typename B>
	using smaller_type_t = std::conditional_t < sizeof(A) < sizeof(B), A, B > ;

	template <typename A, typename B>
	using larger_type_t = std::conditional_t < sizeof(A) < sizeof(B), B, A > ;

	template <typename ...>
	struct smaller_element_type;

	template <typename T>
	struct smaller_element_type<T> { using type = T; };

	template <typename A, typename B, typename ...Rests>
	struct smaller_element_type<A, B, Rests...> {
		using type = smaller_type_t<A, typename smaller_element_type<B, Rests...>::type>;
	};

	template <typename... Ts>
	using smaller_element_type_t = typename smaller_element_type<Ts...>::type;

	template <typename ...> struct larger_element_type;

	template <typename T>
	struct larger_element_type<T> { using type = T; };

	template <typename A, typename B, typename ...Rests>
	struct larger_element_type<A, B, Rests...> {
		using type = larger_type_t<A, typename larger_element_type<B, Rests...>::type>;
	};

	template <typename... Ts>
	using larger_element_type_t = typename larger_element_type<Ts...>::type;

	/* Naive min / max element searh based on loop and recursion */

	template <typename T, T ... vals>
	constexpr T vmax_element(std::integer_sequence<T, vals...> = std::integer_sequence<T, vals...>())
	{
		T arr[sizeof...(vals)]{ vals... },
			max = 0;
		for (size_t i = 0; i != sizeof...(vals); ++i)
			max = arr[i] > max ? max = arr[i] : max;
		return max;
	};

	template<typename T>
	T&& vmin_element(T&& val)
	{
		return std::forward<T>(val);
	};

	template<typename T0, typename T1, typename... Ts>
	auto vmin_element(T0&& val1, T1&& val2, Ts&&... vs)
	{
		return (val1 < val2) ?
			vmin_element(val1, std::forward<Ts>(vs)...) :
			vmin_element(val2, std::forward<Ts>(vs)...);
	}

	/* Abstract Select Pattern for an arbitrary search 	for an argument of a greedy algorithm */

	template <
		template <
		typename xx_Left
		, typename xx_Right
		>
		typename x_Predicate
		, typename... x_Types
	>
		class
		t_SelectImpl;

	template
		<
		template
		<
		typename xx_Left
		, typename xx_Right
		>
		typename x_Predicate
		, typename x_First
		, typename... x_Rest
		>
		class
		t_SelectImpl<x_Predicate, x_First, x_Rest...> final
	{
	public: using
		t_Type = typename x_Predicate<x_First, typename t_SelectImpl<x_Predicate, x_Rest...>::t_Type>::t_Type;
	};

	template
		<
		template
		<
		typename xx_Left
		, typename xx_Right
		>
		typename x_Predicate
		, typename x_Left
		, typename x_Right
		>
		class
		t_SelectImpl<x_Predicate, x_Left, x_Right> final
	{
	public: using
		t_Type = typename x_Predicate<x_Left, x_Right>::t_Type;
	};

	template
		<
		template
		<
		typename xx_Left
		, typename xx_Right
		>
		typename x_Predicate
		, typename... x_Types
		>
		using
		t_Select = typename t_SelectImpl<x_Predicate, x_Types...>::t_Type;

	template
		<
		typename x_Left
		, typename x_Right
		>
		class
		t_MinPredicate final
	{
	public: using
		t_Type = ::std::conditional_t<(sizeof(x_Left) < sizeof(x_Right)), x_Left, x_Right>;
	};



	/* Naive Concepts using void() or void_t */

	template <typename...>
	using void_t = void;

	template <typename T, typename = void>
	struct has_now : std::false_type {};

	template <typename T>
	struct has_now <T, decltype (T().insert(), void())> : std::true_type {};

	template <typename T, typename = void>
	struct has_maybe : std::false_type {};

	template <typename T>
	struct has_maybe <T, void_t <decltype (T().maybe())> > : std::true_type {};

	/* Sort  */
	// swap types at index i and index j in the template argument tuple
	template <std::size_t i, std::size_t j, class Tuple>
	class tuple_element_swap
	{
		template <class IndexSequence>
		struct tuple_element_swap_impl;

		template <std::size_t... indices>
		struct tuple_element_swap_impl<std::index_sequence<indices...>>
		{
			using type = std::tuple
				<
				std::tuple_element_t
				<
				indices != i && indices != j ? indices : indices == i ? j : i, Tuple
				>...
				>;
		};

	public:
		using type = typename tuple_element_swap_impl
			<
			std::make_index_sequence<std::tuple_size<Tuple>::value>
			>::type;
	};

	// selection sort template argument tuple's variadic template's types
	template <template <class, class> class Comparator, class Tuple>
	class tuple_selection_sort
	{
		// selection sort's "loop"
		template <std::size_t i, std::size_t j, std::size_t tuple_size, class LoopTuple>
		struct tuple_selection_sort_impl
		{
			// this is done until we have compared every element in the type list
			using tuple_type = std::conditional_t
				<
				Comparator
				<
				std::tuple_element_t<i, LoopTuple>,
				std::tuple_element_t<j, LoopTuple>
				>::value,
				typename tuple_element_swap<i, j, LoopTuple>::type, // true: swap(i, j)
				LoopTuple                                           // false: do nothing
				>;

			using type = typename tuple_selection_sort_impl // recurse until j == tuple_size
				<
				i, j + 1, tuple_size, tuple_type // using the modified tuple
				>::type;
		};

		template <std::size_t i, std::size_t tuple_size, class LoopTuple>
		struct tuple_selection_sort_impl<i, tuple_size, tuple_size, LoopTuple>
		{
			// once j == tuple_size, we increment i and start j at i + 1 and recurse
			using type = typename tuple_selection_sort_impl
				<
				i + 1, i + 2, tuple_size, LoopTuple
				>::type;
		};

		template <std::size_t j, std::size_t tuple_size, class LoopTuple>
		struct tuple_selection_sort_impl<tuple_size, j, tuple_size, LoopTuple>
		{
			// once i == tuple_size, we know that every element has been compared
			using type = LoopTuple;
		};

	public:
		using type = typename tuple_selection_sort_impl
			<
			0, 1, std::tuple_size<Tuple>::value, Tuple
			>::type;
	};

	template <class T, class U>
	struct descending
		: std::conditional_t<(sizeof(U) > sizeof(T)), std::true_type, std::false_type>
	{};

	template <class T, class U>
	struct ascending
		: std::conditional_t<(sizeof(U) < sizeof(T)), std::true_type, std::false_type>
	{};

	/* Tuple pop_front method */

	template<typename Tuple>
	constexpr auto pop_front(Tuple tuple) {
		static_assert(std::tuple_size<Tuple>::value > 0, "Cannot pop from an empty tuple");
		return std::apply(
			[](auto, auto... rest) { return std::make_tuple(rest...); },
			tuple);
	}

	/* Get yuple element by Index*/
	template< std::size_t I, class T >
	struct tuple_element;

	// recursive case
	template< std::size_t I, class Head, class... Tail >
	struct tuple_element<I, std::tuple<Head, Tail...>>
		: std::tuple_element<I - 1, std::tuple<Tail...>> { };

	// base case
	template< class Head, class... Tail >
	struct tuple_element<0, std::tuple<Head, Tail...>> {
		using type = Head;
	};

	/* Get tuple type */
	template <int N, typename... Ts>
	struct get_tuple_type;

	template <int N, typename T, typename... Ts>
	struct get_tuple_type<N, std::tuple<T, Ts...>>
	{
		using type = typename get_tuple_type<N - 1, std::tuple<Ts...>>::type;
	};

	template <typename T, typename... Ts>
	struct get_tuple_type<0, std::tuple<T, Ts...>>
	{
		using type = T;
	};

	template <class... Args>
	struct type_list
	{
		template <std::size_t N>
		using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
	};

	/* Constexpr for // constexpr for_each */
	template <typename Integer, Integer ...I, typename F>
	constexpr void constexpr_for_each(std::integer_sequence<Integer, I...>, F&& func)
	{
		(func(std::integral_constant<Integer, I>{}), ...);
	}

	template <auto N, typename F>
	constexpr void constexpr_for(F&& func)
	{
		if constexpr (N > 0)
			constexpr_for_each(std::make_integer_sequence<decltype(N), N>{}, std::forward<F>(func));
	}

	/* contineous cast */

	// Чем меньше число, тем "лучше" тип.
	template <typename T>
	inline constexpr std::size_t type_rank = sizeof(T) * 2 + std::is_unsigned_v<T>;

	template <auto> struct value_tag {};

	// Возвращает тип из списка по его "рангу".
	// `R` = `value_tag<ранг>`.
	template <typename R, typename ...P> struct get_type_by_rank {};

	template <typename T, typename ...P>
	struct get_type_by_rank<value_tag<type_rank<T>>, T, P...> { using type = T; };

	template <typename R, typename T, typename ...P>
	struct get_type_by_rank<R, T, P...>	{
		using type = typename get_type_by_rank<R, P...>::type;
	};

	// Проверяет, что `X` представимо типом `T`.
	template <auto X, typename T>
	inline constexpr bool is_representable = [] {
		static_assert(std::is_integral_v<decltype(X)> && std::is_integral_v<T>);
		return !std::is_signed_v<T> && X < 0 ? false : X == T(X);
	}();

	// Кастует `X` к минимальному типу из `P...`.
	template <auto X, typename ...P>
	inline constexpr auto cast_to_smallest = [] {
		constexpr std::size_t min_rank = [] {
			std::size_t min_rank = -1;
			([&] {
				if (type_rank<P> < min_rank && is_representable<X, P>)
					min_rank = type_rank<P>;
				}(), ...);
			return min_rank;
		}();
		return typename get_type_by_rank<value_tag<min_rank>, P...>::type(X);
	}();

	//template <class I, typename ...Ts> struct contineous_cast {};

	template <class I, typename ...Ts> struct contineous_cast_S {
		using type = decltype(cast_to_smallest<I::value, Ts...>);
	};

	template <class I, class ...Ts>
	auto contineous_cast() {
		using optimus = typename contineous_cast_S<I, Ts...>::type;
		return static_cast<optimus>(I::value);
	}

	/* Check is save cast*/
	template <typename From, typename To>
	bool is_save_cast(auto source) {
		return static_cast<From>(static_cast<To>(source)) == source;
	};


	template <typename T>
	constexpr std::string_view type_name() {

		std::string_view name, prefix, suffix;
#ifdef __clang__
		name = __PRETTY_FUNCTION__;
		prefix = "std::string_view type_name() [T = ";
		suffix = "]";
#elif defined(__GNUC__)
		name = __PRETTY_FUNCTION__;
		prefix = "constexpr std::string_view type_name() [with T = ";
		suffix = "; std::string_view = std::basic_string_view<char>]";
#elif defined(_MSC_VER)
		name = __FUNCSIG__;
		prefix = "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl type_name<";
		suffix = ">(void)";
#endif
		name.remove_prefix(prefix.size());
		name.remove_suffix(suffix.size());
		return name;
	}

}

namespace tp {
	/* Type holder aka std::type_identity frpm C++20 */
	template <typename T> struct just_type {
		using type = T;
	};

	/* Type pack */
	template <class ...Ts> struct type_pack {};

	// size / empty

	using empty_pack = type_pack<>;

	template <class ...Ts>
	constexpr size_t size(type_pack<Ts...>) { return sizeof...(Ts);	};

	template <class ...Ts>
	constexpr bool empty(type_pack<Ts...> tp) { return size(tp) == 0; };

	// head / tail

	template <class T, class ...Ts>
	constexpr just_type<T> head(type_pack<T, Ts...>) { return {}; };

	template <class T, class ...Ts>
	constexpr type_pack<Ts...> tail(type_pack<T, Ts...>) { return {}; };

	// operator== / operator!=

	template <class ...Ts, class ...Us>
	constexpr bool operator == (type_pack<Ts...>, type_pack<Us...>) { return false;	};

	template <class ...Ts>
	constexpr bool operator == (type_pack<Ts...>, type_pack<Ts...>) { return true; };

	template <class ...Ts, class ...Us>
	constexpr bool operator != (type_pack<Ts...>, type_pack<Us...>) { return true; };

	template <class ...Ts>
	constexpr bool operator != (type_pack<Ts...>, type_pack<Ts...>) { return false;	};

	template <class T, class U>
	constexpr bool operator == (just_type<T>, just_type<U>) { return false;	};

	template <class T>
	constexpr bool operator == (just_type<T>, just_type<T>) { return true; };

	template <class T, class U>
	constexpr bool operator != (just_type<T>, just_type<U>) { return true; };

	template <class T>
	constexpr bool operator != (just_type<T>, just_type<T>) { return false;	};

	// push type based / value based
	template <class T, class ...Ts>
	constexpr type_pack<T, Ts...> push_front(type_pack<Ts...>) { return {}; };

	template <class ...Ts, class T>
	constexpr type_pack<T, Ts...> push_front(type_pack<Ts...>, just_type<T>) { return {}; };
}
