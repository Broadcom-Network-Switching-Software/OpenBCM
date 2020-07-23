/** \file l3_arp.c
 *  
 *  l3 arp procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3
/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/l3/l3_arp.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <shared/util.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */

/**
 * \brief This gets the bottom 16 bits of src_mac.
 */
#define L3_ARP_GET_SRC_MAC_ADDR_LSB(src_mac) ((src_mac[4] << SAL_UINT8_NOF_BITS) | src_mac[5])

/**
 * \brief This sets the bottom 16 bits of src_mac according to the dual_homing.
 * src_mac serves both as input and output
 */
#define L3_ARP_SET_SRC_MAC_ADDR_LSB(src_mac, dual_homing) \
    src_mac[4] = (dual_homing >> SAL_UINT8_NOF_BITS) & 0xFF; \
    src_mac[5] = (dual_homing & 0xFF);

/*
 * In case of full mac, we add '1' to the source index LSB as indication in EEDB HW.
 * This bit is not part of the address, thus not part of the allocation.
 */
#define SOURCE_ADDRESS_FULL_MASK(profile)   profile = profile | 1;
#define SOURCE_ADDRESS_FULL_UNMASK(profile)  profile = profile & 0xFFFFFFFE;
/*
 * }
 */

/**
* \brief
*   Verify function for bcm_l3_egress_create with BCM_L3_EGRESS_ONLY flag.
*
*  \param [in] unit -Relevant unit.
*  \param [in] flags - Similar to bcm_l3_egress_create api input
*  \param [in] egr - Similar to bcm_l3_egress_create api input
*  \return 
*       shr_error_e - Error return value
 */
