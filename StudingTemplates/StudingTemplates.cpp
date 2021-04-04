// StudingTemplates.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include "./Subjects.h"

#include <functional>
#include<typeindex>

#include "list/singly/hasky.h"

template<typename T>
std::function<std::any(std::any)> mul () {
	std::function<std::any(std::any)> f = [](std::any input) {
		std::type_index index = std::type_index(input.type());
		if (index == std::type_index(typeid(T))) {
			T value = std::any_cast<T>(input);
			value *= value;
			return value;
		}
		else if (index == std::type_index(typeid(std::initializer_list<std::any>))) {
			auto list = std::any_cast<std::initializer_list<std::any>>(input);
			T value = std::any_cast<T>(*list.begin());
			for (auto it = std::next(list.begin()); it != list.end(); ++it) {
				value *= std::any_cast<T>(*it);
			}
			return value;
		}
		return 0;
	};
	return f;
}






int main() {

	auto sm = hasky::sum<int>();
	auto ml = hasky::mul<int>();

	Hasky list{ ml, ml, sm, sm, sm };
	std::any value = list.reduce(1);

	Hasky list2{ ml };
	std::any value2 = list2.reduce({ 2, 2 });

	Hasky list3{ sm };
	std::any value3 = list3.reduce({ 3, 3, 4 });

	std::cout << std::any_cast<int>(value) << std::endl; // output: 4096
	std::cout << std::any_cast<int>(value2) << std::endl; // output: 4
	std::cout << std::any_cast<int>(value3) << std::endl; // output: 10


	//for (auto&& v : list3) {
		//std::cout << v << " ";
	//}

	//static_assert(list3.accumulate([](int32_t i) {return i; }, 0) == 15, "Wrong");
	//static_assert(sll::Accumulate(flist, [](int i) {return i; }, 0) == 15);

	//static_assert(sll::(list2) == "", "");
	//std::cout << sll::toString(list2) << '\n';

	//list2.toString();
	//list3.toString();
	//list4.toString();
	//list5.toString();
	//hasky.print();
	//hasky.print();
	//const hasky list_const{ 1,2,3,4,5 };

	// member functions
	//hasky list1;
	//list1.assign(10, 1).print();
	//hasky list2;
	//list2.assign(list1.begin(), list1.end()).print();
	//hasky list3;
	//list2.assign({1,2,3,4,5,6,7,8,9}).print();



}
