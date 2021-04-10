
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

namespace utilities {


	template<typename T>
	struct is_variant : std::false_type {};

	template<typename ...Args>
	struct is_variant<std::variant<Args...>> : std::true_type {};

	template<typename T>
	inline constexpr bool is_variant_v = is_variant<T>::value;


	//template<typename T>
	//struct is_variadic : std::false_type {};

	//template<typename ...Ts, typename = std::enable_if_t<(sizeof...(Ts) > 0)>>
	//struct is_variadic<Ts...> : std::true_type {};

	//template<typename T>
	//inline constexpr bool is_variadic_v = is_variadic<T>::value;

	//template<typename T> constexpr unsigned argc_v;
	//template<typename... T> constexpr unsigned argc_v<T...> = sizeof...(T);

	//template<template T> constexpr bool is_variadic_v = false;
	//template<template... T> bool is_variadic_v<T...> = argc_v<T...> == 0;


	template <typename T, typename = std::enable_if_t<!is_variant_v<T>>>
	T current_to_common_v(T value) {
		return value;
	}

	template <typename... Args>
	std::common_type_t<Args...> current_to_common_v(const std::variant<Args...>& variant) {

		return std::visit([&](const auto& __variant) {

			if (__variant) {
				using U = typename std::decay_t<decltype(__variant)>;
				auto value = std::get<U>(variant);
				return static_cast<std::common_type_t<Args...>>(value);
			}
			else {
				auto value = std::get<0>(variant);
				return static_cast<std::common_type_t<Args...>>(value);
			}

		}, variant);
	}

	template <typename T>
	struct Printer {
		void operator() (const T& value) { std::cout << value << std::endl; };
	};

	template <typename ... Ts>
	struct Visitor : public Printer<Ts>...
	{ using Printer<Ts>::operator()...; };

	template <typename ... Ts>
	void visit(std::variant<Ts...> variant) {
		std::visit(Visitor<Ts...>{}, variant);
	}

	template <typename T>
	bool has_value(T value) noexcept {
		if constexpr (std::is_convertible_v <T, bool>) {
			return value;
		}
		else if constexpr (is_variant_v<T>) {

			return std::visit([](const auto& elem) {
				return has_value(elem);
				}, value);
		}
		return false;
	}

	template <typename T>
	auto get_optional_value(const std::any& any) noexcept -> std::optional<T> {
		return std::any_cast<T>(&any) ? *std::any_cast<T>(&any) : std::optional<T>();
	}

	template <class... Args>
	auto any_to_variant_cast(std::any any) noexcept -> std::optional<std::variant<Args...>> {
		if (not any.has_value()) return std::nullopt;
		std::optional<std::variant<Args...>> variant = std::nullopt;
		((variant = get_optional_value<Args>(any), true) || ...);
		return variant;
	}

	template <typename ... Ts>
	auto initialize_variant(std::any header, std::variant<Ts...> const &) {
		std::optional<std::variant<Ts...>> variantOpt = std::nullopt;
		variantOpt = any_to_variant_cast<Ts...>(header);
		if (not variantOpt) {
			using CommonType = typename std::common_type_t<Ts...>;
			auto valueOpt = get_optional_value<CommonType>(header);
			if (valueOpt)
				variantOpt = *valueOpt;
		}
		return variantOpt;
	}

	/*
	template <typename ... Ts>
	auto initialize_variant(std::any header, std::variant<Ts...> const& prototype) {
		std::optional<std::variant<Ts...>> variantOpt = std::nullopt;
		variantOpt = get_optional_value<std::variant<Ts...>>(header);
		return variantOpt;
	}
	*/
	template <class T, class... Ts>
	T unpack(std::any any) {
		std::cout << any.has_value() << std::endl;
		std::optional<std::variant<T, Ts...>> variantOpt = get_optional_value<std::variant<T, Ts...>>(any);
		if (variantOpt) {
			return  std::visit([](const auto& elem) {
				return static_cast<T>(elem);
				}, *variantOpt);
		}
		return 0;
	}


}

namespace sigma {

	using utilities::any_to_variant_cast;
	using utilities::get_optional_value;
	using utilities::is_variant_v;
	using utilities::has_value;
	using utilities::initialize_variant;

	class variadic_container {

#define ANY_VARIANTS std::initializer_list<std::any>,\
					 std::vector<std::any>, std::vector<std::vector<std::any>>,\
					 std::list<std::any>

