/** \file bcm/dnx/synce/synce.c
 *
 * Functions for handling SyncE init sequences.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SYNCEDNX

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_synce.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/synce/algo_synce.h>
#include <bcm_int/dnx/port/imb/imb_common.h>

#include <bcm/error.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/synce/synce.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_synce.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/pll/pll.h>

#include <soc/portmod/portmod.h>
#include <phymod/phymod_acc.h>
#include <phymod/phymod.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#define DNX_INVALID_SYNCE_SRC_PORT  -1

/*
 * }
 */

/*
 * MACROs
 * {
 */
#define DNX_NOF_LINKS_IN_FSRD   (dnx_data_fabric.blocks.nof_links_in_fsrd_get(unit))
#define DNX_NOF_LINKS_PER_CORE  (dnx_data_fabric.links.nof_links_get(unit) /  \
                                     dnx_data_device.general.nof_cores_get(unit))
#define DNX_NOF_FSRD_PER_CORE    (dnx_data_fabric.blocks.nof_instances_fsrd_get(unit) / \
                                      dnx_data_device.general.nof_cores_get(unit))
#define DNX_NIF_SYNCE_VALID_LANE_SWAP {3, 2, 1, 0, 7, 6, 5, 4}
#define DNX_STAGE0_SDM_FRAC_DIV_MODE    2
/*
 * }
 */

/**
 * \brief
 *    Get the Synce configuration from DNX DATA,
 *    and configure them using BCM API.
 * \param [in] unit - The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_synce_init(
    int unit)
{
    int synce_index;
    int nof_synce_plls;
    const dnx_data_synce_general_cfg_t *synce_cfg;
    bcm_switch_control_t src_clk_type, div_type;

    SHR_FUNC_INIT_VARS(unit);

    nof_synce_plls = dnx_data_synce.general.nof_plls_get(unit);

    for (synce_index = 0; synce_index < nof_synce_plls; synce_index++)
    {
        synce_cfg = dnx_data_synce.general.cfg_get(unit, synce_index);

        if (synce_cfg->source_clock_port != DNX_INVALID_SYNCE_SRC_PORT)
        {
            if (synce_index == DBAL_ENUM_FVAL_SYNCE_INDEX_MASTER_SYNCE)
            {
                src_clk_type = bcmSwitchSynchronousPortClockSource;
                div_type = bcmSwitchSynchronousPortClockSourceDivCtrl;
            }
            else
            {
                src_clk_type = bcmSwitchSynchronousPortClockSourceBkup;
                div_type = bcmSwitchSynchronousPortClockSourceBkupDivCtrl;
            }
            SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, src_clk_type, synce_cfg->source_clock_port));
            SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, div_type, synce_cfg->output_clock_sel));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Deinit SyncE.
 * \param [in] unit - The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_synce_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Currently nothing to deinit
     */
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set general config for NIF FABRIC synce.
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   SyncE index.
 * \param [in] port -
 *   Logical port ID.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
