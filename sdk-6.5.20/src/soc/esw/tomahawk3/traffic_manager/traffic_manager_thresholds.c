/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        traffic_manager_thresholds.c
 * Purpose:     Tomahawk3 MMU thresholds (ingress and egress
 *              admission control) routines
 * Requires:
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/hash.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)

#include <soc/tomahawk3.h>

#ifndef MIN
#define MIN(a, b)               (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a, b)               (((a)>(b))?(a):(b))
#endif

#define CEIL(x, y)              (((x) + ((y)-1)) / (y))

#define TH3_MMU_SHARED_LIMIT_CHK(val1, val2, flags) \
    (((flags) && (val1 > val2)) || ((!flags) && (val1 < val2)))

typedef void (*soc_mmu_config_buf_default_f)(int unit,
        _soc_mmu_cfg_buf_t *buf, _soc_mmu_device_info_t *devcfg,
        int lossless);
#if 0
static _soc_pkt_size_dist_t
            th3_standard_dist[3] = {
    {64,  100},
    {215,  0},
    {1024, 0}
};
#endif

/*! @fn int _soc_th3_get_pm_from_port(int pnum)
 *  @param pnum Logical port number.
 *  @brief Helper function to calculate PM number from logical port number.
           This function should be called only for front panel ports.
 *  @returns PM number
 */
int
_soc_th3_get_pm_from_port(int unit, int pnum)
{
    int phy_port;
    soc_info_t *si;

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[pnum];
    return ((phy_port - 1) >> 3);
}


/*! @fn int _soc_th3_ports_per_pm_get(
 *    int unit,
 *    int pm_id)
 *  @param unit Chip unit number.
 *  @param pm_id PM num.
 *  @brief Function used to return number of ports in Port Macro
 * Description:
 *      Function used to return number of ports in Port Macro
 * Parameters:
 *      unit - Device number
 *      pm_id - Port Macro no
 * Return Value:
 *      index value
 */

int
_soc_th3_ports_per_pm_get(int unit, int pm_id)
{
    soc_info_t *si;
    int phy_port, i, num_ports = 0;

    si = &SOC_INFO(unit);

    if (pm_id >= _TH3_PBLKS_PER_DEV) {
       return 0;
    }

    phy_port = 1 + (pm_id * _TH3_PORTS_PER_PBLK);
    for (i = 0; i < _TH3_PORTS_PER_PBLK; ) {
        if (si->port_p2l_mapping[phy_port + i] != -1) {
            num_ports ++;
            i += si->port_num_lanes[si->port_p2l_mapping[phy_port + i]];
        }else {
            i ++;
        }
    }

    return num_ports;
}


/*! @fn int _soc_th3_piped_mem_index(
 *    int unit,
 *    soc_port_t port,
 *    soc_mem_t mem,
 *    int arr_off)
 *  @param unit Chip unit number.
 *  @param port logical port number
 *  @param mem Memory name.
 *  @param arr_off Offset.
 *  @brief Function used for index calculation of certain memories
 * Description:
 *      Function used for index calculation of certain memories
 * Parameters:
 *      unit - Device number
 *      port - port number
 *      mem - Memory name
 *      arr_off - Array offset
 * Return Value:
 *      index value
 */

uint32 _soc_th3_piped_mem_index(int unit, soc_port_t port, soc_mem_t mem,
        int arr_off)
{
    int mmu_chip_port, mmu_local_port, index;

    mmu_local_port = SOC_TH3_MMU_LOCAL_PORT(unit, port);
    mmu_chip_port = SOC_TH3_MMU_CHIP_PORT(unit, port);

    switch (mem) {
        case MMU_THDI_PORT_PG_MIN_CONFIGm:
        case MMU_THDI_PORT_PG_SHARED_CONFIGm:
        case MMU_THDI_PORT_PG_RESUME_CONFIGm:
        case MMU_THDI_PORT_PG_HDRM_CONFIGm:
        case MMU_THDI_PORTSP_CONFIGm:
             /* Index is local mmu port 0-19 */
            index = mmu_local_port;
            break;
        case MMU_THDO_CONFIG_PORT_UC0m:
        case MMU_THDO_CONFIG_PORT_UC1m:
        case MMU_THDO_CONFIG_PORT_UC2m:
        case MMU_THDO_PORT_QUEUE_SERVICE_POOLm:
            index = mmu_chip_port;
            break;
        case MMU_THDO_CONFIG_PORTSP_MCm:
        case MMU_THDO_RESUME_PORT_UC0m:
        case MMU_THDO_RESUME_PORT_UC1m:
        case MMU_THDO_RESUME_PORT_UC2m:
        case MMU_THDO_RESUME_PORT_MC0m:
        case MMU_THDO_RESUME_PORT_MC1m:
        case MMU_THDO_RESUME_PORT_MC2m:
             /* Index is port,  spid */
            index = (mmu_chip_port << 2) + arr_off;
            break;
        default:
            return arr_off;
    }

    return index;
}


/*! @fn void _soc_th3_mmu_init_dev_config(
 *    int unit,
 *    _soc_mmu_device_info_t *devcfg,
 *    int lossless)
 *  @param unit Chip unit number.
 *  @param devCfg Pointer to _soc_mmu_device_info_t structure
 *  @param lossless Indicates if lossy or lossless
 *  @brief Function used for index calculation of certain memories
 * Description:
 *      Function used to setup device config
 * Parameters:
 *      unit - Device number
 *      devcfg - Pointer to devcfg structure
 *      lossless - Indicates lossy, lossless or lossy+lossless
 * Return Value:
 *      void
 */

void
_soc_th3_mmu_init_dev_config(int unit, _soc_mmu_device_info_t *devcfg,
        int lossless)
{
    sal_memset(devcfg, 0, sizeof(_soc_mmu_device_info_t));

    devcfg->max_pkt_byte = _TH3_MMU_MAX_PACKET_BYTES;
    devcfg->mmu_hdr_byte = _TH3_MMU_PACKET_HEADER_BYTES;
    devcfg->jumbo_pkt_size = _TH3_MMU_JUMBO_FRAME_BYTES;
    devcfg->default_mtu_size = _TH3_MMU_DEFAULT_MTU_BYTES;
    devcfg->mmu_cell_size = _TH3_MMU_BYTES_PER_CELL;
    devcfg->mmu_total_cell = _TH3_MMU_TOTAL_CELLS_PER_ITM;
    devcfg->num_pg = _TH3_MMU_NUM_PG;
    devcfg->num_service_pool = _TH3_MMU_NUM_POOL;
    devcfg->flags = SOC_MMU_CFG_F_PORT_MIN | SOC_MMU_CFG_F_PORT_POOL_MIN |
                    SOC_MMU_CFG_F_RQE | SOC_MMU_CFG_F_EGR_MCQ_ENTRY;
    devcfg->total_mcq_entry = _TH3_MMU_TOTAL_MCQ_ENTRY(unit);
    devcfg->rqe_queue_num = _TH3_MMU_NUM_RQE_QUEUES;
}

/*! @fn int _soc_th3_default_pg_headroom(
 *    int unit,
 *    soc_port_t port,
 *    int lossless)
 *  @param unit Chip unit number.
 *  @param port Logical port number
 *  @param lossless Indicates if lossy or lossless
 *  @brief Function used to determine PG headroom
 * Description:
 *      Function used to determine PG headroom based on port speed
 *      for oversub ports (No linerate ports in TH3)
 * Parameters:
 *      unit - Device number
 *      port - port number
 *      lossless - Indicates lossy, lossless or lossy+lossless
 * Return Value:
 *      headroom value
 */

STATIC int
_soc_th3_default_pg_headroom(int unit, soc_port_t port, int lossless)
{
    int speed = 10000, hdrm = 0;

    if (IS_CPU_PORT(unit, port)) {
        return 0;
    } else if (IS_MANAGEMENT_PORT(unit, port)) {
        return 0;
    } else if (IS_LB_PORT(unit, port)) {
        return 0;
    } else if (!lossless) {
        return 0;
    }

    speed = SOC_INFO(unit).port_speed_max[port];

    if (speed == 10000) {
        hdrm = _TH3_PG_HEADROOM_OVERSUB_10G;
    } else if (speed == 25000) {
        hdrm = _TH3_PG_HEADROOM_OVERSUB_25G;
    } else if ((speed == 50000) || (speed == 40000)) {
        hdrm = _TH3_PG_HEADROOM_OVERSUB_50G;
    } else if (speed == 100000) {
        hdrm = _TH3_PG_HEADROOM_OVERSUB_100G;
    } else if (speed == 200000) {
        hdrm = _TH3_PG_HEADROOM_OVERSUB_200G;
    } else if (speed == 400000) {
        hdrm = _TH3_PG_HEADROOM_OVERSUB_400G;
    } else {
        hdrm = _TH3_PG_HEADROOM_OVERSUB_10G;
        LOG_WARN(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit,
            "_soc_th3_default_pg_headroom(port: %d) Illegal speed %d\n"),
            port, speed));
    }
    LOG_VERBOSE(BSL_LS_SOC_MMU,
              (BSL_META_U(unit,
              "_soc_th3_default_pg_headroom(port:%d, lossless:%d) hdrm= %d\n"),
              port, lossless, hdrm));
    return hdrm;
}

/*! @fn int soc_th3_mmu_get_shared_size(
 *    int unit,
 *    int* thdi_shared,
 *    int* thdo_db_shared,
 *    int* thdo_mcqe_shared)
 *  @param unit Chip unit number.
 *  @param buf _soc_mmu_cfg_buf_t struct
 *  @param pool_id Service pool id
 *  @brief Function used to determine Shared limits
 * Description:
 *      Function used to determine shared limit
 * Parameters:
 *      unit - Device number
 *      thd_shared - THDI/THDO0 shared limit per itm
 * Return Value:
 *      shared limit value
 */

int
soc_th3_mmu_get_shared_size(int unit, int *thd_shared)
{
    
    int itm;
    uint32 rval;
    soc_info_t *si;
    si = &SOC_INFO(unit);

    for (itm = 0; itm < _TH3_ITMS_PER_DEV; itm ++) {
        if (si->itm_map & (1 << itm)) {
            rval = 0;
            SOC_IF_ERROR_RETURN(
                soc_tomahawk3_itm_reg32_get(unit,
                    MMU_THDI_BUFFER_CELL_LIMIT_SPr, itm, itm, 0, &rval));
            thd_shared[itm] = soc_reg_field_get(unit, MMU_THDI_BUFFER_CELL_LIMIT_SPr,
                                                 rval, LIMITf);
        }

    }

    return SOC_E_NONE;

}


/*! @fn int _soc_th3_get_shared_limit(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    int pool_id)
 *  @param unit Chip unit number.
 *  @param buf _soc_mmu_cfg_buf_t struct
 *  @param pool_id Service pool id
 *  @brief Function used to determine Shared limits
 * Description:
 *      Function used to determine shared limit
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t struct
 *      pool_id - service pool id
 * Return Value:
 *      headroom value
 */

STATIC int
_soc_th3_get_shared_limit(int unit, _soc_mmu_cfg_buf_t *buf, int pool_id)
{
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    int limit, limit_itm;
    int itm;

    limit = _TH3_MMU_TOTAL_CELLS_PER_ITM;
    for (itm = 0; itm < _TH3_ITMS_PER_DEV; itm ++) {
        buf_pool = &buf->pools_itm[itm][pool_id];
        if (buf_pool->total != 0) {
            limit_itm = buf_pool->total
                - buf_pool->prigroup_headroom
                - buf_pool->prigroup_guarantee
                - buf_pool->port_guarantee
                - buf_pool->queue_group_guarantee
                - buf_pool->queue_guarantee
                - buf_pool->mcq_entry_reserved;
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "_soc_th3_get_shared_limit(itm:%d, pool_id:%d):\n"
                "Total cell memory = %d\n"
                "Total PG headroom = %d\n"
                "Total PGMin guarantee = %d\n"
                "Total Port guarantee = %d\n"
                "Total QGrpMin guarantee = %d\n"
                "Total QMin guarantee = %d\n"
                "Total MCQ Entry reserved = %d\n"
                "Shared Limit = %d\n"),
                 itm, pool_id, buf_pool->total,
                 buf_pool->prigroup_headroom,
                 buf_pool->prigroup_guarantee,
                 buf_pool->port_guarantee,
                 buf_pool->queue_group_guarantee,
                 buf_pool->queue_guarantee,
                 buf_pool->mcq_entry_reserved,
                 limit_itm));
        } else {
            limit_itm = 0;
        }
        if (limit_itm < limit) {
            limit = limit_itm;
        }
    }

    return limit;
}


/*! @fn int _soc_th3_get_hdrm_limit(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    int pool_id)
 *  @param unit Chip unit number.
 *  @param buf _soc_mmu_cfg_buf_t struct
 *  @param pool_id Service pool id
 *  @brief Function used to determine total Headroom limits
 * Description:
 *      Function used to determine total Headroom limit
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t struct
 *      pool_id - service pool id
 * Return Value:
 *      headroom value
 */

STATIC int
_soc_th3_get_hdrm_limit(int unit, _soc_mmu_cfg_buf_t *buf, int pool_id)
{
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    int hdrm, hdrm_itm[_TH3_ITMS_PER_DEV];
    int itm;

    for (itm = 0; itm < _TH3_ITMS_PER_DEV; itm ++) {
        buf_pool = &buf->pools_itm[itm][pool_id];
        hdrm_itm[itm] = buf_pool->prigroup_headroom;
    }
    hdrm = MAX(hdrm_itm[0], hdrm_itm[1]);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "_soc_th3_get_hdrm_limit(pool:%d) = %d\n"), pool_id, hdrm));
    return hdrm;
}

/*! @fn void _soc_th3_mmu_config_profiles_pri_to_pg_default_set(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    _soc_mmu_device_info_t *devcfg,
 *    int profile_idx
 *    int input_pri
 *    int lossless
 *    int uc)
 *  @param unit Chip unit number.
 *  @param buf Pointer to _soc_mmu_cfg_buf_t structure
 *  @param devcfg Pointer to _soc_mmu_device_info_t structure
 *  @param lossless Indicates if lossy or lossless
 *  @param profile_idx specifies profile index
 *  @param input_pro specifies input priority value
 *  @param uc specifies if uc or mc
 *  @brief Function to setup the various mapping profiles
 * Description:
 *      Function to setup the various mapping profiles
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t structure
 *      devcfg - _soc_mmu_device_info_t structure
 *      profile_idx - Indicates profile_index
 *      input_pri - Indicates input priority value
 *      lossless - Indicates lossy, lossless or lossy+lossless
 *      uc - Indicates if uc or mc
 * Return Value:
 *      SOC_E_*
 */

