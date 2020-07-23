/*! \file bcmpc_phy_diag.c
 *
 * BCMPC diagnostic APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <phymod/phymod_diagnostics.h>

#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_phy.h>
#include <bcmpc/bcmpc_diag.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcmpc/bcmpc_pmgr_internal.h>


/* Log source for this component */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PHYMOD


/*******************************************************************************
 * Local definitions
 */

/*! Polynomial map. */
typedef struct pc_phy_prbs_poly_s {
    bcmpc_phy_prbs_poly_t pc_mode;
    phymod_prbs_poly_t phy_mode;
} pc_phy_prbs_poly_t;

/*! Eyescan mode map. */
typedef struct pc_phy_eyescan_s {
    bcmpc_phy_eyescan_mode_t pc_mode;
    phymod_eyescan_mode_t phy_mode;
} pc_phy_eyescan_t;

static const pc_phy_prbs_poly_t pc_phy_prbs_poly_map[] = {
    /* bcmpc_phy_prbs_poly_t, phymod_prbs_poly_t */
    { BCMPC_PHY_PRBS_POLY_7, phymodPrbsPoly7 },
    { BCMPC_PHY_PRBS_POLY_9, phymodPrbsPoly9 },
    { BCMPC_PHY_PRBS_POLY_11, phymodPrbsPoly11 },
    { BCMPC_PHY_PRBS_POLY_15, phymodPrbsPoly15 },
    { BCMPC_PHY_PRBS_POLY_23, phymodPrbsPoly23 },
    { BCMPC_PHY_PRBS_POLY_31, phymodPrbsPoly31 },
    { BCMPC_PHY_PRBS_POLY_58, phymodPrbsPoly58 },
    { BCMPC_PHY_PRBS_POLY_49, phymodPrbsPoly49 },
    { BCMPC_PHY_PRBS_POLY_10, phymodPrbsPoly10 },
    { BCMPC_PHY_PRBS_POLY_20, phymodPrbsPoly20 },
    { BCMPC_PHY_PRBS_POLY_13, phymodPrbsPoly13 },
    { BCMPC_PHY_PRBS_POLY_Q13, phymodPrbsPolyQ13 }
};

static const pc_phy_eyescan_t pc_phy_eyescan_map[] = {
    /* bcmpc_phy_eyescan_mode_t, phymod_eyescan_mode_t */
    { BCMPC_PHY_EYESCAN_MODE_FAST, phymodEyescanModeFast },
    { BCMPC_PHY_EYESCAN_MODE_BER_PROJ, phymodEyescanModeBERProj }
};


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Convert the PRBS control flags to PHY.
 *
 * \param [in] uint32_t pc_flags PC flag value.
 *
 * return PHYMOD PRBS control flag value.
 */
static uint32_t
pc_prbs_cfg_flag_to_phy(uint32_t pc_flags)
{
    uint32_t phy_flags = 0;

    if (BCMPC_BIT_GET(pc_flags, BCMPC_PHY_PRBS_CFG_F_RX) ==
        BCMPC_BIT_GET(pc_flags, BCMPC_PHY_PRBS_CFG_F_TX)) {
        phy_flags = 0;
    } else if (BCMPC_BIT_GET(pc_flags, BCMPC_PHY_PRBS_CFG_F_RX)) {
        PHYMOD_PRBS_DIRECTION_RX_SET(phy_flags);
    } else {
        PHYMOD_PRBS_DIRECTION_TX_SET(phy_flags);
    }

    return phy_flags;
}

/*!
 * \brief Convert the PRBS configurations to PHY.
 *
 * \param [in] pc_prbs PC configurations.
 * \param [out] phy_prbs PHYMOD configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to convert the configuration.
 */
static int
pc_prbs_cfg_to_phy(bcmpc_phy_prbs_cfg_t *pc_prbs, phymod_prbs_t *phy_prbs)
{
    int idx;

    phymod_prbs_t_init(phy_prbs);
    phy_prbs->invert = pc_prbs->invert;

    for (idx = 0; idx < COUNTOF(pc_phy_prbs_poly_map); idx++) {
        if (pc_prbs->poly == pc_phy_prbs_poly_map[idx].pc_mode) {
            phy_prbs->poly = pc_phy_prbs_poly_map[idx].phy_mode;
            return SHR_E_NONE;
        }
    }

    return SHR_E_FAIL;
}

/*!
 * \brief Convert the eyescan mode to PHY.
 *
 * \param [in] pc_mode PC eyescan mode.
 * \param [out] phy_mode PHYMOD eyescan mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to convert the mode.
 */
