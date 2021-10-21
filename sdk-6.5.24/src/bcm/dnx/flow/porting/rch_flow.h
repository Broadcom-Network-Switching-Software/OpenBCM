/** \file rch_flow.h
 * l2_egress APIs to flows APIs conversion
*/

#ifndef _RCH_FLOW_INCLUDED__
/*
 * {
 */
#define _RCH_FLOW_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* BCM_DNX_SUPPORT */

/**
  * \brief
 * convert l2_egress create API to flow API
 * \param [in] unit - The unit number.
 * \param [in] egr - egr struct
 */
shr_error_e dnx_rch_create_flow(
    int unit,
    bcm_l2_egress_t * egr);

/**
  * \brief
 * convert l2_egress destory API to flow API
 * header profile
 * \param [in] unit - The unit number.
 * \param [in] encap_id - id to destroy
 */
shr_error_e dnx_rch_destroy_flow(
    int unit,
    bcm_if_t encap_id);

/**
  * \brief
 * convert l2_egress get API to flow API
 * header profile
 * \param [in] unit - The unit number.
 * \param [in] encap_id - id to get
 * \param [out] egr - egr struct
 */
shr_error_e dnx_rch_get_flow(
    int unit,
    bcm_if_t encap_id,
    bcm_l2_egress_t * egr);
#endif  /*_RCH_FLOW_INCLUDED__ */
