/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        firelight_tdm.c
 * Purpose:
 * Requires:
 */


#include <shared/bsl.h>
#include <soc/tdm/core/tdm_top.h>

#include <soc/defs.h>
#include <soc/mem.h>

#if defined(BCM_FIRELIGHT_SUPPORT)
#include <soc/firelight.h>
#include <soc/firelight_tdm.h>
#include <soc/tdm/firelight/tdm_fl_defines.h>
#include <soc/bondoptions.h>


/*** START SDK API COMMON CODE ***/

#define SOC_FL_MAX_LOGICAL_PORTS  66

/*! @fn void _soc_fl_tdm_print_port_map(
        int unit,
        soc_port_map_type_t *port_map)
    @param unit Chip unit number.
    @param port_map Pointer to a soc_port_map_type_t struct variable.
    @brief Debug API to print soc_port_map_type_t
 */
void
_soc_fl_tdm_print_port_map(int unit, soc_port_map_type_t *port_map)
{
    int port;
    int is_hg2;
    int speed, phy_port, idb_port, mmu_port, num_lanes;
    int phy_bmap, p2l_map, m2p_map;

    for (port = 0; port < FIRELIGHT_TDM_LOG_PORTS_PER_DEV; port++) {
        if (port_map->log_port_speed[port] > 0) {
            speed = port_map->log_port_speed[port] / 1000;
            phy_port = port_map->port_l2p_mapping[port];
            idb_port = port_map->port_l2i_mapping[port];
            mmu_port = port_map->port_p2m_mapping[phy_port];
            num_lanes = port_map->port_num_lanes[port];
            phy_bmap = SOC_PBMP_MEMBER(port_map->physical_pbm,
                                       phy_port) ? 1 : 0;
            is_hg2 = SOC_PBMP_MEMBER(port_map->hg2_pbm, port) ? 1 : 0;
            p2l_map = port_map->port_p2l_mapping[phy_port];
            m2p_map = port_map->port_m2p_mapping[mmu_port];
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "log_port=%3d phy_port=%3d speed=%3dG "
                        "%s idb_port=%3d mmu_port=%3d "
                        "num_lanes=%1d phy_bmap=%1d "
                        "p2l_map=%3d "),
                        port, phy_port, speed,
                        (is_hg2 == 1) ? "HG2" : "ETH",
                        idb_port, mmu_port, num_lanes,
                        phy_bmap, p2l_map));
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "m2p_map=%3d\n"),
                         m2p_map));
        }
    }
}

/*! @fn void _soc_fl_tdm_print_tdm_info(
        int unit,
        soc_tdm_schedule_t *tdm_info)
    @param unit Chip unit number.
    @param tdm_info Pointer to a soc_tdm_schedule_t struct variable.
    @brief Debug API to print soc_tdm_schedule_t
 */
void
_soc_fl_tdm_print_tdm_info(int unit, soc_tdm_schedule_t *tdm_info)
{
    int cal_len, j;

    for (cal_len = tdm_info->cal_len; cal_len > 0; cal_len--) {
        if (tdm_info->linerate_schedule[cal_len - 1] != FL_NUM_EXT_PORTS) {
            break;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "\tMAIN CALENDAR: cal_len=%3d \n"),
                cal_len ));
    for (j = 0; j < cal_len; j++) {
        if (j % 16 == 0) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "\n%3d :%3d\t"),
                        j, j + 15));
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "%4d"),
                    tdm_info->linerate_schedule[j]));
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\n")));
}


/*! @fn void _soc_fl_tdm_print_schedule_state(
        int unit,
        soc_port_schedule_state_t *sch_info)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @brief Debug API to print sch_info
 */
