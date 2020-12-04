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
#include <bcm_int/common/generated/bcm_libsum.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/issu.h>

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <shr/shr_debug.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMISSU_CTRL

/*******************************************************************************
 * ISSU related public functions
 */

int
bcmi_ltsw_issu_init(int unit)
{
    soc_ltsw_issu_info_t issu_info;

    SHR_FUNC_ENTER(unit);

    issu_info.warm = bcmi_warmboot_get(unit);
    issu_info.ha_comp_id = BCMI_HA_COMP_ID_ISSU;
    issu_info.libname = "libbcmissu.so";
    issu_info.libsum = BCM_LIBSUM_STR;
    SHR_IF_ERR_EXIT
        (soc_ltsw_issu_init(unit, &issu_info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_issu_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (soc_ltsw_issu_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmi_ltsw_issu_struct_info_report(int unit,
                                  soc_ltsw_ha_mod_id_t comp_id,
                                  soc_ltsw_ha_sub_id_t sub_id,
                                  uint32_t offset,
                                  uint32_t data_size,
                                  size_t instances,
                                  bcmissu_struct_id_t id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (soc_ltsw_issu_struct_info_report(unit, comp_id, sub_id, offset,
                                            data_size, instances, id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_issu_struct_info_get(int unit,
                               bcmissu_struct_id_t id,
                               soc_ltsw_ha_mod_id_t comp_id,
                               soc_ltsw_ha_sub_id_t sub_id,
                               uint32_t *offset)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (soc_ltsw_issu_struct_info_get(unit, id, comp_id, sub_id,
                                         offset));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_issu_struct_info_clear(int unit,
                                 bcmissu_struct_id_t id,
                                 soc_ltsw_ha_mod_id_t comp_id,
                                 soc_ltsw_ha_sub_id_t sub_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (soc_ltsw_issu_struct_info_clear(unit, id, comp_id, sub_id));

exit:
    SHR_FUNC_EXIT();
}
