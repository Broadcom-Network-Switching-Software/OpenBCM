/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>
#include <soc/types.h>
#include <soc/error.h>
#include <soc/dcmn/error.h>
#include <soc/cmic.h>
#include <sal/core/alloc.h>
#include <soc/wb_engine.h>
#include <soc/drv.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_chain.h>

#ifdef PORTMOD_PM8X50_FABRIC_SUPPORT
#include <soc/portmod/pm8x50_fabric.h>
#include <soc/portmod/pm8x50_shared.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT

#define PM8X50_FABRIC_LANES_PER_CORE (8)
#define PM8X50_FABRIC_MAX_PORTS_PER_PM (8)
#define PM8X50_FABRIC_LANES_PER_FMAC (4)

/* WB defines */
#define PM8X50_FABRIC_LANE_IS_BYPASSED_SET(unit, pm_info, is_bypass, lane) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane_is_bypassed], &is_bypass, lane)
#define PM8X50_FABRIC_LANE_IS_BYPASSED_GET(unit, pm_info, is_bypass, lane) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane_is_bypassed], is_bypass, lane)

#define PM8X50_FABRIC_LANE2PORT_SET(unit, pm_info, lane, port) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane2port_map], &port, lane)
#define PM8X50_FABRIC_LANE2PORT_GET(unit, pm_info, lane, port) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane2port_map], port, lane)

#define PM8X50_FABRIC_PLL_ACTIVE_LANES_BITMAP_SET(unit, pm_info, bitmap, pll_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_pll_active_lanes_bitmap], &bitmap, pll_index)

#define PM8X50_FABRIC_PLL_ACTIVE_LANES_BITMAP_GET(unit, pm_info, bitmap, pll_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_pll_active_lanes_bitmap], bitmap, pll_index)

#define PM8X50_FABRIC_PLL_VCO_RATE_SET(unit, pm_info, vco_rate, pll_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_pll_vco_rate], &vco_rate, pll_index)

#define PM8X50_FABRIC_PLL_VCO_RATE_GET(unit, pm_info, vco_rate, pll_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_pll_vco_rate], vco_rate, pll_index)


/*MAC PRBS Defines*/
#define PM8X50_FABRIC_MAC_PRBS_LOCK_SHIFT  (31)
#define PM8X50_FABRIC_MAC_PRBS_LOCK_MASK   (0x1)
#define PM8X50_FABRIC_MAC_PRBS_CTR_SHIFT   (0)
#define PM8X50_FABRIC_MAC_PRBS_CTR_MASK    (0x7FFFFFFF)

#define PM8X50_FABRIC_FMAC_TX (0)
#define PM8X50_FABRIC_FMAC_RX (1)

#define PM8X50_FABRIC_FMAC_0_LANES_MASK (0x0F)
#define PM8X50_FABRIC_FMAC_1_LANES_MASK (0xF0)

#define PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(lane_index_in_pm) \
    ((lane_index_in_pm) % PM8X50_FABRIC_LANES_PER_FMAC)

#define PM8X50_FABRIC_LANE_FMAC_INDEX_GET(lane_index_in_pm) \
    ((lane_index_in_pm) / PM8X50_FABRIC_LANES_PER_FMAC)

/*
 * Input: bitmap
 * Output: lane_index
 */
#define PM8X50_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index) \
    for ((lane_index) = 0; (lane_index) < PM8X50_FABRIC_LANES_PER_CORE; ++(lane_index)) \
    if ((bitmap) & (1 << (lane_index)))

/*
 * Input: pm_info, lane_index_in_pm
 * Output: fmac_blk_id, lane_index_in_fmac
 */
#define PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, lane_index_in_pm, fmac_blk_id, lane_index_in_fmac) \
    fmac_blk_id = (pm_info->pm_data.pm8x50_fabric->fmac_schan_id[PM8X50_FABRIC_LANE_FMAC_INDEX_GET(lane_index_in_pm)]) | SOC_REG_ADDR_SCHAN_ID_MASK; \
    lane_index_in_fmac = PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(lane_index_in_pm)

/* get serdes index in pm (range 0-7) and convert it to fmac_blk_id (out of the 2 possible fmac_blk_ids) and serdes_index_in_fmac (range 0-3) */
#define PM8X50_FABRIC_SERDES_FMAC_DATA_GET(pm_info, serdes_index_in_pm, fmac_blk_id, serdes_index_in_fmac) \
    PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, serdes_index_in_pm, fmac_blk_id, serdes_index_in_fmac)

/*
 * Input: fmac_blk_ids, nof_fmac_blks, first_fmac_index
 * Output: fmac_index
 */
#define PM8X50_FABRIC_PORT_FMACS_ITER(fmac_blk_ids, nof_fmac_blks, first_fmac_index, fmac_index) \
    for ((fmac_index) = (first_fmac_index); (fmac_index) < (first_fmac_index + nof_fmac_blks); ++(fmac_index))


#define PM8X50_FABRIC_ENCODING_IS_RS_FEC(encoding) \
        (encoding == PORTMOD_PCS_64B66B_RS_FEC || encoding == PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC \
        || encoding == PORTMOD_PCS_64B66B_15T_RS_FEC || encoding == PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC)

#define PM8X50_FABRIC_SPEED_IS_PAM4(speed) \
    ((speed > 28125)? 1 : 0)

#define PM8X50_FABRIC_FEC_SYNC_MACHINE_N1_PAM4 (10)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_N2_PAM4 (21)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_N3_PAM4 (100)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_K1_PAM4 (1)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_K2_PAM4 (2)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_K3_PAM4 (2)

#define PM8X50_FABRIC_FEC_SYNC_MACHINE_N1_NRZ (1)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_N2_NRZ (7)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_N3_NRZ (8)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_K1_NRZ (1)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_K2_NRZ (7)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_K3_NRZ (1)

#define PM8X50_FABRIC_FEC_SYNC_MACHINE_IS_TRIPLE_BIT_SLIP(encoding, n1) \
    (encoding == PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC && \
            ((n1 % 7 == 0) || (n1 % 7 == 5) || (n1 % 7 == 6)))

/* Async FIFO configurations that depend on FEC - regular mode */
#define PM8X50_FABRIC_FEC_NONE_SLOW_READ_RATE     (1)
#define PM8X50_FABRIC_FEC_NONE_FMAL_WIDTH         (2)
#define PM8X50_FABRIC_FEC_BASE_R_SLOW_READ_RATE   (3)
#define PM8X50_FABRIC_FEC_BASE_R_FMAL_WIDTH       (2)
#define PM8X50_FABRIC_FEC_RS_SLOW_READ_RATE       (4)
#define PM8X50_FABRIC_FEC_RS_FMAL_WIDTH           (0)

/* Async FIFO configurations that depend on FEC - retimer mode with FEC disabled */
#define PM8X50_FABRIC_FEC_NONE_FMAL_WIDTH_RETIMER_FEC_DISABLED        (0)
#define PM8X50_FABRIC_FEC_BASE_R_SLOW_READ_RATE_RETIMER_FEC_DISABLED  (1)
#define PM8X50_FABRIC_FEC_BASE_R_FMAL_WIDTH_RETIMER_FEC_DISABLED      (0)

#define PM8X50_FABRIC_MAX_NUM_PLLS (2)

#define PM8X50_FABRIC_PLLS_ITER(begin_pll, end_pll, pll_index)  \
    for (pll_index = end_pll - 1; pll_index >= begin_pll; --pll_index)

#define PM8X50_FABRIC_PLLS_INIT(pll_vco_rate_arr, pll_active_lanes_bitmap_arr) \
        do {\
            int pll; \
            for (pll = 0; pll < PM8X50_FABRIC_MAX_NUM_PLLS; ++pll) \
            { \
                pll_vco_rate_arr[pll] = portmodVCOInvalid; \
                pll_active_lanes_bitmap_arr[pll] = 0; \
            } \
        } while (0)


#define PM8X50_FABRIC_PLL_IS_EMPTY(pll_active_lanes_bitmap) \
    (pll_active_lanes_bitmap == 0)

/* is src_bitmap fully included in dst_bitmap */
#define PM8X50_FABRIC_BITMAP_IS_INCLUDED(src_bitmap, dst_bitmap) \
    (((src_bitmap) & (dst_bitmap)) == (src_bitmap))

#define PM8X50_FABRIC_PLL_INDEX_DEFAULT_START   (0)
#define PM8X50_FABRIC_PLL_INDEX_DEFAULT_END     (2)
#define PM8X50_FABRIC_PREFERRED_NRZ_PLL_INDEX   (0)
#define PM8X50_FABRIC_PREFERRED_PAM4_PLL_INDEX  (1)

/*wb vars in pm8x50_fabric buffer. 
  new value must appended to the end.
  Remove values is disallowed*/
typedef enum pm8x50_fabric_wb_vars {
    wb_is_probed = 0,
    wb_is_initialized = 1,
    wb_ports = 2,
    wb_polarity_rx = 3,
    wb_polarity_tx = 4,
    wb_lane_is_bypassed = 5,
    wb_lane2port_map = 6,
    wb_force_single_pll = 7,
    wb_pll_active_lanes_bitmap = 8,
    wb_pll_vco_rate = 9
} pm8x50_fabric_wb_vars_t;

typedef struct pm8x50_fabric_internal_s{
    pm_info_t *pms;
} pm8x50_fabric_internal_t;

struct pm8x50_fabric_s{
    portmod_pbmp_t phys; 
    phymod_ref_clk_t ref_clk;
    phymod_access_t access;
    phymod_dispatch_type_t core_type;
    phymod_firmware_load_method_t fw_load_method;
    phymod_firmware_loader_f fw_loader;
    uint32 fmac_schan_id[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC];
    uint32 fsrd_schan_id;
    int first_phy_offset;
    int core_index;
    int is_rx_ctrl_bypass_supported;
    int force_single_pll0;
    int force_single_pll1;
    int clock_buffer_disable_required;
    int native_single_pll;
};

typedef struct pm8x50_fabric_fec_sync_machine_config_s{
    int n1;
    int n2;
    int n3;
    int k1;
    int k2;
    int k3;
    int slip;
} pm8x50_fabric_fec_sync_machine_config_t;

static phymod_dispatch_type_t pm8x50_fabric_serdes_list[] =
{
#ifdef PHYMOD_BLACKHAWK_SUPPORT
    phymodDispatchTypeBlackhawk,
#endif
#ifdef PHYMOD_BLACKHAWK7_V1L8P1_SUPPORT
    phymodDispatchTypeBlackhawk7_v1l8p1,
#endif
#ifdef PHYMOD_BLACKHAWK7_L8P2_SUPPORT
    phymodDispatchTypeBlackhawk7_l8p2,
#endif
    phymodDispatchTypeInvalid
};

STATIC
int pm8x50_fabric_squelch_set(int unit, int port, pm_info_t pm_info, int flags, int squelch);

static int
pm8x50_fabric_port_retimer_fec_enable_get(int unit, int port, pm_info_t pm_info, int* enable);

/* 1. get bitmap of lanes used for the port - the bitmap is of size 8. First 4 bits: for FMAC0, second 4 bits: for FMAC1.
 * 2. get phy_index of first phy in each FMAC used for the port - [0-7] */
STATIC
int pm8x50_fabric_port_phy_index_get(int unit, int port, pm_info_t pm_info, uint32 *phy_index, uint32 *bitmap){
    int i, rv = 0, tmp_port = 0;
    SOCDNX_INIT_FUNC_DEFS;

    *bitmap = 0;
    *phy_index = -1;

    /*set bitmap - all the lanes used for the port, first 4 bits: for FMAC0, second 4 bits: for FMAC1.*/
    for (i = 0; i < PM8X50_FABRIC_MAX_PORTS_PER_PM; i++) {
        rv = PM8X50_FABRIC_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        SOCDNX_IF_ERR_EXIT(rv);
        if(tmp_port == port) {
            *phy_index = (*phy_index == -1 ? i : *phy_index);
            SHR_BITSET(bitmap, i);
        }
    }

    if(*phy_index == -1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                (_BSL_SOCDNX_MSG("port was not found in internal DB %d"), port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Set/Clear in val bitmap bits of relevant fmac_index.
 * port - logical port number.
 * bitmap - holds all lanes used for this port.
 *      The bitmap is of size 8, it represents 2 FMACs:
 *      bits [0-3]: lanes 0-3 of FMAC0 of the PM
 *      bits [4-7]: lanes 0-3 of FMAC1 of the PM
 * fmac_index - check only lanes of this FMAC [0 or 1] in bitmap
 * val - this is the output - set or clear bits from bitmap in val
 * to_set - 1 - set relevant bits from bitmap in val
 *          0 - clear relevant bits from bitmap in val
 */
STATIC
int pm8x50_fabric_port_fmac_lanes_bitwrite(int unit, int port, uint32 bitmap, int fmac_index, uint32* val, int to_set)
{
    uint32 fmac_active_lanes_bitmap;
    uint32 fmac_lane_mask[] = {PM8X50_FABRIC_FMAC_0_LANES_MASK, PM8X50_FABRIC_FMAC_1_LANES_MASK};
    SOCDNX_INIT_FUNC_DEFS;

    fmac_active_lanes_bitmap = bitmap & fmac_lane_mask[fmac_index];
    fmac_active_lanes_bitmap >>= fmac_index * PM8X50_FABRIC_LANES_PER_FMAC;
    if (to_set) {
        *val |= fmac_active_lanes_bitmap;
    } else {
        uint32 fmac_non_active_lanes_bitmap = ~fmac_active_lanes_bitmap;
        *val &= fmac_non_active_lanes_bitmap;
    }

    SOCDNX_FUNC_RETURN;
}

/* Get FMAC blk id/s and their amount.
 * fmac_blk_ids is an array of size 2.
 *      fmac_blk_ids[0] -> 0 if FMAC0 of the PM is not used for the port / the blk id of FMAC0 in case it is used for the port.
 *      fmac_blk_ids[1] -> 0 if FMAC1 of the PM is not used for the port / the blk id of FMAC1 in case it is used for the port.
 * nof_fmac_blks - count of FMACs used for the port - 1/2.
 * first_fmac_index - the first FMAC of the port - 0/1.
 */
STATIC
int pm8x50_fabric_fmac_blk_ids_get(int unit, int port, pm_info_t pm_info, uint32* fmac_blk_ids, int* nof_fmac_blks, int* first_fmac_index)
{
    uint32 phy_index, bitmap;
    int fmac_index;
    uint32 fmac_lane_mask[] = {PM8X50_FABRIC_FMAC_0_LANES_MASK, PM8X50_FABRIC_FMAC_1_LANES_MASK};
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));

    *nof_fmac_blks = 0;
    *first_fmac_index = -1;

    for (fmac_index = 0; fmac_index < PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC; ++fmac_index)
    {
        fmac_blk_ids[fmac_index] = 0;
        if (bitmap & fmac_lane_mask[fmac_index])
        {
            fmac_blk_ids[fmac_index] = fabric_data->fmac_schan_id[fmac_index] | SOC_REG_ADDR_SCHAN_ID_MASK;
            *first_fmac_index = (*first_fmac_index == -1)? fmac_index : (*first_fmac_index);
            ++(*nof_fmac_blks);
        }
    }

    if(*nof_fmac_blks == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                (_BSL_SOCDNX_MSG("invalid number of FMACs for port %d"), port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

static int 
pm8x50_fabric_port_retimer_set(int unit, soc_port_t port, pm_info_t pm_info, int is_remote_loopback, int enable);

static int 
pm8x50_fabric_port_retimer_get(int unit, soc_port_t port, pm_info_t pm_info, int *is_remote_loopback, int *enable);

int pm8x50_fabric_port_duplex_set(int unit, int port, pm_info_t pm_info,int enable)
{
    return SOC_E_NONE; 
}


int pm8x50_fabric_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    *enable = TRUE; /* Full Duplex */
    return SOC_E_NONE; 
}


int pm8x50_fabric_pm_interface_type_is_supported(int unit, soc_port_if_t interface, int* is_supported){
    *is_supported = (interface == SOC_PORT_IF_SFI);
    return SOC_E_NONE;
}

STATIC
int pm8x50_fabric_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv;
    int buffer_id;
    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    buffer_id = wb_buffer_index;
    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pm8x50_fabric", NULL, NULL, VERSION(1), 1, SOC_WB_ENGINE_PRE_RELEASE);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_probed", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_is_probed] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_initialized", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_is_initialized] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ports", wb_buffer_index, sizeof(int), NULL, PM8X50_FABRIC_MAX_PORTS_PER_PM, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_ports] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "polarity_rx", wb_buffer_index, sizeof(uint32), NULL, PM8X50_FABRIC_MAX_PORTS_PER_PM, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_polarity_rx] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "polarity_tx", wb_buffer_index, sizeof(uint32), NULL, PM8X50_FABRIC_MAX_PORTS_PER_PM, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_polarity_tx] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane_is_bypassed", wb_buffer_index, sizeof(uint32), NULL, PM8X50_FABRIC_MAX_PORTS_PER_PM, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_lane_is_bypassed] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane2port_map", wb_buffer_index, sizeof(int), NULL, PM8X50_FABRIC_MAX_PORTS_PER_PM, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_lane2port_map] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "force_single_pll", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_force_single_pll] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll_active_lanes_bitmap", wb_buffer_index, sizeof(uint32), NULL, PM8X50_FABRIC_MAX_NUM_PLLS, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_pll_active_lanes_bitmap] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll_vco_rate", wb_buffer_index, sizeof(portmod_vco_type_t), NULL, PM8X50_FABRIC_MAX_NUM_PLLS, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_pll_vco_rate] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD, wb_buffer_index, FALSE));

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_pm_phys_get(int unit, pm_info_t pm_info, portmod_pbmp_t* phys)
{
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_ASSIGN(*phys, fabric_data->phys);
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{   
    const portmod_pm8x50_fabric_create_info_internal_t *info = &pm_add_info->pm_specific_info.pm8x50_fabric;
    pm8x50_fabric_t fabric_data;
    int rv, i, invalid_port = -1;
    int is_phymod_probed=0;
    uint32 rx_polarity, tx_polarity, is_core_probed;
    uint32 pll_active_lanes_bitmap = 0; /* no lanes on the PLL */
    portmod_vco_type_t pll_vco_rate = portmodVCOInvalid;
    uint32 all_lanes_in_pm_bitmap=0xFF;
    phymod_core_access_t core_access;
    int is_bypass;
    SOCDNX_INIT_FUNC_DEFS;

    fabric_data = NULL;
    fabric_data = sal_alloc(sizeof(*(pm_info->pm_data.pm8x50_fabric)), "pm8x50_fabric specific_db");    
    SOCDNX_NULL_CHECK(fabric_data);
    sal_memset(fabric_data, 0, sizeof(*(pm_info->pm_data.pm8x50_fabric)));

    pm_info->wb_buffer_id = wb_buffer_index;
    pm_info->pm_data.pm8x50_fabric = fabric_data;

    pm_info->type = portmodDispatchTypePm8x50_fabric;

    fabric_data->core_type = phymodDispatchTypeInvalid;

    if ( (info->ref_clk != phymodRefClk312Mhz) && (info->ref_clk != phymodRefClk156Mhz) )
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("illegal ref clock")));
    }

    SOCDNX_IF_ERR_EXIT(phymod_firmware_load_method_t_validate(pm_add_info->pm_specific_info.pm8x50_fabric.fw_load_method));

    sal_memcpy(&(fabric_data->access), &info->access, sizeof(phymod_access_t));
    fabric_data->first_phy_offset = pm_add_info->pm_specific_info.pm8x50_fabric.first_phy_offset;
    fabric_data->core_index = pm_add_info->pm_specific_info.pm8x50_fabric.core_index;
    for (i = 0; i < PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC; ++i) {
        fabric_data->fmac_schan_id[i] = pm_add_info->pm_specific_info.pm8x50_fabric.fmac_schan_id[i];
    }
    fabric_data->fsrd_schan_id = pm_add_info->pm_specific_info.pm8x50_fabric.fsrd_schan_id;
    fabric_data->ref_clk = info->ref_clk;
    fabric_data->fw_load_method = pm_add_info->pm_specific_info.pm8x50_fabric.fw_load_method;
    fabric_data->fw_loader = pm_add_info->pm_specific_info.pm8x50_fabric.external_fw_loader;
    fabric_data->is_rx_ctrl_bypass_supported = pm_add_info->pm_specific_info.pm8x50_fabric.is_rx_ctrl_bypass_supported;
    fabric_data->force_single_pll0 = pm_add_info->pm_specific_info.pm8x50_fabric.force_single_pll0;
    fabric_data->force_single_pll1 = pm_add_info->pm_specific_info.pm8x50_fabric.force_single_pll1;
    fabric_data->native_single_pll = pm_add_info->pm_specific_info.pm8x50_fabric.native_single_pll;
    fabric_data->clock_buffer_disable_required = pm_add_info->pm_specific_info.pm8x50_fabric.clock_buffer_disable_required;
    
    PORTMOD_PBMP_ASSIGN(fabric_data->phys, pm_add_info->phys);

    /*init wb buffer*/
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_wb_buffer_init(unit, wb_buffer_index,  pm_info));

    if (fabric_data->access.pmd_info_ptr == NULL) {
        fabric_data->access.pmd_info_ptr = sal_alloc(PMD_INFO_DATA_STRUCTURE_SIZE,
                             "PortMod PM PMD info");
        if (fabric_data->access.pmd_info_ptr == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(fabric_data->access.pmd_info_ptr, 0, PMD_INFO_DATA_STRUCTURE_SIZE);
    }

    if (!SOC_WARM_BOOT(unit))
    {
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], -1);
        SOCDNX_IF_ERR_EXIT(rv);

        PM8X50_FABRIC_PORT_BITMAP_ITER(all_lanes_in_pm_bitmap, i)
        {
            is_bypass = SHR_BITGET(&info->bypass_lanes_bitmap[0], i)? 1 : 0;
            rv = PM8X50_FABRIC_LANE_IS_BYPASSED_SET(unit, pm_info, is_bypass, i);
            SOCDNX_IF_ERR_EXIT(rv);
        }

        for (i = 0; i < PM8X50_FABRIC_LANES_PER_CORE; i++)
        {
            rx_polarity = (info->polarity.rx_polarity >> i) & 0x1;
            tx_polarity = (info->polarity.tx_polarity >> i) & 0x1;

            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_rx], &rx_polarity, i);
            SOCDNX_IF_ERR_EXIT(rv);
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_tx], &tx_polarity, i);
            SOCDNX_IF_ERR_EXIT(rv);
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane2port_map], &invalid_port, i);
            SOCDNX_IF_ERR_EXIT(rv);
        }

        /* PLLs */
        PM8X50_FABRIC_PLLS_ITER(0, PM8X50_FABRIC_MAX_NUM_PLLS, i) {
            rv = PM8X50_FABRIC_PLL_ACTIVE_LANES_BITMAP_SET(unit, pm_info, pll_active_lanes_bitmap, i);
            SOCDNX_IF_ERR_EXIT(rv);

            rv = PM8X50_FABRIC_PLL_VCO_RATE_SET(unit, pm_info, pll_vco_rate, i);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    } 
    else 
    {
        rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &is_core_probed);
        SOCDNX_IF_ERR_EXIT(rv);

        if (is_core_probed)
        {
            /* Probe */
            SOCDNX_IF_ERR_EXIT(phymod_core_access_t_init(&core_access));
            sal_memcpy(&core_access.access, &fabric_data->access, sizeof(fabric_data->access));

            SOCDNX_IF_ERR_EXIT(portmod_common_serdes_probe(pm8x50_fabric_serdes_list, &core_access, &is_phymod_probed));
            if (!is_phymod_probed)
            {
                SOCDNX_EXIT_WITH_ERR(PHYMOD_E_INIT, (_BSL_SOCDNX_MSG("failed to probe core")));
            }
            fabric_data->core_type = core_access.type;

            SOCDNX_IF_ERR_EXIT(rv);
        }
    }
