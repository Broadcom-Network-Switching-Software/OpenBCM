/*! \file bcm56880_a0_cth_vlan.c
 *
 * bcm56880 a0 VLAN drivers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_vlan_util.h>
#include <bcmcth/bcmcth_vlan_drv.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmdrd/chip/bcm56880_a0_defs.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_VLAN

/*******************************************************************************
 * Typedefs
 */
#define BCM56880_LOWER_PORTS 80
#define BCM56880_ALL_PORTS 160

/*******************************************************************************
 * Private variables
 */
static int
bcm56880_a0_cth_ing_state_profile_fields_set(int unit, uint32_t trans_id,
                                             bcmltd_sid_t lt_id, bool lower,
                                             vlan_ing_stg_profile_t *lt_info)
{
    MEMBERSHIP_CHECK_ING0_STATE_PROFILE_LOWERm_t lower_buf;
    MEMBERSHIP_CHECK_ING0_STATE_PROFILE_UPPERm_t upper_buf;
    bcmdrd_pbmp_t bitmap_field_value;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    sal_memset(&lower_buf, 0, sizeof(lower_buf));
    sal_memset(&upper_buf, 0, sizeof(upper_buf));
    sal_memset(&bitmap_field_value, 0, sizeof(bitmap_field_value));

    if (lower) {
        for (i = 0;i < BCM56880_LOWER_PORTS; i++) {
            if (lt_info->state[i] & 1) {
                BCMDRD_PBMP_PORT_ADD(bitmap_field_value, i * 2);
            }
            if (lt_info->state[i] & 2) {
                BCMDRD_PBMP_PORT_ADD(bitmap_field_value, i * 2 + 1);
            }
        }
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 MEMBERSHIP_CHECK_ING0_STATE_PROFILE_LOWERm,
                                 (uint32_t *)&lower_buf,
                                 BMPf,
                                 (uint32_t *)&bitmap_field_value));

        /* Set up hardware table index to write. */
        SHR_IF_ERR_EXIT
            (bcmcth_vlan_mreq_write(unit, trans_id, lt_id,
                                    MEMBERSHIP_CHECK_ING0_STATE_PROFILE_LOWERm,
                                    lt_info->stg_id, (uint32_t *)&lower_buf));
    } else {
        for (i = BCM56880_LOWER_PORTS; i < BCM56880_ALL_PORTS; i++) {
            if (lt_info->state[i] & 1) {
                BCMDRD_PBMP_PORT_ADD(bitmap_field_value, (i - BCM56880_LOWER_PORTS) * 2);
            }
            if (lt_info->state[i] & 2) {
                BCMDRD_PBMP_PORT_ADD(bitmap_field_value, (i - BCM56880_LOWER_PORTS) * 2 + 1);
            }
        }
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 MEMBERSHIP_CHECK_ING0_STATE_PROFILE_UPPERm,
                                 (uint32_t *)&upper_buf,
                                 BMPf,
                                 (uint32_t *)&bitmap_field_value));

        /* Set up hardware table index to write. */
        SHR_IF_ERR_EXIT
            (bcmcth_vlan_mreq_write(unit, trans_id, lt_id,
                                    MEMBERSHIP_CHECK_ING0_STATE_PROFILE_UPPERm,
                                    lt_info->stg_id, (uint32_t *)&upper_buf));

    }

exit:
    SHR_FUNC_EXIT();

}