void
_soc_fl_tdm_print_schedule_state(int unit,
                                  soc_port_schedule_state_t *sch_info)
{
    int print_tdm;
    soc_tdm_schedule_t *tdm_schedule;

    print_tdm = 1;

    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "\n\nPRINTING soc_port_schedule_state_t \n")));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "nport= %3d\n"),
                sch_info->nport));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "frequency= %4dMHz\n"),
                sch_info->frequency));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "bandwidth= %4d\n"),
                sch_info->bandwidth));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "io_bandwidth= %4d\n"),
                sch_info->io_bandwidth));

    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "\nPRINTING soc_port_schedule_state_t"
                "::[in_port_map]\n")));
    _soc_fl_tdm_print_port_map(unit, &sch_info->in_port_map);


    if (print_tdm == 1) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "\n====== TDM Tables \n")));
        tdm_schedule = &(sch_info->tdm_ingress_schedule_pipe[0]. \
                           tdm_schedule_slice[0]);
        _soc_fl_tdm_print_tdm_info(unit, tdm_schedule);
    }
}

/*! @fn int _soc_fl_tdm_calculation(
        int unit,
        soc_port_schedule_state_t *sch_info)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @brief API to calculate TDM tables.
 */
int
_soc_fl_tdm_calculation(int unit, soc_port_schedule_state_t *sch_info)
{
    int i, phy_port, lgc_port;
    int pipe = 0;
    int index;
 /*   int mgmt_pm_hg = 0; */
    int tdm_idb_cal_len, tdm_macsec_cal_len;
    int *tdm_idb_pipe_main;
    tdm_soc_t _chip_pkg;
    tdm_mod_t *_tdm_pkg;
    uint32 port_speeds[FIRELIGHT_TDM_PHY_PORTS_PER_DEV];
    uint32 port_states[FIRELIGHT_TDM_PHY_PORTS_PER_DEV];
    uint32 speed_sum = 0;

    sal_memset(port_speeds, 0,
               FIRELIGHT_TDM_PHY_PORTS_PER_DEV * sizeof(uint32));
    sal_memset(port_states, 0,
               FIRELIGHT_TDM_PHY_PORTS_PER_DEV * sizeof(uint32));

    /* Remap input speeds to ETH bitrates */
    soc_fl_port_schedule_speed_remap(unit, sch_info);

    /* Set port states and speeds */
    for (lgc_port = 0;
        lgc_port < FIRELIGHT_TDM_LOG_PORTS_PER_DEV; lgc_port++) {
        if (sch_info->in_port_map.log_port_speed[lgc_port] > 0 ) {
            phy_port = sch_info->in_port_map.port_l2p_mapping[lgc_port];
            if (phy_port < FIRELIGHT_TDM_PHY_PORTS_PER_DEV) {
                port_states[phy_port] = PORT_STATE__LINERATE;
                port_speeds[phy_port] = sch_info->in_port_map. \
                                            log_port_speed[lgc_port];
            }
        }
    }
    /* Print input config */
    if (LOG_CHECK(BSL_LS_SOC_TDM | BSL_INFO)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "frequency: %dMHz\n"),
                    sch_info->frequency));
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "%8s%8s%8s\n"),
                    "port", "speed", "state"));
        for (phy_port = 0;
             phy_port < FIRELIGHT_TDM_PHY_PORTS_PER_DEV; phy_port++) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "%8d%8d%8d\n"),
                        phy_port, port_speeds[phy_port] / 1000,
                        port_states[phy_port]));
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\n")));
    }
    /* Initialize input variables for SW TDM Generation:
       -- unit
       -- num_ext_ports
       -- state
       -- speed
       -- clk_freq
    */
    _chip_pkg.unit = unit;
    _chip_pkg.num_ext_ports = FL_NUM_EXT_PORTS;
    _chip_pkg.state = sal_alloc((_chip_pkg.num_ext_ports)*sizeof(int),
                                "port state list");
    if (_chip_pkg.state == NULL) {
        return SOC_E_MEMORY;
    }
    _chip_pkg.speed = sal_alloc((_chip_pkg.num_ext_ports)*sizeof(int),
                                "port speed list");
    if (_chip_pkg.speed == NULL) {
        sal_free(_chip_pkg.state);
        return SOC_E_MEMORY;
    }
    for (index = 0; index < _chip_pkg.num_ext_ports; index++) {
        _chip_pkg.state[index] = 0;
        _chip_pkg.speed[index] = 0;
    }
    for (index = 0;
         index < _chip_pkg.num_ext_ports &&
         index < FIRELIGHT_TDM_PHY_PORTS_PER_DEV; index++) {
        _chip_pkg.state[index] = port_states[index];
        _chip_pkg.speed[index] = port_speeds[index];
        if (index != 0) {
            /* Don't count the speed of CPU port in speed_sum */
            speed_sum = speed_sum + port_speeds[index];
        }
    }
    /* Shift port state to left one position; required by C_TDM */
    for (index = 1; index < _chip_pkg.num_ext_ports; index++) {
        _chip_pkg.state[index - 1] = _chip_pkg.state[index];
    }
    _chip_pkg.clk_freq = sch_info->frequency;
    /* _chip_pkg.soc_vars.fl.mgmt_pm_hg = mgmt_pm_hg; */
    _chip_pkg.soc_vars.fl.tdm_chk_en = 1;

    if (speed_sum > 420000) {
        /* Don't need to support HG if speed_sum greater than 420Gbps */
        _chip_pkg.soc_vars.fl.cal_hg_en = 0;
    } else {
        _chip_pkg.soc_vars.fl.cal_hg_en = 1;
    }

    /*
     * Get ms_port_en[FL_NUM_EXT_PORTS] from property macsec_port_index
     * and macsec_enable.
     */
    for (phy_port = 0; phy_port < FL_NUM_EXT_PORTS; phy_port++) {
        _chip_pkg.soc_vars.fl.ms_port_en[phy_port] = 0;
    }

    for (lgc_port = 0; lgc_port < SOC_FL_MAX_LOGICAL_PORTS; lgc_port++) {
        if (soc_fl_macsec_idx_get(unit, lgc_port) != -1) {
            phy_port = sch_info->in_port_map.port_l2p_mapping[lgc_port];
            _chip_pkg.soc_vars.fl.ms_port_en[phy_port] = 1;
        }
    }

    /*
     * 1. Calculate ms_clk_freq from
     * (a) If any 100G or 50G or 25G speed in CLPORT are used,
     *      MACsec clock = 950MHz (ms_bandwidth = 600Gbps)
     * (b) if any 40G or 20G or 10G speed in XLPORT are used,
     *      MACsec clock = 380MHz (ms_bandwidth = 240Gbps)
     * (c) If only GPORT is used,
     *      MACsec clock = 190MHz (ms_bandwidth = 120Gbps)
     */
    _chip_pkg.soc_vars.fl.ms_clk_freq = 190;
    for (lgc_port = 0;
         lgc_port < FIRELIGHT_TDM_LOG_PORTS_PER_DEV; lgc_port++) {
         phy_port = sch_info->in_port_map.port_l2p_mapping[lgc_port];
         if (IS_CL_PORT(unit, lgc_port) &&
             _chip_pkg.soc_vars.fl.ms_port_en[phy_port]) {
            if ((_chip_pkg.speed[phy_port] >= 50000) ||
                (_chip_pkg.speed[phy_port] == 25000) ||
                (_chip_pkg.speed[phy_port] == 27000)) {
                _chip_pkg.soc_vars.fl.ms_clk_freq = 950;
                break;
            }
         } else if (IS_XL_PORT(unit, lgc_port) &&
             _chip_pkg.soc_vars.fl.ms_port_en[phy_port]) {
            if ((_chip_pkg.speed[phy_port] == 40000) ||
                (_chip_pkg.speed[phy_port] == 42000) ||
                (_chip_pkg.speed[phy_port] == 20000) ||
                (_chip_pkg.speed[phy_port] == 21000) ||
                (_chip_pkg.speed[phy_port] == 10000) ||
                (_chip_pkg.speed[phy_port] == 11000)) {
                _chip_pkg.soc_vars.fl.ms_clk_freq = 380;
            }
         }
    }


    /* Populate TDM tables by invoking SW TDM API */
    /* coverity[uninit_use_in_call : FALSE] */
    _tdm_pkg = _soc_set_tdm_tbl(SOC_SEL_TDM(&_chip_pkg));

    /* Free the memory allocated to port states and speeds */
    sal_free(_chip_pkg.state);
    sal_free(_chip_pkg.speed);

    /* Copy TDM result into output structure */
    if (_tdm_pkg == NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "Unsupported config for TDM calendar "
                            "generation\n")));
        return SOC_E_FAIL;
    } else {
        /* IDB calendar (0) */
        tdm_idb_pipe_main = _tdm_pkg->_chip_data.cal_0.cal_main;
        tdm_idb_cal_len = _tdm_pkg->_chip_data.cal_0.cal_len;

        if (tdm_idb_pipe_main == NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "Unsupported config for TDM calendar "
                                "generation\n")));
            tdm_fl_main_free(_tdm_pkg);
            sal_free(_tdm_pkg);
            return SOC_E_FAIL;
        } else {
            /* IDB TDM main calendar */
            for (index = 0; index < tdm_idb_cal_len; index++) {
                sch_info->tdm_ingress_schedule_pipe[pipe]. \
                    tdm_schedule_slice[0].linerate_schedule[index] = \
                tdm_idb_pipe_main[index];
            }
            sch_info->tdm_ingress_schedule_pipe[pipe].tdm_schedule_slice[0]. \
                cal_len = tdm_idb_cal_len;

            /* MACSEC TDM main calendar */
            tdm_macsec_cal_len = _tdm_pkg->_chip_data.soc_pkg.soc_vars.fl.ms_cal_len;
            for (index = 0; index < tdm_macsec_cal_len ; index++) {
                sch_info->tdm_ingress_schedule_pipe[pipe]. \
                    tdm_schedule_slice[1].linerate_schedule[index] = \
                _tdm_pkg->_chip_data.soc_pkg.soc_vars.fl.ms_cal[index];
            }
            sch_info->tdm_ingress_schedule_pipe[pipe].tdm_schedule_slice[1]. \
                cal_len = tdm_macsec_cal_len;
        }

        /* Free the memory allocated in TDM algorithm code */
        tdm_fl_main_free(_tdm_pkg);
        sal_free(_tdm_pkg);
    }

    /* Print TDM result */
    pipe = 0;
    /* IDB TDM main calendar */
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "Pipe %d [IDB TDM main calendar]\n"),
                pipe));
    for (index = 0; index < MAX_CAL_LEN; index++) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    " %3d"),
                    sch_info->tdm_ingress_schedule_pipe[pipe]. \
                      tdm_schedule_slice[0].linerate_schedule[index]));
        if (((index + 1) % 16 == 0) && index > 0) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\n")));
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\n")));

    /* If init then just copy in_port_map to out_port_map*/
    if (sch_info->is_flexport == 0) {
        for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
            sch_info->out_port_map.log_port_speed[i] =
                sch_info->in_port_map.log_port_speed[i];
            sch_info->out_port_map.port_p2l_mapping[i] =
                sch_info->in_port_map.port_p2l_mapping[i];
            sch_info->out_port_map.port_l2p_mapping[i] =
                sch_info->in_port_map.port_l2p_mapping[i];
            sch_info->out_port_map.port_p2m_mapping[i] =
                sch_info->in_port_map.port_p2m_mapping[i];
            sch_info->out_port_map.port_num_lanes[i] =
                sch_info->in_port_map.port_num_lanes[i];
        }
        for (i = 0; i < SOC_MAX_NUM_MMU_PORTS; i++) {
            sch_info->out_port_map.port_m2p_mapping[i] =
                sch_info->in_port_map.port_m2p_mapping[i];
        }
        for (i = 0; i < _SHR_PBMP_WORD_MAX; i++) {
            sch_info->out_port_map.physical_pbm.pbits[i] =
                sch_info->in_port_map.physical_pbm.pbits[i];
            sch_info->out_port_map.hg2_pbm.pbits[i] =
                sch_info->in_port_map.hg2_pbm.pbits[i];
        }
    }

    _soc_fl_tdm_print_schedule_state(unit, sch_info);

    return SOC_E_NONE;
}

