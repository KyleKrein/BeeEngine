//
// Created by alexl on 17.08.2023.
//

#pragma once

namespace BeeEngine
{
    template <typename... Ts>
    struct TypeSequence
    {
    };

    template <typename... Ts>
    struct Head;
    template <typename T, typename... Ts>
    struct Head<TypeSequence<T, Ts...>>
    {
        using Type = T;
    };

    template <typename T>
    using Head_t = typename Head<T>::type;

    template <typename... Ts>
    struct Tail;

    template <typename T, typename... Ts>
    struct Tail<TypeSequence<T, Ts...>>
    {
        using Type = TypeSequence<Ts...>; // AHA!
    };
    template <typename T>
    using Tail_t = typename Tail<T>::type;

    template <typename T, typename List>
    struct Cons;
    template <typename T, typename... Ts>
    struct Cons<T, TypeSequence<Ts...>>
    {
        using Type = TypeSequence<T, Ts...>;
    };
    template <typename T, typename List>
    using Cons_t = typename Cons<T, List>::Type;
} // namespace BeeEngine