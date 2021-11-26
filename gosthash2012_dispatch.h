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

#ifdef __GOST3411_HAS_SSE2__
/* Construct SSE2 implementation. */
# define g g_sse2
# define __GOST3411_USE_SSE2__
# include "gosthash2012_sse2.h"
# include "gosthash2012_g.h"
# undef LOAD
# undef STORE
# undef X128R
# undef X128M
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
#if defined __GOST3411_HAS_SSE2__
    g_sse2(h, N, m);
#elif defined  __GOST3411_HAS_REF__
    g_ref(h, N, m);
#else
# error "No implementation of g() is selected."
#endif
}