int
_soc_fl_tdm_set_macsec_calendar(int unit, soc_port_schedule_state_t *sch_info)
{
    uint32 macsec_cal_len;
    int pipe, slot, length;
    uint32 rval, tdm_entry = 0;
    int macsec_port;

    pipe = 0;

    /* Macsec TDM Calendar is taken from slice 1 */
    macsec_cal_len = sch_info->tdm_ingress_schedule_pipe[pipe]. \
                  tdm_schedule_slice[1].cal_len;
    for (length = macsec_cal_len; length > 0; length--) {
        if (sch_info->tdm_ingress_schedule_pipe[pipe].tdm_schedule_slice[1]
                .linerate_schedule[length - 1] != FL_NUM_EXT_PORTS) {
            break;
        }
    }

    for (slot = 0; slot < length; slot ++) {
        macsec_port = sch_info->tdm_ingress_schedule_pipe[
            pipe].tdm_schedule_slice[1].linerate_schedule[slot];

        soc_mem_field32_set(unit, MACSEC_TDM_CALENDARm, &tdm_entry, PORTIDf, macsec_port);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, MACSEC_TDM_CALENDARm, MEM_BLOCK_ALL, slot, &tdm_entry));
    }

    SOC_IF_ERROR_RETURN(READ_MACSEC_TDM_WRAP_PTRr(unit, &rval));
    soc_reg_field_set(unit, MACSEC_TDM_WRAP_PTRr, &rval, ACTIVE_TDM_CALf, 0);
    soc_reg_field_set(unit, MACSEC_TDM_WRAP_PTRr, &rval, TDM_WRAP_PTRf, (length - 1));
    SOC_IF_ERROR_RETURN(WRITE_MACSEC_TDM_WRAP_PTRr(unit, rval));

    return SOC_E_NONE;
}

