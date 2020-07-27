/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * C file to put flowtracker presel related api calls
 */

/* presel configuration variables */
uint8 ftv2_ftfp_presel_test_config = 0;
bcm_field_presel_t ftfp_presel_id1 = -1;
bcm_field_presel_t ftfp_presel_id2 = -1;
bcm_field_presel_t ftfp_presel_id3 = -1;
bcm_field_entry_t presel_eid1 = -1;
bcm_field_entry_t presel_eid2 = -1;
bcm_field_entry_t presel_eid3 = -1;

/*
 * Presel Entry Create does not program any hardware memory.
 * It only stores presel data in software at offsets
 * specified by software.
 */

/* create presel entry as per ftv2_ftfp_presel_test_config */
int
ftv2_fp_presel_create_main(uint32 tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* skip if no test config set */
    if (!ftv2_ftfp_presel_test_config) {
        if (!skip_log) {
            printf("[CINT] Step1 - Skip Presel Entry Create ......\n");
        }
        return rv;
    }

    /* configure presel entries as per test config set */
    switch(ftv2_ftfp_presel_test_config) {
        case 1:
            /* ftv2_ftfp_presel_test_config = 1 */
            /* IPv4 Related Config */
            /* 1st presel entry */
            rv = bcm_field_presel_create(unit, &ftfp_presel_id1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_presel_create => ftfp_presel_id1 0x%08x, rv %d(%s)\n",
                        tc_id,ftfp_presel_id1,rv,bcm_errmsg(rv));
                return rv;
            }
            presel_eid1 = ftfp_presel_id1 | BCM_FIELD_QUALIFY_PRESEL;
            rv = bcm_field_qualify_Stage(unit, presel_eid1, bcmFieldStageIngressFlowtracker);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_Stage => rv %d(%s)\n",
                                                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_L4Ports(unit, presel_eid1, 1, 1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_L4Ports => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_PktDstAddrType(unit, presel_eid1, BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_MAC | BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_IP);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_PktDstAddrType => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_IpType(unit, presel_eid1, bcmFieldIpTypeIpv4NoOpts);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_IpType => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf ("[CINT] Step1 - FTFP Presel 1 Created: 0x%08x\n", ftfp_presel_id1);
            }

            /* 2nd presel entry */
            rv = bcm_field_presel_create(unit, &ftfp_presel_id2);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_presel_create => ftfp_presel_id2 0x%08x, rv %d(%s)\n",
                        tc_id, ftfp_presel_id2, rv, bcm_errmsg(rv));
                return rv;
            }
            presel_eid2 = ftfp_presel_id2 | BCM_FIELD_QUALIFY_PRESEL;
            rv = bcm_field_qualify_Stage(unit, presel_eid2, bcmFieldStageIngressFlowtracker);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_Stage => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_L4Ports(unit, presel_eid2, 1, 1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_L4Ports => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_PktDstAddrType(unit, presel_eid2, BCM_FIELD_PKT_DST_ADDR_TYPE_MCAST_MAC | BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_IP);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_PktDstAddrType => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_IpType(unit, presel_eid2, bcmFieldIpTypeIpv4NoOpts);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_IpType => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf ("[CINT] Step1 - FTFP Presel 2 Created: 0x%08x\n", ftfp_presel_id2);
            }

            /* 3rd presel entry */
            rv = bcm_field_presel_create(unit, &ftfp_presel_id3);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_presel_create => ftfp_presel_id3 0x%08x, rv %d(%s)\n",
                        tc_id,ftfp_presel_id3,rv,bcm_errmsg(rv));
                return rv;
            }
            presel_eid3 = ftfp_presel_id3 | BCM_FIELD_QUALIFY_PRESEL;
            rv = bcm_field_qualify_Stage(unit, presel_eid3, bcmFieldStageIngressFlowtracker);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_Stage => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_L4Ports(unit, presel_eid3, 1, 1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_L4Ports => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_PktDstAddrType(unit, presel_eid3, BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_MAC | BCM_FIELD_PKT_DST_ADDR_TYPE_MCAST_IP);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_PktDstAddrType => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_IpType(unit, presel_eid3, bcmFieldIpTypeIpv4NoOpts);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_IpType => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf ("[CINT] Step1 - FTFP Presel 3 Created: 0x%08x\n", ftfp_presel_id3);
            }
            break;

        case 80:
            /* ftv2_ftfp_presel_test_config = 80 */
            /* IPv6 Related Config */
            /* 1st presel entry */
            rv = bcm_field_presel_create(unit, &ftfp_presel_id1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_presel_create => ftfp_presel_id1 0x%08x, rv %d(%s)\n",
                        tc_id,ftfp_presel_id1,rv,bcm_errmsg(rv));
                return rv;
            }
            presel_eid1 = ftfp_presel_id1 | BCM_FIELD_QUALIFY_PRESEL;
            rv = bcm_field_qualify_Stage(unit, presel_eid1, bcmFieldStageIngressFlowtracker);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_Stage => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_L4Ports(unit, presel_eid1, 1, 1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_L4Ports => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_PktDstAddrType(unit, presel_eid1, BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_MAC | BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_IP);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_PktDstAddrType => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_IpType(unit, presel_eid1, bcmFieldIpTypeIpv6NoExtHdr);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_IpType => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf ("[CINT] Step1 - FTFP Presel 1 Created: 0x%08x\n", ftfp_presel_id1);
            }

            /* 2nd presel entry */
            rv = bcm_field_presel_create(unit, &ftfp_presel_id2);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_presel_create => ftfp_presel_id2 0x%08x, rv %d(%s)\n",
                        tc_id,ftfp_presel_id2,rv,bcm_errmsg(rv));
                return rv;
            }
            presel_eid2 = ftfp_presel_id2 | BCM_FIELD_QUALIFY_PRESEL;
            rv = bcm_field_qualify_Stage(unit, presel_eid2, bcmFieldStageIngressFlowtracker);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_Stage => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_L4Ports(unit, presel_eid2, 1, 1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_L4Ports => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_PktDstAddrType(unit, presel_eid2, BCM_FIELD_PKT_DST_ADDR_TYPE_MCAST_MAC | BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_IP);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_PktDstAddrType => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_IpType(unit, presel_eid2, bcmFieldIpTypeIpv6NoExtHdr);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_IpType => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf ("[CINT] Step1 - FTFP Presel 2 Created: 0x%08x\n", ftfp_presel_id2);
            }

            /* 3rd presel entry */
            rv = bcm_field_presel_create(unit, &ftfp_presel_id3);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_presel_create => ftfp_presel_id3 0x%08x, rv %d(%s)\n",
                        tc_id,ftfp_presel_id3,rv,bcm_errmsg(rv));
                return rv;
            }
            presel_eid3 = ftfp_presel_id3 | BCM_FIELD_QUALIFY_PRESEL;
            rv = bcm_field_qualify_Stage(unit, presel_eid3, bcmFieldStageIngressFlowtracker);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_Stage => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_L4Ports(unit, presel_eid3, 1, 1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_L4Ports => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_PktDstAddrType(unit, presel_eid3, BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_MAC | BCM_FIELD_PKT_DST_ADDR_TYPE_MCAST_IP);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_PktDstAddrType => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_IpType(unit, presel_eid3, bcmFieldIpTypeIpv6NoExtHdr);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_IpType => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf ("[CINT] Step1 - FTFP Presel 3 Created: 0x%08x\n", ftfp_presel_id3);
            }
            break;

        case 90:
            /* VXLAN Presel Config */
            /* 1st presel entry */
            rv = bcm_field_presel_create(unit, &ftfp_presel_id1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_presel_create => ftfp_presel_id1 0x%08x, rv %d(%s)\n",
                        tc_id,ftfp_presel_id1,rv,bcm_errmsg(rv));
                return rv;
            }
            presel_eid1 = ftfp_presel_id1 | BCM_FIELD_QUALIFY_PRESEL;
            rv = bcm_field_qualify_Stage(unit, presel_eid1, bcmFieldStageIngressFlowtracker);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_Stage => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            rv = bcm_field_qualify_PktType(unit, presel_eid1, bcmFieldPktTypeIp4Vxlan);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_field_qualify_PktType => rv %d(%s)\n",
                        tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf ("[CINT] Step1 - FTFP Presel 1 Created: 0x%08x\n", ftfp_presel_id1);
            }
            break;

        default:
            printf("[CINT] TC %2d - invalid presel test config %d\n",tc_id,ftv2_ftfp_presel_test_config);
            return BCM_E_FAIL;
    }

    /* dump diag command */
    if (dump_diag_shell) {
        printf("[CINT] Step1 - Dumping diag shell output ..\n");
        bshell(unit, "fp show");
    }

    /* final step log */
    if (!skip_log) {
        printf ("[CINT] Step1 - Presel Entries Configured Succesfully.\n");
    }

    return BCM_E_NONE;
}