exit:
    if (SOC_FUNC_ERROR)
    {
        pm8x50_fabric_pm_destroy(unit, pm_info);
    }
    SOCDNX_FUNC_RETURN;
}



int pm8x50_fabric_pm_destroy(int unit, pm_info_t pm_info)
{   
    if(pm_info->pm_data.pm8x50_fabric != NULL){
        if (pm_info->pm_data.pm8x50_fabric->access.pmd_info_ptr != NULL) {
            sal_free(pm_info->pm_data.pm8x50_fabric->access.pmd_info_ptr);
            pm_info->pm_data.pm8x50_fabric->access.pmd_info_ptr = NULL;
        }
        sal_free(pm_info->pm_data.pm8x50_fabric);
        pm_info->pm_data.pm8x50_fabric = NULL;
    }
    
    return SOC_E_NONE;
}

int pm8x50_fabric_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info)
{
    core_info->ref_clk = pm_info->pm_data.pm8x50_fabric->ref_clk;
    return SOC_E_NONE;
}

/* check if new lane-mapping in add_info is different from existing lane-mapping */
STATIC
int pm8x50_fabric_pm_lane_mapping_changed_get(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info, int* lane_mapping_changed)
{
    int i;
    phymod_lane_map_t lane_map;
    phymod_core_access_t core_access;
    int cores_num;
    SOCDNX_INIT_FUNC_DEFS;

    *lane_mapping_changed = 0;

    if (add_info->init_config.lane_map[0].num_of_lanes != PM8X50_FABRIC_LANES_PER_CORE)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("add_info->init_config.lane_map[0].num_of_lanes != PM8X50_FABRIC_LANES_PER_CORE")));
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_core_access_get(unit, port, pm_info, 0, 1, &core_access, &cores_num, NULL));

    SOCDNX_IF_ERR_EXIT(phymod_core_lane_map_get(&core_access, &lane_map));

    for(i = 0; i < add_info->init_config.lane_map[0].num_of_lanes; ++i)
    {
        if (lane_map.lane_map_tx[i] != add_info->init_config.lane_map[0].lane_map_tx[i]) {
            *lane_mapping_changed = 1;
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* configure tx FMAC lane-mapping */
STATIC
int pm8x50_fabric_pm_fmac_tx_lane_mapping_set(int unit, int port, pm_info_t pm_info, const phymod_lane_map_t *lane_map)
{
    int rv;
    uint32 field_val, reg_val;
    uint32 fmac_blk_id;
    int lane_index, serdes_index, serdes_index_in_fmac;
    soc_reg_t tx_lane_map_regs[] = {FMAC_TX_LANE_SWAP_0r, FMAC_TX_LANE_SWAP_1r, FMAC_TX_LANE_SWAP_2r, FMAC_TX_LANE_SWAP_3r};
    soc_field_t tx_lane_map_fields[] = {TX_LANE_SWAP_0f, TX_LANE_SWAP_1f, TX_LANE_SWAP_2f, TX_LANE_SWAP_3f};
    uint32 all_lanes_in_pm_bitmap = 0xFF;
    int is_bypass;
    SOCDNX_INIT_FUNC_DEFS;

    /* configure lane swap on all lanes of PM */
    PM8X50_FABRIC_PORT_BITMAP_ITER(all_lanes_in_pm_bitmap, lane_index) {
        serdes_index = lane_map->lane_map_tx[lane_index];
        PM8X50_FABRIC_SERDES_FMAC_DATA_GET(pm_info, serdes_index, fmac_blk_id, serdes_index_in_fmac);

        /* register index is serdes id, register value is logical (FMAC) lane */
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, tx_lane_map_regs[serdes_index_in_fmac], fmac_blk_id, 0, &reg_val));

        rv = PM8X50_FABRIC_LANE_IS_BYPASSED_GET(unit, pm_info, &is_bypass, lane_index);
        SOCDNX_IF_ERR_EXIT(rv);
        /* for bypass lane, the lane-swap in FMAC is aligned to be 1:1 */
        field_val = is_bypass? serdes_index_in_fmac : PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(lane_index);
        soc_reg_field_set(unit, tx_lane_map_regs[serdes_index_in_fmac], &reg_val, tx_lane_map_fields[serdes_index_in_fmac], field_val);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, tx_lane_map_regs[serdes_index_in_fmac], fmac_blk_id, 0, reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* return tx FMAC lane-mapping */
STATIC
int pm8x50_fabric_pm_fmac_tx_lane_mapping_get(int unit, int port, pm_info_t pm_info, phymod_lane_map_t *lane_map)
{
    uint32 field_val, reg_val;
    uint32 fmac_blk_id;
    int lane_index, serdes_index, lane_index_in_fmac;
    soc_reg_t tx_lane_map_regs[] = {FMAC_TX_LANE_SWAP_0r, FMAC_TX_LANE_SWAP_1r, FMAC_TX_LANE_SWAP_2r, FMAC_TX_LANE_SWAP_3r};
    soc_field_t tx_lane_map_fields[] = {TX_LANE_SWAP_0f, TX_LANE_SWAP_1f, TX_LANE_SWAP_2f, TX_LANE_SWAP_3f};
    uint32 all_lanes_in_pm_bitmap = 0xFF;
    SOCDNX_INIT_FUNC_DEFS;

    /*read lane swap on all lanes of PM */
    PM8X50_FABRIC_PORT_BITMAP_ITER(all_lanes_in_pm_bitmap, serdes_index) {

        PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, serdes_index, fmac_blk_id, lane_index_in_fmac);

        /* register index is serdes id, register value is logical (FMAC) lane */
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, tx_lane_map_regs[lane_index_in_fmac], fmac_blk_id, 0, &reg_val));
        field_val = soc_reg_field_get(unit, tx_lane_map_regs[lane_index_in_fmac], reg_val, tx_lane_map_fields[lane_index_in_fmac]);
        if ( field_val >= PM8X50_FABRIC_LANES_PER_FMAC )
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Illegal FMAC swap value")));
        }

        /* Adjust FMAC mapping to CORE mapping by adding 4 lanes to second FMAC */
        lane_index = field_val + PM8X50_FABRIC_LANE_FMAC_INDEX_GET(serdes_index) * PM8X50_FABRIC_LANES_PER_FMAC;

        lane_map->lane_map_tx[lane_index] = serdes_index;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
int pm8x50_fabric_core_init_phase1(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    phymod_core_access_t core_access;
    phymod_core_status_t status;
    phymod_core_init_config_t core_config;
    int rv, fsrd_schan_id;
    portmod_pbmp_t pm_port_phys;
    uint32 is_core_probed;
    soc_reg_above_64_val_t reg_above64_val;
    int is_probed;
    int cores_num;
    int i;
    uint32 rx_polarity, tx_polarity;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys;
    SOCDNX_INIT_FUNC_DEFS;

    PORTMOD_PBMP_ASSIGN(pm_port_phys, fabric_data->phys);
    PORTMOD_PBMP_AND(pm_port_phys, add_info->phys);

    fsrd_schan_id = pm_info->pm_data.pm8x50_fabric->fsrd_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;

    if(!PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {

        SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_RX_H_PWRDNf, 0);
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_TX_H_PWRDNf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

        SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_IDDQf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

        sal_usleep(10);

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_POR_H_RSTBf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

        SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_RX_H_RSTBf, 0xff);
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_TX_H_RSTBf, 0xff);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

        /* Probe */
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_core_access_get(unit, port, pm_info, 0, 1, &core_access, &cores_num, NULL));

         SOCDNX_IF_ERR_EXIT(portmod_common_serdes_probe(pm8x50_fabric_serdes_list, &core_access, &is_probed));
         if (!is_probed)
         {
             SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("failed to probe the core")));
         }
         fabric_data->core_type = core_access.type;

        /* Mark DB as probed */
        is_core_probed = 1;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &is_core_probed);
        SOCDNX_IF_ERR_EXIT(rv);
    } else {
        /* Mark DB as fully probed */
        is_core_probed = 2;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &is_core_probed);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_pm_fmac_tx_lane_mapping_set(unit, port, pm_info, add_info->init_config.lane_map));
    
    /*core status*/
    rv = phymod_core_status_t_init(&status);
    SOCDNX_IF_ERR_EXIT(rv);
    status.pmd_active = FALSE;

    /*core config*/
    phymod_core_init_config_t_init(&core_config);

    /*lane mapping passed to phymod: blackhawk serdes (represented by index in array) -> {lane rx, lane tx} */
    if (add_info->init_config.lane_map[0].num_of_lanes != PM8X50_FABRIC_LANES_PER_CORE)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("add_info->lane_map.num_of_lanes != PM8X50_FABRIC_LANES_PER_CORE")));
    }
    for(i = 0; i < add_info->init_config.lane_map[0].num_of_lanes; ++i)
    {
        core_config.lane_map.lane_map_rx[i] = add_info->init_config.lane_map[0].lane_map_rx[i];
        core_config.lane_map.lane_map_tx[i] = add_info->init_config.lane_map[0].lane_map_tx[i];
    }
    core_config.lane_map.num_of_lanes = add_info->init_config.lane_map[0].num_of_lanes;

    for (i = 0; i < PM8X50_FABRIC_LANES_PER_CORE; i++) {
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_rx], &rx_polarity, i);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_tx], &tx_polarity, i);
        SOCDNX_IF_ERR_EXIT(rv);

        core_config.polarity_map.rx_polarity |= (rx_polarity << i) & 0xff;
        core_config.polarity_map.tx_polarity |= (tx_polarity << i) & 0xff;
    }

    core_config.firmware_load_method = fabric_data->fw_load_method;
    if(fabric_data->fw_load_method == phymodFirmwareLoadMethodExternal){
        /* Fast firmware load */
        core_config.firmware_loader = fabric_data->fw_loader;
    }
    else{
        core_config.firmware_loader = NULL;
    }

    /* These are dummy values. The VCO rates of PLL0, PLL1 will be determined by the speeds of the added ports */
    if (fabric_data->ref_clk == phymodRefClk312Mhz)
    {
        core_config.pll0_div_init_value = phymod_TSCBH_PLL_DIV66; /* VCO Rate => 20.625.Mhz */
        core_config.pll1_div_init_value = phymod_TSCBH_PLL_DIV80; /* VCO Rate => 25Mhz */
    }
    else
    {
        /* phymodRefClk156Mhz */
        core_config.pll0_div_init_value = phymod_TSCBH_PLL_DIV132; /* VCO Rate => 20.625.Mhz */
        core_config.pll1_div_init_value = phymod_TSCBH_PLL_DIV160; /* VCO Rate => 25Mhz */
    }

    if(PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_SET(&core_config);
    }
    
    if(PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_SET(&core_config);
    }

    if(PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_GET(add_info)) {
        PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(&core_config);
    }

    if (PORTMOD_PORT_ADD_F_BYPASS_FW_CRC_CHECK_GET(add_info)) {
        PHYMOD_CORE_INIT_F_BYPASS_CRC_CHECK_SET(&core_config);
    }

    core_config.interface.ref_clock = fabric_data->ref_clk;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_core_access_get(unit, port, pm_info, 0, 1, &core_access, &cores_num, NULL));

    /* in easy reload the purpose is only to update the SW and leave HW as is.
     * skip phymod_core_init function in easy reload because:
     * - it gives an error about core not being in reset state. The core shouldn't be in reset in easy reload.
     * - it is not needed because it doesn't configure any SW (phymod is stateless).
     *   exception- a global phymod struct is initialized in core_init, and so in easy reload it will be done by phymod_phy_pmd_info_init.
     */
    if (SOC_IS_RELOADING(unit))
    {
        if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info) ||
                (!PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) && !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)))
        {
            SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
            params.phyn = 0;
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
            SOC_RELOAD_MODE_SET(unit, FALSE);
            SOCDNX_IF_ERR_EXIT(phymod_phy_pmd_info_init(&phy_access));
            SOC_RELOAD_MODE_SET(unit, TRUE);
        }
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(phymod_core_init(&core_access, &core_config, &status));
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

STATIC
int pm8x50_fabric_core_init_phase2(int unit, int port, pm_info_t pm_info)
{

    int rv, fsrd_schan_id;
    uint32 reg_val, is_core_initialized;
    soc_reg_above_64_val_t reg_above64_val;
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    fsrd_schan_id = pm_info->pm_data.pm8x50_fabric->fsrd_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;

    SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_PLL_0_CTRLr(unit, fsrd_schan_id, &reg_val));
    soc_reg_field_set(unit, FSRD_SRD_PLL_0_CTRLr, &reg_val, SRD_N_PMD_CORE_PLL_0_DP_H_RSTBf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_PLL_0_CTRLr(unit, fsrd_schan_id, reg_val));

    if (!fabric_data->native_single_pll)
    {
        SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_PLL_1_CTRLr(unit, fsrd_schan_id, &reg_val));
        soc_reg_field_set(unit, FSRD_SRD_PLL_1_CTRLr, &reg_val, SRD_N_PMD_CORE_PLL_1_DP_H_RSTBf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_PLL_1_CTRLr(unit, fsrd_schan_id, reg_val));
    }
    SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));
    soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_RX_DP_H_RSTBf, 0xFF);
    soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_TX_DP_H_RSTBf, 0xFF);
    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

    /* Mark DB as initialized */
    is_core_initialized = TRUE;
    rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_initialized], &is_core_initialized);
    SOCDNX_IF_ERR_EXIT(rv);

    exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_disable_init(int unit, int port, pm_info_t pm_info)
{
    uint32 reg_val;
    uint32 field_val[1];
    uint32 phy_index, bitmap;
    int i;
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index, fmac_index;
    soc_field_t reset_fields[] = {FMAC_RX_RST_Nf, FMAC_TX_RST_Nf};
    int flags = 0;
    SOCDNX_INIT_FUNC_DEFS;

    /*Port (MAC + PHY) Enable Disable support*/

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    for (i = 0 ; i < sizeof(reset_fields)/sizeof(*reset_fields); ++i) {
        PM8X50_FABRIC_PORT_FMACS_ITER(fmac_blk_ids, nof_fmac_blks, first_fmac_index, fmac_index) {
            SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_blk_ids[fmac_index], &reg_val));
            *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, reset_fields[i]);
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_fmac_lanes_bitwrite(unit, port, bitmap, fmac_index, field_val, 1));
            soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, reset_fields[i], *field_val);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_blk_ids[fmac_index], reg_val));
        }
    }

    PORTMOD_PORT_ENABLE_TX_SET(flags);
    PORTMOD_PORT_ENABLE_RX_SET(flags);
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_squelch_set(unit, port, pm_info, flags, 1));
    
exit:
    SOCDNX_FUNC_RETURN; 
}

STATIC
int pm8x50_fabric_squelch_set(int unit, int port, pm_info_t pm_info, int flags, int squelch)
{
    portmod_access_get_params_t params;
    phymod_phy_tx_lane_control_t tx_squelch = phymodTxSquelchOff;
    phymod_phy_rx_lane_control_t rx_squelch = phymodRxSquelchOff;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));

    if PORTMOD_PORT_ENABLE_TX_GET(flags)
    {
        tx_squelch = squelch? phymodTxSquelchOn : phymodTxSquelchOff;
    }
    if PORTMOD_PORT_ENABLE_RX_GET(flags)
    {
        rx_squelch = squelch? phymodRxSquelchOn : phymodRxSquelchOff;
    }

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));

    if PORTMOD_PORT_ENABLE_TX_GET(flags)
    {
        SOCDNX_IF_ERR_EXIT(phymod_phy_tx_lane_control_set(&phy_access, tx_squelch));
    }
    if PORTMOD_PORT_ENABLE_RX_GET(flags)
    {
        SOCDNX_IF_ERR_EXIT(phymod_phy_rx_lane_control_set(&phy_access, rx_squelch));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
int pm8x50_fabric_power_set(int unit, int port, pm_info_t pm_info, int power)
{
    portmod_access_get_params_t params;
    phymod_phy_power_t phy_power;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(phymod_phy_power_t_init(&phy_power));
    SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));

    if (power) {
        phy_power.rx = phymodPowerOn;
        phy_power.tx = phymodPowerOn;
    } else {
        phy_power.rx = phymodPowerOff;
        phy_power.tx = phymodPowerOff;
    }

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
    SOCDNX_IF_ERR_EXIT(phymod_phy_power_set(&phy_access, &phy_power));

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 * _pm8x50_port_clock_repeater_buffer_check
 *
 * Background Info:
 *      PLL0 is driving lanes 0-3 directly, and lanes 4-7 through clock repeater.
 *      PLL1 is driving lanes 4-7 directly, and lanes 0-3 through clock repeater.
 * Purpose:
 *      This function will check PLL0/1 TX and RX Clock repeater buffer
 *      could be powered down.
 *      Function calculate,
 *      - txMask: logical lane tx mask corresponding to physical lane 0-3 or 4-7
 *      - rxMask: logical lane rx mask corresponding to physical lane 0-3 or 4-7
 *      and then compare with the lane usage of current PLL,
 *      to decide if tx or rx clock repeater buffer could be powered down.
 *
 * Inputs:
 *     pm_info:              PM info struct
 *     pll_index:            PLL index, 0 or 1
 *     lanes_bitmap:         indicate the logical lane usage of current PLL
 * Output:
 *     tx_clk_rptr_dis_req:  1 means tx clock buffer could be powered down
 *     rx_clk_rptr_dis_req:  1 means rx clock buffer could be powered down
 */
