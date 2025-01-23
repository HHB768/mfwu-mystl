#ifndef __TUPLE_HPP__
#define __TUPLE_HPP__


namespace mfwu {

template <typename ThisArg, typename... RestArgs>
class tuple : private tuple<RestArgs...> {
public:
    using this_type = ThisArg;
    using base_type = tuple<RestArgs...>;

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

    this_type first_;
    // _Tuple_val<this_type>

};  // endof class tuple

template <int idx, typename _Tuple>
struct tuple_element {
    using type = typename tuple_element<idx - 1, typename _Tuple::base_type>::type;
};  // endof struct tuple_element

template <typename _Tuple>
struct tuple_element<0, _Tuple> {
    using type = _Tuple;
};  // endof struct tuple_element<0, ...>

template <int idx, typename _Tuple>
constexpr auto& get(_Tuple& t) {
    using type = typename tuple_element<idx, _Tuple>::type;
    return static_cast<type&>(t).first_;
} 


}  // endof namespace mfwu


#endif  // __TUPLE_HPP__