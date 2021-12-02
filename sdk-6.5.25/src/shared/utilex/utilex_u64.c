/**
 * \file utilex_u64.c
 * multiple-precision arithmetic code 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

/* 
 Contains multiple-precision arithmetic code originally written by David Ireland,
 copyright (c) 2001-8 by D.I. Management Services Pty Limited <www.di-mgt.com.au>,
 and is used with permission"
*/

#include <shared/bsl.h>
#include <sal/types.h>

#include <shared/shrextend/shrextend_error.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/utilex/utilex_u64.h>
#include <shared/utilex/utilex_os_interface.h>

/*****************************************************
 * See details in utilex_u64.h
 *****************************************************/
uint32
utilex_u64_log2_round_up(
    COMPILER_UINT64 xx)
{
    uint32 msb_bit;
    COMPILER_UINT64 calc;

    COMPILER_64_SET(calc, 0, 0);

    if (COMPILER_64_HI(xx) == 0)
    {
        msb_bit = utilex_msb_bit_on(COMPILER_64_LO(xx));
        COMPILER_64_SET(calc, 0, (1u << msb_bit));
    }
    else
    {
        msb_bit = utilex_msb_bit_on(COMPILER_64_HI(xx));
        COMPILER_64_SET(calc, (1u << msb_bit), 0);
        msb_bit += sizeof(COMPILER_64_LO(xx)) * 8;      /* +=32 */
    }

    if (COMPILER_64_GT(xx, calc))
    {
        /*
         * need to make round up.
         */
        msb_bit++;
    }

    return msb_bit;
}

/*****************************************************
 * See details in utilex_u64.h
 *****************************************************/
uint32
utilex_u64_log2_round_down(
    COMPILER_UINT64 xx)
{
    uint32 msb_bit;

    if (COMPILER_64_HI(xx) == 0)
    {
        msb_bit = utilex_msb_bit_on(COMPILER_64_LO(xx));
    }
    else
    {
        msb_bit = utilex_msb_bit_on(COMPILER_64_HI(xx));
        msb_bit += sizeof(COMPILER_64_LO(xx)) * 8;      /* +=32 */
    }

    return msb_bit;
}

/*****************************************************
 * See details in utilex_u64.h
 *****************************************************/
void
utilex_u64_print(
    COMPILER_UINT64 u64,
    uint32 print_type,
    uint32 short_format)
{

    if (print_type == 0)
    {
        /*
         * Decimal Format
         */
        if (short_format)
        {
            if (0 != COMPILER_64_HI(u64))
            {
                LOG_CLI((BSL_META("%u * 2^32 + "), COMPILER_64_HI(u64)));
            }
            LOG_CLI((BSL_META("%u"), COMPILER_64_LO(u64)));
        }
        else
        {
            LOG_CLI((BSL_META("arr[1]: %u, arr[0]: %u\n\r"), COMPILER_64_HI(u64), COMPILER_64_LO(u64)));
        }
    }
    else
    {
        /*
         * Hexadecimal Format
         */
        if (short_format)
        {
            if (0 != COMPILER_64_HI(u64))
            {
                LOG_CLI((BSL_META("0x%X "), COMPILER_64_HI(u64)));
            }
            LOG_CLI((BSL_META("0x%X"), COMPILER_64_LO(u64)));
        }
        else
        {
            LOG_CLI((BSL_META("arr[1]: 0x%8X, arr[0]: 0x%8X\n\r"), COMPILER_64_HI(u64), COMPILER_64_LO(u64)));
        }
    }
    return;
}

/*****************************************************
 * See details in utilex_u64.h
 *****************************************************/
