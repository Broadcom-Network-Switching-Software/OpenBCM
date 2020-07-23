/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        traffic_manager.c
 * Purpose:     Tomahawk3 IDB, EDB and MMU routines
 * Requires:
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/hash.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)

#include <soc/tomahawk3.h>
#include <soc/init/tomahawk3_idb_init.h>
#include <soc/init/tomahawk3_port_init.h>
#include <soc/tomahawk2_tdm.h>
#include <soc/init/tomahawk3_tdm.h>
#include <soc/init/tomahawk3_ep_init.h>
#include <bcm_int/common/tx.h>
#include <soc/flexport/tomahawk3_flexport.h>

soc_th3_mmu_info_t *th3_cosq_mmu_info[SOC_MAX_NUM_DEVICES];
soc_th3_sched_profile_info_t th3_sched_profile_info[SOC_MAX_NUM_DEVICES][SOC_TH3_MAX_NUM_SCHED_PROFILE][SOC_TH3_NUM_GP_QUEUES];
int L1_TO_L0_MAPPING[SOC_MAX_NUM_DEVICES][SOC_TH3_MAX_NUM_SCHED_PROFILE]
                                [SOC_TH3_NUM_GP_QUEUES];
int L0_TO_L1_MAPPING_NUM[SOC_MAX_NUM_DEVICES][SOC_TH3_MAX_NUM_SCHED_PROFILE]
                                        [SOC_TH3_NUM_GP_QUEUES];
int L0_TO_COSQ_MAPPING[SOC_MAX_NUM_DEVICES][SOC_TH3_MAX_NUM_SCHED_PROFILE]
                                        [SOC_TH3_NUM_GP_QUEUES];

int soc_th3_chip_queue_num_get(int unit, soc_port_t port, int qid, int type,
        int *hw_index) {
    /* type 0 UC queue, type 1 MC queue */
    int mmu_local_port, mmuq, pipe, num_ucq;
    uint32 pipe_map;
    soc_info_t *si = &SOC_INFO(unit);
    int q_base_list[] = {0, 276, 516, 744, 972, 1200, 1440, 1680};
    if (!SOC_PORT_VALID(unit, port)) {
        return SOC_E_PORT;
    }
    pipe = si->port_pipe[port];
    mmu_local_port = si->port_l2i_mapping[port];
    soc_tomahawk3_pipe_map_get(unit, &pipe_map);
    if (!(pipe_map & (1U << pipe))) {
        return SOC_E_PORT;
    }
    num_ucq = _soc_th3_get_num_ucq(unit);
    if (type == 0) {
        if (qid == -1) {
            /* return the starting q_num */
            *hw_index = q_base_list[pipe] + mmu_local_port * SOC_TH3_NUM_GP_QUEUES;
        } else {
            if (qid >= num_ucq) {
                return SOC_E_PARAM;
            }
            mmuq = qid;
            *hw_index = q_base_list[pipe] +
                    mmu_local_port * SOC_TH3_NUM_GP_QUEUES + mmuq;
        }
    } else if (type == 1) {
        if (qid == -1) {
            *hw_index = q_base_list[pipe] +
                mmu_local_port * SOC_TH3_NUM_GP_QUEUES + num_ucq;

        } else {
            if (qid >= SOC_TH3_NUM_GP_QUEUES - num_ucq) {
                return SOC_E_PARAM;
            }
            mmuq = num_ucq + qid;
            *hw_index = q_base_list[pipe] +
                    mmu_local_port * SOC_TH3_NUM_GP_QUEUES + mmuq;
        }
    }
    return SOC_E_NONE;
}

uint32
soc_th3_mmu_chip_port_num (int unit, int port)
{
    soc_info_t *si = &SOC_INFO(unit);
    int global_mmu_port, phy_port, mmu_chip_port, pipe;
    pipe = si->port_pipe[port];
    phy_port = si->port_l2p_mapping[port];
    global_mmu_port = si->port_p2m_mapping[phy_port];
    mmu_chip_port = (global_mmu_port % SOC_TH3_MMU_PORT_STRIDE) +
                    (pipe * SOC_TH3_MAX_DEV_PORTS_PER_PIPE);
    return mmu_chip_port;
}

uint32
soc_th3_mmu_local_port_num (int unit, int port)
{
    soc_info_t *si = &SOC_INFO(unit);
    return si->port_l2i_mapping[port];
}


/*
 *Function: soc_tomahawk3_mem_is_itm
 *Purpose: check if memory is in ITM block
 *Params:
 *  unit: Device unit number
 *  mem: base memory
 */
int soc_tomahawk3_mem_is_itm(int unit, soc_mem_t mem)
{
    int block_info_index;

    if (mem != INVALIDm) {
        if (SOC_MEM_IS_VALID(unit, mem)) {
           block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);
           if (SOC_BLOCK_TYPE(unit, block_info_index) == SOC_BLK_MMU_ITM) {
               return 1;
           }
        }
    }
    return 0;
}

/*
 *Function: soc_tomahawk3_reg_is_itm
 *Purpose: check if register is in ITM block
 *Params:
 *  unit: Device unit number
 *  mem: base register
 */
int soc_tomahawk3_reg_is_itm(int unit, soc_reg_t reg)
{
    if (reg != INVALIDr) {
        if (SOC_REG_IS_VALID(unit, reg)) {
            return (SOC_REG_BLOCK_IS(unit, reg, SOC_BLK_MMU_ITM));
        }
    }

    return 0;
}


/*Function to check if the itm, base_index combination is valid
 *Base Type: IPORT/IPIPE: Pipes 0,1,6,7 ITM0, Pipes 2,3,4,5 ITM1
 *Base Type: EPORT/EPIPE: Can be in both ITMs;
 *ITM should be valied for device config (esp. for half setups)
 *Base Type: ITM: Passed ITM is valid
 */
int
soc_tomahawk3_itm_base_index_check(int unit, int base_type, int itm,
                                  int base_index, char *msg)
{
    soc_info_t *si;
    int pipe;
    uint32 map;
    char *base_name;

    si = &SOC_INFO(unit);

    if (itm == -1 || base_index == -1) {
        return SOC_E_NONE;
    }

    if (itm < NUM_ITM(unit) && si->itm_ipipe_map[itm] == 0) {
        if (msg) {
            LOG_CLI((BSL_META_U(unit,
                                "%s: ITM%d is not in config\n"),
                     msg, itm));
        }
        return SOC_E_PARAM;
    }

    switch (base_type) {
    case 0: /* IPORT */
    case 1: /* EPORT */
        base_name = base_type == 0 ? "IPORT" : "EPORT";
        pipe = si->port_pipe[base_index];
        if (pipe == -1) {
            if (msg) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in config\n"),
                         msg, base_name, base_index));
            }
        } else if (itm < NUM_ITM(unit)) { /* Unique access type */
            map = base_type == 0 ?
                si->ipipe_itm_map[pipe] : si->itm_map;
            if (map & (1 << itm)) {
                break;
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in ITM%d\n"),
                         msg, base_name, base_index, itm));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (base_type == 2) {
            base_name = "IPIPE";
            map = si->ipipe_itm_map[base_index];
        } else {
            base_name = "EPIPE";
            map = si->itm_map;
        }
        if (map == 0) {
            if (msg) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in config\n"),
                         msg, base_name, base_index));
            }
        } else if (itm < NUM_ITM(unit)) { /* Unique access type */
            if (map & (1 << itm)) {
                break;
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in ITM%d\n"),
                         msg, base_name, base_index, itm));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 4: /* CHIP */
        break;
    case 5: /* ITM*/
        if (si->itm_ipipe_map[base_index] != 0) {
            break;
        }
        if (msg != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "%s: ITM%d is not in config\n"),
                     msg, base_index));
        }
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

/*
 *Function: _soc_tomahawk3_itm_reg_check
 *Purpose: Check if resigter is in ITM block and passed ITM value is in range
 *         Override itm with access_type for unique memories
 *         Call check to verify itm/base_index combination
 *Params:
 *  unit: Device unit number
 *  reg:  Register name
 *  itm:  ITM#
 *  base_index: base index to be accessed
 */
STATIC int
_soc_tomahawk3_itm_reg_check(int unit, soc_reg_t reg, int itm, int base_index)
{
    int acc_type, base_type;

    if (!SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, reg).block, SOC_BLK_MMU_ITM)) {
        LOG_CLI((BSL_META_U(unit,
                            "%s is not ITM register\n"), SOC_REG_NAME(unit, reg)));
        return SOC_E_PARAM;
    }

    if (SOC_REG_UNIQUE_ACC(unit, reg) != NULL) { /* UNIQUE base register */
        if (itm == -1 || itm >= NUM_ITM(unit)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s bad ITM value %d\n"),
                     SOC_REG_NAME(unit, reg), itm));
            return SOC_E_PARAM;
        }
    } else {
        acc_type = SOC_REG_ACC_TYPE(unit, reg);
        if (acc_type < NUM_ITM(unit)) { /* UNIQUE per ITM register */
            if (itm != acc_type) {
                LOG_CLI((BSL_META_U(unit,
                                    "Ovveride ITM value %d with ACC_TYPE of %s\n"),
                         itm, SOC_REG_NAME(unit, reg)));
            }
            itm = acc_type;
        } else { /* non-UNIQUE register */
            return SOC_E_NONE;
        }
    }
    /*retrieve bits [25:23] of memory address for base_type*/
    base_type = ((SOC_REG_INFO(unit, reg).offset) >> 23) & 0x7;

    return soc_tomahawk3_itm_base_index_check(unit, base_type, itm, base_index,
                                             SOC_REG_NAME(unit, reg));
}

/*
 * Parameters for soc_tomahawk3_itm_reg_access:
 *
 *    base unique  reg/  itm   write action          read action
 *   index  type   mem
 *    ==== ====== ===== ===   ================      =================
 * #1   -1  no     -     -    all ITMs        first pipe in the first ITM
 * #2   -1  yes   base   -1   (same as #1)
 * #3   -1  yes   base  0-3   all pipes in the ITM  first pipe in the ITM
 * #4   -1  yes  itm0-3  -    (same as #3)
 * #5  0-1  no     -     -    applicable ITMs       first ITM has the pipe
 *
 * - S/W will loop through base types (i.e. pipes) if base_index==-1
 * - S/W will loop through applicable ITMs if itm==-1 on unique access type
 *   base view (H/W will do the loop for other access types)
 */
STATIC int
_soc_tomahawk3_itm_reg_access(int unit, soc_reg_t reg, int itm, int base_index,
                             int index, uint64 *data, int write)
{
    soc_info_t *si;
    soc_reg_t orig_reg;
    int port, pipe;
    int base_type, break_after_first;
    int loop_index;
    uint32 base_index_map, itm_map=0;
    soc_pbmp_t base_index_pbmp;
    uint32 inst;

    si = &SOC_INFO(unit);
    orig_reg = reg;
    /*retrieve bits [25:23] of memory address for base_type*/
    base_type = ((SOC_REG_INFO(unit, reg).offset) >> 23) & 0x7;
    break_after_first = TRUE;

    if (itm >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk3_itm_reg_check(unit, reg, itm, base_index));
    }

    if (base_index < -1) {
            return SOC_E_PARAM;
    }
    switch (base_type) {
    case 0: /* IPORT */
    case 1: /* EPORT */
        if (itm >= 0 && SOC_REG_UNIQUE_ACC(unit, reg) != NULL) {
            reg = SOC_REG_UNIQUE_ACC(unit, reg)[itm];
        }
        if (base_index == -1) {
            SOC_PBMP_ASSIGN(base_index_pbmp, PBMP_ALL(unit));
        } else {
            /* This argument is logical port, same as soc_reg_get/set */
            SOC_PBMP_PORT_SET(base_index_pbmp, base_index);
        }
        SOC_PBMP_ITER(base_index_pbmp, port) {
            pipe = si->port_pipe[port];
            if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
                itm_map = base_type == 0 ?
                    si->ipipe_itm_map[pipe]:
                    si->itm_map;

                if (write) {
                    break_after_first = FALSE;
                }
            }
            for (itm = 0; itm < NUM_ITM(unit); itm++) {
                if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
                    if (!(itm_map & (1 << itm))) {
                        continue;
                    }
                    reg = SOC_REG_UNIQUE_ACC(unit, orig_reg)[itm];
                }
                if (write) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_set(unit, reg, port, index, *data));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_get(unit, reg, port, index, data));
                }
                if (break_after_first) {
                    break;
                }
            }
        }
        break;
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (itm >= 0 && SOC_REG_UNIQUE_ACC(unit, reg) != NULL) {
            reg = SOC_REG_UNIQUE_ACC(unit, reg)[itm];
        }
        soc_tomahawk3_pipe_map_get(unit, &base_index_map);
        if (base_index != -1) {
            base_index_map &= 1 << base_index;
            if (base_index_map == 0) {
                return SOC_E_PARAM;
            }
        }

        for (base_index = 0; base_index < _TH3_PIPES_PER_DEV; base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            /* Select the right ITM for IPIPE and both ITMs for EPIPE*/
            if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
                itm_map = base_type == 2 ?
                    si->ipipe_itm_map[base_index]:
                    si->itm_map;
                if (write) {
                    break_after_first = FALSE;
                }
            }
            /*Check the ITM value passed through params*/
            if (itm >=0) {
                itm_map &= (1 << itm);
            }
            for (itm = 0; itm < NUM_ITM(unit); itm++) {
                if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
                    if (!(itm_map & (1 << itm))) {
                        continue;
                    }
                    reg = SOC_REG_UNIQUE_ACC(unit, orig_reg)[itm];
                }
                inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
                if (write) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_set(unit, reg, inst, index, *data));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_get(unit, reg, inst, index, data));
                }
                if (break_after_first) {
                    break;
                }
            }
        }
        break;
    case 4: /* CHIP */
        if (write) {
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, 0, index, *data));
        } else {
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, 0, index, data));
        }
        break;
    case 5: /* ITM */
        if (itm >= 0 && SOC_REG_UNIQUE_ACC(unit, reg) != NULL) {
            reg = SOC_REG_UNIQUE_ACC(unit, reg)[itm];
        }

        if (SOC_REG_UNIQUE_ACC(unit, reg) != NULL ||
            SOC_REG_ACC_TYPE(unit, reg) < NUM_ITM(unit)) {
            base_index_map = 1;
        } else if (base_index == -1) {
            base_index_map = 0x3; /*Use both ITMs*/
        } else {
            base_index_map = 1 << base_index;
        }

        /* Loop through ITM(s) only on UNIQUE type base register */
        if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
            itm_map=si->itm_map;
            break_after_first = FALSE;
        }

        for (loop_index = 0; loop_index < NUM_ITM(unit); loop_index++) {
            if (!(base_index_map & (1 << loop_index))) {
                continue;
            }
            for (itm = 0; itm < NUM_ITM(unit); itm++) {
                if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
                    if (!(itm_map & (1 << itm))) {
                        continue;
                    }
                    reg = SOC_REG_UNIQUE_ACC(unit, orig_reg)[itm];
                }
                inst = loop_index | SOC_REG_ADDR_INSTANCE_MASK;
                if (write) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_set(unit, reg, inst, index, *data));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_get(unit, reg, inst, index, data));
                }
                if (break_after_first) {
                    break;
                }
            }
        }
        break;
    /* No default case as base_type can have value only between 0-5 */
    }

    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_itm_reg32_set
 *Purpose: Set ITM register
 */
int
soc_tomahawk3_itm_reg32_set(int unit, soc_reg_t reg, int itm, int base_index,
                           int index, uint32 data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, data);
    SOC_IF_ERROR_RETURN(_soc_tomahawk3_itm_reg_access(unit, reg, itm, base_index,
                                                     index, &data64, TRUE));

    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_itm_reg32_get
 *Purpose: Get ITM register
 */
int
soc_tomahawk3_itm_reg32_get(int unit, soc_reg_t reg, int itm, int base_index,
                           int index, uint32 *data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, *data);
    SOC_IF_ERROR_RETURN(_soc_tomahawk3_itm_reg_access(unit, reg, itm, base_index,
                                                     index, &data64, FALSE));
    *data = COMPILER_64_LO(data64);

    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_itm_reg_set
 *Purpose: Set ITM register
 */
int
soc_tomahawk3_itm_reg_set(int unit, soc_reg_t reg, int itm, int base_index,
                         int index, uint64 data)
{
    return _soc_tomahawk3_itm_reg_access(unit, reg, itm, base_index,
                                        index, &data, TRUE);
}

/*
 *Function: soc_tomahawk3_itm_reg_get
 *Purpose: Get ITM register
 */
int
soc_tomahawk3_itm_reg_get(int unit, soc_reg_t reg, int itm, int base_index,
                         int index, uint64 *data)
{
    return _soc_tomahawk3_itm_reg_access(unit, reg, itm, base_index,
                                        index, data, FALSE);
}

/*
 *Function to check if the EB, base_index combination is valid
 *Base Type: IPORT/IPIPE: Can be in any of the 8 EBs
 *Base Type: EPORT/EPIPE: Can only be in one EB corresponding to the port/pipe.
 *Base Type: ITM: Can be in any EB
 *EB should be valied for device config (esp. for half setups)
 */
int
soc_tomahawk3_eb_base_index_check(int unit, int base_type, int eb,
                                 int base_index, char *msg)
{
    soc_info_t *si;
    int pipe;
    uint32 map, pipe_map =0;
    char *base_name;


    si = &SOC_INFO(unit);

    if (eb == -1 || base_index == -1) {
        return SOC_E_NONE;
    }

    soc_tomahawk3_pipe_map_get(unit, &pipe_map);

    if ((eb < _TH3_PIPES_PER_DEV) && ((pipe_map & (1 << eb)) == 0)) {
        if (msg) {
            LOG_CLI((BSL_META_U(unit,
                                "%s: EB%d is not in config\n"),
                     msg, eb));
        }
        return SOC_E_PARAM;
    }

    switch (base_type) {
    case 0: /* IPORT */
    case 1: /* EPORT */
        base_name = base_type == 0 ? "IPORT" : "EPORT";
        pipe = si->port_pipe[base_index];
        if (pipe == -1) {
            if (msg) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in config\n"),
                         msg, base_name, base_index));
            }
        } else if (eb < _TH3_PIPES_PER_DEV) { /* Unique access type */
            map = base_type == 0 ?
                pipe_map : (1 << eb);
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in EB%d\n"),
                         msg, base_name, base_index, eb));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (base_type == 2) {
            base_name = "IPIPE";
            map = pipe_map;
        } else {
            base_name = "EPIPE";
            map = (1 << eb);
        }
        if (map == 0) {
            if (msg) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in config\n"),
                         msg, base_name, base_index));
            }
        } else if (eb < _TH3_PIPES_PER_DEV) { /* Unique access type */
            if (map & (1 << eb)) {
                break;
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in EB%d\n"),
                         msg, base_name, base_index, eb));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 4: /* CHIP */
        break;
    case 5: /* ITM */
        if ((si->itm_map & (1 << base_index)) == 0) {
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: ITM%d is not in config\n"),
                         msg, base_index));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

/*
 *Function: _soc_tomahawk3_eb_reg_check
 *Purpose: Check if resigter is in EB block and passed EB value is in range
 *         Override itm with access_type for unique memories
 *         Call check to verify EB/base_index combination
 */
STATIC int
_soc_tomahawk3_eb_reg_check(int unit, soc_reg_t reg, int eb, int base_index)
{
    int acc_type, base_type;

    if (!SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, reg).block, SOC_BLK_MMU_EB)) {
        LOG_CLI((BSL_META_U(unit,
                            "%s is not EB register\n"), SOC_REG_NAME(unit, reg)));
        return SOC_E_PARAM;
    }

    if (SOC_REG_UNIQUE_ACC(unit, reg) != NULL) { /* UNIQUE base register */
        if (eb == -1 || eb >= _TH3_PIPES_PER_DEV) {
            LOG_CLI((BSL_META_U(unit,
                                "%s bad EB value %d\n"),
                     SOC_REG_NAME(unit, reg), eb));
            return SOC_E_PARAM;
        }
    } else {
        acc_type = SOC_REG_ACC_TYPE(unit, reg);
        if (acc_type < _TH3_PIPES_PER_DEV) { /* UNIQUE per EB register */
            if (eb != acc_type) {
                LOG_CLI((BSL_META_U(unit,
                                    "Ovveride EB value %d with ACC_TYPE of %s\n"),
                         eb, SOC_REG_NAME(unit, reg)));
            }
            eb = acc_type;
        } else { /* non-UNIQUE register */
            return SOC_E_NONE;
        }
    }
    /*retrieve bits [25:23] of memory address for base_type*/
    base_type = ((SOC_REG_INFO(unit, reg).offset) >> 23) & 0x7;

    return soc_tomahawk3_eb_base_index_check(unit, base_type, eb, base_index,
                                            SOC_REG_NAME(unit, reg));
}

/*
 *Function: _soc_tomahawk3_eb_reg_access
 *Purpose: Set/get resgisters in EB block
 */
STATIC int
_soc_tomahawk3_eb_reg_access(int unit, soc_reg_t reg, int eb, int base_index,
                            int index, uint64 *data, int write)
{
    soc_info_t *si;
    int port;
    int base_type, loop_index;
    uint32 base_index_map;
    soc_pbmp_t base_index_pbmp;
    uint32 inst;

    si = &SOC_INFO(unit);
    /*retrieve bits [25:23] of memory address for base_type*/
    base_type = ((SOC_REG_INFO(unit, reg).offset) >> 23) & 0x7;

    if (eb >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk3_eb_reg_check(unit, reg, eb, base_index));
    }

    switch (base_type) {
    case 0: /* IPORT */
    case 1: /* EPORT */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            SOC_PBMP_ASSIGN(base_index_pbmp, PBMP_ALL(unit));
        } else {
            /* This argument is logical port, same as soc_reg_get/set */
            SOC_PBMP_PORT_SET(base_index_pbmp, base_index);
        }

        SOC_PBMP_ITER(base_index_pbmp, port) {
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, index, data));
            }
        }
        break;
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        /* No unique access type for such base_type */
        soc_tomahawk3_pipe_map_get(unit, &base_index_map);
        if (base_index != -1) {
            base_index_map &= 1 << base_index;
            if (base_index_map == 0) {
                return SOC_E_PARAM;
            }
        }

        for (loop_index = 0; loop_index < _TH3_PIPES_PER_DEV; loop_index++) {
            if (!(base_index_map & (1 << loop_index))) {
                continue;
            }
            inst = loop_index | SOC_REG_ADDR_INSTANCE_MASK;
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, inst, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, inst, index, data));
            }
        }
        break;
    case 4: /* CHIP */
        if (write) {
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, 0, index, *data));
        } else {
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, 0, index, data));
        }
        break;
    case 5: /* ITM */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            base_index_map = si->itm_map;
        } else {
            base_index_map = 1 << base_index;
        }

        for (loop_index = 0; loop_index < NUM_ITM(unit); loop_index++) {
            if (!(base_index_map & (1 << loop_index))) {
                continue;
            }
            inst = loop_index | SOC_REG_ADDR_INSTANCE_MASK;
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, inst, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, inst, index, data));
            }
        }
        break;
    default:
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

/*
 *Function: _soc_tomahawk3_eb_reg32_set
 *Purpose: Set resgisters in EB block
 */
int
soc_tomahawk3_eb_reg32_set(int unit, soc_reg_t reg, int eb, int base_index,
                          int index, uint32 data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, data);
    SOC_IF_ERROR_RETURN(_soc_tomahawk3_eb_reg_access(unit, reg, eb, base_index,
                                                    index, &data64, TRUE));

    return SOC_E_NONE;
}

/*
 *Function: _soc_tomahawk3_eb_reg32_get
 *Purpose: Get resgisters in EB block
 */
int
soc_tomahawk3_eb_reg32_get(int unit, soc_reg_t reg, int eb, int base_index,
                          int index, uint32 *data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, *data);
    SOC_IF_ERROR_RETURN(_soc_tomahawk3_eb_reg_access(unit, reg, eb, base_index,
                                                    index, &data64, FALSE));
    *data = COMPILER_64_LO(data64);

    return SOC_E_NONE;
}

/*
 *Function: _soc_tomahawk3_eb_reg_set
 *Purpose: Set resgisters in EB block
 */
int
soc_tomahawk3_eb_reg_set(int unit, soc_reg_t reg, int eb, int base_index,
                        int index, uint64 data)
{
    return _soc_tomahawk3_eb_reg_access(unit, reg, eb, base_index,
                                       index, &data, TRUE);
}

/*
 *Function: _soc_tomahawk3_eb_reg_get
 *Purpose: Get resgisters in EB block
 */
int
soc_tomahawk3_eb_reg_get(int unit, soc_reg_t reg, int eb, int base_index,
                        int index, uint64 *data)
{
    return _soc_tomahawk3_eb_reg_access(unit, reg, eb, base_index,
                                       index, data, FALSE);
}

/*
 *Function: _soc_tomahawk3_itm_mem_check
 *Purpose: Check 1. If memory is in ITM block
 *               2. If the passed ITM value is in range
 *         Override ITM value with access type of the ITM memory
 */
