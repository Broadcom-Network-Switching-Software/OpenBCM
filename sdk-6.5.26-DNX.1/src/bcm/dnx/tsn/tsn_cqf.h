/**
 * \file src/bcm/dnx/tsn/tsn_cqf.h
 * 
 *
 * CQF (Cyclic Queuing and Forwarding) functionality
 */

#ifndef _TSN_CQF_H_INCLUDED_
#define _TSN_CQF_H_INCLUDED_

/*
 * INCLUDE FILES:
 * {
 */

/*
 * }
 */

/**
 * \brief -
 *  Verify cqf config set
 *
 * \param [in] unit - the relevant unit
 * \param [in] flags - not used
 * \param [in] port - logical port
 * \param [in] config - CQF config structure
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
shr_error_e dnx_tsn_cqf_config_set(
    int unit,
    uint32 flags,
    bcm_port_t port,
    bcm_tsn_cqf_config_t * config);

/**
 * \brief -
 *  Verify cqf config set
 *
 * \param [in] unit - the relevant unit
 * \param [in] flags - not used
 * \param [in] port - logical port
 * \param [out] config - CQF config structure
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
shr_error_e dnx_tsn_cqf_config_get(
    int unit,
    uint32 flags,
    bcm_port_t port,
    bcm_tsn_cqf_config_t * config);

/**
 * \brief - Init function for Cyclic Queuing and Forwarding
 *
 * \param [in] unit -HW identifier of unit.
 * \return
 *   shr_error_e
 * \remark
 */
shr_error_e dnx_tsn_cqf_init(
    int unit);

#endif /* _TSN_CQF_H_INCLUDED_ */
