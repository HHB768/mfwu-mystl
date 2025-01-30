#ifndef __TUPLE_HPP__
#define __TUPLE_HPP__

#include "common.hpp"

namespace mfwu {

template<typename... Ty> class tuple;

template <>
class tuple<> {
public:
    bool equal(const tuple& other) const {
        return true;
    }
};

template <typename ThisArg, typename... RestArgs>
class tuple<ThisArg, RestArgs...> : public tuple<RestArgs...> {
public:
    using this_type = ThisArg;
    using base_type = tuple<RestArgs...>;

    tuple() = default;
    tuple(ThisArg&& this_arg, RestArgs&&... rest_args)
        : base_type(mfwu::forward<RestArgs>(rest_args)...),
          first_(mfwu::forward<ThisArg>(this_arg)) {}
    

    base_type& get_rest() { return *this; }
    const base_type& get_rest() const { return *this; }

    // template <size_t _Index, class... _Types>
    // friend constexpr tuple_element_t<_Index, tuple<_Types...>>& get(tuple<_Types...>& _Tuple) noexcept;
 
    // template <size_t _Index, class... _Types>
    // friend constexpr const tuple_element_t<_Index, tuple<_Types...>>& get(const tuple<_Types...>& _Tuple) noexcept;
 
    // template <size_t _Index, class... _Types>
    // friend constexpr tuple_element_t<_Index, tuple<_Types...>>&& get(tuple<_Types...>&& _Tuple) noexcept;
 
    // template <size_t _Index, class... _Types>
    // friend constexpr const tuple_element_t<_Index, tuple<_Types...>>&& get(const tuple<_Types...>&& _Tuple) noexcept;
 
    // template <size_t _Index, class... _Types>
    // friend constexpr auto&& _Tuple_get(tuple<_Types...>&& _Tuple) noexcept;
 
    // template <class _Ty, class... _Types>
    // friend constexpr _Ty& get(tuple<_Types...>& _Tuple) noexcept;
 
    // template <class _Ty, class... _Types>
    // friend constexpr const _Ty& get(const tuple<_Types...>& _Tuple) noexcept;
 
    // template <class _Ty, class... _Types>
    // friend constexpr _Ty&& get(tuple<_Types...>&& _Tuple) noexcept;
 
    // template <class _Ty, class... _Types>
    // friend constexpr const _Ty&& get(const tuple<_Types...>&& _Tuple) noexcept;
    template <typename... T2>
    bool equal(const tuple<T2...>& other) const {
        return first_ == other.first_ && base_type::equal(other.get_rest());
    }

    this_type first_;
    // _Tuple_val<this_type>

};  // endof class tuple

template <int idx, typename _Tuple>
struct tuple_element {
    using type = typename tuple_element<idx - 1, typename _Tuple::base_type>::type;
};  // endof struct tuple_element

// template <typename _Tuple>
// struct tuple_element<0, _Tuple> {
//     using type = _Tuple;
// };  // endof struct tuple_element<0, ...>
// NOT SUITABLE bcz structural binding needs a std::tuple_element that returns element type

// template <int idx, typename _Tuple>
// constexpr auto& get(_Tuple& t) {
//     using type = typename tuple_element<idx, _Tuple>::type;
//     return static_cast<type&>(t).first_;
// } 
// template <int idx, typename _Tuple>
// constexpr const auto& get(const _Tuple& t) {
//     using type = typename tuple_element<idx, _Tuple>::type;
//     return static_cast<const type&>(t).first_;
// }
// template <int idx, typename _Tuple>
// constexpr auto&& get(_Tuple&& t) {
//     using type = typename tuple_element<idx, _Tuple>::type;
//     return static_cast<type&&>(t).first_;
// }

template <typename _Tuple>
struct tuple_element<0, _Tuple> {
    using type = decltype(_Tuple::first_);
};  // endof struct tuple_element<0, ...>

// 🔹 Corrected `get<N>` using inheritance  (by GPT 25.01.31)
template <int idx, typename Tuple>
constexpr auto& get(Tuple& t) {
    if constexpr (idx == 0) {
        return t.first_;  // ✅ Base case
    } else {
        return get<idx - 1>(static_cast<typename Tuple::base_type&>(t));
    }
}

template <int idx, typename Tuple>
constexpr const auto& get(const Tuple& t) {
    if constexpr (idx == 0) {
        return t.first_;
    } else {
        return get<idx - 1>(static_cast<const typename Tuple::base_type&>(t));
    }
}

template <int idx, typename Tuple>
constexpr auto&& get(Tuple&& t) {
    if constexpr (idx == 0) {
        return std::forward<decltype(t.first_)>(t.first_);
    } else {
        return get<idx - 1>(static_cast<typename Tuple::base_type&&>(t));
    }
}

template <typename... T1, typename... T2>
bool operator==(const tuple<T1...>& lhs, const tuple<T2...>& rhs) {
    return lhs.equal(rhs);
}

template <typename... Args>
struct tuple {};  // Your tuple-like type

template <typename T>
struct tuple_size;  

template <typename... Args>
struct tuple_size<tuple<Args...>> : std::integral_constant<std::size_t, sizeof...(Args)> {};


// template <typename First, typename... Rest>
// struct tuple_size<mfwu::tuple<First, Rest...>>
//     : std::integral_constant<std::size_t, 1 + tuple_size<mfwu::tuple<Rest...>>::value> {};

// // Base case: Empty tuple
// template <>
// struct tuple_size<mfwu::tuple<>> : std::integral_constant<std::size_t, 0> {};

}  // endof namespace mfwu

namespace std {
    template <std::size_t N, typename Tuple>
    struct tuple_element
        : mfwu::tuple_element<N, Tuple> {};  // Redirect to your custom tuple_element

    template <typename... Args>
    struct tuple_size<mfwu::tuple<Args...>> : mfwu::tuple_size<mfwu::tuple<Args...>> {};
}
#endif  // __TUPLE_HPP__

/*
LOL GIT PUSH FAILURE:

gpt ans:
Why It Worked with HTTPS Before
Your setup likely relied on an implicit fallback where:

Git used SSH keys for authentication despite an HTTPS URL, possibly with some VS Code logic or configuration bridging the two.
git.terminalAuthentication forced HTTPS prompts, which became problematic if no valid credentials were available.
Once you disabled the setting, this interference ceased, and Git's native behavior (possibly SSH authentication) took over.

25.01.24
*/

// i want to implement structural binding but failed... 25.01.30
// GPT TIPS:
// Does This Differ Among Compilers?
// No, this is standardized behavior in C++17. 
// All conforming compilers (GCC, Clang, MSVC) follow the same ADL rules.
// OK, I WILL DO THIS

// ok i can do that 25.01.31