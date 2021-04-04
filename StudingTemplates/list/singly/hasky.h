#include <iostream>
#include <functional>
#include <any>
#include<typeindex>
#include<type_traits>
#include<variant>
#include <optional>

#include<vector>
#include<list>
#include<forward_list>
#include<deque>
#include<queue>
#include<stack>

namespace sigma {

	template <class... Args>
	auto any_to_variant_cast(std::any a)  -> std::variant<Args...> {
		if (!a.has_value())
			throw std::bad_any_cast();
		std::optional<std::variant<Args...>> v = std::nullopt;
		bool found = ((a.type() == typeid(Args) && (v = std::any_cast<Args>(std::move(a)), true)) || ...);
		if (!found)
			throw std::bad_any_cast{};
		return std::move(*v);
	}

	template <class T>
	class variadic_container {
#define ANY_VARIANTS std::initializer_list<std::any>, std::vector<std::any>
#define VARIANTS std::initializer_list<T>, std::vector<T>
		using container_variant = std::variant<VARIANTS>;

	private:
		std::any _container;

	public:
		template<class C> variadic_container(const C& c) : _container(c) {}
		template<class C> variadic_container(C&& c) : _container(c) {}

		template<class C>
		variadic_container& operator=(const C& c) { _container = c; return *this; }
		template<class C>
		variadic_container& operator=(C&& c) { _container = std::move(c); return *this; }

		template <class T>
		T everyValue(std::function<T(T, T)> algebra) {
			auto variant = any_to_variant_cast<ANY_VARIANTS>(_container);
			return std::visit([this, algebra](auto& variant) {
				T value = std::any_cast<T>(*variant.begin());
				for (auto it = std::next(variant.begin()); it != variant.end(); ++it)
					value = algebra(value, std::any_cast<T>(*it));
				return value;
			}, variant);

		}
	};
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
};

template<typename T>
class Calculator {
public:

	static constexpr auto sum = [&](T a, T b) { return a + b; };
	static constexpr auto mul = [&](T a, T b) { return a * b; };
};

namespace term::legion {

	using std::function, std::any, std::any_cast, std::type_index;
	using variadic = sigma::variadic_container<any>;

	template<typename T>
	function<any(any)> legion(const function<T(T, T)>& algebra) { // std::function<T(T,T...)
		function<any(any)> redex = [algebra](any input) {
			type_index index = type_index(input.type());
			if (index == type_index(typeid(T)))
				return algebra(any_cast<T>(input), any_cast<T>(input));
			variadic variadic_container(input);
			return variadic_container.everyValue<T>(algebra);
		};
		return redex;
	}

	template<typename T>
	std::function<std::any(std::any)> sum() {
		return legion<T>(Calculator<T>::sum);
	};
	template<typename T>
	std::function<std::any(std::any)> mul() {
		return legion<T>(Calculator<T>::mul);
	};
}
