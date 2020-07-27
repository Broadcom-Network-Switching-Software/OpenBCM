/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * C file to check flowtracker apis param sanity
 */

/* param sanity check variables */
uint8 ftv2_param_sanity_test_config = 0;
uint8 max_ing_dr_group = 16;
uint8 max_egr_dr_group = 16;

/* api to validate group reason group api's */
int
ftv2_param_sanity_drop_reason(uint32 tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE, idx = 0;
    bcm_flowtracker_drop_reason_group_t ftv2_ing_dr_id1 = -1;
    bcm_flowtracker_drop_reason_group_t ftv2_ing_dr_id2 = -1;
    bcm_flowtracker_drop_reason_group_t ftv2_egr_dr_id1 = -1;
    bcm_flowtracker_drop_reason_group_t ftv2_egr_dr_id2 = -1;
    bcm_flowtracker_drop_reasons_t ing_mmu_dr1;
    bcm_flowtracker_drop_reasons_t egr_dr1;
    bcm_flowtracker_drop_reasons_t ing_mmu_dr1_get;
    bcm_flowtracker_drop_reasons_t egr_dr1_get;
    bcm_flowtracker_drop_reasons_t ing_mmu_dr2;
    bcm_flowtracker_drop_reasons_t egr_dr2;
    bcm_flowtracker_drop_reasons_t ing_mmu_dr2_get;
    bcm_flowtracker_drop_reasons_t egr_dr2_get;
    bcm_flowtracker_drop_reason_group_t ftv2_ing_dr_id3[max_ing_dr_group + 1] = { -1 };
    bcm_flowtracker_drop_reason_group_t ftv2_egr_dr_id3[max_egr_dr_group + 1] = { -1 };

    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(ing_mmu_dr1);
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(egr_dr1);
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(ing_mmu_dr1_get);
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(egr_dr1_get);
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(ing_mmu_dr2);
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(egr_dr2);
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(ing_mmu_dr2_get);
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(egr_dr2_get);

    /* usecase1 - check ingress/mmu group1 gets created succesfully */
    /* bcmFlowtrackerDropReasonMmuEgressMCPoolRed - 43,
     * bcmFlowtrackerDropReasonIngSpanningTreeNotForwarding - 113,
     * bcmFlowtrackerDropReasonIngL3Disable - 123
     */
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(ing_mmu_dr1);
    BCM_FLOWTRACKER_DROP_REASON_SET(ing_mmu_dr1, bcmFlowtrackerDropReasonMmuEgressMCPoolRed);
    BCM_FLOWTRACKER_DROP_REASON_SET(ing_mmu_dr1, bcmFlowtrackerDropReasonIngSpanningTreeNotForwarding);
    BCM_FLOWTRACKER_DROP_REASON_SET(ing_mmu_dr1, bcmFlowtrackerDropReasonIngL3Disable);
    rv = bcm_flowtracker_drop_reason_group_create(unit, 0, ing_mmu_dr1, &ftv2_ing_dr_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase1 - bcm_flowtracker_drop_reason_group_create => ftv2_ing_dr_id1 0x%08x, rv %d(%s)\n",
                                                                         tc_id, ftv2_ing_dr_id1, rv, bcm_errmsg(rv));
        return rv;
    }

    /* usecase2 - check egress group1 gets created succesfully */
    /* bcmFlowtrackerDropReasonEgrEcnTable - 25,
     * bcmFlowtrackerDropReasonEgrProtection -32
     */
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(egr_dr1);
    BCM_FLOWTRACKER_DROP_REASON_SET(egr_dr1, bcmFlowtrackerDropReasonEgrEcnTable);
    BCM_FLOWTRACKER_DROP_REASON_SET(egr_dr1, bcmFlowtrackerDropReasonEgrProtection);
    rv = bcm_flowtracker_drop_reason_group_create(unit, 0, egr_dr1, &ftv2_egr_dr_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 - bcm_flowtracker_drop_reason_group_create => ftv2_egr_dr_id1 0x%08x, rv %d(%s)\n",
                                                                         tc_id, ftv2_egr_dr_id1, rv, bcm_errmsg(rv));
        return rv;
    }

    /* usecase3 - check ingress/mmu group2 gets created succesfully */
    /* bcmFlowtrackerDropReasonMmuIngressPortPG - 1,
     * bcmFlowtrackerDropReasonMmuIngressPortRed - 4
     */
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(ing_mmu_dr2);
    BCM_FLOWTRACKER_DROP_REASON_SET(ing_mmu_dr2, bcmFlowtrackerDropReasonMmuIngressPortPG);
    BCM_FLOWTRACKER_DROP_REASON_SET(ing_mmu_dr2, bcmFlowtrackerDropReasonMmuIngressPortRed);
    rv = bcm_flowtracker_drop_reason_group_create(unit, 0, ing_mmu_dr2, &ftv2_ing_dr_id2);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 - bcm_flowtracker_drop_reason_group_create => ftv2_ing_dr_id2 0x%08x, rv %d(%s)\n",
                                                                         tc_id, ftv2_ing_dr_id2, rv, bcm_errmsg(rv));
        return rv;
    }

    /* usecase4 - check egress group2 gets created succesfully */
    /* bcmFlowtrackerDropReasonEgrAdapt1Miss - 1,
     * bcmFlowtrackerDropReasonEgrFwdDomainNotMember - 4
     */
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(egr_dr2);
    BCM_FLOWTRACKER_DROP_REASON_SET(egr_dr2, bcmFlowtrackerDropReasonEgrAdapt1Miss);
    BCM_FLOWTRACKER_DROP_REASON_SET(egr_dr2, bcmFlowtrackerDropReasonEgrFwdDomainNotMember);
    rv = bcm_flowtracker_drop_reason_group_create(unit, 0, egr_dr2, &ftv2_egr_dr_id2);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase4 - bcm_flowtracker_drop_reason_group_create => ftv2_egr_dr_id2 0x%08x, rv %d(%s)\n",
                                                                                 tc_id, ftv2_egr_dr_id2, rv, bcm_errmsg(rv));
        return rv;
    }

    /* usecase5 - ingress/mmu group1 get succesfully */
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(ing_mmu_dr1_get);
    rv = bcm_flowtracker_drop_reason_group_get(unit, ftv2_ing_dr_id1, &ing_mmu_dr1_get);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase5 - bcm_flowtracker_drop_reason_group_get => ftv2_ing_dr_id1 0x%08x, rv %d(%s)\n",
                                                                     tc_id, ftv2_ing_dr_id1, rv, bcm_errmsg(rv));
        return rv;
    }
    if (!BCM_FLOWTRACKER_DROP_REASON_GET(ing_mmu_dr1_get, bcmFlowtrackerDropReasonMmuEgressMCPoolRed)) {
        printf("[CINT] TC %2d - usecase5 - drop reason bcmFlowtrackerDropReasonMmuEgressMCPoolRed is not set for ftv2_ing_dr_id1 0x%08x\n",
                                                                                                          tc_id, ftv2_ing_dr_id1);
        return BCM_E_FAIL;
    }
    if (!BCM_FLOWTRACKER_DROP_REASON_GET(ing_mmu_dr1_get, bcmFlowtrackerDropReasonIngSpanningTreeNotForwarding)) {
        printf("[CINT] TC %2d - uscase5 - drop reason bcmFlowtrackerDropReasonIngSpanningTreeNotForwarding is not set for ftv2_ing_dr_id1 0x%08x\n",
                                                                                                           tc_id, ftv2_ing_dr_id1);
        return BCM_E_FAIL;
    }
    if (!BCM_FLOWTRACKER_DROP_REASON_GET(ing_mmu_dr1_get, bcmFlowtrackerDropReasonIngL3Disable)) {
        printf("[CINT] TC %2d - usecase5 - drop reason bcmFlowtrackerDropReasonIngL3Disable is not set for ftv2_ing_dr_id1 0x%08x\n",
                                                                                                           tc_id, ftv2_ing_dr_id1);
        return BCM_E_FAIL;
    }

    /* usecase6 - check egress group1 get succesfully */
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(egr_dr1_get);
    rv = bcm_flowtracker_drop_reason_group_get(unit, ftv2_egr_dr_id1, &egr_dr1_get);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase6 - bcm_flowtracker_drop_reason_group_get => ftv2_egr_dr_id1 0x%08x, rv %d(%s)\n",
                                                                      tc_id, ftv2_egr_dr_id1, rv, bcm_errmsg(rv));
        return rv;
    }
    if (!BCM_FLOWTRACKER_DROP_REASON_GET(egr_dr1_get, bcmFlowtrackerDropReasonEgrEcnTable)) {
        printf("[CINT] TC %2d - usecase6 - drop reason bcmFlowtrackerDropReasonEgrEcnTable is not set for ftv2_egr_dr_id1 0x%08x\n",
                                                                                                   tc_id, ftv2_egr_dr_id1);
        return BCM_E_FAIL;
    }
    if (!BCM_FLOWTRACKER_DROP_REASON_GET(egr_dr1_get, bcmFlowtrackerDropReasonEgrProtection)) {
        printf("[CINT] TC %2d - usecase6 - drop reason bcmFlowtrackerDropReasonEgrProtection is not set for ftv2_egr_dr_id1 0x%08x\n",
                                                                                                     tc_id, ftv2_egr_dr_id1);
        return BCM_E_FAIL;
    }

    /* usecase7 - check ingress/mmu group2 get succesfully */
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(ing_mmu_dr2_get);
    rv = bcm_flowtracker_drop_reason_group_get(unit, ftv2_ing_dr_id2, &ing_mmu_dr2_get);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase7 - bcm_flowtracker_drop_reason_group_get => ftv2_ing_dr_id2 0x%08x, rv %d(%s)\n",
                                                                     tc_id, ftv2_ing_dr_id2, rv, bcm_errmsg(rv));
        return rv;
    }
    if (!BCM_FLOWTRACKER_DROP_REASON_GET(ing_mmu_dr2_get, bcmFlowtrackerDropReasonMmuIngressPortPG)) {
        printf("[CINT] TC %2d - usecase7 - drop reason bcmFlowtrackerDropReasonMmuIngressPortPG is not set for ftv2_ing_dr_id2 0x%08x\n",
                                                                                                        tc_id, ftv2_ing_dr_id2);
        return BCM_E_FAIL;
    }
    if (!BCM_FLOWTRACKER_DROP_REASON_GET(ing_mmu_dr2_get, bcmFlowtrackerDropReasonMmuIngressPortRed)) {
        printf("[CINT] TC %2d - usecase7 - drop reason bcmFlowtrackerDropReasonMmuIngressPortRed is not set for ftv2_ing_dr_id2 0x%08x\n",
                                                                                                         tc_id, ftv2_ing_dr_id2);
        return BCM_E_FAIL;
    }

    /* usecase8 - check egress group2 get succesfully */
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(egr_dr2_get);
    rv = bcm_flowtracker_drop_reason_group_get(unit, ftv2_egr_dr_id2, &egr_dr2_get);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase8 - bcm_flowtracker_drop_reason_group_get => ftv2_egr_dr_id2 0x%08x, rv %d(%s)\n",
                                                                      tc_id, ftv2_egr_dr_id2, rv, bcm_errmsg(rv));
        return rv;
    }
    if (!BCM_FLOWTRACKER_DROP_REASON_GET(egr_dr2_get, bcmFlowtrackerDropReasonEgrAdapt1Miss)) {
        printf("[CINT] TC %2d - usecase8 - drop reason bcmFlowtrackerDropReasonEgrAdapt1Miss is not set for ftv2_egr_dr_id2 0x%08x\n",
                                                                                                    tc_id, ftv2_egr_dr_id2);
        return BCM_E_FAIL;
    }
    if (!BCM_FLOWTRACKER_DROP_REASON_GET(egr_dr2_get, bcmFlowtrackerDropReasonEgrFwdDomainNotMember)) {
        printf("[CINT] TC %2d - usecase8 - drop reason bcmFlowtrackerDropReasonEgrFwdDomainNotMember is not set for ftv2_egr_dr_id2 0x%08x\n",
                                                                                                            tc_id, ftv2_egr_dr_id2);
        return BCM_E_FAIL;
    }

    /* usecase9 - check ingress/mmu group1 gets destroyed succesfully */
    rv = bcm_flowtracker_drop_reason_group_delete(unit, ftv2_ing_dr_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase9 - bcm_flowtracker_drop_reason_group_delete => ftv2_ing_dr_id1 0x%08x, rv %d(%s)\n",
                                                                         tc_id, ftv2_ing_dr_id1, rv, bcm_errmsg(rv));
        return rv;
    }
    ftv2_ing_dr_id1 = -1;

    /* usecase10 - check egress group1 gets destroyed succesfully */
    rv = bcm_flowtracker_drop_reason_group_delete(unit, ftv2_egr_dr_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase10 - bcm_flowtracker_drop_reason_group_delete => ftv2_egr_dr_id1 0x%08x, rv %d(%s)\n",
                                                                         tc_id, ftv2_egr_dr_id1, rv, bcm_errmsg(rv));
        return rv;
    }
    ftv2_egr_dr_id1 = -1;

    /* usecase11 - check ingress/mmu group2 gets destroyed succesfully */
    rv = bcm_flowtracker_drop_reason_group_delete(unit, ftv2_ing_dr_id2);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase11 - bcm_flowtracker_drop_reason_group_delete => ftv2_ing_dr_id2 0x%08x, rv %d(%s)\n",
                                                                         tc_id, ftv2_ing_dr_id2, rv, bcm_errmsg(rv));
        return rv;
    }
    ftv2_ing_dr_id2 = -1;

    /* usecase12 - check egress group2 gets destroyed succesfully */
    rv = bcm_flowtracker_drop_reason_group_delete(unit, ftv2_egr_dr_id2);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase12 - bcm_flowtracker_drop_reason_group_delete => ftv2_egr_dr_id2 0x%08x, rv %d(%s)\n",
                                                                         tc_id, ftv2_egr_dr_id2, rv, bcm_errmsg(rv));
        return rv;
    }
    ftv2_egr_dr_id2 = -1;

    /* usecase13 - check mix of reason code among stages is not allowed */
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(ing_mmu_dr1);
    BCM_FLOWTRACKER_DROP_REASON_SET(ing_mmu_dr1, bcmFlowtrackerDropReasonMmuEgressMCPoolRed);
    BCM_FLOWTRACKER_DROP_REASON_SET(ing_mmu_dr1, bcmFlowtrackerDropReasonIngSpanningTreeNotForwarding);
    BCM_FLOWTRACKER_DROP_REASON_SET(ing_mmu_dr1, bcmFlowtrackerDropReasonIngL3Disable);
    BCM_FLOWTRACKER_DROP_REASON_SET(ing_mmu_dr1, bcmFlowtrackerDropReasonEgrEcnTable);
    rv = bcm_flowtracker_drop_reason_group_create(unit, 0, ing_mmu_dr1, &ftv2_ing_dr_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - usecase13 - bcm_flowtracker_drop_reason_group_create => ftv2_ing_dr_id1 0x%08x, rv %d(%s)\n",
                                                                         tc_id, ftv2_ing_dr_id1, rv, bcm_errmsg(rv));
        return BCM_E_FAIL;
    }

    /* usecase14 - check mix of reason code among stages is not allowed */
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(egr_dr1);
    BCM_FLOWTRACKER_DROP_REASON_SET(egr_dr1, bcmFlowtrackerDropReasonEgrEcnTable);
    BCM_FLOWTRACKER_DROP_REASON_SET(egr_dr1, bcmFlowtrackerDropReasonEgrProtection);
    BCM_FLOWTRACKER_DROP_REASON_SET(egr_dr1, bcmFlowtrackerDropReasonIngL3Disable);
    rv = bcm_flowtracker_drop_reason_group_create(unit, 0, egr_dr1, &ftv2_egr_dr_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - usecase14 - bcm_flowtracker_drop_reason_group_create => ftv2_egr_dr_id1 0x%08x, rv %d(%s)\n",
                                                                         tc_id, ftv2_egr_dr_id1, rv, bcm_errmsg(rv));
        return BCM_E_FAIL;
    }

    /* usecase15 - create max ing group */
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(ing_mmu_dr1);
    BCM_FLOWTRACKER_DROP_REASON_SET(ing_mmu_dr1, bcmFlowtrackerDropReasonMmuEgressMCPoolRed);
    BCM_FLOWTRACKER_DROP_REASON_SET(ing_mmu_dr1, bcmFlowtrackerDropReasonIngSpanningTreeNotForwarding);
    BCM_FLOWTRACKER_DROP_REASON_SET(ing_mmu_dr1, bcmFlowtrackerDropReasonIngL3Disable);
    for (idx = 0; idx < max_ing_dr_group; idx++) {
        rv = bcm_flowtracker_drop_reason_group_create(unit, 0, ing_mmu_dr1, &ftv2_ing_dr_id3[idx]);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - usecase15 - bcm_flowtracker_drop_reason_group_create => ftv2_ing_dr_id3 0x%08x, rv %d(%s)\n",
                                                                        tc_id, ftv2_ing_dr_id3[idx], rv, bcm_errmsg(rv));
            return rv;
        }
    }
    /* usecase16 - try creating one more should throw error */
    rv = bcm_flowtracker_drop_reason_group_create(unit, 0, ing_mmu_dr1, &ftv2_ing_dr_id3[idx]);
    if (rv != BCM_E_RESOURCE) {
        printf("[CINT] TC %2d - usecase16 - bcm_flowtracker_drop_reason_group_create => ftv2_ing_dr_id3 0x%08x, rv %d(%s)\n",
                                                                    tc_id, ftv2_ing_dr_id3[idx], rv, bcm_errmsg(rv));
        return BCM_E_FAIL;
    }

    /* usecase17 - create max egress group */
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(egr_dr1);
    BCM_FLOWTRACKER_DROP_REASON_SET(egr_dr1, bcmFlowtrackerDropReasonEgrEcnTable);
    BCM_FLOWTRACKER_DROP_REASON_SET(egr_dr1, bcmFlowtrackerDropReasonEgrProtection);
    for (idx = 0; idx < max_egr_dr_group; idx++) {
        rv = bcm_flowtracker_drop_reason_group_create(unit, 0, egr_dr1, &ftv2_egr_dr_id3[idx]);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - usecase17 - bcm_flowtracker_drop_reason_group_create => ftv2_egr_dr_id3 0x%08x, rv %d(%s)\n",
                                                                        tc_id, ftv2_egr_dr_id3[idx], rv, bcm_errmsg(rv));
            return rv;
        }
    }
    /* usecase18 - try creating one more should throw error */
    rv = bcm_flowtracker_drop_reason_group_create(unit, 0, egr_dr1, &ftv2_egr_dr_id3[idx]);
    if (rv != BCM_E_RESOURCE) {
        printf("[CINT] TC %2d - usecase18 - bcm_flowtracker_drop_reason_group_create => ftv2_egr_dr_id3 0x%08x, rv %d(%s)\n",
                                                                    tc_id, ftv2_egr_dr_id3[idx], rv, bcm_errmsg(rv));
        return BCM_E_FAIL;
    }

    /* usecase19 - destroy all ingress & egress groups created */
    for (idx = 0; idx < max_ing_dr_group; idx++) {
        rv = bcm_flowtracker_drop_reason_group_delete(unit, ftv2_ing_dr_id3[idx]);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - usecase19 - bcm_flowtracker_drop_reason_group_delete => ftv2_ing_dr_id3 0x%08x, rv %d(%s)\n",
                                                                        tc_id, ftv2_ing_dr_id3[idx], rv, bcm_errmsg(rv));
            return rv;
        }
        ftv2_ing_dr_id3[idx] = -1;
    }
    for (idx = 0; idx < max_egr_dr_group; idx++) {
        rv = bcm_flowtracker_drop_reason_group_delete(unit, ftv2_egr_dr_id3[idx]);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - usecase19 - bcm_flowtracker_drop_reason_group_delete => ftv2_egr_dr_id3 0x%08x, rv %d(%s)\n",
                                                                        tc_id, ftv2_egr_dr_id3[idx], rv, bcm_errmsg(rv));
            return rv;
        }
        ftv2_egr_dr_id3[idx] = -1;
    }

    /* usecase20 - setall ingress drop reasons */
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(ing_mmu_dr1);
    BCM_FLOWTRACKER_DROP_REASON_ING_MMU_SET_ALL(ing_mmu_dr1);
    rv = bcm_flowtracker_drop_reason_group_create(unit, 0, ing_mmu_dr1, &ftv2_ing_dr_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase20 - bcm_flowtracker_drop_reason_group_create => ftv2_ing_dr_id1 0x%08x, rv %d(%s)\n",
                                                                                  tc_id, ftv2_ing_dr_id1, rv, bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_drop_reason_group_delete(unit, ftv2_ing_dr_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase20 - bcm_flowtracker_drop_reason_group_delete => ftv2_ing_dr_id1 0x%08x, rv %d(%s)\n",
                                                                                  tc_id, ftv2_ing_dr_id1, rv, bcm_errmsg(rv));
        return rv;
    }
    ftv2_ing_dr_id1 = -1;

    /* usecase21 - setall egress drop reasons */
    BCM_FLOWTRACKER_DROP_REASON_CLEAR_ALL(egr_dr1);
    BCM_FLOWTRACKER_DROP_REASON_EGR_SET_ALL(egr_dr1);
    rv = bcm_flowtracker_drop_reason_group_create(unit, 0, egr_dr1, &ftv2_egr_dr_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase21 - bcm_flowtracker_drop_reason_group_create => ftv2_egr_dr_id1 0x%08x, rv %d(%s)\n",
                                                                                  tc_id, ftv2_egr_dr_id1, rv, bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_drop_reason_group_delete(unit, ftv2_egr_dr_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase21 - bcm_flowtracker_drop_reason_group_delete => ftv2_egr_dr_id1 0x%08x, rv %d(%s)\n",
                                                                                  tc_id, ftv2_egr_dr_id1, rv, bcm_errmsg(rv));
        return rv;
    }
    ftv2_egr_dr_id1 = -1;

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
        bshell(unit, "d chg BSC_DT_DROP_MASK_1");
        bshell(unit, "d chg BSC_DT_DROP_MASK_2");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Drop Reason Group API's param sanity tested succesfully.\n");
    }

    return BCM_E_NONE;
}

