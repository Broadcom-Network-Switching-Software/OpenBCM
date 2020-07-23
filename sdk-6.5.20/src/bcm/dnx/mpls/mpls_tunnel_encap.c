/** \file mpls_tunnel_encap.c MPLS functionality for DNX tunnel encapsulation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLS

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
#include <bcm/mpls.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm/types.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_mpls_access.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_encap.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/algo/qos/algo_qos.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <bcm_int/dnx/port/port_esem.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/switch/switch_svtag.h>
#include <bcm_int/dnx/mpls_l2vpn/mpls_l2vpn.h>
#include <bcm_int/dnx/port/port_match.h>
#include "mpls_evpn.h"

/*
 * }
 */
/*
 * MACROs
 * {
 */

/*
 * List of all the supported flags that are used by the flag 2 field
 */
#define MPLS_TUNNEL_ENCAP_SUPPORTED_FLAGS_2 BCM_MPLS_EGRESS_LABEL_FLAGS2_SVTAG_ENABLE
/*
 * }
 */
/*
 * APIs
 * {
 */

/**
 * \brief
 * Map MPLS EEDB access stage to lif_mngr app type.
 */
static shr_error_e
dnx_mpls_tunnel_initiator_encap_access_to_outlif_phase(
    int unit,
    uint32 encap_access,
    lif_mngr_outlif_phase_e * outlif_phase)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(outlif_phase, _SHR_E_PARAM, "outlif_phase");

    switch (encap_access)
    {
        case bcmEncapAccessTunnel1:
        {
            *outlif_phase = LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_1;
            break;
        }
        case bcmEncapAccessTunnel2:
        case bcmEncapAccessInvalid:
        {
            *outlif_phase = LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_2;
            break;
        }
        case bcmEncapAccessTunnel3:
        {
            *outlif_phase = LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_3;
            break;
        }
        case bcmEncapAccessTunnel4:
        {
            *outlif_phase = LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_4;
            break;
        }
        case bcmEncapAccessNativeArp:
        {
            *outlif_phase = LIF_MNGR_OUTLIF_PHASE_NATIVE_ARP;
            break;
        }
        case bcmEncapAccessArp:
        {
            *outlif_phase = LIF_MNGR_OUTLIF_PHASE_ARP;
            break;
        }
        case bcmEncapAccessRif:
        {
            *outlif_phase = LIF_MNGR_OUTLIF_PHASE_RIF;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "MPLS tunnel encapsulation encap_access value (%d) is not supported",
                         encap_access);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Map lif_mngr app type to MPLS EEDB access stage.
 */
static shr_error_e
dnx_mpls_tunnel_initiator_outlif_phase_to_encap_access(
    int unit,
    uint32 outlif_phase,
    bcm_encap_access_t * encap_access)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(encap_access, _SHR_E_PARAM, "encap_access");

    switch (outlif_phase)
    {
        case LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_1:
        {
            *encap_access = bcmEncapAccessTunnel1;
            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_2:
        {
            /** bcmEncapAccessInvalid is selected for bcmEncapAccessTunnel2 case as well */
            *encap_access = bcmEncapAccessInvalid;
            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_3:
        {
            *encap_access = bcmEncapAccessTunnel3;
            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_4:
        {
            *encap_access = bcmEncapAccessTunnel4;
            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_NATIVE_ARP:
        {
            *encap_access = bcmEncapAccessNativeArp;
            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_ARP:
        {
            *encap_access = bcmEncapAccessArp;
            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_RIF:
        {
            *encap_access = bcmEncapAccessRif;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "lif_mngr app type (%d) is not supported MPLS tunnel encapsulation",
                         outlif_phase);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Allocate global and local lif for mpls tunnel encap.
* \param [in] unit         - Relevant unit.
* \param [in] label_array  - Array containing 'num_labels' labels to be pushed.
* \param [out] global_lif_id_p -
*   Pointer to int. Handling of this variable depends on 'label_array[0].flags'
*   and on 'label_array[0].tunnel_id' as follows:
*     If neither BCM_MPLS_EGRESS_LABEL_REPLACE nor BCM_MPLS_EGRESS_LABEL_WITH_ID is set
*       If BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED is set
*         This procedure loads pointed memory by DNX_ALGO_GPM_LIF_INVALID.
*       else
*         This procedure loads pointed memory by global lif id that was allocated. If
*         allocation has failed, _SHR_E_NOT_FOUND is returned and pointed memory is
*         loaded by DNX_ALGO_GPM_LIF_INVALID
*     else
*       If 'label_array[0].tunnel_id' is NOT encoded as 'LIF' then encode it as such. Use sub_type value
*       as base.
*       If 'label_array[0].tunnel_id' is encoded as 'egr_pointed' (See BCM_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED)
*       or BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED is set
*         then This procedure loads pointed memory by DNX_ALGO_GPM_LIF_INVALID.
*       else This procedure loads pointed memory by global lif id that is either derived from
*         'label_array[0].tunnel_id' (for 'replace' case) or as allocated. For 'with id',
*         value to allocate is taken from 'label_array[0].tunnel_id'
* \param [out] egr_pointed_id_p -
*   Pointer to dnx_egr_pointed_t. handling of this variable depends on 'label_array[0].flags'
*   and on 'label_array[0].tunnel_id' as follows:
*     If neither BCM_MPLS_EGRESS_LABEL_REPLACE nor BCM_MPLS_EGRESS_LABEL_WITH_ID is set
*       If BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED is set
*         This procedure loads pointed memory by id of newly allocated 'virtual egress pointed'
*         object. If no such object could be allocated, an _SHR_E_NOT_FOUND is returned (but
*         no error log is ejected) plus pointed memory is loaded by DNX_ALGO_EGR_POINTED_INVALID.
*       else
*         This procedure loads pointed memory by DNX_ALGO_EGR_POINTED_INVALID.
*     else
*       If 'label_array[0].tunnel_id' is NOT encoded as 'LIF' then encode it as such. Use sub_type value
*       as base.
*       If 'label_array[0].tunnel_id' is neither encoded as 'egr_pointed' (See BCM_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED)
*       nor is BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED set
*         then This procedure loads pointed memory by DNX_ALGO_EGR_POINTED_INVALID.
*       else This procedure loads pointed memory by 'egr_pointed' id that is derived from
*         'label_array[0].tunnel_id' (for 'replace' case) or as allocated. For 'with id',
*         value to allocate is taken from 'label_array[0].tunnel_id'
* \return
*   shr_error_e - Error Type
* \remark
*   * None.
* \see
*   * None
*/
static shr_error_e
dnx_mpls_tunnel_encap_lif_allocate(
    int unit,
    bcm_mpls_egress_label_t * label_array,
    int *global_lif_id_p,
    dnx_egr_pointed_t * egr_pointed_id_p)
{
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t gport;
    int this_is_egr_pointed_case;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    this_is_egr_pointed_case = FALSE;
    if (_SHR_IS_FLAG_SET(label_array[0].flags, (BCM_MPLS_EGRESS_LABEL_REPLACE | BCM_MPLS_EGRESS_LABEL_WITH_ID)))
    {
        /*
         * In 'replace' and 'with_id'  cases, input (label_array[0].tunnel_id) must be
         * encoded as 'interface'
         * If it is not, make it.
         */
        if (!(BCM_L3_ITF_TYPE_IS_LIF(label_array[0].tunnel_id)))
        {
            int l3_itf_dummy;

            l3_itf_dummy = BCM_L3_ITF_SUB_TYPE_VALUE_GET(label_array[0].tunnel_id);
            BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, l3_itf_dummy);
        }
        if (BCM_L3_ITF_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(label_array[0].tunnel_id) ||
            (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED)))
        {
            /*
             * If 'tunnel_id' is encoded as 'egr_pointed', then update 'this_is_egr_pointed_case'
             */
            this_is_egr_pointed_case = TRUE;
        }
    }
    else
    {
        if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED))
        {
            /*
             * If 'egr_pointed' flag is set, update 'this_is_egr_pointed_case'
             */
            this_is_egr_pointed_case = TRUE;
        }
    }

    if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_REPLACE))
    {
        /*
         * Here handle the 'replace' case. Encoded interface is assumed to have been loaded
         * into 'label_array[0].tunnel_id'
         */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, label_array[0].tunnel_id);
        /*
         * At this point, 'gport' has 'type' of 'tunnel'
         */
        if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(gport))
        {
            /*
             * For EGR_POINTED encoded input (label_array[0].tunnel_id), get 'local lif' and
             * 'egr_pointed' ids.
             */
            *global_lif_id_p = DNX_ALGO_GPM_LIF_INVALID;
            *egr_pointed_id_p = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(gport);
        }
        else
        {
            /*
             * For 'all other cases' encoded input (label_array[0].tunnel_id), get 'local lif' and
             * 'global lif' ids.
             */
            lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));
            *global_lif_id_p = gport_hw_resources.global_out_lif;
            *egr_pointed_id_p = DNX_ALGO_EGR_POINTED_INVALID;
        }
    }
    else
    {
        lif_flags = 0;

        /*
         * Jericho mode beginning
         * {
         */
        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        {
            if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ACTION_VALID)
                && (label_array[0].action == BCM_MPLS_EGRESS_ACTION_PHP))
            {
                lif_flags = LIF_MNGR_PHP_GPORT_GLOBAL_LIF;
            }
        }
        /*
         * }
         * Jericho mode end
         */
        if (!(this_is_egr_pointed_case == TRUE))
        {
            /*
             * If 'egr_pointed' is NOT the case here then handle 'global lif'
             */
            *egr_pointed_id_p = DNX_ALGO_EGR_POINTED_INVALID;
            if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_WITH_ID))
            {
                *global_lif_id_p = BCM_L3_ITF_VAL_GET(label_array[0].tunnel_id);
                lif_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
            }

            SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                            (unit, lif_flags, DNX_ALGO_LIF_EGRESS, global_lif_id_p));
        }
        else
        {
            /*
             * if (this_is_egr_pointed_case == TRUE)
             * If 'egr_pointed' IS the case here then set 'global lif' as 'invalid' and
             * handle 'egr_pointed'.
             */
            uint32 alloc_flags;
            int element;

            if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_WITH_ID))
            {
                bcm_if_t intf;

                /*
                 * Since caller is not required to encode 'label_array[0].tunnel_id' in the 'with_id' case
                 * then we do it here, to make sure, before we use the standard macro to get the 'egr_pointed' value.
                 * Note that we could simply apply 'and' with BCM_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_MASK
                 */
                BCM_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(intf, label_array[0].tunnel_id);
                *egr_pointed_id_p = BCM_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(intf);
                /*
                 * Allocate resource corresponding to id specified on input.
                 */
                element = *egr_pointed_id_p;
                alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
                SHR_IF_ERR_EXIT(algo_l3_db.egr_pointed_alloc.
                                egr_pointed_res_manager.allocate_single(unit, alloc_flags, NULL, &element));
            }
            else
            {
                /*
                 * Allocate a new 'egr_pointed' object.
                 */
                alloc_flags = 0;
                SHR_IF_ERR_EXIT(algo_l3_db.egr_pointed_alloc.
                                egr_pointed_res_manager.allocate_single(unit, alloc_flags, NULL, &element));
                *egr_pointed_id_p = element;
            }
            /*
             * Just making sure. dnx_lif_lib_allocate() is supposed to have done that, anyway.
             */
            *global_lif_id_p = DNX_ALGO_GPM_LIF_INVALID;
        }
    }
    /*
     * Just making sure: At this point, one of 'egr_pointed' and 'global_lif' (and only one!)
     * should be valid.
     */
    if ((*egr_pointed_id_p == DNX_ALGO_EGR_POINTED_INVALID) && (*global_lif_id_p == DNX_ALGO_GPM_LIF_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "\r\n"
                     "Both 'egr_pointed' and 'global_lif' are marked as 'invalid' - Probably internal sw error.\r\n");
    }
    else if ((*egr_pointed_id_p != DNX_ALGO_EGR_POINTED_INVALID) && (*global_lif_id_p != DNX_ALGO_GPM_LIF_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "\r\n"
                     "Both 'egr_pointed' and 'global_lif' are marked as 'valid' - Probably internal sw error.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Used by dnx_mpls_tunnel_initiator_create_php() to verify user input.
* \param [in] unit           - Relevant unit.
* \param [in] label_array    - Array containing the php label
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_mpls_tunnel_initiator_create_php_verify(
    int unit,
    bcm_mpls_egress_label_t * label_array)
{
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /*
     * Verification
     */
    if (label_array[0].label != BCM_MPLS_LABEL_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PHP action does not support labels(%d) - provided labels must be %d",
                     label_array[0].label, BCM_MPLS_LABEL_INVALID);
    }

    if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_STAT_ENABLE) &&
        (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PHP does not support statistic under JR mode ");
    }

    if (DNX_QOS_MAP_PROFILE_GET(label_array[0].qos_map_id) >= dnx_data_qos.qos.nof_egress_mpls_php_profiles_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PHP qos map id (0x%x) is out of range ", label_array[0].qos_map_id);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
*  Used by bcm_dnx_mpls_tunnel_initiator_create() to verify user input in case of alternate marking.
* \par DIRECT INPUT:
*   \param [in] unit         -  Relevant unit.
*   \param [in] intf         -  Legacy variable - not used, must be set to 0.
*   \param [in] num_labels   - Number of labels to be used.
*   \param [in] label_array  -  Array containing 'num_labels' labels to be pushed.
* \par INDIRECT INPUT:
*   * \b *label_array \n
*     See 'label_array' in DIRECT INPUT above
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_mpls_tunnel_initiator_create_alternate_marking_verify(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
    uint32 special_label_value;

    SHR_FUNC_INIT_VARS(unit);

    /** Both labels should be set */
    if (num_labels != DNX_MPLS_TUNNEL_ENCAP_TWO_LABELS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "for alternate marking, set num_labels to 2");
    }
    /** Both labels should have ALTERNATE_MARKING flag */
    if (!(_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING))
        || !(_SHR_IS_FLAG_SET(label_array[1].flags, BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "for alternate marking, both labels should have flag BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING set");
    }
    /** LOSS can be set only on flow id (first label) */
    if ((_SHR_IS_FLAG_SET(label_array[1].flags, BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING_LOSS_SET)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "for alternate marking, LOSS can be set only on flow id label 0");
    }
    /** Alternate marking entry cannot be tandem */
    if ((_SHR_IS_FLAG_SET(label_array[1].flags, BCM_MPLS_EGRESS_LABEL_TANDEM)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "for alternate marking, BCM_MPLS_EGRESS_LABEL_TANDEM cannot be set");
    }
    /** TC cannot be set on flow id (first label) */
    if (label_array[0].exp != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot set exp on Flow Id label 0. exp: %d", label_array[0].exp);
    }
    /** Qos model must be pipe */
    if ((label_array[0].egress_qos_model.egress_qos != bcmQosEgressModelPipeMyNameSpace))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Egress QOS model on Special Label must be bcmQosEgressModelPipeMyNameSpace");
    }
    /** Protection cannot be set */
    if (label_array[0].egress_failover_id != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "alternate marking protection is not supported");
    }
    /** Special labels cannot be set */
    if ((_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_CONTROL_WORD)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "for alternate marking, BCM_MPLS_EGRESS_LABEL_CONTROL_WORD cannot be set");
    }
    if ((_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE))
        || (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "for alternate marking, BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE cannot be set");
    }
    /** Special Label cannot be set (taken from global configuration */
    if (label_array[1].label != BCM_MPLS_LABEL_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "for alternate marking, label number 1 is taken from global configuration and cannot be configured by this api");
    }
    SHR_IF_ERR_EXIT(dnx_mpls_alternate_marking_special_label_get(unit, &special_label_value));
    if (special_label_value == DNX_MPLS_ALTERNATE_MARKING_SL_DEFAULT)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "to configure alternate marking encapsulation, first a special label should be set using bcm_switch_control ");
    }

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
*  Used by bcm_dnx_mpls_tunnel_initiator_create() to verify user input.
* \par DIRECT INPUT:
*   \param [in] unit         -  Relevant unit.
*   \param [in] intf         -  Legacy variable - not used, must be set to 0.
*   \param [in] num_labels   - Number of labels to be used.
*   \param [in] label_array  -  Array containing 'num_labels' labels to be pushed.
* \par INDIRECT INPUT:
*   * \b *label_array \n
*     See 'label_array' in DIRECT INPUT above
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_mpls_tunnel_initiator_create_verify(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
    uint32 not_supported_flags;
    bcm_mpls_egress_label_t existing_label_entry[DNX_MPLS_PUSH_MAX_NUM_LABELS];
    int label_count;
    int ii;
    lif_mngr_outlif_phase_e outlif_phase;
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t gport;
    uint32 entry_handle_id;
    int mpls_special_label_encap_handling;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(label_array, _SHR_E_PARAM, "label_array");
    mpls_special_label_encap_handling = dnx_data_mpls.general.mpls_special_label_encap_handling_get(unit);

    not_supported_flags =
        (BCM_MPLS_EGRESS_LABEL_EXP_REMARK | BCM_MPLS_EGRESS_LABEL_PRI_SET | BCM_MPLS_EGRESS_LABEL_PRI_REMARK |
         BCM_MPLS_EGRESS_LABEL_DROP | BCM_MPLS_EGRESS_LABEL_PHP_IPV4 | BCM_MPLS_EGRESS_LABEL_PHP_IPV6 |
         BCM_MPLS_EGRESS_LABEL_PHP_L2 |
         BCM_MPLS_EGRESS_LABEL_WIDE | BCM_MPLS_EGRESS_LABEL_PRESERVE |
         BCM_MPLS_EGRESS_LABEL_EXP_COPY | BCM_MPLS_EGRESS_LABEL_EXP_SET |
         BCM_MPLS_EGRESS_LABEL_TTL_COPY | BCM_MPLS_EGRESS_LABEL_TTL_SET |
         BCM_MPLS_EGRESS_LABEL_ECN_TO_EXP_MAP | BCM_MPLS_EGRESS_LABEL_INT_CN_TO_EXP_MAP |
         BCM_MPLS_EGRESS_LABEL_ECN_EXP_MAP_TRUST);

    if (intf != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "intf parameter must be set to 0");
    }

    /** check supported number of labels */
    if ((num_labels < DNX_MPLS_PUSH_MIN_NUM_LABELS) || (num_labels > DNX_MPLS_PUSH_MAX_NUM_LABELS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "num_labels(%d) is out of range. The range should be %d - %d", num_labels,
                     DNX_MPLS_PUSH_MIN_NUM_LABELS, DNX_MPLS_PUSH_MAX_NUM_LABELS);
    }

    if ((label_array[0].l3_intf_id != 0) && (label_array[0].l3_intf_id != BCM_IF_INVALID))
    {
        bcm_gport_t next_pointer = 0;
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        if (!BCM_L3_ITF_TYPE_IS_LIF(label_array[0].l3_intf_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "next outlif pointer RIF is no longer supported, must be LIF type, l3_intf_id=0x%x",
                         label_array[0].l3_intf_id);
        }

        /*
         * Make sure the next Out LIF exist
         */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_pointer, label_array[0].l3_intf_id);
        if ((dnx_algo_gpm_gport_to_hw_resources
             (unit, next_pointer, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "next outlif pointer does not exist, l3_intf_id=0x%x",
                         label_array[0].l3_intf_id);
        }
    }

    /** remark profile*/
    if (label_array[0].qos_map_id != DNX_QOS_INITIAL_MAP_ID && !DNX_QOS_MAP_IS_REMARK(label_array[0].qos_map_id) &&
        !DNX_QOS_MAP_IS_MPLS_PHP(label_array[0].qos_map_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "qos profile(%d) is not remark profile", label_array[0].qos_map_id);
    }

    /** encap stage */
    if ((label_array[0].encap_access < bcmEncapAccessInvalid) || (label_array[0].encap_access > bcmEncapAccessRif))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "label_array[0].encap_access (%d) is invalid, supported values are %d - %d",
                     label_array[0].encap_access, bcmEncapAccessInvalid, bcmEncapAccessRif);
    }

    /** TTL DECREMENT must be used */
    if (!(_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flag BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT must be used");
    }

    /*
     * In case of using BCM_MPLS_EGRESS_LABEL_PROTECTION,
     * validate egress protection pointer
     */
    if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_PROTECTION))
    {
        SHR_IF_ERR_EXIT(dnx_failover_id_valid_verify
                        (unit, DNX_FAILOVER_TYPE_ENCAP, 0, label_array[0].egress_failover_id));
    }

    /*
     * Currently the SVTAG flag is supported for a single MPLS result type only.
     * The following verification prevent the SVTAG use from the other MPLS result type.
     */
    if (_SHR_IS_FLAG_SET(label_array[0].flags2, BCM_MPLS_EGRESS_LABEL_FLAGS2_SVTAG_ENABLE))
    {

        /*
         * Verify that the device supports SVTAG use.
         */
        if (!dnx_data_switch.svtag.supported_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "This device doesn't support SVTAG (remove the BCM_MPLS_EGRESS_LABEL_FLAGS2_SVTAG_ENABLE flag).");
        }

        /*
         * Make sure that the SVTAG is used only with a single MPLS label
         */
        if (num_labels > 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The BCM_MPLS_EGRESS_LABEL_FLAGS2_SVTAG_ENABLE can only be set with a single MPLS label.");
        }

        /*
         * Verify that the SVTAG isn't used with TANDEM
         */
        if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_TANDEM))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The BCM_MPLS_EGRESS_LABEL_FLAGS2_SVTAG_ENABLE and the BCM_MPLS_EGRESS_LABEL_TANDEM flags are mutually exclusive.");
        }

        /*
         * Verify that the SVTAG isn't used with PHP
         */
        if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ACTION_VALID)
            && (label_array[0].action == BCM_MPLS_EGRESS_ACTION_PHP))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The BCM_MPLS_EGRESS_LABEL_FLAGS2_SVTAG_ENABLE isn't supported with a PHP action.");
        }
    }

    for (ii = 0; ii < num_labels; ii++)
    {
        /** basic check for supported flags */
        if (_SHR_IS_FLAG_SET(label_array[ii].flags, not_supported_flags))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "one of the used flags on label %d is not supported - flags = 0x%08X", ii,
                         label_array[ii].flags);
        }

        /** basic check for supported flags 2 */
        if (_SHR_IS_FLAG_SET(label_array[ii].flags2, ~MPLS_TUNNEL_ENCAP_SUPPORTED_FLAGS_2))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "one of the used flags 2 on label %d is not supported - flags = 0x%08X", ii,
                         label_array[ii].flags2);
        }

        /** check supported actions */
        if ((label_array[ii].action < BCM_MPLS_EGRESS_ACTION_SWAP)
            || (label_array[ii].action > BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "action on label %d not valid = %d", ii, label_array[ii].action);
        }
        else if (_SHR_IS_FLAG_SET(label_array[ii].flags, BCM_MPLS_EGRESS_LABEL_ACTION_VALID)
                 && (label_array[ii].action != BCM_MPLS_EGRESS_ACTION_PHP))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_MPLS_EGRESS_LABEL_ACTION_VALID is set on label %d action(%d) must be PHP",
                         ii, label_array[ii].action);
        }
        else if (!_SHR_IS_FLAG_SET(label_array[ii].flags, BCM_MPLS_EGRESS_LABEL_ACTION_VALID)
                 && label_array[ii].action != BCM_MPLS_EGRESS_ACTION_SWAP)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_MPLS_EGRESS_LABEL_ACTION_VALID is not set on label %d action(%d) must be 0",
                         ii, label_array[ii].action);
        }

        /** check supported labels */
        if ((label_array[ii].action != BCM_MPLS_EGRESS_ACTION_PHP)
            && (!_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING))
            && !DNX_MPLS_LABEL_IN_RANGE(unit, label_array[ii].label))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "MPLS label (%d) is out of range(%d). The range should be 0 - %d",
                         ii, label_array[ii].label, DNX_MPLS_LABEL_MAX(unit));
        }

        if (ii > 0)
        {
            /*
             * Make sure that the 'no GLEM' flag is not set on any label except the
             * first (if at all). For 'no GLEM', the first label represents all.
             */
            if (label_array[ii].flags & BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The flag BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED (0x%08X) may not be set on label_array[%d]. Flags = 0x%08X",
                             BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED, ii, label_array[ii].flags);
            }
            if (label_array[0].l3_intf_id != label_array[ii].l3_intf_id)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "all l3_intf_id values must be equal, label[0].l3_intf_id (%d) != label[%d].l3_intf_id (%d)",
                             label_array[0].l3_intf_id, ii, label_array[ii].l3_intf_id);
            }
            if (label_array[0].action != label_array[ii].action)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "actions are not aligned: label_array[0].action (%d) != label_array[%d].action (%d)",
                             label_array[0].action, ii, label_array[ii].action);
            }

            if (label_array[0].encap_access != label_array[ii].encap_access)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "encap_access are not aligned: label_array[0].encap_access (%d) != label_array[%d].encap_access (%d)",
                             label_array[0].encap_access, ii, label_array[ii].encap_access);
            }
        }
    }

    /** check if WITH_ID set flags */
    if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_WITH_ID))
    {
        if ((num_labels == DNX_MPLS_TUNNEL_ENCAP_TWO_LABELS))
        {
            if (!_SHR_IS_FLAG_SET(label_array[1].flags, BCM_MPLS_EGRESS_LABEL_WITH_ID))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Label 1 is set WITH_ID - label 2 must be set WITH_ID as well");
            }
            else if ((BCM_L3_ITF_VAL_GET(label_array[0].tunnel_id) == 0)
                     || (BCM_L3_ITF_VAL_GET(label_array[1].tunnel_id) == 0))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnels set WITH_ID but tunnel_id_1(%d) or tunnel_id_2(%d) is set to 0",
                             label_array[0].tunnel_id, label_array[1].tunnel_id);
            }
            else if (BCM_L3_ITF_VAL_GET(label_array[0].tunnel_id) != BCM_L3_ITF_VAL_GET(label_array[1].tunnel_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnels set WITH_ID but tunnel_id_1(%d) != tunnel_id_2(%d)",
                             label_array[0].tunnel_id, label_array[1].tunnel_id);
            }
        }
        else if (BCM_L3_ITF_VAL_GET(label_array[0].tunnel_id) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel is set WITH_ID but tunnel_id is set to 0");
        }
    }

    if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_REPLACE))
    {
        dbal_tables_e expected_table;
        if (!_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_WITH_ID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_MPLS_EGRESS_LABEL_REPLACE flag must be set with BCM_MPLS_EGRESS_LABEL_WITH_ID.");
        }

        /*
         * This validation uses sw state stored information that is stored separately
         * for EVPN/IML, because of the use of MPLS-L2VPN module.
         */
        if (DNX_MPLS_TUNNEL_INIT_IS_EVPN(label_array))
        {
            /*
             * EVPN uses MPLS_PORT gport and VPLS outlif phases.
             */
            BCM_GPORT_SUB_TYPE_LIF_SET(gport, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, label_array[0].tunnel_id);
            BCM_GPORT_MPLS_PORT_ID_SET(gport, gport);
            expected_table = DBAL_TABLE_EEDB_EVPN;
            SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_encap_access_to_outlif_phase
                            (unit, label_array[0].encap_access, &outlif_phase));
        }
        else
        {
            /*
             * MPLS tunnel uses TUNNEL gport and MPLS outlif phases.
             */
            BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, label_array[0].tunnel_id);
            expected_table = DBAL_TABLE_EEDB_MPLS_TUNNEL;
            /** Set outlif eedb phase. */
            SHR_IF_ERR_EXIT(dnx_mpls_tunnel_initiator_encap_access_to_outlif_phase
                            (unit, label_array[0].encap_access, &outlif_phase));
        }
        if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(gport) &&
            !_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "EGRESS_POINTED gport can be only replaced with flag BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED");
        }
        if (!BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(gport) &&
            _SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "non EGRESS_POINTED gport can be only replaced with flag BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED unset");
        }

        if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(gport))
        {
            /*
             * For EGR_POINTED encoded input (intf), get 'local lif' and
             * 'egr_pointed' ids.
             */
            lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        }
        else
        {
            /*
             * For 'all other cases' encoded input (intf), get 'local lif' and
             * 'global lif' ids.
             */
            lif_flags =
                DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
        }
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));

        if (gport_hw_resources.outlif_dbal_table_id != expected_table)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "provided interface 0x%08X matches %d instead of %d",
                         label_array[0].tunnel_id, gport_hw_resources.outlif_dbal_table_id, expected_table);
        }

        if (gport_hw_resources.outlif_phase != outlif_phase)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "replacing entry with encap_access(%d) with new value(%d) - not supported, encap_access must not change in REPLACE",
                         gport_hw_resources.outlif_phase, outlif_phase);
        }

        if (DNX_MPLS_TUNNEL_INIT_IS_EVPN(label_array))
        {
            /** Verify no switching between EVPN-UC and IML */
            lif_table_mngr_outlif_info_t outlif_info;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_EVPN, &entry_handle_id));
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                            (unit, gport_hw_resources.local_out_lif, entry_handle_id, &outlif_info));
            if (_SHR_IS_FLAG_SET(outlif_info.table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_IML) &&
                _SHR_IS_FLAG_SET(label_array->flags, BCM_MPLS_EGRESS_LABEL_EVPN))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "IML entry cannot be changes into EVPN entry.\n");
            }
            else if (!_SHR_IS_FLAG_SET(outlif_info.table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_IML) &&
                     _SHR_IS_FLAG_SET(label_array->flags, BCM_MPLS_EGRESS_LABEL_IML))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "EVPN entry cannot be changed into IML entry.\n");
            }
        }

        /** Verify protection state doesn't change */
        bcm_mpls_egress_label_t_init(&existing_label_entry[0]);
        SHR_IF_ERR_EXIT(bcm_dnx_mpls_tunnel_initiator_get
                        (unit, label_array[0].tunnel_id, DNX_MPLS_TUNNEL_ENCAP_TWO_LABELS, existing_label_entry,
                         &label_count));
        if (label_array[0].flags & BCM_MPLS_EGRESS_LABEL_PROTECTION)
        {
            if (existing_label_entry[0].egress_failover_id == dnx_data_failover.path_select.egr_no_protection_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Tunnel ID given is already allocated as non protection and can't replace to protection");
            }
        }
        else
        {
            if (existing_label_entry[0].egress_failover_id != dnx_data_failover.path_select.egr_no_protection_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Tunnel ID given is already allocated as protection and can't replace to non protection");
            }
        }
    }

    if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_TANDEM))
    {
        if (mpls_special_label_encap_handling &&
            (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE) ||
             _SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "tandem entry does not support additional header");
        }
    }

    /**TTL*/
    if ((label_array[0].egress_qos_model.egress_ttl != bcmQosEgressModelPipeMyNameSpace) &&
        (label_array[0].egress_qos_model.egress_ttl != bcmQosEgressModelUniform))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "TTL model only support bcmQosEgressModelPipeMyNameSpace and bcmQosEgressModelUniform");
    }
    if (label_array[0].egress_qos_model.egress_ttl != bcmQosEgressModelPipeMyNameSpace)
    {
        if (label_array[0].ttl != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "TTL must be 0 if ttl model is not pipe");
        }
    }
    if (label_array[0].egress_qos_model.egress_ecn == bcmQosEgressEcnModelEligible)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mpls tunnel encap does not support ECN");
    }
    /** EXP */
    if ((label_array[0].egress_qos_model.egress_qos != bcmQosEgressModelPipeNextNameSpace) &&
        (label_array[0].egress_qos_model.egress_qos != bcmQosEgressModelPipeMyNameSpace))
    {
        if (label_array[0].exp != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "EXP must be 0 if QoS model is not bcmQosEgressModelPipeNextNameSpace or bcmQosEgressModelPipeMyNameSpace");
        }
    }
    else
    {
        if (label_array[0].exp > dnx_data_qos.qos.packet_max_exp_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "EXP is out of range %d", label_array[0].exp);
        }
    }

    if (DNX_MPLS_TUNNEL_INIT_IS_EVPN(label_array))
    {
        SHR_RANGE_VERIFY(num_labels, DNX_MPLS_TUNNEL_ENCAP_ONE_LABELS, DNX_MPLS_TUNNEL_ENCAP_ONE_LABELS, _SHR_E_PARAM,
                         "EVPN/IML label encapsulation with another MPLS label is not supported.");
        /*
         * EVPN/IML verification continues separately
         */
        SHR_EXIT();
    }

    if (!_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE) &&
        _SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE must be set with BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE");
    }

    /*
     * PHP only support pipeNextNameSpace
     */
    if ((label_array[0].action == BCM_MPLS_EGRESS_ACTION_PHP) &&
        (label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PHP does not support bcmQosEgressModelPipeMyNameSpace");
    }

    /*
     *  in case of two labels, validate TTL flags in both labels
     *  are identical
     */
    if (num_labels == DNX_MPLS_TUNNEL_ENCAP_TWO_LABELS)
    {
        /** Validate both labels are set/unset together the TTL/EXP*/
        if (label_array[0].egress_qos_model.egress_ttl != label_array[1].egress_qos_model.egress_ttl)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Both labels need to have same TTL configs");
        }

        if (label_array[0].egress_qos_model.egress_qos != label_array[1].egress_qos_model.egress_qos)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Both labels need to have same EXP configs");
        }

        if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_PROTECTION) ^
            _SHR_IS_FLAG_SET(label_array[1].flags, BCM_MPLS_EGRESS_LABEL_PROTECTION))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Both labels need to have same flag of BCM_MPLS_EGRESS_LABEL_PROTECTION");
        }

        /*
         * In case of both are pipe,
         * validate same TTL value
         */
        if (label_array[0].egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace)
        {
            if (label_array[0].ttl != label_array[1].ttl)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "two labels in mode TTL_SET, but different ttl_value."
                             "label_1 ttl=%d, label_2 ttl=%d", label_array[0].ttl, label_array[1].ttl);
            }
        }

        /*
         * In case of both are pipe,
         * validate same EXP value
         */
        if (label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace)
        {
            if (label_array[0].exp != label_array[1].exp)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "pipeNextNameSpace mode, EXP0 and EXP1 must be same"
                             " label_1 exp=%d, label_2 exp=%d", label_array[0].exp, label_array[1].exp);
            }
        }

        if (label_array[1].egress_qos_model.egress_ecn == bcmQosEgressEcnModelEligible)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "MPLS tunnel encap does not ecn_eligible!");
        }

        /*
         * qos map id for label 0 and label 1 must be same
         */
        if (label_array[0].qos_map_id != label_array[1].qos_map_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "qos profile0 (%d) is different with qos profile1 (%d) ",
                         label_array[0].qos_map_id, label_array[1].qos_map_id);
        }

        /*
         * In case of both use BCM_MPLS_EGRESS_LABEL_PROTECTION,
         * validate same protection values
         */
        if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_PROTECTION))
        {
            if ((label_array[0].egress_failover_id != label_array[1].egress_failover_id) ||
                (label_array[0].egress_failover_if_id != label_array[1].egress_failover_if_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "when BCM_MPLS_EGRESS_LABEL_PROTECTION is used, "
                             "protection parameters of both labels should be the same");
            }
        }
    }

    /*
     * Make sure BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED is allowed for
     * dnx_mpls_tunnel_initiator_create_push() ONLY.
     */
    if ((_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED)) &&
        (DNX_MPLS_TUNNEL_INIT_IS_EVPN(label_array) ||
         (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ACTION_VALID)
          && (label_array[0].action == BCM_MPLS_EGRESS_ACTION_PHP))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED not allowed for EVPN or PHP tunnels - flags = 0x%08X",
                     label_array[0].flags);
    }

    /*
     * RFC8321 Alternate Marking
     */
    if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING) ||
        ((num_labels == DNX_MPLS_TUNNEL_ENCAP_TWO_LABELS)
         && _SHR_IS_FLAG_SET(label_array[1].flags, BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING)))
    {
        SHR_IF_ERR_EXIT(dnx_mpls_tunnel_initiator_create_alternate_marking_verify(unit, intf, num_labels, label_array));
    }
    else if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING_LOSS_SET)
             || ((num_labels == DNX_MPLS_TUNNEL_ENCAP_TWO_LABELS)
                 && _SHR_IS_FLAG_SET(label_array[1].flags, BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING_LOSS_SET)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "to set loss bit in alternate marking using BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING_LOSS_SET, both labels should have flag BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING set");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Used by bcm_dnx_mpls_tunnel_initiator_get() to verify user input.