STATIC 
int _pm8x50_fabric_port_clock_repeater_buffer_check(
    int unit,
    pm_info_t pm_info,
    int pll_index,
    uint8 lanes_bitmap,
    int *tx_clk_rptr_dis_req,
    int *rx_clk_rptr_dis_req)
{
    phymod_lane_map_t lane_map;
    phymod_core_access_t core_access;
    int cores_num;

    uint8 rxMask = 0, txMask = 0;
    uint32_t logical_lane;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_core_access_get(unit, 0, pm_info, 0, 1, &core_access, &cores_num, NULL));

    SOCDNX_IF_ERR_EXIT(phymod_core_lane_map_get(&core_access, &lane_map));


    /* PLL1, find corresponding logical lane for physical lane 0-3, and then calculate bitmap */
    if (pll_index == 1) {
        for (logical_lane = 0; logical_lane < 8; logical_lane++){
            if (lane_map.lane_map_tx[logical_lane] < 4) {
                txMask |= 0x1 << logical_lane;
            }
            if (lane_map.lane_map_rx[logical_lane] < 4) {
                rxMask |= 0x1 << logical_lane;
            }
        }
    }
    /* PLL0, find corresponding logical lane for physical lane 4-7, and then calculate bitmap */
    if (pll_index == 0) {
        for (logical_lane = 0; logical_lane < 8; logical_lane++){
            if (lane_map.lane_map_tx[logical_lane] >= 4) {
                txMask |= 0x1 << logical_lane;
            }
            if (lane_map.lane_map_rx[logical_lane] >= 4) {
                rxMask |= 0x1 << logical_lane;
            }
        }
    }

    *tx_clk_rptr_dis_req = 0;
    *rx_clk_rptr_dis_req = 0;
    if ((lanes_bitmap & txMask) == 0) {
        *tx_clk_rptr_dis_req = 1;
    }
    if ((lanes_bitmap & rxMask) == 0) {
        *rx_clk_rptr_dis_req = 1;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 * _pm8x50_fabric_pll_clock_repeater_buffer_pwrdn
 *
 * Background Info:
 *      PLL0 is driving lanes 0-3 directly, and lanes 4-7 through clock repeater.
 *      PLL1 is driving lanes 4-7 directly, and lanes 0-3 through clock repeater.
 * Purpose:
 *      This function will power down unused TX or RX clock repeater buffer
 * Inputs:
 *     unit:                 unit number
 *     pm_info:              PM info struct
 *     phy_access:           phy_access struct
 *     pll_index:            PLL index, 0 or 1
 * Note:
 *     This function is used for DNX device only
 */

/* STATIC 
*/
int _pm8x50_fabric_pll_clock_repeater_buffer_pwrdn(
    int unit,
    pm_info_t pm_info,
    const phymod_phy_access_t* phy_access,
    int pll_index,
    uint8 pll_lanes_bitmap)
{
    int tx_disable_required = 0, rx_disable_required = 0;

    SOCDNX_INIT_FUNC_DEFS;

    /* disable PLL0 clock repeater buffer, if PLL0 is not used by lanes 4-7 */
    /* disable PLL1 clock repeater buffer, if PLL1 is not used by lanes 0-3 */
    SOCDNX_IF_ERR_EXIT(_pm8x50_fabric_port_clock_repeater_buffer_check(unit, pm_info, pll_index,
                                                                       pll_lanes_bitmap, &tx_disable_required, &rx_disable_required));

    PHYMOD_IF_ERR_RETURN
        (phymod_phy_pll_clock_buffer_pwrdn(phy_access, pll_index, tx_disable_required, rx_disable_required));

exit:
    SOCDNX_FUNC_RETURN; 
}

int pm8x50_fabric_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int i = 0, ii, phy;
    int found = FALSE;
    int rv = 0;
    int ports_db_entry = -1;
    phymod_phy_init_config_t init_config;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys;
    portmod_pbmp_t port_phys_in_pm;
    uint32 phy_index, bitmap;
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 is_core_probed = 0, is_core_initialized = 0;
    uint32 is_bypass = 0, phys_count;
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    uint32 fmac_blk_id;
    int lane_index, lane_index_in_fmac;
    int lane_mapping_changed = 0;
    int first_lane = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));

    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, fabric_data->phys);
    PORTMOD_PBMP_COUNT(port_phys_in_pm, phys_count);

    ii = 0;
    SOC_PBMP_ITER(fabric_data->phys, phy){
        if(PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)){
            rv = PM8X50_FABRIC_LANE2PORT_SET(unit, pm_info, ii, port);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        ii++;
    }

    if(!PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        for( i = 0 ; i < PM8X50_FABRIC_MAX_PORTS_PER_PM; i++){
            rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &ports_db_entry, i);
            SOCDNX_IF_ERR_EXIT(rv);
            if(ports_db_entry < 0 ){ /*free slot found*/
                rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &port, i);
                SOCDNX_IF_ERR_EXIT(rv);
                found = TRUE;
                break;
            }
        }
        if(!found){
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("no free space in the PM db")));
        }
    }

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &is_core_probed);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_core_probed || (is_core_probed == 1 && PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info))){ 
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_core_init_phase1(unit, port, pm_info, add_info));
    }

    /* phymod_core_init is called once per core- for the first port we attach. It configures the lane-mapping.
     * The following dynamic-port scenario is not supported-
     * some of the ports of the core are detached after init sequence and the user wants to change their lane-mapping.
     * we can't call phymod_core_init again when some of the ports are already attached, thus can't reconfigure the lane-mapping.
     */
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_pm_lane_mapping_changed_get(unit, port, pm_info, add_info, &lane_mapping_changed));
    if (lane_mapping_changed) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Changing lane-map configuration on a core (octet) containing active ports is not supported")));
    }

    if(PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        SOC_EXIT;
    }

    /* Mark DB as initialized */
    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_initialized], &is_core_initialized);
    SOCDNX_IF_ERR_EXIT(rv);
    if(!is_core_initialized) {
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_core_init_phase2(unit, port, pm_info));
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_power_set(unit, port, pm_info, 1));
        
    SOCDNX_IF_ERR_EXIT(phymod_phy_init_config_t_init(&init_config));
    init_config.an_en = add_info->autoneg_en;
    init_config.cl72_en = add_info->link_training_en;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));

    params.phyn = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
    /* in order to avoid assuming which indexes of init_config.tx phymod will use for the lanes of the port - fill all array indexes */
    for (lane_index = 0; lane_index < PHYMOD_MAX_LANES_PER_CORE; ++lane_index) {
        /* the speed is not known at this stage. Choosing NRZ defaults */
        SOCDNX_IF_ERR_EXIT(phymod_phy_tx_taps_default_get(&phy_access, phymodSignallingMethodNRZ, &init_config.tx[lane_index]));
    }
    rv = PM8X50_FABRIC_LANE_IS_BYPASSED_GET(unit, pm_info, &is_bypass, phy_index);
    SOCDNX_IF_ERR_EXIT(rv);

    if (!is_bypass)
    {
        PHYMOD_PHY_INIT_F_ENABLE_PASS_THROUGH_CONFIGURATION_SET(&init_config);
    }
    SOCDNX_IF_ERR_EXIT(phymod_phy_init(&phy_access, &init_config));

    if (is_bypass)
    {
        /* if the first lane of the whole ILKN port is in the current pm - set this lane to be the master lane of the whole ILKN port */
        SOC_PBMP_ITER(add_info->phys, phy) {
            first_lane = phy;
            break;
        }
        if (PORTMOD_PBMP_MEMBER(fabric_data->phys, first_lane))
        {
            PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
            SOCDNX_IF_ERR_EXIT(READ_FMAC_RETIMERr(unit, fmac_blk_id, lane_index_in_fmac, reg_above_64_val));
            soc_reg_above_64_field32_set(unit, FMAC_RETIMERr, reg_above_64_val, FMAL_N_TX_ILKN_MASTER_READ_ENf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RETIMERr(unit, fmac_blk_id, lane_index_in_fmac, reg_above_64_val));
        }

        PM8X50_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index) {
            PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, lane_index, fmac_blk_id, lane_index_in_fmac);
            SOCDNX_IF_ERR_EXIT(READ_FMAC_RETIMERr(unit, fmac_blk_id, lane_index_in_fmac, reg_above_64_val));
            soc_reg_above_64_field32_set(unit, FMAC_RETIMERr, reg_above_64_val, FMAL_N_TX_ILKN_SHARING_ENf, 1);
            soc_reg_above_64_field32_set(unit, FMAC_RETIMERr, reg_above_64_val, FMAL_N_TX_DC_BUILDUP_FORCE_ENf, 1);
            soc_reg_above_64_field32_set(unit, FMAC_RETIMERr, reg_above_64_val, FMAL_N_TX_RETIMER_CONTROL_PHASE_PERIODf, 0);
            soc_reg_above_64_field32_set(unit, FMAC_RETIMERr, reg_above_64_val, FMAL_N_RETIMER_FEC_ENf, 0);
            soc_reg_above_64_field32_set(unit, FMAC_RETIMERr, reg_above_64_val, FMAL_N_RETIMER_ENf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RETIMERr(unit, fmac_blk_id, lane_index_in_fmac, reg_above_64_val));
        }
    }
    else
    {
        PM8X50_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index) {
            PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, lane_index, fmac_blk_id, lane_index_in_fmac);
            SOCDNX_IF_ERR_EXIT(READ_FMAC_RETIMERr(unit, fmac_blk_id, lane_index_in_fmac, reg_above_64_val));
            /* FEC on retimer links is disabled by default */
            soc_reg_above_64_field32_set(unit, FMAC_RETIMERr, reg_above_64_val, FMAL_N_RETIMER_FEC_ENf, 0);
            soc_reg_above_64_field32_set(unit, FMAC_RETIMERr, reg_above_64_val, AUTO_DOC_NAME_35f, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RETIMERr(unit, fmac_blk_id, lane_index_in_fmac, reg_above_64_val));
        }
    }

    /*disable phy and mac*/  
    /*SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_set(unit, port, pm_info, flags, 0));*/
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_disable_init(unit, port, pm_info));

exit:
    SOCDNX_FUNC_RETURN; 
}


int pm8x50_fabric_port_detach(int unit, int port, pm_info_t pm_info)
{
    int i = 0;
    int found = FALSE;
    int invalid_port = -1;
    int ports_db_entry = -1;
    int rv = 0;
    int is_last = TRUE;
    int unintialized = 0;
    int fsrd_schan_id;
    uint32 reg_val, tmp_port;
    soc_reg_above_64_val_t reg_above64_val;
    uint32 phy_index, bitmap;
    int pll_index, end_pll, begin_pll, lane_index;
    uint32 pll_active_lanes_bitmap[PM8X50_FABRIC_MAX_NUM_PLLS];
    portmod_vco_type_t pll_vco_rate[PM8X50_FABRIC_MAX_NUM_PLLS];
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys;
    int is_bypass;
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    soc_reg_above_64_val_t reg_above_64_val;
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_power_set(unit, port, pm_info, 0));

    /* remove port from its PLL and power down the PLL if it is free */
    begin_pll = 0;
    end_pll = 2;
    if (fabric_data->force_single_pll0 || fabric_data->native_single_pll)
    {
        begin_pll = 0;
        end_pll = 1;
    }
    else if (fabric_data->force_single_pll1)
    {
        begin_pll = 1;
        end_pll = 2;
    }

    SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));
    SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    PM8X50_FABRIC_PLLS_ITER(begin_pll, end_pll, pll_index)
    {
        SOCDNX_IF_ERR_EXIT(PM8X50_FABRIC_PLL_ACTIVE_LANES_BITMAP_GET(unit, pm_info, &pll_active_lanes_bitmap[pll_index], pll_index));
        SOCDNX_IF_ERR_EXIT(PM8X50_FABRIC_PLL_VCO_RATE_GET(unit, pm_info, &pll_vco_rate[pll_index], pll_index));

        if (SHR_BITGET(&pll_active_lanes_bitmap[pll_index], phy_index))
        {
            PM8X50_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index)
            {
                SHR_BITCLR(&pll_active_lanes_bitmap[pll_index], lane_index);
            }
            if (PM8X50_FABRIC_PLL_IS_EMPTY(pll_active_lanes_bitmap[pll_index]))
            {
                pll_vco_rate[pll_index] = portmodVCOInvalid;
                SOCDNX_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, pll_index, 1));
            }

            SOCDNX_IF_ERR_EXIT(PM8X50_FABRIC_PLL_ACTIVE_LANES_BITMAP_SET(unit, pm_info, pll_active_lanes_bitmap[pll_index], pll_index));
            SOCDNX_IF_ERR_EXIT(PM8X50_FABRIC_PLL_VCO_RATE_SET(unit, pm_info, pll_vco_rate[pll_index], pll_index));

            break;
        }
    }

    rv = PM8X50_FABRIC_LANE_IS_BYPASSED_GET(unit, pm_info, &is_bypass, phy_index);
    SOCDNX_IF_ERR_EXIT(rv);
    if (is_bypass)
    {
        PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RETIMERr(unit, fmac_blk_id, lane_index_in_fmac, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, FMAC_RETIMERr, reg_above_64_val, FMAL_N_TX_ILKN_MASTER_READ_ENf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RETIMERr(unit, fmac_blk_id, lane_index_in_fmac, reg_above_64_val));
    }

    for( i = 0 ; i < PM8X50_FABRIC_MAX_PORTS_PER_PM; i++){
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &ports_db_entry, i);
        SOCDNX_IF_ERR_EXIT(rv);
        if(ports_db_entry == port){
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &invalid_port, i);
            SOCDNX_IF_ERR_EXIT(rv);
            found = TRUE;
        } else if(ports_db_entry >= 0){
            is_last = FALSE;
        }
        rv = PM8X50_FABRIC_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        SOCDNX_IF_ERR_EXIT(rv);
        if(tmp_port == port){
            rv = PM8X50_FABRIC_LANE2PORT_SET(unit, pm_info, i, invalid_port);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }
    if(!found){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("port was not found in the PM db")));
    }

    if (is_last){
        /*Quad in-reset*/
        fsrd_schan_id = pm_info->pm_data.pm8x50_fabric->fsrd_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;

        SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_RX_DP_H_RSTBf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_TX_DP_H_RSTBf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));
        sal_usleep(1);

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_RX_H_RSTBf, 0x0);
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_TX_H_RSTBf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));
        sal_usleep(1);

        SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_PLL_0_CTRLr(unit, fsrd_schan_id, &reg_val));
        soc_reg_field_set(unit, FSRD_SRD_PLL_0_CTRLr, &reg_val, SRD_N_PMD_CORE_PLL_0_DP_H_RSTBf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_PLL_0_CTRLr(unit, fsrd_schan_id, reg_val));

        if (!fabric_data->native_single_pll) {
            SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_PLL_1_CTRLr(unit, fsrd_schan_id, &reg_val));
            soc_reg_field_set(unit, FSRD_SRD_PLL_1_CTRLr, &reg_val, SRD_N_PMD_CORE_PLL_1_DP_H_RSTBf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_PLL_1_CTRLr(unit, fsrd_schan_id, reg_val));
            sal_usleep(1);
        }

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_POR_H_RSTBf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

        sal_usleep(10);

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_RX_H_PWRDNf, 0xff);
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_TX_H_PWRDNf, 0xff);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));
        
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_IDDQf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

        sal_usleep(10);

        /*remove from data - base*/  
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &unintialized);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_initialized], &unintialized);

        fabric_data->core_type = phymodDispatchTypeInvalid;

        SOCDNX_IF_ERR_EXIT(rv);       
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

int pm8x50_fabric_port_replace(int unit, int port, pm_info_t pm_info, int new_port)
{
        
    int i = 0;
    int rv = 0;
    int ports_db_entry;
    uint32 tmp_port;
    SOCDNX_INIT_FUNC_DEFS;
    
    for (i = 0; i < PM8X50_FABRIC_MAX_PORTS_PER_PM; i++){
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &ports_db_entry, i);
        SOCDNX_IF_ERR_EXIT(rv);
        if(ports_db_entry == port){
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &new_port, i);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        rv = PM8X50_FABRIC_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        SOCDNX_IF_ERR_EXIT(rv);
        if (tmp_port == port) {
            rv = PM8X50_FABRIC_LANE2PORT_SET(unit, pm_info, i, new_port);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }
                   
exit:
    SOCDNX_FUNC_RETURN; 
    
}


