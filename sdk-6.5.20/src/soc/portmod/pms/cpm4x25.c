/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/pm4x25.h>
#include <soc/portmod/pm4x25_shared.h>


#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_CPM4X25_SUPPORT

extern int tscf_gen3_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable);
extern int tscf_gen3_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable);
extern int tscf_gen3_phy_synce_clk_ctrl_set(const phymod_phy_access_t* phy, phymod_synce_clk_ctrl_t cfg);
extern int tscf_gen3_phy_synce_clk_ctrl_get(const phymod_phy_access_t* phy, phymod_synce_clk_ctrl_t* cfg);

/*
 * WARMBOOT Macro are in pm4x25_shared.h.
 */

#define CPM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc) \
    do { \
        uint32 is_bypass; \
        phy_acc = 0; \
        _SOC_IF_ERR_EXIT(PM4x25_IS_BYPASSED_GET(unit, pm_info, &is_bypass)); \
        phy_acc = (is_bypass && PM_4x25_INFO(pm_info)->first_phy != -1) ? (PM_4x25_INFO(pm_info)->first_phy | SOC_REG_ADDR_PHY_ACC_MASK) : port ; \
    } while (0)

extern int pm4x25_to_cpm_pm_enable(int unit, int port, pm_info_t pm_info, int port_index, const portmod_port_add_info_t* add_info, int enable);
extern int pm4x25_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data);
static int _cprimod_port_speed_id_to_vco(int speed_id, portmod_encap_t encap);
static int _cprimod_pll_mult_to_vco_freq(int pll_mult);
static int _cpm4x25_port_refclk_select(int unit, int port, portmod_cpm_core_mode_t core_mode);
static int _cpm4x25_port_pll_control_select(int unit, int port, portmod_cpm_core_mode_t core_mode);


STATIC
int _cpm4x25_pm_port_init(int unit, int port, pm_info_t pm_info, int enable)
{
    int phy_acc, i, rv;
    int tmp_port=0;
    int first_index = -1;
    uint32 reg_val;
    soc_field_t port_fields[] = {PORT0f, PORT1f, PORT2f, PORT3f};
    SOC_INIT_FUNC_DEFS;

    CPM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
       rv = PM4x25_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
       _SOC_IF_ERR_EXIT(rv);

       if (tmp_port == port) {
           first_index = i;
           break;
       }
   }

   if (first_index == -1) {
       _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
              (_SOC_MSG("port was not found in internal DB %d"), port));
   }

    if (enable) {
        /* Soft reset */
        _SOC_IF_ERR_EXIT(READ_CLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, port_fields[first_index], 1);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

        soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, port_fields[first_index], 0);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

        /* Port enable */
        _SOC_IF_ERR_EXIT(READ_CLPORT_ENABLE_REGr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &reg_val, port_fields[first_index], 1);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_ENABLE_REGr(unit, phy_acc, reg_val));

    } else {
        /* Port disable */
        _SOC_IF_ERR_EXIT(READ_CLPORT_ENABLE_REGr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &reg_val, port_fields[first_index], 0);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_ENABLE_REGr(unit, phy_acc, reg_val));

         /* Soft reset */
        _SOC_IF_ERR_EXIT(READ_CLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, port_fields[first_index], 1);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));
        _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_set(unit, port, 0));
    }

exit:
    SOC_FUNC_RETURN;
}



int cpm4x25_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int port_index = -1;
    int rv = 0;
    int phy, ii;
    int lane;
    int pll_sel=0;
    int pll_lock=0;
    int phys_count = 0, first_phy = -1;
    int three_ports_mode;
    uint32 pm_is_active = 0;
    uint32 pm_is_bypassed = 0;
    portmod_pbmp_t port_phys_in_pm;
    phymod_phy_access_t phy_access;
    phymod_core_init_config_t init_cfg;
    phymod_core_status_t core_sts;
    cprimod_port_init_config_t port_config;
    uint32 core_is_initialized;
    uint32 *pcs_lane_map;
    uint32 txpi_lane_select[MAX_PORTS_PER_PM4X25]={0};
    int pll0_active_lane_bitmap=0, pll1_active_lane_bitmap=0;
    int pcs_lane, vco_freq;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&init_cfg));
    _SOC_IF_ERR_EXIT(phymod_core_status_t_init(&core_sts));
    _SOC_IF_ERR_EXIT(cprimod_port_init_config_t_init(&port_config));
    _SOC_IF_ERR_EXIT(_cpm4x25_port_pll_control_select(unit, port, add_info->init_config.cpm_core_mode));
    /* select refclock for CPRI or ETH port */
    _SOC_IF_ERR_EXIT(_cpm4x25_port_refclk_select(unit, port, add_info->init_config.cpm_core_mode));

    if((add_info->interface_config.encap_mode != _SHR_PORT_ENCAP_CPRI) &&
      (add_info->interface_config.encap_mode != _SHR_PORT_ENCAP_RSVD4)){
        _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_set(unit, port, 0));
        _rv = pm4x25_port_attach(unit, port, pm_info, add_info);
        SOC_FUNC_RETURN;
    }
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_set(unit, port, 1));
    rv = PM4x25_IS_BYPASSED_GET(unit, pm_info, &pm_is_bypassed);
    _SOC_IF_ERR_EXIT(rv);
    /* Get the first phy related to this port */
    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, PM_4x25_INFO(pm_info)->phys);
    PORTMOD_PBMP_COUNT(port_phys_in_pm, phys_count);
    ii = 0;
    PORTMOD_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy){
        if(PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)){
            rv = PM4x25_LANE2PORT_SET(unit, pm_info, ii, port);
            _SOC_IF_ERR_EXIT(rv);
            first_phy = (first_phy == -1) ? phy : first_phy;
            port_index = (port_index == -1 ? ii : port_index);
        }
        ii++;
    }
    PM_4x25_INFO(pm_info)->first_phy = first_phy;
    if(port_index<0 || port_index>=PM4X25_LANES_PER_CORE) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("can't attach port %d. can't find relevant phy"), port));
    }
    /* init data for port configuration */
    /*
    rv = PM4x25_MAX_SPEED_SET(unit, pm_info, add_info->interface_config.max_speed, port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_INTERFACE_TYPE_SET(unit, pm_info, add_info->interface_config.interface, port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_INTERFACE_MODES_SET(unit, pm_info, add_info->interface_config.interface_modes, port_index);
    _SOC_IF_ERR_EXIT(rv);
    */
    if (add_info->init_config.polarity_overwrite == 1) {
        sal_memcpy(&PM_4x25_INFO(pm_info)->polarity,
            &add_info->init_config.polarity[0], sizeof(phymod_polarity_t));
    }
    if (add_info->init_config.lane_map_overwrite == 1) {
        sal_memcpy(&PM_4x25_INFO(pm_info)->lane_map,
            &add_info->init_config.lane_map[0], sizeof(phymod_lane_map_t));
    }
    if (add_info->init_config.fw_load_method_overwrite == 1) {
        PM_4x25_INFO(pm_info)->fw_load_method =
            add_info->init_config.fw_load_method[0];
    }
    if (add_info->init_config.ref_clk_overwrite == 1) {
        PM_4x25_INFO(pm_info)->ref_clk = add_info->init_config.ref_clk;
    }

    rv = PM4x25_IS_ACTIVE_GET(unit, pm_info, &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);
    if(!pm_is_active){
        three_ports_mode = (add_info->init_config.port_mode_aux_info == portmodModeInfoThreePorts) ? 1 : 0;
        rv = PM4x25_THREE_PORTS_MODE_SET(unit, pm_info, three_ports_mode);
        _SOC_IF_ERR_EXIT(rv);
        rv = pm4x25_to_cpm_pm_enable(unit, port, pm_info, port_index, add_info, 1);
        _SOC_IF_ERR_EXIT(rv);
        pm_is_active = 1;
        rv = PM4x25_IS_ACTIVE_SET(unit, pm_info, pm_is_active);
        _SOC_IF_ERR_EXIT(rv);
    }
    /*
    init_all = (!PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) ? 1 : 0;
    */
    /* No probe for CPM4x25 */
    if (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info)) {
        return (rv);
    }
    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)){
        CPRIMOD_CORE_INIT_F_EXECUTE_PASS1_SET(&init_cfg);
    }
    if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)){
        CPRIMOD_CORE_INIT_F_EXECUTE_PASS2_SET(&init_cfg);
    }

    if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        if (add_info->init_config.cpm_core_mode == portmodCpmMixEthCpri) {
           init_cfg.pll0_div_init_value = _cprimod_port_speed_id_to_vco(add_info->interface_config.speed, add_info->interface_config.encap_mode);
           init_cfg.pll1_div_init_value = 0;
        } else  {
            /* Need to add code for usage of both pll in all cpri mode */
            init_cfg.pll0_div_init_value = _cprimod_port_speed_id_to_vco(add_info->interface_config.speed, add_info->interface_config.encap_mode);
            init_cfg.pll1_div_init_value = _cprimod_port_speed_id_to_vco(add_info->interface_config.speed, add_info->interface_config.encap_mode);
        }
    }

    rv = PM4x25_IS_INITIALIZED_GET(unit, pm_info, &core_is_initialized);
    _SOC_IF_ERR_EXIT(rv);

    if (!PORTMOD_CORE_INIT_FLAG_CPRI_CORE_INITIALZIED_GET(core_is_initialized)) {
        if(!PORTMOD_CORE_INIT_FLAG_FIRMWARE_LOADED_GET(core_is_initialized) ||
            PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
            if ((PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) ||
               (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info))) {
                /*init_cfg.tx_input_voltage=cprimodTxInputVoltageDefault;*/
                init_cfg.firmware_loader = pm4x25_default_fw_loader;
                init_cfg.lane_map.num_of_lanes = PM_4x25_INFO(pm_info)->lane_map.num_of_lanes;
                for(ii=0; ii<CPRIMOD_MAX_LANES_PER_CORE; ii++) {
                    init_cfg.lane_map.lane_map_rx[ii] = PM_4x25_INFO(pm_info)->lane_map.lane_map_rx[ii];
                    init_cfg.lane_map.lane_map_tx[ii] = PM_4x25_INFO(pm_info)->lane_map.lane_map_tx[ii];
                }
            }

            _SOC_IF_ERR_EXIT(cprimod_core_init(unit, port, &PM_4x25_INFO(pm_info)->int_core_access,
                                           &init_cfg, &core_sts));

            /* Set txpi sdm scheme */
            _SOC_IF_ERR_EXIT(_pm4x25_txpi_sdm_scheme_set(unit, port, pm_info, add_info->init_config.txpi_sdm_scheme));
            /* Set txpi lane select */
            pcs_lane_map = (uint32 *)add_info->init_config.lane_map[0].lane_map_tx;
            for (pcs_lane = port_index; pcs_lane < (port_index + add_info->interface_config.port_num_lanes); pcs_lane++) {
                txpi_lane_select[pcs_lane_map[pcs_lane]] = port_index;
               _SOC_IF_ERR_EXIT(_pm4x25_txpi_lane_select_set(unit, port, pm_info, txpi_lane_select, pcs_lane_map[pcs_lane]));
            }

            /* No phy probe, set serdes type to Falcon_dpll for phy diag commands */
            if (add_info->init_config.cpm_core_mode == portmodCpmAllCpri) {
#ifdef PHYMOD_FALCON_DPLL_SUPPORT
                PM_4x25_INFO(pm_info)->int_core_access.type = phymodDispatchTypeFalcon_dpll;
#else
                PM_4x25_INFO(pm_info)->int_core_access.type = phymodDispatchTypeCount;
#endif
                rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                           pm_info->wb_vars_ids[phy_type],
                                           &PM_4x25_INFO(pm_info)->int_core_access.type, 0);
                _SOC_IF_ERR_EXIT(rv);
            }

            if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
                PORTMOD_CORE_INIT_FLAG_FIRMWARE_LOADED_SET(core_is_initialized);
                rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                            pm_info->wb_vars_ids[isInitialized],
                                            &core_is_initialized);
                _SOC_IF_ERR_EXIT(rv);
            }
            if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
                PORTMOD_CORE_INIT_FLAG_CPRI_CORE_INITIALZIED_SET(core_is_initialized);
                rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                            pm_info->wb_vars_ids[isInitialized],
                                            &core_is_initialized);
                _SOC_IF_ERR_EXIT(rv);
            }
        }
    }
    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        return (rv);
    }
    /* initalize port */

    port_config.serdes_scrambler_seed = add_info->interface_config.serdes_scrambler_seed;
    port_config.serdes_scrambler_enable = add_info->interface_config.serdes_scrambler_enable;
    port_config.pll_in_use = cprimodInUseDefault;
    rv = PM4x25_PLL0_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll0_active_lane_bitmap);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_PLL1_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll1_active_lane_bitmap);
    _SOC_IF_ERR_EXIT(rv);
    if (pll0_active_lane_bitmap != 0) {
        port_config.pll_in_use |= cprimodInUsePll0;
    }
    if (pll1_active_lane_bitmap != 0) {
        port_config.pll_in_use |= cprimodInUsePll1;
    }

    rv = _cpm4x25_pm_port_init(unit, port, pm_info, 1);
    if(add_info->interface_config.encap_mode == _SHR_PORT_ENCAP_CPRI) {
        /* coverity[MIXED_ENUMS:FALSE]*/
        switch((cprimod_supported_port_speed_t)add_info->interface_config.speed) {
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_1228P8:
            port_config.tx_cpri_speed  = cprimodSpd1228p8;
            port_config.rx_cpri_speed  = cprimodSpd1228p8;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_2457P6:
            port_config.tx_cpri_speed  = cprimodSpd2457p6;
            port_config.rx_cpri_speed  = cprimodSpd2457p6;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_3072P0:
            port_config.tx_cpri_speed  = cprimodSpd3072p0;
            port_config.rx_cpri_speed  = cprimodSpd3072p0;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_4915P2:
            port_config.tx_cpri_speed  = cprimodSpd4915p2;
            port_config.rx_cpri_speed  = cprimodSpd4915p2;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_6144P0:
            port_config.tx_cpri_speed  = cprimodSpd6144p0;
            port_config.rx_cpri_speed  = cprimodSpd6144p0;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_9830P4:
            port_config.tx_cpri_speed  = cprimodSpd9830p4;
            port_config.rx_cpri_speed  = cprimodSpd9830p4;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_10137P6:
            port_config.tx_cpri_speed  = cprimodSpd10137p6;
            port_config.rx_cpri_speed  = cprimodSpd10137p6;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_12165P12:
            port_config.tx_cpri_speed  = cprimodSpd12165p12;
            port_config.rx_cpri_speed  = cprimodSpd12165p12;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_24330P24:
            port_config.tx_cpri_speed  = cprimodSpd24330p24;
            port_config.rx_cpri_speed  = cprimodSpd24330p24;
            break;
        default:
            port_config.tx_cpri_speed  = cprimodSpd1228p8;
            port_config.rx_cpri_speed  = cprimodSpd1228p8;
            break;
        }
        port_config.tx_rsvd4_speed = cprimodRsvd4SpdMult4X;
        port_config.rx_rsvd4_speed = cprimodRsvd4SpdMult4X;
    } else if (add_info->interface_config.encap_mode == _SHR_PORT_ENCAP_RSVD4) {
        /* coverity[mixed_enums:FALSE]*/
        switch(add_info->interface_config.speed) {
        case cprimodRsvd4SpdMult4X:
            port_config.tx_rsvd4_speed = cprimodRsvd4SpdMult4X;
            port_config.rx_rsvd4_speed = cprimodRsvd4SpdMult4X;
            port_config.tx_cpri_speed  = cprimodSpd3072p0;
            port_config.rx_cpri_speed  = cprimodSpd3072p0;
            break;
        /* coverity[mixed_enums:FALSE]*/
        case cprimodRsvd4SpdMult8X:
            port_config.tx_rsvd4_speed = cprimodRsvd4SpdMult8X;
            port_config.rx_rsvd4_speed = cprimodRsvd4SpdMult8X;
            port_config.tx_cpri_speed  = cprimodSpd6144p0;
            port_config.rx_cpri_speed  = cprimodSpd6144p0;
            break;
        default:
            port_config.tx_rsvd4_speed = cprimodRsvd4SpdMult4X;
            port_config.rx_rsvd4_speed = cprimodRsvd4SpdMult4X;
            port_config.tx_cpri_speed  = cprimodSpd3072p0;
            port_config.rx_cpri_speed  = cprimodSpd3072p0;
            break;
        }
    }
    if (add_info->interface_config.encap_mode == _SHR_PORT_ENCAP_RSVD4) {
        port_config.interface = cprimodRsvd4;
    } else {
        port_config.interface = cprimodCpri;
    }
    port_config.roe_stuffing_bit = add_info->init_config.roe_stuffing_bit;
    port_config.roe_reserved_bit = add_info->init_config.roe_reserved_bit;

    sal_memcpy(&phy_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_core_access_t));
    lane = (SOC_INFO(unit).port_l2p_mapping[port]-1)%CPRIMOD_MAX_LANES_PER_CORE;
    port_config.polarity_rx = ((PM_4x25_INFO(pm_info)->polarity.rx_polarity) >> lane) & 1;
    port_config.polarity_tx = ((PM_4x25_INFO(pm_info)->polarity.tx_polarity) >> lane) & 1;
    phy_access.access.lane_mask = (1<<lane);
    if(add_info->init_config.cpm_core_mode == portmodCpmMixEthCpri) {
        port_config.mix_cpri_enet = 1;
    } else {
        port_config.mix_cpri_enet = 0;
    }
    port_config.vco_for_lane = _cprimod_port_speed_id_to_vco(add_info->interface_config.speed, add_info->interface_config.encap_mode);

    if (PORTMOD_USER_SET_TX_PREEMPHASIS_BY_CONFIG_GET(add_info->init_config.tx_params_user_flag[0])) {
        port_config.tx_params[0].pre = add_info->init_config.tx_params[0].pre;
        port_config.tx_params[0].main = add_info->init_config.tx_params[0].main;
        port_config.tx_params[0].post = add_info->init_config.tx_params[0].post;
        port_config.tx_params_user_config = 1;
    }

    port_config.parity_enable = add_info->init_config.parity_enable;

    _SOC_IF_ERR_EXIT(cprimod_port_init(unit, port, &phy_access, &port_config));

    _SOC_IF_ERR_EXIT(cprimod_cpri_pll_status_get(unit, port, &phy_access, &pll_sel, &pll_lock));
    if(pll_sel == 0) {
        rv = PM4x25_PLL0_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll0_active_lane_bitmap);
        _SOC_IF_ERR_EXIT(rv);
        pll0_active_lane_bitmap |= (phy_access.access.lane_mask);
        rv = PM4x25_PLL0_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll0_active_lane_bitmap);
        _SOC_IF_ERR_EXIT(rv);
        vco_freq = _cprimod_pll_mult_to_vco_freq(port_config.vco_for_lane);
        rv = PM4x25_PLL0_VCO_RATE_SET(unit, pm_info, vco_freq);
        _SOC_IF_ERR_EXIT(rv);
    } else {
        rv = PM4x25_PLL1_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll1_active_lane_bitmap);
        _SOC_IF_ERR_EXIT(rv);
        pll1_active_lane_bitmap |= (phy_access.access.lane_mask);
        rv = PM4x25_PLL1_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll1_active_lane_bitmap);
        _SOC_IF_ERR_EXIT(rv);
        vco_freq = _cprimod_pll_mult_to_vco_freq(port_config.vco_for_lane);
        rv = PM4x25_PLL1_VCO_RATE_SET(unit, pm_info, vco_freq);
        _SOC_IF_ERR_EXIT(rv);
    }
    /* Set PMD TXPI mode */
    _SOC_IF_ERR_EXIT(_pm4x25_port_phy_txpi_mode_set(unit, port, pm_info, add_info->init_config.txpi_mode));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;
    _rv = pm4x25_pm_init(unit, pm_add_info, wb_buffer_index, pm_info);

    SOC_FUNC_RETURN;

}

/*  CPM core supports ETH/CPRI mixed mode, also supports flex between ETH and CPRI.
 *  CPRI port use PLL0. refclock select pll0_lc_refclk (0).
 *  ETH port use PLL0 and PLL1. refclock select pll1_lc_refclk (1).
 *  This function is used to select reference clock of PLL0.
 */
static int _cpm4x25_port_refclk_select(int unit, int port, portmod_cpm_core_mode_t core_mode)
{
    uint32 rval;
    int refclock0 = 0;
    int refclock1 = 0;

    switch(core_mode) {
    case portmodCpmAllCpri:
        refclock0 = 0;
        refclock1 = 1;
        break;
    case portmodCpmAllIEEE:
    case portmodCpmAllHIGIG:
        refclock0 = 1;
        refclock1 = 0;
        break;
    case portmodCpmMixEthCpri:
        refclock0 = 0;
        refclock1 = 0;
        break;
    default:
        refclock0 = 0;
        refclock1 = 1;
        break;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, CLPORT_XGXS0_CTRL_REGr, port, 0, &rval));
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &rval, PLL0_LC_REFSELf, refclock0);
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &rval, PLL1_LC_REFSELf, refclock1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, CLPORT_XGXS0_CTRL_REGr, port, 0, rval));

    return SOC_E_NONE;
}

/*  CPM core supports ETH/CPRI mixed mode, also supports flex between ETH and CPRI.
 *  in all CPRI mode both pll control through CPRI path, in all Ethernet mode both
 *  pll control through Ethernet path. In Mix mode pll0 through CPRI path and pll1
 *  through Ethernet path.
 */
static int _cpm4x25_port_pll_control_select(int unit, int port, portmod_cpm_core_mode_t core_mode)
{
    uint32 data;
    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    switch(core_mode) {
    case portmodCpmAllCpri:
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, CIP_PLL0_CTRLf, 1);
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, CIP_PLL1_CTRLf, 1);
        break;
    case portmodCpmAllIEEE:
    case portmodCpmAllHIGIG:
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, CIP_PLL0_CTRLf, 0);
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, CIP_PLL1_CTRLf, 0);
        break;
    case portmodCpmMixEthCpri:
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, CIP_PLL0_CTRLf, 1);
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, CIP_PLL1_CTRLf, 0);
        break;
    default:
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, CIP_PLL0_CTRLf, 0);
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, CIP_PLL1_CTRLf, 0);
        break;
    }
    SOC_IF_ERROR_RETURN(WRITE_TOP_CTRL_CONFIGr(unit, port, data));
    return SOC_E_NONE;
}

int cpm4x25_port_phy_link_up_event(int unit, int port, pm_info_t pm_info)
{
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_phy_link_up_event(unit, port, pm_info);
        SOC_FUNC_RETURN;
    } else {
        _rv = cprimod_cpri_port_link_up_event(unit, port);
        SOC_FUNC_RETURN;
    }
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_phy_link_down_event(int unit, int port, pm_info_t pm_info)
{
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_phy_link_down_event(unit, port, pm_info);
        SOC_FUNC_RETURN;
    } else {
        _rv = cprimod_cpri_port_link_down_event(unit, port);
        SOC_FUNC_RETURN;
    }
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int* enable)
{
    int val;
    int phy_enable;
    int actual_flags = flags;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_enable_get(unit, port, pm_info, flags, enable);
        SOC_FUNC_RETURN;
    }
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
        PORTMOD_PORT_ENABLE_RX_SET(actual_flags);
        PORTMOD_PORT_ENABLE_TX_SET(actual_flags);
    }
    *enable = 0;
    if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
        _SOC_IF_ERR_EXIT(cprimod_cpri_port_rx_enable_get(unit, port, &phy_enable));
        *enable |= phy_enable;
    }
    if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
        _SOC_IF_ERR_EXIT(cprimod_cpri_port_tx_enable_get(unit, port, &phy_enable));
        *enable |= phy_enable;
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_detach(int unit, int port, pm_info_t pm_info)
{
    int val;
    int enable;
    int invalid_port = -1;
    int tmp_port;
    int i = 0, flags = 0;
    int rv = 0;
    int lane;
    int pll0_active_lane_bitmap=0, pll1_active_lane_bitmap=0;
    int is_last_one = TRUE;
    int port_index = -1;
    int phytype = 0;
    int vco_freq;
    uint32 inactive = 0;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_detach(unit, port, pm_info);
        SOC_FUNC_RETURN;
    }

    _SOC_IF_ERR_EXIT(cpm4x25_port_enable_get(unit, port, pm_info, flags, &enable));
    if(enable){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't detach active port %d"), port));
    }
    _SOC_IF_ERR_EXIT(cprimod_cpri_port_reset_set(unit, port, 1));

    rv = _cpm4x25_pm_port_init(unit, port, pm_info, 0);
    _SOC_IF_ERR_EXIT(rv);

    /*remove from array and check if it was the last one*/
    for( i = 0 ; i < MAX_PORTS_PER_PM4X25; i++){
        rv = PM4x25_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        _SOC_IF_ERR_EXIT(rv);
        if(tmp_port == port){
            port_index = (port_index == -1 ? i : port_index);
            rv = PM4x25_LANE2PORT_SET(unit, pm_info, i, invalid_port);
            _SOC_IF_ERR_EXIT(rv);
        } else if (tmp_port != -1){
            is_last_one = FALSE;
        }
    }

    if ((PM_4x25_INFO(pm_info)->nof_phys[0]) && (port_index == -1))  {
        _SOC_EXIT_WITH_ERR(SOC_E_PORT, (_SOC_MSG("Port %d wasn't found"), port));
    }

    /*deinit PM in case of last one*/
    if(is_last_one){
       rv = pm4x25_to_cpm_pm_enable(unit, port, pm_info, port_index, NULL, 0);
       _SOC_IF_ERR_EXIT(rv);

       PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_CLR(PM_4x25_INFO(pm_info)->int_core_access.device_op_mode);

       rv = PM4x25_IS_ACTIVE_SET(unit, pm_info, inactive);
       _SOC_IF_ERR_EXIT(rv);

       rv = PM4x25_IS_INITIALIZED_SET(unit, pm_info, inactive);
       _SOC_IF_ERR_EXIT(rv);

       rv = PM4x25_IS_BYPASSED_SET(unit, pm_info, inactive);
       _SOC_IF_ERR_EXIT(rv);

       /* Updating phymod type in pm_info structure and warmboot engine */
       phytype = phymodDispatchTypeInvalid;
       rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phy_type], &phytype, 0);
       _SOC_IF_ERR_EXIT(rv);
       PM_4x25_INFO(pm_info)->int_core_access.type = phymodDispatchTypeInvalid;
       /* All ports detached the PM will be resetted vco resetted */
       vco_freq = -1; 
       rv = PM4x25_PLL0_VCO_RATE_SET(unit, pm_info, vco_freq);
       _SOC_IF_ERR_EXIT(rv);
       rv = PM4x25_PLL1_VCO_RATE_SET(unit, pm_info, vco_freq);
       _SOC_IF_ERR_EXIT(rv);
    }
    lane = (SOC_INFO(unit).port_l2p_mapping[port]-1)%CPRIMOD_MAX_LANES_PER_CORE;
    rv = PM4x25_PLL0_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll0_active_lane_bitmap);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_PLL1_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll1_active_lane_bitmap);
    _SOC_IF_ERR_EXIT(rv);
    pll0_active_lane_bitmap &= ~(1<<lane);
    pll1_active_lane_bitmap &= ~(1<<lane);
    rv = PM4x25_PLL0_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll0_active_lane_bitmap);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_PLL1_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll1_active_lane_bitmap);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_replace(int unit, int port, pm_info_t pm_info, int new_port)
{
    int val;
    int i, tmp_port;
    int rv = 0;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_replace(unit, port, pm_info, new_port);
        SOC_FUNC_RETURN;
    }

    /* replace old port with new port */
    for(i = 0; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        _SOC_IF_ERR_EXIT(rv);
        if (tmp_port == port) {
            rv = PM4x25_LANE2PORT_SET(unit, pm_info, i, new_port);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
    int val;
    int actual_flags = flags;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_enable_set(unit, port, pm_info, flags, enable);
        SOC_FUNC_RETURN;
    }
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
        PORTMOD_PORT_ENABLE_RX_SET(actual_flags);
        PORTMOD_PORT_ENABLE_TX_SET(actual_flags);
    }
    /* if no MAC\Phy flags - set both*/
    if((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) && (!PORTMOD_PORT_ENABLE_MAC_GET(flags))){
        PORTMOD_PORT_ENABLE_PHY_SET(actual_flags);
        PORTMOD_PORT_ENABLE_MAC_SET(actual_flags);
    }

    if ((PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) &&
        (PORTMOD_PORT_ENABLE_MAC_GET(actual_flags))) {
        _SOC_IF_ERR_EXIT(cprimod_cpri_port_rx_enable_set(unit, port, enable));
    }
    if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
        _SOC_IF_ERR_EXIT(cprimod_cpri_port_tx_enable_set(unit, port, enable));
        /*
         * if disable.  disable TGEN as well.
         */
        if (!enable) {
            _SOC_IF_ERR_EXIT
                (cprimod_tx_framer_tgen_enable(unit, port, FALSE));
            _SOC_IF_ERR_EXIT
                (cprimod_cpri_tx_datapath_reset(unit, port));
        }

    }

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_link_get(int unit, int port, pm_info_t pm_info, int flags, int* link)
{
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_link_get(unit, port, pm_info, flags, link);
        SOC_FUNC_RETURN;
    }

    /* get CPRI port link status. */
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_link_get(unit, port, link));
exit:

    SOC_FUNC_RETURN;

}

