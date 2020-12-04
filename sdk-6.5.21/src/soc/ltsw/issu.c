/*! \file issu.c
 *
 * BCM internal APIs for ISSU (a.k.a. In Service Software Upgrade).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/cm.h>
#include <soc/ltsw/ha.h>
#include <soc/ltsw/issu.h>

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmissu/issu_api.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMISSU_CTRL

/*!
 * \brief Maximal string length for software signature.
 */
#define SOC_LTSW_ISSU_SW_SIGNATURE_MAX_LEN    64

/*!
 * \brief General control block structure.
 *
 * This data structure will be stored in a single HA block to maintain general
 * elements of s/w. For example maintain the s/w signature value.
 */
typedef struct issu_general_ctrl_s {
    /*! Unit for saving software signature string */
    int unit;

    /*! Software signature string */
    char software_signature[SOC_LTSW_ISSU_SW_SIGNATURE_MAX_LEN];
} issu_general_ctrl_t;

static const char *start_from_ver;
static const char *current_ver;
static bool issu_started = false;
static issu_general_ctrl_t *issu_general_ctrl;

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
static int
handle_general_checks(
    int unit,
    soc_ltsw_ha_mod_id_t comp_id,
    const char *libsum,
    bool warm)
{
    uint32_t blk_size;
    int len;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(libsum, SHR_E_INIT);

    if (issu_general_ctrl) {
        SHR_EXIT();
    }

    blk_size = sizeof(*issu_general_ctrl);
    issu_general_ctrl = soc_ltsw_ha_mem_alloc(unit, comp_id,
                                              SOC_LTSW_HA_SUB_IDS-1,
                                              "issuGeneralCtrl", &blk_size);
    SHR_NULL_CHECK(issu_general_ctrl,  SHR_E_MEMORY);
    if (warm) {
        if (unit != issu_general_ctrl->unit) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    len = sal_strlen(libsum);
    /* During cold boot or ISSU we need to update the s/w checksum */
    if (!warm || issu_started) {
        if (len >= SOC_LTSW_ISSU_SW_SIGNATURE_MAX_LEN) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        sal_strncpy(issu_general_ctrl->software_signature,
                    libsum,
                    len);
        issu_general_ctrl->software_signature[len] = '\0';
        issu_general_ctrl->unit = unit;
    } else if (sal_strncmp(libsum,
                           issu_general_ctrl->software_signature,
                           len)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Software signature mismatch\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
upgrade_start(const char *libname)
{
    char lib_name[80];
    int len;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    assert(libname != NULL);
    assert(issu_started);
    assert(start_from_ver != NULL);
    assert(current_ver != NULL);

    len = sal_snprintf(lib_name, sizeof(lib_name), "%s.%s", libname, current_ver);
    if ((uint32_t)len >= sizeof(lib_name)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Invalid version %s\n"), current_ver));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmissu_native_upgrade_start(start_from_ver, current_ver, lib_name));

exit:
    SHR_FUNC_EXIT();
}


static int
upgrade_done(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmissu_native_upgrade_done());

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * ISSU related public functions
 */

int
soc_ltsw_issu_upgrade_start(const char *from_ver, const char *to_ver)
{
    start_from_ver = from_ver;
    current_ver = to_ver;
    issu_started = true;
    return SHR_E_NONE;
}


int
soc_ltsw_issu_upgrade_done(void)
{
    issu_started = false;
    start_from_ver = NULL;
    current_ver = NULL;
    return SHR_E_NONE;
}

int
soc_ltsw_issu_init(int unit, soc_ltsw_issu_info_t *issu_info)
{
    int dunit;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(issu_info, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (soc_cm_dev_num_get(unit, &dunit));

    dunit += BCMDRD_CONFIG_MAX_UNITS;

    SHR_IF_ERR_EXIT
        (bcmissu_native_unit_init(dunit,
                                  issu_info->ha_comp_id,
                                  issu_info->warm));
    SHR_IF_ERR_EXIT
        (handle_general_checks(unit,
                               issu_info->ha_comp_id,
                               issu_info->libsum,
                               issu_info->warm));

    if (issu_info->warm && issu_started) {
        SHR_IF_ERR_EXIT
            (upgrade_start(issu_info->libname));
        SHR_IF_ERR_EXIT
            (bcmissu_native_unit_upgrade(dunit));
        SHR_IF_ERR_EXIT
            (upgrade_done());
    }

exit:
    SHR_FUNC_EXIT();
}

int
soc_ltsw_issu_detach(int unit)
{
    int dunit;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (soc_cm_dev_num_get(unit, &dunit));

    dunit += BCMDRD_CONFIG_MAX_UNITS;

    SHR_IF_ERR_EXIT
        (bcmissu_native_unit_shutdown(dunit, true));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
soc_ltsw_issu_struct_info_report(
    int unit,
    soc_ltsw_ha_mod_id_t comp_id,
    soc_ltsw_ha_sub_id_t sub_id,
    uint32_t offset,
    uint32_t data_size,
    size_t instances,
    bcmissu_struct_id_t id)
{
    int dunit;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (soc_cm_dev_num_get(unit, &dunit));

    dunit += BCMDRD_CONFIG_MAX_UNITS;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_report(dunit, comp_id, sub_id, offset,
                                    data_size, instances, id));

exit:
    SHR_FUNC_EXIT();
}

int
soc_ltsw_issu_struct_info_get(
    int unit,
    bcmissu_struct_id_t id,
    soc_ltsw_ha_mod_id_t comp_id,
    soc_ltsw_ha_sub_id_t sub_id,
    uint32_t *offset)
{
    int dunit;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (soc_cm_dev_num_get(unit, &dunit));

    dunit += BCMDRD_CONFIG_MAX_UNITS;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_get(dunit, id, comp_id, sub_id,
                                 offset));

exit:
    SHR_FUNC_EXIT();
}

int
soc_ltsw_issu_struct_info_clear(
    int unit,
    bcmissu_struct_id_t id,
    soc_ltsw_ha_mod_id_t comp_id,
    soc_ltsw_ha_sub_id_t sub_id)
{
    int dunit;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (soc_cm_dev_num_get(unit, &dunit));

    dunit += BCMDRD_CONFIG_MAX_UNITS;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_clear(dunit, id, comp_id, sub_id));

exit:
    SHR_FUNC_EXIT();
}

