/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_PORT
#include <shared/bsl.h>
#ifdef PORTMOD_SUPPORT

#include <soc/dnxc/error.h>
#include <soc/dnxc/dnxc_port.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/dnxc_cmic.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnxc/dnxc_verify.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <sal/core/sync.h>
#include <soc/phyreg.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/common/link.h>
#include <math.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_fabric_access.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_soc.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/fabric/fabric.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/port/port.h>
#endif

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>
#include <soc/dnxf/cmn/dnxf_port.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_lane_map_db.h>
#endif

#include <soc/phy/phymod_port_control.h>
#include <phymod/phymod_acc.h>
#include <phymod/phymod.h>

#include <soc/phy/phymod_sim.h>

#include <soc/cmicx_miim.h>

typedef struct dnxc_port_user_access_s
{
    int unit;
    int fsrd_blk_id;
    sal_mutex_t mutex;
} dnxc_port_user_access_t;

dnxc_port_user_access_t *g_port_user_access[SOC_MAX_NUM_DEVICES] = { NULL };

typedef struct dnxc_port_fabric_link_fmac_info_s
{
    int port;
    uint32 fmac_blk_id;
    uint32 fmac_inner_link;
} dnxc_port_fabric_link_fmac_info_t;

#define DNXC_PHYMOD_IF_ERR_RETURN(A) \
    do {   \
        int loc_err ; \
        if ((loc_err = (A)) != PHYMOD_E_NONE) \
        {   if (A > PHYMOD_E_NONE) {\
                return _SHR_E_FAIL;\
            } else {\
                return loc_err;\
            }\
        }\
    } while (0)

#define DNXC_PHYMOD_LOCK_TAKE(t) do{\
        if((t->access.bus->mutex_give != NULL) && (t->access.bus->mutex_take != NULL)){\
            DNXC_PHYMOD_IF_ERR_RETURN(t->access.bus->mutex_take(t->access.user_acc));\
        }\
    }while(0)

#define DNXC_PHYMOD_LOCK_GIVE(t) do{\
        if((t->access.bus->mutex_give != NULL) && (t->access.bus->mutex_take != NULL)){\
            DNXC_PHYMOD_IF_ERR_RETURN(t->access.bus->mutex_give(t->access.user_acc));\
        }\
    }while(0)

#define DNXC_PORT_BER_PROJ_ERRCNT_SIZE      8
#define DNXC_PORT_BER_PROJ_INTEGER_PART_GET(_proj_ber)       ((_proj_ber & 0xFF000000) >> 24)
#define DNXC_PORT_BER_PROJ_DECIMAL_PART_GET(_proj_ber)       ((_proj_ber & 0x00FF0000) >> 16)
#define DNXC_PORT_BER_PROJ_EXPONENT_PART_GET(_proj_ber)      (_proj_ber & 0x0000FFFF)

#define DNXC_PORT_FABRIC_ENCODING_IS_RS_FEC(encoding) \
        (encoding == PORTMOD_PCS_64B66B_RS_FEC || encoding == PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC \
        || encoding == PORTMOD_PCS_64B66B_15T_RS_FEC || encoding == PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC \
        || encoding == PORTMOD_PCS_256B257B_15T_1xN_RS_FEC || encoding == PORTMOD_PCS_256B257B_15T_2xN_RS_FEC)

#define DNXC_PORT_TX_TAP_UNSIGNED_GET(tx_tap) \
    (tx_tap) & 0x000000FF

#define DNXC_PORT_RESOURCE_LANE_CONFIG_CLEAR_ALL(lane_config) \
    (lane_config = 0)

#define DNXC_PORT_RS_FEC_SYNC_MACHINE_AM_PERIOD    (4096)
#define DNXC_PORT_RS_FEC_SYNC_MACHINE_COMPARE_CNT  (2)
#define DNXC_PORT_RS_FEC_SYNC_MACHINE_NIBBLE_CNT   (9)

#define DNXC_PORT_FEC_SYNC_MACHINE_N1 (10)
#define DNXC_PORT_FEC_SYNC_MACHINE_N2 (26)
#define DNXC_PORT_FEC_SYNC_MACHINE_N3 (100)
#define DNXC_PORT_FEC_SYNC_MACHINE_K1 (1)
#define DNXC_PORT_FEC_SYNC_MACHINE_K2 (2)
#define DNXC_PORT_FEC_SYNC_MACHINE_K3 (2)

static int
cl45_bus_read(
    void *user_acc,
    uint32_t core_addr,
    uint32_t reg_addr,
    uint32_t * val)
{

    dnxc_port_user_access_t *cl45_user_data;
    uint16 val16;
    int rv = 0;

    if (user_acc == NULL)
    {
        return _SHR_E_PARAM;
    }
    cl45_user_data = user_acc;

    rv = soc_cmicx_miim_operation(cl45_user_data->unit, 0, 45, core_addr, reg_addr, &val16);
    (*val) = val16;

    return rv;

}

static int
cl45_bus_write(
    void *user_acc,
    uint32_t core_addr,
    uint32_t reg_addr,
    uint32_t val)
{

    uint16 val16;
    dnxc_port_user_access_t *cl45_user_data;

    if (user_acc == NULL)
    {
        return _SHR_E_PARAM;
    }
    cl45_user_data = user_acc;

    val16 = (uint16) val;
    return soc_cmicx_miim_operation(cl45_user_data->unit, 1, 45, core_addr, reg_addr, &val16);

}

static int
mdio_bus_mutex_take(
    void *user_acc)
{
    dnxc_port_user_access_t *user_data;

    if (user_acc == NULL)
    {
        return _SHR_E_PARAM;
    }
    user_data = (dnxc_port_user_access_t *) user_acc;

    return sal_mutex_take(user_data->mutex, sal_mutex_FOREVER);
}

static int
mdio_bus_mutex_give(
    void *user_acc)
{
    dnxc_port_user_access_t *user_data;

    if (user_acc == NULL)
    {
        return _SHR_E_PARAM;
    }
    user_data = (dnxc_port_user_access_t *) user_acc;

    return sal_mutex_give(user_data->mutex);
}

static int
soc_dnxc_port_bus_write_disabled(
    void *user_acc,
    uint32_t * val)
{
#if defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT)
    dnxc_port_user_access_t *user_data;
    if (user_acc == NULL)
    {
        return _SHR_E_PARAM;
    }

    user_data = (dnxc_port_user_access_t *) user_acc;
    *val = 0;
    if (SOC_IS_RELOADING(user_data->unit))
    {
        *val = 1;
    }

#else
    *val = 0;
#endif

    return _SHR_E_NONE;
}

static phymod_bus_t cl45_bus = {
    "dnxc_cl45_with_mutex",
    cl45_bus_read,
    cl45_bus_write,
    soc_dnxc_port_bus_write_disabled,
    mdio_bus_mutex_take,
    mdio_bus_mutex_give,
    NULL,
    NULL,
    0
};

static phymod_bus_t cl45_no_mutex_bus = {
    "dnxc_cl45",
    cl45_bus_read,
    cl45_bus_write,
    soc_dnxc_port_bus_write_disabled,
    NULL,
    NULL,
    NULL,
    NULL,
    0
};

static shr_error_e
soc_dnxc_phy_access_get(
    int unit,
    int core,
    dnxc_port_user_access_t * acc_data,
    uint16 addr,
    phymod_access_t * access,
    int *is_sim)
{
    phymod_dispatch_type_t phymod_type;
    int lpi_access_supported = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(acc_data, _SHR_E_PARAM, "acc_data");
    SHR_NULL_CHECK(access, _SHR_E_PARAM, "access");
    phymod_access_t_init(access);

    PHYMOD_ACC_USER_ACC(access) = acc_data;
    PHYMOD_ACC_ADDR(access) = addr;

    soc_physim_enable_get(unit, is_sim);

    if (*is_sim)
    {

#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX2(unit))
        {
            phymod_type
                = (1 == dnx_data_fabric.general.pm_properties_get(unit, core)->nof_plls)
                ? phymodDispatchTypeBlackhawk7_v1l8p1 : phymodDispatchTypeBlackhawk;
        }
        else
#endif
        {
            phymod_type = phymodDispatchTypeBlackhawk;
        }

        SHR_IF_ERR_EXIT(soc_physim_check_sim(unit, phymod_type, access, 0, is_sim));
        PHYMOD_ACC_F_PHYSIM_SET(access);
    }
    else
    {
        if (lpi_access_supported)
        {

            PHYMOD_ACC_BUS(access) = NULL;
        }
        else
        {

            if (acc_data->mutex != NULL)
            {
                PHYMOD_ACC_BUS(access) = &cl45_bus;
            }
            else
            {
                PHYMOD_ACC_BUS(access) = &cl45_no_mutex_bus;
            }
            PHYMOD_ACC_F_CLAUSE45_SET(access);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_soc_to_phymod_ref_clk(
    int unit,
    int ref_clk,
    phymod_ref_clk_t * phymod_ref_clk)
{
    SHR_FUNC_INIT_VARS(unit);

    *phymod_ref_clk = phymodRefClkCount;
    switch (ref_clk)
    {
        case DNXC_SERDES_REF_CLOCK_DISABLE:
            break;
        case DNXC_SERDES_REF_CLOCK_125:
            *phymod_ref_clk = phymodRefClk125Mhz;
            break;
        case DNXC_SERDES_REF_CLOCK_156_25:
            *phymod_ref_clk = phymodRefClk156Mhz;
            break;
        case DNXC_SERDES_REF_CLOCK_312_5:
            *phymod_ref_clk = phymodRefClk312Mhz;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ref clk %d\n", ref_clk);
    }
exit:
    SHR_FUNC_EXIT;
}

#define UC_TABLE_ENTRY_SIZE (4)

int
soc_dnxc_fabric_broadcast_firmware_loader(
    int unit,
    soc_pbmp_t * pbmp)
{
    soc_reg_above_64_val_t wr_data;
    int i = 0;
    int word_index = 0;
    int nof_phys;
    soc_port_t port;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    phymod_firmware_load_info_t phymod_firmware_load_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    SHR_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));

    _SHR_PBMP_ITER(*pbmp, port)
    {
        SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &phy_access, &nof_phys, NULL));
        break;
    }

    SHR_IF_ERR_EXIT(phymod_phy_firmware_load_info_get(&phy_access, &phymod_firmware_load_info));

    for (i = 0; i < phymod_firmware_load_info.ucode_len; i += UC_TABLE_ENTRY_SIZE)
    {
        SOC_REG_ABOVE_64_CLEAR(wr_data);
        if (i + UC_TABLE_ENTRY_SIZE < phymod_firmware_load_info.ucode_len)
        {
            sal_memcpy((uint8 *) wr_data, phymod_firmware_load_info.ucode_ptr + i, UC_TABLE_ENTRY_SIZE);
        }
        else
        {
            sal_memcpy((uint8 *) wr_data, phymod_firmware_load_info.ucode_ptr + i,
                       phymod_firmware_load_info.ucode_len - i);
        }

        for (word_index = 0; word_index < sizeof(soc_reg_above_64_val_t) / 4; word_index++)
        {
            wr_data[word_index] = _shr_uint32_read((uint8 *) &wr_data[word_index]);
        }

        {
            SHR_IF_ERR_EXIT(WRITE_BRDC_FSRD_FSRD_PROM_MEMm(unit, MEM_BLOCK_ALL, 0, wr_data));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static int
soc_dnxc_fabric_firmware_loader_callback(
    const phymod_core_access_t * core,
    uint32_t length,
    const uint8_t * data)
{
    dnxc_port_user_access_t *user_data;
    soc_reg_above_64_val_t wr_data;
    int i = 0;
    int word_index = 0;

    user_data = core->access.user_acc;

    for (i = 0; i < length; i += UC_TABLE_ENTRY_SIZE)
    {
        SOC_REG_ABOVE_64_CLEAR(wr_data);
        if (i + UC_TABLE_ENTRY_SIZE < length)
        {
            sal_memcpy((uint8 *) wr_data, data + i, UC_TABLE_ENTRY_SIZE);
        }
        else
        {
            sal_memcpy((uint8 *) wr_data, data + i, length - i);
        }

        for (word_index = 0; word_index < sizeof(soc_reg_above_64_val_t) / 4; word_index++)
        {
            wr_data[word_index] = _shr_uint32_read((uint8 *) &wr_data[word_index]);
        }

        PHYMOD_IF_ERR_RETURN(WRITE_FSRD_FSRD_PROM_MEMm(user_data->unit, user_data->fsrd_blk_id, 0, wr_data));
    }

    return PHYMOD_E_NONE;
}

#ifdef BCM_DNX_SUPPORT

shr_error_e
soc_dnxc_single_pll_restriction_disable_get(
    int unit,
    int *single_pll_restriction_disable)
{
    soc_reg_above_64_val_t data;
    int otp_bit = 135;
    int otp_nof_bits = 2;
    uint32 value = 0;

    SHR_FUNC_INIT_VARS(unit);

    *single_pll_restriction_disable =
        dnx_data_port.general.feature_get(unit, dnx_data_port_general_unrestricted_portmod_pll);

    if (!*single_pll_restriction_disable)
    {
        SHR_IF_ERR_EXIT(READ_ECI_RESERVED_51r(unit, data));

        SHR_BITCOPY_RANGE(&value, 0, data, otp_bit, otp_nof_bits);
        *single_pll_restriction_disable = (value == 0x1 ? TRUE : FALSE);
    }
exit:
    SHR_FUNC_EXIT;
}
#endif

static shr_error_e
soc_dnxc_fabric_single_pm8x50_fabric_info_fill(
    int unit,
    int core,
    int cores_num,
    int phy_offset,
    uint32 bypass_lanes_bitmap,
    dnxc_core_address_get_f address_get_func,
    portmod_pm_create_info_t * pm_info)
{
    int phy = -1, pll0_only = 0;
    int core_mac_index = 0;
    int mac_core = -1;
    int fmac_block_id = -1;
    int fsrd_block_id = -1;
    uint16 addr = 0;
    int is_sim = 0;
    int core_port_index = 0;
    uint32 rx_polarity = 0;
    uint32 tx_polarity = 0;
    int bw_update = 0;
    int clock_buffer_disable_required = 0;
    portmod_pm8x50_fabric_create_info_t *pm8x50_fabric_ptr = NULL;
    DNXC_SERDES_REF_CLOCK_TYPE ref_clk_dnxc = DNXC_SERDES_REF_CLOCK_NOF;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, pm_info));

    pm_info->type = portmodDispatchTypePm8x50_fabric;
    pm8x50_fabric_ptr = &pm_info->pm_specific_info.pm8x50_fabric;
    pm8x50_fabric_ptr->core_index = core;
    pm8x50_fabric_ptr->first_phy_offset = 0;

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        pm8x50_fabric_ptr->fw_load_method = dnx_data_port.static_add.fabric_fw_load_method_get(unit);
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        pm8x50_fabric_ptr->fw_load_method = dnxf_data_port.general.fw_load_method_get(unit);
#endif
    }

    if (pm8x50_fabric_ptr->fw_load_method == phymodFirmwareLoadMethodExternal)
    {
        pm8x50_fabric_ptr->external_fw_loader = soc_dnxc_fabric_firmware_loader_callback;
    }

    phy = phy_offset + core * DNXC_PORT_FABRIC_LANES_PER_CORE;

    for (core_mac_index = 0; core_mac_index < PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC; ++core_mac_index)
    {
        mac_core = core * PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC + core_mac_index;
        fmac_block_id = FMAC_BLOCK(unit, mac_core);
        pm8x50_fabric_ptr->fmac_schan_id[core_mac_index] = SOC_BLOCK_INFO(unit, fmac_block_id).schan;
    }
    fsrd_block_id = FSRD_BLOCK(unit, core);
    g_port_user_access[unit][core].fsrd_blk_id = fsrd_block_id;
    pm8x50_fabric_ptr->fsrd_schan_id = SOC_BLOCK_INFO(unit, fsrd_block_id).schan;

    SHR_IF_ERR_EXIT(address_get_func(unit, core, &addr));
    SHR_IF_ERR_EXIT(soc_dnxc_phy_access_get
                    (unit, core, &g_port_user_access[unit][core], addr, &pm8x50_fabric_ptr->access, &is_sim));

    if (is_sim)
    {
        pm8x50_fabric_ptr->fw_load_method = phymodFirmwareLoadMethodNone;
    }

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        int pll_index = -1;
        int nof_fabric_plls = dnx_data_pll.general.fabric_pll_info_get(unit)->key_size[0];

        pll_index =
            (core * DNXC_PORT_FABRIC_LANES_PER_CORE) / (dnx_data_fabric.links.nof_links_get(unit) / nof_fabric_plls);
        ref_clk_dnxc = dnx_data_pll.general.fabric_pll_get(unit, pll_index)->out_freq;
        if (ref_clk_dnxc == DNXC_SERDES_REF_CLOCK_BYPASS)
        {
            ref_clk_dnxc = dnx_data_pll.general.fabric_pll_get(unit, pll_index)->in_freq;
        }
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        ref_clk_dnxc = SOC_INFO(unit).port_refclk_int[phy];
#endif
    }
    SHR_IF_ERR_EXIT(dnxc_soc_to_phymod_ref_clk(unit, ref_clk_dnxc, &pm8x50_fabric_ptr->ref_clk));

    PORTMOD_PBMP_CLEAR(pm_info->phys);
    for (core_port_index = 0; core_port_index < DNXC_PORT_FABRIC_LANES_PER_CORE; core_port_index++)
    {
        if (!SOC_WARM_BOOT(unit))
        {
            if (SOC_IS_DNX(unit))
            {
#ifdef BCM_DNX_SUPPORT
                uint32 link = phy - dnx_data_port.general.fabric_phys_offset_get(unit);

                rx_polarity = dnx_data_fabric.links.polarity_get(unit, link)->rx_polarity;
                tx_polarity = dnx_data_fabric.links.polarity_get(unit, link)->tx_polarity;
#endif
            }
            else
            {
#ifdef BCM_DNXF_SUPPORT
                rx_polarity = dnxf_data_port.static_add.port_info_get(unit, phy)->rx_polarity;
                tx_polarity = dnxf_data_port.static_add.port_info_get(unit, phy)->tx_polarity;
#endif
            }
            pm8x50_fabric_ptr->polarity.rx_polarity |= ((rx_polarity & 0x1) << core_port_index);
            pm8x50_fabric_ptr->polarity.tx_polarity |= ((tx_polarity & 0x1) << core_port_index);

        }

        PORTMOD_PBMP_PORT_ADD(pm_info->phys, phy);
        phy++;
    }

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        pll0_only = dnx_data_fabric.general.force_single_pll_get(unit, core)->enable;
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        pll0_only = dnxf_data_port.pll.force_single_pll_get(unit, core)->enable;
    }