static shr_error_e
dnx_l3_egress_create_arp_verify(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check BCM_L3_FLAGS2_VLAN_TRANSLATION and BCM_L3_FLAGS2_SRC_MAC can't be set together
     */
    if (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VLAN_TRANSLATION)
        && _SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_SRC_MAC))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_L3_FLAGS2_VLAN_TRANSLATION and BCM_L3_FLAGS2_SRC_MAC can't be set together\r\n");
    }
    /*
     * egr->vlan has to be set when using flag BCM_L3_FLAGS2_VLAN_TRANSLATION in order to get VSI from ARP+AC
     */
    if (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VLAN_TRANSLATION) && !(egr->vlan))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "egr->vlan has to be set when using flag BCM_L3_FLAGS2_VLAN_TRANSLATION in order to get VSI from ARP+AC\r\n");
    }
    /*
     * egr->vlan has to be set when using flag BCM_L3_FLAGS2_SRC_MAC in order to get VSI from ARP
     */
    if (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_SRC_MAC) && !(egr->vlan))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "egr->vlan has to be set when using flag BCM_L3_FLAGS2_SRC_MAC in order to get VSI from ARP\r\n");
    }

    /** verify simgle result type requested */
    if (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VLAN_TRANSLATION)
        && _SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_SRC_MAC))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_L3_FLAGS2_VLAN_TRANSLATION can not be set with BCM_L3_FLAGS2_SRC_MAC");
    }

    /** 2 tag vlan translation can be done only if BCM_L3_FLAGS2_VLAN_TRANSLATION is set */
    if (!_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VLAN_TRANSLATION)
        && _SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS can be only set with BCM_L3_FLAGS2_VLAN_TRANSLATION");
    }

    /** arp does not support ecn*/
    if (egr->egress_qos_model.egress_ecn == bcmQosEgressEcnModelEligible)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "arp does not support ecn!");
    }
    if (!(_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VLAN_TRANSLATION)) &&
        (egr->egress_qos_model.egress_qos != bcmQosEgressModelUniform))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "enacp qos model can only be set for ARP+AC!");
    }
    if ((_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_EGRESS_STAT_ENABLE)) &&
        (egr->egress_qos_model.egress_qos != bcmQosEgressModelUniform))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "enacp qos model can not be set for stat ARP AC!");
    }
    if (egr->egress_qos_model.egress_ttl != bcmQosEgressModelUniform)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ttl model can not be set");
    }
    if ((egr->egress_qos_model.egress_qos == bcmQosEgressModelUniform) &&
        (egr->mpls_pkt_pri != 0 || egr->mpls_pkt_cfi != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mpls_pkt_pri and mpls_pkt_cfi must be 0 for uniform model");
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE))
    {
        if (egr->encap_id == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "configuring ARP with BCM_L3_REPLACE must be called with encap_id != 0");
        }
        /*
         * verify virtual egress pointed is not replaced with non virtual
         */
        if (BCM_L3_ITF_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(egr->encap_id) &&
            !_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "EGRESS_POINTED interface can be only replaced with flag BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED");
        }
        if (!BCM_L3_ITF_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(egr->encap_id) &&
            _SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "non EGRESS_POINTED interface can be only replaced with flag BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED unset");
        }
    }

    if ((egr->intf != 0) && !BCM_L3_ITF_TYPE_IS_LIF(egr->intf))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_PARAM,
                          "egr->intf 0x%08X represents here the tunnel that is pointing on ARP entry and if valid must be of type LIF.\r\n%s%s",
                          egr->intf, EMPTY, EMPTY);
    }
    else if (egr->intf != 0)
    {
        /*
         * Check that tunnel exists
         */
        bcm_gport_t tunnel_gport;
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        shr_error_e rv;

        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(tunnel_gport, egr->intf);
        rv = dnx_algo_gpm_gport_to_hw_resources(unit, tunnel_gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                &gport_hw_resources);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "egr->intf 0x%08X does not exist.\r\n%s%s", egr->intf, EMPTY, EMPTY);
        }
    }
    /*
     * encap_id with BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN flags signalizes only tunnel next pointer should be updated
     */
    if (((_SHR_IS_FLAG_SET(flags, BCM_L3_KEEP_DSTMAC) && !_SHR_IS_FLAG_SET(flags, BCM_L3_KEEP_VLAN)) ||
         (_SHR_IS_FLAG_SET(flags, BCM_L3_KEEP_VLAN) && !_SHR_IS_FLAG_SET(flags, BCM_L3_KEEP_DSTMAC))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags BCM_L3_KEEP_DSTMAC|BCM_L3_KEEP_VLAN must be used together.\r\n");
    }
    if ((egr->encap_id == 0)
        && (flags & (BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN)) == (BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags BCM_L3_KEEP_DSTMAC|BCM_L3_KEEP_VLAN must be used with valid encap_id.\r\n");
    }
    if ((egr->stat_id != 0) || (egr->stat_pp_profile != STAT_PP_PROFILE_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "stat_id and stat_pp_profile parameters are only relevant for FEC configuration.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   provides the correct eedb phase for the lif allocation.
*
*    \param [in] flags - Similar to bcm_l3_egress_create api input
*    \param [in] flags2 - Similar to bcm_l3_egress_create api input
*  \return 
*    lif_mngr_outlif_phase_e - phase to be used
 */
lif_mngr_outlif_phase_e
dnx_l3_egress_arp_outlif_phase_get(
    uint32 flags,
    uint32 flags2)
{
    if (!_SHR_IS_FLAG_SET(flags, BCM_L3_NATIVE_ENCAP) && !_SHR_IS_FLAG_SET(flags2, BCM_L3_FLAGS2_VLAN_TRANSLATION))
    {
        return LIF_MNGR_OUTLIF_PHASE_ARP;
    }
    else if (_SHR_IS_FLAG_SET(flags, BCM_L3_NATIVE_ENCAP) && !_SHR_IS_FLAG_SET(flags2, BCM_L3_FLAGS2_VLAN_TRANSLATION))
    {
        return LIF_MNGR_OUTLIF_PHASE_NATIVE_ARP;
    }
    else if (!_SHR_IS_FLAG_SET(flags, BCM_L3_NATIVE_ENCAP) && _SHR_IS_FLAG_SET(flags2, BCM_L3_FLAGS2_VLAN_TRANSLATION))
    {
        return LIF_MNGR_OUTLIF_PHASE_AC;
    }
    else
    {
        return LIF_MNGR_OUTLIF_PHASE_NATIVE_ARP;
    }
}

/**
* \brief
*   Allocation function for bcm_l3_egress_create with BCM_L3_EGRESS_ONLY flag.
*   Allocats global and local lif.
*   Allocated index will be filled inside encap_id parameter.
*   Also ll_local_lif, ll_global_lif store the allocation result.
*
* \param [in] unit -Relevant unit.
* \param [in] flags - Similar to bcm_l3_egress_create api input
* \param [in] egr - Similar to bcm_l3_egress_create api input
* \param [out] ll_local_lif - Local lif id of existing ARP entry, only in case of replace.
* \param [out] ll_global_lif -
*   Pointer to int. Handling of this variable depends on 'egr->flags2'
*   and on 'egr->encap_id' as follows:
*     If neither BCM_L3_REPLACE is set nor (egr->encap_id != 0) {The latter is equivalent to 'with id'}
*       If BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED is set (on 'egr->flags2')
*         This procedure loads pointed memory by DNX_ALGO_GPM_LIF_INVALID.
*       else
*         This procedure loads pointed memory by global lif id that was allocated. If
*         allocation has failed, _SHR_E_NOT_FOUND is returned and pointed memory is
*         loaded by DNX_ALGO_GPM_LIF_INVALID
*     else
*       If 'egr->encap_id' is NOT encoded as 'LIF' then encode it as such. Use sub_type value
*       as base.
*       If 'egr->encap_id' is encoded as 'egr_pointed' (See _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED)
*       or BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED is set (on 'egr->flags2')
*         then This procedure loads pointed memory by DNX_ALGO_GPM_LIF_INVALID.
*       else This procedure loads pointed memory by global lif id that is either derived from
*         'egr->encap_id' (for 'replace' case) or as allocated. For 'with id',
*         value to allocate is taken from 'egr->encap_id'
* \param [out] egr_pointed_id -
*   Pointer to dnx_egr_pointed_t. handling of this variable depends on 'egr->flags2'
*   and on 'egr->encap_id' as follows:
*     If neither BCM_L3_REPLACE is set nor (egr->encap_id != 0) {The latter is equivalent to 'with id'}
*       If BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED is set (on 'egr->flags2')
*         This procedure loads pointed memory by id of newly allocated 'virtual egress pointed'
*         object. If no such object could be allocated, an _SHR_E_NOT_FOUND is returned (but
*         no error log is ejected) plus pointed memory is loaded by DNX_ALGO_EGR_POINTED_INVALID.
*       else
*         This procedure loads pointed memory by DNX_ALGO_EGR_POINTED_INVALID.
*     else
*       If 'egr->encap_id' is NOT encoded as 'LIF' then encode it as such. Use sub_type value
*       as base.
*       If 'egr->encap_id' is neither encoded as 'egr_pointed' (See BCM_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED)
*       nor is BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED set
*         then This procedure loads pointed memory by DNX_ALGO_EGR_POINTED_INVALID.
*       else This procedure loads pointed memory by 'egr_pointed' id that is derived from
*         'egr->encap_id' (for 'replace' case) or as allocated. For 'with id', value to
*         allocate is taken from 'egr->encap_id'
*  \return 
*    shr_error_e - Error return value
 */
static shr_error_e
dnx_l3_egress_create_arp_allocate(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr,
    int *ll_local_lif,
    int *ll_global_lif,
    dnx_egr_pointed_t * egr_pointed_id)
{
    uint32 lif_flags;
    int this_is_egr_pointed_case;
    int with_id;

    SHR_FUNC_INIT_VARS(unit);
    if (egr->encap_id != 0)
    {
        with_id = TRUE;
    }
    else
    {
        with_id = FALSE;
    }
    this_is_egr_pointed_case = FALSE;
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE) || (with_id == TRUE))
    {
        /*
         * In 'replace' and 'with_id'  cases, input (egr->encap_id) must be
         * encoded as 'interface'
         * If it is not, make it.
         */
        if (!(BCM_L3_ITF_TYPE_IS_LIF(egr->encap_id)))
        {
            int l3_itf_dummy;

            l3_itf_dummy = BCM_L3_ITF_SUB_TYPE_VALUE_GET(egr->encap_id);
            BCM_L3_ITF_SET(egr->encap_id, BCM_L3_ITF_TYPE_LIF, l3_itf_dummy);
        }
        if (BCM_L3_ITF_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(egr->encap_id) ||
            (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED)))
        {
            /*
             * If 'encap_id' is encoded as 'egr_pointed', then update 'this_is_egr_pointed_case'
             */
            this_is_egr_pointed_case = TRUE;
        }
    }
    else
    {
        if (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED))
        {
            /*
             * If 'egr_pointed' flag is set, update 'this_is_egr_pointed_case'
             */
            this_is_egr_pointed_case = TRUE;
        }
    }
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE))
    {
        /*
         * Here handle the 'replace' case. Encoded interface is assumed to have been loaded
         * into 'egr->encap_id'
         */
        dnx_algo_gpm_gport_hw_resources_t hw_resources;
        bcm_gport_t gport;

        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, egr->encap_id);
        /*
         * At this point, 'gport' has 'type' of 'tunnel'
         */
        *egr_pointed_id = DNX_ALGO_EGR_POINTED_INVALID;
        if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(gport))
        {
            /*
             * For EGR_POINTED encoded input (egr->encap_id), get 'local lif' and
             * 'egr_pointed' ids.
             */
            *egr_pointed_id = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(gport);
            lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        }
        else
        {
            /*
             * For 'all other cases' of encoded input (egr->encap_id), get 'local lif' and
             * 'global lif' ids.
             */
            lif_flags =
                DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
        }
        sal_memset(&hw_resources, 0, sizeof(hw_resources));
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &hw_resources));
        /*
         * Note that 'invalid' values (e.g., for 'global_out_lif') are acceptable output.
         */
        *ll_local_lif = hw_resources.local_out_lif;
        *ll_global_lif = hw_resources.global_out_lif;
    }
    else
    {
        lif_flags = 0;
        /*
         * Choose entry size according to vlan parameter
         */
        if (!this_is_egr_pointed_case)
        {
            /*
             * If 'egr_pointed' is NOT the case here then handle 'global lif'
             */
            *egr_pointed_id = DNX_ALGO_EGR_POINTED_INVALID;
            /*
             * Allocation with ID
             */
            if (with_id == TRUE)
            {
                *ll_global_lif = BCM_L3_ITF_VAL_GET(egr->encap_id);
                lif_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
            }
            SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                            (unit, lif_flags, DNX_ALGO_LIF_EGRESS, ll_global_lif));
        }
        else
        {
            /*
             * If 'egr_pointed' IS the case here then set 'global lif' as 'invalid' and
             * handle 'egr_pointed'.
             */
            uint32 alloc_flags;
            int element;

            if (with_id == TRUE)
            {
                bcm_if_t intf;

                /*
                 * Since caller is not required to encode 'egr->encap_id' in the 'with_id' case
                 * then we do it here, to make sure, before we use the standard macro to get the 'egr_pointed' value.
                 * Note that we could simply apply 'and' with _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_MASK
                 */
                BCM_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(intf, egr->encap_id);
                *egr_pointed_id = BCM_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(intf);
                /*
                 * Allocate resource corresponding to id specified on input.
                 */
                element = *egr_pointed_id;
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
                *egr_pointed_id = element;
            }
            /*
             * Global lif is invalid.
             */
            *ll_global_lif = DNX_ALGO_GPM_LIF_INVALID;
        }
    }
    /*
     * Just making sure: At this point, one of 'egr_pointed' and 'global_lif' (and only one!)
     * should be valid.
     */
    if ((*egr_pointed_id == DNX_ALGO_EGR_POINTED_INVALID) && (*ll_global_lif == DNX_ALGO_GPM_LIF_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "\r\n"
                     "Both 'egr_pointed' and 'global_lif' are marked as 'invalid' - Probably internal sw error.\r\n");
    }
    else if ((*egr_pointed_id != DNX_ALGO_EGR_POINTED_INVALID) && (*ll_global_lif != DNX_ALGO_GPM_LIF_INVALID))
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
*    Egress ARP QOS model and pipe profile profile update.
*
*    \param [in] unit - Relevant unit.
*    \param [in] egr - Similar to bcm_l3_egress_create api input
*    \param [in] old_qos_index - old nwk_qos index if replace, default -1
*    \param [out] qos_index - network_qos_index for pipe model
*    \param [out] is_first  - if qos_index is first allocated
*    \param [out] is_last   - if qos_index is last for free
*  \return 
*    shr_error_e - Error return value
 */
