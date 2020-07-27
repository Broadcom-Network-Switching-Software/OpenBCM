/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * C file to put FTFP group & entry related api calls
 */

/* FTFP Group Configuration Variables */
uint8 ftv2_ftfp_group_test_config = 0;
int ftfp_group1 = -1;

/* FTFP Group Entry Configuration Variables */
uint8 ftv2_ftfp_group_entry_test_config = 0;
bcm_field_entry_t ftfp_entry1 = -1;

/*
 * Called after presel entry are created, main FT Group qualifying criteria is specified.
 * Programs following HW memories
 *  - BSK_FTFP_LTS_MUX_CTRL_0
 *  - BSK_FTFP_LTS_MASK_0
 *  - BSK_FTFP_LTS_MUX_CTRL_1
 *  - BSK_FTFP_LTS_MASK_1
 *  - BSK_FTFP_LTS_MUX_CTRL_2_PLUS_MASK
 *  - BSK_FTFP_LTS_LOGICAL_TBL_SEL_TCAM
 *  - BSK_FTFP_LTS_LOGICAL_TBL_SEL_SRAM
 */

/* Create FTFP Group as per ftv2_ftfp_group_test_config */
int
ftv2_fp_group_create_main(uint32 tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    bcm_field_group_config_t ftfp_group_config1;

    /* Skip if no test config set */
    if (!ftv2_ftfp_group_test_config) {
        if (!skip_log) {
            printf("[CINT] Step2 - Skip FTFP Group Create ......\n");
        }
        return rv;
    }

    /* Create FTFP group as per config set */
    switch(ftv2_ftfp_group_test_config) {
        case 1:
            /* ftv2_ftfp_group_test_config = 1 */
            /* Create IPv4 FTFP Group */
            bcm_field_group_config_t_init(&ftfp_group_config1);
            ftfp_group_config1.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
            BCM_FIELD_PRESEL_ADD(ftfp_group_config1.preselset, ftfp_presel_id1);
            BCM_FIELD_PRESEL_ADD(ftfp_group_config1.preselset, ftfp_presel_id2);
            BCM_FIELD_PRESEL_ADD(ftfp_group_config1.preselset, ftfp_presel_id3);
            BCM_FIELD_QSET_INIT(ftfp_group_config1.qset);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyStageIngressFlowtracker);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyIpProtocol);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyOuterVlanId);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyInnerVlanId);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyVlanFormat);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyInPort);
            ftfp_group_config1.mode = bcmFieldGroupModeAuto;
            rv = bcm_field_group_config_create(unit, &ftfp_group_config1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_group_config_create => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            ftfp_group1 = ftfp_group_config1.group;
            if (!skip_log) {
                printf("[CINT] Step2 - FTFP Group Created: 0x%08x\n", ftfp_group1);
            }
            break;

        case 80:
            /* ftv2_ftfp_group_test_config = 80 */
            /* Create IPv6 FTFP Group */
            bcm_field_group_config_t_init(&ftfp_group_config1);
            ftfp_group_config1.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
            BCM_FIELD_PRESEL_ADD(ftfp_group_config1.preselset, ftfp_presel_id1);
            BCM_FIELD_PRESEL_ADD(ftfp_group_config1.preselset, ftfp_presel_id2);
            BCM_FIELD_PRESEL_ADD(ftfp_group_config1.preselset, ftfp_presel_id3);
            BCM_FIELD_QSET_INIT(ftfp_group_config1.qset);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyStageIngressFlowtracker);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyOuterVlanId);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyVlanFormat);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyInPort);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyInnerVlanId);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyIp6PktNextHeader);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyIp6PktHopLimit);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyIp6PktTrafficClass);
            ftfp_group_config1.mode = bcmFieldGroupModeAuto;
            rv = bcm_field_group_config_create(unit, &ftfp_group_config1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_group_config_create => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            ftfp_group1 = ftfp_group_config1.group;
            if (!skip_log) {
                printf("[CINT] Step2 - FTFP Group Created: 0x%08x\n", ftfp_group1);
            }
            break;

        case 90:
            /* ftv2_ftfp_group_test_config = 90 */
            /* Create VXLAN FTFP Group */
            bcm_field_group_config_t_init(&ftfp_group_config1);
            ftfp_group_config1.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
            BCM_FIELD_PRESEL_ADD(ftfp_group_config1.preselset, ftfp_presel_id1);
            BCM_FIELD_QSET_INIT(ftfp_group_config1.qset);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyStageIngressFlowtracker);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyOuterVlanId);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyVlanFormat);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyInnerVlanId);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyIpProtocol);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyVxlanNetworkId);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyVxlanHeaderBits8_31);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyVxlanHeaderBits56_63);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyVxlanFlags);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyInPort);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyInnerSrcIp);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyInnerIpProtocol);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyInnerTtl);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyInnerL4DstPort);
            BCM_FIELD_QSET_ADD(ftfp_group_config1.qset, bcmFieldQualifyInnerL4SrcPort);
            ftfp_group_config1.mode = bcmFieldGroupModeAuto;
            rv = bcm_field_group_config_create(unit, &ftfp_group_config1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_group_config_create => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            ftfp_group1 = ftfp_group_config1.group;
            if (!skip_log) {
                printf("[CINT] Step2 - FTFP Group Created: 0x%08x\n", ftfp_group1);
            }
            break;

        default:
            printf("[CINT] TC %2d - invalid ftfp group test config %d\n",tc_id,ftv2_ftfp_group_test_config);
            return BCM_E_FAIL;
    }

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
        printf("[CINT] Step2 - Dumping relevant memories & registers ..\n");
        bshell(unit, "d chg BSK_FTFP_LTS_MUX_CTRL_0");
        bshell(unit, "d chg BSK_FTFP_LTS_MASK_0");
        bshell(unit, "d chg BSK_FTFP_LTS_MUX_CTRL_1");
        bshell(unit, "d chg BSK_FTFP_LTS_MASK_1");
        bshell(unit, "d chg BSK_FTFP_LTS_MUX_CTRL_2_PLUS_MASK");
        bshell(unit, "d chg BSK_FTFP_LTS_LOGICAL_TBL_SEL_TCAM");
        bshell(unit, "d chg BSK_FTFP_LTS_LOGICAL_TBL_SEL_SRAM");
    }

    /* Dump Diag Command */
    if (dump_diag_shell) {
        printf("[CINT] Step 2 - Dumping diag shell output ..\n");
        bshell(unit, "fp show");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf("[CINT] Step2 - FTFP Group Created Succesfully.\n");
    }

    return BCM_E_NONE;
}

