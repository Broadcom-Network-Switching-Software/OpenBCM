/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *     
 */
#include <shared/bsl.h>
#include <shared/error.h>
#include <soc/types.h>
#include <soc/error.h>
#include <sal/core/alloc.h>
#include <soc/wb_engine.h>
#include <soc/drv.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_chain.h>

#ifdef PORTMOD_PM8X100_FABRIC_SUPPORT
/*Access*/
#include <soc/access/access.h>
#include <soc/access/auto_generated/common_enum.h>
#include <soc/access/sbus.h>

#include <soc/portmod/pm8x100_fabric.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#define PM8X100_FABRIC_NEW_ACCESS_DEVICE_TYPE_INFO(unit) (ACCESS_RUNTIME_INFO(unit)->device_type_info)

#define PM8X100_FABRIC_LANES_PER_CORE (8)
#define PM8X100_FABRIC_MAX_PORTS_PER_PM (8)
#define PM8X100_FABRIC_LANES_PER_FMAC (4)

#define PMD_INFO_DATA_STRUCTURE_SIZE    (128)     /* in bytes */

/* WB defines */
#define PM8X100_FABRIC_LANE2PORT_SET(unit, pm_info, lane, port) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane2port_map], &port, lane)
#define PM8X100_FABRIC_LANE2PORT_GET(unit, pm_info, lane, port) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane2port_map], port, lane)

#define PM8X100_FABRIC_PLL_ACTIVE_LANES_BITMAP_SET(unit, pm_info, bitmap, pll_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_pll_active_lanes_bitmap], &bitmap, pll_index)

#define PM8X100_FABRIC_PLL_ACTIVE_LANES_BITMAP_GET(unit, pm_info, bitmap, pll_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_pll_active_lanes_bitmap], bitmap, pll_index)

#define PM8X100_FABRIC_PLL_VCO_RATE_SET(unit, pm_info, vco_rate, pll_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_pll_vco_rate], &vco_rate, pll_index)

#define PM8X100_FABRIC_PLL_VCO_RATE_GET(unit, pm_info, vco_rate, pll_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_pll_vco_rate], vco_rate, pll_index)

#define PM8X100_FABRIC_INVALID_PORT     (-1)

/*MAC PRBS Defines*/
#define PM8X100_FABRIC_MAC_PRBS_LOCK_SHIFT  (31)
#define PM8X100_FABRIC_MAC_PRBS_LOCK_MASK   (0x1)
#define PM8X100_FABRIC_MAC_PRBS_CTR_SHIFT   (0)
#define PM8X100_FABRIC_MAC_PRBS_CTR_MASK    (0x7FFFFFFF)

#define PM8X100_FABRIC_FMAC_TX (0)
#define PM8X100_FABRIC_FMAC_RX (1)

#define PM8X100_FABRIC_FMAC_0_LANES_MASK (0x0F)
#define PM8X100_FABRIC_FMAC_1_LANES_MASK (0xF0)

#define PM8X100_FABRIC_LANE_INDEX_IN_PM_GET(lane) \
    ((lane) % PM8X100_FABRIC_LANES_PER_CORE)

#define PM8X100_FABRIC_LANE_INDEX_IN_FMAC_GET(lane_index_in_pm) \
    ((lane_index_in_pm) % PM8X100_FABRIC_LANES_PER_FMAC)

#define PM8X100_FABRIC_LANE_FMAC_INDEX_GET(lane_index_in_pm) \
    ((lane_index_in_pm) / PM8X100_FABRIC_LANES_PER_FMAC)

/*
 * Input: bitmap
 * Output: lane_index
 */
#define PM8X100_FABRIC_PORT_BITMAP_ITER(bitmap, lane_index) \
    for ((lane_index) = 0; (lane_index) < PM8X100_FABRIC_LANES_PER_CORE; ++(lane_index)) \
    if ((bitmap) & (1 << (lane_index)))

/*
 * Input: pm_info, lane_index_in_pm
 * Output: fmac_blk_id, lane_index_in_fmac
 */
#define PM8X100_FABRIC_LANE_FMAC_BLK_ID_GET(pm_info, lane_index_in_pm, fmac_blk_id) \
    fmac_blk_id = (pm_info->pm_data.pm8x100_fabric->fmac_schan_id[PM8X100_FABRIC_LANE_FMAC_INDEX_GET(lane_index_in_pm)]);

#define PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, lane_index_in_pm, fmac_blk_id, lane_index_in_fmac) \
    PM8X100_FABRIC_LANE_FMAC_BLK_ID_GET(pm_info, lane_index_in_pm, fmac_blk_id) \
    lane_index_in_fmac = PM8X100_FABRIC_LANE_INDEX_IN_FMAC_GET(lane_index_in_pm)

/* get serdes index in pm (range 0-7) and convert it to fmac_blk_id (out of the 2 possible fmac_blk_ids) and serdes_index_in_fmac (range 0-3) */
#define PM8X100_FABRIC_SERDES_FMAC_DATA_GET(pm_info, serdes_index_in_pm, fmac_blk_id, serdes_index_in_fmac) \
    PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, serdes_index_in_pm, fmac_blk_id, serdes_index_in_fmac)

/*
 * Input: fmac_blk_ids, nof_fmac_blks, first_fmac_index
 * Output: fmac_index
 */
#define PM8X100_FABRIC_PORT_FMACS_ITER(fmac_blk_ids, nof_fmac_blks, first_fmac_index, fmac_index) \
    for ((fmac_index) = (first_fmac_index); (fmac_index) < (first_fmac_index + nof_fmac_blks); ++(fmac_index))

#define PM8X100_FABRIC_ENCODING_IS_RS_FEC(encoding) \
        (encoding == PORTMOD_PCS_256B257B_15T_1xN_RS_FEC || encoding == PORTMOD_PCS_256B257B_15T_2xN_RS_FEC \
        || encoding == PORTMOD_PCS_64B66B_15T_RS_FEC || encoding == PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC)

#define PM8X100_FABRIC_ENCODING_IS_PROPRIETARY_RS_FEC(encoding) \
        (encoding == PORTMOD_PCS_64B66B_15T_RS_FEC || encoding == PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC)

#define PM8X100_FABRIC_ENCODING_IS_STANDARD_RS_FEC(encoding) \
        (encoding == PORTMOD_PCS_256B257B_15T_1xN_RS_FEC || encoding == PORTMOD_PCS_256B257B_15T_2xN_RS_FEC)

#define PM8X100_FABRIC_ENCODING_IS_LOW_LATECY_RS_FEC(encoding) \
        (encoding == PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC)

/* Async FIFO configurations that depend on FEC - regular mode */
#define PM8X100_FABRIC_FEC_NONE_SLOW_READ_RATE              (1)
#define PM8X100_FABRIC_FEC_NONE_FMAL_TX_WIDTH               (2)
#define PM8X100_FABRIC_FEC_NONE_50G_FMAL_RX_WIDTH           (2)
#define PM8X100_FABRIC_FEC_NONE_100G_FMAL_RX_WIDTH          (3)
#define PM8X100_FABRIC_FEC_RS_64B66B_15T_FMAL_RX_WIDTH      (0)
#define PM8X100_FABRIC_FEC_RS_256B257B_15T_FMAL_RX_WIDTH    (1)
#define PM8X100_FABRIC_FEC_NONE_ENCODING_TYPE               (0)
#define PM8X100_FABRIC_FEC_RS_ENCODING_TYPE                 (1)
#define PM8X100_FABRIC_FEC_BASE_R_SLOW_READ_RATE            (3)
#define PM8X100_FABRIC_FEC_BASE_R_FMAL_WIDTH                (2)
#define PM8X100_FABRIC_FEC_RS_SLOW_READ_RATE                (4)

#define PM8X100_FABRIC_FEC_RS_64B66B_15T_FRAME_TYPE             (0)
#define PM8X100_FABRIC_FEC_RS_64B66B_15T_LOW_LATENCY_FRAME_TYPE (1)
#define PM8X100_FABRIC_FEC_RS_256B257B_15T_1xN_FRAME_TYPE       (2)
#define PM8X100_FABRIC_FEC_RS_256B257B_15T_2xN_FRAME_TYPE       (3)
 
#define PM8X100_FABRIC_DEFAULT_PLL (0)
#define PM8X100_FABRIC_MAX_NUM_PLLS (1)

/*wb vars in pm8x100_fabric buffer.
  new value must appended to the end.
  Remove values is disallowed*/
typedef enum pm8x100_fabric_wb_vars {
    wb_is_probed = 0,
    wb_is_initialized = 1,
    wb_ports = 2,
    wb_polarity_rx = 3,
    wb_polarity_tx = 4,
    wb_lane2port_map = 5,
    wb_force_single_pll = 6,
    wb_pll_active_lanes_bitmap = 7,
    wb_pll_vco_rate = 8,
} pm8x100_fabric_wb_vars_t;

typedef struct pm8x100_fabric_internal_s{
    pm_info_t *pms;
} pm8x100_fabric_internal_t;

struct pm8x100_fabric_s {
    portmod_pbmp_t phys;
    phymod_ref_clk_t ref_clk;
    phymod_access_t access;
    phymod_dispatch_type_t core_type;
    phymod_firmware_load_method_t fw_load_method;
    phymod_firmware_loader_f fw_loader;
    uint32 fmac_schan_id[PORTMOD_NOF_FMACS_PER_PM8X100_FABRIC];
    uint32 fsrd_schan_id;
    int first_phy_offset;
    int core_index;
};

typedef struct pm8x100_fabric_fec_sync_machine_config_s{
    int n1;
    int n2;
    int n3;
    int k1;
    int k2;
    int k3;
} pm8x100_fabric_fec_sync_machine_config_t;

typedef struct pm8x100_fabric_rs_fec_sync_machine_config_s{
    int am_period;
    int comapre_cnt;
    int nibble_cnt;
} pm8x100_fabric_rs_fec_sync_machine_config_t;

static phymod_dispatch_type_t pm8x100_fabric_serdes_list[] =
{
    phymodDispatchTypeInvalid
};

/******************************************************************************
 * Default acces - LPI 
*******************************************************************************/
STATIC int
lpi_fabric_bus_write(
    void *user_acc,
    uint32_t core_addr,
    uint32_t reg_addr,
    uint32_t val)
{
    /*
     * will force the core_addr to zero since it's NOT being used 
     */
    return portmod_common_phy_sbus_reg_write(FSRD_FSRD_PMI_LPm, user_acc, 0x0, reg_addr, val);
}

STATIC int
lpi_fabric_bus_read(
    void *user_acc,
    uint32_t core_addr,
    uint32_t reg_addr,
    uint32_t * val)
{
    int rv = SOC_E_NONE;

    /*
     * will force the core_addr to zero since it's NOT being used 
     */
    rv = portmod_common_phy_sbus_reg_read(FSRD_FSRD_PMI_LPm, user_acc, 0x0, reg_addr, val);
    /*
     * The returned 32bit data value from FSRD_FSRD_PMI_LPm contains 16bits of the actual data
     * and 16bits from the data mask which are irrelevant.
     * We down shift with 16bits in order to have only the actual data as result.
     */
    *val = (*val >> 16) & 0xffff;
    return (rv);
}

phymod_bus_t lpi_fabric_bus = {
    "LPI Fabric Bus",
    lpi_fabric_bus_read,
    lpi_fabric_bus_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    NULL,
    NULL,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

STATIC
int pm8x100_fabric_squelch_set(int unit, int port, pm_info_t pm_info, int flags, int squelch);

/* 1. get bitmap of lanes used for the port - the bitmap is of size 8. First 4 bits: for FMAC0, second 4 bits: for FMAC1.
 * 2. get phy_index of first phy in each FMAC used for the port - [0-7] 
 * This procedure returns bitmap as it was inherited from PM8x50 which supported ILKNoFabric and we had the possibility to have several lanes to a port
 * Now we keep it for legacy reasons as it is clearer, easy to track and big portion of code is bitmap oriented
 */
STATIC
int pm8x100_fabric_port_phy_index_get(int unit, int port, pm_info_t pm_info, uint32 *phy_index, uint32 *bitmap){
    int i, rv = 0, tmp_port = 0;
    SOC_INIT_FUNC_DEFS;

    *bitmap = 0;
    *phy_index = -1;

    /*set bitmap - all the lanes used for the port, first 4 bits: for FMAC0, second 4 bits: for FMAC1.*/
    for (i = 0; i < PM8X100_FABRIC_MAX_PORTS_PER_PM; i++) {
        rv = PM8X100_FABRIC_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        _SOC_IF_ERR_EXIT(rv);
        if(tmp_port == port) {
            *phy_index = (*phy_index == -1 ? i : *phy_index);
            SHR_BITSET(bitmap, i);
        }
    }

    if(*phy_index == -1) {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                ("port was not found in internal DB %d", port));
    }

exit:
    SOC_FUNC_RETURN;
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
int pm8x100_fabric_port_fmac_lanes_bitwrite(int unit, int port, uint32 bitmap, int fmac_index, uint32* val, int to_set)
{
    uint32 fmac_active_lanes_bitmap;
    uint32 fmac_lane_mask[] = {PM8X100_FABRIC_FMAC_0_LANES_MASK, PM8X100_FABRIC_FMAC_1_LANES_MASK};
    SOC_INIT_FUNC_DEFS;

    fmac_active_lanes_bitmap = bitmap & fmac_lane_mask[fmac_index];
    fmac_active_lanes_bitmap >>= fmac_index * PM8X100_FABRIC_LANES_PER_FMAC;
    if (to_set) {
        *val |= fmac_active_lanes_bitmap;
    } else {
        uint32 fmac_non_active_lanes_bitmap = ~fmac_active_lanes_bitmap;
        *val &= fmac_non_active_lanes_bitmap;
    }

    SOC_FUNC_RETURN;
}

/* Get FMAC blk id/s and their amount.
 * fmac_blk_ids is an array of size 2.
 *      fmac_blk_ids[0] -> 0 if FMAC0 of the PM is not used for the port / the blk id of FMAC0 in case it is used for the port.
 *      fmac_blk_ids[1] -> 0 if FMAC1 of the PM is not used for the port / the blk id of FMAC1 in case it is used for the port.
 * nof_fmac_blks - count of FMACs used for the port - 1/2.
 * first_fmac_index - the first FMAC of the port - 0/1.
 */
STATIC
int pm8x100_fabric_fmac_blk_ids_get(int unit, int port, pm_info_t pm_info, uint32* fmac_blk_ids, int* nof_fmac_blks, int* first_fmac_index)
{
    uint32 phy_index, bitmap[1];
    int fmac_index;
    uint32 fmac_lane_mask[] = {PM8X100_FABRIC_FMAC_0_LANES_MASK, PM8X100_FABRIC_FMAC_1_LANES_MASK};
    pm8x100_fabric_t fabric_data = pm_info->pm_data.pm8x100_fabric;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));

    *nof_fmac_blks = 0;
    *first_fmac_index = -1;

    for (fmac_index = 0; fmac_index < PORTMOD_NOF_FMACS_PER_PM8X100_FABRIC; ++fmac_index)
    {
        fmac_blk_ids[fmac_index] = 0;
        if (bitmap[0] & fmac_lane_mask[fmac_index])
        {
            fmac_blk_ids[fmac_index] = fabric_data->fmac_schan_id[fmac_index];
            *first_fmac_index = (*first_fmac_index == -1)? fmac_index : (*first_fmac_index);
            ++(*nof_fmac_blks);
        }
    }

    if(*nof_fmac_blks == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                ("invalid number of FMACs for port %d", port));
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Used by portmod common APIs
 */
int pm8x100_fabric_port_duplex_set(int unit, int port, pm_info_t pm_info,int enable)
{
    return SOC_E_NONE;
}

/*
 * Used by portmod common APIs
 */
int pm8x100_fabric_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    *enable = TRUE; /* Full Duplex */
    return SOC_E_NONE;
}

/*
 * Used by portmod common APIs
 */
int pm8x100_fabric_pm_interface_type_is_supported(int unit, soc_port_if_t interface, int* is_supported){
    *is_supported = (interface == SOC_PORT_IF_SFI);
    return SOC_E_NONE;
}