int cpm4x25_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    int val;
    int lane;
    phymod_loopback_mode_t loopback;
    phymod_phy_access_t phy_access;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {

        _rv = pm4x25_port_loopback_set(unit, port, pm_info, loopback_type, enable);
        SOC_FUNC_RETURN;
    } else {
        switch(loopback_type) {
        case portmodLoopbackPhyRloopPMD:
            loopback = phymodLoopbackRemotePMD;
            break;
        case portmodLoopbackPhyGloopPMD:
            loopback = phymodLoopbackGlobalPMD;
            break;
        case portmodLoopbackPhyGloopPCS:   /* No PCS loopback doing PMD loopback */
            loopback = phymodLoopbackGlobalPMD;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("Loopback type %d. is not supported for CPRI"), loopback_type));

        }
        sal_memcpy(&phy_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_core_access_t));
        lane = (SOC_INFO(unit).port_l2p_mapping[port]-1)%CPRIMOD_MAX_LANES_PER_CORE;
        phy_access.access.lane_mask = (1<<lane);
        _SOC_IF_ERR_EXIT(tscf_gen3_phy_loopback_set(&phy_access, loopback, enable));
    }

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_loopback_get(int unit, int port, pm_info_t pm_info,
                 portmod_loopback_mode_t loopback_type, int* enable)
{
    int val;
    int lane;
    uint32_t en;
    phymod_loopback_mode_t loopback;
    phymod_phy_access_t phy_access;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_loopback_get(unit, port, pm_info, loopback_type, enable);
        SOC_FUNC_RETURN;
    } else {
        switch(loopback_type) {
        case portmodLoopbackPhyRloopPMD:
            loopback = phymodLoopbackRemotePMD;
            break;
        case portmodLoopbackPhyGloopPMD:
            loopback = phymodLoopbackGlobalPMD;
            break;
        case portmodLoopbackPhyGloopPCS:   /* No PCS loopback doing PMD loopback */
            loopback = phymodLoopbackGlobalPMD;
            break;
        default:
            loopback = phymodLoopbackGlobal;
            break;
        }
        sal_memcpy(&phy_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_core_access_t));
        lane = (SOC_INFO(unit).port_l2p_mapping[port]-1)%CPRIMOD_MAX_LANES_PER_CORE;
        phy_access.access.lane_mask = (1<<lane);
        _SOC_IF_ERR_EXIT(tscf_gen3_phy_loopback_get(&phy_access, loopback, &en));
        *enable = en;
    }

exit:
    SOC_FUNC_RETURN;

}
int cpm4x25_port_autoneg_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags,
                            const phymod_autoneg_control_t* an) {
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_autoneg_set(unit, port, pm_info, phy_flags, an);
        SOC_FUNC_RETURN;
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_autoneg_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, phymod_autoneg_control_t* an) {
    int val;
    SOC_INIT_FUNC_DEFS;
    an->enable = 0;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_autoneg_get(unit, port, pm_info, phy_flags, an);
        SOC_FUNC_RETURN;
    }

exit:

    SOC_FUNC_RETURN;
}
int cpm4x25_port_ability_advert_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags,
                                 portmod_port_ability_t* ability) {
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_ability_advert_get(unit, port, pm_info, phy_flags, ability);
        SOC_FUNC_RETURN;
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_ability_advert_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags,
                                 portmod_port_ability_t* ability) {
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_ability_advert_set(unit, port, pm_info, phy_flags, ability);
        SOC_FUNC_RETURN;
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_autoneg_status_get (int unit, int port, pm_info_t pm_info,
                                    phymod_autoneg_status_t* an_status) {
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_autoneg_status_get(unit, port, pm_info, an_status);
        SOC_FUNC_RETURN;
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_ability_remote_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags,
                                       portmod_port_ability_t* ability) {
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_ability_remote_get(unit, port, pm_info, phy_flags, ability);
        SOC_FUNC_RETURN;
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_encap_set(int unit, int port, pm_info_t pm_info, int flags, portmod_encap_t encap)
{
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_encap_set(unit, port, pm_info, flags, encap);
        SOC_FUNC_RETURN;
    }

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_encap_get(int unit, int port, pm_info_t pm_info, int* flags, portmod_encap_t* encap)
{
    int val;
    cprimod_port_interface_config_t config;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_encap_get(unit, port, pm_info, flags, encap);
        SOC_FUNC_RETURN;
    } else {
        _SOC_IF_ERR_EXIT
            (cprimod_port_rx_interface_config_get(unit, port, &config));
        if (config.interface == cprimodCpri) {
            *encap = _SHR_PORT_ENCAP_CPRI;
        } else if (config.interface == cprimodRsvd4) {
            *encap = _SHR_PORT_ENCAP_RSVD4;
        } else if (config.interface == cprimodTunnel) {
            if (config.port_encap_type == cprimodPortEncapCpri){
                *encap = _SHR_PORT_ENCAP_CPRI;
            } else {
                *encap =  _SHR_PORT_ENCAP_RSVD4;
            }
        } else {
            *encap = _SHR_PORT_ENCAP_COUNT;
        }
    }
exit:
    SOC_FUNC_RETURN;

}

static int
_cprimod_port_speed_t_to_int (
    cprimod_port_speed_t speedi)
{
    int speedo;

    switch(speedi) {
    case cprimodSpd1228p8:
        speedo = 1228;
        break;
    case cprimodSpd2457p6:
        speedo = 2457;
        break;
    case cprimodSpd3072p0:
        speedo = 3072;
        break;
    case cprimodSpd4915p2:
        speedo = 4915;
        break;
    case cprimodSpd6144p0:
        speedo = 6144;
        break;
    case cprimodSpd9830p4:
        speedo = 9830;
        break;
    case cprimodSpd10137p6:
        speedo = 10137;
        break;
    case cprimodSpd12165p12:
        speedo = 12165;
        break;
    case cprimodSpd24330p24:
        speedo = 24330;
        break;
    default:
        speedo = 1228;
        break;
    }

    return speedo;
}

static cprimod_port_speed_t
_int_to_cprimod_port_speed_t (
    int speedi)
{
    cprimod_port_speed_t speedo;
    switch(speedi) {
    case 1228:
        speedo = cprimodSpd1228p8;
        break;
    case 2457:
        speedo = cprimodSpd2457p6;
        break;
    case 3072:
        speedo = cprimodSpd3072p0;
        break;
    case 4915:
        speedo = cprimodSpd4915p2;
        break;
    case 6144:
        speedo = cprimodSpd6144p0;
        break;
    case 9830:
        speedo = cprimodSpd9830p4;
        break;
    case 10137:
        speedo = cprimodSpd10137p6;
        break;
    case 12165:
        speedo = cprimodSpd12165p12;
        break;
    case 24330:
        speedo = cprimodSpd24330p24;
        break;
    default:
        speedo = cprimodSpd1228p8;
        break;
    }

    return speedo;
}

static cprimod_supported_port_speed_t
_int_to_cprimod_supported_port_speed_t (
    int speedi)
{
    cprimod_supported_port_speed_t speedo;
    switch(speedi) {
    case 1228:
        speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_1228P8;
        break;
    case 2457:
        speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_2457P6;
        break;
    case 3072:
        speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_3072P0;
        break;
    case 4915:
        speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_4915P2;
        break;
    case 6144:
        speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_6144P0;
        break;
    case 9830:
        speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_9830P4;
        break;
    case 10137:
        speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_10137P6;
        break;
    case 12165:
        speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_12165P12;
        break;
    case 24330:
        speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_24330P24;
        break;
    default:
        /* 
         * INVALID Speed.
         */
        speedo = -1;
        break;
    }

    return speedo;
}

static int
_cprimod_port_speed_id_to_vco (
   int speed_id, portmod_encap_t encap)
{
    int vco_value;
    int speed_loc;
    if(encap == _SHR_PORT_ENCAP_CPRI) {
        if(speed_id >= 1228) {
            speed_loc = _int_to_cprimod_supported_port_speed_t(speed_id);
        } else {
            speed_loc = speed_id;
        }
        switch(speed_loc) {
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_1228P8:
            vco_value = phymod_TSCF_PLL_DIV160;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_2457P6:
            vco_value = phymod_TSCF_PLL_DIV160;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_3072P0:
            vco_value = phymod_TSCF_PLL_DIV200;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_4915P2:
            vco_value = phymod_TSCF_PLL_DIV160;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_6144P0:
            vco_value = phymod_TSCF_PLL_DIV200;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_9830P4:
            vco_value = phymod_TSCF_PLL_DIV160;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_10137P6:
            vco_value = phymod_TSCF_PLL_DIV165;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_12165P12:
            vco_value = phymod_TSCF_PLL_DIV198;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_24330P24:
            vco_value = phymod_TSCF_PLL_DIV198;
            break;
        default:
            vco_value = phymod_TSCF_PLL_DIV160;
            break;
        }
    } else {
        vco_value = phymod_TSCF_PLL_DIV200;
    }

    return vco_value;
}

static int
_cprimod_pll_mult_to_vco_freq ( int pll_mult) {

    int vco_value;
    switch(pll_mult) {
    case phymod_TSCF_PLL_DIV160 :
        vco_value = portmodVCO19P660G;
        break;
    case phymod_TSCF_PLL_DIV165 :
        vco_value = portmodVCO20P275G;
        break;
    case phymod_TSCF_PLL_DIV198 :
        vco_value = portmodVCO24P330G;
        break;
    case phymod_TSCF_PLL_DIV200 :
        vco_value = portmodVCO24P576G;
        break;
    default :
        vco_value = -1;
        break;
    }
    return vco_value;
}


int cpm4x25_port_interface_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, int phy_init_flags)
{
    int val;
    int lane;
    phymod_phy_access_t phy_access;
    cprimod_port_interface_config_t config_t;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_interface_config_set(unit, port, pm_info, config, phy_init_flags);
        SOC_FUNC_RETURN;
    }
    sal_memcpy(&phy_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_core_access_t));
    lane = (SOC_INFO(unit).port_l2p_mapping[port]-1)%CPRIMOD_MAX_LANES_PER_CORE;
    phy_access.access.lane_mask = (1<<lane);
    config_t.speed = _int_to_cprimod_port_speed_t(config->speed);
    config_t.interface = cprimodCpri;
    if (phy_init_flags == PORTMOD_INIT_F_TX_ONLY) {
        _SOC_IF_ERR_EXIT(cprimod_port_tx_interface_config_set(unit, port, &phy_access,
                                                              &config_t));
    }
    if (phy_init_flags == PORTMOD_INIT_F_RX_ONLY) {
        _SOC_IF_ERR_EXIT(cprimod_port_rx_interface_config_set(unit, port, &phy_access,
                                                              &config_t));
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config, int phy_init_flags)
{
    int val;
    cprimod_port_interface_config_t config_t;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
       _rv = pm4x25_port_interface_config_get(unit, port, pm_info, config, phy_init_flags);
        SOC_FUNC_RETURN;
    }
    if (phy_init_flags == PORTMOD_INIT_F_TX_ONLY) {
        _SOC_IF_ERR_EXIT(cprimod_port_tx_interface_config_get(unit, port,
                                                              &config_t));
    } else {
        _SOC_IF_ERR_EXIT(cprimod_port_rx_interface_config_get(unit, port,
                                                              &config_t));
    }
    config->speed = _cprimod_port_speed_t_to_int (config_t.speed);

    /*
     *  encap_mode is tunnel is the mode that port will go back to
     *  when tunnel is disabled.
     */
    if (config_t.interface == cprimodTunnel) {
        if (config_t.port_encap_type == cprimodPortEncapCpri){
            config->encap_mode =  _SHR_PORT_ENCAP_CPRI;
        } else {
            config->encap_mode =  _SHR_PORT_ENCAP_RSVD4;
        }
        config->interface = SOC_PORT_IF_TUNNEL;
    } else if (config_t.interface == cprimodCpri) {
        config->encap_mode = _SHR_PORT_ENCAP_CPRI;
        config->interface = SOC_PORT_IF_CPRI;
    } else if (config_t.interface == cprimodRsvd4) {
        config->encap_mode = _SHR_PORT_ENCAP_RSVD4;
        config->interface = SOC_PORT_IF_RSVD4;
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_speed_get(int unit, int port, pm_info_t pm_info, int* speed)
{
    int val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _SOC_IF_ERR_EXIT(pm4x25_port_speed_get(unit, port, pm_info, speed));
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_rsvd4_rx_frame_optional_config_set(int unit, int port, pm_info_t pm_info, cprimod_rsvd4_rx_config_field_t field, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_rx_frame_optional_config_set(unit, port,
                                                                     field, value));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_rsvd4_rx_frame_optional_config_get(int unit, int port, pm_info_t pm_info, cprimod_rsvd4_rx_config_field_t field, int* value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_rx_frame_optional_config_get(unit, port,
                                                                     field, value));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_rsvd4_tx_frame_optional_config_set(int unit, int port, pm_info_t pm_info, cprimod_rsvd4_tx_config_field_t field, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_tx_frame_optional_config_set(unit, port,
                                                                     field, value));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_rsvd4_tx_frame_optional_config_get(int unit, int port, pm_info_t pm_info, cprimod_rsvd4_tx_config_field_t field, int* value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_tx_frame_optional_config_get(unit, port,
                                                                     field, value));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_rx_frame_optional_config_set(int unit, int port, pm_info_t pm_info, cprimod_rx_config_field_t field, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rx_frame_optional_config_set(unit, port, field,
                                                               value));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_rx_frame_optional_config_get(int unit, int port, pm_info_t pm_info, cprimod_rx_config_field_t field, int* value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rx_frame_optional_config_get(unit, port, field,
                                                               value));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_tx_frame_optional_config_set(int unit, int port, pm_info_t pm_info, cprimod_tx_config_field_t field, uint32 value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_tx_frame_optional_config_set(unit, port, field,
                                                               value));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_tx_frame_optional_config_get(int unit, int port, pm_info_t pm_info, cprimod_tx_config_field_t field, uint32* value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_tx_frame_optional_config_get(unit, port, field,
                                                               value));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_stage_activate(int unit, int port, pm_info_t pm_info, portmod_cpri_stage_t stage)
{
    cprimod_basic_frame_table_id_t table;
    int sync_enable = TRUE;
    SOC_INIT_FUNC_DEFS;

    if (stage == portmodCpriStageRx) {
        _SOC_IF_ERR_EXIT
            (cprimod_basic_frame_parser_active_table_get(unit, port,&table));
        if (table == cprimod_basic_frame_table_0) {
            _SOC_IF_ERR_EXIT
                (cprimod_basic_frame_parser_active_table_set(unit, port,cprimod_basic_frame_table_1, sync_enable));
        } else {
            _SOC_IF_ERR_EXIT
                (cprimod_basic_frame_parser_active_table_set(unit, port,cprimod_basic_frame_table_0, sync_enable));
        }
    } else {
            _SOC_IF_ERR_EXIT
                (cprimod_basic_frame_assembly_active_table_get(unit, port,&table));
        if (table == cprimod_basic_frame_table_0) {
            _SOC_IF_ERR_EXIT
                (cprimod_basic_frame_assembly_active_table_set(unit, port,cprimod_basic_frame_table_1, sync_enable));
        } else {
            _SOC_IF_ERR_EXIT
                (cprimod_basic_frame_assembly_active_table_set(unit, port,cprimod_basic_frame_table_0, sync_enable));
        }
    }
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_rsvd4_rx_speed_set(int unit, int port, pm_info_t pm_info, cprimod_port_rsvd4_speed_mult_t speed)
{
    int lane;
    phymod_phy_access_t phy_access;
    SOC_INIT_FUNC_DEFS;

    sal_memcpy(&phy_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_core_access_t));
    lane = (SOC_INFO(unit).port_l2p_mapping[port]-1)%CPRIMOD_MAX_LANES_PER_CORE;
    phy_access.access.lane_mask = (1<<lane);
    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_rx_speed_set(unit, port, &phy_access, speed));
    _SOC_IF_ERR_EXIT
        (cprimod_basic_frame_parser_switch_active_table(unit, port));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_rsvd4_rx_speed_get(int unit, int port, pm_info_t pm_info, cprimod_port_rsvd4_speed_mult_t* speed)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_rx_speed_get(unit, port, speed));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_port_rsvd4_tx_speed_set(int unit, int port, pm_info_t pm_info, cprimod_port_rsvd4_speed_mult_t speed)
{
    int lane;
    phymod_phy_access_t phy_access;
    SOC_INIT_FUNC_DEFS;

    sal_memcpy(&phy_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_core_access_t));
    lane = (SOC_INFO(unit).port_l2p_mapping[port]-1)%CPRIMOD_MAX_LANES_PER_CORE;
    phy_access.access.lane_mask = (1<<lane);
    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_tx_speed_set(unit, port, &phy_access, speed));
    _SOC_IF_ERR_EXIT
         (cprimod_basic_frame_assembly_switch_active_table(unit, port));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_rsvd4_tx_speed_get(int unit, int port, pm_info_t pm_info, cprimod_port_rsvd4_speed_mult_t* speed)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_tx_speed_get(unit, port, speed));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_rsvd4_tx_fsm_state_set(int unit, int port, pm_info_t pm_info, cprimod_rsvd4_tx_fsm_state_t state)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_tx_fsm_state_set(unit, port, state));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_rx_fsm_state_set(int unit, int port, pm_info_t pm_info, cprimod_rsvd4_rx_fsm_state_t state)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_rx_fsm_state_set(unit, port, state));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_tx_override_set(int unit, int port, pm_info_t pm_info, cprimod_rsvd4_tx_overide_t parameter, int enable, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_tx_overide_set(unit, port, parameter,
                                                        enable, value));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_rx_override_set(int unit, int port, pm_info_t pm_info, cprimod_rsvd4_rx_overide_t parameter, int enable, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_rx_overide_set(unit, port, parameter,
                                                        enable, value));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_axc_frame_add(int unit, int port, pm_info_t pm_info, uint32 axc_id, portmod_cpri_axc_frame_info_t* frame_info)
{
    uint32 queue_num = 0xFF;
    uint32 num_axc ;
    uint32 num_container;
    uint32 num_bits_per_container;
    int offset;
    SOC_INIT_FUNC_DEFS;

    num_axc                 = 1;
    num_container           = 1;
    num_bits_per_container  = frame_info->num_bits;

    if (frame_info->container_info.map_method == cprimod_cpri_frame_map_method_3) {
        num_axc         = frame_info->container_info.Na;
        num_container   = frame_info->container_info.Nc;
        if (frame_info->num_bits != ( 2 * frame_info->encap_data_info.sample_size * num_container)) {
            return SOC_E_CONFIG;
        }
    }

    if ((num_container > 1) &&
         (num_axc > 1)) {
        if ((frame_info->num_bits % num_container) != 0) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("Number of Bits %d cannot be equally divided between Nc %d.\n"),
                          frame_info->num_bits,
                          num_container));
        }

        num_bits_per_container = frame_info->num_bits / num_container;
        for (offset = 0; offset < num_container; offset++) {
            _SOC_IF_ERR_EXIT
                (cprimod_rx_axc_basic_frame_add(unit, port, axc_id,
                                                frame_info->start_bit+(offset*num_bits_per_container),
                                                num_bits_per_container));
        }
    } else {
        _SOC_IF_ERR_EXIT
            (cprimod_rx_axc_basic_frame_add(unit, port, axc_id, frame_info->start_bit, frame_info->num_bits));
    }
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_rx_axc_container_config_set(unit, port, axc_id, &frame_info->container_info));

    /*
     * In data AxC, the AxC number and Queue number are identical.
     */
    queue_num = axc_id;

    for (offset = 0; offset < num_axc ; offset++) {
        _SOC_IF_ERR_EXIT
            (cprimod_rx_roe_payload_size_set(unit, port, axc_id+offset, &frame_info->roe_payload_info));
        _SOC_IF_ERR_EXIT
            (cprimod_cpri_encap_data_config_set(unit, port, queue_num+offset, &frame_info->encap_data_info ));
    }

    /* activate the basic frame to take the new configuration. */
    _SOC_IF_ERR_EXIT
        (cpm4x25_cpri_port_stage_activate(unit, port, pm_info, portmodCpriStageRx));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_axc_frame_delete(int unit, int port, pm_info_t pm_info, uint32 axc_id)
{
    cprimod_encap_decap_data_config_t encap_data_config;
    cprimod_payload_size_info_t roe_payload_info;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rx_axc_basic_frame_delete(unit, port, axc_id));

    /* activate the basic frame to take the new configuration. */
    _SOC_IF_ERR_EXIT
        (cpm4x25_cpri_port_stage_activate(unit, port, pm_info, portmodCpriStageRx));

    /* Flush encap memory and framer state. */
    _SOC_IF_ERR_EXIT
        (cprimod_encap_memory_clear(unit, port, axc_id));

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_rx_framer_state_clear(unit, port, axc_id));

    /* clear the relate container parser entry. */

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_rx_axc_container_config_clear(unit, port, axc_id));

    /* clear encap data configuration. */
    _SOC_IF_ERR_EXIT
        (cprimod_encap_decap_data_config_t_init(&encap_data_config));

    /* Need to set agnostic_mode to 1 not to run thru allocation */
    encap_data_config.is_agnostic = 1;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_set(unit, port, axc_id, &encap_data_config ));

    /* clear payload info. */
   _SOC_IF_ERR_EXIT
        (cprimod_payload_size_info_t_init(&roe_payload_info));

    _SOC_IF_ERR_EXIT
        (cprimod_rx_roe_payload_size_set(unit, port, axc_id, &roe_payload_info));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_rx_axc_frame_get(int unit, int port, pm_info_t pm_info, uint32 axc_id, uint32 from_get_all, portmod_cpri_axc_frame_info_t* frame_info)
{
    uint32 queue_num = 0xFF;
    cprimod_basic_frame_table_id_t active_table;
    cprimod_basic_frame_usage_entry_t* usage_table=NULL;
    int num_entries = 0;
    uint32 start_bit = 0;
    uint32 num_bits = 0;
    uint32 Naxc = 0;
    uint32 num_container = 0;
    int i;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_basic_frame_parser_active_table_get(unit, port, &active_table));


    usage_table = (cprimod_basic_frame_usage_entry_t *) sal_alloc
                                   ((sizeof(cprimod_basic_frame_table_id_t)*CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY),
                                    "cpm4x25");

    SOC_NULL_CHECK(usage_table);
    num_entries = CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY;


    _SOC_IF_ERR_EXIT
        (cprimod_rx_axc_basic_frame_get(unit, port, axc_id, active_table, usage_table, &num_entries));

    start_bit = usage_table[0].start_bit;

    for (i=0;i<num_entries;i++) {
        num_bits = num_bits + usage_table[i].num_bits;
    }

    if (num_entries == 0) {
        if (from_get_all) {
           frame_info->start_bit = 0;
           frame_info->num_bits = 0;
        } else {
          return SOC_E_NOT_FOUND;
        }
    } else {

        _SOC_IF_ERR_EXIT
            (cprimod_cpri_rx_axc_container_config_get(unit, port, axc_id, &frame_info->container_info));

        frame_info->start_bit = start_bit;
        frame_info->num_bits = num_bits;

        if (frame_info->container_info.map_method == cprimod_cpri_frame_map_method_1) {
            frame_info->container_info.Naxc = num_bits;
            if (num_bits != 0) {
                frame_info->container_info.K   =  frame_info->container_info.cblk_cnt / num_bits;
            }
        }

        if (frame_info->container_info.map_method == cprimod_cpri_frame_map_method_3) {
            Naxc         = frame_info->container_info.Naxc;
            num_container   = num_bits / Naxc;
            frame_info->container_info.Nc  = num_container;
            frame_info->container_info.K   = frame_info->container_info.cblk_cnt/ (num_container * Naxc);
            frame_info->container_info.Nst = 0;
        }

        _SOC_IF_ERR_EXIT
            (cprimod_rx_roe_payload_size_get(unit, port, axc_id, &frame_info->roe_payload_info));
        /*
         * In data AxC, the AxC number and Queue number are identical.
         */
        queue_num = axc_id;
        _SOC_IF_ERR_EXIT
            (cprimod_cpri_encap_data_config_get(unit, port, queue_num, &frame_info->encap_data_info ));
    }
exit:
    sal_free(usage_table);
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_tx_axc_frame_add(int unit, int port, pm_info_t pm_info, uint32 axc_id, portmod_cpri_axc_frame_info_t* frame_info)
{
    uint32 queue_num = 0xFF;
    uint32 num_axc;
    uint32 num_container;
    uint32 num_bits_per_container;
    int offset;

    SOC_INIT_FUNC_DEFS;

    num_axc                 = 1;
    num_container           = 1;
    num_bits_per_container  = frame_info->num_bits;

    if (frame_info->container_info.map_method == cprimod_cpri_frame_map_method_3) {
        num_axc         = frame_info->container_info.Na;
        num_container   = frame_info->container_info.Nc;
        if (frame_info->num_bits != (2 * frame_info->encap_data_info.sample_size * num_container)) {
            return SOC_E_CONFIG;
        }

    }

    if ((num_container > 1) &&
        (num_axc > 1)) {
        if ((frame_info->num_bits % num_container) != 0) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("Number of Bits %d cannot be equally divided between Nc %d.\n"),
                          frame_info->num_bits,
                          num_container));
        }

        num_bits_per_container = frame_info->num_bits / num_container;
        for (offset = 0; offset < num_container; offset++) {
            _SOC_IF_ERR_EXIT
                (cprimod_tx_axc_basic_frame_add(unit, port, axc_id,
                                                frame_info->start_bit+ (offset*num_bits_per_container),
                                                num_bits_per_container));
        }
    } else {
        _SOC_IF_ERR_EXIT
            (cprimod_tx_axc_basic_frame_add(unit, port, axc_id, frame_info->start_bit, frame_info->num_bits));
    }
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_tx_axc_container_config_set(unit, port, axc_id, &frame_info->container_info));

    /*
     * In data AxC, the AxC number and Queue number are identical.
     */
    queue_num = axc_id;

    for (offset = 0; offset < num_axc ; offset++) {

    _SOC_IF_ERR_EXIT
        (cprimod_tx_roe_payload_size_set(unit, port, axc_id+offset, &frame_info->roe_payload_info));
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_set(unit, port, queue_num+offset, &frame_info->encap_data_info ));
    }

    /* activate the basic frame to take the new configuration. */
    _SOC_IF_ERR_EXIT
        (cpm4x25_cpri_port_stage_activate(unit, port, pm_info, portmodCpriStageTx));
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_tx_axc_frame_delete(int unit, int port, pm_info_t pm_info, uint32 axc_id)
{
    cprimod_encap_decap_data_config_t decap_data_config;
    cprimod_payload_size_info_t roe_payload_info;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_axc_basic_frame_delete(unit, port, axc_id));

    _SOC_IF_ERR_EXIT
        (cpm4x25_cpri_port_stage_activate(unit, port, pm_info, portmodCpriStageTx));

    /* Flush the framer state.  */
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_tx_framer_state_clear(unit, port, axc_id));

    _SOC_IF_ERR_EXIT
        (cprimod_decap_memory_clear(unit, port, axc_id));

    /* clear the relate container parser entry. */

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_tx_axc_container_config_clear(unit, port, axc_id));

    /* clear encap data configuration. */
    _SOC_IF_ERR_EXIT
        (cprimod_encap_decap_data_config_t_init(&decap_data_config));

    /* Need to set agnostic_mode to 1 not to run thru allocation */
    decap_data_config.is_agnostic = 1;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_set(unit, port, axc_id, &decap_data_config ));

    /* clear payload info. */
   _SOC_IF_ERR_EXIT
        (cprimod_payload_size_info_t_init(&roe_payload_info));

    _SOC_IF_ERR_EXIT
        (cprimod_tx_roe_payload_size_set(unit, port, axc_id, &roe_payload_info));

