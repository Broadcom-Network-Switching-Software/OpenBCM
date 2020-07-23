/** \file mpls_l2vpn.h
 *      Used by mpls_l2vpn, mpls_port and EVPN(mpls) files
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef __MPLS_L2VPN_INCLUDED__
#define __MPLS_L2VPN_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* BCM_DNX_SUPPORT */

#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
#include <bcm/mpls.h>

/*
 * *********************************************************
 * DEFINES
 * {
 */
/**
 * \brief Minimal allowed service tags for in PWE port
 */
#define DNX_MPLS_L2VPN_INGRESS_SERVICE_TAGS_MIN      1
/**
 * \brief Maximal allowed service tags for in PWE port
 */
#define DNX_MPLS_L2VPN_INGRESS_SERVICE_TAGS_MAX      2
/*
 *
 * MPLS_L2VPN_FLAG_xxx defined to set information that can't be conveyed
 * through the MPLS-Port structure flags.
 */
/**
 * \brief Egress EVPN Label is for Inclusive Multicast (IML)
 */
#define DNX_MPLS_L2VPN_FLAG_EGRESS_IML              (SAL_BIT(0))
/**
 * \brief Ingress EVPN Label is for Inclusive Multicast (IML) expected as BOS.
 */
#define DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITHOUT_ESI (SAL_BIT(1))
/**
 * \brief Ingress EVPN Label is for Inclusive Multicast (IML) with ESI expected as BOS.
 */
#define DNX_MPLS_L2VPN_FLAG_INGRESS_IML_WITH_ESI    (SAL_BIT(2))
/**
 * \brief Ingress EVPN Label is for Inclusive Multicast (IML) with ESI or not.
 *  Flag is available only when dnx_data_mpls.general.mpls_term_1_or_2_labels is set.
 */
#define DNX_MPLS_L2VPN_FLAG_INGRESS_IML             (SAL_BIT(3))

/*
 * }
 * DEFINES
 */
/*
 * MACROS
 * {
 */
/*
 * Check if the #service-tags for in PWE is in range
 */
#define DNX_MPLS_L2VPN_INGRESS_SERVICE_TAGS_NOF_IN_RANGE(_nof_service_tags_) \
            (((_nof_service_tags_) >= DNX_MPLS_L2VPN_INGRESS_SERVICE_TAGS_MIN) && \
             ((_nof_service_tags_) <= DNX_MPLS_L2VPN_INGRESS_SERVICE_TAGS_MAX))
/*
 * }
 * MACROS
 */
/*
 * *********************************************************
 * TYPES
 * {
 */
/**
 * \brief MPLS L2 VPN type
 */
typedef enum
{
    MPLS_L2VPN_TYPE_NOT_DEFINED = 0,
    MPLS_L2VPN_TYPE_MPLS_PORT,
    MPLS_L2VPN_TYPE_EVPN,
    /*
     * Must be last
     */
    MPLS_L2VPN_TYPE_NOF
} dnx_mpls_l2vpn_type_e;
/**
 * \brief MPLS based L2 vpn object
 */
typedef struct
{
    /**
     * \brief BCM MPLS Port module object pointer
     */
    bcm_mpls_port_t *mpls_port;
    /**
     * \brief DNX_MPLS_L2VPN_xxx
     */
    uint32 flags;
    /**
     * \brief MPLS L2 VPN type (VPLS, EVPN)
     */
    dnx_mpls_l2vpn_type_e type;
    /**
     * \brief VPN Identifier
     */
    bcm_vpn_t vpn;
} dnx_mpls_l2vpn_info_t;
/*
 * }
 * TYPES
 */
/*
 * *********************************************************
 * PROTOTYPES
 * {
 */

/**
 * \brief Initialize dnx_mpls_l2vpn_info_t object with the mandatory
 * fields. Assumes mpls_port is not NULL and initialized separately.
 */
void dnx_mpls_l2vpn_info_t_init(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn_info,
    dnx_mpls_l2vpn_type_e type,
    bcm_mpls_port_t * mpls_port);

/**
 * \brief Creates an MPLS L2VPN egress or ingress object (can't be both).
 * \param [in] unit
 * \param [in,out] l2vpn_info - Main structure for this API.
 *              [in] l2vpn_info->vpn - VPN (VSI) value.
 *              [in] l2vpn_info->type - Specify the flavor of the MPLS L2VPN to be created.
 *              [in,out] l2vpn_info->mpls_port - Hold most of the information. Non-VPLS objects need to be translated to this object
 *                                              before being supplied to mpls_l2vpn module. This is for historical reasons, optimizing
 *                                              the mpls_port module and because mpls_port is the best BCM-API struct available for
 *                                              MPLS L2VPN, at the time of the writing of this module.
 */
shr_error_e dnx_mpls_l2vpn_add(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn_info);

/**
 * \brief Retrieves the MPLS L2VPN information pointed to by l2vpn_info->mpls_port->mpls_port_id.
 * \param [in] unit
 * \param [in,out] l2vpn_info - Main structure of this API.
 *              [in] l2vpn_info->vpn - VPN that should contain the MPLS L2VPN object
 *              [in] l2vpn_info->type - Specify the flavor of the MPLS L2VPN to be retrieved.
 *              [in,out] l2vpn_info->mpls_port - Mostly will be populated by the retrieval. This object is originally meant
 *                                              for VPLS. Other flavors need to translate the information back to their own
 *                                              API's structures.
 *                            [in] l2vpn_info->mpls_port->mpls_port_id - the identifier of the MPLS L2VPN object to retrieve
 */
shr_error_e dnx_mpls_l2vpn_get(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn_info);

/**
* \brief Delete the mpls l2vpn object.
* \param [in] unit - Relevant unit.
* \param [in] l2vpn_type - The flavor expected for the object.
* \param [in] is_ingress - Ingress indication.
* \param [in] mpls_l2vpn_id - The port to be deleted.
*/
shr_error_e dnx_mpls_l2vpn_delete(
    int unit,
    dnx_mpls_l2vpn_type_e l2vpn_type,
    int is_ingress,
    bcm_gport_t mpls_l2vpn_id);

/**
 * \brief
 * Map VPLS EEDB access stage to lif_mngr app type.
 */
shr_error_e dnx_mpls_l2vpn_encap_access_to_outlif_phase(
    int unit,
    uint32 encap_access,
    lif_mngr_outlif_phase_e * outlif_phase);

/*
 * }
 * PROTOTYPES
 */

#endif  /*__MPLS_L2VPN_INCLUDED__*/