STATIC
int pm8x100_fabric_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv;
    int buffer_id;
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    buffer_id = wb_buffer_index;
    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pm8x100_fabric", NULL, NULL, VERSION(1), 1, SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_probed", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_is_probed] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_initialized", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_is_initialized] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ports", wb_buffer_index, sizeof(int), NULL, PM8X100_FABRIC_MAX_PORTS_PER_PM, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_ports] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "polarity_rx", wb_buffer_index, sizeof(uint32), NULL, PM8X100_FABRIC_MAX_PORTS_PER_PM, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_polarity_rx] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "polarity_tx", wb_buffer_index, sizeof(uint32), NULL, PM8X100_FABRIC_MAX_PORTS_PER_PM, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_polarity_tx] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane2port_map", wb_buffer_index, sizeof(int), NULL, PM8X100_FABRIC_MAX_PORTS_PER_PM, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_lane2port_map] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "force_single_pll", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_force_single_pll] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll_active_lanes_bitmap", wb_buffer_index, sizeof(uint32), NULL, PM8X100_FABRIC_MAX_NUM_PLLS, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_pll_active_lanes_bitmap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll_vco_rate", wb_buffer_index, sizeof(portmod_vco_type_t), NULL, PM8X100_FABRIC_MAX_NUM_PLLS, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_pll_vco_rate] = wb_var_id;

    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD, wb_buffer_index, FALSE));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_fabric_pm_phys_get(int unit, pm_info_t pm_info, portmod_pbmp_t* phys)
{
    pm8x100_fabric_t fabric_data = pm_info->pm_data.pm8x100_fabric;
    SOC_INIT_FUNC_DEFS;

    SOC_PBMP_ASSIGN(*phys, fabric_data->phys);
    SOC_FUNC_RETURN;
}

int pm8x100_fabric_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
    const portmod_pm8x100_fabric_create_info_internal_t *info = &pm_add_info->pm_specific_info.pm8x100_fabric;
    pm8x100_fabric_t fabric_data;
    int i, invalid_port = -1;
    int is_phymod_probed=0;
    uint32 rx_polarity, tx_polarity, is_core_probed;
    uint32 pll_active_lanes_bitmap = 0; /* no lanes on the PLL */
    portmod_vco_type_t pll_vco_rate = portmodVCOInvalid;
    phymod_core_access_t core_access;
    SOC_INIT_FUNC_DEFS;

    fabric_data = NULL;
    fabric_data = sal_alloc(sizeof(*(pm_info->pm_data.pm8x100_fabric)), "pm8x100_fabric specific_db");
    SOC_NULL_CHECK(fabric_data);
    sal_memset(fabric_data, 0, sizeof(*(pm_info->pm_data.pm8x100_fabric)));

    pm_info->wb_buffer_id = wb_buffer_index;
    pm_info->pm_data.pm8x100_fabric = fabric_data;

    pm_info->type = portmodDispatchTypePm8x100_fabric;

    fabric_data->core_type = phymodDispatchTypeInvalid;

    if (info->ref_clk != phymodRefClk312Mhz)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("illegal ref clock"));
    }

    _SOC_IF_ERR_EXIT(phymod_firmware_load_method_t_validate(pm_add_info->pm_specific_info.pm8x100_fabric.fw_load_method));

    /* Configure access */
    sal_memcpy(&(fabric_data->access), &info->access, sizeof(phymod_access_t));
    if (PHYMOD_ACC_BUS(&fabric_data->access) == NULL)
    {
        PHYMOD_ACC_BUS(&fabric_data->access) = &lpi_fabric_bus;
        /*
         * On cmodel the phy simulator needs to know the core address in order to 
         * allocate sim instace for every serdes/phy.
         * When not on cmodel, addres or also known as core_address will be always set to 0.
         * The reason for this is that this address is ignored when writing to LPI memory,
         * since in order to targed the correct serdes we use the FSRD block ID.
         * This address will be relevant only for MDIO.
         */
        PHYMOD_ACC_ADDR(&fabric_data->access) = 0;
    }

    fabric_data->first_phy_offset = pm_add_info->pm_specific_info.pm8x100_fabric.first_phy_offset;
    fabric_data->core_index = pm_add_info->pm_specific_info.pm8x100_fabric.core_index;
    for (i = 0; i < PORTMOD_NOF_FMACS_PER_PM8X100_FABRIC; ++i) {
        fabric_data->fmac_schan_id[i] = pm_add_info->pm_specific_info.pm8x100_fabric.fmac_schan_id[i];
    }
    fabric_data->fsrd_schan_id = pm_add_info->pm_specific_info.pm8x100_fabric.fsrd_schan_id;
    fabric_data->ref_clk = info->ref_clk;
    fabric_data->fw_load_method = pm_add_info->pm_specific_info.pm8x100_fabric.fw_load_method;
    fabric_data->fw_loader = pm_add_info->pm_specific_info.pm8x100_fabric.external_fw_loader;

    PORTMOD_PBMP_ASSIGN(fabric_data->phys, pm_add_info->phys);

    /*init wb buffer*/
    _SOC_IF_ERR_EXIT(pm8x100_fabric_wb_buffer_init(unit, wb_buffer_index,  pm_info));

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
        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], -1));

        for (i = 0; i < PM8X100_FABRIC_LANES_PER_CORE; i++)
        {
            rx_polarity = (info->polarity.rx_polarity >> i) & 0x1;
            tx_polarity = (info->polarity.tx_polarity >> i) & 0x1;

            _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_rx], &rx_polarity, i));
            _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_tx], &tx_polarity, i));
            _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane2port_map], &invalid_port, i));
        }

        /* Set default PLL 0 */
        _SOC_IF_ERR_EXIT(PM8X100_FABRIC_PLL_ACTIVE_LANES_BITMAP_SET(unit, pm_info, pll_active_lanes_bitmap, PM8X100_FABRIC_DEFAULT_PLL));

        _SOC_IF_ERR_EXIT(PM8X100_FABRIC_PLL_VCO_RATE_SET(unit, pm_info, pll_vco_rate, PM8X100_FABRIC_DEFAULT_PLL));
    }
    else
    {
        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &is_core_probed));

        if (is_core_probed)
        {
            /* Probe */
            _SOC_IF_ERR_EXIT(phymod_core_access_t_init(&core_access));
            sal_memcpy(&core_access.access, &fabric_data->access, sizeof(fabric_data->access));

            _SOC_IF_ERR_EXIT(portmod_common_serdes_probe(pm8x100_fabric_serdes_list, &core_access, &is_phymod_probed));
            if (!is_phymod_probed)
            {
                _SOC_EXIT_WITH_ERR(PHYMOD_E_INIT, ("failed to probe core"));
            }
            fabric_data->core_type = core_access.type;
        }
    }
exit:
    if (SOC_FUNC_ERROR)
    {
        pm8x100_fabric_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN;
}



int pm8x100_fabric_pm_destroy(int unit, pm_info_t pm_info)
{
    if(pm_info->pm_data.pm8x100_fabric != NULL){
        if (pm_info->pm_data.pm8x100_fabric->access.pmd_info_ptr != NULL) {
            sal_free(pm_info->pm_data.pm8x100_fabric->access.pmd_info_ptr);
            pm_info->pm_data.pm8x100_fabric->access.pmd_info_ptr = NULL;
        }
        sal_free(pm_info->pm_data.pm8x100_fabric);
        pm_info->pm_data.pm8x100_fabric = NULL;
    }

    return SOC_E_NONE;
}

int pm8x100_fabric_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info)
{
    core_info->ref_clk = pm_info->pm_data.pm8x100_fabric->ref_clk;
    return SOC_E_NONE;
}

/* check if new lane-mapping in add_info is different from existing lane-mapping */
STATIC
int pm8x100_fabric_pm_lane_mapping_changed_get(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info, int* lane_mapping_changed)
{
    int i;
    phymod_lane_map_t lane_map;
    phymod_core_access_t core_access;
    int cores_num;
    SOC_INIT_FUNC_DEFS;

    *lane_mapping_changed = 0;

    if (add_info->init_config.lane_map[0].num_of_lanes != PM8X100_FABRIC_LANES_PER_CORE)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("add_info->init_config.lane_map[0].num_of_lanes != PM8X100_FABRIC_LANES_PER_CORE"));
    }

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_core_access_get(unit, port, pm_info, 0, 1, &core_access, &cores_num, NULL));

    _SOC_IF_ERR_EXIT(phymod_core_lane_map_get(&core_access, &lane_map));

    for(i = 0; i < add_info->init_config.lane_map[0].num_of_lanes; ++i)
    {
        if (lane_map.lane_map_tx[i] != add_info->init_config.lane_map[0].lane_map_tx[i] ||
                        lane_map.lane_map_rx[i] != add_info->init_config.lane_map[0].lane_map_rx[i]) {
            *lane_mapping_changed = 1;
            break;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

/* configure tx FMAC lane-mapping */
STATIC
int pm8x100_fabric_pm_fmac_tx_lane_mapping_set(int unit, int port, pm_info_t pm_info, const phymod_lane_map_t *lane_map)
{
    uint32 field_val;
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    int lane_index, serdes_index, serdes_index_in_fmac;
    int fmac_blk_id, fmac_rev_blk_id, lane_index_in_fmac;
    int tmp_port;
    
    SOC_INIT_FUNC_DEFS;

    /** The relevant register is FMAC_TX_LANE_SWAPr and for each numel of this reg, there are two fields that need to be mapped.
     * FMAC_TX_LANE_SWAP(N) TX_LANE_SWAP_Nf contains the FMAC lane which is mapped to SerDes TX N
     * FMAC_TX_LANE_SWAP(K) TX_LANE_REV_SWAP_Nf contains the TX which is mapped to FMAC Lane K
     * IMPORTANT NOTE: These registers exist in the FMAC and are accessed with FMAC dimension, but logically they are tied to the PM8x100. 
     * This means that if the TX's are swapped accross quads, then the register fields which need to be configured will be on different FMACs.
     * 
     * Formula: FMAC_TX_LANE_SWAP(N) = < TX_LANE_SWAP_N = the fmac lane connected to txN, TX_LANE_REV_SWAP_N = the tx connected to laneN >
     * 
     * Example 1 (no swap): lane_to_serdes_map_fabric_lane0.BCM8892X=rx0:tx0 would set both fields to 0 in numel 0 of the FMAC_TX_LANE_SWAP reg in FMAC0
     * Example 2 (swap): lane_to_serdes_map_fabric_lane2.BCM8892X=rx2:tx5 needs to set TX_LANE_SWAP_N to 2 in FMAC_TX_LANE_SWAP(5), which is FMAC1 numel 1
     * TX_LANE_REV_SWAP_N needs to be set to 5 on FMAC_TX_LANE_SWAP(2), which is FMAC0 numel 2
     */

    /* configure lane swap on all lanes of PM */
    for (lane_index = 0; lane_index < PM8X100_FABRIC_MAX_PORTS_PER_PM; lane_index++)
    {
        _SOC_IF_ERR_EXIT(PM8X100_FABRIC_LANE2PORT_GET(unit, pm_info, lane_index, &tmp_port));

        if (tmp_port != -1)
        {
            serdes_index = lane_map->lane_map_tx[lane_index];
            /* Get the access dimensions for the FMAC lane and the SerDes Tx */
            PM8X100_FABRIC_SERDES_FMAC_DATA_GET(pm_info, serdes_index, fmac_blk_id, serdes_index_in_fmac);
            PM8X100_FABRIC_SERDES_FMAC_DATA_GET(pm_info, lane_index, fmac_rev_blk_id, lane_index_in_fmac);

            /* Register index is serdes tx, field value is FMAC lane */
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_TX_LANE_SWAP, fmac_blk_id, serdes_index_in_fmac, 0, reg_val));

            field_val = lane_index;
            _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_TX_LANE_SWAP_fTX_LANE_SWAP_N, reg_val, &field_val));
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_TX_LANE_SWAP, fmac_blk_id, serdes_index_in_fmac, 0, reg_val));

            /*
            * Reverse config
            */
            /* Register index is FMAC lane, field value is Serdes Tx */
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_TX_LANE_SWAP, fmac_rev_blk_id, lane_index_in_fmac, 0, reg_val));

            field_val = serdes_index;
            _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_TX_LANE_SWAP_fTX_LANE_REV_SWAP_N, reg_val, &field_val));
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_TX_LANE_SWAP, fmac_rev_blk_id, lane_index_in_fmac, 0, reg_val));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

/* return tx FMAC lane-mapping */
STATIC
int pm8x100_fabric_pm_fmac_tx_lane_mapping_get(int unit, int port, pm_info_t pm_info, phymod_lane_map_t *lane_map)
{
    uint32 field_val, reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    uint32 fmac_blk_id, fmac_rev_blk_id;
    int lane_index, serdes_index, lane_index_in_fmac, serdes_index_in_fmac, tmp_port;

    SOC_INIT_FUNC_DEFS;

    /** read lane swap on all lanes of PM */
    for (serdes_index = 0; serdes_index < PM8X100_FABRIC_MAX_PORTS_PER_PM; serdes_index++)
    {
        _SOC_IF_ERR_EXIT(PM8X100_FABRIC_LANE2PORT_GET(unit, pm_info, serdes_index, &tmp_port));

        if (tmp_port != -1)
        {
            /* Get the access dimensions for SerDes Tx */
            PM8X100_FABRIC_SERDES_FMAC_DATA_GET(pm_info, serdes_index, fmac_blk_id, serdes_index_in_fmac);

            /** 
             * Register index is serdes tx, field value is FMAC lane 
             * Get FMAC Lane Mapping accoridng to SerDes Tx
             */
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_TX_LANE_SWAP, fmac_blk_id, serdes_index_in_fmac, 0, reg_val));
            _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_TX_LANE_SWAP_fTX_LANE_SWAP_N, reg_val, &field_val));

            lane_map->lane_map_tx[field_val] = serdes_index;

            /* Get the lane_index which is mapped to serdes_index and then get the access dimensions */
            lane_index = field_val;
            PM8X100_FABRIC_SERDES_FMAC_DATA_GET(pm_info, lane_index, fmac_rev_blk_id, lane_index_in_fmac);

            /**
             * Register index is FMAC lane, field value is Serdes Tx
             * Get SerDes TX Mapping according to FMAC Lane and check if it corresponds to the above
             */
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_TX_LANE_SWAP, fmac_rev_blk_id, lane_index_in_fmac, 0, reg_val));
            _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_TX_LANE_SWAP_fTX_LANE_REV_SWAP_N, reg_val, &field_val));

            /* Compare the value from the TX_LANE_REV_SWAP field - it should be equal to serdes_index */
            if (field_val != serdes_index)
            {
                _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("Illegal FMAC swap value"));
            }
        }
    }

exit:
    SOC_FUNC_RETURN;
}