exit:
    SOC_FUNC_RETURN;

}



int cpm4x25_cpri_port_tx_axc_frame_get(int unit, int port, pm_info_t pm_info, uint32 axc_id, uint32 from_get_all, portmod_cpri_axc_frame_info_t* frame_info)
{
    uint32 queue_num = 0xFF;
    cprimod_basic_frame_table_id_t active_table;
    cprimod_basic_frame_usage_entry_t* usage_table=NULL;
    int num_entries;
    uint32 start_bit = 0;
    uint32 num_bits = 0;
    uint32 Naxc = 0;
    uint32 num_container = 0;
    int i;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_basic_frame_assembly_active_table_get(unit, port, &active_table));

    usage_table = (cprimod_basic_frame_usage_entry_t *) sal_alloc
                                   ((sizeof(cprimod_basic_frame_table_id_t)*CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY),
                                    "cpm4x25");

    SOC_NULL_CHECK(usage_table);
    num_entries = CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY;


    _SOC_IF_ERR_EXIT
        (cprimod_tx_axc_basic_frame_get(unit, port, axc_id, active_table, usage_table, &num_entries));

    start_bit = usage_table[0].start_bit;

    for (i=0;i<num_entries;i++) {
        num_bits = num_bits + usage_table[i].num_bits;
    }

    if (num_entries == 0) {
        if (from_get_all) {
           frame_info->start_bit = 0;
           frame_info->num_bits = 0;
        } else {
          return SOC_E_NOT_FOUND;
        }
    } else {
    

        _SOC_IF_ERR_EXIT
            (cprimod_cpri_tx_axc_container_config_get(unit, port, axc_id, &frame_info->container_info));

        frame_info->start_bit = start_bit;
        frame_info->num_bits = num_bits;

        if (frame_info->container_info.map_method == cprimod_cpri_frame_map_method_1) {
            frame_info->container_info.Naxc = num_bits;
            if (num_bits != 0) {
                frame_info->container_info.K   =  frame_info->container_info.cblk_cnt / num_bits;
            }
        }

        if (frame_info->container_info.map_method == cprimod_cpri_frame_map_method_3) {
            Naxc         = frame_info->container_info.Naxc;
            num_container   = num_bits / Naxc;
            frame_info->container_info.Nc  = num_container;
            frame_info->container_info.K   = frame_info->container_info.cblk_cnt/ (num_container * Naxc);
            frame_info->container_info.Nst = 0;
        }

         _SOC_IF_ERR_EXIT
             (cprimod_tx_roe_payload_size_get(unit, port, axc_id, &frame_info->roe_payload_info));
         /*
          * In data AxC, the AxC number and Queue number are identical.
          */
         queue_num = axc_id;
         _SOC_IF_ERR_EXIT
             (cprimod_cpri_decap_data_config_get(unit, port, queue_num, &frame_info->encap_data_info ));
    }         
exit:
    sal_free(usage_table);
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_encap_set(int unit, int port, pm_info_t pm_info, uint32 queue_num, cprimod_encap_header_config_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_header_config_set(unit, port, queue_num, config));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_encap_get(int unit, int port, pm_info_t pm_info, uint32 queue_num, cprimod_encap_header_config_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_header_config_get(unit, port, queue_num, config));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_encap_roe_ordering_info_set(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_encap_ordering_info_entry_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_ordering_info_entry_set(unit, port, index, config));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_encap_roe_ordering_info_get(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_encap_ordering_info_entry_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_ordering_info_entry_get(unit, port, index, config));

exit:
    SOC_FUNC_RETURN;

}



int cpm4x25_cpri_port_encap_roe_ordering_sequence_offset_set(int unit, int port, pm_info_t pm_info,
                                                             uint32 queue_num,
                                                             uint32 control,
                                                             cprimod_cpri_ordering_info_offset_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_ordering_info_sequence_offset_set(unit, port, queue_num, control, config));


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_encap_roe_ordering_sequence_offset_get(int unit, int port, pm_info_t pm_info,
                                                             uint32 queue_num,
                                                             uint32 control,
                                                             cprimod_cpri_ordering_info_offset_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_ordering_info_sequence_offset_get(unit, port, queue_num, control,
         config->p_size, config->q_size, config));


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_roe_encap_macda_entry_set(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_mac_addr_t mac_addr)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_mac_da_entry_set(unit, port, index, mac_addr));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_roe_encap_macda_entry_get(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_mac_addr_t mac_addr)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_mac_da_entry_get(unit, port, index, mac_addr));
exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_roe_encap_macsa_entry_set(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_mac_addr_t mac_addr)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_mac_sa_entry_set(unit, port, index, mac_addr));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_roe_encap_macsa_entry_get(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_mac_addr_t mac_addr)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_mac_sa_entry_get(unit, port, index, mac_addr));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_roe_encap_vlan_entry_set(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_vlan_table_id_t table_id, uint32 vlan_id)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_vlan_id_entry_set(unit, port, table_id, index, vlan_id));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_roe_encap_vlan_entry_get(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_vlan_table_id_t table_id, uint32* vlan_id)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_vlan_id_entry_get(unit, port, table_id, index, vlan_id));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_ethertype_config_set(int unit, int port, pm_info_t pm_info,
                                                cprimod_ethertype_t pkt_type,
                                                uint16 ethertype)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_ethertype_config_set(unit, port, pkt_type, ethertype));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_ethertype_config_get(int unit, int port, pm_info_t pm_info,
                                                cprimod_ethertype_t pkt_type,
                                                uint16* ethertype)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_ethertype_config_get(unit, port, pkt_type, ethertype));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_encap_ethertype_config_set(int unit, int port, pm_info_t pm_info,
                                                cprimod_ethertype_t pkt_type,
                                                uint16 ethertype)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_ethertype_config_set(unit, port, pkt_type, ethertype));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_encap_ethertype_config_get(int unit, int port, pm_info_t pm_info,
                                                cprimod_ethertype_t pkt_type,
                                                uint16* ethertype)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_ethertype_config_get(unit, port, pkt_type, ethertype));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_flow_config_set(int unit, int port, pm_info_t pm_info,
                                            uint8 roe_subtype,
                                            uint32 queue_num,
                                            cprimod_cls_option_t cls_option,
                                            cprimod_cls_flow_type_t flow_type,
                                            uint32 ordering_info_index)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_flow_classification_config_set(unit, port, roe_subtype, queue_num, cls_option, flow_type));


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_flow_config_clear(int unit, int port, pm_info_t pm_info, uint8 roe_subtype)
{
    SOC_INIT_FUNC_DEFS;
    
    _SOC_IF_ERR_EXIT
        (cprimod_decap_flow_classification_config_clear(unit, port, roe_subtype));

        
exit:
    SOC_FUNC_RETURN; 
   
}

int cpm4x25_cpri_port_decap_flow_config_get(int unit, int port, pm_info_t pm_info,
                                            uint8 roe_subtype,
                                            uint32* queue_num,
                                            cprimod_cls_option_t* cls_option,
                                            cprimod_cls_flow_type_t* flow_type,
                                            uint32 ordering_info_index)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_flow_classification_config_get(unit, port, roe_subtype, queue_num, cls_option, flow_type));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_queue_to_ordering_info_index_set(int unit, int port, pm_info_t pm_info, uint32 queue_num, uint32 ordering_info_index)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_queue_to_ordering_info_index_mapping_set(unit, port, queue_num, ordering_info_index));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_queue_to_ordering_info_index_get(int unit, int port, pm_info_t pm_info, uint32 queue_num, uint32* ordering_info_index)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_queue_to_ordering_info_index_mapping_get(unit, port, queue_num, ordering_info_index));
exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_decap_flow_to_queue_mapping_set(int unit, int port, pm_info_t pm_info, uint32 flow_id, uint32 queue_num)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_flow_to_queue_mapping_set(unit, port, flow_id, queue_num));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_flow_to_queue_mapping_get(int unit, int port, pm_info_t pm_info, uint32 flow_id, uint32* queue_num)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_flow_to_queue_mapping_get(unit, port, flow_id, queue_num));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_flow_to_queue_mapping_clear(int unit, int port, pm_info_t pm_info, uint32 flow_id)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_flow_to_queue_mapping_clear(unit, port, flow_id));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_roe_ordering_info_set(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_decap_ordering_info_entry_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_ordering_info_entry_set(unit, port, index, config));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_roe_ordering_info_get(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_decap_ordering_info_entry_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_ordering_info_entry_get(unit, port, index, config));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_decap_roe_ordering_sequence_offset_set(int unit, int port, pm_info_t pm_info, uint32 queue_num, cprimod_cpri_ordering_info_offset_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_ordering_info_sequence_offset_set(unit, port, queue_num, config));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_roe_ordering_sequence_offset_get(int unit, int port, pm_info_t pm_info, uint32 queue_num, cprimod_cpri_ordering_info_offset_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_ordering_info_sequence_offset_get(unit, port, queue_num, config->p_size, config->q_size, config));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_frame_tgen_config_set(int unit, int port, pm_info_t pm_info, portmod_tx_frame_tgen_config_t* config)
{
    int val = 0;
    uint8 tgen_hfn = 0;
    uint16 tgen_bfn= 0;
    SOC_INIT_FUNC_DEFS;


    _SOC_IF_ERR_EXIT
        (cprimod_cpri_or_enet_port_get(unit, port, &val));

    if (val == 0) {
  _SOC_EXIT_WITH_ERR
                    (SOC_E_UNAVAIL, ("Port is in Ethernet Mode.\n"));
    }

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_select_counter_set(unit, port, config->tx_gen_bfn_hfn_sel));
    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_offset_set(unit, port, config->tx_tgen_ts_offset));

    tgen_hfn = config->tx_tgen_hfn;
    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_next_hfn_config_set(unit, port, tgen_hfn));

    tgen_bfn = config->tx_tgen_bfn;
    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_next_bfn_config_set(unit, port, tgen_bfn));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_frame_tgen_config_get(int unit, int port, pm_info_t pm_info, portmod_tx_frame_tgen_config_t* config)
{
    uint32 tgen_hfn = 0;
    uint32 tgen_bfn= 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_select_counter_get(unit, port, &config->tx_gen_bfn_hfn_sel));
    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_offset_get(unit, port, &config->tx_tgen_ts_offset));

    tgen_hfn = config->tx_tgen_hfn;
    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_next_hfn_config_get(unit, port, &tgen_hfn));
    config->tx_tgen_hfn = tgen_hfn;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_next_bfn_config_get(unit, port, &tgen_bfn));
    config->tx_tgen_bfn = tgen_bfn;

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_tx_frame_tgen_enable(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_enable(unit, port, enable));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_tx_frame_tgen_ts_offset_set(int unit, int port, pm_info_t pm_info, uint64 offset)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_offset_set(unit, port, offset));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_frame_tgen_ts_offset_get(int unit, int port, pm_info_t pm_info, uint64* offset)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_offset_get(unit, port, offset));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_frame_next_hfn_get(int unit, int port, pm_info_t pm_info, uint32* hfn)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_next_hfn_get(unit, port, hfn));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_frame_next_bfn_get(int unit, int port, pm_info_t pm_info, uint32* bfn)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_next_bfn_get(unit, port, bfn));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_rx_flow_add(int unit, int port, pm_info_t pm_info,
                                        uint32 flow_id,
                                        portmod_rsvd4_rx_flow_info_t* info)
{
    cprimod_header_compare_entry_t compare_entry;
    cprimod_rsvd4_container_config_t container_info;
    cprimod_rsvd4_encap_data_config_t encap_data_info;
    cprimod_payload_size_info_t  payload_info;
    uint32 queue_num=0;

    SOC_INIT_FUNC_DEFS;

    cprimod_rsvd4_container_config_t_init(&container_info);
    container_info.axc_id                   = info->axc_id;
    container_info.stuffing_cnt             = info->stuffing_cnt;
    container_info.msg_ts_mode              = info->msg_ts_mode;
    container_info.msg_ts_cnt               = info->msg_ts_count;
    container_info.frame_sync_mode          = info->frame_sync_mode;
    container_info.master_frame_offset      = info->master_frame_offset;
    container_info.message_number_offset    = info->message_number_offset;
    container_info.container_block_cnt      = info->container_block_count;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_rx_axc_container_config_set(unit, port, flow_id, &container_info));

    cprimod_payload_size_info_t_init(&payload_info);
    payload_info.packet_size        = info->packet_size ;
    payload_info.last_packet_num    = info->last_packet_num ;
    payload_info.last_packet_size   = info->last_packet_size ;

    _SOC_IF_ERR_EXIT
        (cprimod_rx_roe_payload_size_set(unit, port, flow_id, &payload_info));

    /*
     * Queue number is AxC_ID  for data flow.
     */
    queue_num = container_info.axc_id;

    cprimod_rsvd4_encap_data_config_t_init(&encap_data_info);

    encap_data_info.buffer_size             = info->queue_size;
    encap_data_info.priority                = info->priority ;
    encap_data_info.gsm_pad_size            = info->pad_size;
    encap_data_info.gsm_extra_pad_size      = info->extra_pad_size ;
    encap_data_info.gsm_pad_enable          = info->pad_enable ;
    encap_data_info.gsm_control_location    = info->control_location ;
    encap_data_info.bit_reversal            = 1;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_encap_data_config_set(unit, port, queue_num, &encap_data_info));

    _SOC_IF_ERR_EXIT
        (cprimod_rx_rsvd4_gsm_tsn_bitmap_set(unit, port, queue_num, info->tsn_bitmap));

    cprimod_header_compare_entry_t_init(&compare_entry);
    compare_entry.valid         = 1;
    compare_entry.mask          = info->mask;
    compare_entry.match_data    = info->match_data;
    compare_entry.flow_id       = flow_id;
    compare_entry.flow_type     = cprimodFlowTypeData;

    _SOC_IF_ERR_EXIT
        (cprimod_header_compare_entry_add(unit, port, &compare_entry));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_rx_flow_delete(int unit, int port, pm_info_t pm_info, uint32 flow_id)
{
    cprimod_rsvd4_container_config_t container_config;
    cprimod_encap_decap_data_config_t encap_data_config;
    cprimod_payload_size_info_t roe_payload_info;
    uint32 queue_num;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_header_compare_entry_for_flow_delete(unit, port, flow_id, cprimodFlowTypeData));

   _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_rx_axc_container_config_get(unit, port, flow_id, &container_config));

    queue_num  = container_config.axc_id;

    /* Flush encap memory and framer state. */
    _SOC_IF_ERR_EXIT
        (cprimod_encap_memory_clear(unit, port, queue_num));

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_rx_framer_state_clear(unit, port, flow_id));

    /* clear the relate container parser entry. */
    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_rx_axc_container_config_clear(unit, port, flow_id));

    /* clear encap data configuration. */
    _SOC_IF_ERR_EXIT
        (cprimod_encap_decap_data_config_t_init(&encap_data_config));

    /* Need to set agnostic_mode to 1 not to run thru allocation */
    encap_data_config.is_agnostic = 1;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_set(unit, port, queue_num, &encap_data_config ));

    /* clear payload info. */
   _SOC_IF_ERR_EXIT
        (cprimod_payload_size_info_t_init(&roe_payload_info));

    _SOC_IF_ERR_EXIT
        (cprimod_rx_roe_payload_size_set(unit, port, flow_id, &roe_payload_info));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_rsvd4_tx_flow_add(int unit, int port, pm_info_t pm_info, uint32 flow_id, portmod_rsvd4_tx_flow_info_t* info)
{
    cprimod_rsvd4_container_config_t container_info;
    cprimod_payload_size_info_t payload_info;
    cprimod_rsvd4_decap_data_config_t decap_data_info;
    uint32 queue_num;
    int enable =0;

    SOC_INIT_FUNC_DEFS;

   /* Disable Modulo rule and DBM Position Entry associated with this flow. */
    enable = FALSE;
    _SOC_IF_ERR_EXIT
        (cprimod_modulo_rule_flow_enable_set(unit, port, enable, cprimodFlowTypeData,flow_id));

    _SOC_IF_ERR_EXIT
        (cprimod_dbm_position_flow_enable_set(unit, port, enable, cprimodFlowTypeData,flow_id));

    /* clear the tx framer state. */
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_tx_framer_state_clear(unit, port, flow_id));

    /* Flush decap memory and framer state. */
    _SOC_IF_ERR_EXIT
        (cprimod_decap_memory_clear(unit, port, info->axc_id));

    cprimod_rsvd4_container_config_t_init(&container_info);
    container_info.axc_id                   = info->axc_id;
    container_info.stuffing_cnt             = info->stuffing_cnt ;
    container_info.msg_ts_mode              = info->msg_ts_mode ;
    container_info.frame_sync_mode          = info->frame_sync_mode ;
    container_info.master_frame_offset      = info->master_frame_offset ;
    container_info.message_number_offset    = info->message_number_offset ;
    container_info.container_block_cnt      = info->container_block_count ;
    container_info.use_ts_dbm               = info->use_ts_dbm ;
    container_info.ts_dbm_prof_num          = info->ts_dbm_profile_num  ;
    container_info.num_active_slots         = info->num_active_slot ;
    container_info.msg_addr                 = info->msg_addr ;
    container_info.msg_type                 = info->msg_type ;
    container_info.msg_ts_offset            = info->msg_ts_offset ;

    /*
     * This is for MTY C0, msg_ts_cnt for GSM are user defined and
     * 64 for non GSM (LTE & WCDMA).
     */
    if ((info->msg_ts_mode == CPRIMOD_RSVD4_MSG_TS_MODE_GSM_DL)||
        (info->msg_ts_mode == CPRIMOD_RSVD4_MSG_TS_MODE_GSM_UL)) {
        container_info.msg_ts_cnt = info->msg_ts_count;
    } else { /* Non GSM , LTE or WCDMA */
        container_info.msg_ts_cnt = 64;
    }
    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_axc_container_config_set(unit, port, flow_id , &container_info));

    cprimod_payload_size_info_t_init(&payload_info);
    payload_info.packet_size        = info->packet_size ;
    payload_info.last_packet_num    = info->last_packet_num ;
    payload_info.last_packet_size   = info->last_packet_size ;
    _SOC_IF_ERR_EXIT
        (cprimod_tx_roe_payload_size_set(unit, port, flow_id, &payload_info));

    /*
     * Queue number is AxC_ID.
     */
    queue_num = container_info.axc_id;

    cprimod_rsvd4_decap_data_config_t_init(&decap_data_info);
    decap_data_info.buffer_size = info->buffer_size ;
    decap_data_info.cycle_size = info->cycle_size ;
    decap_data_info.gsm_pad_size = info->pad_size ;
    decap_data_info.gsm_extra_pad_size = info->extra_pad_size ;
    decap_data_info.gsm_pad_enable = info->pad_enable ;
    decap_data_info.gsm_control_location = info->control_location ;
    decap_data_info.bit_reversal = 1;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_decap_data_config_set(unit, port, queue_num, &decap_data_info));

   /* Re-Enable Modulo rule and DBM Position Entry associated with this flow. */
    enable = TRUE;
    _SOC_IF_ERR_EXIT
        (cprimod_modulo_rule_flow_enable_set(unit, port, enable, cprimodFlowTypeData,flow_id));

    _SOC_IF_ERR_EXIT
        (cprimod_dbm_position_flow_enable_set(unit, port, enable, cprimodFlowTypeData,flow_id));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_tx_flow_delete(int unit, int port, pm_info_t pm_info, uint32 flow_id)
{
    uint32 queue_num;
    cprimod_rsvd4_container_config_t container_config;
    cprimod_rsvd4_decap_data_config_t decap_data_config;
    cprimod_payload_size_info_t roe_payload_info;
    int enable =0, mod_enable=0, pos_enable=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_axc_container_config_get(unit, port, flow_id, &container_config));

    queue_num = container_config.axc_id;

    /* Get Modulo rule and DBM Position Entry Enable status associated with this flow. */
    _SOC_IF_ERR_EXIT
        (cprimod_modulo_rule_flow_enable_get(unit, port, &mod_enable, cprimodFlowTypeData,flow_id));

    _SOC_IF_ERR_EXIT
        (cprimod_dbm_position_flow_enable_get(unit, port, &pos_enable, cprimodFlowTypeData,flow_id));

    /*
     * WAR for tsn msg count reset.
     */
    if (!mod_enable && !pos_enable) {    
       _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
              (_SOC_MSG("Transmission need to be ON to delete to reset sequece number WAR, %d"), port));
    }

    /* for the msg ts cnt to 1 to peg the count at 0. */
    container_config.msg_ts_cnt     = 1;
    container_config.msg_addr       = 0x1FFF;
    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_axc_container_config_set(unit, port, flow_id , &container_config));
    /* sleep 13ms */
    sal_usleep(13000);

    /* END of WAR */

    /* Disable Modulo rule and DBM Position Entry associated with this flow. */
    enable = FALSE;
    _SOC_IF_ERR_EXIT
        (cprimod_modulo_rule_flow_enable_set(unit, port, enable, cprimodFlowTypeData,flow_id));

    _SOC_IF_ERR_EXIT
        (cprimod_dbm_position_flow_enable_set(unit, port, enable, cprimodFlowTypeData,flow_id));

    /* clear the relate container assembly entry. */
    cprimod_rsvd4_tx_axc_container_config_clear(unit, port, flow_id);

    /* clear the tx framer state. */
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_tx_framer_state_clear(unit, port, flow_id));

    /* Flush decap memory and framer state. */
    _SOC_IF_ERR_EXIT
        (cprimod_decap_memory_clear(unit, port, queue_num));

    /* clear encap data configuration. */
    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_decap_data_config_t_init(&decap_data_config));

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_decap_data_config_set(unit, port, queue_num, &decap_data_config ));

    /* clear payload info. */
    _SOC_IF_ERR_EXIT
        (cprimod_payload_size_info_t_init(&roe_payload_info));

    _SOC_IF_ERR_EXIT
        (cprimod_tx_roe_payload_size_set(unit, port, flow_id, &roe_payload_info));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_dbm_entry_set(int unit, int port, pm_info_t pm_info, uint32 profile_id, cprimod_dbm_rule_entry_t* profile)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_dual_bitmap_rule_entry_set(unit, port, profile_id, profile));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_dbm_entry_get(int unit, int port, pm_info_t pm_info, uint32 profile_id, cprimod_dbm_rule_entry_t* profile)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_dual_bitmap_rule_entry_get(unit, port, profile_id, profile));
exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_rsvd4_secondary_dbm_entry_set(int unit, int port, pm_info_t pm_info, uint32 profile_id, cprimod_secondary_dbm_rule_entry_t* profile)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_secondary_dual_bitmap_rule_entry_set(unit, port, profile_id, profile));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_secondary_dbm_entry_get(int unit, int port, pm_info_t pm_info, uint32 profile_id, cprimod_secondary_dbm_rule_entry_t* profile)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_secondary_dual_bitmap_rule_entry_get(unit, port, profile_id, profile));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_modulo_rule_entry_set(int unit, int port, pm_info_t pm_info,
                                                  uint32 modulo_rule_num,
                                                  cprimod_transmission_rule_type_t modulo_rule_type,
                                                  const cprimod_modulo_rule_entry_t* modulo_rule)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_modulo_rule_entry_set(unit, port, modulo_rule_num, modulo_rule_type, modulo_rule));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_modulo_rule_entry_get(int unit, int port, pm_info_t pm_info,
                                                  uint32 modulo_rule_num,
                                                  cprimod_transmission_rule_type_t modulo_rule_type,
                                                  cprimod_modulo_rule_entry_t* modulo_rule)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_modulo_rule_entry_get(unit, port, modulo_rule_num, modulo_rule_type, modulo_rule));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_rsvd4_dbm_position_entry_set(int unit, int port, pm_info_t pm_info,
                                                   uint32 index,
                                                   cprimod_dbm_pos_table_entry_t* pos_entry)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_dbm_position_entry_set(unit, port, index, pos_entry));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_dbm_position_entry_get(int unit, int port, pm_info_t pm_info,
                                                   uint32 index,
                                                   cprimod_dbm_pos_table_entry_t* pos_entry)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_dbm_position_entry_get(unit, port, index, pos_entry));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_rx_master_frame_sync_config_set(int unit, int port, pm_info_t pm_info, uint32 master_frame_number, uint64 master_frame_start_time)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_rx_master_frame_sync_config_set(unit, port, master_frame_number, master_frame_start_time));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_rx_master_frame_sync_config_get(int unit, int port, pm_info_t pm_info, uint32* master_frame_number, uint64* master_frame_start_time)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_rx_master_frame_sync_config_get(unit, port, master_frame_number, master_frame_start_time));
exit:
    SOC_FUNC_RETURN;
}

/*
 * Control Word Processing.
 */

int cpm4x25_cpri_port_rx_cw_sync_info_get(int unit, int port, pm_info_t pm_info, uint32* hyper_frame_num, uint32* radio_frame_num)
{

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_rx_control_word_sync_info_get(unit, port, hyper_frame_num, radio_frame_num));
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_rx_cw_l1_inband_info_get(int unit, int port, pm_info_t pm_info, portmod_cpri_cw_l1_inband_info_t* l1_inband_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_rx_control_word_l1_inband_info_get(unit, port, l1_inband_info));

exit:
    SOC_FUNC_RETURN;
   
}

int cpm4x25_cpri_port_rx_cw_l1_signal_signal_protection_set(int unit, int port, pm_info_t pm_info, uint32 signal_map, uint32 enable)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_rx_control_word_l1_signal_signal_protection_set(unit, port, signal_map,enable));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_cw_l1_signal_signal_protection_get(int unit, int port, pm_info_t pm_info, uint32 signal_map, uint32* enable)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_rx_control_word_l1_signal_signal_protection_get(unit, port, signal_map,enable));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_cw_slow_hdlc_config_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_slow_hdlc_config_info_t* config_info)
{
    cprimod_slow_hdlc_config_info_t hdlc_config;
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.buffer_size          = config_info->queue_size;
    encap_decap_data.priority             = config_info->priority;
    encap_decap_data.cycle_size           = config_info->cycle_size;
    encap_decap_data.is_agnostic          = 0;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));

    /* added 16 bytes to the buffer.
     * The extra space is required to store the RoE Header for Tx Framer
     */
    encap_decap_data.buffer_size          = config_info->buffer_size + 16;
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));

    hdlc_config.tx_crc_mode     = config_info->tx_crc_mode;
    hdlc_config.tx_flag_size    = config_info->tx_flag_size;
    hdlc_config.crc_init_val    = config_info->crc_init_val;
    hdlc_config.use_fe_mac      = config_info->use_fe_mac;
    hdlc_config.crc_byte_swap   = config_info->crc_byte_swap;
    hdlc_config.cw_sel          = config_info->cw_sel;
    hdlc_config.cw_size         = config_info->cw_size;
    hdlc_config.fcs_size        = config_info->fcs_size;
    hdlc_config.runt_frame_drop = config_info->runt_frame_drop;
    hdlc_config.long_frame_drop = config_info->long_frame_drop ;
    hdlc_config.min_frame_size  = config_info->min_frame_size;
    hdlc_config.max_frame_size  = config_info->max_frame_size;
    hdlc_config.queue_num       = config_info->queue_num ;

    hdlc_config.tx_filling_flag_pattern = config_info->tx_filling_flag_pattern ;
    hdlc_config.no_fcs_err_check        = config_info->no_fcs_err_check ;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_cw_slow_hdlc_config_set(unit, port, &hdlc_config));
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_cw_slow_hdlc_config_get(int unit, int port, pm_info_t pm_info, portmod_cpri_slow_hdlc_config_info_t* config_info)
{
    cprimod_slow_hdlc_config_info_t hdlc_config;
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;


    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_cw_slow_hdlc_config_get(unit, port, &hdlc_config));


    config_info->tx_crc_mode        = hdlc_config.tx_crc_mode;
    config_info->tx_flag_size       = hdlc_config.tx_flag_size;
    config_info->crc_init_val       = hdlc_config.crc_init_val;
    config_info->use_fe_mac         = hdlc_config.use_fe_mac;
    config_info->crc_byte_swap      = hdlc_config.crc_byte_swap;
    config_info->cw_sel             = hdlc_config.cw_sel;
    config_info->cw_size            = hdlc_config.cw_size;
    config_info->fcs_size           = hdlc_config.fcs_size;
    config_info->runt_frame_drop    = hdlc_config.runt_frame_drop;
    config_info->long_frame_drop    = hdlc_config.long_frame_drop;
    config_info->min_frame_size     = hdlc_config.min_frame_size;
    config_info->max_frame_size     = hdlc_config.max_frame_size;
    config_info->queue_num          = hdlc_config.queue_num;

    config_info->tx_filling_flag_pattern = hdlc_config.tx_filling_flag_pattern;
    config_info->no_fcs_err_check        = hdlc_config.no_fcs_err_check;

   _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_get(unit, port, config_info->queue_num, &encap_decap_data));

    config_info->queue_size     = encap_decap_data.buffer_size;
    config_info->priority       = encap_decap_data.priority;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_get(unit, port, config_info->queue_num, &encap_decap_data));

    config_info->cycle_size     = encap_decap_data.cycle_size;
    config_info->buffer_size    = encap_decap_data.buffer_size -16;



exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_cw_fast_eth_config_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_fast_eth_config_info_t* config_info)
{
    cprimod_fast_eth_config_info_t eth_config;
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.buffer_size          = config_info->queue_size;
    encap_decap_data.priority             = config_info->priority;
    encap_decap_data.cycle_size           = config_info->cycle_size;
    encap_decap_data.is_agnostic          = 0;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));

    /* added 16 bytes to the buffer.
     * The extra space is required to store the RoE Header for Tx Framer
     */
    encap_decap_data.buffer_size          = config_info->buffer_size + 16;
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));

    eth_config.tx_crc_mode      = config_info->tx_crc_mode;
    eth_config.schan_start      = config_info->schan_start;
    eth_config.schan_size       = config_info->schan_size;
    eth_config.cw_sel           = config_info->cw_sel;
    eth_config.cw_size          = config_info->cw_size;
    eth_config.min_packet_size  = config_info->min_packet_size;
    eth_config.max_packet_size  = config_info->max_packet_size;
    eth_config.min_packet_drop  = config_info->min_packet_drop;
    eth_config.max_packet_drop  = config_info->max_packet_drop;
    eth_config.strip_crc        = config_info->strip_crc;
    eth_config.min_ipg          = config_info->min_ipg;
    eth_config.queue_num        = config_info->queue_num ;
    eth_config.no_fcs_err_check = config_info->no_fcs_err_check ;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_cw_fast_eth_config_set(unit, port, &eth_config));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_cw_fast_eth_config_get(int unit, int port, pm_info_t pm_info, portmod_cpri_fast_eth_config_info_t* config_info)
{

    cprimod_fast_eth_config_info_t eth_config;
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_cw_fast_eth_config_get(unit, port, &eth_config));

    config_info->tx_crc_mode = eth_config.tx_crc_mode;
    config_info->schan_start = eth_config.schan_start;
    config_info->schan_size = eth_config.schan_size;
    config_info->cw_sel = eth_config.cw_sel;
    config_info->cw_size = eth_config.cw_size;
    config_info->min_packet_size = eth_config.min_packet_size;
    config_info->max_packet_size = eth_config.max_packet_size;
    config_info->min_packet_drop = eth_config.min_packet_drop;
    config_info->max_packet_drop = eth_config.max_packet_drop;
    config_info->strip_crc = eth_config.strip_crc;
    config_info->min_ipg = eth_config.min_ipg;
    config_info->queue_num = eth_config.queue_num;
    config_info->no_fcs_err_check = eth_config.no_fcs_err_check;

   _SOC_IF_ERR_EXIT
        (cprimod_encap_decap_data_config_t_init(&encap_decap_data));
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_get(unit, port, config_info->queue_num, &encap_decap_data));

    config_info->queue_size     = encap_decap_data.buffer_size;
    config_info->priority       = encap_decap_data.priority;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_decap_data_config_t_init(&encap_decap_data));
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_get(unit, port, config_info->queue_num, &encap_decap_data));

    config_info->cycle_size     = encap_decap_data.cycle_size;
    config_info->buffer_size    = encap_decap_data.buffer_size -16;

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_rx_vsd_config_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_rx_vsd_config_info_t* config_info)
{
    cprimod_cpri_rx_vsd_config_info_t vsd_config;
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.buffer_size          = config_info->queue_size;
    encap_decap_data.priority             = config_info->priority;
    encap_decap_data.cycle_size           = 0; /* not used for encap */
    encap_decap_data.is_agnostic          = 0;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));

    vsd_config.subchan_start = config_info->subchan_start ;
    vsd_config.subchan_size  = config_info->subchan_size ;
    vsd_config.subchan_bytes = config_info->subchan_bytes ;
    vsd_config.subchan_steps = config_info->subchan_steps ;
    vsd_config.flow_bytes  = config_info->flow_bytes ;
    vsd_config.queue_num   = config_info->queue_num ;
    vsd_config.rsvd_sector_mask[0] = config_info->rsvd_sector_mask[0] ;
    vsd_config.rsvd_sector_mask[1] = config_info->rsvd_sector_mask[1] ;
    vsd_config.rsvd_sector_mask[2] = config_info->rsvd_sector_mask[2] ;
    vsd_config.rsvd_sector_mask[3] = config_info->rsvd_sector_mask[3] ;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_vsd_cw_config_set(unit, port, &vsd_config));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_vsd_config_get(int unit, int port, pm_info_t pm_info, portmod_cpri_rx_vsd_config_info_t* config_info)
{
    cprimod_cpri_rx_vsd_config_info_t vsd_config;
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_vsd_cw_config_get(unit, port, &vsd_config));

    config_info->subchan_start  = vsd_config.subchan_start;
    config_info->subchan_size   = vsd_config.subchan_size;
    config_info->subchan_bytes  = vsd_config.subchan_bytes;
    config_info->subchan_steps  = vsd_config.subchan_steps;
    config_info->flow_bytes     = vsd_config.flow_bytes;
    config_info->queue_num      = vsd_config.queue_num;

    config_info->rsvd_sector_mask[0]  = vsd_config.rsvd_sector_mask[0];
    config_info->rsvd_sector_mask[1]  = vsd_config.rsvd_sector_mask[1];
    config_info->rsvd_sector_mask[2]  = vsd_config.rsvd_sector_mask[2];
    config_info->rsvd_sector_mask[3]  = vsd_config.rsvd_sector_mask[3];

   _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_get(unit, port, config_info->queue_num, &encap_decap_data));
    config_info->queue_size = encap_decap_data.buffer_size;
    config_info->priority   = encap_decap_data.priority;

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_tx_vsd_config_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_tx_vsd_config_info_t* config_info)
{
    cprimod_cpri_tx_vsd_config_info_t vsd_config;
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.cycle_size           = config_info->cycle_size;
    encap_decap_data.priority             = 0; /* not used for decap */
    encap_decap_data.is_agnostic          = 0;

    /* added 16 bytes to the buffer.
     * The extra space is required to store the RoE Header for Tx Framer
     */
    encap_decap_data.buffer_size          = config_info->buffer_size + 16;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));

    vsd_config.subchan_start = config_info->subchan_start ;
    vsd_config.subchan_size  = config_info->subchan_size ;
    vsd_config.subchan_bytes = config_info->subchan_bytes ;
    vsd_config.subchan_steps = config_info->subchan_steps ;
    vsd_config.flow_bytes  = config_info->flow_bytes ;
    vsd_config.queue_num   = config_info->queue_num ;

    vsd_config.byte_order_swap     = config_info->byte_order_swap;
    vsd_config.rsvd_sector_mask[0] = config_info->rsvd_sector_mask[0] ;
    vsd_config.rsvd_sector_mask[1] = config_info->rsvd_sector_mask[1] ;
    vsd_config.rsvd_sector_mask[2] = config_info->rsvd_sector_mask[2] ;
    vsd_config.rsvd_sector_mask[3] = config_info->rsvd_sector_mask[3] ;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_vsd_cw_config_set(unit, port, &vsd_config));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_vsd_config_get(int unit, int port, pm_info_t pm_info, portmod_cpri_tx_vsd_config_info_t* config_info)
{
    cprimod_cpri_tx_vsd_config_info_t vsd_config;
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_vsd_cw_config_get(unit, port, &vsd_config));

    config_info->subchan_start        =  vsd_config.subchan_start;
    config_info->subchan_size         =  vsd_config.subchan_size;
    config_info->subchan_bytes        =  vsd_config.subchan_bytes;
    config_info->subchan_steps        =  vsd_config.subchan_steps;
    config_info->flow_bytes           =  vsd_config.flow_bytes;
    config_info->queue_num            =  vsd_config.queue_num;
    config_info->byte_order_swap      =  vsd_config.byte_order_swap;
    config_info->rsvd_sector_mask[0]  =  vsd_config.rsvd_sector_mask[0];
    config_info->rsvd_sector_mask[1]  =  vsd_config.rsvd_sector_mask[1];
    config_info->rsvd_sector_mask[2]  =  vsd_config.rsvd_sector_mask[2];
    config_info->rsvd_sector_mask[3]  =  vsd_config.rsvd_sector_mask[3];

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_get(unit, port, config_info->queue_num, &encap_decap_data));

    config_info->cycle_size = encap_decap_data.cycle_size;
    config_info->buffer_size = encap_decap_data.buffer_size-16;


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_vsd_flow_add(int unit, int port, pm_info_t pm_info, uint32 group_id, const portmod_cpri_rx_vsd_flow_info_t* flow_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_vsd_ctrl_flow_add(unit, port, group_id, flow_info));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_rx_vsd_flow_delete(int unit, int port, pm_info_t pm_info, uint32 group_id)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_vsd_ctrl_flow_delete(unit, port, group_id));


exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_rx_vsd_flow_get(int unit, int port, pm_info_t pm_info, uint32 group_id, portmod_cpri_rx_vsd_flow_info_t* flow_info)
{
    SOC_INIT_FUNC_DEFS;

    sal_memset(flow_info, 0, sizeof(portmod_cpri_rx_vsd_flow_info_t));

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_vsd_ctrl_flow_get(unit, port, group_id, flow_info));

exit:
    SOC_FUNC_RETURN;

}



STATIC
int _cpm4x25_port_index_get(int unit, int port, pm_info_t pm_info,
                           int *first_index, uint32 *bitmap)
{
   int i, rv = 0, tmp_port = 0;

   SOC_INIT_FUNC_DEFS;

   *first_index = -1;
   *bitmap = 0;

   for( i = 0 ; i < MAX_PORTS_PER_PM4X25; i++){
       rv = PM4x25_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
       _SOC_IF_ERR_EXIT(rv);

       if(tmp_port == port){
           *first_index = (*first_index == -1 ? i : *first_index);
           *bitmap |=  0x00000001 << i; 
       }
   }

   if(*first_index == -1) {
       _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
              (_SOC_MSG("port was not found in internal DB %d"), port));
   }

exit:
    SOC_FUNC_RETURN;
}


int cpm4x25_cpri_port_tx_vsd_flow_add(int unit, int port, pm_info_t pm_info, uint32 group_id, const portmod_cpri_tx_vsd_flow_info_t* flow_info)
{
    int lane_index;
    uint32 lane_map;
    uint32 group_list[NUM_VSD_ACTIVE_GROUP_WORD];
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (_cpm4x25_port_index_get(unit, port, pm_info, &lane_index, &lane_map));

    PM4x25_CPRI_PORT_VSD_ACTIVE_GROUP_GET(unit, pm_info, group_list, lane_index);

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_vsd_ctrl_flow_add(unit, port, group_id, group_list, flow_info));

    _SOC_IF_ERR_EXIT
        (PM4x25_CPRI_PORT_VSD_ACTIVE_GROUP_SET(unit, pm_info, group_list, lane_index));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_vsd_flow_delete(int unit, int port, pm_info_t pm_info, uint32 group_id)
{
    int lane_index;
    uint32 lane_map;
    uint32 group_list[NUM_VSD_ACTIVE_GROUP_WORD];
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (_cpm4x25_port_index_get(unit, port, pm_info, &lane_index, &lane_map));

    PM4x25_CPRI_PORT_VSD_ACTIVE_GROUP_GET(unit, pm_info, group_list, lane_index);

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_vsd_ctrl_flow_delete(unit, port, group_id, group_list));

    _SOC_IF_ERR_EXIT
        (PM4x25_CPRI_PORT_VSD_ACTIVE_GROUP_SET(unit, pm_info, group_list, lane_index));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_vsd_flow_get(int unit, int port, pm_info_t pm_info, uint32 group_id, portmod_cpri_tx_vsd_flow_info_t* flow_info)
{
    int lane_index;
    uint32 lane_map;
    uint32 group_list[NUM_VSD_ACTIVE_GROUP_WORD];
    SOC_INIT_FUNC_DEFS;

    sal_memset(flow_info, 0, sizeof(portmod_cpri_tx_vsd_flow_info_t));

    _SOC_IF_ERR_EXIT
        (_cpm4x25_port_index_get(unit, port, pm_info, &lane_index, &lane_map));

    _SOC_IF_ERR_EXIT
        (PM4x25_CPRI_PORT_VSD_ACTIVE_GROUP_GET(unit, pm_info, group_list, lane_index));
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_vsd_ctrl_flow_get(unit, port, group_id, group_list, flow_info));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_encap_queue_control_tag_to_flow_map_set(int unit, int port, pm_info_t pm_info, uint32 tag_id, uint32 flow_id)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_control_queue_tag_to_flow_id_map_set(unit, port, tag_id, flow_id));
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_encap_queue_control_tag_to_flow_map_get(int unit, int port, pm_info_t pm_info, uint32 tag_id, uint32* flow_id)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_control_queue_tag_to_flow_id_map_get(unit, port, tag_id, flow_id));
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_rx_vsd_raw_config_set(int unit, int port, pm_info_t pm_info, uint32 index, const portmod_cpri_rx_vsd_raw_config_t* config_info)
{
    cprimod_cpri_rx_vsd_raw_config_t vsd_config;
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.buffer_size          = config_info->queue_size;
    encap_decap_data.priority             = config_info->priority;
    encap_decap_data.cycle_size           = 0; /* not used for encap */
    encap_decap_data.is_agnostic          = 0;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));


    vsd_config.schan_start          = config_info->schan_start ;
    vsd_config.schan_size           = config_info->schan_size ;
    vsd_config.cw_sel               = config_info->cw_sel;
    vsd_config.cw_size              = config_info->cw_size;
    vsd_config.filter_mode          = config_info->filter_mode;
    vsd_config.hyper_frame_index    = config_info->hyper_frame_index;
    vsd_config.hyper_frame_modulo   = config_info->hyper_frame_modulo;
    vsd_config.match_offset         = config_info->match_offset;
    vsd_config.match_value          = config_info->match_value;
    vsd_config.match_mask           = config_info->match_mask;
    vsd_config.queue_num            = config_info->queue_num;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_vsd_raw_config_set(unit, port, index, &vsd_config));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_vsd_raw_config_get(int unit, int port, pm_info_t pm_info, uint32 index, portmod_cpri_rx_vsd_raw_config_t* config_info)
{
    cprimod_cpri_rx_vsd_raw_config_t vsd_config;
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;

   _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_vsd_raw_config_get(unit, port, index, &vsd_config));

    config_info->schan_start        = vsd_config.schan_start;
    config_info->schan_size         = vsd_config.schan_size;
    config_info->cw_sel             = vsd_config.cw_sel;
    config_info->cw_size            = vsd_config.cw_size;
    config_info->filter_mode        = vsd_config.filter_mode;
    config_info->hyper_frame_index  = vsd_config.hyper_frame_index;
    config_info->hyper_frame_modulo = vsd_config.hyper_frame_modulo;
    config_info->match_offset       = vsd_config.match_offset;
    config_info->match_value        = vsd_config.match_value;
    config_info->match_mask         = vsd_config.match_mask;
    config_info->queue_num          = vsd_config.queue_num;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_get(unit, port, config_info->queue_num, &encap_decap_data));
    config_info->queue_size = encap_decap_data.buffer_size;
    config_info->priority   = encap_decap_data.priority;

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_tx_vsd_raw_config_set(int unit, int port, pm_info_t pm_info, uint32 index, const portmod_cpri_tx_vsd_raw_config_t* config_info)
{
    cprimod_cpri_tx_vsd_raw_config_t vsd_config;
    cprimod_encap_decap_data_config_t encap_decap_data;
    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.cycle_size           = config_info->cycle_size;
    encap_decap_data.priority             = 0; /* not used for decap */
    encap_decap_data.is_agnostic          = 0;

    /* added 16 bytes to the buffer.
     * The extra space is required to store the RoE Header for Tx Framer
     */
    encap_decap_data.buffer_size          = config_info->buffer_size + 16;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));


    vsd_config.schan_start          = config_info->schan_start ;
    vsd_config.schan_size           = config_info->schan_size ;
    vsd_config.cw_sel               = config_info->cw_sel;
    vsd_config.cw_size              = config_info->cw_size;
    vsd_config.map_mode             = config_info->map_mode;
    vsd_config.hyper_frame_index    = config_info->hyper_frame_index;
    vsd_config.hyper_frame_modulo   = config_info->hyper_frame_modulo;
    vsd_config.repeat_enable        = config_info->repeat_enable;
    vsd_config.bfn0_filter_enable   = config_info->bfn0_filter_enable;
    vsd_config.bfn1_filter_enable   = config_info->bfn1_filter_enable;
    vsd_config.queue_num            = config_info->queue_num;
    vsd_config.idle_value           = config_info->idle_value;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_vsd_raw_config_set(unit, port, index, &vsd_config));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_vsd_raw_config_get(int unit, int port, pm_info_t pm_info, uint32 index, portmod_cpri_tx_vsd_raw_config_t* config_info)
{
   cprimod_cpri_tx_vsd_raw_config_t vsd_config;
    cprimod_encap_decap_data_config_t encap_decap_data;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_vsd_raw_config_get(unit, port, index, &vsd_config));

    config_info->schan_start        = vsd_config.schan_start;
    config_info->schan_size         = vsd_config.schan_size;
    config_info->cw_sel             = vsd_config.cw_sel;
    config_info->cw_size            = vsd_config.cw_size;
    config_info->map_mode           = vsd_config.map_mode;
    config_info->hyper_frame_index  = vsd_config.hyper_frame_index;
    config_info->hyper_frame_modulo = vsd_config.hyper_frame_modulo;
    config_info->repeat_enable      = vsd_config.repeat_enable;
    config_info->bfn0_filter_enable = vsd_config.bfn0_filter_enable;
    config_info->bfn1_filter_enable = vsd_config.bfn1_filter_enable;
    config_info->queue_num          = vsd_config.queue_num;
    config_info->idle_value         = vsd_config.idle_value;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_get(unit, port, config_info->queue_num, &encap_decap_data));

    config_info->cycle_size  = encap_decap_data.cycle_size;
    /* subtract  16 bytes from the buffer.  The extra space was added when config was
     * added  to store the RoE Header for Tx Framer
     */
    config_info->buffer_size = encap_decap_data.buffer_size-16;

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_vsd_raw_filter_set(int unit, int port, pm_info_t pm_info, uint32 bfn0_value, uint32 bfn0_mask, uint32 bfn1_value, uint32 bfn1_mask)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_vsd_raw_filter_set(unit, port, bfn0_value, bfn0_mask, bfn1_value, bfn1_mask));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_vsd_raw_filter_get(int unit, int port, pm_info_t pm_info, uint32* bfn0_value, uint32* bfn0_mask, uint32* bfn1_value, uint32* bfn1_mask)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_vsd_raw_filter_get(unit, port, bfn0_value, bfn0_mask, bfn1_value, bfn1_mask));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_brcm_rsvd5_config_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_rx_brcm_rsvd5_config_t* config_info)
{
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.buffer_size          = config_info->queue_size;
    encap_decap_data.priority             = config_info->priority;
    encap_decap_data.cycle_size           = 0; /* not used for encap */
    encap_decap_data.is_agnostic          = 0;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_brcm_rsvd5_control_config_set(unit, port,
                                                          config_info->schan_start,
                                                          config_info->schan_size,
                                                          config_info->queue_num,
                                                          config_info->parity_disable));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_brcm_rsvd5_config_get(int unit, int port, pm_info_t pm_info, portmod_cpri_rx_brcm_rsvd5_config_t* config_info)
{
    cprimod_encap_decap_data_config_t encap_decap_data;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_brcm_rsvd5_control_config_get(unit, port,
                                                            &config_info->schan_start,
                                                            &config_info->schan_size,
                                                            &config_info->queue_num,
                                                            &config_info->parity_disable));

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_get(unit, port, config_info->queue_num, &encap_decap_data));

    config_info->queue_size = encap_decap_data.buffer_size;
    config_info->priority   = encap_decap_data.priority;

exit:
    SOC_FUNC_RETURN;
}


int cpm4x25_cpri_port_tx_brcm_rsvd5_config_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_tx_brcm_rsvd5_config_t* config_info)
{
    cprimod_encap_decap_data_config_t encap_decap_data;
    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.cycle_size           = config_info->cycle_size;
    encap_decap_data.priority             = 0; /* not used for decap */
    encap_decap_data.is_agnostic          = 0;

    /* added 16 bytes to the buffer.
     * The extra space is required to store the RoE Header for Tx Framer
     */
    encap_decap_data.buffer_size          = config_info->buffer_size + 16;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_brcm_rsvd5_control_config_set(unit, port,
                                                          config_info->schan_start,
                                                          config_info->schan_size,
                                                          config_info->queue_num,
                                                          config_info->crc_enable));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_tx_brcm_rsvd5_config_get(int unit, int port, pm_info_t pm_info, portmod_cpri_tx_brcm_rsvd5_config_t* config_info)
{
    cprimod_encap_decap_data_config_t encap_decap_data;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_brcm_rsvd5_control_config_get(unit, port,
                                                            &config_info->schan_start,
                                                            &config_info->schan_size,
                                                            &config_info->queue_num,
                                                            &config_info->crc_enable));

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_get(unit, port, config_info->queue_num, &encap_decap_data));

    config_info->cycle_size  = encap_decap_data.cycle_size;
    config_info->buffer_size = encap_decap_data.buffer_size-16;

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_gcw_config_set(int unit, int port, pm_info_t pm_info, uint32 index, const portmod_cpri_rx_gcw_config_t* config_info)
{
    cprimod_cpri_rx_gcw_config_t gcw_config;
    SOC_INIT_FUNC_DEFS;

    gcw_config.Ns   = config_info->Ns;
    gcw_config.Xs   = config_info->Xs;
    gcw_config.Y    = config_info->Y;
    gcw_config.mask = config_info->mask;

    gcw_config.filter_mode          = config_info->filter_mode;
    gcw_config.hyper_frame_index    = config_info->hyper_frame_index;;
    gcw_config.hyper_frame_modulo   = config_info->hyper_frame_modulo;;
    gcw_config.match_mask           = config_info->match_mask;
    gcw_config.match_value          = config_info->match_value;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_gcw_config_set(unit, port, index, &gcw_config));


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_gcw_config_get(int unit, int port, pm_info_t pm_info, uint32 index, portmod_cpri_rx_gcw_config_t* config_info)
{
   cprimod_cpri_rx_gcw_config_t gcw_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_gcw_config_get(unit, port, index, &gcw_config));

    config_info->Ns     = gcw_config.Ns;
    config_info->Xs     = gcw_config.Xs;
    config_info->Y      = gcw_config.Y;
    config_info->mask   = gcw_config.mask;

    config_info->filter_mode        = gcw_config.filter_mode;
    config_info->hyper_frame_index  = gcw_config.hyper_frame_index;
    config_info->hyper_frame_modulo = gcw_config.hyper_frame_modulo;
    config_info->match_mask         = gcw_config.match_mask;
    config_info->match_value        = gcw_config.match_value;

exit:
    SOC_FUNC_RETURN;
}


int cpm4x25_cpri_port_tx_gcw_config_set(int unit, int port, pm_info_t pm_info, uint32 index, const portmod_cpri_tx_gcw_config_t* config_info)
{
    cprimod_cpri_tx_gcw_config_t gcw_config;
    SOC_INIT_FUNC_DEFS;

    gcw_config.Ns   = config_info->Ns;
    gcw_config.Xs   = config_info->Xs;
    gcw_config.Y    = config_info->Y;
    gcw_config.mask = config_info->mask;

    gcw_config.hyper_frame_index    = config_info->hyper_frame_index;;
    gcw_config.hyper_frame_modulo   = config_info->hyper_frame_modulo;;
    gcw_config.repeat_enable        = config_info->repeat_enable;
    gcw_config.bfn0_filter_enable   = config_info->bfn0_filter_enable;
    gcw_config.bfn1_filter_enable   = config_info->bfn1_filter_enable;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_gcw_config_set(unit, port, index, &gcw_config));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_gcw_config_get(int unit, int port, pm_info_t pm_info, uint32 index, portmod_cpri_tx_gcw_config_t* config_info)
{
    cprimod_cpri_tx_gcw_config_t gcw_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_gcw_config_get(unit, port, index, &gcw_config));

    config_info->Ns   = gcw_config.Ns;
    config_info->Xs   = gcw_config.Xs;
    config_info->Y    = gcw_config.Y;
    config_info->mask = gcw_config.mask;

    config_info->hyper_frame_index    = gcw_config.hyper_frame_index;;
    config_info->hyper_frame_modulo   = gcw_config.hyper_frame_modulo;;
    config_info->repeat_enable        = gcw_config.repeat_enable;
    config_info->bfn0_filter_enable   = gcw_config.bfn0_filter_enable;
    config_info->bfn1_filter_enable   = gcw_config.bfn1_filter_enable;

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_gcw_filter_set(int unit, int port, pm_info_t pm_info, uint32 bfn0_value, uint32 bfn0_mask, uint32 bfn1_value, uint32 bfn1_mask)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_gcw_filter_set(unit, port, bfn0_value, bfn0_mask, bfn1_value, bfn1_mask));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_gcw_filter_get(int unit, int port, pm_info_t pm_info, uint32* bfn0_value, uint32* bfn0_mask, uint32* bfn1_value, uint32* bfn1_mask)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_gcw_filter_get(unit, port, bfn0_value, bfn0_mask, bfn1_value, bfn1_mask));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_tx_cw_sync_info_set(int unit, int port, pm_info_t pm_info, const uint32 hyper_frame_num, const uint32 radio_frame_num)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_next_hfn_config_set(unit,port, hyper_frame_num));
    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_next_bfn_config_set(unit,port, radio_frame_num));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_cw_sync_info_get(int unit, int port, pm_info_t pm_info, uint32* hyper_frame_num, uint32* radio_frame_num)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_next_hfn_get(unit,port,hyper_frame_num));
    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_next_bfn_get(unit,port,radio_frame_num));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_cw_l1_inband_info_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_cw_l1_inband_info_t* l1_inband_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_tx_control_word_l1_inband_info_set(unit, port, l1_inband_info));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_cw_l1_inband_info_get(int unit, int port, pm_info_t pm_info, portmod_cpri_cw_l1_inband_info_t* l1_inband_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_tx_control_word_l1_inband_info_get(unit, port, l1_inband_info));
exit:
    SOC_FUNC_RETURN;

}