STATIC void
_soc_th3_mmu_config_profiles_pri_to_pg_default_set(int unit,
    _soc_mmu_cfg_buf_t *buf, _soc_mmu_device_info_t *devcfg, int profile_idx,
    int input_pri, int lossless ,int uc)
{
    _soc_mmu_cfg_buf_mapping_profile_t *buf_map_profiles;

    buf_map_profiles = &buf->mapprofiles[profile_idx];

    if (lossless == 2) { /* Lossy+Lossless mode */
        if (input_pri == 0) {
            if (uc) {
                buf_map_profiles->inpri_to_prigroup_uc[input_pri] = 7;
            } else {
                buf_map_profiles->inpri_to_prigroup_mc[input_pri] = 7;
            }
        } else if (input_pri >= 7) {
            if (uc) {
                buf_map_profiles->inpri_to_prigroup_uc[input_pri] = 0;
            } else {
                buf_map_profiles->inpri_to_prigroup_mc[input_pri] = 0;
            }
        } else {
            if (uc) {
                buf_map_profiles->inpri_to_prigroup_uc[input_pri] = input_pri;
            } else {
                buf_map_profiles->inpri_to_prigroup_mc[input_pri] = input_pri;
            }
        }
    } else if (lossless == 1) { /* Lossless mode */
        if (uc) {
            buf_map_profiles->inpri_to_prigroup_uc[input_pri] = 7;
        } else {
            buf_map_profiles->inpri_to_prigroup_mc[input_pri] = 7;
        }
    } else { /* Lossy mode */
        if (input_pri < 7) {
            if (uc) {
                buf_map_profiles->inpri_to_prigroup_uc[input_pri] = input_pri;
            } else {
                buf_map_profiles->inpri_to_prigroup_mc[input_pri] = input_pri;
            }
        } else {
            if (uc) {
                buf_map_profiles->inpri_to_prigroup_uc[input_pri] = 7;
            } else {
                buf_map_profiles->inpri_to_prigroup_mc[input_pri] = 7;
            }
        }
    }

}

/*! @fn void _soc_th3_mmu_config_mapping_profiles_set(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    _soc_mmu_device_info_t *devcfg,
 *    int lossless)
 *  @param unit Chip unit number.
 *  @param buf Pointer to _soc_mmu_cfg_buf_t structure
 *  @param devcfg Pointer to _soc_mmu_device_info_t structure
 *  @param lossless Indicates if lossy or lossless
 *  @brief Function to setup the various mapping profiles
 * Description:
 *      Function to setup the various mapping profiles
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t structure
 *      devcfg - _soc_mmu_device_info_t structure
 *      lossless - Indicates lossy, lossless or lossy+lossless
 * Return Value:
 *      SOC_E_*
 */

STATIC void
_soc_th3_mmu_config_mapping_profiles_set(int unit, _soc_mmu_cfg_buf_t *buf,
         _soc_mmu_device_info_t *devcfg,  int lossless)
{
    _soc_mmu_cfg_buf_mapping_profile_t *buf_map_profiles;
    int idx, profile_idx;


    for (profile_idx = 0; profile_idx < SOC_MMU_CFG_NUM_PROFILES_MAX;
            profile_idx++) {
        buf_map_profiles = &buf->mapprofiles[profile_idx];
        for (idx = 0; idx < SOC_MMU_CFG_INT_PRI_MAX; idx++) {
            if (lossless == 2) { /* Lossy+Lossless mode */
                if (idx == 0) {
                    buf_map_profiles->inpri_to_prigroup_uc[idx] = 7;
                    buf_map_profiles->inpri_to_prigroup_mc[idx] = 7;
                } else if (idx >= 7) {
                    buf_map_profiles->inpri_to_prigroup_uc[idx] = 0;
                    buf_map_profiles->inpri_to_prigroup_mc[idx] = 0;
                } else {
                    buf_map_profiles->inpri_to_prigroup_uc[idx] = idx;
                    buf_map_profiles->inpri_to_prigroup_mc[idx] = idx;

                }
            } else if (lossless == 1) { /* Lossless mode */
                buf_map_profiles->inpri_to_prigroup_uc[idx] = 7;
                buf_map_profiles->inpri_to_prigroup_mc[idx] = 7;
            } else { /* Lossy mode */
                if (idx < 7) {
                    buf_map_profiles->inpri_to_prigroup_uc[idx] = idx;
                    buf_map_profiles->inpri_to_prigroup_mc[idx] = idx;
                } else {
                    buf_map_profiles->inpri_to_prigroup_uc[idx] = 7;
                    buf_map_profiles->inpri_to_prigroup_mc[idx] = 7;
                }
            }
        }
        if (profile_idx == 0) { /* By default only one profile is valid */
            buf_map_profiles->valid = 1;
        } else {
            buf_map_profiles->valid = 0;
        }
        for (idx = 0; idx < SOC_MMU_CFG_PRI_GROUP_MAX; idx++) {
            buf_map_profiles->prigroup_to_servicepool[idx] = 0;
            buf_map_profiles->prigroup_to_headroompool[idx] = 0;
            buf_map_profiles->pfcpri_to_prigroup[idx] = 0;
        }
    }
}

/*! @fn int _soc_th3_mmu_config_mapping_profiles_set_hw(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    _soc_mmu_device_info_t *devcfg,
 *    int lossless)
 *  @param unit Chip unit number.
 *  @param buf Pointer to _soc_mmu_cfg_buf_t structure
 *  @param devcfg Pointer to _soc_mmu_device_info_t structure
 *  @param lossless Indicates if lossy or lossless
 *  @brief Function used to program the various mapping profiles
 * Description:
 *      Function used to program the various mapping profiles
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t structure
 *      devcfg - _soc_mmu_device_info_t structure
 *      lossless - Indicates lossy, lossless or lossy+lossless
 * Return Value:
 *      SOC_E_*
 */

STATIC int
_soc_th3_mmu_config_mapping_profiles_set_hw(int unit,
        _soc_mmu_cfg_buf_t *buf, _soc_mmu_device_info_t *devcfg,
        int lossless)
{
    _soc_mmu_cfg_buf_mapping_profile_t *buf_map_profiles;
    int idx, loop;
    uint64 rval;
    uint64 fval;
    soc_field_t field_pri[SOC_MMU_CFG_INT_PRI_MAX] = {
            INPPRI0_PGf, INPPRI1_PGf, INPPRI2_PGf,
            INPPRI3_PGf, INPPRI4_PGf, INPPRI5_PGf,
            INPPRI6_PGf, INPPRI7_PGf, INPPRI8_PGf,
            INPPRI9_PGf, INPPRI10_PGf, INPPRI11_PGf,
            INPPRI12_PGf, INPPRI13_PGf, INPPRI14_PGf,
            INPPRI15_PGf};
    soc_field_t field_hpid[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_HPIDf, PG1_HPIDf, PG2_HPIDf,
            PG3_HPIDf, PG4_HPIDf, PG5_HPIDf,
            PG6_HPIDf, PG7_HPIDf};
    soc_field_t field_spid[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_SPIDf, PG1_SPIDf, PG2_SPIDf,
            PG3_SPIDf, PG4_SPIDf, PG5_SPIDf,
            PG6_SPIDf, PG7_SPIDf};
    soc_field_t field_pfcpri[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PFCPRI0_PGf, PFCPRI1_PGf, PFCPRI2_PGf,
            PFCPRI3_PGf, PFCPRI4_PGf, PFCPRI5_PGf,
            PFCPRI6_PGf, PFCPRI7_PGf};


    for (idx = 0; idx < SOC_REG_NUMELS(unit, MMU_THDI_UC_INPPRI_PG_PROFILEr);
            idx++) {
        buf_map_profiles = &buf->mapprofiles[idx];
        if (buf_map_profiles->valid  != 1) {
            continue;
        }
        COMPILER_64_ZERO(rval);
        for (loop = 0; loop < SOC_MMU_CFG_INT_PRI_MAX; loop++) {
            COMPILER_64_SET(
                fval, 0, buf_map_profiles->inpri_to_prigroup_uc[loop]);
            soc_reg64_field_set(unit,
                MMU_THDI_UC_INPPRI_PG_PROFILEr, &rval, field_pri[loop], fval);
        }
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg_set(unit, MMU_THDI_UC_INPPRI_PG_PROFILEr,
                -1, -1, idx, rval));
    }
    for (idx = 0; idx < SOC_REG_NUMELS(unit, MMU_THDI_NONUC_INPPRI_PG_PROFILEr);
            idx++) {
        buf_map_profiles = &buf->mapprofiles[idx];
        if (buf_map_profiles->valid  != 1) {
            continue;
        }
        COMPILER_64_ZERO(rval);
        for (loop = 0; loop < SOC_MMU_CFG_INT_PRI_MAX; loop++) {
            COMPILER_64_SET(
                fval, 0, buf_map_profiles->inpri_to_prigroup_mc[loop]);
            soc_reg64_field_set(unit, MMU_THDI_NONUC_INPPRI_PG_PROFILEr,
                &rval, field_pri[loop], fval);
        }
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg_set(unit,
                MMU_THDI_NONUC_INPPRI_PG_PROFILEr, -1, -1, idx,  rval));
    }

    for (idx = 0; idx < SOC_REG_NUMELS(unit, MMU_THDI_PG_PROFILEr); idx++) {
        buf_map_profiles = &buf->mapprofiles[idx];
        if (buf_map_profiles->valid  != 1) {
            continue;
        }
        COMPILER_64_ZERO(rval);
        for (loop = 0; loop < SOC_MMU_CFG_PRI_GROUP_MAX; loop++) {
            COMPILER_64_SET(
                fval, 0, buf_map_profiles->prigroup_to_headroompool[loop]);
            soc_reg64_field_set(unit,
                MMU_THDI_PG_PROFILEr, &rval, field_hpid[loop], fval);
            COMPILER_64_SET(
                fval, 0, buf_map_profiles->prigroup_to_servicepool[loop]);
            soc_reg64_field_set(unit,
                MMU_THDI_PG_PROFILEr, &rval, field_spid[loop], fval);
        }
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg_set(unit, MMU_THDI_PG_PROFILEr, -1, -1,
                idx,  rval));
    }
    for (idx = 0; idx < SOC_REG_NUMELS(unit, MMU_THDI_PFCPRI_PG_PROFILEr);
            idx++) {
        buf_map_profiles = &buf->mapprofiles[idx];
        if (buf_map_profiles->valid  != 1) {
            continue;
        }
        COMPILER_64_ZERO(rval);
        for (loop = 0; loop < SOC_MMU_CFG_PRI_GROUP_MAX; loop++) {
            COMPILER_64_SET(
                fval, 0, buf_map_profiles->pfcpri_to_prigroup[loop]);
            soc_reg64_field_set(unit,
                MMU_THDI_PFCPRI_PG_PROFILEr, &rval, field_pfcpri[loop], fval);
        }
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg_set(unit, MMU_THDI_PFCPRI_PG_PROFILEr,
                -1, -1,idx,  rval));
    }

    return SOC_E_NONE;
}

/*! @fn int _soc_th3_mmu_config_thdi_set(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    _soc_mmu_device_info_t *devcfg,
 *    int lossless)
 *  @param unit Chip unit number.
 *  @param buf Pointer to _soc_mmu_cfg_buf_t structure
 *  @param devcfg Pointer to _soc_mmu_device_info_t structure
 *  @param lossless Indicates if lossy or lossless
 *  @brief Function to setup the THDI settings
 * Description:
 *      Function used to setup the various ingress admission
 *      threshold limits like min guarantee, shared limits, etc.
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t structure
 *      devcfg - _soc_mmu_device_info_t structure
 *      lossless - Indicates lossy, lossless or lossy+lossless
 * Return Value:
 *      void
 */
STATIC void
_soc_th3_mmu_config_thdi_set(int unit, _soc_mmu_cfg_buf_t *buf,
        _soc_mmu_device_info_t *devcfg,  int lossless)
{
    _soc_mmu_cfg_buf_port_t *buf_port;
     _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    int default_mtu_cells;
    int port, idx;
    int total_pool_size = 0;

    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
    total_pool_size = devcfg->mmu_total_cell;

    PBMP_ALL_ITER(unit, port) {
        if (port >= SOC_MMU_CFG_PORT_MAX) {
            break;
        }
        buf_port = &buf->ports[port];

        buf_port->pri_to_prigroup_profile_idx = 0;
        buf_port->prigroup_profile_idx = 0;

        /* Per-Port Per PG settings */
        /* port_guarantee_enable = 1 implies use PGMin */
        /* port_guarantee_enable = 0 implies use PortSPMin */
        for (idx = 0; idx < _TH3_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf_port->prigroups[idx];
            buf_prigroup->user_delay = -1;
            buf_prigroup->switch_delay = -1;
            buf_prigroup->pool_floor = 0;
            buf_prigroup->pool_resume = 0;
            buf_prigroup->port_guarantee_enable = 1; /* Use PGMIN by default */
            buf_prigroup->guarantee = 0;
            buf_prigroup->headroom = 0;
            buf_prigroup->flow_control_enable = 0;
            buf_prigroup->lossless = 0;
            buf_prigroup->pool_scale = -1;
            if ((lossless == 1) || (lossless == 2)) {
                buf_prigroup->pool_scale = 8;
            }
            /* PG7 is lossless PG */
            if (idx == _TH3_MMU_NUM_PG-1) {
                buf_prigroup->guarantee = (lossless == 0) ? 0: default_mtu_cells;
                buf_prigroup->lossless = (lossless == 0) ? 0 : 1;
                buf_prigroup->headroom = (lossless == 0) ? 0 :
                    _soc_th3_default_pg_headroom(unit, port, lossless);
                buf_prigroup->flow_control_enable = (lossless == 2) ? 1 : 0;
            }
        }

        /* Per-Port Per Pool settings */
        for (idx = 0; idx < _TH3_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            buf_port_pool->guarantee = 0;
            buf_port_pool->pool_limit = 0;
            buf_port_pool->pool_resume = 0;
            if (idx == 0) {
                buf_port_pool->pool_limit = total_pool_size;
                buf_port_pool->pool_resume = total_pool_size;
            }
        }
    }


}

/*! @fn int _soc_th3_mmu_config_thdo_set(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    _soc_mmu_device_info_t *devcfg,
 *    int lossless)
 *  @param unit Chip unit number.
 *  @param buf Pointer to _soc_mmu_cfg_buf_t structure
 *  @param devcfg Pointer to _soc_mmu_device_info_t structure
 *  @param lossless Indicates if lossy or lossless
 *  @brief Function to setup the THDO settings
 * Description:
 *      Function used to setup the various egress admission
 *      threshold limits like min guarantee, shared limits,
 *      color limits, etc.
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t structure
 *      devcfg - _soc_mmu_device_info_t structure
 *      lossless - Indicates lossy, lossless or lossy+lossless
 * Return Value:
 *      void
 */
STATIC void
_soc_th3_mmu_config_thdo_set(int unit, _soc_mmu_cfg_buf_t *buf,
        _soc_mmu_device_info_t *devcfg,  int lossless)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;
    int default_mtu_cells;
    int port, itm, idx;
    int total_pool_size = 0;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "Initializing default MMU config THDO (u =  % d)\n"), unit));

    si = &SOC_INFO(unit);

    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);

    total_pool_size = devcfg->mmu_total_cell; /* per ITM limit */

    for (idx = 0; idx < _TH3_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if (idx == 0) {  /* 100% scale up by 100 */
            buf_pool->total_mcq_entry = SOC_TH3_MMU_MCQ_ENTRY_PER_ITM;
            buf_pool->total = _TH3_MMU_TOTAL_CELLS_PER_ITM;
            buf_pool->size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG; /* 100 Percent */
        } else {
            buf_pool->total_mcq_entry = 0;
            buf_pool->total = 0;
            buf_pool->size = 0;
        }

        for (itm = 0; itm < _TH3_ITMS_PER_DEV; itm ++) {
            sal_memcpy(&buf->pools_itm[itm][idx], buf_pool,
                sizeof(_soc_mmu_cfg_buf_pool_t));
        }
    }

    for (idx = 0; idx < SOC_TH3_MMU_CFG_QGROUP_MAX; idx++) {
        queue_grp = &buf->qgroups[idx];
        if (!IS_CPU_PORT(unit, idx)) { /* CPU is Qmin */
            queue_grp->guarantee = 2 * default_mtu_cells;
            queue_grp->guarantee_mc = 2 * default_mtu_cells;
        }
    }

    /* Per-Port settings */
    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];

         for (idx = 0; idx < _TH3_MMU_NUM_POOL; idx++) {
             buf_port_pool = &buf_port->pools[idx];
             buf_port_pool->pool_limit = 0;
             buf_port_pool->pool_resume = 0;
             if (idx == 0) {
                 buf_port_pool->pool_limit = total_pool_size;
                 buf_port_pool->pool_resume = total_pool_size;
             }
         }
    }

    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];
        for (idx = 0;
                idx < (si->port_num_uc_cosq[port] + si->port_num_cosq[port]);
                idx++) {
            buf_queue = &buf_port->queues[idx];
            if (IS_CPU_PORT(unit, port)) { /* CPU PORT */
                buf_queue->qgroup_min_enable = 0;
                buf_queue->guarantee = default_mtu_cells;
            } else{
                buf_queue->qgroup_min_enable = 1;
                buf_queue->guarantee = 0;
            }
            buf_queue->pool_scale = 8;
            buf_queue->discard_enable = 1;
            buf_queue->color_discard_enable = 0;
            /* resume limits - accounted for 8 cell granularity */
            buf_queue->yellow_limit = 0;
            buf_queue->red_limit = 0;
            buf_queue->pool_resume = 2;
            buf_queue->pool_limit = 0;
            buf_queue->pool_idx = 0;
        }

        /* queue to pool mapping */
        for (idx = 0;
             idx < (si->port_num_cosq[port] + si->port_num_uc_cosq[port]);
             idx++) {
            buf_port->queues[idx].pool_idx = 0;
        }
    }

}

