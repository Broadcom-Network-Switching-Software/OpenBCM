
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INSTRU

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/instru_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_port_pp_access.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>

#include <bcm_int/dnx/instru/instru.h>
#include <bcm_int/dnx/instru/instru_sflow.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm/types.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/port/port_esem.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_snif.h>
#include <bcm/trunk.h>
#include <bcm_int/dnx_dispatch.h>
#include <soc/dnx/swstate/auto_generated/types/trunk_types.h>
#include <src/bcm/dnx/trunk/trunk_sw_db.h>
#include <src/bcm/dnx/trunk/trunk_utils.h>





#include <bcm/instru.h>










#define INSTRU_SFLOW_OAM_LIF_SET      1


#define INSTRU_OAM_KEY_PREFIX   0


#define INSTRU_MDL_MP_TYPE  0


#define INSTRU_SFLOW_NOF_RAW_ENCAP_ENTRIES dnx_data_instru.sflow.nof_sflow_raw_entries_per_stack_get(unit)


#define DNX_INSTRU_SFLOW_VERSION (5)
 

 

 


static shr_error_e
dnx_instru_sflow_encap_id_to_local_lif(
    int unit,
    int sflow_encap_id,
    int *local_lif,
    uint32 *result_type)
{
    dnx_algo_gpm_gport_hw_resources_t hw_res;
    bcm_gport_t encap_in_tunnel;

    SHR_FUNC_INIT_VARS(unit);

    
    sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    BCM_GPORT_TUNNEL_ID_SET(encap_in_tunnel, sflow_encap_id);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, encap_in_tunnel,
                                                       DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &hw_res));

    *local_lif = hw_res.local_out_lif;
    *result_type = hw_res.outlif_dbal_result_type;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_id_verify(
    int unit,
    int sflow_encap_id)
{
    dnx_algo_gpm_gport_hw_resources_t hw_res;
    bcm_gport_t encap_in_tunnel;

    SHR_FUNC_INIT_VARS(unit);

    
    sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    BCM_GPORT_TUNNEL_ID_SET(encap_in_tunnel, sflow_encap_id);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, encap_in_tunnel,
                                                       DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &hw_res));

    if ((hw_res.outlif_dbal_table_id != DBAL_TABLE_EEDB_SFLOW)
        || (hw_res.outlif_dbal_result_type >= DBAL_NOF_RESULT_TYPE_EEDB_SFLOW)
        || (hw_res.outlif_phase != LIF_MNGR_OUTLIF_PHASE_SFLOW))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! sFlow encap 0x%x is not of type sFlow, table_id = %s, result_type = %d, outlif_phase = %d\n",
                     sflow_encap_id, dbal_logical_table_to_string(unit, hw_res.outlif_dbal_table_id),
                     hw_res.outlif_dbal_result_type, hw_res.outlif_phase);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_raw_id_verify(
    int unit,
    int sflow_raw_id)
{
    dnx_algo_gpm_gport_hw_resources_t hw_res;
    bcm_gport_t encap_in_tunnel;

    SHR_FUNC_INIT_VARS(unit);

    
    sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    BCM_GPORT_TUNNEL_ID_SET(encap_in_tunnel, sflow_raw_id);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, encap_in_tunnel,
                                                       DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &hw_res));

    if ((hw_res.outlif_dbal_table_id != DBAL_TABLE_EEDB_DATA_ENTRY)
        || (hw_res.outlif_dbal_result_type != DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! sFlow encap 0x%x is not of type sFlow Data Entry, table_id = %s, result_type = %d\n",
                     sflow_raw_id, dbal_logical_table_to_string(unit, hw_res.outlif_dbal_table_id),
                     hw_res.outlif_dbal_result_type);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_create_verify(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{
    int nof_sflow_encaps;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sflow_encap_info, _SHR_E_PARAM, "sflow_encap_info");

    if (sflow_encap_info->flags &
        (~(BCM_INSTRU_SFLOW_ENCAP_WITH_ID | BCM_INSTRU_SFLOW_ENCAP_REPLACE | BCM_INSTRU_SFLOW_ENCAP_AGGREGATED |
           BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR | BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! Unsupported flag: flags = 0x%08x. Supported flags are: BCM_INSTRU_SFLOW_ENCAP_WITH_ID = 0x%08x, BCM_INSTRU_SFLOW_ENCAP_REPLACE = 0x%08x, "
                     "BCM_INSTRU_SFLOW_ENCAP_AGGREGATED = 0x%08x, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR = 0x%08x, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW = 0x%08x\n",
                     sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID, BCM_INSTRU_SFLOW_ENCAP_REPLACE,
                     BCM_INSTRU_SFLOW_ENCAP_AGGREGATED, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR,
                     BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW);
    }

    
    if (!_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_AGGREGATED))
    {
        
        if (sflow_encap_info->eventor_id != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! eventor_id = %d is only relevant with flag BCM_INSTRU_SFLOW_ENCAP_AGGREGATED (0x%08x)! flags = 0x%08x",
                         sflow_encap_info->eventor_id, BCM_INSTRU_SFLOW_ENCAP_AGGREGATED, sflow_encap_info->flags);
        }
    }
    else
    {
        

        
        if (sflow_encap_info->tunnel_id != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! tunnel_id = 0x%08x is only relevant when flag BCM_INSTRU_SFLOW_ENCAP_AGGREGATED (0x%08x) is OFF! flags = 0x%08x",
                         sflow_encap_info->tunnel_id, BCM_INSTRU_SFLOW_ENCAP_AGGREGATED, sflow_encap_info->flags);
        }
    }

    
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        
        if (!(_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! flags = 0x%08X. flag BCM_INSTRU_SFLOW_ENCAP_REPLACE (0x%08X) is set - flag BCM_INSTRU_SFLOW_ENCAP_WITH_ID (0x%08X) must be set as well!!\n",
                         sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE, BCM_INSTRU_SFLOW_ENCAP_WITH_ID);
        }

        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_verify(unit, sflow_encap_info->sflow_encap_id));
    }
    else
    {
        
        if (!_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR))
        {
            SHR_IF_ERR_EXIT(instru.sflow_info.nof_sflow_encaps.get(unit, &nof_sflow_encaps));

            if (nof_sflow_encaps == dnx_data_instru.sflow.max_nof_sflow_encaps_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Can't create another sFlow instance. Maximum number has been exceeded: %d",
                             nof_sflow_encaps);
            }
        }
    }

    
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR))
    {

        
        if (sflow_encap_info->flags &
            (~
             (BCM_INSTRU_SFLOW_ENCAP_WITH_ID | BCM_INSTRU_SFLOW_ENCAP_REPLACE |
              BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! Unsupported flag: flags = 0x%08x. Supported flags for EXTENDED_INITIATOR are: BCM_INSTRU_SFLOW_ENCAP_WITH_ID = 0x%08x, BCM_INSTRU_SFLOW_ENCAP_REPLACE = 0x%08x, "
                         "BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR = 0x%08x \n",
                         sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID, BCM_INSTRU_SFLOW_ENCAP_REPLACE,
                         BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR);
        }

        
        if ((sflow_encap_info->tunnel_id != 0) || (sflow_encap_info->eventor_id != 0)
            || (sflow_encap_info->sub_agent_id != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! flags = 0x%08x, tunnel_id = %d , eventor_id = %d, sub_agent_id = %d. Are not relevant with flag BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR (0x%08x)!",
                         sflow_encap_info->flags, sflow_encap_info->tunnel_id, sflow_encap_info->eventor_id,
                         sflow_encap_info->sub_agent_id, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR);
        }

        
        if ((sflow_encap_info->stat_cmd != 0) || (sflow_encap_info->counter_command_id != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! flags = 0x%08x, stat_cmd = %d , counter_command_id = %d. Are not relevant with flag BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR (0x%08x)!",
                         sflow_encap_info->flags, sflow_encap_info->stat_cmd, sflow_encap_info->counter_command_id,
                         BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR);
        }

    }

    
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW))
    {

        
        if (sflow_encap_info->flags &
            (~
             (BCM_INSTRU_SFLOW_ENCAP_WITH_ID | BCM_INSTRU_SFLOW_ENCAP_REPLACE | BCM_INSTRU_SFLOW_ENCAP_AGGREGATED |
              BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! Unsupported flag: flags = 0x%08x. Supported flags for EXTENDED_FLOW are: BCM_INSTRU_SFLOW_ENCAP_WITH_ID = 0x%08x, BCM_INSTRU_SFLOW_ENCAP_REPLACE = 0x%08x "
                         "BCM_INSTRU_SFLOW_ENCAP_AGGREGATED = 0x%08x, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW = 0x%08x \n",
                         sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID, BCM_INSTRU_SFLOW_ENCAP_REPLACE,
                         BCM_INSTRU_SFLOW_ENCAP_AGGREGATED, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW);
        }

        
        if (!_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_AGGREGATED))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! Unsupported option: flags = 0x%08x. EXTENDED_FLOW is only supported in AGGREGATED mode!\n",
                         sflow_encap_info->flags);
        }

        
        if ((sflow_encap_info->tunnel_id != 0) || (sflow_encap_info->sub_agent_id != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! flags = 0x%08x. tunnel_id = %d and sub_agent_id = %d are not relevant with flag BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW (0x%08x)!",
                         sflow_encap_info->flags, sflow_encap_info->tunnel_id, sflow_encap_info->sub_agent_id,
                         BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_delete_verify(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sflow_encap_info, _SHR_E_PARAM, "sflow_encap_info");

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_verify(unit, sflow_encap_info->sflow_encap_id));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_sample_interface_flags_verify(
    int unit,
    uint32 flags,
    uint32 supported_flags)
{
    int is_input, is_output;

    SHR_FUNC_INIT_VARS(unit);

    is_input = _SHR_IS_FLAG_SET(flags, BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT);
    is_output = _SHR_IS_FLAG_SET(flags, BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_OUTPUT);

    
    if ((is_input == TRUE) && (is_output == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! Unsupported flags = 0x%08x! both "
                     "BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT(=0x%08x) and BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_OUTPUT(=0x%08x) are set!\n",
                     flags, BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT, BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_OUTPUT);
    }

    if ((is_input == FALSE) && (is_output == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! Unsupported flags = 0x%08x! One of "
                     "BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT(=0x%08x) and BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_OUTPUT(=0x%08x) must be set!\n",
                     flags, BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT, BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_OUTPUT);
    }

    
    if (flags & (~supported_flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! Unsupported flags = 0x%08x! Only 0x%08x are supported!\n", flags, supported_flags);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_sample_interface_verify(
    int unit,
    bcm_instru_sflow_sample_interface_info_t * sample_interface_info)
{
    uint8 is_physical_port;
    uint32 is_system_port;
    uint32 supported_flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sample_interface_info, _SHR_E_PARAM, "sample_interface_info");

    is_system_port = BCM_GPORT_IS_SYSTEM_PORT(sample_interface_info->port);

    
    supported_flags =
        BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT | BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_OUTPUT |
        BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_REPLACE;
    SHR_IF_ERR_EXIT(dnx_instru_sflow_sample_interface_flags_verify
                    (unit, sample_interface_info->flags, supported_flags));

    
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, sample_interface_info->port, &is_physical_port));

    if (is_physical_port == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! Unsupported port = 0x%08x! is not a physical port! flags = 0x%08x.\n",
                     sample_interface_info->port, sample_interface_info->flags);
    }

    if (_SHR_IS_FLAG_SET(sample_interface_info->flags, BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT))
    {
        if (is_system_port == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! Unsupported input port = 0x%08x! is not a system port! flags = 0x%08x.\n",
                         sample_interface_info->port, sample_interface_info->flags);
        }
    }
    else
    {
        uint32 is_mcast = BCM_GPORT_IS_MCAST(sample_interface_info->port);
        uint32 is_voq = BCM_GPORT_IS_UCAST_QUEUE_GROUP(sample_interface_info->port);
        uint32 is_trunk = BCM_GPORT_IS_TRUNK(sample_interface_info->port);

        if ((is_system_port == FALSE) && (is_trunk == FALSE) && (is_mcast == FALSE) && (is_voq == FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! Unsupported output port = 0x%08x! is not a system port, Trunk, MC or FLOW! flags = 0x%08x.\n",
                         sample_interface_info->port, sample_interface_info->flags);
        }

        if (is_system_port)
        {
            
            bcm_trunk_t trunk_id;
            shr_error_e rv;

            rv = bcm_dnx_trunk_find(unit, 0, sample_interface_info->port, &trunk_id);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

            if (rv == _SHR_E_NONE)
            {
                
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error! mapping system port = 0x%08x which is part of trunk is not allowed! Map the whole trunk instead! trunk_id = 0x%08x.\n",
                             sample_interface_info->port, trunk_id);
            }

        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_sample_interface_traverse_verify(
    int unit,
    bcm_instru_sflow_sample_interface_traverse_info_t * sample_interface_traverse_info,
    bcm_instru_sflow_sample_traverse_cb cb,
    void *user_data)
{
    uint32 supported_flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sample_interface_traverse_info, _SHR_E_PARAM, "sample_interface_traverse_info");

    if (!_SHR_IS_FLAG_SET(sample_interface_traverse_info->flags, BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_DELETE_ALL))
    {
        SHR_NULL_CHECK(cb, _SHR_E_PARAM, "cb");
    }

    
    supported_flags =
        BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT | BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_OUTPUT |
        BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_DELETE_ALL;
    SHR_IF_ERR_EXIT(dnx_instru_sflow_sample_interface_flags_verify
                    (unit, sample_interface_traverse_info->flags, supported_flags));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_flags_to_result_type(
    int unit,
    uint32 flags,
    uint32 *dbal_result_type)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (_SHR_IS_FLAG_SET(flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR))
    {
        *dbal_result_type = DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_FIRST;
    }
    else if (_SHR_IS_FLAG_SET(flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW))
    {
        *dbal_result_type = DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND;
    }
    else
    {
        if (_SHR_IS_FLAG_SET(flags, BCM_INSTRU_SFLOW_ENCAP_AGGREGATED))
        {
            *dbal_result_type = DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_SAMP;
        }
        else
        {
            *dbal_result_type = DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP;
        }
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_allocate(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info,
    int *sflow_local_outlif,
    uint32 dbal_result_type)
{
    int lif_alloc_flags = 0, global_lif_id;
    lif_mngr_local_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        
        uint32 result_type;

        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                        (unit, sflow_encap_info->sflow_encap_id, sflow_local_outlif, &result_type));
        SHR_EXIT();
    }

    
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID))
    {
        
        global_lif_id = sflow_encap_info->sflow_encap_id;
        lif_alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
    }

    sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_SFLOW;
    outlif_info.dbal_result_type = dbal_result_type;
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SFLOW;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_alloc_flags, &global_lif_id, NULL, &outlif_info));

    
    if (!_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR))
    {
        SHR_IF_ERR_EXIT(instru.sflow_info.nof_sflow_encaps.inc(unit, 1));
    }

    
    sflow_encap_info->sflow_encap_id = global_lif_id;
    *sflow_local_outlif = outlif_info.local_outlif;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_free(
    int unit,
    int sflow_encap_id,
    int sflow_local_outlif,
    uint32 dbal_result_type)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, sflow_encap_id, NULL, sflow_local_outlif));

    
    if (dbal_result_type != DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_FIRST)
    {
        SHR_IF_ERR_EXIT(instru.sflow_info.nof_sflow_encaps.dec(unit, 1));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_extended_dst_create_verify(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info)
{
    uint32 ipv6_address[UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S];

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sflow_encap_ext_dst_info, _SHR_E_PARAM, "sflow_encap_ext_dst_info");

    if (sflow_encap_ext_dst_info->flags &
        (~(BCM_INSTRU_SFLOW_ENCAP_WITH_ID | BCM_INSTRU_SFLOW_ENCAP_REPLACE | BCM_INSTRU_SFLOW_ENCAP_EXTENDED_IPV6)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! Unsupported flag: flags = 0x%08x. Supported flags are: BCM_INSTRU_SFLOW_ENCAP_WITH_ID = 0x%08x, BCM_INSTRU_SFLOW_ENCAP_REPLACE = 0x%08x, "
                     "BCM_INSTRU_SFLOW_ENCAP_EXTENDED_IPV6 = 0x%08x\n",
                     sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID, BCM_INSTRU_SFLOW_ENCAP_REPLACE,
                     BCM_INSTRU_SFLOW_ENCAP_EXTENDED_IPV6);
    }

    
    if (_SHR_IS_FLAG_SET(sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        
        if (!(_SHR_IS_FLAG_SET(sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! flags = 0x%08X. flag BCM_INSTRU_SFLOW_ENCAP_REPLACE (0x%08X) is set - flag BCM_INSTRU_SFLOW_ENCAP_WITH_ID (0x%08X) must be set as well!!\n",
                         sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE,
                         BCM_INSTRU_SFLOW_ENCAP_WITH_ID);
        }

        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_id_verify(unit, sflow_encap_ext_dst_info->sflow_encap_extended_dst_id));
    }

    
    if (sflow_encap_ext_dst_info->dst_as_path_length != BCM_INSTRU_SFLOW_ENCAP_EXTENDED_NOF_DSTS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! dst_as_path_length = %d. Only %d is supported!",
                     sflow_encap_ext_dst_info->dst_as_path_length, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_NOF_DSTS);

    }

    
    utilex_pp_ipv6_address_struct_to_long(sflow_encap_ext_dst_info->next_hop_ipv6_address, ipv6_address);

    if (!_SHR_IS_FLAG_SET(sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_IPV6))
    {
        
        if (sflow_encap_ext_dst_info->next_hop_ipv4_address == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error! flags = 0x%08x. IPv4 format but next_hop_ipv4_address = 0!",
                         sflow_encap_ext_dst_info->flags);
        }

        if ((ipv6_address[0] != 0) || (ipv6_address[1] != 0) || (ipv6_address[2] != 0) || (ipv6_address[3] != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! flags = 0x%08x. IPv4 format but next_hop_ipv6_address = [0x%08x, 0x%08x, 0x%08x, 0x%08x] is not 0!",
                         sflow_encap_ext_dst_info->flags, ipv6_address[0], ipv6_address[1], ipv6_address[2],
                         ipv6_address[3]);
        }
    }
    else
    {
        
        if ((ipv6_address[0] == 0) && (ipv6_address[1] == 0) && (ipv6_address[2] == 0) && (ipv6_address[3] == 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error! flags = 0x%08x. IPv6 format but next_hop_ipv6_address = 0!",
                         sflow_encap_ext_dst_info->flags);
        }

        if (sflow_encap_ext_dst_info->next_hop_ipv4_address != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error! flags = 0x%08x. Thus, IPv6 format but next_hop_ipv4_address = 0x%08x!",
                         sflow_encap_ext_dst_info->flags, sflow_encap_ext_dst_info->next_hop_ipv4_address);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_extended_dst_get_delete_verify(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sflow_encap_ext_dst_info, _SHR_E_PARAM, "sflow_encap_ext_dst_info");

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_id_verify(unit, sflow_encap_ext_dst_info->sflow_encap_extended_dst_id));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_extended_src_create_verify(
    int unit,
    bcm_instru_sflow_encap_extended_src_info_t * sflow_encap_ext_src_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sflow_encap_ext_src_info, _SHR_E_PARAM, "sflow_encap_ext_src_info");

    if (sflow_encap_ext_src_info->flags & (~(BCM_INSTRU_SFLOW_ENCAP_WITH_ID | BCM_INSTRU_SFLOW_ENCAP_REPLACE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! Unsupported flag: flags = 0x%08x. Supported flags are: BCM_INSTRU_SFLOW_ENCAP_WITH_ID = 0x%08x, BCM_INSTRU_SFLOW_ENCAP_REPLACE = 0x%08x \n",
                     sflow_encap_ext_src_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID, BCM_INSTRU_SFLOW_ENCAP_REPLACE);
    }

    
    if (_SHR_IS_FLAG_SET(sflow_encap_ext_src_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        
        if (!(_SHR_IS_FLAG_SET(sflow_encap_ext_src_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! flags = 0x%08X. flag BCM_INSTRU_SFLOW_ENCAP_REPLACE (0x%08X) is set - flag BCM_INSTRU_SFLOW_ENCAP_WITH_ID (0x%08X) must be set as well!!\n",
                         sflow_encap_ext_src_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE,
                         BCM_INSTRU_SFLOW_ENCAP_WITH_ID);
        }

        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_id_verify(unit, sflow_encap_ext_src_info->sflow_encap_extended_src_id));
    }

    
    if (sflow_encap_ext_src_info->src_as == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error! src_as = 0.\n");
    }

    
    if (sflow_encap_ext_src_info->src_as_peer == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error! src_as_peer = 0.\n");
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_extended_src_get_delete_verify(
    int unit,
    bcm_instru_sflow_encap_extended_src_info_t * sflow_encap_ext_src_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sflow_encap_ext_src_info, _SHR_E_PARAM, "sflow_encap_ext_src_info");

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_id_verify(unit, sflow_encap_ext_src_info->sflow_encap_extended_src_id));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_destination_profile_allocate(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info,
    int *sflow_destination_profile,
    int *write_destination_profile,
    int *old_sflow_destination_profile,
    int *delete_old_destination_profile)
{
    uint8 first_reference, last_reference;
    uint8 success;
    sflow_destination_key_t destination;

    SHR_FUNC_INIT_VARS(unit);

    *write_destination_profile = FALSE;
    *delete_old_destination_profile = FALSE;

    sal_memset(&destination, 0, sizeof(sflow_destination_key_t));
    destination.sub_agent_id = sflow_encap_info->sub_agent_id;
    destination.udp_tunnel = sflow_encap_info->tunnel_id;
    destination.eventor_id = sflow_encap_info->eventor_id;

    
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        
        uint8 found;

        SHR_IF_ERR_EXIT(instru.sflow_info.sflow_encap_to_sflow_destination_profile.find(unit,
                                                                                        &sflow_encap_info->sflow_encap_id,
                                                                                        old_sflow_destination_profile,
                                                                                        &found));

        SHR_IF_ERR_EXIT(instru.sflow_info.sflow_destination_profile.exchange(unit, 0, &destination,
                                                                             *old_sflow_destination_profile, NULL,
                                                                             sflow_destination_profile,
                                                                             &first_reference, &last_reference));

        
        SHR_IF_ERR_EXIT(instru.sflow_info.sflow_encap_to_sflow_destination_profile.delete(unit,
                                                                                          &sflow_encap_info->sflow_encap_id));

        if (last_reference)
        {
            *delete_old_destination_profile = TRUE;
        }
    }
    else
    {
        
        SHR_IF_ERR_EXIT(instru.sflow_info.sflow_destination_profile.allocate_single(unit, 0,
                                                                                    &destination, NULL,
                                                                                    sflow_destination_profile,
                                                                                    &first_reference));
        *old_sflow_destination_profile = -1;
        last_reference = FALSE;
    }

    
    if (first_reference)
    {
        *write_destination_profile = TRUE;
    }

    
    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_encap_to_sflow_destination_profile.insert(unit,
                                                                                      &sflow_encap_info->sflow_encap_id,
                                                                                      sflow_destination_profile,
                                                                                      &success));

    if (!success)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Can't create mapping between sFlow encap and UDP profile.");
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_destination_profile_free(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info,
    int *sflow_destination_profile,
    int *delete_destination_profile)
{
    uint8 last_reference, found;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_encap_to_sflow_destination_profile.find(unit,
                                                                                    &sflow_encap_info->sflow_encap_id,
                                                                                    sflow_destination_profile, &found));

    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_destination_profile.free_single(unit,
                                                                            *sflow_destination_profile,
                                                                            &last_reference));

    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_encap_to_sflow_destination_profile.delete(unit,
                                                                                      &sflow_encap_info->sflow_encap_id));

    if (last_reference)
    {
        *delete_destination_profile = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_raw_lif_alloc(
    int unit,
    lif_mngr_outlif_phase_e outlif_phase,
    int lif_alloc_flags,
    int *local_outlif,
    int *global_lif)
{
    lif_mngr_local_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_DATA_ENTRY;
    outlif_info.dbal_result_type = DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA;

    outlif_info.outlif_phase = outlif_phase;

    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_alloc_flags, global_lif, NULL, &outlif_info));

    *local_outlif = outlif_info.local_outlif;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_raw_lif_free(
    int unit,
    int local_outlif,
    int global_lif)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, global_lif, NULL, local_outlif));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_raw_lif_data_clear(
    int unit,
    int local_outlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);

    
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_header_sampling_raw_lifs_allocate(
    int unit,
    int sflow_destination_profile)
{
    int local_outlif;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_alloc
                    (unit, LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, &local_outlif, NULL));

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.set(unit, sflow_destination_profile, 0, local_outlif));

    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_alloc
                    (unit, LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_2, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, &local_outlif, NULL));

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.set(unit, sflow_destination_profile, 1, local_outlif));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_header_sampling_raw_lifs_free(
    int unit,
    int sflow_destination_profile)
{
    int local_outlif;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.get(unit, sflow_destination_profile, 0, &local_outlif));

    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_free(unit, local_outlif, LIF_MNGR_INVALID));

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.set(unit, sflow_destination_profile, 0, 0));

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.get(unit, sflow_destination_profile, 1, &local_outlif));

    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_free(unit, local_outlif, LIF_MNGR_INVALID));

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.set(unit, sflow_destination_profile, 1, 0));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_header_sampling_raw_lifs_data_clear(
    int unit,
    int sflow_destination_profile)
{
    uint32 entry_handle_id;
    int current_outlif, current_outlif_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    for (current_outlif_index = 0; current_outlif_index < INSTRU_SFLOW_NOF_RAW_ENCAP_ENTRIES; current_outlif_index++)
    {
        SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.get(unit, sflow_destination_profile,
                                                                     current_outlif_index, &current_outlif));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, current_outlif);

        
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_raw_data_lifs_write(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info,
    int sflow_destination_profile,
    uint32 dbal_result_type)
{
    uint32 entry_handle_id;
    int current_outlif, next_outlif;
    dnx_algo_gpm_gport_hw_resources_t hw_res;
    uint32 entry_data[4];
    bcm_ip_t agent_ip_address;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    
    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.get(unit, sflow_destination_profile, 1, &current_outlif));

    
    if (dbal_result_type == DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP)
    {
        
        SHR_IF_ERR_EXIT(instru.sflow_info.agent_ip_address.get(unit, &agent_ip_address));
        entry_data[0] = sflow_encap_info->sub_agent_id;
        entry_data[1] = agent_ip_address;
        entry_data[2] = 1;
        entry_data[3] = DNX_INSTRU_SFLOW_VERSION;
    }
    else
    {
        entry_data[0] = entry_data[1] = entry_data[2] = 0;
        entry_data[3] = sflow_encap_info->eventor_id << 24;
    }

    sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));

    
    if (dbal_result_type == DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP)
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, sflow_encap_info->tunnel_id,
                                                           DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &hw_res));
    }

    next_outlif = hw_res.local_out_lif;

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, current_outlif);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, next_outlif);

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_SFLOW_DATA_EMPTY, INST_SINGLE, entry_data);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_ENTRY_RESERVED, INST_SINGLE, 0x3);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EEDB_DATA_ENTRY, entry_handle_id));
    
    next_outlif = current_outlif;
    entry_data[0] = entry_data[1] = entry_data[2] = entry_data[3] = 0;

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.get(unit, sflow_destination_profile, 0, &current_outlif));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, current_outlif);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, next_outlif);

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_SFLOW_DATA_EMPTY, INST_SINGLE, entry_data);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_ENTRY_RESERVED, INST_SINGLE, 0x3);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_write(
    int unit,
    int sflow_local_outlif,
    uint32 sflow_destination_profile,
    uint16 eventor_id,
    uint32 dbal_result_type)
{
    uint32 entry_handle_id;
    uint32 entry_data[3];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SFLOW, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, sflow_local_outlif);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, dbal_result_type);

    switch (dbal_result_type)
    {
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP:
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_SAMP:
        {
            int next_outlif;
            uint32 esem_access_cmd;

            SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.get(unit, sflow_destination_profile,
                                                                         0, &next_outlif));

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE,
                                         INSTRU_SFLOW_OAM_LIF_SET);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                         next_outlif);

            
            entry_data[0] = 0;
            entry_data[1] = 1;
            entry_data[2] = 1;

            esem_access_cmd = dnx_data_esem.access_cmd.sflow_sample_interface_get(unit);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE, esem_access_cmd);

            break;
        }

        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_FIRST:
        {
            
            entry_data[0] = 0;
            entry_data[1] = 1;
            entry_data[2] = 1;

            break;
        }
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND:
        {
            uint32 esem_access_cmd;

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE,
                                         INSTRU_SFLOW_OAM_LIF_SET);

            
            entry_data[0] = 0;
            entry_data[1] = 0;
            entry_data[2] = eventor_id << 24;

            esem_access_cmd = dnx_data_esem.access_cmd.sflow_sample_interface_get(unit);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE, esem_access_cmd);

            break;
        }
        default:
        {

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Unsupported result type: dbal_result_type = %d. Supported result_type are: DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP = %d, DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_SAMP = %d, "
                         "DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_FIRST = %d, DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND = %d\n",
                         dbal_result_type, DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP,
                         DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_SAMP,
                         DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_FIRST,
                         DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND);
        }
    }

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, entry_data);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_add_oam_lif_db(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info,
    int sflow_outlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_LIF_DB, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, INSTRU_OAM_KEY_PREFIX);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, sflow_outlif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDL_MP_TYPE, INST_SINGLE, INSTRU_MDL_MP_TYPE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_BASE, INST_SINGLE,
                                 sflow_encap_info->stat_cmd);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE,
                                 sflow_encap_info->counter_command_id);
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_oam_lif_db_get(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info,
    int sflow_outlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_LIF_DB, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, INSTRU_OAM_KEY_PREFIX);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, sflow_outlif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id,
                                                        DBAL_FIELD_OAM_COUNTER_BASE, INST_SINGLE,
                                                        (uint32 *) &sflow_encap_info->stat_cmd));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE,
                     (uint32 *) &sflow_encap_info->counter_command_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_destination_get(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{
    uint8 found;
    int sflow_destination_profile;
    sflow_destination_key_t destination_key;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_encap_to_sflow_destination_profile.find(unit,
                                                                                    &sflow_encap_info->sflow_encap_id,
                                                                                    &sflow_destination_profile,
                                                                                    &found));

    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_destination_profile.profile_data_get(unit,
                                                                                 sflow_destination_profile, NULL,
                                                                                 &destination_key));

    sflow_encap_info->tunnel_id = destination_key.udp_tunnel;
    sflow_encap_info->sub_agent_id = destination_key.sub_agent_id;
    sflow_encap_info->eventor_id = destination_key.eventor_id;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_oam_lif_db_delete(
    int unit,
    int sflow_outlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_LIF_DB, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, INSTRU_OAM_KEY_PREFIX);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, sflow_outlif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_clear(
    int unit,
    int sflow_local_outlif,
    uint32 result_type)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SFLOW, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, sflow_local_outlif);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);

    
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