/* destroy presel entries */
int
ftv2_fp_presel_destroy_main(uint32 tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* skip if no test config is set */
    if (!ftv2_ftfp_presel_test_config) {
        if (!skip_log) {
            printf("[CINT] Cleanup - Skip Presel Entry Destroy ......\n");
        }
        return rv;
    }

    /* destroy 1st presel entry */
    if (ftfp_presel_id1 != -1) {
        rv = bcm_field_presel_destroy(unit, ftfp_presel_id1);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_field_presel_destroy => ftfp_presel_id1 0x%08x, rv %d(%s)\n",
                                                             tc_id,ftfp_presel_id1,rv,bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Cleanup - FTFP Presel 1 Destroyed: 0x%08x\n", ftfp_presel_id1);
        }
        ftfp_presel_id1 = -1;
        presel_eid1 = -1;
    }

    /* destroy 2nd presel entry */
    if (ftfp_presel_id2 != -1) {
        rv = bcm_field_presel_destroy(unit, ftfp_presel_id2);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_field_presel_destroy => ftfp_presel_id2 0x%08x, rv %d(%s)\n",
                                                             tc_id,ftfp_presel_id2,rv,bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Cleanup - FTFP Presel 2 Destroyed: 0x%08x\n", ftfp_presel_id2);
        }
        ftfp_presel_id2 = -1;
        presel_eid2 = -1;
    }

    /* destroy 3rd presel entry */
    if (ftfp_presel_id3 != -1) {
        rv = bcm_field_presel_destroy(unit, ftfp_presel_id3);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_field_presel_destroy => ftfp_presel_id3 0x%08x, rv %d(%s)\n",
                                                             tc_id,ftfp_presel_id3,rv,bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Cleanup - FTFP Presel 3 Destroyed: 0x%08x\n", ftfp_presel_id3);
        }
        ftfp_presel_id3 = -1;
        presel_eid3 = -1;
    }

    /* dump diag command */
    if (dump_diag_shell) {
        printf("[CINT] Cleanup - Dumping diag shell output ..\n");
        bshell(unit, "fp show");
    }

    /* final step log */
    if (!skip_log) {
        printf ("[CINT] Cleanup - Presel Entries Destroyed Succesfully.\n");
    }

    return BCM_E_NONE;
}
