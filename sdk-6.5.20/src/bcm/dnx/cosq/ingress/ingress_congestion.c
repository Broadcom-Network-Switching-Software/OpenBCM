/** \file ingress_congestion.c
 * 
 *
 * General Ingress congestion functionality for DNX. \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

/*
 * Include files.
 * {
 */
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/shrextend/shrextend_error.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ingress_congestion_access.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_reassembly.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stat.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/types.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#include "ingress_congestion.h"
#include "ingress_congestion_alloc.h"
#include "ingress_congestion_dbal.h"
#include "ingress_congestion_convert.h"
#include "rate_class_common.h"
#include "voq_rate_class.h"
#include "vsq_rate_class.h"
#include "global_vsq.h"
#include "ingress_congestion_convert.h"

/*
 * }
 */

/*
 * MACROs
 * {
 */
#define INGRESS_CONGESTION_VSQE_RSRC_MNGR_ID "VSQE_ID"
#define INGRESS_CONGESTION_VSQF_RSRC_MNGR_ID "VSQF_ID"

#define INVALID_RATE_CLASS  -1

/**
 * \brief - iterate over the relevant cores
 * if core_id is BCM_CORE_ALL and vsq is not symmetrical will iterate over all supported cores.
 * Otherwise the iteration will be over the given core.
 */
#define DNX_INGRESS_CONGESTION_VSQ_CORES_ITER(unit, vsq_type, core_id, core_index)   \
    for(core_index = ((core_id == BCM_CORE_ALL && vsq_type >= DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG) ? 0 : core_id);\
        core_index < ((core_id == BCM_CORE_ALL && vsq_type >= DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG) ?  \
                      dnx_data_device.general.nof_cores_get(unit) : (core_id + 1)); \
        core_index++)

/*
 * }
 */

/*
 * static declarations
 * {
 */
static const dbal_tables_e dnx_shared_reject_status_thresholds_table_id[] = {
    /** total bytes */
    DBAL_TABLE_INGRESS_CONG_SHARED_REJECT_STATUS_THR_WORDS,
    /** SRAM buffers */
    DBAL_TABLE_INGRESS_CONG_SHARED_REJECT_STATUS_THR_SRAM_BUFFERS,
    /** SRAM PDs */
    DBAL_TABLE_INGRESS_CONG_SHARED_REJECT_STATUS_THR_SRAM_PDS
};
/*
 * }
 */

/*
 * Functions
 * {
 */

static shr_error_e dnx_ingress_congestion_guaranteed_or_global_drop_change_verify(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e res_type,
    int guaranteed_change,
    uint32 global_drop_pool_dp[][DNX_COSQ_NOF_DP]);

static shr_error_e dnx_ingress_congestion_shared_reject_thr_calc_and_set(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e res_type,
    int guaranteed_change,
    uint32 global_drop_pool_dp[][DNX_COSQ_NOF_DP]);

static shr_error_e dnx_ingress_congestion_global_drop_pool_dp_all_get(
    int unit,
    dnx_ingress_congestion_resource_type_e res_type,
    uint32 global_drop_pool_dp[][DNX_COSQ_NOF_DP]);

/**
 * \brief -
 * verify VSQ gport and cosq
 */
static int
dnx_ingress_congestion_gport_vsq_get_verify(
    int unit,
    bcm_gport_t vsq_gport,
    bcm_cos_queue_t cosq)
{

    int category = -1;
    int traffic_class = -1;
    int connection_class = -1;
    int id;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    if (cosq != 0 && cosq != -1)
    {
        /*
         * cosq must be 0/-1 for all VSQs, even for VSQ-PG,
         * because cosq is not relevant anymore for this function.
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "VSQ 0x%x must have cosq 0 or -1\n", vsq_gport);
    }

    if (BCM_COSQ_GPORT_IS_VSQ_CT(vsq_gport))
    {
        category = BCM_COSQ_GPORT_VSQ_CT_GET(vsq_gport);
    }
    else if (BCM_COSQ_GPORT_IS_VSQ_CTTC(vsq_gport))
    {
        category = BCM_COSQ_GPORT_VSQ_CT_GET(vsq_gport);
        traffic_class = BCM_COSQ_GPORT_VSQ_TC_GET(vsq_gport);
    }
    else if (BCM_COSQ_GPORT_IS_VSQ_CTCC(vsq_gport))
    {
        category = BCM_COSQ_GPORT_VSQ_CT_GET(vsq_gport);
        connection_class = BCM_COSQ_GPORT_VSQ_CC_GET(vsq_gport);
    }
    else if (BCM_COSQ_GPORT_IS_VSQ_PP(vsq_gport))
    {
        id = BCM_COSQ_GPORT_VSQ_PP_GET(vsq_gport);
        SHR_RANGE_VERIFY(id, 0, dnx_data_ingr_congestion.vsq.vsq_d_nof_get(unit) - 1,
                         _SHR_E_PARAM, "statistic id %d is out of range\n", id);
    }
    else if (BCM_COSQ_GPORT_IS_VSQ_SRC_PORT(vsq_gport))
    {
        id = BCM_COSQ_GPORT_VSQ_SRC_PORT_GET(vsq_gport);
        SHR_RANGE_VERIFY(id, 0, dnx_data_ingr_congestion.vsq.vsq_e_nof_get(unit) - 1,
                         _SHR_E_PARAM, "src port vsq id %d is out of range\n", id);
    }
    else if (BCM_COSQ_GPORT_IS_VSQ_PG(vsq_gport))
    {
        id = BCM_COSQ_GPORT_VSQ_PG_GET(vsq_gport);
        SHR_RANGE_VERIFY(id, 0, dnx_data_ingr_congestion.vsq.vsq_f_nof_get(unit) - 1,
                         _SHR_E_PARAM, "PG vsq id %d is out of range\n", id);

    }
    else if (!BCM_COSQ_GPORT_IS_VSQ_GL(vsq_gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected vsq 0x%x\n", vsq_gport);
    }

    if (category != -1 && category != 0 && category != 2)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported category specified %d\n", category);
    }

    if (traffic_class != -1)
    {
        SHR_RANGE_VERIFY(traffic_class, 0, DNX_COSQ_NOF_TC - 1,
                         _SHR_E_PARAM, "traffic class %d is out of range\n", traffic_class);
    }

    if (connection_class != -1)
    {
        SHR_RANGE_VERIFY(connection_class, 0, dnx_data_ingr_congestion.vsq.connection_class_nof_get(unit) - 1,
                         _SHR_E_PARAM, "Connection class %d is out of range\n", connection_class);
    }

    if (BCM_COSQ_GPORT_IS_VSQ_CT(vsq_gport) ||
        BCM_COSQ_GPORT_IS_VSQ_CTTC(vsq_gport) || BCM_COSQ_GPORT_IS_VSQ_CTCC(vsq_gport))
    {
        core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(vsq_gport);
        SHR_VAL_VERIFY(core_id, BCM_CORE_ALL, _SHR_E_PARAM, "core all should be used for this VSQ\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * retrieve information from VSQ GPORT:
 * VSQ type - A-F or Global,
 * and VSQ ID - for VSQ A-F
 */
int
dnx_ingress_congestion_gport_vsq_get(
    int unit,
    bcm_gport_t vsq_gport,
    bcm_cos_queue_t cosq,
    int *core_id,
    dnx_ingress_congestion_vsq_group_e * vsq_type,
    int *vsq_id)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_gport_vsq_get_verify(unit, vsq_gport, cosq));

    *core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(vsq_gport);

    if (BCM_COSQ_GPORT_IS_VSQ_GL(vsq_gport))
    {
        *vsq_type = DNX_INGRESS_CONGESTION_VSQ_GLOBAL;
        *vsq_id = 0;
    }
    else if (BCM_COSQ_GPORT_IS_VSQ_CT(vsq_gport))
    {
        *vsq_type = DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY;
        *vsq_id = BCM_COSQ_GPORT_VSQ_CT_GET(vsq_gport);
    }
    else if (BCM_COSQ_GPORT_IS_VSQ_CTTC(vsq_gport))
    {
        *vsq_type = DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY_TRAFFIC_CLS;
        *vsq_id = (BCM_COSQ_GPORT_VSQ_CT_GET(vsq_gport) * DNX_COSQ_NOF_TC) + BCM_COSQ_GPORT_VSQ_TC_GET(vsq_gport);
    }
    else if (BCM_COSQ_GPORT_IS_VSQ_CTCC(vsq_gport))
    {
        *vsq_type = DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY_CNCTN_CLS;
        *vsq_id = (BCM_COSQ_GPORT_VSQ_CT_GET(vsq_gport) * dnx_data_ingr_congestion.vsq.connection_class_nof_get(unit))
            + BCM_COSQ_GPORT_VSQ_CC_GET(vsq_gport);
    }
    else if (BCM_COSQ_GPORT_IS_VSQ_PP(vsq_gport))
    {
        *vsq_type = DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG;
        *vsq_id = BCM_COSQ_GPORT_VSQ_PP_GET(vsq_gport);
    }
    else if (BCM_COSQ_GPORT_IS_VSQ_SRC_PORT(vsq_gport))
    {
        *vsq_type = DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT;
        *vsq_id = BCM_COSQ_GPORT_VSQ_SRC_PORT_GET(vsq_gport);
    }
    else if (BCM_COSQ_GPORT_IS_VSQ_PG(vsq_gport))
    {
        *vsq_type = DNX_INGRESS_CONGESTION_VSQ_GROUP_PG;
        *vsq_id = BCM_COSQ_GPORT_VSQ_PG_GET(vsq_gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure TC->PG mapping for the provided profile
 */
int
dnx_ingress_congestion_vsq_pg_tc_profile_hw_set(
    int unit,
    bcm_core_t core,
    int tc_pg_profile,
    dnx_ingress_congestion_vsq_tc_pg_mapping_t * tc_pg_mapping)
{
    int tc, pg;
    SHR_FUNC_INIT_VARS(unit);

    for (tc = 0; tc < DNX_COSQ_NOF_TC; tc++)
    {
        pg = tc_pg_mapping->pg[tc];
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_tc_pg_map_set(unit, core, tc_pg_profile, tc, pg));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get TC->PG mapping for the provided profile
 */
int
dnx_ingress_congestion_vsq_pg_tc_profile_hw_get(
    int unit,
    bcm_core_t core,
    int tc_pg_profile,
    dnx_ingress_congestion_vsq_tc_pg_mapping_t * tc_pg_mapping)
{
    int tc, pg;
    SHR_FUNC_INIT_VARS(unit);

    for (tc = 0; tc < DNX_COSQ_NOF_TC; tc++)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_tc_pg_map_get(unit, core, tc_pg_profile, tc, &pg));
        tc_pg_mapping->pg[tc] = pg;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get TC->PG mapping for the provided source port
 */
int
dnx_ingress_congestion_vsq_tc_pg_mapping_get(
    int unit,
    bcm_port_t src_port,
    dnx_ingress_congestion_vsq_tc_pg_mapping_t * tc_pg_mapping)
{
    uint32 cgm_input_port[DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_CGM_PRIORITIES_NOF];
    int nof_cgm_entries;
    int core_id;
    int tc_pg_profile;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the relevant core ID from the src_port*/
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core_id));

    SHR_IF_ERR_EXIT(dnx_cosq_cgm_in_port_get(unit, src_port, &nof_cgm_entries, cgm_input_port));

    /** mapping tc cosq_bitmaps to profile*/
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_port_tc_pg_profile_get(unit, core_id, cgm_input_port[0],
                                                                       &tc_pg_profile));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_pg_tc_profile_hw_get(unit, core_id, tc_pg_profile, tc_pg_mapping));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure new TC->PG mapping for the provided source port, including
 *  - update of template manager
 *  - HW update
 */
int
dnx_ingress_congestion_vsq_pg_tc_profile_exchange_and_set(
    int unit,
    bcm_port_t src_port,
    dnx_ingress_congestion_vsq_tc_pg_mapping_t * tc_pg_mapping)
{
    int old_pg_tc_profile = 0;
    int new_pg_tc_profile = 0;
    uint8 is_last = 0, is_allocated = 0;

    uint32 cgm_input_port[DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_CGM_PRIORITIES_NOF], temp_port_indx;
    int nof_cgm_entries;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the relevant core ID from the src_port*/
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core_id));

    SHR_IF_ERR_EXIT(dnx_cosq_cgm_in_port_get(unit, src_port, &nof_cgm_entries, cgm_input_port));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_port_tc_pg_profile_get(unit, core_id, cgm_input_port[0],
                                                                       &old_pg_tc_profile));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_pg_tc_mapping.exchange(unit, core_id, 0, tc_pg_mapping,
                                                                         old_pg_tc_profile, NULL,
                                                                         &new_pg_tc_profile, &is_allocated, &is_last));
    if (is_allocated)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_pg_tc_profile_hw_set(unit, core_id, new_pg_tc_profile,
                                                                        tc_pg_mapping));
    }

    for (temp_port_indx = 0; temp_port_indx < nof_cgm_entries; temp_port_indx++)
    {
        /** configure this VSQ in all lines matching the required local port */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_port_tc_pg_profile_set(unit, core_id,
                                                                           cgm_input_port[temp_port_indx],
                                                                           new_pg_tc_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get VSQ-E index and PG-Base corresponding to PP-port.
 *
 *  \param [in] unit -
 *      The unit id.
 *  \param [in] src_port -
 *      logical port.
 *  \param [out] src_port_vsq_index -
 *      VSQ-E index.
 *  \param [out] pg_base -
 *      PG-Base index.
 */
int
dnx_ingress_congestion_src_vsq_mapping_get(
    int unit,
    int src_port,
    int *src_port_vsq_index,
    int *pg_base)
{
    uint32 cgm_input_port[DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_CGM_PRIORITIES_NOF];
    int nof_cgm_entries;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the relevant core ID from the src_port*/
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core_id));

    SHR_IF_ERR_EXIT(dnx_cosq_cgm_in_port_get(unit, src_port, &nof_cgm_entries, cgm_input_port));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_port_to_src_vsq_map_get(unit, core_id, cgm_input_port[0],
                                                                        src_port_vsq_index, pg_base));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  returns whether the mapping of src port to vsq-e is enabled
 *
 *  \param [in] unit -
 *      The unit id.
 *  \param [in] src_port -
 *      logical port.
 *  \param [out] enable -
 *      Whether the mapping is enabled/disabled.
 */
int
dnx_ingress_congestion_src_vsq_mapping_enable_get(
    int unit,
    int src_port,
    int *enable)
{
    uint32 cgm_input_port[DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_CGM_PRIORITIES_NOF];
    int nof_cgm_entries;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    *enable = 0;

    /** Get the relevant core ID from the src_port*/
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core_id));

    SHR_IF_ERR_EXIT(dnx_cosq_cgm_in_port_get(unit, src_port, &nof_cgm_entries, cgm_input_port));

    if (nof_cgm_entries > 0)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_port_to_src_vsq_enable_get
                        (unit, core_id, cgm_input_port[0], enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - update VSQ SW state with created vsq E and F parameters
 */
int
dnx_ingress_congestion_port_based_vsq_sw_state_update(
    int unit,
    int core_id,
    bcm_port_t src_port,
    int src_port_vsq_index,
    int pg_base)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.port_based_vsq.vsq_e_mapping.src_port.set(unit, core_id,
                                                                                        src_port_vsq_index, src_port));
    if (pg_base != -1)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.port_based_vsq.vsq_f_mapping.src_port.set(unit, core_id, pg_base,
                                                                                            src_port));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set src port to vsq e/f
 */
int
dnx_ingress_congestion_src_vsq_mapping_set(
    int unit,
    bcm_port_t src_port,
    int src_port_vsq_index,
    int pg_base,
    int numq)
{
    uint32 cgm_input_port[DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_CGM_PRIORITIES_NOF], temp_port_indx;
    int nof_cgm_entries;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the relevant core ID from the src_port*/
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core_id));

    SHR_IF_ERR_EXIT(dnx_cosq_cgm_in_port_get(unit, src_port, &nof_cgm_entries, cgm_input_port));

    for (temp_port_indx = 0; temp_port_indx < nof_cgm_entries; temp_port_indx++)
    {
        /** configure this VSQ in all lines matching the required local port */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_port_to_src_vsq_map_set(unit, core_id,
                                                                            cgm_input_port[temp_port_indx],
                                                                            src_port_vsq_index, pg_base));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set src port to vsq e/f
 */
int
dnx_ingress_congestion_src_vsq_mapping_clear(
    int unit,
    bcm_port_t src_port)
{
    uint32 cgm_input_port[DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_CGM_PRIORITIES_NOF], temp_port_indx;
    int nof_cgm_entries;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the relevant core ID from the src_port*/
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core_id));

    SHR_IF_ERR_EXIT(dnx_cosq_cgm_in_port_get(unit, src_port, &nof_cgm_entries, cgm_input_port));

    for (temp_port_indx = 0; temp_port_indx < nof_cgm_entries; temp_port_indx++)
    {
        /** configure this VSQ in all lines matching the required local port */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_port_to_src_vsq_map_clear(unit, core_id,
                                                                              cgm_input_port[temp_port_indx]));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate vsq-e id for non phy ports
 */
shr_error_e
dnx_ingress_congestion_vsq_e_id_alloc(
    int unit,
    bcm_port_t src_port,
    int *vsq_e_id)
{
    int alloc_flags = 0;
    dnx_algo_port_info_s port_info;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, src_port, &port_info));

    /** Get the relevant core ID from the src_port */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core_id));

    /** Note - STIF port is included since at this point it has to be stif_data port */
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, FALSE, TRUE /* STIF */ , FALSE, FALSE))
    {
        /*
         * Note that Flex-E MAC, which is Ingress-TM port is *not* NIF port
         * thus we don't enter this case for Flex-E MAC port,
         * and VSQ-E for this port is allocated in else.
         */

        /** VSQ-E index is mapped 1-1 to NIF interface ID */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, src_port, 0, &core_id, vsq_e_id));
    }
    else
    {
        /** Non NIF port -- VSQ-E id should be allocated   */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.algo_res.vsqe_id.allocate_single
                        (unit, core_id, alloc_flags, NULL, vsq_e_id));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - allocate vsq-e id for non phy ports
 */
shr_error_e
dnx_ingress_congestion_vsq_e_id_dealloc(
    int unit,
    bcm_port_t src_port,
    int vsq_e_id)
{
    dnx_algo_port_info_s port_info;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, src_port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, FALSE, TRUE /* STIF */ , FALSE, FALSE))/** for NIF port, we didn't allocate, so no need to deallocate */
    {
        /** Get the core ID from the src_port */
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core_id));

        /** De-Allocate VSQ-E id */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.algo_res.vsqe_id.free_single(unit, core_id, vsq_e_id));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate vsq-f id for non phy ports
 */