STATIC int
_soc_tomahawk3_itm_mem_check(int unit, soc_mem_t mem, int itm, int base_index)
{
    int block_info_index, acc_type, base_type;

    block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);
    if (SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_ITM) {
        LOG_CLI((BSL_META_U(unit,
                            "%s is not ITM memory\n"), SOC_MEM_NAME(unit, mem)));
        return SOC_E_PARAM;
    }

    if (SOC_MEM_UNIQUE_ACC(unit, mem) != NULL) { /* UNIQUE base memory */
        if (itm == -1 || itm >= NUM_ITM(unit)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s bad ITM value %d\n"),
                     SOC_MEM_NAME(unit, mem), itm));
            return SOC_E_PARAM;
        }
    } else {
        acc_type = SOC_MEM_ACC_TYPE(unit, mem);
        if (acc_type < NUM_ITM(unit)) { /* UNIQUE per ITM memory */
            if (itm != acc_type) {
                LOG_CLI((BSL_META_U(unit,
                                    "Ovveride ITM value %d with ACC_TYPE of %s\n"),
                         itm, SOC_MEM_NAME(unit, mem)));
                itm = acc_type;
            }
        } else { /* non-UNIQUE memory */
            return SOC_E_NONE;
        }
    }
    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;

    return soc_tomahawk3_itm_base_index_check(unit, base_type, itm, base_index,
                                             SOC_MEM_NAME(unit, mem));
}

/*Function to retrieve the ITM memory list corresponding to specific ITM or base_type
 *
 * Base Type:0/1: IPORT/EPORT: No MMU memories available with this base type
 * Base Type:2: IPIPE: returns _PIPEx memory list
 * Base Type:3: EPIPE: returns _ITMx_PIPEy if acc_type is UNIQUE
 *                     returns _PIPEx memories for non-unique memories
 * Base Type:4: CHIP:  returns _ITMx mem list if acc_type = UNIQUE
 * Base Type:5: ITM:   returns _ITMx mem list if acc_type = UNIQUE
 */
STATIC int
_soc_tomahawk3_retrieve_itm_mem_list(int unit, soc_mem_t mem, int itm,
        int base_index, soc_mem_t *mem_list, int max_mem_cnt, int *number_of_mems)
{
#define _TH3_MMU_SBUS_ACC_TYPE_UNIQUE 31
    soc_info_t *si;
    int base_type, acc_type;
    uint32 base_index_map, itm_map=0;
    int mem_index = 0;
    int loop_index;

    si = &SOC_INFO(unit);
    /*retrieve bits [25:23] of memory address for base_type*/
    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;

    switch (base_type) {
        case 2: /* IPIPE */
        case 3: /* EPIPE */
            if (max_mem_cnt < (NUM_ITM(unit) * _TH3_PIPES_PER_DEV)) {
                /*Max combinations is 2 ITMs * 8 PIPEs */
                return SOC_E_INTERNAL;
            }
            /* Memory doesnt have enumeration */
            if (SOC_MEM_UNIQUE_ACC(unit, mem) == NULL) {
                mem_list[mem_index++] = mem;
                break;
            }
            soc_tomahawk3_pipe_map_get(unit, &base_index_map);
            if (base_index != -1) {
                base_index_map &= (1 << base_index);
                if (base_index_map == 0) {
                    return SOC_E_PARAM;
                }
            }
            for (loop_index = 0; loop_index < _TH3_PIPES_PER_DEV; loop_index++) {
                if (!(base_index_map & (1 << loop_index))) {
                    continue;
                }
                acc_type = SOC_MEM_ACC_TYPE
                            (unit, SOC_MEM_UNIQUE_ACC_PIPE(unit, mem, 0));
                if ((acc_type != _TH3_MMU_SBUS_ACC_TYPE_UNIQUE)|| base_type ==2) {
                    /* Single/Duplicate acc_type memories only have _PIPEx*/
                    mem_list[mem_index++] =
                        SOC_MEM_UNIQUE_ACC_PIPE(unit, mem, loop_index);
                } else {
                    /*Unique acc_type memories with _ITMx_PIPEy*/
                    itm_map = si->itm_map;
                    if (itm != -1) {
                        itm_map &= (1 << itm);
                        if (itm_map == 0) {
                            return SOC_E_PARAM;
                        }
                    }
                    for (itm = 0; itm < NUM_ITM(unit); itm++) {
                        if (!(itm_map & (1 << itm))) {
                            continue;
                        }
                        mem_list[mem_index++] =
                            SOC_MEM_UNIQUE_ACC_ITM_PIPE
                                            (unit, mem, itm, loop_index);
                                        LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "mmu_mem %d added to list \n"),
                           SOC_MEM_UNIQUE_ACC_ITM_PIPE
                                            (unit, mem, itm, loop_index)));
                    }
                }
            }
            break;
        case 4: /* CHIP */
        case 5: /* ITM */
           if (max_mem_cnt < NUM_ITM(unit)) {
                /*Max combinations is 2 ITMs */
                return SOC_E_INTERNAL;
            }

            /* No memory enumeration available. Non-unique memories */
            if (SOC_MEM_UNIQUE_ACC(unit, mem) == NULL) {
                mem_list[mem_index++] = mem;
                break;
            }
            itm_map = si->itm_map;
            if (itm != -1) {
                itm_map &= (1 << itm);
                if (itm_map == 0) {
                    return SOC_E_PARAM;
                }
            }
            if (base_index != -1) {
                itm_map &= (1 << base_index);
                if (itm_map == 0) {
                    return SOC_E_PARAM;
                }
            }

            /* Unique memories with _ITMx */
            for (loop_index = 0; loop_index < NUM_ITM(unit); loop_index++) {
                if (!(itm_map & (1 << loop_index))) {
                    continue;
                }
                mem_list[mem_index++] =
                    SOC_MEM_UNIQUE_ACC_ITM(unit, mem, loop_index);
            }
            break;
        case 0: /* IPORT */
        case 1: /* EPORT */
        default:
            return SOC_E_INTERNAL;
    }
    *number_of_mems = mem_index;

#undef _TH3_MMU_SBUS_ACC_TYPE_UNIQUE
    return SOC_E_NONE;
}

/*Function to read/write all ITM memories based on memory list retrieval*/
STATIC int
_soc_tomahawk3_itm_mem_access(int unit, soc_mem_t mem, int itm, int base_index,
                             int copyno, int offset_in_section,
                             void *entry_data, int write)
{
#define _TH3_MMU_SBUS_ACC_TYPE_UNIQUE 31
    int mem_index, mem_cnt;
    int index;
    soc_mem_t mem_list[64] = {INVALIDm};

    if (itm >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk3_itm_mem_check(unit, mem, itm, base_index));
    }
    SOC_IF_ERROR_RETURN
        (_soc_tomahawk3_retrieve_itm_mem_list (unit, mem, itm, base_index,
                                            mem_list, 64, &mem_cnt));
    index = offset_in_section;
    if(write) {
        /*Write to all memories in list*/
        for (mem_index = 0; mem_index < mem_cnt; mem_index ++) {
            SOC_IF_ERROR_RETURN(soc_mem_write
                (unit, mem_list[mem_index], copyno, index, entry_data));
        }
    } else {
        /*Read only from the 1st memory in list*/
        SOC_IF_ERROR_RETURN(soc_mem_read
                (unit, mem_list[0], copyno, index, entry_data));
    }
#undef _TH3_MMU_SBUS_ACC_TYPE_UNIQUE
    return SOC_E_NONE;
}

/*Function to read/write all ITM memories based on memory list retrieval*/
STATIC int
_soc_tomahawk3_itm_mem_access_range(int unit, soc_mem_t mem, int itm, int base_index,
                             int copyno, int idx_min, int idx_max,
                             void *entry_data, int write)
{
#define _TH3_MMU_SBUS_ACC_TYPE_UNIQUE 31
    int mem_cnt;
    soc_mem_t mem_list[64] = {INVALIDm};

    if (itm >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk3_itm_mem_check(unit, mem, itm, base_index));
    }
    SOC_IF_ERROR_RETURN
        (_soc_tomahawk3_retrieve_itm_mem_list (unit, mem, itm, base_index,
                                            mem_list, 64, &mem_cnt));
    if(write) {
        /* Write goes here, but since this was made for WB, not supported */
    } else {
        /*Read only from the 1st memory in list, but read it all*/
        SOC_IF_ERROR_RETURN(soc_mem_read_range
                (unit, mem_list[0], MEM_BLOCK_ANY,
                 idx_min, idx_max, entry_data));
    }
#undef _TH3_MMU_SBUS_ACC_TYPE_UNIQUE
    return SOC_E_NONE;
}
/*
 * Function: soc_tomahawk3_itm_mem_write
 * Purpose: Write to memories of different acc_type/base_type in the ITM block
 */
int
soc_tomahawk3_itm_mem_write(int unit, soc_mem_t mem, int itm, int base_index,
                           int copyno, int offset_in_section, void *entry_data)
{
    return _soc_tomahawk3_itm_mem_access(unit, mem, itm, base_index, copyno,
                                        offset_in_section, entry_data, TRUE);
}

/*
 * Function: soc_tomahawk3_itm_mem_read
 * Purpose: Read memories of different acc_type/base_type in the ITM block
 */
int
soc_tomahawk3_itm_mem_read(int unit, soc_mem_t mem, int itm, int base_index,
                          int copyno, int offset_in_section, void *entry_data)
{
    return _soc_tomahawk3_itm_mem_access(unit, mem, itm, base_index, copyno,
                                        offset_in_section, entry_data, FALSE);
}

int
soc_tomahawk3_itm_mem_read_range(int unit, soc_mem_t mem, int itm, int base_index,
                          int copyno, int idx_min, int idx_max, void *entry_data)
{
    return _soc_tomahawk3_itm_mem_access_range(unit, mem, itm, base_index, copyno,
                                        idx_min, idx_max, entry_data, FALSE);
}
STATIC int
_soc_tomahawk3_eb_mem_check(int unit, soc_mem_t mem, int eb, int base_index)
{
    int block_info_index, acc_type, base_type;

    block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);
    if (SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_EB) {
        LOG_CLI((BSL_META_U(unit,
                            "%s is not EB memory\n"), SOC_MEM_NAME(unit, mem)));
        return SOC_E_PARAM;
    }

    if (SOC_MEM_UNIQUE_ACC(unit, mem) != NULL) { /* UNIQUE base memory */
        if (eb == -1 || eb >= _TH3_PIPES_PER_DEV) {
            LOG_CLI((BSL_META_U(unit,
                                "%s bad PIPE value %d\n"),
                     SOC_MEM_NAME(unit, mem), eb));
            return SOC_E_PARAM;
        }
    } else {
        acc_type = SOC_MEM_ACC_TYPE(unit, mem);
        if (acc_type < _TH3_PIPES_PER_DEV) { /* UNIQUE per EB memory */
            if (eb != acc_type) {
                LOG_CLI((BSL_META_U(unit,
                                    "Ovveride EB value %d with ACC_TYPE of %s\n"),
                         eb, SOC_MEM_NAME(unit, mem)));
                eb = acc_type;
            }
        } else { /* non-UNIQUE memory */
            return SOC_E_NONE;
        }
    }
    /*retrieve bits [25:23] of memory address for base_type*/
    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;

    return soc_tomahawk3_eb_base_index_check(unit, base_type, eb, base_index,
                                            SOC_MEM_NAME(unit, mem));
}

/*Function to retrieve the EB memory list corresponding to specific EB or base_type
 *
 * Base Type:0/1: IPORT/EPORT: No MMU memories available with this base type
 * Base Type:3: EPIPE: returns _PIPEx memory list
 * Base Type:2: IPIPE: returns _EBx_PIPEy if acc_type is UNIQUE
 *                     returns _PIPEx memories for non-unique memories
 * Base Type:4: CHIP:  returns _PIPEx mem list if acc_type = UNIQUE
 * Base Type:5: ITM:   returns _EBx_ITMy mem list if acc_type = UNIQUE
 *                     returns _ITMx mem list for non-unique memories
 */
STATIC int
_soc_tomahawk3_retrieve_eb_mem_list (int unit, soc_mem_t mem, int eb,
                           int base_index, soc_mem_t *mem_list, int max_mem_cnt,
                           int *mem_cnt)
{
    #define _TH3_MMU_SBUS_ACC_TYPE_UNIQUE 31
    soc_info_t *si;
    int mem_index;
    int base_type, acc_type;
    uint32 base_index_map, eb_map=0;
    int loop_index;

    si = &SOC_INFO(unit);
    /*retrieve bits [25:23] of memory address for base_type*/
    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;
    mem_index = 0;

    if (eb >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk3_eb_mem_check(unit, mem, eb, base_index));
    }
    switch (base_type) {
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (max_mem_cnt < (_TH3_PIPES_PER_DEV * _TH3_PIPES_PER_DEV)) {
            /*Max combinations is 8 EBs * 8 PIPEs*/
            return SOC_E_INTERNAL;
        }
        /* no _PIPEx, no _ITMx */
        if (SOC_MEM_UNIQUE_ACC(unit, mem) == NULL) {
            mem_list[mem_index++] = mem;
            break;
        }
        soc_tomahawk3_pipe_map_get(unit, &base_index_map);
        eb_map = base_index_map;
        if (base_index != -1) {
            base_index_map &= (1 << base_index);
            if (base_index_map == 0) {
                return SOC_E_PARAM;
            }
        }
        for (loop_index = 0; loop_index < _TH3_PIPES_PER_DEV; loop_index++) {
            if (!(base_index_map & (1 << loop_index))) {
                continue;
            }
            acc_type = SOC_MEM_ACC_TYPE
                        (unit, SOC_MEM_UNIQUE_ACC_PIPE(unit, mem, 0));
            if ((acc_type != _TH3_MMU_SBUS_ACC_TYPE_UNIQUE)|| (base_type ==3)) {
                /* Single/Duplicate acc_type memories only have _PIPEx*/
                /*Base_type EPIPE memories in block EB have same behavior as Single*/
                mem_list[mem_index++] =
                    SOC_MEM_UNIQUE_ACC_PIPE(unit, mem, loop_index);
            } else {
                /*Unique acc_type memories with _EBx_PIPEy*/
                for (eb = 0; eb < _TH3_PIPES_PER_DEV; eb++) {
                    if (!(eb_map & (1 << eb))) {
                        continue;
                    }
                    mem_list[mem_index++] =
                        SOC_MEM_UNIQUE_ACC_EB_PIPE
                                        (unit, mem, eb, loop_index);
                }
            }
        }
        break;
    case 4: /* CHIP */
        if (max_mem_cnt < _TH3_PIPES_PER_DEV * _TH3_PIPES_PER_DEV) {
            /*Max combinations is 8 EBs*/
            return SOC_E_INTERNAL;
        }
         if (SOC_MEM_UNIQUE_ACC(unit, mem) == NULL) {
            mem_list[mem_index++] = mem;
            break;
        }
        soc_tomahawk3_pipe_map_get(unit, &eb_map);
        if (eb != -1) {
            eb_map &= (1 << eb);
            if (eb_map == 0) {
                return SOC_E_PARAM;
            }
        }
        /*Unique memories across different EBs are enumerated with _PIPEx */
        for (eb = 0; eb < _TH3_PIPES_PER_DEV; eb++) {
            if (!(eb_map & (1 << eb))) {
                continue;
            }
            mem_list[mem_index++] =
                SOC_MEM_UNIQUE_ACC_PIPE(unit, mem, eb);
        }
        break;
    case 5: /* ITM */
        if (max_mem_cnt < NUM_ITM(unit) * _TH3_PIPES_PER_DEV) {
            /*Max combinations is 8 EBs * 2 ITMs */
            return SOC_E_INTERNAL;
        }
        /* no _ITMx, _PIPEx */
        if (SOC_MEM_UNIQUE_ACC(unit, mem) == NULL) {
            mem_list[mem_index++] = mem;
            break;
        }
        soc_tomahawk3_pipe_map_get(unit, &eb_map);
        if (eb != -1) {
            eb_map &= (1 << eb);
            if (eb_map == 0) {
                return SOC_E_PARAM;
            }
        }
        base_index_map = si->itm_map;
        if (base_index != -1) {
            base_index_map &= (1 << base_index);
            if (base_index_map== 0) {
                return SOC_E_PARAM;
            }
        }
        for (loop_index = 0; loop_index < NUM_ITM(unit); loop_index++) {
            if (!(base_index_map & (1 << loop_index))) {
                continue;
            }
            acc_type = SOC_MEM_ACC_TYPE
                        (unit, SOC_MEM_UNIQUE_ACC_PIPE(unit, mem, 0));
            if ((acc_type != _TH3_MMU_SBUS_ACC_TYPE_UNIQUE)) {
                /* Single/Duplicate acc_type memories only have _ITMx*/
                mem_list[mem_index++] =
                    SOC_MEM_UNIQUE_ACC_PIPE(unit, mem, loop_index);
            } else {
                /*Unique acc_type memories with _EBx_ITMy*/
                for (eb = 0; eb < _TH3_PIPES_PER_DEV; eb++) {
                    if (!(eb_map & (1 << eb))) {
                        continue;
                    }
                    mem_list[mem_index++] =
                        SOC_MEM_UNIQUE_ACC_EB_ITM
                                        (unit, mem,eb, loop_index);
                }
            }
        }
        break;
    case 0: /* IPORT */
    case 1: /* EPORT */
    default:
        /*No MMU memories supported with base_tyep IPORT/EPORT*/
        return SOC_E_INTERNAL;
    }
    *mem_cnt = mem_index;

#undef _TH3_MMU_SBUS_ACC_TYPE_UNIQUE
    return SOC_E_NONE;
}

/*Function : _soc_tomahawk3_eb_mem_access
 *Purpose: read/write memories in EB
 */
STATIC int
_soc_tomahawk3_eb_mem_access(int unit, soc_mem_t mem, int eb, int base_index,
                             int copyno, int offset_in_section,
                             void *entry_data, int write)
{
    int mem_index, mem_cnt;
    int index;
    soc_mem_t mem_list[64] = {INVALIDm};

    if (eb >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk3_eb_mem_check(unit, mem, eb, base_index));
    }
    _soc_tomahawk3_retrieve_eb_mem_list (unit, mem, eb, base_index,
                                            mem_list, 64, &mem_cnt);
    index = offset_in_section;
    if(write) {
        /*Write to all memories in list*/
        for (mem_index = 0; mem_index < mem_cnt; mem_index ++) {
            SOC_IF_ERROR_RETURN(soc_mem_write
                (unit, mem_list[mem_index], copyno, index, entry_data));
        }
    } else {
        /*Read only from the 1st memory in list*/
        SOC_IF_ERROR_RETURN(soc_mem_read
                (unit, mem_list[0], copyno, index, entry_data));
    }
    return SOC_E_NONE;
}

/*Function : _soc_tomahawk3_eb_mem_write
 *Purpose: write memories in EB
 */
int
soc_tomahawk3_eb_mem_write(int unit, soc_mem_t mem, int eb, int base_index,
                          int copyno, int offset_in_section, void *entry_data)
{
    return _soc_tomahawk3_eb_mem_access(unit, mem, eb, base_index, copyno,
                                       offset_in_section, entry_data, TRUE);
}

/*Function : _soc_tomahawk3_eb_mem_read
 *Purpose: read memories in EB
 */
int
soc_tomahawk3_eb_mem_read(int unit, soc_mem_t mem, int eb, int base_index,
                         int copyno, int offset_in_section, void *entry_data)
{
    return _soc_tomahawk3_eb_mem_access(unit, mem, eb, base_index, copyno,
                                       offset_in_section, entry_data, FALSE);
}

/*
 *Function: soc_retrieve_mem_list_for_port
 *Purpose: Function to retrieve the memory expansion base on logical port # and
 *base memory name
 *         Users of this function do not need the knowledge of memory's
 *         base_type, access_type or block
 * Fnction only returns the memory list and the address need to be calculated by
 * the user
 */
int
soc_th3_retrieve_mmu_mem_list_for_port(int unit, int port, soc_mem_t mem, int blk_num,
                                soc_mem_t *mem_list, int max_mem_cnt, int *mem_cnt)
{
    int block_info_index, base_type;
    int pipe, base_index;
    soc_info_t *si = &SOC_INFO(unit);

    pipe = si->port_pipe[port];
    /*retrieve bits [25:23] of memory address for base_type*/
    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;

    switch(base_type) {
        case 2: /*IPIPE*/
        case 3: /*EPIPE*/
            base_index = pipe;
            break;
        case 4: /*CHIP*/
            base_index = 0;
            break;
        case 5: /*ITM*/
            /*ITM 0 if port is in pipes 0,1,6,7 else ITM1*/
            base_index = blk_num;
            break;
        default:
            base_index = -1;
            break;
    }

    block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);

    if (SOC_BLOCK_TYPE(unit, block_info_index) == SOC_BLK_MMU_ITM) {
        _soc_tomahawk3_retrieve_itm_mem_list (unit, mem, blk_num, base_index,
                                            mem_list, max_mem_cnt, mem_cnt);
    } else if (SOC_BLOCK_TYPE(unit, block_info_index) == SOC_BLK_MMU_EB) {
        _soc_tomahawk3_retrieve_eb_mem_list (unit, mem, blk_num, base_index,
                                            mem_list, max_mem_cnt, mem_cnt);
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "%s is not ITM or EB memory\n"), SOC_MEM_NAME(unit, mem)));
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

/*
 * CPU port (mmu port 32): 48 MC queues
 * Loopback port (mmu port 33,97,161,225): 10 MC queues
 * Mgmt port (mmu port 96,160): 10 UC, 10 MC queues
 */
int
soc_tomahawk3_num_cosq_init(int unit, int port)
{
    soc_info_t *si;
    int  local_mmu_port, num_uc_q, pipe, pipe_index;
    int pipe_queue_base[_TH3_PIPES_PER_DEV];
    int mc_q_mode = 2; /* mc_q_mode defines the number of UC/MC queues
                      * for all ports except CPU
                      */
    /*Number of queues per pipe is
     *pipe 0: 18 FP, 1 LB, 1 CPU: 18*12 + 12 + 48 = 296
     *pipe 1,5: 18FP, 1 LB, 1 MGMT: 18*12 + 12 + 12 = 240
     *pipe 2,3,4: 18 FP, 1 LB: 18*12 + 12 = 228
     *pipe 6,7: 18 FP, 1 LB, 1 Extra port: 18*12 + 12 + 12 = 240
     */
    int num_queues_pipe[] = {276, 240, 228, 228, 228, 240, 240, 240};
    si = &SOC_INFO(unit);

    mc_q_mode = soc_property_get(unit, spn_MMU_PORT_NUM_MC_QUEUE, _TH3_MC_Q_MODE_DEFAULT);

    if (mc_q_mode == 1) {
        num_uc_q = 10;
    } else if (mc_q_mode == 2) {
        num_uc_q = 8;
    } else if (mc_q_mode == 3) {
        num_uc_q = 6;
    } else {
        num_uc_q = 12;
    }
    pipe_queue_base[0] = 0;
    for (pipe_index = 1; pipe_index < _TH3_PIPES_PER_DEV; pipe_index++) {
        pipe_queue_base[pipe_index] = num_queues_pipe[pipe_index - 1] +
                                        pipe_queue_base[pipe_index - 1];
    }
    local_mmu_port = SOC_TH3_MMU_LOCAL_PORT(unit, port);
    pipe = si->port_pipe[port];
    if (IS_CPU_PORT(unit, port)) {
        si->port_num_cosq[port] = SOC_TH3_NUM_CPU_QUEUES;
        si->port_cosq_base[port] =
            (local_mmu_port * SOC_TH3_NUM_GP_QUEUES) + pipe_queue_base[pipe];
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "CPU info mmu_port:%d, port_cosq_base:%d\n"),
                  local_mmu_port, si->port_cosq_base[port]));
    } else {
        si->port_num_cosq[port] = SOC_TH3_NUM_GP_QUEUES - num_uc_q;
        si->port_cosq_base[port] =
            (local_mmu_port * SOC_TH3_NUM_GP_QUEUES) + num_uc_q
                                                + pipe_queue_base[pipe];
        si->port_num_uc_cosq[port] = num_uc_q;
        si->port_uc_cosq_base[port] =
                     (local_mmu_port * SOC_TH3_NUM_GP_QUEUES) +
                                        pipe_queue_base[pipe];
        si->port_num_ext_cosq[port] = 0;
    }
    return SOC_E_NONE;
}