soc_error_t pm8x50_fabric_fmac_enable_set(int unit, int port, pm_info_t pm_info, int rx_tx, int enable)
{
    soc_field_t field;
    uint32 field_val[1];
    uint32 reg_val, phy_index, bitmap;
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index, fmac_index;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));

    field = ((rx_tx == PM8X50_FABRIC_FMAC_TX) ? FMAC_TX_RST_Nf : FMAC_RX_RST_Nf);

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    PM8X50_FABRIC_PORT_FMACS_ITER(fmac_blk_ids, nof_fmac_blks, first_fmac_index, fmac_index) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_blk_ids[fmac_index], &reg_val));
        *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, field);
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_fmac_lanes_bitwrite(unit, port, bitmap, fmac_index, field_val, (enable? 0 : 1)));
        soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, field, *field_val);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_blk_ids[fmac_index], reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
    uint32 phy_index, bitmap;
    uint32 flags_mask = flags;
    uint32 is_bypass;
    int cur_enable, rv;
    SOCDNX_INIT_FUNC_DEFS;

    /* If no RX\TX flags - set both*/
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
        PORTMOD_PORT_ENABLE_RX_SET(flags_mask);
        PORTMOD_PORT_ENABLE_TX_SET(flags_mask);
    }

    /* if no MAC\Phy flags - set both*/
    if((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) && (!PORTMOD_PORT_ENABLE_MAC_GET(flags))){
        PORTMOD_PORT_ENABLE_PHY_SET(flags_mask);
        PORTMOD_PORT_ENABLE_MAC_SET(flags_mask);
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    rv = PM8X50_FABRIC_LANE_IS_BYPASSED_GET(unit, pm_info, &is_bypass, phy_index);
    SOCDNX_IF_ERR_EXIT(rv);

    if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && PORTMOD_PORT_ENABLE_PHY_GET(flags_mask) && 
             PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        /*Port (MAC + PHY) Enable Disable support*/

        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_get(unit, port, pm_info, flags, &cur_enable)); 
        if ((cur_enable ? 1 : 0) == (enable ? 1 : 0))
        {
            SOC_EXIT;
        }

        if(enable){
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_squelch_set(unit, port, pm_info, flags_mask, 0));

            /* Enable FMAC Tx*/
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_TX, enable));

            /* Enable FMAC Rx*/
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_RX, enable));

        } 
        else
        {
            /* Disable FMAC Rx*/
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_RX, enable));

            /* Disable FMAC Tx*/
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_TX, enable));

            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_squelch_set(unit, port, pm_info, flags_mask, 1));
        }
    }
    else if ((PORTMOD_PORT_ENABLE_RX_GET(flags)) && ((PORTMOD_PORT_ENABLE_MAC_GET(flags)) || is_bypass))
    {
        /*MAC RX Enable/Disable support*/
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_RX, enable));
    } 
    else if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && (!PORTMOD_PORT_ENABLE_PHY_GET(flags_mask)) &&
            PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        if(enable){
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_TX, enable));
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_RX, enable));
        }else{
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_RX, enable));
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_TX, enable));
        }
    }
    else if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && (!PORTMOD_PORT_ENABLE_PHY_GET(flags_mask)) &&
            !PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        if (!is_bypass){
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL , (_BSL_SOCDNX_MSG("Fabric doesn't support only Tx MAC option. (Only ILKN over fabric supports that mode).\n")));
        }
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_TX, enable));
    }
    else if (!PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && PORTMOD_PORT_ENABLE_PHY_GET(flags_mask) &&
             PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        if (!is_bypass){
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL , (_BSL_SOCDNX_MSG("Fabric doesn't support only Phy option. (Only ILKN over fabric supports that mode).\n")));
        }


        if(enable){

            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_squelch_set(unit, port, pm_info, flags_mask, 0));
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_squelch_set(unit, port, pm_info, flags_mask, 1));
        }
    }
    else if (!PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && PORTMOD_PORT_ENABLE_PHY_GET(flags_mask) &&
            !PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        if (is_bypass)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL , (_BSL_SOCDNX_MSG("ILKN over fabric doesn't support Tx PHY option. (Only fabric supports that mode).\n")));
        }
        if (enable)
        {
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_squelch_set(unit, port, pm_info, flags_mask, 0));
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_squelch_set(unit, port, pm_info, flags_mask, 1));
        }
    }
    else
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL , (_BSL_SOCDNX_MSG("Supported options are only: (1) all, (2) MAC + RX \n (3) MAC (but for ILKN over fabric only) (4) Tx MAC (but for ILKN over fabric only)\n (5) Phy (but for ILKN over fabric only)\n")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int pm8x50_fabric_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int *enable)
{
    int rv;
    uint32 reg_val, flags_mask = flags;
    uint32 field_val[1];
    uint32 is_bypass, mac_reset, tx_mac_reset, bitmap;
    uint32 phy_index;
    phymod_phy_access_t phy_access;
    phymod_phy_rx_lane_control_t rx_control;
    phymod_phy_tx_lane_control_t tx_control;
    int rx_enable, tx_enable;
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    portmod_access_get_params_t params;
    int nof_phys;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));    
    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    rv = PM8X50_FABRIC_LANE_IS_BYPASSED_GET(unit, pm_info, &is_bypass, phy_index);
    SOCDNX_IF_ERR_EXIT(rv);

    /*if the first phy in the first FMAC is enabled - assuming the whole port is enabled*/
    PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
    SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_blk_id, &reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_RX_RST_Nf);
    mac_reset = SHR_BITGET(field_val, lane_index_in_fmac);
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_TX_RST_Nf);
    tx_mac_reset = SHR_BITGET(field_val, lane_index_in_fmac);

    /* If no RX\TX flags - set both*/
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
        PORTMOD_PORT_ENABLE_RX_SET(flags_mask);
        PORTMOD_PORT_ENABLE_TX_SET(flags_mask);
    }

    /* if no MAC\Phy flags - set both*/
    if((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) && (!PORTMOD_PORT_ENABLE_MAC_GET(flags))){
        PORTMOD_PORT_ENABLE_PHY_SET(flags_mask);
        PORTMOD_PORT_ENABLE_MAC_SET(flags_mask);
    }
    
    if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && PORTMOD_PORT_ENABLE_PHY_GET(flags_mask) && 
         PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
         SOCDNX_IF_ERR_EXIT(phymod_phy_rx_lane_control_get(&phy_access, &rx_control));
         SOCDNX_IF_ERR_EXIT(phymod_phy_tx_lane_control_get(&phy_access, &tx_control));
         rx_enable = ((!mac_reset) && (rx_control != phymodRxSquelchOn));
         tx_enable = ((!tx_mac_reset) && (tx_control != phymodTxSquelchOn));
         *enable = rx_enable || tx_enable;
    }
    else if ((PORTMOD_PORT_ENABLE_RX_GET(flags)) && ((PORTMOD_PORT_ENABLE_MAC_GET(flags)) || is_bypass))
    {
        *enable = !mac_reset;
    } 
    else if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && (!PORTMOD_PORT_ENABLE_PHY_GET(flags_mask)) &&
            !PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        *enable = !tx_mac_reset;
    }
    else if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && (!PORTMOD_PORT_ENABLE_PHY_GET(flags_mask)) &&
            PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        *enable = (!mac_reset) || (!tx_mac_reset);
    }
    else 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL , (_BSL_SOCDNX_MSG("Supported options are only (1) MAC, TX, RX (2) MAC RX (3) MAC TX or (4) all - MAC, PHY, TX, RX")));
    }


exit:
    SOCDNX_FUNC_RETURN;    
}

int
pm8x50_fabric_port_cdr_lock_get(int unit, int port, pm_info_t pm_info, int* is_locked)
{
    uint32 fsrd_schan_id;
    uint32 phy_index, bitmap;
    uint32 reg32_val;
    uint32 locked_links_bitmap[1];
    SOCDNX_INIT_FUNC_DEFS;

    fsrd_schan_id = pm_info->pm_data.pm8x50_fabric->fsrd_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));

    SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_STATUSr(unit, fsrd_schan_id, &reg32_val));

    locked_links_bitmap[0] = soc_reg_field_get(unit, FSRD_SRD_STATUSr, reg32_val, SRD_N_RX_LOCKf);
    *is_locked = SHR_BITGET(locked_links_bitmap, phy_index)? 1 : 0;

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_electrical_idle_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys;
    phymod_phy_tx_lane_control_t control;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));

    control = enable? phymodTxElectricalIdleEnable : phymodTxElectricalIdleDisable;
    SOCDNX_IF_ERR_EXIT(phymod_phy_tx_lane_control_set(&phy_access, control));

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * pm8x50_fabric_port_fec_sync_machine_config
 * This function configures RS FEC sync machine (relevant only to RS FEC).
 * The sync machine synchronizes on the start of a frame.
 * It chooses a bit as a candidate, then looks for K valid frames out of N frames in 3 stages,
 * to determine if a synchronization on the start of the frame is successful and this is indeed the first bit.
 * The configuration depends both on FEC type and on PAM4/NRZ.
 * Thus this function needs to be called when setting the encoding and when setting the speed.
 * This function is not relevant for bypass (ILKN) port
*/
static
int pm8x50_fabric_port_fec_sync_machine_config(int unit, int port, pm_info_t pm_info)
{
    uint32 properties = 0;
    portmod_port_pcs_t pcs;
    pm8x50_fabric_fec_sync_machine_config_t sync_machine_config;
    uint32 phy_index, bitmap;
    uint32 fmac_blk_id;
    int lane_index, lane_index_in_fmac;
    soc_reg_above_64_val_t reg_above64;
    portmod_speed_config_t speed_config;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_encoding_get(unit, port, pm_info, &properties, &pcs));
    if (!PM8X50_FABRIC_ENCODING_IS_RS_FEC(pcs)) {
        SOC_EXIT;
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_speed_config_get(unit, port, pm_info, &speed_config));

    if (PM8X50_FABRIC_SPEED_IS_PAM4(speed_config.speed)) {
        sync_machine_config.n1 = PM8X50_FABRIC_FEC_SYNC_MACHINE_N1_PAM4;
        sync_machine_config.n2 = PM8X50_FABRIC_FEC_SYNC_MACHINE_N2_PAM4;
        sync_machine_config.n3 = PM8X50_FABRIC_FEC_SYNC_MACHINE_N3_PAM4;
        sync_machine_config.k1 = PM8X50_FABRIC_FEC_SYNC_MACHINE_K1_PAM4;
        sync_machine_config.k2 = PM8X50_FABRIC_FEC_SYNC_MACHINE_K2_PAM4;
        sync_machine_config.k3 = PM8X50_FABRIC_FEC_SYNC_MACHINE_K3_PAM4;
    } else { /*NRZ*/
        sync_machine_config.n1 = PM8X50_FABRIC_FEC_SYNC_MACHINE_N1_NRZ;
        sync_machine_config.n2 = PM8X50_FABRIC_FEC_SYNC_MACHINE_N2_NRZ;
        sync_machine_config.n3 = PM8X50_FABRIC_FEC_SYNC_MACHINE_N3_NRZ;
        sync_machine_config.k1 = PM8X50_FABRIC_FEC_SYNC_MACHINE_K1_NRZ;
        sync_machine_config.k2 = PM8X50_FABRIC_FEC_SYNC_MACHINE_K2_NRZ;
        sync_machine_config.k3 = PM8X50_FABRIC_FEC_SYNC_MACHINE_K3_NRZ;
    }

    if (PM8X50_FABRIC_FEC_SYNC_MACHINE_IS_TRIPLE_BIT_SLIP(pcs, sync_machine_config.n1)) {
        sync_machine_config.slip = 1;
    } else {
        sync_machine_config.slip = 0;
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));

    PM8X50_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index) {
        PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, lane_index, fmac_blk_id, lane_index_in_fmac);
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_above64));
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_FRAME_SYNC_N_1f, sync_machine_config.n1);
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_FRAME_SYNC_N_2f, sync_machine_config.n2);
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_FRAME_SYNC_N_3f, sync_machine_config.n3);
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_FRAME_SYNC_K_1f, sync_machine_config.k1);
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_FRAME_SYNC_K_2f, sync_machine_config.k2);
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_FRAME_SYNC_K_3f, sync_machine_config.k3);
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, AUTO_DOC_NAME_98f, sync_machine_config.slip);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_above64));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* This function is not relevant for bypass (ILKN) port */
int pm8x50_fabric_port_encoding_set(int unit, int port, pm_info_t pm_info, uint32 properties, portmod_port_pcs_t encoding)
{
    int fec_enable = 1;
    int encoding_type = 0;
    int slow_read_rate = 0;
    int rx_ctrl_bypass = 0;
    int flags = 0;
    int enabled = 0;
    int llfc_cig = 0;
    int low_latency_llfc = 0;
    int rsf_err_mark = 0;
    int fec_err_mark = 0;
    int cig_ignore = 0;
    int llfc_after_fec = 0;
    int fmal_width = 0;
    int rsf_frame_type = 3; /*same as HW default, stands for 15T_RS_FEC*/
    uint32 fmac_blk_id;
    int lane_index, lane_index_in_fmac;
    uint32 phy_index, bitmap;
    uint32 reg_val;
    soc_reg_above_64_val_t reg_above64;
    uint64 reg64_val;
    SOCDNX_INIT_FUNC_DEFS;

    switch(encoding){
    case PORTMOD_PCS_64B66B_FEC:
        encoding_type = 2;
        slow_read_rate = PM8X50_FABRIC_FEC_BASE_R_SLOW_READ_RATE;
        fmal_width = PM8X50_FABRIC_FEC_BASE_R_FMAL_WIDTH;
        break;
    case PORTMOD_PCS_64B66B:
        encoding_type = 2;
        slow_read_rate = PM8X50_FABRIC_FEC_NONE_SLOW_READ_RATE;
        fec_enable = 0;
        fmal_width = PM8X50_FABRIC_FEC_NONE_FMAL_WIDTH;
        break;
    case PORTMOD_PCS_64B66B_RS_FEC:
        encoding_type = 4;
        slow_read_rate = PM8X50_FABRIC_FEC_RS_SLOW_READ_RATE;
        llfc_cig = 1;
        fmal_width = PM8X50_FABRIC_FEC_RS_FMAL_WIDTH;
        rsf_frame_type = 1;
        break;
    case PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC:
        encoding_type = 4;
        slow_read_rate = PM8X50_FABRIC_FEC_RS_SLOW_READ_RATE;
        llfc_cig = 1;
        fmal_width = PM8X50_FABRIC_FEC_RS_FMAL_WIDTH;
        rsf_frame_type = 0;
        break;
    case PORTMOD_PCS_64B66B_15T_RS_FEC:
        encoding_type = 4;
        slow_read_rate = PM8X50_FABRIC_FEC_RS_SLOW_READ_RATE;
        llfc_cig = 1;
        fmal_width = PM8X50_FABRIC_FEC_RS_FMAL_WIDTH;
        rsf_frame_type = 3;
        break;
    case PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC:
        encoding_type = 4;
        slow_read_rate = PM8X50_FABRIC_FEC_RS_SLOW_READ_RATE;
        llfc_cig = 1;
        fmal_width = PM8X50_FABRIC_FEC_RS_FMAL_WIDTH;
        rsf_frame_type = 2;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported pcs type %d"), encoding));
    }

    /* handle properties*/
    /*Low latency llfc*/
    if(PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties) && (encoding == PORTMOD_PCS_64B66B)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("low latency llfc is not supported for 64/66 pcs")));
    }
    if(!PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties) && PM8X50_FABRIC_ENCODING_IS_RS_FEC(encoding)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("RS_FEC pcs: low latency llfc is always enabled")));
    }
    low_latency_llfc = PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties);

    /*error detect*/
    if(PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties) && (encoding == PORTMOD_PCS_64B66B)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("error detect is not supported for 64/66 pcs")));
    }
    if(encoding == PORTMOD_PCS_64B66B_FEC){
        fec_err_mark = PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties);
    } else if(PM8X50_FABRIC_ENCODING_IS_RS_FEC(encoding)){
        rsf_err_mark = PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties);
    }

    /*extract cig from llfc cells*/
    if(PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_GET(properties) && (encoding != PORTMOD_PCS_64B66B_FEC)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("extract CIG indications from LLFC cells is supported just for 64/66 kr fec")));
    }
    cig_ignore = PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_GET(properties) ? 0 : 1;

    /*do FEC on LLFC and congestion indication (get llfc/cig values after error correction)*/
    if(PORTMOD_ENCODING_LLFC_AFTER_FEC_GET(properties) && !PM8X50_FABRIC_ENCODING_IS_RS_FEC(encoding)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("llfc after FEC is supported only for rs fecs")));
    }
    llfc_after_fec = PORTMOD_ENCODING_LLFC_AFTER_FEC_GET(properties);

    /*bypass FEC on flow status and credit control cells*/
    if(PORTMOD_ENCODING_CONTROL_CELLS_FEC_BYPASS_GET(properties) && !PM8X50_FABRIC_ENCODING_IS_RS_FEC(encoding)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("control cells FEC bypass is supported only for rs fecs")));
    }
    rx_ctrl_bypass = PORTMOD_ENCODING_CONTROL_CELLS_FEC_BYPASS_GET(properties) ? 3 : 0;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));

    flags = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_get(unit, port, pm_info, flags, &enabled));
    /*disable phy and mac*/
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_set(unit, port, pm_info, flags, 0));

    PM8X50_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index) {
        PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, lane_index, fmac_blk_id, lane_index_in_fmac);
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
        /*encoding type set*/
        soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_ENCODING_TYPEf, encoding_type);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_val));
    }

    PM8X50_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index) {
        PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, lane_index, fmac_blk_id, lane_index_in_fmac);
        SOCDNX_IF_ERR_EXIT(READ_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg64_val));
        soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_N_RX_SLOW_READ_RATEf, slow_read_rate);
        soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_N_RX_WIDTHf, fmal_width);
        soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_N_TX_WIDTHf, fmal_width);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg64_val));
    }

    PM8X50_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index) {
        PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, lane_index, fmac_blk_id, lane_index_in_fmac);
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
        /*enable/disable FEC for 64\66 and 64\66 KR*/
        soc_reg_field_set(unit,FMAC_FPS_RX_FEC_CONFIGURATIONr, &reg_val, FPS_N_RX_FEC_FEC_ENf, fec_enable);
        /*error mark en - relevant only for KR FEC(64B66B_FEC)*/
        if(encoding == PORTMOD_PCS_64B66B_FEC)
        {
            soc_reg_field_set(unit,FMAC_FPS_RX_FEC_CONFIGURATIONr, &reg_val, FPS_N_RX_FEC_ERR_MARK_ENf, fec_err_mark);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_val));
    }

    PM8X50_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index) {
        PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, lane_index, fmac_blk_id, lane_index_in_fmac);
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_TX_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
        soc_reg_field_set(unit, FMAC_FPS_TX_CONFIGURATIONr, &reg_val, FPS_N_TX_FEC_ENf, fec_enable);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FPS_TX_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_val));
    }

    PM8X50_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index) {
        PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, lane_index, fmac_blk_id, lane_index_in_fmac);
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_RX_GENERAL_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
        /*set llfc low latency and enable cig*/
        soc_reg_field_set(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_LOW_LATENCY_LLFCf, low_latency_llfc);
        soc_reg_field_set(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_LOW_LATENCY_CIGf, llfc_cig);
        soc_reg_field_set(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_IGNORE_CIG_LLFC_CELLf, cig_ignore);
        soc_reg_field_set(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_RX_CTRL_BYPf, rx_ctrl_bypass);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_RX_GENERAL_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_val));
    }

    /* RS-FEC configurations */
    PM8X50_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index) {
        PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, lane_index, fmac_blk_id, lane_index_in_fmac);
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_above64));
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_ERR_MARK_ENf, rsf_err_mark);

        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_TX_FRAME_TYPEf, rsf_frame_type);
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_FRAME_TYPEf, rsf_frame_type);

        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_TX_SCRAMBLE_LLFC_BITS_ENf, 0x1);
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_SCRAMBLE_LLFC_BITS_ENf, 0x1);

        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_LLFC_AFTER_FECf, llfc_after_fec);

        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_above64));
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_fec_sync_machine_config(unit, port, pm_info));

    /*restore MAC and PHY */
    flags = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_set(unit, port, pm_info, flags, enabled));

exit:
    SOCDNX_FUNC_RETURN; 
}

int pm8x50_fabric_port_encoding_get(int unit, int port, pm_info_t pm_info, uint32 *properties, portmod_port_pcs_t *encoding)
{
    int encoding_type = 0;
    int rsf_frame_type = 0;
    int low_latency_llfc = 0;
    int err_mark = 0;
    int cig_ignore = 0;
    int llfc_after_fec = 0;
    int rx_ctrl_bypass = 0;
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    uint32 phy_index, bitmap;
    uint32 reg_val;
    soc_reg_above_64_val_t reg_above64;
    int fec_enable;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
 
    *properties = 0;
    encoding_type = soc_reg_field_get(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, reg_val, FMAL_N_ENCODING_TYPEf);
    switch(encoding_type){
    case 2:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
        fec_enable = soc_reg_field_get(unit,FMAC_FPS_RX_FEC_CONFIGURATIONr, reg_val,FPS_N_RX_FEC_FEC_ENf); 
        *encoding = fec_enable ? PORTMOD_PCS_64B66B_FEC : PORTMOD_PCS_64B66B;
        break;
    case 4:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_above64));
        rsf_frame_type = soc_reg_above_64_field32_get(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_FRAME_TYPEf);
        switch(rsf_frame_type) {
        case 0:
            *encoding = PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC;
            break;
        case 1:
            *encoding = PORTMOD_PCS_64B66B_RS_FEC;
            break;
        case 2:
            *encoding = PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC;
            break;
        case 3:
            *encoding = PORTMOD_PCS_64B66B_15T_RS_FEC;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unknown RS-FEC type")));
        }
        break;
    case 0:
        *encoding = PORTMOD_PCS_UNKNOWN;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unknown pcs type %d"), encoding_type));
    }

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_RX_GENERAL_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
    /*get llfc low latency, enable cig and ctrl cells bypass*/
    low_latency_llfc = soc_reg_field_get(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, reg_val ,FMAL_N_LOW_LATENCY_LLFCf);
    cig_ignore = soc_reg_field_get(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, reg_val, FMAL_N_IGNORE_CIG_LLFC_CELLf);
    rx_ctrl_bypass = soc_reg_field_get(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, reg_val, FMAL_N_RX_CTRL_BYPf);

    if (*encoding == PORTMOD_PCS_64B66B_FEC) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
        err_mark = soc_reg_field_get(unit, FMAC_FPS_RX_FEC_CONFIGURATIONr, reg_val, FPS_N_RX_FEC_ERR_MARK_ENf);
        llfc_after_fec = 0;
    } else if (PM8X50_FABRIC_ENCODING_IS_RS_FEC(*encoding)) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_above64));
        err_mark = soc_reg_above_64_field32_get(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_ERR_MARK_ENf);
        llfc_after_fec = soc_reg_above_64_field32_get(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_LLFC_AFTER_FECf);
    } else {
        err_mark = 0;
        llfc_after_fec = 0;
    }

    if(low_latency_llfc){
         PORTMOD_ENCODING_LOW_LATENCY_LLFC_SET(*properties);
    }
    if(!cig_ignore){
         PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_SET(*properties);
    }
    if(err_mark){
         PORTMOD_ENCODING_FEC_ERROR_DETECT_SET(*properties);
    }
    if(llfc_after_fec){
        PORTMOD_ENCODING_LLFC_AFTER_FEC_SET(*properties);
    }
    if(rx_ctrl_bypass){
        PORTMOD_ENCODING_CONTROL_CELLS_FEC_BYPASS_SET(*properties);
    }