static shr_error_e
dnx_l3_egress_arp_qos_calculate(
    int unit,
    bcm_l3_egress_t * egr,
    int old_qos_index,
    int *qos_index,
    uint8 *is_first,
    uint8 *is_last)
{
    uint8 nwk_qos = 0;
    uint8 dp_mapped = 0;
    uint8 new_is_pipe;

    SHR_FUNC_INIT_VARS(unit);

    *is_last = FALSE;
    *is_first = FALSE;
    *qos_index = 0;

    new_is_pipe = ((egr->egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace) ||
                   (egr->egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace)) ? TRUE : FALSE;

    if (new_is_pipe && old_qos_index != DNX_QOS_INVALID_MAP_ID)
    {
        /**update qos index with new pkt_pri and pkt_cfi*/
        nwk_qos = DNX_QOS_IDX_PCP_DEI_TO_NWK_QOS(egr->mpls_pkt_pri, egr->mpls_pkt_cfi);
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_update
                        (unit, nwk_qos, nwk_qos, 0, &old_qos_index, is_first, is_last));
        *qos_index = old_qos_index;
    }
    else if (new_is_pipe)
    {
        nwk_qos = DNX_QOS_IDX_PCP_DEI_TO_NWK_QOS(egr->mpls_pkt_pri, egr->mpls_pkt_cfi);
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_allocate(unit,
                                                                         nwk_qos, nwk_qos, dp_mapped, qos_index,
                                                                         is_first));
    }
    else if (old_qos_index != DNX_QOS_INVALID_MAP_ID)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_free(unit, old_qos_index, is_last));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   HW tables write function for bcm_l3_egress_create with BCM_L3_EGRESS_ONLY flag.