/*! @fn int _soc_fl_tdm_set_idb_calendar(
        int unit,
        soc_port_schedule_state_t *sch_info)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @brief API to initialize the TD3 IDB main calendar.
 */
int
_soc_fl_tdm_set_idb_calendar(int unit, soc_port_schedule_state_t *sch_info)
{
    int rv = SOC_E_NONE;
    uint32 cal_len;
    int pipe, slot, length;
    uint32 rval;
    iarb_tdm_table_entry_t iarb_tdm;
    mmu_arb_tdm_table_entry_t mmu_arb_tdm;
    int mmu_port, phy_port;
    uint32 strap_sts = 0;
    int macsec_ing_byp = 0, macsec_egr_byp = 0;
    int bond_macsec_enable = 0;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    SOC_IF_ERROR_RETURN(READ_TOP_STRAP_STATUSr(unit, &strap_sts));
    macsec_ing_byp = soc_reg_field_get(unit, TOP_STRAP_STATUSr,
                                  strap_sts, STRAP_MACSEC_ING_BYP_ENf);
    macsec_egr_byp = soc_reg_field_get(unit, TOP_STRAP_STATUSr,
                                  strap_sts, STRAP_MACSEC_EGR_BYP_ENf);
    if (SAL_BOOT_SIMULATION) {
        bond_macsec_enable = 1;
    } else {
        bond_macsec_enable = !SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureNoMacsec);
    }
    if ((!macsec_ing_byp || !macsec_egr_byp) && bond_macsec_enable) {
        SOC_IF_ERROR_RETURN
            (_soc_fl_tdm_set_macsec_calendar(unit, sch_info));
    }

    pipe = 0;
    /* TDM Calendar is always taken from slice 0 */
    cal_len = sch_info->tdm_ingress_schedule_pipe[pipe]. \
                  tdm_schedule_slice[0].cal_len;
    for (length = cal_len; length > 0; length--) {
        if (sch_info->tdm_ingress_schedule_pipe[pipe].tdm_schedule_slice[
                0].linerate_schedule[length - 1] != FL_NUM_EXT_PORTS) {
            break;
        }
    }

    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 1);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf, 83);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    for (slot = 0; slot < length; slot ++) {
        phy_port = sch_info->tdm_ingress_schedule_pipe[
            pipe].tdm_schedule_slice[0].linerate_schedule[slot];
        if (phy_port == -1) {
            /* Convert IDLE slot definition */
            phy_port = 127;
        }
        mmu_port = (phy_port != 127) ? si->port_p2m_mapping[phy_port] : 127;

        sal_memset(&iarb_tdm, 0, sizeof(iarb_tdm_table_entry_t));
        sal_memset(&mmu_arb_tdm, 0, sizeof(mmu_arb_tdm_table_entry_t));

        soc_IARB_TDM_TABLEm_field32_set(unit, &iarb_tdm, PORT_NUMf,
                                        phy_port);
        soc_MMU_ARB_TDM_TABLEm_field32_set(unit, &mmu_arb_tdm, PORT_NUMf,
                                        mmu_port);

        if (slot == length - 1) {
            soc_MMU_ARB_TDM_TABLEm_field32_set(unit, &mmu_arb_tdm, WRAP_ENf, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, slot,
                                                &iarb_tdm));
        SOC_IF_ERROR_RETURN(WRITE_MMU_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, slot,
                                                    &mmu_arb_tdm));

    }
    rval = 0;
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 0);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf,
                    length -1);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    /* Update TDM size info */
    rv = soc_fl_tdm_size_set(unit, length);

    return rv;
}