* \par DIRECT INPUT:
*   \param [in] unit         -  Relevant unit.
*   \param [in] intf         -  Interface.
*   \param [in] label_max   - Maximal number of labels to be retrieved.
*   \param [in] label_array  -  Array to contain retreived labels.
*   \param [in] label_count  -  Number of retrieved labels.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
*/
static shr_error_e
dnx_mpls_tunnel_initiator_get_verify(
    int unit,
    bcm_if_t intf,
    int label_max,
    bcm_mpls_egress_label_t * label_array,
    int *label_count)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(label_array, _SHR_E_PARAM, "label_array");
    SHR_NULL_CHECK(label_count, _SHR_E_PARAM, "label_count");

    if (!BCM_L3_ITF_TYPE_IS_LIF(intf))
    {   /* Must be OutLIF */
        SHR_ERR_EXIT(_SHR_E_PARAM, "intf(%x) parameter must be ENCAP type (OutLIF)", intf);
    }
    /*
     * check supported number of labels
     */
    if ((label_max < DNX_MPLS_PUSH_MIN_NUM_LABELS) || (label_max > DNX_MPLS_PUSH_MAX_NUM_LABELS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "label_max(%d) is out of range. The range should be %d - %d", label_max,
                     DNX_MPLS_PUSH_MIN_NUM_LABELS, DNX_MPLS_PUSH_MAX_NUM_LABELS);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Used by bcm_dnx_mpls_tunnel_initiator_clear() to verify user input.
*   \param [in] unit         -  Relevant unit.
*   \param [in] intf         -  Interface.
* \return
*   shr_error_e
*/
static shr_error_e
dnx_mpls_tunnel_initiator_clear_verify(
    int unit,
    bcm_if_t intf)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_L3_ITF_TYPE_IS_LIF(intf))
    {
        /*
         * Must be OutLIF
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "intf(0x%08X) parameter must be ENCAP type (OutLIF)", intf);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

void
dnx_mpls_encap_t_init(
    int unit,
    dnx_mpls_encap_t * encap_info)
{
    if (encap_info != NULL)
    {
        sal_memset(encap_info, 0, sizeof(dnx_mpls_encap_t));
        encap_info->protection_pointer = dnx_data_failover.path_select.egr_no_protection_get(unit);
        encap_info->esem_command_profile = dnx_data_esem.access_cmd.no_action_get(unit);
    }
}

/**
* \brief
*  Calculates the needed lif additional header profile to be used as part of the construction of additional headers in the egress flow.
*   \param [in] has_cw     -  whether cw is resent.
*   \param [in] flags         -  label flags, whether EL or ELI required.
* \return
*    dbal_enum_value_field_etpp_additional_header_profile_stack_attributes_e - profile to be used
*/
static dbal_enum_value_field_etpp_additional_header_profile_stack_attributes_e
dnx_mpls_encap_calc_lif_additional_header_profile(
    int has_cw,
    uint32 flags)
{
    if (has_cw)
    {
        if ((flags & BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE) && (flags & BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE))
        {
            return DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CWOELOELI;
        }
        else if (flags & BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE)
        {
            /** In PWE EL without ELI is flow label */
            return DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CWOFL;
        }
        else
        {
            return DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CW;
        }
    }
    else
    {
        if ((flags & BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE)
            && !(flags & BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE))
        {
            /** In PWE EL without ELI is flow label */
            return DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_FL;
        }
        else if ((flags & BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE)
                 && (flags & BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE))
        {
            return DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_ELOELI;
        }
    }

    return DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_NONE;
}

/**
* \brief
*  Set the relevant CW/EL/ELI label flags based on the lif additional header profile.
*   \param [in] additional_headers_profile     -  lif additional header profile from db.
*   \param [out] has_cw     -  set control word was used
*   \param [out] flags     -  flags defining 'additional_headers_profile'.
* \return
*   none
*/
void
dnx_mpls_encap_lif_additional_header_profile_to_label_flags(
    dbal_enum_value_field_etpp_additional_header_profile_stack_attributes_e additional_headers_profile,
    int *has_cw,
    uint32 *flags)
{
    switch (additional_headers_profile)
    {
        case DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CWOELOELI:
            *has_cw = 1;
            *flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE | BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE;
            break;
        case DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CWOFL:
            *has_cw = 1;
            *flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
            break;
        case DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CW:
            *has_cw = 1;
            break;
        case DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_FL:
            *flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
            break;
        case DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_ELOELI:
            *flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE | BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE;
            break;
        default:
            *has_cw = 0;
            *flags |= 0;
    }
}

/**
* See .h file
*/
int
dnx_mpls_nof_additional_headers(
    int has_cw,
    uint32 flags)
{
    int nof_additional_headers = 0;

    if (has_cw)
    {
        nof_additional_headers++;
    }
    if (flags & BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE)
    {
        nof_additional_headers++;
    }
    if (flags & BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE)
    {
        nof_additional_headers++;
    }

    return nof_additional_headers;
}

/*
 * See .h file
 */
shr_error_e
dnx_mpls_egress_lif_ttl_qos_profiles_sw_allocate(
    int unit,
    int update,
    bcm_switch_network_group_t network_group_id,
    dbal_tables_e dbal_table_id,
    dnx_mpls_encap_t encap_info_orig,
    dnx_mpls_encap_t * encap_info,
    uint8 *ttl_pipe_profile_first_reference,
    uint8 *ttl_pipe_profile_last_reference,
    uint8 *qos_pipe_profile_first_reference,
    uint8 *qos_pipe_profile_last_reference)
{
    int out_lif_profile_orig;
    uint32 alloc_flags = 0;
    uint16 nwk_qos_pipe, nwk_qos_pipe_mapped;
    int ttl_data;

    SHR_FUNC_INIT_VARS(unit);

    if (encap_info->labels_nof > 1)
    {
        uint32 exp0;

        /** For alternate Marking set TC to const value */
        if (_SHR_IS_FLAG_SET(encap_info->label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING))
        {
            exp0 =
                _SHR_IS_FLAG_SET(encap_info->label_array[0].flags,
                                 BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING_LOSS_SET) ? 0x4 : 0;
        }
        else
        {
            exp0 = encap_info->label_array[0].exp;
        }
        nwk_qos_pipe = DNX_QOS_IDX_EXP_TO_NWK_QOS(exp0, encap_info->label_array[1].exp);
        nwk_qos_pipe_mapped = encap_info->label_array[1].exp;
    }
    else
    {
        nwk_qos_pipe = DNX_QOS_IDX_EXP_TO_NWK_QOS(encap_info->label_array[0].exp, encap_info->label_array[0].exp);
        nwk_qos_pipe_mapped = encap_info->label_array[0].exp;
    }

    /** in case of replace we perform exchange for the qos profiles instead of allocation */
    if (update)
    {
        uint8 new_is_pipe;
        uint8 old_is_pipe;

        new_is_pipe = ((encap_info->label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace) ||
                       (encap_info->label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace)) ?
            TRUE : FALSE;
        old_is_pipe =
            ((encap_info_orig.label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace)
             || (encap_info_orig.label_array[0].egress_qos_model.egress_qos ==
                 bcmQosEgressModelPipeMyNameSpace)) ? TRUE : FALSE;
        if (new_is_pipe && old_is_pipe)
        {
            /** update nwk_qos_id, in case of qos model is pipe for original entry as well */
            int network_qos_pipe_profile;

            network_qos_pipe_profile = encap_info_orig.nwk_qos_id;

            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_update(unit, nwk_qos_pipe,
                                                                           nwk_qos_pipe_mapped, 0,
                                                                           &network_qos_pipe_profile,
                                                                           qos_pipe_profile_first_reference,
                                                                           qos_pipe_profile_last_reference));
            encap_info->nwk_qos_id = network_qos_pipe_profile;
        }
        else if (new_is_pipe && !old_is_pipe)
        {
            /** Allocate nwk_qos_id, in case of qos model is not pipe for original entry */
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_allocate(unit, nwk_qos_pipe,
                                                                             nwk_qos_pipe_mapped, 0,
                                                                             &encap_info->nwk_qos_id,
                                                                             qos_pipe_profile_first_reference));
        }
        else if (!new_is_pipe && old_is_pipe)
        {
            /** free nwk_qos_id, in case of qos model is pipe for original entry but not pipe for current one */
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_free
                            (unit, encap_info_orig.nwk_qos_id, qos_pipe_profile_last_reference));
        }

        out_lif_profile_orig = encap_info_orig.out_lif_profile;

        if ((encap_info->label_array[0].egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace) &&
            (encap_info_orig.label_array[0].egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace))

        {
            /** update TTL PIPE profile */
            ttl_data = encap_info->label_array[0].ttl;
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.exchange
                            (unit, alloc_flags, &ttl_data,
                             encap_info_orig.ttl_pipe_profile, NULL, &encap_info->ttl_pipe_profile,
                             ttl_pipe_profile_first_reference, ttl_pipe_profile_last_reference));
        }
        else if (encap_info->label_array[0].egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace)
        {
            /** allocate TTL PIPE profile */
            ttl_data = encap_info->label_array[0].ttl;
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.allocate_single
                            (unit, alloc_flags, &ttl_data, NULL,
                             &encap_info->ttl_pipe_profile, ttl_pipe_profile_first_reference));
        }
        else if (encap_info_orig.label_array[0].egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace)
        {
            /** free TTL PIPE profile */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.free_single
                            (unit, encap_info_orig.ttl_pipe_profile, ttl_pipe_profile_last_reference));
        }

    }
    else
    {
        /** Allocate nwk_qos_id, in case of qos model is pipe  */
        if ((encap_info->label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace) ||
            (encap_info->label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace))
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_allocate(unit, nwk_qos_pipe,
                                                                             nwk_qos_pipe_mapped, 0,
                                                                             &encap_info->nwk_qos_id,
                                                                             qos_pipe_profile_first_reference));
        }

        out_lif_profile_orig = DNX_OUT_LIF_PROFILE_DEFAULT;

        if (encap_info->label_array[0].egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace)
        {
            /** allocate TTL PIPE profile */
            ttl_data = encap_info->label_array[0].ttl;
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.allocate_single
                            (unit, alloc_flags, &ttl_data, NULL,
                             &encap_info->ttl_pipe_profile, ttl_pipe_profile_first_reference));
        }
    }

    SHR_IF_ERR_EXIT(dnx_egress_outlif_profile_alloc(unit,
                                                    network_group_id, out_lif_profile_orig,
                                                    &(encap_info->out_lif_profile), dbal_table_id, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get ttl value based on ttl_pipe_profile
*   \param [in] unit -Relevant unit.
*   \param [out] encap_info - update encap_info->label_array[0].ttl based on encap_info.ttl_pipe_profile
* \return
*   shr_error_e
*/
static shr_error_e
dnx_mpls_get_ttl_value(
    int unit,
    dnx_mpls_encap_t * encap_info)
{
    int reference_counter = 0;
    int ttl;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.profile_data_get
                    (unit, encap_info->ttl_pipe_profile, &reference_counter, &ttl));
    encap_info->label_array[0].ttl = ttl;
    encap_info->label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;

exit:
    SHR_FUNC_EXIT;
}

/**
* See .h file
*/
shr_error_e
dnx_mpls_encap_to_lif_table_manager_info(
    int unit,
    uint32 entry_handle_id,
    dnx_mpls_encap_t * encap_info,
    lif_table_mngr_outlif_info_t * outlif_info)
{
    int nof_additional_headers;
    bcm_gport_t next_pointer = 0;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dbal_enum_value_field_etpp_additional_header_profile_stack_attributes_e additional_headers_profile;
    dbal_tables_e dbal_table_id;
    dbal_enum_value_field_encap_qos_model_e encap_qos_model;
    int mpls_special_label_encap_handling;
    uint8 is_alternate_marking;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(encap_info, _SHR_E_INTERNAL, "encap_info");
    SHR_NULL_CHECK(outlif_info, _SHR_E_INTERNAL, "outlif_info");

    sal_memset(outlif_info, 0, sizeof(lif_table_mngr_outlif_info_t));
    mpls_special_label_encap_handling = dnx_data_mpls.general.mpls_special_label_encap_handling_get(unit);

    /*
     * Take DBAL table ID from handle
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
    /*
     * Fill the phase of the lif
     */
    outlif_info->outlif_phase = encap_info->outlif_phase;

    is_alternate_marking = _SHR_IS_FLAG_SET(encap_info->label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING);

    /*
     * Fill superset handle with all the required fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    /*
     * Configure next pointer to be the ARP or LIF pointer
     */
    if ((encap_info->label_array[0].l3_intf_id != 0) && (encap_info->label_array[0].l3_intf_id != BCM_IF_INVALID))
    {
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_pointer, encap_info->label_array[0].l3_intf_id);
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, next_pointer, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                     gport_hw_resources.local_out_lif);
    }
    else if ((dbal_table_id == DBAL_TABLE_EEDB_MPLS_TUNNEL) && (encap_info->label_array[0].l3_intf_id == 0))
    {
        /** In case of MPLS tunnel, interface 0 will reserve NEXT POINTER field */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, 0);
    }

    /*
     * Set ESEM extraction command
     */
    if (encap_info->esem_command_profile != dnx_data_esem.access_cmd.no_action_get(unit))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE,
                                     encap_info->esem_command_profile);
    }

    if (encap_info->require_name_space)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_NAME_SPACE, INST_SINGLE, 0);;
    }

    /** TTL decrement and pipe_enabled are always set - no need to check field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_DECREMENT_DISABLE, INST_SINGLE, FALSE);

    /*
     * TTL and EXP modes
     */
    if (encap_info->label_array[0].egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_TTL_MODE_PIPE);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_TTL_MODE_UNIFORM);
    }

    SHR_IF_ERR_EXIT(dnx_qos_egress_model_bcm_to_dbal
                    (unit, &encap_info->label_array[0].egress_qos_model, &encap_qos_model));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE, encap_qos_model);

    if (encap_info->label_array[0].action == BCM_MPLS_EGRESS_ACTION_PHP)
    {
        /*
         * Configure qos remark profile
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TERMINATION_REMARK_PROFILE, INST_SINGLE,
                                     DNX_QOS_MAP_PROFILE_GET(encap_info->label_array[0].qos_map_id));
    }
    else
    {
        if (encap_info->label_array[0].egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE,
                                         encap_info->ttl_pipe_profile);
        }

        /** Check EXP  pipe or uniform */
        if ((encap_info->label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace) ||
            (encap_info->label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE,
                                         encap_info->nwk_qos_id);
        }

        /** set additional headers profile for special labels */
        additional_headers_profile =
            dnx_mpls_encap_calc_lif_additional_header_profile(encap_info->has_cw, encap_info->label_array[0].flags);
        if (additional_headers_profile != DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_NONE)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_AHP_MPLS,
                                         INST_SINGLE, additional_headers_profile);
        }

        if (encap_info->labels_nof < DNX_MPLS_TUNNEL_ENCAP_TWO_LABELS)
        {
           /** MPLS 1 case - label1 field is required */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE,
                                         encap_info->label_array[0].label);
        }
        else
        {
            /** MPLS 2 case - label2 field is required as well */
            uint32 second_label_value;
            if (is_alternate_marking)
            {
                SHR_IF_ERR_EXIT(dnx_mpls_alternate_marking_special_label_get(unit, &second_label_value));
            }
            else
            {
                second_label_value = encap_info->label_array[1].label;
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, INST_SINGLE, second_label_value);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_2, INST_SINGLE,
                                         encap_info->label_array[0].label);
        }

        /*
         * Configure qos remark profile
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE,
                                     DNX_QOS_MAP_PROFILE_GET(encap_info->label_array[0].qos_map_id));
    }

    /*
     * Configure outlif profile
     */
    if (encap_info->out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                     encap_info->out_lif_profile);
    }
    /*
     * For PWE only (not needed for MPLS): Set llvp_profile to Default Native AC
     */
    if ((dbal_table_id == DBAL_TABLE_EEDB_PWE) || (dbal_table_id == DBAL_TABLE_EEDB_EVPN))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LLVP_PROFILE, INST_SINGLE,
                                     encap_info->out_lif_llvp_profile);
    }

    /*
     * setting egress protection information
     */
    if (encap_info->protection_pointer > 0)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE,
                                     encap_info->protection_pointer);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE,
                                     encap_info->protection_path);
    }

    /*
     * Statistics - if enabled set stat field to 0 to signal existence
     */
    if (_SHR_IS_FLAG_SET(encap_info->label_array[0].flags, BCM_MPLS_EGRESS_LABEL_STAT_ENABLE))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, 0);
    }

    /**
     * Table specific flags
     */
    /** PHP case */
    if (encap_info->label_array[0].action == BCM_MPLS_EGRESS_ACTION_PHP)
    {
        outlif_info->table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_PHP;
    }

    if (mpls_special_label_encap_handling)
    {
        /** advanced rules for additional headers */
        nof_additional_headers = dnx_mpls_nof_additional_headers(encap_info->has_cw, encap_info->label_array[0].flags);
        switch (nof_additional_headers)
        {
            case 1:
            {
                outlif_info->table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_ONE_SPECIAL_LABEL;
                break;
            }
            case 2:
            {
                outlif_info->table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_TWO_SPECIAL_LABELS;
                break;
            }
            case 3:
            {
                outlif_info->table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_THREE_SPECIAL_LABELS;
                break;
            }
            default:
                break;
        }
    }

    /** advanced rules for TANDEM, no matter if one or two labels */
    if (_SHR_IS_FLAG_SET(encap_info->label_array[0].flags, BCM_MPLS_EGRESS_LABEL_TANDEM))
    {
        outlif_info->table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_TANDEM;
    }

    if (encap_info->is_iml)
    {
        outlif_info->table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_IML;
    }

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
* Fill MPLS encap info according to local outlif
*   \param [in] unit         -  Relevant unit.
*   \param [in] entry_handle_id -  DBAL handle id of type superset
*   \param [in] outlif_info  - Local Outlif Information.
*   \param [out] encap_info  - MPLS encapsulation information.
*
* \return
*   Negative in case of an error. See \ref shr_error_e
* \remark
 */