*   Fills ARP EEDB entry with index ll_local_lif.
*
*    \param [in] unit - Relevant unit.
*    \param [in] flags - Similar to bcm_l3_egress_create api input
*    \param [in] egr - Similar to bcm_l3_egress_create api input
     \param [in] source_idx - Source index
*    \param [in] ll_local_lif - Local lif id allocated for ARP
*    \param [in] ll_global_lif - ARP Global lif
*    \param [in] qos_index - network qos index for pipe qos model
*  \return 
*    shr_error_e - Error return value
 */
static shr_error_e
dnx_l3_egress_create_arp_hw_write(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr,
    uint32 source_idx,
    uint32 *ll_local_lif,
    uint32 ll_global_lif,
    int qos_index)
{
    uint32 entry_handle_id = 0;
    lif_table_mngr_outlif_info_t outlif_info;
    dbal_enum_value_field_encap_qos_model_e qos_model;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&outlif_info, 0, sizeof(lif_table_mngr_outlif_info_t));

    outlif_info.outlif_phase = dnx_l3_egress_arp_outlif_phase_get(egr->flags, egr->flags2);
    outlif_info.global_lif = ll_global_lif;
    if (ll_global_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;
    }

    /*
     * Fill lif table manager handle id with all required fields
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_ARP, &entry_handle_id));
    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    if (egr->vlan != 0)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, egr->vlan);
    }
    if (egr->qos_map_id != 0)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE,
                                     DNX_QOS_MAP_PROFILE_GET(egr->qos_map_id));
    }
    if (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VLAN_TRANSLATION))
    {
        /** This is indication for lif table manager that vlan translation is required */
        outlif_info.table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_VLAN_TRANSLATION;
        if (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS))
        {
            /** Need to do 2 VIDs */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE, 0);
        }
        /** Set EGRESS_LAST_LAYER - indicates outer Ethernet layer */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE,
                                     !_SHR_IS_FLAG_SET(egr->flags, BCM_L3_NATIVE_ENCAP));
        if (!_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_EGRESS_STAT_ENABLE))
        {
            /**set qos model*/
            SHR_IF_ERR_EXIT(dnx_qos_egress_model_bcm_to_dbal(unit, &egr->egress_qos_model, &qos_model));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE, qos_model);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, qos_index);
        }
    }
    if (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_SRC_MAC))
    {
        if (dnx_data_l3.source_address.custom_sa_use_dual_homing_get(unit))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DUAL_HOMING, INST_SINGLE,
                                         L3_ARP_GET_SRC_MAC_ADDR_LSB(egr->src_mac_addr));
        }
        else
        {
            /*
             * source address type full mac has special masking
             */
            SOURCE_ADDRESS_FULL_MASK(source_idx);
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, source_idx);
    }

    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, INST_SINGLE, egr->mac_addr);

    STAT_PP_CHECK_AND_SET_STAT_OBJECT_LIF_FIELDS(egr->flags2, BCM_L3_FLAGS2_EGRESS_STAT_ENABLE);

    /*
     * Fill next pointer
     */
    if (egr->vlan_port_id != BCM_GPORT_INVALID)
    {
        /*
         * Get local lif from Gport
         */
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, egr->vlan_port_id,
                                                                       DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                                       &gport_hw_resources), _SHR_E_NOT_FOUND,
                                    _SHR_E_PARAM);
        /*
         * Fill HW value
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                     gport_hw_resources.local_out_lif);
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE))
    {
        outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    }
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
                    (unit, entry_handle_id, (int *) ll_local_lif, &outlif_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Get src mac prefix by out lif.
*   \param [in] unit    -  Relevant unit.
*   \param [in] local_out_lif   -  pointer to EEDB entry.
*   \param [out] src_mac_prefix   -  src mac prefix.
*   \param [out] nwk_qos_idx   -  nwk_qos index for pipe qos model.
* \return
*   shr_error_e - Non-zero in case of an error.
*/
static shr_error_e
dnx_l3_egress_src_mac_prefix_and_qos_index_get(
    int unit,
    int local_out_lif,
    int *src_mac_prefix,
    int *nwk_qos_idx)
{
    uint32 entry_handle_id;
    lif_table_mngr_outlif_info_t lif_info;
    uint8 is_field_valid;
    uint32 qos_model;
    uint32 old_qos_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_ARP, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info(unit, local_out_lif, entry_handle_id, &lif_info));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, &is_field_valid, (uint32 *) src_mac_prefix));
    if (is_field_valid == TRUE)
    {
        /*
         * source address type full mac has special masking, unmasking can be done on all types
         */
        SOURCE_ADDRESS_FULL_UNMASK(*src_mac_prefix);
    }
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, &is_field_valid, &qos_model));
    if ((is_field_valid == TRUE) &&
        ((qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE) ||
         (qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE)))
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, &is_field_valid, &old_qos_idx));
        *nwk_qos_idx = (int) old_qos_idx;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Delete arp eedb table.
