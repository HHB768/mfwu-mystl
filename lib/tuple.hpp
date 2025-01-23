#ifndef __TUPLE_HPP__
#define __TUPLE_HPP__


namespace mfwu {

template <typename ThisArg, typename... RestArgs>
class tuple : private tuple<RestArgs...> {
public:
    using this_type = ThisArg;
    using base_type = tuple<RestArgs>;

    template <typename ThisArg, typename... RestArgs>
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


}  // endof namespace mfwu


#endif  // __TUPLE_HPP__