static int
soc_th3_check_mmu_mem_init_done(int unit) {
    struct {
        soc_reg_t ref_reg;
        soc_field_t done_field;
    } itm_rf_list[] = {
        {MMU_TOQ_STATUSr, INIT_DONEf},
        {MMU_WRED_MEM_INIT_STATUSr, DONEf},
        {INVALIDr, INVALIDf}
    };
    struct {
        soc_reg_t ref_reg;
        soc_field_t done_field;
    } pipe_rf_list[] = {
        {MMU_EBCFP_INIT_DONEr, DONEf},
        {EBTOQ_STATUSr, INIT_DONEf},
        {INVALIDr, INVALIDf}
    };
    uint32 rval;
    int idx, done, itm, pipe;
    soc_reg_t reg;
    soc_field_t field;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 itm_map = si->itm_map;
    uint32 pipe_map;

    for (idx = 0; itm_rf_list[idx].ref_reg != INVALIDr; idx++) {
        reg = itm_rf_list[idx].ref_reg;
        field = itm_rf_list[idx].done_field;
        for (itm = 0; itm < NUM_ITM(unit); itm++) {
            if (itm_map & (1 << itm)) {
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, 0, itm, &rval));
                done = soc_reg_field_get(unit, reg, rval, field);
                if (!done) {
                    LOG_INFO(BSL_LS_SOC_COSQ,
                        (BSL_META_U(unit,
                            " MMU Memory Init done not set after toggling"
                            "INIT_MEM \n")));
                    return SOC_E_INTERNAL;
                }
            }
        }
    }

    soc_tomahawk3_pipe_map_get(unit, &pipe_map);
    for (idx = 0; pipe_rf_list[idx].ref_reg != INVALIDr; idx++) {
        reg = pipe_rf_list[idx].ref_reg;
        field = pipe_rf_list[idx].done_field;
        for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
            if (pipe_map & (1 << pipe)) {
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, 0, pipe, &rval));
                done = soc_reg_field_get(unit, reg, rval, field);
                if (!done) {
                    LOG_ERROR(BSL_LS_SOC_COSQ,
                        (BSL_META_U(unit,
                            " MMU Memory Init done not set after toggling"
                            "INIT_MEM \n")));
                    return SOC_E_INTERNAL;
                }
            }
        }
    }
    return SOC_E_NONE;
}
/*Function to initialize IDB and MMU memories*/
int
soc_tomahawk3_init_tm_memory(int unit)
{
    uint32 rval;
    soc_reg_t reg;
    int parity_en;
    uint32 pipe_map;
    int in_progress =1, init_usec, pipe;
    soc_timeout_t to;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;

    soc_tomahawk3_pipe_map_get(unit, &pipe_map);

    /* Initialize IDB memory */
    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV && in_progress; pipe++) {
        /* skip pipes that don't exist */
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        rval = 0;
        reg = SOC_REG_UNIQUE_ACC(unit,  IDB_HW_RESET_CONTROLr)[pipe];
        soc_reg_field_set(unit, reg, &rval, RESET_ALLf, 1);
        soc_reg_field_set(unit, reg, &rval, VALIDf, 1);
        soc_reg_field_set(unit, reg, &rval, COUNTf, 0x80);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        init_usec = 10000000;
    } else {
        init_usec = 50000;
    }
    if (!SAL_BOOT_SIMULATION) {
        soc_timeout_init(&to, init_usec, 0);

        /* Wait for IPIPE memory initialization done */
        in_progress = pipe_map;
        do {
            for (pipe = 0; pipe < _TH3_PIPES_PER_DEV && in_progress; pipe++) {
                rval = 0;

                /* skip pipes that don't exist */
                if (!(pipe_map & (1 << pipe))) {
                    continue;
                }

                reg = SOC_REG_UNIQUE_ACC(unit,  IDB_HW_RESET_CONTROLr)[pipe];
                if (in_progress & (1 << pipe)) { /* not done yet */
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                        if (soc_reg_field_get(unit, reg, rval, DONEf)) {
                            in_progress ^= 1 << pipe;
                        }
                }
            }
            if (soc_timeout_check(&to)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "unit %d : IDB_HW_RESET timeout\n"), unit));
                break;
            }
        } while (in_progress != 0);
    }
    parity_en = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
    /*Initialize MMU memory*/
    rval = 0;
    reg = MMU_GLBCFG_MISCCONFIGr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, ECCP_ENf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    sal_usleep(20);
    soc_reg_field_set(unit, reg, &rval, INIT_MEMf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    soc_reg_field_set(unit, reg, &rval, INIT_MEMf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    sal_usleep(30);

    if (!SAL_BOOT_SIMULATION) {
        SOC_IF_ERROR_RETURN(soc_th3_check_mmu_mem_init_done(unit));
    }
    soc_reg_field_set(unit, reg, &rval, ECCP_ENf, parity_en);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));


    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV ; pipe++) {
        if (!SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
            mem = SOC_MEM_UNIQUE_ACC(unit, MMU_EBCFP_MXM_TAG_MEMm)[pipe];
            soc_mem_field32_set(unit, mem, &entry, DATAf, 0x1);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, &entry));
        }
    }

    return SOC_E_NONE;
}

/*Start: Port mapping functions*/
STATIC int
_soc_th3_mmu_port_to_phy_port_map (int unit, int dev_port, int phy_port,
        int mmu_port)
{
    /*
     *MMU_INTFI_MMU_PORT_TO_PHY_PORT_MAPPING
     *MMU_INTFO_MMU_PORT_TO_PHY_PORT_MAPPING
     *MMU_EBPTS_MMU_PORT_TO_PHY_PORT_MAPPING-
     */
    soc_reg_t reg_list[] = {MMU_INTFI_MMU_PORT_TO_PHY_PORT_MAPPINGr,
                            MMU_INTFO_MMU_PORT_TO_PHY_PORT_MAPPINGr,
                            MMU_EBPTS_MMU_PORT_TO_PHY_PORT_MAPPINGr,
                            };
    soc_reg_t reg;
    uint32 rval;
    int reg_index, field_val, num_regs;
    int pipe;
    soc_info_t *si = &SOC_INFO(unit);
    num_regs = sizeof(reg_list) / sizeof(soc_reg_t);

    pipe = si->port_pipe[dev_port];
    for (reg_index = 0; reg_index < num_regs; reg_index ++) {
        rval = 0;
        reg = reg_list[reg_index];
        if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, reg).block,
                    SOC_BLK_MMU_EB)) {
            /* Pipe 0 is special because of CPU port being port 0. Range is 0 - 32 */
            if (pipe  ==  0) {
                field_val = phy_port % (SOC_TH3_MAX_PHY_PORTS_PER_PIPE + 1);
            } else {
                field_val = (phy_port - 1) % SOC_TH3_MAX_PHY_PORTS_PER_PIPE;
            }
            if (IS_LB_PORT(unit, dev_port)) {
                /*Local phy port 33 to be used for all LB ports*/
                field_val = SOC_TH3_LB_PORT_LOCAL_PHY_NUM;
            }
            if(IS_MANAGEMENT_PORT(unit, dev_port)) {
                /*Local phy port 32 to be used for all LB ports*/
                field_val = SOC_TH3_MGMT_PORT_LOCAL_PHY_NUM;
            }
        } else {
            field_val = phy_port;
        }
        soc_reg_field_set(unit, reg, &rval, PHY_PORT_NUMf,
                          field_val);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, dev_port, 0, rval));
    }
    return SOC_E_NONE;
}

STATIC int
_soc_th3_mmu_port_to_dev_port_map (int unit, int dev_port, int mmu_port)
{
    /*
     *MMU_CRB_DEVICE_PORT_TO_MMU_PORT_MAPPING - Global dev to Global mmu
     *MMU_EBCFP_MMU_PORT_TO_DEVICE_PORT_MAPPING - Global MMU to global dev
     *MMU_RQE_PORT_TO_DEVICE_PORT_MAPPING-memory, global dev to global mmu
     *MMU_THDO_DEVICE_PORT_MAP -memory, chip port num to dev port
     */
    soc_reg_t reg_list[] = {MMU_CRB_DEVICE_PORT_TO_MMU_PORT_MAPPINGr,
                            MMU_EBCFP_MMU_PORT_TO_DEVICE_PORT_MAPPINGr};
    soc_mem_t mem_list[] = {MMU_RQE_DEVICE_TO_MMU_PORT_MAPPINGm,
                            MMU_THDO_DEVICE_PORT_MAPm};
    soc_field_t reg_field = INVALIDf, mem_field = INVALIDf;
    soc_reg_t reg;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 rval;
    int reg_index, field_val = -1, index = -1, mem_index,  num_regs, num_mems;
    num_regs = sizeof(reg_list) / sizeof(soc_reg_t);
    num_mems = sizeof(mem_list) / sizeof(soc_mem_t);

    sal_memset(entry, 0, sizeof(entry));
    for (reg_index = 0; reg_index < num_regs; reg_index++) {
        rval = 0;
        reg = reg_list[reg_index];
        switch(reg) {
            case MMU_CRB_DEVICE_PORT_TO_MMU_PORT_MAPPINGr:
                reg_field = MMU_PORTf;
                field_val = mmu_port; /*Global mmu port*/
                index = dev_port;
                break;
            case MMU_EBCFP_MMU_PORT_TO_DEVICE_PORT_MAPPINGr:
                reg_field = DEVICE_PORTf;
                field_val = dev_port;
                /*dev port to mmu port conversion done in soc_reg_write*/
                index = dev_port;
                break;
            default:
                break;
        }
        soc_reg_field_set(unit, reg, &rval, reg_field,
                          field_val);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, index, 0, rval));
    }
    for (mem_index = 0; mem_index < num_mems; mem_index++) {
        mem = mem_list[mem_index];
        switch(mem) {
            case MMU_RQE_DEVICE_TO_MMU_PORT_MAPPINGm:
                mem_field = MMU_PORTf;
                field_val = mmu_port; /*Global mmu port*/
                index = dev_port;
                break;
            case MMU_THDO_DEVICE_PORT_MAPm:
                mem_field = DEVICE_PORTf;
                field_val = dev_port;
                index = SOC_TH3_MMU_CHIP_PORT(unit, dev_port);
                break;
            default:
                break;
        }
        soc_mem_field32_set(unit, mem, &entry, mem_field , field_val);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry));
    }
    return SOC_E_NONE;
}

int
soc_tomahawk3_mmu_port_mapping_init (int unit)
{
    soc_info_t *si;
    int port, phy_port, mmu_port;
    si = &SOC_INFO(unit);


    PBMP_ALL_ITER(unit, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        LOG_INFO(BSL_LS_SOC_COSQ,
            (BSL_META_U(unit,
                " Dev Port:%d MMU_port:%d Phy Port:%d\n"),
			                port, mmu_port, phy_port));
        _soc_th3_mmu_port_to_phy_port_map(unit, port, phy_port, mmu_port);
        _soc_th3_mmu_port_to_dev_port_map(unit, port, mmu_port);
    }
    return SOC_E_NONE;
}
/*End: Port mapping init functions*/

int
soc_tomahawk3_en_mmu_refresh (int unit)
{
    int reg_index;
    soc_field_t refresh_field = REFRESH_DISABLEf;
    soc_reg_t reg_list[] = {MMU_MTRO_CONFIGr, MMU_WRED_REFRESH_CONTROLr};
    uint32 rval;
    int num_regs = sizeof(reg_list) / sizeof(soc_reg_t);

    for (reg_index = 0; reg_index < num_regs; reg_index++) {
        rval = 0;
        soc_reg_field_set(unit, reg_list[reg_index], &rval, refresh_field, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg_list[reg_index],
                    REG_PORT_ANY, 0, rval));
    }
    return SOC_E_NONE;
}

STATIC int
soc_tomahawk3_ifp_cos_map_init_rqe_flush_war (int unit)
{
    
    int mem_index;
    int pri;
    soc_mem_t mem = IFP_COS_MAPm;
    soc_field_t rqe_cos = IFP_RQE_Q_NUMf;
    uint32 entry[SOC_MAX_MEM_WORDS];

    for (pri = 0; pri < _TH3_MMU_NUM_RQE_QUEUES; pri++) {
        mem_index = (_TH3_IFP_COS_MAP_RSVD_PROF * _TH3_MMU_NUM_INT_PRI) + pri;
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, mem_index, &entry));
        soc_mem_field32_set(unit, mem, &entry, rqe_cos, pri);
        SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, mem, MEM_BLOCK_ALL, mem_index, &entry));
    }
    return SOC_E_NONE;
}

/*
 * Function: soc_th3_mmu_rqe_queue_number_init
 * Purpose: Initialize to use separate RQE queue numbers for mirror and CPU
 * packets
 * Param: unit: device number
 */
int
soc_th3_mmu_rqe_queue_number_init(int unit)
{
    uint32 rval = 0;
    soc_reg_t reg;
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /*RQE queue number for CPU hi packets*/
    reg = CPU_HI_RQE_Q_NUMr;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    if (rev_id == BCM56980_A0_REV_ID) {
        soc_reg_field_set(unit, reg, &rval, RQE_Q_NUMf, _TH3_A0_CPU_HI_RQE_Q_NUM);
    } else {
        soc_reg_field_set(unit, reg, &rval, RQE_Q_NUMf, _TH3_CPU_HI_RQE_Q_NUM);
    }
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    /*RQE queue number for CPU lo packets*/
    reg = CPU_LO_RQE_Q_NUMr;
    rval = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    if (rev_id == BCM56980_A0_REV_ID) {
        soc_reg_field_set(unit, reg, &rval, RQE_Q_NUMf, _TH3_A0_CPU_LO_RQE_Q_NUM);
    } else {
        soc_reg_field_set(unit, reg, &rval, RQE_Q_NUMf, _TH3_CPU_LO_RQE_Q_NUM);
    }
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    /*RQE queue number for Mirror packets*/
    reg = MIRROR_RQE_Q_NUMr;
    rval = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    if (rev_id == BCM56980_A0_REV_ID) {
        soc_reg_field_set(unit, reg, &rval, RQE_Q_NUMf, _TH3_A0_MIRR_RQE_Q_NUM);
    } else {
        soc_reg_field_set(unit, reg, &rval, RQE_Q_NUMf, _TH3_MIRR_RQE_Q_NUM);
    }
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    reg = MMU_CRB_RQE_QUEUE_CONFIGr;
    rval = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    if (rev_id == BCM56980_A0_REV_ID) {
        soc_reg_field_set(unit, reg, &rval, CPUHI_RQE_QUEUE_NUMf, _TH3_A0_CPU_HI_RQE_Q_NUM);
        soc_reg_field_set(unit, reg, &rval, CPULO_RQE_QUEUE_NUMf, _TH3_A0_CPU_LO_RQE_Q_NUM);
        soc_reg_field_set(unit, reg, &rval, MIRROR_RQE_QUEUE_NUMf, _TH3_A0_MIRR_RQE_Q_NUM);
    } else {
        soc_reg_field_set(unit, reg, &rval, CPUHI_RQE_QUEUE_NUMf, _TH3_CPU_HI_RQE_Q_NUM);
        soc_reg_field_set(unit, reg, &rval, CPULO_RQE_QUEUE_NUMf, _TH3_CPU_LO_RQE_Q_NUM);
        soc_reg_field_set(unit, reg, &rval, MIRROR_RQE_QUEUE_NUMf, _TH3_MIRR_RQE_Q_NUM);
    }
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    /* Initialize IFP_COS_MAP reserved profile fro RQE flush WAR (SDK-142748)*/
    SOC_IF_ERROR_RETURN(
            soc_tomahawk3_ifp_cos_map_init_rqe_flush_war(unit));

    return SOC_E_NONE;
}

/*
 * Function: soc_tomahawk3_mmu_port_rx_enables
 * Purpose: Enable MMU to receive traffic on different ports as part of init
 * Param: unit: device number
 */
int
soc_tomahawk3_mmu_port_rx_enables(int unit) {
    int pipe, dev_port, itm;
    uint32 rval;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 itm_map = si->itm_map;
    int rv;

    PBMP_ALL_ITER(unit, dev_port) {
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, MMU_CRB_SRC_PORT_CFGr, dev_port, 0,
                                                                &rval));
        soc_reg_field_set(unit, MMU_CRB_SRC_PORT_CFGr, &rval,
                                             RX_PORT_ENf, 1);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, MMU_CRB_SRC_PORT_CFGr, dev_port, 0,
                                                                rval));
    }
    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        if (!SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
            LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "Programming for Pipe:%d\n"), pipe));
            rval = 0;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, MMU_THDO_CONFIG_PORTr, REG_PORT_ANY, pipe,
                               &rval));
            /*Enable for all MMU ports in a pipe*/
            soc_reg_field_set(unit, MMU_THDO_CONFIG_PORTr, &rval,
                                           OUTPUT_PORT_RX_ENABLEf, 0xfffff);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, MMU_THDO_CONFIG_PORTr, REG_PORT_ANY, pipe,
                               rval));
        }
    }
    for(itm = 0; itm < NUM_ITM(unit); itm++) {
        if (itm_map & (1 << itm)) {
            rval = 0;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, MMU_THDO_ENGINE_ENABLES_CFGr, itm,
                               REG_PORT_ANY, &rval));
            /*Enable for all MMU ports in a pipe*/
            soc_reg_field_set(unit, MMU_THDO_ENGINE_ENABLES_CFGr, &rval,
                                           SRC_PORT_DROP_COUNT_ENABLEf, 1);
            soc_reg_field_set(unit, MMU_THDO_ENGINE_ENABLES_CFGr, &rval,
                                           DLB_INTERFACE_ENABLEf, 1);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, MMU_THDO_ENGINE_ENABLES_CFGr, itm,
                               REG_PORT_ANY, rval));

        } else {
            /* Reset WRED engine for the invalid ITM if accessible */
            rval = 0;
            rv = soc_reg32_get(unit, MMU_THDO_ENGINE_ENABLES_CFGr, itm,
                               REG_PORT_ANY, &rval);
            if (rv == SOC_E_NONE) {
                /*Enable for all MMU ports in a pipe*/
                soc_reg_field_set(unit, MMU_THDO_ENGINE_ENABLES_CFGr, &rval,
                                               RESET_WRED_ENGINEf, 1);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, MMU_THDO_ENGINE_ENABLES_CFGr, itm,
                                   REG_PORT_ANY, rval));
            }
        }
    }
    /*Disable THDO counter overflow interrupt (SDK-133360)*/
    rval = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, MMU_THDO_CPU_INT_SETr,
                                        REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, MMU_THDO_CPU_INT_SETr, &rval, COUNTER_OVERFLOWf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, MMU_THDO_CPU_INT_SETr,
                                   REG_PORT_ANY, 0, rval));
    return SOC_E_NONE;
}

/*
 * Function: soc_tomahawk3_cell_pointer_init
 * Purpose: Initialize cell pointer memory in MMU during init
 * Param: unit: device number
 */
int
soc_tomahawk3_cell_pointer_init(int unit)
{
    uint64 rval, cfap_bank;
    uint32 rval32;
    int itm;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 itm_map = si->itm_map;
    COMPILER_64_ZERO(rval);
    /*Enable all 34 CFAP banks in each ITM*/
    COMPILER_64_SET(cfap_bank, 0x3, 0xffffffff);
    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        if (itm_map & (1 << itm)) {
            LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "Programming for ITM:%d\n"), itm));
            SOC_IF_ERROR_RETURN
                (soc_tomahawk3_itm_reg_get(unit, MMU_CFAP_INIT_64r,
                                           itm, itm, 0, &rval));
            COMPILER_64_OR(rval, cfap_bank);
            SOC_IF_ERROR_RETURN
                (soc_tomahawk3_itm_reg_set(unit, MMU_CFAP_INIT_64r,
                                           itm, itm, 0, rval));
        }
    }

    /* SDK-142571 MMU_CFAP_ARBITER_MASK should be 2 */
    rval32 = 0;
    SOC_IF_ERROR_RETURN
        (soc_tomahawk3_itm_reg32_get(unit, MMU_CFAP_ARBITER_MASKr,
                                   -1, -1, 0, &rval32));
    soc_reg_field_set(unit, MMU_CFAP_ARBITER_MASKr, &rval32,
            MASKf, 2);
    SOC_IF_ERROR_RETURN
        (soc_tomahawk3_itm_reg32_set(unit, MMU_CFAP_ARBITER_MASKr,
                                   -1, -1, 0, rval32));

    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_cosq_profile_info_init
 *Purpose: init data struct for scheduler hierachy for a particular profile
 */
int
soc_tomahawk3_cosq_profile_info_init(int unit, int profile_idx)
{
    int queue_num;

    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        th3_sched_profile_info[unit][profile_idx][queue_num].cos = -1;
        th3_sched_profile_info[unit][profile_idx][queue_num].mmuq[0] = -1;
        th3_sched_profile_info[unit][profile_idx][queue_num].mmuq[1] = -1;
        th3_sched_profile_info[unit][profile_idx][queue_num].strict_priority = 0;
        th3_sched_profile_info[unit][profile_idx][queue_num].fc_is_uc_only = 0;
    }
    return SOC_E_NONE;

}

/*
 *Function: soc_cosq_profile_struct_init
 *Purpose: init data struct for scheduler hierachy
 */
int
soc_cosq_profile_struct_init(int unit, int profile)
{
    int queue_num;

    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        L1_TO_L0_MAPPING[unit][profile][queue_num] = -1;
        L0_TO_L1_MAPPING_NUM[unit][profile][queue_num] = 0;
        L0_TO_COSQ_MAPPING[unit][profile][queue_num] = -1;
    }
    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_cosq_port_info_init
 *Purpose: init data struct for scheduler hierachy for a particular port
 */
int
soc_tomahawk3_cosq_port_info_init(int unit, int port_num)
{
    int queue_num;

    if (th3_cosq_mmu_info[unit] == NULL) {
        return SOC_E_MEMORY;
    }

    /*general port initialization*/
    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        /*level 0*/
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L0[queue_num].level = 0;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L0[queue_num].in_use = 0;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L0[queue_num].sched_policy = SOC_TH3_SCHED_MODE_WRR;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L0[queue_num].weight = 0;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L0[queue_num].fc_is_uc_only = 0;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L0[queue_num].hw_index = queue_num;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L0[queue_num].cos = -1;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L0[queue_num].parent = NULL;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L0[queue_num].children[0] = NULL;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L0[queue_num].children[1] = NULL;
    }
    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        /*level 1*/
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L1[queue_num].level = 1;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L1[queue_num].in_use = 0;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L1[queue_num].sched_policy = SOC_TH3_SCHED_MODE_WRR;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L1[queue_num].weight = 0;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L1[queue_num].fc_is_uc_only = 0;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L1[queue_num].hw_index = queue_num;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L1[queue_num].cos = -1;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L1[queue_num].parent = NULL;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L1[queue_num].children[0] = NULL;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            L1[queue_num].children[1] = NULL;
    }
    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        /*level 2*/
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            mmuq[queue_num].level = 2;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            mmuq[queue_num].in_use = 1;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            mmuq[queue_num].sched_policy = SOC_TH3_SCHED_MODE_WRR;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            mmuq[queue_num].weight = 0;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            mmuq[queue_num].fc_is_uc_only = 0;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            mmuq[queue_num].hw_index = queue_num;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            mmuq[queue_num].cos = -1;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            mmuq[queue_num].parent = NULL;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            mmuq[queue_num].children[0] = NULL;
        th3_cosq_mmu_info[unit]->th3_port_info[port_num].
            mmuq[queue_num].children[1] = NULL;
    }
    return SOC_E_NONE;
}

void soc_gport_tree_print(int unit, int port)
{
    int cosq_idx;

    /* Print */
    for (cosq_idx = 0; cosq_idx <  SOC_TH3_COS_MAX; cosq_idx++) {
        LOG_INFO(BSL_LS_SOC_COSQ,
            (BSL_META_U(unit, "\n L0 [%0d, %0d] in_use=%0d, level=%0d, hw_index=%0d, cos=%0d "),
             port, cosq_idx, th3_cosq_mmu_info[unit]->th3_port_info[port].L0[cosq_idx].in_use,
             th3_cosq_mmu_info[unit]->th3_port_info[port].L0[cosq_idx].level,
             th3_cosq_mmu_info[unit]->th3_port_info[port].L0[cosq_idx].hw_index,
             th3_cosq_mmu_info[unit]->th3_port_info[port].L0[cosq_idx].cos));
        if (th3_cosq_mmu_info[unit]->th3_port_info[port].L0[cosq_idx].parent != NULL) {
            LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "parent.hw_index=%0d "),
                 th3_cosq_mmu_info[unit]->th3_port_info[port].L0[cosq_idx].parent->hw_index));
        }
        if (th3_cosq_mmu_info[unit]->th3_port_info[port].L0[cosq_idx].children[0] != NULL) {
            LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "children[0].hw_index=%0d "),
                 th3_cosq_mmu_info[unit]->th3_port_info[port].L0[cosq_idx].children[0]->hw_index));
            if (th3_cosq_mmu_info[unit]->th3_port_info[port].L0[cosq_idx].children[1] != NULL) {
                LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "children[1].hw_index=%0d "),
                 th3_cosq_mmu_info[unit]->th3_port_info[port].L0[cosq_idx].children[1]->hw_index));
            }
        }

        LOG_INFO(BSL_LS_SOC_COSQ,
            (BSL_META_U(unit, "\n L1 [%0d, %0d] in_use=%0d, level=%0d, hw_index=%0d, cos=%0d "),
             port, cosq_idx, th3_cosq_mmu_info[unit]->th3_port_info[port].L1[cosq_idx].in_use,
             th3_cosq_mmu_info[unit]->th3_port_info[port].L1[cosq_idx].level,
             th3_cosq_mmu_info[unit]->th3_port_info[port].L1[cosq_idx].hw_index,
             th3_cosq_mmu_info[unit]->th3_port_info[port].L1[cosq_idx].cos));
        if (th3_cosq_mmu_info[unit]->th3_port_info[port].L1[cosq_idx].parent != NULL) {
            LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "parent.hw_index=%0d "),
                 th3_cosq_mmu_info[unit]->th3_port_info[port].L1[cosq_idx].parent->hw_index));
        }
        if (th3_cosq_mmu_info[unit]->th3_port_info[port].L1[cosq_idx].children[0] != NULL) {
            LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "children[0].hw_index=%0d "),
                 th3_cosq_mmu_info[unit]->th3_port_info[port].L1[cosq_idx].children[0]->hw_index));
            if (th3_cosq_mmu_info[unit]->th3_port_info[port].L1[cosq_idx].children[1] != NULL) {
                LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "children[1].hw_index=%0d "),
                 th3_cosq_mmu_info[unit]->th3_port_info[port].L1[cosq_idx].children[1]->hw_index));
            }
        }

        LOG_INFO(BSL_LS_SOC_COSQ,
            (BSL_META_U(unit, "\n mmuq [%0d, %0d] in_use=%0d, level=%0d, hw_index=%0d, cos=%0d "),
             port, cosq_idx, th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[cosq_idx].in_use,
             th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[cosq_idx].level,
             th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[cosq_idx].hw_index,
             th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[cosq_idx].cos));
        if (th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[cosq_idx].parent != NULL) {
            LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "parent.hw_index=%0d "),
                     th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[cosq_idx].parent->hw_index));
        }
        if (th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[cosq_idx].children[0] != NULL) {
            LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "children[0].hw_index=%0d "),
                 th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[cosq_idx].children[0]->hw_index));
            if (th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[cosq_idx].children[1] != NULL) {
                LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "children[1].hw_index=%0d "),
                 th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[cosq_idx].children[1]->hw_index));
            }
        }
    }
}

