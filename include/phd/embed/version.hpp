//   ----------
// | phd::embed |
//   ----------

// Written in 2026 by Björkus "ThePhD" Dorkus <phdofthehouse@gmail.com>

// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.

// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

#ifndef PHD_EMBED_VERSION_HPP
#define PHD_EMBED_VERSION_HPP

#pragma once

#define PHD_EMBED_ON          |
#define PHD_EMBED_OFF         ^
#define PHD_EMBED_DEFAULT_ON  +
#define PHD_EMBED_DEFAULT_OFF -

#define PHD_EMBED_RAW_IS_ON(OP_SYMBOL) ((3 OP_SYMBOL 3) != 0)
#define PHD_EMBED_RAW_IS_OFF(OP_SYMBOL) ((3 OP_SYMBOL 3) == 0)
#define PHD_EMBED_RAW_IS_DEFAULT_ON(OP_SYMBOL) ((3 OP_SYMBOL 3) > 3)
#define PHD_EMBED_RAW_IS_DEFAULT_OFF(OP_SYMBOL) ((3 OP_SYMBOL 3 OP_SYMBOL 3) < 0)

#define PHD_EMBED_IS_ON(OP_SYMBOL) PHD_EMBED_RAW_IS_ON(OP_SYMBOL ## _I)
#define PHD_EMBED_IS_OFF(OP_SYMBOL) PHD_EMBED_RAW_IS_OFF(OP_SYMBOL ## _I)
#define PHD_EMBED_IS_DEFAULT_ON(OP_SYMBOL) PHD_EMBED_RAW_IS_DEFAULT_ON(OP_SYMBOL ## _I)
#define PHD_EMBED_IS_DEFAULT_OFF(OP_SYMBOL) PHD_EMBED_RAW_IS_DEFAULT_OFF(OP_SYMBOL ## _I)

#if !defined(PHD_EMBED_HAS_INCLUDE)
#if defined(__has_include)
#define PHD_EMBED_HAS_INCLUDE(...) __has_include(__VA_ARGS__)
#else
#define PHD_EMBED_HAS_INCLUDE(...) 0
#endif // __has_include ability
#endif // undefined PHD_EMBED_HAS_INCLUDE

#if !defined(PHD_EMBED_HAS_BUILTIN)
#if defined(__has_include)
#define PHD_EMBED_HAS_BUILTIN(...) __has_builtin(__VA_ARGS__)
#else
#define PHD_EMBED_HAS_BUILTIN(...) 0
#endif // __has_builtin ability
#endif // undefined PHD_EMBED_HAS_BUILTIN

#if defined(PHD_EMBED_HAS_INCLUDE) && (PHD_EMBED_HAS_INCLUDE(<version>))
#include <version>
#endif // <version> check

#if defined(PHD_EMBED_HAS_BUILTIN_STD_EMBED)
#if PHD_EMBED_HAS_BUILTIN_STD_EMBED != 0
#define PHD_EMBED_HAS_BUILTIN_STD_EMBED_I PHD_EMBED_ON
#else
#define PHD_EMBED_HAS_BUILTIN_STD_EMBED_I PHD_EMBED_OFF
#endif
#elif PHD_EMBED_HAS_BUILTIN(__builtin_std_embed)
#define PHD_EMBED_HAS_BUILTIN_STD_EMBED_I PHD_EMBED_DEFAULT_ON
#else
#define PHD_EMBED_HAS_BUILTIN_STD_EMBED_I PHD_EMBED_DEFAULT_OFF
#endif // __has_builtin test

#if defined(PHD_EMBED_USE_STD_EMBED)
#if PHD_EMBED_USE_STD_EMBED != 0
#define PHD_EMBED_USE_STD_EMBED_I PHD_EMBED_ON
#else
#define PHD_EMBED_USE_STD_EMBED_I PHD_EMBED_OFF
#endif
#elif (defined(__cpp_lib_embed) && (__cpp_lib_embed >= 202611L)) && PHD_EMBED_HAS_INCLUDE(<embed>)
#define PHD_EMBED_USE_STD_EMBED_I PHD_EMBED_DEFAULT_ON
#else
#define PHD_EMBED_USE_STD_EMBED_I PHD_EMBED_DEFAULT_OFF
#endif

#if defined(PHD_EMBED_HAS_BUILTIN_ABORT)
#if defined(PHD_EMBED_HAS_BUILTIN_ABORT) && PHD_EMBED_HAS_BUILTIN_ABORT != 0
#define PHD_EMBED_HAS_BUILTIN_ABORT_I PHD_EMBED_ON
#else
#define PHD_EMBED_HAS_BUILTIN_ABORT_I PHD_EMBED_OFF
#endif
#elif PHD_EMBED_HAS_BUILTIN(__builtin_abort)
#define PHD_EMBED_HAS_BUILTIN_ABORT_I PHD_EMBED_DEFAULT_ON
#else
#define PHD_EMBED_HAS_BUILTIN_ABORT_I PHD_EMBED_DEFAULT_OFF
#endif

#if defined(PHD_EMBED_HAS_CONSTEXPR_EXCEPTIONS)
#if defined(PHD_EMBED_HAS_CONSTEXPR_EXCEPTIONS) && PHD_EMBED_HAS_CONSTEXPR_EXCEPTIONS != 0
#define PHD_EMBED_HAS_CONSTEXPR_EXCEPTIONS_I PHD_EMBED_ON
#else
#define PHD_EMBED_HAS_CONSTEXPR_EXCEPTIONS_I PHD_EMBED_OFF
#endif
#elif defined(__cpp_exceptions) && (__cpp_exceptions != 0) && defined(__cpp_constexpr_exceptions) \
     && (__cpp_constexpr_exceptions) != 0 && defined(__cpp_lib_constexpr_exceptions)              \
     && (__cpp_lib_constexpr_exceptions) != 0
#define PHD_EMBED_HAS_CONSTEXPR_EXCEPTIONS_I PHD_EMBED_DEFAULT_ON
#else
#define PHD_EMBED_HAS_CONSTEXPR_EXCEPTIONS_I PHD_EMBED_DEFAULT_OFF
#endif

#if defined(PHD_EMBED_HAS_BUILTIN_SOURCE_LOCATION_AT)
#if defined(PHD_EMBED_HAS_BUILTIN_SOURCE_LOCATION_AT) && PHD_EMBED_HAS_BUILTIN_SOURCE_LOCATION_AT != 0
#define PHD_EMBED_HAS_BUILTIN_SOURCE_LOCATION_AT_I PHD_EMBED_ON
#else
#define PHD_EMBED_HAS_BUILTIN_SOURCE_LOCATION_AT_I PHD_EMBED_OFF
#endif
#elif PHD_EMBED_HAS_BUILTIN(__builtin_source_location_at)
#define PHD_EMBED_HAS_BUILTIN_SOURCE_LOCATION_AT_I PHD_EMBED_DEFAULT_ON
#else
#define PHD_EMBED_HAS_BUILTIN_SOURCE_LOCATION_AT_I PHD_EMBED_DEFAULT_OFF
#endif

#endif