*   \param [in] unit    -  Relevant unit.
*   \param [in] global_out_lif   -
*     global LIF ID. Used to remove entry from GLEM.
*     If it is DNX_ALGO_GPM_LIF_INVALID then do not access GLEM
*     at all.
*   \param [in] local_out_lif   -  pointer to EEDB entry.
* \return
*   shr_error_e - Non-zero in case of an error.
*
* \remark
*
* \see
 */
static shr_error_e
dnx_l3_egress_destroy_arp_hw_clear(
    int unit,
    int global_out_lif,
    int local_out_lif)
{
    uint32 flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (global_out_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        /** Dont update the GLEM. **/
        flags |= LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;
    }

    /** Free Lif table and local LIF allocation */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_outlif_info_clear(unit, local_out_lif, flags));
exit:
    SHR_FUNC_EXIT;
}

/*See l3_arp.h for prototype define*/
shr_error_e
dnx_l3_egress_arp_info_get(
    int unit,
    bcm_if_t intf,
    bcm_l3_egress_t * egr)
{
    uint32 entry_handle_id = 0;
    uint8 remark_profile = 0;
    uint32 vsi;
    uint32 gport;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 lif_flags;
    lif_table_mngr_outlif_info_t outlif_info;
    uint32 src_mac_addr_msb_idx;
    uint8 is_field_valid;
    uint32 next_pointer = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Input 'intf' is assumed to be encoded as 'lif'.
     * Caller is assumed to have taken care of that.
     */
    /*
     * Within this macro, verify sub_type encoding. It may only be 'default' or 'egr_pointed'
     */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, intf);
    /*
     * For EGR_POINTED encoded input (intf), get 'local lif' using
     * 'egr_pointed' id.
     * For 'all other cases', get 'local lif' using
     * 'global lif' id.
     */
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
    /*
     * Get local lif
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));
    /*
     * Get EEDB ARP info from lif tble manager
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_ARP, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                    (unit, gport_hw_resources.local_out_lif, entry_handle_id, &outlif_info));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_VSI, &is_field_valid, &vsi));
    if (is_field_valid == TRUE)
    {
        /** If VSI present in the EEDB entry */
        egr->vlan = (bcm_vlan_t) vsi;
    }

    if (_SHR_IS_FLAG_SET(outlif_info.table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_VLAN_TRANSLATION))
    {
        uint8 is_2tag_valid;

        egr->flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                        (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, &is_2tag_valid));
        if (is_2tag_valid)
        {
            egr->flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS;
        }
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, &is_field_valid));
    if (is_field_valid == TRUE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, &remark_profile));
        if (remark_profile != 0)
        {
            egr->qos_map_id = remark_profile;
            DNX_QOS_REMARK_MAP_SET(egr->qos_map_id);
            DNX_QOS_EGRESS_MAP_SET(egr->qos_map_id);
        }
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, &is_field_valid));
    if (is_field_valid == TRUE)
    {
        egr->flags2 |= BCM_L3_FLAGS2_EGRESS_STAT_ENABLE;
    }
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, &is_field_valid));
    if (is_field_valid == TRUE)
    {
        uint8 qos_model;
        uint8 qos_index = 0;
        /** qos model*/
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE, &qos_model));
        SHR_IF_ERR_EXIT(dnx_qos_egress_model_dbal_to_bcm(unit, qos_model, &egr->egress_qos_model));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                        (unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, &is_field_valid));
        if ((is_field_valid == TRUE) &&
            ((qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE) ||
             (qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE)))
        {
            uint8 nwk_qos;
            uint8 nwk_qos_map;
            uint8 dp_map;

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, &qos_index));
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_attr_get
                            (unit, qos_index, &nwk_qos, &nwk_qos_map, &dp_map));
            egr->mpls_pkt_pri = DNX_QOS_IDX_NWK_QOS_PCP_GET(nwk_qos);
            egr->mpls_pkt_cfi = DNX_QOS_IDX_NWK_QOS_CFI_GET(nwk_qos);
        }
    }
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, &is_field_valid, &src_mac_addr_msb_idx));
    if (is_field_valid == TRUE)
    {
        uint32 dual_homing;
        int ref_count;
        source_address_entry_t source_address_entry;

        egr->flags2 |= BCM_L3_FLAGS2_SRC_MAC;

        /*
         * source address type full mac has special masking, unmasking can be done on all types
         */
        SOURCE_ADDRESS_FULL_UNMASK(src_mac_addr_msb_idx);
        /*
         * get mymac prefix
         */
        SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.egress_source_address_table.profile_data_get
                        (unit, src_mac_addr_msb_idx, &ref_count, (void *) &source_address_entry));
        /*
         * assign src_mac_addr
         */
        sal_memcpy(egr->src_mac_addr, source_address_entry.address.mac_address, sizeof(bcm_mac_t));
        if (dnx_data_l3.source_address.custom_sa_use_dual_homing_get(unit))
        {
            dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_DUAL_HOMING, INST_SINGLE,
                                                &dual_homing);
            L3_ARP_SET_SRC_MAC_ADDR_LSB(egr->src_mac_addr, dual_homing);
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, entry_handle_id, DBAL_FIELD_L2_MAC, INST_SINGLE, egr->mac_addr));

    egr->encap_id = intf;
    if (BCM_L3_ITF_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(intf))
    {
        egr->flags2 |= BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED;
    }

    /** set NATIVE flag based on lif phase */
    if (outlif_info.outlif_phase == LIF_MNGR_OUTLIF_PHASE_NATIVE_ARP)
    {
        egr->flags |= BCM_L3_NATIVE_ENCAP;
    }

    /*
     * Get next pointer
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, &is_field_valid, &next_pointer));
    if ((is_field_valid == TRUE) && (next_pointer != 0))
    {
            /** In case local lif is not found, return next pointer invalid */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, _SHR_CORE_ALL,
                                                    next_pointer, &egr->vlan_port_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*See l3_arp.h for prototype define*/
