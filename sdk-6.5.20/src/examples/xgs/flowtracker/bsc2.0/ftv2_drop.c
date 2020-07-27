/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * C file to put flowtracker drop reason related api calls
 */

/* Drop Reason Group Variables */
uint8 ftv2_drop_test_config = 0;
bcm_flowtracker_drop_reason_group_t ftv2_ingress_dr_id1 = -1;
bcm_flowtracker_drop_reason_group_t ftv2_ingress_dr_id2 = -1;
bcm_flowtracker_drop_reason_group_t ftv2_egress_dr_id1 = -1;
bcm_flowtracker_drop_reason_group_t ftv2_egress_dr_id2 = -1;

/* Expected Ingress/Mmu Drop Vector */
uint16 expected_ing_drop_vector_value = 0;

/* Expected Egress Drop Vector */
uint16 expected_egr_drop_vector_value = 0;

/* Create drop reason group entry as per ftv2_drop_test_config */
int
ftv2_drop_reason_group_create_main(uint32 tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    int i = 0;
    bcm_flowtracker_drop_reasons_t ingress_mmu_dr1;
    bcm_flowtracker_drop_reasons_t ingress_mmu_dr2;
    bcm_flowtracker_drop_reasons_t egress_dr1;

    /* Skip if no test config is set */
    if (!ftv2_drop_test_config) {
        if (!skip_log) {
            printf("[CINT] Step11 - Skip Drop Reason Group Create ......\n");
        }
        return rv;
    }

    /* Configure drop reason entries as per test config set */
    switch(ftv2_drop_test_config) {
        case 13:
            /* ftv2_drop_test_config = 13 */
            BCM_FLOWTRACKER_DROP_REASON_ING_MMU_SET_ALL(ingress_mmu_dr1);
            rv = bcm_flowtracker_drop_reason_group_create(unit, 0, ingress_mmu_dr1, &ftv2_ingress_dr_id1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ingress_mmu bcm_flowtracker_drop_reason_group_create rv %d(%s)\n",
                                                                               tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(ingress_mmu_dr2);
            BCM_FLOWTRACKER_DROP_REASON_SET(ingress_mmu_dr2, bcmFlowtrackerDropReasonIngMacSAEqualsDA);
            rv = bcm_flowtracker_drop_reason_group_create(unit, 0, ingress_mmu_dr2, &ftv2_ingress_dr_id2);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ingress mmu bcm_flowtracker_drop_reason_group_create rv %d(%s)\n",
                                                                                tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            expected_ing_drop_vector_value = 0x3;
            break;

        case 14:
            /* ftv2_drop_test_config = 14 */
            BCM_FLOWTRACKER_DROP_REASON_ING_MMU_SET_ALL(ingress_mmu_dr1);
            rv = bcm_flowtracker_drop_reason_group_create(unit, 0, ingress_mmu_dr1, &ftv2_ingress_dr_id1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ingress_mmu bcm_flowtracker_drop_reason_group_create rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(ingress_mmu_dr2);
            BCM_FLOWTRACKER_DROP_REASON_SET(ingress_mmu_dr2, bcmFlowtrackerDropReasonIngMacSAEqualsDA);
            rv = bcm_flowtracker_drop_reason_group_create(unit, 0, ingress_mmu_dr2, &ftv2_ingress_dr_id2);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - ingress mmu bcm_flowtracker_drop_reason_group_create rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            expected_ing_drop_vector_value = 0x3;
            break;

        case 16:
            /* ftv2_drop_test_config = 16 */
            BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(egress_dr1);
            BCM_FLOWTRACKER_DROP_REASON_SET(egress_dr1, bcmFlowtrackerDropReasonEgrFieldProcessor);
            rv = bcm_flowtracker_drop_reason_group_create(unit, 0, egress_dr1, &ftv2_egress_dr_id1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - egress bcm_flowtracker_drop_reason_group_create rv %d(%s)\n",
                                                                           tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            expected_egr_drop_vector_value = 0x1;
            break;

        case 17:
            /* ftv2_drop_test_config = 17 */
            BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(egress_dr1);
            BCM_FLOWTRACKER_DROP_REASON_SET(egress_dr1, bcmFlowtrackerDropReasonEgrFieldProcessor);
            rv = bcm_flowtracker_drop_reason_group_create(unit, 0, egress_dr1, &ftv2_egress_dr_id1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - egress bcm_flowtracker_drop_reason_group_create rv %d(%s)\n",
                                                                           tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            expected_egr_drop_vector_value = 0x1;
            break;

        default:
            printf("[CINT] TC %2d - invalid drop test config %d\n", tc_id, ftv2_drop_test_config);
            return BCM_E_FAIL;
    }

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
        bshell(unit, "d chg BSC_DT_DROP_MASK_1");
        bshell(unit, "d chg BSC_DT_DROP_MASK_2");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Step11 - Drop Reason Groups Created succesfully.\n");
    }

    return BCM_E_NONE;
}

/* Destroy drop reason group entries */
int
ftv2_drop_reason_group_destroy_main(uint32 tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* Skip if no test config is set */
    if (!ftv2_drop_test_config) {
        if (!skip_log) {
            printf("[CINT] Cleanup - Skip drop reason group destroy ......\n");
        }
        return rv;
    }

    expected_ing_drop_vector_value = 0;
    expected_egr_drop_vector_value = 0;

    /* Destroy 1st drop reason group id entry */
    if (ftv2_ingress_dr_id1 != -1) {
        rv = bcm_flowtracker_drop_reason_group_delete(unit, ftv2_ingress_dr_id1);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - ingress_mmu bcm_flowtracker_drop_reason_group_delete rv %d(%s)\n",
                    tc_id, rv, bcm_errmsg(rv));
            return rv;
        }
        ftv2_ingress_dr_id1 = -1;
    }

    /* Destroy 2nd drop reason group id entry */
    if (ftv2_ingress_dr_id2 != -1) {
        rv = bcm_flowtracker_drop_reason_group_delete(unit, ftv2_ingress_dr_id2);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - ingress_mmu bcm_flowtracker_drop_reason_group_delete rv %d(%s)\n",
                    tc_id, rv, bcm_errmsg(rv));
            return rv;
        }
        ftv2_ingress_dr_id2 = -1;
    }

    /* Destroy 2nd drop reason group id entry */
    if (ftv2_egress_dr_id1 != -1) {
        rv = bcm_flowtracker_drop_reason_group_delete(unit, ftv2_egress_dr_id1);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - egress bcm_flowtracker_drop_reason_group_delete rv %d(%s)\n",
                                                                        tc_id, rv, bcm_errmsg(rv));
            return rv;
        }
        ftv2_egress_dr_id1 = -1;
    }

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
        bshell(unit, "d chg BSC_DT_DROP_MASK_1");
        bshell(unit, "d chg BSC_DT_DROP_MASK_2");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Cleanup - Drop Reason Groups destroyed succesfully.\n");
    }

    return BCM_E_NONE;
}
