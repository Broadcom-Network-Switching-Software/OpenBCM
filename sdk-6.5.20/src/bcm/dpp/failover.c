/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_FAILOVER
#include <shared/bsl.h>
#include "bcm_int/common/debug.h"

#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/switch.h>

#include <bcm_int/control.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/l2.h>
#include <bcm/failover.h>
#include <bcm/multicast.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dpp/multicast.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/failover.h>

#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/JER/jer_drv.h>

#include <shared/swstate/access/sw_state_access.h>

typedef struct _bcm_dpp_failover_bookkeeping_s {
    int         initialized;        /* Set to TRUE when Failover module initialized */
} _bcm_dpp_failover_bookkeeping_t;

_bcm_dpp_failover_bookkeeping_t  _bcm_dpp_failover_bk_info[BCM_MAX_NUM_UNITS] = { {0} };

/* SW state for Failover */
#define _DPP_FAILOVER_INFO(_unit_)   (&_bcm_dpp_failover_bk_info[_unit_])

#define FAILOVER_ACCESS              sw_state_access[unit].dpp.bcm.failover

/*
 * Function:
 *      bcm_dpp_failover_check_init
 * Purpose:
 *      Check if failover is initialized
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

STATIC int _bcm_dpp_failover_check_init(
    int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (!_bcm_dpp_failover_bk_info[unit].initialized) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("not initialized")));
    } else {
        BCM_EXIT;
    }
exit:
        BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_failover_flags_to_type
 * Purpose:
 *      Converts failover creation BCM API flags to an internal Failover Type
 *      that is used for encoding the Failover ID.
 * Parameters:
 *      unit     - Device Number
 *      flags   - The failover flags that were used by the BCM API function
 *      failover_type - The returned failover type value that matches the selected
 *                failover flags.
 * Returns:
 *      BCM_E_PARAM - In case multiple failover ID flags are designated
 */
