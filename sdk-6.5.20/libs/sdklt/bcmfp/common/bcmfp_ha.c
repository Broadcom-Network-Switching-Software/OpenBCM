/*! \file bcmfp_ha.c
 *
 * BCMFP HA memory management APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_ha.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/bcmfp_trans_internal.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_em_internal.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmlrd/bcmlrd_map.h>
#include <shr/shr_util.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_uft.h>
#include <bcmissu/issu_api.h>
#include <bcmfp/generated/bcmfp_ha.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_HA

static void *ha_ptrs[BCMDRD_CONFIG_MAX_UNITS][BCMFP_HA_BLK_ID_MAX];

#define BCMFP_HA_PTR(unit, sub_comp) ha_ptrs[unit][sub_comp]

int
bcmfp_ha_root_blk_get(int unit, bcmfp_ha_root_blk_t **root)
{
    void *ha_blk = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(root, SHR_E_PARAM);

    ha_blk = BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_ROOT);
    if (ha_blk == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    (*root) = (bcmfp_ha_root_blk_t *)
              ((uint8_t *)ha_blk + sizeof(bcmfp_ha_blk_hdr_t));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_ha_block_report(int unit,
                            bcmfp_stage_id_t stage_id,
                            uint8_t blk_id,
                            bcmfp_ha_struct_offset_t *issu_offset)
{
    uint32_t group_oper_info_count = 0;
    uint32_t ha_blk_info_count = 0;
    uint32_t offset = 0;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(issu_offset, SHR_E_PARAM);

    /* Report BCMFP HA block header. */
    offset = issu_offset->ha_blk_hdr_offset;
    size = sizeof(bcmfp_ha_blk_hdr_t);
    bcmissu_struct_info_report(unit,
                               BCMMGMT_FP_COMP_ID,
                               blk_id,
                               offset,
                               size,
                               1,
                               BCMFP_HA_BLK_HDR_T_ID);

    /* Report stage operational information. */
    offset = issu_offset->stage_oper_info_offset;
    size = sizeof(bcmfp_stage_oper_info_t);
    bcmissu_struct_info_report(unit,
                               BCMMGMT_FP_COMP_ID,
                               blk_id,
                               offset,
                               size,
                               1,
                               BCMFP_STAGE_OPER_INFO_T_ID);

    /* Report reference count array of different profiles. */
    if (issu_offset->ref_count_array_size != 0) {
        offset = issu_offset->ref_count_offset;
        size = sizeof(bcmfp_ref_count_t);
        bcmissu_struct_info_report(unit,
                                   BCMMGMT_FP_COMP_ID,
                                   blk_id,
                                   offset,
                                   size,
                                   issu_offset->ref_count_array_size,
                                   BCMFP_REF_COUNT_T_ID);
    }

    /* Report PDD operational information. */
    if (issu_offset->num_pdd_profiles != 0) {
        offset = issu_offset->pdd_oper_info_offset;
        size = sizeof(bcmfp_pdd_oper_info_t);
        bcmissu_struct_info_report(unit,
                                   BCMMGMT_FP_COMP_ID,
                                   blk_id,
                                   offset,
                                   size,
                                   issu_offset->num_pdd_profiles,
                                   BCMFP_PDD_OPER_INFO_T_ID);
    }

    /* Report group(both main and presel) operational information. */
    group_oper_info_count = issu_offset->num_groups +
                            issu_offset->num_presel_groups;
    offset = issu_offset->group_oper_info_offset;
    size = sizeof(bcmfp_group_oper_info_t);
    bcmissu_struct_info_report(unit,
                               BCMMGMT_FP_COMP_ID,
                               blk_id,
                               offset,
                               size,
                               group_oper_info_count,
                               BCMFP_GROUP_OPER_INFO_T_ID);

    /* Report bcmfp_ha_blk_info_t structure array. */
    offset = issu_offset->ha_blk_info_offset;
    ha_blk_info_count = issu_offset->ha_blk_info_array_size;
    size = sizeof(bcmfp_ha_blk_info_t);
    bcmissu_struct_info_report(unit,
                               BCMMGMT_FP_COMP_ID,
                               blk_id,
                               offset,
                               size,
                               ha_blk_info_count,
                               BCMFP_HA_BLK_INFO_T_ID);
exit:
    SHR_FUNC_EXIT();
}

/*
 * Caliculate the bytes required for different refcount segements and
 * offsets of those segements in the HA block. All refcount segments
 * may or may not be required in all devices.
 */
