
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Implementation for PFC SOC functions
 */
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/pfc.h>
#include <soc/feature.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/debug.h>
#include <soc/tomahawk.h>
#include <soc/dma.h>
#include <soc/mmu_config.h>
#include <soc/error.h>

#define _TH3_PFC_PRIORITY_GROUP_NUM 8
#define _TH3_PFC_EB_GROUP_NUM 9

#define _TH3_PFC_PRIORITY_MAX_INDEX 7
#define _TH3_PFC_INDEX_INVALID -1

#define _TH3_PFC_COS_NUM_MASK_OFFSET 12 /* total 12 cos using bits[11:0]*/

typedef struct _soc_th3_pfc_eb_group_config_s {
    int opt_group[TH3_PFC_COS_NUM]; /* PFC optimized COS */
    int eb_group[TH3_PFC_COS_NUM]; /* COS to EB mapping */
    uint8 eb_group_sp[_TH3_PFC_EB_GROUP_NUM]; /* EB group Scheduling Policy
                                        * 1: Static Priority
                                        * 0: Round Robin
                                       */
    uint8 eb_group_opt[_TH3_PFC_EB_GROUP_NUM]; /* 1, EB group is optimized */
    int mmuq_eb_group[TH3_PFC_MMU_QUEUE_NUM]; /* MMU queue EB group mapping */
} _soc_th3_pfc_eb_group_config_t;

typedef enum _soc_th3_pfc_port_speed_id_e {

    _SOC_TH3_PFC_PORT_SPEED_50G = 0,
    _SOC_TH3_PFC_PORT_SPEED_100G = 1,
    _SOC_TH3_PFC_PORT_SPEED_200G = 2,
    _SOC_TH3_PFC_PORT_SPEED_400G = 3,
    _SOC_TH3_PFCPORT_SPEED_NUM = 4

} _SOC_th3_pfc_port_speed_id_t;

static const int opt_group_num_flexport[4][4] = {
                                        {2, 3, 4, 4},
                                        {2, 3, 4, 4},
                                        {1, 2, 3, 3},
                                        {1, 2, 3, 3},
                                    };
static const int magt_opt_group_num_flexport[4] = {4, 4, 3, 3};

STATIC int
_soc_th3_pfc_mtu_encoding_get(int unit, int mtu_size)
{
    int mtu_id;

    if (mtu_size <= 1024) { /* < 1K */
        mtu_id = 0;
    }
    else if (mtu_size <= 2150) { /* < 2.1K */
        mtu_id = 1;
    }
    else if (mtu_size <= 4196) {/* < 4K */
        mtu_id = 2;
    }
    else if (mtu_size <= 9416) {/* < 9K */
        mtu_id = 3;
    }
    else {
        mtu_id = _TH3_PFC_INDEX_INVALID;
    }
    return mtu_id;
}

STATIC int
_soc_th3_pfc_speed_encoding_get(int unit, int speed)
{
    int speed_id;

    if (speed < 50000) {
        speed = 50000;
    }

    switch(speed) {
        case 50000:
            speed_id = _SOC_TH3_PFC_PORT_SPEED_50G;
            break;
        case 100000:
            speed_id = _SOC_TH3_PFC_PORT_SPEED_100G;
            break;
        case 200000:
            speed_id = _SOC_TH3_PFC_PORT_SPEED_200G;
            break;
        case 400000:
            speed_id = _SOC_TH3_PFC_PORT_SPEED_400G;
            break;
        default:
            speed_id = -1;
    }

    return speed_id;
}

STATIC void
_soc_th3_pfc_eb_group_config_t_init(_soc_th3_pfc_eb_group_config_t *config)
{
    int i;
    sal_memset(config, 0, sizeof(_soc_th3_pfc_eb_group_config_t));
    for (i = 0; i < TH3_PFC_COS_NUM; i++) {
        config->eb_group[i] =  _TH3_PFC_INDEX_INVALID;
    }
}

STATIC void
_soc_th3_print_pfc_sched_profile_info (int unit, int profile_id,
        _soc_th3_pfc_eb_group_config_t *eb_config)
{
    int max_cos = 0;
    int cos, idx, l0_idx = 0;
    int mmuq0, mmuq1;
    int fc_only_uc, sp;
    int ebg_grp0, ebg_grp1, pfc_opt_grp, ebg_cos;
    int sp0, sp1;

    max_cos = soc_tomahawk3_get_sched_profile_max_cos(unit, profile_id);

    LOG_VERBOSE(BSL_LS_SOC_MMU, (BSL_META_U(unit,
                " Cos    MMUQ0    MMUQ1    FC_UC    SP    PFC_opt    "
                "EB_grp(cos)  EB_group0  EB_group1  SP(0)  SP(1)\n")));

    for (cos = max_cos; cos >= 0; cos--) {
        for (idx = 0; idx < SOC_TH3_NUM_GP_QUEUES; idx++) {
            if (th3_sched_profile_info[unit][profile_id][idx].cos == cos) {
                l0_idx = idx;
                break;
            }
        }
        mmuq0 = th3_sched_profile_info[unit][profile_id][l0_idx].mmuq[0];
        mmuq1 = th3_sched_profile_info[unit][profile_id][l0_idx].mmuq[1];
        fc_only_uc = th3_sched_profile_info[unit][profile_id][l0_idx].fc_is_uc_only;
        sp = th3_sched_profile_info[unit][profile_id][l0_idx].strict_priority;
        ebg_cos = eb_config->eb_group[cos];
        ebg_grp0 = eb_config->mmuq_eb_group[mmuq0];
        ebg_grp1 = eb_config->mmuq_eb_group[mmuq1];
        pfc_opt_grp = eb_config->opt_group[cos];
        sp0 = eb_config->eb_group_sp[ebg_grp0];
        sp1 = eb_config->eb_group_sp[ebg_grp1];
        LOG_VERBOSE(BSL_LS_SOC_MMU, (BSL_META_U(unit,
                " %2d      %2d      %2d         %d      %d       %d           "
                "%d           %d          %d         %d      %d\n"),
			                cos, mmuq0, mmuq1, fc_only_uc, sp, pfc_opt_grp,
                            ebg_cos, ebg_grp0, ebg_grp1, sp0, sp1));


    }
}

