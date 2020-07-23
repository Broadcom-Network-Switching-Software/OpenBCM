/*! \file bcmcth_mon_flowtracker_util.c
 *
 * Flowtracker util functions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <shr/shr_ha.h>

#include <bcmlrd/bcmlrd_map.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcth/bcmcth_mon_drv.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmcth/bcmcth_mon_flowtracker_util.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

bool
bcmcth_mon_ft_imm_mapped(int unit, const bcmdrd_sid_t sid)
{
    int rv;
    const bcmlrd_map_t *map = NULL;

    rv = bcmlrd_map_get(unit, sid, &map);
    if (SHR_SUCCESS(rv) &&
        map &&
        map->group &&
        map->group[0].dest.kind == BCMLRD_MAP_CUSTOM) {
        return TRUE;
    }
    return FALSE;
}

int
bcmcth_mon_flowtracker_export_template_out_fields_count(
    int unit,
    uint32_t *count)
{
    uint32_t    fid = 0;
    uint32_t    max = 0;
    bcmlrd_field_def_t  def;

    SHR_FUNC_ENTER(unit);

    fid = MON_FLOWTRACKER_EXPORT_TEMPLATEt_EXPORT_ELEMENTSu_TYPEf;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmlrd_table_field_def_get(unit, MON_FLOWTRACKER_EXPORT_TEMPLATEt, fid,
                &def));

    max = def.depth;

    /* Total possible object ID count plus Instance fields
     * count excluding key. 4 elements in the structure
     */
    *count = (max * 4 + MON_FLOWTRACKER_EXPORT_TEMPLATEt_FIELD_COUNT - 1);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_flowtracker_group_out_fields_count(
    int unit,
    uint32_t *count)
{
    uint32_t    fid1 = 0, fid2 = 0, fid3 = 0;
    uint32_t    max = 0;
    bcmlrd_field_def_t  def1, def2, def3;

    SHR_FUNC_ENTER(unit);

    fid1 = MON_FLOWTRACKER_GROUPt_ACTIONSu_TYPEf;
    fid2 = MON_FLOWTRACKER_GROUPt_TRACKING_PARAMETERSu_TYPEf;
    fid3 = MON_FLOWTRACKER_GROUPt_EXPORT_TRIGGERSf;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmlrd_table_field_def_get(unit, MON_FLOWTRACKER_GROUPt, fid1,
                &def1));

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmlrd_table_field_def_get(unit, MON_FLOWTRACKER_GROUPt, fid2,
                &def2));

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmlrd_table_field_def_get(unit, MON_FLOWTRACKER_GROUPt, fid3,
                &def3));

    /* FLOWTRACKER_ACTION_INFO_T has 11 members and
       FLOWTRACKER_TRACKING_PARAM_T has 2 members */

    max = (def1.depth * 11) + (def2.depth * 2) + def3.depth;

    /* Total possible object ID count plus Instance fields
     * count excluding key.
     */
    *count = (max + MON_FLOWTRACKER_GROUPt_FIELD_COUNT - 1);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_flowtracker_control_out_fields_count(
    int unit,
    uint32_t *count)
{
    uint32_t    fid1 = 0;
    uint32_t    max = 0;
    bcmlrd_field_def_t  def1;

    SHR_FUNC_ENTER(unit);

    fid1 = MON_FLOWTRACKER_CONTROLt_MAX_FLOWSf;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmlrd_table_field_def_get(unit, MON_FLOWTRACKER_CONTROLt, fid1,
                &def1));

    max = def1.depth;

    /* Total possible object ID count plus Instance fields
     * count excluding key.
     * max_flows and their corresponding oper fields
     */
    *count = (max * 2 + MON_FLOWTRACKER_CONTROLt_FIELD_COUNT - 1);

exit:
    SHR_FUNC_EXIT();
}