#endif

    if (pll0_only)
    {
        pm8x50_fabric_ptr->force_single_pll0 = 1;
    }
    else
    {
        pm8x50_fabric_ptr->force_single_pll0 = (SOC_IS_RAMON_A0(unit)) ? 1 : 0;
    }

    pm8x50_fabric_ptr->force_single_pll1 = 0;

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        pm8x50_fabric_ptr->native_single_pll
            = (1 == dnx_data_fabric.general.pm_properties_get(unit, core)->nof_plls) ? 1 : 0;
    }
    else
#endif
    {
        pm8x50_fabric_ptr->native_single_pll = 0;
    }
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        int single_pll_restriction_disable;
        SHR_IF_ERR_EXIT(soc_dnxc_single_pll_restriction_disable_get(unit, &single_pll_restriction_disable));

        if (single_pll_restriction_disable == FALSE
            && dnx_data_fabric.general.pm_properties_get(unit, core)->special_pll_check)
        {
            pm8x50_fabric_ptr->force_single_pll1 = 1;
        }
    }
#endif

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        bw_update = dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_bw_update);
        clock_buffer_disable_required =
            dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_clk_repeater_disable);
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        bw_update = dnxf_data_port.general.feature_get(unit, dnxf_data_port_general_bw_update);
        clock_buffer_disable_required = TRUE;
    }
#endif
    if (bw_update)
    {
        pm8x50_fabric_ptr->clock_buffer_bw_update_required = TRUE;
    }

    pm8x50_fabric_ptr->clock_buffer_disable_required = clock_buffer_disable_required;
    pm8x50_fabric_ptr->bypass_lanes_bitmap[0] = bypass_lanes_bitmap;

    pm8x50_fabric_ptr->is_rx_ctrl_bypass_supported = SOC_IS_DNX(unit) ? 0 : 1;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_fabric_single_pm_add(
    int unit,
    int core,
    int cores_num,
    int phy_offset,
    uint32 bypass_lanes_bitmap,
    int use_mutex,
    dnxc_core_address_get_f address_get_func)
{
    portmod_pm_create_info_t pm_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));

    if (g_port_user_access[unit] == NULL)
    {
        SHR_ALLOC_SET_ZERO(g_port_user_access[unit], sizeof(dnxc_port_user_access_t) * cores_num, "g_port_user_access",
                           "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    }

    g_port_user_access[unit][core].unit = unit;
    g_port_user_access[unit][core].mutex = (use_mutex) ? sal_mutex_create("core mutex") : NULL;

    if (soc_is(unit, DNX2_DEVICE) || soc_is(unit, DNX_RAMON))
    {
        SHR_IF_ERR_EXIT
            (soc_dnxc_fabric_single_pm8x50_fabric_info_fill
             (unit, core, cores_num, phy_offset, bypass_lanes_bitmap, address_get_func, &pm_info));
    }
    else
    {
    }

    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));
    SHR_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));
exit:
    if (_func_rv != _SHR_E_NONE)
    {
        soc_dnxc_fabric_pms_destroy(unit, cores_num);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_fabric_pms_add(
    int unit,
    int cores_num,
    int phy_offset,
    uint32 bypass_lanes_bitmap,
    int use_mutex,
    dnxc_core_address_get_f address_get_func)
{
    int core = 0;
    SHR_FUNC_INIT_VARS(unit);

    for (core = 0; core < cores_num; core++)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_fabric_single_pm_add
                        (unit, core, cores_num, phy_offset, bypass_lanes_bitmap, use_mutex, address_get_func));
    }

exit:
    SHR_FUNC_EXIT;
}

void
soc_dnxc_fabric_pms_destroy(
    int unit,
    int cores_num)
{
    int core = 0;
    SHR_FUNC_INIT_VARS(unit);
    if (g_port_user_access[unit] != NULL)
    {
        for (core = 0; core < cores_num; core++)
        {
            if (g_port_user_access[unit][core].mutex != NULL)
            {
                sal_mutex_destroy(g_port_user_access[unit][core].mutex);
            }
        }
        SHR_FREE(g_port_user_access[unit]);
    }

    SHR_VOID_FUNC_EXIT;
}

