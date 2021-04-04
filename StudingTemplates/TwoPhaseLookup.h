
// 1.1.3 ƒвухэтапна€ трансл€ци€

namespace {
	template <typename T>
	void provocation(T t) {
		// first phase lookup error. CompileError throw
		//undeclared(); // error C3861: undeclared: идентификатор не найден
		// second phase lookup error.
		// Ѕросит ошибку только если функци€ есть в единице трансл€ции.
		//undeclared(t); // error C3861: undeclared: идентификатор не найден

		// first phase lookup error
		// Ѕросит ошибку только если функци€ есть в единице трансл€ции.
		//static_assert(sizeof(int) > 10, "int too small"); // error C2338 : int too small
		// second phase lookup error
		// Ѕросит ошибку только если функци€ есть в единице трансл€ции.
		//static_assert(sizeof(T) > 10, "T too small"); // error C2338: T too small
	}
}
