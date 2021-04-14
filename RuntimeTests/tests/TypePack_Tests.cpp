#include ".././pch.h"
#include "./type_container/type_pack.h"

using namespace proccessor::utilities;

TEST(TypePack_Tests, Naive_Is_Same_True) {
	bool status = is_same_v<int, int>;
	EXPECT_EQ(status, true);
}

TEST(TypePack_Tests, Naive_Is_Same_False) {
	bool status = is_same_v<int16_t, int32_t>;
	EXPECT_EQ(status, false);
}

TEST(TypePack_Tests, Naive_Smaller_Type) {
	using min_t = smaller_type_t<int16_t, int32_t>;
	bool status = is_same_v<int16_t, min_t>;
	EXPECT_EQ(status, true);
}
TEST(TypePack_Tests, Naive_Larger_Type) {
	using max_t = larger_type_t<int16_t, int32_t>;
	bool status = is_same_v<int32_t, max_t>;
	EXPECT_EQ(status, true);
}

TEST(TypePack_Tests, Naive_Smaller_Element_Type) {
	using min_t = smaller_element_type_t<int16_t, int32_t, int8_t, long long>;
	bool status = is_same_v<int8_t, min_t>;
	EXPECT_EQ(status, true);
}

TEST(TypePack_Tests, Naive_Larger_Element_Type) {
	using max_t = larger_element_type_t<int16_t, int32_t, int8_t, long long>;
	bool status = is_same_v<long long, max_t>;
	EXPECT_EQ(status, true);
}

TEST(TypePack_Tests, Naive_Abstract_Select_Pattern) {
	using min_t = t_Select<t_MinPredicate, ::std::int16_t, ::std::int32_t, ::std::int8_t, long long>;
	bool status = ::std::is_same_v<::std::int8_t, min_t>;
	EXPECT_EQ(status, true);
}

/*
I still confused with variadic. Want to check recursively variadic or using some constexptr function which type is smallest in ...Ts an arbitrary number of arguments. This is my naive implementation of recursion, but it certainly doesn't work like that. Only the first two checks work correctly.

template <typename U, typename V>
using smaller_type = std::conditional_t < sizeof(U) < sizeof(V), U, V > ;

template <typename U, typename V>
using larger_type = std::conditional_t < sizeof(V) < sizeof(U), U, V > ;

template <typename T, typename ...Ts>
struct min {
    using type = T;
    type = smaller_type<type, Ts>...;
};

template <typename T, typename ...Ts>
struct max {
    using type = T;
    type = larger_type<type, Ts>...;
};

int main() {
    using min_t = min<int16_t, int32_t, int8_t, long long>;
    static_assert(is_same_v<int8_t, min_t>, true);
}

*/