/* BRCM_RSVD4 */
int cpm4x25_rsvd4_rx_control_flow_add (int unit, int port, pm_info_t pm_info, uint32 flow_id, const portmod_control_flow_config_t* config)
{
    cprimod_control_flow_config_t flow_config;
    cprimod_rsvd4_encap_data_config_t encap_decap_data;
    cprimod_header_compare_entry_t hdr_entry;
    cprimod_cpri_tag_option_t tag_option;
    SOC_INIT_FUNC_DEFS;

    cprimod_rsvd4_encap_data_config_t_init(&encap_decap_data);

    encap_decap_data.buffer_size             = config->queue_size;
    encap_decap_data.priority                = config->priority ;
    encap_decap_data.gsm_pad_size            = 0;
    encap_decap_data.gsm_extra_pad_size      = 0;
    encap_decap_data.gsm_pad_enable          = 0;
    encap_decap_data.gsm_control_location    = 0;
    if (config->proc_type == PORTMOD_CONTROL_MSG_PROC_TYPE_FE){
       encap_decap_data.bit_reversal    = 0;
    } else {
       encap_decap_data.bit_reversal    = 1;
    }

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_encap_data_config_set(unit, port, config->queue_num, &encap_decap_data));

    flow_config.proc_type       = config->proc_type;
    flow_config.queue_num       = config->queue_num;
    flow_config.sync_profile    = config->sync_profile;
    flow_config.sync_enable     = config->sync_enable;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_rx_control_flow_config_set(unit, port, flow_id, &flow_config));

    /*
     *  Set tag option to use default tag or look up.
     */
    if ((config->proc_type == PORTMOD_CONTROL_MSG_PROC_TYPE_PAYLOAD) ||
        (config->proc_type == PORTMOD_CONTROL_MSG_PROC_TYPE_FULL_MESSAGE_W_TAG)) {
        tag_option = CPRIMOD_CPRI_TAG_LOOKUP;
    } else {
        tag_option = CPRIMOD_CPRI_TAG_DEFAULT;
    }

    _SOC_IF_ERR_EXIT
        (cprimod_rx_control_flow_tag_option_set(unit, port, flow_id, tag_option));

    hdr_entry.valid = 1;
    hdr_entry.match_data = config->match_data;
    hdr_entry.mask = config->match_mask;
    hdr_entry.flow_id = flow_id;
    hdr_entry.flow_type = cprimodFlowTypeCtrl;

    _SOC_IF_ERR_EXIT
        (cprimod_header_compare_entry_add(unit, port,
                                        &hdr_entry));

exit:
    SOC_FUNC_RETURN;

}
int cpm4x25_rsvd4_rx_control_flow_delete(int unit, int port, pm_info_t pm_info, uint32 flow_id,const portmod_control_flow_config_t* config)
{
    cprimod_header_compare_entry_t hdr_entry;

    SOC_INIT_FUNC_DEFS;

    hdr_entry.valid = 1;
    hdr_entry.match_data = config->match_data;
    hdr_entry.mask = config->match_mask;
    hdr_entry.flow_id = flow_id;
    hdr_entry.flow_type = cprimodFlowTypeCtrl;

    _SOC_IF_ERR_EXIT
        (cprimod_header_compare_entry_delete(unit, port,
                                           &hdr_entry));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_fast_eth_config_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_fast_eth_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_fast_eth_config_set(unit, port, config_info));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_rx_sync_profile_entry_set(int unit, int port, pm_info_t pm_info, uint32 profile_id, const portmod_sync_profile_entry_t* entry)
{
    cprimod_sync_profile_entry_t sync_profile;
    SOC_INIT_FUNC_DEFS;

    sync_profile.count_cycle            = entry->count_cycle;
    sync_profile.message_offset         = entry->message_offset;
    sync_profile.master_frame_offset    = entry->master_frame_offset;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_rx_sync_profile_entry_set(unit, port, profile_id, &sync_profile));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_rx_sync_profile_entry_get(int unit, int port, pm_info_t pm_info, uint32 profile_id, portmod_sync_profile_entry_t* entry)
{
    cprimod_sync_profile_entry_t sync_profile;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_rx_sync_profile_entry_get(unit, port, profile_id, &sync_profile));

    entry->count_cycle          =  sync_profile.count_cycle;
    entry->message_offset       = sync_profile.message_offset;
    entry->master_frame_offset  =  sync_profile.master_frame_offset;

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rx_tag_config_set(int unit, int port, pm_info_t pm_info, uint32 default_tag, uint32 no_match_tag)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rx_tag_config_set(unit, port, default_tag, no_match_tag));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_rx_tag_gen_entry_add(int unit, int port, pm_info_t pm_info, const portmod_tag_gen_entry_t* entry)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rx_tag_gen_entry_add(unit, port, entry));
exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_rx_tag_gen_entry_delete(int unit, int port, pm_info_t pm_info, const portmod_tag_gen_entry_t* entry)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rx_tag_gen_entry_delete(unit, port, entry));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_control_flow_group_member_add(int unit, int port, pm_info_t pm_info,
                                                   uint32 group_id, uint32 priority,
                                                   uint32 queue_num,
                                                   portmod_control_msg_proc_type_t proc_type)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_control_flow_group_member_add(unit , port, group_id,
                                                      priority, queue_num,
                                                      proc_type));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_rsvd4_tx_control_flow_group_member_delete(int unit, int port, pm_info_t pm_info,
                                                      uint32 group_id,
                                                      uint32 priority)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_control_flow_group_member_delete(unit , port, group_id, priority));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_eth_message_config_set(int unit, int port, pm_info_t pm_info, uint32 msg_node, uint32 msg_subnode, uint32 msg_type, uint32 msg_padding)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_eth_message_config_set(unit, port, msg_node, msg_subnode, msg_type, msg_padding));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_rsvd4_tx_single_raw_message_config_set(int unit, int port, pm_info_t pm_info, uint32 msg_id, uint32 msg_type)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_single_raw_message_config_set(unit, port, msg_id, msg_type));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_control_flow_config_set(int unit, int port, pm_info_t pm_info, uint32 flow_id, const portmod_rsvd4_tx_config_info_t* config_info)
{
    cprimod_rsvd4_decap_data_config_t decap_data_info;
    cprimod_rsvd4_tx_config_info_t control_flow;
    SOC_INIT_FUNC_DEFS;

    cprimod_rsvd4_decap_data_config_t_init(&decap_data_info);
    cprimod_rsvd4_tx_config_info_t_init(&control_flow);

    decap_data_info.buffer_size             = config_info->queue_size;
    decap_data_info.cycle_size              = config_info->cycle_size;
    decap_data_info.gsm_pad_size            = 0;
    decap_data_info.gsm_extra_pad_size      = 0;
    decap_data_info.gsm_pad_enable          = 0;
    decap_data_info.gsm_control_location    = 0;
    if (config_info->proc_type == PORTMOD_CONTROL_MSG_PROC_TYPE_FE){
       decap_data_info.bit_reversal    = 0;
    } else {
       decap_data_info.bit_reversal    = 1;
    }

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_decap_data_config_set(unit, port, config_info->queue_num, &decap_data_info));

     control_flow.proc_type = config_info->proc_type;
     control_flow.queue_num = config_info->queue_num;
     control_flow.crc_mode = config_info->crc_mode;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_control_flow_config_set(unit, port, flow_id, &control_flow));


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_control_flow_config_get(int unit, int port, pm_info_t pm_info, uint32 flow_id,  portmod_rsvd4_tx_config_info_t* config_info)
{
    cprimod_rsvd4_decap_data_config_t encap_decap_data;
    cprimod_rsvd4_tx_config_info_t tx_config;
    SOC_INIT_FUNC_DEFS;

    cprimod_rsvd4_decap_data_config_t_init(&encap_decap_data);

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_control_flow_config_get(unit, port, flow_id, &tx_config));


    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_decap_data_config_get(unit, port, tx_config.queue_num, &encap_decap_data ));

    config_info->queue_num= tx_config.queue_num;
    config_info->proc_type= tx_config.proc_type;
    config_info->crc_mode = tx_config.crc_mode;
    config_info->queue_size = encap_decap_data.buffer_size;
    config_info->cycle_size = encap_decap_data.cycle_size;


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_control_flow_header_index_set(int unit, int port, pm_info_t pm_info, uint32 roe_flow_id, uint32 index)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_control_flow_header_index_set(unit, port, roe_flow_id, index));


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_control_flow_header_index_get(int unit, int port, pm_info_t pm_info, uint32 roe_flow_id, uint32* index)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_control_flow_header_index_get(unit, port, roe_flow_id, index));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_control_header_entry_set(int unit, int port, pm_info_t pm_info, uint32 index, uint32 header_node, uint32 header_subnode, uint32 payload_node)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_control_header_entry_set(unit, port, index, header_node, header_subnode, payload_node));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_control_header_entry_get(int unit, int port, pm_info_t pm_info, uint32 index, uint32* header_node, uint32* header_subnode, uint32* payload_node)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_control_header_entry_get(unit, port, index, header_node, header_subnode, payload_node));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_single_tunnel_message_config_set(int unit, int port, pm_info_t pm_info, cprimod_cpri_crc_option_t crc_option)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_single_tunnel_message_config_set(unit, port, crc_option));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_interrupt_link_status_get(int unit, int port,
                                                pm_info_t pm_info,
                                                int* is_link_intr_status)
{
    int phy, lport, ii = 0, val = 0;
    int tmp_link_change_status=0;
    SOC_INIT_FUNC_DEFS;

    /*
     * Check all the CPRI port link status in core
     */
    PORTMOD_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy){
      _SOC_IF_ERR_EXIT(PM4x25_LANE2PORT_GET(unit, pm_info, ii, &lport));
      /* skip if port is invalid */
      if(lport < 0) {
         continue;
      }
      _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, lport, &val));
      /* Only if CPRI port */
      if (val) {
          _SOC_IF_ERR_EXIT(cprimod_cpri_port_interrupt_link_status_get(unit,
                                                  lport, &tmp_link_change_status));
          *is_link_intr_status |= tmp_link_change_status;
      }
       ii++;
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_interrupt_enable_set(int unit, int port,
                                      pm_info_t pm_info,
                                      portmod_cpri_port_intr_type_t intr_type,
                                      int data, int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_cpri_port_interrupt_enable_set(unit, port,
                                                            intr_type, data,
                                                            enable));


exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_interrupt_enable_get(int unit, int port,
                                      pm_info_t pm_info,
                                      portmod_cpri_port_intr_type_t intr_type,
                                      int data, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_cpri_port_interrupt_enable_get(unit, port,
                                                            intr_type, data,
                                                            enable));


exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_pm_interrupt_process(int unit, int pm_id, pm_info_t pm_info)
{
    int lane_index = 0;
    int cpri_port = 0;
    int logical_port = -1;
    SOC_INIT_FUNC_DEFS;

    /*
     * Process all the CPRI port interrupt in the core.
     */
    for (lane_index = 0 ; lane_index < MAX_PORTS_PER_PM4X25; lane_index++) {

        _SOC_IF_ERR_EXIT
            (PM4x25_LANE2PORT_GET(unit, pm_info, lane_index, &logical_port));

        /* skip if invalid port */
        if (logical_port < 0) {
            continue;
        }

        _SOC_IF_ERR_EXIT
            (cprimod_cpri_or_enet_port_get(unit, logical_port, &cpri_port));

        /* Process for  CPRI port only. */
        if (cpri_port) {
            _SOC_IF_ERR_EXIT
                (cprimod_cpri_port_interrupt_process(unit, logical_port));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_fast_clk_bit_time_period_set(int unit, int port, pm_info_t pm_info, uint32 bit_time_period)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_fast_clk_bit_time_period_set(unit, port, bit_time_period));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_fast_clk_bit_time_period_get(int unit, int port, pm_info_t pm_info, uint32* bit_time_period)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_fast_clk_bit_time_period_get(unit, port, bit_time_period));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_1588_bit_time_period_set(int unit, int port, pm_info_t pm_info, portmod_direction_t direction, cprimod_port_speed_t speed)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_1588_bit_time_period_set(unit, port, direction, speed));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_1588_bit_time_period_get(int unit, int port, pm_info_t pm_info, portmod_direction_t direction, uint32* bit_time_period)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_1588_bit_time_period_get(unit, port, direction, bit_time_period));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_1588_timestamp_capture_config_set(int unit, int port, pm_info_t pm_info, portmod_direction_t direction, const cprimod_1588_capture_config_t* config)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_1588_timestamp_capture_config_set(unit, port,
                                                            direction, config));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_1588_timestamp_capture_config_get(int unit, int port, pm_info_t pm_info, portmod_direction_t direction, cprimod_1588_capture_config_t* config)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_1588_timestamp_capture_config_get(unit, port, direction, config));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_1588_captured_timestamp_get(int unit, int port, pm_info_t pm_info, portmod_direction_t direction, portmod_1588_ts_type_t ts_type, uint8 mx_cnt, uint8* count, uint64* captured_time)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_1588_captured_timestamp_get(unit, port, direction, ts_type, mx_cnt, count, captured_time));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_1588_timestamp_adjust_set(int unit, int port, pm_info_t pm_info, portmod_direction_t direction, const cprimod_1588_time_t* adjust_time)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_1588_timestamp_adjust_set(unit, port, direction, adjust_time));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_1588_timestamp_adjust_get(int unit, int port, pm_info_t pm_info, portmod_direction_t direction, cprimod_1588_time_t* adjust_time)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_1588_timestamp_adjust_get(unit, port, direction, adjust_time));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_1588_timestamp_fifo_config_set(int unit, int port, pm_info_t pm_info, portmod_direction_t direction, uint16 modulo)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_1588_timestamp_fifo_config_set(unit, port, direction, modulo));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_1588_timestamp_fifo_config_get(int unit, int port, pm_info_t pm_info, portmod_direction_t direction, uint16* modulo)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_1588_timestamp_fifo_config_get(unit, port, direction, modulo));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_1588_timestamp_cmic_config_set(int unit, int port, pm_info_t pm_info, portmod_direction_t direction, uint16 modulo)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_1588_timestamp_cmic_config_set(unit, port, direction, modulo));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_1588_timestamp_cmic_config_get(int unit, int port, pm_info_t pm_info, portmod_direction_t direction, uint16* modulo)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_1588_timestamp_cmic_config_get(unit, port, direction, modulo));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_vcos_get(int unit, int port, pm_info_t pm_info, const portmod_vcos_speed_config_t* speed_config_list, int size, portmod_dual_vcos_t* dual_vco)
{
    int index = 0;
    int index_enet = 0;
    int vco  = 0;
    int vco0 = 0;
    int vco1 = 0;
    portmod_vcos_speed_config_t vcos_speed_config[4];
    SOC_INIT_FUNC_DEFS;
    if (size == 0) return SOC_E_NONE;
    for (index = 0; index < size; index++){
        if ((speed_config_list[index].port_type == portmodCpmPrtEthOfMixedMode) ||
            (speed_config_list[index].port_type == portmodCpmPrtEthMode)) {
            sal_memcpy(&vcos_speed_config[index_enet], &speed_config_list[index], sizeof(portmod_vcos_speed_config_t));
            index_enet++;
        } else {
            switch(speed_config_list[index].port_type) {
            case portmodCpmPrtRsvd4Mode:
                if ((vco0 == 0) || (vco0 == phymod_TSCF_PLL_DIV200))  {
                    vco0=phymod_TSCF_PLL_DIV200;
                } else if ((vco1 == 0) || (vco1 == phymod_TSCF_PLL_DIV200)) {
                    vco1=phymod_TSCF_PLL_DIV200;
                } else {
                    LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "fail to get vcos -- no vco matches lane %d speed %d\n"),
                    speed_config_list[index].num_lanes, speed_config_list[index].speed));
                    return SOC_E_PARAM;
                }
                break;
            case portmodCpmPrtRsvd4OfMixedMode:
                if ((vco0 == 0) || (vco0 == phymod_TSCF_PLL_DIV200))  {
                    vco0=phymod_TSCF_PLL_DIV200;
                } else {
                    LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "fail to get vcos -- no vco matches lane %d speed %d\n"),
                    speed_config_list[index].num_lanes, speed_config_list[index].speed));
                    return SOC_E_PARAM;
                }
                break;
            case portmodCpmPrtCpriMode:
                vco = _cprimod_port_speed_id_to_vco(speed_config_list[index].speed, _SHR_PORT_ENCAP_CPRI);
                if ((vco0 == 0) || (vco0 == vco)) {
                    vco0=vco;
                } else if ((vco1 == 0) || (vco1 == vco)) {
                    vco1=vco;
                } else {
                    LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "fail to get vcos -- no vco matches lane %d speed %d\n"),
                    speed_config_list[index].num_lanes, speed_config_list[index].speed));
                    return SOC_E_PARAM;
                }
                break;
            case portmodCpmPrtCpriOfMixedMode:
                vco = _cprimod_port_speed_id_to_vco(speed_config_list[index].speed, _SHR_PORT_ENCAP_CPRI);
                if ((vco0 == 0) || (vco0 == vco)) {
                    vco0=vco;
                } else {
                    LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "fail to get vcos -- no vco matches lane %d speed %d\n"),
                    speed_config_list[index].num_lanes, speed_config_list[index].speed));
                    return SOC_E_PARAM;
                }
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "fail to get vcos -- no vco matches lane %d speed %d\n"),
                speed_config_list[index].num_lanes, speed_config_list[index].speed));
                return SOC_E_PARAM;
                break;
            }
        }
    }
    if(index_enet != 0) {
        _rv = pm4x25_port_vcos_get(unit, port, pm_info, vcos_speed_config, index_enet, dual_vco);
    }
    if(vco0 != 0) {
        dual_vco->vco_0 = _cprimod_pll_mult_to_vco_freq(vco0);
    }
    if(vco1 != 0) {
        dual_vco->vco_1 = _cprimod_pll_mult_to_vco_freq(vco1);
    }
    SOC_FUNC_RETURN;
}


/*
 * Validate if a set of speed can be configured for given PORTMACRO without
 * affecting the other active ports.
 *
 * Inputs:
 *     unit:           unit number;
 *     pm_id           portmacro ID;
 *     pm_info:        pm_info data structure;
 *     ports:          Logical port bitmaps which reqiring the input vcos;
 *     all_cpri:       PLL switch all_cpri. 1 => all sister ports are CPRI/RSVD4, 0 => atlest 1 sister port is Ethernet
 *     vco_setting:    information about the speeds to validate: num_speeds, speed_config_list, port_starting_lane_list, speed_for_pcs_bypass_port
 *
 * Output:
 *     vco_setting:    If validation pass, vco_setting will indicating
 *                     what's the new vco rates and whether they are new or not; tvco, is_tvco_new (not using the OVCO fields)
 */
