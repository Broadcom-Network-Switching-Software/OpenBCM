/*! \file issu_hw_patch.c
 *
 * Applying ISSU component fix-up.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_types.h>
#include <bcmissu/issu_internal.h>
#include <bcmlrd/bcmlrd_conf.h>

/*******************************************************************************
 * Local definitions
 */
/* macros/defines */
#define BSL_LOG_MODULE BSL_LS_BCMISSU_DLL

#define PATCH_RECORD_MAX_SIZE 1000
#define PATCH_RECORD_SUB_ID 0x3FF   /* The last sub ID */
#define PATCH_RECORD_SIGNATURE 0x683d8b3a

typedef struct {
    uint32_t signature;
    uint32_t patch_count;
    uint32_t patch_id[PATCH_RECORD_MAX_SIZE];
} patch_record_t;

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Determine if a patch is eligible to be applied.
 *
 * This function checks the list of already applied patches to prevent
 * reapplying of the same patch more than once.
 * It also checks if the patch introduction version is within the range
 * of the current upgrade. This check prevents applying a patch during the
 * first upgrade after cold boot where the list of previously applied patches
 * will be cleared.
 *
 * \param [in] unit Unit associated with this patch.
 * \param [in] patch The patch to check its eligibility.
 * \param [in] ha_rec The HA block contains records of all previously applied
 * patches since the last cold boot.
 *
 * \return true if the patch is eligible to be installed and false otherwise.
 */
static bool patch_is_eligible(int unit,
                              const bcmissu_comp_patch_hdl_t *patch,
                              patch_record_t *ha_rec)
{
    uint32_t j;
    const issu_version_list_t *issu_struct_dict;
    const char *var_name = bcmlrd_variant_name(unit);

    /* First check if the patch variant matches the current variant */
    if (patch->variant[0] != '*' && var_name && var_name[0] != '\0') {
        if (sal_strncasecmp(patch->variant, var_name, strlen(var_name)) != 0) {
            return false;
        }
    }
    /* Second check if the patch had been already applied. */
    for (j = 0; j < ha_rec->patch_count; j++) {
        if (patch->id == ha_rec->patch_id[j]) {
            return false;
        }
    }

    /*
     * Now check that the patch introduction version matches the upgrade
     * version range.
     */
    issu_struct_dict = bcmissu_struct_db_get();
    for (j = 0; issu_struct_dict[j].ver_db; j++) {
        if (sal_strcmp(patch->ver, issu_struct_dict[j].version) == 0) {
            return true;
        }
    }
    return false;
}

/*!
 *\brief Invoke component patch callback.
 *
 * This function receives a vector of h/w patch callback functions. It then
 * invokes every callback function of the vector.
 *
 * \param [in] unit The unit associated with the upgrade.
 * \param [in] patches The patches vector.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int bcmissu_apply_patches(int unit, const bcmissu_comp_patch_hdl_t *patches)
{
    const bcmissu_comp_patch_hdl_t *patch;
    int rv = SHR_E_NONE;
    patch_record_t *patch_ha_rec;
    uint32_t ha_blk_size, actual;
    char ha_id[MAX_BLK_ID_STR_LEN];

    if (!patches) {
        return SHR_E_NONE;
    }

    /*
     * We keep the applied patch IDs within an HA block so when we do upgrade
     * we check that the patch had not been already installed.
     */
    /* Allocate the HA block associated with the patches */
    ha_blk_size = sizeof(patch_record_t);
    actual = ha_blk_size;
    shr_ha_str_to_id("applied_patches",
                     SHR_HA_BLK_USAGE_OTHER,
                     MAX_BLK_ID_STR_LEN,
                     ha_id);
    patch_ha_rec = shr_ha_mem_alloc(unit,
                                    BCMMGMT_ISSU_COMP_ID,
                                    PATCH_RECORD_SUB_ID,
                                    ha_id,
                                    &actual);
    if (!patch_ha_rec || actual < ha_blk_size) {
        return SHR_E_MEMORY;
    }
    /* If the block is new initialize it */
    if (patch_ha_rec->signature != PATCH_RECORD_SIGNATURE) {
        patch_ha_rec->patch_count = 0;
        patch_ha_rec->signature = PATCH_RECORD_SIGNATURE;
    }
    patch = patches;
    while (patch && patch->func) {
        if (patch_is_eligible(unit, patch, patch_ha_rec)) {
            rv = patch->func(unit);
            if (SHR_FAILURE(rv)) {
                break;
            }
            patch_ha_rec->patch_id[patch_ha_rec->patch_count++] = patch->id;
        }
        patch++;
    }
    return rv;
}