dnx_synce_nif_fab_config_set(
    int unit,
    int synce_index,
    bcm_port_t port)
{
    int core_id, fabric_link;
    dnx_algo_port_info_s port_info;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist) ||
        dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_NIF_FAB_CTRL, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, synce_index);

        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
        {
            /*
             * Select the SyncE is for Fabric or NIF
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_NIF_FAB_SEL, INST_SINGLE,
                                         !DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info));
            if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, port, &fabric_link));
                /*
                 * Specify the fabric link
                 */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_FAB_LINK_SEL, INST_SINGLE,
                                             fabric_link);
            }
        }
        if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core)
            && (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, FALSE, TRUE)
                || DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info)))
        {
            /*
             * For multi core device, select the NIF core
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_NIF_CORE_SEL, INST_SINGLE, core_id);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the Fabric SyncE link ID.
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   SyncE index.
 * \param [out] fabric_link -
 *   Fabric link number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
dnx_fabric_synce_link_get(
    int unit,
    int synce_index,
    uint32 *fabric_link)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_NIF_FAB_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, synce_index);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SYNCE_FAB_LINK_SEL, INST_SINGLE, fabric_link);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get NIF or Fabric synce
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   SyncE index.
 * \param [in] is_nif -
 *   Is nif SyncE
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

static int
dnx_synce_nif_fabric_sel_get(
    int unit,
    int synce_index,
    uint32 *is_nif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_NIF_FAB_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, synce_index);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SYNCE_NIF_FAB_SEL, INST_SINGLE, is_nif);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the Fabric link ID in FDR block.
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   SyncE index.
 * \param [in] fsrd_block -
 *   Fabric Serdes Block ID
 * \param [in] fdr_link -
 *   Fabric link number in FDR block.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

static int
dnx_fabric_synce_fdr_link_sel_set(
    int unit,
    int synce_index,
    int fsrd_block,
    int fdr_link)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FDR_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, fsrd_block / DNX_NOF_FSRD_PER_CORE);

    if (synce_index == DBAL_ENUM_FVAL_SYNCE_INDEX_MASTER_SYNCE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MASTER_CLK_SEL, INST_SINGLE, fdr_link);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SLAVE_CLK_SEL, INST_SINGLE, fdr_link);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Set the FMAC ID for fabric synce
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   SyncE index.
 * \param [in] fsrd_block -
 *   Fabric Serdes Block ID
 * \param [in] fmac_id -
 *   FMAC id
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

static int
dnx_fabric_synce_fmac_sel_set(
    int unit,
    int synce_index,
    int fsrd_block,
    int fmac_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FMAC_SEL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, fsrd_block / DNX_NOF_FSRD_PER_CORE);

    if (synce_index == DBAL_ENUM_FVAL_SYNCE_INDEX_MASTER_SYNCE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MASTER_FMAC_INSTANCE, INST_SINGLE, fmac_id);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SLAVE_FMAC_INSTANCE, INST_SINGLE, fmac_id);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Set the Fabric link in each Fabric Serdes block
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   SyncE index.
 * \param [in] fsrd_block -
 *   Fabric Serdes block ID
 * \param [in] fsrd_link -
 *   Fabric link ID in each Fabric Serdes
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

static int
dnx_fabric_synce_fsrd_link_sel_set(
    int unit,
    int synce_index,
    int fsrd_block,
    int fsrd_link)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FSRD_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_SYNCE_CLK_BLK, fsrd_block);

    if (synce_index == DBAL_ENUM_FVAL_SYNCE_INDEX_MASTER_SYNCE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MASTER_CLK_SEL, INST_SINGLE, fsrd_link);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SLAVE_CLK_SEL, INST_SINGLE, fsrd_link);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the Fabric SyncE reference clock link
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   SyncE index.
 * \param [in] port -
 *   Source link for Fabric SyncE
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

static int
dnx_fabric_synce_link_set(
    int unit,
    int synce_index,
    bcm_port_t port)
{
    int fsrd_block, fsrd_link;
    int fdr_link, fabric_link, fmac_id;
    uint32 dummy = 1;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, port, &fabric_link));

    fsrd_block = fabric_link / DNX_NOF_LINKS_IN_FSRD;
    fmac_id = fsrd_block % DNX_NOF_FSRD_PER_CORE;
    fsrd_link = fabric_link % DNX_NOF_LINKS_IN_FSRD;
    fdr_link = fabric_link % DNX_NOF_LINKS_PER_CORE;

    SHR_IF_ERR_EXIT(dnx_fabric_synce_fmac_sel_set(unit, synce_index, fsrd_block, fmac_id));
    SHR_IF_ERR_EXIT(dnx_fabric_synce_fsrd_link_sel_set(unit, synce_index, fsrd_block, fsrd_link));
    SHR_IF_ERR_EXIT(dnx_fabric_synce_fdr_link_sel_set(unit, synce_index, fsrd_block, fdr_link));
    SHR_IF_ERR_EXIT(soc_dnx_synce_pll_set(unit, synce_index, dummy, dummy, dummy, dummy, TRUE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the fabric SyncE divider
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   SyncE index.
 * \param [out] divider -
 *   Fabric SyncE output clock divider
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

static int
dnx_fabric_synce_div_get(
    int unit,
    int synce_index,
    int *divider)
{
    uint32 entry_handle_id;
    uint32 phase1_divider, fabric_link;
    int fsrd_block;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_synce_link_get(unit, synce_index, &fabric_link));
    fsrd_block = fabric_link / DNX_NOF_LINKS_IN_FSRD;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FSRD_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_SYNCE_CLK_BLK, fsrd_block);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DIV_PHASE_1, INST_SINGLE, &phase1_divider);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *divider = phase1_divider + 1;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify function for soc_dnx_fabric_synce_div_set
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   SyncE index.
 * \param [in] divider -
 *   Divider for Fabric SyncE output clock
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

static int
dnx_fabric_synce_div_set_verfiy(
    int unit,
    int synce_index,
    int divider)
{

    int synce_enabled, fsrd_block, synce_enable_to_check;
    int divider_to_check, fsrd_block_to_check;
    uint32 fabric_link;

    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(divider, dnx_data_synce.general.fabric_div_min_get(unit),
                     dnx_data_synce.general.fabric_div_max_get(unit), _SHR_E_PARAM, "SyncE divider is out of bound!\n");
    /*
     * If master synce is enabled, need to check if slave synce is enabled, and vice versa.
     * Issue error in case the user wants a) different dividers and
     * b) the master and slave are from the same BH.
     */

    SHR_IF_ERR_EXIT(dnx_fabric_synce_link_get(unit, synce_index, &fabric_link));
    fsrd_block = fabric_link / DNX_NOF_LINKS_IN_FSRD;

    synce_enable_to_check =
        (synce_index ==
         DBAL_ENUM_FVAL_SYNCE_INDEX_MASTER_SYNCE) ? DBAL_ENUM_FVAL_SYNCE_INDEX_SLAVE_SYNCE :
        DBAL_ENUM_FVAL_SYNCE_INDEX_MASTER_SYNCE;
    SHR_IF_ERR_EXIT(dnx_algo_fabric_synce_enable_get(unit, synce_enable_to_check, &synce_enabled));
    if (synce_enabled)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_synce_link_get(unit, synce_enable_to_check, &fabric_link));
        fsrd_block_to_check = fabric_link / DNX_NOF_LINKS_IN_FSRD;
        SHR_IF_ERR_EXIT(dnx_fabric_synce_div_get(unit, synce_enable_to_check, &divider_to_check));
        if ((fsrd_block == fsrd_block_to_check) && (divider != divider_to_check))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "SyncE master source clock port and slave source clock port in same FSRD, the divider should be same!\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the Fabric Synce PLL divider
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   SyncE index.
 * \param [in] divider -
 *   Divider for Fabric SyncE output clock
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

static int
dnx_fabric_synce_div_set(
    int unit,
    int synce_index,
    int divider)
{
    uint32 entry_handle_id;
    uint32 phase1_divider = divider - 1, fabric_link;
    int fsrd_block;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_synce_link_get(unit, synce_index, &fabric_link));
    fsrd_block = fabric_link / DNX_NOF_LINKS_IN_FSRD;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FSRD_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_SYNCE_CLK_BLK, fsrd_block);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DIV_PHASE_0, INST_SINGLE, phase1_divider / 2);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DIV_PHASE_1, INST_SINGLE, phase1_divider);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Select the reference clock lane number
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   Master or Slave SyncE.
 * \param [in] port -
 *   Logical Port number.
 * \param [in] enable -
 *   enable or disable.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
