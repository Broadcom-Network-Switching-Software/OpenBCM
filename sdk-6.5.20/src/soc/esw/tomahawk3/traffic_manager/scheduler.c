/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Scheduler API implemenation for Tomahawk3
 */


#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/esw/pfc.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/tomahawk3.h>
#include <soc/feature.h>
#include <soc/drv.h>
#include <soc/types.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>
#include <soc/debug.h>
#include <soc/tomahawk3.h>
#include <soc/dma.h>
#include <soc/mmu_config.h>

_soc_mmu_cfg_scheduler_profile_t* soc_mmu_cfg_scheduler_profile_alloc(int unit)
{
    _soc_mmu_cfg_scheduler_profile_t *sched_profile;
    int alloc_size=0;
    int i;

    alloc_size = sizeof(_soc_mmu_cfg_scheduler_profile_t);
    for (i=0; i < SOC_TH3_MAX_NUM_SCHED_PROFILE; i++) {
        alloc_size += sizeof(_soc_mmu_cfg_scheduler_profile_t);
    }
    sched_profile = sal_alloc(alloc_size, "MMU config scheduler profile");
    if (sched_profile == NULL) {
        return NULL;
    }

    sal_memset(sched_profile, 0, alloc_size);
    return sched_profile;
}

void
soc_mmu_cfg_scheduler_profile_free(int unit, _soc_mmu_cfg_scheduler_profile_t *cfg)
{
    if (cfg != NULL) {
        sal_free(cfg);
    }
}


int
_soc_mmu_cfg_scheduler_profile_read(int unit, _soc_mmu_cfg_scheduler_profile_t *sched_profile,
                                    int *scheduler_port_profile_map)
{
    char name[80];
    int idx;
    int port;
    /* char suffix; */

    if (scheduler_port_profile_map == NULL) {
        return SOC_E_PARAM;
    }

    if (sched_profile == NULL) {
        return SOC_E_PARAM;
    } 

    /* scheduler_profile_map_<port>=<profile_idx> */
    PBMP_ALL_ITER(unit, port) {
        scheduler_port_profile_map[port] = soc_property_port_get(unit, port,
            spn_SCHEDULER_PROFILE_MAP, scheduler_port_profile_map[port]);
    }


    for (idx = 0; idx < SOC_TH3_MAX_NUM_SCHED_PROFILE; idx++) {
        /* sched_profile<idx>.profile_valid=1*/
        sal_sprintf(name, "%s_%d.%s",
                    spn_SCHEDULER_PROFILE, idx, spn_PROFILE_VALID);
        sched_profile[idx].valid = soc_property_get(unit, name, sched_profile[idx].valid);
        sal_sprintf(name, "%s_%d.%s",
                    spn_SCHEDULER_PROFILE, idx, spn_NUM_UNICAST_QUEUE);
        (void)soc_property_get_csv(unit, name, 12,
                                        sched_profile[idx].num_unicast_queue);
        sal_sprintf(name, "%s_%d.%s",
                    spn_SCHEDULER_PROFILE, idx, spn_NUM_MULTICAST_QUEUE);
        (void)soc_property_get_csv(unit, name, 12,
                                        sched_profile[idx].num_multicast_queue);
        sal_sprintf(name, "%s_%d.%s",
                    spn_SCHEDULER_PROFILE, idx, spn_SCHED_STRICT_PRIORITY);
        (void)soc_property_get_csv(unit, name, 12,
                                        sched_profile[idx].strict_priority);
        sal_sprintf(name, "%s_%d.%s",
                    spn_SCHEDULER_PROFILE, idx, spn_FLOW_CONTROL_ONLY_UNICAST);
        (void)soc_property_get_csv(unit, name, 12,
                                        sched_profile[idx].flow_control_only_unicast);
    }

    return SOC_E_NONE;
}