int
dnx_instru_sflow_encap_header_sampling_create(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{
    int sflow_global_lif = 0;
    int sflow_destination_profile = 0;
    int sflow_local_outlif = 0;
    int write_destination_profile, old_sflow_destination_profile, delete_old_destination_profile;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_flags_to_result_type(unit, sflow_encap_info->flags, &result_type));

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_allocate(unit, sflow_encap_info, &sflow_local_outlif, result_type));
    sflow_global_lif = sflow_encap_info->sflow_encap_id;

    SHR_IF_ERR_EXIT(dnx_instru_sflow_destination_profile_allocate
                    (unit, sflow_encap_info, &sflow_destination_profile, &write_destination_profile,
                     &old_sflow_destination_profile, &delete_old_destination_profile));

    if (write_destination_profile && (sflow_destination_profile != old_sflow_destination_profile))
    {
        
        SHR_IF_ERR_EXIT(dnx_instru_sflow_header_sampling_raw_lifs_allocate(unit, sflow_destination_profile));
    }

    
    if (write_destination_profile)
    {
        
        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_data_lifs_write
                        (unit, sflow_encap_info, sflow_destination_profile, result_type));
    }

    if (sflow_destination_profile != old_sflow_destination_profile)
    {
        
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_write
                        (unit, sflow_local_outlif, sflow_destination_profile, sflow_encap_info->eventor_id,
                         result_type));
    }

    SHR_IF_ERR_EXIT(dnx_instru_sflow_add_oam_lif_db(unit, sflow_encap_info, sflow_local_outlif));

    if (!_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem(unit, _SHR_CORE_ALL, sflow_global_lif, sflow_local_outlif));
    }

    if (delete_old_destination_profile)
    {
        
        SHR_IF_ERR_EXIT(dnx_instru_sflow_header_sampling_raw_lifs_data_clear(unit, old_sflow_destination_profile));

        SHR_IF_ERR_EXIT(dnx_instru_sflow_header_sampling_raw_lifs_free(unit, sflow_destination_profile));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_extended_create(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{
    int sflow_destination_profile = 0;
    int sflow_local_outlif = 0;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_flags_to_result_type(unit, sflow_encap_info->flags, &result_type));

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_allocate(unit, sflow_encap_info, &sflow_local_outlif, result_type));

    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_write
                    (unit, sflow_local_outlif, sflow_destination_profile, sflow_encap_info->eventor_id, result_type));

    
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW))
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_add_oam_lif_db(unit, sflow_encap_info, sflow_local_outlif));
    }

    if (!_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem
                        (unit, _SHR_CORE_ALL, sflow_encap_info->sflow_encap_id, sflow_local_outlif));
    }