/*
 * Function:
 *     _soc_th3_pfc_opt_bmp_validate
 * Purpose:
 *   To check the validity of COS lists for all PFC priorities.
 * Parameters:
 *     unit       - (IN) unit number
 *     config_array  - (IN) PFC class config array
 *     array_len - (IN) The array length
 * Returns:
 *     SOC_E_XXX
 */

STATIC int
_soc_th3_pfc_opt_bmp_validate(
    int unit,
    soc_cosq_pfc_class_map_config_t *config_array,
    int array_len)
{
    int i, j;
    uint32 opt_bmp_agg;
    if (config_array == NULL)
        return SOC_E_PARAM;

    if (array_len > TH3_PFC_PRIORITY_NUM)
        return SOC_E_PARAM;
/*
*   two conditions have to be met
*   1. for any two optimized PFC priorities, the list of CoS must be either
*      mutually exclusive or equal.
*   2. A priority p0 can be PFC optimized if there is no other priority p1
*      that COS list of p1 is a subset of COS list of p0.
 */
    for (i = 0; i< array_len; i++)   {
        if (!config_array[i].pfc_optimized ||
                    !config_array[i].cos_list_bmp) {
            continue;
        }

        for (j = 0; j < array_len; j++) {
            if (j == i || !config_array[j].cos_list_bmp) {
                continue;
            }
            opt_bmp_agg = config_array[i].cos_list_bmp &
                                              config_array[j].cos_list_bmp;
            if (config_array[j].pfc_optimized) {
                /*  contition 1 */
                if ((opt_bmp_agg != 0) &&
                       (config_array[i].cos_list_bmp != config_array[j].cos_list_bmp)) {
                    return SOC_E_PARAM;
                }
            } else {
                /* condition 2*/
                if (opt_bmp_agg == config_array[j].cos_list_bmp) {
                    return SOC_E_PARAM;
                }

            }
        }
    }

    return SOC_E_NONE;

}

/*
 * Function:
 *     _soc_th3_pfc_rx_config_validate
 * Purpose:
 *   To check if the input Config array is valid
 * Parameters:
 *     unit       - (IN) unit number
 *     config_array  - (IN) PFC class config array
 *     array_len - (IN) The array length
 * Returns:
 *     SOC_E_XXX
 */

STATIC int
_soc_th3_pfc_rx_config_validate(
    int unit,
    soc_cosq_pfc_class_map_config_t *config_array,
    int array_len)
{

    int i;
    if (config_array == NULL) {
        return SOC_E_PARAM;
    }

    for (i = 0; i < array_len; i++) {

        if ((config_array[i].cos_list_bmp >> _TH3_PFC_COS_NUM_MASK_OFFSET)) {
            return SOC_E_PARAM;
        }
    }

    SOC_IF_ERROR_RETURN(_soc_th3_pfc_opt_bmp_validate
                            (unit, config_array, TH3_PFC_PRIORITY_NUM));

    return SOC_E_NONE;
}

/*
 * Function:
 *     _soc_th3_pfc_eb_group_allocate
 * Purpose:
 *   First Assign EB groups to COS
 *   Then set EB scheduling based on
 *      1. EB group assignment,
 *      2. SP_P field from scheduler domain input.
 * Parameters:
 *     unit       - (IN) unit number
 *     eb_config  - (IN/OUT) EB group config
 *     num_opt_group - (IN) the required number of optimized groups
 * Returns:
 *     SOC_E_XXX
 */

STATIC int
_soc_th3_pfc_eb_group_allocate(
    int unit,
    int profile_id,
    _soc_th3_pfc_eb_group_config_t *eb_config,
    int num_opt_group)
{

    int n_avail_eb, non_opt_num;
    int i, cos_num, invalid_index = _TH3_PFC_INDEX_INVALID;
    int rr_set = 0;
    int max_cos = 0;
    /*the maximum index of EB group */
    n_avail_eb = _TH3_PFC_EB_GROUP_NUM - 1;
    /* maximum index of non-opt EB*/
    non_opt_num = n_avail_eb - num_opt_group;

    max_cos = soc_tomahawk3_get_sched_profile_max_cos(unit, profile_id);
    for (cos_num = max_cos; cos_num >= 0; cos_num--) {
        int eb_sp = soc_tomahawk3_get_sp_for_cos(unit, profile_id, cos_num);
        if (eb_config->opt_group[cos_num] !=0 &&
                            eb_config->eb_group[cos_num] == invalid_index) {

            eb_config->eb_group[cos_num] = n_avail_eb--;

            for (i = cos_num -1; i >= 0; i--) {
                if (eb_config->opt_group[i]
                                    == eb_config->opt_group[cos_num]) {
                    eb_config->eb_group[i] = eb_config->eb_group[cos_num];

                    /* the EB group is set to SP as long as there exists one of the
                    * COSs assigned to it is SP planned.
                    */
                    eb_sp |= soc_tomahawk3_get_sp_for_cos(unit, profile_id, i);
                }
            }
            eb_config->eb_group_opt[eb_config->eb_group[cos_num]] = 1;

        } else if (eb_config->eb_group[cos_num] == invalid_index) {
            if (non_opt_num > 0) {
                /* if free EB_GRP is available, then set EB_GRP(cos_num) to
                 * the next available EB_GRP with largest index.
                */
                eb_config->eb_group[cos_num] = n_avail_eb--;
                non_opt_num--;
            } else {
                /* if free EB_GRP is not available, then set EB_GRP(current_cos) to 0
                 * and set scheduling policy to round robin
                */
                eb_config->eb_group[cos_num] = 0;
                rr_set = 1;
            }
        }
        if (!rr_set) {
            if (eb_sp == 1) {
                eb_config->eb_group_sp[eb_config->eb_group[cos_num]] = 1;
            } else {
                rr_set = 1;
            }
        }

    }

    return SOC_E_NONE;

}