int cpm4x25_pm_speed_config_validate(int unit,
                                     int pm_id,
                                     pm_info_t pm_info,
                                     const portmod_pbmp_t* ports,
                                     int all_cpri,
                                     portmod_pm_vco_setting_t* vco_setting)
{
    int i, tmp_port, port = 0;
    int del_lanes = 0;
    int pll0_vco_rate = 0, pll1_vco_rate = 0;
    int pll0_active_lane_bitmap=0, pll1_active_lane_bitmap=0;
    portmod_vcos_speed_config_t speed_config_list[MAX_PORTS_PER_PM4X25];
    portmod_dual_vcos_t new_vco_config;
    int speed_id;
    int rv;
    SOC_INIT_FUNC_DEFS;
    rv = PM4x25_PLL0_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll0_active_lane_bitmap);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_PLL1_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll1_active_lane_bitmap);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_PLL0_VCO_RATE_GET(unit, pm_info, &pll0_vco_rate);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_PLL1_VCO_RATE_GET(unit, pm_info, &pll1_vco_rate);
    _SOC_IF_ERR_EXIT(rv);

    /** Create a bitmap of all ports on the PM */
    for(i = 0 ;i < MAX_PORTS_PER_PM4X25; ++i) {
        _SOC_IF_ERR_EXIT(PM4x25_LANE2PORT_GET(unit, pm_info, i, &tmp_port));
        if (tmp_port != -1) {
            if (PORTMOD_PBMP_MEMBER(*ports, tmp_port)) {
                del_lanes |= (1 << i);
                port = tmp_port;
            }
        }
    }

    for(i = 0 ;i < vco_setting->num_speeds; ++i) {
        speed_config_list[i].port_type = vco_setting->port_type_list[i];
        speed_config_list[i].speed = vco_setting->speed_config_list[i].speed;
        speed_config_list[i].num_lanes = vco_setting->speed_config_list[i].num_lane;
        if((speed_config_list[i].port_type == portmodCpmPrtCpriMode) || (speed_config_list[i].port_type == portmodCpmPrtCpriOfMixedMode)) {
            speed_id= _int_to_cprimod_supported_port_speed_t(speed_config_list[i].speed);
            if (speed_id < 0) {
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                 (_SOC_MSG("Requested Speed is not valid or supported %d\n"), speed_config_list[i].speed));
            }
            speed_config_list[i].speed = speed_id;
        }
        if((speed_config_list[i].port_type == portmodCpmPrtRsvd4Mode) || (speed_config_list[i].port_type == portmodCpmPrtRsvd4OfMixedMode)) {
            if(speed_config_list[i].speed == 3072) {
                speed_config_list[i].speed = cprimodRsvd4SpdMult4X;
            } else if(speed_config_list[i].speed == 6144) {
                speed_config_list[i].speed = cprimodRsvd4SpdMult8X;
            } else {
                _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE,
                 (_SOC_MSG("Bad RSVD4 speed requested %d\n"), speed_config_list[i].speed));
            }
        }
    }

    pll0_active_lane_bitmap &= ~(del_lanes);
    pll1_active_lane_bitmap &= ~(del_lanes);

    _SOC_IF_ERR_EXIT(cpm4x25_port_vcos_get(unit, port, pm_info, speed_config_list, vco_setting->num_speeds, &new_vco_config));

    /* only cpri ports */
    if(all_cpri == 1) {
        if(pll0_active_lane_bitmap == 0) {
            pll0_vco_rate = 0;
        }
        if(pll1_active_lane_bitmap == 0) {
            pll1_vco_rate = 0;
        }
        /* pll1 is free check if we need only one new vco */
        if((pll0_vco_rate > 0) && (pll1_vco_rate <= 0)) {
            if((new_vco_config.vco_1 != 0) && (new_vco_config.vco_0 != pll0_vco_rate) && (new_vco_config.vco_1 != pll0_vco_rate)) {
                 _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE,
                  (_SOC_MSG("FLEX need more VCOs then port can support")));
            }
        /* pll0 is free check if we need only one new vco */
        } else if((pll0_vco_rate <= 0) && (pll1_vco_rate > 0)) {
            if((new_vco_config.vco_1 != 0) && (new_vco_config.vco_0 != pll1_vco_rate) && (new_vco_config.vco_1 != pll1_vco_rate)) {
                 _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE,
                  (_SOC_MSG("FLEX need more VCOs then port can support")));
            }
        /* no is free check if we an use existing vco */
        } else if((pll0_vco_rate > 0) && (pll1_vco_rate > 0)) {
            if((new_vco_config.vco_0 != 0) && (new_vco_config.vco_0 != pll0_vco_rate) && (new_vco_config.vco_0 != pll1_vco_rate)) {
                 _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE,
                  (_SOC_MSG("FLEX need PLL0 that is used by other ports")));
            }
            if((new_vco_config.vco_1 > 0) && (new_vco_config.vco_1 != pll0_vco_rate) && (new_vco_config.vco_1 != pll1_vco_rate)) {
                 _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE,
                  (_SOC_MSG("FLEX need PLL1 that is used by other ports")));
            }
        }
    }

    /* PLL0 has active ports */
    if((all_cpri == 0) && (pll0_active_lane_bitmap != 0)) {
        if(new_vco_config.vco_0 != pll0_vco_rate) {
             _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE,
              (_SOC_MSG("FLEX need PLL0 that is used by other ports")));
        }
    }
    /* PLL1 has active ports */
    if((all_cpri == 0) && (pll1_active_lane_bitmap != 0)) {
        if(new_vco_config.vco_1 != pll1_vco_rate) {
             _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE,
              (_SOC_MSG("FLEX need PLL1 that is used by other ports")));
        }
    }

    /* If pll0 referance is changing all ports needs to go down */
    /* pll0_active_lane_bitmap is already checked and should be in sync */
    /* Check if any ports are up in PLL1 and PLL0 referance needs to change then do not let flex continue */
    if((pll1_active_lane_bitmap != 0) && (pll0_vco_rate > 0)) {
        if((pll0_vco_rate == portmodVCO19P660G) || (pll0_vco_rate == portmodVCO20P275G) ||
           (pll0_vco_rate == portmodVCO24P330G) || (pll0_vco_rate == portmodVCO24P576G)) {
            if((new_vco_config.vco_0 != portmodVCO19P660G) && (new_vco_config.vco_0 != portmodVCO20P275G) &&
               (new_vco_config.vco_0 != portmodVCO24P330G) && (new_vco_config.vco_0 != portmodVCO24P576G)) {
                _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE,
                 (_SOC_MSG("FLEX PLL0 going from CPRI to Ethernet Ref, all ports need to go down")));
            }
        }
        if((pll0_vco_rate != portmodVCO19P660G) && (pll0_vco_rate != portmodVCO20P275G) &&
           (pll0_vco_rate != portmodVCO24P330G) && (pll0_vco_rate != portmodVCO24P576G)) {
            if((new_vco_config.vco_0 == portmodVCO19P660G) || (new_vco_config.vco_0 == portmodVCO20P275G) ||
               (new_vco_config.vco_0 == portmodVCO24P330G) || (new_vco_config.vco_0 == portmodVCO24P576G)) {
                _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE,
                 (_SOC_MSG("FLEX PLL0 going from Ethernet to CPRI Ref, all ports need to go down")));
            }
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_rx_agnostic_config_set (int unit, int port, pm_info_t pm_info,
                                              int enable,
                                              const portmod_cpri_rx_agnostic_config_info_t* config_info)
{
    uint32 queue_num = 0xFF;
    uint32 frame_len =0;
    uint32 cw_len = 0;
    uint32 roe_packet_size;
    cprimod_cpri_container_config_t container_info;
    cprimod_payload_size_info_t roe_payload_info;
    cprimod_encap_decap_data_config_t encap_data_info;
    cprimod_encap_presentation_time_config_t presentation_config;
    cprimod_port_interface_config_t interface_config;
    int lane;
    phymod_phy_access_t phy_access;

    SOC_INIT_FUNC_DEFS;

  /*
     *  Check to see if the port is in CPRI mode.
     *  If not, return error.
     */
    _SOC_IF_ERR_EXIT
        (cprimod_port_rx_interface_config_get(unit, port, &interface_config));

    if(interface_config.interface != cprimodCpri) {
        _SOC_EXIT_WITH_ERR
                    (SOC_E_PARAM, ("Port is not in CPRI Mode.\n"));
    }

    /*
     *  Setup the basic frame. For both enable and disable
     */

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_agnostic_mode_basic_frame_config_set(unit, port, CPRIMOD_DIR_RX, enable));

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_basic_frame_info_get(unit, port, CPRIMOD_DIR_RX, &frame_len, &cw_len));

    if (enable) {
        /* Enable  */

        if (SOC_E_NONE != cprimod_cpri_container_config_t_init(&container_info)) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config_info initialization failed"));
        }

        container_info.map_method           = cprimod_cpri_frame_map_method_1;
        container_info.Naxc                 = frame_len-cw_len;
        container_info.K                    = 1;
        container_info.Nst                  = 0;
        container_info.Na                   = 0;
        container_info.Nc                   = 0;
        container_info.Nv                   = 0;
        container_info.frame_sync_mode      = config_info->sync_mode;
        container_info.basic_frame_offset   = config_info->basic_frame_offset;
        container_info.radio_frame_offset   = config_info->radio_frame_offset;
        container_info.hyper_frame_offset   = config_info->hyper_frame_offset;

        _SOC_IF_ERR_EXIT
            (cprimod_cpri_rx_axc_container_config_set(unit, port, 0 /*Agnostic AxC*/ , &container_info));

        switch (config_info->payload_multiple) {
            case CPRIMOD_CPRI_PAYLOAD_MULTIPLE_1X:
                roe_packet_size = frame_len ;
                break;

            case CPRIMOD_CPRI_PAYLOAD_MULTIPLE_1P5X:
                roe_packet_size = (frame_len*3) /2;
                break;

            case CPRIMOD_CPRI_PAYLOAD_MULTIPLE_2X:
                roe_packet_size = frame_len * 2;
                break;

            case CPRIMOD_CPRI_PAYLOAD_MULTIPLE_4X:
                roe_packet_size = frame_len * 4;
                break;

            case CPRIMOD_CPRI_PAYLOAD_MULTIPLE_8X:
                roe_packet_size = frame_len * 8;
                break;
            default:
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Not Supported Multiple.."));
                break;

        }
        roe_payload_info.packet_size        = roe_packet_size;
        roe_payload_info.last_packet_num    = 0 ; /* not used. */
        roe_payload_info.last_packet_size   = roe_packet_size ; /* not used. */

        _SOC_IF_ERR_EXIT
            (cprimod_rx_roe_payload_size_set(unit, port, 0 /* agnostic axc */, &roe_payload_info));

        queue_num                           = 0;  /* queue # is always 0 for agnostic. */
        encap_data_info.sample_size         = 8;
        encap_data_info.truncation_enable   = 0;
        encap_data_info.truncation_type     = 0; /* Not used */
        encap_data_info.compression_type    = cprimod_no_compress;
        encap_data_info.mux_enable          = 0;
        encap_data_info.bit_reversal        = 0;
        encap_data_info.buffer_size         = (roe_packet_size/8) * 2 ;
        encap_data_info.priority            = 0;
        encap_data_info.cycle_size          = 0;
        encap_data_info.is_agnostic         = 1;

        _SOC_IF_ERR_EXIT
            (cprimod_cpri_encap_data_config_set(unit, port, queue_num, &encap_data_info ));


            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_config_t_init(&presentation_config));

            presentation_config.mod_count           = config_info->pres_mod_count;
            presentation_config.frame_sync_mode     = config_info->sync_mode;
            presentation_config.radio_frame_offset  = config_info->radio_frame_offset;
            presentation_config.hyper_frame_offset  = config_info->hyper_frame_offset;
            presentation_config.approx_inc_disable  = FALSE;

            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_config_set(unit, port, &presentation_config));
    
        if (config_info->ordering_info_option == PORTMOD_CPRI_ROE_ORDERING_INFO_OPTION_TIMESTAMP) {
            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_enable_set(unit, port, TRUE));
        } else {
            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_enable_set(unit, port, FALSE));
        }

    } else {
        _SOC_IF_ERR_EXIT
            (cprimod_encap_presentation_time_enable_set(unit, port, FALSE));

       /*
         * retrieve the interface information and restore.
         */

        sal_memcpy(&phy_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_core_access_t));
        lane = (SOC_INFO(unit).port_l2p_mapping[port]-1)%CPRIMOD_MAX_LANES_PER_CORE;
        phy_access.access.lane_mask = (1<<lane);

        _SOC_IF_ERR_EXIT
            (cprimod_port_rx_interface_config_set(unit, port, &phy_access, &interface_config));
    }

    /*
     * Activate the basic frame 
     */

    _SOC_IF_ERR_EXIT
        (cpm4x25_cpri_port_stage_activate(unit, port, pm_info, portmodCpriStageRx));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_agnostic_config_get (int unit, int port, pm_info_t pm_info,
                                              int *enable,
                                              portmod_cpri_rx_agnostic_config_info_t* config_info)
{
    uint32 frame_len;
    uint32 cw_len;
    uint32 frame_multiple;
    cprimod_cpri_container_config_t container_info;
    cprimod_payload_size_info_t roe_payload_info;
    cprimod_encap_presentation_time_config_t presentation_config;
    cprimod_agnostic_mode_type_t mode;
    int presentation_enable=0;

    SOC_INIT_FUNC_DEFS;



    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_agnostic_mode_get(unit, port, enable, &mode));

    if (*enable && (mode == cprimod_agnostic_mode_cpri)) {

        /*
         * if enabled, get the rest of the information.
         */

        _SOC_IF_ERR_EXIT
            (cprimod_encap_presentation_time_enable_get(unit, port, &presentation_enable));

        if (presentation_enable) {
            config_info->ordering_info_option = PORTMOD_CPRI_ROE_ORDERING_INFO_OPTION_TIMESTAMP;
        } else {
            config_info->ordering_info_option = PORTMOD_CPRI_ROE_ORDERING_INFO_OPTION_SEQ_NUM;
        }

        _SOC_IF_ERR_EXIT
            (cprimod_cpri_basic_frame_info_get(unit, port, CPRIMOD_DIR_RX, &frame_len, &cw_len));

        _SOC_IF_ERR_EXIT
            (cprimod_rx_roe_payload_size_get(unit, port, 0 /*agnostic AxC */, &roe_payload_info));


        _SOC_IF_ERR_EXIT
            (cprimod_cpri_rx_axc_container_config_get(unit, port, 0 /*agnostic axc */, &container_info));

        _SOC_IF_ERR_EXIT
            (cprimod_encap_presentation_time_config_get(unit, port, &presentation_config));



        /* to accomodate 1.5, do calculation on twice the value. */
        frame_multiple = (roe_payload_info.packet_size*2)/frame_len;

        switch (frame_multiple) {
            case 2:
                config_info->payload_multiple = CPRIMOD_CPRI_PAYLOAD_MULTIPLE_1X;
                break;
            case 3:
                config_info->payload_multiple = CPRIMOD_CPRI_PAYLOAD_MULTIPLE_1P5X;
                break;
            case 4:
                config_info->payload_multiple = CPRIMOD_CPRI_PAYLOAD_MULTIPLE_2X;
                break;
            case 8:
                config_info->payload_multiple = CPRIMOD_CPRI_PAYLOAD_MULTIPLE_4X;
                break;
            case 16:
                config_info->payload_multiple = CPRIMOD_CPRI_PAYLOAD_MULTIPLE_8X;
                break;
            default:
                break;
        }

        config_info->sync_mode          = container_info.frame_sync_mode;
        config_info->radio_frame_offset = container_info.radio_frame_offset;
        config_info->hyper_frame_offset = container_info.hyper_frame_offset;
        config_info->basic_frame_offset = container_info.basic_frame_offset;
        config_info->pres_mod_count     = presentation_config.mod_count;
    } else {
        *enable = 0;
        /*
         * if disabled, zero out all the parameters.
         */
        _SOC_IF_ERR_EXIT
            (portmod_cpri_rx_agnostic_config_info_t_init(unit, config_info));
        config_info->ordering_info_option   = 0;
        config_info->sync_mode              = 0;
    }
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_tx_agnostic_config_set (int unit, int port, pm_info_t pm_info,
                                              int enable,
                                              const portmod_cpri_tx_agnostic_config_info_t* config_info)
{
    uint32 queue_num = 0xFF;
    uint32 frame_len =0;
    uint32 cw_len = 0;
    uint32 roe_packet_size;
    cprimod_cpri_container_config_t container_info;
    cprimod_payload_size_info_t roe_payload_info;
    cprimod_encap_decap_data_config_t decap_data_info;
    cprimod_decap_agnostic_mode_config_t agnostic_config;
    cprimod_port_interface_config_t interface_config;

    SOC_INIT_FUNC_DEFS;

   /*
     *  Check to see if the port is in CPRI mode.
     *  If not, return error.
     */
    _SOC_IF_ERR_EXIT
        (cprimod_port_tx_interface_config_get(unit, port, &interface_config));

    if(interface_config.interface != cprimodCpri) {
        _SOC_EXIT_WITH_ERR
                    (SOC_E_PARAM, ("Port is not in CPRI Mode.\n"));
    }

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_basic_frame_info_get(unit, port, CPRIMOD_DIR_TX, &frame_len, &cw_len));

    /*
     *  Program Basic Frame Assembler.
     */

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_agnostic_mode_basic_frame_config_set(unit, port, CPRIMOD_DIR_TX, enable));

    if (enable ) {
        if (SOC_E_NONE != cprimod_cpri_container_config_t_init(&container_info)) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config_info initialization failed"));
        }

        container_info.map_method           = cprimod_cpri_frame_map_method_1;
        container_info.Naxc                 = frame_len;
        container_info.K                    = 1;
        container_info.Nst                  = 0;
        container_info.Na                   = 0;
        container_info.Nc                   = 0;
        container_info.Nv                   = 0;
        container_info.frame_sync_mode      = config_info->sync_mode;
        container_info.basic_frame_offset   = config_info->basic_frame_offset;
        container_info.radio_frame_offset   = config_info->radio_frame_offset;
        container_info.hyper_frame_offset   = config_info->hyper_frame_offset;

        _SOC_IF_ERR_EXIT
            (cprimod_cpri_tx_axc_container_config_set(unit, port, 0, &container_info));

        switch (config_info->payload_multiple) {
            case CPRIMOD_CPRI_PAYLOAD_MULTIPLE_1X:
                roe_packet_size = frame_len ;
                break;

            case CPRIMOD_CPRI_PAYLOAD_MULTIPLE_1P5X:
                roe_packet_size = (frame_len*3) /2;
                break;

            case CPRIMOD_CPRI_PAYLOAD_MULTIPLE_2X:
                roe_packet_size = frame_len * 2;
                break;

            case CPRIMOD_CPRI_PAYLOAD_MULTIPLE_4X:
                roe_packet_size = frame_len * 4;
                break;

            case CPRIMOD_CPRI_PAYLOAD_MULTIPLE_8X:
                roe_packet_size = frame_len * 8;
                break;

            default:
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Not Supported Multiple.."));
                break;

        }
        roe_payload_info.packet_size        = roe_packet_size;
        roe_payload_info.last_packet_num    = 0 ; /* not used. */
        roe_payload_info.last_packet_size   = roe_packet_size ; /* not used. */

        _SOC_IF_ERR_EXIT
            (cprimod_tx_roe_payload_size_set(unit, port, 0, &roe_payload_info));

        queue_num                           = 0;  /* queue # is always 0 for agnostic. */
        decap_data_info.sample_size         = 8;
        decap_data_info.truncation_enable   = 0;
        decap_data_info.truncation_type     = 0; /* Not used */
        decap_data_info.compression_type    = cprimod_no_compress;
        decap_data_info.mux_enable          = 0;
        decap_data_info.bit_reversal        = 0;
        decap_data_info.buffer_size         = roe_packet_size/8 ;
        decap_data_info.priority            = 0;
        /*
         * This cycle size  not used in agnostic,
         * it is configured in cprimod_decap_agnostic_mode_config_set
         * instead.
         */
        decap_data_info.cycle_size          = 0;
        decap_data_info.is_agnostic         = 1;

        _SOC_IF_ERR_EXIT
            (cprimod_cpri_decap_data_config_set(unit, port, queue_num, &decap_data_info ));

        _SOC_IF_ERR_EXIT
            (cprimod_decap_agnostic_mode_config_t_init(&agnostic_config));

        agnostic_config.mod_count       = config_info->queue_mod_count; /* cycle size */
        agnostic_config.packet_size     = roe_packet_size/8 ;
        agnostic_config.queue_num       = queue_num;

        _SOC_IF_ERR_EXIT
            (cprimod_decap_agnostic_mode_config_set(unit, port, &agnostic_config));

        _SOC_IF_ERR_EXIT
            (cprimod_decap_presentation_time_config_set(unit, port, config_info->pres_mod_offset, config_info->pres_mod_count));
    }

    /*
     * enable/disable as well as setting the timestamp or seq_num.
     */

    _SOC_IF_ERR_EXIT
        (cprimod_decap_agnostic_mode_enable_set(unit, port, enable, config_info->ordering_info_option));


    /*
     * Activate the basic frame
     */

    _SOC_IF_ERR_EXIT
        (cpm4x25_cpri_port_stage_activate(unit, port, pm_info, portmodCpriStageTx));


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_agnostic_config_get (int unit, int port, pm_info_t pm_info,
                                              int* enable,
                                              portmod_cpri_tx_agnostic_config_info_t* config_info)
{
    uint32 frame_multiple;
    uint32 frame_len;
    uint32 cw_len;
    cprimod_cpri_container_config_t container_info;
    cprimod_payload_size_info_t roe_payload_info;
    cprimod_decap_agnostic_mode_config_t agnostic_config;
    cprimod_cpri_roe_ordering_info_option_t mode;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_basic_frame_info_get(unit, port, CPRIMOD_DIR_TX, &frame_len, &cw_len));

    _SOC_IF_ERR_EXIT
        (cprimod_decap_agnostic_mode_enable_get(unit, port, enable, &mode));

    if (*enable) {
        /*
         * if enabled, get the rest of the information.
         */
        config_info->ordering_info_option = mode;

        _SOC_IF_ERR_EXIT
            (cprimod_cpri_tx_axc_container_config_get(unit, port, 0, &container_info));

        config_info->sync_mode          = container_info.frame_sync_mode ;
        config_info->basic_frame_offset = container_info.basic_frame_offset;
        config_info->radio_frame_offset = container_info.radio_frame_offset;
        config_info->hyper_frame_offset = container_info.hyper_frame_offset;

        _SOC_IF_ERR_EXIT
            (cprimod_tx_roe_payload_size_get(unit, port, 0, &roe_payload_info));

        /*
         * calculate the double value to accomodate 1.5x case.
         */
        frame_multiple = (roe_payload_info.packet_size *2)/frame_len;

        switch (frame_multiple) {
            case 2:
                config_info->payload_multiple = CPRIMOD_CPRI_PAYLOAD_MULTIPLE_1X;
                break;
            case 3:
                config_info->payload_multiple = CPRIMOD_CPRI_PAYLOAD_MULTIPLE_1P5X;
                break;
            case 4:
                config_info->payload_multiple = CPRIMOD_CPRI_PAYLOAD_MULTIPLE_2X;
                break;
            case 8:
                config_info->payload_multiple = CPRIMOD_CPRI_PAYLOAD_MULTIPLE_4X;
                break;
            case 16:
                config_info->payload_multiple = CPRIMOD_CPRI_PAYLOAD_MULTIPLE_8X;
                break;
            default:
                break;
        }
        _SOC_IF_ERR_EXIT
            (cprimod_decap_agnostic_mode_config_get(unit, port, &agnostic_config));

        config_info->queue_mod_count = agnostic_config.mod_count;

        _SOC_IF_ERR_EXIT
            (cprimod_decap_presentation_time_config_get(unit, port, &config_info->pres_mod_offset, &config_info->pres_mod_count));
    } else {
        /*
         * if disabled, zero out the parameters.
         */
        _SOC_IF_ERR_EXIT
            (portmod_cpri_tx_agnostic_config_info_t_init(unit,config_info));
        config_info->ordering_info_option   = 0;
        config_info->sync_mode              = 0;
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_encap_presentation_time_config_set (int unit, int port, pm_info_t pm_info,
                                                          portmod_presentation_time_attribute_t attribute,
                                                          portmod_presentation_time_t* time)
{
    uint32 time_value;
    uint32 ptp_time_value;
    uint32 user_time_value;
    int lane_index;
    uint32 lane_map;
    SOC_INIT_FUNC_DEFS;

    time_value = time->time;

    _SOC_IF_ERR_EXIT
        (_cpm4x25_port_index_get(unit, port, pm_info, &lane_index, &lane_map));

    switch (attribute) {
        case PORTMOD_ENCAP_PRESENTATION_TIME_ADJUST:

            _SOC_IF_ERR_EXIT
                (PM4x25_USER_TIME_ADJUST_SET(unit, pm_info, time_value, lane_index));

            _SOC_IF_ERR_EXIT
                (PM4x25_PTP_TIME_ADJUST_GET(unit, pm_info, &ptp_time_value, lane_index));

            time_value = (uint32)(((int32)(time_value<<3) + ((int32)ptp_time_value<<3)) >>3) & 0x1FFFFFFF;
            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_adjust_set(unit, port, time_value));
            break;
        case PORTMOD_ENCAP_PRESENTATION_APPRIOX_INC:
            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_approximate_increment_set(unit, port, time_value));
            break;

        case PORTMOD_ENCAP_PRESENTATION_PTP_TIME_ADJUST:

            _SOC_IF_ERR_EXIT
                (PM4x25_USER_TIME_ADJUST_GET(unit, pm_info, &user_time_value, lane_index));
            _SOC_IF_ERR_EXIT
                (PM4x25_PTP_TIME_ADJUST_GET(unit, pm_info, &ptp_time_value, lane_index));
            /*
             * if both stored value are 0, could be init and warmboot from unsupported
             * warmboot version, try to retrieve the user adjustment value from HW.
             */
            if (ptp_time_value == 0) {
                if (user_time_value == 0) {
                    _SOC_IF_ERR_EXIT
                        (cprimod_encap_presentation_time_adjust_get(unit, port, &user_time_value));
                    _SOC_IF_ERR_EXIT
                        (PM4x25_USER_TIME_ADJUST_SET(unit, pm_info, user_time_value, lane_index));
                }
            }

           _SOC_IF_ERR_EXIT
                (PM4x25_PTP_TIME_ADJUST_SET(unit, pm_info, time_value, lane_index));

            time_value = (uint32)(((int32)(time_value<<3) + ((int32)user_time_value<<3)) >>3) & 0x1FFFFFFF;
            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_adjust_set(unit, port, time_value));
            break;

        default:
            break;
    }
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_encap_presentation_time_config_get (int unit, int port, pm_info_t pm_info,
                                                          portmod_presentation_time_attribute_t attribute,
                                                          portmod_presentation_time_t* time)
{
    uint32 time_value;
    int lane_index;
    uint32 lane_map;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (_cpm4x25_port_index_get(unit, port, pm_info, &lane_index, &lane_map));

    switch (attribute) {
        case PORTMOD_ENCAP_PRESENTATION_TIME_ADJUST:
            _SOC_IF_ERR_EXIT
                (PM4x25_PTP_TIME_ADJUST_GET(unit, pm_info, &time_value, lane_index));

            if (time_value == 0) {
                _SOC_IF_ERR_EXIT
                    (cprimod_encap_presentation_time_adjust_get(unit, port, &time_value));
            } else {
                _SOC_IF_ERR_EXIT
                    (PM4x25_USER_TIME_ADJUST_GET(unit, pm_info, &time_value, lane_index));
            }
            break;

        case PORTMOD_ENCAP_PRESENTATION_APPRIOX_INC:
            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_approximate_increment_get(unit, port, &time_value));
            break;

        case PORTMOD_ENCAP_PRESENTATION_PTP_TIME_ADJUST:

            _SOC_IF_ERR_EXIT
                (PM4x25_PTP_TIME_ADJUST_GET(unit, pm_info, &time_value, lane_index));
            break;
        default:
            break;
    }
    time->time = time_value;
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_decap_presentation_time_config_set (int unit, int port, pm_info_t pm_info,
                                                          const portmod_decap_presentation_time_config_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_presentation_time_config_set(unit, port, config->time_mod_offset, config->mod_count));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_decap_presentation_time_config_get (int unit, int port, pm_info_t pm_info,
                                                          portmod_decap_presentation_time_config_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_presentation_time_config_get(unit, port, &(config->time_mod_offset), &(config->mod_count)));

exit:
    SOC_FUNC_RETURN;
}

#ifdef CPRIMOD_SUPPORT
int cpm4x25_cpri_port_compression_lookup_table_set(int unit, int port, pm_info_t pm_info, int table_depth, const uint32* table)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_compression_lookup_table_set(unit, port, table_depth, table));

exit:
    SOC_FUNC_RETURN;

}

#endif /* CPRIMOD_SUPPORT */
#ifdef CPRIMOD_SUPPORT
int cpm4x25_cpri_port_compression_lookup_table_get(int unit, int port, pm_info_t pm_info, int max_depth, int* table_depth, uint32* table)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_compression_lookup_table_get(unit, port, max_depth, table_depth, table));

exit:
    SOC_FUNC_RETURN;

}

#endif /* CPRIMOD_SUPPORT */

#ifdef CPRIMOD_SUPPORT
int cpm4x25_cpri_port_decompression_lookup_table_set(int unit, int port, pm_info_t pm_info, int table_depth, const uint32* table)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT
        (cprimod_decompression_lookup_table_set(unit, port, table_depth, table));

exit:
    SOC_FUNC_RETURN;

}

#endif /* CPRIMOD_SUPPORT */
#ifdef CPRIMOD_SUPPORT
int cpm4x25_cpri_port_decompression_lookup_table_get(int unit, int port, pm_info_t pm_info, int max_depth, int* table_depth, uint32* table)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decompression_lookup_table_get(unit, port, max_depth, table_depth, table));
exit:
    SOC_FUNC_RETURN;

}

#endif /* CPRIMOD_SUPPORT */
#ifdef CPRIMOD_SUPPORT
int cpm4x25_cpri_port_decap_queue_flow_control_set(int unit, int port, pm_info_t pm_info, uint32 queue_num, int enable, uint32_t xon_threshold, uint32_t xoff_threshold)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_decap_queue_flow_control_set(unit, port,
                                                        queue_num,
                                                        enable,
                                                        xon_threshold,
                                                        xoff_threshold));
exit:
    SOC_FUNC_RETURN;

}

#endif /* CPRIMOD_SUPPORT */
#ifdef CPRIMOD_SUPPORT
int cpm4x25_cpri_port_decap_queue_flow_control_get(int unit, int port, pm_info_t pm_info, uint32 queue_num, int* enable, uint32_t* xon_threshold, uint32_t* xoff_threshold)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_decap_queue_flow_control_get(unit, port,
                                                        queue_num,
                                                        enable,
                                                        xon_threshold,
                                                        xoff_threshold));
exit:
    SOC_FUNC_RETURN;

}

#endif /* CPRIMOD_SUPPORT */

#ifdef CPRIMOD_SUPPORT
int cpm4x25_cpri_port_fec_enable_set(int unit, int port, pm_info_t pm_info, portmod_direction_t dir, int enable)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_port_fec_enable_set(unit, port, dir, enable));
exit:
    SOC_FUNC_RETURN;
}
#endif /* CPRIMOD_SUPPORT */

#ifdef CPRIMOD_SUPPORT
int cpm4x25_cpri_port_fec_enable_get(int unit, int port, pm_info_t pm_info, portmod_direction_t dir, int* enable)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_port_fec_enable_get(unit, port, dir, enable));
exit:
    SOC_FUNC_RETURN;
}
#endif /* CPRIMOD_SUPPORT */

#ifdef CPRIMOD_SUPPORT
int cpm4x25_cpri_port_fec_timestamp_config_set(int unit, int port, pm_info_t pm_info, portmod_direction_t dir, const portmod_fec_timestamp_config_t* config)
{
    cprimod_fec_timestamp_config_t cfg;
    SOC_INIT_FUNC_DEFS;

    cprimod_fec_timestamp_config_t_init(&cfg);
    cfg.enable                    = config->enable;
    cfg.control_bit_offset_0      = config->control_bit_offset_0;
    cfg.control_bit_offset_1      = config->control_bit_offset_1;
    cfg.control_bit_offset_2      = config->control_bit_offset_2;
    cfg.control_bit_offset_3      = config->control_bit_offset_3;
    cfg.fec_fifo_ts_source_select = config->fec_fifo_ts_source_select;
    cfg.fec_cmic_ts_source_select = config->fec_cmic_ts_source_select;
    _SOC_IF_ERR_EXIT(cprimod_cpri_port_fec_timestamp_config_set(unit, port, dir, &cfg));
exit:
    SOC_FUNC_RETURN;
}
#endif /* CPRIMOD_SUPPORT */

#ifdef CPRIMOD_SUPPORT
int cpm4x25_cpri_port_fec_timestamp_config_get(int unit, int port, pm_info_t pm_info, portmod_direction_t dir, portmod_fec_timestamp_config_t* config)
{
    cprimod_fec_timestamp_config_t cfg;
    SOC_INIT_FUNC_DEFS;

    cprimod_fec_timestamp_config_t_init(&cfg);
    _SOC_IF_ERR_EXIT(cprimod_cpri_port_fec_timestamp_config_get(unit, port, dir, &cfg));
    config->enable                    = cfg.enable;
    config->control_bit_offset_0      = cfg.control_bit_offset_0;
    config->control_bit_offset_1      = cfg.control_bit_offset_1;
    config->control_bit_offset_2      = cfg.control_bit_offset_2;
    config->control_bit_offset_3      = cfg.control_bit_offset_3;
    config->fec_fifo_ts_source_select = cfg.fec_fifo_ts_source_select;
    config->fec_cmic_ts_source_select = cfg.fec_cmic_ts_source_select;
exit:
    SOC_FUNC_RETURN;
}
#endif /* CPRIMOD_SUPPORT */

#ifdef CPRIMOD_SUPPORT
int cpm4x25_cpri_port_fec_aux_config_set(int unit, int port, pm_info_t pm_info, portmod_fec_aux_config_para_t parameter_id, uint32 value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_port_fec_aux_config_set(unit, port, parameter_id, value));
exit:
    SOC_FUNC_RETURN;
}
#endif /* CPRIMOD_SUPPORT */

#ifdef CPRIMOD_SUPPORT
int cpm4x25_cpri_port_fec_aux_config_get(int unit, int port, pm_info_t pm_info, portmod_fec_aux_config_para_t parameter_id, uint32* value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_port_fec_aux_config_get(unit, port, parameter_id, value));
exit:
    SOC_FUNC_RETURN;
}
#endif /* CPRIMOD_SUPPORT */

#ifdef CPRIMOD_SUPPORT
int cpm4x25_cpri_port_fec_stat_get(int unit, int port, pm_info_t pm_info, portmod_cpri_fec_stat_type_t stat_type, uint32* value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT 
        (cprimod_cpri_port_fec_stat_get(unit, port, stat_type, value));

exit:
    SOC_FUNC_RETURN;

}
#endif /* CPRIMOD_SUPPORT */

int cpm4x25_port_timesync_enable_set(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    int val;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _SOC_IF_ERR_EXIT(pm4x25_port_timesync_enable_set(unit, port, pm_info, enable));
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("Can't operate timesync on CRPI port %d"), port));
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_timesync_enable_get(int unit, int port, pm_info_t pm_info, uint32* enable)
{
    int val;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _SOC_IF_ERR_EXIT(pm4x25_port_timesync_enable_get(unit, port, pm_info, enable));
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("Can't operate timesync on CRPI port %d"), port));
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_timesync_config_set(int unit, int port, pm_info_t pm_info, const portmod_phy_timesync_config_t* config)
{
    int val;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _SOC_IF_ERR_EXIT(pm4x25_port_timesync_config_set(unit, port, pm_info, config));
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("Can't operate timesync on CRPI port %d"), port));
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_timesync_config_get(int unit, int port, pm_info_t pm_info, portmod_phy_timesync_config_t* config)
{
    int val;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _SOC_IF_ERR_EXIT(pm4x25_port_timesync_config_get(unit, port, pm_info, config));
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("Can't operate timesync on CRPI port %d"), port));
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_control_phy_timesync_set(int unit, int port, pm_info_t pm_info, const portmod_port_control_phy_timesync_t config, uint64 value)
{
    int val;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
       _SOC_IF_ERR_EXIT(pm4x25_port_control_phy_timesync_set(unit, port, pm_info, config, value));
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("Can't operate timesync on CRPI port %d"), port));
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_control_phy_timesync_get(int unit, int port, pm_info_t pm_info, const portmod_port_control_phy_timesync_t config, uint64* value)
{
    int val;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _SOC_IF_ERR_EXIT(pm4x25_port_control_phy_timesync_get(unit, port, pm_info, config, value));
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("Can't operate timesync on CRPI port %d"), port));
    }

exit:
    SOC_FUNC_RETURN;
}