/*
 *Function: soc_cosq_struct_init
 *Purpose: init data struct for scheduler hierachy
 */
int
soc_cosq_struct_init(int unit)
{
    int port_num, queue_num, profile, i;
    /*L1 to L0 is port based in reg file. Keep the profile based mapping in software*/
    for (profile = 0; profile < SOC_TH3_MAX_NUM_SCHED_PROFILE; profile++) {
        for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
            L1_TO_L0_MAPPING[unit][profile][queue_num] = -1;
            L0_TO_L1_MAPPING_NUM[unit][profile][queue_num] = 0;
            L0_TO_COSQ_MAPPING[unit][profile][queue_num] = -1;
        }
    }
    if (th3_cosq_mmu_info[unit] == NULL) {
         /*th3_cosq_mmu_info is not freed */
        th3_cosq_mmu_info[unit] =
            sal_alloc(sizeof(soc_th3_mmu_info_t), "soc_th3_mmu_info_t");
        if (th3_cosq_mmu_info[unit] == NULL) {
            return SOC_E_MEMORY;
        }
    }
    /*general port initialization*/
    for (port_num = 0; port_num < SOC_TH3_MAX_NUM_PORTS; port_num++) {
        for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
            /*level 0*/
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L0[queue_num].level = 0;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L0[queue_num].in_use = 0;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L0[queue_num].sched_policy = SOC_TH3_SCHED_MODE_WRR;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L0[queue_num].weight = 0;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L0[queue_num].fc_is_uc_only = 0;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L0[queue_num].hw_index = queue_num;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L0[queue_num].cos = -1;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L0[queue_num].parent = NULL;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L0[queue_num].children[0] = NULL;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L0[queue_num].children[1] = NULL;
        }
        for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
            /*level 1*/
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L1[queue_num].level = 1;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L1[queue_num].in_use = 0;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L1[queue_num].sched_policy = SOC_TH3_SCHED_MODE_WRR;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L1[queue_num].weight = 0;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L1[queue_num].fc_is_uc_only = 0;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L1[queue_num].hw_index = queue_num;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L1[queue_num].cos = -1;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L1[queue_num].parent = NULL;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L1[queue_num].children[0] = NULL;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                L1[queue_num].children[1] = NULL;
        }
        for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
            /*level 2*/
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                mmuq[queue_num].level = 2;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                mmuq[queue_num].in_use = 1;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                mmuq[queue_num].sched_policy = SOC_TH3_SCHED_MODE_WRR;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                mmuq[queue_num].weight = 0;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                mmuq[queue_num].fc_is_uc_only = 0;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                mmuq[queue_num].hw_index = queue_num;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                mmuq[queue_num].cos = -1;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                mmuq[queue_num].parent = NULL;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                mmuq[queue_num].children[0] = NULL;
            th3_cosq_mmu_info[unit]->th3_port_info[port_num].
                mmuq[queue_num].children[1] = NULL;
        }
    }
    /*cpu port initialization*/
    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        /*levl 0*/
        th3_cosq_mmu_info[unit]->th3_cpu_port_info.L0[queue_num].level = 0;
        th3_cosq_mmu_info[unit]->th3_cpu_port_info.L0[queue_num].in_use = 0;
        th3_cosq_mmu_info[unit]->th3_cpu_port_info.
            L0[queue_num].sched_policy = SOC_TH3_SCHED_MODE_WRR;
        th3_cosq_mmu_info[unit]->th3_cpu_port_info.L0[queue_num].weight = 0;
        th3_cosq_mmu_info[unit]->th3_cpu_port_info.
            L0[queue_num].hw_index = queue_num;
        th3_cosq_mmu_info[unit]->th3_cpu_port_info.
            L0[queue_num].cos = queue_num;
        th3_cosq_mmu_info[unit]->th3_cpu_port_info.L0[queue_num].parent = NULL;
        for (i = 0; i < SOC_TH3_NUM_CPU_QUEUES; i++) {
            th3_cosq_mmu_info[unit]->th3_cpu_port_info.
                L0[queue_num].children[i] = NULL;
        }
    }
    for (queue_num = 0; queue_num < SOC_TH3_NUM_CPU_QUEUES; queue_num++) {
        /*level 1*/
        th3_cosq_mmu_info[unit]->th3_cpu_port_info.L1[queue_num].level = 1;
        th3_cosq_mmu_info[unit]->th3_cpu_port_info.L1[queue_num].in_use = 0;
        th3_cosq_mmu_info[unit]->th3_cpu_port_info.L1[queue_num].
            sched_policy = SOC_TH3_SCHED_MODE_WRR;
        th3_cosq_mmu_info[unit]->th3_cpu_port_info.L1[queue_num].weight = 0;
        th3_cosq_mmu_info[unit]->th3_cpu_port_info.L1[queue_num].
            hw_index = queue_num;
        th3_cosq_mmu_info[unit]->th3_cpu_port_info.L1[queue_num].cos = -1;
        th3_cosq_mmu_info[unit]->th3_cpu_port_info.L1[queue_num].parent = NULL;
        for (i = 0; i < SOC_TH3_NUM_CPU_QUEUES; i++) {
            th3_cosq_mmu_info[unit]->th3_cpu_port_info.
                L1[queue_num].children[i] = NULL;
        }
    }
    return SOC_E_NONE;
}
/*
 *Function: soc_tomahawk3_mcq_mode_set
 *Purpose: Program MC_Q_MODE value to configure number of UC and MC queues per
 *          port
 *Params:
 *       unit: Device unit number
 *       mode: mc_q_mode value (rage: 0-3)
 */

int
soc_tomahawk3_mcq_mode_set(int unit, int mode)
{
    soc_reg_t reg;
    uint32 rval;
    int i;
    soc_reg_t reg_array[] = {MMU_INTFO_CONFIG0r,
                             MMU_TOQ_CONFIGr,
                             MMU_THDO_CONFIGr,
                             MMU_CRB_CONFIGr,
                             MMU_WRED_CONFIGr};
    if (mode < 0 || mode > 3) {
        return SOC_E_PARAM;
    }
    for (i = 0; i < sizeof(reg_array) / sizeof(reg); i++) {
        reg = reg_array[i];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, 0, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, MC_Q_MODEf, mode);
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_itm_reg32_set(unit, reg, -1, -1, -1, rval));
    }
    return SOC_E_NONE;
}

/* Function:
 *      soc_tomahawk3_port_schedule_state_basic_setup
 * Purpose:
 *      Setup Global params in port_schedule_state structure
 *      to DV FlexPort API
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (OUT) Port resource state.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
soc_tomahawk3_port_schedule_state_basic_setup(int unit,
                soc_port_schedule_state_t *port_schedule_state)
{
#define _TH3_CPU_LB_PORT_SPEED 10000
#if 0
    soc_control_t *soc = SOC_CONTROL(unit);
#endif
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_map_type_t *in_portmap;
    soc_asf_prop_t *cutthru_prop;
    int port, latency;
    int dpr_freq;
    /* Core clock frequency */
    port_schedule_state->frequency = si->frequency;

    /*MMU lossless mode. default to 1(lossy)*/
    port_schedule_state->lossless = soc_property_get(unit, spn_MMU_LOSSLESS,
            SOC_TH3_MMU_LOSSLESS_DEFAULT_DISABLE);

    /* Construct in_port_map */
    in_portmap = &port_schedule_state->in_port_map;

    /*DPP clock frequency in MHz needs to be passed*/
    dpr_freq = soc_property_get(unit, spn_DPR_CLOCK_FREQUENCY, -1);
    SOC_IF_ERROR_RETURN
        (soc_tomahawk3_dpr_frequency_range_check(unit, dpr_freq));
    in_portmap->port_p2PBLK_inst_mapping[0] = dpr_freq;

    PBMP_ALL_ITER(unit, port) {
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            continue;
        }
        in_portmap->log_port_speed[port] = si->port_speed_max[port];
        in_portmap->port_num_lanes[port] = si->port_num_lanes[port];
        /*Override port speed for CPU and LB ports*/
        if (IS_CPU_PORT(unit, port) || (IS_LB_PORT(unit, port))) {
            in_portmap->log_port_speed[port] = _TH3_CPU_LB_PORT_SPEED;
        }
    }
    sal_memcpy(in_portmap->port_p2l_mapping, si->port_p2l_mapping,
                sizeof(int) * SOC_MAX_NUM_PORTS);
    sal_memcpy(in_portmap->port_l2p_mapping, si->port_l2p_mapping,
                sizeof(int) * SOC_MAX_NUM_PORTS);
    sal_memcpy(in_portmap->port_p2m_mapping, si->port_p2m_mapping,
                sizeof(int) * SOC_MAX_NUM_PORTS);
    sal_memcpy(in_portmap->port_m2p_mapping, si->port_m2p_mapping,
                sizeof(int) * SOC_MAX_NUM_MMU_PORTS);
    sal_memcpy(in_portmap->port_l2i_mapping, si->port_l2i_mapping,
                sizeof(int) * SOC_MAX_NUM_PORTS);
    /*physical pbm is dont care for init
     *sal_memcpy(&in_portmap->physical_pbm, &si->physical_pbm, sizeof(pbmp_t));
     */
    sal_memcpy(&in_portmap->management_pbm, &si->management_pbm,
                sizeof(pbmp_t));
    /* Construct in_port_map */
    cutthru_prop = &port_schedule_state->cutthru_prop;
    SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &latency));
    cutthru_prop->switch_bypass_mode = latency;
    /* Construct tdm_ingress_schedule_pipe and tdm_egress_schedule_pipe */
    port_schedule_state->is_flexport = 0;
    /*DV function to setup out_port_map.
     *Required before calling any DV function
     */
        SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_set_out_port_map(unit,
                port_schedule_state));
#undef _TH3_CPU_LB_PORT_SPEED
    return SOC_E_NONE;
}



/*
 * Function:
 *      soc_tomahawk3_port_schedule_tdm_init
 * Purpose:
 *      Initialize TDM information in port_schedule_state that will be passed
 *      to DV FlexPort API
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (OUT) Port resource state.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      If called during Scheduler Initialization, only below values need to be
 *      properly set:
 *        soc_tdm_schedule_pipe_t::num_slices
 *        soc_tdm_schedule_t:: cal_len
 */
STATIC int
soc_tomahawk3_port_schedule_tdm_init(int unit, soc_port_schedule_state_t
        *port_schedule_state)
{
#if 0
    soc_control_t *soc = SOC_CONTROL(unit);
    _soc_tomahawk2_tdm_t *tdm = soc->tdm_info;
    soc_tdm_schedule_t *sched;
#endif
    soc_tdm_schedule_pipe_t *tdm_ischd, *tdm_eschd;
    int pipe;
    /*int *port_p2PBLK_inst_mapping;*/

#if 0
    if (tdm == NULL) {
        /*XGSSIM will skip tdm configuration on TH2*/
        if (SAL_BOOT_XGSSIM) {
            return SOC_E_NONE;
        }
        return SOC_E_INIT;
    }
#endif
    /* Construct tdm_ingress_schedule_pipe and tdm_egress_schedule_pipe */
    for (pipe = 0; pipe <  _TH3_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        tdm_eschd = &port_schedule_state->tdm_egress_schedule_pipe[pipe];

        tdm_ischd->num_slices = _TH3_OVS_HPIPE_COUNT_PER_PIPE;
        tdm_eschd->num_slices = _TH3_OVS_HPIPE_COUNT_PER_PIPE;

#if 0
    is_flexport = port_schedule_state->is_flexport;
        if (is_flexport) {
            /* TDM Calendar is always in slice 0 */
            sal_memcpy(tdm_ischd->tdm_schedule_slice[0].linerate_schedule,
                        tdm->tdm_pipe[pipe].idb_tdm,
                        sizeof(int)*_TH3_TDM_LENGTH);
            sal_memcpy(tdm_eschd->tdm_schedule_slice[0].linerate_schedule,
                        tdm->tdm_pipe[pipe].mmu_tdm,
                        sizeof(int)*_TH3_TDM_LENGTH);
        }
#endif
#if 0
        /* OVS */
        for (hpipe = 0; hpipe < _TH3_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            /* IDB OVERSUB*/
            sched = &tdm_ischd->tdm_schedule_slice[hpipe];
            sched->cal_len = _TH3_TDM_LENGTH;

            /* MMU OVERSUB */
            sched = &tdm_eschd->tdm_schedule_slice[hpipe];
            sched->cal_len = _TH3_TDM_LENGTH;

        }
#endif
    }
    return SOC_E_NONE;
}

int _soc_th3_get_num_ucq(int unit)
{
    int mc_q_mode, num_ucq;

    mc_q_mode = soc_property_get(unit, spn_MMU_PORT_NUM_MC_QUEUE, 2);
    num_ucq = (mc_q_mode == 0) ? 12 : (mc_q_mode == 1) ? 10 :
              (mc_q_mode == 2) ? 8  : (mc_q_mode == 3) ? 6 : 8;

    return num_ucq;
}

int _soc_th3_get_num_mcq(int unit)
{
    int mc_q_mode, num_mcq;

    mc_q_mode = soc_property_get(unit, spn_MMU_PORT_NUM_MC_QUEUE, 2);
    num_mcq = (mc_q_mode == 0) ? 0 : (mc_q_mode == 1) ? 2 :
              (mc_q_mode == 2) ? 4 : (mc_q_mode == 3) ? 6 : 4;

    return num_mcq;
}


/*
 *Function: soc_tomahawk3_l0_cos_set
 *Purpose: Assign Hardware L0 index to a cos
 */
int
soc_tomahawk3_l0_cos_set(int unit, soc_port_t port, int hw_index, soc_cos_t cos)
{
    if (cos < 0 || cos >= SOC_TH3_NUM_GP_QUEUES || hw_index < 0 || hw_index >=
            SOC_TH3_NUM_GP_QUEUES) {
        return SOC_E_PARAM;
    }
    th3_cosq_mmu_info[unit]->th3_port_info[port].L0[hw_index].cos = cos;
    LOG_INFO(BSL_LS_SOC_COSQ,
            (BSL_META_U(unit, "Port: %0d L0[%0d].cos: %0d in_use:%0d\n"),
             port, hw_index, cos, th3_cosq_mmu_info[unit]->th3_port_info[port].L0[hw_index].in_use));
    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_sched_profile_attach
 *Purpose: Update port structure for scheduler based on profile information
 *         Program the MMU to SCHQ mapping fro the profile in HW
 */
int
soc_tomahawk3_sched_profile_attach(int unit, soc_port_t port, int profile)
{
    soc_reg_t reg;
    uint32 rval;
    uint64 rval_64;
    int queue_num, i;
    int mmuq, schedq;
    soc_reg_t reg_array[] = {MMU_EBPCC_MMUQ_SCHQ_CFGr,
                             MMU_MTRO_MMUQ_SCHQ_CFGr,
                             MMU_QSCH_MMUQ_TO_SCHQ_MAPr,
                             MMU_PORT_MMUQ_SCHQ_CFGr};

    soc_field_t schedq_field[] = {
        SCH_Q_NUM_0f, SCH_Q_NUM_1f,
        SCH_Q_NUM_2f, SCH_Q_NUM_3f,
        SCH_Q_NUM_4f, SCH_Q_NUM_5f,
        SCH_Q_NUM_6f, SCH_Q_NUM_7f,
        SCH_Q_NUM_8f, SCH_Q_NUM_9f,
        SCH_Q_NUM_10f, SCH_Q_NUM_11f,
    };
    soc_field_t mmuq_field[12] = {
        MMU_Q_NUM_0f, MMU_Q_NUM_1f,
        MMU_Q_NUM_2f, MMU_Q_NUM_3f,
        MMU_Q_NUM_4f, MMU_Q_NUM_5f,
        MMU_Q_NUM_6f, MMU_Q_NUM_7f,
        MMU_Q_NUM_8f, MMU_Q_NUM_9f,
        MMU_Q_NUM_10f, MMU_Q_NUM_11f,
    };
    if (!SOC_PORT_VALID(unit, port)) {
        return SOC_E_PORT;
    }
    if (IS_CPU_PORT(unit, port)) {
        return SOC_E_NONE;
    }
    for (i = 0; i < sizeof(reg_array) / sizeof(reg); i++) {
        reg = reg_array[i];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, Q_PROFILE_SELf, profile);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, port, 0, rval));
    }

    /*L1 to L0 mapping construct*/
    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        if (L1_TO_L0_MAPPING[unit][profile][queue_num] != -1) {
            soc_th3_cosq_node_t *parent;
            parent = &th3_cosq_mmu_info[unit]->th3_port_info[port].
                L0[L1_TO_L0_MAPPING[unit][profile][queue_num]];
            th3_cosq_mmu_info[unit]->th3_port_info[port].
                L1[queue_num].parent = parent;
        } else {
            th3_cosq_mmu_info[unit]->th3_port_info[port].
                L1[queue_num].parent = NULL;
        }
    }
    /*L0 to L1 mapping construct*/
    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        if (L0_TO_L1_MAPPING_NUM[unit][profile][queue_num] == 0) {
            th3_cosq_mmu_info[unit]->th3_port_info[port].
                L0[queue_num].in_use = 0;
            continue;
        } else {
            th3_cosq_mmu_info[unit]->th3_port_info[port].L0[queue_num].in_use = 1;
            th3_cosq_mmu_info[unit]->th3_port_info[port].L0[queue_num].cos = 
                L0_TO_COSQ_MAPPING[unit][profile][queue_num];

            if (L0_TO_L1_MAPPING_NUM[unit][profile][queue_num] == 1) {
                th3_cosq_mmu_info[unit]->th3_port_info[port].L0[queue_num].
                    children[0] = &th3_cosq_mmu_info[unit]->th3_port_info[port].
                    L1[queue_num];
            } else if (L0_TO_L1_MAPPING_NUM[unit][profile][queue_num] == 2) {
                th3_cosq_mmu_info[unit]->th3_port_info[port].L0[queue_num].
                    children[0] = &th3_cosq_mmu_info[unit]->th3_port_info[port].
                    L1[queue_num];
                th3_cosq_mmu_info[unit]->th3_port_info[port].L0[queue_num].
                    children[1] = &th3_cosq_mmu_info[unit]->th3_port_info[port].
                    L1[queue_num - 1];
            } else {
                /* No children */
                th3_cosq_mmu_info[unit]->th3_port_info[port].L0[queue_num].
                    children[0] = NULL;
            }
        }
    }
    /*mmuq to schedq mapping construct*/
    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        reg = MMU_EBPCC_MMUQ_SCHQ_PROFILEr;
        SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, port, profile, &rval_64));
        schedq = soc_reg64_field32_get(unit, reg, rval_64, schedq_field[queue_num]);
        th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[queue_num].parent =
            &th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq];
    }
    /*schedq to mmuq mapping construct*/
    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        reg = MMU_PPSCH_SCHQ_MMUQ_PROFILEr;
        SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, port, profile, &rval_64));
        mmuq = soc_reg64_field32_get(unit, reg, rval_64, mmuq_field[queue_num]);
        th3_cosq_mmu_info[unit]->th3_port_info[port].L1[queue_num].children[0]
            = &th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[mmuq];
    }
    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_sched_profile_attach_reinit
 *Purpose: Update port structure for scheduler based on profile information
 *         Program the MMU to SCHQ mapping fro the profile in HW
 */
int
soc_tomahawk3_sched_profile_attach_reinit(int unit, soc_port_t port, int profile)
{
    int queue_num;

    /*L1 to L0 mapping construct*/
    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        if (L1_TO_L0_MAPPING[unit][profile][queue_num] != -1) {
            soc_th3_cosq_node_t *parent;
            parent = &th3_cosq_mmu_info[unit]->th3_port_info[port].
                L0[L1_TO_L0_MAPPING[unit][profile][queue_num]];
            th3_cosq_mmu_info[unit]->th3_port_info[port].
                L1[queue_num].parent = parent;
        } else {
            th3_cosq_mmu_info[unit]->th3_port_info[port].
                L1[queue_num].parent = NULL;
        }
    }
    /*L0 to L1 mapping construct*/
    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        if (L0_TO_L1_MAPPING_NUM[unit][profile][queue_num] == 0) {
            th3_cosq_mmu_info[unit]->th3_port_info[port].
                L0[queue_num].in_use = 0;
            continue;
        } else {
            th3_cosq_mmu_info[unit]->th3_port_info[port].L0[queue_num].in_use = 1;
            th3_cosq_mmu_info[unit]->th3_port_info[port].L0[queue_num].cos = 
                L0_TO_COSQ_MAPPING[unit][profile][queue_num];

            if (L0_TO_L1_MAPPING_NUM[unit][profile][queue_num] == 1) {
                th3_cosq_mmu_info[unit]->th3_port_info[port].L0[queue_num].
                    children[0] = &th3_cosq_mmu_info[unit]->th3_port_info[port].
                    L1[queue_num];
            } else if (L0_TO_L1_MAPPING_NUM[unit][profile][queue_num] == 2) {
                th3_cosq_mmu_info[unit]->th3_port_info[port].L0[queue_num].
                    children[0] = &th3_cosq_mmu_info[unit]->th3_port_info[port].
                    L1[queue_num];
                th3_cosq_mmu_info[unit]->th3_port_info[port].L0[queue_num].
                    children[1] = &th3_cosq_mmu_info[unit]->th3_port_info[port].
                    L1[queue_num - 1];
            } else {
                /* No children */
                th3_cosq_mmu_info[unit]->th3_port_info[port].L0[queue_num].
                    children[0] = NULL;
            }
        }
    }

    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_sched_profile_get
 *Purpose: Get the scheduler rofile for a port
 */
int
soc_tomahawk3_sched_profile_get(int unit, soc_port_t port, int *profile)
{
    soc_reg_t reg;
    uint32 rval;

    if (profile == NULL) {
        return SOC_E_PARAM;
    }

    if (IS_CPU_PORT(unit, port)) {
        return SOC_E_NONE;
    }

    reg = MMU_EBPCC_MMUQ_SCHQ_CFGr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
    *profile = soc_reg_field_get(unit, reg, rval, Q_PROFILE_SELf);

    return SOC_E_NONE;
}

/*
 *Function: soc_profile_port_list_get
 *Purpose: retrieve information of all ports attached to a profile
 */