/* configure rx FMAC lane-mapping */
STATIC
int pm8x100_fabric_pm_fmac_rx_lane_mapping_set(int unit, int port, pm_info_t pm_info, const phymod_lane_map_t *lane_map)
{
    uint32 field_val, reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    uint32 fmac_blk_id;
    int lane_index, serdes_index, serdes_index_in_fmac, serdes_index_in_pm, lane_index_in_pm;
    const soc_reg_t rx_lane_swap_regs[] = {rFMAC_RX_QUAD_SWAP_0, rFMAC_RX_QUAD_SWAP_1, rFMAC_RX_QUAD_SWAP_2, rFMAC_RX_QUAD_SWAP_3};
    const soc_field_t rx_quad_swap_fields[] = {rFMAC_RX_QUAD_SWAP_0_fRX_QUAD_SWAP_0, rFMAC_RX_QUAD_SWAP_1_fRX_QUAD_SWAP_1, rFMAC_RX_QUAD_SWAP_2_fRX_QUAD_SWAP_2, rFMAC_RX_QUAD_SWAP_3_fRX_QUAD_SWAP_3};
    int is_lower_fmac, is_lower_serdes, is_quad_swap, tmp_port;
    SOC_INIT_FUNC_DEFS;

    /* configure lane swap on all lanes of PM */
    for (lane_index = 0; lane_index < PM8X100_FABRIC_MAX_PORTS_PER_PM; lane_index++)
    {
        _SOC_IF_ERR_EXIT(PM8X100_FABRIC_LANE2PORT_GET(unit, pm_info, lane_index, &tmp_port));
        
        if (tmp_port != -1)
        {
            serdes_index = lane_map->lane_map_rx[lane_index];
            serdes_index_in_pm = PM8X100_FABRIC_LANE_INDEX_IN_PM_GET(serdes_index);
            PM8X100_FABRIC_SERDES_FMAC_DATA_GET(pm_info, serdes_index, fmac_blk_id, serdes_index_in_fmac);
            lane_index_in_pm = PM8X100_FABRIC_LANE_INDEX_IN_PM_GET(lane_index);

            is_lower_serdes = 0;
            if (serdes_index_in_pm < PM8X100_FABRIC_LANES_PER_FMAC)
            {
                is_lower_serdes = 1;
            }

            is_lower_fmac = 0;
            if (lane_index_in_pm < PM8X100_FABRIC_LANES_PER_FMAC)
            {

                is_lower_fmac = 1;
            }

            is_quad_swap = is_lower_serdes ^ is_lower_fmac ? 1 : 0;

            /* register index is serdes id, register value is logical (FMAC) lane */
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rx_lane_swap_regs[serdes_index_in_fmac], fmac_blk_id, 0, 0, reg_val));

            field_val = is_quad_swap;
            _SOC_IF_ERR_EXIT(access_field_set(unit, rx_quad_swap_fields[serdes_index_in_fmac], reg_val, &field_val));
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rx_lane_swap_regs[serdes_index_in_fmac], fmac_blk_id, 0, 0, reg_val));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int pm8x100_fabric_pm_fmac_is_quad_swap_get(int unit, int fmac_blk_id, int fmac_inner_link, int *is_quad_swap)
{
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 }, field = 0;
    const soc_reg_t rx_lane_swap_regs[] = {rFMAC_RX_QUAD_SWAP_0, rFMAC_RX_QUAD_SWAP_1, rFMAC_RX_QUAD_SWAP_2, rFMAC_RX_QUAD_SWAP_3};
    const soc_field_t rx_quad_swap_fields[] = {rFMAC_RX_QUAD_SWAP_0_fRX_QUAD_SWAP_0, rFMAC_RX_QUAD_SWAP_1_fRX_QUAD_SWAP_1, rFMAC_RX_QUAD_SWAP_2_fRX_QUAD_SWAP_2, rFMAC_RX_QUAD_SWAP_3_fRX_QUAD_SWAP_3};

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(access_regmem(unit, 0, rx_lane_swap_regs[fmac_inner_link], fmac_blk_id, 0, 0, reg_val));

    /** read quad_swap value */
    _SOC_IF_ERR_EXIT(access_field_get(unit, rx_quad_swap_fields[fmac_inner_link], reg_val, &field));
    *is_quad_swap = field;
exit:
    SOC_FUNC_RETURN;
}

/* return rx FMAC lane-mapping */
STATIC
int pm8x100_fabric_pm_fmac_rx_lane_mapping_get(int unit, int port, pm_info_t pm_info, phymod_lane_map_t *lane_map)
{
    int quad_swap;
    int lane_index, serdes_index;
    int fmac_blk_id, fmac_inner_link;
    uint32 all_lanes_in_pm_bitmap = 0xFF;

    SOC_INIT_FUNC_DEFS;

    /** read lane swap on all lanes of PM */
    PM8X100_FABRIC_PORT_BITMAP_ITER(all_lanes_in_pm_bitmap, serdes_index)
    {
        PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, serdes_index, fmac_blk_id, fmac_inner_link);

        /** read quad_swap value */
        _SOC_IF_ERR_EXIT(pm8x100_fabric_pm_fmac_is_quad_swap_get(unit, fmac_blk_id, fmac_inner_link, &quad_swap));

        if (serdes_index < PM8X100_FABRIC_LANES_PER_FMAC)
        {
            lane_index = fmac_inner_link + (PM8X100_FABRIC_LANE_FMAC_INDEX_GET(serdes_index) + quad_swap) * PM8X100_FABRIC_LANES_PER_FMAC;
        }
        else
        {
            lane_index = fmac_inner_link + (PM8X100_FABRIC_LANE_FMAC_INDEX_GET(serdes_index) - quad_swap) * PM8X100_FABRIC_LANES_PER_FMAC;
        }

        lane_map->lane_map_rx[lane_index] = serdes_index;
    }

exit:
    SOC_FUNC_RETURN;
}

/**
 * Function to prepare core_config based on add_info provided from soc layer
 * core_config is after that provided to before providing to phymod_core_init
 * Decide based on soc information if loading FW on stages or in one step
 * In theory during init loading is on stages as it will be using BRDC
 * and when dynamically attached we assume FW is loaded and we load in one stage with no flags
 */
STATIC
int pm8x100_fabric_core_init_core_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info, phymod_core_init_config_t * core_config, phymod_core_status_t * phy_status)
{
    pm8x100_fabric_t fabric_data = pm_info->pm_data.pm8x100_fabric;
    int i;
    uint32 rx_polarity, tx_polarity;
    SOC_INIT_FUNC_DEFS;

    /*core config*/
    phymod_core_init_config_t_init(core_config);

    for(i = 0; i < add_info->init_config.lane_map[0].num_of_lanes; ++i)
    {
        core_config->lane_map.lane_map_rx[i] = add_info->init_config.lane_map[0].lane_map_rx[i];
        core_config->lane_map.lane_map_tx[i] = add_info->init_config.lane_map[0].lane_map_tx[i];
    }
    core_config->lane_map.num_of_lanes = add_info->init_config.lane_map[0].num_of_lanes;

    for (i = 0; i < PM8X100_FABRIC_LANES_PER_CORE; i++) 
    {
        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_rx], &rx_polarity, i));
        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_tx], &tx_polarity, i));

        core_config->polarity_map.rx_polarity |= (rx_polarity << i) & 0xff;
        core_config->polarity_map.tx_polarity |= (tx_polarity << i) & 0xff;
    }

    core_config->firmware_load_method = fabric_data->fw_load_method;
    if(fabric_data->fw_load_method == phymodFirmwareLoadMethodExternal)
    {
        /* Fast firmware load */
        core_config->firmware_loader = fabric_data->fw_loader;
    }
    else
    {
        core_config->firmware_loader = NULL;
    }

    /* Set the default PLL div value in the init
     * Currently the only option. In other PMs it is set to invalid and not configured
     */
    core_config->pll0_div_init_value = phymod_TSCBH_PLL_DIV170;

    if(PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) 
    {
        PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_SET(core_config);
    }

    if(PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) 
    {
        PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_SET(core_config);
    }

    if(PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_GET(add_info)) 
    {
        PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(core_config);
    }

    if (PORTMOD_PORT_ADD_F_BYPASS_FW_CRC_CHECK_GET(add_info)) 
    {
        PHYMOD_CORE_INIT_F_BYPASS_CRC_CHECK_SET(core_config);
    }

    core_config->interface.ref_clock = fabric_data->ref_clk;

    /*Mark core status as not touched.
     *This is needed the first time we initialize it so the phycode will know to initialize it properly
     */
    _SOC_IF_ERR_EXIT(phymod_core_status_t_init(phy_status));
    phy_status->pmd_active = FALSE;

exit:
    SOC_FUNC_RETURN;
}

/**
 * First phase of attaching and probing a phy core
 * 1. Power up FSRD so we can read/write to phy
 * 2. Probe the serdes to indentify it
 * 3. Set TX/RX lane mapping
 * 4. Init phy core and start loading FW or load FW entirely if not attach on stages
 * Disable port after attach as they should be specifically enabled
 */
STATIC
int pm8x100_fabric_core_init_phase1(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    pm8x100_fabric_t fabric_data = pm_info->pm_data.pm8x100_fabric;
    phymod_core_access_t core_access;
    phymod_core_status_t phy_status;
    phymod_core_init_config_t core_config;
    int fsrd_schan_id;
    portmod_pbmp_t pm_port_phys;
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 }, field_val;
    int successfully_probed = 0;
    int cores_num;

    SOC_INIT_FUNC_DEFS;

    PORTMOD_PBMP_ASSIGN(pm_port_phys, fabric_data->phys);
    PORTMOD_PBMP_AND(pm_port_phys, add_info->phys);

    fsrd_schan_id = pm_info->pm_data.pm8x100_fabric->fsrd_schan_id;

    /**
     * Start the sequence on powering up the FSRD
     * This is needed to write the FW and read phy regs
     */
    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFSRD_SRD_CTRL, fsrd_schan_id, 0, 0, reg_val));
    field_val = 0;
    _SOC_IF_ERR_EXIT(access_field_set(unit, rFSRD_SRD_CTRL_fSRD_N_PMD_LN_RX_H_PWRDN, reg_val, &field_val));
    _SOC_IF_ERR_EXIT(access_field_set(unit, rFSRD_SRD_CTRL_fSRD_N_PMD_LN_TX_H_PWRDN, reg_val, &field_val));
    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFSRD_SRD_CTRL, fsrd_schan_id, 0, 0, reg_val));

    sal_usleep(10);

    field_val = 1;
    _SOC_IF_ERR_EXIT(access_field_set(unit, rFSRD_SRD_CTRL_fSRD_N_PMD_POR_H_RSTB, reg_val, &field_val));
    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFSRD_SRD_CTRL, fsrd_schan_id, 0, 0, reg_val));

    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFSRD_SRD_CTRL, fsrd_schan_id, 0, 0, reg_val));
    field_val = 0xff;
    _SOC_IF_ERR_EXIT(access_field_set(unit, rFSRD_SRD_CTRL_fSRD_N_PMD_LN_RX_H_RSTB, reg_val, &field_val));
    _SOC_IF_ERR_EXIT(access_field_set(unit, rFSRD_SRD_CTRL_fSRD_N_PMD_LN_TX_H_RSTB, reg_val, &field_val));
    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFSRD_SRD_CTRL, fsrd_schan_id, 0, 0, reg_val));

    /**
     * Probe in phycode have the meaning of checking phy core type and to successfully identify it as the first stage
     * and to set the dispatch type core_access->type = phymodDispatchTypeInvalid
     * This should be done only once per PM phy core , only for first port
     * as common function is executed only if core_access structure doesn't have a dispatch type
     */
    /* Get core access info so its params can be changed */
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_core_access_get(unit, port, pm_info, 0, 1, &core_access, &cores_num, NULL));

    _SOC_IF_ERR_EXIT(portmod_common_serdes_probe(pm8x100_fabric_serdes_list, &core_access, &successfully_probed));
    if (!successfully_probed)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("Failed to probe the core"));
    }
    fabric_data->core_type = core_access.type;

    /* Mark DB as probed */
    successfully_probed = 1;
    _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &successfully_probed));

    /**
     * Set Lane mapping to FMAC and PHY
     */
    _SOC_IF_ERR_EXIT(pm8x100_fabric_pm_fmac_tx_lane_mapping_set(unit, port, pm_info, add_info->init_config.lane_map));
    _SOC_IF_ERR_EXIT(pm8x100_fabric_pm_fmac_rx_lane_mapping_set(unit, port, pm_info, add_info->init_config.lane_map));

    if (!SOC_IS_RELOADING(unit))
    {
        /**
         * When core_config is fully populated provide to phymod to set the phycore
         * And start PHY FW load and init
         */
        _SOC_IF_ERR_EXIT(pm8x100_fabric_core_init_core_config_set(unit, port, pm_info, add_info, &core_config, &phy_status));

        _SOC_IF_ERR_EXIT(phymod_core_init(&core_access, &core_config, &phy_status));
    }

exit:
    SOC_FUNC_RETURN;
}

/**
 * This is the second part needed when port is attached
 * 1. If loading FW on stages finish FW load
 * 2. Put FSRD out of reset and enable datapath
 * 3. Mark PM as fully inited
 */
STATIC
int pm8x100_fabric_core_init_phase2(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int fsrd_schan_id;
    phymod_core_status_t phy_status;
    phymod_core_init_config_t core_config;
    phymod_core_access_t core_access;
    int cores_num;
    uint32 is_core_initialized;
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 }, field_val;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    /* in easy reload the purpose is only to update the SW and leave HW as is.
     * skip phymod_core_init function in easy reload because:
     * - it gives an error about core not being in reset state. The core shouldn't be in reset in easy reload.
     * - it is not needed because it doesn't configure any SW (phymod is stateless).
     *   exception- a global phymod struct is initialized in core_init, and so in easy reload it will be done by phymod_phy_pmd_info_init.
     */
    if (SOC_IS_RELOADING(unit))
    {
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        params.phyn = 0;
        _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
        SOC_RELOAD_MODE_SET(unit, FALSE);
        _SOC_IF_ERR_EXIT(phymod_phy_pmd_info_init(&phy_access));
        SOC_RELOAD_MODE_SET(unit, TRUE);
    }
    else
    {

        /**
         * Finish FW load and core init only if we init on stages
         */
        if(PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info))
        {
            /* Get core access info so its params can be checked */
            _SOC_IF_ERR_EXIT(pm8x100_fabric_port_core_access_get(unit, port, pm_info, 0, 1, &core_access, &cores_num, NULL));

            _SOC_IF_ERR_EXIT(pm8x100_fabric_core_init_core_config_set(unit, port, pm_info, add_info, &core_config, &phy_status));
            _SOC_IF_ERR_EXIT(phymod_core_init(&core_access, &core_config, &phy_status));
        }
        /**
         * Finish FSRD out of reset and power up
         */
        fsrd_schan_id = pm_info->pm_data.pm8x100_fabric->fsrd_schan_id;

        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFSRD_SRD_PLL_0_CTRL, fsrd_schan_id, 0, 0, reg_val));
        field_val = 1;
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFSRD_SRD_PLL_0_CTRL_fSRD_N_PMD_CORE_PLL_0_DP_H_RSTB, reg_val, &field_val));
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFSRD_SRD_PLL_0_CTRL, fsrd_schan_id, 0, 0, reg_val));

        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFSRD_SRD_CTRL, fsrd_schan_id, 0, 0, reg_val));
        field_val = 0xff;
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFSRD_SRD_CTRL_fSRD_N_PMD_LN_RX_DP_H_RSTB, reg_val, &field_val));
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFSRD_SRD_CTRL_fSRD_N_PMD_LN_TX_DP_H_RSTB, reg_val, &field_val));
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFSRD_SRD_CTRL, fsrd_schan_id, 0, 0, reg_val));
    }

    /* Mark DB as initialized */
    is_core_initialized = TRUE;
    _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_initialized], &is_core_initialized));

exit:
    SOC_FUNC_RETURN;
}

/**
 * Disable port after attach as they should be specifically enabled
 */
STATIC
int pm8x100_fabric_port_disable_init(int unit, int port, pm_info_t pm_info)
{
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    uint32 field_val[1];
    uint32 phy_index, bitmap[1];
    int i;
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X100_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index, fmac_index;
    soc_field_t reset_fields[] = {rFMAC_RECEIVE_RESET_REGISTER_fFMAC_RX_RST_N, rFMAC_RECEIVE_RESET_REGISTER_fFMAC_TX_RST_N};
    int flags = 0;
    SOC_INIT_FUNC_DEFS;

    /*Port (MAC + PHY) Enable Disable support*/
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));

    _SOC_IF_ERR_EXIT(pm8x100_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    for (i = 0 ; i < sizeof(reset_fields)/sizeof(*reset_fields); ++i) 
    {
        PM8X100_FABRIC_PORT_FMACS_ITER(fmac_blk_ids, nof_fmac_blks, first_fmac_index, fmac_index) 
        {
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_RECEIVE_RESET_REGISTER, fmac_blk_ids[fmac_index], 0, 0, reg_val));
            _SOC_IF_ERR_EXIT(access_field_get(unit, reset_fields[i], reg_val, field_val));
            _SOC_IF_ERR_EXIT(pm8x100_fabric_port_fmac_lanes_bitwrite(unit, port, bitmap[0], fmac_index, field_val, 1));
            _SOC_IF_ERR_EXIT(access_field_set(unit, reset_fields[i], reg_val, field_val));
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_RECEIVE_RESET_REGISTER, fmac_blk_ids[fmac_index], 0, 0, reg_val));
        }
    }

    PORTMOD_PORT_ENABLE_TX_SET(flags);
    PORTMOD_PORT_ENABLE_RX_SET(flags);
    _SOC_IF_ERR_EXIT(pm8x100_fabric_squelch_set(unit, port, pm_info, flags, 1));

