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