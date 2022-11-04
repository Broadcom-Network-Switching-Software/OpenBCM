/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * Cint mgmt credit size
 * The role of this cint is to demonstrate the required set of confiugrations when credit size is different between 2 devices.
 * i.e assuming 2 FAPs and a Fabric
 *
 *      in_port (IXIA port)    port, loopback
 *          |                   /\
 * +-------------------+-------------------+
 * |                   |                   |
 * |    FAP 0          |        FAP 1      |
 * |                   |                   |
 * +---------------------------------------+
 * |                Fabric                 |
 * |                                       |
 * +---------------------------------------+
 *
 * * Getting traffic from fap0 'fap_0_in_port' with 'mac_address' as destination adress.
 * * Forwarding the traffic to fap1 'fap_1_port' (sw loopback).
 * * Forwarding the traffic back to fap0 'fap_0_in_port' (sw loopback).
 * * fap 0 credit size is different from fap1 credit size. This requires setting for each local fap the remote faps credit size
 * * and creating a delay tolerance profile (IPS) which fulfills the destination device credit size and destination port rate.
 *
 */

/**
 * Set remote fap credit size for a local fap
 */
int cint_bcm_mgmt_remote_credit_size_set(int fap_unit, int dest_fap_unit, int credit_size)
{
    int nof_modids;
    int dest_modid;
    int modid_count;
    int i_dest_modid;
    char error_msg[200]={0,};

    /** get number of modids on destination fap */
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_modid_count(dest_fap_unit, &nof_modids), "");

    bcm_stk_modid_config_t dest_modid_array[nof_modids];
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_modid_config_get_all(dest_fap_unit, nof_modids, dest_modid_array, &modid_count), "");

    /** set remote credit size for all remote modids */
    for (i_dest_modid = 0; i_dest_modid < nof_modids; i_dest_modid++)
    {
        dest_modid = dest_modid_array[i_dest_modid].modid;
        snprintf(error_msg, sizeof(error_msg), "dest_fap_id=%d, credit_size=%d", dest_modid, credit_size);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_dest_credit_size_set(fap_unit, dest_modid, credit_size), error_msg);
    }

    printf("cint_bcm_mgmt_remote_credit_size_set: PASS\n\n");
    return BCM_E_NONE;
}

/*
 * * Create delay tolerance profile according to destination fap credit size and destination port rate.
 * * Map VoQ to the created delay tolerance profile
 * * Note - the VoQ ID assumes DNX reference application.
 */
int cint_bcm_mgmt_delay_tolerenace_level_create(int fap_unit, int dest_fap_unit, bcm_port_t dest_port, int credit_size)
{
    int core;
    bcm_cosq_delay_tolerance_t credit_request_profile;
    bcm_cosq_delay_tolerance_preset_attr_t credit_request_attr;
    bcm_port_resource_t port_resource;
    int profile = 20; /** assuming profile 20 is free */
    int dest_modid, base_voq, cos;
    bcm_gport_t voq_gport;
    int profile_is_rate_class = *dnxc_data_get(fap_unit, "iqs", "credit", "profile_is_rate_class", NULL);
    int flags = 0;
    bcm_cosq_rate_class_create_info_t create_info;
    bcm_gport_t rate_class_gport;
    bcm_switch_profile_mapping_t profile_mapping;
    char error_msg[200]={0,};


    /** create delay tolerance profile according to speed and credit size */
    {
        /** get set of recommended values */
        snprintf(error_msg, sizeof(error_msg), "port=%d", dest_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_get(dest_fap_unit, dest_port, &port_resource), error_msg);
        if (profile_is_rate_class)
        {
            create_info.attributes = BCM_COSQ_RATE_CLASS_CREATE_ATTR_SLOW_ENABLED;
            create_info.credit_size = credit_size;
            create_info.rate = port_resource.speed/1000;
            BCM_GPORT_PROFILE_SET(rate_class_gport, profile);
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_rate_class_create(fap_unit, rate_class_gport, flags, &create_info), "");
        }
        else
        {
            bcm_cosq_delay_tolerance_preset_attr_t_init(&credit_request_attr);
            credit_request_attr.rate =  port_resource.speed/1000;
            credit_request_attr.credit_size =  credit_size;
            credit_request_attr.flags = BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_SLOW_ENABLED;
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_delay_tolerance_preset_get(fap_unit, &credit_request_attr, &credit_request_profile), "");

            /** set the threshold */
            snprintf(error_msg, sizeof(error_msg), "profile=%d, credit_size=%d", profile, credit_size);
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_delay_tolerance_level_set(fap_unit, profile, &credit_request_profile), error_msg);
        }
    }

    /** map designated voq to the created delay tolerance profile */
    {
        int modid_count, actual_modid_count;
        BCM_IF_ERROR_RETURN_MSG(bcm_stk_modid_count(dest_fap_unit, &modid_count), "");
        bcm_stk_modid_config_t modid_array[modid_count];

        BCM_IF_ERROR_RETURN_MSG(bcm_stk_modid_config_get_all(dest_fap_unit, modid_count, modid_array, &actual_modid_count), "");
        dest_modid = modid_array[0].modid;

        /** get voq id - assuming DNX reference application */
        snprintf(error_msg, sizeof(error_msg), "dest_modid=%d, dest_port=%d", dest_modid, dest_port);
        BCM_IF_ERROR_RETURN_MSG(appl_dnx_e2e_scheme_logical_port_to_base_voq_get(fap_unit, dest_modid, dest_port, &base_voq), error_msg);

        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, base_voq);

        /** map voq to delay tolerance profile */
        for (cos = 0; cos < 8; cos++)
        {
            if (profile_is_rate_class)
            {
                profile_mapping.profile_type = bcmCosqIngressQueueToRateClass;
                profile_mapping.mapped_profile = profile;
                snprintf(error_msg, sizeof(error_msg), "base_voq=%d, cos=%d, profile=%d", base_voq, cos, profile);
                BCM_IF_ERROR_RETURN_MSG(bcm_cosq_profile_mapping_set(fap_unit, voq_gport, cos, 0, profile_mapping), error_msg);
            }
            else
            {
                snprintf(error_msg, sizeof(error_msg), "base_voq=%d, cos=%d, profile=%d", base_voq, cos, profile);
                BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(fap_unit, voq_gport, cos, profile, 0), error_msg);
            }
        }
    }

    printf("cint_bcm_mgmt_delay_tolerenace_level_create: PASS\n\n");
    return BCM_E_NONE;
}

/*
 * MGMT MGMT CREDIT SIZE END
 */

