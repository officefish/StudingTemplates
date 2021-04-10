#include <type_traits>
#include <any>
#include <functional>
#include <iomanip>
#include <iostream>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

template<class T, class F>
inline std::pair<const std::type_index, std::function<void(std::any const&)>>
to_any_visitor(F const& f)
{
    return {
        std::type_index(typeid(T)),
        [g = f](std::any const& a)
        {
            if constexpr (std::is_void_v<T>)
                g();
            else
                g(std::any_cast<T const&>(a));
        }
    };
}

static std::unordered_map<
    std::type_index, std::function<void(std::any const&)>>
    any_visitor{
        to_any_visitor<void>([] { std::cout << "{}"; }),
        to_any_visitor<int>([](int x) { std::cout << x; }),
        to_any_visitor<unsigned>([](unsigned x) { std::cout << x; }),
        to_any_visitor<float>([](float x) { std::cout << x; }),
        to_any_visitor<double>([](double x) { std::cout << x; }),
        to_any_visitor<int16_t>([](int16_t x) { std::cout << x; }),
        to_any_visitor<int32_t>([](int32_t x) { std::cout << x; }),
        to_any_visitor<char const*>([](char const* s)
            { std::cout << std::quoted(s); }),
    // ... add more handlers for your types ...
};

inline void process(const std::any& a)
{
    if (const auto it = any_visitor.find(std::type_index(a.type()));
        it != any_visitor.cend()) {
        it->second(a);
    }
    else {
        std::cout << "Unregistered type " << std::quoted(a.type().name());
    }
}

template<class T, class F>
inline void register_any_visitor(F const& f)
{
    std::cout << "Register visitor for type "
        << std::quoted(typeid(T).name()) << '\n';
    any_visitor.insert(to_any_visitor<T>(f));
}

/*
template <class... Ts, class Tuple = std::tuple<Ts...>>
Tuple variant_to_tuple(std::variant<Ts...> variant) {
    auto some = std::make_tuple<Ts...> tuple;

}
*/
