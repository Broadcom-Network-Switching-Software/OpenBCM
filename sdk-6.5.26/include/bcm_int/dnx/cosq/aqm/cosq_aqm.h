/*
 * ! \file bcm_int/dnx/cosq/aqm/cosq_aqm.h
 * Reserved.$
 */

#ifndef _DNX_COSQ_AQM_H_INCLUDED_
/*
 * {
 */
#define _DNX_COSQ_AQM_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm/cosq.h>

/*
 * Defines
 * {
 */
#define DNX_COSQ_AQM_PROFILE_RESOURCE "COSQ_AQM_PROFILE_RESOURCE"
#define DNX_COSQ_AQM_FLOW_ID_RESOURCE "COSQ_AQM_FLOW_ID_RESOURCE"

#define DNX_COSQ_AQM_FEATURE_VERIFY(unit) \
    do { \
          if (!dnx_data_cosq_aqm.general.feature_get(unit, dnx_data_cosq_aqm_general_is_aqm_supported)) \
          { \
              SHR_ERR_EXIT(_SHR_E_PARAM, "AQM is not supported on this device"); \
          } \
    } while (0)

/*
 * }
 */
/**
 * \brief - initialize AQM module.
 *
 * \param [in] unit
 *
 * \return
 *   shr_error_e
 * \remark
 *   None
 * \see
 *   None
 */
shr_error_e dnx_cosq_aqm_init(
    int unit);

/**
* \brief
*  function get AQM flow id gport and map it to End to End AQM
*  lateny profile gport/ port and tc
* \param [in] unit -unit id
* \param [in] port - aqm flow id gport
* \param [in] cosq - offset/index
* \param [in] type - property type of the profile
* \param [in] arg  - the value of the property
* \return
*    shr_error_e
* \remark
*   None
* \see
*   NONE
*/
shr_error_e dnx_cosq_aqm_flow_profile_mapping_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    uint32 arg);

/**
* \brief
*  function get AQM flow id gport and get the mapping to End to
*  End AQM lateny profile gport/ port and tc
* \param [in] unit - unit id
* \param [in] port - latency gport
* \param [in] cosq - offset/index
* \param [in] type - property type of the profile
* \param [out] arg  - the value of the property
* \return
*    shr_error_e
* \remark
*   None
* \see
*   NONE
*/
shr_error_e dnx_cosq_aqm_flow_profile_mapping_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    uint32 *arg);

/**
 * \brief - allocate new gport for E2E latency AQM flow ID
 *
 * \param [in] unit - unit
 * \param [in] flags - flags - with/without id
 * \param [out] gport - allocated gport
 *
 * \return
 *    shr_error_e
 * \remark
 *   None
 * \see
 *   None
 */
shr_error_e dnx_cosq_aqm_flow_id_gport_add(
    int unit,
    uint32 flags,
    bcm_gport_t * gport);

/**
 * \brief - remove gport E2E latency AQM flow ID
 *
 * \param [in] unit - unit
 * \param [out] gport - allocated gport
 *
 * \return
 * \remark
 *   None
 * \see
 *   None
 */
shr_error_e dnx_cosq_aqm_flow_id_gport_delete(
    int unit,
    bcm_gport_t gport);

/**
 * \brief set a configuration of the AQM profile engines
 *
 * \param [in] unit -           unit
 * \param [in] aqm_profile -    AQM profile gport
 * \param [in] cosq -           Meaning might change based on type
 * \param [in] type -           The configuration to set
 * \param [in] arg  -           value to set
 * \return
     shr_error_e
 * \remark
 *   None
 * \see
 *   None
 */
shr_error_e dnx_cosq_aqm_config_set(
    int unit,
    bcm_gport_t aqm_profile,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

/**
 * \brief get a configuration of the AQM profile engines
 *
 * \param [in] unit -           unit
 * \param [in] aqm_profile -    AQM profile gport
 * \param [in] cosq -           Meaning might change based on type
 * \param [in] type -           The configuration to get
 * \param [in] arg  -           value to get
 * \return
     shr_error_e
 * \remark
 *   None
 * \see
 *   None
 */
shr_error_e dnx_cosq_aqm_config_get(
    int unit,
    bcm_gport_t aqm_profile,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);

/** } */
#endif /*_DNX_COSQ_AQM_H_INCLUDED_ */
