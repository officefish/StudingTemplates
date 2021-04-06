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

namespace utilities {

	template <typename T>
	auto get_v_opt(const std::any& any) noexcept -> std::optional<T>  {
		return std::any_cast<T>(&any) ? *std::any_cast<T>(&any) : std::optional<T>();
	}

	template <class... Args>
	auto any_to_variant_cast(std::any any) noexcept -> std::optional<std::variant<Args...>> {
		if (not any.has_value()) return std::nullopt;
		std::optional<std::variant<Args...>> variant = std::nullopt;
		((variant = get_v_opt<Args>(any), true) || ...);
		return variant;
	}
}

namespace sigma {

	using utilities::any_to_variant_cast;
	using utilities::get_v_opt;

	template <class T>
	class variadic_container {

#define ANY_VARIANTS std::initializer_list<std::any>, std::vector<std::any>
#define VARIANTS std::initializer_list<T>, std::vector<T>
		using container_variant = std::variant<VARIANTS>;
		using variant_optional = std::optional<std::variant<ANY_VARIANTS>>;
	private:
		std::any _container;

	public:
		template<class C> constexpr variadic_container(const C& c) noexcept : _container(c) {};
		template<class C> constexpr variadic_container(C&& c) noexcept : _container(c) {};

		template<class C>
		variadic_container& operator=(const C& c) noexcept { _container = c; return *this; }
		template<class C>
		variadic_container& operator=(C&& c) noexcept { _container = std::move(c); return *this; }

		template <class T>
		T constexpr everyValue(std::function<T(T, T)> algebra) noexcept {
			variant_optional variantOpt = any_to_variant_cast<ANY_VARIANTS>(_container);
			if (not variantOpt) return 0;
			return std::visit([this, algebra](auto& variant) {
				std::optional<T> valueOpt = get_v_opt<T>(*variant.begin());
				if (not valueOpt) return 0;
				T value = *valueOpt;
				for (auto it = std::next(variant.begin()); it != variant.end(); ++it) {
					valueOpt = *std::any_cast<T>(&(*it));
					if (not valueOpt) return 0;
					value = algebra(value, *valueOpt);
				}
				return value;
			}, *variantOpt);
			return 0;
		}
	};
}


class AnyNode {
public:

	using value = std::function<std::any(std::any)>;

	std::shared_ptr<AnyNode> next = nullptr;
	std::function <std::any(std::any)> data;

	AnyNode() = default;
	~AnyNode() = default;
	AnyNode(value _data) : data(_data) {};
};

class AnyProccessor {
	using node = std::shared_ptr<AnyNode>;
	using value = std::function <std::any(std::any)> ;
public:
	node head = nullptr;
	AnyProccessor() = delete;
	~AnyProccessor() = default;
	AnyProccessor(value data) {
		this->head = Insert(this->head, data);
	}
	AnyProccessor(const std::initializer_list<value>& list) {
		this->head = Merge(this->head, list);
	}
	AnyProccessor& insert(value data) {
		this->head = Insert(this->head, data);
		return *this;
	}

	std::any terminate() {
		return Terminate(this->head);
	}

	std::any reduce(std::any constant) noexcept {
		return Reduce(this->head, constant);
	}
	std::any reduce(const std::initializer_list<std::any>& list) noexcept {
		return Reduce(this->head, list);
	}

	template<typename T>
	T reduce_and_cast(const std::initializer_list<std::any>& list) noexcept {
		std::any value = Reduce(this->head, list);
		std::optional<T> valueOpt = *std::any_cast<T>(&value);
		if (valueOpt) return *valueOpt;
		return 0;
	}

	static node Insert(node head, value data) {
		node newNode = std::make_shared<AnyNode>(data);
		if (head) newNode->next = head;
		return newNode;
	}
	static node BackInsert(node head, value data) {
		node newNode = std::make_shared<AnyNode>(data);
		if (not head) return newNode;
		node current = head;
		while (current->next)
			current = current->next;
		current->next = newNode;
		return head;
	}