exit:
    SOCDNX_FUNC_RETURN; 
}


/* SRD_N_FORCE_SIGNAL_DETECT and FMAL_N_TX_SRD_BACKPRESSURE_EN configurations are irrelevant for ILKN-over-fabric */
STATIC int
_pm8x50_fabric_loopback_serdes_overrides_set(int unit, int port, pm_info_t pm_info)
{
    int i;
    soc_reg_above_64_val_t reg_above64_val;
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    portmod_loopback_mode_t mac_loopbacks[]= { portmodLoopbackMacAsyncFifo, portmodLoopbackMacOuter, portmodLoopbackMacPCS, portmodLoopbackPhyGloopPMD,portmodLoopbackPhyRloopPMD};
    int lb_enable = FALSE;
    uint32 fsrd_schan_id;
    uint32 phy_index, bitmap;
    int backpressure_en=1;
    uint32 reg_val = 0, field_val[1] = {0};
    uint32 force_signal_detect_set = 0;
    SOCDNX_INIT_FUNC_DEFS;
    
    for (i = 0; i < sizeof(mac_loopbacks)/sizeof(mac_loopbacks[0]); i++) {
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_loopback_get(unit, port, pm_info, mac_loopbacks[i], &lb_enable));
        if (lb_enable) {
            force_signal_detect_set = 1;
            if (mac_loopbacks[i] == portmodLoopbackMacAsyncFifo || mac_loopbacks[i] == portmodLoopbackMacPCS) {
                backpressure_en = 0;
            }
            break;
        }
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));

    /*Ignore analog signals from SerDes in case of loopback*/
    fsrd_schan_id = pm_info->pm_data.pm8x50_fabric->fsrd_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;
    SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));
    field_val[0] = soc_reg_above_64_field32_get(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_FORCE_SIGNAL_DETECTf);
    SHR_BITWRITE(field_val, phy_index, force_signal_detect_set);
    soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_FORCE_SIGNAL_DETECTf, field_val[0]);
    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

    /*back-pressure from SerDes Tx should be disabled for portmodLoopbackMacAsyncFifo and portmodLoopbackMacPCS LBs - to ignore flow control from SerDes*/
    PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
    soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_TX_SRD_BACKPRESSURE_ENf, backpressure_en);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}

/* This function is not relevant for bypass (ILKN) port */
STATIC
int pm8x50_fabric_mac_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    uint32 reg_val = 0;
    uint32 field[1] = {0};
    uint32 phy_index, bitmap;
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);

    switch(loopback_type){
    case portmodLoopbackMacOuter:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_LOOPBACK_ENABLE_REGISTERr(unit, fmac_blk_id, &reg_val));
        *field = soc_reg_field_get(unit, FMAC_LOOPBACK_ENABLE_REGISTERr, reg_val, LCL_LPBK_ONf);
        if(enable) {
            SHR_BITSET(field, lane_index_in_fmac);
        } else {
            SHR_BITCLR(field, lane_index_in_fmac);
        }
        soc_reg_field_set(unit, FMAC_LOOPBACK_ENABLE_REGISTERr, &reg_val, LCL_LPBK_ONf, *field);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_LOOPBACK_ENABLE_REGISTERr(unit, fmac_blk_id, reg_val));
        if (enable) {
            LOG_WARN(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "Warning: MAC outer loopback is supported over rates 20.625 and higher\n")));
        }
        break;

    case portmodLoopbackMacAsyncFifo:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
        soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMALN_CORE_LOOPBACKf, enable);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_val));

        break;

    case portmodLoopbackMacPCS:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
        soc_reg_field_set(unit, FMAC_KPCS_CONFIGURATIONr, &reg_val, KPCS_N_RX_DSC_LOOPBACK_ENf, enable);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_val));

        break;
    case portmodLoopbackMacRloop:
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_retimer_set(unit, port, pm_info, TRUE, enable));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("should not reach here")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Checks per a given port if lanes are swapped.
 * Done by checking lane mapping as configured in phymod.
 * Check all the lanes of the port:
 * if lanes aren't swapped the mapping (lane->serdes) will be 1:1.
 */
STATIC
int pm8x50_fabric_port_are_lanes_swapped(int unit, int port, pm_info_t pm_info, int* lanes_are_swapped)
{
    int lane_index;
    uint32 phy_index, bitmap;
    phymod_lane_map_t lane_map;
    phymod_core_access_t core_access;
    int cores_num;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_core_access_get(unit, port, pm_info, 0, 1, &core_access, &cores_num, NULL));
    SOCDNX_IF_ERR_EXIT(phymod_core_lane_map_get(&core_access, &lane_map));

    *lanes_are_swapped = 0;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    PM8X50_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index)
    {
        if (lane_map.lane_map_tx[lane_index] != lane_index)
        {
            *lanes_are_swapped = 1;
            SOC_EXIT;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    int rv;
    uint32 phy_index, bitmap;
    int is_phy_loopback = FALSE;
    int lb_enable = 0;
    uint32 encoding_properties;
    portmod_port_pcs_t encoding = 0;
    uint32 flags;
    int enabled;
    uint32 is_bypass;
    int lanes_are_swapped = 0;
    SOCDNX_INIT_FUNC_DEFS;

    /*Get {MAC} or {MAC and PHY} reset state*/
    flags = (loopback_type == portmodLoopbackMacRloop)? PORTMOD_PORT_ENABLE_MAC : 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_get(unit, port, pm_info, flags, &enabled));

    /*Disable {MAC} or {MAC and PHY}*/
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_set(unit, port, pm_info, flags, 0));

    /* loopback type validation*/
    switch(loopback_type){
    case portmodLoopbackMacAsyncFifo:
    case portmodLoopbackMacOuter:
    case portmodLoopbackMacRloop:
        break;
    case portmodLoopbackMacPCS:
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_encoding_get(unit, port, pm_info, &encoding_properties ,&encoding));
        if((encoding != PORTMOD_PCS_64B66B_FEC) && (encoding != PORTMOD_PCS_64B66B)){
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("unsupported encoding type %d for MAC PCS loopback"), encoding));
        }
        break;
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD: /*slide*/
        is_phy_loopback = TRUE;
        break;
    /*PHY PCS modes and MAC core are not supported*/
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("unsupported loopback type %d"), loopback_type));
    }

    /*check if not already defined*/
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_loopback_get(unit, port, pm_info, loopback_type, &lb_enable));
    if(enable == lb_enable){
        SOC_EXIT;
    }

    /*loopback set*/
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_are_lanes_swapped(unit, port, pm_info, &lanes_are_swapped));
    if(is_phy_loopback){
        if(enable && lanes_are_swapped)
        {
            if (loopback_type == portmodLoopbackPhyGloopPMD) /* limitation is due to lane swap between phy and mac - can be aligned by SW */
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("configuring phy loopback on a lane-swapped port is not supported, align lane-swap to be one-to-one or configure mac loopback")));
            } else if (loopback_type == portmodLoopbackPhyRloopPMD) /* limitation is due to physical lane swap between board and phy */
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("configuring remote phy loopback on a lane-swapped port is not supported, only mac loopback is supported")));
            }
        }
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_loopback_set(unit, port, pm_info,loopback_type, enable));
    } else { /*MAC looopback*/
        if(enable && lanes_are_swapped && (loopback_type == portmodLoopbackMacOuter)) /* limitation is due to lane swap between phy and mac - can be aligned by SW */
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("configuring mac outer loopback on a lane-swapped port is not supported, align lane-swap to be one-to-one or configure mac async loopback")));
        }
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_mac_loopback_set(unit, port, pm_info, loopback_type, enable));
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    rv = PM8X50_FABRIC_LANE_IS_BYPASSED_GET(unit, pm_info, &is_bypass, phy_index);
    SOCDNX_IF_ERR_EXIT(rv);
    if(!is_bypass) {
        SOCDNX_IF_ERR_EXIT(_pm8x50_fabric_loopback_serdes_overrides_set(unit, port, pm_info));
    }
    /*restore {MAC} or {MAC and PHY} */
    flags = (loopback_type == portmodLoopbackMacRloop)? PORTMOD_PORT_ENABLE_MAC : 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_set(unit, port, pm_info, flags, enabled));

exit:
    SOCDNX_FUNC_RETURN; 
}


int pm8x50_fabric_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{
    uint32 reg_val = 0;
    uint32 field[1] = {0};
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    uint32 phy_index, bitmap;
    int is_enabled = 0;
    int is_remote_loopback = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);

    switch(loopback_type){
    case portmodLoopbackMacAsyncFifo:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
        *enable = soc_reg_field_get(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, reg_val, FMALN_CORE_LOOPBACKf);
        break;
    case portmodLoopbackMacOuter:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_LOOPBACK_ENABLE_REGISTERr(unit, fmac_blk_id, &reg_val));
        *field = soc_reg_field_get(unit, FMAC_LOOPBACK_ENABLE_REGISTERr, reg_val, LCL_LPBK_ONf);
        *enable = SHR_BITGET(field, lane_index_in_fmac);
        break;
    case portmodLoopbackMacPCS:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
        *enable = soc_reg_field_get(unit, FMAC_KPCS_CONFIGURATIONr, reg_val, KPCS_N_RX_DSC_LOOPBACK_ENf);
        break;
    case portmodLoopbackMacRloop:
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_retimer_get(unit, port, pm_info, &is_remote_loopback, &is_enabled));
        *enable = (is_remote_loopback && is_enabled) ? TRUE : FALSE;
        break;
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD: /*slide*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_loopback_get(unit, port, pm_info, loopback_type, enable));
        break;
    /*PHY pcs modes and MAC core are not supported*/
    default:
        *enable = 0; /*not supported - no loopback*/
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

#define PM8X50_FABRIC_PORT_RETIMER_ILKN_FIFO_THR(is_retimer_enabled) ((is_retimer_enabled) ? (18) : (5))
#define PM8X50_FABRIC_PORT_RETIMER_AUTO_ADJUST_MAX(is_retimer_enabled) ((is_retimer_enabled) ? (0x4) : (0xa))
#define PM8X50_FABRIC_PORT_RETIMER_AUTO_ADJUST_PERIOD_FIELD_VAL(is_retimer_enabled) ((is_retimer_enabled) ? (0xffffff) : (0x7d0))
#define PM8X50_FABRIC_PORT_RETIMER_AUTO_ADJUST_PERIOD_USEC (20000)
#define PM8X50_FABRIC_PORT_RETIMER_BUCKET_LINK_DOWN_THR(is_retimer_enabled) ((is_retimer_enabled) ? (0x3f) : (0x10))
#define PM8X50_FABRIC_PORT_RETIMER_FABRIC_MAC_MODE(is_rs_fec_mode) ((is_rs_fec_mode) ? (2) : (0))

/**
 * \brief
 *   Set link as re-timer.
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   The link to set as retimer.
 * \param [in] is_remote_loopback -
 *   True to set loopback retimer, False to set pass-through retimer.
 * \param [in] enable -
 *   Enable/Disable the retimer.
 * \return
 *   See \ref shr_error_e.
 * \remark
 *   Pass-through retimer and loopback retimer configurations are the same,
 *   except for 1 field which choose between the modes.
 * \see
 *   None.
 */
static int 
pm8x50_fabric_port_retimer_set(int unit, soc_port_t port, pm_info_t pm_info, int is_remote_loopback, int enable)
{
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks = 0, first_fmac_index = 0;
    uint32 phy_index = 0, bitmap = 0;
    uint32 reg32_val = 0;
    uint64 reg64_val;
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 encoding_properties = 0;
    portmod_port_pcs_t encoding = 0;
    int fabric_mac_mode = 0;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys;
    int retimer_fec_enable;
    portmod_speed_config_t speed_config;
    int slow_read_rate, fmal_width;
    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg64_val);
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    if(nof_fmac_blks > 1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                (_BSL_SOCDNX_MSG("More than one FMAC per fabric port"), port));
    }

    /*
     * Lock Phase Interpolator
     */
    SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));
    SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
    SOCDNX_IF_ERR_EXIT(phymod_phy_tx_phase_lock_set(&phy_access, enable));

    /* DC auto adjust settings */
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_DC_AUTO_ADJUSTr(unit, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg64_val));
    soc_reg64_field32_set(unit, FMAC_FMAL_TX_DC_AUTO_ADJUSTr, &reg64_val, FMAL_N_TX_DC_AUTO_ADJUST_ENf, (enable) ? (1) : (0));
    soc_reg64_field32_set(unit, FMAC_FMAL_TX_DC_AUTO_ADJUSTr, &reg64_val, FMAL_N_TX_DC_AUTO_ADJUST_MINf, 0x2); /* the HW value of MIN - 0x2 - is suitable */
    soc_reg64_field32_set(unit, FMAC_FMAL_TX_DC_AUTO_ADJUSTr, &reg64_val, FMAL_N_TX_DC_AUTO_ADJUST_MAXf, PM8X50_FABRIC_PORT_RETIMER_AUTO_ADJUST_MAX(enable));
    soc_reg64_field32_set(unit, FMAC_FMAL_TX_DC_AUTO_ADJUSTr, &reg64_val, FMAL_03_TX_DC_AUTO_ADJUST_FIELD_8f, PM8X50_FABRIC_PORT_RETIMER_AUTO_ADJUST_PERIOD_FIELD_VAL(enable));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_DC_AUTO_ADJUSTr(unit, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg64_val));

    /* Set allowed fullness level of ILKN FIFO */
    /* Fmal[n]TxIlknFifoThr */
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg64_val));
    soc_reg64_field32_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg64_val, AUTO_DOC_NAME_50f, PM8X50_FABRIC_PORT_RETIMER_ILKN_FIFO_THR(enable));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg64_val));

    /* Disable asynchronous FIFO */
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_encoding_get(unit, port, pm_info, &encoding_properties, &encoding));
    fabric_mac_mode = PM8X50_FABRIC_PORT_RETIMER_FABRIC_MAC_MODE(!enable && !PM8X50_FABRIC_ENCODING_IS_RS_FEC(encoding));
    SOCDNX_IF_ERR_EXIT(READ_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg64_val));
    soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_N_TX_WIDTHf, fabric_mac_mode);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg64_val));

    /* Set the link as down in MAC leacky bucket */
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg32_val));
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg32_val, BKT_LINK_DN_TH_Nf, PM8X50_FABRIC_PORT_RETIMER_BUCKET_LINK_DOWN_THR(enable));
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg32_val));

    /* async FIFO configuration */
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_retimer_fec_enable_get(unit, port, pm_info, &retimer_fec_enable));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_speed_config_get(unit, port, pm_info, &speed_config));
    /* there are special configurations for Async FIFO when enabling retimer without FEC. These configurations depend on FEC from some reason */
    if (speed_config.fec == PORTMOD_PORT_PHY_FEC_NONE)
    {
        fmal_width = (enable && !retimer_fec_enable)? PM8X50_FABRIC_FEC_NONE_FMAL_WIDTH_RETIMER_FEC_DISABLED : PM8X50_FABRIC_FEC_NONE_FMAL_WIDTH;
        SOCDNX_IF_ERR_EXIT(READ_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg64_val));
        soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_N_RX_WIDTHf, fmal_width);
        soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_N_TX_WIDTHf, fmal_width);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg64_val));
    }
    else if (speed_config.fec == PORTMOD_PORT_PHY_FEC_BASE_R)
    {
        slow_read_rate = (enable && !retimer_fec_enable)? PM8X50_FABRIC_FEC_BASE_R_SLOW_READ_RATE_RETIMER_FEC_DISABLED : PM8X50_FABRIC_FEC_BASE_R_SLOW_READ_RATE;
        fmal_width = (enable && !retimer_fec_enable)? PM8X50_FABRIC_FEC_BASE_R_FMAL_WIDTH_RETIMER_FEC_DISABLED : PM8X50_FABRIC_FEC_BASE_R_FMAL_WIDTH;
        SOCDNX_IF_ERR_EXIT(READ_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg64_val));
        soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_N_RX_SLOW_READ_RATEf, slow_read_rate);
        soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_N_RX_WIDTHf, fmal_width);
        soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_N_TX_WIDTHf, fmal_width);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg64_val));
    }

    if (enable)
    {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_DC_AUTO_ADJUSTr(unit, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg64_val));
        soc_reg64_field32_set(unit, FMAC_FMAL_TX_DC_AUTO_ADJUSTr, &reg64_val, FMAL_03_TX_DC_AUTO_ADJUST_FIELD_7f, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_DC_AUTO_ADJUSTr(unit, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg64_val));
    }

    /* Enable retimer */
    SOCDNX_IF_ERR_EXIT(READ_FMAC_RETIMERr(unit, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg_above_64_val));
    soc_reg_above_64_field32_set(unit, FMAC_RETIMERr, reg_above_64_val, FMAL_N_RETIMER_ENf, (enable) ? (1) : (0));
    soc_reg_above_64_field32_set(unit, FMAC_RETIMERr, reg_above_64_val, FMAL_N_RETIMER_REMOTE_LOOPBACK_ENf, (is_remote_loopback) ? (1) : (0));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RETIMERr(unit, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg_above_64_val));


exit:
    SOCDNX_FUNC_RETURN; 
}

/**
 * \brief
 *   Get link re-timer state.
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   The link to set as retimer.
 * \param [out] is_remote_loopback -
 *   True - loopback retimer, False - pass-through retimer.
 * \param [out] enable -
 *   Is the retimer enabled.
 * \return
 *   See \ref shr_error_e.
 * \remark
 *   None.
 * \see
 *   None.
 */
static int 
pm8x50_fabric_port_retimer_get(int unit, soc_port_t port, pm_info_t pm_info, int *is_remote_loopback, int *enable)
{
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks = 0, first_fmac_index = 0;
    uint32 phy_index = 0, bitmap = 0;
    soc_reg_above_64_val_t reg_above_64_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    if(nof_fmac_blks > 1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                (_BSL_SOCDNX_MSG("More than one FMAC per fabric port"), port));
    }

    SOCDNX_IF_ERR_EXIT(READ_FMAC_RETIMERr(unit, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg_above_64_val));
    *enable = soc_reg_above_64_field32_get(unit, FMAC_RETIMERr, reg_above_64_val, FMAL_N_RETIMER_ENf);
    *is_remote_loopback = soc_reg_above_64_field32_get(unit, FMAC_RETIMERr, reg_above_64_val, FMAL_N_RETIMER_REMOTE_LOOPBACK_ENf);

exit:
    SOCDNX_FUNC_RETURN; 
}