dnx_nif_synce_ref_clk_chain_enable_set(
    int unit,
    uint32 synce_index,
    bcm_port_t port,
    int enable)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_REF_CLK_NIF_CHAIN_ENABLE, &entry_handle_id));
    /*
     * clear the orignal chain settings
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, synce_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_NIF_SYNCE_CLK_BLK, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 0);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * set key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_SYNCE_CLK_BLK, ethu_info.ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, synce_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Select the reference clock lane number
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   Master or Slave SyncE.
 * \param [in] port -
 *   Logical Port number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
dnx_nif_synce_ref_clk_lane_sel_set(
    int unit,
    uint32 synce_index,
    bcm_port_t port)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    int first_phy, phy_lane, map_size, swapped_lane, valid_lane;
    int valid_lane_swap[] = DNX_NIF_SYNCE_VALID_LANE_SWAP;
    soc_dnxc_lane_map_db_map_t lane2serdes[DNX_DATA_MAX_NIF_PHYS_NOF_PHYS];
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * Get the first phy
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));
    /*
     * Get Lane Map
     */
    map_size = dnx_data_nif.phys.nof_phys_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_lane_to_serdes_map_get(unit, DNX_ALGO_LANE_MAP_NIF_SIDE, map_size, lane2serdes));
    /*
     * Get pcs lane number and pmd lane number
     */
    phy_lane = first_phy - ethu_info.first_lane_in_ethu;
    swapped_lane = lane2serdes[first_phy].rx_id - ethu_info.first_lane_in_ethu;
    /*
     * Get the DBAL table
     */
    table_id =
        (ethu_info.imb_type ==
         imbDispatchTypeImb_clu) ? DBAL_TABLE_SYNCE_REF_CLK_NIF_CLU_LANE_SEL :
        DBAL_TABLE_SYNCE_REF_CLK_NIF_CDU_LANE_SEL;
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    /*
     * clear the orignal lane settings
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, synce_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_NIF_SYNCE_CLK_BLK, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLOCK_LANE_SEL, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID_LANE_SEL, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LINK_STATUS_LANE_SEL, INST_SINGLE, 0);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * set key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_SYNCE_CLK_BLK, ethu_info.ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, synce_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLOCK_LANE_SEL, INST_SINGLE, swapped_lane);
    valid_lane =
        dnx_data_synce.general.feature_get(unit,
                                           dnx_data_synce_general_synce_no_sdm_mode) ? valid_lane_swap[phy_lane] :
        phy_lane;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID_LANE_SEL, INST_SINGLE, valid_lane);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LINK_STATUS_LANE_SEL, INST_SINGLE, swapped_lane);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Select the NMG Chain for CLU and CDU
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   Master or Slave SyncE.
 * \param [in] port -
 *   Logical Port number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
dnx_nif_synce_nmg_chain_sel_set(
    int unit,
    uint32 synce_index,
    bcm_port_t port)
{
    int core_id;
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    uint32 chain_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_CORE_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, synce_index);

    chain_id = dnx_data_synce.general.nmg_chain_map_get(unit, ethu_info.ethu_id)->chain_id;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NMG_CHAIN_SEL, INST_SINGLE, chain_id);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable output clock, this is for PAD configuration.
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   Master or Slave SyncE.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
dnx_synce_output_clk_enable(
    int unit,
    int synce_index)
{
    uint32 entry_handle_id;
    dbal_fields_e field;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_OUTPUT_CLK_MODE_CTRL, &entry_handle_id));
    /*
     * set value fields
     */
    field = synce_index ? DBAL_FIELD_CLK1_OUTPUT_ENABLE : DBAL_FIELD_CLK0_OUTPUT_ENABLE;
    dbal_entry_value_field32_set(unit, entry_handle_id, field, INST_SINGLE, 0);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Get the reference clock divider in PM and NIF
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   Logical Port number.
 * \param [out] pm_synce_div -
 *   PM synce divider
 * \param [out] nif_synce_div -
 *   NIF synce divider
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

int
dnx_nif_synce_ref_clk_div_get(
    int unit,
    bcm_port_t port,
    int *pm_synce_div,
    int *nif_synce_div)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    phymod_synce_clk_ctrl_t synce_cfg;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    /*
     * Internal PHY - get phy access
     */
    params.phyn = 0;
    params.lane = -1;
    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get
                    (unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phy_access, &nof_phys, NULL));
    SHR_IF_ERR_EXIT(phymod_phy_synce_clk_ctrl_get(phy_access, &synce_cfg));
    {
        if (synce_cfg.stg1_mode == DNX_NIF_SYNCE_PM_DIV_11)
        {
            *pm_synce_div = 11;
            *nif_synce_div = 1;
        }
        else if (synce_cfg.stg1_mode == DNX_NIF_SYNCE_PM_DIV_1)
        {
            *pm_synce_div = 1;
            *nif_synce_div = 11;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "SYNCE: unsupported PM divider mode %d\n", synce_cfg.stg1_mode);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the reference clock divider in NIF module
 * \param [in] unit -
 *   The unit number.
 * \param [in] core -
 *   core ID
 * \param [in] synce_index -
 *   Master or Slave SyncE
 * \param [in] port -
 *   Logical Port number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

static int
dnx_nif_synce_ref_clk_nif_div_set(
    int unit,
    int core,
    int synce_index,
    bcm_port_t port)
{
    uint32 entry_handle_id;
    int nif_synce_div = 1;
    int pm_synce_div = 1;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the clock block ID and phy lane number
     */
    if (dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_no_sdm_mode))
    {
        SHR_IF_ERR_EXIT(dnx_nif_synce_ref_clk_div_get(unit, port, &pm_synce_div, &nif_synce_div));
    }

    /*
     * Config the NIF divider
     */
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_REF_CLK_DIV_NIF_CTRL, &entry_handle_id));

    /*
     * set key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, synce_index);

    /*
     * set value field
     */
    if (nif_synce_div != 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLOCK_DIV_MODE, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DIV_PHASE_ZERO, INST_SINGLE,
                                     ((nif_synce_div + 1) / 2) - 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DIV_PHASE_ONE, INST_SINGLE,
                                     (nif_synce_div / 2) - 1);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLOCK_DIV_MODE, INST_SINGLE, 1);
    }

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Enable the SyncE function
 * \param [in] unit -
 *   The unit number.
 * \param [in] core -
 *   core id
 * \param [in] enable -
 *   Enable or Disable
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