shr_error_e
dnx_mpls_lif_table_manager_to_encap_info(
    int unit,
    uint32 entry_handle_id,
    lif_table_mngr_outlif_info_t * outlif_info,
    dnx_mpls_encap_t * encap_info)
{
    uint32 next_pointer = DNX_MPLS_CFG_LIF_NULL_VALUE;
    uint32 ttl_mode;
    uint32 remark_profile = 0;
    dbal_enum_value_field_etpp_additional_header_profile_stack_attributes_e additional_headers_profile = 0;
    dbal_enum_value_field_encap_qos_model_e qos_model;
    bcm_qos_egress_model_t model;
    dbal_tables_e dbal_table_id;
    uint8 is_field_valid;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(encap_info, _SHR_E_INTERNAL, "encap_info");
    SHR_NULL_CHECK(outlif_info, _SHR_E_INTERNAL, "outlif_info");

    /*
     * Get table id from handle
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

    encap_info->outlif_phase = outlif_info->outlif_phase;

    /** Number of labels */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_2, &is_field_valid));
    if (is_field_valid == TRUE)
    {
        encap_info->labels_nof = 2;
    }
    else if (_SHR_IS_FLAG_SET(outlif_info->table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_PHP))
    {
        encap_info->labels_nof = 0;
        encap_info->label_array[0].action = BCM_MPLS_EGRESS_ACTION_PHP;
        encap_info->label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
    }
    else
    {
        encap_info->labels_nof = 1;
    }

    if (_SHR_IS_FLAG_SET(outlif_info->table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_TANDEM))
    {
        encap_info->label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TANDEM;
        if (encap_info->labels_nof > 1)
        {
            encap_info->label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_TANDEM;
        }
    }

    if (encap_info->labels_nof == 1)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                            DBAL_FIELD_MPLS_LABEL, INST_SINGLE,
                                                            &encap_info->label_array[0].label));
    }
    else if (encap_info->labels_nof == 2)
    {
        uint32 special_label_value;
        /** If two labels, the first is returned in the second member of the array */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL,
                                                            INST_SINGLE, &encap_info->label_array[1].label));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_2,
                                                            INST_SINGLE, &encap_info->label_array[0].label));
        /** Check if alternate marking */
        SHR_IF_ERR_EXIT(dnx_mpls_alternate_marking_special_label_get(unit, &special_label_value));
        if (encap_info->label_array[1].label == special_label_value)
        {
            encap_info->label_array[1].label = BCM_MPLS_LABEL_INVALID;
            encap_info->label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING;
            encap_info->label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING;
        }
    }

    /** Get TTL configurations */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_TTL_MODE, &is_field_valid, &ttl_mode));
    if (is_field_valid == TRUE)
    {
        /** uniform is default, do nothing*/
        if (ttl_mode == DBAL_ENUM_FVAL_TTL_MODE_PIPE)
        {
            encap_info->label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
            if (!_SHR_IS_FLAG_SET(outlif_info->table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_PHP))
            {
                /** In case of not PHP look for TTL value */
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE,
                                 (uint32 *) &encap_info->ttl_pipe_profile));

                SHR_IF_ERR_EXIT(dnx_mpls_get_ttl_value(unit, encap_info));

                if (encap_info->labels_nof == 2)
                {
                    encap_info->label_array[1].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
                }
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found HW entry with no TTL model. This entry type is not supported");
    }

    /** Get QOS configurations */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, &is_field_valid, &qos_model));
    if (is_field_valid == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_model_dbal_to_bcm(unit, qos_model, &model));
        /** Get qos model. In case of alternate marking, set to 0 qos model of label 0 */
        encap_info->label_array[0].egress_qos_model.egress_qos = model.egress_qos;
        encap_info->label_array[0].egress_qos_model.egress_ecn = model.egress_ecn;
        if (encap_info->labels_nof == 2)
        {
            encap_info->label_array[1].egress_qos_model.egress_qos = model.egress_qos;
            encap_info->label_array[1].egress_qos_model.egress_ecn = model.egress_ecn;
        }

        if ((model.egress_qos == bcmQosEgressModelPipeNextNameSpace)
            || (model.egress_qos == bcmQosEgressModelPipeMyNameSpace))
        {
            if (!_SHR_IS_FLAG_SET(outlif_info->table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_PHP))
            {
                /** In case of not PHP look for network qos id value */
                uint32 nwk_qos_id;

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX,
                                                                    INST_SINGLE, &nwk_qos_id));
                encap_info->nwk_qos_id = (int) nwk_qos_id;
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found HW entry with no QOS model. This entry type is not supported");
    }

    if (_SHR_IS_FLAG_SET(outlif_info->table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_PHP))
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_TERMINATION_REMARK_PROFILE, &is_field_valid,
                         &remark_profile));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, &is_field_valid, &remark_profile));
    }
    if ((is_field_valid == TRUE) && (remark_profile != 0))
    {
        DNX_QOS_REMARK_MAP_SET(remark_profile);
        DNX_QOS_EGRESS_MAP_SET(remark_profile);
        encap_info->label_array[0].qos_map_id = remark_profile;
        if ((encap_info->labels_nof == 2))
        {
            encap_info->label_array[1].qos_map_id = remark_profile;
        }
    }

    /*
     * Get Statistics flag - if set stat field exists set the stat flag
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, &is_field_valid));
    if (is_field_valid == TRUE)
    {
        encap_info->label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_STAT_ENABLE;
        if ((encap_info->labels_nof == 2))
        {
            encap_info->label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_STAT_ENABLE;
        }
    }

    if (!_SHR_IS_FLAG_SET(outlif_info->table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_PHP))
    {
        /** Get additional headers profile */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_LIF_AHP_MPLS, &is_field_valid,
                         (uint32 *) &additional_headers_profile));
        if (is_field_valid == TRUE)
        {
            dnx_mpls_encap_lif_additional_header_profile_to_label_flags(additional_headers_profile, &encap_info->has_cw,
                                                                        &encap_info->label_array[0].flags);
        }
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, &is_field_valid, &next_pointer));
    if ((is_field_valid == TRUE) && (next_pointer != DNX_MPLS_CFG_LIF_NULL_VALUE))
    {
        bcm_gport_t next_gport = BCM_GPORT_INVALID;
        /** In case local lif is not found, return next pointer 0 */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, _SHR_CORE_ALL,
                                                    next_pointer, &next_gport));
        if (next_gport != BCM_GPORT_INVALID)
        {
            if (BCM_GPORT_SUB_TYPE_IS_L3_VLAN_TRANSLATION(next_gport))
            {
                /** In case of vlan translation (ARP + AC entry) manually setting the interface */
                BCM_L3_ITF_SET(encap_info->label_array[0].l3_intf_id, _SHR_L3_ITF_TYPE_LIF,
                               BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_GET(next_gport));
            }
            else
            {
                BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(encap_info->label_array[0].l3_intf_id, next_gport);
            }
        }
    }
    else if ((dbal_table_id == DBAL_TABLE_EEDB_MPLS_TUNNEL) && (is_field_valid == FALSE))
    {
        /** No next pointer is indicated by -1 interface id */
        encap_info->label_array[0].l3_intf_id = BCM_IF_INVALID;
    }
    if ((encap_info->labels_nof == 2))
    {
        encap_info->label_array[1].l3_intf_id = encap_info->label_array[0].l3_intf_id;
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, &is_field_valid,
                     (uint32 *) &encap_info->out_lif_profile));

    /** TTL is always decremented */
    encap_info->label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    if ((encap_info->labels_nof == 2))
    {
        encap_info->label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    }

    /** Set llvp_profile to Default Native AC */
    if ((dbal_table_id == DBAL_TABLE_EEDB_PWE) || (dbal_table_id == DBAL_TABLE_EEDB_EVPN))
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_EGRESS_LLVP_PROFILE, &is_field_valid,
                         (uint32 *) &(encap_info->out_lif_llvp_profile)));
    }

    /*
     * getting egress protection information
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, &is_field_valid,
                     (uint32 *) &(encap_info->label_array[0].egress_failover_id)));
    if (is_field_valid == TRUE)
    {

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE,
                         (uint32 *) &(encap_info->label_array[0].egress_failover_if_id)));
    }

    if ((encap_info->labels_nof == 2))
    {
        encap_info->label_array[1].egress_failover_id = encap_info->label_array[0].egress_failover_id;
        encap_info->label_array[1].egress_failover_if_id = encap_info->label_array[0].egress_failover_if_id;
    }

    /*
     * Get ESEM command, only if exists
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, &is_field_valid,
                     (uint32 *) &(encap_info->esem_command_profile)));

    if (!is_field_valid)
    {
        encap_info->esem_command_profile = dnx_data_esem.access_cmd.no_action_get(unit);
    }

    if (_SHR_IS_FLAG_SET(outlif_info->table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_IML))
    {
        encap_info->is_iml = 1;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Create MPLS tunnels in the egress for push.
* \param [in] unit         - Relevant unit.
* \param [in] num_labels   - Number of labels to be used.
* \param [in] label_array  - Array containing 'num_labels' labels to be pushed.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_mpls_tunnel_initiator_create_push(
    int unit,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
    int out_global_lif;
    dnx_egr_pointed_t egr_pointed_id;
    dnx_mpls_encap_t encap_info, encap_info_orig;
    bcm_mpls_egress_label_t label_array_orig[2];
    uint8 ttl_pipe_profile_first_reference = FALSE;
    uint8 ttl_pipe_profile_last_reference = FALSE;
    uint8 qos_pipe_profile_first_reference = FALSE;
    uint8 qos_pipe_profile_last_reference = FALSE;
    uint8 esem_cmd_profile_first_reference = FALSE;
    uint8 esem_cmd_profile_last_reference = FALSE;
    dnx_esem_cmd_data_t esem_cmd_data;
    uint32 lif_flags;
    lif_table_mngr_outlif_info_t outlif_info;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t gport;
    uint32 entry_handle_id_orig, entry_handle_id;
    int local_outlif = 0;
    int old_local_outlif = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * SW allocations
     */
    /** Allocate Local & Global LIFs */
    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_encap_lif_allocate(unit, label_array, &out_global_lif, &egr_pointed_id));

    /** get EEDB entry to be replaced */
    dnx_mpls_encap_t_init(unit, &encap_info_orig);
    bcm_mpls_egress_label_t_init(&label_array_orig[0]);
    bcm_mpls_egress_label_t_init(&label_array_orig[1]);

    /** In case of replace - get original information */
    if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_REPLACE))
    {
        lif_table_mngr_outlif_info_t orig_lif_info;

        lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, label_array[0].tunnel_id);
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));

        /** get EEDB entry to be replaced */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_MPLS_TUNNEL, &entry_handle_id_orig));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                        (unit, gport_hw_resources.local_out_lif, entry_handle_id_orig, &orig_lif_info));
        encap_info_orig.label_array = label_array_orig;
        SHR_IF_ERR_EXIT(dnx_mpls_lif_table_manager_to_encap_info
                        (unit, entry_handle_id_orig, &orig_lif_info, &encap_info_orig));
    }

    /** add  mpls labels to be pushed */
    dnx_mpls_encap_t_init(unit, &encap_info);
    encap_info.labels_nof = num_labels;
    encap_info.label_array = label_array;

    /** Set outlif eedb phase. */
    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_initiator_encap_access_to_outlif_phase(unit, label_array[0].encap_access,
                                                                           &encap_info.outlif_phase));

    /** Protection */
    if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_PROTECTION))
    {
        encap_info.protection_path = (encap_info.label_array[0].egress_failover_if_id) ? 0 : 1;
        DNX_FAILOVER_ID_GET(encap_info.protection_pointer, encap_info.label_array[0].egress_failover_id);
    }

    /** allocate lif, TTL and qos profiles if needed */
    SHR_IF_ERR_EXIT(dnx_mpls_egress_lif_ttl_qos_profiles_sw_allocate
                    (unit, _SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_REPLACE),
                     DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION, DBAL_TABLE_EEDB_MPLS_TUNNEL, encap_info_orig, &encap_info,
                     &ttl_pipe_profile_first_reference, &ttl_pipe_profile_last_reference,
                     &qos_pipe_profile_first_reference, &qos_pipe_profile_last_reference));
    /*
     * writing to HW tables
     */

    /** Write to HW if new TTL profile was allocated */
    if (ttl_pipe_profile_first_reference)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_set
                        (unit, encap_info.ttl_pipe_profile, (uint32) encap_info.label_array[0].ttl));
    }
    if (qos_pipe_profile_first_reference)
    {
        uint32 nwk_qos_pipe, nwk_qos_pipe_mapped;

        if (num_labels > 1)
        {
            int exp0;

            /** For alternate Marking set TC to const value */
            if (_SHR_IS_FLAG_SET(encap_info.label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING))
            {
                exp0 =
                    _SHR_IS_FLAG_SET(encap_info.label_array[0].flags,
                                     BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING_LOSS_SET) ? 0x4 : 0;
            }
            else
            {
                exp0 = encap_info.label_array[0].exp;
            }

            nwk_qos_pipe = DNX_QOS_IDX_EXP_TO_NWK_QOS(exp0, encap_info.label_array[1].exp);
            nwk_qos_pipe_mapped = encap_info.label_array[1].exp;
        }
        else
        {
            nwk_qos_pipe = DNX_QOS_IDX_EXP_TO_NWK_QOS(encap_info.label_array[0].exp, encap_info.label_array[0].exp);
            nwk_qos_pipe_mapped = encap_info.label_array[0].exp;
        }
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_set
                        (unit, encap_info.nwk_qos_id, nwk_qos_pipe, nwk_qos_pipe_mapped, 0));
    }

    /** Add an ESEM command to invoke an SVTAG lookup if needed **/
    if (_SHR_IS_FLAG_SET(label_array[0].flags2, BCM_MPLS_EGRESS_LABEL_FLAGS2_SVTAG_ENABLE))
    {

        dnx_port_esem_cmd_info_t esem_cmd_info;
        esem_cmd_info.nof_accesses = 0;

        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_add(unit, 0, ESEM_ACCESS_TYPE_SVTAG,
                                                     DBAL_ENUM_FVAL_ESEM_APP_DB_ID_OUT_LIF,
                                                     DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID, &esem_cmd_info));

        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_exchange(unit, 0,
                                                   DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_BIT_2, 0,
                                                   &esem_cmd_info,
                                                   encap_info.esem_command_profile,
                                                   &(encap_info.esem_command_profile),
                                                   &esem_cmd_data, &esem_cmd_profile_first_reference,
                                                   &esem_cmd_profile_last_reference));
    }

    /** Write to HW if new esem cmd profile was allocated */
    if (esem_cmd_profile_first_reference)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, encap_info.esem_command_profile, esem_cmd_data));
    }

    /*
     * Fill outlif info struct with all fields relevant info
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_MPLS_TUNNEL, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_mpls_encap_to_lif_table_manager_info(unit, entry_handle_id, &encap_info, &outlif_info));
    /*
     * Find result type, allocate lif and write to HW
     */
    outlif_info.global_lif = out_global_lif;
    if (out_global_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;
    }

    if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_REPLACE))
    {
        outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
        local_outlif = gport_hw_resources.local_out_lif;
    }
    /*
     * Note that 'local_lif' may be changed by this procedure. In that case
     * 'virtual_egr_pointed' system needs to be updated as well. See below.
     */
    old_local_outlif = local_outlif;
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
                    (unit, entry_handle_id, &local_outlif, &outlif_info));
    /*
     * Update tunnel-ID according to either global LIF value or 'egr_pointed' value
     */
    /*
     * The following piece of code should be executed only in the 'not WITH_ID' case if
     * 'label_array[0].tunnel_id' could be assumed to have been fully encoded
     * by the caller in the WITH_ID case.
     *     if (!_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_WITH_ID))
     */
    {
        if (out_global_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            /*
             * Enter here if 'out_global_lif' is 'valid'.
             * Note that, for the 'egr_pointed' case, procedure dnx_mpls_tunnel_encap_lif_allocate()
             * above has returned an 'invalid' global lif.
             */
            /*
             * Note that, effectively, BCM_L3_ITF_SET loads the 'sub_type' field by the 'not_encoded'
             * (= 'default') value. If this becomes not true then immitate the handling for
             * 'egr_pointed' below.
             */
            BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, out_global_lif);
            if (num_labels == DNX_MPLS_TUNNEL_ENCAP_TWO_LABELS)
            {
                BCM_L3_ITF_SET(label_array[1].tunnel_id, BCM_L3_ITF_TYPE_LIF, out_global_lif);
            }
        }
        else
        {
            /*
             * Enter here if 'out_global_lif' is 'invalid' and 'egr_pointed_id' is valid.
             */
            int l3_itf_dummy;
            uint32 mapping_flags = 0;

            BCM_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(l3_itf_dummy, egr_pointed_id);
            BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, l3_itf_dummy);
            if (num_labels == DNX_MPLS_TUNNEL_ENCAP_TWO_LABELS)
            {
                BCM_L3_ITF_SET(label_array[1].tunnel_id, BCM_L3_ITF_TYPE_LIF, l3_itf_dummy);
            }
            /*
             * Now create mapping between 'egr_pointed' and 'local_lif' in case this is not replace
             */
            if (!_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_REPLACE))
            {
                /*
                 * If this is not a 'replace' operation then create mapping between
                 * 'egr_pointed' and 'local_lif'
                 */
                SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_local_lif_mapping_add
                                (unit, mapping_flags, egr_pointed_id, local_outlif));
            }
            else if (old_local_outlif != local_outlif)
            {
                /*
                 * If this is a 'replace' operation but 'local_lif' has been replaced then
                 * update mapping between 'egr_pointed' and 'local_lif':
                 * Remove exiting mapping and add the new one.
                 */
                SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_local_lif_mapping_remove(unit, mapping_flags, egr_pointed_id));
                SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_local_lif_mapping_add
                                (unit, mapping_flags, egr_pointed_id, local_outlif));
            }
        }
    }
    /*
     * After new entry values are saved to HW delete redundant old values from HW
     */
    /** clear from HW the TTL profile */
    if (ttl_pipe_profile_last_reference)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_clear(unit, encap_info_orig.ttl_pipe_profile));
    }

    if (qos_pipe_profile_last_reference)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_hw_clear(unit, encap_info_orig.nwk_qos_id));
    }

    /** clear from HW the esem cmd profile */
    if (esem_cmd_profile_last_reference)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_clear(unit, encap_info_orig.esem_command_profile));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Create MPLS tunnels in the egress for php.
