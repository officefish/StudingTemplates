#pragma once

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
#include<limits>

#include "./../type_container/type_pack.h"

namespace utilities {

	template<typename T>
	struct is_variant : std::false_type {};

	template<typename ...Args>
	struct is_variant<std::variant<Args...>> : std::true_type {};

	template<typename T>
	inline constexpr bool is_variant_v = is_variant<T>::value;

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
	auto initialize_variant(std::any header, std::variant<Ts...> const&) {

		std::optional<std::variant<Ts...>> variantOpt = std::nullopt;
		using CommonType = typename std::common_type_t<Ts...>;
		auto valueOpt = get_optional_value<CommonType>(header);
		if (valueOpt) variantOpt = *valueOpt;
		return variantOpt;
	}

	using proccessor::utilities::constexpr_for;

	template <typename ... Ts>
	auto any_to_common_cast(std::any any) -> std::common_type_t<Ts...> {

		using tuple_t = std::tuple<Ts...>;
		using common_t = std::common_type_t<Ts...>;
		common_t value = 0;

		constexpr_for<std::tuple_size_v<tuple_t>>([&](auto index) {
			constexpr auto i = index.value;
			using optional_t = std::tuple_element_t<i, tuple_t>;
			std::optional<optional_t> optValue = get_optional_value<optional_t>(any);
			if (optValue) {	value = *optValue; };
		});

		return value;
	}

	/*
	template <typename ... Ts>
	auto contineous_cast(auto value) {

		using tuple_t = std::tuple<Ts...>;

		using type_pack = tp::type_pack<>;
		//type_pack pack;

		constexpr_for<std::tuple_size_v<tuple_t>>([&](auto index) {
			constexpr auto i = index.value;
			using optional_t = std::tuple_element_t<i, tuple_t>;
			if (//proccessor::utilities::is_save_cast<decltype(value), optional_t>(value)
				 value < std::numeric_limits<optional_t>::max()
				) {
				//type = proccessor::utilities::just_type<optional_t>{};
				//return;
				std::cout << "save" << std::endl;
				//using type_pack_2 = tp::push_front<optional_t>(type_pack);
				//type_pack pack;
				//std::cout << tp::size(pack) << std::endl;
			}
			else {
				std::cout << "unsave" << std::endl;
			}
		});
		//using type_t = typename decltype(type)::type;
		//static_cast<type_t>(value);
		//std::cout << tp::size(pack) << std::endl;
		return value;
	}
	*/

	template <class T, class... Ts>
	auto unpack_variant(std::any any) {
		std::optional<std::variant<T, Ts...>> variantOpt = std::nullopt;
		if (!any.has_value()) return variantOpt;
		variantOpt = get_optional_value<std::variant<T, Ts...>>(any);
		return variantOpt;
	}



	template<typename, typename = void>
	constexpr bool is_type_complete_v = false;

	template<typename T>
	constexpr bool is_type_complete_v<T, std::void_t<decltype(sizeof(T))>> = true;

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

		template<typename T, typename Algebra>
		T constexpr reduceValue(const Algebra& algebra) noexcept {

			std::optional<container_variant> containerOpt = any_to_variant_cast<ANY_VARIANTS>(_container);
			T value = (not containerOpt)
				? __reduceValue<T>(_container, 0, algebra)
				: 0;
			return (containerOpt)
				? __reduceContainer<T>(*containerOpt, value, algebra)
				: __reduceValue<T>(_container, value, algebra);
		}

		template<typename T, typename Algebra, typename ConstantAlgebra>
		T constexpr reduceValue(const Algebra& algebra, const ConstantAlgebra& constantAlgebra) noexcept {

			std::optional<container_variant> containerOpt = any_to_variant_cast<ANY_VARIANTS>(_container);
			T value = (not containerOpt)
				? __reduceValue<T>(_container, 0, algebra, constantAlgebra)
				: 0;
			return (containerOpt)
				? __reduceContainer<T>(*containerOpt, value, algebra, constantAlgebra)
				: __reduceValue<T>(_container, value, algebra, constantAlgebra);

		}


