/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef BITS_BYTES_MACROS_INCLUDED
/* { */
#define BITS_BYTES_MACROS_INCLUDED



#define BITS_IN_BYTE    8
#define BITS_IN_LONG    32


#ifndef __KERNEL__ /* for kernel, already defined in bitops.h */
#define BIT(x)          (1<<(x))
#endif
#define RBIT(x)         (~(1<<(x)))

#define VAL_MIN(x,y) ( (x)<(y)? (x) : (y) )

/*******************************************
 * SET ALL BITS, BEGINNING AT BIT X, TO 1  *
 */
#define FROM_BIT(x)     (RBIT(x) + 1)

/*******************************************
 * SET ALL BITS UP TO BIT X (NOT INCLUDING *
 * BIT X) TO 1.                            *
 */
#define UPTO_BIT(x)     (~(FROM_BIT(x)))

/***************************************************
* MACROS FOR ACCESSING SPECIFIC BYTES              *
 */

/***************************************************
 * MACROS TO COMPUTE THE UI BITS              *
 */
/* All the bits up until bit x with a verification if x > 31 */
#define UNTIL_BIT(x)               (((x)>= (BITS_IN_LONG-1))? 0xffffffff : ((unsigned  long) (1<<((VAL_MIN((x)%BITS_IN_LONG, 30))+1))-1))
/* All the bits from y to x */
#define BIT_UP_RANGE(x,y)          (UNTIL_BIT(x)-UNTIL_BIT(y))

/* For the UI, the right output in the bit entry for a global number num */
#define BIT_N(num, bit) \
  ((num >= (BITS_IN_LONG * (bit+1))) || (num < (BITS_IN_LONG * bit))) ? 0 : BIT(num % BITS_IN_LONG) 

/* The 16-entry array for the UI function or parameter*/
#define BIT_UI_16(num) \
 {BIT_N(num, 0), BIT_N(num, 1), BIT_N(num, 2), BIT_N(num, 3), BIT_N(num, 4), BIT_N(num, 5), BIT_N(num, 6), BIT_N(num, 7), BIT_N(num, 8), BIT_N(num, 9), BIT_N(num, 10), BIT_N(num, 11), BIT_N(num, 12), BIT_N(num, 13), BIT_N(num, 14), BIT_N(num, 15)}

#define BIT_N_RNG(nmin, nmax, bit) \
  (nmax < (BITS_IN_LONG * bit)) ? 0 : ((nmin >= (BITS_IN_LONG * (bit+1))) ? 0 : ((nmin < (BITS_IN_LONG * bit))? UNTIL_BIT((nmax>=(BITS_IN_LONG * (bit+1)))? BITS_IN_LONG:(nmax%BITS_IN_LONG)) : BIT_UP_RANGE(((nmax>=(BITS_IN_LONG * (bit+1)))? BITS_IN_LONG:(nmax%BITS_IN_LONG)),(nmin%BITS_IN_LONG))))

#define BIT_UI_16_RNG(nmin, nmax) \
  {BIT_N_RNG(nmin, nmax, 0), BIT_N_RNG(nmin, nmax, 1), BIT_N_RNG(nmin, nmax, 2), BIT_N_RNG(nmin, nmax, 3), BIT_N_RNG(nmin, nmax, 4), BIT_N_RNG(nmin, nmax, 5), BIT_N_RNG(nmin, nmax, 6), BIT_N_RNG(nmin, nmax, 7), BIT_N_RNG(nmin, nmax, 8), BIT_N_RNG(nmin, nmax, 9), BIT_N_RNG(nmin, nmax, 10), BIT_N_RNG(nmin, nmax, 11), BIT_N_RNG(nmin, nmax, 12), BIT_N_RNG(nmin, nmax, 13), BIT_N_RNG(nmin, nmax, 14), BIT_N_RNG(nmin, nmax, 15)}

/**************************************************************
 * MACRO TO GET THE OFFSET OF AN ELEMENT IN A                 *
 * GIVEN STRUCTURE TYPE.                                      *
 * 'X' IS THE TYPE OF THE STRUCTURE.                          *
 * 'Y' IS THE THE ELEMENT (IN THE STRUCTURE) WHOSE            *
 *     OFFSET WE NEED.                                        *
 * THE OFFSET RETURNS AS 'unsigned long'.                     *
 */
#define OFFSETOF(x,y) ((unsigned long)(&(((x *)0)->y)))
/*
 * Macros related to handling of 32 bits registers.
 * {
 */
/*
 * Take value and put it in its proper location within a 'long'
 * register (and make sure it does not effect bits outside its
 * predefined mask).
 */
/*
 * Take value and add it in its proper location within a 'long'
 * register ('target') and make sure it does not effect bits outside its
 * predefined mask.
 */
/*
 * Get a value out of location within a 'long' register (and make sure it
 * is not effected by bits outside its predefined mask).
 */
/*
 * }
 */
/**************************************************************
 * MACRO TO GET THE SIZE OF AN ELEMENT IN A                   *
 * GIVEN STRUCTURE TYPE.                                      *
 * 'X' IS THE TYPE OF THE STRUCTURE.                          *
 * 'Y' IS THE  ELEMENT (IN THE STRUCTURE) WHOSE               *
 *     SIZE (IN BYTES) WE NEED.                               *
 * THE SIZE RETURNS AS 'unsigned long'.                       *
 */
#define SIZEOF(x,y) ((unsigned long)(sizeof(((x *)0)->y)))
/**************************************************************
 * MACRO TO CONVERT BIG ENDIAN LONG VARIABLE TO LITTLE ENDIAN *
 * 'X' IS THE INPUT VARIABLE.                                 *
 * 'Y' IS THE OUTPUT VARIABLE                                 *
 */

/**************************************************************
 * MACRO TO CONVERT littel ENDIAN LONG VARIABLE TO BIG ENDIAN *
 * 'X' IS THE INPUT VARIABLE.                                 *
 * 'Y' IS THE OUTPUT VARIABLE                                 *
 */

/* } */
#endif