static int
bcm56880_a0_cth_block_mask_membership_fields_set(int unit, uint32_t trans_id,
                                                 bcmltd_sid_t lt_id, bool lower,
                                                 vlan_ing_egr_stg_member_profile_t *lt_info)
{
    IPOST_MEMBERSHIP_CHECK_BLOCK_MASK_MEMBERSHIP_STATE_LOWERm_t lower_buf;
    IPOST_MEMBERSHIP_CHECK_BLOCK_MASK_MEMBERSHIP_STATE_UPPERm_t upper_buf;
    bcmdrd_pbmp_t bitmap_field_value;
    uint32_t field_value;
    int i;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    if (!BCMCTH_VLAN_LT_FIELD_GET(lt_info->fbmp, STG_STATE_MASK) &&
        !BCMCTH_VLAN_LT_FIELD_GET(lt_info->fbmp, STG_COMPARED_VALUE) &&
        !BCMCTH_VLAN_LT_FIELD_GET(lt_info->fbmp, STG_MASK_TARGET) &&
        !BCMCTH_VLAN_LT_FIELD_GET(lt_info->fbmp, STG_MAKS_ACTION)) {
        SHR_EXIT();
    }
    sal_memset(&lower_buf, 0, sizeof(lower_buf));
    sal_memset(&upper_buf, 0, sizeof(upper_buf));
    sal_memset(&bitmap_field_value, 0, sizeof(bitmap_field_value));

    if (lower) {
        for (i = 0;i < BCM56880_LOWER_PORTS; i++) {
            if (lt_info->state[i] & 1) {
                BCMDRD_PBMP_PORT_ADD(bitmap_field_value, i * 2);
            }
            if (lt_info->state[i] & 2) {
                BCMDRD_PBMP_PORT_ADD(bitmap_field_value, i * 2 + 1);
            }
        }
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 IPOST_MEMBERSHIP_CHECK_BLOCK_MASK_MEMBERSHIP_STATE_LOWERm,
                                 (uint32_t *)&lower_buf,
                                 MEMBER_STATEf,
                                 (uint32_t *)&bitmap_field_value));

        /* Set up hardware table index to write. */
        SHR_IF_ERR_EXIT
            (bcmcth_vlan_mreq_write(unit, trans_id, lt_id,
                                    IPOST_MEMBERSHIP_CHECK_BLOCK_MASK_MEMBERSHIP_STATE_LOWERm,
                                    lt_info->stg_id, (uint32_t *)&lower_buf));
    } else {
        for (i = BCM56880_LOWER_PORTS; i < BCM56880_ALL_PORTS; i++) {
            if (lt_info->state[i] & 1) {
                BCMDRD_PBMP_PORT_ADD(bitmap_field_value, (i - BCM56880_LOWER_PORTS) * 2);
            }
            if (lt_info->state[i] & 2) {
                BCMDRD_PBMP_PORT_ADD(bitmap_field_value, (i - BCM56880_LOWER_PORTS) * 2 + 1);
            }
        }
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 IPOST_MEMBERSHIP_CHECK_BLOCK_MASK_MEMBERSHIP_STATE_UPPERm,
                                 (uint32_t *)&upper_buf,
                                 MEMBER_STATEf,
                                 (uint32_t *)&bitmap_field_value));

        field_value = (uint32_t)(lt_info->compared_state);
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 IPOST_MEMBERSHIP_CHECK_BLOCK_MASK_MEMBERSHIP_STATE_UPPERm,
                                 (uint32_t *)&upper_buf,
                                 STATE_COMPARE_VALUEf,
                                 &field_value));

        field_value = (uint32_t)(lt_info->mask_target);
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 IPOST_MEMBERSHIP_CHECK_BLOCK_MASK_MEMBERSHIP_STATE_UPPERm,
                                 (uint32_t *)&upper_buf,
                                 MASK_ACTION_TARGETf,
                                 &field_value));

        field_value = (uint32_t)(lt_info->mask_action);
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 IPOST_MEMBERSHIP_CHECK_BLOCK_MASK_MEMBERSHIP_STATE_UPPERm,
                                 (uint32_t *)&upper_buf,
                                 MASK_ACTION_TYPEf,
                                 &field_value));

        /* Set up hardware table index to write. */
        SHR_IF_ERR_EXIT
            (bcmcth_vlan_mreq_write(unit, trans_id, lt_id,
                                    IPOST_MEMBERSHIP_CHECK_BLOCK_MASK_MEMBERSHIP_STATE_UPPERm,
                                    lt_info->stg_id, (uint32_t *)&upper_buf));

    }

exit:
    SHR_FUNC_EXIT();

}