shr_error_e
dnx_ingress_congestion_vsq_f_id_alloc(
    int unit,
    bcm_port_t src_port,
    int numq,
    int *vsq_f_id)
{
    int alloc_flags = 0;
    dnx_algo_port_info_s port_info;
    int core_id;
    int phy_port = 0, local_phy_index;
    uint32 vsq_f_default;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, src_port, &port_info));

    /** Get the core ID from the src_port */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core_id));

    /** Note - STIF port is included since at this point it has to be stif_data port */
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, FALSE, TRUE /* STIF */ , FALSE, FALSE))
    {
        /*
         * For NIF port, VSQ-E is hard-coded to be NIF interface ID, where NIF ETH and NIF ILKN are separated.
         * However, VSQ-F is mapped to first phy port,
         * Meaning that VSQ-F of ILKN port is placed on the same HW resources as NIF ETH
         */
        /*
         * Flex-E MAC port is not NIF port, this is not handled here but in the else clause
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, src_port, 0, &phy_port));

        local_phy_index = phy_port % dnx_data_nif.phys.nof_phys_per_core_get(unit); /** VSQ id is local per core */

        /** PG-VSQ base is mapped 1-1 to serdes (multiplied by nof PGs) */
        *vsq_f_id = BCM_COSQ_VSQ_NOF_PG * local_phy_index;

        /*
         * Validate the default VSQ-F is not allocated
         */
        vsq_f_default = dnx_data_ingr_congestion.vsq.vsq_f_default_get(unit);
        if ((vsq_f_default >= *vsq_f_id) && (vsq_f_default < (*vsq_f_id + numq)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "cannot allocate the default VSQF %d\n", vsq_f_default);
        }

    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.algo_res.
                        vsqf_id.allocate_several(unit, core_id, alloc_flags, numq, NULL, vsq_f_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - deallocate vsq-f id for non phy ports
 */
shr_error_e
dnx_ingress_congestion_vsq_f_id_dealloc(
    int unit,
    bcm_port_t src_port,
    int numq,
    int vsq_f_id)
{
    dnx_algo_port_info_s port_info;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, src_port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, FALSE, TRUE /* STIF */ , FALSE, FALSE))/** for NIF port, we didn't allocate, so no need to deallocate */
    {
        /** Get the core ID from the src_port*/
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core_id));

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.algo_res.vsqf_id.free_several(unit, core_id, numq, vsq_f_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - initiate all allocation managers for this module
 */
shr_error_e
dnx_ingress_congestion_vsq_res_mng_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    uint32 core_id;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    /*
     * VSQ-E ID resource manager
     */
    data.first_element = dnx_data_ingr_congestion.vsq.nif_vsq_e_nof_get(unit);
    data.nof_elements = dnx_data_ingr_congestion.vsq.vsq_e_nof_get(unit) - data.first_element;

    sal_strncpy(data.name, INGRESS_CONGESTION_VSQE_RSRC_MNGR_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.algo_res.vsqe_id.alloc(unit));
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.algo_res.vsqe_id.create(unit, core_id, &data, NULL));
    }

    /*
     * VSQ-F ID resource manager
     */
    if (dnx_data_ingr_congestion.vsq.non_nif_vsq_f_nof_get(unit) > 0)
    {
        data.first_element = dnx_data_nif.phys.nof_phys_per_core_get(unit) * BCM_COSQ_VSQ_NOF_PG;
        data.nof_elements = dnx_data_ingr_congestion.vsq.vsq_f_nof_get(unit) - data.first_element;

        sal_strncpy(data.name, INGRESS_CONGESTION_VSQF_RSRC_MNGR_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.algo_res.vsqf_id.alloc(unit));
        for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
        {
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.algo_res.vsqf_id.create(unit, core_id, &data, NULL));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - init VSQ reject maps
 */
static shr_error_e
dnx_ingress_congestion_rjct_map_init(
    int unit)
{
    int i;

    /*
     * Each index represent PG.Admit-Profile.
     * PG.Admit-Profile == 0 by default, and currently we don't configure
     * other PG.Admit-Profiles, so change only profile 0.
     */
    int admit_profile = 0;

    const dnxc_data_table_info_t *dnx_data_table_info;

    const dnx_data_ingr_congestion_init_vsq_words_rjct_map_t *words_reject_reasons;
    const dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *sram_reject_reasons;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap != 0)
    {
        dnx_data_table_info = dnx_data_ingr_congestion.init.vsq_words_rjct_map_info_get(unit);

        /*
         * Set reject bits for Words
         */
        for (i = 0; i < dnx_data_table_info->key_size[0]; i++)
        {

            words_reject_reasons = dnx_data_ingr_congestion.init.vsq_words_rjct_map_get(unit, i);

            SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_vsq_words_reject_map_set(unit, admit_profile,
                                                                                 words_reject_reasons, 1));
        }
    }

    /*
     * Set reject bits for SRAM-Buffers and SRAM-PDs
     */
    dnx_data_table_info = dnx_data_ingr_congestion.init.vsq_sram_rjct_map_info_get(unit);

    for (i = 0; i < dnx_data_table_info->key_size[0]; i++)
    {

        sram_reject_reasons = dnx_data_ingr_congestion.init.vsq_sram_rjct_map_get(unit, i);

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_vsq_sram_reject_map_set(unit, admit_profile,
                                                                            sram_reject_reasons, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * create reject mask for VOQ guaranteed or VSQ guaranteed
 */
static int
dnx_ingress_congestion_reject_bit_set(
    int unit,
    dnx_ingress_congestion_reject_bit_e rjct_bit,
    int rjct_value,
    dnx_ingress_congestion_reject_mask_t * reject_mask)
{
    int index;

    SHR_FUNC_INIT_VARS(unit);

    index = dnx_data_ingr_congestion.dbal.admission_bits_mapping_get(unit, rjct_bit)->index;

    if (index != -1)
    {
        /** valid reject bit */
        reject_mask->mask[index] = rjct_value;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * create reject mask for VOQ guaranteed or VSQ guaranteed
 */
static int
dnx_ingress_congestion_reject_mask_create(
    int unit,
    int is_voq,
    int is_over_voq,
    int is_over_vsq,
    dnx_ingress_congestion_reject_mask_key_t * reject_key,
    dnx_ingress_congestion_reject_mask_t * reject_mask)
{
    int words_in_guaranteed = reject_key->guaranteed.words_in_guaranteed;
    int sram_buffers_in_guaranteed = reject_key->guaranteed.sram_buffers_in_guaranteed;
    int sram_pds_in_guaranteed = reject_key->guaranteed.sram_pds_in_guaranteed;

    int in_guaranteed = words_in_guaranteed || sram_buffers_in_guaranteed || sram_pds_in_guaranteed;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(reject_mask, 0, sizeof(*reject_mask));

    /*
     * in the future we might want to make desicions with a finer resolution regarding the admission test.
     * therefore a preperation for such an occasion is already present here in the form of voq_over_voq etc
     *
     * VOQ-Over-VOQ -- Gurantee in one VOQ category loosen checks in other VOQ metric
     * VSQ-Over-VSQ -- Gurantee in one VOQ category loosen checks in other VSQ metric
     * VOQ-Over-VSQ -- Gurantee in one VOQ category loosen checks in other VSQ metric
     * VSQ-Over-VOQ -- Gurantee in one VSQ category loosen checks in other VOQ metric
     *
     * currently all the above are passed to this function as "true"
     */

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SRAM_PDS_TOTAL_SHARED,
                                                          !((is_over_voq && in_guaranteed) || sram_pds_in_guaranteed),
                                                          reject_mask));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SRAM_PDS_MAX_SIZE,
                                                          !((is_over_voq && in_guaranteed) || sram_pds_in_guaranteed),
                                                          reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SRAM_BUFFERS_TOTAL_SHARED,
                                                          !((is_over_voq && in_guaranteed) ||
                                                            sram_buffers_in_guaranteed), reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SRAM_BUFFERS_MAX_SIZE,
                                                          !((is_over_voq && in_guaranteed) ||
                                                            sram_buffers_in_guaranteed), reject_mask));
    /** Ignore total words admission tests when no DRAM is present */
    if (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap != 0)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                              DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_WORDS_TOTAL_SHARED,
                                                              !((is_over_voq && in_guaranteed) || words_in_guaranteed),
                                                              reject_mask));
    }
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_WORDS_MAX_SIZE,
                                                          !((is_over_voq && in_guaranteed) || words_in_guaranteed),
                                                          reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SYSTEM_RED, 1,
                                                          reject_mask));
    if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_latency_based_admission))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_LATENCY, 1,
                                                              reject_mask));
    }
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_WRED,
                                                          !(is_voq && in_guaranteed), reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_DRAM_BLOCK, 1,
                                                          reject_mask));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_PDS_TOTAL_SHARED,
                                                          !(is_voq ? (is_over_vsq && sram_pds_in_guaranteed) :
                                                            ((is_over_vsq && in_guaranteed) || sram_pds_in_guaranteed)),
                                                          reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_PDS_MAX_SIZE,
                                                          !(is_voq ? (is_over_vsq && sram_pds_in_guaranteed) :
                                                            ((is_over_vsq && in_guaranteed) || sram_pds_in_guaranteed)),
                                                          reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_SRAM_PDS_MAX_SIZE,
                                                          !in_guaranteed, reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_SRAM_PDS_MAX_SIZE,
                                                          !in_guaranteed, reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_SRAM_PDS_MAX_SIZE,
                                                          !in_guaranteed, reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_SRAM_PDS_MAX_SIZE,
                                                          !in_guaranteed, reject_mask));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_BUFFERS_TOTAL_SHARED,
                                                          !(is_voq ? (is_over_vsq && sram_buffers_in_guaranteed) :
                                                            (is_over_vsq && in_guaranteed) ||
                                                            sram_buffers_in_guaranteed), reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_BUFFERS_MAX_SIZE,
                                                          !(is_voq ? (is_over_vsq && sram_buffers_in_guaranteed) :
                                                            (is_over_vsq && in_guaranteed) ||
                                                            sram_buffers_in_guaranteed), reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_SRAM_BUFFERS_MAX_SIZE,
                                                          !in_guaranteed, reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_SRAM_BUFFERS_MAX_SIZE,
                                                          !in_guaranteed, reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_SRAM_BUFFERS_MAX_SIZE,
                                                          !in_guaranteed, reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_SRAM_BUFFERS_MAX_SIZE,
                                                          !in_guaranteed, reject_mask));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_WORDS_TOTAL_SHARED,
                                                          !(is_voq ? (is_over_vsq && words_in_guaranteed) :
                                                            ((is_over_vsq && in_guaranteed) || words_in_guaranteed)),
                                                          reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_WORDS_MAX_SIZE,
                                                          !(is_voq ? (is_over_vsq && words_in_guaranteed) :
                                                            ((is_over_vsq && in_guaranteed) || words_in_guaranteed)),
                                                          reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_WORDS_MAX_SIZE,
                                                          !in_guaranteed, reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_WORDS_MAX_SIZE,
                                                          !in_guaranteed, reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_WORDS_MAX_SIZE,
                                                          !in_guaranteed, reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_WORDS_MAX_SIZE,
                                                          !in_guaranteed, reject_mask));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_F_WORDS_WRED,
                                                          !in_guaranteed, reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_WORDS_WRED,
                                                          !in_guaranteed, reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_WORDS_WRED,
                                                          !in_guaranteed, reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_WORDS_WRED,
                                                          !in_guaranteed, reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_WORDS_WRED,
                                                          !in_guaranteed, reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_WORDS_WRED,
                                                          !in_guaranteed, reject_mask));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_DRAM_BDBS_OCCUPANCY,
                                                          1, reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_SRAM_BUFFERS_OCCUPANCY, 1,
                                                          reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_SRAM_PDS_OCCUPANCY, 1,
                                                          reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_DP_LEVEL, 1,
                                                          reject_mask));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Sets ITM admission tests defaults.
 *
 *   configures defaults for CGM admission tests according to pre defined logic
 *   decided by the Arch team.
 */
static int
dnx_ingress_congestion_admission_tests_default_set(
    int unit)
{
    dnx_ingress_congestion_reject_mask_t reject_mask;
    dnx_ingress_congestion_reject_mask_key_t reject_key;
    int is_voq;
    int i;
    dbal_tables_e dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * VOQ GUARANTEED and VSQ GUARANTEED
     */

    for (is_voq = 0; is_voq <= 1; is_voq++)
    {
        for (reject_key.guaranteed.words_in_guaranteed = 0; reject_key.guaranteed.words_in_guaranteed <= 1;
             reject_key.guaranteed.words_in_guaranteed++)
        {
            for (reject_key.guaranteed.sram_buffers_in_guaranteed = 0;
                 reject_key.guaranteed.sram_buffers_in_guaranteed <= 1;
                 reject_key.guaranteed.sram_buffers_in_guaranteed++)
            {
                for (reject_key.guaranteed.sram_pds_in_guaranteed = 0;
                     reject_key.guaranteed.sram_pds_in_guaranteed <= 1; reject_key.guaranteed.sram_pds_in_guaranteed++)
                {

                    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_mask_create(unit, is_voq, TRUE, TRUE,
                                                                              &reject_key, &reject_mask));

                    dbal_table_id = (is_voq ? DBAL_TABLE_INGRESS_CONG_VOQ_GUARANTEED_RJCT_MASK :
                                     DBAL_TABLE_INGRESS_CONG_VSQ_GUARANTEED_RJCT_MASK);
                    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_reject_mask_set(unit, dbal_table_id,
                                                                                &reject_key, &reject_mask));
                }
            }
        }
    }

    /*
     * VSQ Port Based
     */

    /** Set all relevant bits */
    for (i = 0; i < DNX_INGRESS_CONGESTION_REJECT_BIT_NOF; i++)
    {
        reject_mask.mask[i] = 1;
    }

    sal_memset(&reject_key, 0, sizeof(reject_key));
    reject_key.port_based_vsq.admit_profile = 0;
    reject_key.port_based_vsq.is_lossless = 0;

    /** set mask for default profile (0) non lossless */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_reject_mask_set(unit, DBAL_TABLE_INGRESS_CONG_PORT_BASED_VSQ_RJCT_MASK,
                                                                &reject_key, &reject_mask));

    /** Clear relevant bits */
    for (i = 0; i < DNX_INGRESS_CONGESTION_REJECT_BIT_DRAM_BDBS_OCCUPANCY; i++)
    {
        reject_mask.mask[i] = 0;
    }
    if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_latency_based_admission))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_LATENCY, 1,
                                                              &reject_mask));
    }
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_PDS_MAX_SIZE,
                                                          1, &reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_BUFFERS_MAX_SIZE,
                                                          1, &reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit,
                                                          DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_WORDS_MAX_SIZE,
                                                          1, &reject_mask));

    reject_key.port_based_vsq.is_lossless = 1;
    /** set mask for default profile (0) lossless */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_reject_mask_set(unit, DBAL_TABLE_INGRESS_CONG_PORT_BASED_VSQ_RJCT_MASK,
                                                                &reject_key, &reject_mask));

    /*
     * PP Admission
     */

    /** Set all bits */
    for (i = 0; i < DNX_INGRESS_CONGESTION_REJECT_BIT_NOF; i++)
    {
        reject_mask.mask[i] = 1;
    }

    sal_memset(&reject_key, 0, sizeof(reject_key));
    reject_key.pp_admit_profile = 0;

    /** set mask default PP admission profile (0)  */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_reject_mask_set(unit, DBAL_TABLE_INGRESS_CONG_PP_RJCT_MASK,
                                                                &reject_key, &reject_mask));
    if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_latency_based_admission))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_LATENCY, 1,
                                                              &reject_mask));
    }
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_WRED,
                                                          0, &reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_WORDS_WRED,
                                                          0, &reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_WORDS_WRED,
                                                          0, &reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_WORDS_WRED,
                                                          0, &reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_WORDS_WRED,
                                                          0, &reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_WORDS_WRED,
                                                          0, &reject_mask));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_reject_bit_set(unit, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_F_WORDS_WRED,
                                                          0, &reject_mask));

    reject_key.pp_admit_profile = 1;
    /** set mask for ECN/ECI PP admission profile (1)  */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_reject_mask_set(unit, DBAL_TABLE_INGRESS_CONG_PP_RJCT_MASK,
                                                                &reject_key, &reject_mask));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - init TAR fifo size
 */
static shr_error_e
dnx_ingress_congestion_dram_block_defaults_set(
    int unit)
{
    int dp;
    uint32 dram_block_mask[DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_NOF] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** Init DRAM block mask */

    /** default value is 0xA2E */
    dram_block_mask[DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_GLBL_SRAM_PDBS] = 1;
    dram_block_mask[DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_GLBL_SRAM_BUFFERS] = 1;
    dram_block_mask[DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_GLBL_DRAM_BDBS] = 1;
    dram_block_mask[DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VOQ_WORDS_MAX_QSIZE] = 1;
    dram_block_mask[DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VOQ_SRAM_BUFFERS_MAX_QSIZE] = 1;
    dram_block_mask[DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VOQ_SRAM_PDS_MAX_QSIZE] = 1;

    for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_dram_block_mask_set(unit, dp, dram_block_mask));
    }

    if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_soft_dram_block_supported))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_dram_soft_block_defaults_set(unit));
        /** set all rate classes to be DRAM block eligible */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_dram_block_eligible_set_all(unit, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - init TAR fifo size
 */
static shr_error_e
dnx_ingress_congestion_fifo_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_fifo_size_set(unit, dnx_data_ingr_congestion.init.fifo_size_get(unit)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - init mirror on drop defaults (rate and burst)
 */
static shr_error_e
dnx_ingress_congestion_mirror_on_drop_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_mirror_on_drop_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - do device specific HW initializations
 */
shr_error_e
dnx_ingress_congestion_hw_init(
    int unit)
{
    int core;
    dnx_ingress_congestion_statistics_info_t stats;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Override PP port with reassembly context
     */
    if (dnx_data_ingr_congestion.config.feature_get(unit,
                                                    dnx_data_ingr_congestion_config_pp_port_by_reassembly_overwrite))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_override_pp_port_with_reassembly_context_enable_set(unit, TRUE));
    }

    /*
     * programmable counters init configuration (all ones)
     * this is required in order to trigger the counters. if this is not set, the counters don't count.
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_programmable_counter_init_all_ones_config_set(unit));

    /*
     * Read CGM minimum occupancy registers, in order for future reads to be correct
     * (the registers set to maximum on read, but first reads are always 0).
     */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_free_resource_counter_get(unit, core, &stats));
    }

    /*
     * configure global DP discard to always drop DP3
     */
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_discard_set(unit, BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK));

    /** configure init settings */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_init_settings_config_set(unit));

    /** clear global VSQ resource allocation -- actual resource allocation is done in tune */
    SHR_IF_ERR_EXIT(dnx_global_vsq_resource_alloc_clear(unit));

    /** Configure Admission tests defaults */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_rjct_map_init(unit));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_admission_tests_default_set(unit));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_block_defaults_set(unit));

    /** Configure TAR FIFO size */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_fifo_init(unit));

    /** Configure mirror on drop defaults */
    if (dnx_data_ingr_congestion.mirror_on_drop.feature_get(unit, dnx_data_ingr_congestion_mirror_on_drop_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_mirror_on_drop_init(unit));
    }
    if (dnx_data_dram.hbm.start_disabled_get(unit))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_usage_stop(unit));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief -  Set global reject drop thresholds
 *
 * Global drop thresholds by DP configuration is not accessable by API.
 * initialize them from module init
 */
static shr_error_e
dnx_ingress_congestion_global_drop_thresholds_init(
    int unit)
{
    bcm_gport_t glb_vsq_gport;
    bcm_cosq_vsq_info_t vsq_info;
    int dp;
    bcm_cosq_static_threshold_info_t thresh_info;
    bcm_cosq_static_threshold_t threshold;
    uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&vsq_info, 0x0, sizeof(vsq_info));
    vsq_info.flags = BCM_COSQ_VSQ_GL;
    vsq_info.core_id = BCM_CORE_ALL;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_vsq_create(unit, &vsq_info, &glb_vsq_gport));

    /** set threshold settings */
    for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
    {
        sal_memset(&thresh_info, 0x0, sizeof(thresh_info));
        sal_memset(&threshold, 0x0, sizeof(threshold));

        /*
         * Free OCB Buffers drop threshold
         */
        thresh_info.gport = glb_vsq_gport;
        thresh_info.threshold_action = bcmCosqThreshActionDrop;
        thresh_info.resource_type = bcmResourceOcbBuffers;
        thresh_info.index = BCM_COSQ_TH_INDEX_DP_SET(dp);
        thresh_info.is_free_resource = 1;

        threshold.thresh_set = dnx_data_ingr_congestion.init.dp_global_sram_buffer_drop_get(unit, dp)->set_threshold;
        threshold.thresh_clear =
            dnx_data_ingr_congestion.init.dp_global_sram_buffer_drop_get(unit, dp)->clear_threshold;

        SHR_IF_ERR_EXIT(dnx_cosq_gport_static_threshold_internal_set(unit, flags, &thresh_info, &threshold));

        sal_memset(&thresh_info, 0x0, sizeof(thresh_info));
        sal_memset(&threshold, 0x0, sizeof(threshold));

        /*
         * Free OCB Packet Descriptor Buffers drop threshold
         */
        thresh_info.gport = glb_vsq_gport;
        thresh_info.threshold_action = bcmCosqThreshActionDrop;
        thresh_info.resource_type = bcmResourceOcbPacketDescriptorsBuffers;
        thresh_info.index = BCM_COSQ_TH_INDEX_DP_SET(dp);
        thresh_info.is_free_resource = 1;

        threshold.thresh_set = dnx_data_ingr_congestion.init.dp_global_sram_pdb_drop_get(unit, dp)->set_threshold;
        threshold.thresh_clear = dnx_data_ingr_congestion.init.dp_global_sram_pdb_drop_get(unit, dp)->clear_threshold;

        SHR_IF_ERR_EXIT(dnx_cosq_gport_static_threshold_internal_set(unit, flags, &thresh_info, &threshold));

        sal_memset(&thresh_info, 0x0, sizeof(thresh_info));
        sal_memset(&threshold, 0x0, sizeof(threshold));

        /*
         * Free DRAM Bundle Descriptor Buffers drop threshold
         */
        thresh_info.gport = glb_vsq_gport;
        thresh_info.threshold_action = bcmCosqThreshActionDrop;
        thresh_info.resource_type = bcmResourceDramBundleDescriptorsBuffers;
        thresh_info.index = BCM_COSQ_TH_INDEX_DP_SET(dp);
        thresh_info.is_free_resource = 1;

        threshold.thresh_set = dnx_data_ingr_congestion.init.dp_global_dram_bdb_drop_get(unit, dp)->set_threshold;
        threshold.thresh_clear = dnx_data_ingr_congestion.init.dp_global_dram_bdb_drop_get(unit, dp)->clear_threshold;

        SHR_IF_ERR_EXIT(dnx_cosq_gport_static_threshold_internal_set(unit, flags, &thresh_info, &threshold));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - read SOC properties and configure to HW
 */
static shr_error_e
dnx_ingress_congestion_soc_properties_init(
    int unit)
{
    int dp;
    uint32 is_dp_guarantee_over_admit;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Admission preferences - Guarantee over Admit
     * Get values from Soc property for DP 0-2. DP3 is always set to ADNIT_OVER_GUARANTEE
     */
    for (dp = 0; dp < DNX_COSQ_NOF_DP - 1; ++dp)
    {
        is_dp_guarantee_over_admit =
            dnx_data_ingr_congestion.info.admission_preferences_get(unit, dp)->is_guarantee_over_admit;
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_guarantee_over_admit_set(unit, dp, is_dp_guarantee_over_admit));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_ingress_congestion_init(
    int unit)
{
    dnx_ingress_congestion_vsq_rate_class_info_t vsq_rate_class_default_data;
    dnx_ingress_congestion_vsq_rate_class_info_t vsq_exact_rate_class_info;
    int core_id;
    dnx_ingress_congestion_vsq_group_e vsq_type;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init swstate db
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.init(unit)); /** all swstate variables are cleared by this call */

    /*
     * initialize elements in ingress congestion SW state
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.dram_bound_mutex.create(unit));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.fadt_mutex.create(unit));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.dram_thresholds.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.fadt_tail_drop_thresholds.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.dram_in_use.set(unit, TRUE));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.voq_rate_class.is_created.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.voq_rate_class.ref_count.alloc(unit));

    /**
     * allocate port_based_vsq arrays in sw_state db.
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.port_based_vsq.alloc(unit));
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        /** alloc VSQ-E mapping */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.port_based_vsq.vsq_e_mapping.alloc(unit, core_id));

        /** alloc VSQ-F mapping */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.port_based_vsq.vsq_f_mapping.alloc(unit, core_id));
    }

    /*
     * Init Resource Manager
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_res_mng_init(unit));

    /** Init VSQ rate classes - template manager */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_alloc_vsq_rate_class_template_mngr_init(unit));

    /** Init VSQ rate classes - HW */
    for (vsq_type = DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY; vsq_type < DNX_INGRESS_CONGESTION_VSQ_GROUP_NOF; vsq_type++)
    {
        SHR_IF_ERR_EXIT(dnx_vsq_rate_class_default_data_get(unit, vsq_type, &vsq_rate_class_default_data));
        SHR_IF_ERR_EXIT(dnx_vsq_rate_class_hw_set(unit, BCM_CORE_ALL, 0, vsq_type,
                                                  &vsq_rate_class_default_data, &vsq_exact_rate_class_info));
    }

    /** Init VSQ-F PG-TC bitmap */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_alloc_vsq_pg_tc_mapping_profile_create(unit));

    /** SOC properties configuration */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_soc_properties_init(unit));

    /** Device specific HW initializations */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_hw_init(unit));

    /** set default global drop thresholds */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_drop_thresholds_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_ingress_congestion_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit
     */

    SHR_FUNC_EXIT;
}

/*
 * \brief - cleanup source VSQ when a port is removed
 */
shr_error_e
dnx_ingress_congestion_port_remove(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_info_s port_info;
    bcm_gport_t gport;
    dnx_algo_port_tdm_mode_e tdm_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, port, &tdm_mode));

        if (tdm_mode != DNX_ALGO_PORT_TDM_MODE_BYPASS || DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, FALSE))
        {
            /** when a port is removed, src base VSQs for that port should be removed */
            BCM_GPORT_LOCAL_SET(gport, port);
            /** VSQ destroy does nothing if VSQ is not created */
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_vsq_destroy(unit, gport));
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/* --------------------------------------------------------------------------------------------------------- */
/**
 * \brief - translate cosq_resource as provide by BCM API to ingress threshold type
 */
int
dnx_ingress_congestion_cosq_resource_to_ingress_resource(
    int unit,
    bcm_cosq_resource_t cosq_resource,
    dnx_ingress_congestion_resource_type_e * ingress_resource)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (cosq_resource)
    {
        case bcmResourceBytes:
            *ingress_resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES;
            break;
        case bcmResourceOcbBuffers:
            *ingress_resource = DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS;
            break;
        case bcmResourceOcbPacketDescriptors:
            *ingress_resource = DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS;
            break;
        default:
            *ingress_resource = DNX_INGRESS_CONGESTION_RESOURCE_INVALID;
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected resource type %d\n", cosq_resource);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify arguments of vsq_shared_free_drop_threshold
 */
static int
dnx_ingress_congestion_vsq_shared_free_drop_threshold_arg_verify(
    int unit,
    int core,
    int pool_id,
    int dp,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    dnx_ingress_congestion_resource_type_e resource_type;
    uint32 global_drop_pool_dp[DNX_DATA_MAX_INGR_CONGESTION_VSQ_POOL_NOF][DNX_COSQ_NOF_DP];

    SHR_FUNC_INIT_VARS(unit);

    if (thresh_info->is_free_resource != TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "This threshold is configured for free resources, is_free_resource must be set to 1");

    }

    SHR_RANGE_VERIFY(pool_id, 0, dnx_data_ingr_congestion.vsq.pool_nof_get(unit) - 1,
                     _SHR_E_PARAM, "Invalid pool id value %d", pool_id);

    SHR_RANGE_VERIFY(dp, 0, DNX_COSQ_NOF_DP - 1, _SHR_E_PARAM, "Invalid dp parameter %d", dp);

    switch (thresh_info->resource_type)
    {

        case bcmResourceBytes:
        case bcmResourceOcbBuffers:
        case bcmResourceOcbPacketDescriptors:
            /** expected resources */

            /*
             * threshold pointer is verified in top level API
             * NULL is passed in threshold in get function
             */
            if (threshold != NULL)
            {
                /** set API */
                SHR_IF_ERR_EXIT(dnx_ingress_congestion_cosq_resource_to_ingress_resource(unit,
                                                                                         thresh_info->resource_type,
                                                                                         &resource_type));
                SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit, resource_type, "thresh_set",
                                                                        threshold->thresh_set));

                SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit, resource_type, "thresh_clear",
                                                                        threshold->thresh_clear));

                SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_drop_pool_dp_all_get(unit, resource_type,
                                                                                   global_drop_pool_dp));
                global_drop_pool_dp[pool_id][dp] = threshold->thresh_set;

                SHR_IF_ERR_EXIT(dnx_ingress_congestion_guaranteed_or_global_drop_change_verify
                                (unit, core, resource_type, 0, global_drop_pool_dp));
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected resource type %d\n", thresh_info->resource_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set threshold on Shared VSQ Pool
 */