/**
 * \brief
 *   Set link as a pass-through re-timer.
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   The link to set as retimer.
 * \param [in] pm_info -
 *   Port Macro info (provided by the dispatcher).
 * \param [in] enable -
 *   Enable/Disable the retimer.
 * \return
 *   See \ref shr_error_e.
 * \remark
 *   This function must be called twice, for each link of the retimer links.
 * \see
 *   None.
 */
int 
pm8x50_fabric_port_pass_through_set(int unit, soc_port_t port, pm_info_t pm_info, int loopback_en, int enable)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_retimer_set(unit, port, pm_info, loopback_en, enable));

exit:
    SOCDNX_FUNC_RETURN; 
}

/**
 * \brief
 *   Get if a link is set to work as a pass-through re-timer.
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   The link to get retimer info about.
 * \param [in] pm_info -
 *   Port Macro info (provided by the dispatcher).
 * \param [out] enable -
 *   Is the retimer enabled.
 * \return
 *   See \ref shr_error_e.
 * \remark
 *   If link set to be a loopback retimer (mac remote loopback) this
 *   function will return False in the 'enable' param.
 * \see
 *   None.
 */
int 
pm8x50_fabric_port_pass_through_get(int unit, soc_port_t port, pm_info_t pm_info, int *loopback_en, int *enable)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_retimer_get(unit, port, pm_info, loopback_en, enable));

exit:
    SOCDNX_FUNC_RETURN; 
}

/**
 * \brief
 *   Enable/disable FEC on a re-timer link.
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   The re-timer link on which to enable/disable FEC.
 * \param [in] pm_info -
 *   Port Macro info (provided by the dispatcher).
 * \param [in] enable -
 *   Enable/Disable the FEC.
 * \return
 *   See \ref shr_error_e.
 * \remark
 *   1. disable FEC means that no encoding is done at all, not even 64/66.
 *   2. in case of pass-through re-timer  or loopback re-timer which is not on the same link -
 *   in the common use case, this configuration should be set twice, for each of the retimer links. Except maybe for debug.
 *
 * \see
 *   None.
 */
static int
pm8x50_fabric_port_retimer_fec_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks = 0, first_fmac_index = 0;
    uint32 phy_index = 0, bitmap = 0;
    soc_reg_above_64_val_t reg_above_64_val;
    int lane_index_in_fmac;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    if(nof_fmac_blks != 1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                (_BSL_SOCDNX_MSG("invalid number of FMACs for port %d"), port));
    }

    lane_index_in_fmac = PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index);
    SOCDNX_IF_ERR_EXIT(READ_FMAC_RETIMERr(unit, fmac_blk_ids[first_fmac_index], lane_index_in_fmac, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, FMAC_RETIMERr, reg_above_64_val, FMAL_N_RETIMER_FEC_ENf, (enable) ? (1) : (0));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RETIMERr(unit, fmac_blk_ids[first_fmac_index], lane_index_in_fmac, reg_above_64_val));

exit:
    SOCDNX_FUNC_RETURN;
}

/**
 * \brief
 *   Get if FEC is enabled on a re-timer link.
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   The retimer link to get FEC info about.
 * \param [in] pm_info -
 *   Port Macro info (provided by the dispatcher).
 * \param [out] enable -
 *   Is the FEC enabled on the re-timer link.
 * \return
 *   See \ref shr_error_e.
 * \remark
 *   Enabling the FEC in retimer mode should be done before actually configuring the link as a retimer.
 *   So enable can be 1 before the link is actually a retimer link.
 * \see
 *   None.
 */
static int
pm8x50_fabric_port_retimer_fec_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks = 0, first_fmac_index = 0;
    uint32 phy_index = 0, bitmap = 0;
    soc_reg_above_64_val_t reg_above_64_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    if(nof_fmac_blks != 1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                (_BSL_SOCDNX_MSG("invalid number of FMACs for port %d"), port));
    }

    SOCDNX_IF_ERR_EXIT(READ_FMAC_RETIMERr(unit, fmac_blk_ids[first_fmac_index], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg_above_64_val));
    *enable = soc_reg_above_64_field32_get(unit, FMAC_RETIMERr, reg_above_64_val, FMAL_N_RETIMER_FEC_ENf);

exit:
    SOCDNX_FUNC_RETURN;
}

/**
 * \brief
 *   Enable/disable FEC for a re-timer link.
 *   The configuration should be set only BEFORE configuring the link as a re-timer link.
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   The link on which to enable/disable FEC.
 * \param [in] pm_info -
 *   Port Macro info (provided by the dispatcher).
 * \param [in] enable -
 *   Enable/Disable the FEC.
 * \return
 *   See \ref shr_error_e.
 * \remark
 *   1. disable FEC means that no encoding is done at all, not even 64/66.
 *   2. in case of pass-through re-timer or loopback re-timer which is not on the same link -
 *   in the common use case, this configuration should be set twice, for each of the retimer links. Except maybe for debug.
 *
 * \see
 *   None.
 */
int
pm8x50_fabric_port_pass_through_fec_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_retimer_fec_enable_set(unit, port, pm_info, enable));

exit:
    SOCDNX_FUNC_RETURN;
}

/**
 * \brief
 *   Get if FEC is enabled on a re-timer link.
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   The retimer link to get FEC info about.
 * \param [in] pm_info -
 *   Port Macro info (provided by the dispatcher).
 * \param [out] enable -
 *   Is the FEC enabled on the re-timer link.
 * \return
 *   See \ref shr_error_e.
 * \remark
 *   Enabling the FEC in retimer mode should be done before actually configuring the link as a retimer.
 *   So enable can be 1 before the link is actually a retimer link.
 * \see
 *   None.
 */
int
pm8x50_fabric_port_pass_through_fec_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_retimer_fec_enable_get(unit, port, pm_info, enable));

exit:
    SOCDNX_FUNC_RETURN;
}

/**
 * \brief
 *   DC calibration (auto-adjust) of ILKN FIFO for a retimer link.
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   The link on which to auto-adjust the ILKN FIFO.
 * \param [in] pm_info -
 *   Port Macro info (provided by the dispatcher).
 * \return
 *   See \ref shr_error_e.
 * \remark
 *   None.
 *
 * \see
 *   None.
 */
int
pm8x50_fabric_port_retimer_calibrate(int unit, int port, pm_info_t pm_info)
{
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks = 0, first_fmac_index = 0;
    uint32 phy_index = 0, bitmap = 0;
    uint64 reg64_val;
    int initial_max_val, max_val, max_val_upper_bound, auto_adjust_lock;
    int lane_index_in_fmac;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));
    lane_index_in_fmac = PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index);

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_DC_AUTO_ADJUSTr(unit, fmac_blk_ids[first_fmac_index], lane_index_in_fmac, &reg64_val));
    soc_reg64_field32_set(unit, FMAC_FMAL_TX_DC_AUTO_ADJUSTr, &reg64_val, FMAL_03_TX_DC_AUTO_ADJUST_FIELD_7f, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_DC_AUTO_ADJUSTr(unit, fmac_blk_ids[first_fmac_index], lane_index_in_fmac, reg64_val));

    initial_max_val = soc_reg64_field32_get(unit, FMAC_FMAL_TX_DC_AUTO_ADJUSTr, reg64_val, FMAL_N_TX_DC_AUTO_ADJUST_MAXf);

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, fmac_blk_ids[first_fmac_index], lane_index_in_fmac, &reg64_val));
    max_val_upper_bound = soc_reg64_field32_get(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, reg64_val, AUTO_DOC_NAME_50f);

    for (max_val = initial_max_val; max_val < max_val_upper_bound; ++max_val)
    {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_DC_AUTO_ADJUSTr(unit, fmac_blk_ids[first_fmac_index], lane_index_in_fmac, &reg64_val));
        soc_reg64_field32_set(unit, FMAC_FMAL_TX_DC_AUTO_ADJUSTr, &reg64_val, FMAL_N_TX_DC_AUTO_ADJUST_MAXf, max_val);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_DC_AUTO_ADJUSTr(unit, fmac_blk_ids[first_fmac_index], lane_index_in_fmac, reg64_val));

        sal_usleep(PM8X50_FABRIC_PORT_RETIMER_AUTO_ADJUST_PERIOD_USEC);

        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_DC_AUTO_ADJUSTr(unit, fmac_blk_ids[first_fmac_index], lane_index_in_fmac, &reg64_val));
        auto_adjust_lock = soc_reg64_field32_get(unit, FMAC_FMAL_TX_DC_AUTO_ADJUSTr, reg64_val, FMAL_N_TX_DC_AUTO_ADJUST_LOCKf);
        if (auto_adjust_lock)
        {
            break;
        }
    }

    if (max_val == max_val_upper_bound)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("port %d: retimer calibration failed\n"), port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/**
 * \brief
 *   DC calibration (auto-adjust) of ILKN FIFO for a retimer link.
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   The link on which to auto-adjust the ILKN FIFO.
 * \param [in] pm_info -
 *   Port Macro info (provided by the dispatcher).
 * \return
 *   See \ref shr_error_e.
 * \remark
 *   None.
 *
 * \see
 *   None.
 */
int
pm8x50_fabric_port_pass_through_calibrate(int unit, int port, pm_info_t pm_info)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_retimer_calibrate(unit, port, pm_info));

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_link_get(int unit, int port, pm_info_t pm_info, int flag, int* link)
{
    int enable,
        rv;
    uint32 phy_index, bitmap;
    uint32 
        is_down=0,
        reg_val = 0,
        fld_val[1] = {0},
        rx_pmd_locked;
    phymod_phy_access_t phy_access[1];
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    portmod_access_get_params_t params;
    int nof_phys;
    int is_mac_lb_enabled = 0;
    SOCDNX_INIT_FUNC_DEFS;

    /*link is down unless we find it's up*/
    *link = 0;

    /*check if port is enabled*/
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_get(unit, port, pm_info, 0, &enable));

    if (enable) {
        /* Check if there is a MAC LB set on the lane*/
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_loopback_get(unit, port, pm_info, portmodLoopbackMacAsyncFifo, &is_mac_lb_enabled));

        /*clear rxlos interrupt*/
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
        reg_val = 0;
        SHR_BITSET(fld_val, lane_index_in_fmac);
        soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, &reg_val, RX_LOST_OF_SYNCf, *fld_val);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit, fmac_blk_id, reg_val));
        /*check if Serdes is locked*/
        SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));
        params.phyn = 0;
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, phy_access, &nof_phys, NULL));
        rv = phymod_phy_rx_pmd_locked_get(phy_access, &rx_pmd_locked);
        if (rv != PHYMOD_E_NONE) {
            return rv;
        }

        /* IF MAC LB is set SerDes is expected to be not locked. In this case only the MAC will be locked*/
        if(rx_pmd_locked == 1 || is_mac_lb_enabled)
        {
            /*check if FMAC is synced - check rxlos interrupt*/
            SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit, fmac_blk_id, &reg_val));

            *fld_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, reg_val, RX_LOST_OF_SYNCf);
            is_down = SHR_BITGET(fld_val, lane_index_in_fmac);
            *link = (is_down == 0);
        }
    }
exit:
    SOCDNX_FUNC_RETURN; 
    
}

int pm8x50_fabric_port_prbs_config_set(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, const phymod_prbs_t* config)
{
    uint32 reg_val;
    uint32 phy_index, bitmap;
    uint32 poly_val = 0;
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode == 0){ /*phy*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_prbs_config_set(unit, port, pm_info, flags, config));
    }
    else{
        switch(config->poly){
        case phymodPrbsPoly31:
            poly_val = 1;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported polynomial for MAC PRBS %d"), config->poly));
        }
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
        if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
            soc_reg_field_set(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, &reg_val, KPCS_N_TST_RX_PTRN_SELf, poly_val);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_val));
        }
        if(PHYMOD_PRBS_DIRECTION_TX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
            soc_reg_field_set(unit, FMAC_KPCS_TEST_TX_CONFIGURATIONr, &reg_val, KPCS_N_TST_TX_PTRN_SELf, poly_val);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_val));
        }
    }
exit:
    SOCDNX_FUNC_RETURN; 
    
}

int pm8x50_fabric_port_prbs_config_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, phymod_prbs_t* config)
{
    uint32 reg_val;
    uint32 tx_poly;
    uint32 poly_val = 0; 
    uint32 phy_index, bitmap;
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode == 0){ /*phy*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_prbs_config_get(unit, port, pm_info, flags, config));
    }
    else{
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
        if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
            poly_val = soc_reg_field_get(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, reg_val, KPCS_N_TST_RX_PTRN_SELf);
        }
        if(PHYMOD_PRBS_DIRECTION_TX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
            tx_poly = soc_reg_field_get(unit, FMAC_KPCS_TEST_TX_CONFIGURATIONr, reg_val, KPCS_N_TST_TX_PTRN_SELf);
            if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
                if(poly_val != tx_poly){
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("rx and tx are not configured the same")));
                }
            }
            poly_val = tx_poly;
        }
        switch(poly_val){
        case 1:
            config->poly = phymodPrbsPoly31;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unknown polynomial %u"), poly_val));
        }
        /*no meaning for invert in MAC PRBS*/
        config->invert = 0;
    }

exit:
    SOCDNX_FUNC_RETURN; 
    
}


int pm8x50_fabric_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, int enable)
{
    uint32 reg_val;
    uint32 phy_index, bitmap;
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode == 0){ /*phy*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_prbs_enable_set(unit, port, pm_info, flags, enable));
    }
    else{
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
        if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
            soc_reg_field_set(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, &reg_val, KPCS_N_TST_RX_ENf, enable ? 1 : 0);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_val));
        }
        if(PHYMOD_PRBS_DIRECTION_TX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
            soc_reg_field_set(unit, FMAC_KPCS_TEST_TX_CONFIGURATIONr, &reg_val, KPCS_N_TST_TX_ENf, enable ? 1 : 0);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, reg_val));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int pm8x50_fabric_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, int* enable)
{
    uint32 reg_val;
    int tx_enable;
    uint32 phy_index, bitmap;
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode == 0){ /*phy*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_prbs_enable_get(unit, port, pm_info, flags, enable));
    }
    else{
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
        if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
            *enable = soc_reg_field_get(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, reg_val, KPCS_N_TST_RX_ENf);
        }
        if(PHYMOD_PRBS_DIRECTION_TX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
            tx_enable = soc_reg_field_get(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, reg_val, KPCS_N_TST_RX_ENf);
            /*validate rx and tx are configured the same*/
            if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
                if(*enable != tx_enable){
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("rx and tx are not configured the same")));
                }
            }
            *enable = tx_enable;
        }
    }        
exit:
    SOCDNX_FUNC_RETURN;
}


int pm8x50_fabric_port_prbs_status_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, phymod_prbs_status_t* status)
{
    uint32 reg_val;
    uint32 phy_index, bitmap;
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode == 0){ /*phy*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_prbs_status_get(unit, port, pm_info, flags, status));
    } else {
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        PM8X50_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
        SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_STATUSr(unit, fmac_blk_id, lane_index_in_fmac, &reg_val));
        status->prbs_lock = (reg_val >>  PM8X50_FABRIC_MAC_PRBS_LOCK_SHIFT) & PM8X50_FABRIC_MAC_PRBS_LOCK_MASK;
        status->prbs_lock_loss = 0; /*Not supported*/
        if (status->prbs_lock) {
            status->error_count = (reg_val >>  PM8X50_FABRIC_MAC_PRBS_CTR_SHIFT) & PM8X50_FABRIC_MAC_PRBS_CTR_MASK;
        } else {
            status->error_count = -1;
        }
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

int
pm8x50_fabric_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, const portmod_access_get_params_t* params, int max_phys, 
                                    phymod_phy_access_t* phy_access_arr, int* nof_phys, int* is_most_ext)
{
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    int rv;
    uint32 phy_index, is_bypass, bitmap;
    int lane_index, lane_count;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    rv = PM8X50_FABRIC_LANE_IS_BYPASSED_GET(unit, pm_info, &is_bypass, phy_index);
    SOCDNX_IF_ERR_EXIT(rv);

    if(max_phys < 1){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("array should be at size 1 at least")));
    }
    if(params->lane != 0 && params->lane != -1 && !is_bypass){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("fabric ports are single lane only")));
    }
    
    if(params->phyn != 0 && params->phyn != -1){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("external phy is not supported")));
    }
    
    SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access_arr[0]));

    *nof_phys = 1;

    if (is_bypass) {
        sal_memcpy(&phy_access_arr[0].access, &fabric_data->access, sizeof(fabric_data->access)); 
        if (params->lane == PORTMOD_ALL_LANES_ARE_ACTIVE) {
            phy_access_arr[0].access.lane_mask = bitmap;
        } else {
            lane_count = 0;
            phy_access_arr[0].access.lane_mask = 0;
            PM8X50_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index)
            {
                if (lane_count == params->lane)
                {
                    phy_access_arr[0].access.lane_mask = 1 << lane_index;
                    break;
                }
                ++lane_count;
            }
            if (phy_access_arr[0].access.lane_mask == 0) {
                *nof_phys = 0;
            }
        }
        phy_access_arr[0].type = fabric_data->core_type;
    } else {
        sal_memcpy(&phy_access_arr[0].access, &fabric_data->access, sizeof(fabric_data->access)); 
        SHR_BITSET(&phy_access_arr[0].access.lane_mask, phy_index);
        phy_access_arr[0].type = fabric_data->core_type;
    }

    if(is_most_ext) {
        *is_most_ext = 1;
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

/*
 * Get lane phymod access structure via pm id.
 * This function can be used for getting phy access
 * when port is not attached.
 *
 * If a single lane access is required, please specify the params.lane.
 * Otherwise, set params.lane = -1, meaning get lane access for all the
 * lanes in the given PM.
 */
int pm8x50_fabric_pm_phy_lane_access_get(
    int unit, int pm_id, pm_info_t pm_info,
    const portmod_access_get_params_t* params,
    int max_phys, phymod_phy_access_t* phy_access,
    int* nof_phys, int* is_most_ext)
{
    int i;
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;

    SOCDNX_INIT_FUNC_DEFS;

    for( i = 0 ; i < max_phys; i++) {
        SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access[i]));
    }

    *nof_phys = 1;

    /* internal core */
    sal_memcpy(&phy_access[0].access, &fabric_data->access, sizeof(fabric_data->access)); 

    if (params->lane != -1) {
        /*
         * If lane index is not -1, need to check if there is any port attched on
         * the given lane. If there is active port on the lane, need to check if
         * the lane is working at bypass mode.
         */
        phy_access[0].access.lane_mask = (0x1 << (params->lane));
    } else {
        /*
         * If lane index is -1, assume all the lanes are accessed
         */
        phy_access[0].access.lane_mask = (0x1 << PM8X50_FABRIC_MAX_PORTS_PER_PM) - 1;
    }
    /* only line is availabe for internal. */
    phy_access[0].port_loc = phymodPortLocLine;

    if (is_most_ext) {
        *is_most_ext = 1;
    }
exit:
    SOCDNX_FUNC_RETURN;
}


int pm8x50_fabric_port_update(int unit, int port, pm_info_t pm_info, const portmod_port_update_control_t* update_control)
{
    SOCDNX_INIT_FUNC_DEFS;
    /*Do nothing*/
    SOCDNX_FUNC_RETURN;
}

int
pm8x50_fabric_port_core_access_get(int unit, int port, pm_info_t pm_info, int phyn, int max_cores, 
                                phymod_core_access_t* core_access_arr, int* cores_num, int* is_most_ext)
{
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    if(max_cores < 1){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("array should be at size 1 at least")));
    }
    if(phyn != 0 && phyn != -1){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("external phy is not supported")));
    }
    
    SOCDNX_IF_ERR_EXIT(phymod_core_access_t_init(&core_access_arr[0]));
    sal_memcpy(&core_access_arr[0].access, &fabric_data->access, sizeof(fabric_data->access));
    core_access_arr[0].type = fabric_data->core_type;
    *cores_num = 1;

    if(is_most_ext) {
        *is_most_ext = 1;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_mode_set (int unit, soc_port_t port, 
                     pm_info_t pm_info, const portmod_port_mode_info_t *mode)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("mode set isn't supported")));

