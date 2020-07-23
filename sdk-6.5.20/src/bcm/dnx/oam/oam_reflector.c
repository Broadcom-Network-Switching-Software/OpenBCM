/** \file oam_reflector.c
 * 
 *
 * OAM Reflector procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 /*
  * Include files.
  * {
  */
#include <bcm/oam.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include "oam_internal.h"
#include "oam_counter.h"
#include <soc/dnx/swstate/auto_generated/access/oam_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
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

/**
 * \brief
 *  Verification of bcm_dnx_oam_reflector_encap_create() input parameters.
 *  For details about the parameters refer to bcm_dnx_oam_reflector_encap_create()
 *  side.
*/
static shr_error_e
dnx_oam_reflector_encap_create_verify(
    int unit,
    uint32 flags,
    bcm_if_t * encap_id)
{
    uint32 supported_flags;
    uint32 hw_resources_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t gport;
    int is_allocated, global_lif_id;
    SHR_FUNC_INIT_VARS(unit);

    /** flags */
    supported_flags = BCM_OAM_REFELCTOR_ENCAP_WITH_ID;
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "some of the flags are not supported \n");

    /** encap_id */
    SHR_NULL_CHECK(encap_id, _SHR_E_PARAM, "encap_id");
    if (flags & BCM_OAM_REFELCTOR_ENCAP_WITH_ID)
    {
        /*
         * range check
         */
        global_lif_id = BCM_L3_ITF_VAL_GET(*encap_id);
        LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif_id);

         /** free check */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, *encap_id);
        hw_resources_flags =
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS |
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, hw_resources_flags, &gport_hw_resources));
        if (gport_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Encap ID 0x%x, was already allocated.\n", *encap_id);
        }
    }

    /** make sure that no other encap allocated for OAM reflector */
    SHR_IF_ERR_EXIT(oam_sw_db_info.reflector.is_allocated.get(unit, &is_allocated));
    if (is_allocated == 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "encap ID for oam reflector already allocated.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verification of bcm_dnx_oam_reflector_encap_destroy() input parameters.
 *  For details about the parameters refer to bcm_dnx_oam_reflector_encap_destroy()
 *  side.
*/
static shr_error_e
dnx_oam_reflector_encap_destroy_verify(
    int unit,
    uint32 flags,
    bcm_if_t encap_id)
{
    uint32 supported_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 hw_resources_flags;
    bcm_gport_t gport;
    int global_lif_id;
    SHR_FUNC_INIT_VARS(unit);

    /** flags */
    supported_flags = 0;
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "some of the flags are not supported \n");

    /** encap_id */
    /*
     * range check
     */
    global_lif_id = BCM_L3_ITF_VAL_GET(encap_id);
    LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif_id);
    /** verify outlif allocated and  type */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, encap_id);
    hw_resources_flags =
        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS |
        DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, hw_resources_flags, &gport_hw_resources));
    if (gport_hw_resources.outlif_dbal_table_id != DBAL_TABLE_EEDB_OAM_REFLECTOR)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Expecting OAM reflector encap ID, got 0x%x", encap_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Allocate OUTLIF used by unicast Ethernet loopback Down MEP.
 *  side.
 * \param [in] unit  - unit #.
 * \param [in] flags  - Set to BCM_OAM_REFELCTOR_ENCAP_WITH_ID to allocate a specific OUTLIF or 0 otherwise
 * \param [in,out] encap_id - IN: Specify the required OUTLIF in a case flag BCM_OAM_REFELCTOR_ENCAP_WITH_ID set.
 *                            OUT: the allocated OUTTLIF
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
int
bcm_dnx_oam_reflector_encap_create(
    int unit,
    uint32 flags,
    bcm_if_t * encap_id)
{
    lif_table_mngr_outlif_info_t outlif_info;
    uint32 entry_handle_id;
    uint32 lif_alloc_flags = 0;
    int local_outlif, global_lif_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_reflector_encap_create_verify(unit, flags, encap_id));

    sal_memset(&outlif_info, 0, sizeof(lif_table_mngr_outlif_info_t));

    /** Allocate global OUTLIF */
    if (flags & BCM_OAM_REFELCTOR_ENCAP_WITH_ID)
    {
        lif_alloc_flags = LIF_MNGR_GLOBAL_LIF_WITH_ID;
        global_lif_id = BCM_L3_ITF_VAL_GET(*encap_id);
    }
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                    (unit, lif_alloc_flags, DNX_ALGO_LIF_EGRESS, &global_lif_id));
    outlif_info.global_lif = global_lif_id;

    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_FIRST;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_OAM_REFLECTOR, &entry_handle_id));

    /** Set superset result type */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    /** Set DATA fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, bcmOamOpcodeLBR);

    /** Call lif table manager to allocate lif and set hardware */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
                    (unit, entry_handle_id, &local_outlif, &outlif_info));

    BCM_L3_ITF_SET(*encap_id, BCM_L3_ITF_TYPE_LIF, global_lif_id);
    /** store info in sw state */
    SHR_IF_ERR_EXIT(oam_sw_db_info.reflector.encap_id.set(unit, *encap_id));
    SHR_IF_ERR_EXIT(oam_sw_db_info.reflector.is_allocated.set(unit, 1));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  De-allocate OUTLIF used by UC Ethernet downmep reflector.
 *  side.
 * \param [in] unit  - unit #.
 * \param [in] flags  - Set to 0
 * \param [in] encap_id - Specify the OUTLIF allocated by bcm_dnx_oam_reflector_encap_create().
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
int
bcm_dnx_oam_reflector_encap_destroy(
    int unit,
    uint32 flags,
    bcm_if_t encap_id)
{
    uint32 local_lif;
    uint32 hw_resources_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t gport;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_reflector_encap_destroy_verify(unit, flags, encap_id));

    /** get global lin and local lif*/
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, encap_id);
    hw_resources_flags =
        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, hw_resources_flags, &gport_hw_resources));
    local_lif = gport_hw_resources.local_out_lif;

    /** Clear oam reflector table and free local LIF allocation */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_outlif_info_clear(unit, local_lif, 0));

    /** Delete global lif */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_EGRESS, gport_hw_resources.global_out_lif));

    /** mark as not allocated */
    SHR_IF_ERR_EXIT(oam_sw_db_info.reflector.encap_id.set(unit, -1));
    SHR_IF_ERR_EXIT(oam_sw_db_info.reflector.is_allocated.set(unit, 0));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
