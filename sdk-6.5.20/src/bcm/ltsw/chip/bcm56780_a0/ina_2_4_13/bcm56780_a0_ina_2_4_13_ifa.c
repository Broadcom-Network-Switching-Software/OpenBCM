/*! \file bcm56780_a0_ina_2_4_13_ifa.c
 *
 * BCM56780_A0 INA_2_4_13 IFA driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/ifa.h>
#include <bcm_int/ltsw/xfs/ifa.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_IFA

/******************************************************************************
 * Private functions
 */

static int
sc_ctrl_set(
    int unit,
    const char *lt_name,
    const char *lt_fld_name,
    uint32 val)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = lt_fld_name;
    fields[0].u.val = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, lt_name, &entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_ctrl_get(
    int unit,
    const char *lt_name,
    const char *lt_fld_name,
    uint32 *val)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmlt_field_def_t fld_def;

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = lt_fld_name;

    if (SHR_FAILURE(bcmi_lt_entry_get(unit, lt_name, &entry, NULL, NULL))) {
        sal_memset(&fld_def, 0, sizeof(fld_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, lt_name, lt_fld_name, &fld_def));
        fields[0].u.val = fld_def.def;
    }
    *val = fields[0].u.val;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ina_2_4_13_ltsw_ifa_control_set(
    int unit,
    bcmi_ltsw_ifa_control_t control,
    uint32 arg)
{
    SHR_FUNC_ENTER(unit);

    switch (control) {
        case bcmiIfaControlIntProbeMarker1:
            SHR_IF_ERR_VERBOSE_EXIT
                (sc_ctrl_set(unit, R_IFA_1_PROBE_MARKERs, VALUE_0s, arg));
            break;
        case bcmiIfaControlIntProbeMarker2:
            SHR_IF_ERR_VERBOSE_EXIT
                (sc_ctrl_set(unit, R_IFA_1_PROBE_MARKERs, VALUE_1s, arg));
            break;
        default :
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ina_2_4_13_ltsw_ifa_control_get(
    int unit,
    bcmi_ltsw_ifa_control_t control,
    uint32 *arg)
{
    SHR_FUNC_ENTER(unit);

    switch (control) {
        case bcmiIfaControlIntProbeMarker1:
            SHR_IF_ERR_VERBOSE_EXIT
                (sc_ctrl_get(unit, R_IFA_1_PROBE_MARKERs, VALUE_0s, arg));
            break;
        case bcmiIfaControlIntProbeMarker2:
            SHR_IF_ERR_VERBOSE_EXIT
                (sc_ctrl_get(unit, R_IFA_1_PROBE_MARKERs, VALUE_1s, arg));
            break;
        default :
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }


exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Ifa variant driver function variable for bcm56780_a0 device.
 */
static mbcm_ltsw_ifa_drv_t bcm56780_a0_ina_2_4_13_ltsw_ifa_drv = {
    .ifa_init = xfs_ltsw_ifa_init,
    .ifa_detach = xfs_ltsw_ifa_detach,
    .ifa_control_set = bcm56780_a0_ina_2_4_13_ltsw_ifa_control_set,
    .ifa_control_get = bcm56780_a0_ina_2_4_13_ltsw_ifa_control_get
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_ina_2_4_13_ltsw_ifa_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ifa_drv_set(unit, &bcm56780_a0_ina_2_4_13_ltsw_ifa_drv));

exit:
    SHR_FUNC_EXIT();
}