static int
pc_eyescan_mode_to_phy(bcmpc_phy_eyescan_mode_t pc_mode,
                       phymod_eyescan_mode_t *phy_mode)
{
    int idx;

    for (idx = 0; idx < COUNTOF(pc_phy_eyescan_map); idx++) {
        if (pc_mode == pc_phy_eyescan_map[idx].pc_mode) {
            *phy_mode = pc_phy_eyescan_map[idx].phy_mode;
            return SHR_E_NONE;
        }
    }

    return SHR_E_FAIL;
}

static int
pc_port_speed_get(int unit, bcmpc_pport_t pport, uint32_t *speed) {

    bcmpc_pmgr_port_status_t pst;

    SHR_FUNC_ENTER(unit);

    bcmpc_pmgr_port_status_t_init(&pst);

    SHR_IF_ERR_EXIT
            (bcmpc_pmgr_port_status_get(unit, pport, &pst));

    *speed = pst.speed;

exit:
     SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmpc_phy_prbs_config_set(int unit, bcmpc_pport_t pport, int lane_idx,
                          uint32_t flags, bcmpc_phy_prbs_cfg_t *prbs)
{
    int lane_index = 0, pm_id = 0;
    phymod_phy_access_t phy_access, *pa = &phy_access;
    bcmpc_phy_access_data_t acc_data;
    uint32_t phy_flags = 0;
    phymod_prbs_t phy_prbs;
    bcmpc_pm_lport_info_t lport_info;
    bcmpc_topo_t pm_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(prbs, SHR_E_PARAM);

    BCMPC_PHY_ACCESS_DATA_SET(&acc_data, unit, pport);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_get(unit, pm_id, &pm_info));

    SHR_IF_ERR_EXIT
        (bcmpc_pm_lport_info_get(unit, pport, NULL, &lport_info));
    if ((lane_idx != -1) && (lane_idx < shr_util_popcount(lport_info.lbmp))) {
        lane_index = lane_idx  + (pport - pm_info.base_pport);
        SHR_IF_ERR_EXIT
            (bcmpc_phymod_phy_access_t_init(unit, pport, 1 << lane_index,
                                            &acc_data, pa));
    } else {
        SHR_IF_ERR_EXIT
            (bcmpc_phymod_phy_access_t_init(unit, pport, 0, &acc_data, pa));
    }
    phy_flags = pc_prbs_cfg_flag_to_phy(flags);
    SHR_IF_ERR_EXIT
        (pc_prbs_cfg_to_phy(prbs, &phy_prbs));

    PHY_IF_ERR_EXIT
        (phymod_phy_prbs_config_set(pa, phy_flags, &phy_prbs));

exit:
    SHR_FUNC_EXIT();

}

