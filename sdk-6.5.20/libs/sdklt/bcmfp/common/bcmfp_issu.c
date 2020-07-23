/*! \file bcmfp_issu.c
 *
 * BCMFP HA block update functions in case of ISSU.
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
#include <bcmfp/bcmfp_issu_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_HA

int
bcmfp_issu_offsets_rectify(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint16_t blk_id,
                           bcmfp_ha_struct_offset_t *issu_offset)
{
    uint32_t ha_blk_hdr_offset= 0;
    uint32_t stage_oper_info_offset= 0;
    uint32_t ref_count_offset= 0;
    uint32_t group_oper_info_offset= 0;
    uint32_t pdd_oper_info_offset= 0;
    uint32_t ha_blk_info_offset= 0;
    int diff = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(issu_offset, SHR_E_PARAM);

    /*
     * Get the offsets of segements reported during cold boot.
     * Expected that the offsets are not changed because of
     * the conditions(only when array sizes of the reported segments
     * are not changed) under which this function is called.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_get(unit,
                                 BCMFP_HA_BLK_HDR_T_ID,
                                 BCMMGMT_FP_COMP_ID,
                                 blk_id,
                                 &ha_blk_hdr_offset));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_get(unit,
                                 BCMFP_STAGE_OPER_INFO_T_ID,
                                 BCMMGMT_FP_COMP_ID,
                                 blk_id,
                                 &stage_oper_info_offset));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_get(unit,
                                 BCMFP_GROUP_OPER_INFO_T_ID,
                                 BCMMGMT_FP_COMP_ID,
                                 blk_id,
                                 &group_oper_info_offset));

    if (issu_offset->num_pdd_profiles != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_get(unit,
                                     BCMFP_PDD_OPER_INFO_T_ID,
                                     BCMMGMT_FP_COMP_ID,
                                     blk_id,
                                     &pdd_oper_info_offset));
    }
    if (issu_offset->ha_blk_info_array_size != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_get(unit,
                                     BCMFP_HA_BLK_INFO_T_ID,
                                     BCMMGMT_FP_COMP_ID,
                                     blk_id,
                                     &ha_blk_info_offset));
    }

    if (issu_offset->ref_count_array_size != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_get(unit,
                                     BCMFP_REF_COUNT_T_ID,
                                     BCMMGMT_FP_COMP_ID,
                                     blk_id,
                                     &ref_count_offset));
    }

    issu_offset->ha_blk_hdr_offset = ha_blk_hdr_offset;
    issu_offset->stage_oper_info_offset = stage_oper_info_offset;
    issu_offset->pdd_oper_info_offset = pdd_oper_info_offset;

    /*
     * ref count segment is reported as an array in cold boot.
     * At the same time we maintain offsets to chunks of these
     * array elements in stage oper info segement. So if some
     * element is removed from the bcmfp_ref_count_t strcuture,
     * ref count segment offset or segements that follow will
     * not change, but the offsets of chunks of array elements
     * saved in the stage oper info structure. These sub segment
     * offsets caliculated in issu_offset structure are based
     * on the new structures. So the diff needs to be counted.
     */
    diff = (ref_count_offset -
            issu_offset->ref_count_offset);
    if (diff != 0) {
        if (issu_offset->keygen_prof_ref_count_offset) {
            issu_offset->keygen_prof_ref_count_offset += diff;
        }
        if (issu_offset->action_prof_ref_count_offset) {
            issu_offset->action_prof_ref_count_offset += diff;
        }
        if (issu_offset->qos_prof_ref_count_offset) {
            issu_offset->qos_prof_ref_count_offset += diff;
        }
        if (issu_offset->em_key_attrib_prof_ref_count_offset) {
            issu_offset->em_key_attrib_prof_ref_count_offset += diff;
        }
        if (issu_offset->sbr_prof_ref_count_offset) {
            issu_offset->sbr_prof_ref_count_offset += diff;
        }
        if (issu_offset->psg_prof_ref_count_offset) {
            issu_offset->psg_prof_ref_count_offset += diff;
        }
    }

    diff = (ha_blk_info_offset -
            issu_offset->ha_blk_info_offset);
    if (diff != 0) {
        if (issu_offset->group_entry_map_offset) {
            issu_offset->group_entry_map_offset += diff;
        }
        if (issu_offset->rule_entry_map_offset) {
            issu_offset->rule_entry_map_offset += diff;
        }
        if (issu_offset->policy_entry_map_offset) {
            issu_offset->policy_entry_map_offset += diff;
        }
        if (issu_offset->meter_entry_map_offset) {
            issu_offset->meter_entry_map_offset += diff;
        }
        if (issu_offset->ctr_entry_map_offset) {
            issu_offset->ctr_entry_map_offset += diff;
        }
        if (issu_offset->pdd_entry_map_offset) {
            issu_offset->pdd_entry_map_offset += diff;
        }
        if (issu_offset->sbr_entry_map_offset) {
            issu_offset->sbr_entry_map_offset += diff;
        }
        if (issu_offset->pse_group_map_offset) {
            issu_offset->pse_group_map_offset += diff;
        }
        if (issu_offset->psg_group_map_offset) {
            issu_offset->psg_group_map_offset += diff;
        }
        if (issu_offset->pdd_group_map_offset) {
            issu_offset->pdd_group_map_offset += diff;
        }
        if (issu_offset->sbr_group_map_offset) {
            issu_offset->sbr_group_map_offset += diff;
        }
        if (issu_offset->dpdd_group_map_offset) {
            issu_offset->dpdd_group_map_offset += diff;
        }
        if (issu_offset->dpolicy_group_map_offset) {
            issu_offset->dpolicy_group_map_offset += diff;
        }
        if (issu_offset->dsbr_group_map_offset) {
            issu_offset->dsbr_group_map_offset += diff;
        }
    }

    diff = (group_oper_info_offset -
            issu_offset->group_oper_info_offset);
    if (diff != 0) {
        if (issu_offset->group_oper_info_offset) {
            issu_offset->group_oper_info_offset += diff;
        }
        if (issu_offset->presel_group_oper_info_offset) {
            issu_offset->presel_group_oper_info_offset += diff;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_oper_info_update(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint16_t blk_id,
                             uint32_t new_blk_size,
                             bcmfp_ha_struct_offset_t *issu_offset)
{
    int rv;
    void *ha_mem = NULL;
    void *issu_ha_mem = NULL;
    uint8_t *src_ptr = NULL;
    uint8_t *dst_ptr = NULL;
    size_t size = 0;
    uint32_t pipe_count = 1;
    void *new_ha_mem = NULL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    uint32_t stage_oper_info_offset = 0;
    uint32_t pipe_map = 0;
    const bcmlrd_map_t *map = NULL;
    bcmfp_stage_oper_mode_t oper_mode;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(issu_offset, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_get(unit,
                                 BCMFP_STAGE_OPER_INFO_T_ID,
                                 BCMMGMT_FP_COMP_ID,
                                 blk_id,
                                 &stage_oper_info_offset));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                      ((uint8_t *)ha_mem + stage_oper_info_offset);

    SHR_ALLOC(issu_ha_mem, new_blk_size, "bcmfpTempStageOperInfo");
    SHR_NULL_CHECK(issu_ha_mem, SHR_E_MEMORY);
    sal_memset(issu_ha_mem, 0, new_blk_size);
    blk_hdr = (bcmfp_ha_blk_hdr_t *)issu_ha_mem;

    src_ptr = ha_mem;
    dst_ptr = issu_ha_mem;
    /* Copy BCMFP HA block header. */
    size = sizeof(bcmfp_ha_blk_hdr_t);
    sal_memcpy(dst_ptr, src_ptr, size);
    blk_hdr->blk_size = new_blk_size;

    /* Copy stage operational information. */
    src_ptr = src_ptr + size;
    dst_ptr = dst_ptr + size;
    size = sizeof(bcmfp_stage_oper_info_t);
    sal_memcpy(dst_ptr, src_ptr, size);

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

    /* Copy bcmfp_ref_count_t structure arrays. */
    src_ptr = src_ptr + size;
    dst_ptr = dst_ptr + size;
    size = (stage_oper_info->num_keygen_prof * pipe_count) +
           (stage_oper_info->num_action_prof * pipe_count) +
           (stage_oper_info->num_qos_prof * pipe_count) +
           (stage_oper_info->num_em_key_attrib_profile * pipe_count) +
           (stage_oper_info->num_sbr_profiles * pipe_count) +
           (stage_oper_info->num_psg_profiles * pipe_count);
    size = size * sizeof(bcmfp_ref_count_t);
    if (size != 0) {
        sal_memcpy(dst_ptr, src_ptr, size);
    }

    /*
     * Copy bcmfp_ha_blk_info_t till dpolicy to group ID
     * mappings.
     */
    src_ptr = src_ptr + size;
    dst_ptr = dst_ptr + size;
    size = stage_oper_info->group_entry_map_size +
           stage_oper_info->rule_entry_map_size +
           stage_oper_info->policy_entry_map_size +
           stage_oper_info->meter_entry_map_size +
           stage_oper_info->ctr_entry_map_size +
           stage_oper_info->pdd_entry_map_size +
           stage_oper_info->sbr_entry_map_size +
           stage_oper_info->pse_group_map_size +
           stage_oper_info->psg_group_map_size +
           stage_oper_info->sbr_group_map_size +
           stage_oper_info->pdd_group_map_size +
           stage_oper_info->dpdd_group_map_size;
    size = size * sizeof(bcmfp_ha_blk_info_t);
    if (size != 0) {
        sal_memcpy(dst_ptr, src_ptr, size);
    }

    /*
     * Leave the gap for dpolicy to group ID mapping
     * and copy dsbr to group ID mapping.
     */
    src_ptr = src_ptr + size;
    dst_ptr = dst_ptr + size;
    size = issu_offset->dpolicy_group_map_size;
    size = size * sizeof(bcmfp_ha_blk_info_t);
    dst_ptr = dst_ptr + size;
    size = stage_oper_info->dsbr_group_map_size;
    size = size * sizeof(bcmfp_ha_blk_info_t);
    if (size != 0) {
        sal_memcpy(dst_ptr, src_ptr, size);
    }

    /* Copy group operational information. */
    src_ptr = src_ptr + size;
    dst_ptr = dst_ptr + size;
    size = stage_oper_info->num_groups;
    size = size * sizeof(bcmfp_group_oper_info_t);
    sal_memcpy(dst_ptr, src_ptr, size);

    /* Copy presel group operational information. */
    src_ptr = src_ptr + size;
    dst_ptr = dst_ptr + size;
    size = issu_offset->num_presel_groups;
    size = size * sizeof(bcmfp_group_oper_info_t);
    if (size != 0) {
        sal_memcpy(dst_ptr, src_ptr, size);
    }

    /* Copy presel group operational information. */
    src_ptr = src_ptr + size;
    dst_ptr = dst_ptr + size;
    size = issu_offset->num_pdd_profiles;
    size = size * sizeof(bcmfp_pdd_oper_info_t);
    if (size != 0) {
        sal_memcpy(dst_ptr, src_ptr, size);
    }

    /*
     * Resize the stages active and backup blk IDs and copy the
     * content from newly created BCMFP_HA_BLK_ID_ISSU_UPGRADE
     * to stages active and backup.
     */
    new_ha_mem = shr_ha_mem_realloc(unit,
                                    ha_mem,
                                    new_blk_size);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_set(unit, blk_id, new_ha_mem));
    sal_memcpy(new_ha_mem, issu_ha_mem, new_blk_size);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_ha_block_report(unit,
                                     stage_id,
                                     blk_id,
                                     issu_offset));
exit:
    SHR_FREE(issu_ha_mem);
    SHR_FUNC_EXIT();
}
