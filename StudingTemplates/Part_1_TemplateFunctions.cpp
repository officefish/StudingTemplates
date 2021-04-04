//
#include "Subjects.h"

namespace {

    // 1.1.1 Определение шаблона

    template <typename T>
    T max(T a, T b) {
        return a > b ? a : b;
    }

    template <typename T>
    T foo(T*) {}

    // 1.3 Несколько параметров шаблона

    /*
    template<typename T1, typename Т2>
    T1 max(T1 а, Т2 Ь) {
        return Ь < а ? а : Ь;
    }
    */

    // 1.3.1. Параметр шаблона для возвращаемого типа

    /*
    template<typename T1, typename T2, typename RT>
    RT max(T1 a, T2 b) {
        return Ь < а ? а : Ь;
    }
    */

    /*
    template<typename RT, typename T1, typename T2>
    RT max(T1 a, T2 b) {
        return Ь < а ? а : Ь;
    }
    */

    /*
    Продолжаем оптимизировать вывод типов

    1. Сначала мы позволили компилятору самостоятельно выбрать тип возвращаемого значения
    убрав, заменив тип RT yf встроенный ти auto

    template<typename T1, typename T2>
    auto max(T1 a, T2 b) {
        return Ь < а ? а : Ь;
    }
    */

    /*
    До C++14 единственной возможностью позволить компилятору определить
    тип возвращаемого была реализация функции как часть ее объявления. В С++11
    мы можем воспользоваться тем, что синтаксис завершающего возвращаемого
    типа позволяет использовать параметры вызова. То есть мы можем объявить, что
    возвращаемый тип является производным от того, что дает оператор ? ::

    template<typename Tl, typename T2>
    auto max(Tl a, T2 b) -> decltype(b < a ? a : b) {
        return b < a ? a : b;
    }

    https://en.cppreference.com/w/cpp/language/decltype
    https://habr.com/ru/post/206458/
    https://ci-plus-plus-snachala.ru/?p=4455

    offtopic: some interesting secret of auto
        class Foo {
            static auto self() -> decltype(this);
        };
        #define DECLARE_SELF \
        static auto selfTypeHelper() -> UnPtr<decltype(this)>; \
        using Self = decltype(selfTypeHelper())
        а потом применять его где это необходимо:
        class Foo {
            DECLARE_SELF;
        };
    offtopic::end

    Обратите внимание на то, что

    template<typename Tl, typename Т2>
    auto max(Tl а, Т2 b) -> decltype(b<a?a:b);

    является объявлением, таким образом, компилятор использует правила работы
    оператора ?:, вызванного для параметров а и Ь, чтобы выяснить тип возвращаемого
    значения функции max () во время компиляции. Реализация функции не
    обязана в точности соответствовать объявлению. Например, применения значения true
    в качестве условия оператора ? : в объявлении вполне достаточно:

    template<typename Tl, typename Т2>
    auto max(Tl a, T2 b) -> decltype(true?a:b);

    Однако в любом случае это определение имеет существенный недостаток:
    может случиться так, что возвращаемый тип является ссылочным,
    поскольку в некоторых условиях Т может быть ссылкой. По этой причине необходимо вернуть
    низведенный тип Т, который выглядит следующим образом:

    */

    //#include <type_traits>
    /*
    template<typename Tl, typename T2 >
    auto max(Tl a, T2 b)->typename std::decay<decltype(true ? a : b)>::type {
        return b < a ? a : b;
    }
    */

    /*
    Здесь использованосвойство типа std: : decayO, которое возвращает результирующий тип
    как член type. Оно определяется в стандартной библиотеке в заголовочном файле <type_traits>
    (см. раздел Г.4 приложения Г, “Стандартные утилиты для работы с типами”).
    Поскольку член type является типом, для доступа к нему следует квалифицировать выражение
    с помощью ключевого слова typename (см. раздел 5.1).
    */

    // https://en.cppreference.com/w/cpp/types/decay
    // decay_t<decltype(...)> - хорошая комбинация, чтобы увидеть, что вывел бы auto

    // Предыдущяя версия шаблона была полностью корректна, но начиная с++11 ее синтаксис
    // можно упростить используя шаблонную функцию common_type

    //#include <type_traits>
    /*
    template<typename Tl, typename T2 >
    typename std::common_type<Tl, T2>::type max(Tl a, T2 b) {
        return b < a ? a : b;
    }
    */