STATIC int
_soc_th3_pfc_mmuq_is_mc(int unit, int q_num) {
    int mc_q_mode, num_ucq;
    mc_q_mode = soc_property_get(unit, spn_MMU_PORT_NUM_MC_QUEUE, 2);
    num_ucq = (mc_q_mode == 0) ? 12 : (mc_q_mode == 1) ? 10 :
              (mc_q_mode == 2) ? 8  : (mc_q_mode == 3) ? 6 : 8;

    return (q_num >= 0 && q_num < num_ucq) ? 0 : 1;
}

/*
 * Function:
 *     _soc_th3_pfc_mmu_to_eb_mapping
 * Purpose:
 *     Map MMU queues to EB groups.
 * Parameters:
 *     unit       - (IN) unit number
 *     eb_config  - (IN/OUT) EB group config
 *     profile_id - (IN) Profile index
 * Returns:
 *     SOC_E_XXX
 */
STATIC int
_soc_th3_pfc_mmuq_to_eb_mapping(
    int unit,
    int profile_id,
    _soc_th3_pfc_eb_group_config_t *eb_config)
{

    int i, q_num, cos_f_q, eb_grp_n;
    int fc_only_uc;

    for (q_num = 0; q_num < TH3_PFC_MMU_QUEUE_NUM; q_num++) {
        cos_f_q = soc_tomahawk3_get_cos_for_mmu_queue(unit, profile_id, q_num);
        if (cos_f_q == -1) {
            return SOC_E_PARAM;
        }
        eb_grp_n = eb_config->eb_group[cos_f_q];

        fc_only_uc = soc_tomahawk3_get_fc_only_uc_for_cos(unit, profile_id,
                cos_f_q);
        if (_soc_th3_pfc_mmuq_is_mc(unit, q_num) && fc_only_uc) {
            for (i = eb_grp_n; i >= 0; i--) {
                if (eb_config->eb_group_opt[i] != 1) {
                    break;
                }
            }
            if (i >= 0) {
                eb_config->mmuq_eb_group[q_num] = i;
            } else {
                return SOC_E_FAIL;
            }

        } else {
            eb_config->mmuq_eb_group[q_num] = eb_grp_n;

        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *     _soc_th3_pfc_rx_mapping_hw_write
 * Purpose:
 *     Write PFC-rx realted HW tables/registers
 * Parameters:
 *     unit       -  (IN) unit number
 *     config_arry - (IN) PFC rx config array
 *     len         - (IN) array length
 *     eb_config  -  (IN) EB group config
 *     profile_id -  (IN) Profile index
 * Returns:
 *     SOC_E_XXX
 */

STATIC int
_soc_th3_pfc_rx_mapping_hw_write(
    int unit,
    int profile_id,
    soc_cosq_pfc_class_map_config_t *config_array,
    int len,
    _soc_th3_pfc_eb_group_config_t *eb_config)
{
    int i, pri, cos, pri_ebg, mmuq1 = -1, mmuq2 = -1;
    uint32 cos_bmp, mmuq_bmp, ebg_bmp, sp_bmp, hp_bmp, reg_index, rval;
    uint64 rval64;
    int fc_only_uc = 0, cos_f_q = 0;

    soc_reg_t pri_reg = MMU_INTFI_PFCPRI_PROFILEr,
              mmuq_reg = MMU_EBCFP_MMUQ_EBGRP_PROFILEr,
              sp_reg = MMU_EBQS_EBQ_PROFILEr,
              hp_reg = MMU_OQS_AGER_Q_PROFILE_HP_MMUQr;

    soc_field_t mmuq_field = PFCPRI_MMUQ_BMPf,
                ebg_field = PFCPRI_EBGRP_BMPf,
                sp_field = SP_ENABLE_BMPf,
                hp_field = HP_MMUQ_BITMAPf;

    soc_field_t mmuq_fields[] = {
                    MMUQ0_EBGRP_NUMf, MMUQ1_EBGRP_NUMf, MMUQ2_EBGRP_NUMf,
                    MMUQ3_EBGRP_NUMf, MMUQ4_EBGRP_NUMf, MMUQ5_EBGRP_NUMf,
                    MMUQ6_EBGRP_NUMf, MMUQ7_EBGRP_NUMf, MMUQ8_EBGRP_NUMf,
                    MMUQ9_EBGRP_NUMf, MMUQ10_EBGRP_NUMf, MMUQ11_EBGRP_NUMf
                };

    /* write PFC-rx priority to MMUq(s) and EB group(s) Mapping profile */
    for (pri = 0; pri < TH3_PFC_PRIORITY_NUM && pri < len; pri++) {
        mmuq_bmp = 0;
        ebg_bmp = 0;
        cos_bmp = config_array[pri].cos_list_bmp;

        reg_index = profile_id + pri * TH3_PFC_RX_PROFILE_NUM_ENTRIES;
        SOC_TH3_PFC_BMP_ITER(cos_bmp, TH3_PFC_COS_NUM, cos) {
            int idx = -1;
            int l0_idx = -1;
            for (idx = 0; idx < SOC_TH3_NUM_GP_QUEUES; idx++) {
                if (th3_sched_profile_info[unit][profile_id][idx].cos == cos) {
                    l0_idx = idx;
                    break;
                }
            }
            if (l0_idx != -1) {
                mmuq1 = th3_sched_profile_info[unit][profile_id][l0_idx].mmuq[0];
                mmuq2 = th3_sched_profile_info[unit][profile_id][l0_idx].mmuq[1];
            }

            /* flow control just the UC queue when FC_ONLY_UC=1. */
            if ((mmuq1 != -1) && (mmuq2 != -1)) {
                cos_f_q = soc_tomahawk3_get_cos_for_mmu_queue(unit, profile_id, mmuq1);
                if (cos_f_q == -1) {
                    return SOC_E_PARAM;
                }
                fc_only_uc = soc_tomahawk3_get_fc_only_uc_for_cos(unit, profile_id,
                                                                  cos_f_q);
                if (!(_soc_th3_pfc_mmuq_is_mc(unit, mmuq1) && (fc_only_uc))) {
                    mmuq_bmp |= 1U << mmuq1;
                }

                if (!(_soc_th3_pfc_mmuq_is_mc(unit, mmuq2) && (fc_only_uc))) {
                    mmuq_bmp |= 1U << mmuq2;
                }
            } else {
                if (mmuq1 != -1) {
                    mmuq_bmp |= 1U << mmuq1;
                }
                if (mmuq2 != -1) {
                    mmuq_bmp |= 1U << mmuq2;
                }
            }

            pri_ebg = eb_config->eb_group[cos];
            if (eb_config->eb_group_opt[pri_ebg] == 1) {
                ebg_bmp |= 1U << pri_ebg;
            }
        }

        SOC_TH3_PFC_BMP_ITER(cos_bmp, TH3_PFC_COS_NUM, cos) {
            if (eb_config->eb_group_opt[eb_config->eb_group[cos]] == 0) {
                ebg_bmp = 0;
                break;
            }
        }

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, pri_reg, REG_PORT_ANY, reg_index, &rval));
        soc_reg_field_set(unit, pri_reg, &rval, mmuq_field, mmuq_bmp);
        soc_reg_field_set(unit, pri_reg, &rval, ebg_field, ebg_bmp);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, pri_reg, REG_PORT_ANY, reg_index, rval));

    }

    /* write MMUq to EB group mapping profile */
    COMPILER_64_ZERO(rval64);
    SOC_IF_ERROR_RETURN
            (soc_reg64_get(unit, mmuq_reg, REG_PORT_ANY, profile_id, &rval64));

    for (i = 0; i < TH3_PFC_MMU_QUEUE_NUM; i++) {
        soc_reg64_field32_set(unit, mmuq_reg, &rval64, mmuq_fields[i],
                                            eb_config->mmuq_eb_group[i]);
    }
    SOC_IF_ERROR_RETURN
            (soc_reg64_set(unit, mmuq_reg, REG_PORT_ANY, profile_id, rval64));

    sp_bmp = 0;
    for (i = 0; i < _TH3_PFC_EB_GROUP_NUM; i++) {
        if (eb_config->eb_group_sp[i] == 1) {
            sp_bmp |= 1U << i;
        }
    }
    /* static scheduling setting for EB groups */
    rval = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, sp_reg, REG_PORT_ANY, profile_id, &rval));
    soc_reg_field_set(unit, sp_reg, &rval, sp_field, sp_bmp);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, sp_reg, REG_PORT_ANY, profile_id, rval));

    /* if MMUQ is mapped to a static-priority EB, then it is HP queue */
    hp_bmp = 0;
    for (i = 0; i < TH3_PFC_MMU_QUEUE_NUM; i++) {
        if(eb_config->eb_group_sp[eb_config->mmuq_eb_group[i]] == 1) {
            hp_bmp |= 1U << i;
        }
    }
    rval = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, hp_reg, REG_PORT_ANY, profile_id, &rval));
    soc_reg_field_set(unit, hp_reg, &rval, hp_field, hp_bmp);

    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, hp_reg, REG_PORT_ANY, profile_id, rval));

    return SOC_E_NONE;
}