exit:
    SHR_FUNC_EXIT;
}


int
bcm_dnx_instru_sflow_encap_create(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_create_verify(unit, sflow_encap_info));

    
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR) ||
        _SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW))
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_create(unit, sflow_encap_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_header_sampling_create(unit, sflow_encap_info));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}


int
bcm_dnx_instru_sflow_encap_delete(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{
    int local_lif;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_delete_verify(unit, sflow_encap_info));

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                    (unit, sflow_encap_info->sflow_encap_id, &local_lif, &result_type));

    switch (result_type)
    {
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP:
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_SAMP:
        {
            int sflow_destination_profile, delete_destination_profile = -1;

            
            SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, sflow_encap_info->sflow_encap_id));

            SHR_IF_ERR_EXIT(dnx_instru_sflow_oam_lif_db_delete(unit, local_lif));

            SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_clear(unit, local_lif, result_type));

            
            SHR_IF_ERR_EXIT(dnx_instru_sflow_destination_profile_free
                            (unit, sflow_encap_info, &sflow_destination_profile, &delete_destination_profile));

            if (delete_destination_profile)
            {
                SHR_IF_ERR_EXIT(dnx_instru_sflow_header_sampling_raw_lifs_data_clear(unit, sflow_destination_profile));

                SHR_IF_ERR_EXIT(dnx_instru_sflow_header_sampling_raw_lifs_free(unit, sflow_destination_profile));
            }

            
            SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_free
                            (unit, sflow_encap_info->sflow_encap_id, local_lif, result_type));

            break;
        }
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_FIRST:
        {
            
            SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, sflow_encap_info->sflow_encap_id));

            SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_clear(unit, local_lif, result_type));

            
            SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_free
                            (unit, sflow_encap_info->sflow_encap_id, local_lif, result_type));

            break;
        }
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND:
        {
            
            SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, sflow_encap_info->sflow_encap_id));

            SHR_IF_ERR_EXIT(dnx_instru_sflow_oam_lif_db_delete(unit, local_lif));

            SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_clear(unit, local_lif, result_type));

            
            SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_free
                            (unit, sflow_encap_info->sflow_encap_id, local_lif, result_type));

            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! sflow_encap_id = 0x%08x has unsupported result type: result_type = %d! \n",
                         sflow_encap_info->sflow_encap_id, result_type);
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_get(
    int unit,
    int local_lif,
    uint32 result_type,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (result_type)
    {
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP:
        {
            sflow_encap_info->flags = 0;

            SHR_IF_ERR_EXIT(dnx_instru_sflow_oam_lif_db_get(unit, sflow_encap_info, local_lif));

            SHR_IF_ERR_EXIT(dnx_instru_sflow_destination_get(unit, sflow_encap_info));

            break;
        }
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_SAMP:
        {
            sflow_encap_info->flags = BCM_INSTRU_SFLOW_ENCAP_AGGREGATED;

            SHR_IF_ERR_EXIT(dnx_instru_sflow_oam_lif_db_get(unit, sflow_encap_info, local_lif));

            SHR_IF_ERR_EXIT(dnx_instru_sflow_destination_get(unit, sflow_encap_info));

            break;
        }
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_FIRST:
        {
            sflow_encap_info->flags = BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR;
            sflow_encap_info->tunnel_id = 0;
            sflow_encap_info->sub_agent_id = 0;
            sflow_encap_info->eventor_id = 0;
            sflow_encap_info->stat_cmd = 0;
            sflow_encap_info->counter_command_id = 0;

            break;
        }
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND:
        {
            uint32 entry_handle_id;
            uint32 entry_data[4];

            sflow_encap_info->flags = BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW | BCM_INSTRU_SFLOW_ENCAP_AGGREGATED;

            SHR_IF_ERR_EXIT(dnx_instru_sflow_oam_lif_db_get(unit, sflow_encap_info, local_lif));

            

            
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SFLOW, &entry_handle_id));

            
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif);

            
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);

            dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, entry_data);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            
            sflow_encap_info->eventor_id = (entry_data[2] >> 24);

            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! sflow_encap_id = 0x%08x has unsupported result type: result_type = %d! \n",
                         sflow_encap_info->sflow_encap_id, result_type);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