static int
bcmfp_ha_stage_oper_ref_counts_size_get(int unit,
                                        bcmfp_stage_id_t stage_id,
                                        uint32_t *size,
                                        bcmfp_ha_struct_offset_t *issu_offset,
                                        uint32_t pipe_count)
{
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);
    SHR_NULL_CHECK(issu_offset, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    issu_offset->ref_count_array_size = 0;

    /* FP stage in the device supports global keygen program profiles. */
    if (stage->misc_info->num_keygen_prof != 0) {
        if (issu_offset->ref_count_offset == 0) {
            issu_offset->ref_count_offset = *size;
        }
        issu_offset->num_keygen_prof =
                     stage->misc_info->num_keygen_prof;
        issu_offset->keygen_prof_ref_count_offset = *size;
        *size += (pipe_count * stage->misc_info->num_keygen_prof *
                  sizeof(bcmfp_ref_count_t));
        issu_offset->ref_count_array_size +=
                     (stage->misc_info->num_keygen_prof * pipe_count);
    }

    /* FP stage in the device supports global PDD profiles. */
    if (stage->misc_info->num_pdd_profiles != 0) {
        if (issu_offset->ref_count_offset == 0) {
            issu_offset->ref_count_offset = *size;
        }
        issu_offset->num_action_prof =
                     stage->misc_info->num_pdd_profiles;
        issu_offset->action_prof_ref_count_offset = *size;
        *size += (pipe_count * stage->misc_info->num_pdd_profiles *
                  sizeof(bcmfp_ref_count_t));
        issu_offset->ref_count_array_size +=
                     (stage->misc_info->num_pdd_profiles * pipe_count);
    }

    /* FP stage in the device supports global QOS profiles. */
    if (stage->misc_info->num_qos_prof != 0) {
        if (issu_offset->ref_count_offset == 0) {
            issu_offset->ref_count_offset = *size;
        }
        issu_offset->num_qos_prof =
                     stage->misc_info->num_qos_prof;
        issu_offset->qos_prof_ref_count_offset = *size;
        *size += (pipe_count * stage->misc_info->num_qos_prof *
                  sizeof(bcmfp_ref_count_t));
        issu_offset->ref_count_array_size +=
                     (stage->misc_info->num_qos_prof * pipe_count);
    }

    /*
     * FP stage in the device supports global key attribute profiles.
     * This is applicable to HASH based FP stages.
     */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
        if (stage->misc_info->num_em_key_attrib_profile != 0) {
            if (issu_offset->ref_count_offset == 0) {
                issu_offset->ref_count_offset = *size;
            }
            issu_offset->num_em_key_attrib_profile =
                        stage->misc_info->num_em_key_attrib_profile;
            issu_offset->em_key_attrib_prof_ref_count_offset = *size;
            *size += (pipe_count *
                      stage->misc_info->num_em_key_attrib_profile *
                      sizeof(bcmfp_ref_count_t));
            issu_offset->ref_count_array_size +=
                        (stage->misc_info->num_em_key_attrib_profile *
                         pipe_count);
        }
    }

    /* FP stage in the device supports global SBR profiles. */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_SBR_PROFILES_GLOBAL)) {
        if (stage->misc_info->num_sbr_profiles != 0) {
            if (issu_offset->ref_count_offset == 0) {
                issu_offset->ref_count_offset = *size;
            }
            issu_offset->num_sbr_profiles =
                        stage->misc_info->num_sbr_profiles;
            issu_offset->sbr_prof_ref_count_offset = *size;
            *size += (pipe_count * stage->misc_info->num_sbr_profiles *
                      sizeof(bcmfp_ref_count_t));
            issu_offset->ref_count_array_size +=
                        (stage->misc_info->num_sbr_profiles * pipe_count);
        }
    }

    /* FP stage in the device supports global presel group profiles. */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_PSG_PROFILES_GLOBAL)) {
        if (stage->misc_info->num_presel_groups != 0) {
            if (issu_offset->ref_count_offset == 0) {
                issu_offset->ref_count_offset = *size;
            }
            issu_offset->num_psg_profiles =
                        stage->misc_info->num_presel_groups;
            issu_offset->psg_prof_ref_count_offset = *size;
            *size += (pipe_count * stage->misc_info->num_presel_groups *
                      sizeof(bcmfp_ref_count_t));
            issu_offset->ref_count_array_size +=
                        (stage->misc_info->num_presel_groups * pipe_count);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Caliculate the bytes required for different template IDs to entry ID
 * mapping array segments  and offsets of those segements in the HA block.
 * All the map segments may or may not be required in all devices.
 */
static int
bcmfp_ha_stage_oper_info_entry_maps_size_get(int unit,
                                    bcmfp_stage_id_t stage_id,
                                    uint32_t *size,
                                    bcmfp_ha_struct_offset_t *issu_offset)
{
    uint8_t div_factor = 1;
    bcmfp_stage_t *stage = NULL;
    uint32_t max_entries = 0;
    uint32_t max_meter_id = 0;
    uint32_t max_pdd_id =0;
    uint32_t max_sbr_id =0;
    uint32_t max_ctr_id =0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    div_factor = stage->idp_maps_modulo;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_max_entries_get(unit, stage_id, &max_entries));
    max_entries = (max_entries / div_factor);
    if (max_entries != 0) {
       /* Group, Rule and Policy template id mapping with entry ids */
       issu_offset->ha_blk_info_offset= *size;
       issu_offset->group_entry_map_offset = *size;
       issu_offset->group_entry_map_size = (max_entries + 1);
       *size += (sizeof(bcmfp_ha_blk_info_t) * (max_entries + 1));
       issu_offset->rule_entry_map_offset = *size;
       issu_offset->rule_entry_map_size = (max_entries + 1);
       *size += (sizeof(bcmfp_ha_blk_info_t) * (max_entries + 1));
       issu_offset->policy_entry_map_offset = *size;
       issu_offset->policy_entry_map_size = (max_entries + 1);
       *size += (sizeof(bcmfp_ha_blk_info_t) * (max_entries + 1));
       issu_offset->ha_blk_info_array_size += ((max_entries + 1) * 3);
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Meter template id mapping with entry ids */
    if (!(BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_NO_METER_SUPPORT))) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_max_meter_id_get(unit, stage_id, &max_meter_id));
        max_meter_id = (max_meter_id / div_factor);
        if (max_meter_id != 0)  {
            if (issu_offset->ha_blk_info_offset == 0) {
                issu_offset->ha_blk_info_offset= *size;
            }
            issu_offset->ha_blk_info_array_size += (max_meter_id + 1);
            issu_offset->meter_entry_map_size = (max_meter_id + 1);
            issu_offset->meter_entry_map_offset = *size;
            *size += (sizeof(bcmfp_ha_blk_info_t) * (max_meter_id + 1));
        }
    }

    /* Egress counters ID mapping with entry IDs */
    if ((BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_LEGACY_CTR_SUPPORT))) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_max_ctr_id_get(unit, stage_id, &max_ctr_id));
        max_ctr_id = (max_ctr_id / div_factor);
        if (max_ctr_id != 0) {
            if (issu_offset->ha_blk_info_offset == 0) {
                issu_offset->ha_blk_info_offset= *size;
            }
            issu_offset->ha_blk_info_array_size += (max_ctr_id + 1);
            issu_offset->ctr_entry_map_size = (max_ctr_id + 1);
            issu_offset->ctr_entry_map_offset = *size;
            *size += (sizeof(bcmfp_ha_blk_info_t) * (max_ctr_id + 1));
        }
    }

    /* PDD template ID mapping with entry ids, If PDD is per entry. */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_POLICY_TYPE_PDD) &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_PDD_PER_ENTRY)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_max_pdd_id_get(unit, stage_id, &max_pdd_id));
        if (max_pdd_id > div_factor) {
            max_pdd_id = (max_pdd_id / div_factor);
        }
        if (max_pdd_id != 0)  {
            if (issu_offset->ha_blk_info_offset == 0) {
                issu_offset->ha_blk_info_offset= *size;
            }
            issu_offset->ha_blk_info_array_size += (max_pdd_id + 1);
            issu_offset->pdd_entry_map_size = (max_pdd_id + 1);
            issu_offset->pdd_entry_map_offset = *size;
            *size += (sizeof(bcmfp_ha_blk_info_t) * (max_pdd_id + 1));
        }
    }

    /* SBR template ID mapping with entry ids, If SBR is per entry. */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ACTION_RESOLUTION_SBR) &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_SBR_PER_ENTRY)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_max_sbr_id_get(unit, stage_id, &max_sbr_id));
        if (max_sbr_id > div_factor) {
            max_sbr_id = (max_sbr_id / div_factor);
        }
        if (max_sbr_id != 0)  {
            if (issu_offset->ha_blk_info_offset == 0) {
                issu_offset->ha_blk_info_offset= *size;
            }
            issu_offset->ha_blk_info_array_size += (max_sbr_id + 1);
            issu_offset->sbr_entry_map_size = (max_sbr_id + 1);
            issu_offset->sbr_entry_map_offset = *size;
            *size += (sizeof(bcmfp_ha_blk_info_t) * (max_sbr_id + 1));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Caliculate the bytes required for different template IDs to group ID
 * mapping array segments  and offsets of those segements in the HA block.
 * All the map segments may or may not be required in all devices.
 */
static int
bcmfp_ha_stage_oper_group_maps_size_get(int unit,
                                        bcmfp_stage_id_t stage_id,
                                        uint32_t *size,
                                        bcmfp_ha_struct_offset_t *issu_offset)
{
    bcmfp_stage_t *stage = NULL;
    uint32_t max_presel_id = 0;
    uint32_t max_pdd_id = 0;
    uint32_t max_sbr_id = 0;
    uint32_t max_psg_id = 0;
    uint8_t div_factor = 1;
    uint32_t max_dpolicy_id = 0;
    uint32_t max_entries = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    div_factor = stage->idp_maps_modulo;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_max_entries_get(unit, stage_id, &max_entries));
    max_entries = (max_entries / div_factor);

    /* Presel template id mapping with group ids */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_KEY_TYPE_PRESEL)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_max_presel_id_get(unit, stage_id, &max_presel_id));
        if (max_presel_id > div_factor) {
            max_presel_id = (max_presel_id / div_factor);
        }
        if (max_presel_id != 0)  {
            if (issu_offset->ha_blk_info_offset == 0) {
                issu_offset->ha_blk_info_offset= *size;
            }
            issu_offset->ha_blk_info_array_size += (max_presel_id + 1);
            issu_offset->pse_group_map_size = (max_presel_id + 1);
            issu_offset->pse_group_map_offset = *size;
            *size += (sizeof(bcmfp_ha_blk_info_t) * (max_presel_id + 1));
        }
    }

    /* PSG template id mapping with group ids */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_PRESEL_KEY_DYNAMIC)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_max_psg_id_get(unit, stage_id, &max_psg_id));
        if (max_psg_id > div_factor) {
            max_psg_id = (max_psg_id / div_factor);
        }
        if (max_psg_id != 0)  {
            if (issu_offset->ha_blk_info_offset == 0) {
                issu_offset->ha_blk_info_offset= *size;
            }
            issu_offset->ha_blk_info_array_size += (max_psg_id + 1);
            issu_offset->psg_group_map_size = (max_psg_id + 1);
            issu_offset->psg_group_map_offset = *size;
            *size += (sizeof(bcmfp_ha_blk_info_t) * (max_psg_id + 1));
        }
    }

    /* PDD template id mapping with group ids */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_POLICY_TYPE_PDD)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_max_pdd_id_get(unit, stage_id, &max_pdd_id));
        if (max_pdd_id > div_factor) {
            max_pdd_id = (max_pdd_id / div_factor);
        }
        if (max_pdd_id != 0)  {
            if (issu_offset->ha_blk_info_offset == 0) {
                issu_offset->ha_blk_info_offset= *size;
            }
            issu_offset->ha_blk_info_array_size += (max_pdd_id + 1);
            issu_offset->pdd_group_map_size = (max_pdd_id + 1);
            issu_offset->pdd_group_map_offset = *size;
            *size += (sizeof(bcmfp_ha_blk_info_t) * (max_pdd_id + 1));
        }
    }

    /* SBR template id mapping with group ids */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ACTION_RESOLUTION_SBR)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_max_sbr_id_get(unit, stage_id, &max_sbr_id));
        if (max_sbr_id > div_factor) {
            max_sbr_id = (max_sbr_id / div_factor);
        }
        if (max_sbr_id != 0)  {
            if (issu_offset->ha_blk_info_offset == 0) {
                issu_offset->ha_blk_info_offset= *size;
            }
            issu_offset->ha_blk_info_array_size += (max_sbr_id + 1);
            issu_offset->sbr_group_map_size = (max_sbr_id + 1);
            issu_offset->sbr_group_map_offset = *size;
            *size += (sizeof(bcmfp_ha_blk_info_t) * (max_sbr_id + 1));
        }
    }

    /* Default PDD template id mapping with group ids */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH) &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_POLICY_TYPE_PDD)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_max_pdd_id_get(unit, stage_id, &max_pdd_id));
        if (max_pdd_id > div_factor) {
            max_pdd_id = (max_pdd_id / div_factor);
        }
        if (max_pdd_id != 0)  {
            if (issu_offset->ha_blk_info_offset == 0) {
                issu_offset->ha_blk_info_offset= *size;
            }
            issu_offset->ha_blk_info_array_size += (max_pdd_id + 1);
            issu_offset->dpdd_group_map_size = (max_pdd_id + 1);
            issu_offset->dpdd_group_map_offset = *size;
            *size += (sizeof(bcmfp_ha_blk_info_t) * (max_pdd_id + 1));
        }
    }

    /* Default policy template id mapping with group ids */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH)) {
        max_dpolicy_id = max_entries;
        /*
         * Policy IDs are huge but group IDs are very few so creating
         * limited size hash table.
         */
        if (max_dpolicy_id > div_factor) {
            max_dpolicy_id = (max_dpolicy_id / div_factor);
        }
        if (max_dpolicy_id != 0)  {
            if (issu_offset->ha_blk_info_offset == 0) {
                issu_offset->ha_blk_info_offset= *size;
            }
            issu_offset->ha_blk_info_array_size += (max_dpolicy_id + 1);
            issu_offset->dpolicy_group_map_size = (max_dpolicy_id + 1);
            issu_offset->dpolicy_group_map_offset = *size;
            *size += (sizeof(bcmfp_ha_blk_info_t) * (max_dpolicy_id + 1));
        }
    }

    /* Defaault SBR template id mapping with group ids */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ACTION_RESOLUTION_SBR) &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_max_sbr_id_get(unit, stage_id, &max_sbr_id));
        if (max_sbr_id > div_factor) {
            max_sbr_id = (max_sbr_id / div_factor);
        }
        if (max_sbr_id != 0)  {
            if (issu_offset->ha_blk_info_offset == 0) {
                issu_offset->ha_blk_info_offset= *size;
            }
            issu_offset->ha_blk_info_array_size += (max_sbr_id + 1);
            issu_offset->dsbr_group_map_size = (max_sbr_id + 1);
            issu_offset->dsbr_group_map_offset = *size;
            *size += (sizeof(bcmfp_ha_blk_info_t) * (max_sbr_id + 1));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Caliculate the bytes required for different legacy counter state
 * segments  and offsets of those segements in the HA block.
 * These segments may or may not be required in all devices.
 */
static int
bcmfp_ha_stage_oper_ctrs_stats_size_get(int unit,
                                        bcmfp_stage_id_t stage_id,
                                        uint32_t *size,
                                        bcmfp_ha_struct_offset_t *issu_offset)
{
    uint32_t max_ctr_id = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if ((BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_LEGACY_CTR_SUPPORT))) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_max_ctr_id_get(unit, stage_id, &max_ctr_id));

        if (max_ctr_id != 0)  {
            issu_offset->num_ctr_ids = max_ctr_id;

            /* Bitmap of ctr HW indexes */
            issu_offset->ctr_hw_index_bitmap_offset = *size;
            issu_offset->ctr_hw_index_bitmap_size = ((max_ctr_id + 31) / 32);
            *size += ((max_ctr_id + 31) / 32) * sizeof(uint32_t);

            /* Mapping of ctr_id to ctr HW index */
            issu_offset->ctr_id_to_hw_index_map_offset = *size;
            issu_offset->ctr_id_to_hw_index_map_size = max_ctr_id;
            *size +=  max_ctr_id * sizeof(uint16_t);

            /* Bitmap of CTR_EGR_FP_STAT IDs */
            issu_offset->ctr_egr_stats_id_bmp_offset = *size;
            issu_offset->ctr_egr_stats_id_bmp_size = ((max_ctr_id + 31) / 32);
            *size += ((max_ctr_id + 31) / 32) * sizeof(uint32_t);

            /* Pkt count associated with  CTR_EGR_FP_STAT ID  */
            issu_offset->ctr_egr_stat_pkt_count_map_offset = *size;
            issu_offset->ctr_egr_stat_pkt_count_map_size = max_ctr_id;
            *size +=  max_ctr_id * sizeof(uint64_t);

            /* Byte count associated with  CTR_EGR_FP_STAT ID  */
            issu_offset->ctr_egr_stat_byte_count_map_offset = *size;
            issu_offset->ctr_egr_stat_byte_count_map_size = max_ctr_id;
            *size +=  max_ctr_id * sizeof(uint64_t);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Caliculate the size required for group operational information
 * of max groups supported in the stage. This segment is mandatory
 * for all FP stages in all devices.
 */
static int
bcmfp_ha_group_oper_info_size_get(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint32_t *size,
                              bcmfp_ha_struct_offset_t *issu_offset)
{
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    issu_offset->group_oper_info_offset= *size;
    issu_offset->num_groups = stage->misc_info->num_groups;
    *size += (stage->misc_info->num_groups *
              sizeof(bcmfp_group_oper_info_t));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Caliculate the size required for group operational information
 * of max presel groups supported in the stage. This segment may or
 * may not be required in all FP stages in all devices.
 */
static int
bcmfp_ha_presel_group_oper_info_size_get(int unit,
                                bcmfp_stage_id_t stage_id,
                                uint32_t *size,
                                bcmfp_ha_struct_offset_t *issu_offset)
{
    bcmfp_stage_t *stage = NULL;
    uint32_t max_psg_id =0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (!BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_PRESEL_KEY_DYNAMIC)) {
        SHR_EXIT();
    }

    /*
     * If presel doesnot have fixed key format, then key format
     * will be derived run time and needs to store the presel
     * group operational information. Though the presel group id
     * 0 is not a valid group id, that is used to create a presel
     * group internally if all the qualifiers present in
     * FP_XXX_PRESEL_ENTRY_TEMPLATE LT fits in presel TCAM key.
     * This is the reason for creating memory for one more than
     * num_presel_groups.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_max_psg_id_get(unit, stage_id, &max_psg_id));
    issu_offset->presel_group_oper_info_offset = *size;
    issu_offset->num_presel_groups = (max_psg_id + 1);
    *size += (sizeof(bcmfp_group_oper_info_t) * (max_psg_id + 1));
exit:
    SHR_FUNC_EXIT();

}

/*
 * Caliculate the size required for PDD operational information
 * of max PDD IDs supported in the stage. This segment may or
 * may not be required in all FP stages in all devices.
 */
static int
bcmfp_ha_pdd_oper_info_size_get(int unit,
                            bcmfp_stage_id_t stage_id,
                            uint32_t *size,
                            bcmfp_ha_struct_offset_t *issu_offset)
{
    bcmfp_stage_t *stage = NULL;
    uint32_t max_pdd_id =0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (!BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_POLICY_TYPE_PDD)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_max_pdd_id_get(unit, stage_id, &max_pdd_id));

    if (max_pdd_id != 0) {
        issu_offset->pdd_oper_info_offset = *size;
        issu_offset->num_pdd_profiles = (max_pdd_id + 1);
        *size += (sizeof(bcmfp_pdd_oper_info_t) * (max_pdd_id + 1));
     }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Get the total size of stage operational info HA block of a stage.
 * This function also keeps track of all individual segements sizes
 * and those segments offsets in the HA block. This information is
 * used when HA blocks are intialized(bcmfp_stage_oper_info_init_cb)
 * or when HA block is updated(bcmfp_stage_oper_info_init_wb).
 */
int
bcmfp_ha_stage_oper_size_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t *size,
                             bcmfp_ha_struct_offset_t *issu_offset)
{
    int rv;
    bcmfp_stage_t *stage = NULL;
    uint32_t pipe_count = 1;
    uint32_t pipe_map = 0;
    const bcmlrd_map_t *map = NULL;
    bcmfp_stage_oper_mode_t oper_mode;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);
    *size = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->misc_info == NULL) {
       SHR_EXIT();
    }
    /* Default configuration is GLOBAL*/
    rv = bcmlrd_map_get(unit, FP_CONFIGt, &map);
    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_oper_mode_get(unit,
                                       stage_id,
                                       &oper_mode));

        if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
            /* Get the number of pipes */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_dev_valid_pipes_get(unit,
                                           stage_id,
                                           &pipe_map));
            pipe_count = shr_util_popcount(pipe_map);
        }
    }

    /* HA block header. */
    issu_offset->ha_blk_hdr_offset = *size;
    *size += sizeof(bcmfp_ha_blk_hdr_t);

    /* Stage operational information. */
    issu_offset->stage_oper_info_offset= *size;
    *size += sizeof(bcmfp_stage_oper_info_t);

    /* Accumulate the size required for reference count segements. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_stage_oper_ref_counts_size_get(unit,
                                                 stage_id,
                                                 size,
                                                 issu_offset,
                                                 pipe_count));
    /*
     * Accumulate the size required for different template IDs
     * to entry ID map arrays.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_stage_oper_info_entry_maps_size_get(unit,
                                                      stage_id,
                                                      size,
                                                      issu_offset));
    /*
     * Accumulate the size required for different template IDs
     * to group ID map arrays.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_stage_oper_group_maps_size_get(unit,
                                                 stage_id,
                                                 size,
                                                 issu_offset));
    /*
     * Accumulate the size required for different states of
     * legacy counters.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_stage_oper_ctrs_stats_size_get(unit,
                                                 stage_id,
                                                 size,
                                                 issu_offset));

    /*
     * Accumulate the size required for group operation info of
     * main TCAM groups.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_group_oper_info_size_get(unit,
                                           stage_id,
                                           size,
                                           issu_offset));

    /*
     * Accumulate the size required for group operation info of
     * presel TCAM groups.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_presel_group_oper_info_size_get(unit,
                                              stage_id,
                                              size,
                                              issu_offset));

    /*
     * Accumulate the size required for PDD operation info of
     * PDD template IDs.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_pdd_oper_info_size_get(unit,
                                         stage_id,
                                         size,
                                         issu_offset));
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_fid_offset_info_blk_create(int unit,
                                 uint8_t blk_id,
                                 uint32_t blk_size)
{
    void *ha_ptr = NULL;
    uint8_t backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t idx = 0;
    uint32_t num_ha_elements = 0;
    uint32_t size = blk_size, alloc_sz = 0;
    uint32_t free_blk_offset = 0;
    bcmfp_fid_offset_info_t *fid_offset_info = NULL;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_ha_blk_hdr_t *backup_blk_hdr = NULL;
    bcmfp_ha_root_blk_t *root = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT(
        (blk_id > BCMFP_HA_BLK_ID_MAX) ? SHR_E_PARAM : SHR_E_NONE);
    SHR_IF_ERR_VERBOSE_EXIT(
        (blk_id <= BCMFP_HA_BLK_ID_INVALID) ? SHR_E_PARAM : SHR_E_NONE);
    SHR_IF_ERR_VERBOSE_EXIT(
        (blk_size % sizeof(bcmfp_fid_offset_info_t) != 0) ?
                              SHR_E_PARAM : SHR_E_NONE);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_root_blk_get(unit, &root));
    size += sizeof(bcmfp_ha_blk_hdr_t);
    alloc_sz = size;
    BCMFP_HA_PTR(unit, blk_id) =
                shr_ha_mem_alloc(unit,
                                 BCMMGMT_FP_COMP_ID,
                                 blk_id,
                                 "bcmfpHaBlk",
                                 &alloc_sz);
    SHR_NULL_CHECK(BCMFP_HA_PTR(unit, blk_id), SHR_E_MEMORY);
    if (alloc_sz < size) {
        SHR_IF_ERR_EXIT(shr_ha_mem_free(unit, BCMFP_HA_PTR(unit, blk_id)));
        alloc_sz = size;
        BCMFP_HA_PTR(unit, blk_id) =
            shr_ha_mem_alloc(unit,
                             BCMMGMT_FP_COMP_ID,
                             blk_id,
                             "bcmfpHaBlk",
                             &alloc_sz);
        SHR_NULL_CHECK(BCMFP_HA_PTR(unit, blk_id), SHR_E_MEMORY);
    }
    ha_ptr = BCMFP_HA_PTR(unit, blk_id);
    sal_memset(ha_ptr, 0, size);
    bcmissu_struct_info_report(unit,
                               BCMMGMT_FP_COMP_ID,
                               blk_id,
                               0,
                               sizeof(bcmfp_ha_blk_hdr_t),
                               1,
                               BCMFP_HA_BLK_HDR_T_ID);
    num_ha_elements = blk_size / sizeof(bcmfp_fid_offset_info_t);
     if (!num_ha_elements)  {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "BCMFP:HA memory with block ID:%0d "
                 "is created with size %0d\n"), blk_id, blk_size));
    }
    bcmissu_struct_info_report(unit,
                               BCMMGMT_FP_COMP_ID,
                               blk_id,
                               sizeof(bcmfp_ha_blk_hdr_t),
                               sizeof(bcmfp_fid_offset_info_t),
                               num_ha_elements,
                               BCMFP_FID_OFFSET_INFO_T_ID);

    SHR_BITSET(root->in_use_ha_blk_id_bmp.w, blk_id);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMFP:HA memory with block ID:%0d "
         "is created with size %0d\n"), blk_id, blk_size));
    /* Inialize the HA elements which starts after block header. */
    fid_offset_info = (bcmfp_fid_offset_info_t *)
                      ((uint8_t *)ha_ptr + sizeof(bcmfp_ha_blk_hdr_t));
    free_blk_offset = sizeof(bcmfp_ha_blk_hdr_t);

    SHR_IF_ERR_EXIT(
        (num_ha_elements == 0) ? SHR_E_INTERNAL : SHR_E_NONE);
    /*
     * Initialize the TV(type and value) in HA elements to
     * default values. And link all HA elements to its next
     * HA element.
     */
    for (idx = 0; idx < (num_ha_elements - 1); idx++) {
        free_blk_offset += sizeof(bcmfp_fid_offset_info_t);
        sal_memset(&fid_offset_info[idx], 0, sizeof(bcmfp_fid_offset_info_t));
        fid_offset_info[idx].offset_info.blk_id = BCMFP_HA_BLK_ID_INVALID;
        fid_offset_info[idx].offset_info.blk_offset =
                                         BCMFP_HA_BLK_OFFSET_INVALID;
        fid_offset_info[idx].next_blk_id = blk_id;
        fid_offset_info[idx].next_blk_offset = free_blk_offset;
    }
    /* Last HA element in the block initialized seperately as
     * next_blk_id and next_blk_offset are set to INVALID values.
     */
    for (; idx < num_ha_elements; idx++) {
        sal_memset(&fid_offset_info[idx], 0, sizeof(bcmfp_fid_offset_info_t));
        fid_offset_info[idx].offset_info.blk_id = BCMFP_HA_BLK_ID_INVALID;
        fid_offset_info[idx].offset_info.blk_offset =
                                         BCMFP_HA_BLK_OFFSET_INVALID;
        fid_offset_info[idx].next_blk_id = BCMFP_HA_BLK_ID_INVALID;
        fid_offset_info[idx].next_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    }

    /* Initialize the block header. */
    blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_ptr;
    blk_hdr->free_blk_offset = sizeof(bcmfp_ha_blk_hdr_t);
    blk_hdr->free_ha_element_count = num_ha_elements;
    blk_hdr->blk_size = size;
    blk_hdr->backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
    blk_hdr->signature = BCMFP_HA_STRUCT_SIGN + blk_id;
    blk_hdr->bucket_size = 1;
    blk_hdr->blk_type = BCMFP_HA_BLK_TYPE_FID_OFFSET_INFO;
    /* Create the back up block Id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
    blk_hdr->backup_blk_id = backup_blk_id;
    alloc_sz = size;
    BCMFP_HA_PTR(unit, backup_blk_id) =
                shr_ha_mem_alloc(unit,
                                 BCMMGMT_FP_COMP_ID,
                                 backup_blk_id,
                                 "bcmfpHaBkupBlk",
                                 &alloc_sz);
    SHR_NULL_CHECK(BCMFP_HA_PTR(unit, backup_blk_id), SHR_E_MEMORY);
    if (alloc_sz < size) {
        SHR_IF_ERR_EXIT
            (shr_ha_mem_free(unit, BCMFP_HA_PTR(unit, backup_blk_id)));
        alloc_sz = size;
        BCMFP_HA_PTR(unit, backup_blk_id) =
            shr_ha_mem_alloc(unit,
                             BCMMGMT_FP_COMP_ID,
                             backup_blk_id,
                             "bcmfpHaBkupBlk",
                             &alloc_sz);
        SHR_NULL_CHECK(BCMFP_HA_PTR(unit, backup_blk_id), SHR_E_MEMORY);
    }
    backup_blk_hdr =
        (bcmfp_ha_blk_hdr_t *)BCMFP_HA_PTR(unit, backup_blk_id);
    bcmissu_struct_info_report(unit,
                               BCMMGMT_FP_COMP_ID,
                               backup_blk_id,
                               0,
                               sizeof(bcmfp_ha_blk_hdr_t),
                               1,
                               BCMFP_HA_BLK_HDR_T_ID);
    bcmissu_struct_info_report(unit,
                               BCMMGMT_FP_COMP_ID,
                               backup_blk_id,
                               sizeof(bcmfp_ha_blk_hdr_t),
                               sizeof(bcmfp_fid_offset_info_t),
                               num_ha_elements,
                               BCMFP_FID_OFFSET_INFO_T_ID);
    SHR_BITSET(root->in_use_ha_blk_id_bmp.w, backup_blk_id);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMFP:HA memory with block ID:%0d "
         "is created with size %0d\n"), backup_blk_id, blk_size));
    /* Copy the Active block to backup block. */
    sal_memcpy(BCMFP_HA_PTR(unit, backup_blk_id),
               BCMFP_HA_PTR(unit, blk_id), size);
    backup_blk_hdr->signature = BCMFP_HA_STRUCT_SIGN + backup_blk_id;
    backup_blk_hdr->backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_ha_blk_create(int unit,
                    uint8_t blk_id,
                    uint32_t blk_size,
                    uint32_t bucket_size)
{
    void *ha_ptr = NULL;
    uint8_t backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t idx = 0;
    uint32_t num_ha_elements = 0;
    uint32_t size = blk_size, alloc_sz = 0;
    uint32_t free_blk_offset = 0;
    bcmfp_generic_data_t *ha_element = NULL;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_ha_blk_hdr_t *backup_blk_hdr = NULL;
    bcmfp_ha_root_blk_t *root = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT(
        (blk_id > BCMFP_HA_BLK_ID_MAX) ? SHR_E_PARAM : SHR_E_NONE);
    SHR_IF_ERR_VERBOSE_EXIT(
        (blk_id <= BCMFP_HA_BLK_ID_INVALID) ? SHR_E_PARAM : SHR_E_NONE);
    SHR_IF_ERR_VERBOSE_EXIT(
        (blk_size % sizeof(bcmfp_generic_data_t) != 0) ?
                              SHR_E_PARAM : SHR_E_NONE);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_root_blk_get(unit, &root));
    size += sizeof(bcmfp_ha_blk_hdr_t);
    alloc_sz = size;
    BCMFP_HA_PTR(unit, blk_id) =
                shr_ha_mem_alloc(unit,
                                 BCMMGMT_FP_COMP_ID,
                                 blk_id,
                                 "bcmfpHaBlk",
                                 &alloc_sz);
    SHR_NULL_CHECK(BCMFP_HA_PTR(unit, blk_id), SHR_E_MEMORY);
    if (alloc_sz < size) {
        SHR_IF_ERR_EXIT(shr_ha_mem_free(unit, BCMFP_HA_PTR(unit, blk_id)));
        alloc_sz = size;
        BCMFP_HA_PTR(unit, blk_id) =
            shr_ha_mem_alloc(unit,
                             BCMMGMT_FP_COMP_ID,
                             blk_id,
                             "bcmfpHaBlk",
                             &alloc_sz);
        SHR_NULL_CHECK(BCMFP_HA_PTR(unit, blk_id), SHR_E_MEMORY);
    }
    ha_ptr = BCMFP_HA_PTR(unit, blk_id);
    sal_memset(ha_ptr, 0, size);
    bcmissu_struct_info_report(unit,
                               BCMMGMT_FP_COMP_ID,
                               blk_id,
                               0,
                               sizeof(bcmfp_ha_blk_hdr_t),
                               1,
                               BCMFP_HA_BLK_HDR_T_ID);
    num_ha_elements = blk_size / sizeof(bcmfp_generic_data_t);
    if (!num_ha_elements)  {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "BCMFP:HA memory with block ID:%0d "
                "is created with size %0d\n"), blk_id, blk_size));
    }
    bcmissu_struct_info_report(unit,
                               BCMMGMT_FP_COMP_ID,
                               blk_id,
                               sizeof(bcmfp_ha_blk_hdr_t),
                               sizeof(bcmfp_generic_data_t),
                               num_ha_elements,
                               BCMFP_GENERIC_DATA_T_ID);
    SHR_BITSET(root->in_use_ha_blk_id_bmp.w, blk_id);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMFP:HA memory with block ID:%0d "
         "is created with size %0d\n"), blk_id, blk_size));
    /* Inialize the HA elements which starts after block header. */
    ha_element = (bcmfp_generic_data_t *)((uint8_t *)ha_ptr +
                                  sizeof(bcmfp_ha_blk_hdr_t));
    free_blk_offset = sizeof(bcmfp_ha_blk_hdr_t);

    SHR_IF_ERR_EXIT(
        (num_ha_elements == 0) ? SHR_E_INTERNAL : SHR_E_NONE);
    /* Initialize the TV(type and value) in HA elements to
     * default values. And link all HA elements to its next
     * HA element.
     */
    for (idx = 0; idx < (num_ha_elements - bucket_size); idx++) {
         ha_element[idx].value = 0;
         if (idx % bucket_size == 0) {
             free_blk_offset +=
                  (sizeof(bcmfp_generic_data_t) * bucket_size);
             ha_element[idx].next_blk_id = blk_id;
             ha_element[idx].next_blk_offset = free_blk_offset;
         } else {
             ha_element[idx].next_blk_id = BCMFP_HA_BLK_ID_INVALID;
             ha_element[idx].next_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
         }
    }
    /* Last HA element in the block initialized seperately as
     * next_blk_id and next_blk_offset are set to INVALID values.
     */
    for (; idx < num_ha_elements; idx++) {
        ha_element[idx].value = 0;
        ha_element[idx].next_blk_id = BCMFP_HA_BLK_ID_INVALID;
        ha_element[idx].next_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    }

    /* Initialize the block header. */
    blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_ptr;
    blk_hdr->free_blk_offset = sizeof(bcmfp_ha_blk_hdr_t);
    blk_hdr->free_ha_element_count = num_ha_elements;
    blk_hdr->blk_size = size;
    blk_hdr->backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
    blk_hdr->signature = BCMFP_HA_STRUCT_SIGN + blk_id;
    blk_hdr->bucket_size = bucket_size;
    blk_hdr->blk_type = BCMFP_HA_BLK_TYPE_GENERIC_TV;
    /* Create the back up block Id. */
    SHR_IF_ERR_EXIT(
        bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
    blk_hdr->backup_blk_id = backup_blk_id;
    alloc_sz = size;
    BCMFP_HA_PTR(unit, backup_blk_id) =
                shr_ha_mem_alloc(unit,
                                 BCMMGMT_FP_COMP_ID,
                                 backup_blk_id,
                                 "bcmfpHaBkupBlk",
                                 &alloc_sz);
    SHR_NULL_CHECK(BCMFP_HA_PTR(unit, backup_blk_id), SHR_E_MEMORY);
    if (alloc_sz < size) {
        SHR_IF_ERR_EXIT
            (shr_ha_mem_free(unit, BCMFP_HA_PTR(unit, backup_blk_id)));
        alloc_sz = size;
        BCMFP_HA_PTR(unit, backup_blk_id) =
            shr_ha_mem_alloc(unit,
                             BCMMGMT_FP_COMP_ID,
                             backup_blk_id,
                             "bcmfpHaBkupBlk",
                             &alloc_sz);
        SHR_NULL_CHECK(BCMFP_HA_PTR(unit, backup_blk_id), SHR_E_MEMORY);
    }
    backup_blk_hdr =
        (bcmfp_ha_blk_hdr_t *)BCMFP_HA_PTR(unit, backup_blk_id);
    bcmissu_struct_info_report(unit,
                               BCMMGMT_FP_COMP_ID,
                               backup_blk_id,
                               0,
                               sizeof(bcmfp_ha_blk_hdr_t),
                               1,
                               BCMFP_HA_BLK_HDR_T_ID);
    bcmissu_struct_info_report(unit,
                               BCMMGMT_FP_COMP_ID,
                               backup_blk_id,
                               sizeof(bcmfp_ha_blk_hdr_t),
                               sizeof(bcmfp_generic_data_t),
                               num_ha_elements,
                               BCMFP_GENERIC_DATA_T_ID);
    SHR_BITSET(root->in_use_ha_blk_id_bmp.w, backup_blk_id);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMFP:HA memory with block ID:%0d "
         "is created with size %0d\n"), backup_blk_id, blk_size));
    /* Copy the Active block to backup block. */
    sal_memcpy(BCMFP_HA_PTR(unit, backup_blk_id),
               BCMFP_HA_PTR(unit, blk_id), size);
    backup_blk_hdr->signature = BCMFP_HA_STRUCT_SIGN + backup_blk_id;
    backup_blk_hdr->backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_ha_stage_init(int unit, bcmfp_stage_id_t stage_id, bool warm)
{
    uint32_t blk_size = 0;
    uint32_t req_blk_size = 0;
    bcmfp_ha_root_blk_t *root = NULL;
    void *ha_blk = NULL;
    void *backup_ha_blk = NULL;
    uint8_t blk_id = 0;
    uint8_t backup_blk_id = 0;
    bcmfp_ha_blk_hdr_t *backup_blk_hdr = NULL;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_ha_struct_offset_t issu_offset = { 0 };
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_stage_oper_size_get(unit,
                                      stage_id,
                                      &blk_size,
                                      &issu_offset));
    if (blk_size == 0) {
        SHR_EXIT();
    }
    req_blk_size = blk_size;
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_root_blk_get(unit, &root));
    /* Active HA block corresponding to a given stage. */
    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    BCMFP_HA_PTR(unit, blk_id) =
                 shr_ha_mem_alloc(unit,
                                  BCMMGMT_FP_COMP_ID,
                                  blk_id,
                                  "bcmfpStageBlock",
                                  &blk_size);
    ha_blk = BCMFP_HA_PTR(unit, blk_id);
    SHR_NULL_CHECK(ha_blk, SHR_E_MEMORY);
    if (blk_size < req_blk_size) {
        SHR_IF_ERR_VERBOSE_EXIT
            (shr_ha_mem_free(unit, ha_blk));
        blk_size = req_blk_size;
        BCMFP_HA_PTR(unit, blk_id) =
                 shr_ha_mem_alloc(unit,
                                  BCMMGMT_FP_COMP_ID,
                                  blk_id,
                                  "bcmfpStageBlock",
                                  &blk_size);
        ha_blk = BCMFP_HA_PTR(unit, blk_id);
        SHR_NULL_CHECK(ha_blk, SHR_E_MEMORY);
    }
    SHR_BITSET(root->in_use_ha_blk_id_bmp.w, blk_id);

    sal_memset(ha_blk, 0, blk_size);
    blk_hdr = (bcmfp_ha_blk_hdr_t *)BCMFP_HA_PTR(unit, blk_id);
    blk_hdr->signature = BCMFP_HA_STRUCT_SIGN + blk_id;

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMFP:HA memory for stage id:%d is "
              "created with size %0d\n"), stage_id, blk_size));

    SHR_IF_ERR_EXIT
        (bcmfp_stage_oper_info_init_cb(unit,
                                       stage_id,
                                       blk_size,
                                       &issu_offset));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_ha_block_report(unit,
                                     stage_id,
                                     blk_id,
                                     &issu_offset));

    /* Back up HA block corresponding to stages active HA block. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
    BCMFP_HA_PTR(unit, backup_blk_id) =
            shr_ha_mem_alloc(unit,
                             BCMMGMT_FP_COMP_ID,
                             backup_blk_id,
                             "bcmfpStageBkupBlock",
                             &blk_size);
    backup_ha_blk = BCMFP_HA_PTR(unit, backup_blk_id);
    SHR_NULL_CHECK(backup_ha_blk, SHR_E_MEMORY);
    if (blk_size < req_blk_size) {
        SHR_IF_ERR_VERBOSE_EXIT
            (shr_ha_mem_free(unit, backup_ha_blk));
        blk_size = req_blk_size;
        BCMFP_HA_PTR(unit, backup_blk_id) =
                 shr_ha_mem_alloc(unit,
                                  BCMMGMT_FP_COMP_ID,
                                  backup_blk_id,
                                  "bcmfpStageBkupBlock",
                                  &blk_size);
        backup_ha_blk = BCMFP_HA_PTR(unit, backup_blk_id);
        SHR_NULL_CHECK(backup_ha_blk, SHR_E_MEMORY);
    }
    backup_blk_hdr = (bcmfp_ha_blk_hdr_t *)backup_ha_blk;

    SHR_BITSET(root->in_use_ha_blk_id_bmp.w, backup_blk_id);
    sal_memcpy(BCMFP_HA_PTR(unit, backup_blk_id),
               BCMFP_HA_PTR(unit, blk_id),
               blk_size);
    backup_blk_hdr =
        (bcmfp_ha_blk_hdr_t *)BCMFP_HA_PTR(unit, backup_blk_id);
    backup_blk_hdr->signature = BCMFP_HA_STRUCT_SIGN + backup_blk_id;
    backup_blk_hdr->backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    blk_hdr->backup_blk_id = backup_blk_id;

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMFP:Backup HA memory for stage id:%d "
               "is created with size %0d\n"), stage_id, blk_size));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_ha_block_report(unit,
                                     stage_id,
                                     backup_blk_id,
                                     &issu_offset));

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_ha_ing_init(int unit, bool warm)
{
    bcmfp_control_t *fc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_control_get(unit, &fc));

    if (fc->ifp != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_stage_init(unit, BCMFP_STAGE_ID_INGRESS, warm));
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_ha_vlan_init(int unit, bool warm)
{
    bcmfp_control_t *fc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_control_get(unit, &fc));

    if (fc->vfp != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_stage_init(unit, BCMFP_STAGE_ID_LOOKUP, warm));
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_ha_egr_init(int unit, bool warm)
{
    bcmfp_control_t *fc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_control_get(unit, &fc));

    if (fc->efp != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_stage_init(unit, BCMFP_STAGE_ID_EGRESS, warm));
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_ha_em_init(int unit, bool warm)
{
    bcmfp_control_t *fc = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_control_get(unit, &fc));

    if (fc->emfp != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_stage_init(unit, BCMFP_STAGE_ID_EXACT_MATCH, warm));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_get(unit, BCMFP_STAGE_ID_EXACT_MATCH, &stage));
        SHR_NULL_CHECK(stage, SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_ha_em_ft_init(int unit, bool warm)
{
    bcmfp_control_t *fc = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_control_get(unit, &fc));

    if (fc->emft != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_stage_init(unit, BCMFP_STAGE_ID_FLOW_TRACKER, warm));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_get(unit, BCMFP_STAGE_ID_FLOW_TRACKER, &stage));
        SHR_NULL_CHECK(stage, SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_mem_get(int unit,
                 uint8_t sub_id,
                 void **ha_mem)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ha_mem, SHR_E_PARAM);

    BCMFP_HA_BLK_ID_VALIDATE(unit, sub_id);
    *ha_mem = BCMFP_HA_PTR(unit, sub_id);
    if (*ha_mem == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_ha_blk_hdr_validate(int unit, uint8_t blk_id, void *ha_blk)
{
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ha_blk, SHR_E_PARAM);

    blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_blk;

    if (blk_hdr->signature !=
        (uint32_t)(BCMFP_HA_STRUCT_SIGN + blk_id)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Go throught all valid HA block IDs  saved in root HA block(block ID 0)
 * and get the new pointers of each HA block and save it in non HA memory.
 * This function also validates each HA valid block header and updates
 * sub segments offsets in HA blocks of type BCMFP_HA_BLK_TYPE_STAGE_OPER.
 */
