#include "./Subjects.h"
#pragma warning (disable : 4172) // warning C4172: ����������� ������ ��������� ��� ��������� ���������� : a
#pragma warning (disable : 4101) // warning C4101: ri14: ���������������� ��������� ����������

namespace decltable {

	int foo() { return 1; }
	int& foo1() { int a = 1;  return a; }; // warning C4172
	const int foo2() { return 1; };
	const int& foo3() { return 1; };

	void decltype_table() {

		auto ri = decltable::foo(); // int
		auto ri1 = decltable::foo1(); // int
		auto ri2 = decltable::foo2(); // int
		auto ri3 = decltable::foo3(); // int

		//auto& ri4 = foo(); // �� ��������������
		auto& ri5 = decltable::foo1(); // int&
		//auto& ri6 = foo2(); // �� ��������������
		auto& ri7 = decltable::foo3(); // const int&

		auto&& ri8 = foo(); // int&&
		auto&& ri9 = foo1(); // int&
		auto&& ri10 = foo2(); // const int&&
		auto&& ri11 = foo3(); // const int&

		int k = 5;
		//decltype(k) && rk = k; // �� ��������������
		decltype(foo()) && ri12 = foo(); // int&&
		decltype(foo1()) && ri13 = foo1(); // int&

		int i = 3;
		decltype(i) ri14; // int / warning C4101
		//decltype((i)) ri15; // ??int&  // �� ��������������
		//error C2530 : ri15: ������ ������ ���� ����������������
	}
}