/*! @fn void _soc_mmu_tomahawk3_scheduler_profile_reset(
 *    int unit,
 *    _soc_mmu_cfg_scheduler_profile_t *sched_profile,
 *  @param unit Chip unit number.
 *  @param sched_profile, Pointer to _soc_mmu_cfg_scheduler_profile_t structure
 *  @brief Function to reset sched_profile structure
 * Description:
 *      Function to reset sched_profile structure
 * Parameters:
 *      unit - Device number
 *      sched_profile - _soc_mmu_cfg_scheduler_profile_t structure
 * Return Value:
 *      SUCCESS/FAILURE
 */
int
_soc_mmu_tomahawk3_scheduler_profile_reset(int unit,
             _soc_mmu_cfg_scheduler_profile_t *sched_profile)
{
    int idx, cosq_idx;
    int num_ucq = 0;

    if (sched_profile == NULL) {
        return SOC_E_MEMORY;
    }

    num_ucq = _soc_th3_get_num_ucq(unit);

    for (idx = 0; idx < SOC_TH3_MAX_NUM_SCHED_PROFILE; idx++) {
        for (cosq_idx = 0; cosq_idx < SOC_TH3_COS_MAX; cosq_idx++) {
            /* Default scheduler profile 0 is setup with default values */
            if (idx == 0) {
                if (cosq_idx < num_ucq) {
                    sched_profile[idx].num_unicast_queue[cosq_idx] = 1;
                    sched_profile[idx].num_multicast_queue[cosq_idx] = 0;
                } else {
                    sched_profile[idx].num_unicast_queue[cosq_idx] = 0;
                    sched_profile[idx].num_multicast_queue[cosq_idx] = 1;
                }
                sched_profile[idx].valid = 1;
                sched_profile[idx].strict_priority[cosq_idx] = 0;
                sched_profile[idx].flow_control_only_unicast[cosq_idx] = 0;
            } else {
                sched_profile[idx].num_unicast_queue[cosq_idx] = -1;
                sched_profile[idx].num_multicast_queue[cosq_idx] = -1;
                sched_profile[idx].strict_priority[cosq_idx] = -1;
                sched_profile[idx].flow_control_only_unicast[cosq_idx] = -1;
                sched_profile[idx].valid = 0;
            }
        }
    }
    return SOC_E_NONE;

}

/*! @fn int _soc_mmu_tomahawk3_scheduler_profile_config_check(
 *    int unit,
 *    _soc_mmu_cfg_scheduler_profile_t *sched_profile,
 *  @param unit Chip unit number.
 *  @param sched_profile, Pointer to _soc_mmu_cfg_scheduler_profile_t structure
 *  @brief Function to check if config bcm settings are valid
 * Description:
 *      Function to check if config bcm settings are valid
 * Parameters:
 *      unit - Device number
 *      idx - Profile index
 *      sched_profile - _soc_mmu_cfg_scheduler_profile_t structure
 * Return Value:
 *      SUCCESS/FAILURE
 */
