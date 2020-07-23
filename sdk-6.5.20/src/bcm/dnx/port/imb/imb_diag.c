/** \file imb_diag.c
 *
 *  IMB related diagnostic procedures for DNX.
 */
/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/gtimer/gtimer.h>
#include <bcm_int/dnx/synce/synce.h>
#include <soc/portmod/portmod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_synce.h>
#include <soc/dnx/pll/pll.h>
#include <soc/dnxc/dnxc_port.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#define IMB_DIAG_GTIMER_NOF_CYCLES               (10240)

/**
 * internal SyncE configuration struct -
 * gathers all information about SycnE for cleaner interface
 * between functions
 */
typedef struct imb_diag_synce_config_s
{
    uint32 clk_sel;
    uint32 clk_div;
    uint32 squelch;
    uint32 link_valid_sel;
    uint32 gtimer_mode;
    uint32 clk_chain_sel;
} imb_diag_synce_config_t;

static int imb_diag_synce_config_set(
    int unit,
    bcm_core_t core,
    int synce_clk_blk_id,
    imb_diag_synce_config_t * synce);

static int imb_diag_synce_config_get(
    int unit,
    bcm_core_t core,
    int synce_clk_blk_id,
    imb_diag_synce_config_t * synce);

static int imb_diag_synce_counter_get(
    int unit,
    bcm_core_t core,
    uint32 *synce_cnt);

/**
 * \brief - configure the fabric statistics Gtimer settings.
 *        sub-task to be used during serdes rate measure procedure
 *
 * \param [in] unit - chip unit id
 * \param [in] fmac_blk_id - FMAC block ID
 * \param [in] gtimer - Gtimer config info
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * soc_dnxc_port_stat_gtimer_config_t
 */
static int
imb_diag_fabric_stat_gtimer_config_set(
    int unit,
    int fmac_blk_id,
    soc_dnxc_port_stat_gtimer_config_t * gtimer)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_STAT_GTIMER_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMAC_ID, fmac_blk_id);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GTIMER_START, INST_SINGLE, gtimer->enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GTIMER_PERIOD, INST_SINGLE, gtimer->nof_periods);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the fabric statistics Gtimer.
 *        sub-task to be used during serdes rate measure procedure
 *
 * \param [in] unit - chip unit id
 * \param [in] fmac_blk_id - FMAC block ID
 * \param [out] gtimer - Gtimer config info
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * soc_dnxc_port_stat_gtimer_config_t
 */
static int
imb_diag_fabric_stat_gtimer_config_get(
    int unit,
    int fmac_blk_id,
    soc_dnxc_port_stat_gtimer_config_t * gtimer)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_STAT_GTIMER_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMAC_ID, fmac_blk_id);
    /*
     * set value requests
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GTIMER_START, INST_SINGLE, &gtimer->enable);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GTIMER_PERIOD, INST_SINGLE, &gtimer->nof_periods);
    /*
     * commit request
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set SyncE configuration.sub-task to be used
 *        during serdes rate measure procedure
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] synce_clk_blk_id - SyncE clock block ID
 * \param [in] synce - SyncE config info
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * imb_diag_synce_config_t
 */
static int
imb_diag_synce_config_set(
    int unit,
    bcm_core_t core,
    int synce_clk_blk_id,
    imb_diag_synce_config_t * synce)
{
    uint32 entry_handle_id;
    uint32 chain_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Config SyncE divider Mode
     */
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_REF_CLK_DIV_NIF_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, 0);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLOCK_DIV_MODE, INST_SINGLE, synce->clk_div);
    /*
     * commit values
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Config SyncE Per Core Control
     */
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SYNCE_CORE_CTRL, entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, 0);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SQUELCH_ENABLE, INST_SINGLE, synce->squelch);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LINK_VALID_SEL, INST_SINGLE, synce->link_valid_sel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GTIMER_MODE, INST_SINGLE, synce->gtimer_mode);
    /*
     * commit values
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Config SyncE ref clock lane
     */
    /*
     * alloc DBAL table handle
     */
    table_id =
        (synce_clk_blk_id <
         dnx_data_nif.
         eth.cdu_nof_get(unit)) ? DBAL_TABLE_SYNCE_REF_CLK_NIF_CDU_LANE_SEL : DBAL_TABLE_SYNCE_REF_CLK_NIF_CLU_LANE_SEL;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_SYNCE_CLK_BLK, synce_clk_blk_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, 0);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLOCK_LANE_SEL, INST_SINGLE, synce->clk_sel);
    /*
     * commit values
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Enable NIF chain
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SYNCE_REF_CLK_NIF_CHAIN_ENABLE, entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_SYNCE_CLK_BLK, synce_clk_blk_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, 0);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, synce->clk_chain_sel);
    /*
     * commit values
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Configure NMG chain
     */
    if (dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_nmg_chain_select))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SYNCE_CORE_CTRL, entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, 0);

        chain_id = dnx_data_synce.general.nmg_chain_map_get(unit, synce_clk_blk_id)->chain_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NMG_CHAIN_SEL, INST_SINGLE, chain_id);
        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SyncE configuration. sub-task to be used during
 *        serdes rate measure procedure
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] synce_clk_blk_id - SyncE clock block ID
 * \param [out] synce - SyncE config info
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * imb_diag_synce_config_t
 */