int
dnx_nif_synce_enable_set(
    int unit,
    int core,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /**
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_ENABLE_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /**
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE,
                                 enable ? DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET);

    /**
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the SyncE enable status
 * \param [in] unit -
 *   The unit number.
 * \param [in] core -
 *   core id
 * \param [out] enable -
 *   Enable or Disable
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
int
dnx_nif_synce_enable_get(
    int unit,
    int core,
    int *enable)
{
    uint32 reset;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /**
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_ENABLE_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /**
     * set value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE, &reset);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *enable = (reset == DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET) ? 1 : 0;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable SyncE automatic squelch mode
 * \param [in] unit -
 *   The unit number.
 * \param [in] core -
 *   core ID
 * \param [in] synce_index -
 *   Master or Slave SyncE
 * \param [in] enable -
 *   Enable or Disable
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

static int
dnx_nif_synce_squelch_enable_set(
    int unit,
    int core,
    int synce_index,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_CORE_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, synce_index);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SQUELCH_ENABLE, INST_SINGLE, enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}
/**
 * \brief
 *   Get the divider for SyncE pll
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   Logical Port number.
 * \param [in] ref_clock_out -
 *   output clock frequency
 * \param [out] pdiv -
 *   pdiv for synce pll.
 * \param [out] mdiv -
 *   mdiv for synce pll.
 * \param [out] ndiv -
 *   ndiv for synce pll
 * \param [out] fref -
 *   The effective reference frequency.
 * \param [out] is_bypass -
 *   If bypass the PLL.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

static int
dnx_nif_synce_pll_div_get(
    int unit,
    bcm_port_t port,
    soc_dnxc_init_serdes_ref_clock_t ref_clock_out,
    uint32 *pdiv,
    uint32 *mdiv,
    uint32 *ndiv,
    uint32 *fref,
    int *is_bypass)
{
    portmod_speed_config_t speed_config;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_no_sdm_mode))
    {
        *pdiv = 1;
        *mdiv = 1;
        *ndiv = 1;
        *fref = 1;
        *is_bypass = 1;
    }
    else
    {
        *is_bypass = 0;
        SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &speed_config));
        {
            if ((speed_config.speed == 400000 || speed_config.speed == 200000 || speed_config.speed == 100000
                 || speed_config.speed == 50000) && (speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_272
                                                     || speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_544
                                                     || speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_544_2XN))
            {
                /*
                 * 26.5625G VCO
                 */
                *pdiv = 10;
                *fref = 20;
                if (soc_dnxc_init_serdes_ref_clock_156_25 == ref_clock_out)
                {
                    *mdiv = 17;
                    *ndiv = 220;
                }
                else if (soc_dnxc_init_serdes_ref_clock_125 == ref_clock_out)
                {
                    *mdiv = 17;
                    *ndiv = 176;
                }
                else if (soc_dnxc_init_serdes_ref_clock_25 == ref_clock_out)
                {
                    *mdiv = 85;
                    *ndiv = 176;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported output clock %d\n", ref_clock_out);
                }
            }
            else if (((speed_config.speed == 200000 || speed_config.speed == 100000 || speed_config.speed == 50000
                       || speed_config.speed == 25000) && (speed_config.fec == PORTMOD_PORT_PHY_FEC_NONE
                                                           || speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_FEC))
                     || (speed_config.speed == 250000 && speed_config.fec == PORTMOD_PORT_PHY_FEC_BASE_R))
            {
                /*
                 * 25.781G VCO
                 */
                *pdiv = 6;
                *ndiv = 160;
                *fref = 20;
                if (soc_dnxc_init_serdes_ref_clock_156_25 == ref_clock_out)
                {
                    *mdiv = 20;
                }
                else if (soc_dnxc_init_serdes_ref_clock_125 == ref_clock_out)
                {
                    *mdiv = 25;
                }
                else if (soc_dnxc_init_serdes_ref_clock_25 == ref_clock_out)
                {
                    *mdiv = 125;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported output clock %d\n", ref_clock_out);
                }
            }
            else if ((speed_config.speed == 40000 && speed_config.num_lane == 2) &&
                     (speed_config.fec == PORTMOD_PORT_PHY_FEC_NONE || speed_config.fec == PORTMOD_PORT_PHY_FEC_BASE_R))
            {
                /*
                 * 20.625G VCO
                 */
                *pdiv = 6;
                *ndiv = 200;
                *fref = 16;
                if (soc_dnxc_init_serdes_ref_clock_156_25 == ref_clock_out)
                {
                    *mdiv = 20;
                }
                else if (soc_dnxc_init_serdes_ref_clock_125 == ref_clock_out)
                {
                    *mdiv = 25;
                }
                else if (soc_dnxc_init_serdes_ref_clock_25 == ref_clock_out)
                {
                    *mdiv = 125;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported output clock %d\n", ref_clock_out);
                }
            }
            else if (speed_config.speed == 10000 || (speed_config.speed == 40000 && speed_config.num_lane == 4))
            {
                /*
                 * 20.625G VCO, 10.3125G Serdes Rate
                 */
                *pdiv = 3;
                *ndiv = 200;
                *fref = 16;
                if (soc_dnxc_init_serdes_ref_clock_156_25 == ref_clock_out)
                {
                    *mdiv = 20;
                }
                else if (soc_dnxc_init_serdes_ref_clock_125 == ref_clock_out)
                {
                    *mdiv = 25;
                }
                else if (soc_dnxc_init_serdes_ref_clock_25 == ref_clock_out)
                {
                    *mdiv = 125;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported output clock %d\n", ref_clock_out);
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported speed %d and FEC type %d combination\n", speed_config.speed,
                             speed_config.fec);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  NIF SyncE configuration function.
 *  Used to configure SyncE pll or Nif logic
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   Master or slave synce
 * \param [in] port -
 *   Logical port number
 * \param [in] ref_clock_out -
 *   output clock frequency
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

static int
dnx_nif_synce_config_set(
    int unit,
    int synce_index,
    bcm_port_t port,
    soc_dnxc_init_serdes_ref_clock_t ref_clock_out)
{
    uint32 ndiv = 0, mdiv = 0, pdiv = 0, fref = 0;
    int core_id, is_bypass = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));

    if (dnx_data_pll.synce_pll.feature_get(unit, dnx_data_pll_synce_pll_present))
    {
        SHR_IF_ERR_EXIT(dnx_nif_synce_pll_div_get(unit, port, ref_clock_out, &pdiv, &mdiv, &ndiv, &fref, &is_bypass));
    }
    /*
     * Select the lane in CDU and CLU
     */
    SHR_IF_ERR_EXIT(dnx_nif_synce_ref_clk_lane_sel_set(unit, synce_index, port));
    /*
     * Enable the chain in CDU and CLU
     */
    SHR_IF_ERR_EXIT(dnx_nif_synce_ref_clk_chain_enable_set(unit, synce_index, port, 1));
    /*
     * Select NMG chain
     */
    if (dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_nmg_chain_select))
    {
        SHR_IF_ERR_EXIT(dnx_nif_synce_nmg_chain_sel_set(unit, synce_index, port));
    }
    SHR_IF_ERR_EXIT(dnx_nif_synce_ref_clk_nif_div_set(unit, core_id, synce_index, port));
    /*
     * Enable synce
     */
    SHR_IF_ERR_EXIT(dnx_nif_synce_enable_set(unit, core_id, 1));
    /*
     * Disable Auto-Squelch
     */
    SHR_IF_ERR_EXIT(dnx_nif_synce_squelch_enable_set(unit, core_id, synce_index, 0));
    /*
     * Configure SyncE PLL
     */
    if (dnx_data_pll.synce_pll.feature_get(unit, dnx_data_pll_synce_pll_present))
    {
        SHR_IF_ERR_EXIT(soc_dnx_synce_pll_set(unit, synce_index, ndiv, mdiv, pdiv, fref, is_bypass));
    }
    if (dnx_data_synce.general.cfg_get(unit, synce_index)->squelch_enable)
    {
        SHR_IF_ERR_EXIT(dnx_nif_synce_squelch_enable_set(unit, core_id, synce_index, 1));
    }
    SHR_IF_ERR_EXIT(dnx_synce_output_clk_enable(unit, synce_index));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the Synce divider
 *   0: 125Mhz
 *   1: 156.25Mhz
 *   2: 25Mhz
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   Master or slave synce
 * \param [out] clk_div -
 *   clock divider, also refer to the output clk
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
dnx_nif_synce_clk_div_get(
    int unit,
    uint32 synce_index,
    soc_dnxc_init_serdes_ref_clock_t * clk_div)
{
    uint32 entry_handle_id;
    uint32 mdiv, ndiv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_PLL_CFG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, synce_index);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SYNCE_PLL_CH0_MDIV, INST_SINGLE, &mdiv);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SYNCE_PLL_NDIV_INT, INST_SINGLE, &ndiv);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    switch (mdiv)
    {
            /*
             * the initial vlaue
             */
        case 0:
            /*
             * bypass mode value
             */
        case 1:
            *clk_div = soc_dnxc_init_serdes_ref_clock_25;
            break;
        case 20:
            *clk_div = soc_dnxc_init_serdes_ref_clock_156_25;
            break;
        case 17:
        {
            {
                if (ndiv == 176)
                {
                    *clk_div = soc_dnxc_init_serdes_ref_clock_125;
                }
                else if (ndiv == 220)
                {
                    *clk_div = soc_dnxc_init_serdes_ref_clock_156_25;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "SYNCE: unsupported NDIV %d\n", ndiv);
                }
            }
            break;
        }
        case 25:
            *clk_div = soc_dnxc_init_serdes_ref_clock_125;
            break;
        case 85:
        case 125:
            *clk_div = soc_dnxc_init_serdes_ref_clock_25;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_CONFIG, "SYNCE: unsupported MDIV %d\n", mdiv);
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Set the Synce source port
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   Master or slave synce
 * \param [in] port -
 *   Logical Port number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
