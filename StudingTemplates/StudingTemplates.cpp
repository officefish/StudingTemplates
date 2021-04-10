// StudingTemplates.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include "./Subjects.h"

#include <functional>
#include<typeindex>
#include<optional>
#include<type_traits>

#include "list/singly/hasky.h"

#include "./AnyVisitor.hpp"

int main() {

	auto sm = term::legion::sum<int>();
	auto ml = term::legion::mul<int>();

	AnyProccessor list{ ml, ml, sm, sm, sm };
	std::any value = list.reduce(1);

	std::cout << std::any_cast<int>(value) << std::endl; // output: 4096 // (64*(8*(4+(2+(1+1)))))

	AnyProccessor list2{ ml };
	std::any value2 = list2.reduce({ 1, 2, 3, 4, 5 });
	std::cout << std::any_cast<int>(value2) << std::endl; // output: 120 // (1*(2*(3*(4*(5)))))

	std::variant<int32_t, int16_t, double> v2 = 9.1;
	utilities::visit(v2);
	std::cout << std::boolalpha << utilities::has_value(v2) << std::endl;

	auto v_sm = term::legion::sum<int32_t, int16_t, double>();
	AnyProccessor list3{ v_sm };

	std::any value3 = list3.reduce({1, 2, 400, 2, 19, 0.2});
	//process(value3);
	using CommonType = typename std::common_type_t<int, short, double>;

	//double v3d = std::any_cast<double>(value3);
	//auto v = utilities::unpack<int, short, double>(value3);
	//std::cout << v3d << std::endl;
	//process(value3);

	//std::cout << "value here: " << v << std::endl; // output: 120 // (1*(2*(3*(4*(5)))))
}
