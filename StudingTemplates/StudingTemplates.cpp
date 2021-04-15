// StudingTemplates.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include "./Subjects.h"

#include <functional>
#include<typeindex>
#include<optional>
#include<type_traits>

#include "./any_proccessor/any_proccessor.h"

#include "./AnyVisitor.hpp"
#include "./type_container/type_pack.h"

//#include <ranges>

//#include <experimental/ty>

template <typename T>
void showMeTheRiver(T arg) {
	std::cout << proccessor::utilities::type_name<T>() << std::endl;
}

int main() {


	using proccessor::utilities::cast_to_smallest;
	using proccessor::utilities::contineous_cast;
	/*
	static_assert(std::is_same_v<const unsigned char, decltype(cast_to_smallest<     0, unsigned char, short, int>)>);
	static_assert(std::is_same_v<const unsigned char, decltype(cast_to_smallest<   255, unsigned char, short, int>)>);
	static_assert(std::is_same_v<const short, decltype(cast_to_smallest<   256, unsigned char, short, int>)>);
	static_assert(std::is_same_v<const short, decltype(cast_to_smallest< 32767, unsigned char, short, int>)>);
	static_assert(std::is_same_v<const int, decltype(cast_to_smallest< 32768, unsigned char, short, int>)>);
	static_assert(std::is_same_v<const short, decltype(cast_to_smallest<    -1, unsigned char, short, int>)>);
	static_assert(std::is_same_v<const short, decltype(cast_to_smallest<-32768, unsigned char, short, int>)>);
	static_assert(std::is_same_v<const int, decltype(cast_to_smallest<-32769, unsigned char, short, int>)>);

	constexpr long a = 400;
	using a_const = typename std::integral_constant<decltype(a), a>::type;
	auto b = contineous_cast<a_const, unsigned char, short, int>();

	//using optimus = typename m_contineous_cast<a_const, unsigned char, short, int>::type;
	//optimus b = 400;
	showMeTheRiver(b);
	*/

	auto sm_g = term::reduce::sum<int>(25);
	auto sm_gv = term::reduce::sum<int8_t, int16_t, int32_t>(127);

	AnyProccessor p{ sm_g };
	int i2 = p.now({ 1, 2, 3 }).io().extract<int>();
	std::cout << i2 << std::endl; // output: 4096 // (64*(8*(4+(2+(1+1)))))

	auto sm = term::reduce::sum<int>();
	auto ml = term::reduce::mul<int>();

	AnyProccessor list{ ml, ml, sm, sm, sm };
	int i = list.now({ 1 }).io().extract<int>();
	std::cout << i << std::endl; // output: 4096 // (64*(8*(4+(2+(1+1)))))

	int y = list.io().extract<int8_t, int16_t, int32_t>();
	std::cout << y << std::endl; // output: 4096 // (64*(8*(4+(2+(1+1)))))

	AnyProccessor list2{ ml };
	//	std::any value2 = list2.now({ 1, 2, 3, 4, 5 }).io.give_away();
	//	std::cout << std::any_cast<int>(value2) << std::endl; // output: 120 // (1*(2*(3*(4*(5)))))

	std::variant<int32_t, int16_t, double> v2 = 9.1;
	utilities::visit(v2);
	std::cout << std::boolalpha << utilities::has_value(v2) << std::endl;

	auto v_sm = term::reduce::sum<int32_t, int16_t, double>();
	AnyProccessor list3{ v_sm };

	list3.now({ 9.1, 2.2 });

	auto z = list3.io().extract<int32_t, int16_t, double>();
	std::cout << z << std::endl;

	std::any value3 = list3.io().move();
	//process(value3);
	//using CommonType = typename std::common_type_t<int, short, double>;

	//double v3d = std::any_cast<double>(value3);
	//auto v = utilities::unpack<int, short, double>(value3);
	//std::cout << v3d << std::endl;
	//process(value3);

	//std::cout << "value here: " << v << std::endl; // output: 120 // (1*(2*(3*(4*(5)))))
}