/* Destroy FTFP Groups */
int
ftv2_fp_group_destroy_main(uint32 tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* Skip if no test config set */
    if (!ftv2_ftfp_group_test_config) {
        if (!skip_log) {
            printf("[CINT] Cleanup - Skip FTFP Group Destroy ......\n");
        }
        return rv;
    }

    /* Destroy FTFP group */
    if (ftfp_group1 != -1) {
        rv = bcm_field_group_destroy(unit, ftfp_group1);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_field_group_destroy => rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Cleanup - FTFP Group destroyed: 0x%x\n", ftfp_group1);
        }
        ftfp_group1 = -1;
    }

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
        printf("[CINT] Dumping relevant memories & registers ..\n");
        bshell(unit, "d chg BSK_FTFP_LTS_MUX_CTRL_0");
        bshell(unit, "d chg BSK_FTFP_LTS_MASK_0");
        bshell(unit, "d chg BSK_FTFP_LTS_MUX_CTRL_1");
        bshell(unit, "d chg BSK_FTFP_LTS_MASK_1");
        bshell(unit, "d chg BSK_FTFP_LTS_MUX_CTRL_2_PLUS_MASK");
        bshell(unit, "d chg BSK_FTFP_LTS_LOGICAL_TBL_SEL_TCAM");
        bshell(unit, "d chg BSK_FTFP_LTS_LOGICAL_TBL_SEL_SRAM");
    }

    /* Dump Diag Command */
    if (dump_diag_shell) {
        printf("[CINT] Cleanup - Dumping diag shell output ..\n");
        bshell(unit, "fp show");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf("[CINT] Cleanup - FTFP Group destroyed succesfully.\n");
    }

    return BCM_E_NONE;
}

