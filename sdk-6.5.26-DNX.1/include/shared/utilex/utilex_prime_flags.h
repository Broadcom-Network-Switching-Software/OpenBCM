/** \file utilex_prime_flags.h
 *
 * Prime flags support.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef UTILEX_PRIME_FLAGS_H_INCLUDED
/* { */
#define UTILEX_PRIME_FLAGS_H_INCLUDED

/*
 * This file contains the infrastructure for a different method of flags, based on factoring of prime numbers instead of binary bits.
 *
 * Instead of each bit representing a single flag, with a combination of flags simply being the different bits that are ones,
 * primary flags are accomplished by assigning a unique prime number to each flag, with flag combinations being represented by
 * multiplying flags (since every number can be represented as a unique multiplication of primes). Thus, checking whether a flag
 * is 'on' is done by checking whether the modulo is equal to zero. Accordingly, adding a flag is accomplished by multiplying the
 * the flags variable with the relevant flag bit instead of a bitwise OR (|) as is done with binary bit flags.
 *
 * The problem prime flags solve is one where there is a need for many different flags, but where there are never combinations of
 * many different flags.
 *
 * The main benefit of this approach is the ability to pack more than 32 flags in a 32-bit number.
 *
 * However, before deciding to use prime flags, consider the following limitation:
 * Too many flags combined will result in a multiplication of primes whose value exceeds the 2^32, leading to overflow and
 * undefined behavior.
 *
 * Note that in order to prevent issues such as integer overflows, use the provided functions to check and set flags.
 *
 * Note that the 'natural' neutral (no flag) value is 1, not 0 like with bit flags.
 * The reason for this is that 1 is the value that is 'neutral' (unaffected) to multiplication and division.
 * However, the prime flags APIs are designed to treat both 0 and 1 as neutral (no flag) values.
 */

/*
 * Here is a list of all prime numbers <1000.
 * The lowest prime not currently in use is to the right of the '|'.
 * To add a new prime flag, add the appropriate #define PRIME_FLAG_X and move the '|' to before the next available prime.
 *     2     3     5     7    11    13    17    19    23    29    31    37    41    43
 *    47    53    59    61    67    71    73    79    83    89    97   101   103   107
 *   109   113 | 127   131   137   139   149   151   157   163   167   173   179   181
 *   191   193   197   199   211   223   227   229   233   239   241   251   257   263
 *   269   271   277   281   283   293   307   311   313   317   331   337   347   349
 *   353   359   367   373   379   383   389   397   401   409   419   421   431   433
 *   439   443   449   457   461   463   467   479   487   491   499   503   509   521
 *   523   541   547   557   563   569   571   577   587   593   599   601   607   613
 *   617   619   631   641   643   647   653   659   661   673   677   683   691   701
 *   709   719   727   733   739   743   751   757   761   769   773   787   797   809
 *   811   821   823   827   829   839   853   857   859   863   877   881   883   887
 *   907   911   919   929   937   941   947   953   967   971   977   983   991   997
 */

#define PRIME_FLAG_0         2
#define PRIME_FLAG_1         3
#define PRIME_FLAG_2         5
#define PRIME_FLAG_3         7
#define PRIME_FLAG_4         11
#define PRIME_FLAG_5         13
#define PRIME_FLAG_6         17
#define PRIME_FLAG_7         19
#define PRIME_FLAG_8         23
#define PRIME_FLAG_9         29
#define PRIME_FLAG_10        31
#define PRIME_FLAG_11        37
#define PRIME_FLAG_12        41
#define PRIME_FLAG_13        43
#define PRIME_FLAG_14        47
#define PRIME_FLAG_15        53
#define PRIME_FLAG_16        59
#define PRIME_FLAG_17        61
#define PRIME_FLAG_18        67
#define PRIME_FLAG_19        71
#define PRIME_FLAG_20        73
#define PRIME_FLAG_21        79
#define PRIME_FLAG_22        83
#define PRIME_FLAG_23        89
#define PRIME_FLAG_24        97
#define PRIME_FLAG_25        101
#define PRIME_FLAG_26        103
#define PRIME_FLAG_27        107
#define PRIME_FLAG_28        109
#define PRIME_FLAG_29        113

/*!
 * \brief - Check if a prime flag is already set.
 *
 * \param [in] flags - The flags to check in.
 * \param [in] flag_bit - The desired flag to check for.
 *
 * \return - TRUE if flag_bit exists in flags, FALSE otherwise.
 *
 * Note - in case flags or flag_bit don't yet exist (e.g. equal 0), the function returns FALSE.
 * Note - this function treats both 0 and 1 as neutral (no flag) values.
 */
int utilex_is_prime_flag_set(
    int flags,
    int flag_bit);

/*!
 * \brief - Add a prime flag if not already exists.
 *
 * \param [in] *flags - Pointer to the flags to add the flag to.
 * \param [in] flag_bit - The desired flag to add.
 *
 * Note - if flags already contains flag_bit, no change is made.
 * Note - this function treats both 0 and 1 as neutral (no flag) values.
 */
void utilex_prime_flag_set(
    int *flags,
    int flag_bit);

/* } UTILEX_PRIME_FLAGS_H_INCLUDED*/
#endif