* \param [in] unit           - Relevant unit.
* \param [in] label_array    - Array containing the php label
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_mpls_tunnel_initiator_create_php(
    int unit,
    bcm_mpls_egress_label_t * label_array)
{
    int out_global_lif;
    dnx_egr_pointed_t egr_pointed_id;
    dnx_mpls_encap_t encap_info;
    lif_table_mngr_outlif_info_t outlif_info;
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verification
     */
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_tunnel_initiator_create_php_verify(unit, label_array));

    /*
     * SW allocations
     */
    /** Allocate Local & Global LIFs */
    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_encap_lif_allocate(unit, label_array, &out_global_lif, &egr_pointed_id));
    /*
     * writing to HW tables
     */
    /** add  mpls labels to be pushed */
    dnx_mpls_encap_t_init(unit, &encap_info);
    encap_info.label_array = label_array;
    encap_info.esem_command_profile = dnx_data_esem.access_cmd.no_action_get(unit);
    /** Set outlif eedb phase. */
    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_initiator_encap_access_to_outlif_phase(unit, label_array[0].encap_access,
                                                                           &encap_info.outlif_phase));

    /*
     * Fill outlif info struct with all fields relevant info
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_MPLS_TUNNEL, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_mpls_encap_to_lif_table_manager_info(unit, entry_handle_id, &encap_info, &outlif_info));
    outlif_info.global_lif = out_global_lif;
    if (out_global_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;
    }

    /*
     * Find result type, allocate lif and write to HW
     */
    if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_REPLACE))
    {
        bcm_gport_t gport;

        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, label_array[0].tunnel_id);
        outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources));
    }
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
                    (unit, entry_handle_id, &gport_hw_resources.local_out_lif, &outlif_info));

    /** Update tunnel-ID according to global LIF value */
    if (!_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_WITH_ID))
    {
        BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, out_global_lif);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See mpls.h file
 */
