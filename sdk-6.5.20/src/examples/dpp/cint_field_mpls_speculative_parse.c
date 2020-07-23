/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_field_mpls_speculative_parse.c
 * Purpose: Example for utilizing the field APIs to filter all packets that are IPv4/6 over MPLS.
 *          This is used as a proof of concept for speculative parsing of headers over MPLS.
 *
 *
 *
 * CINT usage:
 *  *   Call mpls_lsr_run_with_defaults_multi_device from cint_mpls_lsr.c
 *  *   Call lsr_filter_non_tcp_udp.
 *  *   IPv4/6 over MPLS packets should now be filtered.
 */


bcm_field_group_config_t drop_mpls_group_config;

int lsr_filter_non_tcp_udp(int *units_ids, int nof_units, int in_port) {
    int result;
    int auxRes;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent[6];
    int unit;
    int i, j, presel_id = 1;
    bcm_field_presel_set_t psset;
    int presel_flags = 0;

    uint32 drop_priority = 20,
           pass_tcp_udp_priority = 21;

    for (j = 0 ; j < nof_units ; j++){
        unit = units_ids[j];

        result = bcm_port_control_set(unit, in_port, bcmPortControlMplsSpeculativeParse, 1);
        if (BCM_E_NONE != result) {
          return result;
        }

        /*********************************/
        /* Build the drop all MPLS packets group */
        /*********************************/
          
        /* Create a presel entity */
        if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
            presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
            result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
            if (BCM_E_NONE != result) {
              printf("Error in bcm_field_presel_create_stage_id\n");
              auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
              return result;
            }
        } else {
            result = bcm_field_presel_create_id(unit, presel_id);
            if (BCM_E_NONE != result) {
              printf("Error in bcm_field_presel_create_id\n");
              auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
              return result;
            }
        }

        result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
        if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_Stage\n");
          return result;
        }

        BCM_FIELD_PRESEL_INIT(psset);
        BCM_FIELD_PRESEL_ADD(psset, presel_id);

        /*
         * Build the qualifier set for the field group.
         */
        BCM_FIELD_QSET_INIT(drop_mpls_group_config.qset);
        BCM_FIELD_QSET_ADD(drop_mpls_group_config.qset, bcmFieldQualifyHeaderFormat);

        /*
         *  Build the action set for the group.
         *	We want to drop all packets except TCP UDP.
         */
        BCM_FIELD_ASET_INIT(drop_mpls_group_config.aset);
        BCM_FIELD_ASET_ADD(drop_mpls_group_config.aset, bcmFieldActionDrop);

        /*
         *  Create the group and set its actions.
         */
        drop_mpls_group_config.preselset = psset;
        drop_mpls_group_config.priority = drop_priority;
        drop_mpls_group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
        drop_mpls_group_config.mode = bcmFieldGroupModeAuto;
        result = bcm_field_group_config_create(unit, &drop_mpls_group_config);
        if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_config_create\n");
          return result;
        }

        /* Create per group: 3 entries for 1/2/3 MPLS labels header */
        for (i=0; i < 6; i++) {
            result = bcm_field_entry_create(unit, drop_mpls_group_config.group, &ent[i]);
            if (BCM_E_NONE != result) {
                printf("Error in bcm_field_entry_create\n");
                auxRes = bcm_field_group_destroy(unit, drop_mpls_group_config.group);
                return result;
            }

            if (i%6==0) {
                result = bcm_field_qualify_HeaderFormat(unit, ent[i], bcmFieldHeaderFormatIp4MplsLabel1);
                if (BCM_E_NONE != result) {
                    printf("Error in bcm_field_qualify_HeaderFormat\n");
                    auxRes = bcm_field_entry_destroy(unit, ent[i]);
                    auxRes = bcm_field_group_destroy(unit, drop_mpls_group_config.group);
                    return result;
                }
            }

            if (i%6==1) {
                result = bcm_field_qualify_HeaderFormat(unit, ent[i], bcmFieldHeaderFormatIp4MplsLabel2);
                if (BCM_E_NONE != result) {
                    printf("Error in bcm_field_qualify_HeaderFormat\n");
                    auxRes = bcm_field_entry_destroy(unit, ent[i]);
                    auxRes = bcm_field_group_destroy(unit, drop_mpls_group_config.group);
                    return result;
                }
            }

            if (i%6==2) {
                result = bcm_field_qualify_HeaderFormat(unit, ent[i], bcmFieldHeaderFormatIp4MplsLabel3);
                if (BCM_E_NONE != result) {
                    printf("Error in bcm_field_qualify_HeaderFormat\n");
                    auxRes = bcm_field_entry_destroy(unit, ent[i]);
                    auxRes = bcm_field_group_destroy(unit, drop_mpls_group_config.group);
                    return result;
                }
            }

            if (i%6==3) {
                result = bcm_field_qualify_HeaderFormat(unit, ent[i], bcmFieldHeaderFormatIp6MplsLabel1);
                if (BCM_E_NONE != result) {
                    printf("Error in bcm_field_qualify_HeaderFormat\n");
                    auxRes = bcm_field_entry_destroy(unit, ent[i]);
                    auxRes = bcm_field_group_destroy(unit, drop_mpls_group_config.group);
                    return result;
                }
            }

            if (i%6==4) {
                result = bcm_field_qualify_HeaderFormat(unit, ent[i], bcmFieldHeaderFormatIp6MplsLabel2);
                if (BCM_E_NONE != result) {
                    printf("Error in bcm_field_qualify_HeaderFormat\n");
                    auxRes = bcm_field_entry_destroy(unit, ent[i]);
                    auxRes = bcm_field_group_destroy(unit, drop_mpls_group_config.group);
                    return result;
                }
            }

            if (i%6==5) {
                result = bcm_field_qualify_HeaderFormat(unit, ent[i], bcmFieldHeaderFormatIp6MplsLabel3);
                if (BCM_E_NONE != result) {
                    printf("Error in bcm_field_qualify_HeaderFormat\n");
                    auxRes = bcm_field_entry_destroy(unit, ent[i]);
                    auxRes = bcm_field_group_destroy(unit, drop_mpls_group_config.group);
                    return result;
                }
            }

            result = bcm_field_action_add(unit, ent[i], bcmFieldActionDrop, 0, 0);
            if (BCM_E_NONE != result) {
                printf("Error in bcm_field_action_add\n");
                auxRes = bcm_field_entry_destroy(unit, ent[i]);
                auxRes = bcm_field_group_destroy(unit, drop_mpls_group_config.group);
                return result;
            }
        }

        /*
         *  Commit the entire group to hardware.
         */
        result = bcm_field_group_install(unit, drop_mpls_group_config.group);
        if (BCM_E_NONE != result) {
            for (i=0; i < 6; i++) {
                auxRes = bcm_field_entry_destroy(unit, ent[i]);
            }
            auxRes = bcm_field_group_destroy(unit, drop_mpls_group_config.group);
            return result;
        }
    }
    return result;
}

