//
#include "Subjects.h"

namespace {

    // 1.1.1 ����������� �������

    template <typename T>
    T max(T a, T b) {
        return a > b ? a : b;
    }

    template <typename T>
    T foo(T*) {}

    // 1.3 ��������� ���������� �������

    /*
    template<typename T1, typename �2>
    T1 max(T1 �, �2 �) {
        return � < � ? � : �;
    }
    */

    // 1.3.1. �������� ������� ��� ������������� ����

    /*
    template<typename T1, typename T2, typename RT>
    RT max(T1 a, T2 b) {
        return � < � ? � : �;
    }
    */

    /*
    template<typename RT, typename T1, typename T2>
    RT max(T1 a, T2 b) {
        return � < � ? � : �;
    }
    */

    /*
    ���������� �������������� ����� �����

    1. ������� �� ��������� ����������� �������������� ������� ��� ������������� ��������
    �����, ������� ��� RT yf ���������� �� auto

    template<typename T1, typename T2>
    auto max(T1 a, T2 b) {
        return � < � ? � : �;
    }
    */

    /*
    �� C++14 ������������ ������������ ��������� ����������� ����������
    ��� ������������� ���� ���������� ������� ��� ����� �� ����������. � �++11
    �� ����� ��������������� ���, ��� ��������� ������������ �������������
    ���� ��������� ������������ ��������� ������. �� ���� �� ����� ��������, ���
    ������������ ��� �������� ����������� �� ����, ��� ���� �������� ? ::

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
        � ����� ��������� ��� ��� ��� ����������:
        class Foo {
            DECLARE_SELF;
        };
    offtopic::end

    �������� �������� �� ��, ���

    template<typename Tl, typename �2>
    auto max(Tl �, �2 b) -> decltype(b<a?a:b);

    �������� �����������, ����� �������, ���������� ���������� ������� ������
    ��������� ?:, ���������� ��� ���������� � � �, ����� �������� ��� �������������
    �������� ������� max () �� ����� ����������. ���������� ������� ��
    ������� � �������� ��������������� ����������. ��������, ���������� �������� true
    � �������� ������� ��������� ? : � ���������� ������ ����������:

    template<typename Tl, typename �2>
    auto max(Tl a, T2 b) -> decltype(true?a:b);

    ������ � ����� ������ ��� ����������� ����� ������������ ����������:
    ����� ��������� ���, ��� ������������ ��� �������� ���������,
    ��������� � ��������� �������� � ����� ���� �������. �� ���� ������� ���������� �������
    ����������� ��� �, ������� �������� ��������� �������:

    */

    //#include <type_traits>
    /*
    template<typename Tl, typename T2 >
    auto max(Tl a, T2 b)->typename std::decay<decltype(true ? a : b)>::type {
        return b < a ? a : b;
    }
    */

    /*
    ����� �������������������� ���� std: : decayO, ������� ���������� �������������� ���
    ��� ���� type. ��� ������������ � ����������� ���������� � ������������ ����� <type_traits>
    (��. ������ �.4 ���������� �, ������������ ������� ��� ������ � ������).
    ��������� ���� type �������� �����, ��� ������� � ���� ������� ��������������� ���������
    � ������� ��������� ����� typename (��. ������ 5.1).
    */

    // https://en.cppreference.com/w/cpp/types/decay
    // decay_t<decltype(...)> - ������� ����������, ����� �������, ��� ����� �� auto

    // ���������� ������ ������� ���� ��������� ���������, �� ������� �++11 �� ���������
    // ����� ��������� ��������� ��������� ������� common_type

    //#include <type_traits>
    /*
    template<typename Tl, typename T2 >
    typename std::common_type<Tl, T2>::type max(Tl a, T2 b) {
        return b < a ? a : b;
    }
    */

    /*
    ���������, ������� � C++14, ����� ��������� ���������� ������ ���� �������,
    �������� _t � ����� �������� � ������� typename � : : type (��. ��������� ��������� � ������� 2.8),
    ����������� ������������� ���� ������������ � std::common_type_t<Tl,�2> // ������������� ������ �++14
    */
    template<typename Tl, typename T2 >
    constexpr typename std::common_type_t<Tl, T2> max(Tl a, T2 b) {
        return b < a ? a : b;
    }