shr_error_e
soc_dnxc_fabric_port_probe(
    int unit,
    int port,
    dnxc_port_init_stage_t init_stage,
    soc_dnxc_port_firmware_config_t * fw_config,
    dnxc_port_fabric_init_config_t * port_config)
{
    portmod_port_add_info_t info;
    int phy = 0;
    int i;
    SHR_FUNC_INIT_VARS(unit);
    if (!SOC_WARM_BOOT(unit))
    {

        SHR_IF_ERR_EXIT(portmod_port_add_info_t_init(unit, &info));

        if (soc_feature(unit, soc_feature_logical_port_num))
        {
            phy = SOC_INFO(unit).port_l2p_mapping[port];
        }
        else if (SOC_IS_DNXF(unit))
        {
#ifdef BCM_DNXF_SUPPORT
            SHR_IF_ERR_EXIT(soc_dnxf_lane_map_db_link_to_fmac_lane_get(unit, port, &phy));
#endif
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid fabric port to phy mapping");
        }

        PORTMOD_PBMP_PORT_ADD(info.phys, phy);
        info.interface_config.interface = SOC_PORT_IF_SFI;

        for (i = 0; i < DNXC_PORT_FABRIC_LANES_PER_CORE; ++i)
        {
            info.init_config.lane_map[0].lane_map_rx[i] = port_config->lane2serdes[i].rx_id;
            info.init_config.lane_map[0].lane_map_tx[i] = port_config->lane2serdes[i].tx_id;
        }
        info.init_config.lane_map[0].num_of_lanes = DNXC_PORT_FABRIC_LANES_PER_CORE;
        if (init_stage == dnxc_port_init_pass1)
        {
            PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_SET(&info);
            PORTMOD_PORT_ADD_F_INIT_PASS1_SET(&info);
        }

        if (init_stage == dnxc_port_init_pass2)
        {
            PORTMOD_PORT_ADD_F_INIT_PASS2_SET(&info);
        }

        if (fw_config->crc_check)
        {
            PORTMOD_PORT_ADD_F_BYPASS_FW_CRC_CHECK_CLR(&info);
        }
        else
        {
            PORTMOD_PORT_ADD_F_BYPASS_FW_CRC_CHECK_SET(&info);
        }

        if (fw_config->load_verify)
        {
            PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_SET(&info);
        }
        else
        {
            PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_CLR(&info);
        }

        SHR_IF_ERR_EXIT(portmod_port_add(unit, port, &info));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_cl72_set(
    int unit,
    soc_port_t port,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_cl72_set(unit, port, (enable ? 1 : 0)));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_cl72_get(
    int unit,
    soc_port_t port,
    int *enable)
{
    uint32 local_enable;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_cl72_get(unit, port, &local_enable));
    *enable = (local_enable ? 1 : 0);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_port_resource_to_lane_rate_get(
    int unit,
    const bcm_port_resource_t * resource,
    int *lane_rate)
{
#ifdef BCM_DNX_SUPPORT
    dnx_algo_port_info_s port_info;
    int num_lane = 0;
#endif
    SHR_FUNC_INIT_VARS(unit);
    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, resource->port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, STIF, L1, FLEXE, MGMT, OTN_PHY)))
        {

            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, resource->port, &num_lane));

            if (num_lane == 0)
            {
                SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
            }
            *lane_rate =
                DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1))
                || DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info) ? resource->speed : (resource->speed / num_lane);
        }
        else
        {

            *lane_rate = resource->speed;
        }
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        *lane_rate = resource->speed;
#endif
    }

    SOC_EXIT;
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_port_portmod_polarity_db_set(
    int unit,
    soc_port_t port,
    int is_rx,
    uint32 value)
{
    phymod_polarity_t polarity;
    int core_port_index;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_polarity_get(unit, port, &polarity));
    core_port_index = port % DNXC_PORT_FABRIC_LANES_PER_CORE;
    if (is_rx)
    {
        SHR_BITWRITE(&polarity.rx_polarity, core_port_index, value);
    }
    else
    {
        SHR_BITWRITE(&polarity.tx_polarity, core_port_index, value);
    }
    SHR_IF_ERR_EXIT(portmod_port_polarity_set(unit, port, &polarity));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_port_phy_control_is_supported_validate(
    int unit,
    soc_port_t port,
    soc_phy_control_t type)
{
    int is_phy_rx_taps_supported = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        is_phy_rx_taps_supported =
            dnx_data_port.general.feature_get(unit, dnx_data_port_general_is_phy_rx_taps_supported);
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        is_phy_rx_taps_supported =
            dnxf_data_port.general.feature_get(unit, dnxf_data_port_general_is_phy_rx_taps_supported);
#endif
    }

    switch (type)
    {

        case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        case SOC_PHY_CONTROL_RX_HIGH_FREQ_PEAK_FILTER:
        case SOC_PHY_CONTROL_RX_VGA:
            break;
        case SOC_PHY_CONTROL_RX_TAP1:
        case SOC_PHY_CONTROL_RX_TAP2:
        case SOC_PHY_CONTROL_RX_TAP3:
        case SOC_PHY_CONTROL_RX_TAP4:
        case SOC_PHY_CONTROL_RX_TAP5:
        case SOC_PHY_CONTROL_RX_TAP6:
            if (!is_phy_rx_taps_supported)
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This control is not supported.");
            }
            break;
        case SOC_PHY_CONTROL_RX_ADAPTATION_RESUME:
        case SOC_PHY_CONTROL_PHASE_INTERP:
        case SOC_PHY_CONTROL_RX_POLARITY:
        case SOC_PHY_CONTROL_TX_POLARITY:
        case SOC_PHY_CONTROL_DUMP:
        case SOC_PHY_CONTROL_CL72_STATUS:
        case SOC_PHY_CONTROL_TX_PATTERN_LENGTH:
        case SOC_PHY_CONTROL_TX_PATTERN_GEN_ENABLE:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA0:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA1:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA2:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA3:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA4:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA5:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA6:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA7:
        case SOC_PHY_CONTROL_TX_PPM_ADJUST:
        case SOC_PHY_CONTROL_RX_SIGNAL_DETECT:
        case SOC_PHY_CONTROL_RX_SEQ_DONE:
        case SOC_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE:
        case SOC_PHY_CONTROL_LP_TX_PRECODER_ENABLE:
        case SOC_PHY_CONTROL_RX_LANE_SQUELCH:
        case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        case SOC_PHY_CONTROL_LINK_TRAINING_INIT_TX_FIR_POST:
        case SOC_PHY_CONTROL_AUTONEG_FEC_OVERRIDE:
        case SOC_PHY_CONTROL_PMD_DIAG_DEBUG_LANE_EVENT_LOG_LEVEL:
        case SOC_PHY_CONTROL_FLEXE_50G_NOFEC_20K_AM_SPACING_ENABLE:
        case SOC_PHY_CONTROL_FEC_BYPASS_INDICATION_ENABLE:
        case SOC_PHY_CONTROL_FEC_CORRUPTION_PERIOD:
        case SOC_PHY_CONTROL_TX_RESET:
            break;
        case SOC_PHY_CONTROL_FIRMWARE_RX_FORCE_EXTENDED_REACH_ENABLE:
        case SOC_PHY_CONTROL_FIRMWARE_RX_FORCE_NORMAL_REACH_ENABLE:
            break;
        case SOC_PHY_CONTROL_FW_AN_FORCE_EXTENDED_REACH_ENABLE:
        case SOC_PHY_CONTROL_FW_AN_FORCE_NORMAL_REACH_ENABLE:
        case SOC_PHY_CONTROL_AN_LINK_FAIL_INHIBIT_TIMER_LT_PAM4:
            if (IS_SFI_PORT(unit, port) || IS_IL_PORT(unit, port))
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This control is not supported.");
            }
            break;

        case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        case SOC_PHY_CONTROL_PRBS_RX_INVERT_DATA:
        case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        case SOC_PHY_CONTROL_PRBS_RX_STATUS:

            if (IS_SFI_PORT(unit, port))
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                             "BCM_PORT_PHY_CONTROL_PRBS_ controls are no longer supported. To enable PRBS please refer to PRBS section in UM.");
            }
            break;

        case SOC_PHY_CONTROL_SCRAMBLER:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "SOC_PHY_CONTROL_SCRAMBLER is no longer supported, to control scrambler please use bcm_port_resource_set/get\n");
            break;
        case SOC_PHY_CONTROL_CL72:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_CL72 is no longer supported, to control link training please use bcm_port_resource_set/get\n");
            break;
        case SOC_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE is no longer supported, to control BR DFE please use bcm_port_resource_set/get\n");
            break;
        case SOC_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE is no longer supported, to control LP DFE please use bcm_port_resource_set/get\n");
            break;
        case SOC_PHY_CONTROL_FIRMWARE_DFE_ENABLE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_FIRMWARE_DFE_ENABLE is no longer supported, to control DFE please use bcm_port_resource_set/get\n");
            break;
        case SOC_PHY_CONTROL_FIRMWARE_CL72_RESTART_TIMEOUT_ENABLE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_FIRMWARE_CL72_RESTART_TIMEOUT_ENABLE is no longer supported, to control CL72 restart timeout please use bcm_port_resource_set/get\n");
            break;
        case SOC_PHY_CONTROL_FIRMWARE_CL72_AUTO_POLARITY_ENABLE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_FIRMWARE_CL72_AUTO_POLARITY_ENABLE is no longer supported, to control CL72 auto polarity please use bcm_port_resource_set/get\n");
            break;
        case SOC_PHY_CONTROL_UNRELIABLE_LOS:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_UNRELIABLE_LOS is no longer supported, to control unreliable los please use bcm_port_resource_set/get\n");
            break;
        case SOC_PHY_CONTROL_MEDIUM_TYPE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_CONTROL_MEDIUM_TYPE is no longer supported, bcm_port_resource_set/get can be used to set the medium\n");
            break;
        case SOC_PHY_CONTROL_FIRMWARE_MODE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_FIRMWARE_MODE is no longer supported, bcm_port_resource_set/get can be used to set firmware configurations\n");
            break;
        case SOC_PHY_CONTROL_INTERFACE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_INTERFACE is no longer supported, to align interface properties please use bcm_port_resource_set/get\n");
            break;
        case SOC_PHY_CONTROL_PREEMPHASIS:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_PREEMPHASIS is no longer supported, please use bcm_port_phy_tx_set/get APIs instead\n");
            break;
        case SOC_PHY_CONTROL_TX_FIR_PRE:
        case SOC_PHY_CONTROL_TX_FIR_PRE2:
        case SOC_PHY_CONTROL_TX_FIR_MAIN:
        case SOC_PHY_CONTROL_TX_FIR_POST:
        case SOC_PHY_CONTROL_TX_FIR_POST2:
        case SOC_PHY_CONTROL_TX_FIR_POST3:
        case SOC_PHY_CONTROL_TX_FIR_MODE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_TX_FIR controls are no longer supported, please use bcm_port_phy_tx_set/get APIs instead\n");
            break;
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_PREEMPHASIS_LANE<> is no longer supported, please use bcm_port_phy_tx_set/get APIs instead\n");
            break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_DRIVER_CURRENT_LANE<> is not supported\n");
            break;
        case SOC_PHY_CONTROL_RX_VGA_RELEASE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_RX_VGA_RELEASE is no longer supported. Please use BCM_PORT_PHY_CONTROL_RX_ADAPTATION_RESUME instead\n");
            break;
        case SOC_PHY_CONTROL_RX_TAP1_RELEASE:
        case SOC_PHY_CONTROL_RX_TAP2_RELEASE:
        case SOC_PHY_CONTROL_RX_TAP3_RELEASE:
        case SOC_PHY_CONTROL_RX_TAP4_RELEASE:
        case SOC_PHY_CONTROL_RX_TAP5_RELEASE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_RX_TAP<>_RELEASE controls are no longer supported. Please use BCM_PORT_PHY_CONTROL_RX_ADAPTATION_RESUME instead\n");
            break;
        case SOC_PHY_CONTROL_RX_RESET:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "Reset RX only using BCM_PORT_PHY_CONTROL_RX is no longer supported. Please use bcm_port_enable_set/get to enable/disable both RX and TX\n");
            break;
        case SOC_PHY_CONTROL_POWER:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_POWER is no longer supported. Please use bcm_port_enable_set/get instead\n");
            break;
        case SOC_PHY_CONTROL_LANE_SWAP:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_LANE_SWAP is no longer supported, please use bcm_port_lane_to_serdes_map_set/get instead\n");
            break;
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_PRBS_DECOUPLED_ controls are no longer supported. To enable PRBS please refer to PRBS section in User Manual\n");
            break;
        case SOC_PHY_CONTROL_LOOPBACK_INTERNAL:
        case SOC_PHY_CONTROL_LOOPBACK_PMD:
        case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_LOOPBACK_ controls are no longer supported. For loopback configuration please use bcm_port_loopback_set/get API\n");
            break;
        case SOC_PHY_CONTROL_RX_SEQ_TOGGLE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_PORT_PHY_CONTROL_RX_SEQ_TOGGLE is no longer supported. To reset the RX path, use BCM_PORT_PHY_CONTROL_RX_LANE_SQUELCH\n");
            break;
        case SOC_PHY_CONTROL_GPIO_CONFIG:
        case SOC_PHY_CONTROL_GPIO_VALUE:
        case SOC_PHY_CONTROL_INTR_MASK:
        case SOC_PHY_CONTROL_INTR_STATUS:
        case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        case SOC_PHY_CONTROL_HG2_BCM_CODEC_ENABLE:
        case SOC_PHY_CONTROL_SHORT_CHANNEL_MODE:
        case SOC_PHY_CONTROL_SHORT_CHANNEL_MODE_STATUS:
        case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
        case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91:
        case SOC_PHY_CONTROL_AUTONEG_MODE:
        case SOC_PHY_CONTROL_TX_FIR_DRIVERMODE:
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This control is not supported\n");
            break;
        case SOC_PHY_CONTROL_EEE:
            if (IS_SFI_PORT(unit, port))
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This control is not supported\n");
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                             "BCM_PORT_PHY_CONTROL_EEE is not required to enable EEE. Please refer to EEE documentation for correct sequence\n");
            }
            break;

        case SOC_PHY_CONTROL_EYE_VAL_HZ_L:
        case SOC_PHY_CONTROL_EYE_VAL_HZ_R:
        case SOC_PHY_CONTROL_EYE_VAL_VT_U:
        case SOC_PHY_CONTROL_EYE_VAL_VT_D:
        {
            int lane_speed;
            portmod_speed_config_t speed_config;
            SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &speed_config));
            if (IS_SFI_PORT(unit, port) || IS_IL_PORT(unit, port))
            {
                lane_speed = speed_config.speed;
            }
            else
            {
                lane_speed = speed_config.speed / speed_config.num_lane;
            }
            if (DNXC_PORT_PHY_SPEED_IS_PAM4(lane_speed))
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                             "This control is not supported for PAM speed, please use BER projection tool instead.\n");
            }
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This control is not supported\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_port_phy_control_set_to_disable_port_get(
    int unit,
    soc_port_t port,
    soc_phy_control_t type,
    int *to_disable)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case SOC_PHY_CONTROL_DUMP:
        case SOC_PHY_CONTROL_TX_PATTERN_LENGTH:
        case SOC_PHY_CONTROL_TX_PATTERN_GEN_ENABLE:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA0:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA1:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA2:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA3:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA4:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA5:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA6:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA7:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
        case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        case SOC_PHY_CONTROL_RX_HIGH_FREQ_PEAK_FILTER:
        case SOC_PHY_CONTROL_RX_VGA:
        case SOC_PHY_CONTROL_RX_TAP1:
        case SOC_PHY_CONTROL_RX_TAP2:
        case SOC_PHY_CONTROL_RX_TAP3:
        case SOC_PHY_CONTROL_RX_TAP4:
        case SOC_PHY_CONTROL_RX_TAP5:
        case SOC_PHY_CONTROL_RX_ADAPTATION_RESUME:
        case SOC_PHY_CONTROL_RX_LANE_SQUELCH:
        case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        case SOC_PHY_CONTROL_FLEXE_50G_NOFEC_20K_AM_SPACING_ENABLE:
            *to_disable = 0;
            break;
        default:
            *to_disable = 1;
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_phy_control_set(
    int unit,
    soc_port_t port,
    int phyn,
    int lane,
    int is_sys_side,
    soc_phy_control_t type,
    uint32 value)
{
    phymod_phy_access_t phys[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    portmod_access_get_params_t params;
    int is_lane_control;
    phymod_ref_clk_t ref_clk_phy = phymodRefClk156Mhz;
    DNXC_SERDES_REF_CLOCK_TYPE ref_clk_dnxc = DNXC_SERDES_REF_CLOCK_NOF;
    int is_legacy_ext_phy = 0;
    int orig_enabled = 0;
    int to_disable = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_control_is_supported_validate(unit, port, type));
    SHR_IF_ERR_EXIT(soc_dnxc_port_enable_get(unit, port, 0, &orig_enabled));

    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_control_set_to_disable_port_get(unit, port, type, &to_disable));
    if (orig_enabled && to_disable)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_enable_set(unit, port, 0, 0));
    }

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_ref_clk_get(unit, port, &ref_clk_dnxc));
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        ref_clk_dnxc = SOC_INFO(unit).port_refclk_int[port];
#endif
    }

    SHR_IF_ERR_EXIT(dnxc_soc_to_phymod_ref_clk(unit, ref_clk_dnxc, &ref_clk_phy));

    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = lane;
    params.phyn = (phyn == -1 ? PORTMOD_PHYN_LAST_ONE : phyn);
    params.sys_side = is_sys_side ? PORTMOD_SIDE_SYSTEM : PORTMOD_SIDE_LINE;
    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get
                    (unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));
    switch (type)
    {

        case SOC_PHY_CONTROL_LANE_SWAP:
        case SOC_PHY_CONTROL_FEC_BYPASS_INDICATION_ENABLE:
        case SOC_PHY_CONTROL_FEC_CORRUPTION_PERIOD:
            is_lane_control = 0;
            break;
        default:
            is_lane_control = 1;
    }

    if (params.phyn != 0)
    {
        SHR_IF_ERR_EXIT(portmod_port_is_legacy_ext_phy_present(unit, port, &is_legacy_ext_phy));
    }

    if (!is_legacy_ext_phy)
    {
        SHR_IF_ERR_EXIT(soc_port_control_set_wrapper
                        (unit, ref_clk_phy, is_lane_control, phys, phys_returned, type, value));
    }
    else
    {
        SHR_IF_ERR_EXIT(portmod_port_ext_phy_control_set(unit, port, phyn, lane, is_sys_side, type, value));
    }

    if (IS_SFI_PORT(unit, port))
    {
        if (type == SOC_PHY_CONTROL_RX_POLARITY || type == SOC_PHY_CONTROL_TX_POLARITY)
        {
            int is_rx = (type == SOC_PHY_CONTROL_RX_POLARITY) ? 1 : 0;
            SHR_IF_ERR_EXIT(soc_dnxc_port_portmod_polarity_db_set(unit, port, is_rx, value));
        }
    }

    if (orig_enabled && to_disable)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_enable_set(unit, port, 0, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_phy_control_get(
    int unit,
    soc_port_t port,
    int phyn,
    int lane,
    int is_sys_side,
    soc_phy_control_t type,
    uint32 *value)
{
    phymod_phy_access_t phys[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    portmod_access_get_params_t params;
    int is_lane_control;
    phymod_ref_clk_t ref_clk_phy = phymodRefClkCount;
    DNXC_SERDES_REF_CLOCK_TYPE ref_clk_dnxc = DNXC_SERDES_REF_CLOCK_NOF;
    int is_legacy_ext_phy = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_control_is_supported_validate(unit, port, type));
    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_ref_clk_get(unit, port, &ref_clk_dnxc));
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        ref_clk_dnxc = SOC_INFO(unit).port_refclk_int[port];
#endif
    }
    SHR_IF_ERR_EXIT(dnxc_soc_to_phymod_ref_clk(unit, ref_clk_dnxc, &ref_clk_phy));

    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = lane;
    params.phyn = (phyn == -1 ? PORTMOD_PHYN_LAST_ONE : phyn);
    params.sys_side = is_sys_side ? PORTMOD_SIDE_SYSTEM : PORTMOD_SIDE_LINE;
    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get
                    (unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));
    switch (type)
    {

        case SOC_PHY_CONTROL_LANE_SWAP:
        case SOC_PHY_CONTROL_FEC_BYPASS_INDICATION_ENABLE:
        case SOC_PHY_CONTROL_FEC_CORRUPTION_PERIOD:
            is_lane_control = 0;
            break;
        default:
            is_lane_control = 1;
    }

    if (params.phyn != 0)
    {
        SHR_IF_ERR_EXIT(portmod_port_is_legacy_ext_phy_present(unit, port, &is_legacy_ext_phy));
    }

    if (!is_legacy_ext_phy)
    {
        SHR_IF_ERR_EXIT(soc_port_control_get_wrapper
                        (unit, ref_clk_phy, is_lane_control, phys, phys_returned, type, value));
        if (type == SOC_PHY_CONTROL_TX_FIR_MAIN)
        {
            *value = DNXC_PORT_TX_TAP_UNSIGNED_GET(*value);
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(portmod_port_ext_phy_control_get(unit, port, phyn, lane, is_sys_side, type, value));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_resource_validate(
    int unit,
    const bcm_port_resource_t * resource)
{
    uint32 pam4_channel_loss;
    int lane_rate = 0;
    SHR_FUNC_INIT_VARS(unit);
    if ((resource->physical_port != 0) || (resource->lanes != 0) || (resource->encap != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "port %d: flags, physical_port, lanes and encap fields of resource are not in use. They should all be initialized to 0\n",
                     resource->port);
    }

    if ((resource->speed == BCM_PORT_RESOURCE_DEFAULT_REQUEST) ||
        (resource->fec_type == BCM_PORT_RESOURCE_DEFAULT_REQUEST) ||
        (resource->link_training == BCM_PORT_RESOURCE_DEFAULT_REQUEST) ||
        (resource->phy_lane_config == BCM_PORT_RESOURCE_DEFAULT_REQUEST))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "port %d: speed, fec_type, link_training and phy_lane_config fields of resource are mandatory as input to this API.\n Call bcm_port_resource_default_get to fill them\n",
                     resource->port);
    }

    if (PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_GET(resource->phy_lane_config))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: bit 14 of phy_lane_config is internal. Do not set it\n", resource->port);
    }

    if (PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_GET(resource->phy_lane_config))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: bit 15 of phy_lane_config is internal. Do not set it\n", resource->port);
    }

    if ((resource->link_training == 1) && !BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_GET(resource->phy_lane_config))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: DFE is mandatory for link training\n", resource->port);
    }

    if (BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_GET(resource->phy_lane_config)
        && !BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_GET(resource->phy_lane_config))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: when LP_DFE is on, DFE must be on as well\n", resource->port);
    }

    if (BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_GET(resource->phy_lane_config)
        && BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(resource->phy_lane_config))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "port %d: setting both FORCE_NS and FORCE_ES is not a valid option. Only one can be set\n",
                     resource->port);
    }

    SHR_IF_ERR_EXIT(soc_dnxc_port_resource_to_lane_rate_get(unit, resource, &lane_rate));
    if (DNXC_PORT_PHY_SPEED_IS_PAM4(lane_rate) && (resource->link_training == 0) &&
        !BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_GET(resource->phy_lane_config)
        && !BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(resource->phy_lane_config))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "port %d: if mode is PAM4 and link training is disabled, either FORCE_NS or FORCE_ES has to be set. They are both 0\n",
                     resource->port);
    }

    if (!DNXC_PORT_PHY_SPEED_IS_PAM4(lane_rate) &&
        (BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_GET(resource->phy_lane_config)
         || BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(resource->phy_lane_config)))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "port %d: if mode is NRZ, both FORCE_NS and FORCE_ES have to be 0. FORCE_NS is %d, FORCE_ES is %d\n",
                     resource->port, BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_GET(resource->phy_lane_config),
                     BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(resource->phy_lane_config));
    }

    if (!DNXC_PORT_PHY_SPEED_IS_PAM4(lane_rate))
    {
        pam4_channel_loss = PORTMOD_PORT_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_GET(resource->phy_lane_config);
        if (pam4_channel_loss)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "port %d: pam4 channel loss configuration - bits [16:21] of phy_lane_config - is expected to be 0 when mode is nrz. speed is %d, pam4 channel loss is %u\n",
                         resource->port, resource->speed, pam4_channel_loss);
        }
    }

    if (resource->link_training == 1)
    {
        pam4_channel_loss = PORTMOD_PORT_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_GET(resource->phy_lane_config);
        if (pam4_channel_loss)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "port %d: pam4 channel loss configuration - bits [16:21] of phy_lane_config - is expected to be 0 when link training is enabled. pam4 channel loss is %u\n",
                         resource->port, pam4_channel_loss);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_resource_get(
    int unit,
    bcm_gport_t port,
    bcm_port_resource_t * resource)
{
    portmod_speed_config_t port_speed_config;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");
    SHR_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &port_speed_config));
    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &port_speed_config));
    PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_CLEAR(port_speed_config.lane_config);
    PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_CLEAR(port_speed_config.lane_config);

    PORTMOD_PORT_PHY_LANE_CONFIG_LP_PREC_EN_CLEAR(port_speed_config.lane_config);
    bcm_port_resource_t_init(resource);
    resource->speed = port_speed_config.speed;
    resource->fec_type = (bcm_port_phy_fec_t) port_speed_config.fec;
    resource->link_training = port_speed_config.link_training;
    resource->phy_lane_config = port_speed_config.lane_config;
    resource->port = port;
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_port_fabric_resource_fec_default_get(
    int unit,
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);
    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        resource->fec_type = dnx_data_fabric.links.default_fec_get(unit, resource->speed)->fec_type;
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        resource->fec_type = dnxf_data_port.general.default_fec_get(unit, resource->speed)->fec_type;
#endif
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_port_fabric_resource_lane_config_default_get(
    int unit,
    bcm_gport_t port,
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);
    DNXC_PORT_RESOURCE_LANE_CONFIG_CLEAR_ALL(resource->phy_lane_config);
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(resource->phy_lane_config);
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(resource->phy_lane_config);
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(resource->phy_lane_config,
                                                 BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE);
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_CLEAR(resource->phy_lane_config);
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_SCRAMBLING_DISABLE_CLEAR(resource->phy_lane_config);
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_CLEAR(resource->phy_lane_config);
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_SET(resource->phy_lane_config);

    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_CLEAR(resource->phy_lane_config);
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_CLEAR(resource->phy_lane_config);
    if (DNXC_PORT_PHY_SPEED_IS_PAM4(resource->speed) && (resource->link_training == 0))
    {
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_SET(resource->phy_lane_config);
    }

    PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_CLEAR(resource->phy_lane_config);
    PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_CLEAR(resource->phy_lane_config);

    PORTMOD_PORT_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_SET(resource->phy_lane_config, 0);
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_fabric_resource_default_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");
    if ((resource->flags != 0) || (resource->physical_port != 0) || (resource->lanes != 0) || (resource->encap != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "port %d: flags, physical_port, lanes and encap fields of bcm_port_resource_t are not in use. They should all be initialized to 0\n",
                     port);
    }

    if ((resource->port != 0) && (resource->port != port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "port %d: resource->port should be 0 or equal to port passed to the API\n", port);
    }

    if (resource->speed == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "port %d: there is no given default value for speed. speed is mandatory as input to this API\n",
                     port);
    }

    if (resource->fec_type == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_fabric_resource_fec_default_get(unit, resource));
    }

    if (resource->link_training == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->link_training = 1;
    }

    if (resource->phy_lane_config == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_fabric_resource_lane_config_default_get(unit, port, resource));
    }

    resource->port = port;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_lane_rate_get(
    int unit,
    bcm_port_t port,
    int *lane_rate)
{
    bcm_port_resource_t resource;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_dnxc_port_resource_get(unit, port, &resource));
    SHR_IF_ERR_EXIT(soc_dnxc_port_resource_to_lane_rate_get(unit, &resource, lane_rate));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_port_signal_quality_encoded_val_to_man_and_exp_get(
    int unit,
    uint32 encoded_val,
    uint16 *man,
    int16 *exp)
{
    uint32 man_extracted, exp_extracted;
    const int SOC_DNXC_PORT_SIGNAL_QUALITY_ER_EXPONENT_OFFSET = 0;
    const int SOC_DNXC_PORT_SIGNAL_QUALITY_ER_MANTISSA_OFFSET = 16;
    const int SOC_DNXC_PORT_SIGNAL_QUALITY_ER_EXPONENT_BITS_NOF = 16;
    const int SOC_DNXC_PORT_SIGNAL_QUALITY_ER_MANTISSA_BITS_NOF = 16;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                    (&encoded_val, SOC_DNXC_PORT_SIGNAL_QUALITY_ER_EXPONENT_OFFSET,
                     SOC_DNXC_PORT_SIGNAL_QUALITY_ER_EXPONENT_BITS_NOF, &exp_extracted));
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                    (&encoded_val, SOC_DNXC_PORT_SIGNAL_QUALITY_ER_MANTISSA_OFFSET,
                     SOC_DNXC_PORT_SIGNAL_QUALITY_ER_MANTISSA_BITS_NOF, &man_extracted));

    *exp = (int16) exp_extracted;
    *man = (uint16) man_extracted;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_signal_quality_encoded_val_to_float_get(
    int unit,
    uint32 encoded_val,
    float *val)
{
    uint16 man = 0;
    int16 exp = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_port_signal_quality_encoded_val_to_man_and_exp_get(unit, encoded_val, &man, &exp));

    soc_dnxc_port_signal_quality_man_and_exp_to_float_get(man, exp, val);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_signal_quality_fabric_feature_verify(
    int unit)
{
    int is_fabric_signal_quality_supported = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        is_fabric_signal_quality_supported =
            dnx_data_port.general.feature_get(unit, dnx_data_port_general_is_fabric_signal_quality_supported);
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        is_fabric_signal_quality_supported =
            dnxf_data_port.general.feature_get(unit, dnxf_data_port_general_is_signal_quality_supported);
#endif
    }

    if (!is_fabric_signal_quality_supported)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Fabric Signal quality not supported on this device");
    }