int
soc_th3_pfc_rx_priority_mapping_profile_get(
    int unit,
    int profile_id,
    int max_count,
    soc_cosq_pfc_class_map_config_t *config_array,
    int *count)
{
    int pri;
    uint32 cos_bmp, mmuq_bmp, ebg_bmp, reg_index, rval, mmuq, cos_id;

    soc_reg_t pri_reg = MMU_INTFI_PFCPRI_PROFILEr;
    soc_field_t mmuq_field = PFCPRI_MMUQ_BMPf,
                ebg_field = PFCPRI_EBGRP_BMPf;

    /* if max_count = 0, config_array has to be NULL */
    if(max_count == 0) {
        if((NULL != config_array))
            return SOC_E_PARAM;
    }

    if(profile_id < TH3_PFC_CLASS_PROFILE_ID_MIN ||
                   profile_id > TH3_PFC_CLASS_PROFILE_ID_MAX) {

        return SOC_E_PARAM;
    }

    for(pri = 0; pri < max_count && pri < TH3_PFC_PRIORITY_NUM; pri++) {
        cos_bmp = 0;

        rval = 0;
        reg_index = profile_id + pri * TH3_PFC_RX_PROFILE_NUM_ENTRIES;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, pri_reg, REG_PORT_ANY, reg_index, &rval));
        mmuq_bmp = soc_reg_field_get(unit, pri_reg, rval, mmuq_field);
        ebg_bmp = soc_reg_field_get(unit, pri_reg, rval, ebg_field);

        if(mmuq_bmp != 0) {
            config_array[pri].pfc_enable = 1;
            SOC_TH3_PFC_BMP_ITER(mmuq_bmp, TH3_PFC_MMU_QUEUE_NUM, mmuq) {
                cos_id = soc_tomahawk3_get_cos_for_mmu_queue(unit, profile_id, mmuq);
                if (cos_id == -1) {
                    return SOC_E_PARAM;
                }
                cos_bmp |= 1U << cos_id;
            }
            config_array[pri].cos_list_bmp = cos_bmp;
        }

        if(ebg_bmp != 0) config_array[pri].pfc_optimized = 1;

    }

    *count = pri;

    return SOC_E_NONE;
}