static int
imb_diag_synce_config_get(
    int unit,
    bcm_core_t core,
    int synce_clk_blk_id,
    imb_diag_synce_config_t * synce)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get SyncE divider Mode
     */
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_REF_CLK_DIV_NIF_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, 0);
    /*
     * set value requests
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CLOCK_DIV_MODE, INST_SINGLE, &synce->clk_div);
    /*
     * commit request
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Get SyncE Per Core Control
     */
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SYNCE_CORE_CTRL, entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, 0);
    /*
     * commit request
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SQUELCH_ENABLE, INST_SINGLE, &synce->squelch);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LINK_VALID_SEL, INST_SINGLE, &synce->link_valid_sel);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GTIMER_MODE, INST_SINGLE, &synce->gtimer_mode);
    /*
     * commit request
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Get SyncE Per CDU Control - ref clock lane
     */
    table_id =
        (synce_clk_blk_id <
         dnx_data_nif.
         eth.cdu_nof_get(unit)) ? DBAL_TABLE_SYNCE_REF_CLK_NIF_CDU_LANE_SEL : DBAL_TABLE_SYNCE_REF_CLK_NIF_CLU_LANE_SEL;
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_SYNCE_CLK_BLK, synce_clk_blk_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, 0);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CLOCK_LANE_SEL, INST_SINGLE, &synce->clk_sel);
    /*
     * commit request
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SYNCE_REF_CLK_NIF_CHAIN_ENABLE, entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_SYNCE_CLK_BLK, synce_clk_blk_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, 0);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &synce->clk_chain_sel);
    /*
     * commit request
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SyncE counter value. sub-task to be used
 *        during serdes rate measure procedure
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [out] synce_cnt - counter value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_diag_synce_counter_get(
    int unit,
    bcm_core_t core,
    uint32 *synce_cnt)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_COUNTER, &entry_handle_id));
    /*
     * set key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, 0);
    /*
     * set value request
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUNTER, INST_SINGLE, synce_cnt);
    /*
     * commit the request
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Calculate the NIF and Fabric PLL according to
 *    the divider and reference clock.
 */