exit:
    SHR_FUNC_EXIT;
}

void
soc_dnxc_port_signal_quality_man_and_exp_to_float_get(
    uint16 man,
    int16 exp,
    float *val)
{
    *val = pow(10, exp);
    *val *= man;
}

shr_error_e
soc_dnxc_port_signal_quality_fabric_encoded_thr_to_hw_thr_get(
    int unit,
    bcm_port_t port,
    int interval,
    uint32 encoded_val,
    float *val)
{
    int lane_rate = 0;
    float thr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_port_lane_rate_get(unit, port, &lane_rate));

    SHR_IF_ERR_EXIT(soc_dnxc_port_signal_quality_encoded_val_to_float_get(unit, encoded_val, &thr));

    *val = lane_rate * interval * thr;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_phy_tx_taps_default_get(
    int unit,
    bcm_port_t port,
    int is_pam4,
    bcm_port_phy_tx_t * tx)
{
    phymod_phy_access_t phys[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    portmod_access_get_params_t params;
    int phy_index;
    phymod_tx_t phymod_tx;
    phymod_phy_signalling_method_t signalling_mode;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(tx, _SHR_E_PARAM, "tx");
    signalling_mode = is_pam4 ? phymodSignallingMethodPAM4 : phymodSignallingMethodNRZ;
    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get
                    (unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));
    SHR_IF_ERR_EXIT(phymod_tx_t_init(&phymod_tx));
    bcm_port_phy_tx_t_init(tx);
    for (phy_index = 0; phy_index < phys_returned; phy_index++)
    {

        if (phys[phy_index].access.lane_mask)
        {
            SHR_IF_ERR_EXIT(phymod_phy_tx_taps_default_get(&phys[phy_index], signalling_mode, &phymod_tx));
            phymod_tx.sig_method = signalling_mode;
            break;
        }
    }

    tx->pre3 = phymod_tx.pre3;
    tx->pre2 = phymod_tx.pre2;
    tx->pre = phymod_tx.pre;
    tx->main = phymod_tx.main;
    tx->post = phymod_tx.post;
    tx->post2 = phymod_tx.post2;
    tx->post3 = phymod_tx.post3;
    tx->tx_tap_mode = (phymod_tx.tap_mode == phymodTxTapMode3Tap) ? bcmPortPhyTxTapMode3Tap : bcmPortPhyTxTapMode6Tap;
    tx->signalling_mode =
        (phymod_tx.sig_method ==
         phymodSignallingMethodNRZ) ? bcmPortPhySignallingModeNRZ : bcmPortPhySignallingModePAM4;
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_port_phy_tx_is_pre3_supported(
    int unit,
    bcm_port_t gport,
    int *is_supported)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_supported = 0;

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        int real_port;
        dnx_algo_gpm_gport_phy_info_t gport_info;
        portmod_dispatch_type_t pm_type;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

        SHR_IF_ERR_EXIT(portmod_port_pm_type_get(unit, gport_info.local_port, &real_port, &pm_type));

        *is_supported = dnx_data_port.general.pm_info_get(unit, pm_type)->is_serdes_tx_pre3_supported;
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        *is_supported =
            dnxf_data_port.static_add.feature_get(unit,
                                                  dnxf_data_port_static_add_serdes_tx_taps_pre3_supported_instead_post3);
    }