static int
dnx_ingress_congestion_vsq_shared_free_drop_threshold_set(
    int unit,
    int core,
    int pool_id,
    int dp,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    dnx_ingress_congestion_resource_type_e resource_type;
    uint32 global_drop_pool_dp[DNX_DATA_MAX_INGR_CONGESTION_VSQ_POOL_NOF][DNX_COSQ_NOF_DP];

    /** Per DP * pool id */
    SHR_FUNC_INIT_VARS(unit);

    /** Verify API arguments */
    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_vsq_shared_free_drop_threshold_arg_verify
                          (unit, core, pool_id, dp, thresh_info, threshold));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_cosq_resource_to_ingress_resource(unit,
                                                                             thresh_info->resource_type,
                                                                             &resource_type));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_vsq_shared_threshold_hw_set(unit, pool_id, dp,
                                                                            resource_type,
                                                                            threshold->thresh_set,
                                                                            threshold->thresh_clear));
    /**
     * update VOQ shared size threshold according to new threshold
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_drop_pool_dp_all_get(unit, resource_type, global_drop_pool_dp));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_shared_reject_thr_calc_and_set
                    (unit, core, resource_type, 0, global_drop_pool_dp));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get threshold on Shared VSQ Pool
 */
static int
dnx_ingress_congestion_vsq_shared_free_drop_threshold_get(
    int unit,
    int core,
    int pool_id,
    int dp,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    dnx_ingress_congestion_resource_type_e resource_type;
    /** Per DP * pool id */
    SHR_FUNC_INIT_VARS(unit);

    /** Verify API arguments */
    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_vsq_shared_free_drop_threshold_arg_verify
                          (unit, core, pool_id, dp, thresh_info, NULL));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_cosq_resource_to_ingress_resource(unit,
                                                                             thresh_info->resource_type,
                                                                             &resource_type));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_vsq_shared_threshold_hw_get(unit, pool_id, dp,
                                                                            resource_type,
                                                                            (uint32 *) &threshold->thresh_set,
                                                                            (uint32 *) &threshold->thresh_clear));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify arguments for global_free_sram_drop_threshold
 */
static int
dnx_ingress_congestion_global_free_sram_drop_threshold_arg_verify(
    int unit,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    if (thresh_info->is_free_resource != TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "This threshold is configured for free resources, is_free_resource must be set to 1");

    }

    /** Per DP or per TC */
    switch (index_type)
    {
        case DNX_COSQ_THRESHOLD_INDEX_DP:
            SHR_RANGE_VERIFY(index_info->dp, 0, DNX_COSQ_NOF_DP - 1, _SHR_E_PARAM,
                             "Invalid dp parameter %d\n", index_info->dp);
            break;
        case DNX_COSQ_THRESHOLD_INDEX_TC:
            SHR_RANGE_VERIFY(index_info->tc, 0, DNX_COSQ_NOF_TC - 1, _SHR_E_PARAM,
                             "Invalid tc parameter %d\n", index_info->tc);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid index type %d\n", index_type);
            break;
    }

    switch (thresh_info->resource_type)
    {
        case bcmResourceOcbBuffers:
            if (threshold != NULL)
            {
                /** called by set API */
                SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit,
                                                                        DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS,
                                                                        "bcmResourceOcbBuffers thresh_set",
                                                                        threshold->thresh_set));

                SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit,
                                                                        DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS,
                                                                        "bcmResourceOcbBuffers thresh_clear",
                                                                        threshold->thresh_clear));
            }
            break;
        case bcmResourceOcbPacketDescriptorsBuffers:
            if (threshold != NULL)
            {
                /** set API */
                if (threshold->thresh_set > dnx_data_ingr_congestion.info.max_sram_pdbs_get(unit))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "bcmResourceOcbPacketDescriptorsBuffers set threshold too high (%d)\n",
                                 threshold->thresh_set);
                }
                if (threshold->thresh_clear > dnx_data_ingr_congestion.info.max_sram_pdbs_get(unit))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "bcmResourceOcbPacketDescriptorsBuffers clear threshold too high (%d)\n",
                                 threshold->thresh_clear);
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected resource type %d\n", thresh_info->resource_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - translate cosq_resource as provide by BCM API to global sram resource type
 */
static int
dnx_ingress_congestion_cosq_resource_to_global_sram_resource(
    int unit,
    bcm_cosq_resource_t cosq_resource,
    dnx_ingress_congestion_global_sram_resource_t * global_sram_resource)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (cosq_resource)
    {
        case bcmResourceOcbPacketDescriptorsBuffers:
            *global_sram_resource = DNX_INGRESS_CONGESTION_GLOBAL_SRAM_PACKET_DESCRIPTORS_BUFFERS;
            break;
        case bcmResourceOcbBuffers:
            *global_sram_resource = DNX_INGRESS_CONGESTION_GLOBAL_SRAM_BUFFERS;
            break;
        default:
            *global_sram_resource = DNX_INGRESS_CONGESTION_GLOBAL_SRAM_RESOURCE_INVALID;
            break;
    }

/** exit: */
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set Global Drop threshold according to free SRAM resources
 */
static int
dnx_ingress_congestion_global_free_sram_drop_threshold_set(
    int unit,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    dnx_ingress_congestion_global_sram_resource_t resource_type;

    /** Per DP or per TC */
    SHR_FUNC_INIT_VARS(unit);

    /** Verify API arguments */
    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_global_free_sram_drop_threshold_arg_verify(unit,
                                                                                            index_type, index_info,
                                                                                            thresh_info, threshold));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_cosq_resource_to_global_sram_resource(unit,
                                                                                 thresh_info->resource_type,
                                                                                 &resource_type));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_global_free_sram_threshold_hw_set
                    (unit,
                     (index_type == DNX_COSQ_THRESHOLD_INDEX_DP ? index_info->dp : index_info->tc),
                     ( /** is_dp */ index_type == DNX_COSQ_THRESHOLD_INDEX_DP),
                     resource_type, threshold->thresh_set, threshold->thresh_clear));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get Global Drop threshold according to free SRAM resources
 */
static int
dnx_ingress_congestion_global_free_sram_drop_threshold_get(
    int unit,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    dnx_ingress_congestion_global_sram_resource_t resource_type;

    /** Per DP or per TC */
    SHR_FUNC_INIT_VARS(unit);

    /** Verify API arguments */
    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_global_free_sram_drop_threshold_arg_verify(unit,
                                                                                            index_type, index_info,
                                                                                            thresh_info, NULL));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_cosq_resource_to_global_sram_resource(unit,
                                                                                 thresh_info->resource_type,
                                                                                 &resource_type));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_global_free_sram_threshold_hw_get
                    (unit,
                     (index_type == DNX_COSQ_THRESHOLD_INDEX_DP ? index_info->dp : index_info->tc),
                     ( /** is_dp */ index_type == DNX_COSQ_THRESHOLD_INDEX_DP),
                     resource_type, (uint32 *) &threshold->thresh_set, (uint32 *) &threshold->thresh_clear));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Verify arguments for global_free_dram_drop_threshold
 */
static int
dnx_ingress_congestion_global_free_dram_drop_threshold_arg_verify(
    int unit,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    if (thresh_info->is_free_resource != TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "This threshold is configured for free resources, is_free_resource must be set to 1");

    }

    /** Per DP or per TC */
    switch (index_type)
    {
        case DNX_COSQ_THRESHOLD_INDEX_DP:
            SHR_RANGE_VERIFY(index_info->dp, 0, DNX_COSQ_NOF_DP - 1, _SHR_E_PARAM,
                             "Invalid dp parameter %d\n", index_info->dp);
            break;
        case DNX_COSQ_THRESHOLD_INDEX_TC:
            SHR_RANGE_VERIFY(index_info->tc, 0, DNX_COSQ_NOF_TC - 1, _SHR_E_PARAM,
                             "Invalid tc parameter %d\n", index_info->tc);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid index type %d\n", index_type);
            break;
    }

    switch (thresh_info->resource_type)
    {
        case bcmResourceDramBundleDescriptorsBuffers:
            /** expected resource */
            if (threshold != NULL)
            {
                /** set API */
                if (threshold->thresh_set > dnx_data_ingr_congestion.info.max_dram_bdbs_get(unit))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "bcmResourceOcbPacketDescriptorsBuffers set threshold too high (%d)",
                                 threshold->thresh_set);
                }
                if (threshold->thresh_clear > dnx_data_ingr_congestion.info.max_dram_bdbs_get(unit))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "bcmResourceOcbPacketDescriptorsBuffers clear threshold too high (%d)",
                                 threshold->thresh_clear);
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected resource type %d", thresh_info->resource_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set Global threshold on Free DRAM resources
 */
static int
dnx_ingress_congestion_global_free_dram_drop_threshold_set(
    int unit,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    /** Per DP or per TC */
    SHR_FUNC_INIT_VARS(unit);

    /** Verify API arguments */
    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_global_free_dram_drop_threshold_arg_verify(unit,
                                                                                            index_type, index_info,
                                                                                            thresh_info, threshold));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_global_free_dram_threshold_hw_set
                    (unit,
                     (index_type == DNX_COSQ_THRESHOLD_INDEX_DP ? index_info->dp : index_info->tc),
                     ( /** is_dp */ index_type == DNX_COSQ_THRESHOLD_INDEX_DP),
                     threshold->thresh_set, threshold->thresh_clear));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Global threshold on Free DRAM resources
 */
static int
dnx_ingress_congestion_global_free_dram_drop_threshold_get(
    int unit,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    /** Per DP or per TC */
    SHR_FUNC_INIT_VARS(unit);

    /** Verify API arguments */
    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_global_free_dram_drop_threshold_arg_verify(unit,
                                                                                            index_type, index_info,
                                                                                            thresh_info, NULL));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_global_free_dram_threshold_hw_get
                    (unit,
                     (index_type == DNX_COSQ_THRESHOLD_INDEX_DP ? index_info->dp : index_info->tc),
                     ( /** is_dp */ index_type == DNX_COSQ_THRESHOLD_INDEX_DP),
                     (uint32 *) &threshold->thresh_set, (uint32 *) &threshold->thresh_clear));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set Global drop threshold
 */
int
dnx_ingress_congestion_vsq_gl_drop_threshold_set(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_VAL_VERIFY(core_id, BCM_CORE_ALL, _SHR_E_PARAM, "This API supports BCM_CORE_ALL value only\n");

    switch (index_type)
    {
        case DNX_COSQ_THRESHOLD_INDEX_POOL_DP:
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_shared_free_drop_threshold_set(unit,
                                                                                      core_id,
                                                                                      index_info->pool_id,
                                                                                      index_info->dp,
                                                                                      thresh_info, threshold));
            break;
        case DNX_COSQ_THRESHOLD_INDEX_DP:
        case DNX_COSQ_THRESHOLD_INDEX_TC:
            switch (thresh_info->resource_type)
            {
                case bcmResourceOcbBuffers:
                case bcmResourceOcbPacketDescriptorsBuffers:
                    /**    Global Free SRAM thresh */
                    SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_free_sram_drop_threshold_set(unit,
                                                                                               index_type, index_info,
                                                                                               thresh_info, threshold));
                    break;
                case bcmResourceDramBundleDescriptorsBuffers:
                    /**    Global Free DRAM thresh */
                    SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_free_dram_drop_threshold_set(unit,
                                                                                               index_type, index_info,
                                                                                               thresh_info, threshold));
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid resource type %d", thresh_info->resource_type);
                    break;
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid index type %d\n", index_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Global drop threshold
 */
int
dnx_ingress_congestion_vsq_gl_drop_threshold_get(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_VAL_VERIFY(core_id, BCM_CORE_ALL, _SHR_E_PARAM, "This API supports BCM_CORE_ALL value only\n");

    switch (index_type)
    {
        case DNX_COSQ_THRESHOLD_INDEX_POOL_DP:
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_shared_free_drop_threshold_get(unit, core_id,
                                                                                      /** indexes */
                                                                                      index_info->pool_id,
                                                                                      index_info->dp,
                                                                                      thresh_info, threshold));
            break;
        case DNX_COSQ_THRESHOLD_INDEX_DP:
        case DNX_COSQ_THRESHOLD_INDEX_TC:
            switch (thresh_info->resource_type)
            {
                case bcmResourceOcbBuffers:
                case bcmResourceOcbPacketDescriptorsBuffers:
                    /**    Global Free SRAM thresh */
                    SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_free_sram_drop_threshold_get(unit,
                                                                                               index_type, index_info,
                                                                                               thresh_info, threshold));
                    break;
                case bcmResourceDramBundleDescriptorsBuffers:
                    /**    Global Free DRAM thresh */
                    SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_free_dram_drop_threshold_get(unit,
                                                                                               index_type, index_info,
                                                                                               thresh_info, threshold));
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid resource type %d\n", thresh_info->resource_type);
                    break;
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid index type %d\n", index_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify arguments for cmd_fifo_sram_free_drop_threshold
 */
static int
dnx_ingress_congestion_cmd_fifo_sram_free_drop_threshold_arg_verify(
    int unit,
    bcm_cast_t cast,
    bcm_cosq_ingress_forward_priority_t priority,
    bcm_cosq_forward_decision_type_t fwd_action,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    if (thresh_info->is_free_resource != TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "This threshold is configured for free resources, is_free_resource must be set to 1");

    }

    switch (cast)
    {
        case bcmCastUC:
            switch (priority)
            {
                case bcmCosqIngrFwdPriorityNone:
                    /** expected value */
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid cast (%d) priority (%d) combination", cast, priority);
                    break;
            }
            break;
        case bcmCastMC:
            switch (priority)
            {
                case bcmCosqIngrFwdPriorityLow:
                case bcmCosqIngrFwdPriorityHigh:
                    /** expected value */
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid cast (%d) priority (%d) combination", cast, priority);
                    break;
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid cast %d", cast);
            break;

    }

    switch (thresh_info->resource_type)
    {
        case bcmResourceOcbBuffers:
            if (threshold != NULL)
            {
                /** set API */
                if (threshold->thresh_set > dnx_data_ingr_congestion.info.resource_get(unit,
                                                                                       DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->max)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "bcmResourceOcbBuffers set threshold too high (%d)",
                                 threshold->thresh_set);
                }
                if (threshold->thresh_clear > dnx_data_ingr_congestion.info.resource_get(unit,
                                                                                         DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->max)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "bcmResourceOcbBuffers clear threshold too high (%d)",
                                 threshold->thresh_clear);
                }
            }
            break;
        case bcmResourceOcbPacketDescriptorsBuffers:
            if (threshold != NULL)
            {
                /** set API */
                if (threshold->thresh_set > dnx_data_ingr_congestion.info.max_sram_pdbs_get(unit))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "bcmResourceOcbPacketDescriptorsBuffers set threshold too high (%d)",
                                 threshold->thresh_set);
                }
                if (threshold->thresh_clear > dnx_data_ingr_congestion.info.max_sram_pdbs_get(unit))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "bcmResourceOcbPacketDescriptorsBuffers clear threshold too high (%d)",
                                 threshold->thresh_clear);
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected resource type %d", thresh_info->resource_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set SRAM Free drop threshold on CMD FIFO
 */
static int
dnx_ingress_congestion_cmd_fifo_sram_free_drop_threshold_set(
    int unit,
    bcm_cast_t cast,
    bcm_cosq_ingress_forward_priority_t priority,
    bcm_cosq_forward_decision_type_t fwd_action,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    dnx_ingress_congestion_global_sram_resource_t resource_type;
    dnx_ingress_congestion_cmd_fifo_type_t fifo_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify API arguments */
    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_cmd_fifo_sram_free_drop_threshold_arg_verify(unit,
                                                                                              cast, priority,
                                                                                              fwd_action, thresh_info,
                                                                                              threshold));

    fifo_id = (cast == bcmCastUC ? DNX_INGRESS_CONGESTION_CMD_FIFO_UC
               : (priority ==
                  bcmCosqIngrFwdPriorityLow ? DNX_INGRESS_CONGESTION_CMD_FIFO_MCL :
                  DNX_INGRESS_CONGESTION_CMD_FIFO_MCH));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_cosq_resource_to_global_sram_resource(unit,
                                                                                 thresh_info->resource_type,
                                                                                 &resource_type));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_cmd_fifo_sram_reject_threshold_hw_set(unit,
                                                                                      fifo_id, fwd_action,
                                                                                      resource_type,
                                                                                      threshold->thresh_set,
                                                                                      threshold->thresh_clear));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SRAM Free drop threshold on CMD FIFO
 */
static int
dnx_ingress_congestion_cmd_fifo_sram_free_drop_threshold_get(
    int unit,
    bcm_cast_t cast,
    bcm_cosq_ingress_forward_priority_t priority,
    bcm_cosq_forward_decision_type_t fwd_action,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    dnx_ingress_congestion_global_sram_resource_t resource_type;
    dnx_ingress_congestion_cmd_fifo_type_t fifo_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify API arguments */
    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_cmd_fifo_sram_free_drop_threshold_arg_verify(unit,
                                                                                              cast, priority,
                                                                                              fwd_action, thresh_info,
                                                                                              NULL));

    fifo_id = (cast == bcmCastUC ? DNX_INGRESS_CONGESTION_CMD_FIFO_UC
               : (priority ==
                  bcmCosqIngrFwdPriorityLow ? DNX_INGRESS_CONGESTION_CMD_FIFO_MCL :
                  DNX_INGRESS_CONGESTION_CMD_FIFO_MCH));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_cosq_resource_to_global_sram_resource(unit,
                                                                                 thresh_info->resource_type,
                                                                                 &resource_type));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_cmd_fifo_sram_reject_threshold_hw_get
                    (unit, fifo_id, fwd_action, resource_type,
                     (uint32 *) &threshold->thresh_set, (uint32 *) &threshold->thresh_clear));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set global Device drop threshold
 */
