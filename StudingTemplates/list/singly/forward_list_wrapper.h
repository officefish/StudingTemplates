/*

*/
#pragma once
#include <iostream>
#include <forward_list>
#include <algorithm>
#include <sstream>

namespace sll {

	template<typename _Value_Type>
	class forward_list_wrapper {

		using value_type = _Value_Type;
		using list_instance = std::forward_list<_Value_Type, std::allocator<_Value_Type>>;
		using list_pointer = std::shared_ptr<std::forward_list<_Value_Type, std::allocator<_Value_Type>>>;
		using initialiser_list = std::initializer_list<_Value_Type>;

		using iterator = std::_Flist_iterator<std::forward_list<_Value_Type>>;
		using const_iterator = std::_Flist_const_iterator<std::forward_list<_Value_Type>>;

		list_pointer _GetPtr() {
			return std::make_shared<std::forward_list<_Value_Type, std::allocator<_Value_Type>>>();
		}

		list_pointer _List_Ptr = nullptr;

		static constexpr std::string LEFT_BRACE() {	return "{ "; }
		static constexpr std::string RIGHT_BRACE() { return " }"; }
		static constexpr std::string COMMA() { return ", ";	}
		static constexpr std::string BREAK() { return "\n";	}

	public:

		// Constructors

		forward_list_wrapper() {
			_List_Ptr = _GetPtr();
		}
		constexpr explicit forward_list_wrapper(value_type _Value) {
			_List_Ptr = _GetPtr();
			_List_Ptr->push_front(_Value);
		}
		constexpr forward_list_wrapper(const list_instance& _Source) {
			list_instance destination;
			std::copy(_Source.begin(), _Source.end(), destination.begin());
			_List_Ptr = list_pointer(destination);
		}
		constexpr forward_list_wrapper(const initialiser_list& _Source) {
			_List_Ptr = std::make_shared<std::forward_list<_Value_Type, std::allocator<_Value_Type>>>(_Source);
		}
		list_instance& operator= (const list_instance& _Source) {
			list_instance destination;
			std::copy(_Source.begin(), _Source.end(), destination.begin());
			_List_Ptr = list_pointer(destination);

		}

		// Destructor

		~forward_list_wrapper() {
			_List_Ptr->clear();
			_List_Ptr.reset();
			_List_Ptr = nullptr;
		}

		// Member functions
		constexpr forward_list_wrapper& assign(size_t count, const value_type& value) {
			_List_Ptr->assign(count, value);
			return *this;
		}
		template< class InputIt >
		forward_list_wrapper& assign(InputIt first, InputIt last) {
			_List_Ptr->assign(first, last);
			return *this;
		}
		constexpr forward_list_wrapper& assign(initialiser_list ilist) {
			_List_Ptr->assign(ilist);
			return *this;
		}

		_NODISCARD auto before_begin() noexcept {
			return _List_Ptr->before_begin();
		}
		_NODISCARD auto before_begin() const noexcept {
			return _List_Ptr->cbefore_begin();
		}

		_NODISCARD auto begin() noexcept {
			return _List_Ptr->begin();
		}

		_NODISCARD auto begin() const noexcept {
			return _List_Ptr->begin();
		}

		_NODISCARD auto cbegin() const noexcept {
			return _List_Ptr->begin();
		}
		_NODISCARD auto end() noexcept {
			return _List_Ptr->end();
		}

		_NODISCARD auto end() const noexcept {
			return _List_Ptr->end();
		}

		_NODISCARD auto cend() const noexcept {
			return _List_Ptr->cend();
		}

		bool empty() noexcept {
			return _List_Ptr->empty();
		}

		auto terminate() noexept {

		}


		/*
		const char* toString(bool braces = true) const noexcept {
			std::stringstream ss;
			if (braces) ss << LEFT_BRACE();
			Print((*_List_Ptr), COMMA(), ss);
			if (braces) ss << RIGHT_BRACE() << BREAK();
			return ss.str().c_str();
		}
		*/

		forward_list_wrapper& print(std::string sep, std::ostream& cout, bool braces = true) noexcept {
			if (braces) cout << LEFT_BRACE();
			Print((*_List_Ptr), sep, cout);
			if (braces) cout << RIGHT_BRACE() << BREAK();
			return *this;
		}

		forward_list_wrapper& print() noexcept {
			this->print(COMMA(), std::cout);
			return *this;
		}

		template<typename Func, typename T>
		constexpr T accumulate(Func func, T init) {
			//for (auto it = begin(); it != end(); ++it) {
			for (auto&& elem : (*_List_Ptr)) {
				init += func(elem);
			}
			return init;
		}



		static void Print(list_instance list, std::string sep, std::ostream& cout) noexcept {
			if (list.empty()) return;
			for (auto it = list.cbegin(); it != list.cend(); ++it) {
				cout << (*it);
				if (std::next(it) != list.cend()) { cout << sep; }
			}
		}

	};

	template<typename Range, typename Func, typename T>
	constexpr T Accumulate(const Range& range, Func func, T init) {
		for (auto&& elem : range) {
			init += func(elem);
		}
		return init;
	}


	template <typename _Value_Type>
	constexpr std::string toString(forward_list_wrapper<_Value_Type> list) noexcept {
		std::stringstream ss;
		for (auto it = list.cbegin(); it != list.cend(); ++it)
			ss << *it << " ";

		return ss.str();
	}
}