		using container_variant = std::variant<ANY_VARIANTS>;

	public:
		template<class C> constexpr variadic_container(const C& c) noexcept : _container(c) {};
		template<class C> constexpr variadic_container(C&& c) noexcept : _container(c) {};

		template<class C>
		variadic_container& operator=(const C& c) noexcept { _container = c; return *this; }
		template<class C>
		variadic_container& operator=(C&& c) noexcept { _container = std::move(c); return *this; }

		template<typename T>
		T constexpr everyValue(const std::function<T(T, T)>& algebra) noexcept {

			T value = 0;
			std::optional<container_variant> containerOpt = any_to_variant_cast<ANY_VARIANTS>(_container);
			return ( containerOpt )
				? __everyContainer<T>(*containerOpt, value, algebra)
				: __everyValue<T>(_container, value, algebra);
		}

	private:
		std::any _container;

		template <typename T,
		typename Algebra = std::function<T(T, T)>>
		T constexpr __everyContainer (
			container_variant const & container,
			T const & value,
			Algebra const & algebra) noexcept {

			return std::visit([this, value, algebra](auto& container) {
				T __value = value;
				for (auto& element : container)
					__value = __everyValue<T>(element, __value, algebra);
				return __value;
			}, container);
		}

		template <typename T,
		typename Algebra = std::function<T(T, T)>,
		typename Variant = std::enable_if_t<is_variant_v<T>>>
		T constexpr __everyValue(
			std::any const & header,
			T const & value,
			Algebra const & algebra) noexcept {

			auto variantOpt = initialize_variant(header, value);
			return (not variantOpt)
				? value
				: (not has_value<T>(value))
					? *variantOpt
					: algebra(value, *variantOpt);
		}

		template <typename T,
		typename NotVariant = std::enable_if_t<!is_variant_v<T>>,
		typename Algebra = std::function<T(T, T)>>
		T constexpr __everyValue(
			std::any const & header,
			T const & value,
			Algebra const & algebra) noexcept {

			std::optional<T> valueOpt = get_optional_value<T>(header);
			return (not valueOpt)
				? value
				: (not has_value<T>(value))
					? *valueOpt
					: algebra(value, *valueOpt);

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
	using value = std::function <std::any(std::any)>;
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

namespace calculator {

	using utilities::is_variant_v;
	using utilities::has_value;
	using utilities::current_to_common_v;

	template<typename T>
	struct Calculator {

		template <typename T, typename U, typename R = std::common_type<T, U>::type>
		static constexpr R _sum(T a, U b) { return a + b; };

		static constexpr auto sum = [](T a, T b) {
			if constexpr (is_variant_v<T>) {
				auto a_value	=	current_to_common_v(a);
				auto b_value	=	current_to_common_v(b);
				T result		=	_sum(a_value, b_value);
				return result;
			}
			else return a + b;
		};

		static constexpr auto mul = [](T a, T b) {
			if constexpr (is_variant_v<T>) {
				return a;
			}
			else { return a * b; }
		};

	};

}

template<typename, typename = void>
constexpr bool is_type_complete_v = false;

template<typename T>
constexpr bool is_type_complete_v
<T, std::void_t<decltype(sizeof(T))>> = true;

namespace term::legion {

	using utilities::get_optional_value;

	using std::function, std::any, std::any_cast, std::type_index;
	using variadic = sigma::variadic_container;


	template<typename T>
	constexpr function<any(any)> legion(const function<T(T, T)>& algebra) noexcept { // std::function<T(T,T...)
		function<any(any)> redex = [algebra](any input) {
			variadic variadic_container(input);
			return variadic_container.everyValue<T>(algebra);
		};
		return redex;
	}

	template<typename T, typename ...Ts>
	constexpr std::function<std::any(std::any)> sum() noexcept {
		if constexpr (sizeof...(Ts) > 0) {
			std::function<std::variant<T, Ts...>(std::variant<T, Ts...>, std::variant<T, Ts...>)> f = calculator::Calculator<std::variant<T, Ts...>>::sum;
			return legion<std::variant<T,Ts...>>(f);
			//return legion<T>(f);
			//return legion<T>(Calculator<std::variant<T, Ts...>>::sum);
		}
		else {
			return legion<T>(calculator::Calculator<T>::sum);
		}
	};

	template<typename T>
	constexpr std::function<std::any(std::any)> mul() noexcept {
		return legion<T>(calculator::Calculator<T>::mul);
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