	static node Merge(node head, const std::initializer_list<value>& list) {
		node dummy = std::make_shared<AnyNode>();
		node current = dummy;
		for (auto& data : list) {
			node tmp = std::make_shared<AnyNode>(data);
			current->next = tmp;
			current = current->next;
		}
		current->next = head;
		dummy = dummy->next;
		return dummy;
	}

	static std::any Terminate(node head) noexcept {
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

template<typename, typename = void>
constexpr bool is_type_complete_v = false;

template<typename T>
constexpr bool is_type_complete_v
<T, std::void_t<decltype(sizeof(T))>> = true;

namespace term::legion {

	using utilities::get_v_opt;

	using std::function, std::any, std::any_cast, std::type_index;
	using variadic = sigma::variadic_container<any>;

	template<typename T>
	constexpr function<any(any)> legion(const function<T(T, T)>& algebra) noexcept { // std::function<T(T,T...)
		function<any(any)> redex = [algebra](any input) {
			std::optional<T> valueOpt = get_v_opt<T>(input);
			if (valueOpt)
				return algebra(*valueOpt, *valueOpt);

			variadic variadic_container(input);
			return variadic_container.everyValue<T>(algebra);
		};
		return redex;
	}

	template<typename T>
	constexpr std::function<std::any(std::any)> sum() noexcept {
		return legion<T>(Calculator<T>::sum);
	};
	template<typename T>
	constexpr std::function<std::any(std::any)> mul() noexcept {
		return legion<T>(Calculator<T>::mul);
	};
}



/*
template <typename ...Variants>
class VariantNode {
public:
	using variants = std::variant<Variants...>;
	using Value = std::function<variants(variants)>;
	using Node = std::shared_ptr<VariantNode<Variants...>>;

	std::optional<Node> next = std::nullopt;
	Value data;

	VariantNode() = default;
	~VariantNode() = default;
	constexpr explicit VariantNode(Value _data) : data(_data)  {};
};

template <typename ...Variants>
class VariantProccessor {
	using variants = std::variant<Variants...>;
	using Value = std::function<variants(variants)>;
	using NodeType = VariantNode<Variants...>;
	using Node = std::shared_ptr<NodeType>;

public:
	std::optional<Node> head = std::nullopt;
	VariantProccessor() = default;
	~VariantProccessor() = default;
	constexpr explicit VariantProccessor(Value data) {
		Node newNode(new NodeType(data));
		if (this->head.has_value()) newNode->next = head.value();
		this->head = newNode;
	}
	VariantProccessor(const std::initializer_list<Value>& list) {
		this->head = Merge(list);
	}

private :
	constexpr Node Insert(Value data) noexcept {
		Node newNode(new NodeType(data));
		if (this->head.has_value()) newNode->next = head.value();
		return newNode;
	}
	constexpr Node BackInsert(Value data) noexcept {
		Node newNode(new NodeType(data));
		if (this->head.has_value()) return newNode;
		Node current = head->value();
		while (current->next)
			current = current->next;
		current->next = newNode;
		return head->value;
	}

	constexpr Node Merge(const std::initializer_list<Value>& list) noexcept {
		Node dummy(new NodeType());
		Node current = dummy;
		for (auto& data : list) {
			Node tmp(new NodeType(data));
			current->next = tmp;
			current = current->next;
		}
		if(this->head.has_value()) current->next = this->head.value();
		dummy = dummy->next;
		return dummy;
	}
	constexpr variants Terminate(Node head) noexcept {
		Node current = head;
		if (not current->next) {
			variants gg;
			return current->data(gg);
		}
		std::function<variants(variants)> func = current->data;
		variants result = Terminate(current->next);
		return func(result);
	}
	constexpr auto Constant(const std::initializer_list<variants>& list) {
		std::function<variants(variants)> f = [list](variants input) {
			return list;
		};
		return f;
	}
	constexpr variants Reduce(const std::initializer_list<std::any>& list) {
		std::function <variants(variants)> cnst = Constant(list);
		this->head = BackInsert(cnst);
		return Terminate(head->value());
	}
};
*/