shr_error_e
dnx_mpls_label_info_to_push_profile(
    int unit,
    bcm_mpls_egress_label_t * label_entry,
    uint8 has_cw,
    dnx_jr_mode_mpls_push_profile_t * push_profile_info)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(label_entry, _SHR_E_PARAM, "info");

    sal_memset(push_profile_info, 0, sizeof(dnx_jr_mode_mpls_push_profile_t));

    push_profile_info->ttl_model = label_entry->egress_qos_model.egress_ttl;
    push_profile_info->ttl = label_entry->ttl;
    push_profile_info->exp_model = label_entry->egress_qos_model.egress_qos;
    push_profile_info->exp = label_entry->exp;

    push_profile_info->has_cw = has_cw;

    push_profile_info->remark_profile = label_entry->qos_map_id;

    push_profile_info->add_entropy_label =
        _SHR_IS_FLAG_SET(label_entry->flags, BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE) ? TRUE : FALSE;
    push_profile_info->add_entropy_label_indicator =
        _SHR_IS_FLAG_SET(label_entry->flags, BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE) ? TRUE : FALSE;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * See mpls_tunnel_encap.h file
 */
shr_error_e
dnx_push_profile_to_mpls_label_info(
    int unit,
    int push_cmd,
    bcm_mpls_port_t * mpls_port)
{
    int ref_count;
    dnx_jr_mode_mpls_push_profile_t push_profile_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mpls_port, _SHR_E_PARAM, "mpls_port");

    /** get push profile info */
    sal_memset(&push_profile_info, 0, sizeof(dnx_jr_mode_mpls_push_profile_t));

    SHR_IF_ERR_EXIT(algo_mpls_db.push_profile.profile_data_get(unit, push_cmd, &ref_count, &push_profile_info));

    if (0 == ref_count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The PUSH profile does not exist");
    }
    mpls_port->egress_label.egress_qos_model.egress_ttl = push_profile_info.ttl_model;
    if (push_profile_info.ttl_model == bcmQosEgressModelPipeMyNameSpace)
    {
        mpls_port->egress_label.ttl = push_profile_info.ttl;
    }

    mpls_port->egress_label.egress_qos_model.egress_qos = push_profile_info.exp_model;
    if ((push_profile_info.exp_model == bcmQosEgressModelPipeNextNameSpace) ||
        (push_profile_info.exp_model == bcmQosEgressModelPipeMyNameSpace))
    {
        mpls_port->egress_label.exp = push_profile_info.exp;
    }

    if (push_profile_info.has_cw)
    {
        mpls_port->flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }

    mpls_port->egress_label.qos_map_id = push_profile_info.remark_profile;

    if (push_profile_info.add_entropy_label)
    {
        mpls_port->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
    }

    if (push_profile_info.add_entropy_label_indicator)
    {
        mpls_port->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Check whether the push profile was already allocated
 * using the template allocation algorithm, if not allocate new
 * profile.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] push_profile_info - A struct containing relevant information for the Term Profile.
 * \param [out] push_profile                     -  Push profile used as reference
 * \param [in] old_push_profile                     -  Push profile used as reference - used in case exchange is needed
 * \param [out] is_first_push_profile_reference  -  Returns 1 if the profile is the first reference
 * \param [out] is_last_push_profile_reference  -  Returns 1 if the profile is the last reference - used in case exchange is needed
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_mpls_push_profile_allocation(
    int unit,
    dnx_jr_mode_mpls_push_profile_t push_profile_info,
    int *push_profile,
    int old_push_profile,
    uint8 *is_first_push_profile_reference,
    uint8 *is_last_push_profile_reference)
{
    int alloc_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_first_push_profile_reference, _SHR_E_PARAM, "is_first_push_profile_reference");

    alloc_flags = *push_profile ? DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID : 0;
    if (is_last_push_profile_reference == NULL)
    {
        SHR_IF_ERR_EXIT(algo_mpls_db.push_profile.allocate_single
                        (unit, alloc_flags, &push_profile_info, NULL, push_profile, is_first_push_profile_reference));
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_mpls_db.push_profile.exchange
                        (unit, 0, &push_profile_info, old_push_profile, NULL, push_profile,
                         is_first_push_profile_reference, is_last_push_profile_reference));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Free push profile using the template manager.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] push_profile - Push profile number to be used for the loookup
 * \param [out] is_last - Indicated this is the last entry
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_mpls_push_profile_free(
    int unit,
    uint32 push_profile,
    uint8 *is_last)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_mpls_db.push_profile.free_single(unit, push_profile, is_last));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Write to DBAL_TABLE_JR1_MPLS_PUSH_COMMAND table.
 * \param [in] unit - Relevant unit.
 * \param [in] encap_info - A pointer to a struct containing relevant information for the Push Profile.
 * \param [in] push_profile - Push Profile number, used as index to the table.
 * \return
 *   Error indication according to shr_error_e enum
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_mpls_push_profile_hw_set(
    int unit,
    dnx_mpls_encap_t * encap_info,
    int push_profile)
{

    uint32 entry_handle_id;
    dbal_enum_value_field_etpp_additional_header_profile_stack_attributes_e additional_headers_profile;
    uint32 remark_profile;
    dbal_enum_value_field_encap_qos_model_e encap_qos_model;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_JR1_MPLS_PUSH_COMMAND table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_JR1_MPLS_PUSH_COMMAND, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PUSH_PROFILE, push_profile);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_JR1_MPLS_PUSH_COMMAND_ETPS_MPLS_1);

    if (encap_info->label_array[0].egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE,
                                     encap_info->ttl_pipe_profile);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_TTL_MODE_PIPE);
    }
    else
    {

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_TTL_MODE_UNIFORM);
    }

    /** qos model */
    SHR_IF_ERR_EXIT(dnx_qos_egress_model_bcm_to_dbal
                    (unit, &encap_info->label_array[0].egress_qos_model, &encap_qos_model));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE, encap_qos_model);

    if ((encap_info->label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace) ||
        (encap_info->label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE,
                                     encap_info->nwk_qos_id);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, 0);
    }

    /** set additional headers profile for special labels */
    additional_headers_profile =
        dnx_mpls_encap_calc_lif_additional_header_profile(encap_info->has_cw, encap_info->label_array[0].flags);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_AHP_MPLS, INST_SINGLE,
                                 additional_headers_profile);
    /*
     * Configure qos remark profile
     */
    remark_profile = DNX_QOS_MAP_PROFILE_GET(encap_info->label_array[0].qos_map_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, remark_profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_QOS_MODEL, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_DECREMENT_DISABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MTU_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE,
                                 encap_info->esem_command_profile);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief
 * Delete the push profile entry form the HW
 *
 * \param [in] unit - Relevant unit.
 * \param [in] push_profile - Push profile number to be deleted
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_mpls_push_profile_hw_delete(
    int unit,
    uint32 push_profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Clear the values written in DBAL_TABLE_JR1_MPLS_PUSH_COMMAND table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_JR1_MPLS_PUSH_COMMAND, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PUSH_PROFILE, push_profile);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See mpls_tunnel_encap.h file
 */