    // 1.4. ��������� ������� �� ���������
    // 1.5. ���������� �������� �������
    int max(int a, int b) {
        return b < a ? a : b;
    }

    template<typename T>
    T* max(T* a, T* b) {
        return *a > *b ? a : b;
    }

    // ������������ �� ���� ����� � ����� �:
    char const* max(char const* a, char const* b) {
        return std::strcmp(b, a) < 0 ? a : b;
    }
}


void part1::templateFunctions() {

    // 1.1.2 ���������� �������

    int i = 42;
    std::cout << "max(7,i): " << ::max(7, i) << "\n";

    double fl = 3.4;
    double f2 = -6.7;
    std::cout << "max(fl,f2): " << ::max(fl, f2) << "\n";

    std::string s1 = "mathematics";
    std::string s2 = "math";
    std::cout << "max(sl,s2): " << ::max(s1, s2) << "\n";

    //std::complex<float> cl, �2; // ����������� �������� <
    //::max(cl, �2);              // ������ ������� ����������

    // ��� void �������� ���������� ���������� ������� ��� �������,
    // ��� ������������ ��� ���������;
    void* vp = nullptr;
    ::foo(vp);

    //::provocation(0.1);

    // 1.2 ����� ���������� �������

    // �������������� �����
    int i2 = 17;
    int const �2 = 42;
    ::max(i2, �2);          // OK: T ��������� ��� int
    ::max(�2, �2);          // OK: T ��������� ��� int

    int& i2r = i2;
    ::max(i2, i2r);         // OK: � ��������� ��� int

    int arr2[4];
    ::max(&i2, arr2);       // OK: � ��������� ��� int*

    //::max(4, 7.2);        // ������: � ����� ���� ������� ��� int ��� double
    //std::string s;
    //::max("hello", s);    // ������: T ����� ���� ������� ���
                            // char const[6] ��� std::string

    /*
        1. ��������� ���������� ����������, ����� ��� ��������������� ������ ����:
        max(static_cast<double>(4), 7.2); // OK
        2. ���� ������� (��� ���������������) ��� � ��� ����, ����� ������������
           ��������� ���� ������������:
        max<double>(4, 7.2); // OK
        3. �������, ��� ��������� ����� ����� ������ ����... ���������� �����
    */

    // 1.3 ��������� ���������� �������
    // p.s. �������� ��� ���� ��������� ������� max

    ::max(4, 7.2);          // OK, �� ��� ������� ���������
                            // ���������� ������������ ���

    // 1.3.1.�������� ������� ��� ������������� ����

    //::max<int, double, double>(4, 7.2); // OK, �� �����������

    // p.s. ��������� ��� ������������� �������� � ������� �� ������ �����
    ::max<double>(4, 7.2);      // OK: ������������ ��� double,
                                // ���� Tl � T2 ��������

    // 1.3.2. ����� ������������� ����

    ::max(7, 42);           // ����������� ������� ��� ���� int
    ::max(7.0, 42.0);       // max<double> (����� ����������)
    ::max('a', 'b');        // max<char> (����� ����������)
    ::max<>(7, 42);         // max<int> (����� ����������)
    ::max<double>(7, 42);   // max<double> (������ ���������� ���)
    ::max('a', 42.7);       // ����������� ������� ��� ���� int

    int a = 7;
    int b = 42;
    auto ml = ::max(a, b); // max ()1 ��� ���� �������� ���� int

    std::string s3 = "hey";
    std::string s4 = "you";
    auto m2 = ::max(s3, s4); // max ()1 ��� ���� std::string
    int* pi = &b;
    int* p2 = &a;
    auto m3 = ::max(pi, p2); // max ()' ��� ���� ����������
    char const* x = "hello";
    char const* y = "world";
    auto m4 = ::max(x, y); // max () ��� ���� ����� � ����� �

    //int �[::max(sizeof(char), 1000u)];
    //std::array<std::string, ::max(sizeof(char), 1000u)> arr;
}