STATIC int
_soc_th3_pfc_check_higher_pri_cos_bmp(int unit, soc_cosq_pfc_class_map_config_t
        *config_array, int pri_num)
{
    int pri;

    for (pri = (pri_num + 1); pri <= _TH3_PFC_PRIORITY_MAX_INDEX; pri++ ) {
        if (config_array[pri_num].cos_list_bmp ==
                config_array[pri].cos_list_bmp) {
            return 1;
        }
    }

    return 0;
}


/*
 * Function:
 *      soc_th3_pfc_rx_priority_mapping_profile_set
 * Purpose:
 *      Set the PFC rx priority to MMU and EB group mapping.
 * Parameters:
 *      unit       - (IN) device id.
 *      port     - (IN) logic port.
 *      config_array   - (IN) PFC rx config array.
 *      count  - (IN) The length of array
 *                    It needs to be fixed at 8. All PFC priorities should
 *                    be configured at once, and the config array must be
 *                    arranged in ascending order of PFC priorities.
 *                    0, 1, ... , 7.
 * Returns:
 *      SOC_E_XXX
 */

int
soc_th3_pfc_rx_priority_mapping_profile_set(
    int unit,
    int profile_id,
    soc_cosq_pfc_class_map_config_t  *config_array,
    int count)
{

    int pri_num, cos_num;
    int opt_count = 0;
    _soc_th3_pfc_eb_group_config_t eb_config;



    if (profile_id < TH3_PFC_CLASS_PROFILE_ID_MIN ||
                   profile_id > TH3_PFC_CLASS_PROFILE_ID_MAX) {
        return SOC_E_PARAM;
    }

    if (config_array == NULL || count != TH3_PFC_PRIORITY_NUM)
        return SOC_E_PARAM;

    SOC_IF_ERROR_RETURN
            (_soc_th3_pfc_rx_config_validate(unit, config_array, count));

    _soc_th3_pfc_eb_group_config_t_init(&eb_config);

    /* Start from priority 7 and check every priority */
    for (pri_num = _TH3_PFC_PRIORITY_MAX_INDEX; pri_num >= 0; pri_num--) {
        if (config_array[pri_num].pfc_optimized == 1
                                && config_array[pri_num].cos_list_bmp !=0) {
            /* If PFC_Optimized(priority) is set and cos_list_bmp is not 0  &&
             * no other higher PFC optimized priority has same cos list */
            if (!_soc_th3_pfc_check_higher_pri_cos_bmp(unit, config_array,
                        pri_num)) {
                    opt_count++;
            }
            /* set PFC_Optimized_Group(cos) for every cos in the list
             *to the same value opt_count
             * opt_count is the number of PFC optimized groups used so far
            */
            for (cos_num = 0; cos_num < TH3_PFC_COS_NUM; cos_num++) {

                if (SOC_TH3_PFC_BMP_MEMBER(
                            config_array[pri_num].cos_list_bmp, cos_num)) {
                    eb_config.opt_group[cos_num] = opt_count;
                }
            }

        }
    }

    SOC_IF_ERROR_RETURN
        (_soc_th3_pfc_eb_group_allocate(unit, profile_id, &eb_config, opt_count));

    SOC_IF_ERROR_RETURN
        (_soc_th3_pfc_mmuq_to_eb_mapping(unit, profile_id, &eb_config));

    /* Add function to print debug info for CoS-domain intermediary values */
    _soc_th3_print_pfc_sched_profile_info(unit, profile_id, &eb_config);

    SOC_IF_ERROR_RETURN
        (_soc_th3_pfc_rx_mapping_hw_write(unit, profile_id, config_array, count,
                                            &eb_config));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th3_pfc_tx_priority_mapping_profile_op
 * Purpose:
 *      To set/get a PG-property profile.
 * Parameters:
 *      unit       - (IN) device id.
 *      profile_id     - (IN) profile ID.
 *      max_count   - (IN) max lenghth of PG array
 *                         valid only for _get operation
 *      op  - (IN) operation type, _get or _set
 *      array_count (IN/OUT) valid length of array
 *                          IN for _set, OUT for _get
 *      pg_array  (IN/OUT) PG array, index by PFC priority
                            IN for _set, OUT for _get
 * Returns:
 *      SOC_E_XXX
 */

int
soc_th3_pfc_tx_priority_mapping_profile_op(
    int unit,
    int profile_id,
    int max_count,
    soc_th3_pfc_pfc_profile_op_t op,
    int *array_count,
    int *pg_array)
{
#define _TH3_PFC_PRIORITY_GROUP_ID_MAX 7
    int i;
    uint32 rval;
    soc_reg_t reg = MMU_THDI_PFCPRI_PG_PROFILEr;

    soc_field_t field_array[TH3_PFC_PRIORITY_NUM] = {
                PFCPRI0_PGf, PFCPRI1_PGf, PFCPRI2_PGf, PFCPRI3_PGf,
                PFCPRI4_PGf, PFCPRI5_PGf, PFCPRI6_PGf, PFCPRI7_PGf
    };
    if (profile_id < TH3_PFC_PROPERTIES_PROFILE_ID_MIN ||
                   profile_id > TH3_PFC_PROPERTIES_PROFILE_ID_MAX) {

        return SOC_E_PARAM;
    }


    SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, profile_id, &rval));

    if (SOC_TH3_PFC_PROFILE_OP_SET == op) {
        if (*array_count > TH3_PFC_PRIORITY_NUM || *array_count <= 0) {
            return SOC_E_PARAM;
        }

        if (pg_array == NULL) {
            return SOC_E_PARAM;
        }

        for (i = 0; i < *array_count; i++) {
            if (pg_array[i] < 0 || pg_array[i] > _TH3_PFC_PRIORITY_GROUP_ID_MAX) {
                return SOC_E_PARAM;
            }
            soc_reg_field_set(unit, reg, &rval, field_array[i], pg_array[i]);
        }
        SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, profile_id, rval));
    } else if (SOC_TH3_PFC_PROFILE_OP_GET == op) {

        if ((0 == max_count && pg_array != NULL) ||
                            (0 != max_count && pg_array == NULL)) {
            return SOC_E_PARAM;
        }

        for (i = 0; i < max_count && i < TH3_PFC_PRIORITY_NUM; i++) {
            pg_array[i] = soc_reg_field_get(unit, reg, rval, field_array[i]);
        }
        *array_count = i;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th3_pfc_pg_profile_op
 * Purpose:
 *      To set/get a PG-property profile.
 * Parameters:
 *      unit       - (IN) device id.
 *      profile_id     - (IN) profile ID.
 *      max_count   - (IN) max lenghth of PG-property config array
 *                         valid only for _get operation
 *      op  - (IN) operation type, _get or _set
 *      array_count (IN/OUT) valid length of array
 *                          IN for _set, OUT for _get
 *      properties  (IN/OUT) PG-property config array
                            IN for _set, OUT for _get
 * Returns:
 *      SOC_E_XXX
 */

int
soc_th3_pfc_pg_profile_op(
    int unit,
    int profile_id,
    int max_count,
    soc_th3_pfc_pfc_profile_op_t op,
    int* array_count,
    soc_cosq_priority_group_properties_t *properties)
{
#define _TH3_PFC_SPID_MAX_INDEX 3
#define _TH3_PFC_HDRMID_MAX_INDEX 3
    int i, pg_id;
    uint64 rval;
    soc_reg_t reg = MMU_THDI_PG_PROFILEr;

    soc_field_t hdrm_farray[_TH3_PFC_PRIORITY_GROUP_NUM] = {
                PG0_HPIDf, PG1_HPIDf, PG2_HPIDf, PG3_HPIDf,
                PG4_HPIDf, PG5_HPIDf, PG6_HPIDf, PG7_HPIDf
    };
    soc_field_t sp_farray[_TH3_PFC_PRIORITY_GROUP_NUM] = {
                PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
                PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf,
    };

    if (profile_id < TH3_PFC_PROPERTIES_PROFILE_ID_MIN ||
                   profile_id > TH3_PFC_PROPERTIES_PROFILE_ID_MAX) {
        return SOC_E_PARAM;
    }


    SOC_IF_ERROR_RETURN
                (soc_reg64_get(unit, reg, REG_PORT_ANY, profile_id, &rval));

    if (SOC_TH3_PFC_PROFILE_OP_SET == op) {
        if (*array_count < 0 || *array_count > _TH3_PFC_PRIORITY_GROUP_NUM) {
            return SOC_E_PARAM;
        }
         if (properties == NULL) {
            return SOC_E_PARAM;
         }

        for (i = 0; i < *array_count; i++) {
            pg_id = properties[i].priority_group_id;
            if (pg_id != -1) {
                if(pg_id < 0 || pg_id >= _TH3_PFC_PRIORITY_GROUP_NUM) {
                    return SOC_E_PARAM;
                }
            } else {
                continue;
            }
            if (properties[i].service_pool_id != -1) {
                if(properties[i].service_pool_id < 0 ||
                    properties[i].service_pool_id > _TH3_PFC_SPID_MAX_INDEX) {
                    return SOC_E_PARAM;
                }
                soc_reg64_field32_set(unit, reg, &rval, sp_farray[pg_id],
                                        properties[i].service_pool_id);
            }
            if (properties[i].headroom_pool_id != -1) {
                if(properties[i].headroom_pool_id < 0 ||
                        properties[i].headroom_pool_id > _TH3_PFC_HDRMID_MAX_INDEX) {
                    return SOC_E_PARAM;
                }
                soc_reg64_field32_set(unit, reg, &rval, hdrm_farray[pg_id],
                                            properties[i].headroom_pool_id);
            }
        }
        SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, reg, REG_PORT_ANY, profile_id, rval));

    } else if (SOC_TH3_PFC_PROFILE_OP_GET == op) {
         if ((0 == max_count && properties != NULL) ||
                            (0 != max_count && properties == NULL)) {
            return SOC_E_PARAM;
         }
         for (i = 0; i < max_count && i < _TH3_PFC_PRIORITY_GROUP_NUM; i++) {
            properties[i].priority_group_id = i;

            properties[i].service_pool_id =
                        soc_reg64_field32_get(unit, reg, rval, sp_farray[i]);
            properties[i].headroom_pool_id =
                        soc_reg64_field32_get(unit, reg, rval, hdrm_farray[i]);
         }
         *array_count = i;
    }