STATIC int
bcmfp_warm_ha_init(int unit)
{
    uint8_t ha_blk_id = 0;
    uint32_t blk_size = 0;
    void *ha_blk = NULL;
    bcmfp_ha_root_blk_t *root = NULL;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_stage_id_t stage_id;

    SHR_FUNC_ENTER(unit);

    ha_blk_id = BCMFP_HA_BLK_ID_ROOT;
    BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_ROOT) =
            shr_ha_mem_alloc(unit,
                             BCMMGMT_FP_COMP_ID,
                             BCMFP_HA_BLK_ID_ROOT,
                             "bcmfpRootBlk",
                             &blk_size);
    ha_blk = BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_ROOT);
    SHR_NULL_CHECK(ha_blk, SHR_E_INTERNAL);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_blk_hdr_validate(unit, ha_blk_id, ha_blk));
    root = (bcmfp_ha_root_blk_t *)
           ((uint8_t *)ha_blk + sizeof(bcmfp_ha_blk_hdr_t));

    if (root->min_ha_blk_id != BCMFP_HA_BLK_ID_MIN ||
        root->max_ha_blk_id != BCMFP_HA_BLK_ID_MAX) {
        /* upgrade or downgrade case. */
    }

    for (ha_blk_id = root->min_ha_blk_id;
         ha_blk_id < root->max_ha_blk_id;
         ha_blk_id++) {
        if (SHR_BITGET(root->in_use_ha_blk_id_bmp.w, ha_blk_id)) {
            blk_size = 0;
            BCMFP_HA_PTR(unit, ha_blk_id) =
                shr_ha_mem_alloc(unit,
                                 BCMMGMT_FP_COMP_ID,
                                 ha_blk_id,
                                 "bcmfpHaBlk",
                                 &blk_size);
            ha_blk = BCMFP_HA_PTR(unit, ha_blk_id);
            SHR_NULL_CHECK(ha_blk, SHR_E_INTERNAL);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ha_blk_hdr_validate(unit, ha_blk_id, ha_blk));

            blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_blk;

            /*
             * Due to addition of new elements to any of the
             * existing data strcutures the HA size might have
             * been increaed. So update the HA block size in
             * BCMFP ha block header.
             */
            blk_hdr->blk_size = blk_size;

            /*
             * If the HA block is corresponding to stage operational
             * information.
             */
            if (blk_hdr->blk_type == BCMFP_HA_BLK_TYPE_STAGE_OPER) {
                BCMFP_HA_BLK_ID_TO_STAGE_ID(ha_blk_id, stage_id);
                blk_hdr->stage_id = stage_id;
                SHR_IF_ERR_VERBOSE_EXIT
                    ((bcmfp_stage_oper_info_init_wb(unit,
                                                    blk_hdr->stage_id,
                                                    ha_blk_id,
                                                    blk_hdr->blk_size)));
            }

            /*
             * If the blk ID is for compression keys then recreate the
             * trie.
             */
            if (blk_hdr->blk_type == BCMFP_HA_BLK_TYPE_DYNAMIC_CKEY) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_compress_trie_recreate(unit, ha_blk));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_mem_set(int unit,
                 uint8_t sub_id,
                 void *ha_mem)
{
    SHR_FUNC_ENTER(unit);
    BCMFP_HA_BLK_ID_VALIDATE(unit, sub_id);
    BCMFP_HA_PTR(unit, sub_id) = ha_mem;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_blk_hdr_get(int unit, uint8_t sub_id,
                     bcmfp_ha_blk_hdr_t **blk_hdr)
{
    void *ha_mem = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(blk_hdr, SHR_E_PARAM);

    BCMFP_HA_BLK_ID_VALIDATE(unit, sub_id);
    ha_mem = BCMFP_HA_PTR(unit, sub_id);
    if (ha_mem == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    *blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_mem;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_backup_blk_id_get(int unit,
                           uint8_t blk_id,
                           uint8_t *backup_blk_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(backup_blk_id, SHR_E_PARAM);

    BCMFP_HA_BLK_ID_VALIDATE(unit, blk_id);

    if (blk_id >= BCMFP_HA_BLK_ID_FIXED_MIN &&
        blk_id <= BCMFP_HA_BLK_ID_FIXED_MAX) {
        *backup_blk_id = (blk_id + 1);
    } else if (blk_id >= BCMFP_HA_BLK_ID_DYNAMIC_MIN &&
               blk_id <= BCMFP_HA_BLK_ID_DYNAMIC_MAX) {
        *backup_blk_id = (blk_id - 1);
    } else {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                 "BCMFP: HA back up block ID:%0d \n"), blk_id));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_fid_offset_info_get(int unit,
                          uint8_t blk_id,
                          uint32_t blk_offset,
                          bcmfp_fid_offset_info_t **fid_offset_info)
{
    void *ha_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fid_offset_info, SHR_E_PARAM);

    *fid_offset_info = NULL;

    BCMFP_HA_BLK_ID_VALIDATE(unit, blk_id);

    ha_ptr = BCMFP_HA_PTR(unit, blk_id);

    *fid_offset_info = (bcmfp_fid_offset_info_t *)
                       ((uint8_t *)ha_ptr + blk_offset);
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_fid_offset_infos_release(int unit, uint8_t blk_id, uint32_t blk_offset)
{
    void *ha_ptr = NULL;
    uint8_t curr_blk_id = 0;
    uint8_t next_blk_id = 0;
    uint32_t curr_blk_offset = 0;
    uint32_t next_blk_offset = 0;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_fid_offset_info_t *curr_fid_offset_info = NULL;

    SHR_FUNC_ENTER(unit);

    curr_blk_id = blk_id;
    curr_blk_offset = blk_offset;
    while ((curr_blk_id != BCMFP_HA_BLK_ID_INVALID) &&
         (curr_blk_offset != BCMFP_HA_BLK_OFFSET_INVALID)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_trans_state_set(unit,
                                   curr_blk_id,
                                   BCMFP_TRANS_STATE_UC_A));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_fid_offset_info_get(unit,
                                       curr_blk_id,
                                       curr_blk_offset,
                                       &curr_fid_offset_info));

        next_blk_id = curr_fid_offset_info->next_blk_id;
        next_blk_offset = curr_fid_offset_info->next_blk_offset;
        ha_ptr = BCMFP_HA_PTR(unit, curr_blk_id);
        if (ha_ptr == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(_func_unit, "%s\n"),
                       shr_errmsg(SHR_E_INTERNAL)));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_ptr;
        sal_memset(curr_fid_offset_info, 0, sizeof(bcmfp_fid_offset_info_t));
        curr_fid_offset_info->offset_info.blk_id = BCMFP_HA_BLK_ID_INVALID;
        curr_fid_offset_info->offset_info.blk_offset =
                                          BCMFP_HA_BLK_OFFSET_INVALID;
        curr_fid_offset_info->next_blk_id = curr_blk_id;
        curr_fid_offset_info->next_blk_offset = blk_hdr->free_blk_offset;
        blk_hdr->free_blk_offset = curr_blk_offset;
        blk_hdr->free_ha_element_count++;
        curr_blk_id = next_blk_id;
        curr_blk_offset = next_blk_offset;
        curr_fid_offset_info = NULL;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_fid_offset_infos_acquire(int unit,
                       uint16_t req_ha_elements,
                       bcmfp_fid_offset_info_t **fid_offset_infos,
                       uint8_t *first_ha_element_blk_id,
                       uint32_t *first_ha_element_blk_offset)
{
    void *ha_ptr = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint8_t first_free_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint16_t pooled_ha_elements = 0;
    uint32_t blk_size = 0;
    uint32_t blk_offset = 0;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_fid_offset_info_t *free_fid_offset_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fid_offset_infos, SHR_E_PARAM);
    SHR_NULL_CHECK(first_ha_element_blk_id, SHR_E_PARAM);
    SHR_NULL_CHECK(first_ha_element_blk_offset, SHR_E_PARAM);

    /*
     * Count the total number of free HA elements in all
     * the HA blocks(created in run time) together.
     */
    for (blk_id = BCMFP_HA_BLK_ID_DYNAMIC_MAX;
         blk_id >= BCMFP_HA_BLK_ID_DYNAMIC_MIN;
         blk_id--) {
         ha_ptr = BCMFP_HA_PTR(unit, blk_id);
         if (ha_ptr == NULL) {
            if (first_free_blk_id == BCMFP_HA_BLK_ID_INVALID) {
                first_free_blk_id = blk_id;
            }
            continue;
         }
         blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_ptr;

         if (blk_hdr->backup_blk_id == BCMFP_HA_BLK_ID_INVALID) {
             continue;
         }

         /*! No free HA elements in this HA block. */
         if (blk_hdr->free_blk_offset == BCMFP_HA_BLK_OFFSET_INVALID) {
             continue;
         }

         /*!
          * This dynamic block is not allocated for basic
          * HA elements.
          */
         if (blk_hdr->blk_type != BCMFP_HA_BLK_TYPE_FID_OFFSET_INFO) {
             continue;
         }

         pooled_ha_elements += blk_hdr->free_ha_element_count;
         if (pooled_ha_elements >= req_ha_elements) {
             break;
         }
    }

    /* There is no enough room and no block ids left over to use. */
    if ((pooled_ha_elements < req_ha_elements) &&
        (first_free_blk_id == BCMFP_HA_BLK_ID_INVALID)) {
        
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /*
     * There is no enough room and free block ids are there then
     * create a new HA block.
     */
    if ((pooled_ha_elements < req_ha_elements) &&
        (first_free_blk_id != BCMFP_HA_BLK_ID_INVALID)) {
        blk_size = (BCMFP_HA_FID_OFFSET_INFO_BLK_ELEMENT_COUNT *
                    sizeof(bcmfp_fid_offset_info_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_fid_offset_info_blk_create(unit,
                                              first_free_blk_id,
                                              blk_size));
    }

    pooled_ha_elements = 0;
    /* Acquire the required number of HA elements. */
    for (blk_id = BCMFP_HA_BLK_ID_DYNAMIC_MAX;
         blk_id >= BCMFP_HA_BLK_ID_DYNAMIC_MIN;
         blk_id--) {
         ha_ptr = BCMFP_HA_PTR(unit, blk_id);
         /* Skip un allocated HA blocks */
         if (ha_ptr == NULL) {
             continue;
         }
         blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_ptr;

         if (blk_hdr->backup_blk_id == BCMFP_HA_BLK_ID_INVALID) {
             continue;
         }
         /* Skip if HA block doesnot have free HA elements. */
         if (blk_hdr->free_blk_offset == BCMFP_HA_BLK_ID_INVALID) {
             continue;
         }

         /*!
          * This dynamic block is not allocated for basic
          * HA elements.
          */
         if (blk_hdr->blk_type != BCMFP_HA_BLK_TYPE_FID_OFFSET_INFO) {
             continue;
         }

         blk_offset = blk_hdr->free_blk_offset;
         if (free_fid_offset_info != NULL) {
             free_fid_offset_info->next_blk_id = blk_id;
             free_fid_offset_info->next_blk_offset = blk_offset;
         }
         /* Acquired free HA elements from the current block id. */
         while (blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_fid_offset_info_get(unit,
                                            blk_id,
                                            blk_offset,
                                            &free_fid_offset_info));
             /* If this is the first free HA element record its
              *  block id and block offset.
              */
             if (*fid_offset_infos == NULL) {
                 *fid_offset_infos = free_fid_offset_info;
                 *first_ha_element_blk_id = blk_id;
                 *first_ha_element_blk_offset = blk_offset;
             }
             pooled_ha_elements++;
             /* Change the block transaction state to Uncommitted Active as
              * some free HA elements in this block are going to be used.
              */
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_trans_state_set(unit, blk_id, BCMFP_TRANS_STATE_UC_A));

             /* Remove the HA element from free HA elements linked list
              * of the HA block and update the number of free HA elements.
              */
             blk_hdr->free_blk_offset = free_fid_offset_info->next_blk_offset;
             blk_hdr->free_ha_element_count--;
             /* Stop if required number of HA elements are acquired. */
             if (pooled_ha_elements == req_ha_elements) {
                 break;
             }
             /* Move to the next free HA element in the HA block. */
             blk_offset = free_fid_offset_info->next_blk_offset;
         }
         /* Stop if required number of HA elements are acquired. */
         if (pooled_ha_elements == req_ha_elements) {
             break;
         }
    }

    /* End of the acquired HA elements linked list. */
    if (free_fid_offset_info != NULL)  {
        free_fid_offset_info->next_blk_id = BCMFP_HA_BLK_ID_INVALID;
        free_fid_offset_info->next_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_element_get(int unit,
                     uint8_t blk_id,
                     uint32_t blk_offset,
                     bcmfp_generic_data_t **ha_element)
{
    void *ha_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ha_element, SHR_E_PARAM);

    *ha_element = NULL;

    BCMFP_HA_BLK_ID_VALIDATE(unit, blk_id);

    BCMFP_HA_BLK_OFFSET_VALIDATE(unit, blk_id, blk_offset);

    ha_ptr = BCMFP_HA_PTR(unit, blk_id);

    *ha_element = (bcmfp_generic_data_t *)((uint8_t *)ha_ptr + blk_offset);
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_elements_release(int unit, uint8_t blk_id, uint32_t blk_offset)
{
    void *ha_ptr = NULL;
    uint8_t curr_blk_id = 0;
    uint8_t next_blk_id = 0;
    uint32_t curr_blk_offset = 0;
    uint32_t next_blk_offset = 0;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_generic_data_t *curr_ha_element = NULL;

    SHR_FUNC_ENTER(unit);

    curr_blk_id = blk_id;
    curr_blk_offset = blk_offset;
    while ((curr_blk_id != BCMFP_HA_BLK_ID_INVALID) &&
         (curr_blk_offset != BCMFP_HA_BLK_OFFSET_INVALID)) {
        SHR_IF_ERR_EXIT
            (bcmfp_trans_state_set(unit, curr_blk_id,
                            BCMFP_TRANS_STATE_UC_A));
        SHR_IF_ERR_EXIT(
            bcmfp_ha_element_get(unit, curr_blk_id,
                 curr_blk_offset, &curr_ha_element));
        next_blk_id = curr_ha_element->next_blk_id;
        next_blk_offset = curr_ha_element->next_blk_offset;
        ha_ptr = BCMFP_HA_PTR(unit, curr_blk_id);
        if (ha_ptr == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(_func_unit, "%s\n"),
                       shr_errmsg(SHR_E_INTERNAL)));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_ptr;
        curr_ha_element->value = 0;
        curr_ha_element->next_blk_id = curr_blk_id;
        curr_ha_element->next_blk_offset = blk_hdr->free_blk_offset;
        blk_hdr->free_blk_offset = curr_blk_offset;
        blk_hdr->free_ha_element_count++;
        if (blk_hdr->bucket_size == BCMFP_HA_BLK_BUCKET_SIZE_TWO) {
            curr_ha_element++;
            curr_ha_element->value = 0;
            curr_ha_element->next_blk_id = BCMFP_HA_BLK_ID_INVALID;
            curr_ha_element->next_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
        }
        curr_blk_id = next_blk_id;
        curr_blk_offset = next_blk_offset;
        curr_ha_element = NULL;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_elements_acquire(int unit,
                    uint16_t req_ha_elements,
                    uint32_t bucket_size,
                    bcmfp_generic_data_t **ha_elements,
                    uint8_t *first_ha_element_blk_id,
                    uint32_t *first_ha_element_blk_offset)
{
    void *ha_ptr = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint8_t first_free_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint16_t pooled_ha_elements = 0;
    uint32_t blk_size = 0;
    uint32_t blk_offset = 0;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_generic_data_t *free_ha_element = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ha_elements, SHR_E_PARAM);
    SHR_NULL_CHECK(first_ha_element_blk_id, SHR_E_PARAM);
    SHR_NULL_CHECK(first_ha_element_blk_offset, SHR_E_PARAM);

    /* Count the total number of free HA elements in all
     * the HA blocks(created in run time) together.
     */
    for (blk_id = BCMFP_HA_BLK_ID_DYNAMIC_MAX;
         blk_id >= BCMFP_HA_BLK_ID_DYNAMIC_MIN;
         blk_id--) {
         ha_ptr = BCMFP_HA_PTR(unit, blk_id);
         if (ha_ptr == NULL) {
            if (first_free_blk_id == BCMFP_HA_BLK_ID_INVALID) {
                first_free_blk_id = blk_id;
            }
            continue;
         }
         blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_ptr;
         /*
          * Requested bucket size and HA block bucket size
          * must be same.
          */
         if (blk_hdr->bucket_size != bucket_size) {
             continue;
         }

         if (blk_hdr->backup_blk_id == BCMFP_HA_BLK_ID_INVALID) {
             continue;
         }

         /*! No free HA elements in this HA block. */
         if (blk_hdr->free_blk_offset == BCMFP_HA_BLK_OFFSET_INVALID) {
             continue;
         }

         /*! This dynamic block is not allocated for basic HA elements. */
         if (blk_hdr->blk_type != BCMFP_HA_BLK_TYPE_GENERIC_TV) {
             continue;
         }

         pooled_ha_elements += blk_hdr->free_ha_element_count;
         if (pooled_ha_elements >= req_ha_elements) {
             break;
         }
    }

    /* There is no enough room and no block ids left over to use. */
    if ((pooled_ha_elements < req_ha_elements) &&
        (first_free_blk_id == BCMFP_HA_BLK_ID_INVALID)) {
        
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* There is no enough room and free block ids are there then
     * create a new HA block.
     */
    if ((pooled_ha_elements < req_ha_elements) &&
        (first_free_blk_id != BCMFP_HA_BLK_ID_INVALID)) {
        blk_size = (BCMFP_HA_GENERIC_TV_BLK_ELEMENT_COUNT *
                    sizeof(bcmfp_generic_data_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_blk_create(unit,
                                 first_free_blk_id,
                                 blk_size,
                                 bucket_size));
    }

    pooled_ha_elements = 0;
    /* Acquire the required number of HA elements. */
    for (blk_id = BCMFP_HA_BLK_ID_DYNAMIC_MAX;
         blk_id >= BCMFP_HA_BLK_ID_DYNAMIC_MIN;
         blk_id--) {
         ha_ptr = BCMFP_HA_PTR(unit, blk_id);
         /* Skip un allocated HA blocks */
         if (ha_ptr == NULL) {
             continue;
         }
         blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_ptr;
         /* Skip if HA block doesnot have free HA elements. */
         if (blk_hdr->free_blk_offset == BCMFP_HA_BLK_ID_INVALID) {
             continue;
         }
         /*
          * Requested bucket size and HA block bucket size
          * must be same.
          */
         if (blk_hdr->bucket_size != bucket_size) {
             continue;
         }

         if (blk_hdr->backup_blk_id == BCMFP_HA_BLK_ID_INVALID) {
             continue;
         }

         /*! This dynamic block is not allocated for basic HA elements. */
         if (blk_hdr->blk_type != BCMFP_HA_BLK_TYPE_GENERIC_TV) {
             continue;
         }

         blk_offset = blk_hdr->free_blk_offset;
         if (free_ha_element != NULL) {
             free_ha_element->next_blk_id = blk_id;
             free_ha_element->next_blk_offset = blk_offset;
         }
         /* Acquired free HA elements from the current block id. */
         while (blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
             SHR_IF_ERR_EXIT(
                 bcmfp_ha_element_get(unit, blk_id,
                          blk_offset, &free_ha_element));
             /* If this is the first free HA element record its
              *  block id and block offset.
              */
             if (*ha_elements == NULL) {
                 *ha_elements = free_ha_element;
                 *first_ha_element_blk_id = blk_id;
                 *first_ha_element_blk_offset = blk_offset;
             }
             pooled_ha_elements++;
             /* Change the block transaction state to Uncommitted Active as
              * some free HA elements in this block are going to be used.
              */
             SHR_IF_ERR_EXIT
                 (bcmfp_trans_state_set(unit, blk_id, BCMFP_TRANS_STATE_UC_A));

             /* Remove the HA element from free HA elements linked list
              * of the HA block and update the number of free HA elements.
              */
             blk_hdr->free_blk_offset = free_ha_element->next_blk_offset;
             blk_hdr->free_ha_element_count--;
             /* Stop if required number of HA elements are acquired. */
             if (pooled_ha_elements == req_ha_elements) {
                 break;
             }
             /* Move to the next free HA element in the HA block. */
             blk_offset = free_ha_element->next_blk_offset;
         }
         /* Stop if required number of HA elements are acquired. */
         if (pooled_ha_elements == req_ha_elements) {
             break;
         }
    }
    /* End of the acquired HA elements linked list. */
    if (free_ha_element != NULL)  {
        free_ha_element->next_blk_id = BCMFP_HA_BLK_ID_INVALID;
        free_ha_element->next_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    }

exit:
    /* Release the acquired HA elements if there is an error. */
    if (SHR_FUNC_ERR() && *ha_elements != NULL) {
        SHR_IF_ERR_CONT
            (bcmfp_ha_elements_release(unit,
                       *first_ha_element_blk_id,
                       *first_ha_element_blk_offset));
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_cleanup(int unit)
{
    uint8_t blk_id = 0;
    uint8_t backup_blk_id = 0;

    SHR_FUNC_ENTER(unit);

    if (BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_INGRESS) != NULL) {
        if (shr_ha_mem_free(unit,
                    BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_INGRESS)) == SHR_E_NONE) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP:HA memory for "
                                "INGRESS stage is freed\n")));
            BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_INGRESS) = NULL;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP: Could not free HA memory for "
                                "INGRESS stage.\n")));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        blk_id = BCMFP_HA_BLK_ID_INGRESS;
        backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
        if (shr_ha_mem_free(unit,
                    BCMFP_HA_PTR(unit, backup_blk_id)) == SHR_E_NONE) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP:HA back up memory for "
                                "INGRESS stage is freed\n")));
            BCMFP_HA_PTR(unit, backup_blk_id) = NULL;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP: Could not free HA backup memory "
                                "for INGRESS stage.\n")));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    if (BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_EGRESS) != NULL) {
        if (shr_ha_mem_free(unit,
                    BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_EGRESS)) == SHR_E_NONE) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP:HA memory for "
                                "EGRESS stage is freed\n")));
            BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_EGRESS) = NULL;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP: Could not free HA memory for "
                                "EGRESS stage.\n")));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        blk_id = BCMFP_HA_BLK_ID_EGRESS;
        backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
        if (shr_ha_mem_free(unit,
                    BCMFP_HA_PTR(unit, backup_blk_id)) == SHR_E_NONE) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP:HA back up memory for "
                                "EGRESS stage is freed\n")));
            BCMFP_HA_PTR(unit, backup_blk_id) = NULL;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP: Could not free HA backup memory "
                                "for EGRESS stage.\n")));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
    if (BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_VLAN) != NULL) {
        if (shr_ha_mem_free(unit,
                    BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_VLAN)) == SHR_E_NONE) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP:HA memory for "
                                "VLAN stage is freed\n")));
            BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_VLAN) = NULL;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP: Could not free HA memory for "
                                "VLAN stage.\n")));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        blk_id = BCMFP_HA_BLK_ID_VLAN;
        backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
        if (shr_ha_mem_free(unit,
            BCMFP_HA_PTR(unit, backup_blk_id)) == SHR_E_NONE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "BCMFP:HA back up memory for "
                       "VLAN stage is freed\n")));
            BCMFP_HA_PTR(unit, backup_blk_id) = NULL;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP: Could not free HA backup memory "
                                "for VLAN stage.\n")));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
    if (BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_EM) != NULL) {
        if (shr_ha_mem_free(unit,
                    BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_EM)) == SHR_E_NONE) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP:HA memory for "
                                "EM stage is freed\n")));
            BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_EM) = NULL;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP: Could not free HA memory for "
                                "EM stage.\n")));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        blk_id = BCMFP_HA_BLK_ID_EM;
        backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
        if (shr_ha_mem_free(unit,
            BCMFP_HA_PTR(unit, backup_blk_id)) == SHR_E_NONE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "BCMFP:HA back up memory for "
                       "EM stage is freed\n")));
            BCMFP_HA_PTR(unit, backup_blk_id) = NULL;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP: Could not free HA backup memory "
                                "for EM stage.\n")));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    for (blk_id = BCMFP_HA_BLK_ID_DYNAMIC_MAX;
         blk_id >= BCMFP_HA_BLK_ID_DYNAMIC_MIN;
         blk_id--) {
        if (BCMFP_HA_PTR(unit, blk_id) == NULL) {
            continue;
        }
        if (shr_ha_mem_free(unit, BCMFP_HA_PTR(unit, blk_id)) == SHR_E_NONE) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP:HA Dynamic  memory for "
                                "block ID: %0d is freed\n"), blk_id));
            BCMFP_HA_PTR(unit, blk_id) = NULL;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP: Could not free HA  dynamic memory "
                                "for block ID: %0d\n"), blk_id));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
        if (shr_ha_mem_free(unit,
                    BCMFP_HA_PTR(unit, backup_blk_id)) == SHR_E_NONE) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP:HA Dynamic  memory for "
                                " backup block ID:%0d is freed\n"), blk_id));
            BCMFP_HA_PTR(unit, backup_blk_id) = NULL;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP: Could not free HA  dynamic memory "
                                "for  backup block ID:%0d\n"), blk_id));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_ha_root_init(int unit, bool warm)
{
    void *ha_blk = NULL;
    uint32_t ha_blk_size = 0;
    uint32_t offset = 0;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_ha_root_blk_t *root = NULL;

    SHR_FUNC_ENTER(unit);

    ha_blk_size = sizeof(bcmfp_ha_blk_hdr_t) +
                  sizeof(bcmfp_ha_root_blk_t);
    BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_ROOT) =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_FP_COMP_ID,
                         BCMFP_HA_BLK_ID_ROOT,
                         "bcmfpRootBlk",
                         &ha_blk_size);
    ha_blk = BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_ROOT);
    SHR_NULL_CHECK(ha_blk, SHR_E_MEMORY);

    bcmissu_struct_info_report(unit,
                                     BCMMGMT_FP_COMP_ID,
                                     BCMFP_HA_BLK_ID_ROOT,
                                     0,
                                     sizeof(bcmfp_ha_blk_hdr_t),
                                     1,
                                     BCMFP_HA_BLK_HDR_T_ID);
    offset += sizeof(bcmfp_ha_blk_hdr_t);
    bcmissu_struct_info_report(unit,
                                     BCMMGMT_FP_COMP_ID,
                                     BCMFP_HA_BLK_ID_ROOT,
                                     offset,
                                     sizeof(bcmfp_ha_root_blk_t),
                                     1,
                                     BCMFP_HA_ROOT_BLK_T_ID);

    blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_blk;
    blk_hdr->blk_size = ha_blk_size;
    blk_hdr->backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    blk_hdr->free_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    blk_hdr->free_ha_element_count = 0;
    blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
    blk_hdr->signature = (BCMFP_HA_STRUCT_SIGN + BCMFP_HA_BLK_ID_ROOT);
    root = (bcmfp_ha_root_blk_t *)
        ((uint8_t *)ha_blk + sizeof(bcmfp_ha_blk_hdr_t));
    root->min_ha_blk_id = BCMFP_HA_BLK_ID_MIN;
    root->max_ha_blk_id = BCMFP_HA_BLK_ID_MAX;
    sal_memset(root->in_use_ha_blk_id_bmp.w,
            0, sizeof(bcmfp_ha_blk_id_bmp_t));
    SHR_BITSET(root->in_use_ha_blk_id_bmp.w,
               BCMFP_HA_BLK_ID_ROOT);
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    sal_memset(ha_ptrs[unit], 0, BCMFP_HA_BLK_ID_MAX * sizeof(void *));
    /* WARM BOOT */
    if (warm == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmfp_warm_ha_init(unit));
        SHR_EXIT();
    }

    /* COLD BOOT:Root HA block creation and initialization. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_root_init(unit, warm));
    /* COLD BOOT:ING HA block creation and initialization. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_ing_init(unit, warm));
    /* COLD BOOT:EGR HA block creation and initialization. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_egr_init(unit, warm));
    /* COLD BOOT:VLAN HA block creation and initialization. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_vlan_init(unit, warm));
    /* COLD BOOT:EM HA block creation and initialization. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_em_init(unit, warm));
    /* COLD BOOT:EM_FT HA block creation and initialization. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_em_ft_init(unit, warm));

exit:
    if (SHR_FUNC_ERR()) {
        bcmfp_ha_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_free_blk_id_get(int unit, uint8_t *blk_id)
{
    uint16_t id = 0;
    void *ha_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(blk_id, SHR_E_PARAM);

    for (id = BCMFP_HA_BLK_ID_DYNAMIC_MAX;
         id >= BCMFP_HA_BLK_ID_DYNAMIC_MIN;
         id--) {
         ha_ptr = BCMFP_HA_PTR(unit, id);
         if (ha_ptr != NULL) {
            continue;
         }
         *blk_id = id;
         break;
    }
    if (id == BCMFP_HA_BLK_ID_DYNAMIC_MIN) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
exit:
    SHR_FUNC_EXIT();
}