int
bcmpc_phy_prbs_status_get(int unit, bcmpc_pport_t pport, int lane_idx,
                          uint32_t flags, bcmpc_phy_prbs_status_t *status)
{
    int lane_index = 0, pm_id = 0;
    phymod_phy_access_t phy_access, *pa = &phy_access;
    bcmpc_phy_access_data_t acc_data;
    phymod_prbs_status_t phy_status;
    bcmpc_pm_lport_info_t lport_info;
    bcmpc_topo_t pm_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(status, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
         (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_get(unit, pm_id, &pm_info));

    BCMPC_PHY_ACCESS_DATA_SET(&acc_data, unit, pport);

    SHR_IF_ERR_EXIT
        (bcmpc_pm_lport_info_get(unit, pport, NULL, &lport_info));
    if ((lane_idx != -1) && (lane_idx < shr_util_popcount(lport_info.lbmp))) {
        lane_index = lane_idx + (pport - pm_info.base_pport);
        SHR_IF_ERR_EXIT
            (bcmpc_phymod_phy_access_t_init(unit, pport, 1 << lane_index,
                                            &acc_data, pa));
    } else {
        SHR_IF_ERR_EXIT
            (bcmpc_phymod_phy_access_t_init(unit, pport, 0, &acc_data, pa));
    }

    phymod_prbs_status_t_init(&phy_status);
    PHY_IF_ERR_EXIT
        (phymod_phy_prbs_status_get(pa, flags, &phy_status));

    status->prbs_lock = phy_status.prbs_lock;
    status->prbs_lock_loss = phy_status.prbs_lock_loss;
    status->error_count = phy_status.error_count;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_phy_eyescan_set(int unit, bcmpc_pport_t pport, uint32_t flags,
                      bcmpc_phy_eyescan_mode_t *mode,
                      bcmpc_phy_eyescan_options_t *options)
{
    uint32_t speed = 0;
    phymod_phy_access_t phy_access, *pa = &phy_access;
    bcmpc_phy_access_data_t acc_data;
    phymod_eyescan_mode_t phy_eyescan_mode;
    phymod_phy_eyescan_options_t phy_eyescan_opt;


    SHR_FUNC_ENTER(unit);

    BCMPC_PHY_ACCESS_DATA_SET(&acc_data, unit, pport);
    SHR_IF_ERR_EXIT
         (bcmpc_phymod_phy_access_t_init(unit, pport, 0, &acc_data, pa));

    SHR_IF_ERR_EXIT
         (pc_eyescan_mode_to_phy(*mode, &phy_eyescan_mode));
    SHR_IF_ERR_EXIT
         (pc_port_speed_get(unit, pport, &speed));

    phymod_phy_eyescan_options_t_init(&phy_eyescan_opt);
    phy_eyescan_opt.timeout_in_milliseconds = options->sample_time;
    phy_eyescan_opt.horz_max = options->horz_max;
    phy_eyescan_opt.horz_min = options->horz_min;
    phy_eyescan_opt.hstep = options->hstep;
    phy_eyescan_opt.vert_max = options->vert_max;
    phy_eyescan_opt.vert_min = options->vert_min;
    phy_eyescan_opt.vstep = options->vstep;
    phy_eyescan_opt.ber_proj_scan_mode = options->ber_proj_scan_mode;
    phy_eyescan_opt.ber_proj_timer_cnt = options->ber_proj_timer_cnt;
    phy_eyescan_opt.ber_proj_err_cnt = options->ber_proj_err_cnt;
    phy_eyescan_opt.mode = 0;
    phy_eyescan_opt.linerate_in_khz = speed * 1000;

    PHY_IF_ERR_EXIT
        (phymod_phy_eyescan_run(pa, flags, phy_eyescan_mode, &phy_eyescan_opt));

exit:
    SHR_FUNC_EXIT();

}

int
bcmpc_phy_pmd_info_dump(int unit, bcmpc_pport_t pport, const char *type)
{
    phymod_phy_access_t phy_access, *pa = &phy_access;
    bcmpc_phy_access_data_t acc_data;

    SHR_FUNC_ENTER(unit);

    BCMPC_PHY_ACCESS_DATA_SET(&acc_data, unit, pport);
    SHR_IF_ERR_EXIT
         (bcmpc_phymod_phy_access_t_init(unit, pport, 0, &acc_data, pa));

    PHY_IF_ERR_EXIT
        (phymod_phy_pmd_info_dump(pa, type));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_phy_prbs_enable_set(int unit, bcmpc_pport_t pport, int lane_idx,
                          uint32_t flags, uint32_t enable)
{
    int lane_index = 0, pm_id = 0;
    uint32_t phy_flags = 0;
    phymod_phy_access_t phy_access, *pa = &phy_access;
    bcmpc_phy_access_data_t acc_data;
    bcmpc_pm_lport_info_t lport_info;
    bcmpc_topo_t pm_info;

    SHR_FUNC_ENTER(unit);

    BCMPC_PHY_ACCESS_DATA_SET(&acc_data, unit, pport);

    SHR_IF_ERR_VERBOSE_EXIT
         (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
         (bcmpc_topo_get(unit, pm_id, &pm_info));

    SHR_IF_ERR_EXIT
         (bcmpc_pm_lport_info_get(unit, pport, NULL, &lport_info));
    if ((lane_idx != -1) && (lane_idx < shr_util_popcount(lport_info.lbmp))) {
         lane_index = lane_idx + (pport - pm_info.base_pport);
        SHR_IF_ERR_EXIT
            (bcmpc_phymod_phy_access_t_init(unit, pport, 1 << lane_index,
                                            &acc_data, pa));
    } else {
        SHR_IF_ERR_EXIT
            (bcmpc_phymod_phy_access_t_init(unit, pport, 0, &acc_data, pa));
    }

    phy_flags = pc_prbs_cfg_flag_to_phy(flags);

    PHY_IF_ERR_EXIT
        (phymod_phy_prbs_enable_set(pa, phy_flags, enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_phy_ber_proj(int unit, bcmpc_pport_t pport,
                   bcmpc_phy_ber_proj_mode_t mode,
                   bcmpc_phy_ber_proj_options_t *proj_cfg)
{
    bcmpc_phy_access_data_t acc_data;
    phymod_phy_access_t phy_access, *pa = &phy_access;
    phymod_phy_ber_proj_options_t option;

    SHR_FUNC_ENTER(unit);

    phymod_phy_ber_proj_options_t_init(&option);
    BCMPC_PHY_ACCESS_DATA_SET(&acc_data, unit, pport);
    SHR_IF_ERR_EXIT
        (bcmpc_phymod_phy_access_t_init(unit, pport, 0, &acc_data, pa));

    option.ber_proj_hist_errcnt_thresh = proj_cfg->ber_proj_hist_errcnt_thresh;
    option.ber_proj_timeout_s = proj_cfg->ber_proj_timeout;
    option.ber_proj_phase = proj_cfg->ber_proj_phase;
    option.ber_proj_fec_size = proj_cfg->ber_proj_fec_size;
    option.ber_proj_prbs_errcnt =
                 (phymod_phy_ber_proj_errcnt_t*)proj_cfg->ber_proj_prbs_errcnt;

    if (mode == BCMPC_PHY_BERPROJ_MODE_POST_FEC) {
        PHY_IF_ERR_EXIT
            (phymod_phy_ber_proj(pa, phymodBERProjModePostFEC, &option));
    } else {
        return SHR_E_FAIL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_phy_port_port_cfg_get(int unit, bcmpc_pport_t pport,
                            bcmpc_cmd_port_cfg_t *port_cfg)
{
    bcmpc_pm_lport_info_t lport_info;
    bcmpc_pmgr_port_status_t pst;

    SHR_FUNC_ENTER(unit);

    bcmpc_pmgr_port_status_t_init(&pst);
    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_port_status_get(unit, pport, &pst));
    port_cfg->speed = pst.speed;
    port_cfg->fec_type = pst.fec_mode;
    SHR_IF_ERR_EXIT
         (bcmpc_pm_lport_info_get(unit, pport, NULL, &lport_info));
    port_cfg->lanes = shr_util_popcount(lport_info.lbmp);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_phy_fec_counter_get(int unit, bcmpc_pport_t pport,
                          bcmpc_cmd_fec_t fec_type,
                          uint32_t flags, uint32_t *value)
{
    bcmpc_phy_access_data_t acc_data;
    phymod_phy_access_t phy_access, *pa = &phy_access;

    SHR_FUNC_ENTER(unit);

    BCMPC_PHY_ACCESS_DATA_SET(&acc_data, unit, pport);
    SHR_IF_ERR_EXIT
        (bcmpc_phymod_phy_access_t_init(unit, pport, 0, &acc_data, pa));

    if (fec_type == BCMPC_CMD_PORT_FEC_RS_544 ||
        fec_type == BCMPC_CMD_PORT_FEC_RS_272 ||
        fec_type == BCMPC_CMD_PORT_FEC_RS_544_2XN ||
        fec_type == BCMPC_CMD_PORT_FEC_RS_272_2XN) {
        if (flags == BCMPC_PHY_FEC_CORRECTED_COUNTER) {
            PHY_IF_ERR_EXIT
                (phymod_phy_fec_cl91_correctable_counter_get(pa, value));
        } else {
            PHY_IF_ERR_EXIT
                (phymod_phy_fec_cl91_uncorrectable_counter_get(pa, value));
        }
    } else {
        if (BCMPC_PHY_FEC_CORRECTED_COUNTER) {
            PHY_IF_ERR_EXIT
                (phymod_phy_fec_correctable_counter_get(pa, value));
        } else {
            PHY_IF_ERR_EXIT
                (phymod_phy_fec_uncorrectable_counter_get(pa, value));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_phy_rsfec_sym_counter_get(int unit, bcmpc_pport_t pport,
                                bcmpc_phy_rsfec_symb_errcnt_t *sym_err_count)
{
    bcmpc_phy_access_data_t acc_data;
    phymod_phy_access_t phy_access, *pa = &phy_access;

    SHR_FUNC_ENTER(unit);

    BCMPC_PHY_ACCESS_DATA_SET(&acc_data, unit, pport);
    SHR_IF_ERR_EXIT
        (bcmpc_phymod_phy_access_t_init(unit, pport, 0, &acc_data, pa));

    PHY_IF_ERR_EXIT
       (phymod_phy_rsfec_symbol_error_counter_get(
        pa, sym_err_count->max_count, &(sym_err_count->actual_count),
        sym_err_count->symbol_errcnt)
       );

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_phy_linkcat(int unit, bcmpc_pport_t pport, int lane, int mode_val)
{
    phymod_phy_access_t phy_acc;
    bcmpc_phy_access_data_t acc_data;
    int mask, first_bit;
    int rv;

    SHR_FUNC_ENTER(unit);

    BCMPC_PHY_ACCESS_DATA_SET(&acc_data, unit, pport);
    SHR_IF_ERR_EXIT
        (bcmpc_phymod_phy_access_t_init(unit, pport, 0, &acc_data, &phy_acc));

    mask = 0x1;
    for (first_bit = 0; first_bit < 32; first_bit++) {
       if ( (phy_acc.access.lane_mask & mask) ) {
            phy_acc.access.lane_mask = 1 << ( first_bit + lane );
            break;
        }
        mask = mask << 1;
    }

    rv = phymod_phy_linkcat(&phy_acc, mode_val);
    if (rv == SHR_E_UNAVAIL) {
        cli_out("LinkCAT not supported for physical port %d\n", pport);
        return SHR_E_NONE;
    } else if (rv != SHR_E_NONE)  {
        cli_out("LinkCAT returns with error code %d\n", rv);
        return (rv);
    }

exit:
    SHR_FUNC_EXIT();
}