static int
dnx_ingress_congestion_device_gl_drop_threshold_set(
    int unit,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (index_type)
    {
        case DNX_COSQ_THRESHOLD_INDEX_CAST_PRIO_FDWACT:
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_cmd_fifo_sram_free_drop_threshold_set(unit,
                                                                                         index_info->cast,
                                                                                         index_info->priority,
                                                                                         index_info->fwd_action,
                                                                                         thresh_info, threshold));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid index type %d\n", index_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get global Device drop threshold
 */
static int
dnx_ingress_congestion_device_gl_drop_threshold_get(
    int unit,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (index_type)
    {
        case DNX_COSQ_THRESHOLD_INDEX_CAST_PRIO_FDWACT:
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_cmd_fifo_sram_free_drop_threshold_get(unit,
                                                                                         index_info->cast,
                                                                                         index_info->priority,
                                                                                         index_info->fwd_action,
                                                                                         thresh_info, threshold));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid index type %d\n", index_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set global Device static (non FADT) threshold
 */
int
dnx_ingress_congestion_global_device_static_threshold_set(
    int unit,
    uint32 flags,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (thresh_info->threshold_action)
    {
        case bcmCosqThreshActionDrop:
            /** VSQ gl tail drop thresholds */
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_device_gl_drop_threshold_set(unit,
                                                                                index_type, index_info,
                                                                                thresh_info, threshold));
            break;
        case bcmCosqThreshActionMarkCongestion:
            /** VOQ gl Mark Congestion thresholds set*/
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_ecn_marking_threshold_set(unit, BCM_CORE_ALL,
                                                                                    index_type, index_info,
                                                                                    thresh_info, threshold));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported threshold_action %d\n", thresh_info->threshold_action);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get global Device static (non FADT) threshold
 */
int
dnx_ingress_congestion_global_device_static_threshold_get(
    int unit,
    uint32 flags,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (thresh_info->threshold_action)
    {
        case bcmCosqThreshActionDrop:
            /** VSQ gl tail drop thresholds */
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_device_gl_drop_threshold_get(unit,
                                                                                index_type, index_info,
                                                                                thresh_info, threshold));
            break;
        case bcmCosqThreshActionMarkCongestion:
            /** VOQ gl Mark Congestion thresholds get*/
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_ecn_marking_threshold_get(unit, BCM_CORE_ALL,
                                                                                    index_type, index_info,
                                                                                    thresh_info, threshold));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported threshold_action %d\n", thresh_info->threshold_action);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create VSQ gport from VSQ group and index
 */
int
dnx_ingress_congestion_vsq_gport_get(
    int unit,
    int core,
    int vsq_group,
    int vsq_group_idx,
    bcm_gport_t * vsq_gport)
{
    int category, traffic_class, connection_class;

    SHR_FUNC_INIT_VARS(unit);

    switch (vsq_group)
    {

        case DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY:
            BCM_COSQ_GPORT_VSQ_CT_SET(*vsq_gport, core, 0, vsq_group_idx);
            break;
        case DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
            category = vsq_group_idx / DNX_COSQ_NOF_TC;
            traffic_class = vsq_group_idx - (category * DNX_COSQ_NOF_TC);
            BCM_COSQ_GPORT_VSQ_CTTC_SET(*vsq_gport, core, 0, category, traffic_class);
            break;
        case DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY_CNCTN_CLS:
            category = vsq_group_idx / dnx_data_ingr_congestion.vsq.connection_class_nof_get(unit);
            connection_class = vsq_group_idx - (category * dnx_data_ingr_congestion.vsq.connection_class_nof_get(unit));
            BCM_COSQ_GPORT_VSQ_CTCC_SET(*vsq_gport, core, 0, category, connection_class);
            break;
        case DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG:
            BCM_COSQ_GPORT_VSQ_PP_SET(*vsq_gport, core, 0, vsq_group_idx);
            break;
        case DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT:
            BCM_COSQ_GPORT_VSQ_SRC_PORT_SET(*vsq_gport, core, 0, vsq_group_idx);
            break;
        case DNX_INGRESS_CONGESTION_VSQ_GROUP_PG:
            BCM_COSQ_GPORT_VSQ_PG_SET(*vsq_gport, core, vsq_group_idx);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid VSQ %d provided.\n", vsq_group);

    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * create VSQ gport according to vsq info
 */
int
dnx_ingress_congestion_vsq_gport_set(
    int unit,
    bcm_cosq_vsq_info_t * vsq_info,
    bcm_gport_t * vsq_gport)
{

    SHR_FUNC_INIT_VARS(unit);

    if (vsq_info->flags == BCM_COSQ_VSQ_GL)
    {
        BCM_COSQ_GPORT_VSQ_GL_SET(*vsq_gport, vsq_info->core_id);
    }

    if (vsq_info->flags == BCM_COSQ_VSQ_CT)
    {
        BCM_COSQ_GPORT_VSQ_CT_SET(*vsq_gport, vsq_info->core_id, 0, vsq_info->ct_id);
    }

    if (vsq_info->flags == BCM_COSQ_VSQ_CTTC)
    {
        BCM_COSQ_GPORT_VSQ_CTTC_SET(*vsq_gport, vsq_info->core_id, 0, vsq_info->ct_id, vsq_info->traffic_class);
    }

    if (vsq_info->flags == BCM_COSQ_VSQ_CTCC)
    {
        BCM_COSQ_GPORT_VSQ_CTCC_SET(*vsq_gport, vsq_info->core_id, 0, vsq_info->ct_id, vsq_info->cc_id);
    }

    if (vsq_info->flags == BCM_COSQ_VSQ_PP)
    {
        BCM_COSQ_GPORT_VSQ_PP_SET(*vsq_gport, vsq_info->core_id, 0, vsq_info->pp_vsq_id);
    }
    /** Arad only types */
    /** Src Port VSQ, and PG VSQ are configured in another another API*/
    if (vsq_info->flags == BCM_COSQ_VSQ_PFC || vsq_info->flags == BCM_COSQ_VSQ_LLFC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported  flags 0x%x\n", vsq_info->flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * retrieve VSQ information
 */
int
dnx_ingress_congestion_vsq_info_get(
    int unit,
    bcm_gport_t vsq,
    bcm_cosq_vsq_info_t * vsq_info)
{
    int ct_id;
    int cc_id;
    int traffic_class;
    bcm_gport_t src_port = DNX_ALGO_PORT_INVALID;
    int pp_vsq_id;
    int core_id;
    int src_port_vsq_index, pg_base;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_gport_vsq_get_verify(unit, vsq, 0));

    core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(vsq);
    ct_id = BCM_COSQ_GPORT_VSQ_CT_GET(vsq);
    traffic_class = BCM_COSQ_GPORT_VSQ_TC_GET(vsq);
    cc_id = BCM_COSQ_GPORT_VSQ_CC_GET(vsq);
    pp_vsq_id = BCM_COSQ_GPORT_VSQ_PP_GET(vsq);

    if (BCM_COSQ_GPORT_IS_VSQ_SRC_PORT(vsq))
    {
        src_port_vsq_index = BCM_COSQ_GPORT_VSQ_SRC_PORT_GET(vsq);
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.port_based_vsq.vsq_e_mapping.src_port.get(unit, core_id,
                                                                                            src_port_vsq_index,
                                                                                            &src_port));
    }
    else if (BCM_COSQ_GPORT_IS_VSQ_PG(vsq))
    {
        pg_base = BCM_COSQ_GPORT_VSQ_PG_GET(vsq);
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.port_based_vsq.vsq_f_mapping.src_port.get(unit, core_id, pg_base,
                                                                                            &src_port));
    }

    vsq_info->flags = _SHR_COSQ_GPORT_VSQ_FLAGS_GET(vsq);
    if (ct_id != -1)
    {
        vsq_info->ct_id = ct_id;
    }
    if (cc_id != -1)
    {
        vsq_info->cc_id = cc_id;
    }
    if (traffic_class != -1)
    {
        vsq_info->traffic_class = traffic_class;
    }
    if (src_port != DNX_ALGO_PORT_INVALID)
    {
        vsq_info->src_port = src_port;
    }
    if (pp_vsq_id != -1)
    {
        vsq_info->pp_vsq_id = pp_vsq_id;
    }
    if (core_id != -1)
    {
        vsq_info->core_id = core_id;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Destroy source VSQ gports
 */
int
dnx_ingress_congestion_src_vsqs_gport_destroy(
    int unit,
    bcm_gport_t vsq_gport)
{
    int pg_base = -1;
    int src_local_port = 0;
    int core_id;
    int cosq = 0;
    int numq = 0;
    int src_port_vsq_index;
    bcm_cosq_vsq_info_t vsq_info;
    int enable;
    dnx_ingress_congestion_pg_params_t pg_params;

    dnx_ingress_congestion_vsq_tc_pg_mapping_t tc_pg_mapping;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&vsq_info, 0x0, sizeof(bcm_cosq_vsq_info_t));

    if (BCM_GPORT_IS_LOCAL(vsq_gport))
    {
        src_local_port = BCM_GPORT_LOCAL_GET(vsq_gport);
    }
    else if (BCM_COSQ_GPORT_IS_VSQ_SRC_PORT(vsq_gport) || BCM_COSQ_GPORT_IS_VSQ_PG(vsq_gport))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_info_get(unit, vsq_gport, &vsq_info));
        src_local_port = vsq_info.src_port;
    }
    else
    {
        src_local_port = vsq_gport;
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_local_port, &core_id));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_src_vsq_mapping_enable_get(unit, src_local_port, &enable));

    if (!enable)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_src_vsq_mapping_get(unit, src_local_port, &src_port_vsq_index, &pg_base));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_e_id_dealloc(unit, src_local_port, src_port_vsq_index));

    /** Map to default rate class*/
    SHR_IF_ERR_EXIT(dnx_vsq_rate_class_destroy(unit, core_id, src_port_vsq_index,
                                               DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.pg_numq.get(unit, src_local_port, &numq));

    if (numq > 0)
    {
        /** for NIF port, we didn't allocate, so no need to deallocate */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_f_id_dealloc(unit, src_local_port, numq, pg_base));
    }

    /** update reverse mapping in SW state */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_port_based_vsq_sw_state_update(unit, core_id, DNX_ALGO_PORT_INVALID,
                                                                          src_port_vsq_index, pg_base));

    /** clear numq in SW state */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.pg_numq.set(unit, src_local_port, 0));

    if (numq > 0)
    {
        for (cosq = 0; cosq < numq; cosq++)
        {
            SHR_IF_ERR_EXIT(dnx_vsq_rate_class_destroy(unit, core_id, pg_base + cosq,
                                                       DNX_INGRESS_CONGESTION_VSQ_GROUP_PG));
        }

        /** All TCs are mapped to offset 0, by default*/
        sal_memset(&tc_pg_mapping, 0x0, sizeof(tc_pg_mapping));
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_pg_tc_profile_exchange_and_set(unit, src_local_port,
                                                                                  &tc_pg_mapping));

        /** PG-params are per PG-VSQ */
        sal_memset(&pg_params, 0x0, sizeof(pg_params));

        for (cosq = 0; cosq < numq; cosq++)
        {
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_pg_params_set(unit, core_id, pg_base + cosq, &pg_params));
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_pg_use_port_guaranteed_set(unit, core_id, pg_base + cosq,
                                                                                   FALSE));

        }
    }

    /** this must be done after dnx_ingress_congestion_vsq_pg_tc_profile_exchange_and_set */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_src_vsq_mapping_clear(unit, src_local_port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Extract list of keys from discard flags
 */
int
dnx_ingress_congestion_discard_key_get(
    int unit,
    bcm_gport_t gport,
    bcm_cosq_gport_discard_t * discard,
    dnx_ingress_congestion_rate_class_threshold_key_t * key_info)
{
    int index = 0;

    int is_ecn;

    SHR_FUNC_INIT_VARS(unit);

    is_ecn = (discard->flags & BCM_COSQ_DISCARD_MARK_CONGESTION);

    if (is_ecn)
    {
        key_info->dp[0] = 0;
        key_info->nof_dp = 1;
    }
    else
    {
        if (discard->flags & BCM_COSQ_DISCARD_COLOR_GREEN)
        {
            key_info->dp[index++] = 0;
        }
        if (discard->flags & BCM_COSQ_DISCARD_COLOR_YELLOW)
        {
            key_info->dp[index++] = 1;
        }
        if (discard->flags & BCM_COSQ_DISCARD_COLOR_RED)
        {
            key_info->dp[index++] = 2;
        }
        if (discard->flags & BCM_COSQ_DISCARD_COLOR_BLACK)
        {
            key_info->dp[index++] = 3;
        }
        key_info->nof_dp = index;
    }

    index = 0;
    if (discard->flags & BCM_COSQ_DISCARD_POOL_0)
    {
        key_info->pool[index++] = 0;
    }
    if (discard->flags & BCM_COSQ_DISCARD_POOL_1)
    {
        key_info->pool[index++] = 1;
    }
    key_info->nof_pool = index;

    /** if no pool flag specified */
    if (key_info->nof_pool == 0)
    {
        if (BCM_COSQ_GPORT_IS_VSQ_SRC_PORT(gport))
        {
            /** If no pool flag specified, set both pools */
            key_info->nof_pool = dnx_data_ingr_congestion.vsq.pool_nof_get(unit);
            for (index = 0; index < key_info->nof_pool; index++)
            {
                key_info->pool[index] = index;
            }
        }
        else
        {
            /** pool is not relevant */
            key_info->nof_pool = 1;
            key_info->pool[0] = 0;
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief -  convert discard parameters to WRED data
 */
int
dnx_ingress_congestion_discard_to_wred_data(
    int unit,
    bcm_cosq_gport_discard_t * discard,
    dnx_ingress_congestion_wred_info_t * wred_data)
{

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(wred_data, 0x0, sizeof(dnx_ingress_congestion_wred_info_t));
    wred_data->wred_en = (discard->flags & BCM_COSQ_DISCARD_ENABLE ? TRUE : FALSE);
    wred_data->min_avrg_th =
        discard->flags & BCM_COSQ_DISCARD_GRANULARITY_1K ? KILO(discard->min_thresh) : discard->min_thresh;
    wred_data->max_avrg_th =
        discard->flags & BCM_COSQ_DISCARD_GRANULARITY_1K ? KILO(discard->max_thresh) : discard->max_thresh;
    wred_data->max_probability = discard->drop_probability;

    SHR_FUNC_EXIT;
}

/**
 * \brief -  convert  WRED data to discard parameters
 */
int
dnx_ingress_congestion_wred_data_to_discard(
    int unit,
    dnx_ingress_congestion_wred_info_t * wred_data,
    bcm_cosq_gport_discard_t * discard)
{

    SHR_FUNC_INIT_VARS(unit);

    discard->min_thresh =
        discard->flags & BCM_COSQ_DISCARD_GRANULARITY_1K ? wred_data->min_avrg_th / KILO(1) : wred_data->min_avrg_th;
    discard->max_thresh =
        discard->flags & BCM_COSQ_DISCARD_GRANULARITY_1K ? wred_data->max_avrg_th / KILO(1) : wred_data->max_avrg_th;
    discard->drop_probability = wred_data->max_probability;
    if (wred_data->wred_en)
    {
        discard->flags |= BCM_COSQ_DISCARD_ENABLE;
    }
    else
    {
        discard->flags &= ~BCM_COSQ_DISCARD_ENABLE;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * verify that the threshold is below maximum of the resource
 */
int
dnx_ingress_congestion_threshold_verify(
    int unit,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    char *threshold_name,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    if (threshold > dnx_data_ingr_congestion.info.resource_get(unit, rsrc_type)->max)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "%s %u is too big\n", threshold_name, threshold);

    }

exit:
    SHR_FUNC_EXIT;

}
/*
 * VOQ code -- start
 */

/**
 * \brief - parse the user configuration to get rate class array, nof rate class and how many queues in each rate class.
 *
 * \param [in] unit -The unit number.
 * \param [in] config- user configuration from API. this function only uses config->numq and config->queue_attributes.rate_class.
 * \param [out] rate_class_array - all the rate classes given by the user (no repetitions)
 * \param [out] nof_rate_class - nof different rate classes given by the user.
 * \param [out] rate_class_nof_queues_to_add - nof queues to add to each rate class. this is an array of size [MAX_RATE_CLASSES].
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_ingress_congestion_voq_rate_class_array_from_config_get(
    int unit,
    bcm_cosq_ingress_queue_bundle_gport_config_t * config,
    uint32 rate_class_array[DNX_COSQ_NOF_TC],
    uint32 *nof_rate_class,
    uint32 rate_class_nof_queues_to_add[DNX_DATA_MAX_INGR_CONGESTION_VOQ_NOF_RATE_CLASS])
{
    int cosq, rate_class;
    int numq = config->numq;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(rate_class_nof_queues_to_add, 0, sizeof(uint32) * dnx_data_ingr_congestion.voq.nof_rate_class_get(unit));
    *nof_rate_class = 0;
    for (cosq = 0; cosq < numq; cosq++)
    {
        rate_class = config->queue_atrributes[cosq].rate_class;

        if (rate_class_nof_queues_to_add[rate_class] == 0)
        {
            /*
             * this is a new rate class
             */
            rate_class_array[*nof_rate_class] = rate_class;
            ++(*nof_rate_class);
        }
        rate_class_nof_queues_to_add[rate_class] += 1;
    }

    SHR_FUNC_EXIT;
}

/*
 * calculate the change in guaranteed when moving nof_queues_mapped
 * from old_rate_class to new_rate_class.
 * the change in guaranteed will be already set in HW resolution granularity,
 * so we do not reach max uint value.
 */
static shr_error_e
dnx_ingress_congestion_rate_class_res_type_guaranteed_calc(
    int unit,
    dnx_ingress_congestion_resource_type_e res_type,
    int old_rate_class,
    int new_rate_class,
    int nof_queues_mapped,
    int *guaranteed_change)
{
    uint32 old_guaranteed = 0, new_guaranteed = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (old_rate_class != INVALID_RATE_CLASS)
    {
        SHR_IF_ERR_EXIT(dnx_voq_rate_class_guaranteed_get(unit, res_type, old_rate_class, &old_guaranteed));
    }
    if (new_rate_class != INVALID_RATE_CLASS)
    {
        SHR_IF_ERR_EXIT(dnx_voq_rate_class_guaranteed_get(unit, res_type, new_rate_class, &new_guaranteed));
    }

    *guaranteed_change = (new_guaranteed - old_guaranteed) * nof_queues_mapped;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ingress_congestion_global_drop_pool_dp_all_get(
    int unit,
    dnx_ingress_congestion_resource_type_e res_type,
    uint32 global_drop_pool_dp[][DNX_COSQ_NOF_DP])
{

    int dp, pool;
    bcm_cosq_resource_t cosq_resource_type_per_dp_per_pool;

    bcm_gport_t glb_vsq_gport;
    bcm_cosq_vsq_info_t vsq_info;

    bcm_cosq_static_threshold_info_t thresh_info;
    bcm_cosq_static_threshold_t dp_pool_threshold;

    SHR_FUNC_INIT_VARS(unit);

    /** get matching resource types */
    switch (res_type)
    {
        case DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS:
        {
            cosq_resource_type_per_dp_per_pool = bcmResourceOcbBuffers;
            break;
        }
        case DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS:
        {
            cosq_resource_type_per_dp_per_pool = bcmResourceOcbPacketDescriptors;
            break;
        }
        case DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES:
        {
            cosq_resource_type_per_dp_per_pool = bcmResourceBytes;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unknown resource type received: %d\n", res_type);
        }
    }

    /** create VSQ global gport */
    sal_memset(&vsq_info, 0x0, sizeof(vsq_info));
    vsq_info.flags = BCM_COSQ_VSQ_GL;
    vsq_info.core_id = BCM_CORE_ALL;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_vsq_create(unit, &vsq_info, &glb_vsq_gport));

    for (pool = 0; pool < dnx_data_ingr_congestion.vsq.pool_nof_get(unit); ++pool)
    {
        for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
        {
            thresh_info.gport = glb_vsq_gport;
            thresh_info.threshold_action = bcmCosqThreshActionDrop;
            thresh_info.is_free_resource = 1;
            thresh_info.index = BCM_COSQ_TH_INDEX_POOL_DP_SET(pool, dp);
            thresh_info.resource_type = cosq_resource_type_per_dp_per_pool;
            SHR_IF_ERR_EXIT(dnx_cosq_gport_static_threshold_internal_get(unit, 0, &thresh_info, &dp_pool_threshold));
            global_drop_pool_dp[pool][dp] = dp_pool_threshold.thresh_set;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 *
 * \brief - Calculate maximal equivalent global drop
 *          currently used in the context of calculating VOQ shared resources space size.
 * Notes:
 *  - Calculation is done per resource
 *  - calculation takes maximum over all DPs.
 *
 * \param unit
 * \param res_type - resource type (bytes, SRAM PDs, SRAM buffers)
 * \param global_drop_pool_dp - relevant global drop theshold - per pool * per dp
 * \param [out] equivalent_global_drop - maximal equivalent global drop
 */
static shr_error_e
dnx_ingress_congestion_global_drop_for_guaranteed_get(
    int unit,
    dnx_ingress_congestion_resource_type_e res_type,
    uint32 global_drop_pool_dp[][DNX_COSQ_NOF_DP],
    uint32 *equivalent_global_drop)
{
    int dp;
    uint32 dp_pool_threshold_sum = 0;
    uint32 threshold;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * global drop according to resource type will be calculated as follows:
     * DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS -   take the per DP/pool OCB buffers global threshold (A+B)
     *                                                  and DP global threshold (C). take the max size between
     *                                                  the two as global drop ((A+B > C) ? A+B : C).
     * DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS     -   take the per DP/pool OCB PDs global threshold (A+B) and
     *                                                  equivalent global threshold based on DP OCB PDBs (C).
     *                                                  take the max size between the two as global drop ((A+B > C) ? A+B : C).
     * DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES  -   take the per DP/pool total bytes global threshold (A+B)
     *                                                  and equivalent global threshold based on Dram BDBs (C).
     *                                                  and take the max size between the 2 as global drop ((A+B > C) ? A+B : C).
     *
     * Note that per-DP thresholds are not configurable by API, only set during init.
     * Thus the equivalent thresholds are statically calculated and stored in dnx data
     */

    /** threshold per dp (marked as C in comment above) */
    threshold = dnx_data_ingr_congestion.init.equivalent_global_drop_get(unit, res_type)->drop;

    for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
    {
        dp_pool_threshold_sum = 0;
        /** get sum of thresholds per dp per pool (marked as A+B in comment above) */
        for (int pool = 0; pool < dnx_data_ingr_congestion.vsq.pool_nof_get(unit); ++pool)
        {
            dp_pool_threshold_sum += global_drop_pool_dp[pool][dp];
        }
        threshold = UTILEX_MAX(threshold, dp_pool_threshold_sum);
    }

    *equivalent_global_drop = threshold;

    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_res_type_guaranteed_verify(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e res_type,
    int guaranteed_change)
{
    uint32 global_drop_pool_dp[DNX_DATA_MAX_INGR_CONGESTION_VSQ_POOL_NOF][DNX_COSQ_NOF_DP];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_drop_pool_dp_all_get(unit, res_type, global_drop_pool_dp));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_guaranteed_or_global_drop_change_verify
                    (unit, core, res_type, guaranteed_change, global_drop_pool_dp));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify guaranteed change or global drop change with
 * respect to defining shared space for VOQ
 */
static shr_error_e
dnx_ingress_congestion_guaranteed_or_global_drop_change_verify(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e res_type,
    int guaranteed_change,
    uint32 global_drop_pool_dp[][DNX_COSQ_NOF_DP])
{
    uint32 max_res, free_resources, voq_total_guaranteed;
    uint32 equivalent_global_drop, equivalent_global_drop_hw;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Max resources from DNX data
     */
    max_res = dnx_data_ingr_congestion.info.resource_get(unit, res_type)->hw_resolution_max;

    /** get maximal equivalent global drop -- need to be substracted from shared size */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_drop_for_guaranteed_get(unit, res_type, global_drop_pool_dp,
                                                                          &equivalent_global_drop));
    equivalent_global_drop_hw =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, res_type, equivalent_global_drop);

    DNXCMN_CORES_ITER(unit, core, core_idx)
    {
        /*
         * total guaranteed for all VOQs from SW state
         */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.guaranteed.
                        voq_guaranteed.get(unit, core_idx, res_type, &voq_total_guaranteed));

        free_resources = max_res - voq_total_guaranteed - equivalent_global_drop_hw;

        /*
         * verify should be done for guaranteed_change == 0
         * since global drop might be changed
         */
        if (guaranteed_change >= 0)
        {
            /** Check we have enough resources */
            if (free_resources <= guaranteed_change)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "Ran out of resource to reserve/guarantee for queue");

            }
            /** Check that after we allocate the new queues, we still have left 2% of the current guaranteed */
            if (free_resources - guaranteed_change - 1 < (voq_total_guaranteed * 2 / 100))
            {
                /** This check can be later changed to verify against a configurable threshold */
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "Ran out of resource to reserve/guarantee for queue");
            }
        }
        else
        {
            /** we cannot free more resources than the current guaranteed */
            if (utilex_abs(guaranteed_change) > voq_total_guaranteed)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE,
                             "Requested change in reserved resource %d is out of range, as only %d are currently guaranteed",
                             guaranteed_change, voq_total_guaranteed);
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - inner function to call HW access function.
 */
static shr_error_e
dnx_ingress_congestion_shared_res_reject_status_thr_hw_set(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e res_type,
    uint32 dp,
    uint32 set_thr,
    uint32 clear_thr)
{
    dbal_tables_e dbal_table_id;
    uint32 hw_set_thr, hw_clear_thr;
    SHR_FUNC_INIT_VARS(unit);

    dbal_table_id = dnx_shared_reject_status_thresholds_table_id[res_type];

    /*
     * converting from threshold to HW granularity was done by caller functions
     */
    hw_set_thr = set_thr;
    hw_clear_thr = clear_thr;

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_shared_res_threshold_set
                    (unit, core, dbal_table_id, dp, hw_set_thr, hw_clear_thr));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_shared_res_reject_status_thr_set(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e res_type,
    int guaranteed_change)
{
    uint32 global_drop_pool_dp[DNX_DATA_MAX_INGR_CONGESTION_VSQ_POOL_NOF][DNX_COSQ_NOF_DP];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_drop_pool_dp_all_get(unit, res_type, global_drop_pool_dp));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_shared_reject_thr_calc_and_set(unit, core, res_type, guaranteed_change,
                                                                          global_drop_pool_dp));

exit:
    SHR_FUNC_EXIT;
}

/**
 * brief -
 * calculate dynamic shared size reject threshold (per dp)
 * according to guaranteed and global drop
 * and set it to HW
 */
static shr_error_e
dnx_ingress_congestion_shared_reject_thr_calc_and_set(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e res_type,
    int guaranteed_change,
    uint32 global_drop_pool_dp[][DNX_COSQ_NOF_DP])
{
    uint32 voq_total_guaranteed, max_res;
    uint32 reject_st_thr, drop_percentage;
    int shared_resources, resources_left, dp;
    uint32 equivalent_global_drop, equivalent_global_drop_hw;
    int core_idx;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Max resources from DNX data
     */
    max_res = dnx_data_ingr_congestion.info.resource_get(unit, res_type)->hw_resolution_max;

    /** get maximal equivalent global drop -- need to be substracted from shared size */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_drop_for_guaranteed_get(unit, res_type, global_drop_pool_dp,
                                                                          &equivalent_global_drop));
    equivalent_global_drop_hw =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, res_type, equivalent_global_drop);

    DNXCMN_CORES_ITER(unit, core, core_idx)
    {

        /*
         * total guaranteed for all VOQs from SW state -- in HW units
         */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.guaranteed.
                        voq_guaranteed.get(unit, core_idx, res_type, &voq_total_guaranteed));

        /*
         * current shared resources
         */
        shared_resources = max_res - voq_total_guaranteed - equivalent_global_drop_hw;

        /*
         * resources lest after the requested change (guaranteed_change can be < 0)
         */
        resources_left = shared_resources - guaranteed_change;

        for (dp = 0; dp < DNX_COSQ_NOF_DP; ++dp)
        {

            if (resources_left)
            {
                drop_percentage =
                    dnx_data_ingr_congestion.info.dp_free_res_presentage_drop_get(unit, dp)->drop_precentage;
                reject_st_thr = resources_left * drop_percentage / 100;
            }
            else
            {
                /*
                 * if no shared resources left, always set reject status
                 */
                reject_st_thr = max_res;
            }
            /*
             * set the same threshold for set and clear reject status.
             * (clear when free resource level is above threshold, set when free resource level is below threshold)
             */
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_shared_res_reject_status_thr_hw_set
                            (unit, core_idx, res_type, dp, reject_st_thr, reject_st_thr));
        }

        /*
         * set new guaranteed value in SW state
         */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.guaranteed.
                        voq_guaranteed.set(unit, core_idx, res_type, voq_total_guaranteed + guaranteed_change));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ingress_congestion_shared_res_reject_status_thr_all_res_types_set(
    int unit,
    int core,
    int guaranteed_change[DNX_INGRESS_CONGESTION_RESOURCE_NOF])
{
    int res_type;

    SHR_FUNC_INIT_VARS(unit);

    for (res_type = 0; res_type < DNX_INGRESS_CONGESTION_RESOURCE_NOF; res_type++)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_shared_res_reject_status_thr_set
                        (unit, core, res_type, guaranteed_change[res_type]));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Calculate the requested guaranteed reservation.
 *
 * \param [in] unit -The unit number.
 * \param [in] old_rate_class - the old rate class profile.
 * \param [in] new_rate_class - the new rate class profile.
 * \param [in] nof_queues_mapped - number of queues to be moved from old rate class to new rate class.
 * \param [out] guaranteed_change - this function returns the reservation change required for the nof_queues_remapped, for all threshold types.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_ingress_congestion_rate_class_total_guaranteed_calc(
    int unit,
    int old_rate_class,
    int new_rate_class,
    int nof_queues_mapped,
    int guaranteed_change[DNX_INGRESS_CONGESTION_RESOURCE_NOF])
{
    int res_type;
    SHR_FUNC_INIT_VARS(unit);

    for (res_type = 0; res_type < DNX_INGRESS_CONGESTION_RESOURCE_NOF; res_type++)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_rate_class_res_type_guaranteed_calc
                        (unit, res_type, old_rate_class, new_rate_class, nof_queues_mapped,
                         &guaranteed_change[res_type]));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - Verify that re-mapping nof_remappped_voqs queues from old_rate_class to new_rate_class
 * is valid in terms of guaranteed thresholds.
 * old_rate_class and new_rate class are rate class profile index.
 */