#undef _TH3_PFC_SPID_MAX_INDEX
#undef _TH3_PFC_HDRMID_MAX_INDEX
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th3_pfc_num_optimized_group_get
 * Purpose:
 *      Get the maximum number of optimized PFC groups for a given port.
 * Parameters:
 *      unit       - (IN) device id.
 *      port       - (IN) logic port number.
 *      num        - (OUT) number of optimized PFC groups supported.
 * Returns:
 *      SOC_E_XXX
 */

int
soc_th3_pfc_num_optimized_group_get(
    int unit,
    soc_port_t port,
    int *num)
{
    int speed, mtu_size;
    uint32 speed_id, mtu_id;
    soc_info_t *si = &SOC_INFO(unit);

    /* PFC optimized group is not supported for LB port */
    if (SOC_PBMP_MEMBER(si->lb_pbm, port)) {
        *num = 0;
        return SOC_E_NONE;
    }

    if (IS_MANAGEMENT_PORT(unit,port)) {
        speed = si->port_speed_max[port];
    } else if (si->flex_eligible) {
        speed = 50000;
    } else {
        speed = si->port_init_speed[port];
        if (speed == 0) {
            speed = si->port_speed_max[port];
        }
    }
    speed_id = _soc_th3_pfc_speed_encoding_get(unit, speed);

    mtu_size = soc_property_get(unit, spn_MMU_PFC_GROUP_OPTIMIZED_MTU_SIZE, 9416);
    mtu_id = _soc_th3_pfc_mtu_encoding_get(unit, mtu_size);

    if (mtu_id == -1 || speed_id == -1) {
        return SOC_E_PARAM;
    }
    if (IS_MANAGEMENT_PORT(unit,port)) {
        *num = magt_opt_group_num_flexport[mtu_id];
    } else if (IS_LB_PORT(unit,port)) {
        *num = 0;
    } else {
        *num = opt_group_num_flexport[mtu_id][speed_id];
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th3_pfc_config_profile_id_set
 * Purpose:
 *      Set the PFC-related profile ID for a given port.
 * Parameters:
 *      unit       - (IN) device id.
 *      port       - (IN) logic port number.
 *      type       - (IN) profile type.
 *      profile_id - (IN) profile ID.
 * Returns:
 *      SOC_E_XXX
 */

int
soc_th3_pfc_config_profile_id_set(
    int unit,
    soc_port_t port,
    soc_th3_pfc_profile_t type,
    uint32 profile_id)
{

    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;
    uint32 rval;
    int i, count = 0, opt_num = 0, max_opt_num = 0;
    soc_cosq_pfc_class_map_config_t config_array[TH3_PFC_PRIORITY_NUM];
    sal_memset(config_array, 0, sizeof(soc_cosq_pfc_class_map_config_t) * TH3_PFC_PRIORITY_NUM);
    if (type >= socTH3PFCProfileMax) {
        return SOC_E_PARAM;
    }

    switch(type) {
        case socTH3PFCPGProperties:
            if (profile_id > TH3_PFC_PROPERTIES_PROFILE_ID_MAX) {
                return SOC_E_PARAM;
            }
            reg = MMU_THDI_ING_PORT_CONFIGr;
            field = PG_PROFILE_SELf;
            break;


        case socTH3PFCReceiveClass:
            if (profile_id > TH3_PFC_CLASS_PROFILE_ID_MAX) {
                return SOC_E_PARAM;
            }
            /* check if a port can have the required number of PFC optimized groups */
            SOC_IF_ERROR_RETURN(soc_th3_pfc_num_optimized_group_get(
                                   unit, port, &max_opt_num));
            SOC_IF_ERROR_RETURN(
                soc_th3_pfc_rx_priority_mapping_profile_get(unit, profile_id,
                    TH3_PFC_PRIORITY_NUM, config_array, &count));
            for (i =0; i < count; i++) {
                if (config_array[i].pfc_optimized) {
                    opt_num++;
                    if (opt_num > max_opt_num) {
                        return SOC_E_PARAM;
                    }
                }
            }
            reg = MMU_EBQS_PORT_CFGr;
            field = PROFILE_SELf;

            SOC_IF_ERROR_RETURN(
                soc_reg32_get(unit, reg, port, 0, &rval));
            soc_reg_field_set(unit, reg, &rval, field, profile_id);
            SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, port, 0, rval));

            reg = MMU_EBCFP_EGR_PORT_CFGr;
            field = PROFILE_SELf;
            SOC_IF_ERROR_RETURN(
                soc_reg32_get(unit, reg, port, 0, &rval));
            soc_reg_field_set(unit, reg, &rval, field, profile_id);
            SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, port, 0, rval));

            reg = MMU_OQS_AGER_DST_PORT_MAPr;
            field = PROFILE_SELf;
            SOC_IF_ERROR_RETURN(
                soc_reg32_get(unit, reg, port, 0, &rval));
            soc_reg_field_set(unit, reg, &rval, field, profile_id);
            SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, port, 0, rval));

            reg = MMU_INTFI_EGR_PORT_CFGr;
            field = PROFILE_SELf;
            break;

        default:
            return SOC_E_UNAVAIL;

    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, field, profile_id);
    SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, port, 0, rval));
    return SOC_E_NONE;

}