/*! @fn int soc_fl_tdm_init(
        int unit,
        soc_port_schedule_state_t *sch_info)
    @param unit Chip unit number.
    @param sch_info Pointer to a soc_port_schedule_state_t struct variable.
    @brief Main API for TDM init
 */
int soc_fl_tdm_init(int unit, soc_port_schedule_state_t *sch_info)
{
    SOC_IF_ERROR_RETURN(_soc_fl_tdm_calculation(unit, sch_info));
    
    SOC_IF_ERROR_RETURN(_soc_fl_tdm_set_idb_calendar(unit, sch_info));

    return SOC_E_NONE;
}

/*! @fn int soc_fl_port_speed_higig2eth(int speed)
 *  @param int speed
 *  @brief Map port speed to ETH bitrates
 */
static int
soc_fl_port_speed_higig2eth(int speed)
{
    switch (speed) {
    case 11000:
        /* 10G */
        return 10000;
    case 21000:
        /* 20G */
        return 20000;
    case 42000:
        /* 40G */
        return 40000;
    case 53000:
        /* 50G */
        return 50000;
    default:
        return speed;
    }
}

/*! @fn void soc_fl_port_schedule_speed_remap(int unit,
 *               soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @brief API to remap speeds to ETH bitrates
 */
void
soc_fl_port_schedule_speed_remap(
    int  unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    int port;

    /* Speed remap for in_port_map*/
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        port_schedule_state->in_port_map.log_port_speed[port] =
            soc_fl_port_speed_higig2eth(
                port_schedule_state->in_port_map.log_port_speed[port]);
    }

}

/*** END SDK API COMMON CODE ***/

#endif /* BCM_FIRELIGHT_SUPPORT */
