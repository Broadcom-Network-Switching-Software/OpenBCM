/*! \file rm_tcam_prio_only_oper_get.c
 *
 *  Get functions to fetch pointer to bcmptm_rm_tcam_prio_only_info_t
 *  and structures within it for any given prio only TCAM LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

/*******************************************************************************
 * Public Functions
 */

/*
 * Get the pointer to tcam information of given TCAM LTID, pipe id.
 */
int
bcmptm_rm_tcam_prio_only_tcam_info_get(int unit,
                        bcmltd_sid_t ltid,
                        bcmptm_rm_tcam_lt_info_t *ltid_info,
                        int pipe_id,
                        bcmptm_rm_tcam_prio_only_info_t **tcam_info)
{
    void *tcam_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_info_get(unit, ltid, pipe_id, ltid_info, &tcam_ptr));

    *tcam_info = (bcmptm_rm_tcam_prio_only_info_t *)
                                 ((uint8_t *)tcam_ptr +
                                  sizeof(bcmptm_rm_tcam_trans_info_t));

exit:
    SHR_FUNC_EXIT();
}

/* Function to get the LTID physical TCAM shared info. */
int
bcmptm_rm_tcam_prio_only_shared_info_get(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              int pipe_id,
                              bcmptm_rm_tcam_prio_only_shr_info_t **shared_info,
                              uint16_t *shared_info_count)
{
    uint32_t offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(shared_info, SHR_E_PARAM);
    SHR_NULL_CHECK(shared_info_count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                ltid,
                                                ltid_info,
                                                pipe_id,
                                                &tcam_info));

    if (tcam_info->shr_info_count == 0) {
        *shared_info_count = 0;
        *shared_info = NULL;
        SHR_FUNC_EXIT();
    }
    offset = tcam_info->shr_info_offset;
    *shared_info = (bcmptm_rm_tcam_prio_only_shr_info_t *)
                   ((uint8_t *)tcam_info + offset);
    *shared_info_count = tcam_info->shr_info_count;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Fetch the address to TCAM entry information of the given LTID.
 * This address points to address of zeroth TCAM index entry information.
 * To get the address to TCAM entry information of any specific TCAM index,
 * increment this pointer by those many TCAM indexes as memory created to
 * hold TCAM entry information of all TCAM indexes is consecutive.
 */
int
bcmptm_rm_tcam_prio_only_entry_info_get(int unit,
                         bcmltd_sid_t ltid,
                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                         int pipe_id,
                         bcmptm_rm_tcam_prio_only_entry_info_t **entry_info)
{
    uint32_t offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                ltid,
                                                ltid_info,
                                                pipe_id,
                                                &tcam_info));

    offset = tcam_info->entry_info_offset;
    *entry_info = (bcmptm_rm_tcam_prio_only_entry_info_t *)
                  ((uint8_t*)tcam_info + offset);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Fetch the address to TCAM entry hash of the given Logical Table id.
 * This address points to the address of hash zero. To get the address
 * to any other valid hash, increment this start pointer by hash times
 * as memory created to hold the hash information of all valid hash
 * values is consecutive.
 */
int
bcmptm_rm_tcam_prio_only_entry_hash_get(int unit,
                                        bcmltd_sid_t ltid,
                                        bcmptm_rm_tcam_lt_info_t *ltid_info,
                                        int pipe_id,
                                        uint8_t lookup_id,
                                        int **entry_hash)
{
    uint32_t offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_hash, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                ltid,
                                                ltid_info,
                                                pipe_id,
                                                &tcam_info));
    offset = tcam_info->entry_hash_offset;

    if (lookup_id == 2) {
        offset += (sizeof(int) * (tcam_info->entry_hash_size / 2));
    }
    *entry_hash = (int *)((uint8_t*)tcam_info + offset);
exit:
    SHR_FUNC_EXIT();
}