dnx_nif_synce_clk_port_sel_set(
    int unit,
    uint32 synce_index,
    bcm_port_t port)
{
    soc_dnxc_init_serdes_ref_clock_t clk_div;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_no_sdm_mode))
    {
        /*
         * For no SDM mode, need to pass the divider together with the clock port
         * to ensure the PLL lock
         */
        SHR_IF_ERR_EXIT(dnx_nif_synce_clk_div_get(unit, synce_index, &clk_div));
    }
    else
    {
        /*
         * For SDM mode, no need to consider the divider when configuring the
         * clock port
         */
        clk_div = -1;
    }
    SHR_IF_ERR_EXIT(dnx_nif_synce_config_set(unit, synce_index, port, clk_div));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the Synce source port
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   Master or slave synce
 * \param [out] port -
 *   Logical Port number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
dnx_nif_synce_clk_port_sel_get(
    int unit,
    uint32 synce_index,
    bcm_port_t * port)
{
    uint32 entry_handle_id;
    uint32 synce_clk_blk_id = 0, chain_enable = 0, synce_idx_tmp;
    uint32 clock_lane = 0, inner_clock_lane = 0;
    int is_end, phy_lane, core_id, ethu_id_in_core, first_phy, nof_lanes;
    soc_dnxc_lane_map_db_map_t serdes2lane[DNX_DATA_MAX_NIF_PHYS_NOF_PHYS];
    int map_size = dnx_data_nif.phys.nof_phys_get(unit);
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get the synce_clk_blk_id by checking which NIF chain is enabled
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_REF_CLK_NIF_CHAIN_ENABLE, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &chain_enable));

        if (chain_enable != 1)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, &synce_idx_tmp));
            if (synce_idx_tmp == synce_index)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_NIF_SYNCE_CLK_BLK, &synce_clk_blk_id));
                break;
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            }
        }
    }
    if (chain_enable == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "SyncE is not configured! \n\n");
    }
    /*
     * Get DBAL table ID
     */
    table_id =
        (synce_clk_blk_id <
         dnx_data_nif.
         eth.cdu_nof_get(unit)) ? DBAL_TABLE_SYNCE_REF_CLK_NIF_CDU_LANE_SEL : DBAL_TABLE_SYNCE_REF_CLK_NIF_CLU_LANE_SEL;
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_SYNCE_CLK_BLK, synce_clk_blk_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, synce_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CLOCK_LANE_SEL, INST_SINGLE, &inner_clock_lane);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Get the port info
     */
    core_id = synce_clk_blk_id >= dnx_data_nif.eth.ethu_nof_per_core_get(unit) ? 1 : 0;
    ethu_id_in_core = synce_clk_blk_id % dnx_data_nif.eth.ethu_nof_per_core_get(unit);
    /*
     * Get the first phy of the ETHU
     */
    SHR_IF_ERR_EXIT(dnx_algo_ethu_id_lane_info_get(unit, core_id, ethu_id_in_core, &first_phy, &nof_lanes));
    /*
     * Get serdes to lane map
     */
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_serdes_to_lane_map_get(unit, DNX_ALGO_LANE_MAP_NIF_SIDE, map_size, serdes2lane));
    clock_lane = first_phy + inner_clock_lane;
    phy_lane = serdes2lane[clock_lane].rx_id;
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, phy_lane, 0, 0, port));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Set the Synce divider
 *   0: 125Mhz
 *   1: 156.25Mhz
 *   2: 25Mhz
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   Master or slave synce
 * \param [in] clk_div -
 *   clock divider, also refer to the output clk
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
dnx_nif_synce_clk_div_set(
    int unit,
    uint32 synce_index,
    soc_dnxc_init_serdes_ref_clock_t clk_div)
{
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_nif_synce_clk_port_sel_get(unit, synce_index, &port));
    SHR_IF_ERR_EXIT(dnx_nif_synce_config_set(unit, synce_index, port, clk_div));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   get the SyncE clock configurations in SDM mode of TSCBH
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   Source link for SyncE
 * \param [in] divider -
 *   synce divider
 * \param [out] config -
 *   SyncE SDM configurations
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
dnx_synce_tscbh_sdm_config_get(
    int unit,
    bcm_port_t port,
    uint32 divider,
    portmod_port_synce_clk_ctrl_t * config)
{
    portmod_speed_config_t speed_config;

    SHR_FUNC_INIT_VARS(unit);

    if (soc_dnxc_init_serdes_ref_clock_25 != divider)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported output clock divider %d\n", divider);
    }

    SHR_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &speed_config));
    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &speed_config));
    {
        /*
         * SDM = ((VCO / OS * 20) * 256) / divider (25)
         */
        if ((speed_config.speed == 400000 || speed_config.speed == 200000 || speed_config.speed == 100000
             || speed_config.speed == 50000) && (speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_272
                                                 || speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_544
                                                 || speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_544_2XN))
        {
            /*
             * VCO: 26.5625G, OS: 1
             */
            config->sdm_val = 13600;
        }
        else if (((speed_config.speed == 200000 || speed_config.speed == 100000 || speed_config.speed == 50000
                   || speed_config.speed == 25000) && (speed_config.fec == PORTMOD_PORT_PHY_FEC_NONE
                                                       || speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_FEC))
                 || (speed_config.speed == 25000 && speed_config.fec == PORTMOD_PORT_PHY_FEC_BASE_R))
        {
            /*
             * VCO: 25.78125G, OS: 1
             */
            config->sdm_val = 13200;
        }
        else if ((speed_config.speed == 40000 || speed_config.speed == 20000) &&
                 (speed_config.fec == PORTMOD_PORT_PHY_FEC_NONE || speed_config.fec == PORTMOD_PORT_PHY_FEC_BASE_R))
        {
            /*
             * VCO: 20.625G, OS: 1
             */
            config->sdm_val = 10560;
        }
        else if (speed_config.speed == 10000)
        {
            /*
             * VCO: 20.625G, OS: 2
             */
            config->sdm_val = 5280;
        }
        else if (speed_config.speed == 12000)
        {
            /*
             * VCO: 25G, OS: 2
             */
            config->sdm_val = 6400;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported speed %d\n", speed_config.speed);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   get the SyncE clock configurations in SDM mode of TSCBH Fabric
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   Source link for SyncE
 * \param [in] divider -
 *   synce divider
 * \param [out] config -
 *   SyncE SDM configurations
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
dnx_synce_tscbh_fabric_sdm_config_get(
    int unit,
    bcm_port_t port,
    uint32 divider,
    portmod_port_synce_clk_ctrl_t * config)
{
    portmod_speed_config_t speed_config;

    SHR_FUNC_INIT_VARS(unit);

    if (soc_dnxc_init_serdes_ref_clock_25 != divider)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported output clock divider %d\n", divider);
    }

    SHR_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &speed_config));
    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &speed_config));

    /*
     * SDM = ((VCO / OS * 20) * 256) / divider (25)
     */
    switch (speed_config.speed)
    {
        case 10312:
            /*
             * VCO: 20.625G, OS: 2
             */
            config->sdm_val = 5280;
            break;

        case 20625:
            /*
             * VCO: 20.625G, OS: 1
             */
            config->sdm_val = 10560;
            break;

        case 41875:
            /*
             * VCO: 20.9375G, OS: 1, PAM4
             */
            config->sdm_val = 10720;
            break;

        case 45000:
            /*
             * VCO: 22.5G, OS: 1, PAM4
             */
            config->sdm_val = 11520;
            break;

        case 11500:
            /*
             * VCO: 23G, OS: 2
             */
            config->sdm_val = 5888;
            break;

        case 23000:
            /*
             * VCO: 23G, OS: 1
             */
            config->sdm_val = 11776;
            break;

        case 47500:
            /*
             * VCO: 23.75G, OS: 1, PAM4
             */
            config->sdm_val = 12160;
            break;

        case 25000:
        case 50000:
            /*
             * VCO: 25G, OS: 1, and PAM4
             */
            config->sdm_val = 12800;
            break;

        case 25781:
        case 51562:
            /*
             * VCO: 25.78125G, OS: 1, and PAM4
             */
            config->sdm_val = 13200;
            break;

        case 53125:
            /*
             * VCO: 26.5625G, OS: 1, PAM4
             */
            config->sdm_val = 13600;
            break;

        case 28125:
        case 56250:
            /*
             * VCO: 28.125G, OS: 1, and PAM4
             */
            config->sdm_val = 14400;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported speed %d\n", speed_config.speed);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   get the SyncE clock configurations in SDM mode of TSCF
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   Source link for SyncE
 * \param [in] divider -
 *   synce divider
 * \param [out] config -
 *   SyncE SDM configurations
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
dnx_synce_tscf_sdm_config_get(
    int unit,
    bcm_port_t port,
    uint32 divider,
    portmod_port_synce_clk_ctrl_t * config)
{
    int nof_phys;
    uint32 os_int, os_remainder;
    uint32_t pll_div;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    DNX_SERDES_REF_CLOCK_TYPE ref_clk = DNX_SERDES_NOF_REF_CLOCKS;

    SHR_FUNC_INIT_VARS(unit);

    if (soc_dnxc_init_serdes_ref_clock_25 != divider)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported output clock divider %d\n", divider);
    }

    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));

    params.phyn = 0;
    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &phy_access, &nof_phys, NULL));
    SHR_IF_ERR_EXIT(phymod_phy_pll_multiplier_get(&phy_access, &pll_div));
    SHR_IF_ERR_EXIT(imb_common_port_over_sampling_get(unit, port, &os_int, &os_remainder));

    ref_clk = dnx_data_pll.general.nif_pll_get(unit, phy_access.access.pll_idx)->out_freq;
    if (ref_clk == DNX_SERDES_REF_CLOCK_BYPASS)
    {
        ref_clk = dnx_data_pll.general.nif_pll_get(unit, phy_access.access.pll_idx)->in_freq;
    }

    /*
     * SDM = ((VCO / OS * 40) * 256) / divider (25)
     */
    switch (ref_clk)
    {
        case DNX_SERDES_REF_CLOCK_125:
            switch (pll_div)
            {
                case phymod_TSCF_PLL_DIV165:
                case 165:
                {
                    /*
                     * VCO = 125 * 165 = 20.625 G
                     */
                    if (os_int == 2 && os_remainder == 0)
                    {
                        config->sdm_val = 2640;
                    }
                    else if (os_int == 1 && os_remainder == 0)
                    {
                        config->sdm_val = 5280;
                    }
                    else if (os_int == 16 && os_remainder == 500)
                    {
                        config->sdm_val = 320;
                    }
                    else if (os_int == 4 && os_remainder == 0)
                    {
                        config->sdm_val = 1320;
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported VCO: 20.625G and OS: %d combination\n", os_int);
                    }
                }
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported ref_clock %d and pll divider %d combination\n",
                                 ref_clk, pll_div);
                    break;
            }
            break;

        case DNX_SERDES_REF_CLOCK_156_25:
            switch (pll_div)
            {
                case phymod_TSCF_PLL_DIV132:
                case 132:
                {
                    /*
                     * VCO = 156.25 * 132 = 20.625 G
                     */
                    if (os_int == 2 && os_remainder == 0)
                    {
                        config->sdm_val = 2640;
                    }
                    else if (os_int == 1 && os_remainder == 0)
                    {
                        config->sdm_val = 5280;
                    }
                    else if (os_int == 16 && os_remainder == 500)
                    {
                        config->sdm_val = 320;
                    }
                    else if (os_int == 4 && os_remainder == 0)
                    {
                        config->sdm_val = 1320;
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported VCO: 20.625G and OS: %d combination\n", os_int);
                    }
                }
                    break;

                case phymod_TSCF_PLL_DIV165:
                case 165:
                {
                    /*
                     * VCO = 156.25 * 165 = 25.78125 G
                     */
                    if (os_int == 2 && os_remainder == 0)
                    {
                        config->sdm_val = 3300;
                    }
                    else if (os_int == 1 && os_remainder == 0)
                    {
                        config->sdm_val = 6600;
                    }
                    else if (os_int == 2 && os_remainder == 500)
                    {
                        config->sdm_val = 2640;
                    }
                    else if (os_int == 20 && os_remainder == 625)
                    {
                        config->sdm_val = 320;
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported VCO: 25.78125G and OS: %d combination\n", os_int);
                    }
                }
                    break;
                case phymod_TSCF_PLL_DIV160:
                case 160:
                {
                    /*
                     * VCO = 156.25 * 160 = 25G
                     */
                    if (os_int == 2 && os_remainder == 0)
                    {
                        config->sdm_val = 3200;
                    }
                    else if (os_int == 1 && os_remainder == 0)
                    {
                        config->sdm_val = 6400;
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported VCO: 25G and OS: %d combination\n", os_int);
                    }
                }
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported ref_clock %d and pll divider %d combination\n",
                                 ref_clk, pll_div);
                    break;
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ref clk %d\n", ref_clk);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   get the SyncE clock configurations in SDM mode
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   Source link for SyncE
 * \param [in] divider -
 *   synce divider
 * \param [out] config -
 *   SyncE SDM configurations
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
dnx_synce_sdm_config_get(
    int unit,
    bcm_port_t port,
    uint32 divider,
    portmod_port_synce_clk_ctrl_t * config)
{
    int real_port = 0;
    portmod_dispatch_type_t pm_type = portmodDispatchTypeCount;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_pm_type_get(unit, port, &real_port, &pm_type));

    switch (pm_type)
    {
        case portmodDispatchTypePm8x50:
            SHR_IF_ERR_EXIT(dnx_synce_tscbh_sdm_config_get(unit, port, divider, config));
            break;

        case portmodDispatchTypePm8x50_fabric:
            SHR_IF_ERR_EXIT(dnx_synce_tscbh_fabric_sdm_config_get(unit, port, divider, config));
            break;

        case portmodDispatchTypePm4x25:
            SHR_IF_ERR_EXIT(dnx_synce_tscf_sdm_config_get(unit, port, divider, config));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported PortMod type %d\n", pm_type);
            break;
    }

    config->stg0_mode = DNX_STAGE0_SDM_FRAC_DIV_MODE;

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Set the Synce divider
 *   2: 25Mhz
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   Master or slave synce
 * \param [in] divider -
 *   Divider for SyncE output clock
 * \param [in] is_nif_synce
 *   Indicates source port is NIF or Fabric
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
dnx_synce_clk_div_set(
    int unit,
    uint32 synce_index,
    int divider,
    uint32 is_nif_synce)
{
    bcm_port_t port;
    portmod_port_synce_clk_ctrl_t synce_clk_config;

    SHR_FUNC_INIT_VARS(unit);

    if (is_nif_synce)
    {
        SHR_RANGE_VERIFY(divider, dnx_data_synce.general.nif_div_min_get(unit),
                         dnx_data_synce.general.nif_div_max_get(unit), _SHR_E_PARAM,
                         "SyncE divider is out of bound!\n");
    }
    else
    {
        SHR_RANGE_VERIFY(divider, dnx_data_synce.general.fabric_div_min_get(unit),
                         dnx_data_synce.general.fabric_div_max_get(unit), _SHR_E_PARAM,
                         "SyncE divider is out of bound!\n");
    }
    SHR_IF_ERR_EXIT(dnx_synce_port_get(unit, synce_index, &port));
    portmod_port_synce_clk_ctrl_t_init(unit, &synce_clk_config);
    SHR_IF_ERR_EXIT(dnx_synce_sdm_config_get(unit, port, divider, &synce_clk_config));
    SHR_IF_ERR_EXIT(portmod_port_synce_clk_ctrl_set(unit, port, &synce_clk_config));

    if (is_nif_synce == 0)
    {
        /** Mark fabric synce is enabled */
        SHR_IF_ERR_EXIT(dnx_algo_fabric_synce_enable_set(unit, synce_index, 1));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Set SyncE source port for Fabric or NIF SyncE
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   Master or slave synce
 * \param [in] port -
 *   logical port number
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
int
dnx_synce_port_set(
    int unit,
    uint32 synce_index,
    bcm_port_t port)
{
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_synce_nif_fab_config_set(unit, synce_index, port));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_fabric_synce_link_set(unit, synce_index, port));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, FALSE, TRUE) ||
             DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_nif_synce_clk_port_sel_set(unit, synce_index, port));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported port type!\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get SyncE source port for Fabric or NIF SyncE
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   Master or slave synce
 * \param [out] port -
 *   logical port number
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
int
dnx_synce_port_get(
    int unit,
    uint32 synce_index,
    bcm_port_t * port)
{
    uint32 is_nif_synce = 1, fabric_link;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_IF_ERR_EXIT(dnx_synce_nif_fabric_sel_get(unit, synce_index, &is_nif_synce));
    }

    if (is_nif_synce)
    {
        SHR_IF_ERR_EXIT(dnx_nif_synce_clk_port_sel_get(unit, synce_index, port));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_fabric_synce_link_get(unit, synce_index, &fabric_link));
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_to_logical_get(unit, fabric_link, port));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set SyncE divider for Fabric or NIF SyncE
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   Master or slave synce
 * \param [in] divider -
 *   synce divider
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
int
dnx_synce_divider_set(
    int unit,
    uint32 synce_index,
    int divider)
{
    uint32 is_nif_synce = 1;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_IF_ERR_EXIT(dnx_synce_nif_fabric_sel_get(unit, synce_index, &is_nif_synce));
    }

    if (dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_no_sdm_mode))
    {
        if (is_nif_synce)
        {
            SHR_RANGE_VERIFY(divider, dnx_data_synce.general.nif_div_min_get(unit),
                             dnx_data_synce.general.nif_div_max_get(unit), _SHR_E_PARAM,
                             "SyncE divider is out of bound!\n");
            SHR_IF_ERR_EXIT(dnx_nif_synce_clk_div_set(unit, synce_index, (soc_dnxc_init_serdes_ref_clock_t) divider));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_fabric_synce_div_set_verfiy(unit, synce_index, divider));
            SHR_IF_ERR_EXIT(dnx_fabric_synce_div_set(unit, synce_index, divider));
            /** Mark fabric synce is enabled */
            SHR_IF_ERR_EXIT(dnx_algo_fabric_synce_enable_set(unit, synce_index, 1));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_synce_clk_div_set(unit, synce_index, divider, is_nif_synce));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get SyncE divider for Fabric or NIF SyncE
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   Master or slave synce
 * \param [out] divider -
 *   synce divider
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
int
dnx_synce_divider_get(
    int unit,
    uint32 synce_index,
    int *divider)
{
    uint32 is_nif_synce = 1;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_no_sdm_mode))
    {
        if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
        {
            SHR_IF_ERR_EXIT(dnx_synce_nif_fabric_sel_get(unit, synce_index, &is_nif_synce));
        }
        if (is_nif_synce)
        {
            SHR_IF_ERR_EXIT(dnx_nif_synce_clk_div_get(unit, synce_index, (soc_dnxc_init_serdes_ref_clock_t *) divider));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_fabric_synce_div_get(unit, synce_index, divider));
        }
    }
    else
    {
        *divider = soc_dnxc_init_serdes_ref_clock_25;
    }
exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