exit:
    SOC_FUNC_RETURN;
}

/**
 * Put phy rx/tx in reset/squelch on or out of reset/squelch off
 */
STATIC
int pm8x100_fabric_squelch_set(int unit, int port, pm_info_t pm_info, int flags, int squelch)
{
    portmod_access_get_params_t params;
    phymod_phy_tx_lane_control_t tx_squelch = phymodTxSquelchOff;
    phymod_phy_rx_lane_control_t rx_squelch = phymodRxSquelchOff;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));

    if PORTMOD_PORT_ENABLE_TX_GET(flags)
    {
        tx_squelch = squelch? phymodTxSquelchOn : phymodTxSquelchOff;
    }
    if PORTMOD_PORT_ENABLE_RX_GET(flags)
    {
        rx_squelch = squelch? phymodRxSquelchOn : phymodRxSquelchOff;
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));

    if PORTMOD_PORT_ENABLE_TX_GET(flags)
    {
        _SOC_IF_ERR_EXIT(phymod_phy_tx_lane_control_set(&phy_access, tx_squelch));
    }
    if PORTMOD_PORT_ENABLE_RX_GET(flags)
    {
        _SOC_IF_ERR_EXIT(phymod_phy_rx_lane_control_set(&phy_access, rx_squelch));
    }

exit:
    SOC_FUNC_RETURN;
}

/**
 * Turn phy power on/off
 */
STATIC
int pm8x100_fabric_phy_power_set(int unit, int port, pm_info_t pm_info, int power)
{
    portmod_access_get_params_t params;
    phymod_phy_power_t phy_power;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(phymod_phy_power_t_init(&phy_power));
    _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));

    if (power) {
        phy_power.rx = phymodPowerOn;
        phy_power.tx = phymodPowerOn;
    } else {
        phy_power.rx = phymodPowerOff;
        phy_power.tx = phymodPowerOff;
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(phymod_phy_power_set(&phy_access, &phy_power));

exit:
    SOC_FUNC_RETURN;
}

/**
 * Power up the PHY and configure phy parameters based on provided info from soc layer 
 * Part of the attach/probe sequence
 */
STATIC
int pm8x100_fabric_port_phy_init(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    phymod_phy_init_config_t init_config;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys;
    int lane_index;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));
    _SOC_IF_ERR_EXIT(phymod_phy_init_config_t_init(&init_config));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));

    /**
     * Power up the phy
     */
    _SOC_IF_ERR_EXIT(pm8x100_fabric_phy_power_set(unit, port, pm_info, 1));

    params.phyn = 0;
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));

    init_config.an_en = add_info->autoneg_en;
    init_config.cl72_en = add_info->link_training_en;

    /* In order to avoid assuming which indexes of init_config.tx phymod will use for the lanes of the port - fill all array indexes */
    for (lane_index = 0; lane_index < PHYMOD_MAX_LANES_PER_CORE; ++lane_index)
    {
        /* the speed is not known at this stage. Choosing PAM4 defaults */
        _SOC_IF_ERR_EXIT(phymod_phy_tx_taps_default_get(&phy_access, phymodSignallingMethodPAM4, &init_config.tx[lane_index]));
        init_config.tx[lane_index].sig_method = phymodSignallingMethodPAM4;
    }
    /*
    * One init_config is fully configured init the PHY
    */
    _SOC_IF_ERR_EXIT(phymod_phy_init(&phy_access, &init_config));

exit:
    SOC_FUNC_RETURN;
}

/* Put phy core in/out of reset
 * This is the oposite proces of the core init/power up which is executed using FSRD registers
 * FSRD_SRD_CTRLr, SRD_N_PMD_LN_RX_H_PWRDNf, , SRD_N_PMD_LN_RX_H_RSTBf
 * The asymmetric approach is needed as the FSRD register accomplish the connected with the PHY with the FSRD wrapper
 * where for power down it is more correct ot use directly a PHY function  
*/
STATIC
int pm8x100_fabric_pm_phy_core_reset(int unit, int port, pm_info_t pm_info, int in_reset)
{
    phymod_core_access_t core_access;
    int cores_num;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_core_access_get(unit, port, pm_info, 0, 1, &core_access, &cores_num, NULL));

    if (in_reset)
    {
        _SOC_IF_ERR_EXIT(phymod_core_reset_set(&core_access, phymodResetModeHard, phymodResetDirectionIn));

    }
    else
    {
        _SOC_IF_ERR_EXIT(phymod_core_reset_set(&core_access, phymodResetModeHard, phymodResetDirectionOut));
    }

exit:
    SOC_FUNC_RETURN;
}

/**
 * Execute all needed to prepare the PM, FSRD, FMAC and PHY to use a fabric link
 */
int pm8x100_fabric_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int i = 0, ii, phy;
    int found = FALSE;
    int ports_db_entry = PM8X100_FABRIC_INVALID_PORT;
    portmod_pbmp_t port_phys_in_pm;
    uint32 phys_count, is_core_already_probed = 0, is_core_initialized = 0;
    pm8x100_fabric_t fabric_data = pm_info->pm_data.pm8x100_fabric;
    int lane_mapping_changed = 0;

    SOC_INIT_FUNC_DEFS;

    /*lane mapping passed to phymod: peregrine serdes (represented by index in array) -> {lane rx, lane tx} */
    if (add_info->init_config.lane_map[0].num_of_lanes != PM8X100_FABRIC_LANES_PER_CORE)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("add_info->lane_map.num_of_lanes != PM8X100_FABRIC_LANES_PER_CORE"));
    }

    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, fabric_data->phys);
    PORTMOD_PBMP_COUNT(port_phys_in_pm, phys_count);

    /**
     * ******
     * PART 1
     * ******
     * If PASS1 or FULL SEQUENCE (not on stages)
     * Configure and init PM
     * Power up the Serdes
     * Probe the Phy
     * Start loading FW on PHY
     */
    if(!PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info))
    {
        /**
         * Set Lane2Port WB data giving which lane from the PM is attached to the port
         */
        ii = 0;
        SOC_PBMP_ITER(fabric_data->phys, phy)
        {
            if(PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy))
            {
                _SOC_IF_ERR_EXIT(PM8X100_FABRIC_LANE2PORT_SET(unit, pm_info, ii, port));
            }
            ii++;
        }

        /**
         * Find PM db entry slot
         */
        for( i = 0 ; i < PM8X100_FABRIC_MAX_PORTS_PER_PM; i++)
        {
            /*
             * Used only to track the number of attached ports on the PM for finding if is_first/is_last
             * Also for sanity validation
             */
            _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &ports_db_entry, i));
            /*if free slot found*/
            if(ports_db_entry < 0 )
            {
                _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &port, i));
                found = TRUE;
                break;
            }
        }
        if(!found)
        {
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("no free space in the PM db"));
        }

        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &is_core_already_probed));
        /**
         * Execute all needed prior to FW load
         * Phase 1 is supposed to be called only once per PM, for the first attached port
         */
        if (!is_core_already_probed)
        {
            _SOC_IF_ERR_EXIT(pm8x100_fabric_core_init_phase1(unit, port, pm_info, add_info));
        }

        /* phymod_core_init is called once per core- for the first port we attach. It configures the lane-mapping.
         * The following dynamic-port scenario is not supported-
         * some of the ports of the core are detached after init sequence and the user wants to change their lane-mapping.
         * we can't call phymod_core_init again when some of the ports are already attached, thus can't reconfigure the lane-mapping.
         * Under EASY RELOAD and if we are on simulator we cannot expect that the phy registers will return meaningful values
         * so the check here is invalid
         * This should happen after the phy_core_init so the change will take place
         */
        if (!SOC_IS_RELOADING(unit) && !SAL_BOOT_SIMULATION)
        {
            _SOC_IF_ERR_EXIT(pm8x100_fabric_pm_lane_mapping_changed_get(unit, port, pm_info, add_info, &lane_mapping_changed));
            if (lane_mapping_changed)
            {
                _SOC_EXIT_WITH_ERR(SOC_E_CONFIG, ("Changing lane-map configuration on a core (octet) containing active ports is not supported"));
            }
        }
    }

    /**
     * ******
     * PART 2
     * ******
     * If PASS2 or FULL SEQUENCE
     * Finish sequence by configuring and init also the phy
     * Finish FW load (if on stages)
     * Put out of reset the SerDeses
     * Mark PM as fully inited
     */
    if(!PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info))
    {
        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_initialized], &is_core_initialized));
        if(!is_core_initialized) 
        {
            /* Finish PM init and mark PM in DB as fully initialized */
            _SOC_IF_ERR_EXIT(pm8x100_fabric_core_init_phase2(unit, port, pm_info, add_info));
        }
        /* Configure phy itself and power it up */
        _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_init(unit, port, pm_info, add_info));

        /*Disable phy and mac. It is supposed to be enabled after speed/fec is configured*/
        _SOC_IF_ERR_EXIT(pm8x100_fabric_port_disable_init(unit, port, pm_info));
    }

exit:
    SOC_FUNC_RETURN;
}


int pm8x100_fabric_port_detach(int unit, int port, pm_info_t pm_info)
{
    int i = 0;
    int found = FALSE;
    int invalid_port = PM8X100_FABRIC_INVALID_PORT;
    int ports_db_entry = PM8X100_FABRIC_INVALID_PORT;
    int is_last = TRUE;
    int unintialized = 0;
    uint32 tmp_port;
    uint32 phy_index, bitmap[1];
    int lane_index;
    uint32 pll_active_lanes_bitmap;
    portmod_vco_type_t pll_vco_rate;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys;
    pm8x100_fabric_t fabric_data = pm_info->pm_data.pm8x100_fabric;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm8x100_fabric_phy_power_set(unit, port, pm_info, 0));

    _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));
    SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));

    _SOC_IF_ERR_EXIT(PM8X100_FABRIC_PLL_ACTIVE_LANES_BITMAP_GET(unit, pm_info, &pll_active_lanes_bitmap, PM8X100_FABRIC_DEFAULT_PLL));
    _SOC_IF_ERR_EXIT(PM8X100_FABRIC_PLL_VCO_RATE_GET(unit, pm_info, &pll_vco_rate, PM8X100_FABRIC_DEFAULT_PLL));

    /**
     * Remove the detached lanes from the active lanes on the PLL
     */
    if (SHR_BITGET(&pll_active_lanes_bitmap, phy_index))
    {
        PM8X100_FABRIC_PORT_BITMAP_ITER(bitmap[0], lane_index)
        {
            SHR_BITCLR(&pll_active_lanes_bitmap, lane_index);
        }

        /**
         * If no more active lanes on the PLL reset the VCO rate
         * and power down the PHY
         */
        if (pll_active_lanes_bitmap == 0)
        {
            pll_vco_rate = portmodVCOInvalid;
            _SOC_IF_ERR_EXIT(phymod_phy_pll_pwrdn(&phy_access, PM8X100_FABRIC_DEFAULT_PLL, 1));
        }

        _SOC_IF_ERR_EXIT(PM8X100_FABRIC_PLL_ACTIVE_LANES_BITMAP_SET(unit, pm_info, pll_active_lanes_bitmap, PM8X100_FABRIC_DEFAULT_PLL));
        _SOC_IF_ERR_EXIT(PM8X100_FABRIC_PLL_VCO_RATE_SET(unit, pm_info, pll_vco_rate, PM8X100_FABRIC_DEFAULT_PLL));
    }

    for( i = 0 ; i < PM8X100_FABRIC_MAX_PORTS_PER_PM; i++)
    {
        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &ports_db_entry, i));
        if(ports_db_entry == port)
        {
            _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &invalid_port, i));
            found = TRUE;
        }
        else if(ports_db_entry >= 0)
        {
            is_last = FALSE;
        }
        _SOC_IF_ERR_EXIT(PM8X100_FABRIC_LANE2PORT_GET(unit, pm_info, i, &tmp_port));
        if(tmp_port == port)
        {
            _SOC_IF_ERR_EXIT(PM8X100_FABRIC_LANE2PORT_SET(unit, pm_info, i, invalid_port));
        }
    }
    if(!found)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("port was not found in the PM db"));
    }

    if (is_last)
    {
        _SOC_IF_ERR_EXIT(pm8x100_fabric_pm_phy_core_reset(unit,  port, pm_info, 1));

        /*remove from data - base*/
        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &unintialized));
        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_initialized], &unintialized));

        fabric_data->core_type = phymodDispatchTypeInvalid;
    }

exit:
    SOC_FUNC_RETURN;
}

soc_error_t pm8x100_fabric_fmac_enable_set(int unit, int port, pm_info_t pm_info, int rx_tx, int enable)
{
    soc_field_t field;
    uint32 field_val[1];
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 }, phy_index, bitmap[1];
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X100_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index, fmac_index;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));

    field = ((rx_tx == PM8X100_FABRIC_FMAC_TX) ? rFMAC_RECEIVE_RESET_REGISTER_fFMAC_TX_RST_N : rFMAC_RECEIVE_RESET_REGISTER_fFMAC_RX_RST_N);

    _SOC_IF_ERR_EXIT(pm8x100_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    PM8X100_FABRIC_PORT_FMACS_ITER(fmac_blk_ids, nof_fmac_blks, first_fmac_index, fmac_index) {
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_RECEIVE_RESET_REGISTER, fmac_blk_ids[fmac_index], 0, 0, reg_val));
        _SOC_IF_ERR_EXIT(access_field_get(unit, field, reg_val, field_val));
        _SOC_IF_ERR_EXIT(pm8x100_fabric_port_fmac_lanes_bitwrite(unit, port, bitmap[0], fmac_index, field_val, (enable? 0 : 1)));
        _SOC_IF_ERR_EXIT(access_field_set(unit, field, reg_val, field_val));
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_RECEIVE_RESET_REGISTER, fmac_blk_ids[fmac_index], 0, 0, reg_val));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_fabric_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
    uint32 flags_mask = flags;
    int cur_enable;
    SOC_INIT_FUNC_DEFS;

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
        /*Port (MAC + PHY) Enable Disable support*/

        _SOC_IF_ERR_EXIT(pm8x100_fabric_port_enable_get(unit, port, pm_info, flags, &cur_enable));
        if ((cur_enable ? 1 : 0) == (enable ? 1 : 0))
        {
            SOC_EXIT;
        }

        if(enable){
            _SOC_IF_ERR_EXIT(pm8x100_fabric_squelch_set(unit, port, pm_info, flags_mask, 0));

            /* Enable FMAC Tx*/
            _SOC_IF_ERR_EXIT(pm8x100_fabric_fmac_enable_set(unit, port, pm_info, PM8X100_FABRIC_FMAC_TX, enable));

            /* Enable FMAC Rx*/
            _SOC_IF_ERR_EXIT(pm8x100_fabric_fmac_enable_set(unit, port, pm_info, PM8X100_FABRIC_FMAC_RX, enable));

        }
        else
        {
            /* Disable FMAC Rx*/
            _SOC_IF_ERR_EXIT(pm8x100_fabric_fmac_enable_set(unit, port, pm_info, PM8X100_FABRIC_FMAC_RX, enable));

            /* Disable FMAC Tx*/
            _SOC_IF_ERR_EXIT(pm8x100_fabric_fmac_enable_set(unit, port, pm_info, PM8X100_FABRIC_FMAC_TX, enable));

            _SOC_IF_ERR_EXIT(pm8x100_fabric_squelch_set(unit, port, pm_info, flags_mask, 1));
        }
    }
    else if ((PORTMOD_PORT_ENABLE_RX_GET(flags)) && ((PORTMOD_PORT_ENABLE_MAC_GET(flags))))
    {
        /*MAC RX Enable/Disable support*/
        _SOC_IF_ERR_EXIT(pm8x100_fabric_fmac_enable_set(unit, port, pm_info, PM8X100_FABRIC_FMAC_RX, enable));
    }
    else if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && (!PORTMOD_PORT_ENABLE_PHY_GET(flags_mask)) &&
            PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        if(enable){
            _SOC_IF_ERR_EXIT(pm8x100_fabric_fmac_enable_set(unit, port, pm_info, PM8X100_FABRIC_FMAC_TX, enable));
            _SOC_IF_ERR_EXIT(pm8x100_fabric_fmac_enable_set(unit, port, pm_info, PM8X100_FABRIC_FMAC_RX, enable));
        }else{
            _SOC_IF_ERR_EXIT(pm8x100_fabric_fmac_enable_set(unit, port, pm_info, PM8X100_FABRIC_FMAC_RX, enable));
            _SOC_IF_ERR_EXIT(pm8x100_fabric_fmac_enable_set(unit, port, pm_info, PM8X100_FABRIC_FMAC_TX, enable));
        }
    }
    else if (!PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && PORTMOD_PORT_ENABLE_PHY_GET(flags_mask) &&
            !PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        if (enable)
        {
            _SOC_IF_ERR_EXIT(pm8x100_fabric_squelch_set(unit, port, pm_info, flags_mask, 0));
        }
        else
        {
            _SOC_IF_ERR_EXIT(pm8x100_fabric_squelch_set(unit, port, pm_info, flags_mask, 1));
        }
    }
    else
    {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL , ("Supported options are only: MAC-RX, MAC-RX + MAC-TX, PHY+TX and ALL, \n"));
    }