	private:
		std::any _container;

		template <typename T, typename Algebra>
		T constexpr __reduceContainer(
			container_variant const& container,
			T const& value,
			Algebra const& algebra) noexcept {

			return std::visit([this, value, algebra](auto& container) {
				T __value = (container.size() == 1)
					? __reduceValue<T>(*container.begin(), value, algebra)
					: value;
				for (auto& element : container)
					__value = __reduceValue<T>(element, __value, algebra);
				return __value;
			}, container);
		}

		template <typename T, typename Algebra, typename ConstantAlgebra>
		T constexpr __reduceContainer(
			container_variant const& container,
			T const& value,
			Algebra const& algebra,
			ConstantAlgebra const& constantAlgebra
			) noexcept {

			return std::visit([this, value, algebra, constantAlgebra](auto& container) {
				T __value = (container.size() == 1)
					? __reduceValue<T>(*container.begin(), value, algebra, constantAlgebra)
					: value;
				for (auto& element : container)
					__value = __reduceValue<T>(element, __value, algebra, constantAlgebra);
				return __value;
				}, container);

		}

		/* Two arguments Algebra specialization */

		template <typename T,
			typename Algebra = std::function<T(T, T)>,
			typename Variant = std::enable_if_t<is_variant_v<T>>>
			T constexpr __reduceValue(
				std::any const& header,
				T const& value,
				Algebra const& algebra) noexcept {

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
			T constexpr __reduceValue(
				std::any const& header,
				T const& value,
				Algebra const& algebra) noexcept {

			std::optional<T> valueOpt = get_optional_value<T>(header);
			return (not valueOpt)
				? value
				: (not has_value<T>(value))
				? *valueOpt
				: algebra(value, *valueOpt);
		}

		/* Two arguments Algebra specialization with capture constant */

		template <typename T,
			typename Algebra = std::function<T(T, T)>,
			typename ConstantAlgebra = std::function<T(T)>,
			typename Variant = std::enable_if_t<is_variant_v<T>>>
			T constexpr __reduceValue(
				std::any const& header,
				T const& value,
				Algebra const& algebra,
				ConstantAlgebra const& constantAlgebra
				) noexcept {

			auto variantOpt = initialize_variant(header, value);
			return (not variantOpt)
				? value
				: (not has_value<T>(value))
				? constantAlgebra(*variantOpt)
				: algebra(value, constantAlgebra(*variantOpt));
		}

		template <typename T,
			typename NotVariant = std::enable_if_t<!is_variant_v<T>>,
			typename Algebra = std::function<T(T, T)>,
			typename ConstantAlgebra = std::function<T(T)>>
			T constexpr __reduceValue(
				std::any const& header,
				T const& value,
				Algebra const& algebra,
				ConstantAlgebra const& constantAlgebra
				) noexcept {

			std::optional<T> valueOpt = get_optional_value<T>(header);
			return (not valueOpt)
				? value
				: (not has_value<T>(value))
				? constantAlgebra(*valueOpt)
				: algebra(value, constantAlgebra(*valueOpt));
		}

	};
}

class ProccessorIO {
public:
	ProccessorIO() = default;
	~ProccessorIO() = default;

	void forward(std::any&& output) {
		this->output = output;
	}
	std::any move() {
		return (this->output) ? *this->output : 0;
	}

	template <typename T>
	T  extract() {
		if (!this->output) return 0;

		std::optional<T> opt = utilities::get_optional_value<T>(*this->output);
		return (opt) ? *opt : 0;
	}