static shr_error_e
imb_diag_ref_clk_int_reminder_get(
    int unit,
    DNX_SERDES_REF_CLOCK_TYPE ref_clk,
    int *ref_clk_int,
    int *ref_clk_remainder)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (ref_clk)
    {
        case DNX_SERDES_REF_CLOCK_156_25:
            *ref_clk_int = 156;
            *ref_clk_remainder = 25;
            break;
        case DNX_SERDES_REF_CLOCK_312_5:
            *ref_clk_int = 312;
            *ref_clk_remainder = 50;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported Ref clock type %d!\n", ref_clk);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Calculate the NIF and Fabric PLL according to
 *    the divider and reference clock.
 */
static shr_error_e
imb_diag_phy_measure_pll_calc(
    int unit,
    DNX_SERDES_REF_CLOCK_TYPE ref_clk,
    uint32 n_div,
    uint32 p_div,
    uint32 m0_div,
    uint32 *ch0_out_int,
    uint32 *ch0_out_remainder)
{
    int ref_clk_int, ref_clk_remainder;

    SHR_FUNC_INIT_VARS(unit);

    if (p_div == 0 || m0_div == 0 || n_div == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Divider value is not allowed to 0! \n");
    }

    SHR_IF_ERR_EXIT(imb_diag_ref_clk_int_reminder_get(unit, ref_clk, &ref_clk_int, &ref_clk_remainder));
    /*
     * '2' is a HW constrait to reach up to 6.25G VCO
     */
    *ch0_out_int = (ref_clk_int / p_div * n_div) * 2 / m0_div;
    *ch0_out_remainder = (ref_clk_remainder / p_div * n_div) * 2 / m0_div;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Measure the clock for NIF PLL.
 */
static shr_error_e
imb_diag_nif_pll_measure_get(
    int unit,
    bcm_port_t port,
    uint32 *ch0_out_int,
    uint32 *ch0_out_remainder)
{
    DNX_SERDES_REF_CLOCK_TYPE ref_clk;
    uint32 p_div, n_div, m0_div;
    int pll_index;
    DNX_PLL3_TYPE pll_type;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Get PLL3 configuration values if supported
     * Currently only supported for JR2 devices.
     * For Q2A and J2C for example the decision was for the PLL values to be static and rate be manipulated only in other relevant PLLs
     */
    if (dnx_data_pll.pll3.nof_pll3_get(unit) > 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_pll_index_get(unit, port, &pll_index));

        pll_type = DNX_PLL3_TYPE_NIF0 + pll_index;
        ref_clk = dnx_data_pll.general.nif_pll_get(unit, pll_index)->in_freq;

        SHR_IF_ERR_EXIT(soc_dnx_pll_3_div_get(unit, pll_type, &n_div, &m0_div, &p_div));
        SHR_IF_ERR_EXIT(imb_diag_phy_measure_pll_calc
                        (unit, ref_clk, n_div, p_div, m0_div, ch0_out_int, ch0_out_remainder));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_diag_ref_clk_int_reminder_get
                        (unit, dnx_data_pll.general.nif_pll_get(unit, 0)->out_freq, (int *) ch0_out_int,
                         (int *) ch0_out_remainder));
    }

exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief - Measure the clock for Fabric PLL.
 */
static shr_error_e
imb_diag_fabric_pll_measure_get(
    int unit,
    bcm_port_t port,
    uint32 *ch0_out_int,
    uint32 *ch0_out_remainder)
{
    DNX_SERDES_REF_CLOCK_TYPE ref_clk;
    uint32 p_div, n_div, m0_div;
    int pll_index;
    DNX_PLL3_TYPE pll_type;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Get PLL3 configuration values if supported
     * Currently only supported for JR2 devices.
     * For Q2A and J2C for example the decision was for the PLL values to be static and rate be manipulated only in other relevant PLLs
     */
    if (dnx_data_pll.pll3.nof_pll3_get(unit) > 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_pll_index_get(unit, port, &pll_index));

        pll_type = DNX_PLL3_TYPE_MAS0 + pll_index;
        ref_clk = dnx_data_pll.general.fabric_pll_get(unit, pll_index)->in_freq;

        SHR_IF_ERR_EXIT(soc_dnx_pll_3_div_get(unit, pll_type, &n_div, &m0_div, &p_div));
        SHR_IF_ERR_EXIT(imb_diag_phy_measure_pll_calc
                        (unit, ref_clk, n_div, p_div, m0_div, ch0_out_int, ch0_out_remainder));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_diag_ref_clk_int_reminder_get
                        (unit, dnx_data_pll.general.fabric_pll_get(unit, 0)->out_freq, (int *) ch0_out_int,
                         (int *) ch0_out_remainder));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See imb_diag.h
 */
int
imb_diag_nif_phy_measure_get(
    int unit,
    bcm_port_t port,
    int is_rx,
    soc_dnxc_port_phy_measure_t * phy_measure)
{
    int first_phy_port, synce_state_read = 0, synce_config_read = 0;
    int clock_speed_int, clock_speed_remainder;
    int tot_time_measured_int, tot_time_measured_remainder;
    int one_bit_clk_period_int, one_bit_clk_period_remainder;
    int serdes_freq_int_temp, serdes_freq_remainder_temp, map_size;
    int synce_enable_default, speed, lane_speed, nof_phys, is_pam4;
    int nif_synce_div, pm_synce_div;
    dnx_algo_port_ethu_access_info_t ethu_info;
    imb_diag_synce_config_t synce_default, synce_config;
    uint32 core_clock, swapped_lane, synce_cnt, vco_div, os_int, os_remainder;
    soc_dnxc_lane_map_db_map_t lane2serdes[DNX_DATA_MAX_NIF_PHYS_NOF_PHYS];
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    phy_measure->lane = first_phy_port;
    /*
     * Get swapped lane, only relevant for Ethernet ports
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, TRUE) || DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
    {
        map_size = dnx_data_nif.phys.nof_phys_get(unit);
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_lane_to_serdes_map_get
                        (unit, DNX_ALGO_LANE_MAP_NIF_SIDE, map_size, lane2serdes));
        swapped_lane = lane2serdes[first_phy_port].rx_id;
    }
    else
    {
        swapped_lane = first_phy_port;
    }
    /*
     * Save syncE configuration to restore at end of function
     */
    SHR_IF_ERR_EXIT(dnx_nif_synce_enable_get(unit, ethu_info.core, &synce_enable_default));
    synce_state_read = 1;
    /*
     * Enable syncE
     */
    SHR_IF_ERR_EXIT(dnx_nif_synce_enable_set(unit, ethu_info.core, 1));
    /*
     * Save syncE configuration to restore at end of function
     */
    SHR_IF_ERR_EXIT(imb_diag_synce_config_get(unit, ethu_info.core, ethu_info.ethu_id, &synce_default));
    synce_config_read = 1;

    /*
     * Config syncE
     */
    synce_config.clk_sel = swapped_lane - ethu_info.first_lane_in_ethu;
    synce_config.clk_div = 1;
    synce_config.squelch = 1;
    synce_config.gtimer_mode = 1;
    synce_config.link_valid_sel = 1;
    synce_config.clk_chain_sel = 1;
    SHR_IF_ERR_EXIT(imb_diag_synce_config_set(unit, ethu_info.core, ethu_info.ethu_id, &synce_config));
    /*
     * Enable ECI Gtimer
     */
    SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, IMB_DIAG_GTIMER_NOF_CYCLES, SOC_BLK_NMG, ethu_info.core));
    /*
     * Wait Gtimer until it reset
     */
    SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_NMG, ethu_info.core));
    core_clock = dnx_data_device.general.core_clock_khz_get(unit);
    clock_speed_int = (1000000 / core_clock);
    /*
     * devided by 100 instead of 10000 for better resolution
     */
    clock_speed_remainder = ((100000000 / (core_clock / 100))) % 10000;
    tot_time_measured_int = (((clock_speed_int * 10000) + clock_speed_remainder) * IMB_DIAG_GTIMER_NOF_CYCLES) / 10000;
    tot_time_measured_remainder =
        ((int) (((clock_speed_int * 10000) + clock_speed_remainder) * (IMB_DIAG_GTIMER_NOF_CYCLES)) % 10000);
    /*
     * Read syncE counter
     */
    SHR_IF_ERR_EXIT(imb_diag_synce_counter_get(unit, ethu_info.core, &synce_cnt));
    /*
     * Get NIF and PM synce divider
     */
    SHR_IF_ERR_EXIT(dnx_nif_synce_ref_clk_div_get(unit, port, &pm_synce_div, &nif_synce_div));
    /*
     * get VCO divider
     */
    vco_div = dnx_data_nif.phys.vco_div_get(unit, ethu_info.ethu_id)->vco_div;
    /*
     * Get serdes_freq from tot_time_measured by removing all the dividers between them
     */
    if (0 == synce_cnt)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Sync Eth counter is 0! \n");
    }
    phy_measure->one_clk_time_measured_int =
        (((tot_time_measured_int * 10000) + (tot_time_measured_remainder)) / (synce_cnt * vco_div)) / 10000;
    phy_measure->one_clk_time_measured_remainder =
        ((((tot_time_measured_int * 10000) + (tot_time_measured_remainder)) / (synce_cnt * vco_div)) % 10000);

    one_bit_clk_period_int =
        ((phy_measure->one_clk_time_measured_int * 10000 +
          phy_measure->one_clk_time_measured_remainder) / nif_synce_div / pm_synce_div) / 10000;
    one_bit_clk_period_remainder =
        (((phy_measure->one_clk_time_measured_int * 10000 +
           phy_measure->one_clk_time_measured_remainder) / nif_synce_div / pm_synce_div) % 10000);
    if (0 == (one_bit_clk_period_int * 10000 + one_bit_clk_period_remainder))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "One bit clk period is 0! \n");
    }

    serdes_freq_int_temp = (10000 / (one_bit_clk_period_int * 10000 + one_bit_clk_period_remainder));
    serdes_freq_remainder_temp = (10000000 / (one_bit_clk_period_int * 10000 + one_bit_clk_period_remainder)) % 1000;

    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, 0, &speed));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
    {
        lane_speed = speed;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_phys));
        lane_speed = speed / nof_phys;
    }
    /*
     * The HW is connected in a way that the oversample affects the SyncE->SerDesRate
     * calculation only on TSCF SGMII/GMII. Otherwise - we should ignore it !
     */
    if (ethu_info.imb_type == imbDispatchTypeImb_clu && lane_speed < 5000)
    {
        /*
         * Get oversample
         */
        SHR_IF_ERR_EXIT(imb_port_over_sampling_get(unit, port, &os_int, &os_remainder));

    }
    else
    {
        os_int = 1;
        os_remainder = 0;
    }
    /*
     * Calc serdes freq
     */
    phy_measure->serdes_freq_int =
        ((((serdes_freq_int_temp * 1000) + serdes_freq_remainder_temp) * 1000) / ((os_int * 1000) +
                                                                                  os_remainder)) / 1000;
    phy_measure->serdes_freq_remainder =
        ((((serdes_freq_int_temp * 1000) + serdes_freq_remainder_temp) * 1000) / ((os_int * 1000) +
                                                                                  os_remainder)) % 1000;

    /*
     * Check if the encoding mode is PAM4
     */
    is_pam4 = DNXC_PORT_PHY_SPEED_IS_PAM4(lane_speed);
    /*
     * Need to double the serdes rate in case of PAM4
     */
    if (is_pam4)
    {
        phy_measure->serdes_freq_int =
            (phy_measure->serdes_freq_int << 1) + (phy_measure->serdes_freq_remainder << 1) / 1000;
        phy_measure->serdes_freq_remainder = (phy_measure->serdes_freq_remainder << 1) % 1000;
    }
    /*
     * Get the NIF PLL output clk
     */
    SHR_IF_ERR_EXIT(imb_diag_nif_pll_measure_get
                    (unit, port, &phy_measure->ref_clk_int, &phy_measure->ref_clk_remainder));
