#include <tuple>
#include <utility>


#ifndef NAMESPACE
#define NAMESPACE std
#endif

#ifndef COUNT
#define COUNT 4
#endif


#ifndef ITERATION
#define ITERATION 100
#endif




namespace simple {
    template<typename ... Ts> struct tuple;
    template<typename T, typename ... Ts> struct tuple<T, Ts...> : tuple<Ts...> {
        static constexpr int size = 1 + sizeof...(Ts);
        constexpr tuple(T t, Ts... ts) : tuple<Ts...>{ts...} , data{t} {};
        T data;
        constexpr T get_helper(std::integral_constant<int, sizeof...(Ts)>) { return data; }
        using tuple<Ts...>::get_helper;
    };
    template<> struct tuple<> {
        static constexpr int size = 0;
        void get_helper() { }

    };

    template<typename T> struct tuple_size { static constexpr int value = T::size; };

    template<int N, typename...Ts> constexpr auto get(tuple<Ts...> t)
    { return t.get_helper(std::integral_constant<int, sizeof...(Ts) - N - 1>{}); }

    template<typename... Ts> tuple<Ts...> make_tuple(Ts... ts) { return {ts...}; }
}




using NAMESPACE::tuple;
using NAMESPACE::tuple_size;
using NAMESPACE::get;


template<typename... Ts, typename T, std::size_t...I>
constexpr auto tuple_append_helper(const tuple<Ts...> &tuple_, const T &t, std::index_sequence<I...>) {
    return tuple<Ts..., T>{get<I>(tuple_)... , t};
}
template<typename T1, typename T>
constexpr auto tuple_append(const T1 &tuple, const T &t) {
    return tuple_append_helper(tuple, t, std::make_index_sequence<tuple_size<T1>::value>());
}



constexpr bool and_() { return true; }
template<typename... T>
constexpr bool and_(bool x, T... t) { return x && and_(t...); }


#if !defined(TEST) || TEST == 1

template <int P = 1, int N = COUNT>
struct CreateTuple {
    using Base = CreateTuple<P, N-1>;
    static constexpr auto value1 = tuple_append(Base::value1, std::integral_constant<int, N*P>{});
    static constexpr auto value2 = tuple_append(Base::value2, N*P);
};
template <int P>
struct CreateTuple<P, 0> {
    static constexpr tuple<> value1 = {};
    static constexpr tuple<> value2 = {};
};


template <typename T> struct Test1;
template <std::size_t... I> struct Test1<std::index_sequence<I...>> {
    static_assert(and_((tuple_size<decltype(CreateTuple<I>::value1)>::value == COUNT)...), "fail value1");
    static_assert(and_((tuple_size<decltype(CreateTuple<I>::value2)>::value == COUNT)...), "fail value2");

    static_assert(and_((get<0>(CreateTuple<I>::value2) == I)...), "fail get 0");
    static_assert(and_((get<COUNT-1>(CreateTuple<I>::value2) == COUNT*I)...), "fail get COUNT-1");

};

Test1<std::make_index_sequence<ITERATION>> t1;




#endif