uint32
utilex_u64_test(
    uint32 silent)
{
    uint32 pass, seed;
    COMPILER_UINT64 u64_1, u64_2, u64_3;
    uint32 seconds, nano_seconds, x, y, rr, nof_test;

#if defined(_MSC_VER)
    /*
     * Add WINDOWS check.
     */
    unsigned __int64 win_u64_1, win_u64_2, win_u64_3;
#endif

    pass = TRUE;

    if (!silent)
    {
        LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("****utilex_u64_test**** START\n")));
    }

    COMPILER_64_SET(u64_1, 0, 0);

    COMPILER_64_SET(u64_2, 0, 0);

    utilex_os_get_time(&seconds, &nano_seconds);
    seed = (uint32) (nano_seconds ^ seconds);

    utilex_os_srand(seed);
    nof_test = 20000;

    /*
     * Test Type 1:
     * x, y are randoms.
     * u64_1 = x * y
     * u64_2 = u64_1/y
     * rr     = u64_1%y
     * Check that rr is 0.
     * Check that u64_2 is x.
     */
    while (--nof_test)
    {
        x = utilex_os_rand();
        y = utilex_os_rand();
        if (y == 0)
        {
            y = 1;
        }
        COMPILER_64_SET(u64_1, 0, x);
        COMPILER_64_UMUL_32(u64_1, y);

#if defined(_MSC_VER)
        win_u64_1 = (__int64) x *y;
        if (((win_u64_1 & 0xFFFFFFFF) != COMPILER_64_LO(u64_1)) || ((win_u64_1 >> 32) != COMPILER_64_HI(u64_1)))
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** Test Type 1: utilex_u64_test - FAIL\n")));
            LOG_INFO(BSL_LS_SOCDNX_COMMON,
                     (BSL_META("**** win_u64_1!=u64_1, nof_test=%u, seed=%u \n"), nof_test, seed));
            pass = FALSE;
            goto exit;
        }
#endif

        COMPILER_64_COPY(u64_2, u64_1);
        UTILEX_64_UDIV_32_WITH_REMAINDER(u64_2, y, rr);

#if defined(_MSC_VER)
        win_u64_2 = win_u64_1 / y;
        if (((win_u64_2 & 0xFFFFFFFF) != COMPILER_64_LO(u64_2)) || ((win_u64_2 >> 32) != COMPILER_64_HI(u64_2)))
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** Test Type 1: utilex_u64_test - FAIL\n")));
            LOG_INFO(BSL_LS_SOCDNX_COMMON,
                     (BSL_META("**** win_u64_2!=u64_2, nof_test=%u, seed=%u \n"), nof_test, seed));
            pass = FALSE;
            goto exit;
        }
#endif

        if (rr != 0)
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** utilex_u64_test - FAIL\n")));
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** rr!=0, nof_test=%u, seed=%u \n"), nof_test, seed));
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** u64_2=u64_1/y=")));
            utilex_u64_print(u64_2, 1, 1);
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META(" <=Check u64_2 == x\n")));
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** rr=u_64_1 mod y=0x%08X <=Check rr == 0\n"), rr));
            pass = FALSE;
            goto exit;
        }
        if ((COMPILER_64_LO(u64_2) != x) || (COMPILER_64_HI(u64_2) != 0))
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** Test Type 1: utilex_u64_test - FAIL\n")));
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** x = %u, y = %u,\n\r"), x, y));
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** u64_1 = ")));
            utilex_u64_print(u64_1, 0, TRUE);
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("\n\r")));
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** u64_2 = ")));
            utilex_u64_print(u64_2, 0, TRUE);
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("\n\r")));
            LOG_INFO(BSL_LS_SOCDNX_COMMON,
                     (BSL_META("**** (u64_2.arr[0] != x).., nof_test=%u, seed=%u \n"), nof_test, seed));
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** u64_2=u64_1/y=")));
            utilex_u64_print(u64_2, 1, 1);
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META(" <=Check u64_2 == x\n")));
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** rr=u_64_1 mod y=0x%08X <=Check rr == 0\n"), rr));
            pass = FALSE;
            goto exit;
        }
    }

    nof_test = 20000;

    /*
     * Test Type 2:
     * u64_1, x are randoms.
     * u64_2 = u64_1 / x
     * rr     = u64_1 % x
     * if u64_2 is in 32 bits
     * u64_3 = u64_2 * x + rr
     * Check that u64_3 is u64_1.
     */
    while (--nof_test)
    {
        x = utilex_os_rand();
        if (x == 0)
        {
            x = 1;
        }
        COMPILER_64_SET(u64_1, utilex_os_rand(), utilex_os_rand());

        COMPILER_64_COPY(u64_2, u64_1);
        UTILEX_64_UDIV_32_WITH_REMAINDER(u64_2, x, rr);

#if defined(_MSC_VER)
        win_u64_1 = COMPILER_64_HI(u64_1);
        win_u64_1 <<= 32;
        win_u64_1 |= COMPILER_64_LO(u64_1);
        win_u64_2 = win_u64_1 / x;
        if (((win_u64_2 & 0xFFFFFFFF) != COMPILER_64_LO(u64_2)) || ((win_u64_2 >> 32) != COMPILER_64_HI(u64_2)))
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** Test Type 2: utilex_u64_test - FAIL\n")));
            LOG_INFO(BSL_LS_SOCDNX_COMMON,
                     (BSL_META("**** win_u64_2!=u64_2, nof_test=%u, seed=%u \n"), nof_test, seed));
            pass = FALSE;
            goto exit;
        }
