/** \file tdm.c
 * 
 *
 * TDM procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TDM

/*
 * Include
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/shrextend/shrextend_error.h>

#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/tdm.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>

#include <bcm_int/dnx/tdm/tdm.h>


#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/drv.h>
#include <soc/cmicx.h>
#include <soc/mem.h>
#include <soc/dnx/swstate/auto_generated/access/tdm_access.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/*
 * Defines
 * {
 */

/*
 * }
 */

/*
* Configure CS valid bits based on whether TDM includes FTMH and whether it's optimized
*/
static shr_error_e
dnx_tdm_update_cs(
    int unit,
    uint8 *is_ftmh,
    uint8 *is_optimized)
{
    int ii = 0, jj = 0, field_0 = 0;
    uint32 is_ftmh_unset_hw_context_id[3][2] = { {DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_MC_CUD_ON_OTMH, 0},
    {DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_PORT_VAL_ON_OTMH, 0},
    {DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_NOTHING_ON_OTMH, 0}
    };
    uint32 is_ftmh_set_is_opt_unset_hw_context_id[3][2] =
        { {DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_MC_CUD_ON_FTMH, 0},
    {DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_PORT_VAL_ON_FTMH, 0},
    {DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_NOTHING_ON_FTMH, 0}
    };
    uint32 is_ftmh_set_is_opt_set_hw_context_id[3][2] =
        { {DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_MC_CUD_ON_OPT_FTMH, 0},
    {DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_PORT_VAL_ON_OPT_FTMH, 0},
    {DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_NOTHING_ON_OPT_FTMH, 0}
    };
    uint32 is_ftmh_unset_enable = 0;
    uint32 is_ftmh_set_is_opt_unset_enable = 0;
    uint32 is_ftmh_set_is_opt_set_enable = 0;
    uint8 sw_is_ftmh, sw_is_optimized;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    for (ii = 0; ii < 3; ii++)
    {
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_TRAP_CONTEXT_ID,
                                                      is_ftmh_unset_hw_context_id[ii][0],
                                                      &is_ftmh_unset_hw_context_id[ii][1]));
    }
    for (ii = 0; ii < 3; ii++)
    {
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_TRAP_CONTEXT_ID,
                                                      is_ftmh_set_is_opt_unset_hw_context_id[ii][0],
                                                      &is_ftmh_set_is_opt_unset_hw_context_id[ii][1]));
    }
    for (ii = 0; ii < 3; ii++)
    {
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_TRAP_CONTEXT_ID,
                                                      is_ftmh_set_is_opt_set_hw_context_id[ii][0],
                                                      &is_ftmh_set_is_opt_set_hw_context_id[ii][1]));
    }

    if (is_ftmh)
    {
        tdm_ftmh_info.tdm_header_is_ftmh.set(unit, *is_ftmh);
        sw_is_ftmh = *is_ftmh;
    }
    else
    {
        tdm_ftmh_info.tdm_header_is_ftmh.get(unit, &sw_is_ftmh);
    }
    if (is_optimized)
    {
        tdm_ftmh_info.use_optimized_ftmh.set(unit, *is_optimized);
        sw_is_optimized = *is_optimized;
    }
    else
    {
        tdm_ftmh_info.use_optimized_ftmh.get(unit, &sw_is_optimized);
    }
    is_ftmh_unset_enable = (sw_is_ftmh == 0);
    is_ftmh_set_is_opt_unset_enable = ((sw_is_ftmh == 1) && (sw_is_optimized == 0));
    is_ftmh_set_is_opt_set_enable = ((sw_is_ftmh == 1) && (sw_is_optimized == 1));

    for (ii = 0; ii < 48; ii++)
    {
        uint32 data[SOC_REG_ABOVE_64_MAX_SIZE_U32] = { 0 };
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, -1, ii, data));
        field_0 = soc_mem_field32_get(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, data, PROGRAMf);
        for (jj = 0; jj < 3; jj++)
        {
            if (((field_0 >> 1) == is_ftmh_unset_hw_context_id[jj][1]) ||
                ((field_0 >> 1) == is_ftmh_set_is_opt_unset_hw_context_id[jj][1]) ||
                ((field_0 >> 1) == is_ftmh_set_is_opt_set_hw_context_id[jj][1]))
            {
                int sys_mc;
                int new_program = 1;
                sys_mc = soc_mem_field32_get(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, data, SYS_MCf);
                if (sys_mc)
                {
                    int sys_mc_entry_is_valid;
                    if (is_ftmh_unset_enable)
                    {
                        sys_mc_entry_is_valid = 1;
                    }
                    else
                    {
                        sys_mc_entry_is_valid = 0;
                    }
                    soc_mem_field32_set(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, data, VALIDf, sys_mc_entry_is_valid);
                    SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, -1, ii, data));
                    break;
                }

                if (is_ftmh_unset_enable)
                {
                    new_program |= ((is_ftmh_unset_hw_context_id[jj][1]) << 1);
                }
                else if (is_ftmh_set_is_opt_unset_enable)
                {
                    new_program |= ((is_ftmh_set_is_opt_unset_hw_context_id[jj][1]) << 1);
                }
                else if (is_ftmh_set_is_opt_set_enable)
                {
                    new_program |= ((is_ftmh_set_is_opt_set_hw_context_id[jj][1]) << 1);
                }
                soc_mem_field32_set(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, data, PROGRAMf, new_program);
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, -1, ii, data));
                break;
            }
        }
    }

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_out_header_type_set(
    int unit,
    int switch_tdm_header_type)
{
    uint8 tdm_header_is_ftmh;
    /*
     * The following variable holds the value of OTMH_4B_WITH_CUD_FORMAT field in PEMLA_OTMHFORMATS DBAL Table
     */
    uint32 tdm_header_is_otmh_4byte_with_cud;
    int tdm_mode;

    SHR_FUNC_INIT_VARS(unit);
    switch (switch_tdm_header_type)
    {
        case BCM_TDM_CONTROL_OUT_HEADER_OTMH:
        {
            tdm_header_is_ftmh = 0;
            tdm_header_is_otmh_4byte_with_cud = 0;
            break;
        }
        case BCM_TDM_CONTROL_OUT_HEADER_FTMH:
        {
            tdm_header_is_ftmh = 1;
            tdm_header_is_otmh_4byte_with_cud = 0;
            break;
        }
        case BCM_TDM_CONTROL_OUT_HEADER_OTMH_4B_WITH_CUD:
        {
            tdm_header_is_ftmh = 1;
            tdm_header_is_otmh_4byte_with_cud = 1;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Switch header type %d is not supported!", switch_tdm_header_type);
            break;
        }
    }

    tdm_mode = dnx_data_tdm.params.mode_get(unit);

    /*
     * "OTMH 4Byte with CUD" outgoing TDM header is supported only when tdm_mode==TDM_MODE_OPTIMIZED
     */
    if ((tdm_header_is_otmh_4byte_with_cud == 1) && (tdm_mode != TDM_MODE_OPTIMIZED))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "OTMH-4Byte-With-CUD Outgoing TDM Header is supported only when tdm_mode=TDM_MODE_OPTIMIZED !");
    }

    
    SHR_IF_ERR_EXIT(dnx_tdm_update_cs(unit, &tdm_header_is_ftmh, NULL));
    /*
     * if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
     * {
     *     SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_PEMLA_TDM, 0, 1, GEN_DBAL_FIELD32,
     *     DBAL_FIELD_TDM_HEADER_IS_FTMH, INST_SINGLE, tdm_header_is_ftmh, GEN_DBAL_FIELD_LAST_MARK));
     * }
     */

    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_PEMLA_OTMHFORMATS, 0, 1, GEN_DBAL_FIELD32 /* type */ , DBAL_FIELD_OTMH_4B_WITH_CUD_FORMAT /* Field
                                                                                                                                                 * * * Id 
                                      */ , INST_SINGLE /* instance */ ,
                                     tdm_header_is_otmh_4byte_with_cud /* value */ ,
                                     GEN_DBAL_FIELD_LAST_MARK));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_out_header_type_get(
    int unit,
    int *switch_tdm_header_type)
{

    uint8 tdm_header_is_ftmh = 0;
    /*
     * The following variable holds the value of OTMH_4B_WITH_CUD_FORMAT field in PEMLA_OTMHFORMATS DBAL Table
     * The initialization is not mandatoty. Good practice.
     */
    uint32 tdm_header_is_otmh_4b_with_cud_format = 0;

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_OTMHFORMATS, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OTMH_4B_WITH_CUD_FORMAT, INST_SINGLE,
                               &tdm_header_is_otmh_4b_with_cud_format);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    
    /*
     * if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
     * {
     *     SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_TDM, entry_handle_id));
     *     dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TDM_HEADER_IS_FTMH, INST_SINGLE,
     *     &tdm_header_is_ftmh); SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
     * }
     */
    tdm_ftmh_info.tdm_header_is_ftmh.get(unit, &tdm_header_is_ftmh);

    switch (tdm_header_is_ftmh)
    {

        case 0:
        {
            *switch_tdm_header_type = BCM_TDM_CONTROL_OUT_HEADER_OTMH;
            break;
        }
        case 1:
        {
            if (tdm_header_is_otmh_4b_with_cud_format == 1)
            {
                *switch_tdm_header_type = BCM_TDM_CONTROL_OUT_HEADER_OTMH_4B_WITH_CUD;
            }
            else
            {
                *switch_tdm_header_type = BCM_TDM_CONTROL_OUT_HEADER_FTMH;
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Switch header type %d is not supported!", *switch_tdm_header_type);
            break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_system_ftmh_type_set(
    int unit,
    uint32 use_optimized_ftmh)
{
    uint32 entry_handle_id;
    uint8 use_optimized_ftmh_u8;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONFIGURATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_FTMH_OPTIMIZED, INST_SINGLE, use_optimized_ftmh);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_TDM, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_OPTIMIZED_FTMH, INST_SINGLE,
                                     use_optimized_ftmh);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    
    use_optimized_ftmh_u8 = (uint8) use_optimized_ftmh;
    SHR_IF_ERR_EXIT(dnx_tdm_update_cs(unit, NULL, &use_optimized_ftmh_u8));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Enable Egress TDM
 */
static shr_error_e
dnx_tdm_global_config(
    int unit,
    int enable)
{
    uint32 entry_handle_id;
    int tdm_tc, tdm_dp;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (enable == TRUE)
    {
        tdm_tc = dnx_data_tdm.params.tdm_tc_get(unit);
        tdm_dp = dnx_data_tdm.params.tdm_dp_get(unit);
    }
    else
    {
        tdm_tc = 0;
        tdm_dp = 0;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONFIGURATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_EGRESS_MODE, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_PACKET_TC, INST_SINGLE, tdm_tc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_PACKET_DP, INST_SINGLE, tdm_dp);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_system_ftmh_type_get(
    int unit,
    uint32 *use_optimized_ftmh_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONFIGURATION, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TDM_FTMH_OPTIMIZED, INST_SINGLE, use_optimized_ftmh_p);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_stream_mc_prefix_set(
    int unit,
    int stream_mc_prefix)
{
    SHR_FUNC_INIT_VARS(unit);
    if ((unsigned int) stream_mc_prefix >= dnx_data_tdm.params.global_sid_offset_nof_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Required TDM global sid offset size, %d, is out of allowed range:%d-%d\n",
                     stream_mc_prefix, 0, dnx_data_tdm.params.global_sid_offset_nof_get(unit) - 1);
    }
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_TDM_CONFIGURATION, 0, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TDM_GLOBAL_SID_OFFSET, INST_SINGLE, stream_mc_prefix,
                                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_stream_mc_prefix_get(
    int unit,
    int *stream_mc_prefix_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_TDM_CONFIGURATION, 0, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TDM_GLOBAL_SID_OFFSET, INST_SINGLE,
                                     (uint32 *) stream_mc_prefix_p, GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_max_pkt_get(
    int unit,
    int *max_size_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(max_size_p, _SHR_E_PARAM, "max_size_p");
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONFIGURATION, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TDM_MAX_SIZE, INST_SINGLE, (uint32 *) max_size_p);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_max_pkt_set(
    int unit,
    int max_size)
{
    uint32 entry_handle_id;
    int pkt_size_upper_limit, pkt_size_lower_limit;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    pkt_size_lower_limit = dnx_data_tdm.params.pkt_size_lower_limit_get(unit);
    pkt_size_upper_limit = dnx_data_tdm.params.pkt_size_upper_limit_get(unit);

    if ((max_size > pkt_size_upper_limit) || (max_size < pkt_size_lower_limit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TDM Packet maximal size:%d is out of allowed range:%d-%d\n", max_size,
                     pkt_size_lower_limit, pkt_size_upper_limit);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONFIGURATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_MAX_SIZE, INST_SINGLE, max_size);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_min_pkt_get(
    int unit,
    int *min_size_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(min_size_p, _SHR_E_PARAM, "min_size_p");
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONFIGURATION, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TDM_MIN_SIZE, INST_SINGLE, (uint32 *) min_size_p);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_min_pkt_set(
    int unit,
    int min_size)
{
    uint32 entry_handle_id;
    int pkt_size_upper_limit, pkt_size_lower_limit;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    pkt_size_lower_limit = dnx_data_tdm.params.pkt_size_lower_limit_get(unit);
    pkt_size_upper_limit = dnx_data_tdm.params.pkt_size_upper_limit_get(unit);

    if ((min_size > pkt_size_upper_limit) || (min_size < pkt_size_lower_limit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TDM Packet minimal size:%d is out of allowed range:%d-%d\n", min_size,
                     pkt_size_lower_limit, pkt_size_upper_limit);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONFIGURATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_MIN_SIZE, INST_SINGLE, min_size);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_init(
    int unit)
{
    int tdm_mode;
    int min_size, max_size;
    uint32 use_optimized_ftmh;
    SHR_FUNC_INIT_VARS(unit);

    
    /** init the TDM sw state structure */
    SHR_IF_ERR_EXIT(tdm_ftmh_info.init(unit));

    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported) != TRUE)
    {
        /*
         * TDM is not supported.
         * Just mark TDM is not enabled and leave
         */
        int enable;

        enable = 0;
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_TDM_CONFIGURATION, 0, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TDM_EGRESS_MODE, INST_SINGLE, enable,
                                         GEN_DBAL_FIELD_LAST_MARK));
        SHR_EXIT();
    }
    if (((tdm_mode = dnx_data_tdm.params.mode_get(unit)) == TDM_MODE_NONE))
    {
        /*
         * Disable Egress TDM and leave
         */
        SHR_IF_ERR_EXIT(dnx_tdm_global_config(unit, FALSE));
        SHR_EXIT();
    }
    /*
     * check that flexe is disabled - otherwise failure
     */
    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_is_supported))
    {
        if (dnx_data_nif.flexe.flexe_mode_get(unit) != DNX_FLEXE_MODE_DISABLED)
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TDM can not coexist with FLEXE, disable one of them\n");
        }
    }
    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        uint32 enable_fabric_links_mask;

        /*
         * A value of '2' stands for enabling the action of the selected mask
         * on TDM_DIRECT_LINKS. If a value of '0' is selected then those
         * masks areignored.
         */
        enable_fabric_links_mask = 2;
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_LINK_BITMAP_CONFIGURATION, 0, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TDM_MASK_MODE, INST_SINGLE,
                                         enable_fabric_links_mask, GEN_DBAL_FIELD_LAST_MARK));
    }
    {
        /*
         * Reset 'full packet mode' so that, even under TDM, any number of entries may be
         * added to multicast tables. However, this implies rplicator engines may be interleaved.
         */
        int full_packet_mode;

        full_packet_mode = 0;
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_TDM_FULL_PACKET_MODE, 0, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_FULL_PACKET_MODE, INST_SINGLE, full_packet_mode,
                                         GEN_DBAL_FIELD_LAST_MARK));

    }
    if (tdm_mode == TDM_MODE_OPTIMIZED)
    {
        use_optimized_ftmh = 1;
    }
    else if ((tdm_mode == TDM_MODE_STANDARD) || (tdm_mode == TDM_MODE_PACKET))
    {
        use_optimized_ftmh = 0;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Bad TDM mode:%d\n", tdm_mode);
    }
    SHR_IF_ERR_EXIT(dnx_tdm_global_config(unit, TRUE));
    SHR_IF_ERR_EXIT(dnx_tdm_system_ftmh_type_set(unit, use_optimized_ftmh));
    /*
     * Configure Min and Max Bypass TDM packet size to relatively lower and upper limit
     */
    min_size = dnx_data_tdm.params.pkt_size_lower_limit_get(unit);
    max_size = dnx_data_tdm.params.pkt_size_upper_limit_get(unit);
    SHR_IF_ERR_EXIT(dnx_tdm_min_pkt_set(unit, min_size));
    SHR_IF_ERR_EXIT(dnx_tdm_max_pkt_set(unit, max_size));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure TDM related device wide parameters
 *
 * \param [in] unit - unit id
 * \param [in] type - configuration option taken from bcm_tdm_control_t
 * \param [in] arg - argument for the configuration
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_tdm_control_set(
    int unit,
    bcm_tdm_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Use case per type
     */
    switch (type)
    {
        case bcmTdmBypassMinPacketSize:        /* Minimum packet size for Bypass TDM packet */
        {
            SHR_IF_ERR_EXIT(dnx_tdm_min_pkt_set(unit, arg));
            break;
        }
        case bcmTdmBypassMaxPacketSize:        /* Maximum packet size for Bypass TDM packet */
        {
            SHR_IF_ERR_EXIT(dnx_tdm_max_pkt_set(unit, arg));
            break;
        }
        case bcmTdmOutHeaderType:      /* One of BCM_TDM_CONTROL_OUT_HEADER_XXX. */
        {
            SHR_IF_ERR_EXIT(dnx_tdm_out_header_type_set(unit, arg));
            break;
        }
        case bcmTdmStreamMulticastPrefix:      /* Set 5 Bit prefix from Stream ID to Multicast ID */
        {
            SHR_IF_ERR_EXIT(dnx_tdm_stream_mc_prefix_set(unit, arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Type not supported: %d", type);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Obtain TDM related device wide parameters
 *
 * \param [in] unit - unit id
 * \param [in] type - configuration option taken from bcm_tdm_control_t
 * \param [in] arg  - pointer where parameter value will be put into
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_tdm_control_get(
    int unit,
    bcm_tdm_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Use case per type
     */
    switch (type)
    {
        case bcmTdmBypassMinPacketSize:        /* Minimum packet size for Bypass TDM packet */
        {
            SHR_IF_ERR_EXIT(dnx_tdm_min_pkt_get(unit, arg));
            break;
        }
        case bcmTdmBypassMaxPacketSize:        /* Maximum packet size for Bypass TDM packet */
        {
            SHR_IF_ERR_EXIT(dnx_tdm_max_pkt_get(unit, arg));
            break;
        }
        case bcmTdmOutHeaderType:      /* One of BCM_TDM_CONTROL_OUT_HEADER_XXX. */
        {
            SHR_IF_ERR_EXIT(dnx_tdm_out_header_type_get(unit, arg));
            break;
        }
        case bcmTdmStreamMulticastPrefix:      /* Set 5 Bit prefix from Stream ID to Multicast ID */
        {
            SHR_IF_ERR_EXIT(dnx_tdm_stream_mc_prefix_get(unit, arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Type not supported: %d", type);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