exit:
    SOC_FUNC_RETURN;
}


int pm8x100_fabric_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int *enable)
{
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 }, flags_mask = flags;
    uint32 field_val[1];
    uint32 mac_reset, tx_mac_reset, bitmap[1];
    uint32 phy_index;
    phymod_phy_access_t phy_access;
    phymod_phy_rx_lane_control_t rx_control;
    phymod_phy_tx_lane_control_t tx_control;
    int rx_enable, tx_enable;
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    portmod_access_get_params_t params;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));
    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));

    /*if the first phy in the first FMAC is enabled - assuming the whole port is enabled*/
    PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);

    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_RECEIVE_RESET_REGISTER, fmac_blk_id, 0, 0, reg_val));
    _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_RECEIVE_RESET_REGISTER_fFMAC_RX_RST_N, reg_val, field_val));
    mac_reset = SHR_BITGET(field_val, lane_index_in_fmac);
    _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_RECEIVE_RESET_REGISTER_fFMAC_TX_RST_N, reg_val, field_val));
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
         _SOC_IF_ERR_EXIT(phymod_phy_rx_lane_control_get(&phy_access, &rx_control));
         _SOC_IF_ERR_EXIT(phymod_phy_tx_lane_control_get(&phy_access, &tx_control));
         rx_enable = ((!mac_reset) && (rx_control != phymodRxSquelchOn));
         tx_enable = ((!tx_mac_reset) && (tx_control != phymodTxSquelchOn));
         *enable = rx_enable || tx_enable;
    }
    else if ((PORTMOD_PORT_ENABLE_RX_GET(flags)) && ((PORTMOD_PORT_ENABLE_MAC_GET(flags))))
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
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL , ("Supported options are only (1) MAC, TX, RX (2) MAC RX (3) MAC TX or (4) all - MAC, PHY, TX, RX"));
    }


exit:
    SOC_FUNC_RETURN;
}

int pm8x100_fabric_port_electrical_idle_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys;
    phymod_phy_tx_lane_control_t control;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));

    control = enable? phymodTxElectricalIdleEnable : phymodTxElectricalIdleDisable;
    _SOC_IF_ERR_EXIT(phymod_phy_tx_lane_control_set(&phy_access, control));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_fabric_port_encoding_set(int unit, int port, pm_info_t pm_info, uint32 properties, portmod_port_pcs_t encoding)
{
    uint32 encoding_type, slow_read_rate;
    int flags = 0;
    int enabled = 0;
    uint32 llfc_cig = 0;
    uint32 low_latency_llfc = 0;
    uint32 rsf_err_mark = 0;
    uint32 fmal_rx_width, fmal_tx_width = 0, field_val;
    uint32 rsf_frame_type = 3; /*same as HW default, stands for 15T_RS_FEC*/
    uint32 fmac_blk_id = 0;
    int lane_index, lane_index_in_fmac = 0;
    uint32 phy_index, bitmap[1];
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    portmod_speed_config_t speed_config;
    SOC_INIT_FUNC_DEFS;

    /** Most of the configuration is the same for all RS FEC Types */
    encoding_type = PM8X100_FABRIC_FEC_RS_ENCODING_TYPE;
    slow_read_rate = PM8X100_FABRIC_FEC_RS_SLOW_READ_RATE;
    fmal_tx_width = 0;

    switch(encoding) {
        case PORTMOD_PCS_64B66B:
            /* bcmPortPhyFecNone */
            encoding_type = 0;
            slow_read_rate = PM8X100_FABRIC_FEC_NONE_SLOW_READ_RATE;
            fmal_tx_width = PM8X100_FABRIC_FEC_NONE_FMAL_TX_WIDTH;
            /** The fmal_rx_width depends on speed and in bcmPortPhyFecNone we need to get the speed, since FecNone is available on both speeds */
            _SOC_IF_ERR_EXIT(pm8x100_fabric_port_speed_config_get(unit, port, pm_info, &speed_config));
            if (speed_config.speed == 53125)
            {
                fmal_rx_width = PM8X100_FABRIC_FEC_NONE_50G_FMAL_RX_WIDTH;
            }
            else
            {
                fmal_rx_width = PM8X100_FABRIC_FEC_NONE_100G_FMAL_RX_WIDTH;
            }
            break;
        case PORTMOD_PCS_64B66B_15T_RS_FEC:
            /* bcmPortPhyFecRs545 */
            rsf_frame_type = PM8X100_FABRIC_FEC_RS_64B66B_15T_FRAME_TYPE;
            fmal_rx_width = PM8X100_FABRIC_FEC_RS_64B66B_15T_FMAL_RX_WIDTH;
            break;
        case PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC:
            /* bcmPortPhyFecRs304 */
            rsf_frame_type = PM8X100_FABRIC_FEC_RS_64B66B_15T_LOW_LATENCY_FRAME_TYPE;
            fmal_rx_width = PM8X100_FABRIC_FEC_RS_64B66B_15T_FMAL_RX_WIDTH;
            break;
        case PORTMOD_PCS_256B257B_15T_1xN_RS_FEC:
            /* bcmPortPhyFecRs544 */
            rsf_frame_type = PM8X100_FABRIC_FEC_RS_256B257B_15T_1xN_FRAME_TYPE;
            low_latency_llfc = 0;
            llfc_cig = 0;
            fmal_rx_width = PM8X100_FABRIC_FEC_RS_256B257B_15T_FMAL_RX_WIDTH;
            break;
        case PORTMOD_PCS_256B257B_15T_2xN_RS_FEC:
            /* bcmPortPhyFecRs544_2xN */
            rsf_frame_type = PM8X100_FABRIC_FEC_RS_256B257B_15T_2xN_FRAME_TYPE;
            low_latency_llfc = 0;
            llfc_cig = 0;
            fmal_rx_width = PM8X100_FABRIC_FEC_RS_256B257B_15T_FMAL_RX_WIDTH;
            break;
        default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("unsupported pcs type %d", encoding));
    }

    /* handle properties*/
    /* Low latency llfc*/
    if(PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties) && (encoding == PORTMOD_PCS_64B66B)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("low latency llfc is not supported for 64/66 pcs"));
    }

    if (PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties) && PM8X100_FABRIC_ENCODING_IS_PROPRIETARY_RS_FEC(encoding)){
        low_latency_llfc = PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties);
        llfc_cig = PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties);
    }

    /* error detect*/
    if(PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties) && (encoding == PORTMOD_PCS_64B66B)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("error detect is not supported for 64/66 pcs"));
    }
    if(PM8X100_FABRIC_ENCODING_IS_RS_FEC(encoding)){
        rsf_err_mark = PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties);
    }

    /* extract cig from llfc cells*/
    if(PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_GET(properties)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("extract CIG indications from LLFC cells is not supported on this device"));
    }

    /* do FEC on LLFC and congestion indication (get llfc/cig values after error correction) */
    if(PORTMOD_ENCODING_LLFC_AFTER_FEC_GET(properties) && !PM8X100_FABRIC_ENCODING_IS_RS_FEC(encoding)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("llfc after FEC is supported only for rs fecs"));
    }

    /* bypass FEC on flow status and credit control cells */
    if(PORTMOD_ENCODING_CONTROL_CELLS_FEC_BYPASS_GET(properties) && !PM8X100_FABRIC_ENCODING_IS_RS_FEC(encoding)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("control cells FEC bypass is supported only for rs fecs"));
    }

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));

    flags = 0;
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_enable_get(unit, port, pm_info, flags, &enabled));
    /** disable phy and mac */
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_enable_set(unit, port, pm_info, flags, 0));

    PM8X100_FABRIC_PORT_BITMAP_ITER(bitmap[0], lane_index) {
        PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, lane_index, fmac_blk_id, lane_index_in_fmac);
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_FMAL_GENERAL_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        /*encoding type set*/
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_FMAL_GENERAL_CONFIGURATION_fFMAL_N_ENCODING_TYPE, reg_val, &encoding_type));
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_FMAL_GENERAL_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
    }

    PM8X100_FABRIC_PORT_BITMAP_ITER(bitmap[0], lane_index) {
        PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, lane_index, fmac_blk_id, lane_index_in_fmac);
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_ASYNC_FIFO_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_ASYNC_FIFO_CONFIGURATION_fFMAL_N_RX_SLOW_READ_RATE, reg_val, &slow_read_rate));
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_ASYNC_FIFO_CONFIGURATION_fFMAL_N_RX_WIDTH, reg_val, &fmal_rx_width));
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_ASYNC_FIFO_CONFIGURATION_fFMAL_N_TX_WIDTH, reg_val, &fmal_tx_width));
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_ASYNC_FIFO_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
    }

    PM8X100_FABRIC_PORT_BITMAP_ITER(bitmap[0], lane_index) {
        PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, lane_index, fmac_blk_id, lane_index_in_fmac);
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_FMAL_RX_GENERAL_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        /*set llfc low latency and enable cig*/
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_FMAL_RX_GENERAL_CONFIGURATION_fFMAL_N_LOW_LATENCY_LLFC, reg_val, &low_latency_llfc));
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_FMAL_RX_GENERAL_CONFIGURATION_fFMAL_N_LOW_LATENCY_CIG, reg_val, &llfc_cig));
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_FMAL_RX_GENERAL_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
    }

    /** RS-FEC configurations */
    PM8X100_FABRIC_PORT_BITMAP_ITER(bitmap[0], lane_index) {
        PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, lane_index, fmac_blk_id, lane_index_in_fmac);
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_RSF_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_RSF_CONFIGURATION_fRSF_N_RX_ERR_MARK_EN, reg_val, &rsf_err_mark));
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_RSF_CONFIGURATION_fRSF_N_FRAME_TYPE, reg_val, &rsf_frame_type));
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_RSF_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
    }

    /** PCS: Enable New block types */
    PM8X100_FABRIC_PORT_BITMAP_ITER(bitmap[0], lane_index) {
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        field_val = PM8X100_FABRIC_ENCODING_IS_STANDARD_RS_FEC(encoding) ? 1 : 0;
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_KPCS_CONFIGURATION_fKPCS_N_NEW_64_66_BLOCK_TYPES, reg_val, &field_val));
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
    }

    /** restore MAC and PHY */
    flags = 0;
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_enable_set(unit, port, pm_info, flags, enabled));

exit:
    SOC_FUNC_RETURN; 
}

int pm8x100_fabric_port_encoding_get(int unit, int port, pm_info_t pm_info, uint32 *properties, portmod_port_pcs_t *encoding)
{
    uint32 encoding_type = 0;
    uint32 rsf_frame_type = 0;
    uint32 low_latency_llfc = 0;
    uint32 err_mark = 0;
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    uint32 phy_index, bitmap[1];
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));
    PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_FMAL_GENERAL_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
 
    *properties = 0;
    _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_FMAL_GENERAL_CONFIGURATION_fFMAL_N_ENCODING_TYPE, reg_val, &encoding_type));
    switch(encoding_type){
        case 0:
            *encoding = PORTMOD_PCS_64B66B;
            break;
        case 1:
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_RSF_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
            _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_RSF_CONFIGURATION_fRSF_N_FRAME_TYPE, reg_val, &rsf_frame_type));
            switch(rsf_frame_type) {
            case 0:
                *encoding = PORTMOD_PCS_64B66B_15T_RS_FEC;
                break;
            case 1:
                *encoding = PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC;
                break;
            case 2:
                *encoding = PORTMOD_PCS_256B257B_15T_1xN_RS_FEC;
                break;
            case 3:
                *encoding = PORTMOD_PCS_256B257B_15T_2xN_RS_FEC;
                break;
            default:
                _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("unknown RS-FEC type"));
            }
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("unknown pcs type %d", encoding_type));
    }

    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_FMAL_RX_GENERAL_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
    /*get llfc low latency, enable cig and ctrl cells bypass*/
    _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_FMAL_RX_GENERAL_CONFIGURATION_fFMAL_N_LOW_LATENCY_LLFC, reg_val, &low_latency_llfc));

    if (PM8X100_FABRIC_ENCODING_IS_RS_FEC(*encoding)) {
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_RSF_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_RSF_CONFIGURATION_fRSF_N_RX_ERR_MARK_EN, reg_val, &err_mark));
    } else {
        err_mark = 0;
    }

    if(low_latency_llfc){
         PORTMOD_ENCODING_LOW_LATENCY_LLFC_SET(*properties);
    }
    if(err_mark){
         PORTMOD_ENCODING_FEC_ERROR_DETECT_SET(*properties);
    }

exit:
    SOC_FUNC_RETURN; 
}


