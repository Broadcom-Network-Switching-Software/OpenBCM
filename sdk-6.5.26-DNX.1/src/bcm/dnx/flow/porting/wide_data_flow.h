/** \file wide_data_flow.h
 * wide data APIs to flows APIs conversion
*/

#ifndef _WIDE_DATA_FLOW_INCLUDED__
/*
 * {
 */
#define _WIDE_DATA_FLOW_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

/**
  * \brief
 * set wide data by flow lif API
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] gport - global lif gport
 * \param [in] flags - wide data flags
 * \param [in] data - wide data
 */
shr_error_e dnx_wide_data_set_flow(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    uint64 data);

/**
  * \brief
 * get wide data by flow lif API
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] gport - global lif gport
 * \param [in] flags - wide data flags
 * \param [out] data - wide data
 */
shr_error_e dnx_wide_data_get_flow(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    uint64 *data);

#endif  /*_WIDE_DATA_FLOW_INCLUDED__ */
