/*
 * GOST R 34.11-2012.
 * Construct and dispatch multi-arch g() functions.
 *
 * Copyright (c) 2021 Vitaly Chikunov <vt@altlinux.org>.
 *
 * This file is distributed under the same license as OpenSSL.
 */

#if defined(__x86_64__) || defined(__e2k__)
# ifdef _MSC_VER
#  include <intrin.h>
# else
#  include <x86intrin.h>
# endif
#endif

/* '__has_builtin is supported on gcc >= 10, clang >= 3 and icc >= 21.' */
#ifndef __has_builtin
# define __has_builtin(x) 0
#endif

/* Construct MMX implementation. */
#ifdef __GOST3411_HAS_MMX__
# define g g_mmx
# define __GOST3411_USE_MMX__
# if defined(__clang__)
#  pragma clang attribute push (__attribute__((target("mmx"))), apply_to = function)
# elif defined(__GNUC__)
#  pragma GCC push_options
#  pragma GCC target("mmx")
# endif
# include "gosthash2012_mmx.h"
# include "gosthash2012_g.h"
# if defined(__clang__)
#  pragma clang attribute pop
# elif defined(__GNUC__)
#  pragma GCC pop_options
# endif
# undef XLOAD
# undef STORE
# undef TRANSPOSE
# undef XTRANSPOSE
# undef XLPS32
# undef XLPS
# undef __GOST3411_USE_MMX__
# undef g
#endif

/*
 * Construct SSE2 implementation. SSE2 is baseline in x86_64, but a feature
 * on IA-32, thus pass target() for IA-32.
 */
#ifdef __GOST3411_HAS_SSE2__
# define g g_sse2
# define __GOST3411_USE_SSE2__
# if defined(__clang__)
#  pragma clang attribute push (__attribute__((target("sse2"))), apply_to = function)
# elif defined(__GNUC__)
#  pragma GCC push_options
#  pragma GCC target("sse2")
# endif
# include "gosthash2012_sse2.h"
# include "gosthash2012_g.h"
# if defined(__clang__)
#  pragma clang attribute pop
# elif defined(__GNUC__)
#  pragma GCC pop_options
# endif
# undef LOAD
# undef STORE
# undef X128R
# undef X128M
# undef EXTRACT
# undef EXTRACT32
# undef EXTRACT64
# undef XLPS128M
# undef XLPS128R
# undef ROUND128
# undef __GOST3411_USE_SSE2__
# undef g
# ifdef __x86_64__
  /*
   * On x86_64 there is always SSE2, so no need to even build reference
   * implementation. But only if SSE2 is actually compiled, since it could
   * be disabled with -mno-sse2.
   */
#  undef __GOST3411_HAS_REF__
# endif
#endif

/* Construct SSE4.1 implementation. */
#ifdef __GOST3411_HAS_SSE41__
# define g g_sse41
# define __GOST3411_USE_SSE41__
# if defined(__clang__)
#  pragma clang attribute push (__attribute__((target("sse4.1"))), apply_to = function)
# elif defined(__GNUC__)
#  pragma GCC push_options
#  pragma GCC target("sse4.1")
# endif
# include "gosthash2012_sse41.h"
# include "gosthash2012_g.h"
# if defined(__clang__)
#  pragma clang attribute pop
# elif defined(__GNUC__)
#  pragma GCC pop_options
# endif
# undef LOAD
# undef UNLOAD
# undef X128R
# undef X128M
# undef EXTRACT
# undef EXTRACT32
# undef EXTRACT64
# undef XLPS128M
# undef XLPS128R
# undef ROUND128
# undef __GOST3411_USE_SSE41__
# undef g
#endif

#ifdef __GOST3411_HAS_REF__
/* Construct Reference implementation. */
# define g g_ref
# include "gosthash2012_ref.h"
# include "gosthash2012_g.h"
# undef X
# undef XLPS
# undef ROUND
# undef g
#endif /* __GOST3411_HAS_REF__ */

static void g(union uint512_u *h, const union uint512_u * RESTRICT N,
    const union uint512_u * RESTRICT m)
{
#if __has_builtin(__builtin_cpu_supports)
# if defined __GOST3411_HAS_SSE41__
    if (__builtin_cpu_supports("sse4.1"))
	return g_sse41(h, N, m);
# endif
# if defined __GOST3411_HAS_SSE2__
    if (__builtin_cpu_supports("sse2"))
	return g_sse2(h, N, m);
# endif
# if defined __GOST3411_HAS_MMX__
    if (__builtin_cpu_supports("mmx"))
	return g_mmx(h, N, m);
# endif
# if defined __GOST3411_HAS_REF__
    g_ref(h, N, m);
# endif
# if !defined __GOST3411_HAS_SSE41__ && \
    !defined __GOST3411_HAS_SSE2__ && \
    !defined __GOST3411_HAS_REF__
#  error "No dynamic implementation of g() is selected."
# endif
#else /* No dynamic dispatcher. */
# if defined __GOST3411_HAS_SSE2__
    g_sse2(h, N, m);
# elif defined  __GOST3411_HAS_REF__
    g_ref(h, N, m);
# else
#  error "No static implementation of g() is selected."
# endif
#endif
}