/*! @fn int _soc_th3_mmu_config_thresholds_pool_set(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    _soc_mmu_device_info_t *devcfg,
 *    int lossless)
 *  @param unit Chip unit number.
 *  @param buf Pointer to _soc_mmu_cfg_buf_t structure
 *  @param devcfg Pointer to _soc_mmu_device_info_t structure
 *  @param lossless Indicates if lossy or lossless
 *  @brief Function to setup the default per-pool ingress/egress thresholds
 * Description:
 *      Function used to setup the default ingress and egress admission
 *      threshold limits that are per-pool and have dependencies
 *      between thdi and thdo
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t structure
 *      devcfg - _soc_mmu_device_info_t structure
 *      lossless - Indicates lossy, lossless or lossy+lossless
 * Return Value:
 *      void
 */

STATIC void
_soc_th3_mmu_config_thresholds_pool_set(int unit, _soc_mmu_cfg_buf_t
        *buf,_soc_mmu_device_info_t *devcfg,  int lossless)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    int default_mtu_cells;
    int itm, pipe, pipe_map, idx;
    int num_ports_speed[6], cpu_ports, lb_ports, mgmt_ports;
    int pm, port;
    int pg_headroom[6];
    int total_headroom_itm[_TH3_ITMS_PER_DEV];
    int total_pgmin_itm[_TH3_ITMS_PER_DEV];
    int total_qgrpmin_itm[_TH3_ITMS_PER_DEV];
    int total_qmin_itm[_TH3_ITMS_PER_DEV];
    int pg_lossless;
    int headroom_oversub_cells[6] =
        {_TH3_PG_HEADROOM_OVERSUB_400G,
         _TH3_PG_HEADROOM_OVERSUB_200G,
         _TH3_PG_HEADROOM_OVERSUB_100G,
         _TH3_PG_HEADROOM_OVERSUB_50G,
         _TH3_PG_HEADROOM_OVERSUB_25G,
         _TH3_PG_HEADROOM_OVERSUB_10G};
    int speed;
    int ports[6], num_ports, total_num_ports;

    si = &SOC_INFO(unit);

    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);

    for (itm = 0; itm < _TH3_ITMS_PER_DEV; itm ++) {
        /* THDI - Calculating the total PGMin and total PG Headroom
           per ITM
           if (not flex port)
           Total PGMIN : pgmin * number of ports in ITM
           Total Headroom: Sum of (headroom(speed) * number of ports(speed)
                           * num_active_pg) for all possible port speeds
           if (flex port)
           Total PGMIN : MIN(max ports per PM, Max ports per ITM) * pgmin
           Total Headroom :
        */
        num_ports_speed[0] = num_ports_speed[1] = num_ports_speed[2] = 0;
        num_ports_speed[3] = num_ports_speed[4] = num_ports_speed[5] = 0;
        cpu_ports = lb_ports = mgmt_ports = 0;
        ports[0] = ports[1] = ports[2] = ports[3] = 0;

        PBMP_ALL_ITER(unit, port) {
            pipe_map = si->itm_ipipe_map[itm];
            pipe = si->port_pipe[port];
            if (pipe_map & (1 << pipe)) {
                if (IS_CPU_PORT(unit, port)) {
                    cpu_ports++;
                }
                else if (IS_LB_PORT(unit, port)) {
                    lb_ports++;
                }
                else if (IS_MANAGEMENT_PORT(unit, port)) {
                    mgmt_ports++;
                }
                else {
                    pg_lossless = 0;
                    for (idx = 0; idx < _TH3_MMU_NUM_PG; idx++) {
                        buf_prigroup =
                            &buf->ports[port].prigroups[idx];
                        if (buf_prigroup->lossless)
                            pg_lossless = 1;
                    }
                    if (pg_lossless == 0) {
                        continue;
                    }

                    
                    /* Also what about lossy case */
                    pm =  _soc_th3_get_pm_from_port(unit, port);
                    num_ports = _soc_th3_ports_per_pm_get(unit, pm);
                    if (num_ports == 1) {
                        ports[0] ++;
                    } else if (num_ports == 2) {
                        ports[1] ++;
                    } else if ((num_ports == 4) || (num_ports == 3)) {
                        ports[2] ++;
                    } else if ((num_ports == 8)) {
                        ports[3] ++;
                    }

                    /* Non-flexport case */
                    speed = SOC_INFO(unit).port_speed_max[port];
                    if (speed == 10000) {
                        num_ports_speed[5]++;
                    } else if (speed == 25000) {
                        num_ports_speed[4]++;
                    } else if ((speed == 50000) || (speed == 40000)) {
                        num_ports_speed[3]++;
                    } else if (speed == 100000) {
                        num_ports_speed[2]++;
                    } else if (speed == 200000) {
                        num_ports_speed[1]++;
                    } else if (speed == 400000) {
                        num_ports_speed[0]++;
                    } else {
                        num_ports_speed[5]++;
                    }
                }
            } /* if pipe in itm */
        } /* all ports */

        if ((lossless == 1) || (lossless == 2)) {

            /* pg min for fp, cpu, lb, mgmt, mmu_pg_min = 8 */
            if (si->flex_eligible) {
                pg_headroom[0] = ports[0] * headroom_oversub_cells[0];
                pg_headroom[1] = ports[1] * headroom_oversub_cells[1] * 2;
                pg_headroom[2] = ports[2] * headroom_oversub_cells[2] * 4;
                pg_headroom[3] = ports[3] * headroom_oversub_cells[3] * 8;
                total_headroom_itm[itm] =
                    MIN((pg_headroom[0] + pg_headroom[1] +
                         pg_headroom[2] + pg_headroom[3]),
                        (72-num_ports_speed[0] * headroom_oversub_cells[1]));
                total_pgmin_itm[itm] =
                    (MIN(ports[0] + ports[1]*2 + ports[2]*4 + ports[3]*8, 72)
                    + mgmt_ports + cpu_ports + lb_ports) * default_mtu_cells;
            }
            else {
                pg_headroom[0] = num_ports_speed[0] * headroom_oversub_cells[0];
                pg_headroom[1] = num_ports_speed[1] * headroom_oversub_cells[1];
                pg_headroom[2] = num_ports_speed[2] * headroom_oversub_cells[2];
                pg_headroom[3] = num_ports_speed[3] * headroom_oversub_cells[3];
                pg_headroom[4] = num_ports_speed[4] * headroom_oversub_cells[4];
                pg_headroom[5] = num_ports_speed[5] * headroom_oversub_cells[5];
                total_headroom_itm[itm] = pg_headroom[0] + pg_headroom[1] +
                                     pg_headroom[2] + pg_headroom[3] +
                                     pg_headroom[4] + pg_headroom[5];
                total_pgmin_itm[itm] =  (num_ports_speed[0] +
                                         num_ports_speed[1] +
                                         num_ports_speed[2] +
                                         num_ports_speed[3] +
                                         num_ports_speed[4] +
                                         num_ports_speed[5] +
                                         cpu_ports +
                                         mgmt_ports +
                                         lb_ports) * default_mtu_cells;
            }
        } else {
            total_headroom_itm[itm]= 0;
            total_pgmin_itm[itm] = 0;
        }
    }
    /* total_headroom = MAX(total_headroom_itm[0], total_headroom_itm[1]); */
    /* total_pgmin = MIN(total_pgmin_itm[0], total_pgmin_itm[1]); */
    for (itm = 0; itm < _TH3_ITMS_PER_DEV; itm ++) {
        buf_pool = &buf->pools_itm[itm][0];
        buf_pool->prigroup_headroom = total_headroom_itm[itm];
        buf_pool->prigroup_guarantee = total_pgmin_itm[itm];
    }

    for (itm = 0; itm < _TH3_ITMS_PER_DEV; itm ++) {
        cpu_ports = lb_ports = mgmt_ports = 0;
        ports[0] = ports[1] = ports[2] = ports[3] = 0;
        num_ports_speed[0] = num_ports_speed[1] = num_ports_speed[2] = 0;
        num_ports_speed[3] = num_ports_speed[4] = num_ports_speed[5] = 0;
        ports[0] = ports[1] = ports[2] = ports[3] = 0;
        /* THDO - Egress admission */
        PBMP_ALL_ITER(unit, port) {
            if (IS_CPU_PORT(unit, port)) {
                cpu_ports++;
            }
            else if (IS_LB_PORT(unit, port)) {
                lb_ports++;
            }
            else if (IS_MANAGEMENT_PORT(unit, port)) {
                mgmt_ports++;
            }
            else {
                /* Flex port case */
                pm =  _soc_th3_get_pm_from_port(unit, port);
                num_ports = _soc_th3_ports_per_pm_get(unit, pm);
                if (num_ports == 1) {
                    ports[0] ++;
                } else if (num_ports == 2) {
                    ports[1] ++;
                } else if ((num_ports == 4) || (num_ports == 3)) {
                    ports[2] ++;
                } else if ((num_ports == 8)) {
                    ports[3] ++;
                }

                /* Non-flex port case */
                speed = SOC_INFO(unit).port_speed_max[port];
                if (speed == 10000) {
                    num_ports_speed[5]++;
                } else if (speed == 25000) {
                    num_ports_speed[4]++;
                } else if ((speed == 50000) || (speed == 40000)) {
                    num_ports_speed[3]++;
                } else if (speed == 100000) {
                    num_ports_speed[2]++;
                } else if (speed == 200000) {
                    num_ports_speed[1]++;
                } else if (speed == 400000) {
                    num_ports_speed[0]++;
                } else {
                    num_ports_speed[5]++;
                }
            }
        }
        /* THD0 - Calculating the total QGRPMin
           if (not flex port)
           Total QGrpMin : qgrpmin * number of ports
           if (flex port)
           Total QGrpMin : MIN(max ports per PM, Max ports per ITM) * qgrpmin
        */
        if (si->flex_eligible) {
            total_qgrpmin_itm[itm] =
               (MIN((ports[0] + ports[1] * 2 + ports[2] * 4 + ports[3] * 8), 72)
                + mgmt_ports + lb_ports)
               * (2 * default_mtu_cells);
        }
        else {
            total_num_ports = num_ports_speed[5] + num_ports_speed[4] +
                num_ports_speed[3] + num_ports_speed[2] +
                num_ports_speed[1] + num_ports_speed[0];
            total_qgrpmin_itm[itm] =
                (total_num_ports +
                + mgmt_ports + lb_ports)
                * (2 * default_mtu_cells);
        }

        /* qmin for cpu * number of cpu queues */
        total_qmin_itm[itm] = default_mtu_cells * SOC_TH3_NUM_CPU_QUEUES;
    }


    for (itm = 0; itm < _TH3_ITMS_PER_DEV; itm ++) {
        buf_pool = &buf->pools_itm[itm][0];
        buf_pool->queue_group_guarantee = total_qgrpmin_itm[itm];
        buf_pool->queue_guarantee = total_qmin_itm[itm];
        /* Total qmin reserved for MC_CQE is 0 */
        buf_pool->mcq_entry_reserved = total_qgrpmin_itm[itm];
    } /* itm */

}


/*! @fn void _soc_th3_mmu_config_thresholds_set(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    _soc_mmu_device_info_t *devcfg,
 *    int lossless)
 *  @param unit Chip unit number.
 *  @param buf Pointer to _soc_mmu_cfg_buf_t structure
 *  @param devcfg Pointer to _soc_mmu_device_info_t structure
 *  @param lossless Indicates if lossy or lossless
 *  @brief Function to setup the default ingress/egress admission control
 * Description:
 *      Function used to setup the default ingress and egress admission
 *      threshold limits like min guarantee, shared limits,
 *      color limits, etc.
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t structure
 *      devcfg - _soc_mmu_device_info_t structure
 *      lossless - Indicates lossy, lossless or lossy+lossless
 * Return Value:
 *      void
 */

STATIC void
_soc_th3_mmu_config_thresholds_set(int unit, _soc_mmu_cfg_buf_t
        *buf,_soc_mmu_device_info_t *devcfg,  int lossless)
{
    _soc_th3_mmu_config_thdi_set(unit, buf, devcfg, lossless);
    _soc_th3_mmu_config_thdo_set(unit, buf, devcfg, lossless);
    _soc_th3_mmu_config_thresholds_pool_set(unit, buf, devcfg, lossless);

}

/*! @fn int _soc_th3_mmu_config_thdi_per_itm_set_hw(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    _soc_mmu_device_info_t *devcfg,
 *    int lossless)
 *  @param unit Chip unit number.
 *  @param buf Pointer to _soc_mmu_cfg_buf_t structure
 *  @param devcfg Pointer to _soc_mmu_device_info_t structure
 *  @param lossless Indicates if lossy or lossless
 *  @brief Function to program per ITM Threshold regs/mem
 * Description:
 *      Function used to program per ITM THDI regs/mem
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t structure
 *      devcfg - _soc_mmu_device_info_t structure
 *      lossless - Indicates lossy, lossless or lossy+lossless
 * Return Value:
 *      SOC_E_*
 */

