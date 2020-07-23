/*! \file bcm56780_a0_sec_intr.c
 *
 * File containing device level interrupt handling for bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd_sec_intr.h>
#include <bcmbd/chip/bcm56780_a0_sec_intr.h>
#include <bcmsec/bcmsec_utils_internal.h>
#include <bcmsec/bcmsec_utils.h>
#include <bcmsec/bcmsec_event.h>
#include <bcmsec/bcmsec_pt_internal.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmsec/bcmsec_pt_internal.h>
#include <bcmsec/chip/bcm56780_a0_sec_sa_policy.h>
#include "bcm56780_a0_sec_interrupt.h"

/*******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE  BSL_LS_BCMSEC_EVENT

/*******************************************************************************
 * Private functions.
 */
static void
bcm56780_a0_sec_intr_handler(int unit, uint32_t param)
{
    int intr_num, blk_id, rv;
    bcmsec_event_trigger_t event;

    intr_num = SEC_INTR_NUM_GET(param);
    blk_id = SEC_INTR_INST_GET(param);

    event.blk_id = blk_id;
    if (intr_num == SEC_INTR_SA_EXPIRY_ENCRYPT) {
        event.intr_num = SA_EXPIRY_ENCRYPT;
    } else if (intr_num == SEC_INTR_SA_EXPIRY_DECRYPT) {
        event.intr_num = SA_EXPIRY_DECRYPT;
    }

    rv = bcmsec_post_event(unit, &event);

    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Unable to handle interrupt %d for "
                            "block %d\n"), intr_num, blk_id));
    }
    return;
}

/*******************************************************************************
 * Public functions
 */

int
bcm56780_a0_sec_intr_init(int unit, bool warm)
{
    int intr_num;
    SHR_FUNC_ENTER(unit);

    intr_num = SEC_INTR_SA_EXPIRY_ENCRYPT;
    SHR_IF_ERR_EXIT
        (bcmsec_interrupt_enable(unit, intr_num,
                &bcm56780_a0_sec_intr_handler, INTR_ENABLE));

    intr_num = SEC_INTR_SA_EXPIRY_DECRYPT;
    SHR_IF_ERR_EXIT
        (bcmsec_interrupt_enable(unit, intr_num,
                bcm56780_a0_sec_intr_handler, INTR_ENABLE));
exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_sec_sa_expire_handler(int unit, int blk_id, int direction)
{
    bcmdrd_sid_t mem0, mem1;
    bcmdrd_fid_t fid;
    bcmltd_sid_t ltid = -1;
    bcmsec_pt_info_t pt_info = {0};
    uint32_t ltmbuf[2] = {0};
    uint64_t flags = 0;
    uint32_t num_expired = 0, valid, expiry_type, sa_index;
    bcmsec_sa_status_imm_update_t sa_status;

    SHR_FUNC_ENTER(unit);
    BCMSEC_PT_DYN_INFO(pt_info, 0, blk_id, flags);
    mem0 = MACSEC_INTR_FIFO_COUNTr;
    if (direction == SA_EXPIRY_ENCRYPT) {
        fid = ESEC_KEY_EXPIRY_FIFO_COUNTf;
        mem1 = ESEC_SA_EXPIRE_STATUSm;
    } else if (direction == SA_EXPIRY_DECRYPT) {
        fid = ISEC_KEY_EXPIRY_FIFO_COUNTf;
        mem1 = ISEC_SA_EXPIRE_STATUSm;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get the number of expired entries. */
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_read(unit, mem0, ltid, &pt_info, ltmbuf));

    SHR_IF_ERR_EXIT
        (bcmsec_field_get(unit, mem0, fid, ltmbuf, &num_expired));

    /* Read num_expired times the expiry status register. */
    while (num_expired--) {
        /* Read expiry status register. */
        SHR_IF_ERR_EXIT
            (bcmsec_pt_indexed_read(unit, mem1, ltid, &pt_info, ltmbuf));
        fid = VALIDf;
        SHR_IF_ERR_EXIT
            (bcmsec_field_get(unit, mem1, fid, ltmbuf, &valid));

        /* Entry is not valid. Should not occur. */
        if (!valid) {
            LOG_WARN(BSL_LOG_MODULE,(BSL_META_U(unit,
                    "SA expiry from block %d, direction %d does not have "
                    "valid status field.\n"), blk_id, direction));
            continue;
        }

        fid = EXPIRY_TYPEf;
        SHR_IF_ERR_EXIT
            (bcmsec_field_get(unit, mem1, fid, ltmbuf, &expiry_type));

        fid = SA_INDEXf;
        SHR_IF_ERR_EXIT
            (bcmsec_field_get(unit, mem1, fid, ltmbuf, &sa_index));

        if (expiry_type == 0) {
            sa_status.sa_status_type = SA_VALID_AND_SOFT_EXPIRED;
        } else {
            sa_status.sa_status_type = SA_VALID_AND_HARD_EXPIRED;
        }
        sa_status.blk_id = blk_id;
        sa_status.sa_index = sa_index;
        sa_status.dir = direction;
        sa_status.ipsec = -1;
        SHR_IF_ERR_EXIT
            (bcmsec_sa_status_imm_update(unit, &sa_status));
    }

exit:
    SHR_FUNC_EXIT();
}