int cpm4x25_cpri_port_ecc_interrupt_enable_set(int unit, int port,
                                                   int enable)
{
    int val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));

    if(val) {
        _SOC_IF_ERR_EXIT(cprimod_cpri_port_ecc_interrupt_enable_set(unit,
                                                            port, enable));
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
              (_SOC_MSG("Can't operate timesync on Non-CRPI port %d"), port));
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_pm_ecc_interrupt_status_get(int unit, int pm_id,
                                             pm_info_t pm_info,
                                             portmod_cpri_ecc_intr_info_t* ecc_err_info)
{
    int lane_index = 0;
    int cpri_port = 0;
    int logical_port = -1;
    cprimod_cpri_ecc_intr_info_t *ecc_info =
                                (cprimod_cpri_ecc_intr_info_t *) ecc_err_info;
    SOC_INIT_FUNC_DEFS;

    /*
     * Look for first RoE port in PM.
     */
    for (lane_index = 0 ; lane_index < MAX_PORTS_PER_PM4X25; lane_index++) {

        _SOC_IF_ERR_EXIT
            (PM4x25_LANE2PORT_GET(unit, pm_info, lane_index, &logical_port));

        /* skip if invalid port */
        if (logical_port < 0) {
            continue;
        }
        /*
         *  Logical port is valid.
         */
        _SOC_IF_ERR_EXIT
            (cprimod_cpri_or_enet_port_get(unit, logical_port, &cpri_port));

        if (cpri_port) {
            break;
        }
    }
    if (cpri_port) {
        _SOC_IF_ERR_EXIT
            (cprimod_cpri_port_ecc_interrupt_status_get(unit, logical_port, ecc_info));
    } else {
        /*
         * No valid RoE port is found.
         */
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, (_SOC_MSG("Can't operate on PM without any RoE port. pm_id  %d"), pm_id));
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_init(int unit, int port, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_cpri_init(unit, port));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_rx_gcw_word_get(int unit, int port, pm_info_t pm_info, int group_index, uint16* gcw_word)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_gcw_word_get(unit, port, group_index, gcw_word));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_gcw_word_set(int unit, int port, pm_info_t pm_info, int group_index, uint16 gcw_word)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_gcw_word_set(unit, port, group_index, gcw_word));

exit:
    SOC_FUNC_RETURN;

}
int cpm4x25_port_rx_mac_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    int roe_port=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_or_enet_port_get(unit, port, &roe_port));
    if (roe_port) {
        /* RoE port */
        _SOC_IF_ERR_EXIT
            (cprimod_cpri_port_rx_enable_set(unit, port, enable));
    } else {
        /* Eth Port */
        _SOC_IF_ERR_EXIT
            (pm4x25_port_rx_mac_enable_set(unit, port, pm_info, enable));
    }
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_rx_mac_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    int roe_port=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_or_enet_port_get(unit, port, &roe_port));
    if (roe_port) {
        /* RoE port */
        _SOC_IF_ERR_EXIT
            (cprimod_cpri_port_rx_enable_get(unit, port, enable));

    } else {
        /* Eth Port */
        _SOC_IF_ERR_EXIT
            (pm4x25_port_rx_mac_enable_get(unit, port, pm_info, enable));
    }
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_tx_mac_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    int roe_port=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_or_enet_port_get(unit, port, &roe_port));
    if (roe_port) {
        /* RoE port */
        _SOC_IF_ERR_EXIT
            (cprimod_cpri_port_tx_enable_set(unit, port, enable));

    } else {
        /* Eth Port */
        _SOC_IF_ERR_EXIT
            (pm4x25_port_tx_mac_enable_set(unit, port, pm_info, enable));
    }
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_tx_mac_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    int roe_port=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_or_enet_port_get(unit, port, &roe_port));
    if (roe_port) {
        /* RoE port */
        _SOC_IF_ERR_EXIT
            (cprimod_cpri_port_tx_enable_get(unit, port, enable));

    } else {
        /* Eth Port */
        _SOC_IF_ERR_EXIT
            (pm4x25_port_tx_mac_enable_get(unit, port, pm_info, enable));
    }
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_drain_cell_stop(int unit, int port, pm_info_t pm_info, const portmod_drain_cells_t* drain_cells)
{
    int roe_port=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_or_enet_port_get(unit, port, &roe_port));
    if (roe_port) {
        /* RoE port */
        _SOC_IF_ERR_EXIT
            (cprimod_cpri_mac_drain_stop(unit, port));

    } else {
        /* Eth Port */
        _SOC_IF_ERR_EXIT
            (pm4x25_port_drain_cell_stop(unit, port, pm_info, drain_cells));
    }
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_drain_cell_start(int unit, int port, pm_info_t pm_info)
{
    int roe_port=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_or_enet_port_get(unit, port, &roe_port));
    if (roe_port) {
        /* RoE port */
        _SOC_IF_ERR_EXIT
            (cprimod_cpri_mac_drain_start(unit, port));
    } else {
        /* Eth Port */
        _SOC_IF_ERR_EXIT
            (pm4x25_port_drain_cell_start(unit, port, pm_info));
    }
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_mac_reset_set(int unit, int port, pm_info_t pm_info, int val)
{
    int roe_port=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_or_enet_port_get(unit, port, &roe_port));
    if (roe_port) {
        /* RoE port */
        _SOC_IF_ERR_EXIT
            (cprimod_cpri_mac_credit_reset_set(unit, port, val));
    } else {
        /* Eth Port */
        _SOC_IF_ERR_EXIT
            (pm4x25_port_mac_reset_set(unit, port, pm_info, val));
    }
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_mac_reset_get(int unit, int port, pm_info_t pm_info, int* val)
{
    int roe_port=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_or_enet_port_get(unit, port, &roe_port));
    if (roe_port) {
        /* RoE port */
        _SOC_IF_ERR_EXIT
            (cprimod_cpri_mac_credit_reset_get(unit, port, val));
    } else {
        /* Eth Port */
        _SOC_IF_ERR_EXIT
            (pm4x25_port_mac_reset_get(unit, port, pm_info, val));
    }
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_mac_reset_check(int unit, int port, pm_info_t pm_info, int enable, int* reset)
{
    int roe_port=0;
    int rx_en=0;
    int tx_en=0;
    int soft_reset=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_or_enet_port_get(unit, port, &roe_port));
    if (roe_port) {
        /* RoE port */
        *reset = 1;
        _SOC_IF_ERR_EXIT
             (cprimod_cpri_port_rx_enable_get(unit, port, &rx_en));
        _SOC_IF_ERR_EXIT
             (cprimod_cpri_port_tx_enable_get(unit, port, &tx_en));

        if ((rx_en == enable) && (tx_en == enable)){
            if (enable) {
                *reset = 0;
            } else {
                _SOC_IF_ERR_EXIT
                    (cprimod_cpri_mac_credit_reset_get(unit, port, &soft_reset));
                if (soft_reset) {
                    *reset = 0;
                }
            }
        }
    } else {
        /* Eth Port */
        _SOC_IF_ERR_EXIT
            (pm4x25_port_mac_reset_check(unit, port, pm_info, enable, reset));
    }


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_tx_down(int unit, int port, pm_info_t pm_info)
{
    int roe_port=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_or_enet_port_get(unit, port, &roe_port));
    if (roe_port) {
        /* RoE port */
        _SOC_IF_ERR_EXIT
                (cprimod_cpri_mac_credit_reset_set(unit, port, 1));
    } else {
        /* Eth Port */
        _SOC_IF_ERR_EXIT
            (pm4x25_port_tx_down(unit, port, pm_info));
    }

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_drain_cells_rx_enable(int unit, int port, pm_info_t pm_info, int rx_en)
{
    int roe_port=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_or_enet_port_get(unit, port, &roe_port));
    if (roe_port) {
        /* RoE port */
        _SOC_IF_ERR_EXIT
             (cprimod_cpri_port_tx_enable_set(unit, port, 1));
        _SOC_IF_ERR_EXIT
             (cprimod_cpri_port_rx_enable_set(unit, port, rx_en));
        /* if rx_enable , remove mac_soft_reset() */
        if (rx_en) {
            _SOC_IF_ERR_EXIT
                (cprimod_cpri_mac_credit_reset_set(unit, port, 0));
        }

    } else {
        /* Eth Port */
        _SOC_IF_ERR_EXIT
            (pm4x25_port_drain_cells_rx_enable(unit, port, pm_info, rx_en));
    }
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_egress_queue_drain_rx_en(int unit, int port, pm_info_t pm_info, int rx_en)
{
    int roe_port=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_or_enet_port_get(unit, port, &roe_port));
    if (roe_port) {
        /* RoE port */
        _SOC_IF_ERR_EXIT
             (cprimod_cpri_port_rx_enable_set(unit, port, rx_en));

        _SOC_IF_ERR_EXIT
             (cprimod_cpri_mac_credit_reset_set(unit, port, 0));

    } else {
        /* Eth Port */
        _SOC_IF_ERR_EXIT
            (pm4x25_port_egress_queue_drain_rx_en(unit, port, pm_info, rx_en));
    }
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_mac_ctrl_set(int unit, int port, pm_info_t pm_info, uint64 ctrl)
{
    int roe_port=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_or_enet_port_get(unit, port, &roe_port));
    if (roe_port) {
        /* RoE port */
        /* do nothing. */
    } else {
        /* Eth Port */
        _SOC_IF_ERR_EXIT
            (pm4x25_port_mac_ctrl_set(unit, port, pm_info, ctrl));
    }

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_egress_queue_drain_get(int unit, int port, pm_info_t pm_info, uint64* ctrl, int* rxen)
{
    int roe_port=0;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_or_enet_port_get(unit, port, &roe_port));
    if (roe_port) {
        /* RoE port */
        _SOC_IF_ERR_EXIT
             (cprimod_cpri_port_rx_enable_get(unit, port, rxen));
    } else {
        /* Eth Port */
        _SOC_IF_ERR_EXIT
            (pm4x25_port_egress_queue_drain_get(unit, port, pm_info, ctrl, rxen));
    }

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_tunnel_mode_config_set(int unit, int port, pm_info_t pm_info, int enable,
                                                const portmod_cpri_rx_tunnel_mode_config_t* config_info)
{
    uint32 queue_num = 0xFF;
    uint32 roe_payload_size;
    cprimod_cpri_container_config_t container_info;
    cprimod_payload_size_info_t roe_payload_info;
    cprimod_encap_decap_data_config_t encap_data_info;
    cprimod_encap_presentation_time_config_t presentation_config;
    int is_10bit_mode;
    cprimod_port_interface_config_t interface_config;
    int lane;
    phymod_phy_access_t phy_access;
    cprimod_port_rsvd4_speed_mult_t rsvd4_speed;
    int restore_rsvd4_mode = FALSE;

    SOC_INIT_FUNC_DEFS;

    roe_payload_size = config_info->payload_size * 8;  /* convert the bytes to bits. */

    _SOC_IF_ERR_EXIT
        (cprimod_port_interface_config_t_init(&interface_config));
    if (!enable) {
        /*
         * retrieve the interface information and restore.
         */

        _SOC_IF_ERR_EXIT
            (cprimod_port_rx_interface_config_get(unit, port, &interface_config));

        restore_rsvd4_mode = (interface_config.port_encap_type == cprimodPortEncapRsvd4)? TRUE:FALSE;
    }

    /*
     *  Setup Port PCS  in Tunnel Mode.
     */
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_tunnel_mode_set(unit, port, enable,
                                              config_info->payload_size,
                                              restore_rsvd4_mode,
                                              interface_config.speed));

    /*
     *  Setup the basic frame and framer. For both enable and disable
     */
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_tunnel_mode_basic_frame_config_set(unit, port, CPRIMOD_DIR_RX, enable, restore_rsvd4_mode));

    if (enable) {
        /* Enable  */
        _SOC_IF_ERR_EXIT
            (cprimod_cpri_interface_bus_width_get(unit, port, &is_10bit_mode));

        if (SOC_E_NONE != cprimod_cpri_container_config_t_init(&container_info)) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config_info initialization failed"));
        }

        container_info.map_method           = cprimod_cpri_frame_map_method_1;
        container_info.Naxc                 = is_10bit_mode ? 10:40;
        container_info.K                    = 1;
        container_info.Nst                  = 0;
        container_info.Na                   = 0;
        container_info.Nc                   = 0;
        container_info.Nv                   = 0;
        container_info.frame_sync_mode      = PORTMOD_CPRI_FRAME_SYNC_MODE_TUNNEL;
        container_info.basic_frame_offset   = 0;
        container_info.radio_frame_offset   = 0;
        container_info.hyper_frame_offset   = 0;

        _SOC_IF_ERR_EXIT
            (cprimod_cpri_rx_axc_container_config_set(unit, port, CPRIMOD_AXC_ID_AGNOSTIC , &container_info));


        roe_payload_info.packet_size        = roe_payload_size;
        roe_payload_info.last_packet_num    = 0 ; /* not used. */
        roe_payload_info.last_packet_size   = roe_payload_size ; /* not used. */

        _SOC_IF_ERR_EXIT
            (cprimod_rx_roe_payload_size_set(unit, port, CPRIMOD_AXC_ID_AGNOSTIC, &roe_payload_info));

        queue_num                           = 0;  /* queue # is always 0 for agnostic. */
        encap_data_info.sample_size         = 8;
        encap_data_info.truncation_enable   = 0;
        encap_data_info.truncation_type     = 0; /* Not used */
        encap_data_info.compression_type    = cprimod_no_compress;
        encap_data_info.mux_enable          = 0;
        encap_data_info.bit_reversal        = 0;
        encap_data_info.buffer_size         = (roe_payload_size/8) * 2 ;
        encap_data_info.priority            = 0;
        encap_data_info.cycle_size          = 0;
        encap_data_info.is_agnostic         = 1;

        _SOC_IF_ERR_EXIT
            (cprimod_cpri_encap_data_config_set(unit, port, queue_num, &encap_data_info ));


            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_config_t_init(&presentation_config));

            presentation_config.mod_count           = config_info->pres_mod_count;
            presentation_config.frame_sync_mode     = PORTMOD_CPRI_FRAME_SYNC_MODE_TUNNEL;
            presentation_config.radio_frame_offset  = 0;
            presentation_config.hyper_frame_offset  = 0;
            presentation_config.approx_inc_disable  = TRUE;

            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_config_set(unit, port, &presentation_config));

        if (config_info->ordering_info_option == PORTMOD_CPRI_ROE_ORDERING_INFO_OPTION_TIMESTAMP) {
            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_enable_set(unit, port, TRUE));
        } else {
            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_enable_set(unit, port, FALSE));
        }

        _SOC_IF_ERR_EXIT
            (cprimod_basic_frame_parser_switch_active_table(unit, port));

    } else { /* Disable */
        /* Disable need to turn off presentation time. */
        _SOC_IF_ERR_EXIT
            (cprimod_encap_presentation_time_enable_set(unit, port, FALSE));

        sal_memcpy(&phy_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_core_access_t));
        lane = (SOC_INFO(unit).port_l2p_mapping[port]-1)%CPRIMOD_MAX_LANES_PER_CORE;
        phy_access.access.lane_mask = (1<<lane);

        if(interface_config.port_encap_type == cprimodPortEncapCpri) {
            interface_config.interface = cprimodCpri;
            _SOC_IF_ERR_EXIT
                (cprimod_port_rx_interface_config_set(unit, port, &phy_access, &interface_config));

            _SOC_IF_ERR_EXIT
                (cpm4x25_cpri_port_stage_activate(unit, port, pm_info, portmodCpriStageRx));

        } else if(interface_config.port_encap_type == cprimodPortEncapRsvd4) {
            interface_config.interface = cprimodRsvd4;

            if(interface_config.speed == cprimodSpd3072p0){
                rsvd4_speed = cprimodRsvd4SpdMult4X;
            } else if (interface_config.speed == cprimodSpd6144p0) {
                rsvd4_speed = cprimodRsvd4SpdMult8X;
            } else {
                _SOC_EXIT_WITH_ERR
                    (SOC_E_PARAM, ("RSVD4 Unsupported Speed Detected."));
            }
            _SOC_IF_ERR_EXIT
                (cprimod_port_rsvd4_rx_speed_set(unit, port, &phy_access, rsvd4_speed));

            _SOC_IF_ERR_EXIT
                (cprimod_basic_frame_parser_switch_active_table(unit, port));

        }
    }

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_tunnel_mode_config_get(int unit, int port, pm_info_t pm_info, int* enable, portmod_cpri_rx_tunnel_mode_config_t* config_info)
{
    int pcs_tunnel_mode_enable = 0;
    uint32 roe_payload_size;
    int presentation_enable =0;
    cprimod_encap_presentation_time_config_t presentation_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_tunnel_mode_get(unit, port, &pcs_tunnel_mode_enable, &roe_payload_size));

    if (pcs_tunnel_mode_enable) {

        /*
         * if enabled, get the rest of the information.
         */
        *enable = TRUE;
        config_info->payload_size = roe_payload_size;

        /* get presenation time configuration. */

        _SOC_IF_ERR_EXIT
            (cprimod_encap_presentation_time_enable_get(unit, port, &presentation_enable));

        _SOC_IF_ERR_EXIT
            (cprimod_encap_presentation_time_config_t_init(&presentation_config));

        _SOC_IF_ERR_EXIT
            (cprimod_encap_presentation_time_config_get(unit, port, &presentation_config));

        config_info->pres_mod_count         = presentation_config.mod_count;

        if (presentation_enable) {
            config_info->ordering_info_option   = PORTMOD_CPRI_ROE_ORDERING_INFO_OPTION_TIMESTAMP;
        } else {
            config_info->ordering_info_option = PORTMOD_CPRI_ROE_ORDERING_INFO_OPTION_SEQ_NUM;
        }
    } else { /* DISABLED */
        *enable = FALSE;
        config_info->ordering_info_option = 0;
    }
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_tunnel_mode_config_set(int unit, int port, pm_info_t pm_info, int enable, const portmod_cpri_tx_tunnel_mode_config_t* config_info)
{
    uint32 queue_num = 0xFF;
    uint32 roe_payload_size;
    cprimod_cpri_container_config_t container_info;
    cprimod_payload_size_info_t roe_payload_info;
    cprimod_encap_decap_data_config_t decap_data_info;
    cprimod_decap_agnostic_mode_config_t agnostic_config;
    uint32 divider=1;
    uint32 frame_tick=0;
    uint32 tick_bitmap=0;
    uint32 tick_bitmap_size=0;
    cprimod_port_interface_config_t interface_config;
    cprimod_port_rsvd4_speed_mult_t rsvd4_speed;
    int restore_rsvd4_mode = FALSE;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_port_tx_interface_config_get(unit, port, &interface_config));

    restore_rsvd4_mode = (interface_config.port_encap_type == cprimodPortEncapRsvd4)? TRUE:FALSE;

    roe_payload_size = config_info->payload_size * 8;

    /*
     *  Setup Port PCS  in Tunnel Mode.
     */

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_agnostic_mode_set(unit, port, enable,
                                                cprimod_agnostic_mode_tunnel,
                                                config_info->payload_size,
                                                restore_rsvd4_mode,
                                                interface_config.speed));

    /*
     *  Program Basic Frame Assembler.
     */

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_tunnel_mode_basic_frame_config_set(unit, port, CPRIMOD_DIR_TX, enable, restore_rsvd4_mode));

    if (enable ) {
        if (SOC_E_NONE != cprimod_cpri_container_config_t_init(&container_info)) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config_info initialization failed"));
        }

        container_info.map_method           = cprimod_cpri_frame_map_method_1;
        container_info.Naxc                 = 40; /* hard coded to 40 bits. */
        container_info.K                    = 1;
        container_info.Nst                  = 0;
        container_info.Na                   = 0;
        container_info.Nc                   = 0;
        container_info.Nv                   = 0;
        container_info.frame_sync_mode      = PORTMOD_CPRI_FRAME_SYNC_MODE_TUNNEL;
        container_info.basic_frame_offset   = 0;
        container_info.radio_frame_offset   = 0;
        container_info.hyper_frame_offset   = 0;

        _SOC_IF_ERR_EXIT
            (cprimod_cpri_tx_axc_container_config_set(unit, port, 0, &container_info));

        roe_payload_info.packet_size        = roe_payload_size;
        roe_payload_info.last_packet_num    = 0 ; /* not used. */
        roe_payload_info.last_packet_size   = roe_payload_size ; /* not used. */

        _SOC_IF_ERR_EXIT
            (cprimod_tx_roe_payload_size_set(unit, port, 0, &roe_payload_info));

        queue_num                           = CPRIMOD_AXC_ID_AGNOSTIC;  /* queue # is always 0 for agnostic. */
        decap_data_info.sample_size         = 8;
        decap_data_info.truncation_enable   = 0;
        decap_data_info.truncation_type     = 0; /* Not used */
        decap_data_info.compression_type    = cprimod_no_compress;
        decap_data_info.mux_enable          = 0;
        decap_data_info.bit_reversal        = 0;
        decap_data_info.buffer_size         = roe_payload_size/8 ;
        decap_data_info.priority            = 0;
        /*
         * This cycle size  not used in agnostic,
         * it is configured in cprimod_decap_agnostic_mode_config_set
         * instead.
         */
        decap_data_info.cycle_size          = 0;
        decap_data_info.is_agnostic         = 1;

        _SOC_IF_ERR_EXIT
            (cprimod_cpri_decap_data_config_set(unit, port, queue_num, &decap_data_info ));

        _SOC_IF_ERR_EXIT
            (cprimod_decap_agnostic_mode_config_t_init(&agnostic_config));

        agnostic_config.mod_count       = config_info->queue_mod_count; /* cycle size */
        agnostic_config.packet_size     = roe_payload_size/8 ;
        agnostic_config.queue_num       = queue_num;

        _SOC_IF_ERR_EXIT
            (cprimod_decap_agnostic_mode_config_set(unit, port, &agnostic_config));


        _SOC_IF_ERR_EXIT
            (cprimod_decap_presentation_time_config_set(unit, port, config_info->pres_mod_offset, config_info->pres_mod_count));

        /*
         * Tunnel Mode Bit Block Event Generation.
         * This will set Assembly FIFO for CDC at 48 entries.
         * Divider is 4 for 3G & 2.4, 8 for 1.2G  and 
         * 1 for all other speed.
         */
        if ((interface_config.speed == cprimodSpd3072p0) ||
            (interface_config.speed == cprimodSpd2457p6)) {
            divider     = 4;
        } else if (interface_config.speed == cprimodSpd1228p8){
            divider     = 8;
        } else {
            divider     = 1;
        }

        frame_tick          = 48; /* CPRIF_TUNNEL_MODE_NUM_OF_BFA_ENTRIES */
        tick_bitmap         = 0;
        tick_bitmap_size    = 0;

        _SOC_IF_ERR_EXIT
            (cprimod_tx_framer_tgen_tick_set(unit, port, divider, frame_tick, tick_bitmap, tick_bitmap_size));

        _SOC_IF_ERR_EXIT
            (cprimod_basic_frame_assembly_switch_active_table(unit, port));

    } else {
       /*
         * retrieve the interface information and restore.
         */

        if(interface_config.port_encap_type == cprimodPortEncapCpri) {

            _SOC_IF_ERR_EXIT
                (cprimod_cpri_tx_framer_tgen_tick_config_set(unit, port, interface_config.speed));

            _SOC_IF_ERR_EXIT
                (cpm4x25_cpri_port_stage_activate(unit, port, pm_info, portmodCpriStageTx));

        } else if(interface_config.port_encap_type == cprimodPortEncapRsvd4) {

            if (interface_config.speed == cprimodSpd3072p0){
                rsvd4_speed = cprimodRsvd4SpdMult4X;
            } else if (interface_config.speed == cprimodSpd6144p0) {
                rsvd4_speed = cprimodRsvd4SpdMult8X;
            } else {
                _SOC_EXIT_WITH_ERR
                    (SOC_E_PARAM, ("RSVD4 Unsupported Speed Detected."));
            }

            _SOC_IF_ERR_EXIT
                (cprimod_rsvd4_tx_framer_tgen_tick_config_set(unit, port, rsvd4_speed));

            _SOC_IF_ERR_EXIT
                (cprimod_basic_frame_assembly_switch_active_table(unit, port));
        }
    }

    /*
     * enable/disable as well as setting the timestamp or seq_num.
     */

    _SOC_IF_ERR_EXIT
         (cprimod_decap_agnostic_mode_enable_set(unit, port, enable, config_info->ordering_info_option));

    /*
     * Activate the basic frame
     */


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_tunnel_mode_config_get(int unit, int port, pm_info_t pm_info, int* enable, portmod_cpri_tx_tunnel_mode_config_t* config_info)
{
    uint32 roe_payload_size;
    int agnostic_enable=FALSE;
    portmod_cpri_roe_ordering_info_option_t mode;
    cprimod_agnostic_mode_type_t agnostic_mode;
    cprimod_decap_agnostic_mode_config_t agnostic_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_agnostic_mode_get(unit, port, enable, &agnostic_mode,  &roe_payload_size));

    if ( *enable && ( agnostic_mode == cprimod_agnostic_mode_tunnel)) {
        *enable = TRUE;
    } else {
        *enable = FALSE;
    }

    config_info->payload_size = roe_payload_size;

    if (*enable) {
        _SOC_IF_ERR_EXIT
            (cprimod_decap_agnostic_mode_enable_get(unit, port, &agnostic_enable, &mode));

        config_info->ordering_info_option = mode;

        _SOC_IF_ERR_EXIT
            (cprimod_decap_agnostic_mode_config_get(unit, port, &agnostic_config));

        config_info->queue_mod_count = agnostic_config.mod_count ;

        _SOC_IF_ERR_EXIT
            (cprimod_decap_presentation_time_config_get(unit, port, &config_info->pres_mod_offset, &config_info->pres_mod_count));
    }
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_port_rx_agnostic_config_set(int unit, int port, pm_info_t pm_info, int enable, const portmod_rsvd4_rx_agnostic_config_t* config_info)
{
    uint32 queue_num = 0xFF;
    uint32 roe_payload_size;
    cprimod_rsvd4_container_config_t container_info;
    cprimod_payload_size_info_t roe_payload_info;
    cprimod_encap_decap_data_config_t encap_data_info;
    cprimod_encap_presentation_time_config_t presentation_config;
    cprimod_port_interface_config_t interface_config;
    int lane;
    cprimod_port_rsvd4_speed_mult_t rsvd4_speed;
    phymod_phy_access_t phy_access;

    SOC_INIT_FUNC_DEFS;

    /*
     *  Check to see if the port is in RSVD4 mode.
     *  If not, return error.
     */
    _SOC_IF_ERR_EXIT
        (cprimod_port_rx_interface_config_get(unit, port, &interface_config));

    if(interface_config.interface != cprimodRsvd4) {
        _SOC_EXIT_WITH_ERR
                    (SOC_E_PARAM, ("Port is not in RSVD4 Mode.\n"));
    }

    /*
     * convert msg group multiple to bits.
     * Each Msg group has 19 bytes per msg * 21 msgs N_MG + 1 K)*8
     */
    roe_payload_size = config_info->payload_multiple * 400 *8 ;

    /*
     *  Setup the basic frame. For both enable and disable
     */
    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_agnostic_mode_basic_frame_config_set(unit, port, CPRIMOD_DIR_RX, enable));

    if (enable) {
        /* Enable  */

        cprimod_rsvd4_container_config_t_init(&container_info);
        container_info.axc_id                   = CPRIMOD_AXC_ID_AGNOSTIC;
        container_info.stuffing_cnt             = 0;
        container_info.msg_ts_mode              = CPRIMOD_RSVD4_MSG_TS_MODE_WCDMA; /* No timestamp */
        container_info.msg_ts_cnt               = 0; /* No timestamp */
        container_info.frame_sync_mode          = config_info->frame_sync_mode;
        container_info.master_frame_offset      = config_info->master_frame_offset;
        container_info.message_number_offset    = config_info->message_group_number;
        container_info.container_block_cnt      = 3200; /* 400 Bytes * 8 bits , MG size */

        _SOC_IF_ERR_EXIT
            (cprimod_rsvd4_rx_axc_container_config_set(unit, port, CPRIMOD_AXC_ID_AGNOSTIC, &container_info));

        roe_payload_info.packet_size        = roe_payload_size;
        roe_payload_info.last_packet_num    = 0 ; /* not used. */
        roe_payload_info.last_packet_size   = roe_payload_size ; /* not used. */

        _SOC_IF_ERR_EXIT
            (cprimod_rx_roe_payload_size_set(unit, port, CPRIMOD_AXC_ID_AGNOSTIC, &roe_payload_info));

        queue_num                           = 0;  /* queue # is always 0 for agnostic. */
        encap_data_info.sample_size         = 8;
        encap_data_info.truncation_enable   = 0;
        encap_data_info.truncation_type     = 0; /* Not used */
        encap_data_info.compression_type    = cprimod_no_compress;
        encap_data_info.mux_enable          = 0;
        encap_data_info.bit_reversal        = TRUE;
        encap_data_info.buffer_size         = (roe_payload_size/8) * 2 ;
        encap_data_info.priority            = 0;
        encap_data_info.cycle_size          = 0;
        encap_data_info.is_agnostic         = 1;

        _SOC_IF_ERR_EXIT
            (cprimod_cpri_encap_data_config_set(unit, port, queue_num, &encap_data_info ));


            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_config_t_init(&presentation_config));

            if ((config_info->frame_sync_mode == CPRIMOD_RSVD4_FRAME_SYNC_MODE_NO_SYNC)||
                (config_info->frame_sync_mode == CPRIMOD_RSVD4_FRAME_SYNC_MODE_TS_NO_SYNC)) {

                presentation_config.frame_sync_mode = CPRIMOD_CPRI_FRAME_SYNC_MODE_HYPER;

            } else if ((config_info->frame_sync_mode == CPRIMOD_RSVD4_FRAME_SYNC_MODE_MSG_OFFSET)||
                       (config_info->frame_sync_mode == CPRIMOD_RSVD4_FRAME_SYNC_MODE_TS_MSG_OFFSET)) {

                presentation_config.frame_sync_mode = CPRIMOD_CPRI_FRAME_SYNC_MODE_RADIO;

            } else if ((config_info->frame_sync_mode == CPRIMOD_RSVD4_FRAME_SYNC_MODE_MSG_AND_MASTER_OFFSET)||
                       (config_info->frame_sync_mode == CPRIMOD_RSVD4_FRAME_SYNC_MODE_TS_MSG_AND_MASTER_OFFSET)) {

                presentation_config.frame_sync_mode = CPRIMOD_CPRI_FRAME_SYNC_MODE_BASIC;

            }

            presentation_config.mod_count           = config_info->pres_mod_count;
            presentation_config.radio_frame_offset  = config_info->master_frame_offset;
            presentation_config.hyper_frame_offset  = config_info->message_group_number;
            presentation_config.approx_inc_disable  = TRUE;

            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_config_set(unit, port, &presentation_config));

        if (config_info->ordering_info_option == PORTMOD_CPRI_ROE_ORDERING_INFO_OPTION_TIMESTAMP) {
            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_enable_set(unit, port, TRUE));
        } else {
            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_enable_set(unit, port, FALSE));
        }
    } else { /* Disable */
        /* Disable need to turn off presentation time. */
        _SOC_IF_ERR_EXIT
            (cprimod_encap_presentation_time_enable_set(unit, port, FALSE));

        if(interface_config.speed == cprimodSpd3072p0){
            rsvd4_speed = cprimodRsvd4SpdMult4X;
        } else if (interface_config.speed == cprimodSpd6144p0) {
            rsvd4_speed = cprimodRsvd4SpdMult8X;
        } else {
            _SOC_EXIT_WITH_ERR
                (SOC_E_PARAM, ("RSVD4 Unsupported Speed Detected."));
        }
        sal_memcpy(&phy_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_core_access_t));
        lane = (SOC_INFO(unit).port_l2p_mapping[port]-1)%CPRIMOD_MAX_LANES_PER_CORE;
        phy_access.access.lane_mask = (1<<lane);

        _SOC_IF_ERR_EXIT
            (cprimod_port_rsvd4_rx_speed_set(unit, port, &phy_access, rsvd4_speed));

    }
    /* 
     * Fwd Ildes to framer for rsvd4 agnostic mode.
     */
    _SOC_IF_ERR_EXIT
        (cprimod_port_rsvd4_rx_frame_optional_config_set(unit, port,
                                                         CPRIMOD_RSVD4_RX_CONFIG_FWD_IDLE_TO_FRAMER, 
                                                         enable));
    /*
     *  Swtich  the basic frame
     */
    _SOC_IF_ERR_EXIT
        (cprimod_basic_frame_parser_switch_active_table(unit, port));
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_rsvd4_port_rx_agnostic_config_get(int unit, int port, pm_info_t pm_info, int* enable, portmod_rsvd4_rx_agnostic_config_t* config_info)
{
    cprimod_agnostic_mode_type_t mode;
    cprimod_payload_size_info_t roe_payload_info;
    int presentation_enable =0;
    cprimod_encap_presentation_time_config_t presentation_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_agnostic_mode_get(unit, port, enable, &mode));

    if (*enable && (mode == cprimod_agnostic_mode_rsvd4)) {
        /*
         * if enabled in rsvd4 mode, get the rest of the information.
         */

        _SOC_IF_ERR_EXIT
            (cprimod_rx_roe_payload_size_get(unit, port, CPRIMOD_AXC_ID_AGNOSTIC, &roe_payload_info));

        /*
         * convert packet size in bits to mutliple of message group.
         * 400 bytes per msg grp
         */
        config_info->payload_multiple = roe_payload_info.packet_size/3200;

        /* get presenation time configuration. */

        _SOC_IF_ERR_EXIT
            (cprimod_encap_presentation_time_enable_get(unit, port, &presentation_enable));

            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_config_t_init(&presentation_config));

            _SOC_IF_ERR_EXIT
                (cprimod_encap_presentation_time_config_get(unit, port, &presentation_config));

            config_info->pres_mod_count         = presentation_config.mod_count;
            config_info->master_frame_offset    = presentation_config.radio_frame_offset;
            config_info->message_group_number   = presentation_config.hyper_frame_offset;

            if (presentation_config.frame_sync_mode == CPRIMOD_CPRI_FRAME_SYNC_MODE_HYPER) {
                config_info->frame_sync_mode = CPRIMOD_RSVD4_FRAME_SYNC_MODE_NO_SYNC;
            } else if (presentation_config.frame_sync_mode == CPRIMOD_CPRI_FRAME_SYNC_MODE_RADIO) {
                config_info->frame_sync_mode = CPRIMOD_RSVD4_FRAME_SYNC_MODE_MSG_OFFSET;
            } else if (presentation_config.frame_sync_mode == CPRIMOD_CPRI_FRAME_SYNC_MODE_BASIC) {
                config_info->frame_sync_mode = CPRIMOD_RSVD4_FRAME_SYNC_MODE_MSG_AND_MASTER_OFFSET;
            }

        if (presentation_enable) {
            config_info->ordering_info_option = PORTMOD_CPRI_ROE_ORDERING_INFO_OPTION_TIMESTAMP;

        } else {
            config_info->ordering_info_option = PORTMOD_CPRI_ROE_ORDERING_INFO_OPTION_SEQ_NUM;
        }
    } else { /* DISABLED */
        *enable = FALSE;
        config_info->ordering_info_option = 0;
    }
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_rsvd4_port_tx_agnostic_config_set(int unit, int port, pm_info_t pm_info, int enable, const portmod_rsvd4_tx_agnostic_config_t* config_info)
{
    uint32 queue_num = 0xFF;
    uint32 roe_payload_size;
    cprimod_rsvd4_container_config_t container_info;
    cprimod_payload_size_info_t roe_payload_info;
    cprimod_encap_decap_data_config_t decap_data_info;
    cprimod_decap_agnostic_mode_config_t agnostic_config;
    cprimod_port_interface_config_t interface_config;
    int lane;
    cprimod_port_rsvd4_speed_mult_t rsvd4_speed;
    phymod_phy_access_t phy_access;
    SOC_INIT_FUNC_DEFS;

   /*
     *  Check to see if the port is in RSVD4 mode.
     *  If not, return error.
     */
    _SOC_IF_ERR_EXIT
        (cprimod_port_tx_interface_config_get(unit, port, &interface_config));

    if(interface_config.interface != cprimodRsvd4) {
        _SOC_EXIT_WITH_ERR
                    (SOC_E_PARAM, ("Port is not in RSVD4 Mode.\n"));
    }

    roe_payload_size = config_info->payload_multiple * 400 *8 ;

    /*
     *  Setup Port PCS  in Agnostic Mode.
     */

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_agnostic_mode_set(unit, port, enable,
                                                cprimod_agnostic_mode_rsvd4,
                                                roe_payload_size,
                                                FALSE,
                                                interface_config.speed));
    /*
     *  Program Basic Frame Assembler.
     */

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_agnostic_mode_basic_frame_config_set(unit, port, CPRIMOD_DIR_TX, enable));

    if (enable) {

        cprimod_rsvd4_container_config_t_init(&container_info);
        container_info.axc_id                   = CPRIMOD_AXC_ID_AGNOSTIC;
        container_info.stuffing_cnt             = 0;
        container_info.msg_ts_mode              = CPRIMOD_RSVD4_MSG_TS_MODE_WCDMA; /* No timestamp */
        container_info.msg_ts_cnt               = 0; /* No timestamp */
        container_info.frame_sync_mode          = config_info->frame_sync_mode;
        container_info.master_frame_offset      = config_info->master_frame_offset;
        container_info.message_number_offset    = config_info->message_group_number;
        container_info.container_block_cnt      = 3200; /* 400 Bytes * 8 bits , MG size */
        container_info.use_ts_dbm               = FALSE;
        container_info.msg_addr                 = 0;
        container_info.msg_type                 = 0;
        container_info.msg_ts_offset            = 0;

        _SOC_IF_ERR_EXIT
            (cprimod_rsvd4_tx_axc_container_config_set(unit, port, CPRIMOD_AXC_ID_AGNOSTIC, &container_info));

        roe_payload_info.packet_size        = roe_payload_size;
        roe_payload_info.last_packet_num    = 0 ; /* not used. */
        roe_payload_info.last_packet_size   = roe_payload_size ; /* not used. */

        _SOC_IF_ERR_EXIT
            (cprimod_tx_roe_payload_size_set(unit, port, 0, &roe_payload_info));

        queue_num                           = CPRIMOD_AXC_ID_AGNOSTIC;  /* queue # is always 0 for agnostic. */
        decap_data_info.sample_size         = 8;
        decap_data_info.truncation_enable   = 0;
        decap_data_info.truncation_type     = 0; /* Not used */
        decap_data_info.compression_type    = cprimod_no_compress;
        decap_data_info.mux_enable          = 0;
        decap_data_info.bit_reversal        = 0;
        decap_data_info.buffer_size         = roe_payload_size/8 ;
        decap_data_info.priority            = 0;
        /*
         * This cycle size  not used in agnostic,
         * it is configured in cprimod_decap_agnostic_mode_config_set
         * instead.
         */
        decap_data_info.cycle_size          = 0;
        decap_data_info.is_agnostic         = 1;

        _SOC_IF_ERR_EXIT
            (cprimod_cpri_decap_data_config_set(unit, port, queue_num, &decap_data_info));

        _SOC_IF_ERR_EXIT
            (cprimod_decap_agnostic_mode_config_t_init(&agnostic_config));

        agnostic_config.mod_count       = config_info->queue_mod_count; /* cycle size */
        agnostic_config.packet_size     = roe_payload_size/8 ;
        agnostic_config.queue_num       = queue_num;

        _SOC_IF_ERR_EXIT
            (cprimod_decap_agnostic_mode_config_set(unit, port, &agnostic_config));

        _SOC_IF_ERR_EXIT
            (cprimod_decap_presentation_time_config_set(unit, port, config_info->pres_mod_offset, config_info->pres_mod_count));

        /*
         * set padding size to 0.
         */
        _SOC_IF_ERR_EXIT
            (cprimod_rsvd4_tx_padding_size_set(unit, port, 0));

    } else {

        if(interface_config.speed == cprimodSpd3072p0){
            rsvd4_speed = cprimodRsvd4SpdMult4X;
        } else if (interface_config.speed == cprimodSpd6144p0) {
            rsvd4_speed = cprimodRsvd4SpdMult8X;
        } else {
            _SOC_EXIT_WITH_ERR
                (SOC_E_PARAM, ("RSVD4 Unsupported Speed Detected."));
        }
        sal_memcpy(&phy_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_core_access_t));
        lane = (SOC_INFO(unit).port_l2p_mapping[port]-1)%CPRIMOD_MAX_LANES_PER_CORE;
        phy_access.access.lane_mask = (1<<lane);

        _SOC_IF_ERR_EXIT
            (cprimod_port_rsvd4_tx_speed_set(unit, port, &phy_access, rsvd4_speed));
    }

    /*
     * enable/disable as well as setting the timestamp or seq_num.
     */

    _SOC_IF_ERR_EXIT
         (cprimod_decap_agnostic_mode_enable_set(unit, port, enable, config_info->ordering_info_option));

    /*
     *  switch the basic frame active table.
     */
    _SOC_IF_ERR_EXIT
         (cprimod_basic_frame_assembly_switch_active_table(unit, port));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_port_tx_agnostic_config_get(int unit, int port, pm_info_t pm_info, int* enable, portmod_rsvd4_tx_agnostic_config_t* config_info)
{
    cprimod_agnostic_mode_type_t mode;
    uint32 tunnel_payload=0;
    cprimod_payload_size_info_t roe_payload_info;
    cprimod_rsvd4_container_config_t container_info;
    int presentation_enable =0;
    cprimod_decap_agnostic_mode_config_t agnostic_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_agnostic_mode_get(unit, port, enable, &mode, &tunnel_payload));

    if (*enable && (mode == cprimod_agnostic_mode_rsvd4)) {
        /*
         * if enabled in rsvd4 mode, get the rest of the information.
         */

        _SOC_IF_ERR_EXIT
            (cprimod_tx_roe_payload_size_get(unit, port, CPRIMOD_AXC_ID_AGNOSTIC, &roe_payload_info));

        /*
         * convert packet size in bits to mutliple of message group.
         * 400 bytes per msg grp
         */
        config_info->payload_multiple = roe_payload_info.packet_size/3200;

        _SOC_IF_ERR_EXIT
            (cprimod_rsvd4_tx_axc_container_config_get(unit, port, CPRIMOD_AXC_ID_AGNOSTIC, &container_info));

        config_info->frame_sync_mode        = container_info.frame_sync_mode;
        config_info->master_frame_offset    = container_info.master_frame_offset;
        config_info->message_group_number   = container_info.message_number_offset;

        _SOC_IF_ERR_EXIT
            (cprimod_decap_agnostic_mode_config_get(unit, port, &agnostic_config));
        config_info->queue_mod_count = agnostic_config.mod_count;


        /* get presenation time configuration. */

        _SOC_IF_ERR_EXIT
            (cprimod_decap_agnostic_mode_enable_get(unit, port, &presentation_enable, &config_info->ordering_info_option));

        _SOC_IF_ERR_EXIT
            (cprimod_decap_presentation_time_config_get(unit, port, &config_info->pres_mod_offset, &config_info->pres_mod_count));

    } else { /* DISABLED */
        *enable = FALSE;
        config_info->ordering_info_option = 0;
    }
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_frame_config_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_port_frame_config_t* config)
{
    cprimod_cpri_encap_header_field_mapping_t encap_map;
    cprimod_cpri_decap_header_field_mapping_t decap_map;
    int len_field_adjust = 0;
    int ext_hdr_enable=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_header_field_mapping_t_init(&encap_map));

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_header_field_mapping_t_init(&decap_map));

    switch (config->frame_type) {

        case PORTMOD_CPRI_ROE_FRAME_FORMAT_IEEE1914:
        case PORTMOD_CPRI_ROE_FRAME_FORMAT_RSVD3:

            /* default mapping */
            len_field_adjust    = 0;
            ext_hdr_enable      = FALSE;
            break;

        case PORTMOD_CPRI_ROE_FRAME_FORMAT_ECPRI8:

            /* Encap mapping */
            encap_map.hdr_byte0_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_31_24;
            encap_map.hdr_byte1_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_OPCODE;
            encap_map.hdr_byte2_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_LENGTH_15_8;
            encap_map.hdr_byte3_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_LENGTH_7_0;
            encap_map.hdr_byte4_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_SUBTYPE;
            encap_map.hdr_byte5_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_FLOW_ID;
            encap_map.hdr_byte6_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_15_8;
            encap_map.hdr_byte7_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_7_0;

            /* Decap Mapping */
            decap_map.subtype_sel             = config->decap_primary_key;
            decap_map.flowid_sel              = config->decap_secondary_key;
            decap_map.length_15_8_sel         = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_2;
            decap_map.length_7_0_sel          = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_3;
            decap_map.orderinginfo_31_24_sel  = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_4; /* will masked off*/
            decap_map.orderinginfo_23_16_sel  = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_5; /* will nasked off*/
            decap_map.orderinginfo_15_8_sel   = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_6;
            decap_map.orderinginfo_7_0_sel    = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_7;

            _SOC_IF_ERR_EXIT
                (cprimod_cpri_decap_header_field_mask_set( unit, port,
                                                           CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_31_24,
                                                           0x00));
            _SOC_IF_ERR_EXIT
                (cprimod_cpri_decap_header_field_mask_set( unit, port,
                                                           CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_23_16,
                                                           0x00));

            /*
             * RoE length is IQ data only, does not include the common header.
             * eCPRI length include PC_ID and SEQ_ID. Add Length adjust on
             * encap side and subtract on decap side.
             */
            len_field_adjust    = 4;
            ext_hdr_enable      = FALSE;
            break;

        case PORTMOD_CPRI_ROE_FRAME_FORMAT_ECPRI12:

            /* Encap mapping */
            encap_map.hdr_byte0_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_SUBTYPE;
            encap_map.hdr_byte1_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_OPCODE;
            encap_map.hdr_byte2_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_LENGTH_15_8;
            encap_map.hdr_byte3_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_LENGTH_7_0;
            encap_map.hdr_byte4_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_UNMAPPED;
            encap_map.hdr_byte5_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_UNMAPPED;
            encap_map.hdr_byte6_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_UNMAPPED;
            encap_map.hdr_byte7_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_FLOW_ID;
            encap_map.hdr_byte8_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_31_24;
            encap_map.hdr_byte9_sel  = CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_23_16;
            encap_map.hdr_byte10_sel = CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_15_8;
            encap_map.hdr_byte11_sel = CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_7_0;

            /* Decap Mapping */
            decap_map.subtype_sel             = config->decap_primary_key;
            decap_map.flowid_sel              = config->decap_secondary_key;
            decap_map.length_15_8_sel         = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_2;
            decap_map.length_7_0_sel          = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_3;
            decap_map.orderinginfo_31_24_sel  = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_8;
            decap_map.orderinginfo_23_16_sel  = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_9;
            decap_map.orderinginfo_15_8_sel   = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_10;
            decap_map.orderinginfo_7_0_sel    = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_11;

            /*
             * RoE length is IQ data only, does not include the common header.
             * eCPRI length include PC_ID and SEQ_ID. Add Length adjust on
             * encap side and subtract on decap side.
             */
            len_field_adjust    = 8;
            ext_hdr_enable      = TRUE;
            break;

            default:
           _SOC_EXIT_WITH_ERR
                (SOC_E_PARAM, ("UnSupported Frame Format."));

    }

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_header_field_adjustment_config_set (unit, port, CPRIMOD_DIR_RX, ext_hdr_enable , len_field_adjust));

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_header_field_adjustment_config_set (unit, port, CPRIMOD_DIR_TX, ext_hdr_enable , len_field_adjust*-1));

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_header_field_mapping_set(unit, port, &encap_map));

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_header_field_mapping_set(unit, port, &decap_map));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_compression_saturation_config_set(int unit, int port, pm_info_t pm_info, uint32 threshold_value)
{
    SOC_INIT_FUNC_DEFS;
   
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_compression_saturation_config_set(unit, port, threshold_value));
exit:
    SOC_FUNC_RETURN;
   
}