shr_error_e
dnx_mpls_encap_get_from_push_cmd(
    int unit,
    int push_cmd,
    dnx_mpls_encap_t * encap_info)
{
    uint32 entry_handle_id;
    uint32 ttl_mode;
    uint32 remark_profile;
    dbal_enum_value_field_etpp_additional_header_profile_stack_attributes_e additional_headers_profile;
    dbal_enum_value_field_encap_qos_model_e qos_model;
    bcm_qos_egress_model_t model;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(encap_info, _SHR_E_INTERNAL, "encap_info");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_JR1_MPLS_PUSH_COMMAND, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PUSH_PROFILE, push_cmd);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Get TTL configurations */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                                        &ttl_mode));

    if (ttl_mode == DBAL_ENUM_FVAL_TTL_MODE_PIPE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE,
                         (uint32 *) &encap_info->ttl_pipe_profile));

        encap_info->label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        SHR_IF_ERR_EXIT(dnx_mpls_get_ttl_value(unit, encap_info));
    }
    else
    {
        encap_info->label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    }

    /** Get QOS configurations */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE,
                                                        &qos_model));
    SHR_IF_ERR_EXIT(dnx_qos_egress_model_dbal_to_bcm(unit, qos_model, &model));

    encap_info->label_array[0].egress_qos_model.egress_qos = model.egress_qos;
    encap_info->label_array[0].egress_qos_model.egress_ecn = model.egress_ecn;
    if ((model.egress_qos == bcmQosEgressModelPipeMyNameSpace)
        || (model.egress_qos == bcmQosEgressModelPipeNextNameSpace))
    {
        uint32 nwk_qos_id;
        uint8 network_qos_pipe;
        uint8 network_qos_pipe_mapped;
        uint8 dp_pipe_mapped;

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX,
                                                            INST_SINGLE, &nwk_qos_id));
        encap_info->nwk_qos_id = (int) nwk_qos_id;

        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_attr_get(unit,
                                                                         (int) nwk_qos_id,
                                                                         &network_qos_pipe,
                                                                         &network_qos_pipe_mapped, &dp_pipe_mapped));

        encap_info->label_array[0].exp = network_qos_pipe & DNX_QOS_EXP_MASK;
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, &remark_profile));
    if (remark_profile != 0)
    {
        DNX_QOS_REMARK_MAP_SET(remark_profile);
        DNX_QOS_EGRESS_MAP_SET(remark_profile);
        encap_info->label_array[0].qos_map_id = remark_profile;
    }

    /** Get additional headers profile */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LIF_AHP_MPLS, INST_SINGLE,
                     (uint32 *) &additional_headers_profile));
    dnx_mpls_encap_lif_additional_header_profile_to_label_flags(additional_headers_profile, &encap_info->has_cw,
                                                                &encap_info->label_array[0].flags);

    /** TTL is always decremented */
    encap_info->label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
*   see mpls_tunnel_encap.h
*/
shr_error_e
dnx_mpls_tunnel_create_push_entry_from_eei(
    int unit,
    bcm_mpls_egress_label_t * label_entry,
    uint8 has_cw,
    uint8 is_replace,
    int *push_cmd)
{
    dnx_jr_mode_mpls_push_profile_t push_profile_info;
    uint8 is_first_push_profile_reference = 0;
    uint8 is_last_push_profile_reference = 0;
    dnx_mpls_encap_t encap_info, encap_info_orig;
    bcm_mpls_egress_label_t label_array[2];
    bcm_mpls_egress_label_t label_array_orig[2];
    uint8 ttl_pipe_profile_first_reference = FALSE;
    uint8 ttl_pipe_profile_last_reference = FALSE;
    uint8 qos_pipe_profile_first_reference = FALSE;
    uint8 qos_pipe_profile_last_reference = FALSE;
    int old_push_cmd = *push_cmd;

    SHR_FUNC_INIT_VARS(unit);

    /** Allocation for Push profile */
    SHR_IF_ERR_EXIT(dnx_mpls_label_info_to_push_profile(unit, label_entry, has_cw, &push_profile_info));

    dnx_mpls_encap_t_init(unit, &encap_info_orig);
    bcm_mpls_egress_label_t_init(&label_array_orig[0]);
    bcm_mpls_egress_label_t_init(&label_array_orig[1]);
    encap_info_orig.label_array = label_array_orig;
    if (is_replace)
    {
        SHR_IF_ERR_EXIT(dnx_mpls_encap_get_from_push_cmd(unit, old_push_cmd, &encap_info_orig));
        /*
         * Exchange opeation, update the property with same profile ID
         */
        SHR_IF_ERR_EXIT(dnx_mpls_push_profile_allocation
                        (unit, push_profile_info, push_cmd, old_push_cmd, &is_first_push_profile_reference,
                         &is_last_push_profile_reference));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mpls_push_profile_allocation
                        (unit, push_profile_info, push_cmd, 0, &is_first_push_profile_reference, NULL));
    }
    /*
     * writing to HW tables
     */
    if (is_first_push_profile_reference)
    {
        /** add  mpls labels to be pushed */
        dnx_mpls_encap_t_init(unit, &encap_info);
        bcm_mpls_egress_label_t_init(&label_array[0]);
        bcm_mpls_egress_label_t_init(&label_array[1]);

        /** convert push profile to encap information*/
        label_array[0].egress_qos_model.egress_qos = push_profile_info.exp_model;
        label_array[0].egress_qos_model.egress_ttl = push_profile_info.ttl_model;
        label_array[0].flags |= push_profile_info.add_entropy_label ? BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE : 0;
        label_array[0].flags |=
            push_profile_info.add_entropy_label_indicator ? BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE : 0;

        label_array[0].exp = push_profile_info.exp;
        label_array[0].ttl = push_profile_info.ttl;
        label_array[0].qos_map_id = push_profile_info.remark_profile;
        encap_info.labels_nof = 1;
        encap_info.label_array = label_array;
        encap_info.esem_command_profile = push_profile_info.esem_command_profile;
        encap_info.has_cw = push_profile_info.has_cw;

        /** allocate lif, TTL and qos profiles if needed */
        SHR_IF_ERR_EXIT(dnx_mpls_egress_lif_ttl_qos_profiles_sw_allocate
                        (unit, is_replace, DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION, DBAL_TABLE_EEDB_MPLS_TUNNEL,
                         encap_info_orig, &encap_info, &ttl_pipe_profile_first_reference,
                         &ttl_pipe_profile_last_reference, &qos_pipe_profile_first_reference,
                         &qos_pipe_profile_last_reference));

        /*
         * writing to HW tables
         */

        /** Write to HW if new TTL profile was allocated */
        if (ttl_pipe_profile_first_reference)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_set
                            (unit, encap_info.ttl_pipe_profile, (uint32) encap_info.label_array[0].ttl));
        }

        if (qos_pipe_profile_first_reference)
        {
            /*
             * only one label
             */
            uint32 nwk_qos = encap_info.label_array[0].exp;
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_set(unit, encap_info.nwk_qos_id,
                                                                        nwk_qos, nwk_qos, 0));
        }

        /** Set profile termination in case it is the first time. */
        SHR_IF_ERR_EXIT(dnx_mpls_push_profile_hw_set(unit, &encap_info, *push_cmd));

        /** clear from HW the TTL and QOS profile */
        if (ttl_pipe_profile_last_reference)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_clear(unit, encap_info_orig.ttl_pipe_profile));
        }

        if (qos_pipe_profile_last_reference)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_hw_clear(unit, encap_info_orig.nwk_qos_id));
        }
    }

    if (is_last_push_profile_reference)
    {
        /** Delete profile  in case it is the last reference. */
        SHR_IF_ERR_EXIT(dnx_mpls_push_profile_hw_delete(unit, old_push_cmd));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
*   see mpls_tunnel_encap.h
*/
shr_error_e
dnx_mpls_tunnel_delete_push_entry_from_eei(
    int unit,
    int push_cmd)
{
    uint8 is_last_push_profile_reference = 0;
    dnx_mpls_encap_t encap_info;
    bcm_mpls_egress_label_t label_array[2];
    uint8 ttl_pipe_profile_last_reference = FALSE;
    uint8 qos_pipe_profile_last_reference = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_mpls_push_profile_free(unit, push_cmd, &is_last_push_profile_reference));

    /*
     * writing to HW tables
     */
    if (is_last_push_profile_reference)
    {
        dnx_mpls_encap_t_init(unit, &encap_info);
        bcm_mpls_egress_label_t_init(&label_array[0]);
        bcm_mpls_egress_label_t_init(&label_array[1]);
        encap_info.label_array = label_array;
        SHR_IF_ERR_EXIT(dnx_mpls_encap_get_from_push_cmd(unit, push_cmd, &encap_info));

        /*
         * Clear SW tables
         */
        if (encap_info.label_array[0].egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace)
        {
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.free_single(unit, encap_info.ttl_pipe_profile,
                                                                                &ttl_pipe_profile_last_reference));
        }

        if ((encap_info.label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace) ||
            (encap_info.label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace))
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_free
                            (unit, encap_info.nwk_qos_id, &qos_pipe_profile_last_reference));
        }

        /** clear from HW the TTL profile */
        if (ttl_pipe_profile_last_reference)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_clear(unit, encap_info.ttl_pipe_profile));
        }
        if (qos_pipe_profile_last_reference)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_hw_clear(unit, encap_info.nwk_qos_id));
        }

        /** Set profile termination in case it is the first time. */
        SHR_IF_ERR_EXIT(dnx_mpls_push_profile_hw_delete(unit, push_cmd));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Used by bcm_dnx_mpls_tunnel_initiator_traverse_verify() to verify user input.