/* Create FTFP Group entry using ftv2_ftfp_group_entry_test_config
 * HW memory programmed are:
 *  - d chg BSK_SESSION_KEY_LTS_MUX_CTRL_0
 *  - d chg BSK_SESSION_KEY_LTS_MASK_0
 *  - d chg BSK_SESSION_KEY_LTS_MUX_CTRL_1
 *  - d chg BSK_SESSION_KEY_LTS_MASK_1
 *  - d chg BSK_SESSION_KEY_LTS_MUX_CTRL_2_PLUS_MASK
 *  - d chg BSK_SESSION_DATA_LTS_MASK_0
 *  - d chg BSK_SESSION_DATA_LTS_MUX_CTRL_1_PLUS_MASK
 *  - d chg BSK_ALU_DATA_LTS_MUX_CTRL_PLUS_MASK
 *  - d chg BSK_FTFP_COMBO_TCAM_POLICY
 *  - d chg BSK_FTFP_TCAM
 *  - d chg BSK_FTFP_POLICY
 */
int
ftv2_fp_group_entry_install_main(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* Skip this FTFP Entry Create if no test config set */
    if (!ftv2_ftfp_group_entry_test_config) {
        if (!skip_log) {
            printf("[CINT] Step17 - Skip FTFP Entry Create & Install ......\n");
        }
        return rv;
    }

    /* Create & Install FTFP Group Entries as per test config set */
    switch(ftv2_ftfp_group_entry_test_config) {
        case 1:
            /* ftv2_ftfp_group_entry_test_config = 1 */
            /* Create & Install IPv4 FTFP Group Entry */
            rv = bcm_field_entry_create(unit, ftfp_group1, &ftfp_entry1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_entry_create => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_action_add(unit, ftfp_entry1, bcmFieldActionFlowtrackerGroupId, flow_group_id1, 0);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_action_add => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_IpProtocol(unit, ftfp_entry1, pkt_protocol, 0xff);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_PktDstAddrType => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_InPort(unit, ftfp_entry1, traffic_ingress_port, 0xff);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_InPort => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_OuterVlanId(unit, ftfp_entry1, pkt_ingress_vlan, 0xfff);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_OuterVlanId => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_VlanFormat(unit, ftfp_entry1, pkt_vlan_format, 0xf);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_VlanFormat => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_entry_install(unit, ftfp_entry1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_entry_install => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step17 - FTFP Group Entry Installed: 0x%08x\n",ftfp_entry1);
            }
            break;

        case 80:
            /* ftv2_ftfp_group_entry_test_config = 80 */
            /* Create & Install IPv6 FTFP Group Entry */
            rv = bcm_field_entry_create(unit, ftfp_group1, &ftfp_entry1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_entry_create => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_action_add(unit, ftfp_entry1, bcmFieldActionFlowtrackerGroupId, flow_group_id1, 0);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_action_add => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_InPort(unit, ftfp_entry1, traffic_ingress_port, 0xff);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_InPort => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_Ip6PktNextHeader(unit, ftfp_entry1, pkt_protocol, 0xfff);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_Ip6PktNextHeader => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_VlanFormat(unit, ftfp_entry1, pkt_vlan_format, 0xf);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_VlanFormat => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_entry_install(unit, ftfp_entry1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_entry_install => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step17 - FTFP Group Entry Installed: 0x%08x\n",ftfp_entry1);
            }
            break;

        case 90:
            /* ftv2_ftfp_group_entry_test_config = 90 */
            /* Create & Install Vxlan FTFP Group Entry */
            rv = bcm_field_entry_create(unit, ftfp_group1, &ftfp_entry1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_entry_create => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_action_add(unit, ftfp_entry1, bcmFieldActionFlowtrackerGroupId, flow_group_id1, 0);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_action_add => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_IpProtocol(unit, ftfp_entry1, pkt_protocol, 0xff);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_IpProtocol => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_VxlanNetworkId(unit, ftfp_entry1, pkt_in_vnid, 0xffffff);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_VxlanNetworkId => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_entry_install(unit, ftfp_entry1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_entry_install => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step17 - FTFP Group Entry Installed: 0x%08x\n",ftfp_entry1);
            }
            break;

        default:
            printf("[CINT] TC %2d - invalid ftfp group entry test config %d\n",tc_id, ftv2_ftfp_group_entry_test_config);
            return BCM_E_FAIL;
    }

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
        printf("[CINT] Step17 - Dumping relevant memories & registers ..\n");
        bshell(unit, "d chg BSK_SESSION_KEY_LTS_MUX_CTRL_0");
        bshell(unit, "d chg BSK_SESSION_KEY_LTS_MASK_0");
        bshell(unit, "d chg BSK_SESSION_KEY_LTS_MUX_CTRL_1");
        bshell(unit, "d chg BSK_SESSION_KEY_LTS_MASK_1");
        bshell(unit, "d chg BSK_SESSION_KEY_LTS_MUX_CTRL_2_PLUS_MASK");
        bshell(unit, "d chg BSK_SESSION_DATA_LTS_MASK_0");
        bshell(unit, "d chg BSK_SESSION_DATA_LTS_MUX_CTRL_1_PLUS_MASK");
        bshell(unit, "d chg BSK_ALU_DATA_LTS_MUX_CTRL_PLUS_MASK");
        bshell(unit, "d chg BSK_FTFP_COMBO_TCAM_POLICY");
        bshell(unit, "d chg BSK_FTFP_TCAM");
        bshell(unit, "d chg BSK_FTFP_POLICY");
    }

    /* Dump Diag Command */
    if (dump_diag_shell) {
        printf("[CINT] Step17 - Dumping diag shell output ....\n");
        bshell(unit, "fp show");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Step17 - FTFP Group Entry Installed Succesfully.\n");
    }

    return BCM_E_NONE;
}

/*
 * Destroy FTFP Group entries
 */
int
ftv2_fp_group_entry_destroy_main(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* Skip this entry destroy if no test config set */
    if (!ftv2_ftfp_group_entry_test_config) {
        if (!skip_log) {
            printf("[CINT] Cleanup - Skip FTFP Entry Destroy ......\n");
        }
        return rv;
    }

    /* Destroy FTFP Group Entries */
    if (ftfp_entry1 != -1) {
        rv = bcm_field_entry_destroy(unit, ftfp_entry1);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_field_entry_destroy => rv %d(%s)\n",
                                                    tc_id,rv,bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Cleanup - FTFP Group Entry Destroyed: 0x%08x\n",ftfp_entry1);
        }
        ftfp_entry1 = -1;
    }

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
        printf("[CINT] Dumping relevant memories & registers ..\n");
        bshell(unit, "d chg BSK_SESSION_KEY_LTS_MUX_CTRL_0");
        bshell(unit, "d chg BSK_SESSION_KEY_LTS_MASK_0");
        bshell(unit, "d chg BSK_SESSION_KEY_LTS_MUX_CTRL_1");
        bshell(unit, "d chg BSK_SESSION_KEY_LTS_MASK_1");
        bshell(unit, "d chg BSK_SESSION_KEY_LTS_MUX_CTRL_2_PLUS_MASK");
        bshell(unit, "d chg BSK_SESSION_DATA_LTS_MASK_0");
        bshell(unit, "d chg BSK_SESSION_DATA_LTS_MUX_CTRL_1_PLUS_MASK");
        bshell(unit, "d chg BSK_ALU_DATA_LTS_MUX_CTRL_PLUS_MASK");
        bshell(unit, "d chg BSK_FTFP_COMBO_TCAM_POLICY");
        bshell(unit, "d chg BSK_FTFP_TCAM");
        bshell(unit, "d chg BSK_FTFP_POLICY");
    }

    /* Dump Diag Command */
    if (dump_diag_shell) {
        printf("[CINT] Cleanup - Dumping diag shell output ....\n");
        bshell(unit, "fp show");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Cleanup - FTFP Group Entry Destroyed Succesfully.\n");
    }

    return BCM_E_NONE;
}