int
_soc_mmu_tomahawk3_scheduler_profile_config_check(int unit, int idx,
             _soc_mmu_cfg_scheduler_profile_t *sched_profile)
{
    int idx1, cosq_idx;
    int total_numq, total_num_ucq, total_num_mcq = 0;
    int num_uc_q, num_mc_q;
    int current_cosq_idx;

    num_uc_q = _soc_th3_get_num_ucq(unit);
    num_mc_q = _soc_th3_get_num_mcq(unit);

    total_num_ucq = 0;
    total_num_mcq = 0;
    current_cosq_idx = 0;

    for (idx1 = 0; idx1 < SOC_TH3_COS_MAX; idx1++) {
        /* Get total number of UC queues */
        if (sched_profile[idx].num_unicast_queue[idx1] != -1) {
            total_num_ucq += sched_profile[idx].num_unicast_queue[idx1];
        }
        /* Get total number of MC queues */
        if (sched_profile[idx].num_multicast_queue[idx1] != -1) {
            total_num_mcq += sched_profile[idx].num_multicast_queue[idx1];
        }
    }

    /* Total number of UC and MC Queues cannot exceed those
       specified by mcq mode */
    if (total_num_ucq > num_uc_q) {
        LOG_INFO(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "Profile %d Max unicast queues per port is %d\n"),
                idx, num_uc_q));
        return SOC_E_FAIL;
    }
    if (total_num_mcq > num_mc_q) {
        LOG_INFO(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "Profile %d Max multicast queues per port is %d\n"),
                idx, num_mc_q));
       return SOC_E_FAIL;
    }


    for (cosq_idx = 0; cosq_idx < SOC_TH3_COS_MAX; cosq_idx++) {
        /* All possible legal configs are UC+UC, UC+MC, UC, MC, NONE */
        if ((sched_profile[idx].num_multicast_queue[cosq_idx] +
             sched_profile[idx].num_unicast_queue[cosq_idx]) > 2 ) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "Profile %d Max unicast + multicast queues per cos is 2\n"), idx));
            return SOC_E_FAIL;
        }
        if (sched_profile[idx].num_unicast_queue[cosq_idx] > 2) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "Profile %d Max unicast queues per cos is 2\n"), idx));
            return SOC_E_FAIL;
        }
        if (sched_profile[idx].num_multicast_queue[cosq_idx] > 1) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "Profile %d Max multicast queues per cos is 2\n"), idx));
            return SOC_E_FAIL;
        }

        /* Make sure that L0->L1 node connection possible */
        /* L1.n can connect to L0.n and L0.n+1 only */
        /* L1.11 can connect to L0.11 only */
        total_numq = sched_profile[idx].num_unicast_queue[cosq_idx] +
                     sched_profile[idx].num_multicast_queue[cosq_idx];

        if ((total_numq > 0) && (current_cosq_idx == SOC_TH3_COS_MAX)) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "[1] Profile %d cos %d Cos to queue mapping error\n"),
                idx, cosq_idx));
            return SOC_E_FAIL;
        }

        if (total_numq == 2) {
            current_cosq_idx += 2;
        } else if (total_numq == 1) {
            current_cosq_idx += 1;
        } else {
            continue;
        }
    }
    return SOC_E_NONE;
}


/*! @fn int _soc_mmu_tomahawk3_scheduler_profile_check(
 *    int unit,
 *    _soc_mmu_cfg_scheduler_profile_t *sched_profile,
 *  @param unit Chip unit number.
 *  @param sched_profile, Pointer to _soc_mmu_cfg_scheduler_profile_t structure
 *  @brief Function to check if config bcm settings are valid
 * Description:
 *      Function to check if config bcm settings are valid
 * Parameters:
 *      unit - Device number
 *      sched_profile - _soc_mmu_cfg_scheduler_profile_t structure
 * Return Value:
 *      SUCCESS/FAILURE
 */
int
_soc_mmu_tomahawk3_scheduler_profile_check(int unit,
             _soc_mmu_cfg_scheduler_profile_t *sched_profile)
{
    int idx;

    for (idx = 0; idx < SOC_TH3_MAX_NUM_SCHED_PROFILE; idx++) {
        if (sched_profile[idx].valid != 1) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "Skipping invalid scheduler profile %d\n"), idx));
            continue;
        }
        SOC_IF_ERROR_RETURN(_soc_mmu_tomahawk3_scheduler_profile_config_check(unit,
                           idx, sched_profile));

    }

    return SOC_E_NONE;
}