int cpm4x25_cpri_port_compression_saturation_config_get(int unit, int port, pm_info_t pm_info, uint32* threshold_value)
{
    SOC_INIT_FUNC_DEFS;
   
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_compression_saturation_config_get(unit, port, threshold_value));


exit:
    SOC_FUNC_RETURN;
   
}


int cpm4x25_cpri_port_decompression_saturation_config_set(int unit, int port, pm_info_t pm_info, uint32 sat_value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_decompression_saturation_config_set(unit, port, sat_value));


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decompression_saturation_config_get(int unit, int port, pm_info_t pm_info, uint32* sat_value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_decompression_saturation_config_get(unit, port, sat_value));


exit:
    SOC_FUNC_RETURN;

}
int cpm4x25_cpri_port_rx_pipeline_clear(int unit, int port, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_pipeline_clear(unit, port));
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_tx_pipeline_clear(int unit, int port, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_pipeline_clear(unit, port));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_enable_get(int unit, int port, pm_info_t pm_info, int *enable)
{
    int lane;
    phymod_phy_access_t phy_access;
    phymod_phy_tx_lane_control_t lane_control;
    SOC_INIT_FUNC_DEFS;
    sal_memcpy(&phy_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_core_access_t));
    lane = (SOC_INFO(unit).port_l2p_mapping[port]-1)%CPRIMOD_MAX_LANES_PER_CORE;
    phy_access.access.lane_mask = (1<<lane);

    _SOC_IF_ERR_EXIT
        (phymod_phy_tx_lane_control_get(&phy_access, &lane_control));
    if(lane_control == phymodTxSquelchOn) {
        *enable = 0;
    } else {
        *enable = 1;
    }
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    int lane;
    phymod_phy_access_t phy_access;
    SOC_INIT_FUNC_DEFS;
    sal_memcpy(&phy_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_core_access_t));
    lane = (SOC_INFO(unit).port_l2p_mapping[port]-1)%CPRIMOD_MAX_LANES_PER_CORE;
    phy_access.access.lane_mask = (1<<lane);

    _SOC_IF_ERR_EXIT
        (phymod_phy_tx_lane_control_set(&phy_access, (enable)? phymodTxSquelchOff:phymodTxSquelchOn));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_synce_clk_ctrl_set(int unit, int port, pm_info_t pm_info,
                                    const portmod_port_synce_clk_ctrl_t* cfg)
{
    phymod_synce_clk_ctrl_t phy_synce_cfg;
    phymod_phy_access_t phy_access;
    int val, lane;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if (val == 0) {
        /* Ethernet port. */
        _rv = pm4x25_port_synce_clk_ctrl_set(unit, port, pm_info, cfg);
        SOC_FUNC_RETURN;
    } else {
        /* Cpri port. */
        phymod_synce_clk_ctrl_t_init(&phy_synce_cfg);
        phy_synce_cfg.stg0_mode = cfg->stg0_mode;
        phy_synce_cfg.stg1_mode = cfg->stg1_mode;
        phy_synce_cfg.sdm_val = cfg->sdm_val;

        sal_memcpy(&phy_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_core_access_t));
        lane = (SOC_INFO(unit).port_l2p_mapping[port] - 1) % CPRIMOD_MAX_LANES_PER_CORE;
        phy_access.access.lane_mask = (1 << lane);
        /*
         * SyncE logic sits inside TSC.
         * Call TSC driver for SyncE programming.
         */
        _SOC_IF_ERR_EXIT(tscf_gen3_phy_synce_clk_ctrl_set(&phy_access,
                                                          phy_synce_cfg));
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_synce_clk_ctrl_get(int unit, int port, pm_info_t pm_info,
                                    portmod_port_synce_clk_ctrl_t* cfg)
{
    phymod_synce_clk_ctrl_t phy_synce_cfg;
    phymod_phy_access_t phy_access;
    int val, lane;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if (val == 0) {
        /* Ethernet port. */
        _rv = pm4x25_port_synce_clk_ctrl_get(unit, port, pm_info, cfg);
        SOC_FUNC_RETURN;
    } else {
        phymod_synce_clk_ctrl_t_init(&phy_synce_cfg);

        sal_memcpy(&phy_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_core_access_t));
        lane = (SOC_INFO(unit).port_l2p_mapping[port]-1) % CPRIMOD_MAX_LANES_PER_CORE;
        phy_access.access.lane_mask = (1 << lane);
        _SOC_IF_ERR_EXIT(tscf_gen3_phy_synce_clk_ctrl_get(&phy_access,
                                                          &phy_synce_cfg));
        cfg->stg0_mode = phy_synce_cfg.stg0_mode;
        cfg->stg1_mode = phy_synce_cfg.stg1_mode;
        cfg->sdm_val = phy_synce_cfg.sdm_val;
    }

exit:
    SOC_FUNC_RETURN;
}

#ifdef CPRIMOD_SUPPORT
int cpm4x25_cpri_port_speed_set(int unit, int port, pm_info_t pm_info, cprimod_supported_port_speed_t speed_id)
{
    int rv;
    int lane,i;
    int tmp_port=0;
    int pll_sel=0;
    int pll_lock=0;
    int port_rst =0;
    cprimod_port_init_config_t port_config;
    phymod_phy_access_t phy_access;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_init_config_t_init(&port_config));

    port_config.pll_in_use = cprimodInUseDefault;
    for( i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        _SOC_IF_ERR_EXIT(rv);
        /*
         * Get all the PLL usage information for the core.
         * The port is out of reset and use the PLL.
         */
        if(tmp_port > 0) {
            sal_memcpy(&phy_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_core_access_t));
            lane = (SOC_INFO(unit).port_l2p_mapping[tmp_port]-1)%CPRIMOD_MAX_LANES_PER_CORE;
            phy_access.access.lane_mask = (1<<lane);
            _SOC_IF_ERR_EXIT(cprimod_cpri_port_reset_get(unit, tmp_port, &port_rst));
            if(port_rst == 0) {
               _SOC_IF_ERR_EXIT(cprimod_cpri_pll_status_get(unit, tmp_port, &phy_access, &pll_sel, &pll_lock));
            }
            if ((port_rst==0) && (pll_sel==0)) {
               port_config.pll_in_use |= cprimodInUsePll0;
            }
            if ((port_rst==0) && (pll_sel==1)) {
               port_config.pll_in_use |= cprimodInUsePll1;
            }
        }
    }

    switch (speed_id) {
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_1228P8:
            port_config.tx_cpri_speed  = cprimodSpd1228p8;
            port_config.rx_cpri_speed  = cprimodSpd1228p8;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_2457P6:
            port_config.tx_cpri_speed  = cprimodSpd2457p6;
            port_config.rx_cpri_speed  = cprimodSpd2457p6;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_3072P0:
            port_config.tx_cpri_speed  = cprimodSpd3072p0;
            port_config.rx_cpri_speed  = cprimodSpd3072p0;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_4915P2:
            port_config.tx_cpri_speed  = cprimodSpd4915p2;
            port_config.rx_cpri_speed  = cprimodSpd4915p2;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_6144P0:
            port_config.tx_cpri_speed  = cprimodSpd6144p0;
            port_config.rx_cpri_speed  = cprimodSpd6144p0;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_9830P4:
            port_config.tx_cpri_speed  = cprimodSpd9830p4;
            port_config.rx_cpri_speed  = cprimodSpd9830p4;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_10137P6:
            port_config.tx_cpri_speed  = cprimodSpd10137p6;
            port_config.rx_cpri_speed  = cprimodSpd10137p6;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_12165P12:
            port_config.tx_cpri_speed  = cprimodSpd12165p12;
            port_config.rx_cpri_speed  = cprimodSpd12165p12;
            break;
        case CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_24330P24:
            port_config.tx_cpri_speed  = cprimodSpd24330p24;
            port_config.rx_cpri_speed  = cprimodSpd24330p24;
            break;
        default:
            _SOC_EXIT_WITH_ERR (SOC_E_PARAM, ("UnSupported Speed."));
            break;
    }
    
    port_config.interface = cprimodCpri;

    port_config.vco_for_lane = _cprimod_port_speed_id_to_vco(speed_id, _SHR_PORT_ENCAP_CPRI);

    sal_memcpy(&phy_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_core_access_t));
    lane = (SOC_INFO(unit).port_l2p_mapping[port]-1)%CPRIMOD_MAX_LANES_PER_CORE;
    phy_access.access.lane_mask = (1<<lane);

    _SOC_IF_ERR_EXIT(cprimod_cpri_port_speed_set(unit, port, &phy_access, &port_config));
    
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_debug_set(int unit, int port, pm_info_t pm_info, portmod_cpri_debug_attr_t type, int index, int buffer_size, const uint32* data)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_debug_set(unit, port, type, index, buffer_size, data));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_debug_get(int unit, int port, pm_info_t pm_info, portmod_cpri_debug_attr_t type, int index, int buffer_size, uint32* data)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_debug_get(unit, port, type, index, buffer_size, data));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_debug_clear(int unit, int port, pm_info_t pm_info, portmod_cpri_debug_attr_t type, int index)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_debug_clear(unit, port, type, index));

exit:
    SOC_FUNC_RETURN;

}

#endif /* CPRIMOD_SUPPORT */

#endif /* PORTMOD_CPM4X25_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