#endif

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_port_phy_tx_is_serdes_tx_soc_prop_dict_format_supported(
    int unit,
    int *is_supported)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_supported = 0;

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        *is_supported = dnx_data_port.general.feature_get(unit,
                                                          dnx_data_port_general_is_serdes_tx_soc_prop_dict_format_supported);
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        *is_supported = dnxf_data_port.general.feature_get(unit,
                                                           dnxf_data_port_general_is_serdes_tx_soc_prop_dict_format_supported);
    }
#endif

    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_phy_tx_serdex_tx_tap_pre3_verify(
    int unit,
    bcm_port_t port,
    bcm_port_phy_tx_t * tx)
{
    int is_pre3_supported = 0;
    int is_dict_format_supported = 0;
    int is_init;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_tx_is_serdes_tx_soc_prop_dict_format_supported(unit, &is_dict_format_supported));

    is_init = !(dnxc_init_is_init_done_get(unit));
    if (!is_init || is_dict_format_supported)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_phy_tx_is_pre3_supported(unit, port, &is_pre3_supported));

        if (is_pre3_supported && tx->post3 != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "port %d: TX FIR parameter post3 is not supported but is given a value of %d", port,
                         tx->post3);
        }
        else if (!is_pre3_supported && tx->pre3 != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "port %d: TX FIR parameter pre3 is not supported but is given a value of %d", port, tx->pre3);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_phy_tx_set(
    int unit,
    bcm_port_t port,
    int phyn,
    int lane,
    int is_sys_side,
    bcm_port_phy_tx_t * tx)
{
    phymod_tx_t phymod_tx;
    phymod_phy_access_t phys[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    portmod_access_get_params_t params;
    int phy_index;
    int is_pre3_supported;
    int is_init;
    int is_dict_format_supported = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tx, _SHR_E_PARAM, "tx");

    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = lane;
    params.phyn = (phyn == -1 ? PORTMOD_PHYN_LAST_ONE : phyn);
    params.sys_side = is_sys_side ? PORTMOD_SIDE_SYSTEM : PORTMOD_SIDE_LINE;
    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get
                    (unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));

    SHR_IF_ERR_EXIT(phymod_tx_t_init(&phymod_tx));
    phymod_tx.pre3 = tx->pre3;
    phymod_tx.pre2 = tx->pre2;
    phymod_tx.pre = tx->pre;
    phymod_tx.main = tx->main;
    phymod_tx.post = tx->post;
    phymod_tx.post2 = tx->post2;
    phymod_tx.post3 = tx->post3;
    phymod_tx.tap_mode = (tx->tx_tap_mode == bcmPortPhyTxTapMode3Tap) ? phymodTxTapMode3Tap : phymodTxTapMode6Tap;
    phymod_tx.sig_method =
        (tx->signalling_mode == bcmPortPhySignallingModeNRZ) ? phymodSignallingMethodNRZ : phymodSignallingMethodPAM4;

    is_init = !(dnxc_init_is_init_done_get(unit));

    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_tx_is_serdes_tx_soc_prop_dict_format_supported(unit, &is_dict_format_supported));

    if (is_init && !is_dict_format_supported)
    {

        SHR_IF_ERR_EXIT(soc_dnxc_port_phy_tx_is_pre3_supported(unit, port, &is_pre3_supported));
        if (is_pre3_supported)
        {
            phymod_tx.pre3 = tx->post3;
            phymod_tx.post3 = 0;
        }
        else
        {
            phymod_tx.pre3 = 0;
            phymod_tx.post3 = tx->post3;
        }
    }

    for (phy_index = 0; phy_index < phys_returned; phy_index++)
    {

        if (phys[phy_index].access.lane_mask)
        {
            SHR_IF_ERR_EXIT(phymod_phy_tx_set(&phys[phy_index], &phymod_tx));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_phy_tx_get(
    int unit,
    bcm_port_t port,
    int phyn,
    int lane,
    int is_sys_side,
    bcm_port_phy_tx_t * tx)
{
    phymod_tx_t phymod_tx;
    phymod_phy_access_t phys[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    portmod_access_get_params_t params;
    int phy_index;
    int is_pre3_supported;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(tx, _SHR_E_PARAM, "tx");
    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = lane;
    params.phyn = (phyn == -1 ? PORTMOD_PHYN_LAST_ONE : phyn);
    params.sys_side = is_sys_side ? PORTMOD_SIDE_SYSTEM : PORTMOD_SIDE_LINE;
    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get
                    (unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));
    bcm_port_phy_tx_t_init(tx);
    for (phy_index = 0; phy_index < phys_returned; phy_index++)
    {

        if (phys[phy_index].access.lane_mask)
        {
            SHR_IF_ERR_EXIT(phymod_phy_tx_get(&phys[phy_index], &phymod_tx));
            tx->pre2 = phymod_tx.pre2;
            tx->pre = phymod_tx.pre;
            tx->main = phymod_tx.main & 0xFF;
            tx->post = phymod_tx.post;
            tx->post2 = phymod_tx.post2;

            SHR_IF_ERR_EXIT(soc_dnxc_port_phy_tx_is_pre3_supported(unit, port, &is_pre3_supported));
            if (is_pre3_supported)
            {
                tx->pre3 = phymod_tx.pre3;
                tx->post3 = 0;
            }
            else
            {
                tx->pre3 = 0;
                tx->post3 = phymod_tx.post3;
            }

            tx->tx_tap_mode =
                (phymod_tx.tap_mode == phymodTxTapMode3Tap) ? bcmPortPhyTxTapMode3Tap : bcmPortPhyTxTapMode6Tap;
            tx->signalling_mode =
                (phymod_tx.sig_method ==
                 phymodSignallingMethodNRZ) ? bcmPortPhySignallingModeNRZ : bcmPortPhySignallingModePAM4;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_phy_reg_get(
    int unit,
    soc_port_t port,
    uint32 flags,
    uint32 phy_reg_addr,
    uint32 *phy_data)
{
    uint16 phy_rd_data = 0;
    uint32 reg_flag, phy_id, phy_reg;
    int rv;
    int nof_phys_structs = 0;
    phymod_phy_access_t phy_access[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    portmod_access_get_params_t params;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    if (flags & SOC_PHY_INTERNAL)
    {
        params.phyn = 0;
    }

    if ((flags & SOC_PHY_NOMAP) == 0)
    {
        rv = portmod_port_phy_lane_access_get(unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phy_access,
                                              &nof_phys_structs, NULL);
        SHR_IF_ERR_EXIT(rv);
    }

    if (flags & (SOC_PHY_I2C_DATA8 | SOC_PHY_I2C_DATA16))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported flags for ramon\n");
    }

    reg_flag = SOC_PHY_REG_FLAGS(phy_reg_addr);
    if (reg_flag & SOC_PHY_REG_INDIRECT)
    {
        if (flags & BCM_PORT_PHY_NOMAP)
        {

            SHR_ERR_EXIT(_SHR_E_PARAM, "indirect register access is not supported if SOC_PHY_NOMAP flag is set\n");
        }
        phy_reg_addr &= ~SOC_PHY_REG_INDIRECT;
        rv = phymod_phy_reg_read(phy_access, phy_reg_addr, phy_data);
        SHR_IF_ERR_EXIT(rv);
    }
    else
    {
        if (flags & SOC_PHY_NOMAP)
        {
            phy_id = port;
            phy_reg = phy_reg_addr;
            rv = soc_cmicx_miim_operation(unit, 0, 45, phy_id, phy_reg, &phy_rd_data);
            SHR_IF_ERR_EXIT(rv);
            *phy_data = phy_rd_data;
        }
        else
        {
            DNXC_PHYMOD_LOCK_TAKE(phy_access);
            rv = PHYMOD_BUS_READ(&phy_access[0].access, phy_reg_addr, phy_data);
            DNXC_PHYMOD_LOCK_GIVE(phy_access);
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_phy_reg_set(
    int unit,
    soc_port_t port,
    uint32 flags,
    uint32 phy_reg_addr,
    uint32 phy_data)
{
    uint16 phy_wr_data;
    uint32 reg_flag, phy_id, phy_reg;
    int rv;
    int nof_phys_structs = 0;
    phymod_phy_access_t phy_access[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    portmod_access_get_params_t params;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    if (flags & SOC_PHY_INTERNAL)
    {
        params.phyn = 0;
    }

    if ((flags & SOC_PHY_NOMAP) == 0)
    {
        rv = portmod_port_phy_lane_access_get(unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phy_access,
                                              &nof_phys_structs, NULL);
        SHR_IF_ERR_EXIT(rv);
    }

    if (flags & (SOC_PHY_I2C_DATA8 | SOC_PHY_I2C_DATA16))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported flags for ramon\n");
    }

    reg_flag = SOC_PHY_REG_FLAGS(phy_reg_addr);
    if (reg_flag & SOC_PHY_REG_INDIRECT)
    {
        if (flags & BCM_PORT_PHY_NOMAP)
        {

            SHR_ERR_EXIT(_SHR_E_PARAM, "indirect register access is not supported if SOC_PHY_NOMAP flag is set\n");
        }
        phy_reg_addr &= ~SOC_PHY_REG_INDIRECT;
        rv = phymod_phy_reg_write(phy_access, phy_reg_addr, phy_data);
        SHR_IF_ERR_EXIT(rv);
    }
    else
    {
        if (flags & BCM_PORT_PHY_NOMAP)
        {
            phy_id = port;
            phy_reg = phy_reg_addr;
            phy_wr_data = phy_data & 0xffff;
            rv = soc_cmicx_miim_operation(unit, 1, 45, phy_id, phy_reg, &phy_wr_data);
            SHR_IF_ERR_EXIT(rv);
        }
        else
        {
            DNXC_PHYMOD_LOCK_TAKE(phy_access);
            rv = PHYMOD_BUS_WRITE(&phy_access[0].access, phy_reg_addr, phy_data);
            DNXC_PHYMOD_LOCK_GIVE(phy_access);
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_phy_reg_modify(
    int unit,
    soc_port_t port,
    uint32 flags,
    uint32 phy_reg_addr,
    uint32 phy_data,
    uint32 phy_mask)
{
    uint32 phy_rd_data;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_reg_get(unit, port, flags, phy_reg_addr, &phy_rd_data));
    phy_data |= (phy_rd_data & ~phy_mask);
    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_reg_set(unit, port, flags, phy_reg_addr, phy_data));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_control_low_latency_llfc_set(
    int unit,
    soc_port_t port,
    int value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if ((encoding != PORTMOD_PCS_64B66B_FEC) && !DNXC_PORT_FABRIC_ENCODING_IS_RS_FEC(encoding))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Low latency LLFC control is supported only by KR FEC and RS FECs\n");
    }
    if (value)
    {
        properties |= PORTMOD_ENCODING_LOW_LATENCY_LLFC;
    }
    else
    {
        properties &= ~PORTMOD_ENCODING_LOW_LATENCY_LLFC;
    }
    SHR_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, properties, encoding));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_control_low_latency_llfc_get(
    int unit,
    soc_port_t port,
    int *value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if ((encoding != PORTMOD_PCS_64B66B_FEC) && !DNXC_PORT_FABRIC_ENCODING_IS_RS_FEC(encoding))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Low latency LLFC control is supported only by KR FEC and RS FECs\n");
    }
    *value = PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_control_fec_error_detect_set(
    int unit,
    soc_port_t port,
    int value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_FEC && !DNXC_PORT_FABRIC_ENCODING_IS_RS_FEC(encoding))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Error detect control is supported only by KR FEC and RS FECs\n");
    }

    if (value)
    {
        properties |= PORTMOD_ENCODING_FEC_ERROR_DETECT;
    }
    else
    {
        properties &= ~PORTMOD_ENCODING_FEC_ERROR_DETECT;
    }

    SHR_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, properties, encoding));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_control_fec_error_detect_get(
    int unit,
    soc_port_t port,
    int *value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_FEC && !DNXC_PORT_FABRIC_ENCODING_IS_RS_FEC(encoding))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Error detect control is supported only by KR FEC and RS FECs\n");
    }

    *value = PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_control_llfc_after_fec_enable_set(
    int unit,
    soc_port_t port,
    int value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (!DNXC_PORT_FABRIC_ENCODING_IS_RS_FEC(encoding))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "LLFC after FEC control is supported only by RS FECs\n");
    }

    if (value)
    {
        properties |= PORTMOD_ENCODING_LLFC_AFTER_FEC;
    }
    else
    {
        properties &= ~PORTMOD_ENCODING_LLFC_AFTER_FEC;
    }

    SHR_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, properties, encoding));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_control_llfc_after_fec_enable_get(
    int unit,
    soc_port_t port,
    int *value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (!DNXC_PORT_FABRIC_ENCODING_IS_RS_FEC(encoding))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "LLFC after FEC control is supported only by RS FECs\n");
    }

    *value = PORTMOD_ENCODING_LLFC_AFTER_FEC_GET(properties);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_control_power_set(
    int unit,
    soc_port_t port,
    uint32 flags,
    soc_dnxc_port_power_t power)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, soc_dnxc_port_power_on == power ? 1 : 0));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_control_power_get(
    int unit,
    soc_port_t port,
    soc_dnxc_port_power_t * power)
{
    int enable;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, &enable));
    *power = enable ? soc_dnxc_port_power_on : soc_dnxc_port_power_off;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_prbs_tx_enable_set(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int value)
{
    int flags = 0;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);
    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);
    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_prbs_enable_set(unit, port, portmod_mode, flags, value));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_prbs_rx_enable_set(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int value)
{
    int flags = 0;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);
    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);
    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_prbs_enable_set(unit, port, portmod_mode, flags, value));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_prbs_tx_enable_get(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int *value)
{
    int flags = 0;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);
    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);
    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_prbs_enable_get(unit, port, portmod_mode, flags, value));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_prbs_rx_enable_get(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int *value)
{
    int flags = 0;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);
    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);
    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_prbs_enable_get(unit, port, portmod_mode, flags, value));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_prbs_rx_status_get(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int *value)
{
    phymod_prbs_status_t status;
    int flags = PHYMOD_PRBS_STATUS_F_CLEAR_ON_READ;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);
    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);
    SHR_IF_ERR_EXIT(phymod_prbs_status_t_init(&status));
    SHR_IF_ERR_EXIT(portmod_port_prbs_status_get(unit, port, portmod_mode, flags, &status));
    if (status.prbs_lock_loss)
    {
        *value = -2;
    }
    else if (!status.prbs_lock)
    {
        *value = -1;
    }
    else
    {
        *value = status.error_count;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_prbs_tx_invert_data_get(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int *invert)
{
    int flags = 0;
    phymod_prbs_t config;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);
    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);
    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, portmod_mode, flags, &config));
    *invert = config.invert ? 1 : 0;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_prbs_rx_invert_data_get(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int *invert)
{
    int flags = 0;
    phymod_prbs_t config;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);
    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);
    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, portmod_mode, flags, &config));
    *invert = config.invert ? 1 : 0;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_prbs_tx_invert_data_set(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int invert)
{
    int flags = 0;
    phymod_prbs_t config;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);
    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);
    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, portmod_mode, flags, &config));
    config.invert = invert ? 1 : 0;
    SHR_IF_ERR_EXIT(portmod_port_prbs_config_set(unit, port, portmod_mode, flags, &config));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_prbs_rx_invert_data_set(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int invert)
{
    int flags = 0;
    phymod_prbs_t config;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);
    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);
    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, portmod_mode, flags, &config));
    config.invert = invert ? 1 : 0;
    SHR_IF_ERR_EXIT(portmod_port_prbs_config_set(unit, port, portmod_mode, flags, &config));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_prbs_polynomial_set(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int value)
{
    int flags = 0;
    phymod_prbs_poly_t poly;
    phymod_prbs_t config;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);
    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);
    SHR_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, portmod_mode, flags, &config));
    SHR_IF_ERR_EXIT(soc_prbs_poly_to_phymod(value, &poly));
    config.poly = poly;
    SHR_IF_ERR_EXIT(portmod_port_prbs_config_set(unit, port, portmod_mode, flags, &config));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_prbs_polynomial_get(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int *value)
{
    int flags = 0;
    phymod_prbs_t config;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);
    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);
    SHR_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, portmod_mode, flags, &config));
    SHR_IF_ERR_EXIT(phymod_prbs_poly_to_soc(config.poly, (uint32 *) value));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_loopback_set(
    int unit,
    soc_port_t port,
    portmod_loopback_mode_t loopback)
{
    portmod_loopback_mode_t current_lb = portmodLoopbackCount;
#ifdef ADAPTER_SERVER_MODE
    int rv;
#endif
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_dnxc_port_loopback_get(unit, port, &current_lb));
    if (current_lb == loopback)
    {

        SHR_EXIT();
    }
    if (current_lb != portmodLoopbackCount)
    {

        SHR_IF_ERR_EXIT(portmod_port_loopback_set(unit, port, current_lb, 0));
    }
    if (loopback != portmodLoopbackCount)
    {

        SHR_IF_ERR_EXIT(portmod_port_loopback_set(unit, port, loopback, 1));
    }