STATIC int
_soc_th3_mmu_config_thdi_per_itm_set_hw(int unit, _soc_mmu_cfg_buf_t *buf,
        _soc_mmu_device_info_t *devcfg, int lossless)
{
    uint32 fval, rval;
    int idx;
    int jumbo_frame_cells;
    int limit, headroom;
    int pool_resume = 0;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    /* int pool_pg_headroom = 0; */

    jumbo_frame_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->jumbo_pkt_size +
                                                    devcfg->mmu_hdr_byte,
                                                    devcfg->mmu_cell_size);

    /* Per-ITM programming */
    rval = 0;
    fval = _TH3_MMU_PHYSICAL_CELLS_PER_ITM - _TH3_MMU_RSVD_CELLS_CFAP_PER_ITM;
    soc_reg_field_set(unit, MMU_CFAP_FULLTHRESHOLDSETr, &rval,
            CFAPFULLSETPOINTf, fval);
    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_set(unit, MMU_CFAP_FULLTHRESHOLDSETr, -1, -1,
                                   -1, rval));

    rval = 0;
    soc_reg_field_set(unit, MMU_CFAP_FULL_RESUME_OFFSETr, &rval,
                      OFFSETf, (2 * jumbo_frame_cells));
    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_set(unit, MMU_CFAP_FULL_RESUME_OFFSETr, -1,
                                   -1,  -1, rval));

    rval = 0;
    soc_reg_field_set(unit, MMU_CFAP_BANKFULLr, &rval, LIMITf,
                        _TH3_CFAP_BANK_FULL_LIMIT);
    SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit, MMU_CFAP_BANKFULLr, -1, -1,
                                       -1, rval));

    /* Input thresholds */
    rval = 0;
    soc_reg_field_set(unit, MMU_THDI_POOL_CONFIGr, &rval,
                   LAST_PKT_ACCEPT_MODEf, _TH3_LAST_PKT_ACCEPT_MODE_POOL_DEFAULT);
    soc_reg_field_set(unit, MMU_THDI_POOL_CONFIGr, &rval,
                   COLOR_AWAREf, _TH3_COLOR_AWARE_DEFAULT);
    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_set(unit, MMU_THDI_POOL_CONFIGr, -1, -1,
                                   -1, rval));

    rval = 0;
    soc_reg_field_set(unit, MMU_THDI_CPU_SPID_OVERRIDE_CTRLr, &rval,
                      ENABLEf, _TH3_SPID_OVERRIDE_ENABLE_DEFAULT);
    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_set(unit, MMU_THDI_CPU_SPID_OVERRIDE_CTRLr,
                                   -1,  -1,  -1, rval));

    rval = 0;
    soc_reg_field_set(unit, MMU_THDI_MIRROR_SPID_OVERRIDE_CTRLr, &rval,
                      ENABLEf, _TH3_SPID_OVERRIDE_ENABLE_DEFAULT);
    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_set(unit, MMU_THDI_MIRROR_SPID_OVERRIDE_CTRLr,
                                   -1,  -1,  -1, rval));

    rval = 0;
    soc_reg_field_set(unit, MMU_THDI_MC_SPID_OVERRIDE_CTRLr, &rval,
                      ENABLEf, _TH3_SPID_OVERRIDE_ENABLE_DEFAULT);
    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_set(unit, MMU_THDI_MC_SPID_OVERRIDE_CTRLr,
                                   -1,  -1, -1, rval));


    /* Per-ITM Per-Pool programming */
    for (idx = 0; idx < _TH3_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools_itm[0][idx];
        limit = _soc_th3_get_shared_limit(unit, buf, idx);
        headroom = _soc_th3_get_hdrm_limit(unit, buf, idx);

        if (buf_pool->total != 0) {
            pool_resume = 2 * jumbo_frame_cells;
        } else {
            pool_resume = 0;
        }

        rval = 0;
        soc_reg_field_set(unit, MMU_THDI_HDRM_BUFFER_CELL_LIMIT_HPr, &rval,
                  LIMITf, headroom);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit,
                MMU_THDI_HDRM_BUFFER_CELL_LIMIT_HPr, -1, -1, idx,  rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDI_BUFFER_CELL_LIMIT_SPr, &rval, LIMITf,
                          limit);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit, MMU_THDI_BUFFER_CELL_LIMIT_SPr,
                                       -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDI_CELL_RESET_LIMIT_OFFSET_SPr, &rval,
                          OFFSETf, pool_resume);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit,
                MMU_THDI_CELL_RESET_LIMIT_OFFSET_SPr, -1,  -1,  idx,  rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDI_CELL_SPAP_YELLOW_OFFSET_SPr, &rval,
                          OFFSETf, _TH3_SPAP_YELLOW_OFFSET_DEFAULT);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit,
                MMU_THDI_CELL_SPAP_YELLOW_OFFSET_SPr, -1,  -1,  idx,  rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDI_CELL_SPAP_RED_OFFSET_SPr, &rval,
                          OFFSETf, _TH3_SPAP_RED_OFFSET_DEFAULT);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit,
                MMU_THDI_CELL_SPAP_RED_OFFSET_SPr,-1,  -1,  idx,  rval));

    }

    return SOC_E_NONE;
}

/*! @fn int _soc_th3_mmu_config_thdi_per_port_set_hw(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    _soc_mmu_device_info_t *devcfg,
 *    int lossless)
 *  @param unit Chip unit number.
 *  @param buf Pointer to _soc_mmu_cfg_buf_t structure
 *  @param devcfg Pointer to _soc_mmu_device_info_t structure
 *  @param lossless Indicates if lossy or lossless
 *  @brief Function to program per PORT/PG Threshold regs/mem
 * Description:
 *      Function used to program per PORT/PG THDI regs/mem
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t structure
 *      devcfg - _soc_mmu_device_info_t structure
 *      lossless - Indicates lossy, lossless or lossy+lossless
 * Return Value:
 *      SOC_E_*
 */

STATIC int
_soc_th3_mmu_config_thdi_per_port_set_hw(int unit, int port,
        _soc_mmu_cfg_buf_t *buf, _soc_mmu_device_info_t *devcfg,
        int lossless, int flex, int shared_limit)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;

    int limit;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    uint32 rval;
    int idx, midx;
    int pipe;
    int pg_lossless;
    int pg_flow_control_enable;
    int pause_enable;

    /* Per Port Per PG fields*/
    mmu_thdi_port_pg_min_config_entry_t thdi_port_pg_min_config_mem;
    mmu_thdi_port_pg_shared_config_entry_t thdi_port_pg_shared_config_mem;
    mmu_thdi_port_pg_resume_config_entry_t thdi_port_pg_resume_config_mem;
    mmu_thdi_port_pg_hdrm_config_entry_t thdi_port_pg_hdrm_config_mem;
    soc_field_t field_pgmin_limit[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_MIN_LIMITf, PG1_MIN_LIMITf, PG2_MIN_LIMITf,
            PG3_MIN_LIMITf, PG4_MIN_LIMITf, PG5_MIN_LIMITf,
            PG6_MIN_LIMITf, PG7_MIN_LIMITf};
    soc_field_t field_use_portsp[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_USE_PORTSP_MINf, PG1_USE_PORTSP_MINf, PG2_USE_PORTSP_MINf,
            PG3_USE_PORTSP_MINf, PG4_USE_PORTSP_MINf, PG5_USE_PORTSP_MINf,
            PG6_USE_PORTSP_MINf, PG7_USE_PORTSP_MINf};
    soc_field_t field_pghdrm_limit[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_HDRM_LIMITf, PG1_HDRM_LIMITf, PG2_HDRM_LIMITf,
            PG3_HDRM_LIMITf, PG4_HDRM_LIMITf, PG5_HDRM_LIMITf,
            PG6_HDRM_LIMITf, PG7_HDRM_LIMITf};
    soc_field_t field_pgshared_limit[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_SHARED_LIMITf, PG1_SHARED_LIMITf, PG2_SHARED_LIMITf,
            PG3_SHARED_LIMITf, PG4_SHARED_LIMITf, PG5_SHARED_LIMITf,
            PG6_SHARED_LIMITf, PG7_SHARED_LIMITf};
    soc_field_t field_pgshared_dynamic[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_SHARED_DYNAMICf, PG1_SHARED_DYNAMICf, PG2_SHARED_DYNAMICf,
            PG3_SHARED_DYNAMICf, PG4_SHARED_DYNAMICf, PG5_SHARED_DYNAMICf,
            PG6_SHARED_DYNAMICf, PG7_SHARED_DYNAMICf};
    soc_field_t field_pgreset_floor[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_RESET_FLOORf, PG1_RESET_FLOORf, PG2_RESET_FLOORf,
            PG3_RESET_FLOORf, PG4_RESET_FLOORf, PG5_RESET_FLOORf,
            PG6_RESET_FLOORf, PG7_RESET_FLOORf};
    soc_field_t field_pgreset_offset[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_RESET_OFFSETf, PG1_RESET_OFFSETf, PG2_RESET_OFFSETf,
            PG3_RESET_OFFSETf, PG4_RESET_OFFSETf, PG5_RESET_OFFSETf,
            PG6_RESET_OFFSETf, PG7_RESET_OFFSETf};

    /*Per Port Per Pool fields */
    soc_field_t field_sp_min[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            PORTSP0_MIN_LIMITf, PORTSP1_MIN_LIMITf,
            PORTSP2_MIN_LIMITf, PORTSP3_MIN_LIMITf};
    soc_field_t field_sp_shared[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            PORTSP0_SHARED_LIMITf, PORTSP1_SHARED_LIMITf,
            PORTSP2_SHARED_LIMITf, PORTSP3_SHARED_LIMITf};
    soc_field_t field_sp_resume[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            PORTSP0_RESUME_LIMITf, PORTSP1_RESUME_LIMITf,
            PORTSP2_RESUME_LIMITf, PORTSP3_RESUME_LIMITf};

    si = &SOC_INFO(unit);
    pipe = si->port_pipe[port];
    buf_port = &buf->ports[port];

    /* Input port settings */
    pg_lossless = 0;
    pg_flow_control_enable = 0;
    for (idx = 0; idx < _TH3_MMU_NUM_PG; idx++) {
        buf_prigroup = &buf->ports[port].prigroups[idx];
        pg_lossless = (pg_lossless | (buf_prigroup->lossless << idx)) & 0xff;
        pg_flow_control_enable = (pg_flow_control_enable |
            (buf_prigroup->flow_control_enable << idx)) & 0xff;
    }


    rval = 0;
    pause_enable = (lossless == 1) ? 1: 0;
    soc_reg_field_set(unit, MMU_THDI_ING_PORT_CONFIGr, &rval,
                      PAUSE_ENABLEf, pause_enable);
    soc_reg_field_set(unit, MMU_THDI_ING_PORT_CONFIGr, &rval,
                      PG_IS_LOSSLESSf, pg_lossless);
    soc_reg_field_set(unit, MMU_THDI_ING_PORT_CONFIGr, &rval,
                      PFC_PG_ENABLEf, pg_flow_control_enable);
    soc_reg_field_set(unit, MMU_THDI_ING_PORT_CONFIGr, &rval,
                      PG_PROFILE_SELf, buf_port->prigroup_profile_idx);
    soc_reg_field_set(unit, MMU_THDI_ING_PORT_CONFIGr, &rval,
                 INPPRI_PROFILE_SELf, buf_port->pri_to_prigroup_profile_idx);
    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_set(unit, MMU_THDI_ING_PORT_CONFIGr,
                                       -1, port, 0, rval));


    /*****************************************
     * Input port per port per pool settings *
 */
    mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORTSP_CONFIGm)[pipe];
    if (mem != INVALIDm) {
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, port,
                 MMU_THDI_PORTSP_CONFIGm, 0);
        sal_memset(&entry, 0,
                    sizeof(entry));
        for (idx = 0; idx < _TH3_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            soc_mem_field32_set(unit, mem, entry,
                    field_sp_min[idx],  buf_port_pool->guarantee);
            soc_mem_field32_set(unit, mem, entry,
                    field_sp_resume[idx],
                    buf_port_pool->pool_resume);
            soc_mem_field32_set(unit, mem, entry,
                    field_sp_shared[idx],
                    buf_port_pool->pool_limit);
        }
        SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                               midx, entry));
    }

    /***************************************************
     * Input port per port per priority group settings *
 */

    limit = _soc_th3_get_shared_limit(unit, buf, 0);

    mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_MIN_CONFIGm)[pipe];
    if (mem != INVALIDm) {
        sal_memset(&thdi_port_pg_min_config_mem, 0,
                sizeof(thdi_port_pg_min_config_mem));
        for (idx = 0; idx < _TH3_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf->ports[port].prigroups[idx];

            midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, port,
                    MMU_THDI_PORT_PG_MIN_CONFIGm,
                                              0);
            soc_mem_field32_set(unit, mem,
                    &thdi_port_pg_min_config_mem, field_pgmin_limit[idx],
                    buf_prigroup->guarantee);
            soc_mem_field32_set(unit, mem,
                    &thdi_port_pg_min_config_mem, field_use_portsp[idx],
                    !buf_prigroup->port_guarantee_enable);
        }
        SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                               midx, &thdi_port_pg_min_config_mem));
    }

    mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_HDRM_CONFIGm)[pipe];
    if (mem != INVALIDm) {
        sal_memset(&thdi_port_pg_hdrm_config_mem, 0,
                sizeof(thdi_port_pg_hdrm_config_mem));
        for (idx = 0; idx < _TH3_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf->ports[port].prigroups[idx];

            midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, port,
                    MMU_THDI_PORT_PG_HDRM_CONFIGm,
                                              0);
            soc_mem_field32_set(unit, mem,
                    &thdi_port_pg_hdrm_config_mem, field_pghdrm_limit[idx],
                    buf_prigroup->headroom);
        }
        SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                               midx, &thdi_port_pg_hdrm_config_mem));
    }

    mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_SHARED_CONFIGm)[pipe];
    if (mem != INVALIDm) {
        sal_memset(&thdi_port_pg_shared_config_mem, 0,
            sizeof(thdi_port_pg_shared_config_mem));
        for (idx = 0; idx < _TH3_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf->ports[port].prigroups[idx];
            buf_prigroup->pool_limit = limit;
            if (flex) {
                buf_prigroup->pool_limit = shared_limit;
            }

            midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, port,
                    MMU_THDI_PORT_PG_SHARED_CONFIGm,
                                              0);
            if (buf_prigroup->pool_scale != -1) {
                soc_mem_field32_set(unit, mem,
                    &thdi_port_pg_shared_config_mem,
                    field_pgshared_limit[idx], buf_prigroup->pool_scale);
                soc_mem_field32_set(unit, mem,
                    &thdi_port_pg_shared_config_mem,
                    field_pgshared_dynamic[idx], 1);
            }
            else {
                soc_mem_field32_set(unit, mem,
                    &thdi_port_pg_shared_config_mem,
                    field_pgshared_limit[idx], buf_prigroup->pool_limit);
                soc_mem_field32_set(unit, mem,
                    &thdi_port_pg_shared_config_mem,
                    field_pgshared_dynamic[idx], 0);
            }
        }
        SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                               midx, &thdi_port_pg_shared_config_mem));
    }

    mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_RESUME_CONFIGm)[pipe];
    if (mem != INVALIDm) {
        sal_memset(&thdi_port_pg_resume_config_mem, 0,
            sizeof(thdi_port_pg_resume_config_mem));
        for (idx = 0; idx < _TH3_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf->ports[port].prigroups[idx];

            midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, port,
                    MMU_THDI_PORT_PG_RESUME_CONFIGm, 0);
            soc_mem_field32_set(unit, mem,
                    &thdi_port_pg_resume_config_mem, field_pgreset_floor[idx],
                    buf_prigroup->pool_floor);
            soc_mem_field32_set(unit, mem,
                    &thdi_port_pg_resume_config_mem, field_pgreset_offset[idx],
                    buf_prigroup->pool_resume);
        }
        SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                               midx, &thdi_port_pg_resume_config_mem));
    }
    return SOC_E_NONE;
}

/*! @fn int _soc_th3_mmu_config_thdo_per_itm_set_hw(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    _soc_mmu_device_info_t *devcfg,
 *    int lossless)
 *  @param unit Chip unit number.
 *  @param buf Pointer to _soc_mmu_cfg_buf_t structure
 *  @param devcfg Pointer to _soc_mmu_device_info_t structure
 *  @param lossless Indicates if lossy or lossless
 *  @brief Function to program per ITM Threshold regs/mem
 * Description:
 *      Function used to program per ITM THDO regs/mem
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t structure
 *      devcfg - _soc_mmu_device_info_t structure
 *      lossless - Indicates lossy, lossless or lossy+lossless
 * Return Value:
 *      SOC_E_*
 */