/*
 * Function:
 *      soc_th3_pfc_config_profile_id_get
 * Purpose:
 *      Get the PFC-related profile ID for a given port.
 * Parameters:
 *      unit       - (IN) device id.
 *      port       - (IN) logic port number.
 *      type       - (IN) profile type.
 *      profile_id - (OUT) profile ID.
 * Returns:
 *      SOC_E_XXX
 */

int
soc_th3_pfc_config_profile_id_get(
    int unit,
    soc_port_t port,
    soc_th3_pfc_profile_t type,
    uint32 *profile_id)
{

    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;
    uint32 rval, fval;

    if (profile_id == NULL || type >= socTH3PFCProfileMax) {
        return SOC_E_PARAM;
    }

    switch(type) {
        case socTH3PFCPGProperties:
            reg = MMU_THDI_ING_PORT_CONFIGr;
            field = PG_PROFILE_SELf;
            break;

        case socTH3PFCReceiveClass:
            reg = MMU_INTFI_EGR_PORT_CFGr;
            field = PROFILE_SELf;
            break;

        default:
            return SOC_E_UNAVAIL;

    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
    fval = soc_reg_field_get(unit, reg, rval, field);
    *profile_id = fval;

    return SOC_E_NONE;

}

/*
 * Function:
 *      soc_th3_pfc_rx_init_from_cfg
 * Purpose:
 *      To set PFC class profile from config variables
 *      during init time.
 *      config variables should follow the format below
 *      1. mmu_pfc_class_profile_<profile_id>.pfc_priority<priority number>. \
 *       cos_list=<Comma Seperated Values>
 *      2. mmu_pfc_class_profile_<profile_id>.pfc_priority. \
 *       optimized=<Comma Seperated Values>, the number of values should be equal
 *       to the # of PFC priorities. 0: non-optimized, 1: optimized.
 * Parameters:
 *      unit       - (IN) device id.
 * Returns:
 *      SOC_E_XXX
 */

int
soc_th3_pfc_rx_init_from_cfg(int unit)
{
    int i, j, cos_idx, count, rv = SOC_E_NONE, set_profile = 0;
    char s_name[SOC_PROPERTY_NAME_MAX], s_opt[SOC_PROPERTY_NAME_MAX];
    int cos_list[TH3_PFC_COS_NUM], opt_val[TH3_PFC_PRIORITY_NUM];
    soc_cosq_pfc_class_map_config_t rx_config[TH3_PFC_PRIORITY_NUM];
    int mtu_size;
    uint32 rval = 0;

    for (i = 0; i <= TH3_PFC_CLASS_PROFILE_ID_MAX; i++) {
        set_profile = 0;
        sal_memset(rx_config, 0, TH3_PFC_PRIORITY_NUM * sizeof(rx_config[0]));
        sal_memset(opt_val, 0, TH3_PFC_PRIORITY_NUM * sizeof(opt_val[0]));

        /* read pfc optimized config per profile */
        sal_sprintf(s_opt, "%s_%d.%s.%s",
                        spn_MMU_PFC_CLASS_PROFILE, i,
                            spn_PFC_PRIORITY, spn_OPTIMIZED);
        (void) soc_property_get_csv
                        (unit, s_opt, TH3_PFC_PRIORITY_NUM, opt_val);

        /* read cos_list per PFC priority per Profile
         * , and fill structure soc_cosq_pfc_class_map_config_t
        */
        for (j = 0; j < TH3_PFC_PRIORITY_NUM; j++) {

            rx_config[j].pfc_optimized = opt_val[j];

            sal_sprintf(s_name, "%s_%d.%s%d.%s",
                        spn_MMU_PFC_CLASS_PROFILE, i,
                          spn_PFC_PRIORITY, j, spn_COS_LIST);
            count = soc_property_get_csv
                        (unit, s_name, TH3_PFC_COS_NUM, cos_list);
            if (!count) {
                continue;
            }
            for (cos_idx = 0; cos_idx < count; cos_idx++) {
                rx_config[j].cos_list_bmp |= 1U << cos_list[cos_idx];
                rx_config[j].pfc_enable = 1;
            }
            if (!set_profile) {
                set_profile = 1;
            }
        }

        if (set_profile) {
            rv = soc_th3_pfc_rx_priority_mapping_profile_set
                    (unit, i, rx_config, TH3_PFC_PRIORITY_NUM);
            if (rv != SOC_E_NONE) {
                /* Output error log */
            }
        }
    }

    mtu_size =  soc_property_get(unit, spn_MMU_PFC_GROUP_OPTIMIZED_MTU_SIZE, 9416);

    if (mtu_size < 9416) {
        mtu_size = (mtu_size + _TH3_MMU_BYTES_PER_CELL - 1) / _TH3_MMU_BYTES_PER_CELL;
        rval = 0;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, MMU_EBCFP_OPT_EBGRP_REDUCED_MTUr,
                           REG_PORT_ANY, 0, &rval));

        soc_reg_field_set(unit, MMU_EBCFP_OPT_EBGRP_REDUCED_MTUr, &rval,
                         REDUCED_MTUf, mtu_size);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, MMU_EBCFP_OPT_EBGRP_REDUCED_MTUr,
                           REG_PORT_ANY, 0, rval));

       /* specify to use this setting for all 8 profiles and 8 PFC priorities. */
        for (i = 0; i <= TH3_PFC_CLASS_PROFILE_ID_MAX; i++) {
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, MMU_EBCFP_OPT_EBGRP_MTU_PROFILEr,
                               REG_PORT_ANY, i, &rval));
            soc_reg_field_set(unit, MMU_EBCFP_OPT_EBGRP_MTU_PROFILEr,
                             &rval, OPT_EBGRP_MAX_MTU_BMPf, 0);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, MMU_EBCFP_OPT_EBGRP_MTU_PROFILEr,
                               REG_PORT_ANY, i, rval));
        }
    }
    return SOC_E_NONE;
}

