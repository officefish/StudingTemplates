#include ".././pch.h"
#include "./any_proccessor/any_proccessor.h"

TEST(AnyProccessor_Tests, Is_Not_Trivially_Constructible) {
	bool status = std::is_trivially_constructible<AnyProccessor, const AnyProccessor&>::value;
	EXPECT_EQ(status, false);
}
TEST(AnyProccessor_Tests, Is_Nothrow_Constructible) {
	bool status = std::is_nothrow_constructible<AnyProccessor, std::function <std::any(std::any)>>::value;
	EXPECT_EQ(status, true);
}
TEST(AnyProccessor_Tests, Is_Not_Trivially_Copy_Constructible) {
	bool status = std::is_trivially_constructible<AnyProccessor, typename std::add_lvalue_reference<
		typename std::add_const<AnyProccessor>::type>::type>{};
	EXPECT_EQ(status, false);
}
TEST(AnyProccessor_Tests, Is_Nothrow_Copy_Constructible) {
	bool status = std::is_nothrow_constructible<AnyProccessor, typename std::add_lvalue_reference<
		typename std::add_const<AnyProccessor>::type>::type> {};
	EXPECT_EQ(status, true);
}
TEST(AnyProccessor_Tests, Is_Not_Trivially_Move_Constructible) {
	bool status = std::is_trivially_constructible<AnyProccessor, typename std::add_rvalue_reference<AnyProccessor>::type>{};
	EXPECT_EQ(status, false);
}
TEST(AnyProccessor_Tests, Is_Nothrow_Move_Constructible) {
	bool status = std::is_nothrow_constructible<AnyProccessor, typename std::add_rvalue_reference<AnyProccessor>::type>{};
	EXPECT_EQ(status, true);
}
TEST(AnyProccessor_Tests, Is_Not_Trivially_Copy_Assignable) {
	bool status = std::is_trivially_copy_assignable<AnyProccessor>::value;
	EXPECT_EQ(status, false);
}
TEST(AnyProccessor_Tests, Is_Nothrow_Copy_Assignable) {
	bool status = std::is_nothrow_copy_assignable<AnyProccessor>::value;
	EXPECT_EQ(status, true);
}
TEST(AnyProccessor_Tests, Is_Not_Trivially_Move_Assignable) {
	bool status = std::is_trivially_move_assignable<AnyProccessor>::value;
	EXPECT_EQ(status, false);
}
TEST(AnyProccessor_Tests, Is_Nothrow_Move_Assignable) {
	bool status = std::is_nothrow_move_assignable<AnyProccessor>::value;
	EXPECT_EQ(status, true);
}
TEST(AnyProccessor_Tests, Is_Not_Trivially_Distructible) {
	bool status = std::is_trivially_destructible<AnyProccessor>::value;
	EXPECT_EQ(status, false);
}
TEST(AnyProccessor_Tests, Is_Nothrow_Distructible) {
	bool status = std::is_nothrow_destructible<AnyProccessor>::value;
	EXPECT_EQ(status, true);
}
TEST(AnyProccessor_Tests, Is_Final) {
	bool status = std::is_final<AnyProccessor>::value;
	EXPECT_EQ(status, true);
}
TEST(AnyProccessor_Tests, Is_ZeroInt_Argument_Consrtuctible) {
	std::optional<AnyProccessor> opt = AnyProccessor(0);
	EXPECT_EQ(opt.has_value(), true);
}
TEST(AnyProccessor_Tests, Is_ZeroInt_Argument_Aggregate_Consrtuctible) {
	std::optional<AnyProccessor> opt = AnyProccessor{ 0 };
	EXPECT_EQ(opt.has_value(), true);
}


