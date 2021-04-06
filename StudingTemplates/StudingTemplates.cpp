// StudingTemplates.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include "./Subjects.h"

#include <functional>
#include<typeindex>
#include<optional>
#include<type_traits>

#include "list/singly/hasky.h"



int main() {

	auto sm = term::legion::sum<int>();
	auto ml = term::legion::mul<int>();

	AnyProccessor list{ ml, ml, sm, sm, sm };
	std::any value = list.reduce(1);

	std::cout << std::any_cast<int>(value) << std::endl; // output: 4096 // (64*(8*(4+(2+(1+1)))))

	AnyProccessor list2{ ml };
	std::any value2 = list2.reduce({ 1, 2, 3, 4, 5 });

	std::cout << std::any_cast<int>(value2) << std::endl; // output: 120 // (1*(2*(3*(4*(5)))))

}