/* PFC priority to PG mapping is also handled in THDI code*/

/*
 * Function:
 *      soc_th3_pfc_priority_to_pg_map_init_from_cfg
 * Purpose:
 *      To set PFC-tx priority to PG mapping profile from config variables
 *      during init time.
 *      config variables should follow the format below:
 *      buf.mapprofile<profile_id>.pfcclass_to_priority_group=<csv>
 * Parameters:
 *      unit       - (IN) device id.
 * Returns:
 *      SOC_E_XXX
 */

int
soc_th3_pfc_priority_to_pg_map_init_from_cfg(int unit)
{
    int i, count, rv = SOC_E_NONE;
    char s_name[SOC_PROPERTY_NAME_MAX];
    int pg_list[TH3_PFC_PRIORITY_NUM];

    sal_memset(pg_list, 0, TH3_PFC_PRIORITY_NUM * sizeof(pg_list[0]));

    for (i = 0; i <= TH3_PFC_CLASS_PROFILE_ID_MAX; i++) {

        /* read PFC-tx to PG mapping per profile */
        sal_sprintf(s_name, "%s.%s%d.%s",
                        spn_BUF, spn_MAPPROFILE, i, spn_PFCCLASS_TO_PRIORITY_GROUP);
        count = soc_property_get_csv
                        (unit, s_name, TH3_PFC_PRIORITY_NUM, pg_list);
        if (count > 0) {
            rv = soc_th3_pfc_tx_priority_mapping_profile_op(
                unit, i, TH3_PFC_PRIORITY_NUM, SOC_TH3_PFC_PROFILE_OP_SET,
                    &count, pg_list);
        }

        if (rv != SOC_E_NONE) {
            /* Output error log */
        }

    }
    return SOC_E_NONE;
}

#endif