	template <typename T, typename ...Ts,
	typename Common_t = std::common_type_t<T, Ts...>,
	typename Variadic = std::enable_if_t<sizeof...(Ts)>>
	Common_t extract() {

		if (!this->output) return 0;

		std::optional<Common_t> opt = std::nullopt;
		std::optional<std::variant<T, Ts...>> variantOpt = utilities::unpack_variant<T, Ts...>(*this->output);
		opt = (variantOpt.has_value())
			? utilities::current_to_common_v<T, Ts...>(*variantOpt)
			: utilities::any_to_common_cast<T, Ts...>(*this->output);
		return (opt) ? *opt : 0;
	}


private:
	std::optional<std::any> output = std::nullopt;
};


class AnyNode {
public:

	using value = std::function<std::any(std::any)>;

	std::shared_ptr<AnyNode> next = nullptr;
	std::optional<value> data;

	AnyNode() = default;
	~AnyNode()  { this->next = nullptr; };
	explicit AnyNode(value _data) noexcept { data = _data; };
};

class AnyProccessor final {
	using node = std::shared_ptr<AnyNode>;
	using value = std::function <std::any(std::any)>;
public:
	AnyProccessor() = delete;
	~AnyProccessor() noexcept { Free(this->head); __io.reset(); };
	explicit AnyProccessor(value data) noexcept {
		this->head = Insert(this->head, data);
	}
	explicit AnyProccessor(const std::initializer_list<value>& list) noexcept {
		this->head = Merge(this->head, list);
	}
	AnyProccessor(const AnyProccessor& source) noexcept {
		this->head = Copy(source.head);
	}
	AnyProccessor& operator=(const AnyProccessor& source) noexcept {
		this->head = Copy(source.head);
	}
	AnyProccessor(AnyProccessor&& other) noexcept {
		Free(this->head);
		__io.reset();
		this->head = other.head;
		this->__io = other.__io;
		other.head = nullptr;
		other.__io = nullptr;
	}
	AnyProccessor& operator=(AnyProccessor&& other) noexcept {
		if (this->head == other.head) return *this;
		Free(this->head);
		__io.reset();
		this->head = other.head;
		this->__io = other.__io;
		other.head = nullptr;
		other.__io = nullptr;
		return *this;
	}

	AnyProccessor& insert(value data) {
		this->head = Insert(this->head, data);
		return *this;
	}
	AnyProccessor& now(std::any constant) noexcept {
		this->__io->forward(Do(this->head, constant));
		return *this;
	}
	AnyProccessor& now(const std::initializer_list<std::any>& list) noexcept {
		this->__io->forward(Do(this->head, list));
		return *this;
	}
	ProccessorIO& io() const noexcept {
		return *this->__io;
	}

private:
	node head = nullptr;
	std::shared_ptr<ProccessorIO> __io = std::make_shared<ProccessorIO>();

	static void Free(node head) {
		node current = head;
		node next;
		while (current) {
			next = current->next;
			current.reset();
			current = next;
		}
	}

	node Copy(const node head) {
		node dummy = std::make_shared<AnyNode>();
		node current = dummy;
		node copy = head;
		while (copy) {
			node tmp = (copy->data)
				? std::make_shared<AnyNode>(*copy->data)
				: std::make_shared<AnyNode>();
			current->next = tmp;
			current = current->next;
			copy = copy->next;
		}
		dummy = dummy->next;
		return dummy;
	}

	static node Insert(node head, value data) noexcept {
		node newNode = std::make_shared<AnyNode>(data);
		if (head) newNode->next = head;
		return newNode;
	}
	static node BackInsert(node head, value data) noexcept {
		node newNode = std::make_shared<AnyNode>(data);
		if (!head) return newNode;
		node current = head;
		while (current->next)
			current = current->next;
		current->next = newNode;
		return head;
	}

	static node Merge(node head, const std::initializer_list<value>& list) noexcept {
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
		if (!current->next) {
			std::any gg;
			return (current->data)
				? (*current->data)(gg)
				: 0;
		}
		std::optional<value> func = (current->data)
			? (*current->data)
			: 0;
		std::any result = Terminate(current->next);
		return (func) ? (*func)(result) : 0;
	}