static shr_error_e
dnx_ingress_congestion_rate_class_total_guaranteed_verify(
    int unit,
    int core,
    int old_rate_class,
    int new_rate_class,
    uint32 nof_remapped_voqs)
{
    int res_type;
    int guaranteed_change;
    SHR_FUNC_INIT_VARS(unit);

    for (res_type = 0; res_type < DNX_INGRESS_CONGESTION_RESOURCE_NOF; res_type++)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_rate_class_res_type_guaranteed_calc
                        (unit, res_type, old_rate_class, new_rate_class, nof_remapped_voqs, &guaranteed_change));

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_res_type_guaranteed_verify(unit, core, res_type, guaranteed_change));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_ingress_congestion_voq_bundle_gport_config_rate_class_verify(
    int unit,
    bcm_cosq_ingress_queue_bundle_gport_config_t * config)
{
    int rate_class, rate_class_index, old_rate_class;
    int rate_class_queues_to_add = 0;
    uint32 rate_class_nof_queues_to_add[DNX_DATA_MAX_INGR_CONGESTION_VOQ_NOF_RATE_CLASS];
    uint32 seen_rate_classes[DNX_COSQ_NOF_TC];
    uint32 nof_rate_class = 0, is_created;
    int core = config->local_core_id;
    int cosq, numq;
    SHR_FUNC_INIT_VARS(unit);

    numq = config->numq;
    /*
     * Verify rate_class is in range
     */
    for (cosq = 0; cosq < numq; cosq++)
    {
        if ((config->queue_atrributes[cosq].rate_class < 0) ||
                (config->queue_atrributes[cosq].rate_class > dnx_data_ingr_congestion.voq.nof_rate_class_get(unit) - 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Rate class ID out of range - must be between 0 and %d", dnx_data_ingr_congestion.voq.nof_rate_class_get(unit) - 1);
        }
    }

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_rate_class_array_from_config_get
                    (unit, config, seen_rate_classes, &nof_rate_class, rate_class_nof_queues_to_add));

    /*
     * verify rate class attributes
     */
    for (rate_class_index = 0; rate_class_index < nof_rate_class; rate_class_index++)
    {
        rate_class = seen_rate_classes[rate_class_index];
        rate_class_queues_to_add = rate_class_nof_queues_to_add[rate_class];

        /*
         * Verify rate class is in range
         */
        SHR_RANGE_VERIFY(rate_class, 0, dnx_data_ingr_congestion.voq.nof_rate_class_get(unit) - 1, _SHR_E_PARAM,
                         "rate class %d is out of range\n", rate_class);

        /*
         * Make sure rate class is created using designated function
         */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.voq_rate_class.is_created.get(unit, rate_class, &is_created));
        if (!is_created)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Rate Class %d is not created. See bcm_cosq_gport_rate_clas_create", rate_class);
        }

        /*
         * We are adding new VOQs, so old_rate_class is invalid
         */
        old_rate_class = INVALID_RATE_CLASS;
        /*
         * Veirfy guaranteed is valid when adding the new queues to the specified rate class.
         */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_rate_class_total_guaranteed_verify
                        (unit, core, old_rate_class, rate_class, rate_class_queues_to_add));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_voq_guaranteed_set(
    int unit,
    int is_add,
    bcm_cosq_ingress_queue_bundle_gport_config_t * config)
{
    int rate_class, rate_class_index, old_rate_class, new_rate_class;
    int rate_class_queues_to_add = 0, res_type;
    uint32 rate_class_nof_queues_to_add[DNX_DATA_MAX_INGR_CONGESTION_VOQ_NOF_RATE_CLASS];
    uint32 nof_rate_class = 0;
    uint32 seen_rate_classes[DNX_COSQ_NOF_TC] = { 0 };
    int guaranteed_change_per_rate_class[DNX_INGRESS_CONGESTION_RESOURCE_NOF] = { 0 };
    int guaranteed_change_total[DNX_INGRESS_CONGESTION_RESOURCE_NOF] = { 0 };
    int core = config->local_core_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_rate_class_array_from_config_get
                    (unit, config, seen_rate_classes, &nof_rate_class, rate_class_nof_queues_to_add));

    for (rate_class_index = 0; rate_class_index < nof_rate_class; rate_class_index++)
    {
        rate_class = seen_rate_classes[rate_class_index];
        rate_class_queues_to_add = rate_class_nof_queues_to_add[rate_class];

        if (is_add)
        {
            old_rate_class = INVALID_RATE_CLASS;
            new_rate_class = rate_class;
        }
        else
        {
            old_rate_class = rate_class;
            new_rate_class = INVALID_RATE_CLASS;
        }

        for (res_type = 0; res_type < DNX_INGRESS_CONGESTION_RESOURCE_NOF; res_type++)
        {
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_rate_class_res_type_guaranteed_calc
                            (unit, res_type, old_rate_class, new_rate_class, rate_class_queues_to_add,
                             &guaranteed_change_per_rate_class[res_type]));
            guaranteed_change_total[res_type] += guaranteed_change_per_rate_class[res_type];
        }
    }
    /*
     * update guaranteed
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_shared_res_reject_status_thr_all_res_types_set
                    (unit, core, guaranteed_change_total));
exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - Verify function for dnx_cosq_voq_to_rate_class_profile_map_set.
 * all arguments have already been verified in previously called functions.
 */
static shr_error_e
dnx_ingress_congestion_voq_to_rate_class_profile_map_set_verify(
    int unit,
    int core,
    int qid,
    int rate_class)
{
    uint32 old_rate_class;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get old rate class attributes.
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_rate_class_get(unit, core, qid, &old_rate_class));

    /*
     * Validate that re-mapping the queue is legal for quaranteed thresholds.
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_rate_class_total_guaranteed_verify
                    (unit, core, old_rate_class, rate_class, 1));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Purpose: Re-map ingress queue to a rate class profile.
 * stages:
 * 1. Validate that remapping the queue is legal for all dependent features. (taildrop)
 * 2. Update HW: re-map queue (qid) to profile
 */
shr_error_e
dnx_ingress_congestion_voq_to_rate_class_profile_map_set(
    int unit,
    int core,
    int voq,
    int rate_class)
{
    uint32 old_rate_class;
    int32 guaranteed_change[DNX_INGRESS_CONGESTION_RESOURCE_NOF];
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_voq_to_rate_class_profile_map_set_verify(unit, core, voq, rate_class));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_rate_class_get(unit, core, voq, &old_rate_class));

    if (old_rate_class != rate_class)
    {
        /*
         * Update mapping in HW:
         */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_rate_class_set(unit, core, voq, rate_class));

        /*
         * Update tail drop reservation
         */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_rate_class_total_guaranteed_calc
                        (unit, old_rate_class, rate_class, 1, guaranteed_change));

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_shared_res_reject_status_thr_all_res_types_set
                        (unit, core, guaranteed_change));

        /*
         * update ref count for the rate classes
         */
        DNXCMN_CORES_ITER(unit, core, core_idx)
        {
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.voq_rate_class.ref_count.dec(unit, core_idx, old_rate_class, 1));
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.voq_rate_class.ref_count.inc(unit, core_idx, rate_class, 1));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_voq_to_rate_class_profile_map_get(
    int unit,
    int core,
    int voq,
    int *rate_class)
{
    uint32 rate_class_profile;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_rate_class_get(unit, core, voq, &rate_class_profile));
    *rate_class = rate_class_profile;
exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - get inner resource type from bcm resource type
 * the inner resource type is specific to the resources controlled by DRAM thresholds,
 * and is used in the code as access index in arrays.
 */
static shr_error_e
dnx_ingress_congestion_dram_internal_resource_type_get(
    int unit,
    bcm_cosq_resource_t resource,
    dnx_ingress_congestion_dram_bound_resource_type_e * internal_resource)
{
    SHR_FUNC_INIT_VARS(unit);

    *internal_resource = DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_INVALID;

    switch (resource)
    {
        case bcmResourceOcbBytes:
            *internal_resource = DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BYTES;
            break;
        case bcmResourceOcbBuffers:
            *internal_resource = DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BUFFERS;
            break;
        case bcmResourceOcbPacketDescriptors:
            *internal_resource = DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_PDS;
            break;
        default:
            break;
    }

    SHR_FUNC_EXIT;
}

/*
 * brief - get inner resource type from bcm resource type
 * the inner resource type is used in the code as access index in arrays.
 */
static shr_error_e
dnx_ingress_congestion_internal_resource_type_get(
    int unit,
    bcm_cosq_resource_t resource,
    dnx_ingress_congestion_resource_type_e * internal_resource)
{
    SHR_FUNC_INIT_VARS(unit);

    *internal_resource = DNX_INGRESS_CONGESTION_RESOURCE_INVALID;

    switch (resource)
    {
        case bcmResourceBytes:
            *internal_resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES;
            break;
        case bcmResourceOcbBuffers:
            *internal_resource = DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS;
            break;
        case bcmResourceOcbPacketDescriptors:
            *internal_resource = DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS;
            break;
        default:
            break;
    }

    SHR_FUNC_EXIT;
}

/*
 * brief - update the DRAM threshold in SW state.
 * if the DRAM is heating up, we might want to turn it off.
 * so we keep a copy of the thresholds in SW state,
 * and only when the DRAM is restored, we write the latest values from SW state to HW.
 *
 * Note: this function should be called only when holding the mutex!
 */
static shr_error_e
dnx_ingress_congestion_dram_fadt_thresholds_sw_state_set(
    int unit,
    int rate_class,
    dnx_ingress_congestion_dram_bound_resource_type_e resource_type,
    bcm_cosq_gport_fadt_threshold_type_t threshold_type,
    bcm_cosq_fadt_threshold_t * threshold)
{
    bcm_cosq_fadt_threshold_t hw_fadt = { 0 };
    dnx_cosq_ingress_dram_thresholds_t rate_class_dram_thresholds;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * we write to SW state the same values written in HW, so we need to convert to HW resolution
     */
    SHR_IF_ERR_EXIT(dnx_ingress_cogestion_convert_fadt_dram_threshold_to_internal_get
                    (unit, resource_type, threshold, &hw_fadt));
    /*
     * get rate class thresholds
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.dram_thresholds.get(unit, rate_class, &rate_class_dram_thresholds));

    /*
     * update specific threshold
     */
    if (threshold_type == bcmCosqFadtDramBound)
    {
        rate_class_dram_thresholds.dram_bound_thresholds[resource_type] = hw_fadt;
    }
    else if (threshold_type == bcmCosqFadtDramBoundRecoveryFailure)
    {
        rate_class_dram_thresholds.dram_recovery_fail_thresholds[resource_type] = hw_fadt;
    }

    /*
     * set rate class threshold
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.dram_thresholds.set(unit, rate_class, &rate_class_dram_thresholds));
exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - get DRAM threshold stored in SW state.
 * if the DRAM is heating up, we might want to turn it off.
 * so we keep a copy of the thresholds in SW state,
 * and only when the DRAM is restored, we write the latest values from SW state to HW.
 */
static shr_error_e
dnx_ingress_congestion_dram_fadt_thresholds_sw_state_get(
    int unit,
    int rate_class,
    dnx_ingress_congestion_dram_bound_resource_type_e resource_type,
    bcm_cosq_gport_fadt_threshold_type_t threshold_type,
    bcm_cosq_fadt_threshold_t * threshold)
{
    bcm_cosq_fadt_threshold_t hw_fadt = { 0 };
    dnx_cosq_ingress_dram_thresholds_t rate_class_dram_thresholds;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get info from SW state
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.dram_thresholds.get(unit, rate_class, &rate_class_dram_thresholds));

    if (threshold_type == bcmCosqFadtDramBound)
    {
        hw_fadt = rate_class_dram_thresholds.dram_bound_thresholds[resource_type];
    }
    else if (threshold_type == bcmCosqFadtDramBoundRecoveryFailure)
    {
        hw_fadt = rate_class_dram_thresholds.dram_recovery_fail_thresholds[resource_type];
    }

    /*
     * Convert back to DRAM threshold values
     */
    SHR_IF_ERR_EXIT(dnx_ingress_cogestion_convert_fadt_internal_to_dram_threshold_get
                    (unit, resource_type, &hw_fadt, threshold));
exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - Verify function for dnx_ingress_congestion_dram_bound_fadt_threshold_set/get
 */
static shr_error_e
dnx_ingress_congestion_dram_bound_fadt_threshold_verify(
    int unit,
    bcm_cosq_fadt_info_t * fadt_info,
    bcm_cosq_fadt_threshold_t * threshold,
    int is_set)
{
    int is_res_range = FALSE;
    int max_res, alpha_min, alpha_max;
    dnx_ingress_congestion_dram_bound_resource_type_e resource;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify resource type
     */
    switch (fadt_info->resource_type)
    {
        case bcmResourceOcbBuffers:
        case bcmResourceOcbPacketDescriptors:
        case bcmResourceOcbBytes:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid resource type %d", fadt_info->resource_type);
    }

    if (is_set)
    {
        /*
         * Get inner resource type from BCM resource type
         */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_internal_resource_type_get
                        (unit, fadt_info->resource_type, &resource));

        max_res = dnx_data_ingr_congestion.dram_bound.resource_get(unit, resource)->max;
        /*
         * Verify max threshold values don't exceed max res
         */
        if (threshold->thresh_max > max_res || threshold->resource_range_max > max_res)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid fadt: max resources for resource type %d is %d",
                         fadt_info->resource_type, max_res);
        }

        /*
         * Verify min threshold values don't exceed max threshold values
         */
        if (threshold->thresh_min > threshold->thresh_max)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid fadt: min parameter %d, max parameter %d", threshold->thresh_min,
                         threshold->thresh_max);
        }

        if (threshold->resource_range_min > threshold->resource_range_max)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid fadt: resource_range_min parameter %d, resource_range_min parameter %d",
                         threshold->resource_range_min, threshold->resource_range_max);
        }

        /*
         * Verify resource range is supported for the resource type (if resource range values are given)
         */
        is_res_range = dnx_data_ingr_congestion.dram_bound.resource_get(unit, resource)->is_resource_range;
        if ((threshold->resource_range_min || threshold->resource_range_max) && !is_res_range)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid fadt: resource type %d does not support resource range, please set it to 0",
                         fadt_info->resource_type);
        }

        /*
         * Verify Alpha
         */
        alpha_min = dnx_data_ingr_congestion.dram_bound.fadt_alpha_min_get(unit);
        alpha_max = dnx_data_ingr_congestion.dram_bound.fadt_alpha_max_get(unit);

        if (threshold->alpha < alpha_min || threshold->alpha > alpha_max)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid fadt: Alpha factor %d is out of bounds", threshold->alpha);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dram_bound_fadt_threshold_set(
    int unit,
    bcm_cosq_fadt_info_t * fadt_info,
    bcm_cosq_fadt_threshold_t * threshold)
{
    int dram_in_use, rate_class, is_set = TRUE, is_mutex_locked = FALSE;
    dnx_ingress_congestion_dram_bound_resource_type_e resource;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_dram_bound_fadt_threshold_verify(unit, fadt_info, threshold, is_set));

    rate_class = BCM_GPORT_PROFILE_GET(fadt_info->gport);

    /*
     * Get inner resource type from BCM resource type
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_internal_resource_type_get(unit, fadt_info->resource_type, &resource));

    /*
     * Take mutex
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.dram_bound_mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    /*
     * Update dram thresholds in SW-State
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_fadt_thresholds_sw_state_set
                    (unit, rate_class, resource, fadt_info->thresh_type, threshold));

    /*
     * Check sw-state if need to update HW
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.dram_in_use.get(unit, &dram_in_use));
    if (dram_in_use)
    {
        if (fadt_info->thresh_type == bcmCosqFadtDramBound)
        {
            SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_dram_bound_set(unit, resource, rate_class, threshold));
        }
        else if (fadt_info->thresh_type == bcmCosqFadtDramBoundRecoveryFailure)
        {
            SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_dram_recovery_fail_set(unit, resource, rate_class, threshold));
        }
    }
exit:
    if (is_mutex_locked)
    {
        /** give the mutex before exiting */
        SHR_IF_ERR_CONT(dnx_ingress_congestion_db.sync_manager.dram_bound_mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dram_bound_fadt_threshold_get(
    int unit,
    bcm_cosq_fadt_info_t * fadt_info,
    bcm_cosq_fadt_threshold_t * threshold)
{
    int rate_class, is_set = FALSE;
    dnx_ingress_congestion_dram_bound_resource_type_e resource;
    SHR_FUNC_INIT_VARS(unit);

    rate_class = BCM_GPORT_PROFILE_GET(fadt_info->gport);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_dram_bound_fadt_threshold_verify(unit, fadt_info, threshold, is_set));

    /*
     * Get inner resource type from BCM resource type
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_internal_resource_type_get(unit, fadt_info->resource_type, &resource));

    /*
     * Get info from SW state
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_fadt_thresholds_sw_state_get
                    (unit, rate_class, resource, fadt_info->thresh_type, threshold));

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_ingress_congestion_voq_congestion_notification_fadt_threshold_verify(
    int unit,
    bcm_cosq_fadt_info_t * fadt_info,
    bcm_cosq_fadt_threshold_t * threshold,
    int is_set)
{
    int max_res, alpha_min, alpha_max;
    int is_dram_present = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    dnx_ingress_congestion_resource_type_e resource;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Assert cosq is 0
     */
    SHR_VAL_VERIFY(fadt_info->cosq, 0, _SHR_E_PARAM, "Cosq arg is not supported for this device. please set to 0");

    /*
     * Verify resource type
     */
    switch (fadt_info->resource_type)
    {
        case bcmResourceOcbBuffers:
        case bcmResourceOcbPacketDescriptors:
        case bcmResourceBytes:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid resource type %d", fadt_info->resource_type);
    }

    if (is_set)
    {
        /*
         * Get inner resource type from BCM resource type
         */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_internal_resource_type_get(unit, fadt_info->resource_type, &resource));

        max_res = dnx_data_ingr_congestion.info.resource_get(unit, resource)->max;
        /*
         * Verify max threshold values don't exceed max res
         */
        if (threshold->thresh_max > max_res)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid fadt: max resources for resource type %d is %d",
                         fadt_info->resource_type, max_res);
        }

        /*
         * Verify min threshold values don't exceed max threshold values
         */
        if (threshold->thresh_min > threshold->thresh_max)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid fadt: min parameter %d, max parameter %d", threshold->thresh_min,
                         threshold->thresh_max);
        }

        /*
         * Verify Alpha
         */
        alpha_min = dnx_data_ingr_congestion.info.resource_get(unit, resource)->fadt_alpha_min;
        alpha_max = dnx_data_ingr_congestion.info.resource_get(unit, resource)->fadt_alpha_max;

        if (threshold->alpha < alpha_min || threshold->alpha > alpha_max)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid fadt: Alpha factor %d is out of bounds", threshold->alpha);
        }

        /*
         * when no DRAM present, total bytes threshold should be static (other resources are in OCB)
         */
        if (!is_dram_present &&
            resource == DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES && threshold->thresh_min != threshold->thresh_max)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Total bytes threshold should be static when no DRAMs present");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ingress_congestion_voq_congestion_notification_fadt_threshold_set(
    int unit,
    uint32 flags,
    bcm_cosq_fadt_info_t * fadt_info,
    bcm_cosq_fadt_threshold_t * threshold)
{
    int rate_class, is_set = TRUE;
    dnx_ingress_congestion_resource_type_e resource;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_voq_congestion_notification_fadt_threshold_verify
                          (unit, fadt_info, threshold, is_set));

    /*
     * get rate class from Gport
     */
    rate_class = BCM_GPORT_PROFILE_GET(fadt_info->gport);

    /*
     * get internal resource type
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_internal_resource_type_get(unit, fadt_info->resource_type, &resource));

    /*
     * set threshold to HW
     */
    SHR_IF_ERR_EXIT(dnx_voq_rate_class_congestion_notification_fadt_set(unit, rate_class, resource, threshold));

exit:

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ingress_congestion_voq_congestion_notification_fadt_threshold_get(
    int unit,
    uint32 flags,
    bcm_cosq_fadt_info_t * fadt_info,
    bcm_cosq_fadt_threshold_t * threshold)
{

    int rate_class, is_set = FALSE;
    dnx_ingress_congestion_resource_type_e resource;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_voq_congestion_notification_fadt_threshold_verify
                          (unit, fadt_info, threshold, is_set));

    /*
     * get rate class from Gport
     */
    rate_class = BCM_GPORT_PROFILE_GET(fadt_info->gport);

    /*
     * get internal resource type
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_internal_resource_type_get(unit, fadt_info->resource_type, &resource));

    /*
     * get threshold from HW
     */
    SHR_IF_ERR_EXIT(dnx_voq_rate_class_congestion_notification_fadt_get(unit, rate_class, resource, threshold));
exit:
    SHR_FUNC_EXIT;

}

/*
 * brief - write DRAM recovery threshold to SW state.
 * Note: this function should be called when holding the mutex!
 */
static shr_error_e
dnx_ingress_congestion_dram_recovery_threshold_sw_state_set(
    int unit,
    int rate_class,
    int recovery_thr)
{
    uint32 hw_recovery_thr;
    dnx_ingress_congestion_dram_bound_resource_type_e resource;
    dnx_cosq_ingress_dram_thresholds_t rate_class_dram_thresholds;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * DRAM recovery is only set in bytes
     */
    resource = DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BYTES;
    /*
     * we write to SW state the same values written in HW, so we need to convert to HW resolution
     */
    hw_recovery_thr = DNX_INGRESS_CONGESTION_CONVERT_DRAM_THRESHOLD_TO_INTERNAL_GET(unit, resource, recovery_thr);

    /*
     * get rate class thresholds
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.dram_thresholds.get(unit, rate_class, &rate_class_dram_thresholds));

    /*
     * update specific threshold
     */
    rate_class_dram_thresholds.dram_recovery_threshold = hw_recovery_thr;

    /*
     * set rate class threshold
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.dram_thresholds.set(unit, rate_class, &rate_class_dram_thresholds));
exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - get DRAM recovery threshold from SW state
 */
static shr_error_e
dnx_ingress_congestion_dram_recovery_threshold_sw_state_get(
    int unit,
    int rate_class,
    int *recovery_thr)
{
    uint32 hw_recovery_thr;
    dnx_ingress_congestion_dram_bound_resource_type_e resource;
    dnx_cosq_ingress_dram_thresholds_t rate_class_dram_thresholds;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * DRAM recovery is only set in bytes
     */
    resource = DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BYTES;

    /*
     * get rate class thresholds
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.dram_thresholds.get(unit, rate_class, &rate_class_dram_thresholds));

    /*
     * retrieve specific threshold
     */
    hw_recovery_thr = rate_class_dram_thresholds.dram_recovery_threshold;

    /*
     * we write to SW state the same values written in HW, so we need to convert back from HW resolution to bytes
     */
    *recovery_thr = DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_DRAM_THRESHOLD_GET(unit, resource, hw_recovery_thr);
exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify function for dnx_ingress_congestion_dram_recovery_threshold_set/get
 */
static shr_error_e
dnx_ingress_congestion_dram_recovery_threshold_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold,
    int is_set)
{
    uint32 max_res, threshold_val, is_vsq_allowed = FALSE;
    dnx_ingress_congestion_dram_bound_resource_type_e resource;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify gport is Rate Class profile Gport
     */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_rate_class_gport_verify(unit, gport, cosq, is_vsq_allowed));

    /*
     * Verify threshold type (flags already verified by caller)
     */
    if (threshold->type != bcmCosqThresholdBytes)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Invalid threshold type %d", threshold->type);
    }

    /*
     * Verify threshold value
     */
    if (is_set)
    {
        /*
         * Recovery threshold is given only in bytes
         */
        resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES;
        max_res = dnx_data_ingr_congestion.info.resource_get(unit, resource)->max;
        threshold_val = threshold->value;

        if (threshold_val > max_res)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid threshold %u", threshold_val);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - set DRAM recovery threshold.
 * if DRAM is currently redirected to OCB, the threshold will be updated only in SW state,
 * and when the DRAM is restored, the values from SW state will be written to HW.
 */
shr_error_e
dnx_ingress_congestion_dram_recovery_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold)
{
    int dram_in_use, rate_class, is_set = TRUE, is_mutex_locked = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_dram_recovery_threshold_verify(unit, gport, cosq, threshold, is_set));

    rate_class = BCM_GPORT_PROFILE_GET(gport);

    /*
     * Take mutex
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.dram_bound_mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    /*
     * Update threshold in SW-State
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_recovery_threshold_sw_state_set(unit, rate_class, threshold->value));

    /*
     * Check sw-state if need to update HW
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.dram_in_use.get(unit, &dram_in_use));
    if (dram_in_use)
    {
        SHR_IF_ERR_EXIT(dnx_voq_rate_class_dram_recovery_set(unit, rate_class, threshold->value));
    }
exit:
    if (is_mutex_locked)
    {
        /** give the mutex before exiting */
        SHR_IF_ERR_CONT(dnx_ingress_congestion_db.sync_manager.dram_bound_mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}

