
// 1.1.3 ����������� ����������

namespace {
	template <typename T>
	void provocation(T t) {
		// first phase lookup error. CompileError throw
		//undeclared(); // error C3861: undeclared: ������������� �� ������
		// second phase lookup error.
		// ������ ������ ������ ���� ������� ���� � ������� ����������.
		//undeclared(t); // error C3861: undeclared: ������������� �� ������

		// first phase lookup error
		// ������ ������ ������ ���� ������� ���� � ������� ����������.
		//static_assert(sizeof(int) > 10, "int too small"); // error C2338 : int too small
		// second phase lookup error
		// ������ ������ ������ ���� ������� ���� � ������� ����������.
		//static_assert(sizeof(T) > 10, "T too small"); // error C2338: T too small
	}
}
