//
// Created by alexl on 06.11.2023.
//

#pragma once

// Макрос для ALWAYS_INLINE
#if defined(_MSC_VER)
#define BEE_ALWAYS_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define BEE_ALWAYS_INLINE __attribute__((always_inline)) inline
#else
#define BEE_ALWAYS_INLINE inline
#endif

// Макрос для NEVER_INLINE
#if defined(_MSC_VER)
#define BEE_NEVER_INLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
#define BEE_NEVER_INLINE __attribute__((noinline))
#else
#define BEE_NEVER_INLINE
#endif