*  See description of input parameters on header of bcm_dnx_mpls_tunnel_initiator_traverse_verify()
 * below.
*/
static shr_error_e
dnx_mpls_tunnel_initiator_traverse_verify(
    int unit,
    bcm_mpls_tunnel_initiator_traverse_info_t * additional_info,
    bcm_mpls_tunnel_initiator_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(additional_info, BCM_E_PARAM, "additional_info");
    SHR_NULL_CHECK(cb, BCM_E_PARAM, "bcm_mpls_tunnel_initiator_traverse_cb");

    if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_MPLS_EGRESS_LABEL_IML) &&
        _SHR_IS_FLAG_SET(additional_info->flags, BCM_MPLS_EGRESS_LABEL_EVPN))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Flags element (0x%08X) may only have one of BCM_MPLS_EGRESS_LABEL_IML,BCM_MPLS_EGRESS_LABEL_EVPN (0x%08X) set\r\n",
                     additional_info->flags, (BCM_MPLS_EGRESS_LABEL_IML | BCM_MPLS_EGRESS_LABEL_EVPN));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Traverse all MPLS tunnels in the egress and run a callback function
 *   provided by the user.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] additional_info   -
 *   Pointer to a structure of type bcm_mpls_tunnel_initiator_traverse_info_t.
 *   This structure contains extra info which will, among other things, serve
 *   to help this procedure determine which table to traverse:
 *     Inspect 'additional_info->flags' (constructed per BCM_MPLS_EGRESS_LABEL_*):
 *       If BCM_MPLS_EGRESS_LABEL_IML is set
 *         then table is DBAL_TABLE_EEDB_EVPN and result type is DBAL_RESULT_TYPE_EEDB_EVPN_ETPS_MPLS_1_IML
 *       Else if BCM_MPLS_EGRESS_LABEL_EVPN is set
 *         then table is DBAL_TABLE_EEDB_EVPN and result type is 'any but DBAL_RESULT_TYPE_EEDB_EVPN_ETPS_MPLS_1_IML'
 *       Otherwise,
 *         table is DBAL_TABLE_EEDB_MPLS_TUNNEL and result type is 'any'
 *     Note that only one of BCM_MPLS_EGRESS_LABEL_IML, BCM_MPLS_EGRESS_LABEL_EVPN may be set.
 *   See 'cb' below.
 * \param [in] cb   -
 *   Pointer to a procedure of type bcm_mpls_tunnel_switch_traverse_cb.
 *   This procedure invokes specified callback function for each entry
 *   In one of the corresponding tables:
 *     DBAL_TABLE_EEDB_EVPN
 *     DBAL_TABLE_EEDB_MPLS_TUNNEL
 * \param [in] user_data -
 *   General pointer to user data that will be passed on to the callback
 *   function,'cb, specified above
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *   DBAL_TABLE_EEDB_EVPN
 *   DBAL_TABLE_EEDB_MPLS_TUNNEL
 *   BCM_MPLS_EGRESS_LABEL_*
 * \remark
 *   For all tables, key is 'local outlif'
 */