exit:
    SOCDNX_FUNC_RETURN;  
}

int pm8x50_fabric_port_mode_get (int unit, soc_port_t port, 
                     pm_info_t pm_info, portmod_port_mode_info_t *mode)
{
    SOCDNX_INIT_FUNC_DEFS;

    mode->lanes = 1;
    mode->cur_mode = portmodPortModeSingle;
    
    SOCDNX_FUNC_RETURN;
}

STATIC
void  _pm8x50_fabric_ber_proj_port_to_phy_get(soc_port_phy_ber_proj_params_t* params,
                                             phymod_phy_ber_proj_options_t* options)
{
    options->ber_proj_phase = params->ber_proj_phase;
    options->ber_proj_hist_errcnt_thresh = params->ber_proj_hist_errcnt_thresh;
    options->ber_proj_timeout_s = params->ber_proj_timeout_s;
    switch (params->ber_proj_fec_type) {
        case PORTMOD_PORT_PHY_FEC_RS_544:
            options->ber_proj_fec_size = PORTMOD_PORT_FEC_FRAME_SIZE_RS_544;
            break;
        default:
            options->ber_proj_fec_size = PORTMOD_PORT_FEC_FRAME_SIZE_INVALID;
    }
    options->ber_proj_prbs_errcnt = (phymod_phy_ber_proj_errcnt_t*)params->ber_proj_prbs_errcnt;
}

int pm8x50_fabric_port_diag_ctrl(int unit, soc_port_t port, pm_info_t pm_info,uint32 inst, int op_type, 
    int op_cmd, const void *arg) 
{
    phymod_phy_access_t phy_access[1];
    int nof_phys, i;
    portmod_access_get_params_t params;
    phymod_tx_t  ln_txparam[PHYMOD_MAX_LANES_PER_CORE];
    phymod_phy_ber_proj_options_t options;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));  
    params.phyn = 0;
    
    for (i = 0; i < PHYMOD_MAX_LANES_PER_CORE; i++) {
        SOC_IF_ERROR_RETURN(phymod_tx_t_init(&ln_txparam[i]));
    }

    switch(op_cmd) {
        case PHY_DIAG_CTRL_DSC:
            LOG_INFO(BSL_LS_SOC_PHY,(BSL_META_U(unit, 
                "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_DSC 0x%x\n"), unit, 
                port, PHY_DIAG_CTRL_DSC));
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, 
                phy_access, &nof_phys, NULL));
            if((*phy_access).access.lane_mask){
                SOC_IF_ERROR_RETURN(phymod_phy_pmd_info_dump(phy_access, (void*)arg));
            }
            break;

       case PHY_DIAG_CTRL_BER_PROJ:
            LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_BER_PROJ 0x%x\n"), unit, port,
                 PHY_DIAG_CTRL_BER_PROJ));
            _pm8x50_fabric_ber_proj_port_to_phy_get((soc_port_phy_ber_proj_params_t*)arg, &options);
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1,
                phy_access, &nof_phys, NULL));
            if((*phy_access).access.lane_mask){
                SOC_IF_ERROR_RETURN(phymod_phy_ber_proj(phy_access, phymodBERProjModePostFEC, &options));
            }
            break;

       default:
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, 
                phy_access, &nof_phys, NULL));
            
            if(op_type == PHY_DIAG_CTRL_SET) {
                LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                    "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_SET 0x%x\n"),
                    unit, port, PHY_DIAG_CTRL_SET));
                if (!SAL_BOOT_SIMULATION) {
                    if( !((*phy_access).access.lane_mask == 0)){
                        SOC_IF_ERROR_RETURN(portmod_pm_phy_control_set(phy_access, nof_phys, 
                            op_cmd, ln_txparam, (*phy_access).access.lane_mask, PTR_TO_INT(arg)));
                    }
                }
            } else if(op_type == PHY_DIAG_CTRL_GET) {
                LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                    "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_GET 0x%x\n"),
                    unit, port, PHY_DIAG_CTRL_GET));
                if (!SAL_BOOT_SIMULATION) {
                    if( !((*phy_access).access.lane_mask == 0)){
                        SOC_IF_ERROR_RETURN(portmod_pm_phy_control_get(phy_access,nof_phys,
                            op_cmd, ln_txparam, (*phy_access).access.lane_mask, (uint32 *)arg));
                    } else {
                        *(uint32 *)arg = 0;
                    }
                }
            } else {
                SOC_IF_ERROR_RETURN(SOC_E_UNAVAIL);
            }

            break;
    }

exit:
    SOCDNX_FUNC_RETURN;

}

int pm8x50_fabric_port_polarity_set(int unit, int port, pm_info_t pm_info, const phymod_polarity_t* polarity)
{
    int rv, i, rx_polarity, tx_polarity;
    SOCDNX_INIT_FUNC_DEFS;

    for (i = 0; i < PM8X50_FABRIC_LANES_PER_CORE; i++) {
        rx_polarity = (polarity->rx_polarity >> i) & 0x1;
        tx_polarity = (polarity->tx_polarity >> i) & 0x1;

        rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_rx], &rx_polarity, i);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_tx], &tx_polarity, i);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_polarity_get(int unit, int port, pm_info_t pm_info, phymod_polarity_t* polarity)
{
    int rv, i, rx_polarity, tx_polarity;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(phymod_polarity_t_init(polarity));

    for (i = 0; i < PM8X50_FABRIC_LANES_PER_CORE; i++) {
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_rx], &rx_polarity, i); 
        SOCDNX_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_tx], &tx_polarity, i);
        SOCDNX_IF_ERR_EXIT(rv);

        polarity->rx_polarity |= rx_polarity << i; 
        polarity->tx_polarity |= tx_polarity << i; 
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
int pm8x50_fabric_port_speed_config_fabric_speed_to_vco_get(int unit, const int speed, portmod_vco_type_t* vco)
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (speed)
    {
        case 20625:
        case 10312:
            *vco = portmodVCO20P625G;
            break;
        case 23000:
        case 11500:
            *vco = portmodVCO23G;
            break;
        case 50000:
        case 25000:
            *vco = portmodVCO25G;
            break;
        case 25781:
            *vco = portmodVCO25P781G;
            break;
        case 53125:
            *vco =  portmodVCO26P562G;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("speed %d not supported"), speed));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
int pm8x50_fabric_port_speed_config_new_to_legacy_encoding_get(int unit, int port, pm_info_t pm_info, const portmod_fec_t new_encoding, portmod_port_pcs_t* legacy_encoding)
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (new_encoding)
    {
        case PORTMOD_PORT_PHY_FEC_NONE:
            *legacy_encoding = PORTMOD_PCS_64B66B;
            break;
        case PORTMOD_PORT_PHY_FEC_BASE_R:
            *legacy_encoding = PORTMOD_PCS_64B66B_FEC;
            break;
        case PORTMOD_PORT_PHY_FEC_RS_206:
            *legacy_encoding = PORTMOD_PCS_64B66B_RS_FEC;
            break;
        case PORTMOD_PORT_PHY_FEC_RS_108:
            *legacy_encoding = PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC;
            break;
        case PORTMOD_PORT_PHY_FEC_RS_545:
            *legacy_encoding = PORTMOD_PCS_64B66B_15T_RS_FEC;
            break;
        case PORTMOD_PORT_PHY_FEC_RS_304:
            *legacy_encoding = PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("invalid FEC %d for fabric port"), new_encoding));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
int pm8x50_fabric_port_speed_config_legacy_to_new_encoding_get(int unit, int port, pm_info_t pm_info, const portmod_port_pcs_t legacy_encoding, portmod_fec_t* new_encoding)
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (legacy_encoding)
    {
        case PORTMOD_PCS_64B66B:
            *new_encoding = PORTMOD_PORT_PHY_FEC_NONE;
            break;
        case PORTMOD_PCS_64B66B_FEC:
            *new_encoding = PORTMOD_PORT_PHY_FEC_BASE_R;
            break;
        case PORTMOD_PCS_64B66B_RS_FEC:
            *new_encoding = PORTMOD_PORT_PHY_FEC_RS_206;
            break;
        case PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC:
            *new_encoding = PORTMOD_PORT_PHY_FEC_RS_108;
            break;
        case PORTMOD_PCS_64B66B_15T_RS_FEC:
            *new_encoding = PORTMOD_PORT_PHY_FEC_RS_545;
            break;
        case PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC:
            *new_encoding = PORTMOD_PORT_PHY_FEC_RS_304;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("invalid FEC %d for fabric port"), legacy_encoding));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
/**
 * Function to get the PLL/VCO value for a given speed for both
 * Fabric link and ILKNoFabric link
 */
int pm8x50_fabric_pm_speed_vco_get(int unit, int speed, int is_bypass, portmod_vco_type_t* required_vco)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(required_vco);

    if (is_bypass)
    {
        pm8x50_shared_pcs_bypassed_vco_get(speed, required_vco);
        if (required_vco == portmodVCOInvalid)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("speed %d not supported"), speed));
        }
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_speed_config_fabric_speed_to_vco_get(unit, speed, required_vco));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

/**
 * Function to get array of current PLLs and
 *  array based on the first one but removed of the PLLs
 *  that will not be required if the current allocation is successful
 */
static int pm8x50_fabric_pm_speed_config_validate_plls_get(int unit,pm_info_t pm_info, const portmod_pbmp_t* ports, portmod_pm_vco_setting_t* vco_setting, int begin_pll, int end_pll, portmod_vco_type_t* pll_vco_rate_curr, portmod_vco_type_t* pll_vco_rate_curr_updated)
{
    int port, speed_index, err_speed_index;
    int pll_index;
    portmod_vco_type_t pll_vco_rate_requested[PM8X50_FABRIC_MAX_NUM_PLLS];
    uint32 requested_lanes_bitmap_required[1];
    uint32 pll_active_lanes_bitmap_curr[PM8X50_FABRIC_MAX_NUM_PLLS];
    portmod_vco_type_t required_vco = portmodVCOInvalid;
    uint32 phy_index, bitmap;
    int lane_index;

    SOCDNX_INIT_FUNC_DEFS;

    /*
     * INIT VARIABLES
     * {
     */
    /** initialize (zero) arrays holding PLLs to be calculated next */
    PM8X50_FABRIC_PLLS_ITER(0, PM8X50_FABRIC_MAX_NUM_PLLS, pll_index)
    {
        pll_vco_rate_requested[pll_index] = portmodVCOInvalid;
        pll_vco_rate_curr_updated[pll_index] = portmodVCOInvalid;
    }
    requested_lanes_bitmap_required[0] = 0;
    PM8X50_FABRIC_PLLS_INIT(pll_vco_rate_curr, pll_active_lanes_bitmap_curr);
    /*
     * }
     */

    /*
     *  1. calculate the required VCO rates based on the speeds given by the user as input, and add ports to the calculated VCOs bitmaps
     */
    /**
     * Get an array of required new vco rates from the requested speeds
     * NOTE! This array is not final for the application ,
     * just to get what plls we need and to validate that the assignemt can happen
     */
    for (speed_index = 0; speed_index < vco_setting->num_speeds; ++speed_index)
    {
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_pm_speed_vco_get(unit, vco_setting->speed_config_list[speed_index].speed, vco_setting->speed_for_pcs_bypass_port[speed_index], &required_vco));

        if ((required_vco != pll_vco_rate_requested[0]) && (required_vco != pll_vco_rate_requested[1]))
        {
            PM8X50_FABRIC_PLLS_ITER(begin_pll, end_pll, pll_index)
            {
                if (pll_vco_rate_requested[pll_index] == portmodVCOInvalid)
                {
                    pll_vco_rate_requested[pll_index] = required_vco;
                    break;
                }
            }
            /*
             * Check if provided sed of ports/speeds can be set theoretically on PM8x50.
             * NOTE! We don't take into account at this point the current VCO allocation set on the PM
             * NOTE! PM8X50_FABRIC_PLLS_ITER iterates backwards so that is why the check is also opposite
             */
            if (pll_index < begin_pll)
            {
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "The given speeds require more than %d PLLs:\n"), end_pll - begin_pll));
                for (err_speed_index = 0; err_speed_index <= speed_index; ++err_speed_index)
                {
                    LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n speed %d\n"), vco_setting->speed_config_list[err_speed_index].speed));
                }
                SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_PARAM);
            }
        }
    }

    /**
     *  Add the lanes of the ports to the required PLLs bitmap
     */
    PORTMOD_PBMP_ITER(*ports, port)
    {
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        PM8X50_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index)
        {
            SHR_BITSET(&requested_lanes_bitmap_required[0], lane_index);
        }
    }

    /**
     *  Get curr VCO rates and bitmaps of the PLL/s and copy them to curr_updated array
     */
    PM8X50_FABRIC_PLLS_ITER(begin_pll, end_pll, pll_index)
    {
        SOCDNX_IF_ERR_EXIT(PM8X50_FABRIC_PLL_VCO_RATE_GET(unit, pm_info, &pll_vco_rate_curr[pll_index], pll_index));
        SOCDNX_IF_ERR_EXIT(PM8X50_FABRIC_PLL_ACTIVE_LANES_BITMAP_GET(unit, pm_info, &pll_active_lanes_bitmap_curr[pll_index], pll_index));

        pll_vco_rate_curr_updated[pll_index] = pll_vco_rate_curr[pll_index];
    }

    /**
     *  Freeing current PLL/s with VCO rate that will not be required anymore after this assignment is over
     */
    PM8X50_FABRIC_PLLS_ITER(begin_pll, end_pll, pll_index)
    {
        /* a PLL is a candidate for freeing only if it has a valid rate and its VCO is different from the required VCO/s */
        if ((pll_vco_rate_curr[pll_index] != portmodVCOInvalid)
                && (pll_vco_rate_curr[pll_index] != pll_vco_rate_requested[0]) && (pll_vco_rate_curr[pll_index] != pll_vco_rate_requested[1]))
        {
            /* if all of the lanes of the current PLL are included in the requested ports */
            if (PM8X50_FABRIC_BITMAP_IS_INCLUDED(pll_active_lanes_bitmap_curr[pll_index], requested_lanes_bitmap_required[0]))
            {
                pll_vco_rate_curr_updated[pll_index] = portmodVCOInvalid;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* Validate a set of speed config within a port macro.
 * The function receives ports pbmp and requested speed to configure for each port.
 * It calculates new TVCO (PLL1), OVCO (PLL0) based on the speeds input.
 * It returns TVCO and OVCO rates and if the rates are different from current rates.
 * This function doesn't do any hardware changes, it does only calculations */
int pm8x50_fabric_pm_speed_config_validate(int unit, int pm_id, pm_info_t pm_info, const portmod_pbmp_t* ports, int flag, portmod_pm_vco_setting_t* vco_setting)
{
    int port, speed_index, preferred_pll_index, other_pll_index;
    uint32 ports_count;
    int end_pll, begin_pll, pll_index;
    portmod_vco_type_t pll_vco_rate_curr[PM8X50_FABRIC_MAX_NUM_PLLS];
    portmod_vco_type_t pll_vco_rate_curr_updated[PM8X50_FABRIC_MAX_NUM_PLLS];
    portmod_vco_type_t required_vco = portmodVCOInvalid;
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    int nof_allowed_plls = 0, other_pll_is_valid = FALSE, preferred_pll_is_valid = FALSE;

    SOCDNX_INIT_FUNC_DEFS;


    /*
     * VALIDATE INPUT
     * {
     */
    if (!(flag & PORTMOD_PM_SPEED_VALIDATE_F_PLL_SWITCH_DEFAULT))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("flag %d is not supported. Only flag %d is supported by DNX"), flag, PORTMOD_PM_SPEED_VALIDATE_F_PLL_SWITCH_DEFAULT));
    }

    PORTMOD_PBMP_COUNT(*ports, ports_count);
    if (ports_count != vco_setting->num_speeds)
    {
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Regarding the core with the following ports:\n")));
        PORTMOD_PBMP_ITER(*ports, port)
        {
            LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n port %d\n"), port));
        }
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("given num of ports is different from num of speeds: num of ports is %d and num of speeds is %d"), ports_count, vco_setting->num_speeds));
    }

    /*
     * }
     */


    /** get - should we use only PLL0, only PLL1 OR both {PLL0, PLL1} for this Chip */
    begin_pll = PM8X50_FABRIC_PLL_INDEX_DEFAULT_START;
    end_pll = PM8X50_FABRIC_PLL_INDEX_DEFAULT_END;
    if (fabric_data->force_single_pll0 || fabric_data->native_single_pll)
    {
        begin_pll = 0;
        end_pll = 1;
    }
    else if (fabric_data->force_single_pll1)
    {
        begin_pll = 1;
        end_pll = 2;
    }
    nof_allowed_plls = end_pll - begin_pll;
    /*
     * Get array of current PLLs
     * And
     * And array of sanitized PLLs
     * The sanitized PLLs array is created after removing (if possible) the PLLs that we won't require after this assignment from the current PLLs array
     * So after this function pll_vco_rate_curr_updated will hold only PLLs that are in use from other ports not provided to this API
     * Over the freed PLL indexes we can apply our preferences for PLL and if possible they will be taken into account.
     * Example 1:
     * We have
     *  - first  4 lanes of the PM to PLL-A
     *  - second 4 lanes of the PM to PLL-B
     *  We provide to the API:
     *   - first 4 lanes to be with speed that require PLL-C
     *
     *   The below function in this case will return an array of:
     *   {PLL-Invalid, PLL-B}
     *
     * Example 2:
     * We have
     *  - first  4 lanes of the PM to PLL-A
     *  - second 4 lanes of the PM to PLL-B
     *  We provide to the API:
     *   - all 8 lanes to be with speed that require PLL-C
     *
     *   The below function in this case will return an array of:
     *   {PLL-Invalid, PLL-Invalid}
     *
     * Example 3:
     * We have
     *  - first  4 lanes of the PM to PLL-A
     *  - second 4 lanes of the PM to PLL-B
     *  We provide to the API:
     *   - first 3 lanes to be with speed that require PLL-C
     *
     *   The below function in this case will return an Error because 2 PLLs are not enough
 */
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_pm_speed_config_validate_plls_get(unit, pm_info, ports, vco_setting, begin_pll, end_pll, pll_vco_rate_curr, pll_vco_rate_curr_updated));

    /** Calculate the required VCO rates based on the speeds given by the user as input
     * and MAP the VCO to preferred PLL index if possible*/
    for (speed_index = 0; speed_index < vco_setting->num_speeds; ++speed_index)
    {

        other_pll_is_valid = FALSE;
        preferred_pll_is_valid = FALSE;
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_pm_speed_vco_get(unit, vco_setting->speed_config_list[speed_index].speed, vco_setting->speed_for_pcs_bypass_port[speed_index], &required_vco));

        /**
         * If we don't have the required VCO in the current updated PLLs array
         */
        if ((required_vco != pll_vco_rate_curr_updated[0]) && (required_vco != pll_vco_rate_curr_updated[1]))
        {
            if ((PM8X50_FABRIC_SPEED_IS_PAM4(vco_setting->speed_config_list[speed_index].speed)))
            {
                /*
                 * IF speed is PAM4 TRY to assign to PLL0
                 */
                preferred_pll_index = PM8X50_FABRIC_PREFERRED_PAM4_PLL_INDEX;
            }
            else
            {
                /*
                * IF speed is NRZ TRY to assign to PLL0
                */
                preferred_pll_index = PM8X50_FABRIC_PREFERRED_NRZ_PLL_INDEX;
            }
            other_pll_index =  preferred_pll_index ? 0 : 1;

            /**
             * Check if PLLs are valid for assignment
             * Depends on fabric_data->force_single_pll0 / fabric_data->force_single_pll1
             */
            PM8X50_FABRIC_PLLS_ITER(begin_pll, end_pll, pll_index)
            {
                if (pll_index == preferred_pll_index)
                {
                    preferred_pll_is_valid = TRUE;
                }
                if (pll_index == other_pll_index)
                {
                    other_pll_is_valid = TRUE;
                }
            }

            /*
             * IF preferred PLL is NOT EMPTY or NOT VALID assign to the other one
             */
            if (pll_vco_rate_curr_updated[preferred_pll_index] == portmodVCOInvalid && preferred_pll_is_valid)
            {
                pll_vco_rate_curr_updated[preferred_pll_index] = required_vco;
            }
            else
            {
                /**
                 * If preferred PLL is not valid for assignment
                 * AND other PLL is different and still used by other ports return an error
                 */
                if ((pll_vco_rate_curr_updated[other_pll_index] != portmodVCOInvalid &&  pll_vco_rate_curr_updated[other_pll_index] != required_vco) || !other_pll_is_valid)
                {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                            (_BSL_SOCDNX_MSG("Not enough free PLLs to complete the assignment! Number of allowed PLLs for relevant PM is %d"), nof_allowed_plls));
                }
                pll_vco_rate_curr_updated[other_pll_index] = required_vco;
            }

        }
    }

    /** Update function output */
    vco_setting->ovco = pll_vco_rate_curr_updated[0];
    vco_setting->is_ovco_new = (pll_vco_rate_curr_updated[0] == pll_vco_rate_curr[0])? 0 : 1;

    vco_setting->tvco = pll_vco_rate_curr_updated[1];
    vco_setting->is_tvco_new = (pll_vco_rate_curr_updated[1] == pll_vco_rate_curr[1])? 0 : 1;