int
soc_profile_port_list_get(int unit, int profile, int port_in_profile[])
{
    soc_reg_t reg = MMU_EBPCC_MMUQ_SCHQ_CFGr;
    int profile_in_use;
    uint32 rval;
    soc_port_t port;

    SOC_PBMP_ITER(PBMP_ALL(unit), port) {
        if (!IS_CPU_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
            profile_in_use = soc_reg_field_get(unit, reg, rval, Q_PROFILE_SELf);
            if (profile == profile_in_use) {
                port_in_profile[port] = 1;
            } else {
                port_in_profile[port] = 0;
            }
        }
    }
    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_l1_to_l0_profile_set
 *Purpose: Update L1 to L0 mapping information in HW and teh SW structure
 */
int
soc_tomahawk3_l1_to_l0_profile_set(int unit, int profile, int L0, int schedq,
        int port_in_profile[])
{
    soc_reg_t reg = INVALIDr;
    uint64 rval_64;
    uint32 mapping = 0;
    int port;
    uint32 rval;

    if (profile < 0 || profile >= SOC_TH3_MAX_NUM_SCHED_PROFILE) {
        return SOC_E_PARAM;
    }

    L1_TO_L0_MAPPING[unit][profile][schedq] = L0;
    L0_TO_L1_MAPPING_NUM[unit][profile][L0]++;
    LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "Profile %d L1_TO_L0_MAPPING[%d]: %d L0_TO_L1_MAPPING_NUM[%d]: %d\n"),
                        profile, schedq,  L1_TO_L0_MAPPING[unit][profile][schedq], L0, L0_TO_L1_MAPPING_NUM[unit][profile][L0]));

    for (port = 0; port < SOC_TH3_MAX_NUM_PORTS; port++) {
        if (port_in_profile[port] == 1) {
            /* Set same value of L1 to L0 map in MTRO*/
            reg = MMU_MTRO_MMUQ_SCHQ_CFGr;
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
            mapping = soc_reg_field_get(unit, reg, rval, L1_TO_L0_MAPf);
             /*L1_TO_L0_MAP [x] configures L1[x], where x = 0..10.  Note L1[11]
              *is only connected to L0[11] so is not included in this config.
              *bit[x]=0: (default) L1[x] connected to L0[x];
              *bit[x]=1: (default) L1[x] connected to L0[x+1]
              */
            if (schedq == L0)
                mapping = mapping & ~(1 << schedq);
            else
                mapping = mapping | (1 << schedq);

            soc_reg_field_set(unit, reg, &rval, L1_TO_L0_MAPf, mapping);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, port, 0, rval));

            reg = MMU_QSCH_PORT_CONFIGr;
            SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, port, 0, &rval_64));
            soc_reg64_field32_set(unit, reg, &rval_64, L1_TO_L0_MAPPINGf, mapping);
            SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, reg, port, 0, rval_64));

            th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq].parent =
                &th3_cosq_mmu_info[unit]->th3_port_info[port].L0[L0];
            th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq].in_use = 1;
            if (th3_cosq_mmu_info[unit]->th3_port_info[port].L0[L0].children[0]
                                                                  == NULL) {
                th3_cosq_mmu_info[unit]->th3_port_info[port].L0[L0].children[0]
                    = &th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq];
                th3_cosq_mmu_info[unit]->th3_port_info[port].L0[L0].in_use = 1;
            } else {
                th3_cosq_mmu_info[unit]->th3_port_info[port].L0[L0].children[1]
                    = &th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq];
            }
        }
    }
    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_l1_to_l0_profile_set_reinit
 *Purpose: Update L1 to L0 mapping information in HW and teh SW structure
 */
int
soc_tomahawk3_l1_to_l0_profile_set_reinit(int unit, int profile, int L0, int schedq,
        int port_in_profile[])
{
    int port;

    if (profile < 0 || profile >= SOC_TH3_MAX_NUM_SCHED_PROFILE) {
        return SOC_E_PARAM;
    }

    L1_TO_L0_MAPPING[unit][profile][schedq] = L0;
    L0_TO_L1_MAPPING_NUM[unit][profile][L0]++;

    for (port = 0; port < SOC_TH3_MAX_NUM_PORTS; port++) {
        if (port_in_profile[port] == 1) {
            th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq].parent =
                &th3_cosq_mmu_info[unit]->th3_port_info[port].L0[L0];
            th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq].in_use = 1;
            if (th3_cosq_mmu_info[unit]->th3_port_info[port].L0[L0].children[0]
                                                                  == NULL) {
                th3_cosq_mmu_info[unit]->th3_port_info[port].L0[L0].children[0]
                    = &th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq];
                th3_cosq_mmu_info[unit]->th3_port_info[port].L0[L0].in_use = 1;
            } else {
                th3_cosq_mmu_info[unit]->th3_port_info[port].L0[L0].children[1]
                    = &th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq];
            }
        }
    }
    return SOC_E_NONE;
}


/*
 *Function: soc_tomahawk3_l1_to_l0_profile_update_per_port
 *Purpose: Update L1 to L0 mapping information in HW and teh SW structure
 */
int
soc_tomahawk3_l1_to_l0_profile_update_per_port(int unit, int port, int profile)
{
    int L0[SOC_TH3_COS_MAX];
    int schedq[SOC_TH3_COS_MAX];
    int i;

    for (i = 0; i < SOC_TH3_COS_MAX; i++) {
        L0[i] = L1_TO_L0_MAPPING[unit][profile][i];
        schedq[i] = i;
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_l1_to_l0_profile_set_per_port(unit, port, profile, L0[i], schedq[i]));
    }
    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_l1_to_l0_profile_set_per_port
 *Purpose: Update L1 to L0 mapping information in HW and teh SW structure
 */
int
soc_tomahawk3_l1_to_l0_profile_set_per_port(int unit, int port, int profile, int L0, int schedq)
{
    soc_reg_t reg = INVALIDr;
    uint64 rval_64;
    uint32 mapping = 0;
    uint32 rval;

    if (profile < 0 || profile >= SOC_TH3_MAX_NUM_SCHED_PROFILE) {
        return SOC_E_PARAM;
    }

    /* Set same value of L1 to L0 map in MTRO*/
    reg = MMU_MTRO_MMUQ_SCHQ_CFGr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
    mapping = soc_reg_field_get(unit, reg, rval, L1_TO_L0_MAPf);
     /*L1_TO_L0_MAP [x] configures L1[x], where x = 0..10.  Note L1[11]
      *is only connected to L0[11] so is not included in this config.
      *bit[x]=0: (default) L1[x] connected to L0[x];
      *bit[x]=1: (default) L1[x] connected to L0[x+1]
      */
    if (schedq == L0) {
        mapping = mapping & ~(1 << schedq);
    }
    else {
        mapping = mapping | (1 << schedq);
    }

    soc_reg_field_set(unit, reg, &rval, L1_TO_L0_MAPf, mapping);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, port, 0, rval));

    reg = MMU_QSCH_PORT_CONFIGr;
    SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, port, 0, &rval_64));
    soc_reg64_field32_set(unit, reg, &rval_64, L1_TO_L0_MAPPINGf, mapping);
    SOC_IF_ERROR_RETURN
        (soc_reg64_set(unit, reg, port, 0, rval_64));

    th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq].parent =
        &th3_cosq_mmu_info[unit]->th3_port_info[port].L0[L0];
    th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq].in_use = 1;
    if (th3_cosq_mmu_info[unit]->th3_port_info[port].L0[L0].children[0]
                                                          == NULL) {
        th3_cosq_mmu_info[unit]->th3_port_info[port].L0[L0].children[0]
            = &th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq];
        th3_cosq_mmu_info[unit]->th3_port_info[port].L0[L0].in_use = 1;
    } else {
        th3_cosq_mmu_info[unit]->th3_port_info[port].L0[L0].children[1]
            = &th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq];
    }

    soc_gport_tree_print(unit, port);

    return SOC_E_NONE;
}


/*! @fn void soc_print_port_schedule_state(int unit,
 *               soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @brief Debug API to print port_schedule_state
 */
void
soc_th3_print_port_schedule_state(int                        unit,
                              soc_port_schedule_state_t *port_schedule_state)
{

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "\n\nPRINTING soc_port_schedule_state_t \n")));

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "frequency= %4dMHz\n"), port_schedule_state->frequency));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "DPP frequency= %4dMHz\n"),
                port_schedule_state->in_port_map.port_p2PBLK_inst_mapping[0]));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "lossless= %1d\n"), port_schedule_state->lossless));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "is_flexport= %1d\n"), port_schedule_state->is_flexport));

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "\nPRINTING soc_port_schedule_state_t::in_port_map\n")));
    soc_print_port_map(unit, &port_schedule_state->in_port_map);

    if (port_schedule_state->is_flexport == 1) {
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "\nPRINTING soc_port_schedule_state_t::out_port_map\n")));
        soc_print_port_map(unit, &port_schedule_state->out_port_map);
    }

#if 0
    if ((port_schedule_state->is_flexport == 1) &&
        (port_schedule_state->nport > 0)) {
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "\n====== FLEXPORT RESOURCES ===== num entries= %d = \n"),
                  port_schedule_state->nport));
        for (i = 0; i < port_schedule_state->nport; i++) {
            soc_print_port_resource(unit, &port_schedule_state->resource[i], i);
        }
    }
#endif
}

/*Function:
 *  soc_tomahawk3_tdm_update
 *Purpose:
 *  Setup port_schedule_state structure and call TDM init
 *Params:
 * unit: Chip unit number
 */
int
soc_tomahawk3_tdm_update (int unit)
{
    soc_port_schedule_state_t *port_schedule_state;
    int rv;

    port_schedule_state = sal_alloc(sizeof(soc_port_schedule_state_t),
                                    "Tomahawk3 soc_port_schedule_state_t");
    if (port_schedule_state == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(port_schedule_state, 0, sizeof(soc_port_schedule_state_t));
    rv = soc_tomahawk3_port_schedule_state_basic_setup(unit,
            port_schedule_state);
    if (rv != SOC_E_NONE) {
        goto exit;
    }
    rv = soc_tomahawk3_port_schedule_tdm_init(unit, port_schedule_state);
    if (rv != SOC_E_NONE) {
        goto exit;
    }

    soc_th3_print_port_schedule_state(unit, port_schedule_state);

    rv = soc_tomahawk3_tdm_init(unit, port_schedule_state);

    if (rv != SOC_E_NONE) {
        LOG_CLI((BSL_META_U(unit,
            "Unable to configure TDM, please contact your "
            "Field Applications Engineer or Sales Manager for "
            "additional assistance.\n")));
        sal_free(port_schedule_state);
        return rv;
    }

    /* Update SOC TDM info */
    /*The TDM infor needs to be captured in SOC_CONTROL->tdm_info before
     *port_schedule_state is freed. This function needs to be defined for TH3*/
    /*rv = soc_th2_soc_tdm_update(unit, port_schedule_state);*/

exit:
    sal_free(port_schedule_state);
    return rv;
}

/*
 *Function: soc_tomahawk3_cpu_l1_to_l0_profile_set
 *Purpose: Update L1 to L0 mapping information  for CPU port
 */
int
soc_tomahawk3_cpu_l1_to_l0_profile_set(int unit, int L0, int schedq)
{
    soc_reg_t reg = INVALIDr;
    uint32 rval;
    th3_cosq_mmu_info[unit]->th3_cpu_port_info.L0[L0].in_use = 1;
    th3_cosq_mmu_info[unit]->th3_cpu_port_info.L0[L0].children[schedq] =
        &th3_cosq_mmu_info[unit]->th3_cpu_port_info.L1[schedq];
    th3_cosq_mmu_info[unit]->th3_cpu_port_info.L1[schedq].in_use = 1;
    th3_cosq_mmu_info[unit]->th3_cpu_port_info.L1[schedq].parent =
        &th3_cosq_mmu_info[unit]->th3_cpu_port_info.L0[L0];

    reg = MMU_MTRO_CPU_L1_TO_L0_MAPPINGr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, schedq, &rval));
    soc_reg_field_set(unit, reg, &rval, SELECTf, L0);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, schedq, rval));

    reg = MMU_QSCH_CPU_L1_TO_L0_MAPPINGr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, schedq, &rval));
    soc_reg_field_set(unit, reg, &rval, SELECTf, L0);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, schedq, rval));
    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_mmuq_to_schedq_profile_set_per_port
 *Purpose: Updated MMUQ to SCHQ mapping profile
 */
int
soc_tomahawk3_mmuq_to_schedq_profile_set_per_port(int unit, int port, int schedq,
        int mmuq)
{
    th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[mmuq].parent =
        &th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq];
    th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq].children[0]
        = &th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[mmuq];
    return SOC_E_NONE;
}
/*
 *Function: soc_tomahawk3_mmuq_to_schedq_profile_set
 *Purpose: Updated MMUQ to SCHQ mapping profile
 */
int
soc_tomahawk3_mmuq_to_schedq_profile_set(int unit, int profile, int schedq,
        int mmuq, int port_in_profile[])
{
    soc_reg_t reg = INVALIDr;
    uint64 rval_64;
    int port;
    soc_field_t schedq_field[12] = {
        SCH_Q_NUM_0f, SCH_Q_NUM_1f,
        SCH_Q_NUM_2f, SCH_Q_NUM_3f,
        SCH_Q_NUM_4f, SCH_Q_NUM_5f,
        SCH_Q_NUM_6f, SCH_Q_NUM_7f,
        SCH_Q_NUM_8f, SCH_Q_NUM_9f,
        SCH_Q_NUM_10f, SCH_Q_NUM_11f,
    };
    soc_field_t mmuq_field[12] = {
        MMU_Q_NUM_0f, MMU_Q_NUM_1f,
        MMU_Q_NUM_2f, MMU_Q_NUM_3f,
        MMU_Q_NUM_4f, MMU_Q_NUM_5f,
        MMU_Q_NUM_6f, MMU_Q_NUM_7f,
        MMU_Q_NUM_8f, MMU_Q_NUM_9f,
        MMU_Q_NUM_10f, MMU_Q_NUM_11f,
    };

    if (profile < 0 || profile >= SOC_TH3_MAX_NUM_SCHED_PROFILE) {
        return SOC_E_PARAM;
    }
    reg = MMU_EBPCC_MMUQ_SCHQ_PROFILEr;
    SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, 0, profile, &rval_64));
    soc_reg64_field32_set(unit, reg, &rval_64, schedq_field[mmuq], schedq);
    SOC_IF_ERROR_RETURN
        (soc_reg64_set(unit, reg, 0, profile, rval_64));

    reg = MMU_MTRO_MMUQ_SCHQ_PROFILEr;
    SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, 0, profile, &rval_64));
    soc_reg64_field32_set(unit, reg, &rval_64, schedq_field[mmuq], schedq);
    SOC_IF_ERROR_RETURN
        (soc_reg64_set(unit, reg, 0, profile, rval_64));

    reg = MMU_PPSCH_SCHQ_MMUQ_PROFILEr;
    SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, 0, profile, &rval_64));
    soc_reg64_field32_set(unit, reg, &rval_64, mmuq_field[schedq], mmuq);
    SOC_IF_ERROR_RETURN
        (soc_reg64_set(unit, reg, 0, profile, rval_64));

    for (port = 0; port < SOC_TH3_MAX_NUM_PORTS; port++) {
        if (port_in_profile[port] == 1) {
            th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[mmuq].parent =
                &th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq];
            th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq].children[0]
                = &th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[mmuq];
        }
    }
    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_mmuq_to_schedq_profile_set_reinit
 *Purpose: Updated MMUQ to SCHQ mapping profile
 */
int
soc_tomahawk3_mmuq_to_schedq_profile_set_reinit(int unit, int profile, int schedq,
        int mmuq, int port_in_profile[])
{
    int port;

    if (profile < 0 || profile >= SOC_TH3_MAX_NUM_SCHED_PROFILE) {
        return SOC_E_PARAM;
    }

    for (port = 0; port < SOC_TH3_MAX_NUM_PORTS; port++) {
        if (port_in_profile[port] == 1) {
            th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[mmuq].parent =
                &th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq];
            th3_cosq_mmu_info[unit]->th3_port_info[port].L1[schedq].children[0]
                = &th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[mmuq];
        }
    }
    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_sched_update_init
 *Purpose: Wrapper function to set L0->schedq->MMUQ map for a specific profile
 */
int
soc_tomahawk3_sched_update_init(int unit, int profile, int *L0, int *schedq,
        int *mmuq, int *cos, int *strict_priority, int* fc_is_uc_only)
{
    int port_in_profile[SOC_TH3_MAX_NUM_PORTS] = {0};
    int port, queue;

    if (profile < 0 || profile >= SOC_TH3_MAX_NUM_SCHED_PROFILE) {
        return SOC_E_PARAM;
    }
    /*check which ports is using this profile*/
    soc_profile_port_list_get(unit, profile, port_in_profile);

    PBMP_ALL_ITER(unit, port) {
        if (port_in_profile[port]) {
            LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "Port info init for port %0d\n"),
                        port));
            soc_tomahawk3_cosq_port_info_init(unit, port);
        }
    }

    soc_tomahawk3_cosq_profile_info_init(unit, profile);

    for (queue = 0; queue < SOC_TH3_NUM_GP_QUEUES; queue++) {
        /* initialize per profile structure */
        int cosq_index;
        cosq_index = L0[queue];
        if (cos[queue] == -1) {
            continue;
        }
        th3_sched_profile_info[unit][profile][cosq_index].cos = cos[queue];
        th3_sched_profile_info[unit][profile][cosq_index].strict_priority = strict_priority[queue];
        th3_sched_profile_info[unit][profile][cosq_index].fc_is_uc_only = fc_is_uc_only[queue];
        if (th3_sched_profile_info[unit][profile][cosq_index].mmuq[0] == -1) {
            th3_sched_profile_info[unit][profile][cosq_index].mmuq[0] = mmuq[queue];
        }
        else {
            th3_sched_profile_info[unit][profile][cosq_index].mmuq[1] = mmuq[queue];
        }

        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_l1_to_l0_profile_set(unit, profile, L0[queue], schedq[queue],
                                                port_in_profile));
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_mmuq_to_schedq_profile_set(unit, profile, schedq[queue],
                                                      mmuq[queue], port_in_profile));
        L0_TO_COSQ_MAPPING[unit][profile][queue] = cos[queue];
    }

    SOC_IF_ERROR_RETURN
        (soc_cosq_sched_policy_init(unit, profile));

    PBMP_ALL_ITER(unit, port) {
        if (port_in_profile[port]) {
            for (queue = 0; queue < SOC_TH3_NUM_GP_QUEUES; queue++) {
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk3_l0_cos_set(unit, port, queue, cos[queue]));
            }

            LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "Attach port %d to profile %d\n"),
                 port, profile));
            SOC_IF_ERROR_RETURN
                (soc_tomahawk3_sched_profile_attach(unit, port, profile));
        }
    }
    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_sched_update_reinit
 *Purpose: Wrapper function to set L0->schedq->MMUQ map for a specific profile
 */
int
soc_tomahawk3_sched_update_reinit(int unit, int profile, int *L0, int *schedq,
        int *mmuq, int *cos, int *strict_priority, int* fc_is_uc_only)
{
    int port_in_profile[SOC_TH3_MAX_NUM_PORTS] = {0};
    int port, queue;

    if (profile < 0 || profile >= SOC_TH3_MAX_NUM_SCHED_PROFILE) {
        return SOC_E_PARAM;
    }
    /*check which ports is using this profile*/
    soc_profile_port_list_get(unit, profile, port_in_profile);

    SOC_IF_ERROR_RETURN(soc_cosq_profile_struct_init(unit, profile));

    PBMP_ALL_ITER(unit, port) {
        if (port_in_profile[port]) {
            LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "Port info init for port %0d\n"),
                        port));
            soc_tomahawk3_cosq_port_info_init(unit, port);
        }
    }

    for (queue = 0; queue < SOC_TH3_NUM_GP_QUEUES; queue++) {
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_l1_to_l0_profile_set_reinit(unit, profile, L0[queue], schedq[queue],
                                                port_in_profile));
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_mmuq_to_schedq_profile_set_reinit(unit, profile, schedq[queue],
                                                      mmuq[queue], port_in_profile));
        L0_TO_COSQ_MAPPING[unit][profile][queue] = cos[queue];
    }

    SOC_IF_ERROR_RETURN
        (soc_cosq_sched_policy_reinit(unit, profile));

    PBMP_ALL_ITER(unit, port) {
        if (port_in_profile[port]) {
            for (queue = 0; queue < SOC_TH3_NUM_GP_QUEUES; queue++) {
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk3_l0_cos_set(unit, port, queue, cos[queue]));
            }

            LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "Attach port %d to profile %d\n"),
                 port, profile));
            SOC_IF_ERROR_RETURN
                (soc_tomahawk3_sched_profile_attach_reinit(unit, port, profile));
        }
    }
    return SOC_E_NONE;
}


/*
 *Function: soc_tomahawk3_sched_update_flex_per_port
 *Purpose: Wrapper function to set L0->schedq->MMUQ map for a specific profile
 */
int
soc_tomahawk3_sched_update_flex_per_port(int unit, int port, int profile, int *L0, int *schedq,
        int *mmuq, int *cos)
{
    int queue;

    if (profile < 0 || profile >= SOC_TH3_MAX_NUM_SCHED_PROFILE) {
        return SOC_E_PARAM;
    }

    soc_tomahawk3_cosq_port_info_init(unit, port);

    for (queue = 0; queue < SOC_TH3_NUM_GP_QUEUES; queue++) {
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_l1_to_l0_profile_set_per_port(unit, port, profile, L0[queue], schedq[queue]));
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_mmuq_to_schedq_profile_set_per_port(unit, port, schedq[queue],
                                                      mmuq[queue]));
    }

    /* Not needed for flex . Done in cosq_port_attach
    SOC_IF_ERROR_RETURN
        (soc_cosq_sched_policy_init(unit, profile, sp_child, sp_parent));
    */

    for (queue = 0; queue < SOC_TH3_NUM_GP_QUEUES; queue++) {
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_l0_cos_set(unit, port, queue, cos[queue]));
    }

    LOG_INFO(BSL_LS_SOC_COSQ,
        (BSL_META_U(unit, "Attach port %d to profile %d\n"),
         port, profile));
    SOC_IF_ERROR_RETURN
        (soc_tomahawk3_sched_profile_attach(unit, port, profile));

    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_sched_profile_set
 *Purpose: Wrapper function to set L0->schedq->MMUQ map for a specific profile
 */
int
soc_tomahawk3_sched_profile_set(int unit, int profile, int L0, int schedq,
        int mmuq)
{
    int port_in_profile[SOC_TH3_MAX_NUM_PORTS] = {0};

    if (profile < 0 || profile >= SOC_TH3_MAX_NUM_SCHED_PROFILE) {
        return SOC_E_PARAM;
    }
    /*check which ports is using this profile*/
    soc_profile_port_list_get(unit, profile, port_in_profile);

    SOC_IF_ERROR_RETURN
        (soc_tomahawk3_l1_to_l0_profile_set(unit, profile, L0, schedq,
                                            port_in_profile));
    SOC_IF_ERROR_RETURN
        (soc_tomahawk3_mmuq_to_schedq_profile_set(unit, profile, schedq,
                                                  mmuq, port_in_profile));

    return SOC_E_NONE;
}

/*
 *Function: soc_cosq_hierarchy_init
 *Purpose: Wrapper function to setup scheduler hierarchy for the unit
 */
int
soc_cosq_hierarchy_init(int unit)
{
#define _TH3_MMU_NUM_CPU_QUEUES_PER_COS 4
    int profile = 0;
    int queue, cpu_l1;
    soc_port_t port;
    int i, rv;
    _soc_mmu_cfg_scheduler_profile_t *sched_profile;
    int L0[SOC_TH3_NUM_GP_QUEUES];
    int schedq[SOC_TH3_NUM_GP_QUEUES];
    int mmuq[SOC_TH3_NUM_GP_QUEUES];
    int cos_list[SOC_TH3_NUM_GP_QUEUES];
    int strict_priority[SOC_TH3_NUM_GP_QUEUES];
    int fc_is_uc_only[SOC_TH3_NUM_GP_QUEUES];
    int sched_port_profile_map[SOC_TH3_MAX_NUM_PORTS];
    int port_in_profile[SOC_TH3_MAX_NUM_PORTS] = {0};

    rv = SOC_E_NONE;

    /* Override default init from config.bcm */
    sched_profile = soc_mmu_cfg_scheduler_profile_alloc(unit);
    if (!sched_profile) {
        return SOC_E_MEMORY;
    }

    /*Map all ports to use profile 0 by default*/
    for (i = 0; i < SOC_TH3_MAX_NUM_PORTS; i++)
        sched_port_profile_map[i] = 0;

    /* Override via bcm config */
    rv = _soc_mmu_tomahawk3_scheduler_profile_reset(unit, sched_profile);
    if (rv != SOC_E_NONE) {
        goto exit;
    }
    _soc_mmu_cfg_scheduler_profile_read(unit, sched_profile, sched_port_profile_map);
    rv = _soc_mmu_tomahawk3_scheduler_profile_check(unit, sched_profile);

    /* If overriden values break the rules, do back to default */
    if (SOC_FAILURE(rv)) {
        LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit, "Override failed Resetting scheduler profile to default init\n")));
        rv = _soc_mmu_tomahawk3_scheduler_profile_reset(unit, sched_profile);
        if (rv != SOC_E_NONE) {
            goto exit;
        }
    }

    /* Init */

    /*Map ports to profile. Default is profile 0 */
    PBMP_ALL_ITER(unit, port) {
        LOG_INFO(BSL_LS_SOC_COSQ,
            (BSL_META_U(unit, "Attach port %d to profile %d\n"),
             port, sched_port_profile_map[port]));
        rv = soc_tomahawk3_sched_profile_attach(unit, port, sched_port_profile_map[port]);
        if (rv != SOC_E_NONE) {
            goto exit;
        }
    }

    for (profile = 0; profile < SOC_TH3_MAX_NUM_SCHED_PROFILE; profile++) {
        soc_tomahawk3_cosq_profile_info_init(unit, profile);

        if (sched_profile[profile].valid != 1) {

            continue;
        }

        rv = _soc_scheduler_profile_mapping_setup(unit, sched_profile, profile,
            L0, schedq, mmuq, cos_list, strict_priority, fc_is_uc_only);
        if (rv != SOC_E_NONE) {
            goto exit;
        }

        for (queue = 0; queue < SOC_TH3_NUM_GP_QUEUES; queue++) {
            /* initialize per profile structure */
            int cosq_index;
            cosq_index = L0[queue];
            if (cos_list[queue] == -1) {
                continue;
            }
            th3_sched_profile_info[unit][profile][cosq_index].cos = cos_list[queue];
            th3_sched_profile_info[unit][profile][cosq_index].strict_priority = strict_priority[queue];
            th3_sched_profile_info[unit][profile][cosq_index].fc_is_uc_only = fc_is_uc_only[queue];
            if (th3_sched_profile_info[unit][profile][cosq_index].mmuq[0] == -1) {
                th3_sched_profile_info[unit][profile][cosq_index].mmuq[0] = mmuq[queue];
            }
            else {
                th3_sched_profile_info[unit][profile][cosq_index].mmuq[1] = mmuq[queue];
            }

            /*initilization of L0.i->schedq.i->mmuq.i*/
            rv = soc_tomahawk3_sched_profile_set(unit, profile, L0[queue], schedq[queue], mmuq[queue]);
            if (rv != SOC_E_NONE) {
                goto exit;
            }
            L0_TO_COSQ_MAPPING[unit][profile][queue] = cos_list[queue];
        }
        rv = soc_cosq_sched_policy_init(unit, profile);
        if (rv != SOC_E_NONE) {
            goto exit;
        }

        /*check which ports is using this profile*/
        soc_profile_port_list_get(unit, profile, port_in_profile);

        PBMP_ALL_ITER(unit, port) {
            if (port_in_profile[port]) {
                for (queue = 0; queue < SOC_TH3_NUM_GP_QUEUES; queue++) {
                    /* Max number of device ports in TH3 is 160*/
                    if (cos_list[queue] == -1) {
                        continue;
                    }
                    rv = soc_tomahawk3_l0_cos_set(unit, port, queue, cos_list[queue]);
                    if (rv != SOC_E_NONE) {
                        goto exit;
                    }
                }
            }
        }
    }

    /* CPU Port */
    for (queue = 0; queue < SOC_TH3_NUM_GP_QUEUES; queue++) {
        for(i = 0; i < _TH3_MMU_NUM_CPU_QUEUES_PER_COS; i++) {
            cpu_l1 = queue * _TH3_MMU_NUM_CPU_QUEUES_PER_COS + i;
            if (cpu_l1 >= SOC_TH3_NUM_CPU_QUEUES) {
                break;
            } else {
                rv = soc_tomahawk3_cpu_l1_to_l0_profile_set(unit, 0, cpu_l1);
                if (rv != SOC_E_NONE) {
                    goto exit;
                }
            }
        }
    }

