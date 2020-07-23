/**
 * \file utilex_rand.c
 *  Random numbers generation for Math utils
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>
#include <sal/types.h>

#include <shared/utilex/utilex_rand.h>
#include <shared/utilex/utilex_os_interface.h>

/*
 * Internal implementation of random
 * {
 */

/* Period parameters */
#define UTILEX_RAND_UMASK 0x80000000UL  /* most significant w-r bits */
#define UTILEX_RAND_LMASK 0x7fffffffUL  /* least significant r bits */
#define UTILEX_RAND_MIXBITS(u,v) ( ((u) & UTILEX_RAND_UMASK) | ((v) & UTILEX_RAND_LMASK) )
#define UTILEX_RAND_TWIST(u,v) ((UTILEX_RAND_MIXBITS(u,v) >> 1) ^ ((v)&1UL ? UTILEX_RAND_MATRIX_A : 0UL))

#define UTILEX_RAND_INIT_INDICATOR 0x12FF87A5

/* initializes state[UTILEX_RAND_N] with a seed */
static void
utilex_rand_init(
    UTILEX_RAND * r,
    uint32 s)
{
    int j;

    if (NULL == r)
    {
        goto exit;
    }

    r->state[0] = s & 0xffffffffUL;
    for (j = 1; j < UTILEX_RAND_N; j++)
    {
        r->state[j] = (1812433253UL * (r->state[j - 1] ^ (r->state[j - 1] >> 30)) + j);
        r->state[j] &= 0xffffffffUL;
    }
    r->left = 1;
    r->initf = UTILEX_RAND_INIT_INDICATOR;

exit:
    return;
}

/* if utilex_rand_init() has not been called, */
/* a default initial seed is used           */
static void
utilex_rand_check_init(
    UTILEX_RAND * r)
{

    if (r->initf != UTILEX_RAND_INIT_INDICATOR)
    {
        utilex_rand_init(r, 5489UL);
    }
}

static void
utilex_rand_next_state(
    UTILEX_RAND * r)
{
    uint32 *p = NULL;
    int j;

    if (NULL == r)
    {
        goto exit;
    }

    utilex_rand_check_init(r);

    p = r->state;

    r->left = UTILEX_RAND_N;
    r->next = r->state;

    for (j = UTILEX_RAND_N - UTILEX_RAND_M + 1; --j; p++)
    {
        *p = p[UTILEX_RAND_M] ^ UTILEX_RAND_TWIST(p[0], p[1]);
    }

    for (j = UTILEX_RAND_M; --j; p++)
    {
        *p = p[UTILEX_RAND_M - UTILEX_RAND_N] ^ UTILEX_RAND_TWIST(p[0], p[1]);
    }

    *p = p[UTILEX_RAND_M - UTILEX_RAND_N] ^ UTILEX_RAND_TWIST(p[0], r->state[0]);
exit:
    return;
}

