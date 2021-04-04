// StudingTemplates.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include "./Subjects.h"

#include <functional>
#include<typeindex>

#include "list/singly/hasky.h"

int main() {

	auto sm = term::legion::sum<int>();
	auto ml = term::legion::mul<int>();

	//Hasky list{ ml, ml, sm, sm, sm };
	//std::any value = list.reduce(1);

	Hasky list2{ ml };
	std::any value2 = list2.reduce({ 2, 2 });

	//Hasky list3{ sm };
	//std::any value3 = list3.reduce({ 3, 3, 4 });

	//Hasky list4{ sm };
	//std::vector<std::any> constants{ 1, 2, 3, 4, 5 };
	//std::any value4 = list4.reduce(constants);

	//std::cout << std::any_cast<int>(value) << std::endl; // output: 4096 // (64*(8*(4+(2+(1+1)))))
	std::cout << std::any_cast<int>(value2) << std::endl; // output: 4 (2+2)
	//std::cout << std::any_cast<int>(value3) << std::endl; // output: 10 (3+3+4)
	//std::cout << std::any_cast<int>(value4) << std::endl; // output: 10 (1+2+3+4+5)

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
