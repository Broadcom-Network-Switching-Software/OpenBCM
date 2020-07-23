/*! \file ser_intr_dispatch.c
 *
 * Interface functions for SER interrupt dispatching messages
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_msgq.h>
#include <shr/shr_debug.h>
#include <bcmdrd_config.h>
#include <shr/shr_error.h>
#include "bcmdrd/bcmdrd_pt.h"
#include <sal/sal_sleep.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmbd/bcmbd_port_intr.h>
#include <bcmbd/bcmbd_mmu_intr.h>
#include <bcmbd/bcmbd_ipep_intr.h>
#include <bcmbd/bcmbd_db_intr.h>

#include "ser_intr_dispatch.h"
#include "ser_config.h"

/******************************************************************************
 * Local definitions
 */

#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/*!
 * \brief interrupt dispatching messages queue (per unit)
 */
static sal_msgq_t intr_dispatch_msg_q[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief interrupt dispatching messages length (per unit)
 */
static uint32_t intr_dispatch_msg_q_length[BCMDRD_CONFIG_MAX_UNITS];

/******************************************************************************
 * Private Functions
 */
/* Post SER message to the queue. */
static void
bcmptm_ser_msg_post(int unit, bcmptm_ser_intr_msg_q_info_t *ser_msg)
{
    int rv = SHR_E_NONE;

    if (intr_dispatch_msg_q[unit] == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Interrupt dispatch message queue is not created\n")));
        return;
    }
    if (bcmptm_ser_msg_count(unit) == intr_dispatch_msg_q_length[unit]) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to post interrupt which ID is [%d] to SERC thread.\n"),
                  ser_msg->ser_intr_num));
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Interrupt dispatch message queue is full - "
                             "SERC thread may not correct SER error in time.\n")));
        /* Msg queue is full, just return, interrupt will be reported. */
        return;
    }
    /*
     * Must disable insterrupt before msg is posted to SERC by sal_msgq_post.
     * Otherwise, SERC may enable interrupt before this routine disable interrupt.
     */
    if (ser_msg->ser_intr_num != 0) {
        (void)bcmptm_ser_intr_enable(unit, ser_msg->ser_intr_num, 0);
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Post interrupt which ID is [%d] to SERC thread.\n"),
                  ser_msg->ser_intr_num));
    }

    rv = sal_msgq_post(intr_dispatch_msg_q[unit],
                       (void *)ser_msg,
                       SAL_MSGQ_NORMAL_PRIORITY,
                       SAL_MSGQ_NOWAIT);
    if (rv != SHR_E_NONE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to post SER msg to SERC thread\n")));
        /* If the message is not posted successfully, should re-enable interrupt. */
        bcmptm_ser_intr_enable(unit, ser_msg->ser_intr_num, 1);
    }
}

/******************************************************************************
 * Public Functions
 */
void
bcmptm_ser_intr_msg_q_length_set(int unit, uint32_t length)
{
    intr_dispatch_msg_q_length[unit] = length;
}

void
bcmptm_ser_intr_msg_q_destroy(int unit)
{
    if (intr_dispatch_msg_q[unit] != NULL) {
        sal_msgq_destroy(intr_dispatch_msg_q[unit]);
        intr_dispatch_msg_q[unit] = NULL;
    }
}

int
bcmptm_ser_intr_msg_q_create(int unit)
{
    int rv = SHR_E_NONE;
    uint32_t intr_msg_q_depth = 0;

    SHR_FUNC_ENTER(unit);

    intr_msg_q_depth = intr_dispatch_msg_q_length[unit];

    if (intr_msg_q_depth == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "intr_msg_q_depth(0) is invalid\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    bcmptm_ser_intr_msg_q_destroy(unit);

    intr_dispatch_msg_q[unit] =
        sal_msgq_create(sizeof(bcmptm_ser_intr_msg_q_info_t),
                        intr_msg_q_depth, "intr dispatch message queue");

    rv = (intr_dispatch_msg_q[unit] == NULL) ? SHR_E_FAIL : SHR_E_NONE;
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to create intr message queue (depth %d)\n"),
                  intr_msg_q_depth));
        SHR_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_ser_notify(int unit, uint32_t intr_param)
{
    bcmptm_ser_intr_msg_q_info_t ser_msg;
    ser_msg.ser_intr_num = intr_param;

    bcmptm_ser_msg_post(unit, &ser_msg);
}