int _bcm_dpp_failover_flags_to_type(
    int unit,
    uint32 flags,
    int32 *failover_type)
{
    uint32 nof_failover_type_flags;
    BCMDNX_INIT_FUNC_DEFS;

    /* Initiate the returned value in case no failover type flag is supplied */
    *failover_type = DPP_FAILOVER_TYPE_NONE;
    nof_failover_type_flags = 0;

    /* Set the returned failover type according to the relevant flags*/
    if (flags & BCM_FAILOVER_FEC) {
        *failover_type = DPP_FAILOVER_TYPE_FEC;
        nof_failover_type_flags++;
    }

    if (flags & BCM_FAILOVER_INGRESS) {
        *failover_type = DPP_FAILOVER_TYPE_INGRESS;
        nof_failover_type_flags++;
    }

    if (flags & BCM_FAILOVER_ENCAP) {
        *failover_type = DPP_FAILOVER_TYPE_ENCAP;
        nof_failover_type_flags++;
    }

    if (flags & BCM_FAILOVER_L2_LOOKUP) {
        *failover_type = DPP_FAILOVER_TYPE_L2_LOOKUP;
        nof_failover_type_flags++;
    }

    /* Return an error if more than one failover type flag was designated */
    if (nof_failover_type_flags > 1) {
        *failover_type = DPP_FAILOVER_TYPE_NONE;
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Multiple failover ID flags aren't allowed")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_get_nof_failover_ids_by_type
 * Purpose:
 *      Retrieve the number of failover IDs of a failover table per failover type
 * Parameters:
 *      unit     - Device Number
 *      failover_type - The failover type of the failover table.
 *      nof_failover_ids - The retrieved number of failover IDs for the table.
 * Returns:
 *      BCM_E_INTERNAL - Unsupported failover type
 */
int _bcm_dpp_get_nof_failover_ids_by_type(
    int unit,
    int32 failover_type,
    uint32 *nof_failover_ids)
{
    BCMDNX_INIT_FUNC_DEFS;

    switch (failover_type) {
    case DPP_FAILOVER_TYPE_L2_LOOKUP:
    case DPP_FAILOVER_TYPE_FEC:
        *nof_failover_ids = SOC_DPP_DEFS_GET(unit, nof_failover_fec_ids);
        break;
    case DPP_FAILOVER_TYPE_INGRESS:
        *nof_failover_ids = SOC_DPP_DEFS_GET(unit, nof_failover_ingress_ids);

        /* Coupled mode enables only half the valid range as 1 bit from the
           path pointer is used in the HW for the failover path value. */
        if (SOC_IS_JERICHO(unit) && (SOC_DPP_IS_PROTECTION_INGRESS_COUPLED(unit))) {
            *nof_failover_ids >>= 1;
        }
        break;
    case DPP_FAILOVER_TYPE_ENCAP:
        *nof_failover_ids = SOC_DPP_DEFS_GET(unit, nof_failover_egress_ids);

        /* Coupled mode enables only half the valid range as 1 bit from the
           path pointer is used in the HW for the failover path value. */
        if (SOC_IS_JERICHO(unit) && (SOC_DPP_IS_PROTECTION_EGRESS_COUPLED(unit))) {
            *nof_failover_ids >>= 1;
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unsupported failover type")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_failover_is_valid_type
 * Purpose:
 *      Validates a failover type
 * Parameters: 
 *      unit     - Device Number 
 *      failover_type - The validated failover type 
 * Returns:
 *      FALSE - The failover type is Invalid
 *      TRUE -  The failover type is Valid
 */
int _bcm_dpp_failover_is_valid_type(
    int unit,
    int32 failover_type)
{
    BCMDNX_INIT_FUNC_DEFS;

    switch (failover_type) {
    case DPP_FAILOVER_TYPE_ENCAP:
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            return FALSE;
        }
    case DPP_FAILOVER_TYPE_L2_LOOKUP:
    case DPP_FAILOVER_TYPE_FEC:
    case DPP_FAILOVER_TYPE_INGRESS:
        return TRUE;
    default:
        break;
    }

    return FALSE;

    BCMDNX_FUNC_RETURN;
}



/*
 * Function:
 *      _bcm_dpp_failover_is_valid_id
 * Purpose:
 *      Validates a failover ID by checking the value range that is appropriate
 *      for the encoded failover type
 * Parameters: 
 *      unit     - Device Number 
 *      failover_id - The validated failover ID
 *      failover_type_match - The required Failover type to compare with the
 *          failover type that is encoded in the failover ID
 * Returns:
 *      BCM_E_NONE - The failover ID is Valid
 *      BCM_E_XXX -  The failover ID is Invalid
 */
int _bcm_dpp_failover_is_valid_id(
    int unit,
    int32 failover_id,
    int32 failover_type_match)
{
    uint32 nof_failover_ids, failover_id_val;
    int32 failover_type;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    /* Get the encoded failover type from the supplied failover ID */
    DPP_FAILOVER_TYPE_GET(failover_type, failover_id);

    /* Verify that the encoded failover type matches the required failover type */
    if (failover_type != failover_type_match) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
            (_BSL_BCM_MSG("The failover ID has incompatible failover type %d, expected %d"), failover_type, failover_type_match));
    }

    /* Get the upper range for the failover ID accrding to the failover type */
    rv = _bcm_dpp_get_nof_failover_ids_by_type(unit, failover_type, &nof_failover_ids);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Verify that the supplied failover ID is within the upper range */
    DPP_FAILOVER_ID_GET(failover_id_val, failover_id);
    if (failover_id_val > nof_failover_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The failover ID is out of range - %d"), failover_id_val));
    }

    return BCM_E_NONE;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_failover_state_hw_set
 * Purpose:
 *      Set the failover state to the HW by calling the relevant SOC API
 *      function according to the failover type
 * Parameters: 
 *      unit     - Device Number 
 *      failover_id - The failover ID with no encoded failover type
 *      failover_type - The type of failover to set
 *      failover_status - The new failover state
 * Returns:
 *      BCM_E_XXX - HW access failure
 */
int _bcm_dpp_failover_state_hw_set(
    int unit,
    int32 failover_id,
    int32 failover_type,
    uint8 failover_status)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    soc_error_t soc_rv;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* Up till Arad+, a common HW table is used */
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        soc_sand_rv = soc_ppd_frwrd_fec_protection_oam_instance_status_set(soc_sand_dev_id, failover_id, failover_status);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    } else {
        /* Set the HW state according to the failover type */
        switch (failover_type) {
        case DPP_FAILOVER_TYPE_FEC:
            soc_sand_rv = soc_ppd_frwrd_fec_protection_oam_instance_status_set(soc_sand_dev_id, failover_id, failover_status);
            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
            break;
        case DPP_FAILOVER_TYPE_INGRESS:
            soc_rv = (MBCM_PP_DRIVER_CALL(unit, mbcm_pp_ingress_protection_state_set,(unit, failover_id, failover_status)));
            BCMDNX_IF_ERR_EXIT(soc_rv);
            break;
        case DPP_FAILOVER_TYPE_ENCAP:
            soc_rv = (MBCM_PP_DRIVER_CALL(unit, mbcm_pp_egress_protection_state_set,(unit, failover_id, failover_status)));
            BCMDNX_IF_ERR_EXIT(soc_rv);
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failover type %d not supported"), failover_type));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_failover_state_hw_get
 * Purpose:
 *      Retrieve the failover state from the HW by calling the relevant SOC API
 *      function according to the failover type
 * Parameters: 
 *      unit     - Device Number 
 *      failover_id - The failover ID with no encoded failover type
 *      failover_type - The type of failover to set
 *      failover_status - The return failover state
 * Returns:
 *      BCM_E_XXX - HW access failure
 */
int _bcm_dpp_failover_state_hw_get(
    int unit,
    int32 failover_id,
    int32 failover_type,
    uint8 *failover_status)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int soc_rv;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* Up till Arad+, a common HW table is used */
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        soc_sand_rv = soc_ppd_frwrd_fec_protection_oam_instance_status_get(soc_sand_dev_id, failover_id, failover_status);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    } else {
        /* Set the HW state according to the failover type */
        switch (failover_type) {
        case DPP_FAILOVER_TYPE_FEC:
            soc_sand_rv = soc_ppd_frwrd_fec_protection_oam_instance_status_get(soc_sand_dev_id, failover_id, failover_status);
            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
            break;
        case DPP_FAILOVER_TYPE_INGRESS:
            soc_rv = (MBCM_PP_DRIVER_CALL(unit, mbcm_pp_ingress_protection_state_get,(unit, failover_id, failover_status)));
            BCMDNX_IF_ERR_EXIT(soc_rv);
            break;
        case DPP_FAILOVER_TYPE_ENCAP:
            soc_rv = (MBCM_PP_DRIVER_CALL(unit, mbcm_pp_egress_protection_state_get,(unit, failover_id, failover_status)));
            BCMDNX_IF_ERR_EXIT(soc_rv);
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failover type %d not supported"), failover_type));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_failover_create_verify
 * Purpose:
 *      Validate the parameters that are supplied to bcm_petra_failover_create()
 * Parameters: 
 *      unit    - Device Number
 *      flags   - The flags field that is supplied to bcm_petra_failover_create()
 *      failover_id - The failover ID that is supplied to bcm_petra_failover_create()
 *      failover_type - The failover type that was decoded from the failover_id
 * Returns:
 *      BCM_E_XXX - Parameter validation failure
 */
int _bcm_dpp_failover_create_verify(
    int unit,
    uint32 flags,
    bcm_failover_t *failover_id,
    int32 failover_type)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    /* Verify that the failover module is initiated */
    rv = _bcm_dpp_failover_check_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Verify that the failover ID contains a valid failover type */
    if (failover_type == DPP_FAILOVER_TYPE_NONE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("A failover type flag must be selected")));
    }

    /* Verify that the supplied flags are consistent */
    if( (flags & BCM_FAILOVER_REPLACE) && !(flags & BCM_FAILOVER_WITH_ID)) {

        /* Replace without ID*/
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Supply REPLACE flag with WITH_ID flag")));
    }

    /* Egress Protection isn't supported prior to the Jericho device */
    if (!(SOC_IS_JERICHO(unit)))
    {
        if ((failover_type == DPP_FAILOVER_TYPE_ENCAP) || (flags & BCM_FAILOVER_PAIRED)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Egress failover ID allocation isn't supported prior to the Jericho device")));
        }
    }

    /* Verify that a request for an allocation of a pair of failover IDs is performed only
       in the context of Egress failover and only when in egress decoupled mode. */
    if (flags & BCM_FAILOVER_PAIRED) {
        if (failover_type != DPP_FAILOVER_TYPE_ENCAP) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Allocating a pair of failover IDs is available only for Egress failover type")));
        }

        if (SOC_DPP_IS_PROTECTION_EGRESS_COUPLED(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Allocating a pair of failover IDs is available only when in Egress Decoupled mode")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_failover_reset
 * Purpose:
 *      Resets all failover SW object allocations.
 *      The failover type L2_LOOKUP isn't supported as the failover ID in this
 *      case represents a FEC entry that shouldn't be reset in other cases.
 * Parameters:
 *      unit    - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_petra_failover_reset(
    int    unit)
{
    int rv;
    int failover_id, failover_type,
        failover_types[] = { DPP_FAILOVER_TYPE_FEC, DPP_FAILOVER_TYPE_INGRESS, DPP_FAILOVER_TYPE_ENCAP };
    uint32 nof_failover_tables, nof_failover_ids, failover_type_idx;
    BCMDNX_INIT_FUNC_DEFS;

    /* Set the Number of Failover tables to reset according to the device.
       Up till Arad+, one common failover table is used. */
    nof_failover_tables = ((SOC_IS_ARADPLUS_AND_BELOW(unit)) ? 1 : (sizeof(failover_types) / sizeof(failover_types[0])));

    /* Traverse the failover types that require reset */
    for (failover_type_idx = 0; failover_type_idx < nof_failover_tables; ++failover_type_idx ) {

        /* Get the failover ID range for the type */
        failover_type = failover_types[failover_type_idx];
        rv = _bcm_dpp_get_nof_failover_ids_by_type(unit, failover_type, &nof_failover_ids);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Traverse all the failover IDs for failover type */
        for (failover_id = 1; failover_id < nof_failover_ids; ++failover_id) {

            /* Check whether the failover is allocated before de-allocation */
            rv = bcm_dpp_am_failover_is_alloced(unit, failover_type, failover_id);
            if (rv != BCM_E_EXISTS) {
                continue;
            }
            rv = bcm_dpp_am_failover_dealloc(unit, failover_type, 1, failover_id);
            if (rv != BCM_E_NOT_FOUND) {
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

    /* FIX: replace by one call for alloc-mngr to do it faster */

exit:
    BCMDNX_FUNC_RETURN;
}


/* initialize all SW/HW resource used by this module
 *  if already initialized then "reset" status.
 *  if not initialized allocate all need SW resources
 */
int
bcm_petra_failover_init(
    int    unit)
{
    int rv;
    uint8 is_allocated;
    _bcm_dpp_failover_bookkeeping_t *failover_info = _DPP_FAILOVER_INFO(unit);

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (!SOC_WARM_BOOT(unit)) {

        /* For Jericho, Allocate a SW DB for Egress Protection */
        if (SOC_IS_JERICHO(unit)) {
            /* set up initial values for VLAN state */
            BCMDNX_IF_ERR_EXIT(FAILOVER_ACCESS.is_allocated(unit, &is_allocated));
            if (!is_allocated) {
                BCMDNX_IF_ERR_EXIT(FAILOVER_ACCESS.alloc(unit));
            }
        }

        /*
         * This shouldn't be done in WB as the reset deletes alloc_mngr 
         * allocations that were already restored as part of the WB init process. 
         */
        if (failover_info->initialized) {
            rv = bcm_petra_failover_reset(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Set the HW status for ingress drop failover-id */
        rv = _bcm_dpp_failover_state_hw_set(unit, _BCM_INGRESS_PROTECTION_POINTER_INVALID, DPP_FAILOVER_TYPE_INGRESS, 0);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* FIX: allocate alloc-mngr if not initialized*/

    failover_info->initialized = 1;

exit:
    BCMDNX_FUNC_RETURN;
}


/* free all SW/HW resource used by this module */
int
bcm_petra_failover_cleanup(
    int    unit)
{
    int rv;
    _bcm_dpp_failover_bookkeeping_t *failover_info = _DPP_FAILOVER_INFO(unit);

    BCMDNX_INIT_FUNC_DEFS;
    /* if not initialized yet, nothing to do */
    if (FALSE == failover_info->initialized) {
        BCM_EXIT;
    }

    /* do the clean */

    /* HW: 
     * no need to update HW, as resource is not in use 
     * when allocated it will be reset 
     */

    /* 
     * SW: Destroy sw resources used for Failover-id
     */
    rv = bcm_petra_failover_reset(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    failover_info->initialized = 0;

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_failover_create
 * Purpose:
 *      Creates a failover object.
 *      The object is allocated in the SW using the alloc manager mechanism before
 *      the HW entry can be used.
 *      In general the failover object is a Protection Pointer, while for Ring
 *      Protection (L2_LOOKUP) the object is a FEC entry.
 * Parameters:
 *      unit    - Device Number
 *      flags   - The failover flags that were used by the BCM API function
 *      failover_id - The returned failover ID, encoded with the failover type.
 *                In case of a REPLACE operation, the specified failover ID is
 *                used, although the type doesn't have to be encoded in it.
 * Returns:
 *      BCM_E_PARAM - In case multiple failover ID flags are designated
 */
int
bcm_petra_failover_create(
    int             unit,
    uint32          flags,
    bcm_failover_t *failover_id)
{
    int rv = BCM_E_NONE;
    int alloc_flags = 0;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_FEC_ID fec_id = 0;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO    hw_fec_entry_info;
    SOC_PPC_FRWRD_FEC_PROTECT_INFO  protect_info;
    uint8                           fec_success;
    int32 failover_type, nof_alloc_ids = 1, failover_id_idx;
    bcm_failover_t tmp_failover_id;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    soc_sand_dev_id = (unit);
   
    rv = _bcm_dpp_failover_check_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Convert the Failover creation flags to an internal failover type for encoding in the failover ID */
    rv = _bcm_dpp_failover_flags_to_type(unit, flags, &failover_type);
    BCMDNX_IF_ERR_EXIT(rv);
    if (failover_type == DPP_FAILOVER_TYPE_NONE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("A failover type flag must be selected")));
    }

    /* Validate the parameters */
    rv = _bcm_dpp_failover_create_verify(unit, flags, failover_id, failover_type);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Set the alloc manager flags */
    tmp_failover_id = 0;
    if (flags & BCM_FAILOVER_WITH_ID) {
        if ((failover_type == DPP_FAILOVER_TYPE_INGRESS) && (*failover_id == _BCM_INGRESS_PROTECTION_POINTER_INVALID)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Failover ID")));
        }
        alloc_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
        DPP_FAILOVER_TYPE_RESET(tmp_failover_id, *failover_id);
    }

    /* L2 Lookup mode (Arad only) is a Ring Protection mode that requires a
       a special failover group. This group doesn't require any failover
       typical resources, but allocates a FEC object instead */
    if ((failover_type == DPP_FAILOVER_TYPE_L2_LOOKUP)) {
        fec_id = tmp_failover_id;
        if (!(flags & BCM_FAILOVER_REPLACE)) {
            /* Allocate a SW DB entry for the allocated FEC */
            rv = bcm_dpp_am_fec_alloc(unit, alloc_flags, 0, 1, &fec_id);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Set a default HW FEC Entry. The FEC fields will be set again
               upon vlan-port associations to the failover group */
            SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(&hw_fec_entry_info);
            hw_fec_entry_info.type = SOC_PPC_FEC_TYPE_SIMPLE_DEST;
            SOC_PPC_FRWRD_FEC_PROTECT_INFO_clear(&protect_info);
            soc_sand_rv = soc_ppd_frwrd_fec_entry_add(soc_sand_dev_id,
                                                      fec_id,
                                                      SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE,
                                                      &hw_fec_entry_info,    /* Working */
                                                      NULL, /* Protection */
                                                      &protect_info,
                                                      &fec_success);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        tmp_failover_id = fec_id;
    } else {

        /* If the allocation is for a pair of Egress failover IDs */
        if (flags & BCM_FAILOVER_PAIRED) {
            nof_alloc_ids = 2;
        }

        /* For all non Ring-Protection failover types, a failover object should be allocated */
        rv = bcm_dpp_am_failover_alloc(unit, alloc_flags, failover_type, nof_alloc_ids, &tmp_failover_id);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Set the SW state for Egress failover to the number of subsequent failover ID allocations */
        if (failover_type == DPP_FAILOVER_TYPE_ENCAP) {
            rv = FAILOVER_ACCESS.id_sequence.set(unit, tmp_failover_id, (uint8)nof_alloc_ids);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Clean up the HW status of the allocated failover-id */
        for (failover_id_idx = 0; failover_id_idx < nof_alloc_ids; failover_id_idx++) {
            rv = _bcm_dpp_failover_state_hw_set(unit, tmp_failover_id + failover_id_idx, failover_type, 1);
            BCM_SAND_IF_ERR_EXIT(rv);
        }
    }

    /* Encode the failover type into the failover ID */
    DPP_FAILOVER_TYPE_SET(*failover_id, tmp_failover_id, failover_type);
    
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_failover_destroy
 * Purpose:
 *      Destroys a failover object.
 *      The object is deallocated in the SW using the alloc manager mechanism.
 *      In general the failover object is a Protection Pointer, while for Ring
 *      Protection (L2_LOOKUP) the object is a FEC entry.
 * Parameters:
 *      unit    - Device Number
 *      failover_id - The failover ID that should be destroyed.
 *                The ID should include an encoded failover type in order to
 *                address the relevant object.
 * Returns:
 *      BCM_E_PARAM - In case multiple failover ID flags are designated
 */
int
bcm_petra_failover_destroy(
    int             unit,
    bcm_failover_t  failover_id)
{
    SOC_PPC_FEC_ID fec_id;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv, failover_id_val;
    unsigned int soc_sand_dev_id;
    int32 failover_type, nof_alloc_ids = 1, reset_value = 0;
    uint8 uint8_nof_alloc_ids;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    soc_sand_dev_id = (unit);

    rv = _bcm_dpp_failover_check_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Get the encoded failover type value from the supplied failover ID */
    DPP_FAILOVER_TYPE_GET(failover_type, failover_id);

    /* Validate the failover type */
    if (_bcm_dpp_failover_is_valid_type(unit, failover_type) == FALSE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Failover type in Failover ID.")));
    }

    /* Get the encoded failover ID value from the supplied failover ID */
    DPP_FAILOVER_ID_GET(failover_id_val, failover_id);

    if ((failover_type == DPP_FAILOVER_TYPE_INGRESS) && (failover_id_val == _BCM_INGRESS_PROTECTION_POINTER_INVALID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Failover ID")));
    }

    /* Check whether the failover id is of type L2 Lookup (Arad only) */
    if ((DPP_FAILOVER_IS_L2_LOOKUP(failover_id))) {
        /* Free the HW Entry */
        DPP_FAILOVER_TYPE_RESET(fec_id, failover_id);
        soc_sand_rv = soc_ppd_frwrd_fec_remove(soc_sand_dev_id,
                                               fec_id);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* De-allocate the SW DB FEC entry */
        rv = bcm_dpp_am_fec_dealloc(unit, 0 /*flags*/, 1 /*size*/, fec_id);
        BCMDNX_IF_ERR_EXIT(rv); 
        BCM_EXIT;
    }

    /* De-allocate the failover ID */
    /* If Egress failover, get the number of subsequent failover ID allocations from the SW DB, before resetting the value */
    if (failover_type == DPP_FAILOVER_TYPE_ENCAP) {
        rv = FAILOVER_ACCESS.id_sequence.get(unit, failover_id_val, &uint8_nof_alloc_ids);
        BCMDNX_IF_ERR_EXIT(rv);

        nof_alloc_ids = uint8_nof_alloc_ids;
        uint8_nof_alloc_ids = reset_value;
        rv = FAILOVER_ACCESS.id_sequence.set(unit, failover_id_val, uint8_nof_alloc_ids);
        BCMDNX_IF_ERR_EXIT(rv);
    }


    /* Perform the de-allocation */
    rv = bcm_dpp_am_failover_dealloc(unit, failover_type, nof_alloc_ids, failover_id_val);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_failover_get
 * Purpose:
 *      Retrieve a failover object state.
 *      The state is retrieved from the appropriate HW failover table according
 *      to the failover type that is encoded in the failover ID.
 *      The failover type L2_LOOKUP isn't supported as the failover ID in this
 *      case represents a FEC entry that has no state.
 * Parameters:
 *      unit    - Device Number
 *      failover_id - The failover ID for which the state is retrieved.
 *                The ID should include an encoded failover type in order to
 *                address the relevant object.
 *      enable  - The retrieved failover state (0 / 1)
 * Returns:
 *      BCM_E_PARAM - Unsupported failover ID value
 */
int
bcm_petra_failover_get(
    int    unit,
    bcm_failover_t    failover_id,
    int *    enable)
{
    int rv = BCM_E_NONE;
    uint8 failover_status;
    int32 failover_type;
    uint32 failover_id_val;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_failover_check_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Get the encoded failover type from the supplied failover ID */
    DPP_FAILOVER_TYPE_GET(failover_type, failover_id);

    /* Validate the failover type */
    if (_bcm_dpp_failover_is_valid_type(unit, failover_type) == FALSE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Failover type in Failover ID.")));
    }

    /* Verify that the failover is allocated in a failover table */
    DPP_FAILOVER_ID_GET(failover_id_val, failover_id);

    if ((failover_type == DPP_FAILOVER_TYPE_INGRESS) && (failover_id_val == _BCM_INGRESS_PROTECTION_POINTER_INVALID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Failover ID")));
    }

    rv = bcm_dpp_am_failover_is_alloced(unit, failover_type, failover_id_val);
    if (rv != BCM_E_EXISTS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Failover ID not allocated")));
    }

    /* Get the failover state from the HW */
    rv = _bcm_dpp_failover_state_hw_get(unit, failover_id_val, failover_type, &failover_status);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Store the retrieved state*/
    *enable = failover_status;

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_failover_set
 * Purpose:
 *      Set a failover object state.
 *      The state is set to the appropriate HW failover table according
 *      to the failover type that is encoded in the failover ID.
 *      The failover type L2_LOOKUP isn't supported as the failover ID in this
 *      case represents a FEC entry that has no state.
 * Parameters:
 *      unit    - Device Number
 *      failover_id - The failover ID to which the state is set.
 *                The ID should include an encoded failover type in order to
 *                address the relevant object.
 *      enable  - The failover state vlaue (TRUE / FALSE)
 * Returns:
 *      BCM_E_PARAM - Unsupported failover ID or enable state
 */
int
bcm_petra_failover_set(
    int    unit,
    bcm_failover_t    failover_id,
    int    enable)
{
    int rv = BCM_E_NONE;
    int32 failover_type;
    uint32 failover_id_val;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    rv = _bcm_dpp_failover_check_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Validate the failover state to set */
    if ((enable < 0) || (enable > 1)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("enable has to be 0 or 1")));
    }

    /* Get the encoded failover type from the supplied failover ID */
    DPP_FAILOVER_TYPE_GET(failover_type, failover_id);

    /* Validate the failover type */
    if (_bcm_dpp_failover_is_valid_type(unit, failover_type) == FALSE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Failover type in Failover ID.")));
    }

    /* Verify that the failover is allocated in a failover table */
    DPP_FAILOVER_ID_GET(failover_id_val, failover_id);

    if ((failover_type == DPP_FAILOVER_TYPE_INGRESS) && (failover_id_val == _BCM_INGRESS_PROTECTION_POINTER_INVALID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Failover ID")));
    }

    rv = bcm_dpp_am_failover_is_alloced(unit, failover_type, failover_id_val);
    if (rv != BCM_E_EXISTS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Failover ID not allocated")));
    }

    /* Set the failover state to the HW */
    rv = _bcm_dpp_failover_state_hw_set(unit, failover_id_val, failover_type, enable);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

#if defined(INCLUDE_L3)
/* 
 * Set a failover object to enable or disable (note that failover object
 * 0 is reserved.
 */
int 
bcm_petra_failover_status_set(int unit, bcm_failover_element_t *failover, 
                              int enable)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported use bcm_petra_failover_set")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Get a failover object to enable or disable (note that failover object
 * 0 is reserved.
 */
int
bcm_petra_failover_status_get(int unit, bcm_failover_element_t *failover, 
                              int *enable)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported use bcm_petra_failover_get")));
exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* INCLUDE_L3 */

