/*! \file bcm56996_a0_fp_egr_fixed_key.c
 *
 * API for fixed key entry/group attributes for Tomahawk-4GG(56996_A0)
 * device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56996_a0_fp.h>
#include <bcmdrd/chip/bcm56996_a0_enum.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

#define BCM56996_A0_KEY_MATCH_TYPE_IPV4_SINGLE            (1)
#define BCM56996_A0_KEY_MATCH_TYPE_IPV6_SINGLE            (2)
#define BCM56996_A0_KEY_MATCH_TYPE_IPV6_DOUBLE            (3)
#define BCM56996_A0_KEY_MATCH_TYPE_IPV4_L2_L3_DOUBLE      (4)
#define BCM56996_A0_KEY_MATCH_TYPE_L2_SINGLE              (5)
#define BCM56996_A0_KEY_MATCH_TYPE_IPV4_IPV6_DOUBLE       (6)
#define BCM56996_A0_KEY_MATCH_TYPE_HIGIG_DOUBLE           (9)
#define BCM56990_A0_KEY_MATCH_TYPE_LOOPBACK_DOUBLE        (10)
#define BCM56996_A0_KEY_MATCH_TYPE_L2_DOUBLE              (11)


int
bcmfp_bcm56996_a0_egress_group_selcode_key_get(int unit,
                                               bcmfp_group_t *fg)
{
    bcmfp_group_type_t port_pkt_type;
    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    port_pkt_type = fg->group_port_pkt_type;

    switch (fg->group_slice_mode) {
        case BCMFP_GROUP_SLICE_MODE_L2_SINGLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_ANY:
                    fg->ext_codes[0].fpf3 = EFP_KEY4;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            /* FPF2 will be used to hold H/W slice mode in
             * EFP_SLICE_CONTROL register.
             */
            fg->ext_codes[0].fpf2 = 0;
            break;
        case BCMFP_GROUP_SLICE_MODE_L3_SINGLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV4:
                    fg->ext_codes[0].fpf3 = EFP_KEY1;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV6:
                    fg->ext_codes[0].fpf3 = EFP_KEY2;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP:
                    fg->ext_codes[0].fpf3 = EFP_KEY4;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            /* FPF2 will be used to hold H/W slice mode in
             * EFP_SLICE_CONTROL register.
             */
            fg->ext_codes[0].fpf2 = 1;
            break;
        case BCMFP_GROUP_SLICE_MODE_L3_ANY_SINGLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IP:
                    fg->ext_codes[0].fpf3 = EFP_KEY1;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP:
                    fg->ext_codes[0].fpf3 = EFP_KEY4;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            /* FPF2 will be used to hold H/W slice mode in
             * EFP_SLICE_CONTROL register.
             */
            fg->ext_codes[0].fpf2 = 3;
            break;
        case BCMFP_GROUP_SLICE_MODE_L3_DOUBLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV4:
                    fg->ext_codes[0].fpf3 = EFP_KEY1;
                    fg->ext_codes[1].fpf3 = EFP_KEY4;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV6:
                    fg->ext_codes[0].fpf3 = EFP_KEY3;
                    fg->ext_codes[1].fpf3 = EFP_KEY2;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP:
                    fg->ext_codes[0].fpf3 = EFP_KEY8;
                    fg->ext_codes[1].fpf3 = EFP_KEY4;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            /* FPF2 will be used to hold H/W slice mode in
             * EFP_SLICE_CONTROL register.
             */
            fg->ext_codes[0].fpf2 = 2;
            fg->ext_codes[1].fpf2 = 2;
            break;
        case BCMFP_GROUP_SLICE_MODE_L3_ANY_DOUBLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_HIGIG_PACKET_ANY:
                    fg->ext_codes[0].fpf3 = EFP_KEY6;
                    fg->ext_codes[1].fpf3 = EFP_KEY4;
                    break;
                case BCMFP_GROUP_TYPE_PORT_FRONT_PACKET_ANY:
                    fg->ext_codes[0].fpf3 = EFP_KEY1;
                    fg->ext_codes[1].fpf3 = EFP_KEY4;
                    break;
                case BCMFP_GROUP_TYPE_PORT_LOOPBACK_PACKET_ANY:
                    fg->ext_codes[0].fpf3 = EFP_KEY7;
                    fg->ext_codes[1].fpf3 = EFP_KEY4;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            /* FPF2 will be used to hold H/W slice mode in
             * EFP_SLICE_CONTROL register.
             */
            fg->ext_codes[0].fpf2 = 4;
            fg->ext_codes[1].fpf2 = 4;
            break;
        case BCMFP_GROUP_SLICE_MODE_L3_ALT_DOUBLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV4:
                    fg->ext_codes[0].fpf3 = EFP_KEY1;
                    fg->ext_codes[1].fpf3 = EFP_KEY4;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV6:
                    fg->ext_codes[0].fpf3 = EFP_KEY2;
                    fg->ext_codes[1].fpf3 = EFP_KEY4;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP:
                    fg->ext_codes[0].fpf3 = EFP_KEY8;
                    fg->ext_codes[1].fpf3 = EFP_KEY4;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            /* FPF2 will be used to hold H/W slice mode in
             * EFP_SLICE_CONTROL register.
             */
            fg->ext_codes[0].fpf2 = 5;
            fg->ext_codes[1].fpf2 = 5;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56996_a0_egress_entry_key_match_type_set(int unit,
                               bcmfp_stage_t *stage,
                               bcmfp_group_id_t group_id,
                               bcmfp_group_oper_info_t *opinfo,
                               bcmfp_group_slice_mode_t slice_mode,
                               bcmfp_group_type_t  port_pkt_type,
                               uint32_t **ekw)
{
    int key_match_sbit = 0;
    int key_match_width = 4;
    int key_match_mask_sbit = 0;
    uint8_t part_idx = 0;
    uint8_t parts_cnt = 0;
    uint32_t data = 0;
    uint32_t mask = 0xf;
    int key_minbit = 0, mask_minbit = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ekw, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 opinfo->flags,
                                 &parts_cnt));

    switch (slice_mode) {
        case BCMFP_GROUP_SLICE_MODE_L2_SINGLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_ANY:
                    data = BCM56996_A0_KEY_MATCH_TYPE_L2_SINGLE;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;

        case BCMFP_GROUP_SLICE_MODE_L3_SINGLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV4:
                    data = BCM56996_A0_KEY_MATCH_TYPE_IPV4_SINGLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV6:
                    data = BCM56996_A0_KEY_MATCH_TYPE_IPV6_SINGLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP:
                    data = BCM56996_A0_KEY_MATCH_TYPE_L2_SINGLE;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;

        case BCMFP_GROUP_SLICE_MODE_L3_ANY_SINGLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IP:
                    data = BCM56996_A0_KEY_MATCH_TYPE_IPV4_SINGLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP:
                    data = BCM56996_A0_KEY_MATCH_TYPE_L2_SINGLE;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;

        case BCMFP_GROUP_SLICE_MODE_L3_DOUBLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV4:
                    data = BCM56996_A0_KEY_MATCH_TYPE_IPV4_L2_L3_DOUBLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV6:
                    data = BCM56996_A0_KEY_MATCH_TYPE_IPV6_DOUBLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP:
                    data = BCM56996_A0_KEY_MATCH_TYPE_L2_DOUBLE;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;

        case BCMFP_GROUP_SLICE_MODE_L3_ANY_DOUBLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_HIGIG_PACKET_ANY:
                    data = BCM56996_A0_KEY_MATCH_TYPE_HIGIG_DOUBLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_FRONT_PACKET_ANY:
                    data = BCM56996_A0_KEY_MATCH_TYPE_IPV4_L2_L3_DOUBLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_LOOPBACK_PACKET_ANY:
                    data = BCM56990_A0_KEY_MATCH_TYPE_LOOPBACK_DOUBLE;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;

        case BCMFP_GROUP_SLICE_MODE_L3_ALT_DOUBLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV4:
                    data = BCM56996_A0_KEY_MATCH_TYPE_IPV4_L2_L3_DOUBLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV6:
                    data = BCM56996_A0_KEY_MATCH_TYPE_IPV4_IPV6_DOUBLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP:
                    data = BCM56996_A0_KEY_MATCH_TYPE_L2_DOUBLE;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
    }

    key_minbit = bcmdrd_pt_field_minbit(unit,
            stage->hw_entry_info[BCMFP_GROUP_MODE_SINGLE]->sid,
            KEYf);
    if (key_minbit < 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    mask_minbit = bcmdrd_pt_field_minbit(unit,
            stage->hw_entry_info[BCMFP_GROUP_MODE_SINGLE]->sid,
            MASKf);

    if (mask_minbit < 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Update each part with the proper key match_type. */
    for (part_idx = 0; part_idx < parts_cnt; ++part_idx) {
        key_match_sbit = key_minbit;
        bcmdrd_field_set(ekw[part_idx],
                         key_match_sbit,
                         key_match_sbit + key_match_width - 1,
                         &data);
        key_match_mask_sbit = mask_minbit;
        bcmdrd_field_set(ekw[part_idx],
                         key_match_mask_sbit,
                         key_match_mask_sbit + key_match_width - 1,
                         &mask);
    }

exit:

    SHR_FUNC_EXIT();
}