STATIC int
_soc_th3_mmu_config_thdo_per_itm_set_hw(int unit, _soc_mmu_cfg_buf_t *buf,
                              _soc_mmu_device_info_t *devcfg, int lossless)
{
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    uint32 rval;
    int idx;
    int jumbo_frame_cells;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;
    int limit, resume_limit, mcq_resume_limit;
    int pool_resume = 0;
    /* int pool_pg_headroom = 0; */
    int port;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    mmu_thdo_config_mc_qgroup_entry_t thdo_config_mc_qgroup_mem;

    jumbo_frame_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->jumbo_pkt_size +
                                                    devcfg->mmu_hdr_byte,
                                                    devcfg->mmu_cell_size);
    pool_resume = 2 * jumbo_frame_cells;

    /* Per-ITM programming */

    /* Output thresholds */
    /* SDK-129104: Change Reset values */
    rval = 0;
    soc_reg_field_set(unit, MMU_THDO_SHARED_DB_DYNAMIC_THRESH_CAPr, &rval,
            UC_Q_THRESH_CAPf, _TH3_MMU_TOTAL_CELLS_PER_ITM);
    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_set(unit, MMU_THDO_SHARED_DB_DYNAMIC_THRESH_CAPr,
                                   -1, -1, -1, rval));

    rval = 0;
    soc_reg_field_set(unit, MMU_THDO_MC_SHARED_CQE_DYNAMIC_THRESH_CAPr, &rval,
            MC_Q_THRESH_CAPf, (SOC_TH3_MMU_MCQ_ENTRY_PER_ITM));
    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_set(unit, MMU_THDO_MC_SHARED_CQE_DYNAMIC_THRESH_CAPr,
                                   -1, -1, -1, rval));

    rval = 0;
    soc_reg_field_set(unit, MMU_THDO_SHARED_DB_POOL_CONFIGr, &rval,
            LAST_PKT_ACCEPT_MODEf,  _TH3_LAST_PKT_ACCEPT_MODE_POOL_DEFAULT);
    soc_reg_field_set(unit, MMU_THDO_SHARED_DB_POOL_CONFIGr, &rval,
            POOL_COLOR_LIMIT_ENABLEf, _TH3_COLOR_AWARE_DEFAULT);
    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_set(unit, MMU_THDO_SHARED_DB_POOL_CONFIGr,
                                   -1, -1, -1, rval));

    rval = 0;
    soc_reg_field_set(unit, MMU_THDO_MC_SHARED_CQE_POOL_CONFIGr, &rval,
        LAST_PKT_ACCEPT_MODEf, _TH3_LAST_PKT_ACCEPT_MODE_POOL_DEFAULT);
    soc_reg_field_set(unit, MMU_THDO_MC_SHARED_CQE_POOL_CONFIGr, &rval,
        POOL_COLOR_LIMIT_ENABLEf, _TH3_COLOR_AWARE_DEFAULT);
    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_set(unit,
            MMU_THDO_MC_SHARED_CQE_POOL_CONFIGr,-1,   -1,   -1,  rval));

    /* Per-ITM Per-Pool programming */
    for (idx = 0; idx < _TH3_MMU_NUM_POOL; idx++) {
        limit = _soc_th3_get_shared_limit(unit, buf, idx);
        buf_pool = &buf->pools_itm[0][idx];

        if ((buf_pool->total != 0) &&
            ((limit - pool_resume) > 0)) {
            resume_limit = limit - pool_resume;
        } else {
            resume_limit = 0;
        }
        if ((buf_pool->total_mcq_entry != 0) &&
            ((buf_pool->total_mcq_entry - pool_resume) > 0)) {
            mcq_resume_limit = buf_pool->total_mcq_entry - pool_resume;
        } else {
            mcq_resume_limit = 0;
        }

        rval = 0;
        soc_reg_field_set(unit, MMU_THDO_SHARED_DB_POOL_SHARED_LIMITr, &rval,
                  SHARED_LIMITf, limit);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit,
                MMU_THDO_SHARED_DB_POOL_SHARED_LIMITr, -1, -1, idx,  rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDO_SHARED_DB_POOL_YELLOW_SHARED_LIMITr,
                   &rval, YELLOW_SHARED_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit,
                MMU_THDO_SHARED_DB_POOL_YELLOW_SHARED_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDO_SHARED_DB_POOL_RED_SHARED_LIMITr, &rval,
                  RED_SHARED_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit,
                MMU_THDO_SHARED_DB_POOL_RED_SHARED_LIMITr, -1, -1, idx,  rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDO_SHARED_DB_POOL_RESUME_LIMITr, &rval,
                  RESUME_LIMITf, resume_limit/8);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit,
                MMU_THDO_SHARED_DB_POOL_RESUME_LIMITr, -1, -1, idx,  rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDO_SHARED_DB_POOL_YELLOW_RESUME_LIMITr,
                   &rval, YELLOW_RESUME_LIMITf, resume_limit/8);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit,
                MMU_THDO_SHARED_DB_POOL_YELLOW_RESUME_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDO_SHARED_DB_POOL_RED_RESUME_LIMITr, &rval,
                  RED_RESUME_LIMITf, resume_limit/8);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit,
                MMU_THDO_SHARED_DB_POOL_RED_RESUME_LIMITr, -1, -1, idx,  rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDO_MC_SHARED_CQE_POOL_SHARED_LIMITr, &rval,
                  SHARED_LIMITf, buf_pool->total_mcq_entry);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit,
                MMU_THDO_MC_SHARED_CQE_POOL_SHARED_LIMITr, -1, -1, idx,  rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDO_MC_SHARED_CQE_POOL_YELLOW_SHARED_LIMITr,
                   &rval, YELLOW_SHARED_LIMITf, buf_pool->total_mcq_entry/8);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit,
                MMU_THDO_MC_SHARED_CQE_POOL_YELLOW_SHARED_LIMITr, -1, -1,
                idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDO_MC_SHARED_CQE_POOL_RED_SHARED_LIMITr,
                   &rval, RED_SHARED_LIMITf, buf_pool->total_mcq_entry/8);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit,
                MMU_THDO_MC_SHARED_CQE_POOL_RED_SHARED_LIMITr, -1, -1,
                idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDO_MC_SHARED_CQE_POOL_RESUME_LIMITr, &rval,
                  RESUME_LIMITf, mcq_resume_limit/8);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit,
                MMU_THDO_MC_SHARED_CQE_POOL_RESUME_LIMITr, -1, -1, idx,  rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDO_MC_SHARED_CQE_POOL_YELLOW_RESUME_LIMITr,
                   &rval, YELLOW_RESUME_LIMITf, mcq_resume_limit/8);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit,
                MMU_THDO_MC_SHARED_CQE_POOL_YELLOW_RESUME_LIMITr, -1, -1,
                idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDO_MC_SHARED_CQE_POOL_RED_RESUME_LIMITr,
                   &rval, RED_RESUME_LIMITf, mcq_resume_limit/8);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit,
                MMU_THDO_MC_SHARED_CQE_POOL_RED_RESUME_LIMITr, -1, -1,
                idx, rval));


    }

    mem = MMU_THDO_CONFIG_UC_QGROUP0m;
    if (mem != INVALIDm) {
        PBMP_ALL_ITER(unit, port) {
            queue_grp = &buf->qgroups[port];
            sal_memset(&entry, 0,
                    sizeof(entry));
            soc_mem_field32_set(unit, mem, entry, MIN_LIMITf,
                    queue_grp->guarantee);
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                    SOC_TH3_MMU_CHIP_PORT(unit, port), entry));
        }
    }

    mem = MMU_THDO_CONFIG_MC_QGROUPm;
    if (mem != INVALIDm) {
        PBMP_ALL_ITER(unit, port) {
            queue_grp = &buf->qgroups[port];
            sal_memset(&thdo_config_mc_qgroup_mem, 0,
                    sizeof(thdo_config_mc_qgroup_mem));
            soc_mem_field32_set(unit, mem, &thdo_config_mc_qgroup_mem,
                    MIN_LIMITf, queue_grp->guarantee_mc);
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                        SOC_TH3_MMU_CHIP_PORT(unit, port),
                         &thdo_config_mc_qgroup_mem));

        }
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_th3_mmu_config_thdo_per_port_set_hw(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    _soc_mmu_device_info_t *devcfg,
 *    int lossless)
 *  @param unit Chip unit number.
 *  @param buf Pointer to _soc_mmu_cfg_buf_t structure
 *  @param devcfg Pointer to _soc_mmu_device_info_t structure
 *  @param lossless Indicates if lossy or lossless
 *  @brief Function to program per PORT/PG Threshold regs/mem
 * Description:
 *      Function used to program per PORT/PG THDI regs/mem
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t structure
 *      devcfg - _soc_mmu_device_info_t structure
 *      lossless - Indicates lossy, lossless or lossy+lossless
 * Return Value:
 *      SOC_E_*
 */

STATIC int
_soc_th3_mmu_config_thdo_per_port_set_hw(int unit, int port,
        _soc_mmu_cfg_buf_t *buf, _soc_mmu_device_info_t *devcfg,
        int lossless)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    soc_mem_t mem;
    int base, numq;
    int idx, midx, loop;
    /* int pipe; */
    uint32 entry[SOC_MAX_MEM_WORDS];

    /* Per-Queue variables */
    soc_mem_t mmu_thdo_per_queue_mem[] = {
        MMU_THDO_QUEUE_CONFIGm,
        MMU_THDO_Q_TO_QGRP_MAPD0m,
        MMU_THDO_QUEUE_RESUME_OFFSETm
    };
    int mmu_thdo_per_queue_mem_count = 3;
    soc_field_t field_qspid[12] = {
            QUEUE0_SPIDf, QUEUE1_SPIDf, QUEUE2_SPIDf,
            QUEUE3_SPIDf, QUEUE4_SPIDf, QUEUE5_SPIDf,
            QUEUE6_SPIDf, QUEUE7_SPIDf, QUEUE8_SPIDf,
            QUEUE9_SPIDf, QUEUE10_SPIDf, QUEUE11_SPIDf};


    /* Per port per pool */
    soc_mem_t mmu_thdo_resume_port_mem[] = {
        MMU_THDO_RESUME_PORT_UC0m,
        MMU_THDO_RESUME_PORT_MC0m
    };
    int mmu_thdo_resume_port_mem_count = 2;


    si = &SOC_INFO(unit);

    mem = MMU_THDO_CONFIG_PORT_UC0m;
    if (mem != INVALIDm) {
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, port,
             mem, 0);

        sal_memset(&entry, 0,
            sizeof(entry));
        for (idx = 0; idx < _TH3_MMU_NUM_POOL; idx++) {
            switch (idx) {
                case 0:
                    soc_mem_field32_set(unit, mem, entry,
                        SP0_COLOR_LIMIT_ENABLEf,  0);
                    soc_mem_field32_set(unit, mem, entry,
                        SP0_SHARED_LIMITf,  _TH3_MMU_TOTAL_CELLS_PER_ITM);
                    soc_mem_field32_set(unit, mem, entry,
                        SP0_RED_LIMITf,  _TH3_MMU_TOTAL_CELLS_PER_ITM/8);
                    soc_mem_field32_set(unit, mem, entry,
                        SP0_YELLOW_LIMITf,  _TH3_MMU_TOTAL_CELLS_PER_ITM/8);
                    break;
                case 1:
                    soc_mem_field32_set(unit, mem, entry,
                        SP1_COLOR_LIMIT_ENABLEf,  0);
                    soc_mem_field32_set(unit, mem, entry,
                        SP1_SHARED_LIMITf,  _TH3_MMU_TOTAL_CELLS_PER_ITM);
                    soc_mem_field32_set(unit, mem, entry,
                        SP1_RED_LIMITf,  _TH3_MMU_TOTAL_CELLS_PER_ITM/8);
                    soc_mem_field32_set(unit, mem, entry,
                        SP1_YELLOW_LIMITf,  _TH3_MMU_TOTAL_CELLS_PER_ITM/8);
                    break;
                case 2:
                    soc_mem_field32_set(unit, mem, entry,
                        SP2_COLOR_LIMIT_ENABLEf,  0);
                    soc_mem_field32_set(unit, mem, entry,
                        SP2_SHARED_LIMITf,  _TH3_MMU_TOTAL_CELLS_PER_ITM);
                    soc_mem_field32_set(unit, mem, entry,
                        SP2_RED_LIMITf,  _TH3_MMU_TOTAL_CELLS_PER_ITM/8);
                    soc_mem_field32_set(unit, mem, entry,
                        SP2_YELLOW_LIMITf,  _TH3_MMU_TOTAL_CELLS_PER_ITM/8);
                    break;
                case 3:
                    soc_mem_field32_set(unit, mem, entry,
                        SP3_COLOR_LIMIT_ENABLEf,  0);
                    soc_mem_field32_set(unit, mem, entry,
                        SP3_SHARED_LIMITf,  _TH3_MMU_TOTAL_CELLS_PER_ITM);
                    soc_mem_field32_set(unit, mem, entry,
                        SP3_RED_LIMITf,  _TH3_MMU_TOTAL_CELLS_PER_ITM/8);
                    soc_mem_field32_set(unit, mem, entry,
                        SP3_YELLOW_LIMITf,  _TH3_MMU_TOTAL_CELLS_PER_ITM/8);
                    break;
            }
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                           midx, entry));
    }

    mem = MMU_THDO_CONFIG_PORTSP_MCm;
    if (mem != INVALIDm) {

        for (idx = 0; idx < _TH3_MMU_NUM_POOL; idx++) {
            midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, port,
                              MMU_THDO_CONFIG_PORTSP_MCm, idx);

            sal_memset(&entry, 0,
                    sizeof(entry));
            soc_mem_field32_set(unit, mem, entry,
                    COLOR_LIMIT_ENABLEf,  0);
            soc_mem_field32_set(unit, mem, entry,
                    SP_SHARED_LIMITf,  _TH3_MMU_TOTAL_CELLS_PER_ITM);
            soc_mem_field32_set(unit, mem, entry,
                    SP_RED_LIMITf,  _TH3_MMU_TOTAL_CELLS_PER_ITM/8);
            soc_mem_field32_set(unit, mem, entry,
                    SP_YELLOW_LIMITf,  _TH3_MMU_TOTAL_CELLS_PER_ITM/8);
            SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                   midx, entry));
        }
    }

    for (loop = 0; loop < mmu_thdo_resume_port_mem_count; loop++) {
        mem = mmu_thdo_resume_port_mem[loop];

        if (mem != INVALIDm) {
            for (idx = 0; idx < _TH3_MMU_NUM_POOL; idx++) {
                midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, port,
                              mmu_thdo_resume_port_mem[loop], idx);

                sal_memset(&entry, 0,
                        sizeof(entry));

                soc_mem_field32_set(unit, mem, entry,
                        SHARED_RESUMEf,  _TH3_MMU_TOTAL_CELLS_PER_ITM/8);
                soc_mem_field32_set(unit, mem, entry,
                        RED_RESUMEf,  _TH3_MMU_TOTAL_CELLS_PER_ITM/8);
                soc_mem_field32_set(unit, mem, entry,
                        YELLOW_RESUMEf,  _TH3_MMU_TOTAL_CELLS_PER_ITM/8);
                SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                       midx, entry));
            }
        }
    }

    /* UC + MC */
    numq = si->port_num_uc_cosq[port] + si->port_num_cosq[port];
    if (si->port_num_uc_cosq[port] == 0) {
        base = si->port_cosq_base[port];
    } else {
        base = si->port_uc_cosq_base[port];
    }

    for (loop = 0; loop < mmu_thdo_per_queue_mem_count; loop++) {
        mem = mmu_thdo_per_queue_mem[loop];
        if ((numq != 0) && (mem != INVALIDm)) {
            for (idx = 0; idx < numq; idx++) {
                buf_queue = &buf->ports[port].queues[idx];

                sal_memset(&entry, 0,
                        sizeof(entry));

                if (mem ==  MMU_THDO_QUEUE_CONFIGm) {
                    soc_mem_field32_set(unit, mem, entry,
                                DISABLE_QUEUINGf, 0);
                    soc_mem_field32_set(unit, mem, entry,
                                LIMIT_ENABLEf, buf_queue->discard_enable);
                    soc_mem_field32_set(unit, mem, entry,
                                COLOR_ENABLEf, buf_queue->
                                color_discard_enable);
                    soc_mem_field32_set(unit, mem, entry,
                                COLOR_LIMIT_MODEf, 1);
                    soc_mem_field32_set(unit, mem, entry,
                                MIN_LIMITf, buf_queue->guarantee);
                    soc_mem_field32_set(unit, mem, entry,
                                USE_QGROUP_MINf, buf_queue->qgroup_min_enable);
                    soc_mem_field32_set(unit, mem, entry,
                                SPIDf, buf_queue->pool_idx);
                    if (buf_queue->pool_scale != -1) {
                        soc_mem_field32_set(unit, mem, entry,
                                LIMIT_DYNAMICf, 1);
                        soc_mem_field32_set(unit, mem, entry,
                                SHARED_ALPHAf, buf_queue->pool_scale);
                    } else {
                        soc_mem_field32_set(unit, mem, entry,
                                LIMIT_DYNAMICf, 0);
                        soc_mem_field32_set(unit, mem, entry,
                                SHARED_LIMITf, buf_queue->pool_limit);
                    }
                    soc_mem_field32_set(unit, mem, entry,
                                        LIMIT_YELLOWf, buf_queue->yellow_limit);
                    soc_mem_field32_set(unit, mem, entry,
                                        LIMIT_REDf, buf_queue->red_limit);
                    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                base + idx,  entry));
                }
                if ((mem ==  MMU_THDO_Q_TO_QGRP_MAPD0m) ||
                    (mem ==  MMU_THDO_Q_TO_QGRP_MAPD1m) ||
                    (mem ==  MMU_THDO_Q_TO_QGRP_MAPD2m)) {
                    soc_mem_field32_set(unit, mem, entry,
                                Q_SPIDf, buf_queue->pool_idx);
                    soc_mem_field32_set(unit, mem, entry,
                                QGROUP_VALIDf, buf_queue->qgroup_min_enable);
                    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                        base + idx, entry));
                }
                if (mem ==  MMU_THDO_QUEUE_RESUME_OFFSETm) {
                    soc_mem_field32_set(unit, mem, entry,
                                RESUME_OFFSETf, buf_queue->pool_resume);
                    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                        base + idx, entry));
                }
            } /* numq */
        } /* valid mem */
    } /* all mems */

    mem = MMU_THDO_PORT_QUEUE_SERVICE_POOLm;
    midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, port,
                 MMU_THDO_PORT_QUEUE_SERVICE_POOLm, 0);
    if ((numq != 0) && (mem != INVALIDm)) {
        for (idx = 0; idx < numq; idx++) {
            /* Doesn't apply for CMIC Port */
            if (IS_CPU_PORT(unit, port)) {
                continue;
            }
            buf_queue = &buf->ports[port].queues[idx];

            sal_memset(&entry, 0,
                        sizeof(entry));

            soc_mem_field32_set(unit, mem,
                    entry, field_qspid[idx],
                    buf_queue->pool_idx);
        } /* numq */
    } /* valid mem */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx,
                        entry));

    return SOC_E_NONE;
}