int _soc_scheduler_profile_mapping_setup(int unit, _soc_mmu_cfg_scheduler_profile_t *sched_profile,
                                         int profile_index, int *L0, int *schedq, int *mmuq,
                                         int *cos_list, int *strict_priority, int *fc_is_uc_only)
{
    int mcq_base, ucq_base;
    int total_numq, num_ucq, num_mcq;
    int sp, fc_uc;
    int cosq_idx;
    int current_q_pos = 0;

    ucq_base = 0;
    mcq_base = _soc_th3_get_num_ucq(unit);

    if ((sched_profile == NULL) ||
        (L0 == NULL) ||
        (schedq == NULL) ||
        (cos_list == NULL) ||
        (strict_priority == NULL) ||
        (fc_is_uc_only == NULL) ||
        (mmuq == NULL)) {
        return SOC_E_MEMORY;
    }

    if (profile_index < 0 || profile_index >= SOC_TH3_MAX_NUM_SCHED_PROFILE) {
        return SOC_E_PARAM;
    }

    for (cosq_idx = 0; cosq_idx < SOC_TH3_COS_MAX; cosq_idx++) {
        cos_list[cosq_idx] = -1;
        strict_priority[cosq_idx] = 0;
        fc_is_uc_only[cosq_idx] = 0;
    }

    for (cosq_idx = 0; cosq_idx < SOC_TH3_COS_MAX; cosq_idx++) {
        if ((sched_profile[profile_index].num_unicast_queue[cosq_idx] == -1) &&
            (sched_profile[profile_index].num_multicast_queue[cosq_idx] == -1)) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "Profile %d Not a valid cos %d\n"),
                profile_index, cosq_idx));
            continue;
        }

        total_numq = sched_profile[profile_index].num_unicast_queue[cosq_idx] +
                     sched_profile[profile_index].num_multicast_queue[cosq_idx];
        num_ucq = sched_profile[profile_index].num_unicast_queue[cosq_idx];
        num_mcq = sched_profile[profile_index].num_multicast_queue[cosq_idx];
        sp = sched_profile[profile_index].strict_priority[cosq_idx];
        fc_uc = sched_profile[profile_index].flow_control_only_unicast[cosq_idx];

        if (total_numq == 0) {
            continue;
        }

        if (current_q_pos == SOC_TH3_COS_MAX) {
            return SOC_E_FAIL;
        }

        if (total_numq == 2) {
            L0[current_q_pos] = current_q_pos+1;
            L0[current_q_pos+1] = current_q_pos+1;
            schedq[current_q_pos] = current_q_pos;
            schedq[current_q_pos+1] = current_q_pos+1;
            cos_list[current_q_pos] = cosq_idx;
            cos_list[current_q_pos+1] = cosq_idx;
            strict_priority[current_q_pos] = sp;
            strict_priority[current_q_pos+1] = sp;
            fc_is_uc_only[current_q_pos] = fc_uc;
            fc_is_uc_only[current_q_pos+1] = fc_uc;
            if (num_ucq == 2) {
                mmuq[current_q_pos] = ucq_base++;
                mmuq[current_q_pos+1] = ucq_base++;
            }
            else if ((num_ucq == 1) && (num_mcq == 1)) {
                mmuq[current_q_pos] = ucq_base++;
                mmuq[current_q_pos+1] = mcq_base++;
            }
            current_q_pos += 2;
        } else if (total_numq == 1) {
            L0[current_q_pos] = current_q_pos;
            schedq[current_q_pos] = current_q_pos;
            cos_list[current_q_pos] = cosq_idx;
            strict_priority[current_q_pos] = sp;
            fc_is_uc_only[current_q_pos] = fc_uc;
            if (num_ucq == 1) {
                mmuq[current_q_pos] = ucq_base++;
            }
            else if (num_mcq == 1) {
                mmuq[current_q_pos] = mcq_base++;
            }
            current_q_pos += 1;
        } else {
            return SOC_E_PARAM;
        }
    }

    for (cosq_idx = 0; cosq_idx <  SOC_TH3_COS_MAX; cosq_idx++) {
        LOG_INFO(BSL_LS_SOC_COSQ,
            (BSL_META_U(unit, "Profile %d cos %d L0 %d schedq %d mmuq %d cos_list %d strict_priority %d\n"),
            profile_index, cosq_idx, L0[cosq_idx], schedq[cosq_idx],
            mmuq[cosq_idx], cos_list[cosq_idx], strict_priority[cosq_idx]));
    }
    return SOC_E_NONE;
}


#endif  /* defined (BCM_TOMAHAWK3_SUPPORT) */
