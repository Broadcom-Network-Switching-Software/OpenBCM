/*
 * cint_latency_utils.c
 *
 *  Created on: Oct 18, 2021
 *      Author: sa892315
 */

 /* Explanation:
 *  This cint includes general functions for latency module
 */



/**
 * \brief
 *  get the given port's rate class
 * \param [in] unit - unit id
 * \param [in] out_port -
 * \param [out] rate_class_get - port's rate class
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static int
latency_port_rate_class_get(int unit,
        int port,
        int * rate_class_get)
{
    int base_voq;
    bcm_gport_t voq_gport;
    bcm_switch_profile_mapping_t voq_profile_params;

    /* get the port's base_voq */
    BCM_IF_ERROR_RETURN_MSG(appl_dnx_e2e_scheme_logical_port_to_base_voq_get(unit, 0, port, &base_voq), "");

    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(&voq_gport, base_voq);
    voq_profile_params.profile_type = bcmCosqIngressQueueToRateClass;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_profile_mapping_get(unit, voq_gport, 0 /*cosq*/, 0 /*flags*/, &voq_profile_params), "");

    *rate_class_get = BCM_GPORT_PROFILE_GET(voq_profile_params.mapped_profile);

    return BCM_E_NONE;
}

/**
 * \brief
 *  change port's voq tail drop
 * \param [in] unit - unit id
 * \param [in] out_port -
 * \param [in] voq_size - the queue size
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
latency_voq_max_size_set(
        int unit,
        int out_port,
        int voq_size)
{

    uint32 flags;
    int dp, cosq = 0, rate_class, base_voq;
    bcm_gport_t rate_class_gport, port_gport, voq_gport;
    bcm_cosq_gport_size_t bytes_fadt_drop_params;
    bcm_switch_profile_mapping_t voq_profile_params;

    /** get voq's rate class*/
    BCM_IF_ERROR_RETURN_MSG(latency_port_rate_class_get(unit, out_port, &rate_class), "");

    /** Set queue size*/
    {
        flags = BCM_COSQ_GPORT_SIZE_BYTES;
        BCM_GPORT_PROFILE_SET(rate_class_gport, rate_class);

        bytes_fadt_drop_params.size_min = voq_size;
        bytes_fadt_drop_params.size_max = voq_size;
        bytes_fadt_drop_params.size_fadt_min = voq_size;
        bytes_fadt_drop_params.size_alpha_max = 0;

        for(dp = 0; dp < 4; dp++)
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_color_size_set(unit, rate_class_gport, cosq, dp, flags, &bytes_fadt_drop_params), "");

        }
    }

    return BCM_E_NONE;

}