int
bcm_dnx_mpls_tunnel_initiator_traverse(
    int unit,
    bcm_mpls_tunnel_initiator_traverse_info_t * additional_info,
    bcm_mpls_tunnel_initiator_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 dbal_table;
    uint32 dbal_result_type;
    uint32 flags;
    int is_end;
    dbal_condition_types_e dbal_condition_for_result_type;
    uint32 local_out_lif;
    int core_id;
    bcm_gport_t tunnel_id;
    bcm_if_t intf;
    /*
     * If 'filter_by_result_type' is non-zero then use result_type and condition
     * to filter entries from specified table. Otherwise, get all entries without filtering.
     */
    int filter_by_result_type;
    /*
     * Maximal number of mpls labels correlated with one 'local_out_lif'.
     */
    int label_max;
    bcm_mpls_egress_label_t *label_array_p = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Verification.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_tunnel_initiator_traverse_verify(unit, additional_info, cb, user_data));
    /*
     * Add this just to silence compiler's warning.
     */
    dbal_condition_for_result_type = DBAL_CONDITION_NONE;
    /*
     * All table operations are for all cores.
     */
    core_id = BCM_CORE_ALL;
    tunnel_id = 0;
    label_max = DNX_MPLS_PUSH_MAX_NUM_LABELS;
    flags = additional_info->flags;
    if (flags & BCM_MPLS_EGRESS_LABEL_IML)
    {
        /*
         * IML case.
         */
        dbal_table = DBAL_TABLE_EEDB_EVPN;
        /*
         * Filter by result type: Use enries marked as DBAL_RESULT_TYPE_EEDB_EVPN_ETPS_MPLS_1_IML only.
         */
        filter_by_result_type = TRUE;
        dbal_result_type = DBAL_RESULT_TYPE_EEDB_EVPN_ETPS_MPLS_1_IML;
        dbal_condition_for_result_type = DBAL_CONDITION_EQUAL_TO;
    }
    else if (flags & BCM_MPLS_EGRESS_LABEL_EVPN)
    {
        /*
         * EVPN case.
         */
        dbal_table = DBAL_TABLE_EEDB_EVPN;
        /*
         * Filter by result type: Use any entries but those marked as DBAL_RESULT_TYPE_EEDB_EVPN_ETPS_MPLS_1_IML.
         */
        filter_by_result_type = TRUE;
        dbal_result_type = DBAL_RESULT_TYPE_EEDB_EVPN_ETPS_MPLS_1_IML;
        dbal_condition_for_result_type = DBAL_CONDITION_NOT_EQUAL_TO;
    }
    else
    {
        /*
         * MPLS case.
         */
        dbal_table = DBAL_TABLE_EEDB_MPLS_TUNNEL;
        /*
         * Do not filter entries.
         */
        filter_by_result_type = FALSE;
    }
    /*
     * Iterate over specified table and filter by result type, if required.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    if (filter_by_result_type)
    {
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, dbal_condition_for_result_type,
                         &dbal_result_type, NULL));
    }
    /*
     * Get first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    SHR_ALLOC(label_array_p, sizeof(*label_array_p) * label_max, "label_array_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    while (!is_end)
    {
        int label_count;

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, &local_out_lif));
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                    core_id, local_out_lif, &tunnel_id));
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf, tunnel_id);
        if (intf == _SHR_L3_ITF_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "\r\n"
                         "Failed to convert local_out_lif (0x%08X), via 'tunnel_id' gport (0x%08X), to 'intf' (0x%08X)\r\n"
                         "Internal SW error?\r\n", local_out_lif, tunnel_id, intf);
        }
        /*
         * Get all corresponding MPLS labels.
         */
        SHR_IF_ERR_EXIT(bcm_dnx_mpls_tunnel_initiator_get(unit, intf, label_max, label_array_p, &label_count));
        SHR_IF_ERR_EXIT((*cb) (unit, label_count, label_array_p, user_data));
        /*
         * Get next entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }
exit:
    SHR_FREE(label_array_p);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Create MPLS tunnels in the egress for push or php.
*   \param [in] unit         -  Relevant unit.
*   \param [in] intf         -  Legacy variable - not used, must be set to 0.
*   \param [in] num_labels   - Number of labels to be used.
*   \param [in,out] label_array  -  Array containing 'num_labels' labels to be pushed.
*       PHP flow:
*           label_array.action - PHP (BCM_MPLS_EGRESS_ACTION_PHP), must be set with BCM_MPLS_EGRESS_LABEL_ACTION_VALID flag
*           label_array.label - must be invalid (BCM_MPLS_LABEL_INVALID)
*           label_array.tunnel_id - (in-out parameter) global lif to be used, encoded as BCM_L3_ITF_TYPE_LIF. Must be defined if WITH_ID flag is set.
*           label_array.l3_intf_id - next pointer key (ARP id or MPLS tunnel_id) encoded as BCM_L3_ITF_TYPE_LIF
*           label_array.encap_access - Encapsulation Access stage
*           label_array.qos_map_id - qos remark profile
*           label_array.flags -
*               BCM_MPLS_EGRESS_LABEL_WITH_ID - use 'label_array.tunnel_id' value for global lif allocation
*               BCM_MPLS_EGRESS_LABEL_REPLACE - replace the tunnel parameters related to 'label_array.tunnel_id'
*               BCM_MPLS_EGRESS_LABEL_TTL_SET - configure TTL PIPE mode
*               BCM_MPLS_EGRESS_LABEL_TTL_COPY - configure TTL UNIFORM mode
*               BCM_MPLS_EGRESS_LABEL_EXP_SET - configure EXP PIPE mode
*               BCM_MPLS_EGRESS_LABEL_EXP_COPY - configure EXP UNIFORM mode
*
*       encapsulation (PUSH) flow:
*           label_array.tunnel_id - Global lif to be used. Must be defined if WITH_ID flag is set.
*           label_array.label - MPLS label to be used for the tunnel
*           label_array.egress_qos_model -
*                 QOS model - bcmQosEgressModelUniform, bcmQosEgressModelPipeMyNameSpace, bcmQosEgressModelPipeNextNameSpace, default is uniform
                  TTL model - only support bcmQosEgressModelUniform and bcmQosEgressModelPipeMyNameSpace, default uniform.
*           label_array.ttl - The TTL value used in case 'Pipe' TTL model is set.
*           label_array.exp - The EXP value (for the case of 'Pipe').
*           label_array.qos_map_id - The egress remark QOS profile.
*           label_array.egress_failover_id - Protection pointer created by failover APIs.
*           label_array.egress_failover_if_id - Set to 1 in case this tunnel is the primary one. A value of 0 indicates this is the secondary tunnel.
*           label_array.l3_intf_id - Connect this entry to one of the following next EEDB chain: ARP, IP/MPLS, none.
*           label_array.encap_access - MPLS encap phase to be used
*           label_array.flags -
*               BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED - do not write to GLEM, not supported for EVPN or PHP
*               BCM_MPLS_EGRESS_LABEL_PROTECTION - enable protection
*               BCM_MPLS_EGRESS_LABEL_TTL_SET - Invalid, replaced by egress_qos_model
*               BCM_MPLS_EGRESS_LABEL_TTL_COPY - Invalid, replaced by egress_qos_model
*               BCM_MPLS_EGRESS_LABEL_EXP_SET - Invalid, replaced by egress_qos_model
*               BCM_MPLS_EGRESS_LABEL_EXP_COPY - Invalid, replaced by egress_qos_model
*               BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT - Indicate TTL decrement is done, must be set.
*               BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE - enable entropy label (EL)
*               BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE - entropy label indication (ELI)
*               BCM_MPLS_EGRESS_LABEL_WITH_ID - use 'label_array.tunnel_id' value for global lif allocation.
*               BCM_MPLS_EGRESS_LABEL_REPLACE - update tunnel indicated by 'label_array.tunnel_id'
*               BCM_MPLS_EGRESS_LABEL_TANDEM - indicates that this tunnel can be processed with another tunnel in same encap stage.
*               BCM_MPLS_EGRESS_LABEL_STAT_ENABLE - enable statistics
*               BCM_MPLS_EGRESS_LABEL_EVPN - tunnel is of EVPN type
*               BCM_MPLS_EGRESS_LABEL_IML - tunnel is of IML type
*
* \return
*   Negative in case of an error. See \ref shr_error_e, for example: MPLS label is out of range.
*   Zero in case of NO ERROR
* \remark
*   * It is not possible to set encap_access phase during replace (as it changes EEDB OutLIF location)
*/
int
bcm_dnx_mpls_tunnel_initiator_create(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_ARRAY(unit, label_array, num_labels);

    /*
     * Verification.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_tunnel_initiator_create_verify(unit, intf, num_labels, label_array));

    if (DNX_MPLS_TUNNEL_INIT_IS_EVPN(label_array))
    {
        SHR_IF_ERR_EXIT(dnx_mpls_evpn_encap_add(unit, label_array));
    }
    else if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ACTION_VALID)
             && (label_array[0].action == BCM_MPLS_EGRESS_ACTION_PHP))
    {
        SHR_IF_ERR_EXIT(dnx_mpls_tunnel_initiator_create_php(unit, label_array));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mpls_tunnel_initiator_create_push(unit, num_labels, label_array));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get MPLS tunnels in the egress for push related to intf.
*   \param [in] unit         -  Relevant unit.
*   \param [in] intf         -  Interface.
*   \param [in] label_max   - Maximal number of labels to be retrieved.
*   \param [out] label_array  -  Array to contain retrieved labels.
*   \param [out] label_count  -  Number of retrieved labels.
* \remark
*   full list of 'label_array' parameters is in 'bcm_dnx_mpls_tunnel_initiator_create'
* \return
*   Negative in case of an error. See \ref shr_error_e, for example: MPLS label is out of range.
*   Zero in case of NO ERROR
*/
int
bcm_dnx_mpls_tunnel_initiator_get(
    int unit,
    bcm_if_t intf,
    int label_max,
    bcm_mpls_egress_label_t * label_array,
    int *label_count)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 lif_flags;
    bcm_gport_t gport;
    dnx_mpls_encap_t encap_info;
    int reference_counter = 0;
    bcm_mpls_egress_label_t lookup_label_array[2];
    lif_table_mngr_outlif_info_t lif_info;
    uint8 is_evpn;
    dnx_egr_pointed_t egr_pointed_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verification.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_tunnel_initiator_get_verify(unit, intf, label_max, label_array, label_count));

    /*
     * Check if this is an EVPN/IML egress object
     */
    SHR_IF_ERR_EXIT(dnx_mpls_evpn_egress_label_is_evpn(unit, intf, &gport, &is_evpn));
    /*
     * EVPN/IML objects should be retrieved using mpls_l2vpn module
     */
    if (is_evpn)
    {
        SHR_IF_ERR_EXIT(dnx_mpls_evpn_encap_get(unit, gport, label_array, label_count));
    }
    else
    {
        /*
         * Non-EVPN/IML. Normal MPLS get.
         */
        /*
         * get local and global out lifs
         */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, intf);
        /*
         * At this point, 'gport' has 'type' of 'tunnel'
         */
        egr_pointed_id = DNX_ALGO_EGR_POINTED_INVALID;
        if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(gport))
        {
            /*
             * For EGR_POINTED encoded input (intf), get 'local lif' and
             * 'egr_pointed' ids.
             */
            lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
            egr_pointed_id = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(gport);
        }
        else
        {
            /*
             * For 'all other cases' encoded input (intf), get 'local lif' and
             * 'global lif' ids.
             */
            lif_flags =
                DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
        }
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));

        /** get EEDB entry */
        /*
         * First allocate dbal handle to be filled by lif table manager
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_MPLS_TUNNEL, &entry_handle_id));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                        (unit, gport_hw_resources.local_out_lif, entry_handle_id, &lif_info));
        dnx_mpls_encap_t_init(unit, &encap_info);
        bcm_mpls_egress_label_t_init(&lookup_label_array[0]);
        bcm_mpls_egress_label_t_init(&lookup_label_array[1]);
        encap_info.label_array = lookup_label_array;
        SHR_IF_ERR_EXIT(dnx_mpls_lif_table_manager_to_encap_info(unit, entry_handle_id, &lif_info, &encap_info));
        if (egr_pointed_id != DNX_ALGO_EGR_POINTED_INVALID)
        {
            /*
             * If input 'intf' is of subtype 'EGRESS_POINTED' then set the corresponding flag, as output.
             */
            encap_info.label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED;
        }
        *label_count = encap_info.labels_nof;

        if (encap_info.label_array[0].egress_failover_id != dnx_data_failover.path_select.egr_no_protection_get(unit))
        {
            DNX_FAILOVER_SET(encap_info.label_array[0].egress_failover_id,
                             encap_info.label_array[0].egress_failover_id, DNX_FAILOVER_TYPE_ENCAP);
            encap_info.label_array[0].egress_failover_if_id = encap_info.label_array[0].egress_failover_if_id ? 0 : 1;
            encap_info.label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_PROTECTION;
            if (encap_info.labels_nof > 1)
            {
                encap_info.label_array[1].egress_failover_id = encap_info.label_array[0].egress_failover_id;
                encap_info.label_array[1].egress_failover_if_id = encap_info.label_array[0].egress_failover_if_id;
                encap_info.label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_PROTECTION;
            }
        }

        if (encap_info.label_array[0].action != BCM_MPLS_EGRESS_ACTION_PHP)
        {
            if (encap_info.label_array[0].egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace)
            {
                int ttl;
                SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.profile_data_get
                                (unit, encap_info.ttl_pipe_profile, &reference_counter, &ttl));
                encap_info.label_array[0].ttl = ttl;
                if (encap_info.labels_nof > 1)
                {
                    encap_info.label_array[1].ttl = encap_info.label_array[0].ttl;
                }
            }

            /*
             * Retrieve exp value from QOS table
             * nwk_pipe and nwk_pipe_mapped are 1:1 mapping (equal)
             */
            if ((encap_info.label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace) ||
                (encap_info.label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace))
            {
                uint8 nwk_pipe, nwk_pipe_mapped, dp_pipe_mapped;
                SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_attr_get(unit, encap_info.nwk_qos_id,
                                                                                 &nwk_pipe, &nwk_pipe_mapped,
                                                                                 &dp_pipe_mapped));
                encap_info.label_array[0].exp = DNX_QOS_IDX_NWK_QOS_GET_EXP0(nwk_pipe);
                if (encap_info.labels_nof > 1)
                {
                    encap_info.label_array[1].exp = DNX_QOS_IDX_NWK_QOS_GET_EXP1(nwk_pipe);
                }
            }
            else if (_SHR_IS_FLAG_SET(label_array[0].flags, BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING))
            {
                /*
                 * For Alternate Marking set exp to 0 and extrace LOSS bit
                 */
                uint8 nwk_pipe, nwk_pipe_mapped, dp_pipe_mapped;
                SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_attr_get(unit, encap_info.nwk_qos_id,
                                                                                 &nwk_pipe, &nwk_pipe_mapped,
                                                                                 &dp_pipe_mapped));
                encap_info.label_array[0].exp = 0;
                encap_info.label_array[1].exp = DNX_QOS_IDX_NWK_QOS_GET_EXP1(nwk_pipe);
                if ((DNX_QOS_IDX_NWK_QOS_GET_EXP0(nwk_pipe) & 0x4) == 0x4)
                {
                    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING_LOSS_SET;
                }
            }
        }

        /** configure return struct */
        sal_memcpy(&label_array[0], &lookup_label_array[0], sizeof(bcm_mpls_egress_label_t));

        SHR_IF_ERR_EXIT(dnx_mpls_tunnel_initiator_outlif_phase_to_encap_access
                        (unit, gport_hw_resources.outlif_phase, &label_array[0].encap_access));

        if (gport_hw_resources.global_out_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            /*
             * 'global out lif' is valid (non-egr_pointed case)
             */
            BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, gport_hw_resources.global_out_lif);
        }
        else
        {
            /*
             * 'global out lif' is invalid ('egr_pointed' case)
             */
            int l3_itf_dummy;

            BCM_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(l3_itf_dummy, egr_pointed_id);
            BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, l3_itf_dummy);
        }
        if ((encap_info.labels_nof > 1) && (label_max >= DNX_MPLS_TUNNEL_ENCAP_TWO_LABELS))
        {
            sal_memcpy(&label_array[1], &lookup_label_array[1], sizeof(bcm_mpls_egress_label_t));
            label_array[1].tunnel_id = label_array[0].tunnel_id;
            label_array[1].action = label_array[0].action;
            label_array[1].encap_access = label_array[0].encap_access;
        }

        /*
         * Get ESEM information
         */
        if (encap_info.esem_command_profile != dnx_data_esem.access_cmd.no_action_get(unit))
        {
            uint8 svtag_present;
            dnx_esem_cmd_data_t esem_cmd_data;

            /** Check if an SVTAG lookup is present */
            SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_sw_get(unit, encap_info.esem_command_profile, &esem_cmd_data, NULL));
            SHR_IF_ERR_EXIT(dnx_switch_svtag_is_present_in_esem_cmd(unit, &esem_cmd_data, &svtag_present));
            label_array[0].flags2 |= (svtag_present * BCM_MPLS_EGRESS_LABEL_FLAGS2_SVTAG_ENABLE);
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete MPLS tunnels in the egress for push related to intf.
*   \param [in] unit         -  Relevant unit.
*   \param [in] intf         -  Interface.
* \return
*   Negative in case of an error. See \ref shr_error_e, for example: MPLS label is out of range.
*   Zero in case of NO ERROR
*/
int
bcm_dnx_mpls_tunnel_initiator_clear(
    int unit,
    bcm_if_t intf)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 lif_flags;
    bcm_gport_t gport;
    int new_out_lif_profile;
    etpp_out_lif_profile_info_t out_lif_profile_info;
    dnx_mpls_encap_t encap_info;
    uint8 ttl_last_reference = 0;
    uint8 qos_last_reference = 0;
    uint8 is_evpn;
    dnx_egr_pointed_t egr_pointed_id;
    uint32 entry_handle_id;
    uint8 is_last = 0;

    bcm_mpls_egress_label_t label_array[2];
    lif_table_mngr_outlif_info_t lif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Verification.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_tunnel_initiator_clear_verify(unit, intf));
    /*
     * Check if the deleted label is EVPN
     */
    SHR_IF_ERR_EXIT(dnx_mpls_evpn_egress_label_is_evpn(unit, intf, &gport, &is_evpn));
    if (is_evpn)
    {
        /*
         * Let mpls_l2vpn handle the deletion of EVPN/IML
         */
        SHR_IF_ERR_EXIT(dnx_mpls_evpn_encap_delete(unit, gport));
    }
    else
    {
        /*
         * Normal egress label deletion
         */
        int is_egr_pointed;
        /*
         * get local lif. Get global out lif or 'egr_pointed' object
         */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, intf);
        /*
         * At this point, 'gport' has 'type' of 'tunnel'
         */
        egr_pointed_id = DNX_ALGO_EGR_POINTED_INVALID;
        if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(gport))
        {
            /*
             * For EGR_POINTED encoded input (label_array[0].tunnel_id), get 'local lif' and
             * 'egr_pointed' ids.
             */
            is_egr_pointed = TRUE;
            lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
            egr_pointed_id = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(gport);
        }
        else
        {
            /*
             * For 'all other cases' encoded input (label_array[0].tunnel_id), get 'local lif' and
             * 'global lif' ids.
             */
            is_egr_pointed = FALSE;
            lif_flags =
                DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
        }
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));
        /*
         * Get MPLS encap info
         */
        /** get EEDB entry */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_MPLS_TUNNEL, &entry_handle_id));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                        (unit, gport_hw_resources.local_out_lif, entry_handle_id, &lif_info));
        dnx_mpls_encap_t_init(unit, &encap_info);
        bcm_mpls_egress_label_t_init(&label_array[0]);
        bcm_mpls_egress_label_t_init(&label_array[1]);
        encap_info.label_array = label_array;
        SHR_IF_ERR_EXIT(dnx_mpls_lif_table_manager_to_encap_info(unit, entry_handle_id, &lif_info, &encap_info));

        /*
         * Clear SW tables
         */
        if (label_array[0].action != BCM_MPLS_EGRESS_ACTION_PHP)
        {
            if (encap_info.label_array[0].egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace)
            {
                SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.free_single(unit, encap_info.ttl_pipe_profile,
                                                                                    &ttl_last_reference));
            }
            if ((encap_info.label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace) ||
                (encap_info.label_array[0].egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace))
            {
                SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_free
                                (unit, encap_info.nwk_qos_id, &qos_last_reference));
            }
        }
        /*
         * Free outlif profile:
         * It is done by calling exchange function with default profile data:
         */
        etpp_out_lif_profile_info_t_init(unit, &out_lif_profile_info);
        SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                        (unit, DBAL_TABLE_EEDB_MPLS_TUNNEL, 0, &out_lif_profile_info, encap_info.out_lif_profile,
                         &new_out_lif_profile, &is_last));
        if (new_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Somthing is wrong! on freeing oulif_profile, didn't get default out lif profile, default outlif profile = %d, new_out_lif_profile = %d",
                         DNX_OUT_LIF_PROFILE_DEFAULT, new_out_lif_profile);
        }
        /*
         * Clear HW tables
         */
        /*
         * For GLEM, See dnx_lif_table_mngr_outlif_info_clear() below.
         */
        /*
         * clear from HW the TTL profile
         */
        if (ttl_last_reference)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_clear(unit, encap_info.ttl_pipe_profile));
        }
        if (qos_last_reference)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_hw_clear(unit, encap_info.nwk_qos_id));
        }
        /*
         * Free Lif table and local LIF allocation
         */
        if (is_egr_pointed == TRUE)
        {
            /*
             * For EGR_POINTED encoded input (label_array[0].tunnel_id)
             */
            uint32 alloc_flags = 0;
            int element;
            lif_flags = LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;
            /*
             * Now remove table entries mapping between 'egr_pointed' and 'local_lif'
             */
            SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_local_lif_mapping_remove(unit, alloc_flags, egr_pointed_id));
            /*
             * Release (deallocate) this 'egr_pointed' object.
             */
            element = egr_pointed_id;
            SHR_IF_ERR_EXIT(algo_l3_db.egr_pointed_alloc.egr_pointed_res_manager.free_single(unit, element));
        }
        else
        {
            lif_flags = 0;
            /** delete global lif */
            SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free
                            (unit, DNX_ALGO_LIF_EGRESS, gport_hw_resources.global_out_lif));
        }

        if (encap_info.esem_command_profile != dnx_data_esem.access_cmd.no_action_get(unit))
        {
            uint8 remove_esem_cmd_profile;

            SHR_IF_ERR_EXIT(dnx_port_esem_cmd_free(unit, encap_info.esem_command_profile, &remove_esem_cmd_profile));

            /** clear from HW the esem cmd profile */
            if (remove_esem_cmd_profile)
            {
                SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_clear(unit, encap_info.esem_command_profile));
            }
        }

        /*
         * Note that, for non 'egr_pointed' objects, procedure dnx_lif_lib_remove_from_glem() is called from
         * within dnx_lif_table_mngr_outlif_info_clear() thus deleting global to local mapping from GLEM
         * For 'egr_pointed' objects, GLEM is not accessed because the LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLEM
         * flag is set.
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_outlif_info_clear(unit, gport_hw_resources.local_out_lif, lif_flags));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Delete all MPLS tunnels in the egress for push related to intf.
 *
 * \param [in] unit       -  device unit number.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 * \remark
 *   None.
 * \see
 *   * None
 */
int
bcm_dnx_mpls_tunnel_initiator_clear_all(
    int unit)
{

    const dbal_tables_e eedb_tables[] = {
        DBAL_TABLE_EEDB_MPLS_TUNNEL,
        DBAL_TABLE_EEDB_EVPN
    };
    int table_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    for (table_idx = 0; table_idx < (sizeof(eedb_tables) / sizeof(dbal_tables_e)); table_idx++)
    {
        bcm_gport_t tunnel_id = 0;
        uint32 entry_handle_id;
        int is_end;
        uint32 local_out_lif[1];
        bcm_if_t intf;
        int core_id = _SHR_CORE_ALL;
        /*
         * Free outlif profile
         * Delete mpls labels
         * Delete global and local out lifs
         * Delete global to local mapping from GLEM
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, eedb_tables[table_idx], &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
        /*
         * Receive first entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif));
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                        core_id, local_out_lif[0], &tunnel_id));
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf, tunnel_id);
            SHR_IF_ERR_EXIT(bcm_dnx_mpls_tunnel_initiator_clear(unit, intf));
            /*
             * Receive next entry in table.
             */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        DBAL_HANDLE_FREE(unit, entry_handle_id);
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * End of APIs
 * }
 */