    /*
    Поскольку, начиная с C++14, можно упростить применение такого рода свойств,
    добавляя _t к имени свойства и опуская typename и : : type (см. подробное изложение в разделе 2.8),
    определение возвращаемого типа превращается в std::common_type_t<Tl,Т2> // Эквивалентная запись С++14
    */
    template<typename Tl, typename T2 >
    constexpr typename std::common_type_t<Tl, T2> max(Tl a, T2 b) {
        return b < a ? a : b;
    }

    // 1.4. Аргументы шаблона по умолчанию
    // 1.5. Перегрузка шаблонов функций
    int max(int a, int b) {
        return b < a ? a : b;
    }

    template<typename T>
    T* max(T* a, T* b) {
        return *a > *b ? a : b;
    }

    // Максимальная из двух строк в стиле С:
    char const* max(char const* a, char const* b) {
        return std::strcmp(b, a) < 0 ? a : b;
    }
}


void part1::templateFunctions() {

    // 1.1.2 Применение шаблона

    int i = 42;
    std::cout << "max(7,i): " << ::max(7, i) << "\n";

    double fl = 3.4;
    double f2 = -6.7;
    std::cout << "max(fl,f2): " << ::max(fl, f2) << "\n";

    std::string s1 = "mathematics";
    std::string s2 = "math";
    std::cout << "max(sl,s2): " << ::max(s1, s2) << "\n";

    //std::complex<float> cl, с2; // Отсутствует оператор <
    //::max(cl, с2);              // Ошибка времени компиляции

    // тип void является корректным аргументом шаблона при условии,
    // что получающийся код корректен;
    void* vp = nullptr;
    ::foo(vp);

    //::provocation(0.1);

    // 1.2 Вывод аргументов шаблона

    // преобразование типов
    int i2 = 17;
    int const с2 = 42;
    ::max(i2, с2);          // OK: T выводится как int
    ::max(с2, с2);          // OK: T выводится как int

    int& i2r = i2;
    ::max(i2, i2r);         // OK: Т выводится как int

    int arr2[4];
    ::max(&i2, arr2);       // OK: Т выводится как int*

    //::max(4, 7.2);        // Ошибка: Т может быть выведен как int или double
    //std::string s;
    //::max("hello", s);    // Ошибка: T может быть выведен как
                            // char const[6] или std::string

    /*
        1. Выполнить приведение аргументов, чтобы они соответствовали одному типу:
        max(static_cast<double>(4), 7.2); // OK
        2. Явно указать (или квалифицировать) тип Т для того, чтобы предупредить
           выведение типа компилятором:
        max<double>(4, 7.2); // OK
        3. Указать, что параметры могут иметь разные типы... рассмотрим позже
    */

    // 1.3 Несколько параметров шаблона
    // p.s. добавили еще одну шаблонную функцию max

    ::max(4, 7.2);          // OK, но тип первого аргумента
                            // определяет возвращаемый тип

    // 1.3.1.Параметр шаблона для возвращаемого типа

    //::max<int, double, double>(4, 7.2); // OK, но утомительно

    // p.s. Поставили тип возвращаемого значения в шаблоне на первое место
    ::max<double>(4, 7.2);      // OK: возвращаемый тип double,
                                // типы Tl и T2 выведены

    // 1.3.2. Вывод возвращаемого типа

    ::max(7, 42);           // Нешаблонная функция для двух int
    ::max(7.0, 42.0);       // max<double> (вывод аргументов)
    ::max('a', 'b');        // max<char> (вывод аргументов)
    ::max<>(7, 42);         // max<int> (вывод аргументов)
    ::max<double>(7, 42);   // max<double> (вывода аргументов нет)
    ::max('a', 42.7);       // Нешаблонная функция для двух int

    int a = 7;
    int b = 42;
    auto ml = ::max(a, b); // max ()1 для двух значений типа int

    std::string s3 = "hey";
    std::string s4 = "you";
    auto m2 = ::max(s3, s4); // max ()1 для двух std::string
    int* pi = &b;
    int* p2 = &a;
    auto m3 = ::max(pi, p2); // max ()' для двух указателей
    char const* x = "hello";
    char const* y = "world";
    auto m4 = ::max(x, y); // max () для двух строк в стиле С

    //int а[::max(sizeof(char), 1000u)];
    //std::array<std::string, ::max(sizeof(char), 1000u)> arr;
}
