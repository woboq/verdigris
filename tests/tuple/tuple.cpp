/* Copyright 2016 Olivier Goffart, Woboq GmbH */
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

    template<typename... Ts, typename T, std::size_t...I>
    constexpr auto tuple_append_helper(const tuple<Ts...> &tuple_, const T &t, std::index_sequence<I...>) {
        return tuple<Ts..., T>{get<I>(tuple_)... , t};
    }
    template<typename T1, typename T>
    constexpr auto tuple_append(const T1 &tuple, const T &t) {
        return tuple_append_helper(tuple, t, std::make_index_sequence<tuple_size<T1>::value>());
    }

}


template<typename... Ts, typename T, std::size_t...I>
constexpr auto tuple_append_helper(const std::tuple<Ts...> &tuple_, const T &t, std::index_sequence<I...>) {
    return std::tuple<Ts..., T>{std::get<I>(tuple_)... , t};
}
template<typename... Ts, typename T>
constexpr auto tuple_append(const std::tuple<Ts...> &tuple, const T &t) {
    return tuple_append_helper(tuple, t, std::make_index_sequence<sizeof...(Ts)>());
}




namespace binary {

    template <typename T> struct Leaf {
        T data;
        static constexpr int Depth = 0;
        static constexpr int Count = 1;
        static constexpr bool Balanced = true;
    };

    template <class A, class B> struct Node {
        A a;
        B b;
        static constexpr int Count = A::Count + B::Count;
        static constexpr int Depth = A::Depth + 1;
        static constexpr bool Balanced = A::Depth == B::Depth && B::Balanced;
    };

    /* Add the node 'N' to thre tree 'T' */
    template <class T, typename N, bool Balanced = T::Balanced > struct Add {
        typedef Node<T, Leaf<N> > Result;
        static constexpr Result add(T t, N n) { return {t, n}; }
    };
    template <class A, class B, typename N> struct Add<Node<A, B>, N, false> {
        typedef Node<A, typename Add<B, N>::Result > Result;
        static constexpr Result add(Node<A, B> t, N n) { return {t.a , Add<B, N>::add(t.b, n) }; }
    };

    template <class T, int I, typename = void> struct Get;
    template <class N> struct Get<Leaf<N>, 0> {
        static constexpr N get(Leaf<N> t) { return t.data; }
    };
    template <class A, class B, int I> struct Get<Node<A,B>, I, std::enable_if_t<(A::Count <= I)>> {
        static constexpr auto get(Node<A,B> t) { return Get<B,I - A::Count>::get(t.b); }
    };
    template <class A, class B, int I> struct Get<Node<A,B>, I, std::enable_if_t<(A::Count > I)>> {
        static constexpr auto get(Node<A,B> t) { return Get<A,I>::get(t.a); }
    };



    template<typename T = void>
    template<typename T> struct tuple {
        static constexpr int size = T::Count;
        T root;
    };
    struct tuple<> { static constexpr int size = 0; };

    template<typename T> struct tuple_size { static constexpr int value = T::size; };
    //template<typename... Ts> tuple<Ts...> make_tuple(Ts... ts) { return {ts...}; }

    template<typename N>
    constexpr tuple<Leaf<N>> tuple_append(tuple<>, N n)
    { return {{n}}; }
    template<typename Root, typename N>
    constexpr tuple<typename Add<Root,N>::Result> tuple_append(tuple<Root> t, N n)
    { return {Add<Root,N>::add(t.root,n)}; }

    template<int N, typename Root> constexpr auto get(tuple<Root> t)
    { return Get<Root, N>::get(t.root); }

    template<int I, typename T> using tree_element_t = decltype(get<I>(std::declval<T>()));

}



using NAMESPACE::tuple;
using NAMESPACE::tuple_size;
using NAMESPACE::get;





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
