/**
 * \file utilex_prime_flags.c
 *  Prime flags support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#include <assert.h>
#include <shared/bsl.h>
#include <sal/types.h>
#include <sal/limits.h>

#include <shared/utilex/utilex_prime_flags.h>

/*!
 * \brief - Check if a prime flag is already set.
 *
 * \param [in] flags - The flags to check in.
 * \param [in] flag_bit - The desired flag to check for.
 *
 * \return - TRUE if flag_bit exists in flags, FALSE otherwise.
 *
 * Note - in case flags or flag_bit don't yet exist (e.g. equal 0), the function returns FALSE.
 */
int
utilex_is_prime_flag_set(
    int flags,
    int flag_bit)
{
    if ((flags > 0) && (flag_bit > 1))
    {
        return ((flags % flag_bit) == 0);
    }
    else
    {
        return FALSE;
    }
}

/*!
 * \brief - Add a prime flag if not already exists.
 *
 * \param [in] *flags - Pointer to the flags to add the flag to.
 * \param [in] flag_bit - The desired flag to add.
 *
 * Note - if flags already contains flag_bit, no change is made.
 */
void
utilex_prime_flag_set(
    int *flags,
    int flag_bit)
{
    if (!utilex_is_prime_flag_set((*flags), flag_bit))
    {
        /*
         * protect against integer overflows
         */
        assert((*flags) < SAL_UINT32_MAX / flag_bit);
        (*flags) = ((*flags) > 0) ? ((*flags) * flag_bit) : flag_bit;
    }
}
