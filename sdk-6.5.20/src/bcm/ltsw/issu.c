/*! \file issu.c
 *
 * BCM internal APIs for ISSU (a.k.a. In Service Software Upgrade).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/generated/bcm_libsum.h>

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmissu/issu_api.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMISSU_CTRL

#define ISSU_UNIT(_u) (bcmi_ltsw_dev_dunit(_u) + BCMDRD_CONFIG_MAX_UNITS)

/*!
 * \brief Maximal string length for software signature.
 */
#define BCMINT_ISSU_SW_SIGNATURE_MAX_LEN    64

/*!
 * \brief General control block structure.
 *
 * This data structure will be stored in a single HA block to maintain general
 * elements of s/w. For example maintain the s/w signature value.
 */
typedef struct bcmint_issu_general_ctrl_s {
    /*! Unit for saving software signature string */
    int unit;

    /*! Software signature string */
    char software_signature[BCMINT_ISSU_SW_SIGNATURE_MAX_LEN];
} bcmint_issu_general_ctrl_t;

static bool bcmi_issu_started = false;
static bcmint_issu_general_ctrl_t *issu_general_ctrl;

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Process general purpose checks.
 *
 * This function is intended to process general purpose checks related to power
 * up. Initially it checks for s/w checksum mismatch during warm boot but it
 * can be expended later to verify other attributes.
 * The function allocates dedicated HA block (from the generic block pool)
 * to maintain the software signature. During warm boot this signature is
 * compared to the actual signature. The function fails if there is a
 * mismatch.
 *
 * \param [in] unit Unit number for saving checksum.
 * \param [in] comp_id The component ID is used to allocate the HA block.
 * \param [in] warm Indicates if it is cold or warm boot.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_MEMORY Failed to allocate HA memory.
 * \return SHR_E_INTERNAL The s/w signature string length is too long.
 * \return SHR_E_FAIL An attempt to perform warm boot with mismatch s/w
 * signature.
 */
static int handle_general_checks(int unit, shr_ha_mod_id comp_id, bool warm)
{
    uint32_t blk_size;
    int len = sal_strlen(BCM_LIBSUM_STR);

    SHR_FUNC_ENTER(unit);

    if (issu_general_ctrl) {
        SHR_EXIT();
    }

    blk_size = sizeof(*issu_general_ctrl);
    issu_general_ctrl = bcmi_ltsw_ha_mem_alloc(unit, comp_id, SHR_HA_SUB_IDS-1,
                                               "issuGeneralCtrl", &blk_size);
    SHR_NULL_CHECK(issu_general_ctrl,  SHR_E_MEMORY);
    if (warm) {
        if (unit != issu_general_ctrl->unit) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* During cold boot or ISSU we need to update the s/w checksum */
    if (!warm || bcmi_issu_started) {
        if (len >= BCMINT_ISSU_SW_SIGNATURE_MAX_LEN) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        sal_strncpy(issu_general_ctrl->software_signature,
                    BCM_LIBSUM_STR,
                    len);
        issu_general_ctrl->software_signature[len] = '\0';
        issu_general_ctrl->unit = unit;
    } else if (sal_strncmp(BCM_LIBSUM_STR,
                           issu_general_ctrl->software_signature,
                           len)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Software signature mismatch\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * ISSU related public functions
 */

int
bcmi_ltsw_issu_upgrade_start(const char *from_ver, const char *to_ver)
{
    char lib_name[80];
    int len;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    len = sal_snprintf(lib_name, sizeof(lib_name), "libbcmissu.so.%s", to_ver);
    if ((uint32_t)len >= sizeof(lib_name)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Invalid version %s\n"), to_ver));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmissu_native_upgrade_start(from_ver, to_ver, lib_name));

    bcmi_issu_started = true;

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_issu_upgrade_done(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmissu_native_upgrade_done());

    bcmi_issu_started = false;

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_issu_init(int unit)
{
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmissu_native_unit_init(ISSU_UNIT(unit), BCMI_HA_COMP_ID_ISSU, warm));

    SHR_IF_ERR_EXIT
        (handle_general_checks(unit, BCMI_HA_COMP_ID_ISSU, warm));

    if (warm && bcmi_issu_started) {
        SHR_IF_ERR_EXIT
            (bcmissu_native_unit_upgrade(ISSU_UNIT(unit)));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_issu_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmissu_native_unit_shutdown(ISSU_UNIT(unit), true));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmi_ltsw_issu_struct_info_report(int unit,
                                  shr_ha_mod_id comp_id,
                                  shr_ha_sub_id sub_id,
                                  uint32_t offset,
                                  uint32_t data_size,
                                  size_t instances,
                                  bcmissu_struct_id_t id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_report(ISSU_UNIT(unit), comp_id, sub_id, offset,
                                    data_size, instances, id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_issu_struct_info_get(int unit,
                               bcmissu_struct_id_t id,
                               shr_ha_mod_id comp_id,
                               shr_ha_sub_id sub_id,
                               uint32_t *offset)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_get(ISSU_UNIT(unit), id, comp_id, sub_id,
                                 offset));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_issu_struct_info_clear(int unit,
                                 bcmissu_struct_id_t id,
                                 shr_ha_mod_id comp_id,
                                 shr_ha_sub_id sub_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_clear(ISSU_UNIT(unit), id, comp_id, sub_id));

exit:
    SHR_FUNC_EXIT();
}