exit:
    soc_mmu_cfg_scheduler_profile_free(unit, sched_profile);

#undef _TH3_MMU_NUM_CPU_QUEUES_PER_COS

    return rv;
}

/*
 *Function: soc_cosq_sched_policy_init
 *Purpose: Initialize scheduling discipline in HW and SW structure based on config bcm
 */
int
soc_cosq_sched_policy_init(int unit, int profile)
{
    soc_reg_t reg = INVALIDr;
    uint64 rval;
    int queue_num;
    soc_port_t port;
    int port_in_profile[SOC_TH3_MAX_NUM_PORTS] = {0};

    if (profile < 0 || profile >= SOC_TH3_MAX_NUM_SCHED_PROFILE) {
        return SOC_E_PARAM;
    }

    /*check which ports is using this profile*/
    soc_profile_port_list_get(unit, profile, port_in_profile);

    PBMP_ALL_ITER(unit, port) {
        /* Max number of device ports in TH3 is 160*/
        if (!IS_CPU_PORT(unit, port)) {
            if (port_in_profile[port] == 0)
                continue;
            reg = MMU_QSCH_PORT_CONFIGr;
            SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, port, 0, &rval));
            soc_reg64_field32_set(unit, reg, &rval, ENABLE_WRRf, 1);
            soc_reg64_field32_set(unit, reg, &rval, SP_L0_CHILDf, 0);
            soc_reg64_field32_set(unit, reg, &rval, SP_L0_PARENTf, 0);
            SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, reg, port, 0, rval));
            for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
                th3_cosq_mmu_info[unit]->th3_port_info[port].L0[queue_num].
                    sched_policy = SOC_TH3_SCHED_MODE_WRR;
                th3_cosq_mmu_info[unit]->th3_port_info[port].L1[queue_num].
                    sched_policy = SOC_TH3_SCHED_MODE_WRR;
            }
        } else {
            reg = MMU_QSCH_CPU_PORT_CONFIGr;
            SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, 0, 0, &rval));
            soc_reg64_field32_set(unit, reg, &rval, ENABLE_WRRf, 1);
            soc_reg64_field32_set(unit, reg, &rval, SP_L0f, 0);
            soc_reg64_field32_set(unit, reg, &rval, SP_L1f, 0);
            SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, reg, 0, 0, rval));
            for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
                th3_cosq_mmu_info[unit]->th3_cpu_port_info.L0[queue_num].
                    sched_policy = SOC_TH3_SCHED_MODE_WRR;
            }
            for (queue_num = 0; queue_num < SOC_TH3_NUM_CPU_QUEUES; queue_num++) {
                th3_cosq_mmu_info[unit]->th3_cpu_port_info.L1[queue_num].
                    sched_policy = SOC_TH3_SCHED_MODE_WRR;
            }
        }
    }
    return SOC_E_NONE;
}

/*
 *Function: soc_cosq_sched_policy_reinit
 *Purpose: Initialize scheduling discipline in HW and SW structure based on config bcm
 */
int
soc_cosq_sched_policy_reinit(int unit, int profile)
{
    int queue_num;
    soc_port_t port;
    int port_in_profile[SOC_TH3_MAX_NUM_PORTS] = {0};

    if (profile < 0 || profile >= SOC_TH3_MAX_NUM_SCHED_PROFILE) {
        return SOC_E_PARAM;
    }

    /*check which ports is using this profile*/
    soc_profile_port_list_get(unit, profile, port_in_profile);

    PBMP_ALL_ITER(unit, port) {
        /* Max number of device ports in TH3 is 160*/
        if (!IS_CPU_PORT(unit, port)) {
            if (port_in_profile[port] == 0)
                continue;
            for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
                th3_cosq_mmu_info[unit]->th3_port_info[port].L0[queue_num].
                    sched_policy = SOC_TH3_SCHED_MODE_WRR;
                th3_cosq_mmu_info[unit]->th3_port_info[port].L1[queue_num].
                    sched_policy = SOC_TH3_SCHED_MODE_WRR;
            }
        } else {
            for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
                th3_cosq_mmu_info[unit]->th3_cpu_port_info.L0[queue_num].
                    sched_policy = SOC_TH3_SCHED_MODE_WRR;
            }
            for (queue_num = 0; queue_num < SOC_TH3_NUM_CPU_QUEUES; queue_num++) {
                th3_cosq_mmu_info[unit]->th3_cpu_port_info.L1[queue_num].
                    sched_policy = SOC_TH3_SCHED_MODE_WRR;
            }
        }
    }
    return SOC_E_NONE;
}



/*
 *Function: soc_tomahawk3_schduler_hier_show
 *Purpose: FUnction to display configured hierarchy
 */
int
soc_tomahawk3_schduler_hier_show(int unit,int port)
{
    int queue, queue_num;

     if (SOC_PORT_VALID(unit, port)) {
        LOG_CLI(("PORT        L0          COS         SCHEDQ      MMUQ\n"));
        if (IS_CPU_PORT(unit, port)) {
            for (queue = 0; queue < SOC_TH3_NUM_GP_QUEUES; queue++) {
                for (queue_num = 0; queue_num < SOC_TH3_NUM_CPU_QUEUES;
                                                            queue_num++) {
                    if (th3_cosq_mmu_info[unit]->th3_cpu_port_info.L0[queue].
                            children[queue_num] != NULL) {
                        LOG_CLI((BSL_META_U(unit,
                            "%-8d    "), port));
                        LOG_CLI((BSL_META_U(unit,
                            "%-8d    "),
                            th3_cosq_mmu_info[unit]->th3_cpu_port_info.L0[queue].
                            hw_index));
                        LOG_CLI((BSL_META_U(unit,
                            "%-8d    "),
                            th3_cosq_mmu_info[unit]->th3_cpu_port_info.
                            L0[queue].cos));
                        LOG_CLI((BSL_META_U(unit,
                            "%-8d    \n"),
                            queue_num));
                    }
                }
            }
        } else {
            for (queue = 0; queue < SOC_TH3_NUM_GP_QUEUES; queue++) {
                /* Max number of device ports in TH3 is 160*/
                /* coverity[overrun-local] */
                if (th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[queue].
                        parent == NULL ||
                    th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[queue].
                    parent->parent == NULL) {
                    LOG_CLI((BSL_META_U(unit,
                             "%-8d    unknown    unknown    unknown    %d\n"),
                             port, queue));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                        "%-8d    "), port));
                    LOG_CLI((BSL_META_U(unit,
                        "%-8d    "),
                        th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[queue].
                        parent->parent->hw_index));
                    LOG_CLI((BSL_META_U(unit,
                        "%-8d    "),
                        th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[queue].
                        parent->parent->cos));
                    LOG_CLI((BSL_META_U(unit,
                        "%-8d    "),
                        th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[queue].
                        parent->hw_index));
                    LOG_CLI((BSL_META_U(unit,
                        "%-8d    \n"), queue));
                }
            }
        }
    } else {
        LOG_CLI(("Invalid port: %d \n", port));
        return SOC_E_PORT;
    }
    return SOC_E_NONE;
}


/*
 *Function: soc_tomahawk3_scheduler_init
 *Purpose: Wrapper for init code
 */
int
soc_tomahawk3_scheduler_init(int unit)
{
    int mode = 2;
    uint32 rval;
    int pipe, voq_fairness_en = 1;
    uint32 pipe_map, itm_map;
    soc_info_t *si = &SOC_INFO(unit);
    int itm, num_valid_itms = 0;
    /* int port, cosq_idx; */

    mode = soc_property_get(unit, spn_MMU_PORT_NUM_MC_QUEUE, _TH3_MC_Q_MODE_DEFAULT);
    SOC_IF_ERROR_RETURN
        (soc_tomahawk3_mcq_mode_set(unit, mode));
    SOC_IF_ERROR_RETURN
        (soc_cosq_struct_init(unit));
    SOC_IF_ERROR_RETURN
        (soc_cosq_hierarchy_init(unit));

    itm_map = si->itm_map;
    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        if (itm_map & (1 << itm)) {
            num_valid_itms++;
        }
    }
    if (num_valid_itms < NUM_ITM(unit)) {
        voq_fairness_en = 0;
    }

    soc_tomahawk3_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        if (pipe_map & (1 << pipe)) {
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, MMU_QSCH_SPECIAL_CONFIGr,
                        pipe, 0, &rval));
            soc_reg_field_set(unit, MMU_QSCH_SPECIAL_CONFIGr, &rval,
                    ENABLE_THDO_INFOf, voq_fairness_en);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, MMU_QSCH_SPECIAL_CONFIGr,
                        pipe, 0, rval));
        }
    }

    return SOC_E_NONE;
}

/*Function: soc_tomahawk3_idb_init_update
 *Purpose: IDB init sequence and threhsold configuration
 *Param:
 *  unit: Device unit number
 */
int
soc_tomahawk3_idb_init_update(int unit)
{
    soc_port_schedule_state_t *port_schedule_state;
    int rv = SOC_E_NONE;

    port_schedule_state = sal_alloc(sizeof(soc_port_schedule_state_t),
                                    "Tomahawk3 soc_port_schedule_state_t");
    if (port_schedule_state == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(port_schedule_state, 0, sizeof(soc_port_schedule_state_t));

    rv = soc_tomahawk3_port_schedule_state_basic_setup(unit,
        port_schedule_state);
    if (rv != SOC_E_NONE) {
        goto exit;
    }

    SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_init(unit, port_schedule_state));

exit:
    sal_free(port_schedule_state);
    return rv;
}

/*Function: soc_tomahawk3_cdport_init
 *Purpose: CDPORT init sequence for pm8x50 ports
 *Param:
 *  unit: Device unit number
 */
int
soc_tomahawk3_cdport_init (int unit)
{
    soc_port_schedule_state_t *port_schedule_state;
    int pipe, pm, pmnum;
    soc_info_t *si = &SOC_INFO(unit);
    int rv;

    port_schedule_state = sal_alloc(sizeof(soc_port_schedule_state_t),
                                    "Tomahawk3 soc_port_schedule_state_t");
    if (port_schedule_state == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(port_schedule_state, 0, sizeof(soc_port_schedule_state_t));

    rv = soc_tomahawk3_port_schedule_state_basic_setup(unit,
            port_schedule_state);
    if (rv != SOC_E_NONE) {
        sal_free(port_schedule_state);
        return rv;
    }

    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        if (!SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
            for (pm = 0; pm < _TH3_PBLKS_PER_PIPE; pm++) {
                pmnum = pipe * _TH3_PBLKS_PER_PIPE + pm;
                /*Set CDPORT_MAC_MODE based on port speed*/
                rv = soc_tomahawk3_set_cdmac_port_mode(
                        unit, &port_schedule_state->out_port_map, pmnum);
                if (rv != SOC_E_NONE) {
                    sal_free(port_schedule_state);
                    return rv;
                }
            }
        }
    }

    sal_free(port_schedule_state);
    return rv;
}

/*Function:
 *  soc_tomahawk3_ep_top_init
 *Purpose:
 *  EP to Dev port map
 *  Initialize cell credits from MMU tpo EDB based on port speed
 *  Set CT class init
 *  EDB start xmit count
 *  EGR_ENABLE.PRT_ENABLE is set
 *Params:
 * unit: Chip unit number
 */
int
soc_tomahawk3_ep_top_init(int unit)
{
    soc_port_schedule_state_t *port_schedule_state;
    int rv;

    port_schedule_state = sal_alloc(sizeof(soc_port_schedule_state_t),
                                    "Tomahawk3 soc_port_schedule_state_t");
    if (port_schedule_state == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(port_schedule_state, 0, sizeof(soc_port_schedule_state_t));

    rv = soc_tomahawk3_port_schedule_state_basic_setup(unit,
                                                       port_schedule_state);
    if (rv != SOC_E_NONE) {
        sal_free(port_schedule_state);
        return rv;
    }

    rv = soc_tomahawk3_ep_init(unit, port_schedule_state);

    sal_free(port_schedule_state);
    return rv;
}

/*Function:
 *  soc_tomahawk3_slot_pipeline_config
 *Purpose:
 *  Configure slot pipieline latency in EP based on chip latency mode
 *  and Core to DPP clock frequency ratio
 *Params:
 * unit: Chip unit number
 */
int
soc_tomahawk3_slot_pipeline_config(int unit)
{
    soc_port_schedule_state_t *port_schedule_state;
    int rv;
    port_schedule_state = sal_alloc(sizeof(soc_port_schedule_state_t),
                                    "Tomahawk3 soc_port_schedule_state_t");
    if (port_schedule_state == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(port_schedule_state, 0, sizeof(soc_port_schedule_state_t));

    rv = soc_tomahawk3_port_schedule_state_basic_setup(unit,
                                                       port_schedule_state);
    if (rv != SOC_E_NONE) {
        sal_free(port_schedule_state);
        return rv;
    }
    /*Epost slot pipeline config*/
    rv = soc_tomahawk3_ep_set_slot_pipeline_config(unit, port_schedule_state);
    if (rv != SOC_E_NONE) {
        sal_free(port_schedule_state);
        return rv;
    }
    /*Ipost slot pipeline config*/
    rv = soc_tomahawk3_ip_slot_pipeline_config(unit, port_schedule_state);

    sal_free(port_schedule_state);
    return rv;
}

/*Function:
 *  soc_tomahawk3_enable_ingress_forwarding
 *Purpose:
 *  Enable traffic forwarding to port in IP
 *Params:
 * unit: Chip unit number
 */
int
soc_tomahawk3_enable_ingress_forwarding (int unit)
{
    soc_port_schedule_state_t *port_schedule_state;
    int rv;
        port_schedule_state = sal_alloc(sizeof(soc_port_schedule_state_t),
                                    "Tomahawk3 soc_port_schedule_state_t");
    if (port_schedule_state == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(port_schedule_state, 0, sizeof(soc_port_schedule_state_t));

    rv = soc_tomahawk3_port_schedule_state_basic_setup(unit,
                                                       port_schedule_state);
    if (rv != SOC_E_NONE) {
        sal_free(port_schedule_state);
        return rv;
    }

    rv = soc_tomahawk3_en_epc_link_bmap(unit, port_schedule_state);
    if (rv != SOC_E_NONE) {
        sal_free(port_schedule_state);
        return rv;
    }
    rv = soc_tomahawk3_en_dest_port_bmap(unit, port_schedule_state);

    sal_free(port_schedule_state);
    return rv;
}

/*
 * Function: _soc_tomahawk3_per_port_speed_encode_program
 * Purpose: Initialize cell pointer memory in MMU during init
 * Param: unit: device number
 *      : port: Global device port number
 *      : port_speed: Port speed in Mbps
 */
STATIC int
_soc_tomahawk3_per_port_speed_encode_program(int unit, int port,
        int port_speed)
{
    struct {
        soc_reg_t ref_reg;
        soc_field_t encode_field;
    } rf_list[] = {
        {MMU_CRB_SRC_PORT_CFGr, PORT_SPEEDf},
        {MMU_EBCTM_EPORT_CT_CFGr, DST_PORT_SPEEDf},
        {MMU_SCB_SOURCE_PORT_CFGr, PORT_SPEEDf},
        {MMU_EBPCC_EPORT_CFGr, DST_PORT_SPEEDf},
        {MMU_EBQS_PORT_CFGr, PORT_SPEEDf},
        {INVALIDr, INVALIDf}
    };
    uint32 rval;
    int i, special_speed_encode_val = 0, speed_encode_val = 0, oqs_speed_encoding = 0;
    soc_reg_t oqs_reg = MMU_OQS_AGER_DST_PORT_MAPr;

    /*Retrieve encoding value to be used based on port speed*/
    /* Speed encoding
     *0 - Reserved,
     *1 - 10G,
     *2 - 25G,
     *3 - 40G,
     *4 - 50G,
     *5 - 100G,
     *6 - 200G,
     *7 - 400G
     */
    switch (port_speed) {
        case 400000: speed_encode_val = 7; break;
        case 200000: speed_encode_val = 6; break;
        case 100000: speed_encode_val = 5; break;
        case 50000:  speed_encode_val = 4; break;
        case 40000:  speed_encode_val = 3; break;
        case 25000:  speed_encode_val = 2; break;
        case 10000:  speed_encode_val = 1; break;
        default:     speed_encode_val = 0; break;
    }
    /* SDK-130693: Special configuration needed for ports with
       port speed below 100G in SCB */
    switch (port_speed) {
        case 400000: special_speed_encode_val = 7; break;
        case 200000: special_speed_encode_val = 6; break;
        case 100000: special_speed_encode_val = 5; break;
        case 50000:  special_speed_encode_val = 5; break;
        case 40000:  special_speed_encode_val = 5; break;
        case 25000:  special_speed_encode_val = 5; break;
        case 10000:  special_speed_encode_val = 5; break;
        default:     special_speed_encode_val = 0; break;
    }
    for (i = 0; rf_list[i].ref_reg != INVALIDr; i++) {
        if (!SOC_REG_IS_VALID(unit, rf_list[i].ref_reg)) {
            continue;
        }
        if ((rf_list[i].ref_reg == MMU_EBCTM_EPORT_CT_CFGr) &&
                ((IS_MANAGEMENT_PORT(unit, port)) ||
                 (IS_CPU_PORT(unit, port)) ||
                 (IS_LB_PORT(unit, port)))) {
            /*Register valid only for non-management front panel ports*/
            continue;
        }
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, rf_list[i].ref_reg, port, 0, &rval));
        if (rf_list[i].ref_reg == MMU_SCB_SOURCE_PORT_CFGr) {
            /* SDK-130693: Special configuration needed for ports with
               port speed below 100G in SCB */
            soc_reg_field_set(unit, rf_list[i].ref_reg, &rval,
                        rf_list[i].encode_field, special_speed_encode_val);
        } else {
            soc_reg_field_set(unit, rf_list[i].ref_reg, &rval,
                        rf_list[i].encode_field, speed_encode_val);
        }
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, rf_list[i].ref_reg, port, 0, rval));
    }
    /*Setup port speed in  OQS ager based on 4-speed encoding*/
    /*port_speed encoding - 0:<=50GE, 1:100GE, 2:200GE, 3:400GE*/
    if (port_speed <= 50000) {
        oqs_speed_encoding = 0;
    } else if (port_speed == 100000) {
        oqs_speed_encoding = 1;
    } else if (port_speed == 200000) {
        oqs_speed_encoding = 2;
    } else if (port_speed == 400000) {
        oqs_speed_encoding = 3;
    } else {
        return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, oqs_reg, port, 0, &rval));
    soc_reg_field_set(unit, oqs_reg, &rval, PORT_SPEEDf, oqs_speed_encoding);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, oqs_reg, port, 0, rval));
    return SOC_E_NONE;
}

/*
 * Function: soc_tomahawk3_tm_port_speed_encoding
 * Purpose: Setup port speed encoding in different MMU stages
 * Param: unit: device number
 */
int
soc_tomahawk3_tm_port_speed_encoding(int unit)
{
    int port, speed;
    soc_info_t *si = &SOC_INFO(unit);
    int speed_encode;
    /* recommended values of ager_limit based on speed encoding*/
    int ager_limit[4] = {4, 4, 3, 2};
    soc_reg_t reg;
    soc_field_t field;
    int index, cpu_ager_limit;
    uint32 rval = 0;

    /*Front panel ports*/
    PBMP_PORT_ITER(unit, port) {
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            continue;
        }
        speed = si->port_speed_max[port];
        SOC_IF_ERROR_RETURN
        (_soc_tomahawk3_per_port_speed_encode_program(unit, port, speed));
    }
    /*LB ports*/
#define _TH3_SPL_PORT_SPEED 10000
    PBMP_LB_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
        (_soc_tomahawk3_per_port_speed_encode_program(unit, port,
                                                      _TH3_SPL_PORT_SPEED));
    }
    /*CPU port*/
#define _TH3_CPU_PORT_NUM 0
    SOC_IF_ERROR_RETURN
    (_soc_tomahawk3_per_port_speed_encode_program(unit, _TH3_CPU_PORT_NUM,
                                                  _TH3_SPL_PORT_SPEED));
#undef _TH3_CPU_PORT_NUM
#undef _TH3_SPL_PORT_SPEED

    /* Configure OQS ager limit based on port speed encoding. */
    reg = MMU_OQS_AGER_LIMITr;
    field = AGER_LIMITf;
    /* OQS port_speed encoding - 0:<=50GE, 1:100GE, 2:200GE, 3:400GE*/
    for (speed_encode = 0; speed_encode < 4; speed_encode++) {
        /* Program same ager limit for 2 indices mapped to the same speed encode */
        /* indexed by {port_speed[1:0], HP_MMUQ[0]} */
        for (index = (speed_encode * 2); index < ((speed_encode + 1) * 2);
                index++) {
            rval = 0;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, index, &rval));
            soc_reg_field_set(unit, reg, &rval, field, ager_limit[speed_encode]);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, index, rval));
        }
    }
    /* Ager limit config for CPU port */
    index = 8;
    rval = 0;
    cpu_ager_limit = 4;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, index, &rval));
    soc_reg_field_set(unit, reg, &rval, field, cpu_ager_limit);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, index, rval));

    return SOC_E_NONE;
}

/*
 * Function to Set Parent(L0) for a given L1 queue of the CPU port
 * Note: Only child @ L1 level is supported.
 * Because only L1 level queues can be attached to different parent (L0.0-L0.9)
 * of the CPU port.
 */
int
soc_th3_cosq_cpu_parent_set(int unit, int child_index, int child_level,
                           int parent_index)
{
    if (child_level != SOC_TH3_NODE_LVL_L1) {
        /* Only child at L1 are allowed to move */
        return SOC_E_PARAM;
    }
    soc_tomahawk3_cpu_l1_to_l0_profile_set(unit, parent_index, child_index);

    return SOC_E_NONE;
}

int
soc_th3_sched_weight_set(int unit, int port, int level,
                        int index, int weight, int mc)
{
    soc_info_t *si;
    soc_mem_t mem = INVALIDm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int pipe, local_mmu_port;

    si = &SOC_INFO(unit);

    pipe = si->port_pipe[port];
    local_mmu_port = SOC_TH3_MMU_LOCAL_PORT(unit, port);

    if (level == SOC_TH3_NODE_LVL_L0) {
        mem = MMU_QSCH_L0_WEIGHT_MEMm;
        if (IS_CPU_PORT(unit, port)) {
            index = TH3_MMU_CPU_MCQ_OFFSET + index;
        } else if (IS_LB_PORT(unit, port)) {
            index = TH3_MMU_LB_MCQ_OFFSET + index;
        } else if (SOC_PBMP_MEMBER(si->management_pbm, port)) {
            index = TH3_MMU_MGMT_Q_OFFSET + index;
        } else {
            index = (local_mmu_port  * SOC_TH3_NUM_GP_QUEUES) + index;
        }
    } else if (level == SOC_TH3_NODE_LVL_L1) {
        mem = MMU_QSCH_L1_WEIGHT_MEMm;

        if (IS_CPU_PORT(unit, port)) {
            index = TH3_MMU_CPU_MCQ_OFFSET + index;
        } else if (IS_LB_PORT(unit, port)) {
            index = TH3_MMU_LB_MCQ_OFFSET + index;
        } else if (SOC_PBMP_MEMBER(si->management_pbm, port)) {
            index = TH3_MMU_MGMT_Q_OFFSET + index;
        } else {
            index = (local_mmu_port  * SOC_TH3_NUM_GP_QUEUES) + index;
        }
    } else {
        return SOC_E_PARAM;
    }

    mem = SOC_MEM_UNIQUE_ACC(unit, mem)[pipe];

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
    soc_mem_field32_set(unit, mem, &entry, WEIGHTf, weight);
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry));

    return SOC_E_NONE;
}