/*
 * brief - get DRAM recovery threshold
 */
shr_error_e
dnx_ingress_congestion_dram_recovery_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold)
{
    int rate_class, is_set = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_dram_recovery_threshold_verify(unit, gport, cosq, threshold, is_set));

    rate_class = BCM_GPORT_PROFILE_GET(gport);

    /*
     * get info from SW state
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_recovery_threshold_sw_state_get(unit, rate_class, &threshold->value));

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify function for dnx_ingress_congestion_gport_threshold_set/get
 */
static shr_error_e
dnx_ingress_congestion_gport_threshold_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold)
{
    uint32 supported_flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_COSQ_GPORT_IS_VSQ(gport))
    {
        if (!BCM_COSQ_GPORT_IS_VSQ_GL(gport))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported vsq gport 0x%x\n", gport);
        }
        else
        {
            if (threshold->flags & BCM_COSQ_THRESHOLD_NOT_COMMIT)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Flag BCM_COSQ_THRESHOLD_NOT_COMMIT is not supported. flags 0x%x",
                             threshold->flags);
            }
            if (threshold->flags & BCM_COSQ_THRESHOLD_DROP)
            {
                /*
                 * VSQ gl tail drop thresholds
                 */
                SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                             "bcm_gport_threshold_set is deprecated for INGRESS DROP. Please use bcm_gport_static_threshold_set instead");
            }
            /*
             * Flow control set
             */
            if (threshold->flags & BCM_COSQ_THRESHOLD_FLOW_CONTROL)
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                             "bcm_gport_threshold_set is deprecated for INGRESS FLOW CONTROL. Please use bcm_gport_static_threshold_set instead");
            }
        }
    }
    else
    {
        /*
         * Verify flags.
         */
        supported_flags = (BCM_COSQ_THRESHOLD_INGRESS | BCM_COSQ_THRESHOLD_QSIZE_RECOVERY);
        SHR_MASK_VERIFY(threshold->flags, supported_flags, _SHR_E_PARAM, "non supported flags specified.\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_gport_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_gport_threshold_verify(unit, gport, cosq, threshold));
    /*
     * Currently the only supported threshold is DRAM recovery
     */
    if (threshold->flags & BCM_COSQ_THRESHOLD_QSIZE_RECOVERY)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_recovery_threshold_set(unit, gport, cosq, threshold));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_gport_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_gport_threshold_verify(unit, gport, cosq, threshold));
    /*
     * Currently the only supported threshold is DRAM recovery
     */
    if (threshold->flags & BCM_COSQ_THRESHOLD_QSIZE_RECOVERY)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_recovery_threshold_get(unit, gport, cosq, threshold));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dram_usage_stop(
    int unit)
{
    int dram_in_use, rate_class, resource_type, is_res_range, is_mutex_locked = FALSE;
    uint32 max_total_bytes =
        dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES)->max;
    uint32 nof_rate_class = dnx_data_ingr_congestion.voq.nof_rate_class_get(unit);
    bcm_cosq_fadt_threshold_t dram_thresholds[DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_NOF];
    bcm_cosq_fadt_threshold_t default_tail_drop_fadt_ocb_only_thresholds = {
        .thresh_min = 0,
        .thresh_max = dnx_data_ingr_congestion.fadt_tail_drop.default_max_size_byte_threshold_for_ocb_only_get(unit),
        .alpha = 0,
        .resource_range_max = 0,
        .resource_range_min = 0,
    };
    uint32 max_res;
    SHR_FUNC_INIT_VARS(unit);

    /** take mutex */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.dram_bound_mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    /** check if dram usage is already stopped, in that case go to exit */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.dram_in_use.get(unit, &dram_in_use));
    if (dram_in_use == FALSE)
    {
        SHR_EXIT();
    }

    /** change sw-state to not update HW regarding dram bound thresholds when using BCM APIs*/
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.dram_in_use.set(unit, FALSE));

    /** set registers to stop dram from receiving any more traffic */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_dram_bound_traffic_to_dram_stop(unit));

    /** update HW with dram bound profile to prevent dram from recieving any more traffic */
    for (resource_type = 0; resource_type < DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_NOF; ++resource_type)
    {
        /** check if resource range is supported for the resource type */
        is_res_range = dnx_data_ingr_congestion.dram_bound.resource_get(unit, resource_type)->is_resource_range;
        /** get max resources for the resource type */
        max_res = dnx_data_ingr_congestion.dram_bound.resource_get(unit, resource_type)->max;

        /** set thresholds to make sure no traffic goes to DRAM */
        dram_thresholds[resource_type].thresh_max = max_res;
        dram_thresholds[resource_type].thresh_min = max_res;
        dram_thresholds[resource_type].alpha = 0;
        if (is_res_range)
        {
            dram_thresholds[resource_type].resource_range_max = max_res;
            dram_thresholds[resource_type].resource_range_min = 0;
        }
        /** Configure the same thresholds for all rate classes */
        for (rate_class = 0; rate_class < nof_rate_class; ++rate_class)
        {
            SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_dram_bound_set
                            (unit, resource_type, rate_class, &dram_thresholds[resource_type]));

            SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_dram_recovery_fail_set
                            (unit, resource_type, rate_class, &dram_thresholds[resource_type]));

            /** DRAM recovery is always set in bytes, so its not depended on resource type, only call it on first iteration */
            if (resource_type == 0)
            {
                SHR_IF_ERR_EXIT(dnx_voq_rate_class_dram_recovery_set(unit, rate_class, max_total_bytes));
            }
        }
    }

    /** Update tail drop FADT total bytes threshold to match OCB only rate class if needed */
    for (rate_class = 0; rate_class < nof_rate_class; ++rate_class)
    {
        for (int dp = 0; dp < DNX_COSQ_NOF_DP; ++dp)
        {
            bcm_cosq_fadt_threshold_t tail_drop_fadt;
            /** get FADT tail drop for total_bytes */
            SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_tail_drop_sw_get
                            (unit, DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES, rate_class, dp, &tail_drop_fadt));

            if (tail_drop_fadt.thresh_max > default_tail_drop_fadt_ocb_only_thresholds.thresh_max)
            {
                /*
                 * if the current FADT tail drop configured is less strict than the default for OCB only,
                 * change the HW to the values matching the OCB only default configuration
                 */
                SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_tail_drop_hw_set
                                (unit, DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES, rate_class, dp,
                                 &default_tail_drop_fadt_ocb_only_thresholds));
            }
        }
    }

exit:
    if (is_mutex_locked)
    {
        /** give mutex */
        SHR_IF_ERR_CONT(dnx_ingress_congestion_db.sync_manager.dram_bound_mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dram_usage_restore(
    int unit)
{
    uint32 nof_rate_class = dnx_data_ingr_congestion.voq.nof_rate_class_get(unit);
    int rate_class, dram_in_use, resource_type, is_dram_bound_mutex_locked = FALSE, is_fadt_mutex_locked = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    /** take mutexes */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.dram_bound_mutex.take(unit, sal_mutex_FOREVER));
    is_dram_bound_mutex_locked = TRUE;
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.fadt_mutex.take(unit, sal_mutex_FOREVER));
    is_fadt_mutex_locked = TRUE;

    /** check if dram usage was already restored, in that case go to exit */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.dram_in_use.get(unit, &dram_in_use));

    if (dram_in_use == TRUE)
    {
        SHR_EXIT();
    }

    /** set sw-state to update HW regarding dram bound thresholds when using BCM APIs */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.dram_in_use.set(unit, TRUE));

    /** set registers to restore dram to receive traffic */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_dram_bound_traffic_to_dram_restore(unit));

    /** update HW according to SW-State saved info */
    for (rate_class = 0; rate_class < nof_rate_class; ++rate_class)
    {
        int dram_recovery_thr;
        for (resource_type = 0; resource_type < DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_NOF; ++resource_type)
        {
            bcm_cosq_fadt_threshold_t threshold;
            /** get rate class thresholds from SW state and set thresholds to HW*/
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_fadt_thresholds_sw_state_get
                            (unit, rate_class, resource_type, bcmCosqFadtDramBound, &threshold));
            SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_dram_bound_set(unit, resource_type, rate_class, &threshold));
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_fadt_thresholds_sw_state_get
                            (unit, rate_class, resource_type, bcmCosqFadtDramBoundRecoveryFailure, &threshold));
            SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_dram_recovery_fail_set
                            (unit, resource_type, rate_class, &threshold));
        }
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_recovery_threshold_sw_state_get
                        (unit, rate_class, &dram_recovery_thr));
        SHR_IF_ERR_EXIT(dnx_voq_rate_class_dram_recovery_set(unit, rate_class, dram_recovery_thr));

        for (resource_type = 0; resource_type < DNX_INGRESS_CONGESTION_RESOURCE_NOF; ++resource_type)
        {
            for (int dp = 0; dp < DNX_COSQ_NOF_DP; ++dp)
            {
                bcm_cosq_fadt_threshold_t tail_frop_fadt;
                SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.fadt_tail_drop_thresholds.resource.
                                dp.get(unit, rate_class, resource_type, dp, &tail_frop_fadt));
                SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_tail_drop_hw_set
                                (unit, resource_type, rate_class, dp, &tail_frop_fadt));
            }
        }
    }
exit:
    if (is_dram_bound_mutex_locked)
    {
        /** give mutex */
        SHR_IF_ERR_CONT(dnx_ingress_congestion_db.sync_manager.dram_bound_mutex.give(unit));
    }
    if (is_fadt_mutex_locked)
    {
        /** give mutex */
        SHR_IF_ERR_CONT(dnx_ingress_congestion_db.sync_manager.fadt_mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ingress_congestion_voq_ocb_only_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int is_ocb_only)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_ocb_only != TRUE && is_ocb_only != FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "is_ocb_only is set to %d, but should be set to %d or %d only", is_ocb_only, TRUE,
                     FALSE);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_voq_ocb_only_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int is_ocb_only)
{
    int rate_class;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_voq_ocb_only_set_verify(unit, gport, cosq, is_ocb_only));

    rate_class = BCM_GPORT_PROFILE_GET(gport);

    SHR_IF_ERR_EXIT(dnx_voq_rate_class_ocb_only_set(unit, rate_class, is_ocb_only));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_voq_ocb_only_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *is_ocb_only)
{
    int rate_class;
    SHR_FUNC_INIT_VARS(unit);

    rate_class = BCM_GPORT_PROFILE_GET(gport);

    SHR_IF_ERR_EXIT(dnx_voq_rate_class_ocb_only_get(unit, rate_class, is_ocb_only));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set WRED configuration
 */
int
dnx_ingress_congestion_voq_discard_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard)
{
    dbal_tables_e table_id, avrg_table_id;
    dnx_ingress_congestion_dbal_wred_key_t dbal_key = { 0 };
    dnx_ingress_congestion_wred_info_t wred_data;
    dnx_ingress_congestion_rate_class_threshold_key_t rate_class_key;

    int index;
    int is_ecn;
    uint32 ecn_threshold;

    SHR_FUNC_INIT_VARS(unit);

    is_ecn = (discard->flags & BCM_COSQ_DISCARD_MARK_CONGESTION);

    table_id = (is_ecn ? DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_ECN :
                DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_DROP);
    avrg_table_id = DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_AVRG_PARAMS;
    dbal_key.rate_class = BCM_GPORT_PROFILE_GET(gport);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_discard_key_get(unit, gport, discard, &rate_class_key));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_discard_to_wred_data(unit, discard, &wred_data));

    /** configure WRED params per DP */
    for (index = 0; index < rate_class_key.nof_dp; index++)
    {
        dbal_key.dp = rate_class_key.dp[index];
        SHR_IF_ERR_EXIT(dnx_rate_class_common_wred_set(unit, table_id, &dbal_key, &wred_data));
    }

    /** Configure average weight - not per DP */
    SHR_IF_ERR_EXIT(dnx_rate_class_common_wred_avrg_set(unit, table_id, avrg_table_id, &dbal_key, discard->gain));

    if (is_ecn)
    {
        ecn_threshold =
            discard->flags & BCM_COSQ_DISCARD_GRANULARITY_1K ? KILO(discard->ecn_thresh) : discard->ecn_thresh;
        /** Configure ECN drop */
        SHR_IF_ERR_EXIT(dnx_voq_rate_class_ecn_drop_set(unit, dbal_key.rate_class, ecn_threshold));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve WRED configuration
 */
int
dnx_ingress_congestion_voq_discard_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard)
{
    dbal_tables_e table_id, avrg_table_id;
    dnx_ingress_congestion_dbal_wred_key_t dbal_key = { 0 };
    dnx_ingress_congestion_wred_info_t wred_data;
    dnx_ingress_congestion_rate_class_threshold_key_t rate_class_key;

    int is_ecn;
    uint32 ecn_threshold;
    SHR_FUNC_INIT_VARS(unit);

    is_ecn = (discard->flags & BCM_COSQ_DISCARD_MARK_CONGESTION);

    table_id = (is_ecn ? DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_ECN :
                DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_DROP);
    avrg_table_id = DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_AVRG_PARAMS;
    dbal_key.rate_class = BCM_GPORT_PROFILE_GET(gport);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_discard_key_get(unit, gport, discard, &rate_class_key));

    dbal_key.dp = rate_class_key.dp[0];

    SHR_IF_ERR_EXIT(dnx_rate_class_common_wred_get(unit, table_id, &dbal_key, &wred_data));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_wred_data_to_discard(unit, &wred_data, discard));

    /** get average weight - not per DP */
    SHR_IF_ERR_EXIT(dnx_rate_class_common_wred_avrg_get(unit, table_id, avrg_table_id, &dbal_key,
                                                        (uint32 *) &discard->gain));

    if (is_ecn)
    {
        SHR_IF_ERR_EXIT(dnx_voq_rate_class_ecn_drop_get(unit, dbal_key.rate_class, &ecn_threshold));
        discard->ecn_thresh =
            discard->flags & BCM_COSQ_DISCARD_GRANULARITY_1K ? ecn_threshold / KILO(1) : ecn_threshold;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * VOQ code -- end
 */

/*
 * VSQ code - start
 */

/**
 * \brief -
 * set VSQ WRED configuration
 */
int
dnx_ingress_congestion_vsq_discard_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard)
{
    int core_id = BCM_CORE_ALL;
    bcm_core_t current_core;
    dnx_ingress_congestion_vsq_rate_class_info_t data_rate_class;
    int index, pool_index;
    int curr_dp;
    int vsq_index;
    dnx_ingress_congestion_vsq_rate_class_wred_info_t *wred;
    uint32 *wred_exp_weight_ptr;

    dnx_ingress_congestion_vsq_group_e vsq_type;
    dnx_ingress_congestion_wred_hw_params_t wred_hw_params;
    dnx_ingress_congestion_wred_info_t wred_info;
    dnx_ingress_congestion_rate_class_threshold_key_t rate_class_key;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data_rate_class, 0x0, sizeof(dnx_ingress_congestion_vsq_rate_class_info_t));

    /** retrieve VSQ id */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_gport_vsq_get(unit, gport, cosq, &core_id, &vsq_type, &vsq_index));

    /** make coverity happy */
    SHR_MAX_VERIFY(vsq_type, DNX_INGRESS_CONGESTION_VSQ_GROUP_PG, _SHR_E_INTERNAL,
                   "vsq_type %d is not expected\n", vsq_type);

    DNX_INGRESS_CONGESTION_VSQ_CORES_ITER(unit, vsq_type, core_id, current_core)
    {
        /** Wred settings is defined by Rate Class profile */
        /** Get Rate class data */

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_alloc_vsq_rate_class_data_get(unit, current_core, vsq_type, vsq_index,
                                                                             &data_rate_class));

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_discard_key_get(unit, gport, discard, &rate_class_key));
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_discard_to_wred_data(unit, discard, &wred_info));

        /** calculate exact parameters */
        dnx_ingress_congestion_convert_wred_to_hw_get(unit, &wred_info, &wred_hw_params);
        dnx_ingress_congestion_convert_hw_to_wred_get(unit, &wred_hw_params, &wred_info);

        /** update rate class data at the relevant places */
        for (pool_index = 0; pool_index < rate_class_key.nof_pool; pool_index++)
        {
            wred = dnx_vsq_rate_class_wred_ptr_get(unit, vsq_type, rate_class_key.pool[pool_index], &data_rate_class);
            for (index = 0; index < rate_class_key.nof_dp; index++)
            {
                curr_dp = rate_class_key.dp[index];
                wred->wred_params[curr_dp].wred_en = wred_info.wred_en;
                wred->wred_params[curr_dp].min_avrg_th = wred_info.min_avrg_th;
                wred->wred_params[curr_dp].max_avrg_th = wred_info.max_avrg_th;
                wred->wred_params[curr_dp].max_probability = wred_info.max_probability;
            }
        }

        /** WRED exp wq parameter is same for all dps */
        wred_exp_weight_ptr = dnx_vsq_rate_class_exp_weight_wred_ptr_get(unit, vsq_type, &data_rate_class);
        *wred_exp_weight_ptr = discard->gain;

        /*
         *  Settings WRED parameters is done in two stages:
         * 1. Find new profile according to given Thresholds, and set accordingly
         * 2. If exact is different than given thresholds, Find new profile according to exact
         *  and set accordingly the profile management
         */

        /*
         * Note: This process might lead to a false confirm, when configuration is not exact and there are not enough
         * profiles that are free. This can be resolved if all configurations are exact
         */

        /** Find new profile according to given settings and commit changes */
        SHR_IF_ERR_EXIT(dnx_vsq_rate_class_exchange_and_set(unit, current_core, vsq_type, vsq_index, &data_rate_class));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * retrieve VSQ WRED configuration
 */
int
dnx_ingress_congestion_vsq_discard_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard)
{
    int core_id = BCM_CORE_ALL;
    int vsq_id;
    dnx_ingress_congestion_vsq_group_e vsq_type;
    dnx_ingress_congestion_rate_class_threshold_key_t rate_class_key;
    int rate_cls;
    dnx_ingress_congestion_wred_info_t wred_data;

    SHR_FUNC_INIT_VARS(unit);

    /** retrieve VSQ id */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_gport_vsq_get(unit, gport, cosq, &core_id, &vsq_type, &vsq_id));

    /** make coverity happy */
    SHR_MAX_VERIFY(vsq_type, DNX_INGRESS_CONGESTION_VSQ_GROUP_PG, _SHR_E_INTERNAL,
                   "vsq_type %d is not expected\n", vsq_type);

    SHR_IF_ERR_EXIT(dnx_vsq_rate_class_mapping_get(unit, core_id, vsq_type, vsq_id, &rate_cls));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_discard_key_get(unit, gport, discard, &rate_class_key));

    SHR_IF_ERR_EXIT(dnx_vsq_rate_class_wred_get(unit, core_id, vsq_type, rate_cls,
                                                rate_class_key.pool[0], rate_class_key.dp[0],
                                                &wred_data, (uint32 *) &discard->gain));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_wred_data_to_discard(unit, &wred_data, discard));

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - get resource type from flags.
 * used for color_size_set/get
 */
shr_error_e
dnx_ingress_congestion_color_size_res_type_get(
    int unit,
    uint32 flags,
    dnx_ingress_congestion_resource_type_e * res_type)
{
    SHR_FUNC_INIT_VARS(unit);
    *res_type = DNX_INGRESS_CONGESTION_RESOURCE_INVALID;

    if (flags & BCM_COSQ_GPORT_SIZE_BYTES)
    {
        *res_type = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES;
    }
    else if ((flags & BCM_COSQ_GPORT_SIZE_PACKET_DESC) && (flags & BCM_COSQ_GPORT_SIZE_OCB))
    {
        *res_type = DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS;
    }
    else if ((flags & BCM_COSQ_GPORT_SIZE_BUFFERS) && (flags & BCM_COSQ_GPORT_SIZE_OCB))
    {
        *res_type = DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * set VSQ tail drop configuration
 */
int
dnx_ingress_congestion_vsq_color_size_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_color_t color,
    uint32 flags,
    bcm_cosq_gport_size_t * gport_size)
{
    int core_id;
    bcm_core_t current_core;
    dnx_ingress_congestion_vsq_rate_class_info_t data_rate_class;
    int index;
    int vsq_index;
    dnx_ingress_congestion_resource_type_e res_type;

    dnx_ingress_congestion_vsq_group_e vsq_type;
    uint32 max_thresh;

    bcm_color_t start_color;
    bcm_color_t end_color;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_color_size_res_type_get(unit, flags, &res_type));

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_threshold_verify(unit, res_type, "size_max", gport_size->size_max));

    sal_memset(&data_rate_class, 0x0, sizeof(dnx_ingress_congestion_vsq_rate_class_info_t));

    /** retrieve VSQ id */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_gport_vsq_get(unit, gport, cosq, &core_id, &vsq_type, &vsq_index));

    /** make coverity happy */
    SHR_MAX_VERIFY(vsq_type, DNX_INGRESS_CONGESTION_VSQ_GROUP_PG, _SHR_E_INTERNAL,
                   "vsq_type %d is not expected\n", vsq_type);

    if (flags & BCM_COSQ_GPORT_SIZE_COLOR_BLIND)
    {
        start_color = 0;
        end_color = DNX_COSQ_NOF_DP;
    }
    else
    {
        start_color = color;
        end_color = color + 1;
    }

    DNX_INGRESS_CONGESTION_VSQ_CORES_ITER(unit, vsq_type, core_id, current_core)
    {
        /** Get old rate class data */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_alloc_vsq_rate_class_data_get(unit, current_core, vsq_type, vsq_index,
                                                                             &data_rate_class));

        /** Change related information */
        /** convert to exact */
        max_thresh = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, res_type, gport_size->size_max);

        for (index = start_color; index < end_color; index++)
        {
            /** this API is used for VSQ A-D only */
            data_rate_class.data.vsq_a_d_rate_class_info.taildrop.max_size_th[res_type][index] = max_thresh;
        }

        /** Find new profile according to given settings and commit changes */
        SHR_IF_ERR_EXIT(dnx_vsq_rate_class_exchange_and_set(unit, current_core, vsq_type, vsq_index, &data_rate_class));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * retrieve VSQ tail drop configuration
 */
int
dnx_ingress_congestion_vsq_color_size_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_color_t color,
    uint32 flags,
    bcm_cosq_gport_size_t * gport_size)
{
    int core_id = BCM_CORE_ALL;
    int vsq_index;
    dnx_ingress_congestion_resource_type_e res_type;
    dnx_ingress_congestion_vsq_group_e vsq_type;
    int rate_cls;

    SHR_FUNC_INIT_VARS(unit);

    /** retrieve VSQ id */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_gport_vsq_get(unit, gport, cosq, &core_id, &vsq_type, &vsq_index));

    /** make coverity happy */
    SHR_MAX_VERIFY(vsq_type, DNX_INGRESS_CONGESTION_VSQ_GROUP_PG, _SHR_E_INTERNAL,
                   "vsq_type %d is not expected\n", vsq_type);

    SHR_IF_ERR_EXIT(dnx_vsq_rate_class_mapping_get(unit, core_id, vsq_type, vsq_index, &rate_cls));

    if (flags & BCM_COSQ_GPORT_SIZE_COLOR_BLIND)
    {
        color = 0;
    }

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_color_size_res_type_get(unit, flags, &res_type));

    SHR_IF_ERR_EXIT(dnx_vsq_rate_class_tail_drop_get(unit, core_id, vsq_type, res_type, rate_cls, color,
                                                     &gport_size->size_max));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Convert Flow Control flags to resource type FC threshold
 */