int
bcm_dnx_instru_sflow_encap_get(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{
    int local_lif;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                    (unit, sflow_encap_info->sflow_encap_id, &local_lif, &result_type));

    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_get(unit, local_lif, result_type, sflow_encap_info));

exit:
    SHR_FUNC_EXIT;
}


int
bcm_dnx_instru_sflow_encap_traverse(
    int unit,
    bcm_instru_sflow_encap_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SFLOW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        bcm_instru_sflow_encap_info_t sflow_encap_info;
        uint32 result_type, local_out_lif;
        bcm_gport_t gport;

        sal_memset(&sflow_encap_info, 0, sizeof(bcm_instru_sflow_encap_info_t));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE,
                                                                INST_SINGLE, &result_type));

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, &local_out_lif));

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                    _SHR_CORE_ALL, local_out_lif, &gport));

        sflow_encap_info.sflow_encap_id = BCM_GPORT_TUNNEL_ID_GET(gport);

        
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_get(unit, local_out_lif, result_type, &sflow_encap_info));

        
        if (cb != NULL)
        {
            
            SHR_IF_ERR_EXIT((*cb) (unit, &sflow_encap_info, user_data));
        }

        
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_instru_sflow_virtual_register_set(
    int unit,
    dbal_fields_e field,
    int value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_SFLOW, &entry_handle_id));

    
    dbal_entry_value_field32_set(unit, entry_handle_id, field, INST_SINGLE, value);

    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_instru_sflow_virtual_register_get(
    int unit,
    dbal_fields_e field,
    int *value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_SFLOW, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field, INST_SINGLE, (uint32 *) value));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_instru_sflow_sampling_rate_set(
    int unit,
    int sampling_rate)
{
    uint32 prob_max_val;

    SHR_FUNC_INIT_VARS(unit);

    
    prob_max_val = dnx_data_snif.ingress.prob_max_val_get(unit);

    if ((sampling_rate < 1) || (sampling_rate > prob_max_val))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "sampling_rate = %d is out of range [1:%d]!\n", sampling_rate, prob_max_val);
    }

    SHR_IF_ERR_EXIT(dnx_instru_sflow_virtual_register_set(unit, DBAL_FIELD_SFLOW_SAMPLING_RATE, sampling_rate));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_instru_sflow_sampling_rate_get(
    int unit,
    int *sampling_rate)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_virtual_register_get(unit, DBAL_FIELD_SFLOW_SAMPLING_RATE, sampling_rate));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_instru_sflow_uptime_set(
    int unit,
    int uptime)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_virtual_register_set(unit, DBAL_FIELD_SFLOW_UPTIME, uptime));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_instru_sflow_uptime_get(
    int unit,
    int *uptime)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_virtual_register_get(unit, DBAL_FIELD_SFLOW_UPTIME, uptime));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_instru_sflow_myrouter_as_number_set(
    int unit,
    int as_number)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_virtual_register_set(unit, DBAL_FIELD_SFLOW_MYROUTER_AS_NUMBER, as_number));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_instru_sflow_myrouter_as_number_get(
    int unit,
    int *as_number)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_virtual_register_get(unit, DBAL_FIELD_SFLOW_MYROUTER_AS_NUMBER, as_number));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_instru_sflow_agent_id_address_set(
    int unit,
    bcm_ip_t agent_ip_address)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(instru.sflow_info.agent_ip_address.set(unit, agent_ip_address));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_instru_sflow_agent_id_address_get(
    int unit,
    bcm_ip_t * agent_ip_address)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(instru.sflow_info.agent_ip_address.get(unit, agent_ip_address));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_extended_dst_raw_data_lif_write(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info,
    lif_mngr_outlif_phase_e outlif_phase,
    int current_local_outlif,
    int next_local_lif)
{
    uint32 entry_handle_id;
    uint32 entry_data[4];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    switch (outlif_phase)
    {
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1:
        {
            
            entry_data[0] = sflow_encap_ext_dst_info->dst_as_path[0];
            entry_data[1] = sflow_encap_ext_dst_info->dst_as_path[1];
            entry_data[2] = sflow_encap_ext_dst_info->dst_as_path[2];
            entry_data[3] = sflow_encap_ext_dst_info->dst_as_path[3];

            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_2:
        {

            
            entry_data[0] = 2;
            entry_data[1] = 4;
            entry_data[2] = 1;
            entry_data[3] = 0;

            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_3:
        {

            
            if (!_SHR_IS_FLAG_SET(sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_IPV6))
            {
                entry_data[0] = sflow_encap_ext_dst_info->next_hop_ipv4_address;
                entry_data[1] = 0;
                entry_data[2] = 0;
                entry_data[3] = 0;
            }
            else
            {
                utilex_pp_ipv6_address_struct_to_long(sflow_encap_ext_dst_info->next_hop_ipv6_address, entry_data);
            }

            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_4:
        {

            
            entry_data[0] = 56;

            if (!_SHR_IS_FLAG_SET(sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_IPV6))
            {
                
                entry_data[1] = 1;
            }
            else
            {
                
                entry_data[1] = 2;
            }
            entry_data[2] = 1003;
            entry_data[3] = 2;

            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_5:
        {

            
            entry_data[0] = 1;
            entry_data[1] = 248;
            entry_data[2] = 0;
            entry_data[3] = 0;

            break;
        }
        default:
        {

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Unsupported SFLOW Extended Destination RAW pahse: outlif_phase = %d. \n",
                         outlif_phase);
        }
    }

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, current_local_outlif);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_SFLOW_DATA_EMPTY, INST_SINGLE, entry_data);

    
    if (!_SHR_IS_FLAG_SET(sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                     next_local_lif);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_ENTRY_RESERVED, INST_SINGLE, 0x3);

    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_extended_dst_raw_data_lif_next_outlif_get(
    int unit,
    int current_local_outlif,
    int *next_local_outlif)
{
    uint32 entry_handle_id;
    uint32 next_outlif_ptr;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, current_local_outlif);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                   &next_outlif_ptr);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *next_local_outlif = (int) next_outlif_ptr;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_extended_dst_raw_data_lif_get(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info,
    lif_mngr_outlif_phase_e outlif_phase,
    int current_local_outlif)
{
    uint32 entry_handle_id;
    uint32 entry_data[4];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, current_local_outlif);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_SFLOW_DATA_EMPTY, INST_SINGLE, entry_data);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    switch (outlif_phase)
    {
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1:
        {
            
            sflow_encap_ext_dst_info->dst_as_path[0] = entry_data[0];
            sflow_encap_ext_dst_info->dst_as_path[1] = entry_data[1];
            sflow_encap_ext_dst_info->dst_as_path[2] = entry_data[2];
            sflow_encap_ext_dst_info->dst_as_path[3] = entry_data[3];

            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_2:
        {

            
            sflow_encap_ext_dst_info->dst_as_path_length = 4;

            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_3:
        {

            
            sflow_encap_ext_dst_info->next_hop_ipv4_address = entry_data[0];
            utilex_pp_ipv6_address_long_to_struct(entry_data, sflow_encap_ext_dst_info->next_hop_ipv6_address);

            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_4:
        {

            
            if (entry_data[1] == 1)
            {
                
                entry_data[0] = entry_data[1] = entry_data[2] = entry_data[3] = 0;
                utilex_pp_ipv6_address_long_to_struct(entry_data, sflow_encap_ext_dst_info->next_hop_ipv6_address);
            }
            else if (entry_data[1] == 2)
            {
                
                sflow_encap_ext_dst_info->next_hop_ipv4_address = 0;

                sflow_encap_ext_dst_info->flags = BCM_INSTRU_SFLOW_ENCAP_EXTENDED_IPV6;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Error! reading sFlow encap 0x%x, Next-Hop-IP-Type = %d! Valid values are IPv4(=1) and IPv6(=2).\n",
                             sflow_encap_ext_dst_info->sflow_encap_extended_dst_id, entry_data[1]);
            }

            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_5:
        {

            

            break;
        }
        default:
        {

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Unsupported SFLOW Extended Destination RAW pahse: outlif_phase = %d. \n",
                         outlif_phase);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
dnx_instru_sflow_encap_extended_dst_create(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info)
{
    int current_local_outlif, next_local_outlif;
    int outlif_phase;
    int lif_alloc_flags;

    SHR_FUNC_INIT_VARS(unit);

    
    lif_alloc_flags = LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF;
    next_local_outlif = 0;
    for (outlif_phase = LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_5; outlif_phase > LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1;
         outlif_phase--)
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_alloc
                        (unit, outlif_phase, lif_alloc_flags, &current_local_outlif, NULL));

        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_raw_data_lif_write
                        (unit, sflow_encap_ext_dst_info, outlif_phase, current_local_outlif, next_local_outlif));

        next_local_outlif = current_local_outlif;
    }

    
    if (_SHR_IS_FLAG_SET(sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID))
    {
        lif_alloc_flags = LIF_MNGR_GLOBAL_LIF_WITH_ID;
    }
    else
    {
        lif_alloc_flags = 0;
    }
    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_alloc
                    (unit, LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1, lif_alloc_flags, &current_local_outlif,
                     &(sflow_encap_ext_dst_info->sflow_encap_extended_dst_id)));

    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_raw_data_lif_write
                    (unit, sflow_encap_ext_dst_info, LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1, current_local_outlif,
                     next_local_outlif));

    SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem
                    (unit, _SHR_CORE_ALL, sflow_encap_ext_dst_info->sflow_encap_extended_dst_id, current_local_outlif));

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_instru_sflow_encap_extended_dst_replace(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info)
{
    int current_local_outlif, next_local_outlif;
    int outlif_phase;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                    (unit, sflow_encap_ext_dst_info->sflow_encap_extended_dst_id, &current_local_outlif, &result_type));

    
    for (outlif_phase = LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1; outlif_phase <= LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_5;
         outlif_phase++)
    {
        
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_raw_data_lif_next_outlif_get
                        (unit, current_local_outlif, &next_local_outlif));

        
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_raw_data_lif_write
                        (unit, sflow_encap_ext_dst_info, outlif_phase, current_local_outlif, next_local_outlif));

        current_local_outlif = next_local_outlif;
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_instru_sflow_encap_extended_dst_create(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_extended_dst_create_verify(unit, sflow_encap_ext_dst_info));

    if (_SHR_IS_FLAG_SET(sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_replace(unit, sflow_encap_ext_dst_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_create(unit, sflow_encap_ext_dst_info));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_instru_sflow_encap_extended_dst_delete(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info)
{
    int current_local_outlif, next_local_outlif;
    int outlif_phase;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_extended_dst_get_delete_verify(unit, sflow_encap_ext_dst_info));

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                    (unit, sflow_encap_ext_dst_info->sflow_encap_extended_dst_id, &current_local_outlif, &result_type));

    
    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem
                    (unit, _SHR_CORE_ALL, sflow_encap_ext_dst_info->sflow_encap_extended_dst_id));

    
    for (outlif_phase = LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1; outlif_phase <= LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_5;
         outlif_phase++)
    {
        
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_raw_data_lif_next_outlif_get
                        (unit, current_local_outlif, &next_local_outlif));

        
        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_data_clear(unit, current_local_outlif));

        
        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_free(unit, current_local_outlif, LIF_MNGR_INVALID));

        current_local_outlif = next_local_outlif;
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_instru_sflow_encap_extended_dst_get(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info)
{
    int current_local_outlif, next_local_outlif;
    int outlif_phase;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_extended_dst_get_delete_verify(unit, sflow_encap_ext_dst_info));

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                    (unit, sflow_encap_ext_dst_info->sflow_encap_extended_dst_id, &current_local_outlif, &result_type));

    
    for (outlif_phase = LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1; outlif_phase <= LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_5;
         outlif_phase++)
    {
        
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_raw_data_lif_next_outlif_get
                        (unit, current_local_outlif, &next_local_outlif));

        
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_raw_data_lif_get
                        (unit, sflow_encap_ext_dst_info, outlif_phase, current_local_outlif));

        current_local_outlif = next_local_outlif;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_extended_src_raw_data_lif_write(
    int unit,
    bcm_instru_sflow_encap_extended_src_info_t * sflow_encap_ext_src_info,
    int local_outlif)
{
    uint32 entry_handle_id;
    uint32 entry_data[4];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    
    entry_data[0] = sflow_encap_ext_src_info->src_as;
    entry_data[1] = sflow_encap_ext_src_info->src_as_peer;
    entry_data[2] = 0;
    entry_data[3] = 0;

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_SFLOW_DATA_EMPTY, INST_SINGLE, entry_data);

    
    if (!_SHR_IS_FLAG_SET(sflow_encap_ext_src_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, 0);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_ENTRY_RESERVED, INST_SINGLE, 0x3);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_encap_extended_src_raw_data_lif_get(
    int unit,
    bcm_instru_sflow_encap_extended_src_info_t * sflow_encap_ext_src_info,
    int local_outlif)
{
    uint32 entry_handle_id;
    uint32 entry_data[4];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_SFLOW_DATA_EMPTY, INST_SINGLE, entry_data);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    
    sflow_encap_ext_src_info->src_as = entry_data[0];
    sflow_encap_ext_src_info->src_as_peer = entry_data[1];

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
bcm_dnx_instru_sflow_encap_extended_src_create(
    int unit,
    bcm_instru_sflow_encap_extended_src_info_t * sflow_encap_ext_src_info)
{
    int local_outlif;
    int lif_alloc_flags;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_extended_src_create_verify(unit, sflow_encap_ext_src_info));

    if (_SHR_IS_FLAG_SET(sflow_encap_ext_src_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        uint32 result_type;

        
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                        (unit, sflow_encap_ext_src_info->sflow_encap_extended_src_id, &local_outlif, &result_type));

        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_src_raw_data_lif_write
                        (unit, sflow_encap_ext_src_info, local_outlif));
    }
    else
    {
        
        if (_SHR_IS_FLAG_SET(sflow_encap_ext_src_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID))
        {
            lif_alloc_flags = LIF_MNGR_GLOBAL_LIF_WITH_ID;
        }
        else
        {
            lif_alloc_flags = 0;
        }

        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_alloc
                        (unit, LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_6, lif_alloc_flags, &local_outlif,
                         &(sflow_encap_ext_src_info->sflow_encap_extended_src_id)));

        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_src_raw_data_lif_write
                        (unit, sflow_encap_ext_src_info, local_outlif));

        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem
                        (unit, _SHR_CORE_ALL, sflow_encap_ext_src_info->sflow_encap_extended_src_id, local_outlif));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_instru_sflow_encap_extended_src_delete(
    int unit,
    bcm_instru_sflow_encap_extended_src_info_t * sflow_encap_ext_src_info)
{
    int current_local_outlif;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_extended_src_get_delete_verify(unit, sflow_encap_ext_src_info));

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                    (unit, sflow_encap_ext_src_info->sflow_encap_extended_src_id, &current_local_outlif, &result_type));

    
    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem
                    (unit, _SHR_CORE_ALL, sflow_encap_ext_src_info->sflow_encap_extended_src_id));

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_data_clear(unit, current_local_outlif));

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_free
                    (unit, current_local_outlif, sflow_encap_ext_src_info->sflow_encap_extended_src_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_instru_sflow_encap_extended_src_get(
    int unit,
    bcm_instru_sflow_encap_extended_src_info_t * sflow_encap_ext_src_info)
{
    int current_local_outlif;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_extended_src_get_delete_verify(unit, sflow_encap_ext_src_info));

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                    (unit, sflow_encap_ext_src_info->sflow_encap_extended_src_id, &current_local_outlif, &result_type));

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_src_raw_data_lif_get
                    (unit, sflow_encap_ext_src_info, current_local_outlif));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_sample_interface_input_system_port_to_source_system_port(
    int unit,
    bcm_gport_t system_port,
    uint32 *source_system_port)
{
    bcm_trunk_t trunk_id;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    
    rv = bcm_dnx_trunk_find(unit, 0, system_port, &trunk_id);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (rv == _SHR_E_NOT_FOUND)
    {
        
        *source_system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(system_port);
    }
    else
    {
        
        int pool, group;
        trunk_group_member_info_t member_info;

        BCM_TRUNK_ID_GROUP_GET(group, trunk_id);
        BCM_TRUNK_ID_POOL_GET(pool, trunk_id);
        member_info.flags = 0;
        member_info.system_port = system_port;
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_index_in_group_find(unit, pool, group, &member_info));
        SHR_IF_ERR_EXIT(dnx_trunk_utils_spa_encode(unit, pool, group, member_info.index, source_system_port));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_sample_interface_input_source_system_port_to_system_port(
    int unit,
    bcm_gport_t source_system_port,
    uint32 *system_port)
{
    int is_lag_member;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_trunk_system_port_is_spa(unit, source_system_port, &is_lag_member));

    if (is_lag_member == FALSE)
    {
        BCM_GPORT_SYSTEM_PORT_ID_SET(*system_port, source_system_port);
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_dnx_trunk_spa_to_system_phys_port_map_get
                        (unit, 0, source_system_port, (bcm_gport_t *) system_port));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_sample_interface_input_add(
    int unit,
    bcm_instru_sflow_sample_interface_info_t * sample_interface_info)
{
    uint32 entry_handle_id;
    uint32 port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_sample_interface_input_system_port_to_source_system_port
                    (unit, sample_interface_info->port, &port));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PEM_ACCESS_1_DB, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, port);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SFLOW_INPUT_INTERFACE, INST_SINGLE,
                                 sample_interface_info->interface);

    if (_SHR_IS_FLAG_SET(sample_interface_info->flags, BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_REPLACE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_sample_interface_output_add(
    int unit,
    bcm_instru_sflow_sample_interface_info_t * sample_interface_info)
{
    uint32 entry_handle_id;
    uint32 destination;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, sample_interface_info->port, &destination));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PEM_ACCESS_2_DB, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, destination);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SFLOW_OUTPUT_INTERFACE, INST_SINGLE,
                                 sample_interface_info->interface);

    if (_SHR_IS_FLAG_SET(sample_interface_info->flags, BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_REPLACE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_sample_interface_input_remove(
    int unit,
    bcm_instru_sflow_sample_interface_info_t * sample_interface_info)
{
    uint32 entry_handle_id;
    uint32 port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_sample_interface_input_system_port_to_source_system_port
                    (unit, sample_interface_info->port, &port));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PEM_ACCESS_1_DB, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, port);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_sample_interface_output_remove(
    int unit,
    bcm_instru_sflow_sample_interface_info_t * sample_interface_info)
{
    uint32 entry_handle_id;
    uint32 destination;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(sample_interface_info->port))
    {
        flags = ALGO_GPM_ENCODE_DESTINATION_EGRESS_MULTICAST;
    }
    else
    {
        flags = ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE;
    }

    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, flags, sample_interface_info->port, &destination));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PEM_ACCESS_2_DB, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, destination);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_sample_interface_input_get(
    int unit,
    bcm_instru_sflow_sample_interface_info_t * sample_interface_info)
{
    uint32 entry_handle_id;
    uint32 port, interface;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_instru_sflow_sample_interface_input_system_port_to_source_system_port
                    (unit, sample_interface_info->port, &port));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PEM_ACCESS_1_DB, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, port);

    
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SFLOW_INPUT_INTERFACE, INST_SINGLE, &interface);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    sample_interface_info->interface = (int) interface;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_sample_interface_output_get(
    int unit,
    bcm_instru_sflow_sample_interface_info_t * sample_interface_info)
{
    uint32 entry_handle_id;
    uint32 destination, interface;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(sample_interface_info->port))
    {
        flags = ALGO_GPM_ENCODE_DESTINATION_EGRESS_MULTICAST;
    }
    else
    {
        flags = ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE;
    }

    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, flags, sample_interface_info->port, &destination));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PEM_ACCESS_2_DB, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, destination);

    
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SFLOW_OUTPUT_INTERFACE, INST_SINGLE, &interface);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    sample_interface_info->interface = (int) interface;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_sample_interface_input_traverse(
    int unit,
    bcm_instru_sflow_sample_interface_traverse_info_t * sample_interface_traverse_info,
    bcm_instru_sflow_sample_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PEM_ACCESS_1_DB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        bcm_instru_sflow_sample_interface_info_t sample_interface_info;
        uint32 port, interface;
        uint32 system_port;

        sample_interface_info.flags = BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT;

        
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, &port));

        
        SHR_IF_ERR_EXIT(dnx_instru_sflow_sample_interface_input_source_system_port_to_system_port
                        (unit, port, &system_port));

        sample_interface_info.port = system_port;

        
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_SFLOW_INPUT_INTERFACE, INST_SINGLE, &interface));
        sample_interface_info.interface = interface;

        
        if (_SHR_IS_FLAG_SET(sample_interface_traverse_info->flags, BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_DELETE_ALL))
        {
            SHR_IF_ERR_EXIT(dnx_instru_sflow_sample_interface_input_remove(unit, &sample_interface_info));
        }
        else
        {
            SHR_IF_ERR_EXIT(cb(unit, &sample_interface_info, user_data));
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_instru_sflow_sample_interface_output_traverse(
    int unit,
    bcm_instru_sflow_sample_interface_traverse_info_t * sample_interface_traverse_info,
    bcm_instru_sflow_sample_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PEM_ACCESS_2_DB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        bcm_instru_sflow_sample_interface_info_t sample_interface_info;
        uint32 destination, interface;

        sample_interface_info.flags = BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_OUTPUT;

        
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_DESTINATION, &destination));

        SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, destination, &(sample_interface_info.port)));

        
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_SFLOW_OUTPUT_INTERFACE, INST_SINGLE, &interface));
        sample_interface_info.interface = interface;

        
        if (_SHR_IS_FLAG_SET(sample_interface_traverse_info->flags, BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_DELETE_ALL))
        {
            SHR_IF_ERR_EXIT(dnx_instru_sflow_sample_interface_output_remove(unit, &sample_interface_info));
        }
        else
        {
            SHR_IF_ERR_EXIT(cb(unit, &sample_interface_info, user_data));
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_instru_sflow_sample_interface_add(
    int unit,
    bcm_instru_sflow_sample_interface_info_t * sample_interface_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_sample_interface_verify(unit, sample_interface_info));

    if (_SHR_IS_FLAG_SET(sample_interface_info->flags, BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT))
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_sample_interface_input_add(unit, sample_interface_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_sample_interface_output_add(unit, sample_interface_info));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_instru_sflow_sample_interface_remove(
    int unit,
    bcm_instru_sflow_sample_interface_info_t * sample_interface_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_sample_interface_verify(unit, sample_interface_info));

    if (_SHR_IS_FLAG_SET(sample_interface_info->flags, BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT))
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_sample_interface_input_remove(unit, sample_interface_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_sample_interface_output_remove(unit, sample_interface_info));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_instru_sflow_sample_interface_get(
    int unit,
    bcm_instru_sflow_sample_interface_info_t * sample_interface_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_sample_interface_verify(unit, sample_interface_info));

    if (_SHR_IS_FLAG_SET(sample_interface_info->flags, BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT))
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_sample_interface_input_get(unit, sample_interface_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_sample_interface_output_get(unit, sample_interface_info));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_instru_sflow_sample_interface_traverse(
    int unit,
    bcm_instru_sflow_sample_interface_traverse_info_t * sample_interface_traverse_info,
    bcm_instru_sflow_sample_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_sample_interface_traverse_verify
                          (unit, sample_interface_traverse_info, cb, user_data));

    if (_SHR_IS_FLAG_SET(sample_interface_traverse_info->flags, BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT))
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_sample_interface_input_traverse
                        (unit, sample_interface_traverse_info, cb, user_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_sample_interface_output_traverse
                        (unit, sample_interface_traverse_info, cb, user_data));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