/*! @fn int _soc_th3_mmu_config_thdi_set_hw(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    _soc_mmu_device_info_t *devcfg,
 *    int lossless)
 *  @param unit Chip unit number.
 *  @param buf Pointer to _soc_mmu_cfg_buf_t structure
 *  @param devcfg Pointer to _soc_mmu_device_info_t structure
 *  @param lossless Indicates if lossy or lossless
 *  @brief Function to program Threshold regs/mem
 * Description:
 *      Function used to program THDI regs/mem
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t structure
 *      devcfg - _soc_mmu_device_info_t structure
 *      lossless - Indicates lossy, lossless or lossy+lossless
 * Return Value:
 *      SOC_E_*
 */

STATIC int
_soc_th3_mmu_config_thdi_set_hw(int unit, _soc_mmu_cfg_buf_t *buf,
                              _soc_mmu_device_info_t *devcfg, int lossless)
{
    int port;

    SOC_IF_ERROR_RETURN
        (_soc_th3_mmu_config_thdi_per_itm_set_hw(unit, buf, devcfg,
                                                 lossless));

    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (_soc_th3_mmu_config_thdi_per_port_set_hw(unit, port, buf,
                                                      devcfg, lossless, 0, 0));
    }
    return SOC_E_NONE;
}


/*! @fn int _soc_th3_mmu_config_thdo_set_hw(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    _soc_mmu_device_info_t *devcfg,
 *    int lossless)
 *  @param unit Chip unit number.
 *  @param buf Pointer to _soc_mmu_cfg_buf_t structure
 *  @param devcfg Pointer to _soc_mmu_device_info_t structure
 *  @param lossless Indicates if lossy or lossless
 *  @brief Function to program Threshold regs/mem
 * Description:
 *      Function used to program THDO regs/mem
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t structure
 *      devcfg - _soc_mmu_device_info_t structure
 *      lossless - Indicates lossy, lossless or lossy+lossless
 * Return Value:
 *      SOC_E_*
 */

STATIC int
_soc_th3_mmu_config_thdo_set_hw(int unit, _soc_mmu_cfg_buf_t *buf,
                              _soc_mmu_device_info_t *devcfg, int lossless)
{
    int port;

    SOC_IF_ERROR_RETURN
        (_soc_th3_mmu_config_thdo_per_itm_set_hw(unit, buf, devcfg,
                                                 lossless));

    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (_soc_th3_mmu_config_thdo_per_port_set_hw(unit, port, buf,
                                                      devcfg, lossless));
    }
    return SOC_E_NONE;
}

/*! @fn int _soc_th3_mmu_config_thresholds_set_hw(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    _soc_mmu_device_info_t *devcfg,
 *    int lossless)
 *  @param unit Chip unit number.
 *  @param buf Pointer to _soc_mmu_cfg_buf_t structure
 *  @param devcfg Pointer to _soc_mmu_device_info_t structure
 *  @param lossless Indicates if lossy or lossless
 *  @brief Function to program Threshold regs/mem
 * Description:
 *      Function used to program ingress/egress threshold regs/mem
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t structure
 *      devcfg - _soc_mmu_device_info_t structure
 *      lossless - Indicates lossy, lossless or lossy+lossless
 * Return Value:
 *      void
 */

STATIC int
_soc_th3_mmu_config_thresholds_set_hw(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless)
{
    SOC_IF_ERROR_RETURN(
        _soc_th3_mmu_config_thdi_set_hw(unit, buf, devcfg, lossless));
    SOC_IF_ERROR_RETURN(
        _soc_th3_mmu_config_thdo_set_hw(unit, buf, devcfg, lossless));

    return SOC_E_NONE;

}

/*! @fn int _soc_th3_mmu_cfg_buf_calculate(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    _soc_mmu_device_info_t *devcfg)
 *  @param unit Chip unit number.
 *  @param buf Pointer to _soc_mmu_cfg_buf_t structure
 *  @param devcfg Pointer to _soc_mmu_device_info_t structure
 *  @brief Function to check if config bcm settings are valid
 * Description:
 *      Function to check if config bcm settings are valid
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t structure
 *      devcfg - _soc_mmu_device_info_t structure
 * Return Value:
 *      SUCCESS/FAILURE
 */

STATIC int
_soc_th3_mmu_cfg_buf_calculate(int unit, _soc_mmu_cfg_buf_t *buf,
                           _soc_mmu_device_info_t *devcfg)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_qgroup_t *queue_group;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;

    int port, idx, pool_idx;
    int qgidx, total_usable_buffer;
    uint8 *qgpid;
    int itm_map, itm, pipe;
    int headroom_itm[SOC_MMU_CFG_ITM_MAX][SOC_MMU_CFG_SERVICE_POOL_MAX];
    int pg_min_itm[SOC_MMU_CFG_ITM_MAX][SOC_MMU_CFG_SERVICE_POOL_MAX];
    int port_min_itm[SOC_MMU_CFG_ITM_MAX][SOC_MMU_CFG_SERVICE_POOL_MAX];
    int queue_min_itm[SOC_MMU_CFG_ITM_MAX][SOC_MMU_CFG_SERVICE_POOL_MAX];
    int qgroup_min_itm[SOC_MMU_CFG_ITM_MAX][SOC_MMU_CFG_SERVICE_POOL_MAX];
    int mcq_entry_reserved_itm[SOC_MMU_CFG_ITM_MAX][SOC_MMU_CFG_SERVICE_POOL_MAX];
    int hdrm_pool_id, port_prof;

    si = &SOC_INFO(unit);

    for (idx = 0; idx < SOC_MMU_CFG_SERVICE_POOL_MAX; idx++) {
        for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm ++) {
            queue_min_itm[itm][idx] = 0;
            qgroup_min_itm[itm][idx] = 0;
            pg_min_itm[itm][idx] = 0;
            port_min_itm[itm][idx] = 0;
            headroom_itm[itm][idx] = 0;
            mcq_entry_reserved_itm[itm][idx] = 0;
        }
    }

    total_usable_buffer = _SOC_MMU_CFG_MMU_TOTAL_CELLS(devcfg);

    for (idx = 0; idx < _SOC_MMU_CFG_DEV_POOL_NUM(devcfg); idx++) {
        for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm ++) {
            buf_pool = &buf->pools_itm[itm][idx];
            if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
                continue;
            }
            if (buf_pool->size & _MMU_CFG_BUF_PERCENT_FLAG) {
                buf_pool->total = ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) *
                                    total_usable_buffer) / 10000;
                if (devcfg->flags & SOC_MMU_CFG_F_EGR_MCQ_ENTRY) {
                    buf_pool->total_mcq_entry =
                        ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) *
                                                 devcfg->total_mcq_entry) / 10000;
                }
            }
        }
    }

    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];
        pipe = si->port_pipe[port];

        /* pg headroom and pg min */
        for (idx = 0; idx < _SOC_MMU_CFG_DEV_PG_NUM(devcfg); idx++) {
            buf_prigroup = &buf_port->prigroups[idx];
            port_prof = buf_port->prigroup_profile_idx;
            hdrm_pool_id =
            buf->mapprofiles[port_prof].prigroup_to_headroompool[idx];
            if (buf_prigroup->user_delay != -1 &&
                buf_prigroup->switch_delay != -1) {
                /*
                 * number of max leftever cells =
                 *   port speed (megabits per sec) * 10**6 (megabits to bit) *
                 *   delay (nsec) / 10**9 (nsecs to second) /
                 *   8 (bits per byte) /
                 *   (IPG (12 bytes) + preamble (8 bytes) +
                 *    worse case packet size (145 bytes)) *
                 *   worse case packet size (2 cells)
                 * heandroom =
                 *   mtu (cells) + number of leftover cells
                 */
                buf_prigroup->headroom = buf_prigroup->pkt_size +
                    si->port_speed_max[port] *
                    (buf_prigroup->user_delay + buf_prigroup->switch_delay) *
                    2 / (8 * (12 + 8 + 145) * 1000);
            }
            for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm ++) {
                itm_map = si->itm_ipipe_map[itm];
                if (itm_map & (1 << pipe)) {
                    headroom_itm[itm][hdrm_pool_id] += buf_prigroup->headroom;
                    pg_min_itm[itm][buf_prigroup->pool_idx] += buf_prigroup->guarantee;

                }
            }
        }

        /* port min */
        for (idx = 0; idx < _SOC_MMU_CFG_DEV_POOL_NUM(devcfg); idx++) {
            buf_port_pool = &buf_port->pools[idx];
            for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm++) {
                itm_map = si->itm_ipipe_map[itm];
                if (itm_map & (1 << pipe)) {
                    port_min_itm[itm][idx] += buf_port_pool->guarantee;
                }
            }
        }

        /* queue min */
        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[idx];
            for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm++) {
                if (buf_queue->qgroup_min_enable == 0) {
                    queue_min_itm[itm][buf_queue->pool_idx] += buf_queue->guarantee;
                    if (!IS_CPU_PORT(unit, port)) {
                        mcq_entry_reserved_itm[itm][buf_queue->pool_idx] +=
                            buf_queue->guarantee;
                    }
                }

            }
        }

        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];
            for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm ++) {
                if (buf_queue->qgroup_min_enable == 0) {
                    queue_min_itm[itm][buf_queue->pool_idx] += buf_queue->guarantee;
                }
            }
        }
    }

    qgpid = sal_alloc(sizeof(uint8)*SOC_TH3_MMU_CFG_QGROUP_MAX, "queue2grp");
    if (qgpid == NULL) {
        return SOC_E_MEMORY;
    }

    for (idx = 0; idx < SOC_TH3_MMU_CFG_QGROUP_MAX; idx++) {
        qgpid[idx] = 0;
    }

    /* Check all the queues belonging to queue group use same service pool. */

    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];

        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[idx];

            if (buf_queue->qgroup_min_enable == 0) {
                continue;
            }

            qgidx = SOC_TH3_MMU_CHIP_PORT(unit, port);
            qgpid[qgidx] |= 1 << buf_queue->pool_idx;
        }

        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];

            if (buf_queue->qgroup_min_enable == 0) {
                continue;
            }
            qgidx = SOC_TH3_MMU_CHIP_PORT(unit, port);
            qgpid[qgidx] |= 1 << buf_queue->pool_idx;
        }
    }

    PBMP_ALL_ITER(unit, port) {
        queue_group = &buf->qgroups[port];
        qgidx = SOC_TH3_MMU_CHIP_PORT(unit, port);
        if (qgpid[qgidx] & (qgpid[qgidx] - 1)) {
            LOG_CLI((BSL_META_U(unit,
                   "Queue belonging to same group use different Pools !!")));
            sal_free(qgpid);
            return SOC_E_PARAM;
        }
        for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm++) {
            pool_idx = 0;
            if (qgpid[qgidx] == 0) { /* No qgrp min */
                continue;
            }
            if (qgpid[qgidx] & 1) {
                 pool_idx = 0;
            } else if (qgpid[qgidx] & 2) {
                 pool_idx = 1;
            } else if (qgpid[qgidx] & 4) {
                 pool_idx = 2;
            } else if (qgpid[qgidx] & 8) {
                 pool_idx = 3;
            }
            qgroup_min_itm[itm][pool_idx] += queue_group->guarantee;
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                  "(port:%d, spid:%d) guarantee= %d\n"),
                  port, pool_idx, queue_group->guarantee));
            if (!IS_CPU_PORT(unit, port)) {
                mcq_entry_reserved_itm[itm][pool_idx] +=
                    queue_group->guarantee;
            }
        }
    }

    sal_free(qgpid);

    for (idx = 0; idx < _SOC_MMU_CFG_DEV_POOL_NUM(devcfg); idx++) {
        for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm ++) {
            buf_pool = &buf->pools_itm[itm][idx];
            buf_pool->queue_guarantee = queue_min_itm[itm][idx];
            buf_pool->queue_group_guarantee = qgroup_min_itm[itm][idx];
            buf_pool->prigroup_headroom = headroom_itm[itm][idx];
            buf_pool->prigroup_guarantee = pg_min_itm[itm][idx];
            buf_pool->port_guarantee = port_min_itm[itm][idx];
            buf_pool->mcq_entry_reserved = mcq_entry_reserved_itm[itm][idx];
        }
    }

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit,
                            "MMU buffer usage:\n")));
        for (idx = 0; idx < _SOC_MMU_CFG_DEV_POOL_NUM(devcfg); idx++) {
            for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm ++) {
                buf_pool = &buf->pools_itm[itm][idx];
                if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
                    continue;
                }
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                        "  Itm %d Pool %d total prigroup guarantee: %d\n"),
                         itm, idx, buf_pool->prigroup_guarantee));
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                        "  Itm %d Pool %d total prigroup headroom: %d\n"),
                         itm, idx, buf_pool->prigroup_headroom));
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                        "  Itm %d Pool %d total queue guarantee: %d\n"),
                         itm, idx, buf_pool->queue_guarantee));
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                        "  Itm %d Pool %d total mcq entry reserved: %d\n"),
                         itm, idx, buf_pool->mcq_entry_reserved));
            }
        }
    }

    return SOC_E_NONE;
}

