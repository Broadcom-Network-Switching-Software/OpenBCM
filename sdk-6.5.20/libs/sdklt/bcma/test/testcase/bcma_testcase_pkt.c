/*! \file bcma_testcase_pkt.c
 *
 *  Packet I/O test common functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmltd/chip/bcmltd_device_constants.h>
#include <bcma/test/util/bcma_testutil_port.h>
#include <bcma/bcmpkt/bcma_bcmpkt_test.h>
#include "bcma_testcase_pkt_internal.h"

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

int
bcma_testcase_pkt_select(int unit)
{
    return true;
}

int
bcma_testcase_pkt_dev_cleanup_cb(int unit, void *bp, uint32_t option)
{

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcma_bcmpkt_test_dev_cleanup(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcma_testcase_pkt_dev_init_cb(int unit, void *bp, uint32_t option)
{

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcma_bcmpkt_test_dev_init(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcma_testcase_pkt_chan_qmap_set_cb(int unit, void *bp, uint32_t option)
{
    int chan = 1;
    SHR_BITDCLNAME(queue_bmp, MAX_CPU_COS) = {0};

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_BITSET_RANGE(queue_bmp, 0, MAX_CPU_COS);
    SHR_IF_ERR_EXIT
        (bcma_bcmpkt_test_chan_qmap_set(unit, chan, queue_bmp, MAX_CPU_COS));
exit:
    SHR_FUNC_EXIT();
}