/* generates a random number on [0,0xffffffff]-interval */
static uint32
utilex_rand_genrand_int32(
    UTILEX_RAND * r)
{
    uint32 y = 0;

    if (NULL == r)
    {
        goto exit;
    }

    utilex_rand_check_init(r);

    if (--(r->left) == 0)
    {
        utilex_rand_next_state(r);
    }
    y = *(r->next)++;

    /*
     * Tempering 
     */
    y ^= (y >> UTILEX_RAND_TEMU);
    y ^= (y << UTILEX_RAND_TEMS) & UTILEX_RAND_TEMB;
    y ^= (y << UTILEX_RAND_TEMT) & UTILEX_RAND_TEMC;
    y ^= (y >> UTILEX_RAND_TEML);

exit:
    return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
static uint32
utilex_rand_genrand_int31(
    UTILEX_RAND * r)
{
    uint32 y = 0;

    if (NULL == r)
    {
        goto exit;
    }

    utilex_rand_check_init(r);

    if (--(r->left) == 0)
    {
        utilex_rand_next_state(r);
    }
    y = *(r->next)++;

    /*
     * Tempering 
     */
    y ^= (y >> UTILEX_RAND_TEMU);
    y ^= (y << UTILEX_RAND_TEMS) & UTILEX_RAND_TEMB;
    y ^= (y << UTILEX_RAND_TEMT) & UTILEX_RAND_TEMC;
    y ^= (y >> UTILEX_RAND_TEML);

exit:
    return y >> 1;
}

/*
 * }
 */

/*
 * API of RAND
 * {
 */

/*****************************************************
 * See details in utilex_rand.h
 */
void
utilex_rand_seed_set(
    UTILEX_RAND * r,
    uint32 seed)
{

    if (NULL == r)
    {
        goto exit;
    }

    r->seed = seed;
    utilex_rand_init(r, seed);

exit:
    return;
}

/*****************************************************
 * See details in utilex_rand.h
 */
uint32
utilex_rand_seed_get(
    UTILEX_RAND * r)
{
    uint32 seed = 0;

    if (NULL == r)
    {
        goto exit;
    }
    seed = r->seed;

exit:
    return seed;
}

/*****************************************************
 * See details in utilex_rand.h
 */
uint32
utilex_rand_coin_1(
    UTILEX_RAND * r,
    uint32 positive_bias)
{
    uint32 x = 0;

    if (NULL == r)
    {
        goto exit;
    }

    if (positive_bias >= UTILEX_RAND_PERCENT_100)
    {
        x = 1;
    }
    else
    {
        if (utilex_rand_modulo(r, UTILEX_RAND_PERCENT_100) <= positive_bias)
        {
            x = 1;
        }
        else
        {
            x = 0;
        }
    }

exit:
    return x;
}

/*****************************************************
 * See details in utilex_rand.h
 */
uint32
utilex_rand_coin_2(
    UTILEX_RAND * r,
    uint32 positive_bias,
    uint32 negative_bias)
{
    uint32 x = 0;

    if (NULL == r)
    {
        goto exit;
    }

    if (utilex_rand_modulo(r, positive_bias + negative_bias) <= positive_bias)
    {
        x = 1;
    }
    else
    {
        x = 0;
    }

exit:
    return x;
}

/*****************************************************
 * See details in utilex_rand.h
 */
uint32
utilex_rand_range(
    UTILEX_RAND * r,
    uint32 min,
    uint32 max)
{
    uint32 x = 0;

    if (NULL == r)
    {
        goto exit;
    }

    if (min >= max)
    {
        x = min;
    }
    else
    {
        x = min + utilex_rand_modulo(r, max - min);
    }

exit:
    return x;
}

/*****************************************************
 * See details in utilex_rand.h
 */
int
utilex_rand_int(
    UTILEX_RAND * r)
{
    uint32 x = 0;

    if (NULL == r)
    {
        goto exit;
    }

    x = utilex_rand_genrand_int31(r);

exit:
    return x;
}

/*****************************************************
 * See details in utilex_rand.h
 */
uint32
utilex_rand_u_long(
    UTILEX_RAND * r)
{
    uint32 x = 0;

    if (NULL == r)
    {
        goto exit;
    }

    x = utilex_rand_genrand_int32(r);

exit:
    return x;
}

/*****************************************************
 * See details in utilex_rand.h
 */
uint32
utilex_rand_modulo(
    UTILEX_RAND * r,
    uint32 m)
{
    uint32 x = 0;

    if (NULL == r)
    {
        goto exit;
    }

    x = utilex_rand_genrand_int32(r) % m;

exit:
    return x;
}

/*
 * }
 */

/*
 * {
 * UTILEX_RAND more APIs.
 * These function are more complicated random functions.
 */

static void
utilex_rand_array_permute_u_long_swap(
    unsigned char arr_char[],
    uint32 indx_1,
    uint32 indx_2)
{
    uint32 temp_val;
    uint32 *array = (uint32 *) arr_char;

    temp_val = array[indx_1];
    array[indx_1] = array[indx_2];
    array[indx_2] = temp_val;
}

/*****************************************************
 * See details in utilex_rand.h
 */
void
utilex_rand_array_permute_u_long(
    UTILEX_RAND * r,
    uint32 array[],
    uint32 nof_items)
{

    if ((NULL == r) || (NULL == array))
    {
        goto exit;
    }

    utilex_rand_array_permute(r, (unsigned char *) array, nof_items, utilex_rand_array_permute_u_long_swap);

exit:
    return;
}

/*****************************************************
 * See details in utilex_rand.h
 */
void
utilex_rand_array_permute(
    UTILEX_RAND * r,
    unsigned char array[],
    uint32 nof_items,
    UTILEX_RAND_SWAP_ARR_FUNC swap)
{
    uint32 arr_end;
    uint32 indx_to_switch;

    if ((NULL == r) || (NULL == array) || (NULL == swap))
    {
        goto exit;
    }

    for (arr_end = nof_items; arr_end > 1;)
    {
        indx_to_switch = utilex_rand_modulo(r, arr_end--);
        (*swap) (array, arr_end, indx_to_switch);
    }

exit:
    return;
}

/*****************************************************
 * See details in utilex_rand.h
 */
void
utilex_rand_print(
    UTILEX_RAND * r)
{

    if (NULL == r)
    {
        LOG_CLI((BSL_META("ERROR: utilex_rand_print - got NULL -\n\r")));
        goto exit;
    }
    LOG_CLI((BSL_META("%u"), utilex_rand_seed_get(r)));

exit:
    return;
}

/*****************************************************
 * This is print test. That is, no automatic test for
 * this module.
 */
int
utilex_rand_test(
    uint32 silent)
{
    int i, pass;
    uint32 seed = 0x65464;
    UTILEX_RAND utilex_rand;
#define UTILEX_RAND_TEST_ARR_SIZE 200
    uint32 array[UTILEX_RAND_TEST_ARR_SIZE];

    pass = TRUE;

    utilex_rand_seed_set(&utilex_rand, seed);

    LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("1000 outputs of utilex_rand_u_long()\n")));
    for (i = 0; i < 1000; i++)
    {
        LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("%10u "), utilex_rand_u_long(&utilex_rand)));
        if (i % 5 == 4)
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("\n")));
        }
    }

    LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("1000 outputs of utilex_rand_coin_1(, 50%%)\n")));
    for (i = 0; i < 1000; i++)
    {
        LOG_INFO(BSL_LS_SOCDNX_COMMON,
                 (BSL_META("%1u"), utilex_rand_coin_1(&utilex_rand, 50 * UTILEX_RAND_PERCENT_ONE)));
        if (i % 40 == 39)
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("\n")));
        }
        else
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META(" ")));
        }
    }

    LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("1000 outputs of utilex_rand_coin_2(, 1%%, 10%%)\n")));
    for (i = 0; i < 1000; i++)
    {
        LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("%1u"), utilex_rand_coin_2(&utilex_rand, 1, 10)));
        if (i % 40 == 39)
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("\n")));
        }
        else
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META(" ")));
        }
    }

    LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("2 outputs of utilex_rand_array_permute_u_long()\n")));
    for (i = 0; i < UTILEX_RAND_TEST_ARR_SIZE; i++)
    {
        array[i] = i;
    }

    utilex_rand_array_permute_u_long(&utilex_rand, array, UTILEX_RAND_TEST_ARR_SIZE);
    for (i = 0; i < UTILEX_RAND_TEST_ARR_SIZE; i++)
    {
        LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("%3u"), array[i]));
        if (i % 20 == 19)
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("\n")));
        }
        else
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META(" ")));
        }
    }

    LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("\n")));
    utilex_rand_array_permute_u_long(&utilex_rand, array, UTILEX_RAND_TEST_ARR_SIZE);
    for (i = 0; i < UTILEX_RAND_TEST_ARR_SIZE; i++)
    {
        LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("%3u"), array[i]));
        if (i % 20 == 19)
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("\n")));
        }
        else
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META(" ")));
        }
    }

    return pass;
}