void
_soc_th3_mmu_cfg_buf_read(int unit, _soc_mmu_cfg_buf_t *buf,
                                 _soc_mmu_device_info_t *devcfg)
{
    int itm, idx;
    _soc_mmu_cfg_buf_pool_t *buf_pool;

    _soc_mmu_cfg_buf_read(unit, buf, devcfg);

    for (idx = 0; idx < _TH3_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        for (itm = 0; itm < _TH3_ITMS_PER_DEV; itm ++) {
            sal_memcpy(&buf->pools_itm[itm][idx], buf_pool,
                sizeof(_soc_mmu_cfg_buf_pool_t));
        }
    }
}

/*! @fn int _soc_th3_mmu_cfg_buf_check(
 *    int unit,
 *    _soc_mmu_cfg_buf_t *buf,
 *    _soc_mmu_device_info_t *devcfg,
 *    int lossless)
 *  @param unit Chip unit number.
 *  @param buf Pointer to _soc_mmu_cfg_buf_t structure
 *  @param devcfg Pointer to _soc_mmu_device_info_t structure
 *  @param lossless Indicates if lossy or lossless
 *  @brief Function to check if config bcm settings are valid
 * Description:
 *      Function to check if config bcm settings are valid
 * Parameters:
 *      unit - Device number
 *      buf - _soc_mmu_cfg_buf_t structure
 *      devcfg - _soc_mmu_device_info_t structure
 *      lossless - Indicates lossy, lossless or lossy+lossless
 * Return Value:
 *      SUCCESS/FAILURE
 */

STATIC int
_soc_th3_mmu_cfg_buf_check(int unit, _soc_mmu_cfg_buf_t *buf,
              _soc_mmu_device_info_t *devcfg, int lossless)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_mapping_profile_t *buf_map_profiles;
    int yellow_cells, red_cells;
    int profile_idx, port, idx;
    uint32 pool_map;
    int limit;

    si = &SOC_INFO(unit);

    SOC_IF_ERROR_RETURN(_soc_th3_mmu_cfg_buf_calculate(unit, buf, devcfg));

    pool_map = 0;

    for (idx = 0; idx < _TH3_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools_itm[0][idx];
        if (buf_pool->total != 0) {
            pool_map |= 1 << idx;
        }

        if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
            continue;
        }

        limit = _soc_th3_get_shared_limit(unit, buf, idx);
        if (limit <= 0) {
            LOG_CLI((BSL_META_U(unit,
                                "Pool %d has no shared space after "
                                "deducting guaranteed !!"), idx));
            return SOC_E_RESOURCE;
        }

        if (buf_pool->yellow_size & _MMU_CFG_BUF_PERCENT_FLAG) {
            yellow_cells = (buf_pool->yellow_size & ~_MMU_CFG_BUF_PERCENT_FLAG) *
                buf_pool->total / 10000;
        } else {
            yellow_cells = buf_pool->yellow_size;
        }
        if (buf_pool->red_size & _MMU_CFG_BUF_PERCENT_FLAG) {
            red_cells = (buf_pool->red_size & ~_MMU_CFG_BUF_PERCENT_FLAG) *
                buf_pool->total / 10000;
        } else {
            red_cells = buf_pool->red_size;
        }

        if (yellow_cells > red_cells) {
            LOG_CLI((BSL_META_U(unit,
                                "MMU config pool %d: Yellow cells offset is higher "
                                "than red cells\n"), idx));
        }
        if (red_cells > buf_pool->total) {
            LOG_CLI((BSL_META_U(unit,
                                "MMU config pool %d: Red cells offset is higher "
                                "than pool shared cells\n"), idx));
        }
    }

    for (profile_idx = 0; profile_idx < SOC_MMU_CFG_NUM_PROFILES_MAX;
        profile_idx++) {
        buf_map_profiles = &buf->mapprofiles[profile_idx];

        if (buf_map_profiles->valid == 0) {
            continue;
        }

        /* input priority to priority group mapping for uc*/
        for (idx = 0; idx < SOC_MMU_CFG_INT_PRI_MAX; idx++) {
            if ((buf_map_profiles->inpri_to_prigroup_uc[idx] < 0) ||
                (buf_map_profiles->inpri_to_prigroup_uc[idx] >= SOC_MMU_CFG_PRI_GROUP_MAX)) {
                LOG_CLI((BSL_META_U(unit,
                                    "MMU config profile(%d) input priority %d: "
                                    "priority group (%d)\n"),
                         profile_idx, idx, buf_map_profiles->inpri_to_prigroup_uc[idx]));
                /* use default value */
                _soc_th3_mmu_config_profiles_pri_to_pg_default_set(unit, buf,
                    devcfg, profile_idx, idx, lossless, 1);
            }
        }

        /* input priority to priority group mapping for mc*/
        for (idx = 0; idx < SOC_MMU_CFG_INT_PRI_MAX; idx++) {
            if ((buf_map_profiles->inpri_to_prigroup_mc[idx] < 0) ||
                (buf_map_profiles->inpri_to_prigroup_mc[idx] >= SOC_MMU_CFG_PRI_GROUP_MAX)) {
                LOG_CLI((BSL_META_U(unit,
                                    "MMU config profile(%d) input priority %d: "
                                    "priority group (%d)\n"),
                         profile_idx, idx, buf_map_profiles->inpri_to_prigroup_mc[idx]));
                /* use default value */
                _soc_th3_mmu_config_profiles_pri_to_pg_default_set(unit, buf,
                    devcfg, profile_idx, idx, lossless, 0);
            }
        }

        /* priority group to pool mapping */
        for (idx = 0; idx < SOC_MMU_CFG_PRI_GROUP_MAX; idx++) {
            if ((buf_map_profiles->prigroup_to_servicepool[idx] < 0) ||
                (buf_map_profiles->prigroup_to_servicepool[idx] >= _TH3_MMU_NUM_POOL)) {
                LOG_CLI((BSL_META_U(unit,
                                    "MMU config profile %d prigroup %d: "
                                    "Invalid pool value (%d)\n"),
                         profile_idx, idx, buf_map_profiles->prigroup_to_servicepool[idx]));
                /* Set to default service pool */
                buf_map_profiles->prigroup_to_servicepool[idx] = 0;
            } else if (!(pool_map & (1 << buf_map_profiles->prigroup_to_servicepool[idx]))) {
                LOG_CLI((BSL_META_U(unit,
                                    "MMU config profile %d prigroup %d: "
                                    "Service Pool %d has no space and cannot be assigned\n"),
                         profile_idx, idx, buf_map_profiles->prigroup_to_servicepool[idx]));
                /* Set to default service pool */
                buf_map_profiles->prigroup_to_servicepool[idx] = 0;
            }

            if ((buf_map_profiles->prigroup_to_headroompool[idx] < 0) ||
                (buf_map_profiles->prigroup_to_headroompool[idx] >= _TH3_MMU_NUM_POOL)) {
                LOG_CLI((BSL_META_U(unit,
                                    "MMU config profile %d prigroup %d: "
                                    "Invalid pool value (%d)\n"),
                         profile_idx, idx, buf_map_profiles->prigroup_to_headroompool[idx]));
                /* Set to default headroompool pool */
                buf_map_profiles->prigroup_to_headroompool[idx] = 0;
            }
        }
    }

    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];

        /* profile for input priority to priority group mapping */
        if ((buf_port->pri_to_prigroup_profile_idx < 0) ||
            (buf_port->pri_to_prigroup_profile_idx >= SOC_MMU_CFG_NUM_PROFILES_MAX)) {
            LOG_CLI((BSL_META_U(unit,
                "MMU config port %d: Invalid profile index(%d) for input priority "
                "to priority group mapping\n"),
                     port, buf_port->pri_to_prigroup_profile_idx));
            buf_port->pri_to_prigroup_profile_idx = 0;
        }

        /* profile for priority group to pool mapping */
        if ((buf_port->prigroup_profile_idx < 0) ||
            (buf_port->prigroup_profile_idx >= SOC_MMU_CFG_NUM_PROFILES_MAX)) {
            LOG_CLI((BSL_META_U(unit,
                "MMU config port %d: Invalid profile index(%d) for priority group"
                "to pool mapping\n"),
                     port, buf_port->prigroup_profile_idx));
            buf_port->prigroup_profile_idx = 0;
        }

        /* queue to pool mapping */
        for (idx = 0;
                idx < (si->port_num_uc_cosq[port] + si->port_num_cosq[port]);
                idx++) {
            buf_queue = &buf->ports[port].queues[idx];

            if (buf_queue->pool_idx < 0 ||
                buf_queue->pool_idx >= _SOC_MMU_CFG_DEV_POOL_NUM(devcfg)) {
                LOG_CLI((BSL_META_U(unit,
                                    "MMU config port %d idx %d: "
                         "Invalid pool value (%d)\n"),
                         port, idx, buf_queue->pool_idx));
                buf_queue->pool_idx = 0;
            } else if (!(pool_map & (1 << buf_queue->pool_idx))) {
                LOG_CLI((BSL_META_U(unit,
                    "MMU config port %d idx %d: Pool %d has no space and cannot be assigned\n"),
                         port, idx, buf_queue->pool_idx));
                buf_queue->pool_idx = 0;
            }
        }
    }

    return SOC_E_NONE;


}


/*! @fn int soc_th3_mmu_config_init_thresholds(
 *    int unit,
 *    int test_only)
 *  @param unit Chip unit number.
 *  @param test_only
 *  @brief Function to initialize mmu thresholds to default values
 * Description:
 *      Function to initialize mmu thresholds to default values
 * Parameters:
 *      unit - Device number
 *      test_only - Test only
 * Return Value:
 *      SOC_E_*
 */

int
soc_th3_mmu_config_init_thresholds(int unit, int test_only)
{
    int rv;
    int lossless = 1;
    _soc_mmu_cfg_buf_t *buf;
    _soc_mmu_device_info_t devcfg;

    buf = soc_mmu_cfg_alloc(unit);
    if (!buf) {
        return SOC_E_MEMORY;
    }

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS,
            SOC_TH3_MMU_LOSSLESS_DEFAULT_DISABLE);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "MMU config init : lossless=%d \n"), lossless));

    _soc_th3_mmu_init_dev_config(unit, &devcfg, lossless);

    /* Default config */
    _soc_th3_mmu_config_mapping_profiles_set(unit, buf, &devcfg, lossless);
    _soc_th3_mmu_config_thresholds_set(unit, buf, &devcfg, lossless);


    if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
        _soc_th3_mmu_cfg_buf_read(unit, buf, &devcfg);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO override done\n")));
    }
    rv = _soc_th3_mmu_cfg_buf_check(unit, buf, &devcfg, lossless);
    if (!test_only) {
        if (SOC_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "MMU config: Use default setting\n")));
            _soc_th3_mmu_config_mapping_profiles_set(unit, buf, &devcfg, lossless);
            _soc_th3_mmu_config_thresholds_set(unit, buf, &devcfg, lossless);
        }
        /* _soc_th3_mmu_config_buf_calculate(unit, buf, &devcfg, lossless); */
        rv = _soc_th3_mmu_config_mapping_profiles_set_hw(unit, buf, &devcfg,
             lossless);
        SOC_IF_ERROR_RETURN(
            _soc_th3_mmu_config_thresholds_set_hw(unit, buf, &devcfg, lossless));
    }

    soc_mmu_cfg_free(unit, buf);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO init done\n")));

    return rv;
}


/*! @fn int soc_th3_mmu_config_flex_thresholds(
 *    int unit,
 *    int test_only)
 *  @param unit Chip unit number.
 *  @param test_only
 *  @brief Function to initialize mmu thresholds to default values
 * Description:
 *      Function to initialize mmu thresholds to default values
 * Parameters:
 *      unit - Device number
 *      test_only - Test only
 * Return Value:
 *      SOC_E_*
 */

int
soc_th3_mmu_config_flex_thresholds(int unit, int port, int shared_limit)
{
    int rv;
    int lossless = 1;
    _soc_mmu_cfg_buf_t *buf;
    _soc_mmu_device_info_t devcfg;

    buf = soc_mmu_cfg_alloc(unit);
    if (!buf) {
        return SOC_E_MEMORY;
    }

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS,
            SOC_TH3_MMU_LOSSLESS_DEFAULT_DISABLE);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "MMU config init : lossless=%d \n"), lossless));

    _soc_th3_mmu_init_dev_config(unit, &devcfg, lossless);

    /* Default config */
    _soc_th3_mmu_config_mapping_profiles_set(unit, buf, &devcfg, lossless);
    _soc_th3_mmu_config_thresholds_set(unit, buf, &devcfg, lossless);


    if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
        _soc_th3_mmu_cfg_buf_read(unit, buf, &devcfg);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO override done\n")));
    }
    rv = _soc_th3_mmu_cfg_buf_check(unit, buf, &devcfg, lossless);
    if (SOC_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "MMU config: Use default setting\n")));
        _soc_th3_mmu_config_mapping_profiles_set(unit, buf, &devcfg, lossless);
        _soc_th3_mmu_config_thresholds_set(unit, buf, &devcfg, lossless);
    }
    SOC_IF_ERROR_RETURN(
        _soc_th3_mmu_config_thdi_per_port_set_hw(unit, port, buf,
                                                  &devcfg, lossless, 1, shared_limit));
    SOC_IF_ERROR_RETURN(
        _soc_th3_mmu_config_thdo_per_port_set_hw(unit, port, buf,
                                                      &devcfg, lossless));

    soc_mmu_cfg_free(unit, buf);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO flex done\n")));

    return rv;
}

/*! @fn int soc_th3_flex_reconf_thresholds(
 *    int unit,
 *    int port)
 *  @param unit Chip unit number.
 *  @param port Logical port number.
 *  @brief Function to initialize mmu thresholds to default values
 * Description:
 *      Function to reconfigure MMU thresholds for the Flexed port.
 * Return Value:
 *      SOC_E_*
 */