#ifdef ADAPTER_SERVER_MODE

    rv = _bcm_linkscan_available(unit);
    if (rv == BCM_E_NONE)
    {

        if (loopback == portmodLoopbackPhyGloopPMD || loopback == portmodLoopbackMacAsyncFifo)
        {

            SHR_IF_ERR_EXIT(_bcm_link_force(unit, port, TRUE, TRUE));
        }
        else
        {

            SHR_IF_ERR_EXIT(_bcm_link_force(unit, port, FALSE, FALSE));
        }
    }
#endif
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_loopback_get(
    int unit,
    soc_port_t port,
    portmod_loopback_mode_t * loopback)
{
    portmod_loopback_mode_t supported_lb_modes[] = {
        portmodLoopbackMacAsyncFifo, portmodLoopbackMacOuter, portmodLoopbackMacPCS, portmodLoopbackMacRloop,
        portmodLoopbackPhyGloopPMD, portmodLoopbackPhyGloopPCS, portmodLoopbackPhyRloopPMD
    };
    int i = 0, rv;
    int enable = 0;
    SHR_FUNC_INIT_VARS(unit);
    *loopback = portmodLoopbackCount;
    for (i = 0; i < COUNTOF(supported_lb_modes); i++)
    {

        if (IS_SFI_PORT(unit, port) || IS_IL_PORT(unit, port))
        {
            if (supported_lb_modes[i] == portmodLoopbackPhyGloopPCS)
            {
                continue;
            }
        }
        rv = portmod_port_loopback_get(unit, port, supported_lb_modes[i], &enable);
        if (rv == _SHR_E_UNAVAIL)
        {

            continue;
        }
        SHR_IF_ERR_EXIT(rv);
        if (enable)
        {
            *loopback = supported_lb_modes[i];
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_pre_enable_set(
    int unit,
    soc_port_t port,
    int enable)
{
    int is_init_sequence;
    SHR_FUNC_INIT_VARS(unit);
    is_init_sequence = !(dnxc_init_is_init_done_get(unit));

    if (!is_init_sequence)
    {
        if (SOC_IS_DNX(unit))
        {
#ifdef BCM_DNX_SUPPORT
            if (IS_SFI_PORT(unit, port))
            {
                int link_id;

                SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, port, &link_id));

                SHR_IF_ERR_EXIT(dnx_fabric_link_isolation_set(unit, link_id, 1));
            }
#endif
        }
        else
        {
#ifdef BCM_DNXF_SUPPORT
            SHR_IF_ERR_EXIT(soc_dnxf_port_pre_enable_isolate_set(unit, port));

#endif
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_post_enable_set(
    int unit,
    soc_port_t port,
    int enable)
{
    int is_init_sequence;
    SHR_FUNC_INIT_VARS(unit);

    is_init_sequence = !(dnxc_init_is_init_done_get(unit));

    if (enable)
    {
        if (!is_init_sequence)
        {
            if (SOC_IS_DNX(unit))
            {
#ifdef BCM_DNX_SUPPORT
                if (IS_SFI_PORT(unit, port))
                {
                    int is_link_isolated, link_id;

                    sal_usleep(4000);

                    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, port, &link_id));

                    SHR_IF_ERR_EXIT(dnx_fabric_db.links.is_link_isolated.get(unit, link_id, &is_link_isolated));
                    if (is_link_isolated == 0)
                    {
                        SHR_IF_ERR_EXIT(dnx_fabric_link_isolation_set(unit, link_id, 0));
                    }
                }

#endif
            }
            else
            {
#ifdef BCM_DNXF_SUPPORT
                SHR_IF_ERR_EXIT(soc_dnxf_port_post_enable_isolate_set(unit, port));
#endif
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_enable_set(
    int unit,
    soc_port_t port,
    uint32 flags,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_dnxc_port_pre_enable_set(unit, port, enable));
    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, (int) flags, enable));
    SHR_IF_ERR_EXIT(soc_dnxc_port_post_enable_set(unit, port, enable));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_enable_get(
    int unit,
    soc_port_t port,
    uint32 flags,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, port, flags, enable));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_rx_locked_get(
    int unit,
    soc_port_t port,
    uint32 *rx_locked)
{
    phymod_phy_access_t phys[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    portmod_access_get_params_t params;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = PORTMOD_PHYN_LAST_ONE;
    params.sys_side = PORTMOD_SIDE_LINE;
    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get
                    (unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));
    SHR_IF_ERR_EXIT(phymod_phy_rx_pmd_locked_get(phys, rx_locked));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_extract_cig_from_llfc_enable_set(
    int unit,
    soc_port_t port,
    int value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_FEC)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Extract Congestion Ind from LLFC cells control is supported only by KR_FEC\n");
    }
    if (value)
    {
        properties |= PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC;
    }
    else
    {
        properties &= ~PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC;
    }
    SHR_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, properties, encoding));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_extract_cig_from_llfc_enable_get(
    int unit,
    soc_port_t port,
    int *value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_FEC)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Extract Congestion Ind from LLFC cells control is supported only by KR_FEC\n");
    }
    *value = PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_GET(properties);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_control_cells_fec_bypass_enable_set(
    int unit,
    soc_port_t port,
    int value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (!DNXC_PORT_FABRIC_ENCODING_IS_RS_FEC(encoding))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Control cells FEC bypass control is supported only by RS FECs");
    }

    if (value)
    {
        PORTMOD_ENCODING_CONTROL_CELLS_FEC_BYPASS_SET(properties);
    }
    else
    {
        PORTMOD_ENCODING_CONTROL_CELLS_FEC_BYPASS_CLR(properties);
    }
    SHR_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, properties, encoding));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_control_cells_fec_bypass_enable_get(
    int unit,
    soc_port_t port,
    int *value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (!DNXC_PORT_FABRIC_ENCODING_IS_RS_FEC(encoding))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Control cells FEC bypass control is supported only by RS FECs");
    }

    *value = PORTMOD_ENCODING_CONTROL_CELLS_FEC_BYPASS_GET(properties);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_port_link_pms_get(
    int unit,
    bcm_port_t link,
    int *pm_start,
    int *nof_pms)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        int nof_links_in_fsrd = 0, nof_pms_get = 0;

        if (link != -1)
        {
            nof_links_in_fsrd = dnx_data_fabric.blocks.nof_links_in_fsrd_get(unit);

            *pm_start = link / nof_links_in_fsrd;
            *nof_pms = 1;
        }
        else
        {
            nof_pms_get = dnx_data_fabric.blocks.nof_pms_get(unit);

            *pm_start = 0;
            *nof_pms = nof_pms_get;
        }

        SHR_EXIT();
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        if (link != -1)
        {
            int pm_inner_link;
            SHR_IF_ERR_EXIT(soc_dnxf_drv_link_to_block_mapping(unit, link, pm_start, &pm_inner_link, SOC_BLK_FSRD));
            *nof_pms = 1;
        }
        else
        {
            *pm_start = 0;
            *nof_pms = dnxf_data_port.general.nof_pms_get(unit);
        }

