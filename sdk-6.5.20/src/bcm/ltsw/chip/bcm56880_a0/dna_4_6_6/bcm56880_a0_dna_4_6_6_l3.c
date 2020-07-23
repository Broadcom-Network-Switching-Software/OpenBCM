/*! \file bcm56880_a0_dna_4_6_6_l3.c
 *
 * BCM56880_A0 DNA_4_6_6 L3 Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/l3.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/util.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_L3

/* L3 reserved mc address type.*/
#define L3_RSV_MC_IP_TYPE 0x40

/******************************************************************************
 * Private functions
 */
static int
bcm56880_a0_dna_4_6_6_l3_ipv6_reserved_multicast_addr_multi_set(
    int unit,
    int num,
    bcm_ip6_t *ip6_addr,
    bcm_ip6_t *ip6_mask)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;
    const char *lt_name[2] = {L3_HEADER_VALIDATION_1_DST_IPV6s,
                              L3_HEADER_VALIDATION_2_DST_IPV6s};
    const char *lt_key[2] = {L3_HEADER_VALIDATION_1_DST_IPV6_IDs,
                             L3_HEADER_VALIDATION_2_DST_IPV6_IDs};
    uint64_t v6[2];
    uint64_t v6_mask[2];
    int i, j;

    SHR_FUNC_ENTER(unit);

    if (num <= 0 || num > 2) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!ip6_addr || !ip6_mask) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    for(j = 0; j < 2; j++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, lt_name[j], &eh));

        for (i = 0; i < num; i++) {
            bcmi_ltsw_util_ip6_to_uint64(v6, (bcm_ip6_t *)ip6_addr[i]);
            bcmi_ltsw_util_ip6_to_uint64(v6_mask, (bcm_ip6_t *)ip6_mask[i]);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, lt_key[j], i));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, VALUE_LOWERs, v6[0]));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, VALUE_UPPERs, v6[1]));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, MASK_LOWERs, v6_mask[0]));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, MASK_UPPERs, v6_mask[1]));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, ADDRESS_TYPEs, L3_RSV_MC_IP_TYPE));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_free(eh));
        eh = BCMLT_INVALID_HDL;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_dna_4_6_6_l3_ipv6_reserved_multicast_addr_multi_get(
    int unit,
    int num,
    bcm_ip6_t *ip6_addr,
    bcm_ip6_t *ip6_mask)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;
    const char *lt_name = L3_HEADER_VALIDATION_1_DST_IPV6s;
    uint64_t v6[2], v6_mask[2], add_type;
    int i;

    SHR_FUNC_ENTER(unit);

    if (num <= 0 || num > 2) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!ip6_addr || !ip6_mask) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    for (i = 0; i < num; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, L3_HEADER_VALIDATION_1_DST_IPV6_IDs, i));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, ADDRESS_TYPEs, &add_type));

        if (!COMPILER_64_EQ(add_type, L3_RSV_MC_IP_TYPE)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, VALUE_LOWERs, &v6[0]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, VALUE_UPPERs, &v6[1]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, MASK_LOWERs, &v6_mask[0]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, MASK_UPPERs, &v6_mask[1]));

        bcmi_ltsw_util_uint64_to_ip6((bcm_ip6_t *)&ip6_addr[i], v6);
        bcmi_ltsw_util_uint64_to_ip6((bcm_ip6_t *)&ip6_mask[i], v6_mask);

    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

static mbcm_ltsw_l3_drv_t bcm56880_a0_dna_4_6_6_ltsw_l3_drv = {

    .l3_ipv6_reserved_multicast_addr_multi_set = bcm56880_a0_dna_4_6_6_l3_ipv6_reserved_multicast_addr_multi_set,
    .l3_ipv6_reserved_multicast_addr_multi_get = bcm56880_a0_dna_4_6_6_l3_ipv6_reserved_multicast_addr_multi_get

};

/******************************************************************************
 * Public functions
 */

int
bcm56880_a0_dna_4_6_6_ltsw_l3_drv_attach(int unit)
{
    return mbcm_ltsw_l3_drv_set(unit,
                                &bcm56880_a0_dna_4_6_6_ltsw_l3_drv);
}