STATIC int
_pm8x100_fabric_loopback_serdes_overrides_set(int unit, int port, pm_info_t pm_info)
{
    int i;
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    portmod_loopback_mode_t mac_loopbacks[]= { portmodLoopbackMacAsyncFifo, portmodLoopbackMacOuter, portmodLoopbackMacPCS, portmodLoopbackPhyGloopPMD,portmodLoopbackPhyRloopPMD};
    int lb_enable = FALSE;
    uint32 fsrd_schan_id;
    uint32 phy_index, bitmap[1];
    uint32 backpressure_en=1;
    uint32 field_val[1] = {0};
    uint32 force_signal_detect_set = 0;
    SOC_INIT_FUNC_DEFS;

    for (i = 0; i < sizeof(mac_loopbacks)/sizeof(mac_loopbacks[0]); i++) {
        _SOC_IF_ERR_EXIT(pm8x100_fabric_port_loopback_get(unit, port, pm_info, mac_loopbacks[i], &lb_enable));
        if (lb_enable) {
            force_signal_detect_set = 1;
            if (mac_loopbacks[i] == portmodLoopbackMacAsyncFifo || mac_loopbacks[i] == portmodLoopbackMacPCS) {
                backpressure_en = 0;
            }
            break;
        }
    }

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));

    /*Ignore analog signals from SerDes in case of loopback*/
    fsrd_schan_id = pm_info->pm_data.pm8x100_fabric->fsrd_schan_id;

    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFSRD_SRD_CTRL, fsrd_schan_id, 0, 0, reg_val));
    _SOC_IF_ERR_EXIT(access_field_get(unit, rFSRD_SRD_CTRL_fSRD_N_FORCE_SIGNAL_DETECT, reg_val, field_val));
    SHR_BITWRITE(field_val, phy_index, force_signal_detect_set);
    _SOC_IF_ERR_EXIT(access_field_set(unit, rFSRD_SRD_CTRL_fSRD_N_FORCE_SIGNAL_DETECT, reg_val, field_val));
    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFSRD_SRD_CTRL, fsrd_schan_id, 0, 0, reg_val));

    /*back-pressure from SerDes Tx should be disabled for portmodLoopbackMacAsyncFifo and portmodLoopbackMacPCS LBs - to ignore flow control from SerDes*/
    PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_FMAL_GENERAL_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));

    _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_FMAL_GENERAL_CONFIGURATION_fFMAL_N_TX_SRD_BACKPRESSURE_EN, reg_val, &backpressure_en));
    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_FMAL_GENERAL_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int pm8x100_fabric_mac_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    uint32 field[1] = {0};
    uint32 phy_index, bitmap[1];
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));
    PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);

    switch(loopback_type){
    case portmodLoopbackMacOuter:
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_LOOPBACK_ENABLE_REGISTER, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_LOOPBACK_ENABLE_REGISTER_fLCL_LPBK_ON, reg_val, field));
        if(enable) {
            SHR_BITSET(field, lane_index_in_fmac);
        } else {
            SHR_BITCLR(field, lane_index_in_fmac);
        }
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_LOOPBACK_ENABLE_REGISTER_fLCL_LPBK_ON, reg_val, field));
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_LOOPBACK_ENABLE_REGISTER, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        break;

    case portmodLoopbackMacAsyncFifo:
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_FMAL_GENERAL_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_FMAL_GENERAL_CONFIGURATION_fFMALN_CORE_LOOPBACK, reg_val, (uint32*)&enable));
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_FMAL_GENERAL_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        break;

    case portmodLoopbackMacPCS:
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_KPCS_CONFIGURATION_fKPCS_N_RX_DSC_LOOPBACK_EN, reg_val, (uint32*)&enable));
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("should not reach here"));
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Checks per a given port if lanes are swapped.
 * Done by checking lane mapping as configured in phymod.
 * Check all the lanes of the port:
 * if lanes aren't swapped the mapping (lane->serdes) will be 1:1.
 */
STATIC
int pm8x100_fabric_port_are_lanes_swapped(int unit, int port, pm_info_t pm_info, int* lanes_are_swapped)
{
    int lane_index;
    uint32 phy_index, bitmap[1];
    phymod_lane_map_t lane_map;
    phymod_core_access_t core_access;
    int cores_num;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_core_access_get(unit, port, pm_info, 0, 1, &core_access, &cores_num, NULL));
    _SOC_IF_ERR_EXIT(phymod_core_lane_map_get(&core_access, &lane_map));

    *lanes_are_swapped = 0;

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));
    PM8X100_FABRIC_PORT_BITMAP_ITER(bitmap[0], lane_index)
    {
        /* Check if TX lane is mapped to the same RX lane. */
        if (lane_map.lane_map_tx[lane_index] != lane_map.lane_map_rx[lane_index])
        {
            *lanes_are_swapped = 1;
            SOC_EXIT;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_fabric_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    int is_phy_loopback = FALSE;
    int lb_enable = 0;
    uint32 encoding_properties;
    portmod_port_pcs_t encoding = 0;
    uint32 flags;
    int enabled;
    int lanes_are_swapped = 0;
    SOC_INIT_FUNC_DEFS;

    /*Get {MAC} or {MAC and PHY} reset state*/
    flags = (loopback_type == portmodLoopbackMacRloop)? PORTMOD_PORT_ENABLE_MAC : 0;
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_enable_get(unit, port, pm_info, flags, &enabled));

    /*Disable {MAC} or {MAC and PHY}*/
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_enable_set(unit, port, pm_info, flags, 0));

    /* loopback type validation*/
    switch(loopback_type){
    case portmodLoopbackMacAsyncFifo:
    case portmodLoopbackMacOuter:
    case portmodLoopbackMacRloop:
        break;
    case portmodLoopbackMacPCS:
        _SOC_IF_ERR_EXIT(pm8x100_fabric_port_encoding_get(unit, port, pm_info, &encoding_properties ,&encoding));
        if((encoding != PORTMOD_PCS_64B66B_FEC) && (encoding != PORTMOD_PCS_64B66B)){
            _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, ("unsupported encoding type %d for MAC PCS loopback", encoding));
        }
        break;
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD: /*slide*/
        is_phy_loopback = TRUE;
        break;
    /*PHY PCS modes and MAC core are not supported*/
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, ("unsupported loopback type %d", loopback_type));
    }

    /*check if not already defined*/
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_loopback_get(unit, port, pm_info, loopback_type, &lb_enable));
    if(enable == lb_enable){
        SOC_EXIT;
    }

    /*loopback set*/
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_are_lanes_swapped(unit, port, pm_info, &lanes_are_swapped));
    if(is_phy_loopback){
        if(enable && lanes_are_swapped)
        {
            if (loopback_type == portmodLoopbackPhyGloopPMD) /* limitation is due to lane swap between phy and mac - can be aligned by SW */
            {
                _SOC_EXIT_WITH_ERR(SOC_E_CONFIG, ("configuring phy loopback on a lane-swapped port is not supported, align lane-swap to be one-to-one or configure mac loopback"));
            } else if (loopback_type == portmodLoopbackPhyRloopPMD) /* limitation is due to physical lane swap between board and phy */
            {
                _SOC_EXIT_WITH_ERR(SOC_E_CONFIG, ("configuring remote phy loopback on a lane-swapped port is not supported, only mac loopback is supported"));
            }
        }
        _SOC_IF_ERR_EXIT(portmod_common_phy_loopback_set(unit, port, pm_info,loopback_type, enable));
    } else { /*MAC looopback*/
        if(enable && lanes_are_swapped && (loopback_type == portmodLoopbackMacOuter)) /* limitation is due to lane swap between phy and mac - can be aligned by SW */
        {
            _SOC_EXIT_WITH_ERR(SOC_E_CONFIG, ("configuring mac outer loopback on a lane-swapped port is not supported, align lane-swap to be one-to-one or configure mac async loopback"));
        }
        _SOC_IF_ERR_EXIT(pm8x100_fabric_mac_loopback_set(unit, port, pm_info, loopback_type, enable));
    }

    _SOC_IF_ERR_EXIT(_pm8x100_fabric_loopback_serdes_overrides_set(unit, port, pm_info));

    /*restore {MAC} or {MAC and PHY} */
    flags = (loopback_type == portmodLoopbackMacRloop)? PORTMOD_PORT_ENABLE_MAC : 0;
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_enable_set(unit, port, pm_info, flags, enabled));

exit:
    SOC_FUNC_RETURN;
}


int pm8x100_fabric_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    uint32 field[1] = {0};
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    uint32 phy_index, bitmap[1];
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));
    PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);

    switch(loopback_type){
    case portmodLoopbackMacAsyncFifo:
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_FMAL_GENERAL_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_FMAL_GENERAL_CONFIGURATION_fFMALN_CORE_LOOPBACK, reg_val, field));
        *enable = field[0];
        break;
    case portmodLoopbackMacOuter:
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_LOOPBACK_ENABLE_REGISTER, fmac_blk_id, 0, 0, reg_val));
        _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_LOOPBACK_ENABLE_REGISTER_fLCL_LPBK_ON, reg_val, field));
        *enable = SHR_BITGET(field, lane_index_in_fmac);
        break;
    case portmodLoopbackMacPCS:
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_KPCS_CONFIGURATION_fKPCS_N_RX_DSC_LOOPBACK_EN, reg_val, field));
        *enable = field[0];
        break;
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD: /*slide*/
        _SOC_IF_ERR_EXIT(portmod_common_phy_loopback_get(unit, port, pm_info, loopback_type, enable));
        break;
    /*PHY pcs modes and MAC core are not supported*/
    default:
        *enable = 0; /*not supported - no loopback*/
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_fabric_port_prbs_config_set(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, const phymod_prbs_t* config)
{
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    uint32 phy_index, bitmap[1];
    uint32 poly_val = 0;
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    SOC_INIT_FUNC_DEFS;

    if(mode == 0){ /*phy*/
        _SOC_IF_ERR_EXIT(portmod_common_phy_prbs_config_set(unit, port, pm_info, flags, config));
    }
    else{
        switch(config->poly){
        case phymodPrbsPoly31:
            poly_val = 1;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("unsupported polynomial for MAC PRBS %d", config->poly));
        }
        _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));
        PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
        if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_TEST_RX_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
            _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_KPCS_TEST_RX_CONFIGURATION_fKPCS_N_TST_RX_PTRN_SEL, reg_val, &poly_val));
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_TEST_RX_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        }
        if(PHYMOD_PRBS_DIRECTION_TX_GET(flags) || (flags == 0)){
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_TEST_TX_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
            _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_KPCS_TEST_TX_CONFIGURATION_fKPCS_N_TST_TX_PTRN_SEL, reg_val, &poly_val));
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_TEST_TX_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        }
    }
exit:
    SOC_FUNC_RETURN;

}

int pm8x100_fabric_port_prbs_config_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, phymod_prbs_t* config)
{
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    uint32 tx_poly;
    uint32 poly_val = 0;
    uint32 phy_index, bitmap[1];
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    SOC_INIT_FUNC_DEFS;

    if(mode == 0){ /*phy*/
        _SOC_IF_ERR_EXIT(portmod_common_phy_prbs_config_get(unit, port, pm_info, flags, config));
    }
    else{
        _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));
        PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
        if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_TEST_RX_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
            _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_KPCS_TEST_RX_CONFIGURATION_fKPCS_N_TST_RX_PTRN_SEL, reg_val, &poly_val));
        }
        if(PHYMOD_PRBS_DIRECTION_TX_GET(flags) || (flags == 0)){
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_TEST_TX_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
            _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_KPCS_TEST_TX_CONFIGURATION_fKPCS_N_TST_TX_PTRN_SEL, reg_val, &tx_poly));
            if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
                if(poly_val != tx_poly){
                    _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("rx and tx are not configured the same"));
                }
            }
            poly_val = tx_poly;
        }
        switch(poly_val){
        case 1:
            config->poly = phymodPrbsPoly31;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("unknown polynomial %u", poly_val));
        }
        /*no meaning for invert in MAC PRBS*/
        config->invert = 0;
    }

exit:
    SOC_FUNC_RETURN;

}


int pm8x100_fabric_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, int enable)
{
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    uint32 phy_index, bitmap[1];
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    SOC_INIT_FUNC_DEFS;

    if(mode == 0)
    { /*phy*/
        _SOC_IF_ERR_EXIT(portmod_common_phy_prbs_enable_set(unit, port, pm_info, flags, enable));
    }
    else
    {
        _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));
        PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
        if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_TEST_RX_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
            _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_KPCS_TEST_RX_CONFIGURATION_fKPCS_N_TST_RX_EN, reg_val, (uint32*)&enable));
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_TEST_RX_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        }
        if(PHYMOD_PRBS_DIRECTION_TX_GET(flags) || (flags == 0)){
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_TEST_TX_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
            _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_KPCS_TEST_TX_CONFIGURATION_fKPCS_N_TST_TX_EN, reg_val, (uint32*)&enable));
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_TEST_TX_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
        }
    }

exit:
    SOC_FUNC_RETURN;
}


int pm8x100_fabric_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, int* enable)
{
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    uint32 tx_enable;
    uint32 phy_index, bitmap[1];
    uint32 fmac_blk_id, enabled;
    int lane_index_in_fmac;
    SOC_INIT_FUNC_DEFS;

    if(mode == 0)
    { /*phy*/
        _SOC_IF_ERR_EXIT(portmod_common_phy_prbs_enable_get(unit, port, pm_info, flags, enable));
    }
    else
    {
        _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));
        PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
        if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_TEST_RX_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
            _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_KPCS_TEST_RX_CONFIGURATION_fKPCS_N_TST_RX_EN, reg_val, &enabled));
            *enable = enabled;
        }
        if(PHYMOD_PRBS_DIRECTION_TX_GET(flags) || (flags == 0)){
            _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_TEST_TX_CONFIGURATION, fmac_blk_id, lane_index_in_fmac, 0, reg_val));
            _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_KPCS_TEST_TX_CONFIGURATION_fKPCS_N_TST_TX_EN, reg_val, &tx_enable));
            /*validate rx and tx are configured the same*/
            if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
                if(*enable != tx_enable){
                    _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("rx and tx are not configured the same"));
                }
            }
            *enable = tx_enable;
        }
    }
exit:
    SOC_FUNC_RETURN;
}


int pm8x100_fabric_port_prbs_status_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, phymod_prbs_status_t* status)
{
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    uint32 phy_index, bitmap[1];
    uint32 fmac_blk_id;
    int lane_index_in_fmac;
    SOC_INIT_FUNC_DEFS;

    if(mode == 0)
    { /*phy*/
        _SOC_IF_ERR_EXIT(portmod_common_phy_prbs_status_get(unit, port, pm_info, flags, status));
    } 
    else
    {
        _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));
        PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_blk_id, lane_index_in_fmac);
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_KPCS_TEST_RX_STATUS, fmac_blk_id, lane_index_in_fmac, 0, reg_val));

        status->prbs_lock = (reg_val[0] >>  PM8X100_FABRIC_MAC_PRBS_LOCK_SHIFT) & PM8X100_FABRIC_MAC_PRBS_LOCK_MASK;
        status->prbs_lock_loss = 0; /*Not supported*/
        if (status->prbs_lock) {
            status->error_count = (reg_val[0] >>  PM8X100_FABRIC_MAC_PRBS_CTR_SHIFT) & PM8X100_FABRIC_MAC_PRBS_CTR_MASK;
        } else {
            status->error_count = -1;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int
pm8x100_fabric_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, const portmod_access_get_params_t* params, int max_phys,
                                    phymod_phy_access_t* phy_access_arr, int* nof_phys, int* is_most_ext)
{
    pm8x100_fabric_t fabric_data = pm_info->pm_data.pm8x100_fabric;
    uint32 phy_index, bitmap[1];
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));

    if(max_phys < 1)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("array should be at size 1 at least"));
    }
    if(params->lane != 0 && params->lane != -1)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("fabric ports are single lane only"));
    }

    if(params->phyn != 0 && params->phyn != -1)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("external phy is not supported"));
    }

    _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access_arr[0]));

    *nof_phys = 1;

    sal_memcpy(&phy_access_arr[0].access, &fabric_data->access, sizeof(fabric_data->access));
    SHR_BITSET(&phy_access_arr[0].access.lane_mask, phy_index);
    phy_access_arr[0].type = fabric_data->core_type;

    if(is_most_ext)
    {
        *is_most_ext = 1;
    }

    /*
     * phymod_access_t_init sets access.tvco_pll_index to 1
     * however peregrine5_tc_phy_speed_config_set makes sure it is 0
     * In general PMD code expects only one VCO and it is the tvco
     * tvco means this vco , ovco means other vco
     */
    phy_access_arr[0].access.tvco_pll_index = 0;
exit:
    SOC_FUNC_RETURN;
}

int
pm8x100_fabric_port_core_access_get(int unit, int port, pm_info_t pm_info, int phyn, int max_cores,
                                phymod_core_access_t* core_access_arr, int* cores_num, int* is_most_ext)
{
    pm8x100_fabric_t fabric_data = pm_info->pm_data.pm8x100_fabric;
    SOC_INIT_FUNC_DEFS;

    if(max_cores < 1)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("array should be at size 1 at least"));
    }
    if(phyn != 0 && phyn != -1)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("external phy is not supported"));
    }

    _SOC_IF_ERR_EXIT(phymod_core_access_t_init(&core_access_arr[0]));
    sal_memcpy(&core_access_arr[0].access, &fabric_data->access, sizeof(fabric_data->access));
    core_access_arr[0].type = fabric_data->core_type;
    *cores_num = 1;

    if(is_most_ext)
    {
        *is_most_ext = 1;
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_fabric_port_mode_set (int unit, soc_port_t port,
                     pm_info_t pm_info, const portmod_port_mode_info_t *mode)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, ("mode set isn't supported"));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_fabric_port_mode_get (int unit, soc_port_t port,
                     pm_info_t pm_info, portmod_port_mode_info_t *mode)
{
    SOC_INIT_FUNC_DEFS;

    mode->lanes = 1;
    mode->cur_mode = portmodPortModeSingle;

    SOC_FUNC_RETURN;
}