shr_error_e
dnx_l3_egress_arp_info_delete(
    int unit,
    bcm_if_t intf)
{
    int local_out_lif = 0;
    uint32 gport;
    int global_lif = 0;
    int src_mac_prefix = -1;
    int nwk_qos_index = -1;
    uint8 last_ref = 0;
    uint8 is_last_nwk_qos_id = FALSE;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dnx_egr_pointed_t egr_pointed_id;
    uint32 lif_flags;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Input 'intf' is assumed to be encoded as 'lif'.
     * Caller is assumed to have taken care of that.
     */
    /*
     * Verify sub_type encoding. It may only be 'default' or 'egr_pointed'
     */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, intf);
    /*
     * At this point, 'gport' has 'type' of 'tunnel'
     */
    egr_pointed_id = DNX_ALGO_EGR_POINTED_INVALID;
    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(gport))
    {
        /*
         * For EGR_POINTED encoded input (intf), get 'local lif' using
         * 'egr_pointed' id.
         */
        lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        egr_pointed_id = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(gport);
    }
    else
    {
        /*
         * For 'all other cases' encoded input (intf), get 'local lif' using
         * 'global lif' id.
         */
        lif_flags =
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    }
    /*
     * Get local lif
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));
    global_lif = gport_hw_resources.global_out_lif;
    /*
     * At this point, either 'egr_pointed_id' is invalid (DNX_ALGO_EGR_POINTED_INVALID)
     * or 'global_lif' is invalid (DNX_ALGO_GPM_LIF_INVALID).
     */
    local_out_lif = gport_hw_resources.local_out_lif;
    /*
     * Get src mac prefix
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_src_mac_prefix_and_qos_index_get
                    (unit, local_out_lif, &src_mac_prefix, &nwk_qos_index));
    if (src_mac_prefix != -1)
    {
        SHR_IF_ERR_EXIT(dnx_l3_intf_egress_mymac_prefix_free(unit, src_mac_prefix, &last_ref));
    }
    if (nwk_qos_index != DNX_QOS_INVALID_MAP_ID)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_free(unit, nwk_qos_index, &is_last_nwk_qos_id));
    }
    /*
     * Clear the HW ARP table and GLEM table.
     * But, do NOT access GLEM if 'global_lif' is invalid (DNX_ALGO_GPM_LIF_INVALID).
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_destroy_arp_hw_clear(unit, global_lif, local_out_lif));
    /*
     * Clear the SRC MAC addr table
     */
    if (last_ref)
    {
        SHR_IF_ERR_EXIT(dnx_l3_intf_egress_mymac_clear(unit, TRUE, src_mac_prefix));
    }
    /**free qos resource*/
    if (is_last_nwk_qos_id)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_hw_clear(unit, nwk_qos_index));
    }
    /*
     * Free the LIF resource
     */
    if (egr_pointed_id != DNX_ALGO_EGR_POINTED_INVALID)
    {
        /*
         * For EGR_POINTED encoded input (intf)
         */
        uint32 alloc_flags;
        int element;
        /*
         * Now remove table entries mapping between 'egr_pointed' and 'local_lif'
         */
        alloc_flags = 0;
        SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_local_lif_mapping_remove(unit, alloc_flags, egr_pointed_id));
        /*
         * Release (deallocate) this 'egr_pointed' object.
         */
        alloc_flags = 0;
        element = egr_pointed_id;
        SHR_IF_ERR_EXIT(algo_l3_db.egr_pointed_alloc.egr_pointed_res_manager.free_single(unit, element));
    }
    if (global_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        /*
         * Remove 'global_lif'. Note that for the EGR_POINTED case, 'global_lif' is invalid (DNX_ALGO_GPM_LIF_INVALID)
         * and, so, 'free operation' does not apply to it.
         */
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_EGRESS, global_lif));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Edit next pointer of the given gport.
 * \param [in] unit - Relevant unit.
 * \param [in] tunnel_gport - The gport of tunnel to change next pointer for
 * \param [in] next_local_out_lif - next pointer
 * \return
 *   \retval Negative in case of an error. Zero in case of NO ERROR
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l3_arp_tunnel_encap_update_next_pointer(
    int unit,
    bcm_gport_t tunnel_gport,
    int next_local_out_lif)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources, next_gport_hw_resources;
    uint32 lif_flags;
    uint32 entry_handle_id;
    uint8 is_last_tunnel = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * At this point, 'tunnel_gport' has 'type' of 'tunnel'
     */
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, tunnel_gport, lif_flags, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    /*
     * Find the last tunnel in the linked list, this tunnel will point to arp
     */
    while (is_last_tunnel == FALSE)
    {
        uint8 is_field_on_handle;
        uint32 next_tunnel_lif;
        bcm_gport_t next_tunnel_gport = BCM_GPORT_INVALID;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources.local_out_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.outlif_dbal_result_type);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, &is_field_on_handle, &next_tunnel_lif));
        if (!is_field_on_handle)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "given interface intf doesn't have NEXT_POINTER attribute.\r\n");
        }

        if (next_tunnel_lif != 0)
        {
            /*
             * Get Gport from next pointer, if not found or next lif is not tunnel then this is the last tunnel
             * Otherwise continue looking for the last one
             */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                                        DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, _SHR_CORE_ALL,
                                                        next_tunnel_lif, &next_tunnel_gport));
            if (next_tunnel_gport != BCM_GPORT_INVALID)
            {
                SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                            (unit, next_tunnel_gport, lif_flags, &next_gport_hw_resources),
                                            _SHR_E_NOT_FOUND, _SHR_E_PARAM);
                if (next_gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_EEDB_MPLS_TUNNEL)
                {
                    sal_memcpy(&gport_hw_resources, &next_gport_hw_resources,
                               sizeof(dnx_algo_gpm_gport_hw_resources_t));
                    is_last_tunnel = FALSE;
                }
                else
                {
                    is_last_tunnel = TRUE;
                }
            }
            else
            {
                is_last_tunnel = TRUE;
            }
        }
        else
        {
            is_last_tunnel = TRUE;
        }
        if (!is_last_tunnel)
        {
            DBAL_HANDLE_FREE(unit, entry_handle_id);
        }
    }
    /*
     * Set next pointer for the tunnel
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                 next_local_out_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_l3_egress_create_arp(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr)
{
    int ll_local_lif = 0;
    int ll_global_lif = 0;
    int src_mac_addr_prefix_profile = 0, old_src_mac_addr_prefix_profile;
    uint8 is_first_src_mac_addr_reference = 0, is_last_src_mac_addr_reference = 0;
    uint8 is_first_nwk_qos_id = FALSE, is_last_nwk_qos_id = FALSE;
    int nwk_qos_index = 0;
    int old_qos_index = DNX_QOS_INVALID_MAP_ID;
    bcm_l3_egress_t old_egr;

    /*
     * Initialize to silence some compilers.
     */
    dnx_egr_pointed_t egr_pointed_id = 0;
    int old_ll_local_lif = 0;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify egress (ARP) 
     */
    SHR_INVOKE_VERIFY_DNX(dnx_l3_egress_create_arp_verify(unit, flags, egr));
    /*
     * SW Allocations
     */
    /** Allocate egress (ARP or ARP+AC) */
    SHR_IF_ERR_EXIT(dnx_l3_egress_create_arp_allocate
                    (unit, flags, egr, &ll_local_lif, &ll_global_lif, &egr_pointed_id));

    /*
     * At this point, either 'll_global_lif' is valid or 'egr_pointed_id' is valid (at least one but not both).
     */
    /*
     * If encap_id is given with BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN | BCM_L3_REPLACE flags,
     * we assume Link-layer is not changed and we don't update it.
     * Useful in case the only need is to update multiple Tunnel-intfs such as MPLS-Tunnel or IP-Tunnel to same Link-Layer
     */
    if (!((flags & (BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN)) == (BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN)))
    {
        bcm_l3_egress_t_init(&old_egr);
        if (_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE))
        {
            SHR_IF_ERR_EXIT(dnx_l3_egress_arp_info_get(unit, egr->encap_id, &old_egr));
            /**Get src mac prefix and qos index*/
            SHR_IF_ERR_EXIT(dnx_l3_egress_src_mac_prefix_and_qos_index_get
                            (unit, ll_local_lif, &old_src_mac_addr_prefix_profile, &old_qos_index));
        }

        /** Allocate src mac addr prefix when BCM_L3_FLAGS2_SRC_MAC is set */
        if (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_SRC_MAC))
        {
            if (_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE) && _SHR_IS_FLAG_SET(old_egr.flags2, BCM_L3_FLAGS2_SRC_MAC))
            {
                SHR_IF_ERR_EXIT(dnx_l3_intf_egress_mymac_prefix_exchange
                                (unit, TRUE, egr->src_mac_addr, old_src_mac_addr_prefix_profile,
                                 &src_mac_addr_prefix_profile, &is_first_src_mac_addr_reference,
                                 &is_last_src_mac_addr_reference));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_l3_intf_egress_mymac_prefix_allocate
                                (unit, TRUE, egr->src_mac_addr, &src_mac_addr_prefix_profile,
                                 &is_first_src_mac_addr_reference));
            }
        }
        else if (_SHR_IS_FLAG_SET(old_egr.flags2, BCM_L3_FLAGS2_SRC_MAC) && (old_src_mac_addr_prefix_profile != -1))
        {
            SHR_IF_ERR_EXIT(dnx_l3_intf_egress_mymac_prefix_free
                            (unit, old_src_mac_addr_prefix_profile, &is_last_src_mac_addr_reference));
        }
        if (ll_global_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            /*
             * Enter here if 'll_global_lif' is 'valid'.
             * Note that, for the 'egr_pointed' case, procedure dnx_l3_egress_create_arp_allocate()
             * above has returned an 'invalid' global lif.
             */
            /*
             * Note that, effectively, BCM_L3_ITF_SET loads the 'sub_type' field by the 'not_encoded'
             * (= 'default') value. If this becomes not true then imitate the handling for
             * 'egr_pointed' below.
             */
            BCM_L3_ITF_SET(egr->encap_id, BCM_L3_ITF_TYPE_LIF, ll_global_lif);
        }
        else
        {
            /*
             * Enter here if 'll_global_lif' is 'invalid' and 'egr_pointed_id' is valid.
             */
            int l3_itf_dummy;

            BCM_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(l3_itf_dummy, egr_pointed_id);
            BCM_L3_ITF_SET(egr->encap_id, BCM_L3_ITF_TYPE_LIF, l3_itf_dummy);
        }
        SHR_IF_ERR_EXIT(dnx_l3_egress_arp_qos_calculate
                        (unit, egr, old_qos_index, &nwk_qos_index, &is_first_nwk_qos_id, &is_last_nwk_qos_id));
        /*
         * Write to HW tables
         */
        if (is_first_nwk_qos_id)
        {
            uint32 nwk_qos = 0;
            uint32 dp_mapped = 0;

            nwk_qos = DNX_QOS_IDX_PCP_DEI_TO_NWK_QOS(egr->mpls_pkt_pri, egr->mpls_pkt_cfi);
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_set
                            (unit, nwk_qos_index, nwk_qos, nwk_qos, dp_mapped));
        }
        if (is_first_src_mac_addr_reference)
        {
            /** Set prefix MSB SrcMAC information in case it is the first time. */
            SHR_IF_ERR_EXIT(dnx_l3_intf_egress_mymac_set(unit, TRUE, egr->src_mac_addr, src_mac_addr_prefix_profile));
        }
        /*
         * Write ARP info the EEDB
         * Note that 'll_local_lif' may be changed by this procedure. In that case
         * 'virtual_egr_pointed' system needs to be updated as well. See below.
         */
        old_ll_local_lif = ll_local_lif;
        SHR_IF_ERR_EXIT(dnx_l3_egress_create_arp_hw_write
                        (unit, flags, egr, src_mac_addr_prefix_profile, (uint32 *) &ll_local_lif, ll_global_lif,
                         nwk_qos_index));
        /*
         * Clear unused profiles after entry set/updated
         */
        if (is_last_src_mac_addr_reference)
        {
            SHR_IF_ERR_EXIT(dnx_l3_intf_egress_mymac_clear(unit, TRUE, old_src_mac_addr_prefix_profile));
        }
        if ((old_qos_index != DNX_QOS_INVALID_MAP_ID) && is_last_nwk_qos_id)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_hw_clear(unit, old_qos_index));
        }
        if (egr_pointed_id != DNX_ALGO_EGR_POINTED_INVALID)
        {
            uint32 mapping_flags = 0;
            if (!_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE))
            {
                /*
                 * If this is not a 'replace' operation then create mapping between
                 * 'egr_pointed' and 'local_lif'
                 */
                SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_local_lif_mapping_add
                                (unit, mapping_flags, egr_pointed_id, ll_local_lif));
            }
            else if (old_ll_local_lif != ll_local_lif)
            {
                /*
                 * If this is a 'replace' operation but 'll_local_lif' has been replaced then
                 * update mapping between 'egr_pointed' and 'local_lif':
                 * Remove exiting mapping and add the new one.
                 */
                SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_local_lif_mapping_remove(unit, flags, egr_pointed_id));
                SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_local_lif_mapping_add
                                (unit, mapping_flags, egr_pointed_id, ll_local_lif));
            }
        }
    }
    /*
     * In case interface is valid, it represents tunnel that should point on the created ARP
     */
    if (egr->intf != 0)
    {
        bcm_gport_t tunnel_gport;

        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(tunnel_gport, egr->intf);
        SHR_IF_ERR_EXIT(dnx_l3_arp_tunnel_encap_update_next_pointer(unit, tunnel_gport, ll_local_lif));
    }

exit:
    SHR_FUNC_EXIT;
}
