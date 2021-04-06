#pragma once
namespace ambient {

    template <typename Function>
    struct function_traits : public function_traits<decltype(&Function::operator())> {};

    template <typename ClassType, typename ReturnType, typename... Args>
    struct function_traits<ReturnType(ClassType::*)(Args...) const> {
        typedef ReturnType(*pointer)(Args...);
        typedef const std::function<ReturnType(Args...)> function;
    };

    template <typename Function>
    typename function_traits<Function>::function to_function(Function& lambda) {
        return static_cast<typename function_traits<Function>::function>(lambda);
    }

    template <class L>
    struct overload_lambda : L {
        overload_lambda(L l) : L(l) {}
        template <typename... T>
        void operator()(T&& ... values) {
            // here you can access the target std::function with
            to_function(*(L*)this)(std::forward<T>(values)...);
        }
    };

    template <class L>
    overload_lambda<L> lambda(L l) {
        return overload_lambda<L>(l);
    }

}


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