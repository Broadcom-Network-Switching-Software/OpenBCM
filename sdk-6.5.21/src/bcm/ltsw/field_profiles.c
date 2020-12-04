/*! \file field_profiles.c
 *
 * Field Module Profiles Implementation above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>

#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/mbcm/field.h>

#include <shr/shr_debug.h>
/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FP

/*!
 * \brief Get profile entries from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index First profile entry index.
 * \param [in] pinfo Profile information.
 * \param [out] profile Profile entries.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_field_profile_get(int unit,
        bcmi_ltsw_field_stage_t stage_id,
        int index,
        ltsw_field_profile_info_t *pinfo, void *profile)
{
    int offset;

    SHR_FUNC_ENTER(unit);

    for (offset = 0; offset < pinfo->entries_per_set; offset ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pinfo->lt_get(unit, stage_id, index + offset,
                           (char*)profile + pinfo->entry_size * offset));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add profile.
 *
 * If same entries existed, just increament reference count.
 *
 * \param [in] unit Unit number.
 * \param [in] profile Profile to be added.
 * \param [in] ref_count Add how many reference counts for this profile.
 * \param [in] pinfo Profile information.
 * \param [out] index Index of the added or existed profile enrty.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_field_profile_add(int unit,
                            bcmi_ltsw_field_stage_t stage_id,
                            void *profile, int ref_count,
                            ltsw_field_profile_info_t *pinfo, int *index)
{
    int entry_idx, offset, rv, rvt;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_allocate(unit,
                pinfo->phd, profile, 0, pinfo->entries_per_set, &entry_idx);
    if (rv == SHR_E_NONE) {
        /* write profile to LT entries starting from entry_idx */
        for (offset = 0; offset < pinfo->entries_per_set; offset ++) {
            rvt = pinfo->lt_ins(unit, stage_id, entry_idx + offset,
                    (char*)profile + pinfo->entry_size * offset);
            if (SHR_FAILURE(rvt)) {
                /* free allocated index */
                (void)bcmi_ltsw_profile_index_free(unit, pinfo->phd, entry_idx);
                SHR_ERR_EXIT(rvt);
            }
        }
    } else if (rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(rv);
    }
    if (ref_count > 1) {
        /* increment reference count if more than one ref_count */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_increase
             (unit, pinfo->phd, pinfo->entries_per_set, entry_idx,
              (ref_count - 1)));
    }

    *index = entry_idx;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add profile reference.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index of the added profile entry.
 * \param [in] ref_count Add how many reference counts for this profile.
 * \param [in] pinfo Profile information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_field_profile_add_ref(int unit, int index, int ref_count,
        ltsw_field_profile_info_t *pinfo)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase
         (unit, pinfo->phd, pinfo->entries_per_set, index, ref_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete profile.
 *
 * If the entry is referred by more than one soure, just decreament ref count.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index of the first entry to be deleted.
 * \param [in] pinfo Profile information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_field_profile_delete(int unit,
        bcmi_ltsw_field_stage_t stage_id,
        int index,
        ltsw_field_profile_info_t *pinfo)
{
    int offset, rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_free(unit, pinfo->phd, index);
    if (rv == SHR_E_NONE) {
        /* delete LT entries starting from index */
        for (offset = 0; offset < pinfo->entries_per_set; offset ++) {
            SHR_IF_ERR_VERBOSE_EXIT(pinfo->lt_del(unit, stage_id,
                                                  index + offset));
        }
    } else if (rv != SHR_E_BUSY) {
        SHR_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover hash and reference count for profile.
 *
 * Hash is recovered from HW. Reference count is always increamented.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile index to be recovered.
 * \param [in] pinfo Profile information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_field_profile_recover(int unit,
                        bcmi_ltsw_field_stage_t stage_id,
                        int index,
                        ltsw_field_profile_info_t *pinfo)
{
    int profile_size;
    void *profile = NULL;
    uint32_t ref_count;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, pinfo->phd, index, &ref_count));
    /* the profile entry is not allocated */
    if (ref_count == 0) {
        profile_size = pinfo->entry_size * pinfo->entries_per_set;
        SHR_ALLOC(profile, profile_size, "profile");
        SHR_NULL_CHECK(profile, SHR_E_MEMORY);
        sal_memset(profile, 0, profile_size);
        /* read profile LT entries */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_field_profile_get(unit, stage_id,
                                           index, pinfo, profile));
        /* recover profile hash */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update
             (unit, pinfo->phd, profile, pinfo->entries_per_set, index));
    }

    /* increment reference count */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase
         (unit, pinfo->phd, pinfo->entries_per_set, index, 1));

exit:
    SHR_FREE(profile);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare profile set.
 *
 * Compare if given profile entries equals to the entries in LT.
 *
 * \param [in] unit Unit number.
 * \param [in] stage_id Stage Id.
 * \param [in] entries First profile entry in the set.
 * \param [in] index Profile table index to be compared.
 * \param [in] pinfo Profile information.
 * \param [out] cmp 0 if equal, otherwise not equal.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_field_ingress_profile_cmp(int unit,
        void *entries, int index,
        ltsw_field_profile_info_t *pinfo, int *cmp)
{
    int profile_size;
    void *profile = NULL;

    SHR_FUNC_ENTER(unit);

    profile_size = pinfo->entry_size * pinfo->entries_per_set;
    SHR_ALLOC(profile, profile_size, "profile");
    SHR_NULL_CHECK(profile, SHR_E_MEMORY);
    sal_memset(profile, 0, profile_size);

    /* read LT entries starting from index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_profile_get(unit, bcmiFieldStageIngress,
                                       index, pinfo, profile));

    *cmp = sal_memcmp(entries, profile, profile_size);

exit:
    SHR_FREE(profile);
    SHR_FUNC_EXIT();
}