int
bcmptm_ser_msg_recv(int unit, bcmptm_ser_intr_msg_q_info_t *ser_msg,
                    uint32_t usec)
{
    return sal_msgq_recv(intr_dispatch_msg_q[unit], (void *)ser_msg, usec);
}

size_t
bcmptm_ser_msg_count(int unit)
{
    return sal_msgq_count_get(intr_dispatch_msg_q[unit]);
}

int
bcmptm_ser_intr_dispatch(int unit, uint32_t ser_intr_num,
                         bcmptm_ser_info_type_t *ser_info_type,
                         bcmptm_pre_serc_info_t *serc_info)
{
    int sub_lvl_intr, cdmac_inst, cmic_intr_num;
    bcmptm_ser_intr_route_info_t *intr_map = NULL;
    int i = 0, j = 0, intr_map_num, rv = SHR_E_NONE, internal;
    int sub_ser_info_index = 0;
    uint32_t sub_intr_flag;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_ser_intr_map_get(unit, &cdmac_inst,
                                 &intr_map, &intr_map_num);
    SHR_IF_ERR_EXIT(rv);

    *ser_info_type = BCMPTM_SER_INFO_TYPE_NONE;
    serc_info->port = -1;
    serc_info->blk_inst = 0;
    sub_intr_flag = BCMPTM_SER_INTR_FLG_GET(ser_intr_num);
    for (i = 0; i < intr_map_num; i++) {
        if (sub_intr_flag != intr_map[i].sub_intr_flag) {
            continue;
        }
        if (sub_intr_flag == BCMPTM_SER_NON_SUB_LVL_INT) {
            cmic_intr_num = BCMPTM_SER_INTR_NUM_GET(ser_intr_num);
            j = intr_map[i].cmic_intr_num_start;
            internal = intr_map[i].intr_interval;
            serc_info->blk_inst = 0;
            for ( ; j <= intr_map[i].cmic_intr_num_end; ) {
                if (cmic_intr_num == j) {
                    break;
                }
                serc_info->blk_inst++;
                j += internal + 1;
            }
            if (j <= intr_map[i].cmic_intr_num_end) {
                sub_ser_info_index = 0;
                break;
            }
        } else {
            sub_lvl_intr = BCMPTM_SER_INTR_NUM_GET(ser_intr_num);
            for (j = 0; j < intr_map[i].depth; j++) {
                if (sub_lvl_intr == intr_map[i].sub_intr_num[j]) {
                    break;
                }
            }
            if (j < intr_map[i].depth) {
                sub_ser_info_index = j;
                break;
            }
        }
    }
    if (i == intr_map_num) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to distinguish SER interrupt[%x]\n"),
                  ser_intr_num));
        /* error */
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    *ser_info_type = intr_map[i].ser_route_type;
    serc_info->stat_depth = 1;
    if (sub_intr_flag == BCMPTM_SER_NON_SUB_LVL_INT) {
        serc_info->stat_depth = intr_map[i].depth;
    } else if (sub_intr_flag == BCMPTM_SER_PORT_INTR_FLG) {
        serc_info->port = BCMPTM_SER_INTR_INST_GET(ser_intr_num);
        serc_info->blk_inst  = (serc_info->port  - 1) / cdmac_inst;
    } else {
        serc_info->blk_inst = BCMPTM_SER_INTR_INST_GET(ser_intr_num);
    }
    if (intr_map[i].fifo) {
        serc_info->stat_info =
            (void *)((bcmptm_serc_fifo_t *)(intr_map[i].serc_info)
                     + sub_ser_info_index);
    } else {
        serc_info->stat_info =
            (void *)((bcmptm_serc_non_fifo_t *)(intr_map[i].serc_info)
                     + sub_ser_info_index);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_intr_enable(int unit, uint32_t ser_intr_num, int enable)
{
    int cmic_intr_num, sub_lvl_intr, intr_inst;
    int cmic_intr_start = 0, cmic_intr_end =  0, port, pm_inst, cdmac_inst;
    bcmptm_ser_intr_route_info_t *intr_map = NULL;
    int i = 0, j = 0, intr_map_num, rv = SHR_E_NONE;
    uint32_t sub_intr_flag = 0, interval;
    bcmptm_ser_info_type_t ser_route_type;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_ser_intr_map_get(unit, &cdmac_inst,
                                 &intr_map, &intr_map_num);
    SHR_IF_ERR_EXIT(rv);

    sub_intr_flag = BCMPTM_SER_INTR_FLG_GET(ser_intr_num);
    if (sub_intr_flag == BCMPTM_SER_NON_SUB_LVL_INT) {
        cmic_intr_num = BCMPTM_SER_INTR_NUM_GET(ser_intr_num);
        if (enable) {
            bcmptm_ser_lp_intr_enable(unit, 1, cmic_intr_num, 1);
        } else {
            bcmptm_ser_lp_intr_enable(unit, 0, cmic_intr_num, 0);
        }
        /* exit */
        SHR_EXIT();
    }
    sub_lvl_intr = BCMPTM_SER_INTR_NUM_GET(ser_intr_num);
    for (i = 0; i < intr_map_num; i++) {
        if (sub_intr_flag != intr_map[i].sub_intr_flag) {
            continue;
        }
        for (j = 0; j < intr_map[i].depth; j++) {
            if (sub_lvl_intr == intr_map[i].sub_intr_num[j]) {
                break;
            }
        }
        if (j < intr_map[i].depth) {
            cmic_intr_start = intr_map[i].cmic_intr_num_start;
            cmic_intr_end = intr_map[i].cmic_intr_num_end;
            break;
        }
    }
    if (i == intr_map_num) {
        /* error */
        SHR_IF_ERR_EXIT(SHR_E_CONFIG);
    }
    ser_route_type = intr_map[i].ser_route_type;
    interval = intr_map[i].intr_interval;
    if (sub_intr_flag == BCMPTM_SER_MMU_INTR_FLG) {
        if (enable) {
            rv = bcmbd_mmu_intr_enable(unit, sub_lvl_intr);
        } else {
            rv = bcmbd_mmu_intr_disable(unit, sub_lvl_intr);
            SHR_IF_ERR_EXIT(rv);
            intr_inst = BCMPTM_SER_INTR_INST_GET(ser_intr_num);
            BCMPTM_SER_INTR_INST_SET(sub_lvl_intr, intr_inst);
            /* Clear interrupt */
            rv = bcmbd_mmu_intr_clear(unit, sub_lvl_intr);
        }
        cmic_intr_num = cmic_intr_start;
    } else if (sub_intr_flag == BCMPTM_SER_IPEP_INTR_FLG) {
        if (enable) {
            rv = bcmbd_ipep_intr_enable(unit, sub_lvl_intr);
        } else {
            rv = bcmbd_ipep_intr_disable(unit, sub_lvl_intr);
        }
        cmic_intr_num = cmic_intr_start;
        cmic_intr_num += BCMPTM_SER_INTR_INST_GET(ser_intr_num) * (interval + 1);
    }  else if (sub_intr_flag == BCMPTM_SER_EDB_IDB_INTR_FLG) {
        if (enable) {
            rv = bcmbd_db_intr_enable(unit, sub_lvl_intr);
        } else {
            rv = bcmbd_db_intr_disable(unit, sub_lvl_intr);
        }
        cmic_intr_num = cmic_intr_start;
        cmic_intr_num += BCMPTM_SER_INTR_INST_GET(ser_intr_num) * (interval + 1);
    }  else if (sub_intr_flag == BCMPTM_SER_PORT_INTR_FLG) {
        port = BCMPTM_SER_INTR_INST_GET(ser_intr_num);
        BCMPTM_SER_INTR_INST_SET(sub_lvl_intr, port);
        if (enable) {
            rv = bcmbd_port_intr_enable(unit, sub_lvl_intr);
        } else {
            rv = bcmbd_port_intr_disable(unit, sub_lvl_intr);
        }
        if (ser_route_type == BCMPTM_SER_INFO_TYPE_XLPORT) {
            pm_inst = 0;
        } else {
            pm_inst = (port - 1)/ cdmac_inst;
        }
        cmic_intr_num = cmic_intr_start + pm_inst * (interval + 1);
    } else {
        /* error */
        rv = SHR_E_CONFIG;
    }
    SHR_IF_ERR_EXIT(rv);

    if (cmic_intr_num > cmic_intr_end) {
        rv = SHR_E_CONFIG;
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_all_intr_enable(int unit, int enable)
{
    int i = 0, j = 0, rv = SHR_E_NONE, intr_map_num, interval;
    bcmptm_ser_intr_route_info_t *intr_map = NULL;
    bcmdrd_pbmp_t pbmp;
    uint32_t sub_intr_flag = 0, intr_param;
    int sub_lvl_intr, port, cmic_intr_start, cmic_intr_end;
    int sub_logic_intr;
    bcmptm_serc_non_fifo_t *sub_intr_info = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_ser_intr_map_get(unit, NULL, &intr_map, &intr_map_num);
    SHR_IF_ERR_EXIT(rv);
    /* CMIC level */
    for (i = 0; i < intr_map_num; i++) {
        cmic_intr_start = intr_map[i].cmic_intr_num_start;
        cmic_intr_end = intr_map[i].cmic_intr_num_end;
        sub_intr_flag = intr_map[i].sub_intr_flag;
        interval = intr_map[i].intr_interval;
        for (j = cmic_intr_start; j <= cmic_intr_end; j += interval + 1) {
            if (enable) {
                bcmptm_ser_lp_intr_enable(unit, 1, j, 1);
            } else {
                bcmptm_ser_lp_intr_enable(unit, 1, j, 0);
            }
            if (sub_intr_flag != BCMPTM_SER_NON_SUB_LVL_INT) {
                continue;
            }
            if (enable) {
                intr_param = j;
                BCMPTM_SER_INTR_FLG_SET(intr_param, BCMPTM_SER_NON_SUB_LVL_INT);
                rv = bcmptm_ser_lp_intr_func_set(unit, j,
                                                 bcmptm_ser_notify,
                                                 intr_param);
                SHR_IF_ERR_EXIT(rv);
            }
        }
    }
    /* Sub-level */
    for (i = 0; i < intr_map_num; i++) {
        sub_intr_flag = intr_map[i].sub_intr_flag;
        if (sub_intr_flag == BCMPTM_SER_NON_SUB_LVL_INT) {
            continue;
        }
        for (j = 0; j < intr_map[i].depth; j++) {
            sub_lvl_intr = intr_map[i].sub_intr_num[j];
            intr_param = sub_lvl_intr;
            BCMPTM_SER_INTR_FLG_SET(intr_param, sub_intr_flag);
            if (sub_intr_flag == BCMPTM_SER_MMU_INTR_FLG) {
                if (enable) {
                    rv = bcmbd_mmu_intr_func_set(unit, sub_lvl_intr,
                                                 bcmptm_ser_notify,
                                                 intr_param);
                    SHR_IF_ERR_EXIT(rv);

                    rv = bcmbd_mmu_intr_enable(unit, sub_lvl_intr);
                } else {
                    rv = bcmbd_mmu_intr_disable(unit, sub_lvl_intr);
                }
            } else if (sub_intr_flag == BCMPTM_SER_IPEP_INTR_FLG) {
                if (enable) {
                    rv = bcmbd_ipep_intr_func_set(unit, sub_lvl_intr,
                                                  bcmptm_ser_notify,
                                                  intr_param);
                    SHR_IF_ERR_EXIT(rv);

                    rv = bcmbd_ipep_intr_enable(unit, sub_lvl_intr);
                } else {
                    rv = bcmbd_ipep_intr_disable(unit, sub_lvl_intr);
                }
            } else if (sub_intr_flag == BCMPTM_SER_PORT_INTR_FLG) {
                rv = bcmbd_port_intr_func_set(unit, sub_lvl_intr,
                                              bcmptm_ser_notify,
                                              intr_param);
                SHR_IF_ERR_EXIT(rv);

                sub_intr_info = (bcmptm_serc_non_fifo_t *)(intr_map[i].serc_info);
                sub_intr_info += j;

                sal_memset(&pbmp, 0, sizeof(pbmp));

                rv = bcmdrd_dev_blktype_pbmp
                    (unit, sub_intr_info->blk_type, &pbmp);
                SHR_IF_ERR_EXIT(rv);

                BCMDRD_PBMP_ITER(pbmp, port) {
                    sub_logic_intr = sub_lvl_intr;
                    BCMPTM_SER_INTR_INST_SET(sub_logic_intr, port);
                    if (enable) {
                        rv = bcmbd_port_intr_enable(unit, sub_logic_intr);
                    } else {
                        rv = bcmbd_port_intr_disable(unit, sub_logic_intr);
                    }
                }
            } else if (sub_intr_flag == BCMPTM_SER_EDB_IDB_INTR_FLG) {
                if (enable) {
                    rv = bcmbd_db_intr_func_set(unit, sub_lvl_intr,
                                                bcmptm_ser_notify,
                                                intr_param);
                    SHR_IF_ERR_EXIT(rv);

                    rv = bcmbd_db_intr_enable(unit, sub_lvl_intr);
                } else {
                    rv = bcmbd_db_intr_disable(unit, sub_lvl_intr);
                }
            } else {
                rv = SHR_E_CONFIG;
            }
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