/*
 * Function to Get Parent(L0) for a given L1 queue of the CPU port
 * Note: Only child @ L1 level is supported.
 * Because only L1 level queues can be attached to different parent (L0.0-L0.9)
 * of the CPU port.
 */
int
soc_th3_cosq_cpu_parent_get(int unit, int child_index, int child_level,
                           int *parent_index)
{
    soc_reg_t reg = MMU_QSCH_CPU_L1_TO_L0_MAPPINGr;
    uint32 rval = 0;

    if (child_level != SOC_TH3_NODE_LVL_L1) {
        /* Only child at L1 are allowed to move */
        return SOC_E_PARAM;
    }
    /* Set the L1 queue's parent to the given L0 node */
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, 0, child_index, &rval));
    *parent_index = soc_reg_field_get(unit, reg, rval, SELECTf);

    return SOC_E_NONE;
}

/*
 * Function to Get Parent(L0) for a given L1 queue of any FP port
 * Note: Only child @ L1 level is supported.
 * Because only L1 level queues can be attached to different parent (L0.0-L0.11)
 * of the FP port.
 */
int
soc_th3_cosq_parent_get(int unit, int port, int child_index, int child_level,
                           int *parent_index)
{
    soc_reg_t reg = MMU_QSCH_PORT_CONFIGr;
    uint64 rval_64;
    uint32 mapping = 0;
    soc_reg_t prof_reg = MMU_MTRO_MMUQ_SCHQ_CFGr;
    soc_reg_t map_reg = MMU_MTRO_MMUQ_SCHQ_PROFILEr;
    int port_profile = 0;
    uint32 rval;
    soc_field_t schedq_field[12] = {
        SCH_Q_NUM_0f, SCH_Q_NUM_1f,
        SCH_Q_NUM_2f, SCH_Q_NUM_3f,
        SCH_Q_NUM_4f, SCH_Q_NUM_5f,
        SCH_Q_NUM_6f, SCH_Q_NUM_7f,
        SCH_Q_NUM_8f, SCH_Q_NUM_9f,
        SCH_Q_NUM_10f, SCH_Q_NUM_11f,
    };


    if (child_level == SOC_TH3_NODE_LVL_L1) {
        /* Set the L1 queue's parent to the given L0 node */
        SOC_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg, 0, port, &rval_64));
        mapping = soc_reg64_field32_get(unit, reg, rval_64, L1_TO_L0_MAPPINGf);

        if ((mapping & (1 << child_index)) == 0) {
            *parent_index = child_index;
        } else {
            *parent_index = child_index+1;
        }
    } else if (child_level == SOC_TH3_NODE_LVL_L2) {
        /* Retrieve schedq mapped to MMUQ*/
        rval = 0;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, prof_reg, port, 0, &rval));
        port_profile = soc_reg_field_get(unit, prof_reg, rval, Q_PROFILE_SELf);
        SOC_IF_ERROR_RETURN
            (soc_reg64_get(unit, map_reg, REG_PORT_ANY, port_profile, &rval_64));
        *parent_index = soc_reg64_field32_get(unit, map_reg, rval_64,
                schedq_field[child_index]);

    } else {
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

int
soc_th3_sched_weight_get(int unit, int port, int level,
                        int index, int *weight, int mc)
{
    soc_info_t *si;
    soc_mem_t mem = INVALIDm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int pipe, local_mmu_port;

    si = &SOC_INFO(unit);

    pipe = si->port_pipe[port];
    local_mmu_port = SOC_TH3_MMU_LOCAL_PORT(unit, port);

    if (level == SOC_TH3_NODE_LVL_L0) {
        mem = MMU_QSCH_L0_WEIGHT_MEMm;
    } else if (level == SOC_TH3_NODE_LVL_L1) {
        mem = MMU_QSCH_L1_WEIGHT_MEMm;
    } else {
        return SOC_E_PARAM;
    }

    if (IS_CPU_PORT(unit, port)) {
        index = TH3_MMU_CPU_MCQ_OFFSET + index;
    } else if (IS_LB_PORT(unit, port)) {
        index = TH3_MMU_LB_MCQ_OFFSET + index;
    } else if (SOC_PBMP_MEMBER(si->management_pbm, port)) {
        index = TH3_MMU_MGMT_Q_OFFSET + index;
    } else {
        index = (local_mmu_port * SOC_TH3_NUM_GP_QUEUES) + index;
    }

    mem = SOC_MEM_UNIQUE_ACC(unit, mem)[pipe];
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
    *weight = soc_mem_field32_get(unit, mem, &entry, WEIGHTf);

    return SOC_E_NONE;
}

int
soc_th3_cosq_sched_mode_get(int unit, soc_port_t port, int level, int index,
                           soc_th3_sched_mode_e* mode, int* weight, int mc)
{
    soc_reg_t reg = INVALIDr;
    uint32 wrr_mask;
    uint64 rval;
    soc_field_t reg_field = INVALIDf;
    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                 "START: soc_th3_cosq_sched_mode_get Port:%d Level:%d index=%d\n"),
                 port, level, index));

    /* selection between SP and WxRR is based on weight property. */
    SOC_IF_ERROR_RETURN(soc_th3_sched_weight_get(unit, port,
                                                level, index, weight, mc));

    if (IS_CPU_PORT(unit, port)) {
        reg = MMU_QSCH_CPU_PORT_CONFIGr;
    } else {
        reg = MMU_QSCH_PORT_CONFIGr;
    }
    if (level == SOC_TH3_NODE_LVL_L1) {
        if (IS_CPU_PORT(unit, port)) {
            reg_field = SP_L1f;
        } else {
            reg_field = SP_L0_PARENTf;
        }
        if (IS_CPU_PORT(unit, port)) {
            int parent_index = 0;
             SOC_IF_ERROR_RETURN
                (soc_th3_cosq_cpu_parent_get(unit, index, SOC_TH3_NODE_LVL_L1,
                                            &parent_index));
            index = parent_index;
        } else {
            int parent_index = 0;
             SOC_IF_ERROR_RETURN
                (soc_th3_cosq_parent_get(unit, port, index, SOC_TH3_NODE_LVL_L1,
                                            &parent_index));
            index = parent_index;
        }
    } else if (level == SOC_TH3_NODE_LVL_L0) {
        if (IS_CPU_PORT(unit, port)) {
            reg_field = SP_L0f;
        } else {
            reg_field = SP_L0_CHILDf;
        }
    } else {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, port, 0, &rval));
    wrr_mask = soc_reg64_field32_get(unit, reg, rval, reg_field);
    if (wrr_mask & (1 << index)) {
        *mode = SOC_TH3_SCHED_MODE_STRICT;
    } else {
        wrr_mask = soc_reg64_field32_get(unit, reg, rval, ENABLE_WRRf);
        if (wrr_mask) {
            *mode = SOC_TH3_SCHED_MODE_WRR;
        } else {
            *mode = SOC_TH3_SCHED_MODE_WERR;
        }

    }
    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                 "END: soc_th3_cosq_sched_mode_get Port:%d Level:%d index=%d MODE=%d WT=%d\n"),
                 port, level, index, *mode, *weight));

    return SOC_E_NONE;
}

int
soc_th3_cosq_sched_mode_set(int unit, soc_port_t port, int level, int index,
                           soc_th3_sched_mode_e mode, int weight, int mc)
{
    soc_reg_t reg = INVALIDr;
    uint32 wrr_mask;
    uint64 rval;
    soc_field_t reg_field = INVALIDf;

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                 "START: soc_th3_cosq_sched_mode_set Port:%d Level:%d index=%d MODE=%d WT=%d\n"),
                 port, level, index, mode, weight));

    /* selection between SP and WxRR is based on weight property. */
    SOC_IF_ERROR_RETURN(soc_th3_sched_weight_set(unit, port,
                                                level, index, weight, mc));

    if (IS_CPU_PORT(unit, port)) {
        reg = MMU_QSCH_CPU_PORT_CONFIGr;
    } else {
        reg = MMU_QSCH_PORT_CONFIGr;
    }

    if (level == SOC_TH3_NODE_LVL_L0) {
        if (IS_CPU_PORT(unit, port)) {
            reg_field = SP_L0f;
        } else {
            reg_field = SP_L0_CHILDf;
        }
        /* index = 0; */
    } else if (level == SOC_TH3_NODE_LVL_L1) {
        if (IS_CPU_PORT(unit, port)) {
            reg_field = SP_L1f;
        } else {
            reg_field = SP_L0_PARENTf;
        }
        if (IS_CPU_PORT(unit, port)) {
            int parent_index = 0;
            reg = MMU_QSCH_CPU_PORT_CONFIGr;
            SOC_IF_ERROR_RETURN
                (soc_th3_cosq_cpu_parent_get(unit, index, SOC_TH3_NODE_LVL_L1,
                                            &parent_index));
            index = parent_index;
        } else {
            int parent_index = 0;
            reg = MMU_QSCH_PORT_CONFIGr;
            SOC_IF_ERROR_RETURN
                (soc_th3_cosq_parent_get(unit, port, index, SOC_TH3_NODE_LVL_L1,
                                            &parent_index));
            index = parent_index;
        }
    } else {
        return SOC_E_PARAM;
    }

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                 "END: soc_th3_cosq_sched_mode_set Port:%d Level:%d index=%d MODE=%d WT=%d\n"),
                 port, level, index, mode, weight));

    SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, port, 0, &rval));
    if (mode == SOC_TH3_SCHED_MODE_WRR) {
        soc_reg64_field32_set(unit, reg, &rval, ENABLE_WRRf, 1);
        wrr_mask = soc_reg64_field32_get(unit, reg, rval, reg_field);
        wrr_mask &= ~(1 << index);
        soc_reg64_field32_set(unit, reg, &rval, reg_field, wrr_mask);
    } else if (mode == SOC_TH3_SCHED_MODE_WERR) {
        soc_reg64_field32_set(unit, reg, &rval, ENABLE_WRRf, 0);
        wrr_mask = soc_reg64_field32_get(unit, reg, rval, reg_field);
        wrr_mask &= ~(1 << index);
        soc_reg64_field32_set(unit, reg, &rval, reg_field, wrr_mask);
    } else {
        wrr_mask = soc_reg64_field32_get(unit, reg, rval, reg_field);
        wrr_mask |= (1 << index);
        soc_reg64_field32_set(unit, reg, &rval, reg_field, wrr_mask);
    }

    SOC_IF_ERROR_RETURN(soc_reg64_set(unit, reg, port, 0, rval));

    /* WAR: set port 32 besides CPU reg,
       as CPU WRED takes some info from port 32.
    if (reg2 != INVALIDr) {
        SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg2, port, 0, &rval));
        wrr_mask = soc_reg_field_get(unit, reg2, rval, ENABLE_WRRf);
        wrr_mask &= ~(1 << index);
        wrr_mask |= (fval << index);
        soc_reg64_field32_set(unit, reg2, &rval, ENABLE_WRRf, wrr_mask);
        SOC_IF_ERROR_RETURN(soc_reg64_set(unit, reg2, port, 0, rval));
    }
       Note: Do not need this workaround in TH3? */

    return SOC_E_NONE;
}


/*
 *Function: soc_tomahawk3_scb_init
 *Purpose: Wrapper for init code
 */
int
soc_tomahawk3_scb_init(int unit)
{
    soc_reg_t reg;
    uint32 rval;
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* SDK-130693: Special configuration needed for ports with
       port speed below 100G in SCB */
    reg = MMU_SCB_ARBITER_AGER_CFGr;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, AGER_LIMIT_50f, 0);
    SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    if (rev_id != BCM56980_A0_REV_ID) {
        reg = MMU_SCB_SRAF_FIFO_THRESHr;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, THRESHf, 12);
        SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }
    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_wred_init
 *Purpose: Wrapper for init code
 */
int
soc_tomahawk3_wred_init(int unit)
{
    soc_reg_t reg;
    uint64 rval;

    /* initiate Queue length update from both THDO simultaneously */
    reg = MMU_WRED_CONFIG_READYr;
    SOC_IF_ERROR_RETURN
        (soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg64_field32_set(unit, reg, &rval, WRED_READYf, 1);
    SOC_IF_ERROR_RETURN
            (soc_reg64_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

/*
 *Function: soc_tomahawk3_rl_init
 *Purpose: Wrapper for init code
 */
int
soc_tomahawk3_rl_init(int unit)
{
    uint32 rval;

    /* SDK-141834 SWAR to change MMU_RL_CONFIG value */
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, MMU_RL_CONFIGr, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, MMU_RL_CONFIGr, &rval, ITREORDER_LIMITf, 511);
    SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, MMU_RL_CONFIGr, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

/*
 * Function: soc_tomahawk3_get_cos_for_mmu_queue
 * Purpose: Helper function that returns the cos value for an mmuq
*/

int soc_tomahawk3_get_cos_for_mmu_queue(int unit, int profile, int mmuq)
{
    int queue_num;
    int cos = -1;

    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        if (th3_sched_profile_info[unit][profile][queue_num].mmuq[0] == mmuq) {
            cos = th3_sched_profile_info[unit][profile][queue_num].cos;
            break;
        }
        if (th3_sched_profile_info[unit][profile][queue_num].mmuq[1] == mmuq) {
            cos = th3_sched_profile_info[unit][profile][queue_num].cos;
            break;
        }
    }
    return cos;

}

/*
 * Function: soc_tomahawk3_get_fc_only_uc_for_cos
 * Purpose:
 *   Helper function that returns the fc_only_uc for a given {profile, cos}
*/

int
soc_tomahawk3_get_fc_only_uc_for_cos(int unit, int profile, int cos)
{
    int queue_num;
    int fc_only_uc = 0;

    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        if (th3_sched_profile_info[unit][profile][queue_num].cos == cos) {
            fc_only_uc =
                th3_sched_profile_info[unit][profile][queue_num].fc_is_uc_only;
            break;
        }
    }
    return fc_only_uc;

}

/*
 * Function: soc_tomahawk3_get_sp_for_cos
 * Purpose:
 *   Helper function that returns the strict priority for a given {profile, cos}
*/

int
soc_tomahawk3_get_sp_for_cos(int unit, int profile, int cos)
{
    int queue_num;
    int sp = 0;

    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        if (th3_sched_profile_info[unit][profile][queue_num].cos == cos) {
            sp =
                th3_sched_profile_info[unit][profile][queue_num].strict_priority;
            break;
        }
    }
    return sp;

}

int soc_tomahawk3_profile_exists(int unit, int profile)
{
    int profile_exists = 0;
    int queue_num;

    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        if (th3_sched_profile_info[unit][profile][queue_num].cos != -1) {
            profile_exists = 1;
            break;
        }
    }
    return profile_exists;
}

int
soc_tomahawk3_get_sched_profile_max_cos(int unit, int profile) {
    int max_cos = 0;
    int queue_num, cos;

    for (queue_num = 0; queue_num < SOC_TH3_NUM_GP_QUEUES; queue_num++) {
        cos = th3_sched_profile_info[unit][profile][queue_num].cos;
        if (cos > max_cos) {
            max_cos = cos;
        }
    }
    return max_cos;
}

STATIC int
soc_transmit_rqe_flush_packet (int unit, int rqe_num) {
    bcm_pkt_t *pkt = NULL;
    uint8 *payload;
    int rv = SOC_E_NONE;
    bcm_port_t dest_port = _TH3_MMU_SPARE_DST_PORT; /* Use spare port as destination */

    /* Create test packet */
    pkt = soc_cm_salloc(unit, sizeof(bcm_pkt_t), "pkt buffer");
    if (pkt == NULL) {
        return SOC_E_MEMORY;
    }
    payload = soc_cm_salloc(unit, 68,
                            "pkt alloc data");
    if (payload == NULL) {
        soc_cm_sfree(unit, pkt);
        return BCM_E_FAIL;
    }
    sal_memset(payload, 1, 68);
    BCM_PKT_ONE_BUF_SETUP(pkt, payload, 68);

    pkt->flags = BCM_TX_LINKDOWN_TRANSMIT;
    pkt->flags2 = BCM_PKT_F2_MC_QUEUE;
    pkt->flags |= BCM_PKT_F_ROUTED;
    pkt->flags |= BCM_TX_PRIO_INT;
    pkt->unit = unit;
    pkt->call_back = NULL;
    BCM_PKT_TX_LEN_SET(pkt, 68);
    pkt->vlan = 1;
    pkt->prio_int = rqe_num;
    pkt->cos = 0;
    pkt->rqe_q_num = rqe_num;
    SOC_PBMP_CLEAR(pkt->tx_l3pbmp);
    SOC_PBMP_CLEAR(pkt->tx_pbmp);
    SOC_PBMP_PORT_ADD(pkt->tx_l3pbmp, dest_port);
    SOC_PBMP_PORT_ADD(pkt->tx_pbmp, dest_port);
    pkt->dest_port = dest_port;
    pkt->dst_gport = dest_port;

    /* LB HDR fields for SOBMH packet*/
    pkt->flags2 |= BCM_PKT_F2_CPU_TX_PROC;
    pkt->rx_port = _TH3_MMU_SPARE_SRC_PORT; /* Spare port from ITM1*/
    pkt->txprocmh_destination_type = 15;
    pkt->txprocmh_destination = dest_port;
    pkt->txprocmh_qos_fields_valid = 1;

    pkt->pkt_data[0].data[12] = 0x81;
    pkt->pkt_data[0].data[13] = 0x00;
    pkt->pkt_data[0].data[14] = 0x00;
    pkt->pkt_data[0].data[15] = 0x01;

    rv = bcm_common_tx(unit, pkt, NULL);

    if (payload != NULL) {
        soc_cm_sfree(unit, payload);
    }
    if (pkt != NULL) {
        soc_cm_sfree(unit, pkt);
    }

    return rv;
}

int
soc_tomahawk3_cpu_tx_rqe_queues_for_flush (int unit, int itm) {
    int num_rqe_queues;
    int rqe_q, i;
    int num_packets = 7; /*Transmit 7 packets fro each queue*/
    uint16 dev_id;
    uint8 rev_id;
    int rv = SOC_E_NONE;
    uint32 rd_val, rval, wr_val;
    soc_reg_t reg_int_stat = MMU_RQE_INT_STATr;
    soc_reg_t reg_int_en = MMU_RQE_INT_ENr;
    int invld_dest_en = 0;
    int itm_int;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 itm_map = si->itm_map;
    int count = 0;
    bcm_pbmp_t link_bmap;
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS];

    /* Disable RQE_INVALID_DEST interrupt */
    SOC_IF_ERROR_RETURN(
            soc_reg32_rawport_get(unit, reg_int_en, REG_PORT_ANY, 0, &rval));
    invld_dest_en = soc_reg_field_get(unit, reg_int_en, rval, INVALID_DESTf);
    wr_val = 0;
    soc_reg_field_set(unit, reg_int_en, &rval, INVALID_DESTf, wr_val);
    SOC_IF_ERROR_RETURN(
            soc_reg32_rawport_set(unit, reg_int_en, REG_PORT_ANY, 0, rval));

    /* Add Spare ports 158 & 98 to IP's  EPC_LINK_BMAP */
    soc_mem_lock(unit, EPC_LINK_BMAPm);
    rv = soc_mem_read(unit, EPC_LINK_BMAPm, MEM_BLOCK_ANY, 0, entry);
    if (rv != SOC_E_NONE) {
        soc_mem_unlock(unit, EPC_LINK_BMAPm);
        return SOC_E_INTERNAL;
    }
    soc_mem_pbmp_field_get(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
            &link_bmap);
    SOC_PBMP_PORT_ADD(link_bmap, _TH3_MMU_SPARE_DST_PORT);
    SOC_PBMP_PORT_ADD(link_bmap, _TH3_MMU_SPARE_SRC_PORT);
    soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
            &link_bmap);
    rv = soc_mem_write(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0, &entry);
    soc_mem_unlock(unit, EPC_LINK_BMAPm);
    if (rv != SOC_E_NONE) {
        return SOC_E_INTERNAL;
    }

    /* Add Spare ports 158 & 98 to IP's  ING_DEST_PORT_ENABLE */
    soc_mem_lock(unit, ING_DEST_PORT_ENABLEm);
    rv = soc_mem_read(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ANY,
                             0, entry);
    if (rv != SOC_E_NONE) {
        soc_mem_unlock(unit, ING_DEST_PORT_ENABLEm);
        return SOC_E_INTERNAL;
    }
    soc_mem_pbmp_field_get(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf,
            &link_bmap);
    BCM_PBMP_PORT_ADD(link_bmap, _TH3_MMU_SPARE_DST_PORT);
    BCM_PBMP_PORT_ADD(link_bmap, _TH3_MMU_SPARE_SRC_PORT);
    soc_mem_pbmp_field_set(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf,
            &link_bmap);
    rv = soc_mem_write(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ALL, 0,
                &entry);
    soc_mem_unlock(unit, ING_DEST_PORT_ENABLEm);
    if (rv != SOC_E_NONE) {
        return SOC_E_INTERNAL;
    }

    /* Transmit 7 packets to each RQE queue in use */
    soc_cm_get_id(unit, &dev_id, &rev_id);
    if (rev_id == BCM56980_A0_REV_ID) {
        num_rqe_queues = _TH3_A0_MMU_NUM_RQE_QUEUES;
    } else {
        num_rqe_queues = _TH3_MMU_NUM_RQE_QUEUES;
    }
    for (rqe_q = 0; rqe_q < num_rqe_queues; rqe_q++) {
        for (i = 0; i < num_packets; i++) {
            SOC_IF_ERROR_RETURN(
                    soc_transmit_rqe_flush_packet(unit, rqe_q));
        }
    }

    /* Wait for MMU_RQE_INT_STAT.FLUSH_DONE interrupt to be set */
    while (1) {
        SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg_int_stat,
                                                    itm, 0, &rval));
        rd_val = soc_reg_field_get(unit, reg_int_stat,
                                    rval, FLUSH_DONEf);
        if (rd_val == 1) {
            break;
        }
        sal_usleep(1 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
        count++;
        if (count > 30000) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                           "RQE Port Flush : FLUSH_DONE isn't reset even after 30000 "
                           "iterations\n")));
            return SOC_E_FAIL;
        }
    }
    /* Clear RQE INVALID_DEST INT STAT*/
    wr_val = 1; /*w1tc*/
    for (itm_int = 0; itm_int < NUM_ITM(unit); itm_int++) {
        if (itm_map & (1 << itm_int)) {
            rval = 0;
            soc_reg_field_set(unit, reg_int_stat, &rval, INVALID_DESTf, wr_val);
            SOC_IF_ERROR_RETURN(
                    soc_reg32_rawport_set(unit, reg_int_stat, itm_int, 0, rval));
        }
    }

    /* Restore RQE INVALID_DEST INT_EN */
    SOC_IF_ERROR_RETURN(
            soc_reg32_rawport_get(unit, reg_int_en, REG_PORT_ANY, 0, &rval));
    wr_val = invld_dest_en;
    soc_reg_field_set(unit, reg_int_en, &rval, INVALID_DESTf, wr_val);
    SOC_IF_ERROR_RETURN(
            soc_reg32_rawport_set(unit, reg_int_en, REG_PORT_ANY, 0, rval));

    /* Remove Spare ports 158 & 98 from IP's EPC_LINK_BMAP */
    soc_mem_lock(unit, EPC_LINK_BMAPm);
    rv = soc_mem_read(unit, EPC_LINK_BMAPm, MEM_BLOCK_ANY, 0, entry);
    if (rv != SOC_E_NONE) {
        soc_mem_unlock(unit, EPC_LINK_BMAPm);
        return SOC_E_INTERNAL;
    }
    soc_mem_pbmp_field_get(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
            &link_bmap);
    BCM_PBMP_PORT_REMOVE(link_bmap, _TH3_MMU_SPARE_DST_PORT);
    BCM_PBMP_PORT_REMOVE(link_bmap, _TH3_MMU_SPARE_SRC_PORT);
    soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
            &link_bmap);
    rv = soc_mem_write(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0,
                &entry);
    soc_mem_unlock(unit, EPC_LINK_BMAPm);
    if (rv != SOC_E_NONE) {
        return SOC_E_INTERNAL;
    }

    /* Remove Spare ports 158 & 98 from IP's  ING_DEST_PORT_ENABLE */
    soc_mem_lock(unit, ING_DEST_PORT_ENABLEm);
    rv = soc_mem_read(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ANY,
                             0, entry);
    if (rv != SOC_E_NONE) {
        soc_mem_unlock(unit, ING_DEST_PORT_ENABLEm);
        return SOC_E_INTERNAL;
    }
    soc_mem_pbmp_field_get(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf,
            &link_bmap);
    BCM_PBMP_PORT_REMOVE(link_bmap, _TH3_MMU_SPARE_DST_PORT);
    BCM_PBMP_PORT_REMOVE(link_bmap, _TH3_MMU_SPARE_SRC_PORT);
    soc_mem_pbmp_field_set(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf,
            &link_bmap);
    rv = soc_mem_write(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ALL, 0,
                &entry);
    soc_mem_unlock(unit, ING_DEST_PORT_ENABLEm);
    if (rv != SOC_E_NONE) {
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

int
soc_th3_mmu_enable_non_func_intr(int unit) {
    int i;
    soc_reg_t loop_reg;
    soc_field_t loop_field;
    uint32 rval;
    struct {
        soc_reg_t int_reg;
        soc_field_t int_en_field;
    } int_rf_list[] = {
        {MMU_MTRO_CPU_INT_ENr, CNT_OVERFLOWf},
        {MMU_THDI_CPU_INT_ENr, POOL_COUNTER_OVERFLOWf},
        {MMU_THDI_CPU_INT_ENr, POOL_COUNTER_UNDERFLOWf},
        {MMU_THDO_CPU_INT_ENr, COUNTER_UNDERFLOWf},
        {MMU_THDO_CPU_INT_ENr, SPID_MISMATCHf},
        {MMU_THDR_QE_CPU_INT_ENr, CNT_OVERFLOWf},
        {MMU_THDR_QE_CPU_INT_ENr, CNT_UNDERFLOWf},
        {MMU_EBCTM_CPU_INT_ENr, OVERFLOW_INTR_ENf},
        {MMU_EBCTM_CPU_INT_ENr, UNDERFLOW_INTR_ENf},
        {MMU_EBCFP_CPU_INT_ENr, EB_MISCONFIGUREDf},
        {MMU_EBCFP_CPU_INT_ENr, MTU_VIOLATIONf},
        {MMU_CRB_CPU_INT_ENr, MTU_VIOLATION_INTR_ENf},
        {MMU_CRB_CPU_INT_ENr, INV_SRC_PORT_INTR_ENf},
        {MMU_CRB_CPU_INT_ENr, INV_DST_PORT_INTR_ENf},
        {MMU_RQE_INT_ENr, INVALID_DESTf},
        {MMU_RQE_INT_ENr, CCP_OVERFLOWf},
        {MMU_RQE_INT_ENr, L3_PURGEf},
        {INVALIDr, INVALIDf}
    };
    soc_reg_t itm_int_reg = MMU_ITMCFG_CPU_INT_ENr;
    soc_reg_t eb_int_reg = MMU_EBCFG_CPU_INT_ENr;
    soc_field_t itm_intr_fields[6] = {THDR_INT_ENf, THDO_INT_ENf, THDI_INT_ENf,
                                     RQE_INT_ENf, CRB_INT_ENf, CFAP_INT_ENf};
    soc_field_t eb_intr_fields[3] = {MTRO_INT_ENf, EBCTM_INT_ENf, EBCFP_INT_ENf};

    /* Set Leaf block level interrupt enables*/
    for (i = 0; int_rf_list[i].int_reg != INVALIDr; i++) {
        if (!SOC_REG_IS_VALID(unit, int_rf_list[i].int_reg)) {
            continue;
        }
        loop_reg = int_rf_list[i].int_reg;
        loop_field = int_rf_list[i].int_en_field;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, loop_reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, loop_reg, &rval, loop_field, 1);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, loop_reg, REG_PORT_ANY, 0, rval));
    }

    /* Set ITM level interrupt enables */
    for (i = 0; i < (sizeof(itm_intr_fields)) / (sizeof(soc_field_t)); i++) {
        rval = 0;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, itm_int_reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, itm_int_reg, &rval, itm_intr_fields[i], 1);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, itm_int_reg, REG_PORT_ANY, 0, rval));
    }
    /* Set EB level interrupt enables */
    for (i = 0; i < (sizeof(eb_intr_fields)) / (sizeof(soc_field_t)); i++) {
        rval = 0;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, eb_int_reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, eb_int_reg, &rval, eb_intr_fields[i], 1);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, eb_int_reg, REG_PORT_ANY, 0, rval));
    }
    /* All GLB interrupts required are enabled as part of SER init */
    return SOC_E_NONE;
}