exit:
    SOCDNX_FUNC_RETURN;
}

/* Reconfigure the vco rate for pm core.
 * The function receives TVCO rate in vco[0] and OVCO rate in vco[1].
 * If any of the VCO rates are different from existing, it updates VCO rates in WB
 * and also zeroes bitmap of each such VCO.
 * No hardware changes, only portmod WB updates.
 * size of vco array should be 2 (nof plls) */
int pm8x50_fabric_pm_vco_reconfig(int unit, int pm_id, pm_info_t pm_info, const portmod_vco_type_t* vco)
{
    int pll_index, end_pll, begin_pll;
    portmod_vco_type_t pll_vco_rate[PM8X50_FABRIC_MAX_NUM_PLLS];
    uint32 pll_active_lanes_bitmap[PM8X50_FABRIC_MAX_NUM_PLLS];
    portmod_vco_type_t vco_aligned[PM8X50_FABRIC_MAX_NUM_PLLS];
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;

    SOCDNX_INIT_FUNC_DEFS;

    PM8X50_FABRIC_PLLS_INIT(pll_vco_rate, pll_active_lanes_bitmap);

    begin_pll = 0;
    end_pll = 2;
    if (fabric_data->force_single_pll0 || fabric_data->native_single_pll)
    {
        begin_pll = 0;
        end_pll = 1;
    }
    else if (fabric_data->force_single_pll1)
    {
        begin_pll = 1;
        end_pll = 2;
    }

    PM8X50_FABRIC_PLLS_ITER(begin_pll, end_pll, pll_index)
    {
        SOCDNX_IF_ERR_EXIT(PM8X50_FABRIC_PLL_VCO_RATE_GET(unit, pm_info, &pll_vco_rate[pll_index], pll_index));
        SOCDNX_IF_ERR_EXIT(PM8X50_FABRIC_PLL_ACTIVE_LANES_BITMAP_GET(unit, pm_info, &pll_active_lanes_bitmap[pll_index], pll_index));
    }

    /* Current VCOs as stored in WB in the PM: pll_vco_rate[0] == OVCO, pll_vco_rate[1] == TVCO.
     * Requested VCOs as received as input in "vco" parameter: vco[0] == TVCO, vco[1] == OVCO.
     * swap input OVCO and TVCO in "vco" param in order to compare current and requested VCOs */
    vco_aligned[0] = vco[1];
    vco_aligned[1] = vco[0];

    PM8X50_FABRIC_PLLS_ITER(begin_pll, end_pll, pll_index)
    {
        /* if requested VCO is different from current VCO */
        if (vco_aligned[pll_index] != pll_vco_rate[pll_index])
        {
            pll_vco_rate[pll_index] = vco_aligned[pll_index];
            SOCDNX_IF_ERR_EXIT(PM8X50_FABRIC_PLL_VCO_RATE_SET(unit, pm_info, pll_vco_rate[pll_index], pll_index));

            pll_active_lanes_bitmap[pll_index] = 0;
            SOCDNX_IF_ERR_EXIT(PM8X50_FABRIC_PLL_ACTIVE_LANES_BITMAP_SET(unit, pm_info, pll_active_lanes_bitmap[pll_index], pll_index));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* Set the speed config for the specified port, includes:
 * 1) encoding, FEC
 * 2) speed
 * 3) link_training, CL72
 * 4) lane_config - mostly firmware-related parameters
 * Configuration 1) is done in the PM and all other configurations via a single phymod API.
 * Max. 2 PLLs are supported per Blackhawk core, meaning 2 VCO rates.
 * Each active port is assigned to a PLL, according to its speed.
 */
int pm8x50_fabric_port_speed_config_set(int unit, int port, pm_info_t pm_info, const portmod_speed_config_t* speed_config)
{
    int rv;
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    uint32 encoding_properties = 0;
    portmod_port_pcs_t legacy_encoding;
    uint32 phy_index, bitmap;
    portmod_vco_type_t required_vco = portmodVCOInvalid;
    portmod_pmd_lane_config_t portmod_pmd_lane_config;
    phymod_phy_speed_config_t phymod_speed_config;
    uint32 pll_active_lanes_bitmap[PM8X50_FABRIC_MAX_NUM_PLLS];
    portmod_vco_type_t pll_vco_rate[PM8X50_FABRIC_MAX_NUM_PLLS];
    int lane_index, pll_index, end_pll, begin_pll;
    phymod_phy_pll_state_t old_pll_state, new_pll_state;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys;
    int is_bypass;

    SOCDNX_INIT_FUNC_DEFS;

    /** 1. fill phymod_speed_config struct from received portmod_speed_config struct */
    SOCDNX_IF_ERR_EXIT(phymod_phy_speed_config_t_init(&phymod_speed_config));
    phymod_speed_config.data_rate = speed_config->speed;
    phymod_speed_config.linkTraining = speed_config->link_training;

    SOCDNX_IF_ERR_EXIT(portmod_pmd_lane_config_t_init(unit, &portmod_pmd_lane_config));
    SOCDNX_IF_ERR_EXIT(portmod_common_pmd_lane_config_decode(speed_config->lane_config, &portmod_pmd_lane_config));
    phymod_speed_config.pmd_lane_config = portmod_pmd_lane_config.pmd_firmware_lane_config;
    phymod_speed_config.PAM4_channel_loss = portmod_pmd_lane_config.pam4_channel_loss;

    if (fabric_data->force_single_pll0)
    {
        PHYMOD_SPEED_CONFIG_ONLY_PLL0_IS_ACTIVE_SET(&phymod_speed_config);
    }

    if (fabric_data->ref_clk == phymodRefClk156Mhz)
    {
        PHYMOD_SPEED_CONFIG_REF_CLK_IS_156P25MHZ_SET(&phymod_speed_config);
    }

    /** 2. PLLs configuration - will be changed based on the received speed_config->speed */
    begin_pll = 0;
    end_pll = 2;
    if (fabric_data->force_single_pll0 || fabric_data->native_single_pll)
    {
        begin_pll = 0;
        end_pll = 1;
    }
    else if (fabric_data->force_single_pll1)
    {
        begin_pll = 1;
        end_pll = 2;
    }

    PM8X50_FABRIC_PLLS_INIT(pll_vco_rate, pll_active_lanes_bitmap);
    PM8X50_FABRIC_PLLS_ITER(begin_pll, end_pll, pll_index)
    {
        SOCDNX_IF_ERR_EXIT(PM8X50_FABRIC_PLL_VCO_RATE_GET(unit, pm_info, &pll_vco_rate[pll_index], pll_index));
        SOCDNX_IF_ERR_EXIT(PM8X50_FABRIC_PLL_ACTIVE_LANES_BITMAP_GET(unit, pm_info, &pll_active_lanes_bitmap[pll_index], pll_index));
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    rv = PM8X50_FABRIC_LANE_IS_BYPASSED_GET(unit, pm_info, &is_bypass, phy_index);
    SOCDNX_IF_ERR_EXIT(rv);

    /* to change the VCO rate of a port that is currently assigned to a PLL - remove it from the PLL */
    if (is_bypass)
    {
        pm8x50_shared_pcs_bypassed_vco_get(speed_config->speed, &required_vco);
        if (required_vco == portmodVCOInvalid)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("speed %d not supported"), speed_config->speed));
        }
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_speed_config_fabric_speed_to_vco_get(unit, speed_config->speed, &required_vco));
    }
    PM8X50_FABRIC_PLLS_ITER(begin_pll, end_pll, pll_index)
    {
        if (SHR_BITGET(&pll_active_lanes_bitmap[pll_index], phy_index) && (required_vco != pll_vco_rate[pll_index]))
        {
            PM8X50_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index)
            {
                SHR_BITCLR(&pll_active_lanes_bitmap[pll_index], lane_index);
            }
            break;
        }
    }

    /* init old pll state - the bitmaps of PLL1, PLL0 without the current port. will be passed to phymod */
    SOCDNX_IF_ERR_EXIT(phymod_phy_pll_state_t_init(&old_pll_state));
    old_pll_state.pll0_lanes_bitmap = pll_active_lanes_bitmap[0];
    old_pll_state.pll1_lanes_bitmap = pll_active_lanes_bitmap[1];

    /* new PLL state will be returned from phymod API, holding the updated state of both PLLs */
    SOCDNX_IF_ERR_EXIT(phymod_phy_pll_state_t_init(&new_pll_state));

    /* update portmod lanes bitmap of relevant PLL */
    PM8X50_FABRIC_PLLS_ITER(begin_pll, end_pll, pll_index)
    {
        if (pll_vco_rate[pll_index] == required_vco)
        {
            PM8X50_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index)
            {
                SHR_BITSET(&pll_active_lanes_bitmap[pll_index], lane_index);
            }
            break;
        }
    }


    /* if no PLL is configured to the desired rate */
    if (pll_index < begin_pll)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("port %d: the required VCO rate %d was not configured in pm8x50_fabric_pm_vco_reconfig"), port, required_vco));
    }

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));

    /*
     * IF SPEED is NRZ AND
     *  PLL0 is same as required VCO
     *  tell the phymod code to prefer PLL0 also
     */
    if (!PM8X50_FABRIC_SPEED_IS_PAM4(speed_config->speed))
    {
        /**
         * The blackhawk driver will always try to allocate PLL1 first and than PLL0.
         * The allocation of PLLs is already done and the preferences of PLLs should be set at this point.
         * Here we only force the Phymod to also go along with the preferences
         *
         * NOTE! We reuse PHYMOD_SPEED_CONFIG_ONLY_PLL0_IS_ACTIVE_SET only in the meaning of
         * prefer PLL0 in this case
 */
        if(pll_vco_rate[PM8X50_FABRIC_PREFERRED_NRZ_PLL_INDEX] == required_vco)
        {
            PHYMOD_SPEED_CONFIG_ONLY_PLL0_IS_ACTIVE_SET(&phymod_speed_config);
        }
    }
    /** 3. call the phymod function which configures all the above configurations */

    /* in easy reload the purpose is only to update the SW and leave HW as is.
     * skip phymod_speed_config function in easy reload because:
     * - it is not needed because it doesn't configure any SW (phymod is stateless).
     * - it gives an error about core not being in reset state. The core shouldn't be reset in easy reload. */
    if (!SOC_IS_RELOADING(unit))
    {
        SOCDNX_IF_ERR_EXIT(phymod_phy_speed_config_set(&phy_access, &phymod_speed_config, &old_pll_state, &new_pll_state));
    }

    /** 4. check that we got the expected bitmaps of both PLLs from phymod - then can update SW state */

    /* in easy reload mode there is no need to validate that phymod returned correct bitmaps, because if it didn't - we would get an error before the easy reload step.
     * in this case we will update the WB engine with the bitmaps as they were calculated here */
    if (((new_pll_state.pll0_lanes_bitmap == pll_active_lanes_bitmap[0]) && (new_pll_state.pll1_lanes_bitmap == pll_active_lanes_bitmap[1])) || SOC_IS_RELOADING(unit))
    {
        SOCDNX_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_pll_active_lanes_bitmap], &pll_active_lanes_bitmap[0], 0));
        SOCDNX_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_pll_active_lanes_bitmap], &pll_active_lanes_bitmap[1], 1));
        PM8X50_FABRIC_PLLS_ITER(begin_pll, end_pll, pll_index)
        {
            SOCDNX_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_pll_vco_rate], &pll_vco_rate[pll_index], pll_index));
        }
    }
    else
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("port %d: didn't get expected PLL bitmaps from phymod!"), port));
    }

    if (!fabric_data->native_single_pll)
    {
        /** 5. power down unused PLLs if more than one */
        PM8X50_FABRIC_PLLS_ITER(0, PM8X50_FABRIC_MAX_NUM_PLLS, pll_index)
        {
            SOCDNX_IF_ERR_EXIT(PM8X50_FABRIC_PLL_ACTIVE_LANES_BITMAP_GET(unit, pm_info, &pll_active_lanes_bitmap[pll_index], pll_index));
            if (PM8X50_FABRIC_PLL_IS_EMPTY(pll_active_lanes_bitmap[pll_index]))
            {
                SOCDNX_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, pll_index, 1));
            }
        }
    }
    
    /** 6. configure the FEC- unrelated to phymod_phy_speed_config - done in the PM.
     * RS-FEC configuration depends on the speed, so this step is done after speed configuration.
     * FEC configuration is irrelevant for bypass port, since ILKN has its own FEC */
    
    if (!PORTMOD_SPEED_CONFIG_F_SKIP_FEC_CONFIGURATION_GET(speed_config) && !is_bypass)
    {
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_speed_config_new_to_legacy_encoding_get(unit, port, pm_info, speed_config->fec, &legacy_encoding));

        /* set default fec-related configurations that go with the fec in speed_config->fec */
        if (legacy_encoding == PORTMOD_PCS_64B66B_FEC)
        {
            PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_SET(encoding_properties);
        } else if (PM8X50_FABRIC_ENCODING_IS_RS_FEC(legacy_encoding))
        {
            PORTMOD_ENCODING_LOW_LATENCY_LLFC_SET(encoding_properties);

            if (PM8X50_FABRIC_SPEED_IS_PAM4(speed_config->speed))
            {
                PORTMOD_ENCODING_LLFC_AFTER_FEC_SET(encoding_properties);
            }
            else
            {
                if (fabric_data->is_rx_ctrl_bypass_supported)
                {
                    PORTMOD_ENCODING_CONTROL_CELLS_FEC_BYPASS_SET(encoding_properties);
                }
            }
        }

        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_encoding_set(unit, port, pm_info, encoding_properties, legacy_encoding));
    }


exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_speed_config_get(int unit, int port, pm_info_t pm_info, portmod_speed_config_t* speed_config)
{
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    portmod_port_pcs_t legacy_encoding;
    uint32 properties = 0;
    phymod_phy_speed_config_t phymod_speed_config;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys;
    portmod_pmd_lane_config_t portmod_lane_config;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_encoding_get(unit, port, pm_info, &properties, &legacy_encoding));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_speed_config_legacy_to_new_encoding_get(unit, port, pm_info, legacy_encoding, &speed_config->fec));

    SOCDNX_IF_ERR_EXIT(phymod_phy_speed_config_t_init(&phymod_speed_config));

    if (fabric_data->ref_clk == phymodRefClk156Mhz)
    {
        PHYMOD_SPEED_CONFIG_REF_CLK_IS_156P25MHZ_SET(&phymod_speed_config);
    }

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));

    SOCDNX_IF_ERR_EXIT(phymod_phy_speed_config_get(&phy_access, &phymod_speed_config));

    speed_config->speed = phymod_speed_config.data_rate;
    speed_config->link_training = phymod_speed_config.linkTraining;
    speed_config->num_lane = 1;

    portmod_lane_config.pmd_firmware_lane_config = phymod_speed_config.pmd_lane_config;
    portmod_lane_config.pam4_channel_loss = phymod_speed_config.PAM4_channel_loss;
    SOCDNX_IF_ERR_EXIT(portmod_common_pmd_lane_config_encode(&portmod_lane_config, (uint32*)(&speed_config->lane_config)));

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_lane_map_set(int unit, int port, pm_info_t pm_info, uint32 flags, const phymod_lane_map_t* lane_map)
{
    uint32 mac_only;
    SOCDNX_INIT_FUNC_DEFS;

    mac_only = PORTMOD_LANE_MAP_MAC_ONLY_GET(flags);
    if (mac_only == 0)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("port %d: pm8x50_fabric supports MAC only lane map set"), port));
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_pm_fmac_tx_lane_mapping_set(unit, port, pm_info, lane_map));

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_lane_map_get(int unit, int port, pm_info_t pm_info, uint32 flags,phymod_lane_map_t* lane_map)
{
    uint32 mac_only;
    int lane_index;
    SOCDNX_INIT_FUNC_DEFS;

    mac_only = PORTMOD_LANE_MAP_MAC_ONLY_GET(flags);
    if (mac_only == 0)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("port %d: pm8x50_fabric supports MAC only lane map set"), port));
    }

    lane_map->num_of_lanes = PM8X50_FABRIC_LANES_PER_CORE;

    /* RX - direct mapping */
    for (lane_index = 0 ; lane_index < PM8X50_FABRIC_LANES_PER_CORE ; lane_index++)
    {
        lane_map->lane_map_rx[lane_index] = lane_index;
    }

    /* TX - FMAC configuration */
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_pm_fmac_tx_lane_mapping_get(unit, port, pm_info, lane_map));

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_synce_clk_ctrl_set(int unit, int port, pm_info_t pm_info,
                                   const portmod_port_synce_clk_ctrl_t* cfg)
{
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    int fsrd_schan_id;
    uint32 reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    fsrd_schan_id = fabric_data->fsrd_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;

    SOCDNX_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, fsrd_schan_id, &reg_val));
    soc_reg_field_set(unit, FSRD_SYNC_E_SELECTr, &reg_val, SYNC_E_CLK_DIVf, cfg->sdm_val);
    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SYNC_E_SELECTr(unit, fsrd_schan_id, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_synce_clk_ctrl_get(int unit, int port, pm_info_t pm_info,
                                   portmod_port_synce_clk_ctrl_t* cfg)
{
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    int fsrd_schan_id;
    uint32 reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    fsrd_schan_id = fabric_data->fsrd_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;

    SOCDNX_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, fsrd_schan_id, &reg_val));
    cfg->sdm_val = soc_reg_field_get(unit, FSRD_SYNC_E_SELECTr, reg_val, SYNC_E_CLK_DIVf);

exit:
    SOCDNX_FUNC_RETURN;
}

/* enable credits for ILKN over fabric port - credits from SerDes Tx to ILKN core */
int pm8x50_fabric_port_credits_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    int fsrd_schan_id;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    fsrd_schan_id = fabric_data->fsrd_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;

    SOCDNX_IF_ERR_EXIT(READ_FSRD_TX_ILKN_CREDITr(unit, fsrd_schan_id, &reg_val));
    soc_reg_field_set(unit, FSRD_TX_ILKN_CREDITr, &reg_val, TX_ILKN_CREDIT_ENf, enable? 1 : 0);
    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_TX_ILKN_CREDITr(unit, fsrd_schan_id, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_core_add(int unit, int pm_id, pm_info_t pm_info, int flags, const portmod_port_add_info_t* add_info)
{

    SOCDNX_INIT_FUNC_DEFS;

    /*
     * Do nothing
     */

    SOCDNX_FUNC_RETURN;
}

#endif /* PORTMOD_PM8X50_FABRIC_SUPPORT */

#undef _ERR_MSG_MODULE_NAME

