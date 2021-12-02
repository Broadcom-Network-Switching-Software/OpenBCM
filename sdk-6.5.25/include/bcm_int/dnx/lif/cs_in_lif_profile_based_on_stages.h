/*
 * in_lif_profile_based_on_stages.h
 *
 *  Created on: Jul 9, 2021
 *      Author: sk889844
 */

#ifndef INCLUDE_BCM_INT_DNX_LIF_CS_IN_LIF_PROFILE_BASED_ON_STAGES_H_
#define INCLUDE_BCM_INT_DNX_LIF_CS_IN_LIF_PROFILE_BASED_ON_STAGES_H_

#define DNX_CS_LIF_PROFILE_DEFAULT_NAMESPACE -1
/**
 * \brief
 *   calculates cs_in_lif_profile_value according to in_lif_profile properties
 * \param [in] unit -
 *   The unit number.
 * \param [in] in_lif_profile_info -
 *   Pointer to in_lif_profile_info, which includes the lif profile properties
 * \param [out] cs_vtt_in_lif_profile_value -the concluded cs_vtt_in_lif_profile_value value
 * \param [out] cs_vtt_to_flp_profile_value -the concluded cs_vtt_to_flp_profile_value value
 * \param [out] cs_flp_to_flp_profile_value -the concluded cs_flp_to_flp_profile_value value
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_in_lif_profile_algo_convert_to_cs_in_lif_profile_based_on_stages(
    int unit,
    in_lif_profile_info_t * in_lif_profile_info,
    uint8 *cs_vtt_in_lif_profile_value,
    uint8 *cs_vtt_to_flp_profile_value,
    uint8 *cs_flp_to_flp_profile_value);

/**
 * \brief
 *   calculates cs_in_lif_profile_value according to in_lif_profile properties
 * \param [in] unit -
 *   The unit number.
 * \param [in] in_lif_profile_template_data - data stored in the template manager
 * \param [out] in_lif_profile_info -
 *   Pointer to in_lif_profile_info, which includes the lif profile properties
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_in_lif_profile_translate_to_ingress_flags_based_on_stages(
    int unit,
    in_lif_template_data_t in_lif_profile_template_data,
    in_lif_profile_info_t * in_lif_profile_info);

/**
 * \brief
 *   removes swstate saved info per gport
 * \param [in] unit -
 *   The unit number.
 * \param [in] in_lif_profile_info -
 *   Pointer to in_lif_profile_info, which includes the lif profile properties
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_in_lif_profile_remove_cs_in_lif_profile_based_on_stages(
    int unit,
    in_lif_profile_info_t * in_lif_profile_info);
/*
 * \brief
 *      initialize cs lif profile fields of the structure in_lif_profile_info_t to the default values
 * \param [in] unit -
 *   The unit number.
 * \param [out] in_lif_profile_info -
 *   Pointer to in_lif_profile_info \n
 *     Pointed memory includes LIF-PROFILE properties initiated to the default values
 * \return
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
void cs_in_lif_profile_info_t_init(
    int unit,
    in_lif_profile_info_t * in_lif_profile_info);

/*
 * \brief
 *      Verify function that checks the presence of lif_gport parameter
 * \param [in] unit -
 *   The unit number.
 * \param [in] in_lif_profile_info -
 *   pointer to the lif profile structure
 * \return
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cs_in_lif_profile_lif_gport_verify(
    int unit,
    in_lif_profile_info_t * in_lif_profile_info);

#endif /* INCLUDE_BCM_INT_DNX_LIF_CS_IN_LIF_PROFILE_BASED_ON_STAGES_H_ */
