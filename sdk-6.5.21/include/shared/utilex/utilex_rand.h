/** \file utilex_rand.h
 * 
 * Random numbers generator for math utils. 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef UTILEX_RAND_H_INCLUDED
/* { */
#define UTILEX_RAND_H_INCLUDED

#ifdef  __cplusplus
extern "C"
{
#endif

#include <shared/utilex/utilex_framework.h>
#include <shared/shrextend/shrextend_error.h>
/*
 * Random constants - Do not edit.
 * {
 */
    /*
     * Random array of 351 long
     */
#define UTILEX_RAND_N   351
#define UTILEX_RAND_M   175
#define UTILEX_RAND_MATRIX_A  0xEABD75F5
#define UTILEX_RAND_TEMU  11
#define UTILEX_RAND_TEMS  7
#define UTILEX_RAND_TEMT  15
#define UTILEX_RAND_TEML  17
#define UTILEX_RAND_TEMB  0x655E5280UL
#define UTILEX_RAND_TEMC  0xFFD58000
/*
 * }
 */

/*
 * Random status structure
 */
    typedef struct
    {
        uint32 state[UTILEX_RAND_N];    /* the array for the state vector */
        uint32 left;            /* how many lest in state */
        uint32 initf;           /* is initialized */
        uint32 *next;

        /*
         */
        uint32 seed;
    } UTILEX_RAND;

/*
 * {
 * UTILEX_RAND APIs.
 * These function are basic random functions.
 */

/*
 * Various utilex_rand constants
 */
    enum
    {
        UTILEX_RAND_PERCENT_ONE = 10000,
        UTILEX_RAND_PERCENT_100 = 100 * UTILEX_RAND_PERCENT_ONE
    };

/*****************************************************
*NAME:
* utilex_rand_seed_set
* utilex_rand_seed_get
*FUNCTION:
*   Set/Get seed for random.
*INPUT:
*  UTILEX_DIRECT:
*    UTILEX_RAND     *r         -
*      Random structure
*     uint32 seed -
*      seed value
*OUTPUT:
*  UTILEX_DIRECT:
*    None
*  UTILEX_INDIRECT:
*    Random structure
 */
    void utilex_rand_seed_set(
    UTILEX_RAND * r,
    uint32 seed);
    uint32 utilex_rand_seed_get(
    UTILEX_RAND * r);

/*****************************************************
*NAME:
* utilex_rand_coin_1
*FUNCTION:
*   Get 0/1 based on random coin fliping with positive bias
*INPUT:
*  UTILEX_DIRECT:
*    UTILEX_RAND     *r         -
*      Random structure
*      uint32 positive_bias -
*      Positive bias in precents.
*      Assuming 0<=positive_bias<=100*UTILEX_RAND_ONE_PERCENT
*OUTPUT:
*  UTILEX_DIRECT:
*    None
*  UTILEX_INDIRECT:
*    Random structure
 */
    uint32 utilex_rand_coin_1(
    UTILEX_RAND * r,
    uint32 positive_bias);

/*****************************************************
*NAME:
* utilex_rand_coin_2
*FUNCTION:
*   Get 0/1 based on random coin flipping with
*  positive/negative bias
*INPUT:
*  UTILEX_DIRECT:
*    UTILEX_RAND     *r         -
*      Random structure
*      uint32 positive_bias -
*      Positive bias in precents.
*      Assuming 0<=positive_bias<=100*UTILEX_RAND_ONE_PERCENT
*      uint32 negative_bias -
*      negative bias in precents.
*      Assuming 0<=negative_bias<=100*UTILEX_RAND_ONE_PERCENT
*OUTPUT:
*  UTILEX_DIRECT:
*    None
*  UTILEX_INDIRECT:
*    Random structure
 */
    uint32 utilex_rand_coin_2(
    UTILEX_RAND * r,
    uint32 positive_bias,
    uint32 negative_bias);

/*****************************************************
*NAME:
* utilex_rand_range
*FUNCTION:
*   Get a number in the range min<=x<=max
*INPUT:
*  UTILEX_DIRECT:
*    UTILEX_RAND     *r         -
*      Random structure
*        uint32 min -
*      Min value
*        uint32 max -
*      Max value
*OUTPUT:
*  UTILEX_DIRECT:
*    uint32
*  UTILEX_INDIRECT:
*    Random structure
 */
    uint32 utilex_rand_range(
    UTILEX_RAND * r,
    uint32 min,
    uint32 max);

/*****************************************************
*NAME:
* utilex_rand_range
*FUNCTION:
*   Get a random number in the range
*  0<=x<=Max-integer-positive-value
*INPUT:
*  UTILEX_DIRECT:
*    UTILEX_RAND     *r         -
*      Random structure
*OUTPUT:
*  UTILEX_DIRECT:
*    int
*  UTILEX_INDIRECT:
*    Random structure
 */
    int utilex_rand_int(
    UTILEX_RAND * r);

/*****************************************************
*NAME:
* utilex_rand_u_long
*FUNCTION:
*   Get a number of 32 bits
*INPUT:
*  UTILEX_DIRECT:
*    UTILEX_RAND     *r         -
*      Random structure
*OUTPUT:
*  UTILEX_DIRECT:
*    int
*  UTILEX_INDIRECT:
*    Random structure
 */
    uint32 utilex_rand_u_long(
    UTILEX_RAND * r);

/*****************************************************
*NAME:
* utilex_rand_u_long
*FUNCTION:
*   Get a random number in the range o and m-1
*INPUT:
*  UTILEX_DIRECT:
*    UTILEX_RAND     *r         -
*      Random structure
*        uint32 m -
*      Module number
*OUTPUT:
*  UTILEX_DIRECT:
*    int
*  UTILEX_INDIRECT:
*    Random structure
 */
    uint32 utilex_rand_modulo(
    UTILEX_RAND * r,
    uint32 m);

/*
 * }
 */

/*
 * {
 * UTILEX_RAND more APIs.
 * These function are more complicated random functions.
 */

/*****************************************************
*NAME:
* utilex_rand_array_permute_u_long
*FUNCTION:
*   Given array of u-longs 'array', the function will
*  generate one permutation of this array.
*INPUT:
*  UTILEX_DIRECT:
*    UTILEX_RAND     *r         -
*      Random structure
*    UTILEX_INOUT uint32 array[],
*      Array of 'unsigned-longs'.
*        uint32 array_size
*      Number of 'u-long' in 'array'
*OUTPUT:
*  UTILEX_INDIRECT:
*    + 'array' will be changed.
*    + Random structure
 */
    void utilex_rand_array_permute_u_long(
    UTILEX_INOUT UTILEX_RAND * r,
    UTILEX_INOUT uint32 array[],
    uint32 array_size);

/*****************************************************
*NAME:
* utilex_rand_array_permute_u_long
*FUNCTION:
*   Given array of u-longs 'array', the function will
*  generate one permutation of this array.
*INPUT:
*  UTILEX_DIRECT:
*    UTILEX_INOUT UTILEX_RAND               *r,
*      Random structure
*    UTILEX_INOUT unsigned char           array[],
*      Array of 'STRUCTUTE'.
*        uint32           nof_items,
*      Number of 'STRUCTUTE' in 'array'
*        UTILEX_RAND_SWAP_ARR_FUNC swap
*      Swap function of 'STRUCTUTE' in 'array'.
*OUTPUT:
*  UTILEX_INDIRECT:
*    + 'array' will be changed.
*    + Random structure
*    + 'swap' will be called approx. nof_items
*SEE ALSO:
*   Implementation of 'utilex_rand_array_permute_u_long()'.
 */
/*
 */
    typedef void (
    *UTILEX_RAND_SWAP_ARR_FUNC) (
    unsigned char array[],
    uint32 indx_1,
    uint32 indx_2);
/*
 */
    void utilex_rand_array_permute(
    UTILEX_INOUT UTILEX_RAND * r,
    UTILEX_INOUT unsigned char array[],
    uint32 nof_items,
    UTILEX_RAND_SWAP_ARR_FUNC swap);

/*
 * }
 */

/*prints the seed*/
    void utilex_rand_print(
    UTILEX_RAND * r);

    int utilex_rand_test(
    uint32);

#ifdef  __cplusplus
}
#endif

/* } UTILEX_RAND_H_INCLUDED*/
#endif