STATIC int
soc_th3_clear_itm_int_stat (int unit, int itm, soc_field_t itm_intr_field)
{
    uint32 rval = 0;
    soc_reg_t itm_intr_reg = MMU_ITMCFG_CPU_INT_STATr;
    soc_reg_t glb_intr_reg = MMU_GLBCFG_CPU_INT_STATr;
    soc_field_t glb_fields[] = {ITMCFG0_INT_STATf, ITMCFG1_INT_STATf};
    int wr_val = 1; /* w1tc */

    /*Clear ITM level interrupt*/
    rval = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, itm_intr_reg, itm, 0, &rval));
    soc_reg_field_set(unit, itm_intr_reg, &rval, itm_intr_field, wr_val);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, itm_intr_reg, itm, 0, rval));
    /*Clear GLB level interrupt*/
    rval = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, glb_intr_reg, itm, 0, &rval));
    soc_reg_field_set(unit, glb_intr_reg, &rval, glb_fields[itm], wr_val);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, glb_intr_reg, itm, 0, rval));

    return SOC_E_NONE;
}

STATIC int
soc_th3_clear_eb_int_stat (int unit, int pipe, soc_field_t eb_intr_field)
{
    uint32 rval = 0;
    soc_reg_t eb_intr_reg = MMU_EBCFG_CPU_INT_STATr;
    soc_reg_t glb_intr_reg = MMU_GLBCFG_CPU_INT_STATr;
    soc_field_t glb_fields[] = {EBCFG0_INT_STATf, EBCFG1_INT_STATf,
        EBCFG2_INT_STATf, EBCFG3_INT_STATf, EBCFG4_INT_STATf, EBCFG5_INT_STATf,
        EBCFG6_INT_STATf, EBCFG7_INT_STATf};
    int wr_val = 1; /* w1tc */

    /*Clear EB level interrupt*/
    rval = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, eb_intr_reg, pipe, 0, &rval));
    soc_reg_field_set(unit, eb_intr_reg, &rval, eb_intr_field, wr_val);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, eb_intr_reg, pipe, 0, rval));
    /*Clear GLB level interrupt*/
    rval = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, glb_intr_reg, pipe, 0, &rval));
    soc_reg_field_set(unit, glb_intr_reg, &rval, glb_fields[pipe], wr_val);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, glb_intr_reg, pipe, 0, rval));

    return SOC_E_NONE;
}



int
soc_th3_process_itm_cfap_int (int unit, int itm, soc_field_t itm_intr_field) {
    soc_reg_t block_intr_reg = MMU_CFAP_INT_STATr;
    uint64 rval64;
    soc_field_t bst_int = BST_TRIG_STATf;
    int wr_val = 1; /* w1tc */

    COMPILER_64_ZERO(rval64);
    SOC_IF_ERROR_RETURN(
            soc_reg64_get(unit, block_intr_reg, itm, 0, &rval64));
    if (soc_reg64_field32_get(unit, block_intr_reg, rval64, bst_int)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- Servicing CFAP"
                                        "BST intr from ITM = %0d \n"),
                             unit, itm));
        /* Clear BST interrupt*/
        soc_reg64_field32_set(unit, block_intr_reg, &rval64, bst_int, wr_val);
        SOC_IF_ERROR_RETURN(
                soc_reg64_set(unit, block_intr_reg, itm, 0, rval64));
        /* Clear ITM and GLB level interrupts */
        SOC_IF_ERROR_RETURN(
            soc_th3_clear_itm_int_stat(unit, itm, itm_intr_field));
        /* BST callback for stats processing */
        SOC_IF_ERROR_RETURN
            (soc_th3_process_mmu_bst(unit, itm, itm_intr_field));
    }
    return SOC_E_NONE;
}

int
soc_th3_process_itm_thdi_int (int unit, int itm, soc_field_t itm_intr_field) {
    soc_reg_t block_intr_reg = MMU_THDI_CPU_INT_STATr;
    uint32 rval = 0, intr_reg_val = 0;
    soc_field_t thdi_intr_fields[] = {POOL_COUNTER_UNDERFLOW_STATf,
        POOL_COUNTER_OVERFLOW_STATf};
    soc_field_t bst_int = BST_TRIG_STATf;
    int wr_val = 1; /* w1tc */
    int i;

    SOC_IF_ERROR_RETURN(
            soc_reg32_get(unit, block_intr_reg, itm, 0, &intr_reg_val));
    if (soc_reg_field_get(unit, block_intr_reg, intr_reg_val, bst_int)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- Servicing THDI"
                                        "BST intr from ITM = %0d \n"),
                             unit, itm));
        /* Clear BST interrupt*/
        SOC_IF_ERROR_RETURN(
                soc_reg32_get(unit, block_intr_reg, itm, 0, &rval));
        soc_reg_field_set(unit, block_intr_reg, &rval, bst_int, wr_val);
        SOC_IF_ERROR_RETURN(
                soc_reg32_set(unit, block_intr_reg, itm, 0, rval));
        /* Clear ITM and GLB level interrupts */
        SOC_IF_ERROR_RETURN(
            soc_th3_clear_itm_int_stat(unit, itm, itm_intr_field));
        /* BST callback for stats processing */
        SOC_IF_ERROR_RETURN
            (soc_th3_process_mmu_bst(unit, itm, itm_intr_field));
    } else {
        for (i = 0; i < (sizeof(thdi_intr_fields) / sizeof(soc_field_t)); i++) {
            SOC_IF_ERROR_RETURN(
                soc_reg32_get(unit, block_intr_reg, itm, 0, &intr_reg_val));
            if (!soc_reg_field_get(unit, block_intr_reg,
                                   intr_reg_val, thdi_intr_fields[i])) {
                continue;
            }
            switch(thdi_intr_fields[i]) {
                case POOL_COUNTER_UNDERFLOWf:
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- THDI Pool Counter"
                                        "Underflow intr set for ITM = %0d \n"),
                             unit, itm));
                    break;
                case POOL_COUNTER_OVERFLOWf:
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- THDI Pool Counter"
                                        "Overflow intr set for ITM = %0d \n"),
                             unit, itm));
                    break;
                default:
                    break;
            }
            /*Clear block(THDI) level interrupt*/
            rval = 0;
            SOC_IF_ERROR_RETURN(
                    soc_reg32_get(unit, block_intr_reg, itm, 0, &rval));
            soc_reg_field_set(unit, block_intr_reg, &rval,
                    thdi_intr_fields[i], wr_val);
            SOC_IF_ERROR_RETURN(
                    soc_reg32_set(unit, block_intr_reg, itm, 0, rval));
        }
        /* Clear ITM and GLB level interrupts */
        SOC_IF_ERROR_RETURN(
            soc_th3_clear_itm_int_stat(unit, itm, itm_intr_field));

    }
    return SOC_E_NONE;
}

int
soc_th3_process_itm_thdo_int (int unit, int itm, soc_field_t itm_intr_field) {
    soc_reg_t block_intr_reg = MMU_THDO_CPU_INT_STATr;
    soc_reg_t thdo_bst_int_reg = MMU_THDO_BST_CPU_INT_STATr;
    uint32 rval = 0;
    soc_field_t thdo_intr_fields[] = {COUNTER_UNDERFLOWf, SPID_MISMATCHf};
    soc_field_t bst_int = BSTf;
    int wr_val = 1; /* w1tc */
    int i;

    SOC_IF_ERROR_RETURN(
            soc_reg32_get(unit, thdo_bst_int_reg, itm, 0, &rval));
    if (soc_reg_field_get(unit, thdo_bst_int_reg, rval, bst_int)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- Servicing THDO"
                                        "BST intr from ITM = %0d \n"),
                             unit, itm));
        /* Clear BST interrupt*/
        soc_reg_field_set(unit, thdo_bst_int_reg, &rval, bst_int, wr_val);
        SOC_IF_ERROR_RETURN(
                soc_reg32_set(unit, thdo_bst_int_reg, itm, 0, rval));
        /* Clear ITM and GLB level interrupts */
        SOC_IF_ERROR_RETURN(
            soc_th3_clear_itm_int_stat(unit, itm, itm_intr_field));
        /* BST callback for stats processing */
        SOC_IF_ERROR_RETURN
            (soc_th3_process_mmu_bst(unit, itm, itm_intr_field));
    } else {
        for (i = 0; i < (sizeof(thdo_intr_fields) / sizeof(soc_field_t)); i++) {
            SOC_IF_ERROR_RETURN(
                soc_reg32_get(unit, block_intr_reg, itm, 0, &rval));
            if (!soc_reg_field_get(unit, block_intr_reg,
                                   rval, thdo_intr_fields[i])) {
                continue;
            }
            switch(thdo_intr_fields[i]) {
                case SPID_MISMATCHf:
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- SPID Mismatch"
                             "intr set for ITM = %0d \n"),
                             unit, itm));
                    break;
                case COUNTER_UNDERFLOWf:
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- THDO Counter"
                             " Underflow intr set for ITM = %0d \n"),
                             unit, itm));

                    break;
                default:
                    break;
            }
            /*Clear block(THDO) level interrupt*/
            rval = 0;
            SOC_IF_ERROR_RETURN(
                    soc_reg32_get(unit, block_intr_reg, itm, 0, &rval));
            soc_reg_field_set(unit, block_intr_reg, &rval,
                    thdo_intr_fields[i], wr_val);
            SOC_IF_ERROR_RETURN(
                    soc_reg32_set(unit, block_intr_reg, itm, 0, rval));
        }
        /* Clear ITM and GLB level interrupts */
        SOC_IF_ERROR_RETURN(
            soc_th3_clear_itm_int_stat(unit, itm, itm_intr_field));
    }

    return SOC_E_NONE;
}

int
soc_th3_process_itm_thdr_int (int unit, int itm, soc_field_t itm_intr_field) {
    soc_reg_t block_intr_reg = MMU_THDR_QE_CPU_INT_STATr;
    uint32 rval = 0;
    soc_field_t thdr_intr_fields[] = {CNT_UNDERFLOWf, CNT_OVERFLOWf};
    soc_field_t bst_int = BSTf;
    int wr_val = 1; /* w1tc */
    int i;

    SOC_IF_ERROR_RETURN(
            soc_reg32_get(unit, block_intr_reg, itm, 0, &rval));
    if (soc_reg_field_get(unit, block_intr_reg, rval, bst_int)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- Servicing THDR QE"
                                        "BST intr from ITM = %0d \n"),
                             unit, itm));
        /* Clear BST interrupt*/
        soc_reg_field_set(unit, block_intr_reg, &rval, bst_int, wr_val);
        SOC_IF_ERROR_RETURN(
                soc_reg32_set(unit, block_intr_reg, itm, 0, rval));
        /* Clear ITM and GLB level interrupts */
        SOC_IF_ERROR_RETURN(
            soc_th3_clear_itm_int_stat(unit, itm, itm_intr_field));
        /* BST callback for stats processing */
        SOC_IF_ERROR_RETURN
            (soc_th3_process_mmu_bst(unit, itm, itm_intr_field));
    } else {
        for (i = 0; i < (sizeof(thdr_intr_fields) / sizeof(soc_field_t)); i++) {
            SOC_IF_ERROR_RETURN(
                soc_reg32_get(unit, block_intr_reg, itm, 0, &rval));
            if (!soc_reg_field_get(unit, block_intr_reg,
                                   rval, thdr_intr_fields[i])) {
                continue;
            }
            switch(thdr_intr_fields[i]) {
                case CNT_OVERFLOWf:
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- THDR QE Counter"
                             " Overflow intr set for ITM = %0d \n"),
                             unit, itm));
                    break;
                case CNT_UNDERFLOWf:
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- THDR QE Counter"
                             " Underflow intr set for ITM = %0d \n"),
                             unit, itm));

                    break;
                default:
                    break;
            }
            /*Clear block(THDR QE) level interrupt*/
            rval = 0;
            SOC_IF_ERROR_RETURN(
                    soc_reg32_get(unit, block_intr_reg, itm, 0, &rval));
            soc_reg_field_set(unit, block_intr_reg, &rval,
                    thdr_intr_fields[i], wr_val);
            SOC_IF_ERROR_RETURN(
                    soc_reg32_set(unit, block_intr_reg, itm, 0, rval));
        }
        /* Clear ITM and GLB level interrupts */
        SOC_IF_ERROR_RETURN(
            soc_th3_clear_itm_int_stat(unit, itm, itm_intr_field));
    }

    return SOC_E_NONE;
}

int
soc_th3_process_itm_rqe_int (int unit, int itm, soc_field_t itm_intr_field) {
    soc_reg_t block_intr_reg = MMU_RQE_INT_STATr;
    uint32 rval = 0;
    soc_field_t thdr_intr_fields[] = {INVALID_DESTf, CCP_OVERFLOWf, L3_PURGEf};
    int wr_val = 1; /* w1tc */
    int i;

    for (i = 0; i < (sizeof(thdr_intr_fields) / sizeof(soc_field_t)); i++) {
        SOC_IF_ERROR_RETURN(
            soc_reg32_get(unit, block_intr_reg, itm, 0, &rval));
        if (!soc_reg_field_get(unit, block_intr_reg,
                               rval, thdr_intr_fields[i])) {
            continue;
        }
        switch(thdr_intr_fields[i]) {
            case CCP_OVERFLOWf:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit, "Unit: %0d -- Multicast copy count"
                         " exceeded 1K intr set for ITM = %0d \n"),
                         unit, itm));
                break;
            case INVALID_DESTf:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit, "Unit: %0d -- Invalid destination port"
                         " in L2/L3 bitmap intr set for ITM = %0d \n"),
                         unit, itm));

                break;
            case L3_PURGEf:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit, "Unit: %0d -- Packet drop due to"
                         " invalid multicast config intr set for ITM = %0d \n"),
                         unit, itm));

                break;
            default:
                break;
        }
        /*Clear block(RQE) level interrupt*/
        rval = 0;
        SOC_IF_ERROR_RETURN(
                soc_reg32_get(unit, block_intr_reg, itm, 0, &rval));
        soc_reg_field_set(unit, block_intr_reg, &rval,
                thdr_intr_fields[i], wr_val);
        SOC_IF_ERROR_RETURN(
                soc_reg32_set(unit, block_intr_reg, itm, 0, rval));
    }
    /* Clear ITM and GLB level interrupts */
    SOC_IF_ERROR_RETURN(
        soc_th3_clear_itm_int_stat(unit, itm, itm_intr_field));

    return SOC_E_NONE;
}

int
soc_th3_process_itm_crb_int (int unit, int itm, soc_field_t itm_intr_field) {
    soc_reg_t block_intr_reg = MMU_CRB_CPU_INT_STATr;
    uint32 rval = 0;
    soc_field_t thdr_intr_fields[] = {MTU_VIOLATION_INTR_STATf,
        INV_SRC_PORT_INTR_STATf, INV_DST_PORT_INTR_STATf};
    int wr_val = 1; /* w1tc */
    int i;

    for (i = 0; i < (sizeof(thdr_intr_fields) / sizeof(soc_field_t)); i++) {
        SOC_IF_ERROR_RETURN(
            soc_reg32_get(unit, block_intr_reg, itm, 0, &rval));
        if (!soc_reg_field_get(unit, block_intr_reg,
                               rval, thdr_intr_fields[i])) {
            continue;
        }
        switch(thdr_intr_fields[i]) {
            case MTU_VIOLATION_INTR_STATf:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit, "Unit: %0d -- MTU violation"
                         " intr set for ITM = %0d \n"),
                         unit, itm));
                break;
            case INV_SRC_PORT_INTR_STATf:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit, "Unit: %0d -- Invalid source port"
                         " intr set for ITM = %0d \n"),
                         unit, itm));
                break;
            case INV_DST_PORT_INTR_STATf:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit, "Unit: %0d -- Invalid dest port"
                         " intr set for ITM = %0d \n"),
                         unit, itm));
                break;
            default:
                break;
        }
        /*Clear block(THDR QE) level interrupt*/
        rval = 0;
        SOC_IF_ERROR_RETURN(
                soc_reg32_get(unit, block_intr_reg, itm, 0, &rval));
        soc_reg_field_set(unit, block_intr_reg, &rval,
                thdr_intr_fields[i], wr_val);
        SOC_IF_ERROR_RETURN(
                soc_reg32_set(unit, block_intr_reg, itm, 0, rval));
    }
    /* Clear ITM and GLB level interrupts */
    SOC_IF_ERROR_RETURN(
        soc_th3_clear_itm_int_stat(unit, itm, itm_intr_field));

    return SOC_E_NONE;
}

int
soc_th3_process_eb_ebctm_int (int unit, int pipe, soc_field_t eb_intr_field) {
    soc_reg_t block_intr_reg = MMU_EBCTM_CPU_INT_STATr;
    uint32 rval = 0;
    soc_field_t ebctm_intr_fields[] = {OVERFLOW_INTR_STATf, UNDERFLOW_INTR_STATf};
    int wr_val = 1; /* w1tc */
    int i;

    for (i = 0; i < (sizeof(ebctm_intr_fields) / sizeof(soc_field_t)); i++) {
        SOC_IF_ERROR_RETURN(
            soc_reg32_get(unit, block_intr_reg, pipe, 0, &rval));
        if (!soc_reg_field_get(unit, block_intr_reg,
                               rval, ebctm_intr_fields[i])) {
            continue;
        }
        switch(ebctm_intr_fields[i]) {
            case OVERFLOW_INTR_STATf:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit, "Unit: %0d -- EBCTM counters Overflow"
                         " intr set for PIPE = %0d \n"),
                         unit, pipe));
                break;
            case UNDERFLOW_INTR_STATf:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit, "Unit: %0d -- EBCTM counters Underflow"
                         " intr set for PIPE = %0d \n"),
                         unit, pipe));

                break;
            default:
                break;
        }
        /*Clear block(EBCTM) level interrupt*/
        rval = 0;
        SOC_IF_ERROR_RETURN(
                soc_reg32_get(unit, block_intr_reg, pipe, 0, &rval));
        soc_reg_field_set(unit, block_intr_reg, &rval,
                ebctm_intr_fields[i], wr_val);
        SOC_IF_ERROR_RETURN(
                soc_reg32_set(unit, block_intr_reg, pipe, 0, rval));
    }
    /* Clear EB and GLB level interrupts */
    SOC_IF_ERROR_RETURN(
        soc_th3_clear_eb_int_stat(unit, pipe, eb_intr_field));

    return SOC_E_NONE;
}

int
soc_th3_process_eb_ebcfp_int (int unit, int pipe, soc_field_t eb_intr_field) {
    soc_reg_t block_intr_reg = MMU_EBCFP_CPU_INT_STATr;
    uint32 rval = 0;
    soc_field_t ebcfp_intr_field = MTU_VIOLATIONf;
    int wr_val = 1; /* w1tc */

        SOC_IF_ERROR_RETURN(
            soc_reg32_get(unit, block_intr_reg, pipe, 0, &rval));
        if (soc_reg_field_get(unit, block_intr_reg,
                               rval, ebcfp_intr_field)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "Unit: %0d -- MTU violation in EB"
                     " group intr set for PIPE = %0d \n"),
                     unit, pipe));
        }
        /*Clear block(EBCFP) level interrupt*/
        rval = 0;
        SOC_IF_ERROR_RETURN(
                soc_reg32_get(unit, block_intr_reg, pipe, 0, &rval));
        soc_reg_field_set(unit, block_intr_reg, &rval,
                ebcfp_intr_field, wr_val);
        SOC_IF_ERROR_RETURN(
                soc_reg32_set(unit, block_intr_reg, pipe, 0, rval));
    /* Clear EB and GLB level interrupts */
    SOC_IF_ERROR_RETURN(
        soc_th3_clear_eb_int_stat(unit, pipe, eb_intr_field));

    return SOC_E_NONE;
}

int
soc_th3_process_eb_mtro_int (int unit, int pipe, soc_field_t eb_intr_field) {
    soc_reg_t block_intr_reg = MMU_MTRO_CPU_INT_STATr;
    uint32 rval = 0;
    soc_field_t mtro_intr_field = CNT_OVERFLOWf;
    int wr_val = 1; /* w1tc */

    if (soc_reg_field_get(unit, block_intr_reg, rval, mtro_intr_field)) {
        SOC_IF_ERROR_RETURN(
            soc_reg32_get(unit, block_intr_reg, pipe, 0, &rval));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "Unit: %0d -- Counter overflow in MTRO"
                 " intr set for PIPE = %0d \n"),
                 unit, pipe));
        /*Clear block(MTRO) level interrupt*/
        rval = 0;
        SOC_IF_ERROR_RETURN(
                soc_reg32_get(unit, block_intr_reg, pipe, 0, &rval));
        soc_reg_field_set(unit, block_intr_reg, &rval,
                mtro_intr_field, wr_val);
        SOC_IF_ERROR_RETURN(
                soc_reg32_set(unit, block_intr_reg, pipe, 0, rval));
        /* Clear EB and GLB level interrupts */
        SOC_IF_ERROR_RETURN(
            soc_th3_clear_eb_int_stat(unit, pipe, eb_intr_field));
    }

    return SOC_E_NONE;
}

#endif /* BCM_TOMAHAWK3_SUPPORT */