#endif
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_cable_swap_set_verify(
    int unit,
    bcm_port_t link,
    bcm_fabric_link_control_t type,
    int arg)
{
    int enable, pm_id, nof_pms, pm_index, is_enable;
    int link_iter;

    SHR_FUNC_INIT_VARS(unit);

    if (arg != 0 && arg != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid arg value");
    }

    if (type == bcmFabricLinkCableSwapMasterMode)
    {

        SHR_IF_ERR_EXIT(soc_dnxc_port_link_pms_get(unit, link, &pm_id, &nof_pms));

        for (pm_index = pm_id; pm_index < pm_id + nof_pms; ++pm_index)
        {
            if (SOC_IS_DNX(unit))
            {
#ifdef BCM_DNX_SUPPORT
                SHR_IF_ERR_EXIT(dnx_fabric_db.cable_swap.is_enable.get(unit, pm_index, &is_enable));
#endif
            }
            else
            {
#ifdef BCM_DNXF_SUPPORT
                SHR_IF_ERR_EXIT(dnxf_state.cable_swap.is_enable.get(unit, pm_index, &is_enable));
#endif
            }

            if (is_enable)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "setting PM %d as a master must be done while PM's cable swap is disabled",
                             pm_index);
            }
        }
    }

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        int fabric_port_base = 0, nof_links_in_blk = 0, nof_links = 0;
        int start_link, end_link;

        fabric_port_base = dnx_data_port.general.fabric_port_base_get(unit);
        nof_links_in_blk = dnx_data_fabric.blocks.nof_links_in_fsrd_get(unit);

        nof_links = dnx_data_fabric.links.nof_links_get(unit);

        if (link != -1)
        {

            start_link = (link / nof_links_in_blk) * nof_links_in_blk;
            end_link = start_link + nof_links_in_blk;
        }
        else
        {

            start_link = 0;
            end_link = nof_links;
        }

        for (link_iter = start_link; link_iter < end_link; ++link_iter)
        {
            SHR_IF_ERR_EXIT(soc_dnxc_port_enable_get(unit, (fabric_port_base + link_iter), 0, &enable));
            if (enable)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "all ports on same PM must be disabled prior to setting a cable swap configuration of a port on that PM");
            }
        }
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        soc_pbmp_t links;

        SOC_PBMP_CLEAR(links);

        if (link != -1)
        {
            SHR_IF_ERR_EXIT(soc_dnxc_port_link_pms_get(unit, link, &pm_id, &nof_pms));

            SHR_IF_ERR_EXIT(soc_dnxf_link_device_mapping_bmp_get(unit, pm_id, SOC_BLK_FSRD, &links));
        }
        else
        {

            SOC_PBMP_ASSIGN(links, PBMP_SFI_ALL(unit));
        }

        SOC_PBMP_ITER(links, link_iter)
        {
            SHR_IF_ERR_EXIT(soc_dnxc_port_enable_get(unit, link_iter, 0, &enable));
            if (enable)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "all ports on same PM must be disabled prior to setting a cable swap configuration of a port on that PM");
            }
        }
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_cable_swap_set(
    int unit,
    bcm_port_t link,
    bcm_fabric_link_control_t type,
    int arg)
{
    int pm_id, pm_index, nof_pms;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(soc_dnxc_port_cable_swap_set_verify(unit, link, type, arg));

    SHR_IF_ERR_EXIT(soc_dnxc_port_link_pms_get(unit, link, &pm_id, &nof_pms));
    for (pm_index = pm_id; pm_index < pm_id + nof_pms; ++pm_index)
    {
        switch (type)
        {
            case bcmFabricLinkCableSwapEnable:
                if (SOC_IS_DNX(unit))
                {
#ifdef BCM_DNX_SUPPORT
                    SHR_IF_ERR_EXIT(dnx_fabric_db.cable_swap.is_enable.set(unit, pm_index, arg));
#endif
                }
                else
                {
#ifdef BCM_DNXF_SUPPORT
                    SHR_IF_ERR_EXIT(dnxf_state.cable_swap.is_enable.set(unit, pm_index, arg));
#endif
                }
                break;
            case bcmFabricLinkCableSwapMasterMode:
                if (SOC_IS_DNX(unit))
                {
#ifdef BCM_DNX_SUPPORT
                    SHR_IF_ERR_EXIT(dnx_fabric_db.cable_swap.is_master.set(unit, pm_index, arg));
#endif
                }
                else
                {
#ifdef BCM_DNXF_SUPPORT
                    SHR_IF_ERR_EXIT(dnxf_state.cable_swap.is_master.set(unit, pm_index, arg));
#endif
                }
                break;
            default:
                break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_cable_swap_get_verify(
    int unit,
    bcm_port_t link,
    bcm_fabric_link_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_cable_swap_get(
    int unit,
    bcm_port_t link,
    bcm_fabric_link_control_t type,
    int *arg)
{
    int pm_id, nof_pms;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(soc_dnxc_port_cable_swap_get_verify(unit, link, type, arg));

    SHR_IF_ERR_EXIT(soc_dnxc_port_link_pms_get(unit, link, &pm_id, &nof_pms));
    switch (type)
    {
        case bcmFabricLinkCableSwapEnable:
            if (SOC_IS_DNX(unit))
            {
#ifdef BCM_DNX_SUPPORT
                SHR_IF_ERR_EXIT(dnx_fabric_db.cable_swap.is_enable.get(unit, pm_id, arg));
#endif
            }
            else
            {
#ifdef BCM_DNXF_SUPPORT
                SHR_IF_ERR_EXIT(dnxf_state.cable_swap.is_enable.get(unit, pm_id, arg));
#endif
            }
            break;
        case bcmFabricLinkCableSwapMasterMode:
            if (SOC_IS_DNX(unit))
            {
#ifdef BCM_DNX_SUPPORT
                SHR_IF_ERR_EXIT(dnx_fabric_db.cable_swap.is_master.get(unit, pm_id, arg));
#endif
            }
            else
            {
#ifdef BCM_DNXF_SUPPORT
                SHR_IF_ERR_EXIT(dnxf_state.cable_swap.is_master.get(unit, pm_id, arg));
#endif
            }
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_fabric_fec_control_verify(
    int unit,
    bcm_port_t port,
    bcm_port_control_t control_type)
{
    int is_combination_valid = 0, is_control_type_valid = 0;
    bcm_port_resource_t port_resource;

    SHR_FUNC_INIT_VARS(unit);

    bcm_port_resource_t_init(&port_resource);

    SHR_IF_ERR_EXIT(soc_dnxc_port_resource_get(unit, port, &port_resource));

    SHR_IF_ERR_EXIT(soc_dnxc_port_fabric_fec_control_verify_params_get
                    (unit, port_resource.fec_type, control_type, &is_combination_valid, &is_control_type_valid));

    if (!is_control_type_valid)
    {
        switch (control_type)
        {
            case bcmPortControlLlfcCellsCongestionIndEnable:
                SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                             "bcmPortControlLlfcCellsCongestionIndEnable is not available. Always enabled for device %s \n",
                             soc_dev_name(unit));
                break;
            case bcmPortControlLLFCAfterFecEnable:
                SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                             "bcmPortControlLLFCAfterFecEnable is not available. LLFC After FEC is always enabled for device %s \n",
                             soc_dev_name(unit));
                break;
            case bcmPortControlControlCellsFecBypassEnable:
                SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                             "bcmPortControlControlCellsFecBypassEnable is not available. Control Cells FEC bypass is disabled for device %s \n",
                             soc_dev_name(unit));
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Control Type is not valid FEC Control Type\n");
                break;
        }
    }
    if (!is_combination_valid)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "This FEC control is not supported for this FEC Type.");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_fabric_fec_control_verify_params_get(
    int unit,
    bcm_port_phy_fec_t fec_type,
    bcm_port_control_t control_type,
    int *is_combination_valid,
    int *is_control_type_valid)
{
    int idx;

    SHR_FUNC_INIT_VARS(unit);

    (*is_combination_valid) = FALSE;
    (*is_control_type_valid) = FALSE;

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        for (idx = 0; idx < dnx_data_fabric.links.supported_fec_controls_info_get(unit)->key_size[0]; idx++)
        {
            if (dnx_data_fabric.links.supported_fec_controls_get(unit, idx)->is_valid)
            {
                if (control_type == dnx_data_fabric.links.supported_fec_controls_get(unit, idx)->control_type)
                {
                    (*is_control_type_valid) = TRUE;
                }
                if ((fec_type == dnx_data_fabric.links.supported_fec_controls_get(unit, idx)->fec_type) &&
                    (control_type == dnx_data_fabric.links.supported_fec_controls_get(unit, idx)->control_type))
                {
                    (*is_combination_valid) = TRUE;
                    break;
                }
            }
        }
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        for (idx = 0; idx < dnxf_data_port.general.supported_fec_controls_info_get(unit)->key_size[0]; idx++)
        {
            if (dnxf_data_port.general.supported_fec_controls_get(unit, idx)->is_valid)
            {
                if (control_type == dnxf_data_port.general.supported_fec_controls_get(unit, idx)->control_type)
                {
                    (*is_control_type_valid) = TRUE;
                }
                if ((fec_type == dnxf_data_port.general.supported_fec_controls_get(unit, idx)->fec_type) &&
                    (control_type == dnxf_data_port.general.supported_fec_controls_get(unit, idx)->control_type))
                {
                    (*is_combination_valid) = TRUE;
                    break;
                }
            }
        }
#endif
    }

    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_fabric_fmac_tx_padding_verify(
    int unit,
    bcm_port_t port,
    int pad_size,
    int is_set)
{
    bcm_port_resource_t resource;
    int is_supported = 0;
    uint32 is_valid = 0;
    int i = 0;
    int table_size = 0;
    int min_port_speed = 0;
    int valid_pad_size = 0;
    char valid_pad_size_buf[5];
    char valid_pad_sizes[30] = "";
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        is_supported = dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_fmac_tx_padding_supported);
        if (is_supported)
        {
            min_port_speed = dnx_data_fabric.general.fmac_tx_padding_min_port_speed_get(unit);
            table_size = dnx_data_fabric.general.supported_fmac_tx_padding_sizes_info_get(unit)->key_size[0];
        }
    }
    else
#endif
    {
#ifdef BCM_DNXF_SUPPORT
        is_supported = dnxf_data_port.general.feature_get(unit, dnxf_data_port_general_fmac_tx_padding_supported);
        if (is_supported)
        {
            min_port_speed = dnxf_data_port.general.fmac_tx_padding_min_port_speed_get(unit);
            table_size = dnxf_data_port.general.supported_fmac_tx_padding_sizes_info_get(unit)->key_size[0];
        }
#endif
    }

    if (!is_supported)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Error, FMAC TX padding not supported for this device\n");
    }

    SHR_IF_ERR_EXIT(soc_dnxc_port_resource_get(unit, port, &resource));
    if (resource.speed < min_port_speed)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Error, FMAC Tx padding not supported for port speeds below %d\n", min_port_speed);
    }

    if (is_set)
    {
        for (i = 0; i < table_size; i++)
        {
#ifdef BCM_DNX_SUPPORT
            if (SOC_IS_DNX(unit))
            {
                valid_pad_size = dnx_data_fabric.general.supported_fmac_tx_padding_sizes_get(unit, i)->size;
            }
#endif
#ifdef BCM_DNXF_SUPPORT
            if (SOC_IS_DNXF(unit))
            {
                valid_pad_size = dnxf_data_port.general.supported_fmac_tx_padding_sizes_get(unit, i)->size;
            }
#endif
            if (pad_size == valid_pad_size)
            {
                is_valid = 1;
                break;
            }
            else
            {
                sal_snprintf(valid_pad_size_buf, 5, "%d ", valid_pad_size);
                sal_strncat_s(valid_pad_sizes, valid_pad_size_buf, 30);
            }
        }
        if (!is_valid)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error, wrong padding size for port %d\r\n"
                         "Valid values are:%s\n", port, valid_pad_sizes);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_fabric_max_speed_get(
    int unit,
    int *max_speed)
{
    int idx;

    SHR_FUNC_INIT_VARS(unit);

    *max_speed = 0;

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        for (idx = 0; idx < dnx_data_fabric.links.supported_interfaces_info_get(unit)->key_size[0]; idx++)
        {
            if (dnx_data_fabric.links.supported_interfaces_get(unit, idx)->is_valid)
            {
                *max_speed =
                    (*max_speed <
                     dnx_data_fabric.links.supported_interfaces_get(unit,
                                                                    idx)->speed) ? dnx_data_fabric.
                    links.supported_interfaces_get(unit, idx)->speed : *max_speed;
            }
        }
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        for (idx = 0; idx < dnxf_data_port.general.supported_interfaces_info_get(unit)->key_size[0]; idx++)
        {
            if (dnxf_data_port.general.supported_interfaces_get(unit, idx)->is_valid)
            {
                *max_speed =
                    (*max_speed <
                     dnxf_data_port.general.supported_interfaces_get(unit,
                                                                     idx)->speed) ? dnxf_data_port.
                    general.supported_interfaces_get(unit, idx)->speed : *max_speed;
            }
        }
#endif
    }

    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_fabric_fmacs_pbmp_to_be_enabled_get(
    int unit,
    soc_pbmp_t sfi_pbmp,
    soc_pbmp_t enable_pbmp,
    soc_pbmp_t * fmacs_to_be_enabled_bmp)
{

    soc_pbmp_t fmacs_enabled_bmp;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&fmacs_enabled_bmp, 0, sizeof(fmacs_enabled_bmp));
    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_pbmp_to_fmac_bmp_convert(unit, sfi_pbmp, &fmacs_enabled_bmp));
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_pbmp_to_fmac_bmp_convert(unit, enable_pbmp, fmacs_to_be_enabled_bmp));
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        SHR_IF_ERR_EXIT(soc_dnxf_port_pbmp_to_fmac_bmp_convert(unit, sfi_pbmp, &fmacs_enabled_bmp));
        SHR_IF_ERR_EXIT(soc_dnxf_port_pbmp_to_fmac_bmp_convert(unit, enable_pbmp, fmacs_to_be_enabled_bmp));