int
ftv2_param_sanity_check(uint32 tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE, iter = 0;
    uint32 optionschk = 0;
    bcm_flowtracker_check_t chk_pri_id1 = -1;
    bcm_flowtracker_check_t chk_sec_id1 = -1;
    bcm_flowtracker_check_info_t chk_pri_info1;
    bcm_flowtracker_check_info_t chk_pri_info1_get;
    bcm_flowtracker_check_info_t chk_sec_info1;
    bcm_flowtracker_check_info_t chk_sec_info1_get;
    bcm_flowtracker_check_action_info_t chk_action_info1;
    bcm_flowtracker_check_action_info_t chk_action_info1_get;
    bcm_flowtracker_check_export_info_t chk_export_info1;
    bcm_flowtracker_check_export_info_t chk_export_info1_get;
    bcm_flowtracker_check_delay_info_t chk_delay_info1;
    bcm_flowtracker_check_delay_info_t chk_delay_info1_get;

    /* test1 - check all api set/get/destroy calls with valid params */
    optionschk = 0;
    bcm_flowtracker_check_info_t_init(&chk_pri_info1);
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeSrcIPv4;
    chk_pri_info1.min_value = 10;
    chk_pri_info1.max_value = 20;
    chk_pri_info1.mask_value = 0x0000FFFF;
    chk_pri_info1.shift_value = 5;
    chk_pri_info1.operation = bcmFlowtrackerCheckOpEqual;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test1 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    optionschk = BCM_FLOWTRACKER_CHECK_SECOND;
    bcm_flowtracker_check_info_t_init(&chk_sec_info1);
    chk_sec_info1.param = bcmFlowtrackerTrackingParamTypeDstIPv4;
    chk_sec_info1.min_value = 30;
    chk_sec_info1.max_value = 40;
    chk_sec_info1.mask_value = 0x00FFFFFF;
    chk_sec_info1.shift_value = 10;
    chk_sec_info1.operation = bcmFlowtrackerCheckOpGreater;
    chk_sec_info1.primary_check_id = chk_pri_id1;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_sec_info1, &chk_sec_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test1 secondary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    bcm_flowtracker_check_action_info_t_init(&chk_action_info1);
    chk_action_info1.param = bcmFlowtrackerTrackingParamTypeTTL;
    chk_action_info1.action = bcmFlowtrackerCheckActionUpdateValue;
    chk_action_info1.mask_value = 0x0F;
    chk_action_info1.shift_value = 2;
    rv = bcm_flowtracker_check_action_info_set(unit, chk_pri_id1, chk_action_info1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test1 bcm_flowtracker_check_action_info_set rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    bcm_flowtracker_check_export_info_t_init(&chk_export_info1);
    chk_export_info1.export_check_threshold = 10;
    chk_export_info1.operation = bcmFlowtrackerCheckOpGreaterEqual;
    rv = bcm_flowtracker_check_export_info_set(unit, chk_pri_id1, chk_export_info1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test1 bcm_flowtracker_check_export_info_set rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    bcm_flowtracker_check_delay_info_t_init(&chk_delay_info1);
    chk_delay_info1.src_ts = bcmFlowtrackerTimestampSourceNTP;
    chk_delay_info1.dst_ts = bcmFlowtrackerTimestampSourceCheckEvent1;
    chk_delay_info1.offset = 200;
    chk_delay_info1.gran = 2;
    rv = bcm_flowtracker_check_delay_info_set(unit, chk_pri_id1, chk_delay_info1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test1 bcm_flowtracker_check_delay_info_set rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    bcm_flowtracker_check_info_t_init(&chk_pri_info1_get);
    rv = bcm_flowtracker_check_get(unit, chk_pri_id1, &chk_pri_info1_get);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test1 primary bcm_flowtracker_check_get rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    if ((chk_pri_info1_get.param != bcmFlowtrackerTrackingParamTypeSrcIPv4) ||
        (chk_pri_info1_get.min_value != 10) ||
        (chk_pri_info1_get.max_value != 20) ||
        (chk_pri_info1_get.mask_value != 0x0000FFFF) ||
        (chk_pri_info1_get.shift_value != 5) ||
        (chk_pri_info1_get.operation != bcmFlowtrackerCheckOpEqual)) {
        printf("[CINT] TC %2d - test1 primary check get params are not same as configured\n",tc_id);
        return BCM_E_FAIL;
    }
    bcm_flowtracker_check_info_t_init(&chk_sec_info1_get);
    rv = bcm_flowtracker_check_get(unit, chk_sec_id1, &chk_sec_info1_get);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test1 secondary bcm_flowtracker_check_get rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    if ((chk_sec_info1_get.param != bcmFlowtrackerTrackingParamTypeDstIPv4) ||
        (chk_sec_info1_get.min_value != 30) ||
        (chk_sec_info1_get.max_value != 40) ||
        (chk_sec_info1_get.mask_value != 0x00FFFFFF) ||
        (chk_sec_info1_get.shift_value != 10) ||
        (chk_sec_info1_get.operation != bcmFlowtrackerCheckOpGreater)) {
        printf("[CINT] TC %2d - test1 secondary check get params are not same as configured\n",tc_id);
        return BCM_E_FAIL;
    }
    bcm_flowtracker_check_action_info_t_init(&chk_action_info1_get);
    rv = bcm_flowtracker_check_action_info_get(unit, chk_pri_id1, &chk_action_info1_get);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test1 bcm_flowtracker_check_action_info_get rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    if ((chk_action_info1_get.param != bcmFlowtrackerTrackingParamTypeTTL) ||
        (chk_action_info1_get.action != bcmFlowtrackerCheckActionUpdateValue) ||
        (chk_action_info1_get.mask_value != 0x0F) ||
        (chk_action_info1_get.shift_value != 2)) {
        printf("[CINT] TC %2d - test1 action info get params are not same as configured\n",tc_id);
        return BCM_E_FAIL;
    }
    bcm_flowtracker_check_export_info_t_init(&chk_export_info1_get);
    rv = bcm_flowtracker_check_export_info_get(unit, chk_pri_id1, &chk_export_info1_get);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test1 bcm_flowtracker_check_export_info_get rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    if ((chk_export_info1_get.export_check_threshold != 10) ||
        (chk_export_info1_get.operation != bcmFlowtrackerCheckOpGreaterEqual)) {
        printf("[CINT] TC %2d - test1 export info get params are not same as configured\n",tc_id);
        return BCM_E_FAIL;
    }
    bcm_flowtracker_check_delay_info_t_init(&chk_delay_info1_get);
    rv = bcm_flowtracker_check_delay_info_get(unit, chk_pri_id1, &chk_delay_info1_get);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test1 bcm_flowtracker_check_delay_info_get rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    if ((chk_delay_info1_get.src_ts != bcmFlowtrackerTimestampSourceNTP) ||
        (chk_delay_info1_get.dst_ts != bcmFlowtrackerTimestampSourceCheckEvent1) ||
        (chk_delay_info1_get.offset != 200) ||
        (chk_delay_info1_get.gran != 2)) {
        printf("[CINT] TC %2d - test1 delay info get params are not same as configured\n",tc_id);
        return BCM_E_FAIL;
    }
    rv = bcm_flowtracker_check_destroy(unit, chk_sec_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test1 secondary bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_check_destroy(unit, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test1 primary bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }

    /* test2 - secondary checker cannot be created with id */
    optionschk = 0;
    bcm_flowtracker_check_info_t_init(&chk_pri_info1);
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeSrcIPv4;
    chk_pri_info1.min_value = 10;
    chk_pri_info1.max_value = 20;
    chk_pri_info1.mask_value = 0x0000FFFF;
    chk_pri_info1.shift_value = 5;
    chk_pri_info1.operation = bcmFlowtrackerCheckOpEqual;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test2 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    optionschk = BCM_FLOWTRACKER_CHECK_SECOND | BCM_FLOWTRACKER_CHECK_WITH_ID;
    bcm_flowtracker_check_info_t_init(&chk_sec_info1);
    chk_sec_info1.param = bcmFlowtrackerTrackingParamTypeDstIPv4;
    chk_sec_info1.min_value = 30;
    chk_sec_info1.max_value = 40;
    chk_sec_info1.mask_value = 0x00FFFFFF;
    chk_sec_info1.shift_value = 10;
    chk_sec_info1.operation = bcmFlowtrackerCheckOpGreater;
    chk_sec_info1.primary_check_id = chk_pri_id1;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_sec_info1, &chk_sec_id1);
    if (rv != BCM_E_CONFIG) {
        printf("[CINT] TC %2d - test2 secondary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    rv = bcm_flowtracker_check_destroy(unit, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test2 primary bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }

    /* test3 - secondary checker can be associated with event flag */
    optionschk = 0;
    bcm_flowtracker_check_info_t_init(&chk_pri_info1);
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeSrcIPv4;
    chk_pri_info1.min_value = 10;
    chk_pri_info1.max_value = 20;
    chk_pri_info1.mask_value = 0x0000FFFF;
    chk_pri_info1.shift_value = 5;
    chk_pri_info1.operation = bcmFlowtrackerCheckOpEqual;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test3 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    optionschk = BCM_FLOWTRACKER_CHECK_SECOND;
    bcm_flowtracker_check_info_t_init(&chk_sec_info1);
    chk_sec_info1.flags = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1;
    chk_sec_info1.param = bcmFlowtrackerTrackingParamTypeDstIPv4;
    chk_sec_info1.min_value = 30;
    chk_sec_info1.max_value = 40;
    chk_sec_info1.mask_value = 0x00FFFFFF;
    chk_sec_info1.shift_value = 10;
    chk_sec_info1.operation = bcmFlowtrackerCheckOpGreater;
    chk_sec_info1.primary_check_id = chk_pri_id1;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_sec_info1, &chk_sec_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test3 event flag1 secondary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_check_destroy(unit, chk_sec_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test3 secondary bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_check_destroy(unit, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test3 primary bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }

    /* test4 - params not allowed for checker operation */
    optionschk = 0;
    bcm_flowtracker_check_info_t_init(&chk_pri_info1);
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypePktCount;
    chk_pri_info1.operation = bcmFlowtrackerCheckOpEqual;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeByteCount;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeTimestampNewLearn;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeTimestampFlowStart;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeTimestampFlowEnd;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeChipDelay;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeE2EDelay;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeIPATDelay;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeIPDTDelay;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeFlowtrackerCheck;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeSrcIPv6;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeDstIPv6;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeInnerSrcIPv6;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeInnerDstIPv6;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test4 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }

    /* test5 - GlobalMask is allowed */
    optionschk = 0;
    bcm_flowtracker_check_info_t_init(&chk_pri_info1);
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeNextHopB;
    chk_pri_info1.max_value = 0xFFFF;
    chk_pri_info1.operation = bcmFlowtrackerCheckOpMask;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test5 bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_check_destroy(unit, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test5 bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_pri_info1.max_value = 0x1FFFF;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test5 bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }

    /* test7 - check with event flag */
    optionschk = 0;
    chk_pri_id1 = -1;
    bcm_flowtracker_check_info_t_init(&chk_pri_info1);
    chk_pri_info1.flags = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1;
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeSrcIPv4;
    chk_pri_info1.operation = bcmFlowtrackerCheckOpEqual;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test7 bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_check_destroy(unit, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test7 bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_pri_info1.flags = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test7 bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_check_destroy(unit, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test7 bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_pri_info1.flags = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT3;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test7 bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_check_destroy(unit, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test7 bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_pri_info1.flags = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT4;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test7 bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_check_destroy(unit, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test7 bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }

    /* test11 - params across different stages */
    optionschk = 0;
    bcm_flowtracker_check_info_t_init(&chk_pri_info1);
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeSrcIPv4;
    chk_pri_info1.operation = bcmFlowtrackerCheckOpEqual;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test11 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    optionschk = BCM_FLOWTRACKER_CHECK_SECOND;
    bcm_flowtracker_check_info_t_init(&chk_sec_info1);
    chk_sec_info1.param = bcmFlowtrackerTrackingParamTypeMMUQueueId;
    chk_sec_info1.operation = bcmFlowtrackerCheckOpGreater;
    chk_sec_info1.primary_check_id = chk_pri_id1;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_sec_info1, &chk_sec_id1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test11 secondary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    bcm_flowtracker_check_action_info_t_init(&chk_action_info1);
    chk_action_info1.param = bcmFlowtrackerTrackingParamTypeMMUQueueId;
    chk_action_info1.action = bcmFlowtrackerCheckActionUpdateValue;
    rv = bcm_flowtracker_check_action_info_set(unit, chk_pri_id1, chk_action_info1);
    if (rv != BCM_E_PARAM) {
        printf("[CINT] TC %2d - test11 bcm_flowtracker_check_action_info_set rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    rv = bcm_flowtracker_check_destroy(unit, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - test11 primary bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

int
ftv2_param_sanity_group(uint32 tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE, idx = 0;
    uint32 optionschk = 0;
    bcm_flowtracker_check_t chk_pri_id1 = -1;
    bcm_flowtracker_check_t chk_pri_id2 = -1;
    bcm_flowtracker_check_t chk_pri_id3 = -1;
    bcm_flowtracker_check_t chk_sec_id1 = -1;
    bcm_flowtracker_check_t chk_sec_id2 = -1;
    bcm_flowtracker_check_t chk_sec_id3 = -1;
    bcm_flowtracker_check_info_t chk_pri_info1;
    bcm_flowtracker_check_info_t chk_pri_info2;
    bcm_flowtracker_check_info_t chk_pri_info3;
    bcm_flowtracker_check_info_t chk_sec_info1;
    bcm_flowtracker_check_info_t chk_sec_info2;
    bcm_flowtracker_check_info_t chk_sec_info3;
    bcm_flowtracker_check_action_info_t chk_action_info1;
    bcm_flowtracker_check_action_info_t chk_action_info2;
    bcm_flowtracker_check_action_info_t chk_action_info3;
    bcm_flowtracker_check_export_info_t chk_export_info1;
    bcm_flowtracker_check_export_info_t chk_export_info2;
    bcm_flowtracker_check_export_info_t chk_export_info3;
    bcm_flowtracker_group_info_t flow_group_info;
    bcm_flowtracker_group_t flow_group_id1 = -1;
    int max_num_tracking_params = 20;
    int actual_num_tracking_params1 = 0;
    bcm_flowtracker_tracking_param_info_t *tracking_info_tmp = NULL;
    bcm_flowtracker_tracking_param_info_t tracking_info1[max_num_tracking_params];
    int max_export_elements = 30;
    int actual_num_in_export_elements1 = 0, actual_num_out_export_elements1 = 0;
    bcm_flowtracker_export_element_info_t export_elems_ip[max_export_elements];
    bcm_flowtracker_export_element_info_t export_elems_op[max_export_elements];

    /* usecase1 */
    optionschk = 0;
    chk_pri_id1 = -1;
    bcm_flowtracker_check_info_t_init(&chk_pri_info1);
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeTTL;
    chk_pri_info1.min_value = 1;
    chk_pri_info1.max_value = 200;
    chk_pri_info1.operation = bcmFlowtrackerCheckOpInRange;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase1 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    bcm_flowtracker_check_action_info_t_init(&chk_action_info1);
    chk_action_info1.param = bcmFlowtrackerTrackingParamTypeIPProtocol;
    chk_action_info1.action = bcmFlowtrackerCheckActionCounterIncr;
    rv = bcm_flowtracker_check_action_info_set(unit, chk_pri_id1, chk_action_info1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase1 bcm_flowtracker_check_action_info_set rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    bcm_flowtracker_check_export_info_t_init(&chk_export_info1);
    chk_export_info1.export_check_threshold = 1;
    chk_export_info1.operation = bcmFlowtrackerCheckOpGreaterEqual;
    rv = bcm_flowtracker_check_export_info_set(unit, chk_pri_id1, chk_export_info1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase1 bcm_flowtracker_check_export_info_set rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    flow_group_id1 = -1;
    bcm_flowtracker_group_info_t_init(&flow_group_info);
    flow_group_info.observation_domain_id = 0;
    rv = bcm_flowtracker_group_create (unit, 0, &flow_group_id1, &flow_group_info);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase1 bcm_flowtracker_group_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    tracking_info_tmp = tracking_info1;
    for (idx = 0; idx < max_num_tracking_params; idx++) {
        bcm_flowtracker_tracking_param_info_t_init(tracking_info_tmp);
        tracking_info_tmp++;
    }
    tracking_info1[0].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    tracking_info1[0].param = bcmFlowtrackerTrackingParamTypeSrcIPv4;
    tracking_info1[1].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    tracking_info1[1].param = bcmFlowtrackerTrackingParamTypeDstIPv4;
    tracking_info1[2].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    tracking_info1[2].param = bcmFlowtrackerTrackingParamTypeL4SrcPort;
    tracking_info1[3].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    tracking_info1[3].param = bcmFlowtrackerTrackingParamTypeL4DstPort;
    tracking_info1[4].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    tracking_info1[4].param = bcmFlowtrackerTrackingParamTypeIPProtocol;
    tracking_info1[5].param = bcmFlowtrackerTrackingParamTypeIPProtocol;
    tracking_info1[6].param = bcmFlowtrackerTrackingParamTypePktCount;
    tracking_info1[7].param = bcmFlowtrackerTrackingParamTypeTTL;
    tracking_info1[8].param = bcmFlowtrackerTrackingParamTypeL4SrcPort;
    actual_num_tracking_params1 = 9;
    rv = bcm_flowtracker_group_tracking_params_set(unit, flow_group_id1, actual_num_tracking_params1, tracking_info1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase1 bcm_flowtracker_group_tracking_params_set => rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_group_check_add(unit, flow_group_id1, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase1 bcm_flowtracker_group_check_add => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    for (idx = 0; idx < max_export_elements; idx++) {
        export_elems_ip[idx].data_size = 0;
        export_elems_ip[idx].element = bcmFlowtrackerExportElementTypeReserved;
        export_elems_op[idx].data_size = 0;
        export_elems_op[idx].element = bcmFlowtrackerExportElementTypeReserved;
    }
    export_elems_ip[0].element = bcmFlowtrackerExportElementTypeSrcIPv4;
    export_elems_ip[0].data_size = 4;
    export_elems_ip[1].element = bcmFlowtrackerExportElementTypeDstIPv4;
    export_elems_ip[1].data_size = 4;
    export_elems_ip[2].element = bcmFlowtrackerExportElementTypeL4SrcPort;
    export_elems_ip[2].data_size = 2;
    export_elems_ip[3].element = bcmFlowtrackerExportElementTypeL4DstPort;
    export_elems_ip[3].data_size = 2;
    export_elems_ip[4].element = bcmFlowtrackerExportElementTypeIPProtocol;
    export_elems_ip[4].data_size = 1;
    export_elems_ip[5].element = bcmFlowtrackerExportElementTypeIPProtocol;
    export_elems_ip[5].data_size = 1;
    export_elems_ip[6].element = bcmFlowtrackerExportElementTypePktCount;
    export_elems_ip[6].data_size = 4;
    export_elems_ip[7].element = bcmFlowtrackerExportElementTypeTTL;
    export_elems_ip[7].data_size = 1;
    export_elems_ip[8].element = bcmFlowtrackerExportElementTypeL4SrcPort;
    export_elems_ip[8].data_size = 2;
    export_elems_ip[9].element = bcmFlowtrackerExportElementTypeFlowtrackerGroup;
    export_elems_ip[9].data_size = 2;
    actual_num_in_export_elements1 = 10;
    rv = bcm_flowtracker_export_template_validate(unit, flow_group_id1,
                                            actual_num_in_export_elements1, export_elems_ip,
                                            max_export_elements,
                                            export_elems_op, &actual_num_out_export_elements1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase1 bcm_flowtracker_export_template_validate => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_group_check_delete(unit, flow_group_id1, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase1 bcm_flowtracker_group_check_delete => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_group_destroy(unit, flow_group_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase1 bcm_flowtracker_group_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    flow_group_id1 = -1;
    rv = bcm_flowtracker_check_destroy(unit, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase1 bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_pri_id1 = -1;


    /* usecase 2 */
    chk_pri_id1 = -1;
    bcm_flowtracker_check_info_t_init(&chk_pri_info1);
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeTTL;
    chk_pri_info1.min_value = 1;
    chk_pri_info1.max_value = 200;
    chk_pri_info1.mask_value = 0xFFFF;
    chk_pri_info1.shift_value = 2;
    chk_pri_info1.operation = bcmFlowtrackerCheckOpInRange;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_pri_id2 = -1;
    bcm_flowtracker_check_info_t_init(&chk_pri_info2);
    chk_pri_info2.param = bcmFlowtrackerTrackingParamTypeMMUQueueId;
    chk_pri_info2.min_value = 2;
    chk_pri_info2.max_value = 300;
    chk_pri_info2.mask_value = 0xFFFFFF;
    chk_pri_info2.shift_value = 4;
    chk_pri_info2.operation = bcmFlowtrackerCheckOpInRange;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info2, &chk_pri_id2);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_pri_id3 = -1;
    bcm_flowtracker_check_info_t_init(&chk_pri_info3);
    chk_pri_info3.param = bcmFlowtrackerTrackingParamTypeNextHopB;
    chk_pri_info3.min_value = 3;
    chk_pri_info3.max_value = 400;
    chk_pri_info3.mask_value = 0xFFFF;
    chk_pri_info3.shift_value = 2;
    chk_pri_info3.operation = bcmFlowtrackerCheckOpInRange;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info3, &chk_pri_id3);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    flow_group_id1 = -1;
    bcm_flowtracker_group_info_t_init(&flow_group_info);
    flow_group_info.observation_domain_id = 0;
    rv = bcm_flowtracker_group_create (unit, 0, &flow_group_id1, &flow_group_info);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 bcm_flowtracker_group_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    tracking_info_tmp = tracking_info1;
    for (idx = 0; idx < max_num_tracking_params; idx++) {
        bcm_flowtracker_tracking_param_info_t_init(tracking_info_tmp);
        tracking_info_tmp++;
    }
    tracking_info1[0].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    tracking_info1[0].param = bcmFlowtrackerTrackingParamTypeSrcIPv4;
    tracking_info1[1].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    tracking_info1[1].param = bcmFlowtrackerTrackingParamTypeDstIPv4;
    tracking_info1[2].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    tracking_info1[2].param = bcmFlowtrackerTrackingParamTypeL4SrcPort;
    tracking_info1[3].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    tracking_info1[3].param = bcmFlowtrackerTrackingParamTypeL4DstPort;
    tracking_info1[4].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    tracking_info1[4].param = bcmFlowtrackerTrackingParamTypeIPProtocol;
    tracking_info1[5].param = bcmFlowtrackerTrackingParamTypeTTL;
    tracking_info1[6].param = bcmFlowtrackerTrackingParamTypeL4SrcPort;
    tracking_info1[7].param = bcmFlowtrackerTrackingParamTypeIngPort;
    actual_num_tracking_params1 = 8;
    rv = bcm_flowtracker_group_tracking_params_set(unit, flow_group_id1, actual_num_tracking_params1, tracking_info1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 bcm_flowtracker_group_tracking_params_set => rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_group_check_add(unit, flow_group_id1, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 bcm_flowtracker_group_check_add => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_group_check_add(unit, flow_group_id1, chk_pri_id2);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 bcm_flowtracker_group_check_add => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_group_check_add(unit, flow_group_id1, chk_pri_id3);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 bcm_flowtracker_group_check_add => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    for (idx = 0; idx < max_export_elements; idx++) {
        export_elems_ip[idx].data_size = 0;
        export_elems_ip[idx].element = bcmFlowtrackerExportElementTypeReserved;
        export_elems_ip[idx].check_id = 0;
        export_elems_op[idx].data_size = 0;
        export_elems_op[idx].element = bcmFlowtrackerExportElementTypeReserved;
        export_elems_op[idx].check_id = 0;
    }
    export_elems_ip[0].element = bcmFlowtrackerExportElementTypeSrcIPv4;
    export_elems_ip[0].data_size = 4;
    export_elems_ip[1].element = bcmFlowtrackerExportElementTypeDstIPv4;
    export_elems_ip[1].data_size = 4;
    export_elems_ip[2].element = bcmFlowtrackerExportElementTypeL4SrcPort;
    export_elems_ip[2].data_size = 2;
    export_elems_ip[3].element = bcmFlowtrackerExportElementTypeL4DstPort;
    export_elems_ip[3].data_size = 2;
    export_elems_ip[4].element = bcmFlowtrackerExportElementTypeIPProtocol;
    export_elems_ip[4].data_size = 1;
    export_elems_ip[5].element = bcmFlowtrackerExportElementTypeTTL;
    export_elems_ip[5].data_size = 1;
    export_elems_ip[6].element = bcmFlowtrackerExportElementTypeL4SrcPort;
    export_elems_ip[6].data_size = 2;
    export_elems_ip[7].element = bcmFlowtrackerExportElementTypeIngPort;
    export_elems_ip[7].data_size = 1;
    export_elems_ip[8].element = bcmFlowtrackerExportElementTypeFlowtrackerGroup;
    export_elems_ip[8].data_size = 2;
    actual_num_in_export_elements1 = 9;
    rv = bcm_flowtracker_export_template_validate(unit, flow_group_id1,
            actual_num_in_export_elements1, export_elems_ip,
            max_export_elements,
            export_elems_op, &actual_num_out_export_elements1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 bcm_flowtracker_export_template_validate => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    bshell(unit, "d chg BSC_DT_ALU_MASK_PROFILE");
    rv = bcm_flowtracker_group_check_delete(unit, flow_group_id1, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 bcm_flowtracker_check_delete rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_group_check_delete(unit, flow_group_id1, chk_pri_id2);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 bcm_flowtracker_check_delete rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_group_check_delete(unit, flow_group_id1, chk_pri_id3);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 bcm_flowtracker_check_delete rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    bshell(unit, "d chg BSC_DT_ALU_MASK_PROFILE");
    rv = bcm_flowtracker_group_destroy(unit, flow_group_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 bcm_flowtracker_group_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    flow_group_id1 = -1;
    bshell(unit, "d chg BSC_DT_ALU_MASK_PROFILE");
    rv = bcm_flowtracker_check_destroy(unit, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_pri_id1 = -1;
    rv = bcm_flowtracker_check_destroy(unit, chk_pri_id2);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_pri_id2 = -1;
    rv = bcm_flowtracker_check_destroy(unit, chk_pri_id3);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase2 bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_pri_id3 = -1;


    /* usecase 3 */
    chk_pri_id1 = -1;
    optionschk = 0;
    bcm_flowtracker_check_info_t_init(&chk_pri_info1);
    chk_pri_info1.param = bcmFlowtrackerTrackingParamTypeTTL;
    chk_pri_info1.min_value = 1;
    chk_pri_info1.operation = bcmFlowtrackerCheckOpGreater;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info1, &chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_sec_id1 = -1;
    optionschk = BCM_FLOWTRACKER_CHECK_SECOND;
    bcm_flowtracker_check_info_t_init(&chk_sec_info1);
    chk_sec_info1.param = bcmFlowtrackerTrackingParamTypeL4SrcPort;
    chk_sec_info1.max_value = 200;
    chk_sec_info1.operation = bcmFlowtrackerCheckOpSmaller;
    chk_sec_info1.primary_check_id = chk_pri_id1;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_sec_info1, &chk_sec_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 secondary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_pri_id2 = -1;
    optionschk = 0;
    bcm_flowtracker_check_info_t_init(&chk_pri_info2);
    chk_pri_info2.param = bcmFlowtrackerTrackingParamTypeNextHopB;
    chk_pri_info2.min_value = 2;
    chk_pri_info2.operation = bcmFlowtrackerCheckOpGreaterEqual;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info2, &chk_pri_id2);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_sec_id2 = -1;
    optionschk = BCM_FLOWTRACKER_CHECK_SECOND;
    bcm_flowtracker_check_info_t_init(&chk_sec_info2);
    chk_sec_info2.param = bcmFlowtrackerTrackingParamTypeNextHopA;
    chk_sec_info2.max_value = 301;
    chk_sec_info2.operation = bcmFlowtrackerCheckOpSmallerEqual;
    chk_sec_info2.primary_check_id = chk_pri_id2;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_sec_info2, &chk_sec_id2);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 secondary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_pri_id3 = -1;
    optionschk = 0;
    bcm_flowtracker_check_info_t_init(&chk_pri_info3);
    chk_pri_info3.param = bcmFlowtrackerTrackingParamTypeQosAttr;
    chk_pri_info3.min_value = 3;
    chk_pri_info3.operation = bcmFlowtrackerCheckOpGreater;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_pri_info3, &chk_pri_id3);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 primary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_sec_id3 = -1;
    optionschk = BCM_FLOWTRACKER_CHECK_SECOND;
    bcm_flowtracker_check_info_t_init(&chk_sec_info3);
    chk_sec_info3.param = bcmFlowtrackerTrackingParamTypeEgressPort;
    chk_sec_info3.min_value = 4;
    chk_sec_info3.max_value = 201;
    chk_sec_info3.operation = bcmFlowtrackerCheckOpSmallerEqual;
    chk_sec_info3.primary_check_id = chk_pri_id3;
    rv = bcm_flowtracker_check_create(unit, optionschk, chk_sec_info3, &chk_sec_id3);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 secondary bcm_flowtracker_check_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    flow_group_id1 = -1;
    bcm_flowtracker_group_info_t_init(&flow_group_info);
    flow_group_info.observation_domain_id = 0;
    rv = bcm_flowtracker_group_create (unit, 0, &flow_group_id1, &flow_group_info);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 bcm_flowtracker_group_create rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    tracking_info_tmp = tracking_info1;
    for (idx = 0; idx < max_num_tracking_params; idx++) {
        bcm_flowtracker_tracking_param_info_t_init(tracking_info_tmp);
        tracking_info_tmp++;
    }
    tracking_info1[0].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    tracking_info1[0].param = bcmFlowtrackerTrackingParamTypeSrcIPv4;
    tracking_info1[1].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    tracking_info1[1].param = bcmFlowtrackerTrackingParamTypeDstIPv4;
    tracking_info1[2].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    tracking_info1[2].param = bcmFlowtrackerTrackingParamTypeL4SrcPort;
    tracking_info1[3].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    tracking_info1[3].param = bcmFlowtrackerTrackingParamTypeL4DstPort;
    tracking_info1[4].flags = BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY;
    tracking_info1[4].param = bcmFlowtrackerTrackingParamTypeIPProtocol;
    tracking_info1[5].param = bcmFlowtrackerTrackingParamTypeChipDelay;
    tracking_info1[6].param = bcmFlowtrackerTrackingParamTypeE2EDelay;
    actual_num_tracking_params1 = 7;
    rv = bcm_flowtracker_group_tracking_params_set(unit, flow_group_id1, actual_num_tracking_params1, tracking_info1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 bcm_flowtracker_group_tracking_params_set => rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_group_check_add(unit, flow_group_id1, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 bcm_flowtracker_group_check_add => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_group_check_add(unit, flow_group_id1, chk_pri_id2);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 bcm_flowtracker_group_check_add => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_group_check_add(unit, flow_group_id1, chk_pri_id3);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 bcm_flowtracker_group_check_add => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    for (idx = 0; idx < max_export_elements; idx++) {
        export_elems_ip[idx].data_size = 0;
        export_elems_ip[idx].element = bcmFlowtrackerExportElementTypeReserved;
        export_elems_op[idx].data_size = 0;
        export_elems_op[idx].element = bcmFlowtrackerExportElementTypeReserved;
    }
    export_elems_ip[0].element = bcmFlowtrackerExportElementTypeSrcIPv4;
    export_elems_ip[0].data_size = 4;
    export_elems_ip[1].element = bcmFlowtrackerExportElementTypeDstIPv4;
    export_elems_ip[1].data_size = 4;
    export_elems_ip[2].element = bcmFlowtrackerExportElementTypeL4SrcPort;
    export_elems_ip[2].data_size = 2;
    export_elems_ip[3].element = bcmFlowtrackerExportElementTypeL4DstPort;
    export_elems_ip[3].data_size = 2;
    export_elems_ip[4].element = bcmFlowtrackerExportElementTypeIPProtocol;
    export_elems_ip[4].data_size = 1;
    export_elems_ip[5].element = bcmFlowtrackerExportElementTypeChipDelay;
    export_elems_ip[5].data_size = 4;
    export_elems_ip[6].element = bcmFlowtrackerExportElementTypeE2EDelay;
    export_elems_ip[6].data_size = 4;
    export_elems_ip[7].element = bcmFlowtrackerExportElementTypeFlowtrackerGroup;
    export_elems_ip[7].data_size = 2;
    actual_num_in_export_elements1 = 8;
    rv = bcm_flowtracker_export_template_validate(unit, flow_group_id1,
            actual_num_in_export_elements1, export_elems_ip,
            max_export_elements,
            export_elems_op, &actual_num_out_export_elements1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 bcm_flowtracker_export_template_validate => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_group_check_delete(unit, flow_group_id1, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 bcm_flowtracker_group_check_delete => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_group_check_delete(unit, flow_group_id1, chk_pri_id2);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 bcm_flowtracker_group_check_delete => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_group_check_delete(unit, flow_group_id1, chk_pri_id3);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 bcm_flowtracker_group_check_delete => rv %d(%s)\n",tc_id, rv, bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_flowtracker_group_destroy(unit, flow_group_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 bcm_flowtracker_group_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    flow_group_id1 = -1;
    rv = bcm_flowtracker_check_destroy(unit, chk_sec_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_sec_id1 = -1;
    rv = bcm_flowtracker_check_destroy(unit, chk_sec_id2);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_sec_id2 = -1;
    rv = bcm_flowtracker_check_destroy(unit, chk_sec_id3);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_sec_id3 = -1;
    rv = bcm_flowtracker_check_destroy(unit, chk_pri_id1);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_pri_id1 = -1;
    rv = bcm_flowtracker_check_destroy(unit, chk_pri_id2);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_pri_id2 = -1;
    rv = bcm_flowtracker_check_destroy(unit, chk_pri_id3);
    if (BCM_FAILURE(rv)) {
        printf("[CINT] TC %2d - usecase3 bcm_flowtracker_check_destroy rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
        return rv;
    }
    chk_pri_id3 = -1;

    return BCM_E_NONE;
}

/* check flowtracker apis param sanity */
int
ftv2_param_sanity_test_main(uint32 tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* skip if no test config is set */
    if (!ftv2_param_sanity_test_config) {
        if (!skip_log) {
            printf("[CINT] Skip param sanity check ......\n");
        }
        return rv;
    }

    switch(ftv2_param_sanity_test_config) {
        case 1:
            /* ftv2_param_sanity_test_config = 1 */
            /* validate param sanity for drop reason api's */
            rv = ftv2_param_sanity_drop_reason(tc_id, debug_flags);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - drop reason apis param sanity failed with rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
                return rv;
            }

            /* validate param sanity for flow checker api's */
            rv = ftv2_param_sanity_check(tc_id, debug_flags);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - check apis param sanity failed with rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
                return rv;
            }

            /* validate param sanity for flow checker groups */
            rv = ftv2_param_sanity_group(tc_id, debug_flags);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - flowtracker group apis param sanity failed with rv %d(%s)\n",tc_id,rv,bcm_errmsg(rv));
                return rv;
            }
            /* No packet sent , hence setting packet handled as true */
            packet_handled_by_watcher = 1;
            break;

        default:
            printf("[CINT] TC %2d - invalid param sanity test config %d\n",tc_id,ftv2_param_sanity_test_config);
            return BCM_E_FAIL;
    }

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Flow tracker APIs param sanity completed succesfully.\n");
    }

    return BCM_E_NONE;
}