static int
bcm56880_a0_cth_egr_state_profile_fields_set(int unit, uint32_t trans_id,
                                             bcmltd_sid_t lt_id, bool lower,
                                             vlan_egr_stg_profile_t *lt_info)
{
    EGR_MEMBERSHIP_STATE_PROFILE_LOWERm_t lower_buf;
    EGR_MEMBERSHIP_STATE_PROFILE_UPPERm_t upper_buf;
    bcmdrd_pbmp_t bitmap_field_value;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    sal_memset(&lower_buf, 0, sizeof(lower_buf));
    sal_memset(&upper_buf, 0, sizeof(upper_buf));
    sal_memset(&bitmap_field_value, 0, sizeof(bitmap_field_value));

    if (lower) {
        for (i = 0;i < BCM56880_LOWER_PORTS; i++) {
            if (lt_info->state[i] & 1) {
                BCMDRD_PBMP_PORT_ADD(bitmap_field_value, i * 2);
            }
            if (lt_info->state[i] & 2) {
                BCMDRD_PBMP_PORT_ADD(bitmap_field_value, i * 2 + 1);
            }
        }
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 EGR_MEMBERSHIP_STATE_PROFILE_LOWERm,
                                 (uint32_t *)&lower_buf, BMPf,
                                 (uint32_t *)&bitmap_field_value));

        /* Set up hardware table index to write. */
        SHR_IF_ERR_EXIT
                (bcmcth_vlan_mreq_write(unit, trans_id, lt_id,
                                        EGR_MEMBERSHIP_STATE_PROFILE_LOWERm,
                                        lt_info->stg_id, (uint32_t *)&lower_buf));
    } else {
        for (i = BCM56880_LOWER_PORTS; i < BCM56880_ALL_PORTS; i++) {
            if (lt_info->state[i] & 1) {
                BCMDRD_PBMP_PORT_ADD(bitmap_field_value, (i - BCM56880_LOWER_PORTS) * 2);
            }
            if (lt_info->state[i] & 2) {
                BCMDRD_PBMP_PORT_ADD(bitmap_field_value, (i - BCM56880_LOWER_PORTS) * 2 + 1);
            }
        }
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 EGR_MEMBERSHIP_STATE_PROFILE_UPPERm,
                                 (uint32_t *)&upper_buf, BMPf,
                                 (uint32_t *)&bitmap_field_value));

        /* Set up hardware table index to write. */
        SHR_IF_ERR_EXIT
                (bcmcth_vlan_mreq_write(unit, trans_id, lt_id,
                                         EGR_MEMBERSHIP_STATE_PROFILE_UPPERm,
                                         lt_info->stg_id, (uint32_t *)&upper_buf));

    }

exit:
    SHR_FUNC_EXIT();

}