STATIC
void  _pm8x100_fabric_ber_proj_port_to_phy_get(soc_port_phy_ber_proj_params_t* params,
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

int pm8x100_fabric_port_diag_ctrl(int unit, soc_port_t port, pm_info_t pm_info,uint32 inst, int op_type,
    int op_cmd, const void *arg)
{
    phymod_phy_access_t phy_access[1];
    int nof_phys, i;
    portmod_access_get_params_t params;
    phymod_tx_t  ln_txparam[PHYMOD_MAX_LANES_PER_CORE];
    phymod_phy_ber_proj_options_t options;
    SOC_INIT_FUNC_DEFS;

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
            _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1,
                phy_access, &nof_phys, NULL));
            if((*phy_access).access.lane_mask){
                SOC_IF_ERROR_RETURN(phymod_phy_pmd_info_dump(phy_access, (void*)arg));
            }
            break;

       case PHY_DIAG_CTRL_BER_PROJ:
            LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_BER_PROJ 0x%x\n"), unit, port,
                 PHY_DIAG_CTRL_BER_PROJ));
            _pm8x100_fabric_ber_proj_port_to_phy_get((soc_port_phy_ber_proj_params_t*)arg, &options);
            _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1,
                phy_access, &nof_phys, NULL));
            if((*phy_access).access.lane_mask){
                SOC_IF_ERROR_RETURN(phymod_phy_ber_proj(phy_access, phymodBERProjModePostFEC, &options));
            }
            break;

       default:
            _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1,
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
    SOC_FUNC_RETURN;

}

int pm8x100_fabric_port_polarity_set(int unit, int port, pm_info_t pm_info, const phymod_polarity_t* polarity)
{
    int rv, i, rx_polarity, tx_polarity;
    SOC_INIT_FUNC_DEFS;

    for (i = 0; i < PM8X100_FABRIC_LANES_PER_CORE; i++) {
        rx_polarity = (polarity->rx_polarity >> i) & 0x1;
        tx_polarity = (polarity->tx_polarity >> i) & 0x1;

        rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_rx], &rx_polarity, i);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_tx], &tx_polarity, i);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_fabric_port_polarity_get(int unit, int port, pm_info_t pm_info, phymod_polarity_t* polarity)
{
    int rv, i, rx_polarity, tx_polarity;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(phymod_polarity_t_init(polarity));

    for (i = 0; i < PM8X100_FABRIC_LANES_PER_CORE; i++) {
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_rx], &rx_polarity, i);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_tx], &tx_polarity, i);
        _SOC_IF_ERR_EXIT(rv);

        polarity->rx_polarity |= rx_polarity << i;
        polarity->tx_polarity |= tx_polarity << i;
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int pm8x100_fabric_port_speed_config_fabric_speed_to_vco_get(int unit, const int speed, portmod_vco_type_t* vco)
{
    SOC_INIT_FUNC_DEFS;

    SOC_NULL_CHECK(vco);

    switch (speed)
    {
        case 106250:
        case 53125:
            *vco =  portmodVCO53P125G;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("speed %d not supported", speed));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int pm8x100_fabric_port_speed_config_new_to_legacy_encoding_get(int unit, int port, pm_info_t pm_info, const portmod_fec_t new_encoding, portmod_port_pcs_t* legacy_encoding)
{
    SOC_INIT_FUNC_DEFS;

    switch (new_encoding)
    {
        case PORTMOD_PORT_PHY_FEC_NONE:
            *legacy_encoding = PORTMOD_PCS_64B66B;
            break;
        case PORTMOD_PORT_PHY_FEC_RS_545:
            *legacy_encoding = PORTMOD_PCS_64B66B_15T_RS_FEC;
            break;
        case PORTMOD_PORT_PHY_FEC_RS_304:
            *legacy_encoding = PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC;
            break;
        case PORTMOD_PORT_PHY_FEC_RS_544:
            *legacy_encoding = PORTMOD_PCS_256B257B_15T_1xN_RS_FEC;
            break;
        case PORTMOD_PORT_PHY_FEC_RS_544_2XN:
            *legacy_encoding = PORTMOD_PCS_256B257B_15T_2xN_RS_FEC;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("invalid FEC %d for fabric port", new_encoding));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int pm8x100_fabric_port_speed_config_legacy_to_new_encoding_get(int unit, int port, pm_info_t pm_info, const portmod_port_pcs_t legacy_encoding, portmod_fec_t* new_encoding)
{
    SOC_INIT_FUNC_DEFS;

    switch (legacy_encoding)
    {
        case PORTMOD_PCS_64B66B:
            *new_encoding = PORTMOD_PORT_PHY_FEC_NONE;
            break;
        case PORTMOD_PCS_64B66B_15T_RS_FEC:
            *new_encoding = PORTMOD_PORT_PHY_FEC_RS_545;
            break;
        case PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC:
            *new_encoding = PORTMOD_PORT_PHY_FEC_RS_304;
            break;
        case PORTMOD_PCS_256B257B_15T_1xN_RS_FEC:
            *new_encoding = PORTMOD_PORT_PHY_FEC_RS_544;
            break;
        case PORTMOD_PCS_256B257B_15T_2xN_RS_FEC:
            *new_encoding = PORTMOD_PORT_PHY_FEC_RS_544_2XN;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("invalid FEC %d for fabric port", legacy_encoding));
    }

exit:
    SOC_FUNC_RETURN;
}

/* Validate a set of speed config within a port macro.
 * The function receives ports pbmp and requested speed to configure for each port.
 * It calculates new TVCO (PLL1 or This VCO), OVCO (PLL0 or Other VCO) based on the speeds input.
 * It returns TVCO and OVCO rates and if the rates are different from current rates.
 * This function doesn't do any hardware changes, it does only calculations
 * As PM8x100 supports only 1PLL it will return valid value only for PLL0
 */
int pm8x100_fabric_pm_speed_config_validate(int unit, int pm_id, pm_info_t pm_info, const portmod_pbmp_t* ports, int flag, portmod_pm_vco_setting_t* vco_setting)
{
    int speed_index;
    uint32 ports_count;
    portmod_vco_type_t current_vco, required_vco = portmodVCOInvalid, vco_to_set = portmodVCOInvalid;

    SOC_INIT_FUNC_DEFS;


    /*
     * VALIDATE INPUT
     * {
     */
    if (!(flag & PORTMOD_PM_SPEED_VALIDATE_F_PLL_SWITCH_DEFAULT))
    {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("flag %d is not supported. Only flag %d is supported by DNX", flag, PORTMOD_PM_SPEED_VALIDATE_F_PLL_SWITCH_DEFAULT));
    }

    PORTMOD_PBMP_COUNT(*ports, ports_count);
    if (ports_count != vco_setting->num_speeds)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("given num of ports is different from num of speeds: num of ports is %d and num of speeds is %d", ports_count, vco_setting->num_speeds));
    }

    /*
     * }
     */

    /**
     *  Get curr VCO rates and bitmaps of the PLL/s and copy them to curr_updated array
     */
    _SOC_IF_ERR_EXIT(PM8X100_FABRIC_PLL_VCO_RATE_GET(unit, pm_info, &current_vco, PM8X100_FABRIC_DEFAULT_PLL));

    /** Calculate the required VCO rates based on the speeds given by the user as input
     * and MAP the VCO to preferred PLL index if possible*/
    for (speed_index = 0; speed_index < vco_setting->num_speeds; ++speed_index)
    {

        _SOC_IF_ERR_EXIT(pm8x100_fabric_port_speed_config_fabric_speed_to_vco_get(unit, vco_setting->speed_config_list[speed_index].speed, &required_vco));
        /*
         * If VCO not yet initialized and free, configure it for the speed
         */
        if (current_vco == required_vco || current_vco == portmodVCOInvalid)
        {
            vco_to_set = required_vco;
        }
        else
        {
            /*
             * Check each VCO that it is not different than the one about to be set
             */
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Given speed configuration for PM %d requires more then 1 PLL", pm_id));
        }

    }

    /**
     * PM8x100 supports only 1 PLL and it is preffered to be tvco with already set therminolgay
     * as they stand for ThisVCO and OtherVCO
     */
    vco_setting->tvco = vco_to_set;
    vco_setting->is_tvco_new = vco_to_set != current_vco ? 1 : 0;

    vco_setting->ovco = portmodVCOInvalid ;
    vco_setting->is_ovco_new = FALSE;

exit:
    SOC_FUNC_RETURN;
}

/* This function returns required pll divider based on the ref_clk and vco */
static
int pm8x100_fabric_vco_to_pll_get(
    int unit,
    portmod_vco_type_t vco,
    uint32_t* pll)
{
    SOC_INIT_FUNC_DEFS;

    switch (vco) {
        case portmodVCO53P125G:
            *pll = phymod_TSCBH_PLL_DIV170;
            break;
        case portmodVCOInvalid:
            *pll = phymod_TSCBH_PLL_DIVNONE;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("Unsupported VCO provided: %d", vco));
    }

exit:
    SOC_FUNC_RETURN;
}

/* Reconfigure the vco rate for pm core.
 * Size of vco array is always two for legacy reasons - TVCO rate in vco[0] and OVCO rate in vco[1].
 * PM8x100 however supoports only 1 PLL so it is assumed that relevant VCO is in TVCO vco[0]
 * Function always sets the value to the PLL
 * Function nd also zeroes bitmap of each such VCO.
 */
int pm8x100_fabric_pm_vco_reconfig(int unit, int pm_id, pm_info_t pm_info, const portmod_vco_type_t* vco)
{
    uint32 pll_active_lanes_bitmap;
    pm8x100_fabric_t fabric_data = pm_info->pm_data.pm8x100_fabric;
    uint32_t pll_div = phymod_TSCBH_PLL_DIVNONE;
    portmod_vco_type_t vco_to_set;
    phymod_phy_access_t phy_access;

    SOC_INIT_FUNC_DEFS;

    /*
     * PM8x100 supports only 1 PLL
     * Assume it is set in the TVCO place
     */
    vco_to_set = vco[0];

    _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));
    sal_memcpy(&phy_access.access, &fabric_data->access, sizeof(fabric_data->access));
    phy_access.type = fabric_data->core_type;

    /* Get the PLL divider */
    _SOC_IF_ERR_EXIT(pm8x100_fabric_vco_to_pll_get(unit, vco_to_set, &pll_div));

    /* in easy reload the purpose is only to update the SW and leave HW as is.
     * skip phymod_speed_config function in easy reload because:
     * - it is not needed because it doesn't configure any SW (phymod is stateless).
     * - it gives an error about core not being in reset state. The core shouldn't be reset in easy reload.
     *
     * BH simulator had hardcoded value for 53G if we read pll value. This made easy reload tests to pass with default values
     * Peregrine doesn't have such hardcoded pll value and if we don't execute this step for Cmodel in Easy Reload test will fail.
     * As we still want to test the rest and the other Easy Reload features we implement another workaround.
     * On real device this step should be skipped as HW should keep the values intact
     */
    if (!SOC_IS_RELOADING(unit) || SAL_BOOT_SIMULATION)
    {
        _SOC_IF_ERR_EXIT(phymod_phy_pll_reconfig(&phy_access, PM8X100_FABRIC_DEFAULT_PLL, pll_div, fabric_data->ref_clk));
    }

    /*
     * Update the PM WB with the new PLL value
     */
    _SOC_IF_ERR_EXIT(PM8X100_FABRIC_PLL_VCO_RATE_SET(unit, pm_info, vco_to_set, PM8X100_FABRIC_DEFAULT_PLL));
    /*
     * Zero the active lanes bitmap as it will be populated per port in speed_config_set function
     * Currently PLL is assumed to be drving no ports as it is newly set
     */
    pll_active_lanes_bitmap = 0;
    _SOC_IF_ERR_EXIT(PM8X100_FABRIC_PLL_ACTIVE_LANES_BITMAP_SET(unit, pm_info, pll_active_lanes_bitmap, PM8X100_FABRIC_DEFAULT_PLL));

exit:
    SOC_FUNC_RETURN;
}

/* Set the speed config for the specified port, includes:
 * 1) encoding, FEC
 * 2) speed
 * 3) link_training, CL72
 * 4) lane_config - mostly firmware-related parameters
 * Configuration 1) is done in the PM and all other configurations via a single phymod API.
 * Max. 1 PLLs are supported per Peregrine core, meaning 1 VCO rates.
 * Each active port is assigned to a PLL, according to its speed.
 */
