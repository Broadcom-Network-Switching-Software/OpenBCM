#ifndef _BCM_DNX_INT_INTR_HEADER_
#define _BCM_DNX_INT_INTR_HEADER_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*************
 * FUNCTIONS *
 *************/
 /**
 * \brief
 *   Initialize dnx interrupt module.
 * 
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_intr_init(
    int unit);
/**
 * \brief
 *   Deinitialize dnx interrupt module.
 * 
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_intr_deinit(
    int unit);

#endif