static int
bcm56880_a0_cth_vlan_ing_stg_profile_set(int unit, uint32_t trans_id,
                                         bcmltd_sid_t lt_id,
                                         vlan_ing_stg_profile_t *lt_info)
{
    bool state_profile_lower_update = false;
    bool state_profile_upper_update = false;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    if (BCMCTH_VLAN_LT_FIELD_GET(lt_info->fbmp, STG_STATE)) {
        for (i = 0; i < BCM56880_ALL_PORTS; i++) {
            if (BCMCTH_VLAN_LT_FIELD_GET(
                    lt_info->state_array_bmp, i)) {
                if (i < BCM56880_LOWER_PORTS) {
                    state_profile_lower_update = TRUE;
                } else {
                    state_profile_upper_update = TRUE;
                }
            }
        }
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "bcm56880_a0_cth_vlan_ing_stg_profile_set.\n")));

    if (state_profile_lower_update) {
        SHR_IF_ERR_EXIT
            (bcm56880_a0_cth_ing_state_profile_fields_set(unit,
                                                          trans_id,
                                                          lt_id,
                                                          1,
                                                          lt_info));
    }
    if (state_profile_upper_update) {
        SHR_IF_ERR_EXIT
            (bcm56880_a0_cth_ing_state_profile_fields_set(unit,
                                                          trans_id,
                                                          lt_id,
                                                          0,
                                                          lt_info));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_vlan_ing_egr_stg_member_profile_set(int unit, uint32_t trans_id,
                                                    bcmltd_sid_t lt_id,
                                                    vlan_ing_egr_stg_member_profile_t *lt_info)
{
    bool block_mask_membership_lower_update = false;
    bool block_mask_membership_upper_update = false;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    if (BCMCTH_VLAN_LT_FIELD_GET(lt_info->fbmp, STG_STATE_MASK)) {
        for (i = 0; i < BCM56880_ALL_PORTS; i++) {
            if (BCMCTH_VLAN_LT_FIELD_GET(
                    lt_info->state_array_bmp, i)) {
                if (i < BCM56880_LOWER_PORTS) {
                    block_mask_membership_lower_update = TRUE;
                } else {
                    block_mask_membership_upper_update = TRUE;
                }
            }
        }
    }


    if (BCMCTH_VLAN_LT_FIELD_GET(lt_info->fbmp, STG_COMPARED_VALUE) ||
        BCMCTH_VLAN_LT_FIELD_GET(lt_info->fbmp, STG_MASK_TARGET) ||
        BCMCTH_VLAN_LT_FIELD_GET(lt_info->fbmp, STG_MAKS_ACTION)) {
        block_mask_membership_upper_update = TRUE;
    }

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "bcm56880_a0_cth_vlan_ing_stg_flood_block_profile_set.\n")));

    if (block_mask_membership_lower_update) {
        SHR_IF_ERR_EXIT
            (bcm56880_a0_cth_block_mask_membership_fields_set(unit,
                                                              trans_id,
                                                              lt_id,
                                                              1,
                                                              lt_info));
    }

    if (block_mask_membership_upper_update) {
        SHR_IF_ERR_EXIT
            (bcm56880_a0_cth_block_mask_membership_fields_set(unit,
                                                              trans_id,
                                                              lt_id,
                                                              0,
                                                              lt_info));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_vlan_egr_stg_profile_set(int unit, uint32_t trans_id,
                                         bcmltd_sid_t lt_id,
                                         vlan_egr_stg_profile_t *lt_info)
{
    bool state_profile_lower_update = false;
    bool state_profile_upper_update = false;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    if (BCMCTH_VLAN_LT_FIELD_GET(lt_info->fbmp, STG_STATE)) {
        for (i = 0; i < BCM56880_ALL_PORTS; i++) {
            if (BCMCTH_VLAN_LT_FIELD_GET(
                    lt_info->state_array_bmp, i)) {
                if (i < BCM56880_LOWER_PORTS) {
                    state_profile_lower_update = TRUE;
                } else {
                    state_profile_upper_update = TRUE;
                }
            }
        }
    }

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "bcm56880_a0_cth_vlan_egr_stg_profile_set.\n")));

    if (state_profile_lower_update) {
        SHR_IF_ERR_EXIT
            (bcm56880_a0_cth_egr_state_profile_fields_set(unit,
                                                      trans_id,
                                                      lt_id,
                                                      1,
                                                      lt_info));
    }
    if (state_profile_upper_update) {
        SHR_IF_ERR_EXIT
            (bcm56880_a0_cth_egr_state_profile_fields_set(unit,
                                                      trans_id,
                                                      lt_id,
                                                      0,
                                                      lt_info));
    }

exit:
    SHR_FUNC_EXIT();

}

static bcmcth_vlan_drv_t bcm56880_a0_cth_vlan_drv = {
           .xlate_port_grp_set = NULL,
           .ing_stg_profile_set = bcm56880_a0_cth_vlan_ing_stg_profile_set,
           .ing_egr_stg_member_profile_set = bcm56880_a0_cth_vlan_ing_egr_stg_member_profile_set,
           .egr_stg_profile_set = bcm56880_a0_cth_vlan_egr_stg_profile_set
};
/*******************************************************************************
 * Public Functions
 */
bcmcth_vlan_drv_t *
bcm56880_a0_cth_vlan_drv_get(int unit)
{
    return &bcm56880_a0_cth_vlan_drv;
}