static int
dnx_ingress_congestion_pfc_flags_to_resource(
    int unit,
    uint32 flags,
    dnx_ingress_congestion_resource_type_e * resource)
{
    dnx_ingress_congestion_resource_type_e resource_type = DNX_INGRESS_CONGESTION_RESOURCE_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    switch (flags & (BCM_COSQ_PFC_BYTES | BCM_COSQ_PFC_OCB | BCM_COSQ_PFC_BUFFERS | BCM_COSQ_PFC_PACKET_DESC))
    {
        case BCM_COSQ_PFC_BYTES:
            resource_type = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES;
            break;
        case BCM_COSQ_PFC_OCB | BCM_COSQ_PFC_BUFFERS:
            resource_type = DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS;
            break;
        case BCM_COSQ_PFC_OCB | BCM_COSQ_PFC_PACKET_DESC:
            resource_type = DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown resource type.\n");
    }

    *resource = resource_type;

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_ingress_congestion_pfc_flags_to_pool_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    int pool[],
    int *nof_pools)
{
    int index = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_COSQ_PFC_POOL_0)
    {
        pool[index++] = 0;
    }
    if (flags & BCM_COSQ_PFC_POOL_1)
    {
        pool[index++] = 1;
    }

    *nof_pools = index;
    if (*nof_pools == 0)
    {
        if (BCM_COSQ_GPORT_IS_VSQ_SRC_PORT(gport))
        {
            /** If no pool flag specified, set both pools */
            *nof_pools = dnx_data_ingr_congestion.vsq.pool_nof_get(unit);
            for (index = 0; index < *nof_pools; index++)
            {
                pool[index] = index;
            }
        }
        else
        {
            /** pool is not relevant */
            *nof_pools = 1;
            pool[0] = 0;
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 * verify VSQ FC parameters
 */
static int
dnx_ingress_congestion_vsq_fc_threshold_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 flags,
    bcm_cosq_pfc_config_t * pfc_threshold)
{
    uint32 allowed_flags;

    SHR_FUNC_INIT_VARS(unit);

    /** NULL check */
    SHR_NULL_CHECK(pfc_threshold, _SHR_E_PARAM, "pfc_threshold");

    allowed_flags = BCM_COSQ_PFC_BYTES | BCM_COSQ_PFC_OCB | BCM_COSQ_PFC_PACKET_DESC | BCM_COSQ_PFC_BUFFERS;

    if (BCM_COSQ_GPORT_IS_VSQ_SRC_PORT(gport))
    {
        allowed_flags |= (BCM_COSQ_PFC_POOL_0 | BCM_COSQ_PFC_POOL_1);
    }

    SHR_MASK_VERIFY(flags, allowed_flags, _SHR_E_PARAM, "Unsupported flag provided.\n");

    /*
     * Verify exactly one resource flag is set
     */
    SHR_NOF_SET_BITS_IN_RANGE_VERIFY(flags,
                                     (BCM_COSQ_PFC_BYTES | BCM_COSQ_PFC_BUFFERS |
                                      BCM_COSQ_PFC_PACKET_DESC), 1, 1, _SHR_E_PARAM,
                                     "Exactly one resource flag should be specified\n");

    /*
     * Make sure buffers and packet descriptors is always specified with OCB
     */
    if ((flags & (BCM_COSQ_PFC_BUFFERS | BCM_COSQ_PFC_PACKET_DESC)) && !(flags & BCM_COSQ_PFC_OCB))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Buffers and Packet descriptors should always be specified with OCB flag\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * verify VSQ FC parameters
 */
static int
dnx_ingress_congestion_vsq_fc_threshold_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 flags,
    bcm_cosq_pfc_config_t * pfc_threshold)
{

    dnx_ingress_congestion_resource_type_e rsrc_type = DNX_INGRESS_CONGESTION_RESOURCE_INVALID;
    int alpha_min, alpha_max;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_fc_threshold_verify(unit, gport, cosq, flags, pfc_threshold));

    if ((pfc_threshold->xon_threshold_bd != 0) || (pfc_threshold->xoff_threshold_bd != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "xon_threshold_bd %d and xoff_threshold_bd %d must be 0\n",
                     pfc_threshold->xon_threshold_bd, pfc_threshold->xoff_threshold_bd);
    }

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_pfc_flags_to_resource(unit, flags, &rsrc_type));

    if (BCM_COSQ_GPORT_IS_VSQ_PG(gport))
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit, rsrc_type, "xoff_fadt_threshold.thresh_max",
                                                                pfc_threshold->xoff_fadt_threshold.thresh_max));
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit, rsrc_type, "xoff_fadt_threshold.thresh_min",
                                                                pfc_threshold->xoff_fadt_threshold.thresh_min));
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit, rsrc_type, "xon_fadt_offset",
                                                                pfc_threshold->xon_fadt_offset));

        /*
         * Verify alpha
         */
        alpha_min = dnx_data_ingr_congestion.info.resource_get(unit, rsrc_type)->fadt_alpha_min;
        alpha_max = dnx_data_ingr_congestion.info.resource_get(unit, rsrc_type)->fadt_alpha_max;
        SHR_RANGE_VERIFY(pfc_threshold->xoff_fadt_threshold.alpha, alpha_min, alpha_max,
                         _SHR_E_PARAM, "alpha %d is out of range\n", pfc_threshold->xoff_fadt_threshold.alpha);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit, rsrc_type, "xoff_threshold",
                                                                pfc_threshold->xoff_threshold));
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_threshold_verify(unit, rsrc_type, "xon_threshold",
                                                                pfc_threshold->xon_threshold));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * set FC threshold for VSQ
 */
int
dnx_ingress_congestion_vsq_fc_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 flags,
    bcm_cosq_pfc_config_t * pfc_threshold)
{
    int core_id = BCM_CORE_ALL;
    int current_core;
    dnx_ingress_congestion_vsq_rate_class_info_t data_rate_class;
    int vsq_index;
    dnx_ingress_congestion_vsq_group_e vsq_type;
    int pool[DNX_DATA_MAX_INGR_CONGESTION_VSQ_POOL_NOF];

    int pool_id = 0, nof_pools = 0;
    dnx_ingress_congestion_resource_type_e rsrc_type = DNX_INGRESS_CONGESTION_RESOURCE_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_vsq_fc_threshold_set_verify(unit, gport, cosq, flags, pfc_threshold));

    /** retrieve VSQ id */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_gport_vsq_get(unit, gport, cosq, &core_id, &vsq_type, &vsq_index));

    /** make coverity happy */
    SHR_MAX_VERIFY(vsq_type, DNX_INGRESS_CONGESTION_VSQ_GROUP_PG, _SHR_E_INTERNAL,
                   "vsq_type %d is not expected\n", vsq_type);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_pfc_flags_to_resource(unit, flags, &rsrc_type));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_pfc_flags_to_pool_get(unit, gport, flags, pool, &nof_pools));

    DNX_INGRESS_CONGESTION_VSQ_CORES_ITER(unit, vsq_type, core_id, current_core)
    {

        /** Get Rate class data */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_alloc_vsq_rate_class_data_get(unit, current_core, vsq_type, vsq_index,
                                                                             &data_rate_class));

        for (pool_id = 0; pool_id < nof_pools; ++pool_id)
        {

            if (vsq_type == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
            {
                /** FC thresholds for VSQ-F */
                data_rate_class.data.vsq_f_rate_class_info.fadt_fc[rsrc_type].set.max_threshold =
                    DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, rsrc_type,
                                                                       pfc_threshold->xoff_fadt_threshold.thresh_max);
                data_rate_class.data.vsq_f_rate_class_info.fadt_fc[rsrc_type].set.min_threshold =
                    DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, rsrc_type,
                                                                       pfc_threshold->xoff_fadt_threshold.thresh_min);
                data_rate_class.data.vsq_f_rate_class_info.fadt_fc[rsrc_type].set.alpha =
                    pfc_threshold->xoff_fadt_threshold.alpha;

                data_rate_class.data.vsq_f_rate_class_info.fadt_fc[rsrc_type].clear_offset =
                    DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, rsrc_type, pfc_threshold->xon_fadt_offset);
            }
            else
            {
                /** FC thresholds for VSQs A-E */
                dnx_cosq_hyst_threshold_t *fc;

                fc = ((vsq_type == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT) ?
                      /** VSQ-E */
                      &data_rate_class.data.vsq_e_rate_class_info.pool[pool_id].fc[rsrc_type] :
                      /** VSQ A-D */
                      &data_rate_class.data.vsq_a_d_rate_class_info.fc[rsrc_type]);

                fc->set = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, rsrc_type,
                                                                             pfc_threshold->xoff_threshold);
                fc->clear = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, rsrc_type,
                                                                               pfc_threshold->xon_threshold);
            }

        }

        /** Find new profile according to given settings and commit changes */
        SHR_IF_ERR_EXIT(dnx_vsq_rate_class_exchange_and_set(unit, current_core, vsq_type, vsq_index, &data_rate_class));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get FC threshold for VSQ
 */
int
dnx_ingress_congestion_vsq_fc_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 flags,
    bcm_cosq_pfc_config_t * pfc_threshold)
{
    int core_id = BCM_CORE_ALL;
    int vsq_index;
    int rate_cls;
    int pool[DNX_DATA_MAX_INGR_CONGESTION_VSQ_POOL_NOF];
    int nof_pools;

    dnx_ingress_congestion_resource_type_e rsrc_type = DNX_INGRESS_CONGESTION_RESOURCE_INVALID;

    dnx_ingress_congestion_vsq_group_e vsq_type;

    dnx_cosq_fadt_hyst_threshold_t fadt_fc;
    dnx_cosq_hyst_threshold_t hyst_fc;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_vsq_fc_threshold_verify(unit, gport, cosq, flags, pfc_threshold));

    /** retrieve VSQ id */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_gport_vsq_get(unit, gport, cosq, &core_id, &vsq_type, &vsq_index));

    /** make coverity happy */
    SHR_MAX_VERIFY(vsq_type, DNX_INGRESS_CONGESTION_VSQ_GROUP_PG, _SHR_E_INTERNAL,
                   "vsq_type %d is not expected\n", vsq_type);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_pfc_flags_to_resource(unit, flags, &rsrc_type));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_pfc_flags_to_pool_get(unit, gport, flags, pool, &nof_pools));

    SHR_IF_ERR_EXIT(dnx_vsq_rate_class_mapping_get(unit, core_id, vsq_type, vsq_index, &rate_cls));

    if (vsq_type == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
    {
        /** FC thresholds for VSQ-F */
        SHR_IF_ERR_EXIT(dnx_vsq_rate_class_fadt_fc_get(unit, core_id, rsrc_type, rate_cls, &fadt_fc));

        pfc_threshold->xoff_fadt_threshold.thresh_max = fadt_fc.set.max_threshold;
        pfc_threshold->xoff_fadt_threshold.thresh_min = fadt_fc.set.min_threshold;
        pfc_threshold->xoff_fadt_threshold.alpha = fadt_fc.set.alpha;
        pfc_threshold->xon_fadt_offset = fadt_fc.clear_offset;

    }
    else
    {
        /** FC thresholds for VSQs A-E */

        SHR_IF_ERR_EXIT(dnx_vsq_rate_class_hyst_fc_get(unit, core_id, vsq_type, pool[0], rsrc_type, rate_cls,
                                                       &hyst_fc));

        pfc_threshold->xoff_threshold = hyst_fc.set;
        pfc_threshold->xon_threshold = hyst_fc.clear;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * find maximum reserved amount of the VSQ across all colors.
 *
 * Since each (VSQ-E,pool)/VSQ-F is counted once in total guaranteed, need to find
 * the maximum reserved amount of the VSQ across all colors.
 * Each pool of VSQ-E is counted separately.
 */
static uint32
dnx_ingress_congestion_vsq_max_reserved_get(
    int unit,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    int pool_id,
    dnx_ingress_congestion_vsq_rate_class_info_t * data_rate_class)
{

    uint32 *guaranteed_array;
    uint32 max_reserved = 0;
    int color = 0;

    if (vsq_type == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
    {
        guaranteed_array = data_rate_class->data.vsq_e_rate_class_info.pool[pool_id].guaranteed_size[rsrc_type];
    }
    else
    {
        guaranteed_array = data_rate_class->data.vsq_f_rate_class_info.guaranteed_size[rsrc_type];
    }

    /** Find maximum reserved size of all colors in VSQ */
    for (color = 0; color < DNX_COSQ_NOF_DP; ++color)
    {
        max_reserved = UTILEX_MAX(max_reserved, guaranteed_array[color]);
    }

    return max_reserved;
}

/**
 * \brief -
 *  Check there are enough resources after reserve_change will be updated,
 */
static int
dnx_ingress_congestion_global_vsq_partition_verify(
    int unit,
    int core_id,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    dnx_ingress_congestion_global_vsq_resource_alloc_params_t resource_alloc_hw_params[],
    uint32 total_guaranteed_hw)
{
    uint32 total_size_hw;
    int pool_id;
    uint32 sum_all_segments_hw;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Since total resource in bytes is 4G for Jericho 2,
     * we can not perform signed arithmetics on the resources in user units.
     * Thus we perform all the checks related to sum of all segments in HW units.
     * (In HW units, total bytes is 2^28 for Jericho 2)
     */

    sum_all_segments_hw = total_guaranteed_hw;

    for (pool_id = 0; pool_id < dnx_data_ingr_congestion.vsq.pool_nof_get(unit); pool_id++)
    {
        sum_all_segments_hw += resource_alloc_hw_params[pool_id].shared_pool;
        sum_all_segments_hw += resource_alloc_hw_params[pool_id].nominal_headroom;
    }

    total_size_hw = dnx_data_ingr_congestion.info.resource_get(unit, rsrc_type)->hw_resolution_max;
    if (sum_all_segments_hw > total_size_hw)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                     "Run out of VSQ resources for resource %d. Consider to reduce pools size.\n", rsrc_type);

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Check there are enough resources after reserve_change will be updated,
 */
static int
dnx_ingress_congestion_vsq_total_guaranteed_verify(
    int unit,
    int core_id,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    uint32 old_max_guaranteed,
    uint32 new_max_guaranteed)
{
    uint32 total_guaranteed_hw, new_total_guaranteed_hw;
    uint32 old_guaranteed_hw, new_guaranteed_hw;
    int pool_id;

    dnx_ingress_congestion_global_vsq_resource_alloc_params_t
        resource_alloc_hw_params[DNX_DATA_MAX_INGR_CONGESTION_VSQ_POOL_NOF];

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Since total resource in bytes is 4G for Jericho 2,
     * we can not perform signed arithmetics on the resources in user units.
     * Thus we perform all the checks related to sum of all segments in HW units.
     * (In HW units, total bytes is 2^28 for Jericho 2)
     */

    /** total guaranteed is stored in SW state in HW units */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.guaranteed.vsq_guaranteed.get(unit, core_id, rsrc_type,
                                                                            &total_guaranteed_hw));

    old_guaranteed_hw = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, rsrc_type, old_max_guaranteed);

    new_guaranteed_hw = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, rsrc_type, new_max_guaranteed);

    new_total_guaranteed_hw = total_guaranteed_hw - old_guaranteed_hw + new_guaranteed_hw;

    for (pool_id = 0; pool_id < dnx_data_ingr_congestion.vsq.pool_nof_get(unit); pool_id++)
    {
        /** this function returns values in HW units */
        SHR_IF_ERR_EXIT(dnx_global_vsq_resource_alloc_hw_get(unit, core_id, rsrc_type, pool_id,
                                                             &resource_alloc_hw_params[pool_id]));
    }

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_vsq_partition_verify(unit, core_id, rsrc_type,
                                                                       resource_alloc_hw_params,
                                                                       new_total_guaranteed_hw));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  update the new reserved resources in sw_state.
 */
static int
dnx_ingress_congestion_vsq_total_reserve_update(
    int unit,
    int core_id,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    uint32 old_max_guaranteed,
    uint32 new_max_guaranteed)
{
    uint32 old_guaranteed_hw, new_guaranteed_hw, total_guaranteed_hw;

    SHR_FUNC_INIT_VARS(unit);

    old_guaranteed_hw = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, rsrc_type, old_max_guaranteed);

    new_guaranteed_hw = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, rsrc_type, new_max_guaranteed);

    /** vsq guaranteed SW state is in HW units */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.guaranteed.vsq_guaranteed.get(unit, core_id, rsrc_type,
                                                                            &total_guaranteed_hw));

    total_guaranteed_hw += (new_guaranteed_hw - old_guaranteed_hw);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.guaranteed.vsq_guaranteed.set(unit, core_id, rsrc_type,
                                                                            total_guaranteed_hw));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify resource allocation for source VSQs - VSQ-E and VSQ-F
 */
static shr_error_e
dnx_ingress_congestion_src_vsq_resource_allocation_set_verify(
    int unit,
    uint32 flags,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    bcm_cosq_allocation_entity_t * target,
    bcm_cosq_resource_amounts_t * amounts)
{
    int core_id;
    dnx_ingress_congestion_vsq_group_e vsq_type;
    int vsq_index;

    dnx_ingress_congestion_pg_params_t pg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_gport_vsq_get(unit, target->gport, 0, &core_id, &vsq_type, &vsq_index));

    if (vsq_type == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_pg_params_get(unit, core_id, vsq_index, &pg_params));

        if (pg_params.pool_id != target->pool_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Target's pool_id(%d) must be equal to the allocated PG-VSQ(%d) pool_id(%d)\n",
                         target->pool_id, target->gport, pg_params.pool_id);
        }

        if (pg_params.is_lossless == FALSE && (amounts->nominal_headroom != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "PG-VSQ(0x%x) must be lossless in order to configure headroom\n", target->gport);
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set resource allocation for source VSQs - VSQ-E and VSQ-F
 */
shr_error_e
dnx_ingress_congestion_src_vsq_resource_allocation_set(
    int unit,
    uint32 flags,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    bcm_cosq_allocation_entity_t * target,
    bcm_cosq_resource_amounts_t * amounts)
{
    int core_id;
    dnx_ingress_congestion_vsq_group_e vsq_type;
    int vsq_index;
    dnx_ingress_congestion_resource_type_e temp_rsrc_type;
    int temp_dp;

    dnx_ingress_congestion_vsq_rate_class_info_t data_rate_class;
    bcm_color_t color, start_color, end_color;
    uint32 old_max_reserved = 0, new_max_reserved = 0;

    uint32 guaranteed = 0;
    int use_port_guaranteed;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_src_vsq_resource_allocation_set_verify(unit, flags, rsrc_type,
                                                                                        target, amounts));

    /** retrieve VSQ id */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_gport_vsq_get(unit, target->gport, 0, &core_id, &vsq_type, &vsq_index));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_alloc_vsq_rate_class_data_get(unit, core_id, vsq_type, vsq_index,
                                                                         &data_rate_class));

    if (flags & BCM_COSQ_RSRC_ALLOC_COLOR_BLIND)
    {
        /** Configure all colors the same */
        start_color = 0;
        end_color = DNX_COSQ_NOF_DP;
    }
    else
    {
        /** Configure only specific color */
        start_color = target->color;
        end_color = target->color + 1;
    }

    /** Configure max_headroom and max_shared_pool */
    for (color = start_color; color < end_color; ++color)
    {

        if (vsq_type == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
        {
            data_rate_class.data.vsq_e_rate_class_info.pool[target->pool_id].shared_size[rsrc_type][color] =
                DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, rsrc_type, amounts->max_shared_pool);
            data_rate_class.data.vsq_e_rate_class_info.pool[target->pool_id].headroom_size[rsrc_type] =
                DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, rsrc_type, amounts->max_headroom);
        }
        else if (vsq_type == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
        {
            if (amounts->shared_pool_fadt.thresh_max == 0 &&
                amounts->shared_pool_fadt.thresh_min == 0 && amounts->shared_pool_fadt.alpha == 0)
            {

                /*
                 * In case user didn't fill shared pool FADT struct, use max_shared_pool to configure taildrop
                 */
                data_rate_class.data.vsq_f_rate_class_info.shared_size[rsrc_type][color].max_threshold =
                    DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, rsrc_type, amounts->max_shared_pool);
                data_rate_class.data.vsq_f_rate_class_info.shared_size[rsrc_type][color].min_threshold =
                    DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, rsrc_type, amounts->max_shared_pool);
                data_rate_class.data.vsq_f_rate_class_info.shared_size[rsrc_type][color].alpha = 0;
            }
            else
            {
                /** Configure shared pool FADT */
                data_rate_class.data.vsq_f_rate_class_info.shared_size[rsrc_type][color].max_threshold =
                    DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, rsrc_type,
                                                                       amounts->shared_pool_fadt.thresh_max);
                data_rate_class.data.vsq_f_rate_class_info.shared_size[rsrc_type][color].min_threshold =
                    DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, rsrc_type,
                                                                       amounts->shared_pool_fadt.thresh_min);
                data_rate_class.data.vsq_f_rate_class_info.shared_size[rsrc_type][color].alpha =
                    amounts->shared_pool_fadt.alpha;
            }

            data_rate_class.data.vsq_f_rate_class_info.headroom_size[rsrc_type].max_headroom =
                DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, rsrc_type, amounts->max_headroom);
            data_rate_class.data.vsq_f_rate_class_info.headroom_size[rsrc_type].max_headroom_nominal =
                DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, rsrc_type, amounts->nominal_headroom);
            data_rate_class.data.vsq_f_rate_class_info.headroom_size[rsrc_type].max_headroom_extension =
                DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, rsrc_type,
                                                                   amounts->max_headroom - amounts->nominal_headroom);
        }
    }

    /** Find old maximum reserved size of all colors in VSQ */
    old_max_reserved = dnx_ingress_congestion_vsq_max_reserved_get(unit, rsrc_type, vsq_type, target->pool_id,
                                                                   &data_rate_class);

    guaranteed = amounts->reserved;

    /** Configure reserved (guaranteed) size */
    for (color = start_color; color < end_color; ++color)
    {
        if (vsq_type == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
        {

            data_rate_class.data.vsq_e_rate_class_info.pool[target->pool_id].guaranteed_size[rsrc_type][color] =
                DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, rsrc_type, guaranteed);
        }
        else if (vsq_type == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
        {

            data_rate_class.data.vsq_f_rate_class_info.guaranteed_size[rsrc_type][color] =
                DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, rsrc_type, guaranteed);
        }
    }

    /*
     * Find the new maximum reserved size of all colors in VSQ
     * rate class is consistent with the reserved configured in the HW
     */
    new_max_reserved = dnx_ingress_congestion_vsq_max_reserved_get(unit, rsrc_type, vsq_type, target->pool_id,
                                                                   &data_rate_class);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_vsq_total_guaranteed_verify(unit, core_id, rsrc_type,
                                                                             old_max_reserved, new_max_reserved));

    /** Find new profile according to given settings and commit changes */
    SHR_IF_ERR_EXIT(dnx_vsq_rate_class_exchange_and_set(unit, core_id, vsq_type, vsq_index, &data_rate_class));

    if (vsq_type == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
    {
        /*
         * If reservation is configured, it needs to be taken from VSQF (i.e. use_port_guaranteed = FALSE).
         * If reservation is NOT configured, it needs to be taken from VSQE (i.e. use_port_guaranteed = TRUE).
         */
        use_port_guaranteed = TRUE;
        for (temp_rsrc_type = 0; temp_rsrc_type < DNX_INGRESS_CONGESTION_RESOURCE_NOF; ++temp_rsrc_type)
        {
            for (temp_dp = 0; temp_dp < DNX_COSQ_NOF_DP; temp_dp++)
            {
                if (data_rate_class.data.vsq_f_rate_class_info.guaranteed_size[temp_rsrc_type][temp_dp] != 0)
                {
                    use_port_guaranteed = FALSE;
                    break;
                }
            }
            if (use_port_guaranteed == FALSE)
            {
                break;
            }
        }

        /** PG params are per PG-VSQ */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_pg_use_port_guaranteed_set(unit, core_id, vsq_index,
                                                                               use_port_guaranteed));
    }

    /** update total reserved resources */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_total_reserve_update(unit, core_id, rsrc_type,
                                                                    old_max_reserved, new_max_reserved));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 * Get resource allocation for source VSQs - VSQ-E and VSQ-F
 */
