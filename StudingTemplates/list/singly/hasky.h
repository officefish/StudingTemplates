#include <iostream>
#include <functional>
#include <any>
#include<typeindex>
#include"../.././AbientLambda.h"

namespace hsk {

	/*
	template<typename ... Args> struct FunctorImpl;

	template<typename R, typename ... Args>
	struct FunctorImpl<R(Args ...)> {
		typedef R(*FT)(Args ...);
		FunctorImpl(FT fn) : m_fn(fn) { ; }
		R operator () (Args ... args) {
			return m_fn(args ...);
		}
		FT m_fn;
	};
	template<typename FT>
	struct Functor : public FunctorImpl<FT>
	{
		Functor() : FunctorImpl<FT>(NULL) { ; }
		Functor(FT fn) : FunctorImpl<FT>(fn) { ; }
	};


	template<typename T>
	T sm(T only) {
		return only;
	};
	template<typename T, typename ...Targs>
	T sm (const T& First, const Targs&... rest) {
		T value = First;
		value += sm(rest...);
		//std::cout << "I call you bitch" << std::endl;
		std::cout <<"result: " << value << std::endl;
		return value;
	};




	template<typename T>
	struct memfun_type {
		using type = T;
	};

	template<typename Ret, typename Class, typename... Args>
	struct memfun_type<Ret(Class::*)(Args...) const> {
		using type = std::function<Ret(Args...)>;
	};

	template<typename F>
	typename memfun_type<decltype(&F::operator())>::type
		FFL(F const& func)
	{ // Function from lambda !
		return func;
	}
	*/


}

class Node {
public:

	using value = std::function<std::any(std::any)> ;

	std::shared_ptr<Node> next = nullptr;
	std::function < std::any(std::any) > data;

	Node() = default;
	~Node() = default;
	Node(value _data) : data(_data) {};
};

class Hasky {
	using node = std::shared_ptr<Node>;
	using value = std::function <std::any(std::any)> ;
public:
	node head = nullptr;
	Hasky() = delete;
	~Hasky() = default;
	Hasky(value data) {
		this->head = Insert(this->head, data);
	}
	Hasky(const std::initializer_list<value>& list) {
		this->head = Merge(this->head, list);
	}
	Hasky& insert(value data) {
		this->head = Insert(this->head, data);
		return *this;
	}

	std::any terminate() {
		return Terminate(this->head);
	}

	std::any reduce (std::any constant) {
		return Reduce(this->head, constant);
	}
	std::any reduce(const std::initializer_list<std::any>& list) {
		return Reduce(this->head, list);
	}

	static node Insert(node head, value data) {
		node newNode = std::make_shared<Node>(data);
		if (head) newNode->next = head;
		return newNode;
	}
	static node BackInsert(node head, value data) {
		node newNode = std::make_shared<Node>(data);
		if (not head) return newNode;
		node current = head;
		while (current->next)
			current = current->next;
		current->next = newNode;
		return head;
	}

	static node Merge(node head, const std::initializer_list<value>& list) {
		node dummy = std::make_shared<Node>();
		node current = dummy;
		for (auto& data : list) {
			node tmp = std::make_shared<Node>(data);
			current->next = tmp;
			current = current->next;
		}
		current->next = head;
		dummy = dummy->next;
		return dummy;
	}

	static std::any Terminate(node head) {
		node current = head;
		if (not current->next) {
			std::any gg;
			return current->data(gg);
		}
		std::function<std::any(std::any)> func = current->data;
		std::any result = Terminate(current->next);
		//std::cout << result << "\n";
		return func(result);
	}

	static std::function<std::any(std::any)> Constant(std::any value) {
		std::function<std::any(std::any)> f = [value](std::any input) {
			return value;
		};
		return f;
	}
	static std::function<std::any(std::any)> Constant(const std::initializer_list<std::any>& list) {
		std::function<std::any(std::any)> f = [list](std::any input) {
			return list;
		};
		return f;
	}
	static std::any Reduce(node head, const std::initializer_list<std::any>& list) {
		std::function <std::any(std::any)> cnst = Constant(list);
		head = BackInsert(head, cnst);
		return Terminate(head);
	}
	static std::any Reduce(node head, std::any constant) {
		std::function <std::any(std::any)> cnst = Constant(constant);
		head = BackInsert(head, cnst);
		return Terminate(head);
	}

	template<typename T>
	static constexpr auto sum = [&](T a, T b) {return a + b; };

	template<typename T>
	static constexpr auto mul = [&](T a, T b) {return a * b; };
};

namespace hasky {

	template<typename T>
	std::function<std::any(std::any)> producer(const std::function<T(T, T)>& algebra) { // std::function<T(T,T...)

		std::function<std::any(std::any)> f = [algebra](std::any input) {
			std::type_index index = std::type_index(input.type());
			if (index == std::type_index(typeid(T))) {
				T value = std::any_cast<T>(input);
				value = algebra(value, value);
				return value;
			}
			else if (index == std::type_index(typeid(std::initializer_list<std::any>))) {
				auto list = std::any_cast<std::initializer_list<std::any>>(input);
				T value = std::any_cast<T>(*list.begin());
				for (auto it = std::next(list.begin()); it != list.end(); ++it) {
					value = algebra(value, std::any_cast<T>(*it));
				}
				return value;
			}
			return 0;
		};
		return f;
	}

	template<typename T>
	std::function<std::any(std::any)> sum() {
		return producer<T>(Hasky::sum<T>);
	};
	template<typename T>
	std::function<std::any(std::any)> mul() {
		return producer<T>(Hasky::mul<T>);
	};
}