	static std::function<std::any(std::any)> Constant(std::any value) noexcept {
		std::function<std::any(std::any)> f = [value](std::any input) {
			return value;
		};
		return f;
	}
	static std::function<std::any(std::any)> Constant(const std::initializer_list<std::any>& list) noexcept {
		std::function<std::any(std::any)> f = [list](std::any input) {
			return list;
		};
		return f;
	}
	static std::any Do(node head, const std::initializer_list<std::any>& list) noexcept {
		std::function <std::any(std::any)> cnst = Constant(list);
		head = BackInsert(head, cnst);
		return Terminate(head);
	}

	static std::any Do(node head, std::any constant) noexcept {
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

		static constexpr T sum(T a, T b) {
			if constexpr (is_variant_v<T>) {
				auto a_value = current_to_common_v(a);
				auto b_value = current_to_common_v(b);
				T result = (a_value + b_value);
				return result;
			}
			else return a + b;
		};

		static constexpr T mul(T a, T b) {
			if constexpr (is_variant_v<T>) {
				auto a_value = current_to_common_v(a);
				auto b_value = current_to_common_v(b);
				T result = (a_value * b_value);
				return result;
			}
			else  return a * b;
		};

	};

}

namespace term::reduce {
	using namespace std::placeholders;
	using std::function, std::any, std::variant;
	using variadic = sigma::variadic_container;
	using calculator::Calculator;

	template<typename T, typename Algebra>
	constexpr function<any(any)> reduce(const Algebra& algebra) noexcept { // std::function<T(T,T...)
		function<any(any)> redex = [algebra](any input) {
			variadic variadic_container(input);
			return variadic_container.reduceValue<T>(algebra);
		};
		return redex;
	}

	template<typename T, typename Algebra, typename ConstantAlgebra>
	constexpr function<any(any)> reduce(const Algebra& algebra, const ConstantAlgebra& constantAlgebra) noexcept { // std::function<T(T,T...)
		function<any(any)> redex = [algebra, constantAlgebra](any input) {
			variadic variadic_container(input);
			return variadic_container.reduceValue<T>(algebra, constantAlgebra);
		};
		return redex;
	}


	/* Non-generic lambda expressions / Лямбы без состояния */
	template<typename T, typename ...Ts,
		typename R = function<any(any)> >
		constexpr R sum() noexcept {
		return sizeof...(Ts)
			? reduce<variant<T, Ts...>>(Calculator<variant<T, Ts...>>::sum)
			: reduce<T>(Calculator<T>::sum);
	};

	template<typename T, typename ...Ts,
		typename R = function<any(any)> >
		constexpr R mul() noexcept {
		return sizeof...(Ts)
			? reduce<variant<T, Ts...>>(Calculator<variant<T, Ts...>>::mul)
			: reduce<T>(Calculator<T>::mul);
	};

	/* Generic lambda expressions / Лямбы c состоянием (захваченной переменной) */
	template<typename T, typename ...Ts,
	typename Single = std::enable_if_t<!sizeof...(Ts)>,
	typename R = function<any(any)> >
	constexpr R sum(T constant) noexcept {
		std::function<T(T, T)> algebra = Calculator<T>::sum;
		std::function<T(T)> constantAlgebra = std::bind(Calculator<T>::sum, _1, constant);
		return reduce<T>(algebra, constantAlgebra);
	};

	template<typename T, typename ...Ts,
	typename Single = std::enable_if_t<sizeof...(Ts)>,
	typename Variant = variant<T, Ts...>,
	typename R = function<any(any)> >
	constexpr R sum(Variant constant) noexcept {
		std::function<Variant(Variant, Variant)> algebra = Calculator<Variant>::sum;
		std::function<Variant(Variant)> constantAlgebra = std::bind(Calculator<Variant>::sum, _1, constant);
		return reduce<Variant>(algebra, constantAlgebra);
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