exit:
    if (unit < SOC_MAX_NUM_DEVICES)
    {
        if ((_func_rv = dnx_gtimer_clear(unit, SOC_BLK_NMG, ethu_info.core)) < 0)
        {
            cli_out("write to ECI_GTIMER_CONFIGURATIONr failed\n");
        }
        if (synce_state_read)
        {
            if ((_func_rv = dnx_nif_synce_enable_set(unit, ethu_info.core, synce_enable_default)) < 0)
            {
                cli_out("Write to ECI_GP_CONTROL_9r failed\n");
            }
        }
        if (synce_config_read)
        {
            if ((_func_rv = imb_diag_synce_config_set(unit, ethu_info.core, ethu_info.ethu_id, &synce_default)) < 0)
            {
                cli_out("Failed to recover default synce configuration!\n");
            }
        }
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - provide the fabric PHY measurment
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] is_rx - indicate the FIFO used to measure
 * \param [out] phy_measure - phy measure output
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_diag_fabric_phy_measure_get(
    int unit,
    bcm_port_t port,
    int is_rx,
    soc_dnxc_port_phy_measure_t * phy_measure)
{
    int fabric_mac_bus_size = 0, counter_disabled = 0, clock_speed_int = 0, clock_speed_remainder = 0;
    uint64 rate_start, rate_end;
    uint32 rate;
    bcm_stat_val_t counter_type;
    bcm_port_resource_t resource;
    int first_phy = 0;
    int fmac_blk_id;
    int override_gtimer_config = FALSE, rv;
    dnx_algo_port_info_s port_info;
    soc_dnxc_port_stat_gtimer_config_t gtimer_default, gtimer_config;
    bcm_pbmp_t phys;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, port, &resource));
    /*
     * Get the first phy info and FMAC bus size for
     *ILKN OVER FABRIC and FABRIC ports
     */
    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        fabric_mac_bus_size = dnx_data_fabric.general.fmac_bus_size_get(unit, resource.fec_type)->size;
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, port, &first_phy));
    }
    else
    {
        fabric_mac_bus_size = dnx_data_nif.ilkn.fmac_bus_size_get(unit);
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
        _SHR_PBMP_FIRST(phys, first_phy);
    }
    phy_measure->lane = first_phy;

    /*
     * Get default GTIMER value
     */
    fmac_blk_id = first_phy / dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
    SHR_IF_ERR_EXIT(imb_diag_fabric_stat_gtimer_config_get(unit, fmac_blk_id, &gtimer_default));
    /*
     * Reset Gtimer
     */
    gtimer_config.enable = 0;
    gtimer_config.nof_periods = DNXC_PORT_STAT_GTIMER_NOF_CYCLES;
    SHR_IF_ERR_EXIT(imb_diag_fabric_stat_gtimer_config_set(unit, fmac_blk_id, &gtimer_config));
    override_gtimer_config = TRUE;
    /*
     * clear stats
     */
    rv = bcm_dnx_stat_sync(unit);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_DISABLED);
    /*
     * if counter thread is disabled, no need to sync
     */
    if (rv == _SHR_E_DISABLED)
    {
        counter_disabled = 1;
    }
    counter_type = is_rx ? snmpBcmRxAsynFifoRate : snmpBcmTxAsynFifoRate;
    SHR_IF_ERR_EXIT(bcm_dnx_stat_get(unit, port, counter_type, &rate_start));
    /*
     * start gtimer stats
     */
    gtimer_config.enable = 1;
    SHR_IF_ERR_EXIT(imb_diag_fabric_stat_gtimer_config_set(unit, fmac_blk_id, &gtimer_config));
    sal_usleep(10);
    /*
     * getting rate
     */
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_dnx_stat_sync(unit), BCM_E_DISABLED);
    SHR_IF_ERR_EXIT(bcm_dnx_stat_get(unit, port, counter_type, &rate_end));
    /*
     * If counter thread is enabled, subtract collected rate_end from
     * collected rate_start.
     * If counter thread is disabled, rate_start clears counter, and therefore rate_end is correct rate
     */
    if (!counter_disabled)
    {
        COMPILER_64_SUB_64(rate_end, rate_start);
    }
    rate = COMPILER_64_LO(rate_end);
    /*
     * Calculate the serdes rate value
     */
    clock_speed_int = 1000000 / dnx_data_fabric.general.fmac_clock_khz_get(unit);
    clock_speed_remainder = (1000000 / (dnx_data_fabric.general.fmac_clock_khz_get(unit) / 1000)) % 1000;

    if (((DNXC_PORT_STAT_GTIMER_NOF_CYCLES / 1000) * (clock_speed_int * 1000 + (clock_speed_remainder % 1000))) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Bad calculation value! \n");
    }
    phy_measure->serdes_freq_int =
        (rate * fabric_mac_bus_size * 1000) / (DNXC_PORT_STAT_GTIMER_NOF_CYCLES *
                                               (clock_speed_int * 1000 + (clock_speed_remainder % 1000)));
    phy_measure->serdes_freq_remainder =
        ((rate * fabric_mac_bus_size * 1000) /
         ((DNXC_PORT_STAT_GTIMER_NOF_CYCLES / 1000) * (clock_speed_int * 1000 + (clock_speed_remainder % 1000)))) %
        1000;
    /*
     * Get the Farbic PLL output clk
     */
    SHR_IF_ERR_EXIT(imb_diag_fabric_pll_measure_get
                    (unit, port, &phy_measure->ref_clk_int, &phy_measure->ref_clk_remainder));

exit:
    /*
     * Restore original Gtimer configuration
     */
    if (override_gtimer_config)
    {
        if ((_func_rv = imb_diag_fabric_stat_gtimer_config_set(unit, fmac_blk_id, &gtimer_default)) < 0)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U
                       (unit,
                        "Error in imb_diag_fabric_gtimer_config_set: Failed to recover the default Gtimer configuration!\n")));
        }
    }
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