int
soc_th3_flex_reconf_thresholds(int unit, int port, int pipe) {

    int i, itm = -1, shd_size[2];
    soc_info_t *si = &SOC_INFO(unit);
    uint32 itm_map;

    for (i = 0; i < NUM_ITM(unit); i++) {
        itm_map = si->itm_ipipe_map[i];
        if (itm_map & (1 << pipe)) {
            itm = i;
            break;
        }
    }

    if (itm < 0 || itm > NUM_ITM(unit)) {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN(soc_th3_mmu_get_shared_size(unit, shd_size));
    SOC_IF_ERROR_RETURN(
        soc_th3_mmu_config_flex_thresholds(unit, port, shd_size[itm]));

    return SOC_E_NONE;
}

/*
 * Check and set threshold to register
 *
 * Parameter:
 *   reg/mem    - reg or mem
 *   field      - threshold field
 *   unique     - if the acc_type is unique or duplicate
 *   itm        - if unique, inidicate the itm #
 *   val        - threshold value
 *   decrease   - 1: only new val < current val will be set to reg
 *                0: only new val > current val will be set to reg
 *   chk_zero   - only non-zero val could be set to reg
 */
int
_soc_th3_mmu_config_shared_limit_chk_set(int unit,
        soc_reg_t reg, soc_mem_t mem, soc_field_t field, int index,
        int itm, uint32 val, int decrease, int chk_zero)
{
    uint32 rval, entry[SOC_MAX_MEM_WORDS];
    uint32 cur_val;

    if (reg != INVALIDr) {
        rval = 0;
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_itm_reg32_get(unit, reg, itm, -1, index, &rval));

        cur_val = soc_reg_field_get(unit, reg, rval, field);
        if (((!chk_zero) || (chk_zero && (cur_val != 0))) &&
            TH3_MMU_SHARED_LIMIT_CHK(cur_val, val, decrease)) {
            soc_reg_field_set(unit, reg, &rval, field, val);
            SOC_IF_ERROR_RETURN
                (soc_tomahawk3_itm_reg32_set(unit, reg, itm, -1, index, rval));
        }
    } else if (mem != INVALIDm) {
        sal_memset(entry, 0, sizeof(entry));
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_itm_mem_read(unit, mem, itm, -1,
                MEM_BLOCK_ALL, index, entry));

        cur_val = soc_mem_field32_get(unit, mem, entry, field);
        if (((!chk_zero) || (chk_zero && (cur_val != 0))) &&
            TH3_MMU_SHARED_LIMIT_CHK(cur_val, val, decrease)) {
            soc_mem_field32_set(unit, mem, entry, field, val);
            SOC_IF_ERROR_RETURN
                (soc_tomahawk3_itm_mem_write(unit, mem, itm, -1,
                    MEM_BLOCK_ALL, index, entry));
        }
    } else {
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}


int soc_th3_mmu_config_res_limits_update(int unit, int *delta,
                                         int pool, int post_update)
{
    soc_info_t *si;
    soc_reg_t reg;
    soc_field_t field;
    int limit[_TH3_ITMS_PER_DEV] = {0};
    int new_limit, itm;
    int resume_limit, pool_resume;
    uint32 rval;
    int cur_val;

    si = &SOC_INFO(unit);
    pool_resume = 2 * _TH3_MMU_JUMBO_PACKET_SIZE;

    /* THDI: Per pool shared settings */
    for (itm = 0; itm < _TH3_ITMS_PER_DEV; itm ++) {
        limit[itm] = _TH3_MMU_TOTAL_CELLS_PER_ITM;
        if (delta[itm] != 0) {
            if (si->itm_map & (1 << itm)) {
                reg = MMU_THDI_BUFFER_CELL_LIMIT_SPr;
                field = LIMITf;
                rval = 0;
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk3_itm_reg32_get(unit, reg, itm, -1, pool, &rval));

                cur_val = soc_reg_field_get(unit, reg, rval, field);
                limit[itm] = cur_val - delta[itm];
                if (limit[itm] < 0) {
                    return SOC_E_RESOURCE;
                }
            }
        }
    }

    /* Get the min of the 2 itm limits */
    new_limit = MIN(limit[0], limit[1]);

    for (itm = 0; itm < _TH3_ITMS_PER_DEV; itm ++) {
        if (si->itm_map & (1 << itm)) {
            reg = MMU_THDI_BUFFER_CELL_LIMIT_SPr;
            field = LIMITf;
            SOC_IF_ERROR_RETURN
                (_soc_th3_mmu_config_shared_limit_chk_set
                    (unit, reg, INVALIDm, field, pool, itm, new_limit,
                     post_update, 1));
        }
    }

    /* THDO - Shared settings per Pool */
    for (itm = 0; itm < _TH3_ITMS_PER_DEV; itm ++) {
        limit[itm] = _TH3_MMU_TOTAL_CELLS_PER_ITM;
        if (delta[itm] != 0) {
            reg = MMU_THDO_SHARED_DB_POOL_SHARED_LIMITr;
            field = SHARED_LIMITf;
            rval = 0;
            SOC_IF_ERROR_RETURN
                (soc_tomahawk3_itm_reg32_get(unit, reg, itm, -1, pool, &rval));
            cur_val = soc_reg_field_get(unit, reg, rval, field);
            limit[itm] = cur_val - delta[itm];
            if (limit[itm] < 0) {
                return SOC_E_RESOURCE;
            }
        }
    }

    /* Get the min of the 2 itm limits */
    new_limit = MIN(limit[0], limit[1]);

    reg = MMU_THDO_SHARED_DB_POOL_SHARED_LIMITr;
    field = SHARED_LIMITf;
    SOC_IF_ERROR_RETURN
        (_soc_th3_mmu_config_shared_limit_chk_set
            (unit, reg, INVALIDm, field, pool, -1, new_limit,
             post_update, 1));

    reg = MMU_THDO_SHARED_DB_POOL_YELLOW_SHARED_LIMITr;
    field = YELLOW_SHARED_LIMITf;
    SOC_IF_ERROR_RETURN
        (_soc_th3_mmu_config_shared_limit_chk_set
            (unit, reg, INVALIDm, field, pool, -1, new_limit/8,
             post_update, 1));

    reg = MMU_THDO_SHARED_DB_POOL_RED_SHARED_LIMITr;
    field = RED_SHARED_LIMITf;
    SOC_IF_ERROR_RETURN
        (_soc_th3_mmu_config_shared_limit_chk_set
            (unit, reg, INVALIDm, field, pool, -1, new_limit/8,
             post_update, 1));

    if ((new_limit != 0) && (new_limit > pool_resume)) {
        resume_limit = new_limit - pool_resume;
    } else {
        resume_limit = 0;
    }

    reg = MMU_THDO_SHARED_DB_POOL_RESUME_LIMITr;
    field = RESUME_LIMITf;
    SOC_IF_ERROR_RETURN
        (_soc_th3_mmu_config_shared_limit_chk_set
            (unit, reg, INVALIDm, field, pool, -1, resume_limit/8,
             post_update, 1));

    reg = MMU_THDO_SHARED_DB_POOL_YELLOW_RESUME_LIMITr;
    field = YELLOW_RESUME_LIMITf;
    SOC_IF_ERROR_RETURN
        (_soc_th3_mmu_config_shared_limit_chk_set
            (unit, reg, INVALIDm, field, pool, -1, resume_limit/8,
             post_update, 1));

    reg = MMU_THDO_SHARED_DB_POOL_RED_RESUME_LIMITr;
    field = RED_RESUME_LIMITf;
    SOC_IF_ERROR_RETURN
        (_soc_th3_mmu_config_shared_limit_chk_set
            (unit, reg, INVALIDm, field, pool, -1, resume_limit/8,
             post_update, 1));

    return SOC_E_NONE;

}


int soc_th3_cal_egress_rsvd_limit(int unit, int* total_egr_rsvd_limit) {
    int rsvd_limit = 0;
    soc_mem_t mem;
    int port, numq, base, idx;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int qgroup_valid;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    PBMP_ALL_ITER(unit, port) {
        /* UC + MC */
        numq = si->port_num_uc_cosq[port] + si->port_num_cosq[port];
        if (si->port_num_uc_cosq[port] == 0) {
            base = si->port_cosq_base[port];
        } else {
            base = si->port_uc_cosq_base[port];
        }

        if (numq != 0) {
            for (idx = 0; idx < numq; idx++) {
                mem = MMU_THDO_QUEUE_CONFIGm;
                sal_memset(&entry, 0,
                        sizeof(entry));
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                base + idx, entry));
                if (!soc_mem_field32_get(unit, mem, entry, USE_QGROUP_MINf)) {
                    qgroup_valid = 0;
                    rsvd_limit += soc_mem_field32_get(unit, mem, entry, MIN_LIMITf);
                } else {
                    qgroup_valid = 1;
                }
                if (qgroup_valid == 1) {
                    if (idx < _soc_th3_get_num_ucq(unit)) {
                        mem = MMU_THDO_CONFIG_UC_QGROUP0m;
                        SOC_IF_ERROR_RETURN
                            (soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                        port, entry));
                        rsvd_limit += soc_mem_field32_get(unit, mem, entry, MIN_LIMITf);
                        break;
                    } else {
                        mem = MMU_THDO_CONFIG_MC_QGROUPm;
                        SOC_IF_ERROR_RETURN
                            (soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                        port, entry));
                        rsvd_limit += soc_mem_field32_get(unit, mem, entry, MIN_LIMITf);
                        break;
                    }
                }
            }
        }
    }

    total_egr_rsvd_limit[0] = rsvd_limit;
    total_egr_rsvd_limit[1] = rsvd_limit;

    return SOC_E_NONE;
}


/*
 * Inputs:
 *          Cable length (in meters)
 *          Packet size ditribution structure
 *          Packet in the way (MTU size)
 */
/*
 * Fixed attributes:
 *                  IPG size: 20 bytes
 *                  PFC frame size: 64 Bytes
 *                  Device specific per-port speed params for PFC response time,
 *                  tx/rx delay and additional buffer
 */
/* Fixed attributes for PG headroom calculation */
#define IPG_SIZE_BYTES    20
#define PFC_FRAME_SIZE_BYTES  64
typedef struct _soc_mmu_pfc_speed_attrs_s
{
    int port_speed;
    int pfc_response; /* PFC response in PQs */
    int tx_rx_delay; /* Max delay in ns */
    int max_add_buffer; /* Max additional buffer in bytes */
} _soc_mmu_pfc_speed_attrs_t;

_soc_mmu_pfc_speed_attrs_t th3_pfc_speed_attrs[7] =
{
    { 10000,    36,  454,   22342},
    { 25000,   110,  454,   22342},
    { 40000,   175,  454,   22342},
    { 50000,   175,  454,   22342},
    {100000,   339,  364,   51166},
    {200000,   693,  359,  101854},
    {400000,  1385,  280,  203708}
};


static int
_soc_th3_get_pfc_port_speed_attrs (int unit, int port_speed, int *pfc_response,
        int *tx_rx_delay, int *max_add_buffer) {
    int i, speed_found = 0;

    for (i = 0; i < (sizeof(th3_pfc_speed_attrs) /
                sizeof(_soc_mmu_pfc_speed_attrs_t)); i++) {
        if (th3_pfc_speed_attrs[i].port_speed == port_speed) {
            *pfc_response = th3_pfc_speed_attrs[i].pfc_response;
            *tx_rx_delay = th3_pfc_speed_attrs[i].tx_rx_delay;
            *max_add_buffer = th3_pfc_speed_attrs[i].max_add_buffer;
            speed_found = 1;
            break;
        }
    }
    if (!speed_found) {
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

int
soc_th3_get_port_pg_headroom (int unit, int port_speed,
        int cable_length, int array_size, _soc_pkt_size_dist_t *pkt_size_dst,
        int *pg_hdrm) {
    unsigned int mtu = 0,  pfc_tx_dly = 0, pfc_resp_time = 0,
        max_tx_rx_dly = 0;
    unsigned int  last_packet = 0;
    unsigned long pkt_dist_num = 0, pkt_dist_denom = 1;
    int port_pq = 0, i, speed_txrx_dly = 0;
    unsigned int max_pkt_size_dist = 0;
    unsigned int mult_size_dist, packet_bytes;
    unsigned long sum_delays = 0;
    unsigned long cable_rtt = 0, sum_factor = 0;
    unsigned long tot_inflight = 0;
    int addl_buf = 0;
    int total_dist_perc = 0;

    /* Check for sum of packet distribution perc to be 100 */
    for (i = 0; i < array_size; i++) {
        total_dist_perc += pkt_size_dst[i].dist_perc;
    }
    if (total_dist_perc != 100) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(
            _soc_th3_get_pfc_port_speed_attrs(unit, port_speed, &port_pq,
                &speed_txrx_dly, &addl_buf));
    /* Derive inflight traffic components */
    /* Packet in the way: Ethernet MTU (worst case) */
    mtu = soc_property_get(unit, spn_MAX_MTU_SIZE, 9216);
    /* RTT for 10G, 100m is 1250 */
    cable_rtt = CEIL(((1250  * cable_length * (port_speed/10000))/ 100), 1);
    /* PFC mesage tx delay */
    pfc_tx_dly = (PFC_FRAME_SIZE_BYTES + IPG_SIZE_BYTES);
    /* PFC response time - Pause quanta based on port speed */
    pfc_resp_time = port_pq * 64;

    /* Switch delay based on port_speed */
    max_tx_rx_dly = CEIL((speed_txrx_dly * (port_speed / 1000) / 8), 1);

    /* Retrieve max pkt size in the distribution */
    for (i = 0; i < array_size; i++) {
        if (pkt_size_dst[i].dist_perc > 0) {
            max_pkt_size_dist = MAX(max_pkt_size_dist,
                    pkt_size_dst[i].pkt_size);
        }
    }
    /* Last packet in transmission based on packet distribution */
    last_packet = _MMU_CFG_MMU_BYTES_TO_CELLS(max_pkt_size_dist +
                                              _TH3_MMU_PACKET_HEADER_BYTES,
                                              _TH3_MMU_BYTES_PER_CELL) *
                                              _TH3_MMU_BYTES_PER_CELL;

    /* Retrieve numerator and denominator of pkt size distrbution factor */
    for (i = 0; i < array_size; i++) {
        if (pkt_size_dst[i].dist_perc > 0) {
            mult_size_dist = pkt_size_dst[i].pkt_size *
                    pkt_size_dst[i].dist_perc;
            packet_bytes = (_MMU_CFG_MMU_BYTES_TO_CELLS(
                        pkt_size_dst[i].pkt_size +
                        _TH3_MMU_PACKET_HEADER_BYTES,
                        _TH3_MMU_BYTES_PER_CELL) *
                        _TH3_MMU_BYTES_PER_CELL);
            pkt_dist_num += CEIL((packet_bytes * mult_size_dist) /
                (pkt_size_dst[i].pkt_size + IPG_SIZE_BYTES), 1) ;
            if (pkt_dist_denom == 1) {
                pkt_dist_denom = mult_size_dist;
            } else {
                pkt_dist_denom += mult_size_dist;
            }
        }
    }

    sum_delays = (mtu + cable_rtt + pfc_tx_dly + pfc_resp_time +
            max_tx_rx_dly);
    sum_factor = (sum_delays * pkt_dist_num) / pkt_dist_denom;
    tot_inflight = sum_factor + addl_buf + last_packet;

    *pg_hdrm = CEIL((tot_inflight / _TH3_MMU_BYTES_PER_CELL), 1);

    LOG_VERBOSE(BSL_LS_SOC_MMU,
          (BSL_META_U(unit,
          "mtu: %u, cable_rtt: %lu, pfc_tx_dly : %u, pfc_resp_time : %u,"
          "max_tx_rx_dly : %u, addl_buf: %u, last_packet: %u \n"),
          mtu, cable_rtt, pfc_tx_dly, pfc_resp_time, max_tx_rx_dly, addl_buf,
          last_packet));
        LOG_VERBOSE(BSL_LS_SOC_MMU,
          (BSL_META_U(unit,
          "pkt_dist_num: %lu, pkt_dist_denom: %lu, sum_delays : %lu,"
          "sum_factor: %lu,"
          "tot_inflight: %lu pg_hdrm : %d\n"),
          pkt_dist_num, pkt_dist_denom, sum_delays, sum_factor, tot_inflight,
          *pg_hdrm));

    return SOC_E_NONE;

}

#endif /* BCM_TOMAHAWK3_SUPPORT */
