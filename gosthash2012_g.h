/*
 * GOST R 34.11-2012 Multi-arch template of g() function.
 *
 * Copyright (c) 2013 Alexey Degtyarev <alexey@renatasystems.org>.
 * Copyright (c) 2013 Cryptocom LTD.
 *
 * This file is distributed under the same license as OpenSSL.
 *
 * Implementation notes:
 *
 * Should have already included "gosthash2012_{ref,sse2}.h"
 * Control define __GOST3411_USE_SSE2__ (Note '_USE_' not '_HAS_').
 */
static void g(union uint512_u *h, const union uint512_u * RESTRICT N,
              const union uint512_u * RESTRICT m)
{
#if defined __GOST3411_USE_SSE2__
    __m128i xmm0, xmm2, xmm4, xmm6; /* XMMR0-quadruple */
    __m128i xmm1, xmm3, xmm5, xmm7; /* XMMR1-quadruple */
    unsigned int i;

    LOAD(N, xmm0, xmm2, xmm4, xmm6);
    XLPS128M(h, xmm0, xmm2, xmm4, xmm6);

    ULOAD(m, xmm1, xmm3, xmm5, xmm7);
    XLPS128R(xmm0, xmm2, xmm4, xmm6, xmm1, xmm3, xmm5, xmm7);

    for (i = 0; i < 11; i++)
        ROUND128(i, xmm0, xmm2, xmm4, xmm6, xmm1, xmm3, xmm5, xmm7);

    XLPS128M((&C[11]), xmm0, xmm2, xmm4, xmm6);
    X128R(xmm0, xmm2, xmm4, xmm6, xmm1, xmm3, xmm5, xmm7);

    X128M(h, xmm0, xmm2, xmm4, xmm6);
    ULOAD(m, xmm1, xmm3, xmm5, xmm7);
    X128R(xmm0, xmm2, xmm4, xmm6, xmm1, xmm3, xmm5, xmm7);

    STORE(h, xmm0, xmm2, xmm4, xmm6);
# ifdef __i386__
    /* Restore the Floating-point status on the CPU */
    /* This is only required on MMX, but EXTRACT32 is using MMX */
    _mm_empty();
# endif
#else
    union uint512_u Ki, data;
    unsigned int i;

    XLPS(h, N, (&data));

    /* Starting E() */
    Ki = data;
    XLPS((&Ki), ((const union uint512_u *)&m[0]), (&data));

    for (i = 0; i < 11; i++)
        ROUND(i, (&Ki), (&data));

    XLPS((&Ki), (&C[11]), (&Ki));
    X((&Ki), (&data), (&data));
    /* E() done */

    X((&data), h, (&data));
    X((&data), m, h);
#endif
}