#endif

        if (COMPILER_64_HI(u64_2) != 0)
        {
            continue;
        }

        y = COMPILER_64_LO(u64_2);

        if (COMPILER_64_HI(u64_2))
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON,
                     (BSL_META("**** Overflow on conversion of u64 to long (u64 = %u, %u)\n"), COMPILER_64_LO(u64_2),
                      COMPILER_64_HI(u64_2)));
        }
        COMPILER_64_SET(u64_3, 0, x);
        COMPILER_64_UMUL_32(u64_3, y);

#if defined(_MSC_VER)
        win_u64_3 = (__int64) x *win_u64_2;
        if (((win_u64_3 & 0xFFFFFFFF) != COMPILER_64_LO(u64_3)) || ((win_u64_3 >> 32) != COMPILER_64_HI(u64_3)))
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** Test Type 2: utilex_u64_test - FAIL\n")));
            LOG_INFO(BSL_LS_SOCDNX_COMMON,
                     (BSL_META("**** win_u64_1!=u64_1, nof_test=%u, seed=%u \n"), nof_test, seed));
            pass = FALSE;
            goto exit;
        }
#endif

        COMPILER_64_ADD_32(u64_3, rr);

        if ((COMPILER_64_LO(u64_3) != COMPILER_64_LO(u64_1)) || (COMPILER_64_HI(u64_3) != COMPILER_64_HI(u64_1)))
        {
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** utilex_u64_test - FAIL\n")));
            LOG_INFO(BSL_LS_SOCDNX_COMMON,
                     (BSL_META("**** (COMPILER_64_LO(u64_3) != COMPILER_64_LO(u64_1)).., nof_test=%u, seed=%u \n"),
                      nof_test, seed));
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** u64_2=u64_1/y=")));
            utilex_u64_print(u64_2, 1, 1);
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META(" <=Check u64_2 == x\n")));
            LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("**** rr=u_64_1 mod y=0x%08X <=Check rr == 0\n"), rr));
            pass = FALSE;
            goto exit;
        }
    }

    COMPILER_64_SET(u64_1, 5, 4);

    COMPILER_64_SET(u64_2, 4, 5);

    COMPILER_64_SET(u64_3, 0, (uint32) (-1));

    COMPILER_64_SUB_64(u64_1, u64_2);
    /*
     * If u64_1 is smaller than u64_2, then it will become negative.
     * In that case, set 'pass' to FALSE and set 'u64_1' to '0'
     * Note that, as far as 'coverity' is concerned, the 'pass=FALSE' clause is dead code
     * and, so, needs to be set as 'false positive'
     */
    if (COMPILER_64_BITTEST(u64_1, 63))
    {
         /* coverity[Logically dead code:FALSE] */
        pass = FALSE;
        COMPILER_64_SET(u64_1, 0, 0);
    }
    else
    {
        pass = TRUE;
    }

    if (COMPILER_64_GT(u64_1, u64_3) || COMPILER_64_GT(u64_3, u64_1) || !pass)
    {
        LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("COMPILER_64_SUB_64 failed(1).\n\r")));
        pass = FALSE;
        goto exit;
    }

    COMPILER_64_SET(u64_1, 5, 4);

    COMPILER_64_SET(u64_2, 4, 5);

    COMPILER_64_SUB_64(u64_2, u64_1);
    /*
     * If u64_2 is smaller than u64_1, then it will become negative.
     * In NOT, set 'pass' to FALSE and set 'u64_2' to '0'
     * Note that, as far as 'coverity' is concerned, the 'pass=FALSE' clause is dead code
     * and, so, needs to be set as 'false positive'
     */
    if (!COMPILER_64_BITTEST(u64_2, 63))
    {
         /* coverity[Logically dead code:FALSE] */
        pass = FALSE;
        COMPILER_64_SET(u64_2, 0, 0);
    }
    else
    {
        pass = TRUE;
    }

    if (!COMPILER_64_IS_ZERO(u64_2))
    {
        LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("COMPILER_64_SUB_64 failed(2).\n\r")));
        pass = FALSE;
        goto exit;
    }
    if (!pass)  /* The test is passed if the subtraction is failed */
    {
        pass = TRUE;
    }
    else
    {
        LOG_INFO(BSL_LS_SOCDNX_COMMON, (BSL_META("COMPILER_64_SUB_64 failed(3).\n\r")));
        pass = FALSE;
        goto exit;
    }

exit:
    return pass;
}