shr_error_e
dnx_ingress_congestion_src_vsq_resource_allocation_get(
    int unit,
    uint32 flags,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    bcm_cosq_allocation_entity_t * target,
    bcm_cosq_resource_amounts_t * amounts)
{
    int core_id;
    dnx_ingress_congestion_vsq_group_e vsq_type;
    int vsq_index;
    int rate_class;

    dnx_ingress_congestion_vsq_e_resource_alloc_params_t vsq_e_resource_alloc_params;
    dnx_ingress_congestion_vsq_f_resource_alloc_params_t vsq_f_resource_alloc_params;

    SHR_FUNC_INIT_VARS(unit);

    /** retrieve VSQ id */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_gport_vsq_get(unit, target->gport, 0, &core_id, &vsq_type, &vsq_index));

    SHR_IF_ERR_EXIT(dnx_vsq_rate_class_mapping_get(unit, core_id, vsq_type, vsq_index, &rate_class));

    /** Get reserved */
    if (vsq_type == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
    {

        SHR_IF_ERR_EXIT(dnx_vsq_e_rate_class_resource_allocation_get(unit, core_id, rsrc_type, rate_class,
                                                                     target->pool_id, target->color,
                                                                     &vsq_e_resource_alloc_params));

        amounts->reserved = vsq_e_resource_alloc_params.guaranteed;
        amounts->max_shared_pool = vsq_e_resource_alloc_params.shared_pool;
        amounts->max_headroom = vsq_e_resource_alloc_params.headroom;
        amounts->nominal_headroom = amounts->max_headroom;
    }
    else if (vsq_type == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
    {

        SHR_IF_ERR_EXIT(dnx_vsq_f_rate_class_resource_allocation_get(unit, core_id, rsrc_type,
                                                                     rate_class, target->color,
                                                                     &vsq_f_resource_alloc_params));

        amounts->reserved = vsq_f_resource_alloc_params.guaranteed;
        amounts->shared_pool_fadt.thresh_min = vsq_f_resource_alloc_params.shared_pool_fadt.min_threshold;
        amounts->shared_pool_fadt.thresh_max = vsq_f_resource_alloc_params.shared_pool_fadt.max_threshold;
        amounts->shared_pool_fadt.alpha = vsq_f_resource_alloc_params.shared_pool_fadt.alpha;
        amounts->max_headroom = vsq_f_resource_alloc_params.max_headroom;
        amounts->nominal_headroom = vsq_f_resource_alloc_params.nominal_headroom;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set resource allocation for global VSQ
 */
static shr_error_e
dnx_ingress_congestion_global_vsq_resource_allocation_set_verify(
    int unit,
    uint32 flags,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    bcm_cosq_allocation_entity_t * target,
    bcm_cosq_resource_amounts_t * amounts)
{
    bcm_core_t core_id;

    int lossless_pool = -1;
    int my_pool_id, other_pool_id;

    uint32 total_guaranteed_hw;
    dnx_ingress_congestion_global_vsq_resource_alloc_params_t
        resource_alloc_hw_params[DNX_DATA_MAX_INGR_CONGESTION_VSQ_POOL_NOF];

    SHR_FUNC_INIT_VARS(unit);

    core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(target->gport);

    /*
     * Don't let the user configure something he didn't want to,
     * so force him to set max_headroom and nominal_headroom the same.
     */
    if (rsrc_type == DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES && amounts->max_headroom != amounts->nominal_headroom)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "max_headroom must be equal to nominal_headroom when allocating total Words.");
    }

    /*
     * Only one pool can be associated with lossless traffic.
     * The first pool to allocate Headroom will be the lossless pool.
     */
    if (amounts->max_headroom != 0 || amounts->nominal_headroom != 0)    /** new headroom to configure */
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.port_based_vsq.lossless_pool_id.get(unit, core_id, &lossless_pool));

        if (lossless_pool != -1 && target->pool_id != lossless_pool)
        {
            /** validate headroom is allocated only for the lossless pool */
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Can't allocate headroom to pool %d. Headroom is already assigned to Pool %d.",
                         target->pool_id, lossless_pool);
        }
    }

    /*
     * Since total resource in bytes is 4G for Jericho 2,
     * we can not perform signed arithmetics on the resources in user units.
     * Thus we perform all the checks related to sum of all segments in HW units.
     * (In HW units, total bytes is 2^28 for Jericho 2)
     */

    /** vsq guaranteed SW state is in HW units */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.guaranteed.vsq_guaranteed.get(unit, core_id, rsrc_type,
                                                                            &total_guaranteed_hw));

    my_pool_id = target->pool_id;
    resource_alloc_hw_params[my_pool_id].shared_pool =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, rsrc_type, amounts->max_shared_pool);
    resource_alloc_hw_params[my_pool_id].nominal_headroom =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, rsrc_type, amounts->nominal_headroom);

    other_pool_id = (my_pool_id == 0 ? 1 : 0);

    /** Get existing global configurations */
    /** this function returns values in HW units */
    SHR_IF_ERR_EXIT(dnx_global_vsq_resource_alloc_hw_get(unit, core_id, rsrc_type, other_pool_id,
                                                         &resource_alloc_hw_params[other_pool_id]));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_vsq_partition_verify(unit, core_id, rsrc_type,
                                                                       resource_alloc_hw_params, total_guaranteed_hw));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set resource allocation for global VSQ
 */
shr_error_e
dnx_ingress_congestion_global_vsq_resource_allocation_set(
    int unit,
    uint32 flags,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    bcm_cosq_allocation_entity_t * target,
    bcm_cosq_resource_amounts_t * amounts)
{
    bcm_core_t core_id;

    dnx_ingress_congestion_global_vsq_resource_alloc_params_t resource_alloc_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_global_vsq_resource_allocation_set_verify(unit, flags, rsrc_type,
                                                                                           target, amounts));

    core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(target->gport);

    /*
     * Only one pool can be associated with lossless traffic.
     * The first pool to allocate Headroom will be the lossless pool.
     */
    if (((amounts->max_headroom != 0) || (amounts->nominal_headroom != 0)))   /** new headroom to configure */
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.port_based_vsq.lossless_pool_id.set(unit, core_id, target->pool_id));
    }

    resource_alloc_params.shared_pool = amounts->max_shared_pool;
    resource_alloc_params.nominal_headroom = amounts->nominal_headroom;
    resource_alloc_params.max_headroom = amounts->max_headroom;

    SHR_IF_ERR_EXIT(dnx_global_vsq_resource_alloc_set(unit, core_id, rsrc_type, target->pool_id,
                                                      &resource_alloc_params));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get resource allocation for global VSQ
 */
shr_error_e
dnx_ingress_congestion_global_vsq_resource_allocation_get(
    int unit,
    uint32 flags,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    bcm_cosq_allocation_entity_t * target,
    bcm_cosq_resource_amounts_t * amounts)
{
    bcm_core_t core_id;
    uint32 guaranteed;

    dnx_ingress_congestion_global_vsq_resource_alloc_params_t resource_alloc_params;

    SHR_FUNC_INIT_VARS(unit);

    core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(target->gport);

    SHR_IF_ERR_EXIT(dnx_global_vsq_resource_alloc_get(unit, core_id, rsrc_type, target->pool_id,
                                                      &resource_alloc_params));

    amounts->max_shared_pool = resource_alloc_params.shared_pool;
    amounts->nominal_headroom = resource_alloc_params.nominal_headroom;
    amounts->max_headroom = resource_alloc_params.max_headroom;

    /** total guaranteed is stored in SW state in HW units */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.guaranteed.vsq_guaranteed.get(unit, core_id, rsrc_type, &guaranteed));

    amounts->reserved = DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, rsrc_type, guaranteed);

exit:
    SHR_FUNC_EXIT;
}

/*
 * VSQ code - end
 */

/*
 * Other functions - start
 */

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_override_pp_port_with_reassembly_context_enable_set(
    int unit,
    int enable)
{
    uint32 max_reassembly_ctxt = dnx_data_ingr_reassembly.context.nof_contexts_get(unit) - 1;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_override_pp_port_with_reassembly_context_enable_range_set
                    (unit, 0, max_reassembly_ctxt, enable));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ingress_congestion_discard_set_verify(
    int unit,
    uint32 flags)
{
    uint32 enable_ecn;
    SHR_FUNC_INIT_VARS(unit);

    enable_ecn = flags & BCM_COSQ_DISCARD_MARK_CONGESTION;
    /*
     * Verify unsupported flag
     */
    if (enable_ecn)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flag BCM_COSQ_DISCARD_MARK_CONGESTION is not supported. ECN is enabled by default");
    }
    /*
     * Verify flags combinations
     */
    switch (flags)
    {
        case (BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_ALL):
        case (BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK | BCM_COSQ_DISCARD_COLOR_RED | BCM_COSQ_DISCARD_COLOR_YELLOW):
        case (BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK | BCM_COSQ_DISCARD_COLOR_RED):
        case (BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK):
            break;
        case 0:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flag %d. Black packets are always dropped.\n", flags);
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flag combination 0x%x\n", flags);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_discard_set(
    int unit,
    uint32 flags)
{
    uint32 discard_dp;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_discard_set_verify(unit, flags));

    switch (flags)
    {
        case (BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_ALL):
            /** Drop all packets */
            discard_dp = 0;
            break;
        case (BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK | BCM_COSQ_DISCARD_COLOR_RED | BCM_COSQ_DISCARD_COLOR_YELLOW):
            /** Drop all packets with DP 1 or above */
            discard_dp =
                1;
            break;
        case (BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK | BCM_COSQ_DISCARD_COLOR_RED):
            /** Drop all packets with DP 2 or above */
            discard_dp = 2;
            break;
        case (BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK):
            /** Drop all packets with DP 3 or above */
            discard_dp = 3;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported flag combination 0x%x\n", flags);
    }

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_global_dp_discard_set(unit, discard_dp));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_discard_get(
    int unit,
    uint32 *flags)
{
    uint32 discard_dp = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_global_dp_discard_get(unit, &discard_dp));

    switch (discard_dp)
    {
        case 0:
            /** Drop all packets */
            *flags |= (BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_ALL);
            break;
        case 1:
            /** Drop all packets with DP 1 or above */
            *flags |=
                (BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK | BCM_COSQ_DISCARD_COLOR_RED |
                 BCM_COSQ_DISCARD_COLOR_YELLOW);
            break;
        case 2:
            /** Drop all packets with DP 2 or above */
            *flags |= (BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK | BCM_COSQ_DISCARD_COLOR_RED);
            break;
        case 3:
            /** Drop all packets with DP 3 or above */
            *flags |= (BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported flag combination\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify procedure for bcm_dnx_cosq_mirror_on_drop_group_set API
 */
static shr_error_e
dnx_ingress_congestion_mirror_on_drop_group_set_verify(
    int unit,
    uint32 flags,
    int group_id,
    bcm_cosq_mirror_on_drop_group_info_t * group_info)
{
    int reject_reason_id = 0;
    const dnx_data_stat_drop_reasons_drop_reason_index_t *reject_reason_hw;

    SHR_FUNC_INIT_VARS(unit);

    /** NULL check */
    SHR_NULL_CHECK(group_info, _SHR_E_PARAM, "group_info");

    if (dnx_data_ingr_congestion.mirror_on_drop.feature_get(unit, dnx_data_ingr_congestion_mirror_on_drop_is_supported)
        == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Mirror on Drop feature is not supported for this device\n");
    }

    /** check group id range */
    if ((group_id < 0) || (group_id >= dnx_data_ingr_congestion.mirror_on_drop.nof_groups_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Incorrect group id=%d\n", group_id);
    }

    /** check amount of drop reasons (array size) validity */
    if ((group_info->drop_reasons_count < 0) ||
        (group_info->drop_reasons_count > dnx_data_stat.drop_reasons.nof_drop_reasons_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Incorrect drop_reasons_count=%d.\n", group_info->drop_reasons_count);
    }

    /** check provided reject reasons are supported */
    for (reject_reason_id = 0; reject_reason_id < group_info->drop_reasons_count; reject_reason_id++)
    {
        /** verify drop reason is in range */
        if ((group_info->reasons_array[reject_reason_id] < 0) ||
            (group_info->reasons_array[reject_reason_id] >= bcmCosqDropReasonRejectCount))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Drop reason %d is out of range 0-%d.\n",
                         group_info->reasons_array[reject_reason_id], (bcmCosqDropReasonRejectCount - 1));
        }

        reject_reason_hw =
            dnx_data_stat.drop_reasons.drop_reason_index_get(unit, group_info->reasons_array[reject_reason_id]);

        if (reject_reason_hw->index == -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported reject_reason=%d\n", group_info->reasons_array[reject_reason_id]);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - API to set mirror on drop group info (reject bitmap and type)
 *
 * \param [in] unit - The unit number.
 * \param [in] flags - flags.
 * \param [in] group_id - drop group id.
 * \param [in] group_info - mirror on drop group info.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_mirror_on_drop_group_set(
    int unit,
    uint32 flags,
    int group_id,
    bcm_cosq_mirror_on_drop_group_info_t * group_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_mirror_on_drop_group_set_verify(unit, flags, group_id, group_info));

    /** set drop group */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_mirror_on_drop_group_set(unit, flags, group_id, group_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify procedure for bcm_dnx_cosq_mirror_on_drop_group_get API
 */
static shr_error_e
dnx_ingress_congestion_mirror_on_drop_group_get_verify(
    int unit,
    uint32 flags,
    int group_id,
    bcm_cosq_mirror_on_drop_group_info_t * group_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** NULL check */
    SHR_NULL_CHECK(group_info, _SHR_E_PARAM, "group_info");

    if (dnx_data_ingr_congestion.mirror_on_drop.feature_get(unit, dnx_data_ingr_congestion_mirror_on_drop_is_supported)
        == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Mirror on Drop feature is not supported for this device\n");
    }

    /** check group id range */
    if ((group_id < 0) || (group_id >= dnx_data_ingr_congestion.mirror_on_drop.nof_groups_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Incorrect group id=%d\n", group_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - API to get mirror on drop group info (reject bitmap and type)
 *
 * \param [in] unit - The unit number.
 * \param [in] flags - flags.
 * \param [in] group_id - drop group id.
 * \param [out] group_info - mirror on drop group info.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_mirror_on_drop_group_get(
    int unit,
    uint32 flags,
    int group_id,
    bcm_cosq_mirror_on_drop_group_info_t * group_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_mirror_on_drop_group_get_verify(unit, flags, group_id, group_info));

    /** set drop group */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_mirror_on_drop_group_get(unit, flags, group_id, group_info));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
static shr_error_e
dnx_ingress_congestion_mirror_on_drop_aging_set_verify(
    int unit,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    /** check if mirror on drop is supported */
    if (dnx_data_ingr_congestion.
        mirror_on_drop.feature_get(unit, dnx_data_ingr_congestion_mirror_on_drop_is_supported) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Mirror on Drop feature is not supported for this device\n");
    }

    /** verify aging time */
    if (arg < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid aging time=%d\n", arg);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_ingress_congestion_mirror_on_drop_aging_set(
    int unit,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_mirror_on_drop_aging_set_verify(unit, type, arg));

    /** set aging */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_mirror_on_drop_aging_set(unit, type, arg));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
static shr_error_e
dnx_ingress_congestion_mirror_on_drop_aging_get_verify(
    int unit,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    /** check if mirror on drop is supported */
    if (dnx_data_ingr_congestion.
        mirror_on_drop.feature_get(unit, dnx_data_ingr_congestion_mirror_on_drop_is_supported) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Mirror on Drop feature is not supported for this device\n");
    }

    /** verify NULL ptr */
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_ingress_congestion_mirror_on_drop_aging_get(
    int unit,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_mirror_on_drop_aging_get_verify(unit, type, arg));

    /** get aging */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_mirror_on_drop_aging_get(unit, type, arg));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_voq_is_in_dram_get(
    int unit,
    bcm_gport_t voq_gport,
    bcm_cos_queue_t cosq,
    int *is_in_dram)
{
    int core, queue_id;
    dbal_enum_value_field_dram_bound_state_e dram_bound_status;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get(unit, voq_gport, cosq, FALSE, &core, &queue_id));

    if (core == BCM_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Info cannot be retrieved for BCM_CORE_ALL. VOQ gport core id has to be specific!");
    }

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_dram_bound_state_get(unit, core, queue_id, &dram_bound_status));

    /**
     *  ALL values different than DBAL_ENUM_FVAL_DRAM_BOUND_STATE_S2F
     *  means that the VOQ is in DRAM
     */
    if (dram_bound_status == DBAL_ENUM_FVAL_DRAM_BOUND_STATE_S2F)
    {
        *is_in_dram = FALSE;
    }
    else
    {
        *is_in_dram = TRUE;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - translate congestion notification resource type from dbal to API
 */
static int
dnx_cosq_inst_congestion_resource_dbal_to_bcm_translate(
    int unit,
    dbal_enum_value_field_congestion_notification_resource_type_e dbal_type,
    bcm_cosq_resource_t * resource)
{

    SHR_FUNC_INIT_VARS(unit);

    switch (dbal_type)
    {
        case DBAL_ENUM_FVAL_CONGESTION_NOTIFICATION_RESOURCE_TYPE_TOTAL_WORDS:
            *resource = bcmResourceBytes;
            break;
        case DBAL_ENUM_FVAL_CONGESTION_NOTIFICATION_RESOURCE_TYPE_SRAM_BUFFERS:
            *resource = bcmResourceOcbBuffers;
            break;
        case DBAL_ENUM_FVAL_CONGESTION_NOTIFICATION_RESOURCE_TYPE_SRAM_PDS:
            *resource = bcmResourceOcbPacketDescriptors;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected resource type %d\n", dbal_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
dnx_cosq_inst_congestion_info_get(
    int unit,
    uint32 flags,
    bcm_cosq_inst_congestion_info_key_t * key,
    bcm_cosq_inst_congestion_info_data_t * data)
{
    dbal_enum_value_field_congestion_notification_resource_type_e resource_type;
    uint32 is_valid;

    SHR_FUNC_INIT_VARS(unit);

    /** init */
    sal_memset(data, 0, sizeof(bcm_cosq_inst_congestion_info_data_t));

    /** get next congestion element info (id, resources) according to provided element type */
    switch (key->type)
    {
        case bcmCosqInstCongestionInfoTypePgVsq:
        {
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_notification_fifo_pb_vsq_pg_hw_get
                            (unit, key->core_id, &is_valid, &resource_type, &data->id));
            data->valid = is_valid;
            break;
        }
        case bcmCosqInstCongestionInfoTypeLlfcVsq:
        {
            uint32 pool_id;
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_notification_fifo_pb_vsq_llfc_hw_get
                            (unit, key->core_id, &is_valid, &resource_type, &data->id, &pool_id));
            data->valid = is_valid;
            data->pool_id = pool_id;
            break;
        }
        case bcmCosqInstCongestionInfoTypeVoq:
        {
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_notification_fifo_voq_hw_get
                            (unit, key->core_id, &is_valid, &resource_type, &data->id));
            data->valid = is_valid;
            break;
        }
        default:
        {
            /** TBD support other congestion elements */
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected element type %d\n", key->type);
        }
    }

    /** exit in case no congestion element was found */
    if (data->valid == FALSE)
    {
        SHR_EXIT();
    }

    /** translate dbal resource type to bcm resource type */
    SHR_IF_ERR_EXIT(dnx_cosq_inst_congestion_resource_dbal_to_bcm_translate(unit, resource_type, &(data->resource)));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_voq_gl_ecn_resource_to_dbal_info(
        int unit,
        bcm_cosq_resource_t cosq_resource,
        dbal_tables_e * table,
        dbal_fields_e * thresh_field)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (cosq_resource)
    {
        case bcmResourceOcbBuffers:
            *table = DBAL_TABLE_INGRESS_CONG_GLOBAL_ECN_MARKING_THRESHOLDS_TABLE;
            *thresh_field = DBAL_FIELD_ECN_MARKING_TOTAL_FREE_SRAM_BUFFERS_TH;
            break;
        case bcmResourceOcbPacketDescriptorsBuffers:
            *table = DBAL_TABLE_INGRESS_CONG_GLOBAL_ECN_MARKING_THRESHOLDS_TABLE;
            *thresh_field = DBAL_FIELD_ECN_MARKING_TOTAL_FREE_SRAM_PDBS_TH;
            break;
        case bcmResourceDramBundleDescriptorsBuffers:
            *table = DBAL_TABLE_INGRESS_CONG_GLOBAL_ECN_MARKING_THRESHOLDS_TABLE;
            *thresh_field = DBAL_FIELD_ECN_MARKING_TOTAL_FREE_DRAM_BDBS_TH;
            break;
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong resource=%d.\n", cosq_resource);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to verify the arguments of dnx_cosq_voq_gl_ecn_marking_threshold_set/get.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] index_info - additional information derived from the index
 *   \param [in] thresh_info - complementary information describing the threshold to be set
 *   \param [in,out] threshold - threshold value
 *   \param [in] is_set -indicates weather the function was called from the set or get API
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_ingress_congestion_voq_gl_ecn_marking_threshold_arg_verify(
        int unit,
        int core_id,
        dnx_cosq_threshold_index_type_t index_type,
        dnx_cosq_threshold_index_info_t * index_info,
        bcm_cosq_static_threshold_info_t * thresh_info,
        bcm_cosq_static_threshold_t * threshold,
        uint8 is_set)
{
    int threshold_set;
    dbal_tables_e table;
    dbal_fields_e thresh_field;
    bcm_cosq_static_threshold_t threshold_internal;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(index_info, _SHR_E_PARAM, "index_info")
    SHR_NULL_CHECK(thresh_info, _SHR_E_PARAM, "thresh_info")
    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold")

    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE /** allow_all */ );

    if (thresh_info->is_free_resource != TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "This threshold is configured for free resources, is_free_resource must be set to 1");
    }

    if (thresh_info->index != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                             "This index is not used,index must be set to 0");
    }

    if (is_set){
        if (threshold->thresh_set != threshold->thresh_clear)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "thresh_set and tresh_clear must have equal values");
        }
    }

    switch (thresh_info->resource_type)
    {

        case bcmResourceOcbBuffers:
        case bcmResourceOcbPacketDescriptorsBuffers:
        case bcmResourceDramBundleDescriptorsBuffers:
            if (is_set)
            {
                SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_encode(unit, threshold->thresh_set,
                                                                             (uint32 *) &threshold_internal.thresh_set));
                /** Get the appropriate DBAL table and fields */
                SHR_IF_ERR_EXIT(dnx_cosq_voq_gl_ecn_resource_to_dbal_info(unit, thresh_info->resource_type, &table, &thresh_field));

                /** Check threshold value */
                SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                                (unit, table, thresh_field, FALSE, 0, 0, &threshold_set));
                if (threshold_internal.thresh_set > threshold_set)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Threshold value to set ECN Marking(%d) is bigger than the maximum allowed(%d).\n",
                                 threshold_internal.thresh_set, threshold_set);
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected resource type %d", thresh_info->resource_type);
            break;
    }
    if(thresh_info->threshold_action != bcmCosqThreshActionMarkCongestion)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "thresh_action is wrong %u\n",  thresh_info->threshold_action);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_ingress_congestion_global_ecn_marking_threshold_set(
        int unit,
        int core_id,
        dnx_cosq_threshold_index_type_t index_type,
        dnx_cosq_threshold_index_info_t * index_info,
        bcm_cosq_static_threshold_info_t * thresh_info,
        bcm_cosq_static_threshold_t * threshold)
{

    SHR_FUNC_INIT_VARS(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_voq_gl_ecn_marking_threshold_arg_verify(unit, core_id, index_type, index_info, thresh_info, threshold, TRUE));

    /** set hw*/
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_ecn_marking_threshold_hw_set(unit, core_id, thresh_info->resource_type, threshold->thresh_set));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_ingress_congestion_global_ecn_marking_threshold_get(
        int unit,
        int core_id,
        dnx_cosq_threshold_index_type_t index_type,
        dnx_cosq_threshold_index_info_t * index_info,
        bcm_cosq_static_threshold_info_t * thresh_info,
        bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNX(dnx_ingress_congestion_voq_gl_ecn_marking_threshold_arg_verify(unit, core_id, index_type, index_info, thresh_info, threshold, FALSE));

    /** set hw*/
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_ecn_marking_threshold_hw_get(unit, core_id, thresh_info->resource_type,
                                                                            (uint32 *) &threshold->thresh_set, (uint32 *) &threshold->thresh_clear));

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