int pm8x100_fabric_port_speed_config_set(int unit, int port, pm_info_t pm_info, const portmod_speed_config_t* speed_config)
{
    uint32 encoding_properties = 0;
    portmod_port_pcs_t legacy_encoding;
    uint32 phy_index, bitmap[1];
    portmod_vco_type_t required_vco = portmodVCOInvalid;
    portmod_pmd_lane_config_t portmod_pmd_lane_config;
    phymod_phy_speed_config_t phymod_speed_config;
    uint32 pll_active_lanes_bitmap;
    portmod_vco_type_t pll_vco_rate;
    int lane_index;
    phymod_phy_pll_state_t old_pll_state, new_pll_state;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    /** 1. fill phymod_speed_config struct from received portmod_speed_config struct */
    /* resource.speed */
    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_t_init(&phymod_speed_config));
    phymod_speed_config.data_rate = speed_config->speed;

    /* resource.link_training */
    phymod_speed_config.linkTraining = speed_config->link_training;

    /* resource.lane_config */
    _SOC_IF_ERR_EXIT(portmod_pmd_lane_config_t_init(unit, &portmod_pmd_lane_config));
    _SOC_IF_ERR_EXIT(portmod_common_pmd_lane_config_decode(speed_config->lane_config, &portmod_pmd_lane_config));
    phymod_speed_config.pmd_lane_config = portmod_pmd_lane_config.pmd_firmware_lane_config;
    phymod_speed_config.PAM4_channel_loss = portmod_pmd_lane_config.pam4_channel_loss;

    PHYMOD_SPEED_CONFIG_ONLY_PLL0_IS_ACTIVE_SET(&phymod_speed_config);

    /** 2. PLLs configuration - will be changed based on the received speed_config->speed */
    _SOC_IF_ERR_EXIT(PM8X100_FABRIC_PLL_VCO_RATE_GET(unit, pm_info, &pll_vco_rate, PM8X100_FABRIC_DEFAULT_PLL));
    _SOC_IF_ERR_EXIT(PM8X100_FABRIC_PLL_ACTIVE_LANES_BITMAP_GET(unit, pm_info, &pll_active_lanes_bitmap, PM8X100_FABRIC_DEFAULT_PLL));

    /* init old pll state - the bitmaps of PLL0 without the current port. will be passed to phymod */
    _SOC_IF_ERR_EXIT(phymod_phy_pll_state_t_init(&old_pll_state));
    old_pll_state.pll0_lanes_bitmap = pll_active_lanes_bitmap;

    /*
     * Get required VCO and update the active lanes bitmap for the PLL with the new port to be added
     */
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_speed_config_fabric_speed_to_vco_get(unit, speed_config->speed, &required_vco));

    _SOC_IF_ERR_EXIT(phymod_phy_pll_state_t_init(&new_pll_state));

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));
    /* update portmod lanes bitmap of relevant PLL */
    if (pll_vco_rate == required_vco)
    {
        PM8X100_FABRIC_PORT_BITMAP_ITER(bitmap[0], lane_index)
        {
            SHR_BITSET(&pll_active_lanes_bitmap, lane_index);
        }
    }
    else
    {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("port %d: the required VCO rate %d was not configured in pm8x100_fabric_pm_vco_reconfig", port, required_vco));
    }

    /** 3. call the phymod function which configures all the above configurations */

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));

    /* in easy reload the purpose is only to update the SW and leave HW as is.
     * skip phymod_speed_config function in easy reload because:
     * - it is not needed because it doesn't configure any SW (phymod is stateless).
     * - it gives an error about core not being in reset state. The core shouldn't be reset in easy reload.
     */
    if (!SOC_IS_RELOADING(unit))
    {
        _SOC_IF_ERR_EXIT(phymod_phy_speed_config_set(&phy_access, &phymod_speed_config, &old_pll_state, &new_pll_state));
    }
    /** 4. check that we got the expected bitmaps of both PLLs from phymod - then can update SW state */

    /* in easy reload mode there is no need to validate that phymod returned correct bitmaps, because if it didn't - we would get an error before the easy reload step.
     * in this case we will update the WB engine with the bitmaps as they were calculated here */
    if ((new_pll_state.pll0_lanes_bitmap == pll_active_lanes_bitmap) || SOC_IS_RELOADING(unit))
    {
        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_pll_active_lanes_bitmap], &pll_active_lanes_bitmap, PM8X100_FABRIC_DEFAULT_PLL));
        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_pll_vco_rate], &pll_vco_rate, PM8X100_FABRIC_DEFAULT_PLL));
    }
    else
    {

        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("port %d: didn't get expected PLL bitmap from phymod actual (%x), expected (%x)",
                                              port, new_pll_state.pll0_lanes_bitmap,pll_active_lanes_bitmap));
    }

    /** 6. configure the FEC- unrelated to phymod_phy_speed_config - done in the PM.
     * RS-FEC configuration depends on the speed, so this step is done after speed configuration.
     */
    if (!PORTMOD_SPEED_CONFIG_F_SKIP_FEC_CONFIGURATION_GET(speed_config))
    {
        _SOC_IF_ERR_EXIT(pm8x100_fabric_port_speed_config_new_to_legacy_encoding_get(unit, port, pm_info, speed_config->fec, &legacy_encoding));

        /* set default fec-related configurations that go with the fec in speed_config->fec */
        if (PM8X100_FABRIC_ENCODING_IS_LOW_LATECY_RS_FEC(legacy_encoding))
        {
            PORTMOD_ENCODING_LOW_LATENCY_LLFC_SET(encoding_properties);
        }

        _SOC_IF_ERR_EXIT(pm8x100_fabric_port_encoding_set(unit, port, pm_info, encoding_properties, legacy_encoding));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_fabric_port_speed_config_get(int unit, int port, pm_info_t pm_info, portmod_speed_config_t* speed_config)
{
    portmod_port_pcs_t legacy_encoding;
    uint32 properties = 0;
    phymod_phy_speed_config_t phymod_speed_config;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys;
    portmod_pmd_lane_config_t portmod_lane_config;
    uint32 phy_index, bitmap[1];

    SOC_INIT_FUNC_DEFS;


    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_encoding_get(unit, port, pm_info, &properties, &legacy_encoding));
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_speed_config_legacy_to_new_encoding_get(unit, port, pm_info, legacy_encoding, &speed_config->fec));

    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_t_init(&phymod_speed_config));

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(phymod_phy_speed_config_get(&phy_access, &phymod_speed_config));

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));

    speed_config->speed = phymod_speed_config.data_rate;
    speed_config->link_training = phymod_speed_config.linkTraining;
    speed_config->num_lane = 1;

    portmod_lane_config.pmd_firmware_lane_config = phymod_speed_config.pmd_lane_config;
    portmod_lane_config.pam4_channel_loss = phymod_speed_config.PAM4_channel_loss;
    _SOC_IF_ERR_EXIT(portmod_common_pmd_lane_config_encode(&portmod_lane_config, (uint32*)(&speed_config->lane_config)));

exit:

    SOC_FUNC_RETURN;
}

int pm8x100_fabric_port_lane_map_set(int unit, int port, pm_info_t pm_info, uint32 flags, const phymod_lane_map_t* lane_map)
{
    uint32 mac_only;

    SOC_INIT_FUNC_DEFS;

    mac_only = PORTMOD_LANE_MAP_MAC_ONLY_GET(flags);
    if (mac_only == 0)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("port %d: pm8x100_fabric supports MAC only lane map set", port));
    }

    _SOC_IF_ERR_EXIT(pm8x100_fabric_pm_fmac_tx_lane_mapping_set(unit, port, pm_info, lane_map));
    _SOC_IF_ERR_EXIT(pm8x100_fabric_pm_fmac_rx_lane_mapping_set(unit, port, pm_info, lane_map));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_fabric_port_lane_map_get(int unit, int port, pm_info_t pm_info, uint32 flags, phymod_lane_map_t* lane_map)
{
    uint32 mac_only;
    phymod_core_access_t core_access;
    int cores_num;
    phymod_lane_map_t phy_lane_map;
    int i;
    SOC_INIT_FUNC_DEFS;

    mac_only = PORTMOD_LANE_MAP_MAC_ONLY_GET(flags);
    if (mac_only == 0)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("port %d: pm8x100_fabric supports MAC only lane map set", port));
    }

    lane_map->num_of_lanes = PM8X100_FABRIC_LANES_PER_CORE;

    _SOC_IF_ERR_EXIT(pm8x100_fabric_pm_fmac_tx_lane_mapping_get(unit, port, pm_info, lane_map));
    _SOC_IF_ERR_EXIT(pm8x100_fabric_pm_fmac_rx_lane_mapping_get(unit, port, pm_info, lane_map));

    /**
     * This function is not used during device runtime or inside APIs, but is exposed to CINT and is used in regression to verify correct lane_mapping.
     * We check if the lane mapping inside the portmod code is the same as the lane mapping inside the phy.
     */

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_core_access_get(unit, port, pm_info, 0, 1, &core_access, &cores_num, NULL));
    _SOC_IF_ERR_EXIT(phymod_core_lane_map_get(&core_access, &phy_lane_map));

    for (i = 0; i < lane_map->num_of_lanes; i++)
    {
        if ((phy_lane_map.lane_map_rx[i] != lane_map->lane_map_rx[i]) || (phy_lane_map.lane_map_tx[i] != lane_map->lane_map_tx[i]))
        {
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, ("port %d: lane map in phy does not match lane map in portmod", port));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_fabric_port_synce_clk_ctrl_set(int unit, int port, pm_info_t pm_info,
                                   const portmod_port_synce_clk_ctrl_t* cfg)
{
    pm8x100_fabric_t fabric_data = pm_info->pm_data.pm8x100_fabric;
    int fsrd_schan_id;
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 }, sdm_val;

    SOC_INIT_FUNC_DEFS;

    fsrd_schan_id = fabric_data->fsrd_schan_id;

    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFSRD_SYNC_E_SELECT, fsrd_schan_id, 0, 0, reg_val));
    sdm_val = cfg->sdm_val;
    _SOC_IF_ERR_EXIT(access_field_set(unit, rFSRD_SYNC_E_SELECT_fSYNC_E_CLK_DIV, reg_val, &sdm_val));
    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFSRD_SYNC_E_SELECT, fsrd_schan_id, 0, 0, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int pm8x100_fabric_port_synce_clk_ctrl_get(int unit, int port, pm_info_t pm_info,
                                   portmod_port_synce_clk_ctrl_t* cfg)
{
    pm8x100_fabric_t fabric_data = pm_info->pm_data.pm8x100_fabric;
    int fsrd_schan_id;
    uint32 sdm_val;
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };

    SOC_INIT_FUNC_DEFS;

    fsrd_schan_id = fabric_data->fsrd_schan_id;

    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFSRD_SYNC_E_SELECT, fsrd_schan_id, 0, 0, reg_val));
    _SOC_IF_ERR_EXIT(access_field_get(unit, rFSRD_SYNC_E_SELECT_fSYNC_E_CLK_DIV, reg_val, &sdm_val));
    cfg->sdm_val = sdm_val;

exit:
    SOC_FUNC_RETURN;
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
int pm8x100_fabric_pm_phy_lane_access_get(
    int unit, int pm_id, pm_info_t pm_info,
    const portmod_access_get_params_t* params,
    int max_phys, phymod_phy_access_t* phy_access,
    int* nof_phys, int* is_most_ext)
{
    int i;
    pm8x100_fabric_t fabric_data = pm_info->pm_data.pm8x100_fabric;
    SOC_INIT_FUNC_DEFS;

    for( i = 0 ; i < max_phys; i++) 
    {
        _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access[i]));
    }

    *nof_phys = 1;

    /* internal core */
    sal_memcpy(&phy_access[0].access, &fabric_data->access, sizeof(fabric_data->access)); 

    if (params->lane != -1) 
    {
        phy_access[0].access.lane_mask = (0x1 << (params->lane));
    } 
    else 
    {
        /* If lane index is -1, assume all the lanes are accessed */
        phy_access[0].access.lane_mask = (0x1 << PM8X100_FABRIC_MAX_PORTS_PER_PM) - 1;
    }
    /* only line is availabe for internal. */
    phy_access[0].port_loc = phymodPortLocLine;
    /* Get phymod dispatch type */
    phy_access[0].type = fabric_data->core_type;

exit:
    SOC_FUNC_RETURN;
}

/*
 * \brief - Set the FMAC TX Padding value
 *
 * \param [in] unit -     Unit number
 * \param [in] port - fabric logical port
 * \param [in] pad_size - Pad size in bytes
 * \return
 *      shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int pm8x100_fabric_port_tx_padding_set(int unit, int port, pm_info_t pm_info, int pad_size)
{
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    uint32 phy_index, bitmap[1];
    uint32 fmac_index;
    uint32 pad_size_val;
    int fmac_inner_link;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));
    PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_index, fmac_inner_link);

    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_FMAL_TX_GENERAL_CONFIGURATION, fmac_index, fmac_inner_link, 0, reg_val));
    pad_size_val = pad_size ? 1 : 0;
    _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_FMAL_TX_GENERAL_CONFIGURATION_fFMAL_TX_GENERAL_CONFIGURATION_03_FIELD_9, reg_val, &pad_size_val));
    if (pad_size)
    {
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_FMAL_TX_GENERAL_CONFIGURATION_fFMAL_TX_GENERAL_CONFIGURATION_03_FIELD_10, reg_val, (uint32*)&pad_size));
    }
    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_FMAL_TX_GENERAL_CONFIGURATION, fmac_index, fmac_inner_link, 0, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/*
 * \brief - Get the FMAC TX Padding value
 *
 * \param [in] unit -     Unit number
 * \param [in] port - fabric logical port
 * \param [out] pad_size - Pad size in bytes
 * \return
 *      shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int pm8x100_fabric_port_tx_padding_get(int unit, int port, pm_info_t pm_info, int * pad_size)
{
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    uint32 phy_index, bitmap[1];
    uint32 fmac_index, field_val, pad_size_val;
    int fmac_inner_link;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));
    PM8X100_FABRIC_LANE_FMAC_DATA_GET(pm_info, phy_index, fmac_index, fmac_inner_link);
   
    /*
     * pad_size = 0 means TX padding is disabled
     */
    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_FMAL_TX_GENERAL_CONFIGURATION, fmac_index, fmac_inner_link, 0, reg_val));
    _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_FMAL_TX_GENERAL_CONFIGURATION_fFMAL_TX_GENERAL_CONFIGURATION_03_FIELD_9, reg_val, &field_val));

    if (field_val)
    {
        _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_FMAL_TX_GENERAL_CONFIGURATION_fFMAL_TX_GENERAL_CONFIGURATION_03_FIELD_10, reg_val, &pad_size_val));
        *pad_size = pad_size_val;
    }
    else
    {
        *pad_size = 0;
    }
exit:
    SOC_FUNC_RETURN;
}


/**
 * \brief
 *   Set packets per tiks shaper.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link -
 *   The fabric link number.
 * \param [in] burst -
 *   Burst of the shaper.
 * \param [in] nof_tiks -
 *   Rate in packets per tiks. A value of zero disables rate limiting (full speed).
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
int pm8x100_fabric_port_rate_egress_ppt_set(int unit, int port, pm_info_t pm_info, uint32 burst,uint32 nof_tiks)
{
    uint32 limit_max = 0, burst_val;
    int max_length = 0;
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    int fmac_index = 0;
    uint32 phy_index, bitmap[1];

    const access_device_field_t *field_info;
    const access_device_field_t *field_info_array = PM8X100_FABRIC_NEW_ACCESS_DEVICE_TYPE_INFO(unit)->local_fields;
    access_local_field_id_t local_field_id;
 
    SOC_INIT_FUNC_DEFS;

    /*
     * Valdiate input
     */
    local_field_id = PM8X100_FABRIC_NEW_ACCESS_DEVICE_TYPE_INFO(unit)->fields_global2local_map[rFMAC_TX_CELL_LIMIT_fCELL_LIMIT_COUNT];
    field_info = field_info_array + local_field_id;
    max_length = field_info->size_in_bits;

    limit_max = 0;
    SHR_BITSET_RANGE(&limit_max, 0, max_length);
    if (limit_max < burst)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Burst %u is too big - should be < %u", burst, limit_max));
    }

    local_field_id = PM8X100_FABRIC_NEW_ACCESS_DEVICE_TYPE_INFO(unit)->fields_global2local_map[rFMAC_TX_CELL_LIMIT_fCELL_LIMIT_PERIOD];
    field_info = field_info_array + local_field_id;
    max_length = field_info->size_in_bits;

    limit_max = 0;
    SHR_BITSET_RANGE(&limit_max, 0, max_length);
    if (limit_max < nof_tiks)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Burst %u is too big relative to pps %u", burst, nof_tiks));
    }

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));
    PM8X100_FABRIC_LANE_FMAC_BLK_ID_GET(pm_info, phy_index, fmac_index);

    /** disable shaper handle */
    if (burst == 0)
    {
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE, rFMAC_TX_CELL_LIMIT, fmac_index, 0, 0, reg_val));
    }
    else if (burst < 3)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Burst %u is too small - should be > 2", burst));
    }
    else
    {
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_TX_CELL_LIMIT, fmac_index, 0, 0, reg_val));
        burst_val = burst - 2;
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_TX_CELL_LIMIT_fCELL_LIMIT_COUNT, reg_val, &burst_val));
        _SOC_IF_ERR_EXIT(access_field_set(unit, rFMAC_TX_CELL_LIMIT_fCELL_LIMIT_PERIOD, reg_val, &nof_tiks));
        _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE | FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_TX_CELL_LIMIT, fmac_index, 0, 0, reg_val));
    }

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief
 *   Get packets per tiks shaper.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link -
 *   The fabric link number.
 * \param [out] burst -
 *   Burst of the shaper.
 * \param [out] nof_tiks -
 *   Rate in packets per tiks. A value of zero denotes shaper is disabled.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
int pm8x100_fabric_port_rate_egress_ppt_get(int unit, int port, pm_info_t pm_info, uint32 * burst,uint32 * nof_tiks)
{
    uint32 reg_val[SBUS_MAX_NOF_DATA_UINT32S] = { 0 };
    int fmac_index = 0;
    uint32 phy_index, bitmap[1];
    uint32 cell_limit_count_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm8x100_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, bitmap));
    PM8X100_FABRIC_LANE_FMAC_BLK_ID_GET(pm_info, phy_index, fmac_index);

    _SOC_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_INST_TYPE_SBUS_ID, rFMAC_TX_CELL_LIMIT, fmac_index, 0, 0, reg_val));
    _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_TX_CELL_LIMIT_fCELL_LIMIT_PERIOD, reg_val, nof_tiks));
    if (*nof_tiks == 0)
    {
        *burst = 0;
    }
    else
    {
        /** +2 for shaper accuracy */
        _SOC_IF_ERR_EXIT(access_field_get(unit, rFMAC_TX_CELL_LIMIT_fCELL_LIMIT_COUNT, reg_val, &cell_limit_count_val));
        *burst = cell_limit_count_val + 2;
    }

exit:
    SOC_FUNC_RETURN;
}


#endif /* PORTMOD_PM8X100_FABRIC_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