#endif
    }

    SOC_PBMP_AND(fmacs_enabled_bmp, *fmacs_to_be_enabled_bmp);

    SOC_PBMP_REMOVE(*fmacs_to_be_enabled_bmp, fmacs_enabled_bmp);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_fabric_fmacs_pbmp_to_be_disabled_get(
    int unit,
    soc_pbmp_t sfi_pbmp,
    soc_pbmp_t disable_pbmp,
    soc_pbmp_t * fmacs_to_be_disabled_bmp)
{

    soc_pbmp_t fmacs_enabled_bmp;
    soc_pbmp_t fmacs_that_should_remain_enabled_bmp;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&fmacs_that_should_remain_enabled_bmp, 0, sizeof(fmacs_that_should_remain_enabled_bmp));
    sal_memset(&fmacs_enabled_bmp, 0, sizeof(fmacs_enabled_bmp));

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_pbmp_to_fmac_bmp_convert(unit, sfi_pbmp, &fmacs_enabled_bmp));
        BCM_PBMP_REMOVE(sfi_pbmp, disable_pbmp);
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_pbmp_to_fmac_bmp_convert
                        (unit, sfi_pbmp, &fmacs_that_should_remain_enabled_bmp));
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        SHR_IF_ERR_EXIT(soc_dnxf_port_pbmp_to_fmac_bmp_convert(unit, sfi_pbmp, &fmacs_enabled_bmp));
        BCM_PBMP_REMOVE(sfi_pbmp, disable_pbmp);
        SHR_IF_ERR_EXIT(soc_dnxf_port_pbmp_to_fmac_bmp_convert(unit, sfi_pbmp, &fmacs_that_should_remain_enabled_bmp));
#endif
    }

    BCM_PBMP_REMOVE(fmacs_enabled_bmp, fmacs_that_should_remain_enabled_bmp);
    BCM_PBMP_ASSIGN(*fmacs_to_be_disabled_bmp, fmacs_enabled_bmp);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_port_post_ber_proj_fec_mode_verify(
    int unit,
    bcm_pbmp_t pbmp)
{
    int is_fec_mode_supported = FALSE;
    int port;
    int prbs_phy_rx_enabled, prbs_mac_rx_enabled;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        is_fec_mode_supported =
            dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_fec_ber_proj_supported);
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        is_fec_mode_supported = dnxf_data_port.general.feature_get(unit, dnxf_data_port_general_fec_ber_proj_supported);
#endif
    }

    if (!is_fec_mode_supported)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BER Projection FEC mode not supported on this device");
    }

    BCM_PBMP_ITER(pbmp, port)
    {
        if (!IS_SFI_PORT(unit, port))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BER Projection FEC mode supported only on Fabric ports");
        }

        SHR_IF_ERR_EXIT(soc_dnxc_port_prbs_rx_enable_get
                        (unit, port, soc_dnxc_port_prbs_mode_phy, &prbs_phy_rx_enabled));
        SHR_IF_ERR_EXIT(soc_dnxc_port_prbs_rx_enable_get
                        (unit, port, soc_dnxc_port_prbs_mode_mac, &prbs_mac_rx_enabled));
        if (prbs_phy_rx_enabled || prbs_mac_rx_enabled)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "PRBS not supported with BER Projection FEC mode. To use FEC mode, disable PRBS.");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_fabric_fec_ber_projection_display(
    int unit,
    bcm_pbmp_t pbmp,
    int sample_time)
{
    uint32 num_ports = 0;
    uint32 x, y, z = 0;
    int i, port, port_idx = 0, nof_counters = 0;
    phymod_phy_ber_proj_errcnt_t *errcnt_array = NULL;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        nof_counters = dnx_data_fabric.general.nof_ber_proj_error_analyzer_counters_get(unit);
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        nof_counters = dnxf_data_port.general.nof_ber_proj_error_analyzer_counters_get(unit);
#endif
    }

    SHR_IF_ERR_EXIT(soc_dnxc_port_post_ber_proj_fec_mode_verify(unit, pbmp));

    BCM_PBMP_COUNT(pbmp, num_ports);
    SHR_ALLOC_SET_ZERO(errcnt_array, sizeof(phymod_phy_ber_proj_errcnt_t) * num_ports, "BER error cnt array",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    BCM_PBMP_ITER(pbmp, port)
    {
        LOG_CLI(("Post-FEC BER: Port %d", port));
        LOG_CLI(("\n -------------------------------------------------------------\n"));
        LOG_CLI(("  FEC Error Analyzer Error_Counts for Port %d:\n", port));
        LOG_CLI((" -------------------------------------------------------------\n"));
        for (i = 0; i < nof_counters; i++)
        {
            if (errcnt_array[port_idx].prbs_errcnt[i] == 0xFFFFFFFF)
            {
                LOG_CLI(("    FEC Frames with > %2d Errors (t=%2d) =        MAX \n", i, i));
            }
            else
            {
                LOG_CLI(("    FEC Frames with > %2d Errors (t=%2d) = %10u \n", i, i,
                         errcnt_array[port_idx].prbs_errcnt[i]));
            }
        }
        LOG_CLI((" -------------------------------------------------------------\n"));

        if (errcnt_array[port_idx].prbs_proj_ber)
        {

            x = DNXC_PORT_BER_PROJ_INTEGER_PART_GET(errcnt_array[port_idx].prbs_proj_ber);
            y = DNXC_PORT_BER_PROJ_DECIMAL_PART_GET(errcnt_array[port_idx].prbs_proj_ber);
            z = DNXC_PORT_BER_PROJ_EXPONENT_PART_GET(errcnt_array[port_idx].prbs_proj_ber);

            LOG_CLI(("\n  FEC Error Analyzer Projected BER (Equivalent projected post-FEC BER for t=%d) for Port %d = %d.%de-%d\n\n", nof_counters, port, x, y, z));
        }
        else
        {
            LOG_CLI(("\n << WARNING: Not enough valid measured points available for BER Projection for Port %d ! >>\n\n", port));
        }

        ++port_idx;
    }

exit:
    SHR_FREE(errcnt_array);
    SHR_FUNC_EXIT;
}

static void
soc_dnxc_port_post_ber_proj_errcnt_copy(
    int unit,
    phymod_phy_ber_proj_errcnt_t * errcnt_result,
    int actual_errcnt,
    int nof_counters,
    bcm_port_ber_proj_analyzer_errcnt_t * errcnt_output)
{
    int i = 0, j = 0;
    for (i = 0; i < actual_errcnt; i++)
    {
        errcnt_output[i].start_hist_errcnt_thresh = errcnt_result[i].start_hist_errcnt_thresh;
        errcnt_output[i].max_errcnt_thresh = errcnt_result[i].max_errcnt_thresh;
        errcnt_output[i].proj_ber = errcnt_result[i].prbs_proj_ber;

        for (j = 0; j < nof_counters; j++)
        {
            errcnt_output[i].ber_proj_prbs_errcnt[j] = errcnt_result[i].prbs_errcnt[j];
        }
    }
}

static shr_error_e
soc_dnxc_port_post_ber_proj_prbs_mode_get(
    int unit,
    bcm_port_t port,
    int lane,
    const bcm_port_ber_proj_params_t * ber_proj,
    int max_errcnt,
    int nof_counters,
    bcm_port_ber_proj_analyzer_errcnt_t * errcnt_array,
    int *actual_errcnt)
{
    soc_port_phy_ber_proj_params_t ber_proj_params;
    phymod_phy_ber_proj_errcnt_t errcnt_result[DNXC_PORT_BER_PROJ_ERRCNT_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(errcnt_result, 0, sizeof(phymod_phy_ber_proj_errcnt_t) * DNXC_PORT_BER_PROJ_ERRCNT_SIZE);

    ber_proj_params.ber_proj_fec_type = (_shr_port_phy_fec_t) (ber_proj->fec_type);
    ber_proj_params.ber_proj_hist_errcnt_thresh = ber_proj->hist_errcnt_thresh;
    ber_proj_params.ber_proj_timeout_s = ber_proj->interval_second;
    ber_proj_params.ber_proj_phase = SOC_PORT_PHY_BER_PROJ_P_ALL;
    ber_proj_params.ber_proj_prbs_errcnt = (soc_phy_prbs_errcnt_t *) errcnt_result;
    ber_proj_params.without_print = 0;
    *actual_errcnt = 0;

    SHR_IF_ERR_EXIT(portmod_common_post_ber_proj_get(unit, port, 0, lane, 0,
                                                     &ber_proj_params, max_errcnt, actual_errcnt));

    soc_dnxc_port_post_ber_proj_errcnt_copy(unit, errcnt_result, *actual_errcnt, nof_counters, errcnt_array);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_port_post_ber_proj_fec_mode_get(
    int unit,
    bcm_port_t port,
    const bcm_port_ber_proj_params_t * ber_proj,
    int nof_counters,
    bcm_port_ber_proj_analyzer_errcnt_t * errcnt_array,
    int *actual_errcnt)
{
    phymod_phy_ber_proj_errcnt_t *errcnt_result = NULL;
    bcm_pbmp_t pbmp;
    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    SHR_IF_ERR_EXIT(soc_dnxc_port_post_ber_proj_fec_mode_verify(unit, pbmp));

    SHR_ALLOC_SET_ZERO(errcnt_result, sizeof(phymod_phy_ber_proj_errcnt_t), "BER error cnt array",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    *actual_errcnt = 1;

    soc_dnxc_port_post_ber_proj_errcnt_copy(unit, errcnt_result, *actual_errcnt, nof_counters, errcnt_array);

exit:
    SHR_FREE(errcnt_result);
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_post_ber_proj_get(
    int unit,
    bcm_port_t port,
    int lane,
    const bcm_port_ber_proj_params_t * ber_proj,
    int max_errcnt,
    bcm_port_ber_proj_analyzer_errcnt_t * errcnt_array,
    int *actual_errcnt)
{
    int nof_counters = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        nof_counters = dnx_data_fabric.general.nof_ber_proj_error_analyzer_counters_get(unit);
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        nof_counters = dnxf_data_port.general.nof_ber_proj_error_analyzer_counters_get(unit);
#endif
    }

    if (ber_proj->mode == bcmPortBerProjModePrbs)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_post_ber_proj_prbs_mode_get
                        (unit, port, lane, ber_proj, max_errcnt, nof_counters, errcnt_array, actual_errcnt));
    }
    else if (ber_proj->mode == bcmPortBerProjModeFec)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_post_ber_proj_fec_mode_get
                        (unit, port, ber_proj, nof_counters, errcnt_array, actual_errcnt));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid BER Projection mode");
    }

exit:
    SHR_FUNC_EXIT;
}

#endif

#undef BSL_LOG_MODULE
