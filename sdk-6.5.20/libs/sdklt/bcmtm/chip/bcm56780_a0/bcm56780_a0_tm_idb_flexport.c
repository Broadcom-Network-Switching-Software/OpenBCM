/*! \file bcm56780_a0_tm_idb_flexport.c
 *
 * File containing flexport sequence for bcm56780_a0.
 *
 * The code in this file is shared with DV team. This file will only be updated
 * by porting DV code changes. If any extra change needs to be made to the
 * shared code, please also inform the DV team.
 *
 * DV file: $DV/trident4/dv/ngsdk_interface/flexport/idb_flexport.c
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmtm/chip/bcm56780_a0_tm.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include "bcm56780_a0_tm_e2e_settings.h"
#include "bcm56780_a0_tm_idb_flexport.h"


#define BSL_LOG_MODULE  BSL_LS_BCMTM_FLEXPORT

/*******************************************************************************
 * START SDKLT-DV SHARED CODE
 */


/*******************************************************************************
 * LOCAL CONSTANT VARIABLES
 */
#define IDB_OBM_PRI_MAP_ENTRY_COUNT 8
#define MGMT_OBM_PRI_MAP_ENTRY_COUNT 8

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static int
bcm56780_a0_tm_get_pipe_pm_from_phy_port(int pnum)
{
    return ((pnum==TD4_X9_PHY_PORT_LPBK0)                        ? 1 :
            (pnum==TD4_X9_PHY_PORT_LPBK1)                        ? 3 :
            (pnum==TD4_X9_PHY_PORT_CPU)                          ? 0 :
            (pnum==TD4_X9_PHY_PORT_MNG0)                         ? 1 :
            (pnum==TD4_X9_PHY_PORT_MNG1)                         ? 3 :
            (pnum==TD4_X9_PHY_PORT_RDB0)                         ? 0 :
            (pnum==TD4_X9_PHY_PORT_RDB1)                         ? 2 :
            (((pnum - 1) %0x28) >> 3));
}

static int
bcm56780_a0_tm_get_subp_from_phy_port(int pnum)
{
    return ((pnum - 1) & 0x7);
}

static int
bcm56780_a0_tm_speed_to_e2e_settings_class_map(int speed)
{
    int e2e_settings_class;
    int idx;

    e2e_settings_class = 0;
    for (idx = 0; idx < COUNTOF(bcm56780_a0_idb_e2e_settings_tbl); idx++) {
        if (bcm56780_a0_idb_e2e_settings_tbl[idx].speed == speed) {
            e2e_settings_class = idx;
            break;
        }
    }
    return e2e_settings_class;
}

static int
bcm56780_a0_tm_idb_wr_obm_shared_config(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmtm_drv_info_t *drv_info;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int phy_port;
    int pipe_num;
    int pm_num;
    int num_lanes;
    int lossless;
    int sop_discard_mode;
    const bcmdrd_sid_t obm_shared_cfg[TD4_X9_PBLKS_PER_PIPE]= {
        IDB_OBM0_SHARED_CONFIGr, IDB_OBM1_SHARED_CONFIGr,
        IDB_OBM2_SHARED_CONFIGr, IDB_OBM3_SHARED_CONFIGr,
        IDB_OBM4_SHARED_CONFIGr
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    num_lanes = port_map->lport_map[lport].num_lanes;
    lossless = drv_info->lossless;
    sop_discard_mode = bcm56780_a0_obm_settings[lossless][num_lanes].sop_discard_mode;
    reg = obm_shared_cfg[pm_num];
    fid = SOP_DISCARD_MODEf;
    fval = sop_discard_mode;
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_wr_obm_thresh(int unit, int lport)
{
    bcmtm_drv_info_t *drv_info;
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t fval;
    int idx;
    uint32_t lossless0_discard, lossy_discard, lossy_low_pri, discard_limit,
             lossless1_discard;
    int phy_port;
    int pm_num;
    int subp;
    int num_lanes;
    int pipe_num;
    int lossless;
    static const bcmdrd_sid_t obm_thresh_pp[TD4_X9_PBLKS_PER_PIPE][2] = {
        {IDB_OBM0_THRESHOLDr, IDB_OBM0_THRESHOLD_1r},
        {IDB_OBM1_THRESHOLDr, IDB_OBM1_THRESHOLD_1r},
        {IDB_OBM2_THRESHOLDr, IDB_OBM2_THRESHOLD_1r},
        {IDB_OBM3_THRESHOLDr, IDB_OBM3_THRESHOLD_1r},
        {IDB_OBM4_THRESHOLDr, IDB_OBM4_THRESHOLD_1r}
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    num_lanes = port_map->lport_map[lport].num_lanes;
    lossless = drv_info->lossless;
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    discard_limit     = bcm56780_a0_obm_settings[lossless][num_lanes].discard_limit;
    lossless1_discard = bcm56780_a0_obm_settings[lossless][num_lanes].lossless_discard;
    lossless0_discard = bcm56780_a0_obm_settings[lossless][num_lanes].lossless_discard;
    lossy_discard     = bcm56780_a0_obm_settings[lossless][num_lanes].lossy_discard;
    lossy_low_pri     = bcm56780_a0_obm_settings[lossless][num_lanes].lossy_low_pri;

    for (idx = 0; idx < 2; idx++) {
        reg = obm_thresh_pp[pm_num][idx];
        BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
        sal_memset(ltmbuf, 0, sizeof(ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, ltmbuf));
        if (idx == 0) {
            fid = LOSSLESS0_DISCARDf;
            fval = lossless0_discard;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            fid = LOSSY_DISCARDf;
            fval = lossy_discard;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            fid = LOSSY_LOW_PRIf;
            fval = lossy_low_pri;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));
        } else {
            fid = DISCARD_LIMITf;
            fval = discard_limit;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            fid = LOSSLESS1_DISCARDf;
            fval = lossless1_discard;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));
        }
        BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_wr_obm_flow_ctrl_cfg (int unit, int lport)
{
    bcmtm_drv_info_t *drv_info;
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t fval;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int lossless;
    const bcmdrd_sid_t obm_flow_ctrl_cfg[TD4_X9_PBLKS_PER_PIPE] = {
        IDB_OBM0_FLOW_CONTROL_CONFIGr,
        IDB_OBM1_FLOW_CONTROL_CONFIGr,
        IDB_OBM2_FLOW_CONTROL_CONFIGr,
        IDB_OBM3_FLOW_CONTROL_CONFIGr,
        IDB_OBM4_FLOW_CONTROL_CONFIGr
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    lossless = drv_info->lossless;
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    reg = obm_flow_ctrl_cfg[pm_num];

    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, ltmbuf));

    if (lossless == TD4_X9_OBM_PARAMS_LOSSLESS_INDEX) {
        fid = FC_TYPEf;
        fval = 0;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

        fid = PORT_FC_ENf;
        fval = 1;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

        fid = LOSSLESS0_FC_ENf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

        fval = 0;
        fid = LOSSLESS1_FC_ENf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

        fval = 65535;
        fid = LOSSLESS0_PRIORITY_PROFILEf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

        fval = 0;
        fid = LOSSLESS1_PRIORITY_PROFILEf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));
    } else if (lossless == TD4_X9_OBM_PARAMS_LOSSY_INDEX) {
        fid = FC_TYPEf;
        fval = 0;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

        fid = PORT_FC_ENf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

        fid = LOSSLESS0_FC_ENf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

        fid = LOSSLESS1_FC_ENf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

        fid = LOSSLESS0_PRIORITY_PROFILEf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

        fid = LOSSLESS1_PRIORITY_PROFILEf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));
    } else {
        fid = FC_TYPEf;
        fval = 1;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

        fid = PORT_FC_ENf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

        fid = LOSSLESS0_FC_ENf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

        fval = 0;
        fid = LOSSLESS1_FC_ENf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

        fval = 1;
        fid = LOSSLESS0_PRIORITY_PROFILEf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

        fval = 0;
        fid = LOSSLESS1_PRIORITY_PROFILEf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));
    }
    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_wr_obm_fc_threshold(int unit, int lport)
{
    bcmtm_drv_info_t *drv_info;
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t fval;
    int idx;
    uint32_t lossless0_xon, lossless0_xoff, lossless1_xon, lossless1_xoff,
             port_xon, port_xoff;
    int phy_port;
    int pm_num;
    int subp;
    int num_lanes;
    int lossless;
    int pipe_num;
    const bcmdrd_sid_t obm_fc_thresh_pp[TD4_X9_PBLKS_PER_PIPE][2] = {
        {IDB_OBM0_FC_THRESHOLDr, IDB_OBM0_FC_THRESHOLD_1r},
        {IDB_OBM1_FC_THRESHOLDr, IDB_OBM1_FC_THRESHOLD_1r},
        {IDB_OBM2_FC_THRESHOLDr, IDB_OBM2_FC_THRESHOLD_1r},
        {IDB_OBM3_FC_THRESHOLDr, IDB_OBM3_FC_THRESHOLD_1r},
        {IDB_OBM4_FC_THRESHOLDr, IDB_OBM4_FC_THRESHOLD_1r}
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    num_lanes = port_map->lport_map[lport].num_lanes;
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    lossless = drv_info->lossless;
    lossless0_xon  = bcm56780_a0_obm_settings[lossless][num_lanes].lossless_xon;
    lossless1_xon  = bcm56780_a0_obm_settings[lossless][num_lanes].lossless_xon;
    lossless0_xoff = bcm56780_a0_obm_settings[lossless][num_lanes].lossless_xoff;
    lossless1_xoff = bcm56780_a0_obm_settings[lossless][num_lanes].lossless_xoff;
    port_xon       = bcm56780_a0_obm_settings[lossless][num_lanes].port_xon;
    port_xoff      = bcm56780_a0_obm_settings[lossless][num_lanes].port_xoff;

    for (idx = 0; idx < 2; idx++) {
        reg = obm_fc_thresh_pp[pm_num][idx];
        sal_memset(ltmbuf, 0, sizeof(ltmbuf));
        BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, ltmbuf));
        if (idx == 0) {
            fid = LOSSLESS0_XONf;
            fval = lossless0_xon;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            fid = LOSSLESS0_XOFFf;
            fval = lossless0_xoff;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            fid = LOSSLESS1_XONf;
            fval = lossless1_xon;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            fid = LOSSLESS1_XOFFf;
            fval = lossless1_xoff;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));
        }
        else {
            fid = PORT_XONf;
            fval = port_xon;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            fid = PORT_XOFFf;
            fval = port_xoff;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));
        }
        BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_wr_obm_ct_thresh(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t fval;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int speed_encoding;
    uint32_t ct_threshold;
    static const bcmdrd_sid_t obm_ct_thresh[TD4_X9_PBLKS_PER_PIPE] = {
        IDB_OBM0_CT_THRESHOLDr, IDB_OBM1_CT_THRESHOLDr,
        IDB_OBM2_CT_THRESHOLDr, IDB_OBM3_CT_THRESHOLDr,
        IDB_OBM4_CT_THRESHOLDr
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    speed_encoding =
        bcm56780_a0_tm_speed_to_e2e_settings_class_map(port_map->lport_map[lport].max_speed);
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    ct_threshold =
        bcm56780_a0_idb_e2e_settings_tbl[speed_encoding].obm_ct_threshold;
    reg = obm_ct_thresh[pm_num];

    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, ltmbuf));
    fid = CUT_THROUGH_OKf;
    fval = ct_threshold;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_wr_obm_port_config(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    static const bcmdrd_sid_t obm_port_config[TD4_X9_PBLKS_PER_PIPE] = {
        IDB_OBM0_PORT_CONFIGr, IDB_OBM1_PORT_CONFIGr,
        IDB_OBM2_PORT_CONFIGr, IDB_OBM3_PORT_CONFIGr,
        IDB_OBM4_PORT_CONFIGr
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    reg = obm_port_config[pm_num];

    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    fid = OUTER_TPID_ENABLEf;
    fval = 1;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_obm_port_type(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int port_hg3 = 0;
    static const bcmdrd_sid_t obm_port_config[TD4_X9_PBLKS_PER_PIPE] = {
        IDB_OBM0_PORT_CONFIGr, IDB_OBM1_PORT_CONFIGr,
        IDB_OBM2_PORT_CONFIGr, IDB_OBM3_PORT_CONFIGr,
        IDB_OBM4_PORT_CONFIGr
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    reg = obm_port_config[pm_num];

    if (port_map->lport_map[lport].flags & BCMTM_PORT_IS_HG3) {
        port_hg3 = 1;
    }

    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    fid = PORT_TYPEf;
    fval = port_hg3;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_wr_obm_force_saf_config(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int speed_encoding;
    int unsatisfied_threshold;
    static const bcmdrd_sid_t obm_force_saf_config_regs[TD4_X9_PBLKS_PER_PIPE] = {
        IDB_OBM0_DBG_Ar, IDB_OBM1_DBG_Ar,
        IDB_OBM2_DBG_Ar, IDB_OBM3_DBG_Ar, IDB_OBM4_DBG_Ar
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    speed_encoding =
        bcm56780_a0_tm_speed_to_e2e_settings_class_map(port_map->lport_map[lport].max_speed);
    unsatisfied_threshold =
        bcm56780_a0_idb_e2e_settings_tbl[speed_encoding].obm_unsatisfied_threshold;
    reg = obm_force_saf_config_regs[pm_num];

    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    fid = FIELD_Cf;
    fval = unsatisfied_threshold;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_wr_obm_lag_detection_config(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int speed_encoding;
    int enter_threshold;
    int exit_threshold;
    static const bcmdrd_sid_t obm_lag_detection_config_regs[TD4_X9_PBLKS_PER_PIPE] = {
        IDB_OBM0_DBG_Br, IDB_OBM1_DBG_Br,
        IDB_OBM2_DBG_Br, IDB_OBM3_DBG_Br, IDB_OBM4_DBG_Br
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    speed_encoding =
        bcm56780_a0_tm_speed_to_e2e_settings_class_map(port_map->lport_map[lport].max_speed);
    enter_threshold =
        bcm56780_a0_idb_e2e_settings_tbl[speed_encoding].obm_enter_lagging_threshold;
    exit_threshold =
        bcm56780_a0_idb_e2e_settings_tbl[speed_encoding].obm_exit_lagging_threshold;
    reg = obm_lag_detection_config_regs[pm_num];

    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));

    fid = FIELD_Af;
    fval = enter_threshold;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = FIELD_Bf;
    fval = exit_threshold;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_obm_pri_map_set(int unit, int lport)
{
    bcmtm_drv_info_t *drv_info;
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t mem;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int pipe_num;
    int pm_num;
    int subp;
    int phy_port;
    int entry_index;
    int field_index;
    int count;
    int lossless;
    uint32_t priority;
    static const bcmdrd_sid_t obm_pri_map_mem[TD4_X9_PBLKS_PER_PIPE]
                                             [TD4_X9_PORTS_PER_PBLK] = {
        {IDB_OBM0_PRI_MAP_PORT0m, IDB_OBM0_PRI_MAP_PORT1m,
        IDB_OBM0_PRI_MAP_PORT2m, IDB_OBM0_PRI_MAP_PORT3m,
        IDB_OBM0_PRI_MAP_PORT4m, IDB_OBM0_PRI_MAP_PORT5m,
        IDB_OBM0_PRI_MAP_PORT6m, IDB_OBM0_PRI_MAP_PORT7m},
        {IDB_OBM1_PRI_MAP_PORT0m, IDB_OBM1_PRI_MAP_PORT1m,
        IDB_OBM1_PRI_MAP_PORT2m, IDB_OBM1_PRI_MAP_PORT3m,
        IDB_OBM1_PRI_MAP_PORT4m, IDB_OBM1_PRI_MAP_PORT5m,
        IDB_OBM1_PRI_MAP_PORT6m, IDB_OBM1_PRI_MAP_PORT7m},
        {IDB_OBM2_PRI_MAP_PORT0m, IDB_OBM2_PRI_MAP_PORT1m,
        IDB_OBM2_PRI_MAP_PORT2m, IDB_OBM2_PRI_MAP_PORT3m,
        IDB_OBM2_PRI_MAP_PORT4m, IDB_OBM2_PRI_MAP_PORT5m,
        IDB_OBM2_PRI_MAP_PORT6m, IDB_OBM2_PRI_MAP_PORT7m},
        {IDB_OBM3_PRI_MAP_PORT0m, IDB_OBM3_PRI_MAP_PORT1m,
        IDB_OBM3_PRI_MAP_PORT2m, IDB_OBM3_PRI_MAP_PORT3m,
        IDB_OBM3_PRI_MAP_PORT4m, IDB_OBM3_PRI_MAP_PORT5m,
        IDB_OBM3_PRI_MAP_PORT6m, IDB_OBM3_PRI_MAP_PORT7m},
        {IDB_OBM4_PRI_MAP_PORT0m, IDB_OBM4_PRI_MAP_PORT1m,
        IDB_OBM4_PRI_MAP_PORT2m, IDB_OBM4_PRI_MAP_PORT3m,
        IDB_OBM4_PRI_MAP_PORT4m, IDB_OBM4_PRI_MAP_PORT5m,
        IDB_OBM4_PRI_MAP_PORT6m, IDB_OBM4_PRI_MAP_PORT7m}
    };
    static const bcmdrd_fid_t obm_ob_pri_fields[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf, OFFSET2_OB_PRIORITYf,
        OFFSET3_OB_PRIORITYf, OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    bcmtm_port_map_get(unit, &port_map);
    lossless = drv_info->lossless;
    count = COUNTOF(obm_ob_pri_fields);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    mem = obm_pri_map_mem[pm_num][subp];

    for (entry_index = 0; entry_index < IDB_OBM_PRI_MAP_ENTRY_COUNT; entry_index++)
    {
        BCMTM_PT_DYN_INFO(pt_info, entry_index, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, mem, ltid, &pt_info, &ltmbuf));
        for (field_index = 0; field_index < count; field_index++) {
            priority = (lossless == TD4_X9_OBM_PARAMS_LOSSY_INDEX) ?
                                    TD4_X9_OBM_PRIORITY_LOSSY_LO :
                       (lossless == TD4_X9_OBM_PARAMS_LOSSLESS_INDEX) ?
                                    TD4_X9_OBM_PRIORITY_LOSSLESS0 :
                       (field_index == 0) ? TD4_X9_OBM_PRIORITY_LOSSLESS0 :
                                    TD4_X9_OBM_PRIORITY_LOSSY_LO;
            fid = obm_ob_pri_fields[field_index];
            fval = priority;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, mem, fid, &ltmbuf, &fval));
        }
        BCMTM_PT_DYN_INFO(pt_info, entry_index, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, mem, ltid, &pt_info, &ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_obm_buffer_config(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    uint32_t buffer_start_addr;
    uint32_t buffer_end_addr;
    int phy_port;
    int pm_num;
    int subp;
    int num_lanes;
    int pipe_num;
    static const bcmltd_sid_t obm_buffer_config_regs[TD4_X9_PBLKS_PER_PIPE] = {
        IDB_OBM0_BUFFER_CONFIGr, IDB_OBM1_BUFFER_CONFIGr,
        IDB_OBM2_BUFFER_CONFIGr, IDB_OBM3_BUFFER_CONFIGr,
        IDB_OBM4_BUFFER_CONFIGr
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    num_lanes = port_map->lport_map[lport].num_lanes;
    buffer_start_addr =
        bcm56780_a0_obm_buffer_config_settings[num_lanes][subp].buffer_start;
    buffer_end_addr =
        bcm56780_a0_obm_buffer_config_settings[num_lanes][subp].buffer_end;
    reg = obm_buffer_config_regs[pm_num];

    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    fid = BUFFER_STARTf;
    fval = buffer_start_addr;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = BUFFER_ENDf;
    fval = buffer_end_addr;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_wr_ca_ct_thresh(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t fval;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    uint32_t ct_threshold;
    int speed_encoding;
    static const bcmdrd_sid_t ca_ct_thresh[TD4_X9_PBLKS_PER_PIPE] = {
        IDB_CA0_CT_CONTROLr,IDB_CA1_CT_CONTROLr,
        IDB_CA2_CT_CONTROLr,IDB_CA3_CT_CONTROLr,
        IDB_CA4_CT_CONTROLr
    };
    static const bcmdrd_fid_t ca_ct_threshold_fields[] = {
        PORT0_CT_THRESHOLDf, PORT1_CT_THRESHOLDf,
        PORT2_CT_THRESHOLDf, PORT3_CT_THRESHOLDf,
        PORT4_CT_THRESHOLDf, PORT5_CT_THRESHOLDf,
        PORT6_CT_THRESHOLDf, PORT7_CT_THRESHOLDf
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    speed_encoding =
        bcm56780_a0_tm_speed_to_e2e_settings_class_map(port_map->lport_map[lport].max_speed);
    ct_threshold =
        bcm56780_a0_idb_e2e_settings_tbl[speed_encoding].ca_ct_threshold;
    reg = ca_ct_thresh[pm_num];

    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, ltmbuf));
    fid = ca_ct_threshold_fields[subp];
    fval = ct_threshold;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_ca_bmop_set(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t fval;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    uint32_t bmop_disable;
    int speed_encoding;
    static const bcmdrd_fid_t ca_bmop_disable_fields[] = {
        PORT0_BUBBLE_MOP_DISABLEf, PORT1_BUBBLE_MOP_DISABLEf,
        PORT2_BUBBLE_MOP_DISABLEf, PORT3_BUBBLE_MOP_DISABLEf,
        PORT4_BUBBLE_MOP_DISABLEf, PORT5_BUBBLE_MOP_DISABLEf,
        PORT6_BUBBLE_MOP_DISABLEf, PORT7_BUBBLE_MOP_DISABLEf
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    speed_encoding =
        bcm56780_a0_tm_speed_to_e2e_settings_class_map(port_map->lport_map[lport].max_speed);
    bmop_disable = bcm56780_a0_idb_e2e_settings_tbl[speed_encoding].bmop_disable[0];
    reg = IDB_CA_CONTROL_2r;
    BCMTM_PT_DYN_INFO(pt_info, pm_num, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, ltmbuf));
    fid = ca_bmop_disable_fields[subp];
    fval = bmop_disable;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, pm_num, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_wr_ca_force_saf_config(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int speed_encoding;
    int unsatisfied_threshold;
    static const bcmdrd_sid_t ca_force_saf_config_regs[TD4_X9_PBLKS_PER_PIPE] = {
        IDB_CA0_DBG_Ar, IDB_CA1_DBG_Ar,
        IDB_CA2_DBG_Ar, IDB_CA3_DBG_Ar,
        IDB_CA4_DBG_Ar
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    speed_encoding =
        bcm56780_a0_tm_speed_to_e2e_settings_class_map(port_map->lport_map[lport].max_speed);
    unsatisfied_threshold =
        bcm56780_a0_idb_e2e_settings_tbl[speed_encoding].ca_unsatisfied_threshold;
    reg = ca_force_saf_config_regs[pm_num];
    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    fid = FIELD_Cf;
    fval = unsatisfied_threshold;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_wr_ca_lag_detection_config(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int speed_encoding;
    int enter_threshold;
    int exit_threshold;
    static const bcmdrd_sid_t ca_lag_detection_config_regs[TD4_X9_PBLKS_PER_PIPE] = {
        IDB_CA0_DBG_Br, IDB_CA1_DBG_Br,
        IDB_CA2_DBG_Br, IDB_CA3_DBG_Br,
        IDB_CA4_DBG_Br
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    speed_encoding =
        bcm56780_a0_tm_speed_to_e2e_settings_class_map(port_map->lport_map[lport].max_speed);
    enter_threshold =
        bcm56780_a0_idb_e2e_settings_tbl[speed_encoding].ca_enter_lagging_threshold;
    exit_threshold =
        bcm56780_a0_idb_e2e_settings_tbl[speed_encoding].ca_exit_lagging_threshold;

    reg = ca_lag_detection_config_regs[pm_num];
    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));

    fid = FIELD_Af;
    fval = enter_threshold;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = FIELD_Bf;
    fval = exit_threshold;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_ca_buffer_config(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    uint32_t buffer_start_addr;
    uint32_t buffer_end_addr;
    int phy_port;
    int pm_num;
    int subp;
    int num_of_lanes;
    int pipe_num;
    static const bcmdrd_sid_t ca_buffer_config_regs[TD4_X9_PBLKS_PER_PIPE] = {
        IDB_CA0_BUFFER_CONFIGr, IDB_CA1_BUFFER_CONFIGr,
        IDB_CA2_BUFFER_CONFIGr, IDB_CA3_BUFFER_CONFIGr,
        IDB_CA4_BUFFER_CONFIGr
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    num_of_lanes = port_map->lport_map[lport].num_lanes;
    buffer_start_addr = subp * TD4_X9_CA_BUFFER_SIZE / TD4_X9_PORTS_PER_PBLK;
    buffer_end_addr = buffer_start_addr +
                      num_of_lanes * TD4_X9_CA_BUFFER_SIZE/TD4_X9_PORTS_PER_PBLK -1;

    reg = ca_buffer_config_regs[pm_num];
    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));

    fid = BUFFER_STARTf;
    fval = buffer_start_addr;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = BUFFER_ENDf;
    fval = buffer_end_addr;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_ca_peek_depth_cfg(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t fval;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    int peek_depth;
    int speed_encoding;
    static const bcmdrd_fid_t ca_peek_depth_fields[] = {
        PORT0_CA_PEEK_DEPTHf, PORT1_CA_PEEK_DEPTHf,
        PORT2_CA_PEEK_DEPTHf, PORT3_CA_PEEK_DEPTHf,
        PORT4_CA_PEEK_DEPTHf, PORT5_CA_PEEK_DEPTHf,
        PORT6_CA_PEEK_DEPTHf, PORT7_CA_PEEK_DEPTHf
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    speed_encoding =
        bcm56780_a0_tm_speed_to_e2e_settings_class_map(port_map->lport_map[lport].max_speed);
    peek_depth = bcm56780_a0_peek_depth_settings_tbl[speed_encoding].peek_depth;

    reg = IDB_CA_CONTROL_2r;
    BCMTM_PT_DYN_INFO(pt_info, pm_num, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, ltmbuf));
    fid = ca_peek_depth_fields[subp];
    fval = peek_depth;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, pm_num, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_obm_tdm_cfg(int unit, int pm_num, int pipe_num)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int phy_port_base;
    int num_lanes;
    int max_lanes;
    int curr_lport;
    int lane;
    int idx;
    static const bcmltd_sid_t obm_tdm_regs[TD4_X9_PBLKS_PER_PIPE] = {
        IDB_OBM0_TDMr, IDB_OBM1_TDMr,
        IDB_OBM2_TDMr, IDB_OBM3_TDMr,
        IDB_OBM4_TDMr
    };
    static const int tdm_index[TD4_X9_PORTS_PER_PBLK] = {
        0, 4, 2, 6, 1, 5, 3, 7
    };
    int lane_to_port_map[TD4_X9_PORTS_PER_PBLK];

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    /* Set the lane_to_port_map based on num_lanes. */
    for (lane = 0; lane < TD4_X9_PORTS_PER_PBLK; lane++) {
        lane_to_port_map[lane] = lane;
    }
    phy_port_base = pipe_num * TD4_X9_PBLKS_PER_PIPE * TD4_X9_PORTS_PER_PBLK +
                    pm_num * TD4_X9_PORTS_PER_PBLK + 1;
    for (lane = 0; lane < TD4_X9_PORTS_PER_PBLK; lane++) {
        curr_lport = port_map->pport_map[phy_port_base + lane].lport;
        if (!((curr_lport == -1) ||
              (port_map->lport_map[curr_lport].max_speed == 0))) {
            num_lanes = port_map->lport_map[curr_lport].num_lanes;
            max_lanes = MIN(lane + num_lanes, TD4_X9_PORTS_PER_PBLK);
            for (idx = lane; idx < max_lanes; idx++){
                lane_to_port_map[idx] = lane;
            }
        }
    }

    reg = obm_tdm_regs[pm_num];
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));

    fid = SLOT0_PORT_NUMf;
    fval = lane_to_port_map[tdm_index[0]];
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = SLOT1_PORT_NUMf;
    fval = lane_to_port_map[tdm_index[1]];
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = SLOT2_PORT_NUMf;
    fval = lane_to_port_map[tdm_index[2]];
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = SLOT3_PORT_NUMf;
    fval = lane_to_port_map[tdm_index[3]];
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = SLOT4_PORT_NUMf;
    fval = lane_to_port_map[tdm_index[4]];
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = SLOT5_PORT_NUMf;
    fval = lane_to_port_map[tdm_index[5]];
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = SLOT6_PORT_NUMf;
    fval = lane_to_port_map[tdm_index[6]];
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = SLOT7_PORT_NUMf;
    fval = lane_to_port_map[tdm_index[7]];
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/* This is a global reg, not per subport;
 * However, every mgm port setup will overwrite this
 */
static int
bcm56780_a0_mgmt_obm_shared_config(int unit, int lport)
{
    bcmtm_drv_info_t *drv_info;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int pipe_num;
    int lossless;
    int sop_discard_mode;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    pipe_num = 0;
    lossless = drv_info->lossless;
    sop_discard_mode =
        bcm56780_mgmt_obm_settings[lossless].sop_discard_mode;
    fid = SOP_DISCARD_MODEf;
    fval = sop_discard_mode;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pipe_num);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read
        (unit, MGMT_OBM_SHARED_CONFIGr, ltid, &pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT(bcmtm_field_set
        (unit, MGMT_OBM_SHARED_CONFIGr, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pipe_num);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write
        (unit, MGMT_OBM_SHARED_CONFIGr, ltid, &pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_mgmt_obm_thresh(int unit, int lport)
{
    bcmtm_drv_info_t *drv_info;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t fval;
    uint32_t lossless0_discard, lossy_discard, lossy_low_pri, discard_limit,
             lossless1_discard;
    int phy_port;
    int subp;
    int pipe_num;
    int lossless;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = 0;
    lossless = drv_info->lossless;
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    discard_limit =
        bcm56780_mgmt_obm_settings[lossless].discard_limit;
    lossless1_discard =
        bcm56780_mgmt_obm_settings[lossless].lossless_discard;
    lossless0_discard =
        bcm56780_mgmt_obm_settings[lossless].lossless_discard;
    lossy_discard =
        bcm56780_mgmt_obm_settings[lossless].lossy_discard;
    lossy_low_pri =
        bcm56780_mgmt_obm_settings[lossless].lossy_low_pri;

    BCMTM_PT_DYN_INFO(pt_dyn_info, subp, pipe_num);
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read
        (unit, MGMT_OBM_THRESHOLDr, ltid, &pt_dyn_info, ltmbuf));

    fid = LOSSLESS0_DISCARDf;
    fval = lossless0_discard;
    SHR_IF_ERR_EXIT(bcmtm_field_set
        (unit, MGMT_OBM_THRESHOLDr, fid, ltmbuf, &fval));

    fid = LOSSY_DISCARDf;
    fval = lossy_discard;
    SHR_IF_ERR_EXIT(bcmtm_field_set
        (unit, MGMT_OBM_THRESHOLDr, fid, ltmbuf, &fval));

    fid = LOSSY_LOW_PRIf;
    fval = lossy_low_pri;
    SHR_IF_ERR_EXIT(bcmtm_field_set
        (unit, MGMT_OBM_THRESHOLDr, fid, ltmbuf, &fval));

    fid = DISCARD_LIMITf;
    fval = discard_limit;
    SHR_IF_ERR_EXIT(bcmtm_field_set
        (unit, MGMT_OBM_THRESHOLDr, fid, ltmbuf, &fval));

    fid = LOSSLESS1_DISCARDf;
    fval = lossless1_discard;
    SHR_IF_ERR_EXIT(bcmtm_field_set
        (unit, MGMT_OBM_THRESHOLDr, fid, ltmbuf, &fval));

      BCMTM_PT_DYN_INFO(pt_dyn_info, subp, pipe_num);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write
        (unit, MGMT_OBM_THRESHOLDr, ltid, &pt_dyn_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_mgmt_obm_flow_ctrl_cfg (int unit, int lport)
{
    bcmtm_drv_info_t *drv_info;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t fval;
    int phy_port;
    int subp;
    int pipe_num;
    int lossless;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = 0;
    lossless = drv_info->lossless;
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);

    BCMTM_PT_DYN_INFO(pt_dyn_info, subp, pipe_num);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read
        (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, ltid, &pt_dyn_info, ltmbuf));

    if (lossless == TD4_X9_OBM_PARAMS_LOSSLESS_INDEX) {
        fid = FC_TYPEf;
        fval = 0;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));

        fid = PORT_FC_ENf;
        fval = 1;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));

        fid = LOSSLESS0_FC_ENf;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));

        fval = 0;
        fid = LOSSLESS1_FC_ENf;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));

        fval = 65535;
        fid = LOSSLESS0_PRIORITY_PROFILEf;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));

        fval = 0;
        fid = LOSSLESS1_PRIORITY_PROFILEf;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));
    } else if (lossless == TD4_X9_OBM_PARAMS_LOSSY_INDEX) {
        fid = FC_TYPEf;
        fval = 0;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));

        fid = PORT_FC_ENf;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));

        fid = LOSSLESS0_FC_ENf;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));

        fid = LOSSLESS1_FC_ENf;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));

        fid = LOSSLESS0_PRIORITY_PROFILEf;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));

        fid = LOSSLESS1_PRIORITY_PROFILEf;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));
    } else {
        fid = FC_TYPEf;
        fval = 1;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));

        fid = PORT_FC_ENf;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));

        fid = LOSSLESS0_FC_ENf;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));

        fval = 0;
        fid = LOSSLESS1_FC_ENf;
         SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));

        fval = 1;
        fid = LOSSLESS0_PRIORITY_PROFILEf;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));

        fval = 0;
        fid = LOSSLESS1_PRIORITY_PROFILEf;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, fid, ltmbuf, &fval));
    }
    BCMTM_PT_DYN_INFO(pt_dyn_info, subp, pipe_num);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write
        (unit, MGMT_OBM_FLOW_CONTROL_CONFIGr, ltid, &pt_dyn_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_mgmt_obm_fc_threshold(int unit, int lport)
{
    bcmtm_drv_info_t *drv_info;
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t fval;
    int idx;
    uint32_t lossless0_xon, lossless0_xoff, lossless1_xon, lossless1_xoff,
             port_xon, port_xoff;
   int phy_port;
    int subp;
    int lossless;
    int pipe_num;
    const bcmdrd_sid_t mgmt_obm_fc_thresh_pp[2] = {MGMT_OBM_FC_THRESHOLDr,
                                                   MGMT_OBM_FC_THRESHOLD_1r};

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = 0;
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    lossless = drv_info->lossless;
    lossless0_xon  =
        bcm56780_mgmt_obm_settings[lossless].lossless_xon;
    lossless1_xon  =
        bcm56780_mgmt_obm_settings[lossless].lossless_xon;
    lossless0_xoff =
        bcm56780_mgmt_obm_settings[lossless].lossless_xoff;
    lossless1_xoff =
        bcm56780_mgmt_obm_settings[lossless].lossless_xoff;
    port_xon       =
        bcm56780_mgmt_obm_settings[lossless].port_xon;
    port_xoff      =
        bcm56780_mgmt_obm_settings[lossless].port_xoff;

    for (idx = 0; idx < 2; idx++) {
        reg = mgmt_obm_fc_thresh_pp[idx];
        sal_memset(ltmbuf, 0, sizeof(ltmbuf));
        BCMTM_PT_DYN_INFO(pt_dyn_info, subp, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_dyn_info, ltmbuf));
        if (idx == 0) {
            fid = LOSSLESS0_XONf;
            fval = lossless0_xon;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            fid = LOSSLESS0_XOFFf;
            fval = lossless0_xoff;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            fid = LOSSLESS1_XONf;
            fval = lossless1_xon;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            fid = LOSSLESS1_XOFFf;
            fval = lossless1_xoff;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));
        }
        else {
            fid = PORT_XONf;
            fval = port_xon;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            fid = PORT_XOFFf;
            fval = port_xoff;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));
        }
        BCMTM_PT_DYN_INFO(pt_dyn_info, subp, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_dyn_info, ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_mgmt_obm_port_config(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int phy_port;
    int subp;
    int pipe_num;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = 0;
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);

    BCMTM_PT_DYN_INFO(pt_dyn_info, subp, pipe_num);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read
        (unit, MGMT_OBM_PORT_CONFIGr, ltid, &pt_dyn_info, &ltmbuf));
    fid = OUTER_TPID_ENABLEf;
    fval = 1;
    SHR_IF_ERR_EXIT(bcmtm_field_set
        (unit, MGMT_OBM_PORT_CONFIGr, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_dyn_info, subp, pipe_num);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write
        (unit, MGMT_OBM_PORT_CONFIGr, ltid, &pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_mgmt_obm_monitor_stats_config(int unit, int lport)
{
    bcmtm_drv_info_t *drv_info;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int phy_port;
    int subp;
    int pipe_num;
    int lossless;
    int drop_count_select;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = 0;
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    lossless = drv_info->lossless;
    if (lossless == TD4_X9_OBM_PARAMS_LOSSLESS_INDEX){
        drop_count_select = TD4_X9_OBM_PARAMS_DROP_COUNT_SELECT_LOSSLESS0;
    } else {
        drop_count_select = TD4_X9_OBM_PARAMS_DROP_COUNT_SELECT_LOSSY_LO;
    }
    BCMTM_PT_DYN_INFO(pt_dyn_info, subp, pipe_num);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read
        (unit, MGMT_OBM_MONITOR_STATS_CONFIGr, ltid, &pt_dyn_info, &ltmbuf));
    fid = DROP_COUNT_SELECTf;
    fval = drop_count_select;
    SHR_IF_ERR_EXIT(bcmtm_field_set
        (unit, MGMT_OBM_MONITOR_STATS_CONFIGr, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write
        (unit, MGMT_OBM_MONITOR_STATS_CONFIGr, ltid, &pt_dyn_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
    return 0;
}
static int
bcm56780_a0_mgmt_obm_pri_map_set(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t mem;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int pipe_num;
    int subp;
    int phy_port;
    int entry_index;
    int field_index;
    int count;
    uint32_t priority;
    static const bcmdrd_sid_t mgmt_obm_pri_map_mem[2] =
        {MGMT_OBM_PRI_MAP_PORT0m, MGMT_OBM_PRI_MAP_PORT1m};
    static const bcmdrd_fid_t mgmt_obm_ob_pri_fields[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf, OFFSET2_OB_PRIORITYf,
        OFFSET3_OB_PRIORITYf, OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    count = COUNTOF(mgmt_obm_ob_pri_fields);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = 0;
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    mem = mgmt_obm_pri_map_mem[subp];

    for (entry_index = 0; entry_index < MGMT_OBM_PRI_MAP_ENTRY_COUNT; entry_index++)
    {
        BCMTM_PT_DYN_INFO(pt_dyn_info, entry_index, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, mem, ltid, &pt_dyn_info, &ltmbuf));
        for (field_index = 0; field_index < count; field_index++) {
            priority = TD4_X9_OBM_PRIORITY_LOSSY_LO;
            fid = mgmt_obm_ob_pri_fields[field_index];
            fval = priority;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, mem, fid, &ltmbuf, &fval));
        }
        BCMTM_PT_DYN_INFO(pt_dyn_info, entry_index, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, mem, ltid, &pt_dyn_info, &ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_mgmt_obm_buffer_config(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    uint32_t buffer_start_addr;
    uint32_t buffer_end_addr;
    int phy_port;
    int subp;
    int pipe_num;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = 0;
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);

    buffer_start_addr =
        bcm56780_mgmt_obm_buffer_config_settings[subp].buffer_start;
    buffer_end_addr =
        bcm56780_mgmt_obm_buffer_config_settings[subp].buffer_end;

    BCMTM_PT_DYN_INFO(pt_dyn_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, MGMT_OBM_BUFFER_CONFIGr, ltid, &pt_dyn_info, &ltmbuf));
    fid = BUFFER_STARTf;
    fval = buffer_start_addr;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, MGMT_OBM_BUFFER_CONFIGr, fid, &ltmbuf, &fval));

    fid = BUFFER_ENDf;
    fval = buffer_end_addr;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, MGMT_OBM_BUFFER_CONFIGr, fid, &ltmbuf, &fval));

    BCMTM_PT_DYN_INFO(pt_dyn_info, subp, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, MGMT_OBM_BUFFER_CONFIGr, ltid, &pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_mgmt_obm_tdm_cfg(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int slot0_port_num;
    int slot1_port_num;
    int phy_port;
    int subp;
    int pipe_num;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = 0;
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pipe_num);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read
        (unit, MGMT_OBM_TDMr, ltid, &pt_dyn_info, &ltmbuf));

    if (subp == 0) {
        slot0_port_num = 0;
        /* if 40G (4 lanes) port then both slots go to subport 0 */
        slot1_port_num = ((port_map->lport_map[lport].cur_speed > 0) &&
                  (port_map->lport_map[lport].cur_speed <= 10000)) ? 1 : 0;

        fid = SLOT0_PORT_NUMf;
        fval = slot0_port_num;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_TDMr, fid, &ltmbuf, &fval));

        fid = SLOT1_PORT_NUMf;
        fval = slot1_port_num;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_TDMr, fid, &ltmbuf, &fval));
    } else {
        slot1_port_num = ((port_map->lport_map[lport].cur_speed > 0) &&
                  (port_map->lport_map[lport].cur_speed <= 10000)) ? 1 : 0;
        fid = SLOT1_PORT_NUMf;
        fval = slot1_port_num;
        SHR_IF_ERR_EXIT(bcmtm_field_set
            (unit, MGMT_OBM_TDMr, fid, &ltmbuf, &fval));
    }

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pipe_num);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write
        (unit, MGMT_OBM_TDMr, ltid, &pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_mgmt_obm_reset(int unit, int lport, int reset)
{
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int phy_port;
    int subp;
    int pipe_num;
    static const bcmdrd_fid_t mgmt_obm_reset_fields[] = {
        PORT0_RESETf, PORT1_RESETf
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = 0;
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pipe_num);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read
        (unit, MGMT_OBM_CONTROLr, ltid, &pt_dyn_info, &ltmbuf));

    fid = mgmt_obm_reset_fields[subp];
    fval = reset;
    SHR_IF_ERR_EXIT(bcmtm_field_set
        (unit, MGMT_OBM_CONTROLr, fid, &ltmbuf, &fval));

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pipe_num);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write
        (unit, MGMT_OBM_CONTROLr, ltid, &pt_dyn_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_pa_reset(int unit, int pm_num, int pipe_num, int reset)
{
    bcmtm_drv_info_t *drv_info;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    static const bcmdrd_fid_t idb_pa_reset_fields[] = {
        PM0_PA_RST_Lf, PM1_PA_RST_Lf, PM2_PA_RST_Lf,
        PM3_PA_RST_Lf, PM4_PA_RST_Lf
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    reg = IDB_PA_RESET_CONTROLr;
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    fid = idb_pa_reset_fields[pm_num];
    fval = reset ? 0 : 1;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_config_pnum_mapping_tbl(int unit, int lport, int valid)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t mem;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int pipe, phy_port, idb_port;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    idb_port = port_map->lport_map[lport].mlocal_port;
    pipe = port_map->lport_map[lport].pipe;
    mem = ING_PHY_TO_IDB_PORT_MAPm;
    BCMTM_PT_DYN_INFO(pt_info,
                     (phy_port - 1 - (pipe * TD4_X9_PHYS_PORTS_PER_PIPE)), pipe);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, mem, ltid, &pt_info, &ltmbuf));
    fid = IDB_PORTf;
    fval = (valid) ? idb_port : 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, mem, fid, &ltmbuf, &fval));
    fid = VALIDf;
    fval = (valid) ? 1 : 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, mem, fid, &ltmbuf, &fval));

    BCMTM_PT_DYN_INFO(pt_info,
                     (phy_port - 1 - (pipe * TD4_X9_PHYS_PORTS_PER_PIPE)), pipe);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, mem, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_config_idb_to_dev_pmap_tbl(int unit, int lport, int valid)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t mem;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int pipe, idb_port;
    int local_dev_port;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    idb_port = port_map->lport_map[lport].mlocal_port;
    pipe = port_map->lport_map[lport].pipe;
    local_dev_port = lport % TD4_X9_DEV_PORTS_PER_PIPE;

    mem = ING_IDB_TO_DEVICE_PORT_MAPm;
    BCMTM_PT_DYN_INFO(pt_info, idb_port, pipe);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, mem, ltid, &pt_info, &ltmbuf));
    fid = DEVICE_PORTf;
    fval = (valid) ? local_dev_port : 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, mem, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, idb_port, pipe);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, mem, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_force_saf_duration_timer_cfg(int unit, int pipe_num)
{
    bcmtm_drv_info_t *drv_info;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int core_clock_freq;
    int duration_timer;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    core_clock_freq = drv_info->frequency;
    duration_timer = 2 * core_clock_freq;

    reg = IDB_DBG_Br;
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    fid = FIELD_Af;
    fval = duration_timer/2;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
   /* SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));*/
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_sch_set_cal_config_per_port(int unit,
                                    idb_sch_inst_name_e sched_inst_name,
                                    int lport, int is_port_down)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t fval;
    uint32_t fval_64[BCMTM_MAX_ENTRY_WSIZE] = {0};
    int phy_port;
    int pm_num;
    int pipe_num;
    int subp;
    int num_50g_slots;
    int is_special_slot;
    int lane, slot;
    int port_sched_table[TD4_X9_TDM_LENGTH];
    int pm_mapping[TD4_X9_PORTS_PER_PBLK] = {0,4,2,6,1,5,3,7};
    uint32_t this_port_cal_slot_bitmap[2];
    int this_port_number;
    int this_slot_valid;
    int this_port_speed;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    num_50g_slots =
        (port_map->lport_map[lport].max_speed + TDM_SPEED_40G) / TDM_SPEED_50G;

    /*
     * Special port detection:
     * Port is not front panel port (local IDB port number >= 18).
     */
    is_special_slot =
        (port_map->lport_map[lport].mlocal_port >= TD4_X9_FP_PORTS_PER_PIPE) ? 1 : 0;
    for (slot = 0; slot < TD4_X9_TDM_LENGTH; slot++) {
        port_sched_table[slot] = 0;
    }
    if (!is_special_slot)
    {
        for (lane = subp; lane < subp + num_50g_slots; lane++)
        {
            slot = pm_mapping[lane] * TD4_X9_PBLKS_PER_PIPE + pm_num;
            port_sched_table[slot] = 1;
        }
    }
    else {
        port_sched_table[TD4_X9_TDM_LENGTH - 1] = 1;
    }

    this_port_number = (is_port_down == 1) ? 0 :
        ((is_special_slot == 1) ? TD4_X9_TDM_SCHED_SPECIAL_SLOT :
        port_map->lport_map[lport].mlocal_port);

    this_slot_valid  = (is_port_down == 1) ? 0 : 1;

    if (is_port_down == 1) {
        this_port_speed = 0;
    } else if (is_special_slot == 1) {
        this_port_speed = 3; /* Speed 50G for Aux ports */
        /* Special slot in pipe 0 is shared between cpu and RDB port.
         * programming max speed for the slot as 400G.
         */
        if (bcmtm_lport_is_rdb(unit, lport) || pipe_num == 0 ) {
            this_port_speed = 0; /* For RDB ports */
        }
    } else {
        switch (port_map->lport_map[lport].max_speed) {
            case TDM_SPEED_400G:
                this_port_speed = 0;
                break;
            case TDM_SPEED_200G:
                this_port_speed = 1;
                break;
            case TDM_SPEED_100G:
                this_port_speed = 2;
                break;
            case TDM_SPEED_50G:
                this_port_speed = 3;
                break;
            default: /* Speed < 50G */
                this_port_speed = 3;
                break;
        }
    }

    SHR_BITCLR_RANGE(this_port_cal_slot_bitmap, 0, 64);
    if (is_port_down == 0) {
        for (slot = 0; slot < TD4_X9_TDM_LENGTH; slot++) {
            if (port_sched_table[slot]==1) {
                SHR_BITSET(this_port_cal_slot_bitmap, slot);
            }
        }
    }

    /* Select the right reg based on sched instance. */
    switch (sched_inst_name) {
    case TDM_IDB_PORT_SCHED:
        reg = IS_CAL_CONFIGr;
        break;
    case TDM_IDB_PKT_SCHED:
        reg = IS_PKSCH_CAL_CONFIGr;
        break;
    default:
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "scheduler instance name is not supported %d\n"),
                   sched_inst_name));
        return SHR_E_PARAM;
        break;
    }
    for (slot = 0; slot < TD4_X9_TDM_LENGTH; slot++) {
        if (port_sched_table[slot] == 1) {
            BCMTM_PT_DYN_INFO(pt_info, slot, pipe_num);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, ltmbuf));

            fid = THIS_PORT_NUMBERf;
            fval = this_port_number;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            fid = THIS_PORT_CAL_SLOT_BITMAPf;
            fval_64[0] = this_port_cal_slot_bitmap[0];
            fval_64[1] = this_port_cal_slot_bitmap[1];
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, fval_64));

            fid = THIS_SLOT_VALIDf;
            fval = this_slot_valid;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            fid = THIS_PORT_SPEEDf;
            fval = this_port_speed;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));

            BCMTM_PT_DYN_INFO(pt_info, slot, pipe_num);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, ltmbuf));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_sch_set_max_spacing(int unit, int pipe_num)
{
    int max_spacing_all_speeds;
    int max_spacing_special_slot;
    bcmtm_drv_info_t *drv_info;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    int freq_idx;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    reg = IS_MAX_SPACINGr;

    freq_idx = (drv_info->frequency == 950)? CLK_950MHZ:
               (drv_info->frequency == 1175)? CLK_1175MHZ:
               (drv_info->frequency == 1025)? CLK_1025MHZ: CLK_1350MHZ;

    max_spacing_all_speeds =
        bcm56780_a0_max_spacing_settings_tbl[freq_idx].all_speed;

    max_spacing_special_slot = TDM_LENGTH;
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    fid = MAX_SPACING_ALL_SPEEDSf;
    fval = max_spacing_all_speeds;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    fid = MAX_SPACING_SPECIAL_SLOTf;
    fval = max_spacing_special_slot;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_sch_set_feature_ctrl(int unit, int pipe_num)
{
    int disable_max_spacing;
    int enable_bubble_mop;
    bcmtm_drv_info_t *drv_info;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    reg = IS_FEATURE_CTRLr;
    disable_max_spacing = 0;
    enable_bubble_mop = 1;
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));

    fid = DISABLE_MAX_SPACINGf;
    fval = disable_max_spacing;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = ENABLE_BUBBLE_MOP_400Gf;
    fval = enable_bubble_mop;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = ENABLE_BUBBLE_MOP_200Gf;
    fval = enable_bubble_mop;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = ENABLE_BUBBLE_MOP_100Gf;
    fval = enable_bubble_mop;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = ENABLE_BUBBLE_MOP_50Gf;
    fval = enable_bubble_mop;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_sch_set_cbmg_value(int unit, int pipe_num)
{
    int terminal_counter_value, mmu_null_slot_counter_value;
    int null_bw;
    bcmtm_drv_info_t *drv_info;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    reg =  IS_CBMG_VALUEr;
    /* TD4_X9 only even pipe has null slot generation. */
    if (pipe_num % 2 == 0) {
        /*
         * TD4_X9 bandwidth requirements are roughly as follows:
         * For minimum guarantee of 8.1G(IP) & 3.4(EP) computed at 64B packets,
         * Null/IDLE slots should be picked with an average TDM of once every
         * configurable terminal_counter_value cycles.
         * CBMG_value = [(64+20)*8] * CoreFreq(MHz) / BW(Mbps)
         * IP: IFP Refresh 8.2MHz + 3.8MHz SBUS = 12 MHz total
         * EP: 3.8Mhz SBUS + 1.2MHz TCAM parity scan = 5MHz total
         * Simplified formula:
         * CBMG_value = FLOOR[ CoreFreq(MHz) / NullRate(MHz) ]
         */
        null_bw = 12;
        terminal_counter_value = drv_info->frequency / null_bw -1;
        /*
         * TD4_X9 update: NULL slot value default to 0. Recommended value is 5000.
         * MMU EBPTS Null slot value should be 0 if under PP bypass mode.
         * mmu_null_slot_counter_value = (bypass_mode == 0) ? normal_val : bypass_val;
         */
        mmu_null_slot_counter_value = 5000;
    } else {
        terminal_counter_value = 0;
        mmu_null_slot_counter_value = 5000;
    }
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    fid = TERMINAL_COUNTER_VALUEf;
    fval = terminal_counter_value;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    fid = MMU_NULL_SLOT_COUNTER_VALUEf;
    fval = mmu_null_slot_counter_value;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_sch_set_urg_cell_spacing(int unit, int pipe_num)
{
    bcmtm_drv_info_t *drv_info;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    bcm56780_a0_urg_pick_spacing_settings_t urg_pick_space;
    int freq_idx;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    reg =  IS_URG_CELL_SPACINGr;

    freq_idx = (drv_info->frequency == 950) ? CLK_950MHZ:
               (drv_info->frequency == 1175) ? CLK_1175MHZ:
               (drv_info->frequency == 1025) ? CLK_1025MHZ: CLK_1350MHZ;
    urg_pick_space =
        bcm56780_a0_urg_pick_spacing_settings_tbl[freq_idx];

    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));

    fid = URG_PICK_400G_SPACINGf;
    fval = urg_pick_space.spacing_400;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = URG_PICK_200G_SPACINGf;
    fval = urg_pick_space.spacing_200;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = URG_PICK_100G_SPACINGf;
    fval = urg_pick_space.spacing_100;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = URG_PICK_50G_SPACINGf;
    fval = urg_pick_space.spacing_50;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}


static int
bcm56780_a0_tm_idb_sch_set_min_spacing(int unit, int pipe_num)
{
    bcmtm_drv_info_t *drv_info;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;
    bcm56780_a0_min_spacing_settings_t *min_spacing;
    int clk_code, speed_code;

    static const bcmdrd_fid_t any_cell_spacing_fields[] = {
        ANY_CELL_50G_MIN_SPACINGf,  ANY_CELL_100G_MIN_SPACINGf,
        ANY_CELL_200G_MIN_SPACINGf, ANY_CELL_400G_MIN_SPACINGf
    };

    static const bcmdrd_fid_t inter_pkt_spacing_fields[] = {
        INTER_PKT_50G_MIN_SPACINGf,  INTER_PKT_100G_MIN_SPACINGf,
        INTER_PKT_200G_MIN_SPACINGf, INTER_PKT_400G_MIN_SPACINGf
    };

    static const bcmdrd_fid_t mid_pkt_spacing_fields[] = {
        MID_PKT_50G_MIN_SPACINGf,  MID_PKT_100G_MIN_SPACINGf,
        MID_PKT_200G_MIN_SPACINGf, MID_PKT_400G_MIN_SPACINGf
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    clk_code = (drv_info->frequency ==  950)? CLK_950MHZ:
               (drv_info->frequency == 1175)? CLK_1175MHZ:
               (drv_info->frequency == 1025)? CLK_1025MHZ: CLK_1350MHZ;


    min_spacing =(bcm56780_a0_min_spacing_settings_t *)
                 (&bcm56780_a0_min_spacing_settings_tbl[clk_code][0]);

    /* ANY_CELL_XXX_MIN_SPACINGf */
    reg = IS_MIN_CELL_SPACINGr;
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    for (speed_code = 0; speed_code < 4; speed_code++) {
        fid = any_cell_spacing_fields[speed_code];
        fval = min_spacing[speed_code].any_cell;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    }
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));

    /* INTER_PKT_XXX_MIN_SPACINGf */
    reg = IS_MIN_CELL_SPACING_EOP_TO_SOPr;
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    for (speed_code = 0; speed_code < 4; speed_code++) {
        fid = inter_pkt_spacing_fields[speed_code];
        fval = min_spacing[speed_code].inter_pkt;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    }
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));

    /* MID_PKT_XXX_MIN_SPACINGf */
    reg = IS_MIN_PORT_PICK_SPACING_WITHIN_PKTr;
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    for (speed_code = 0; speed_code < 4; speed_code++) {
        fid = mid_pkt_spacing_fields[speed_code];
        fval = min_spacing[speed_code].mid_pkt;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    }
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_set_pksch_pkt_credits(int unit, int pipe_num)
{
    int pkt_credit_count;
    int pkt_credit_50g;
    int pkt_credit_100g;
    int pkt_credit_200g;
    int pkt_credit_400g;
    bcmtm_drv_info_t *drv_info;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    /* TD4_X9 recommended */
    pkt_credit_count = 26;
    reg = IS_PKSCH_PKT_CREDITS_PER_PIPEr;
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    fid = PKT_CREDIT_COUNTf;
    fval = pkt_credit_count;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));

    reg = IS_PKSCH_PKT_CREDITS_PER_PORTr;
    pkt_credit_50g = 1;
    pkt_credit_100g = 2;
    pkt_credit_200g = 3;
    pkt_credit_400g = 4;
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    fid = PKT_CREDITS_FOR_50Gf;
    fval = pkt_credit_50g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = PKT_CREDITS_FOR_100Gf;
    fval = pkt_credit_100g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = PKT_CREDITS_FOR_200Gf;
    fval = pkt_credit_200g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));

    fid = PKT_CREDITS_FOR_400Gf;
    fval = pkt_credit_400g;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_sch_cpu_mgmt_lb_cfg(int unit, int pipe_num)
{
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    reg = IS_PKSCH_CPU_MGMT_LB_RATIOSr;
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    fid = LB_Q_VALUEf;
    fval = 7;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    fid = CPU_OR_MGMT_P_VALUEf;
    fval = 1;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();

}

static int
bcm56780_a0_tm_idb_lpbk_ca_peek_depth_cfg(int unit, int pipe_num)
{
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    reg = IDB_CA_LPBK_CONTROLr;
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    fid = PEEK_DEPTHf;

    fval = (TD4_X9_PIPE_HAS_RDB(pipe_num)) ? 6 : 2;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_cpu_ca_peek_depth_cfg(int unit, int pipe_num)
{
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    reg = IDB_CA_CPU_CONTROLr;
    BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
    fid = PEEK_DEPTHf;
    fval = 2;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, reg, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_obm_poll_buffer_empty(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t total_count;
    int phy_port;
    int pipe_num;
    int pm_num;
    int subp;
    int timeout_in_usec;
    int timeout_count;
    bool psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    bool asim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    const bcmdrd_sid_t obm_usage_1_regs[TD4_X9_PBLKS_PER_PIPE] = {
        IDB_OBM0_USAGE_1r, IDB_OBM1_USAGE_1r,
        IDB_OBM2_USAGE_1r, IDB_OBM3_USAGE_1r,
        IDB_OBM4_USAGE_1r
    };

    SHR_FUNC_ENTER(unit);

    /* Skip the function if it is simulation. */
    if (psim || asim) {
        return SHR_E_NONE;
    }

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    /* Timeout max 25 usec */
    timeout_in_usec = 25;
    timeout_count = 0;
    reg = obm_usage_1_regs[pm_num];
    do {
        BCMTM_PT_DYN_INFO(pt_info, subp, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, ltmbuf));
        fid = TOTAL_COUNTf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, reg, fid, ltmbuf, &total_count));
        if (total_count == 0) {
            break;
        }

        sal_usleep(1 + (emul ? 1 : 0) * EMULATION_FACTOR);

        timeout_count++;
        if (timeout_count > timeout_in_usec) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "OBM Polling timeout for lport %0d pport %0d "
                                  "pipe %0d pm %0d subp %0d\n"),
                       lport, phy_port, pipe_num, pm_num, subp));
        }
    } while (total_count > 0);
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_ca_poll_buffer_empty(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t ca_fifo_empty;
    int phy_port;
    int pipe_num;
    int pm_num;
    int subp;
    int timeout_in_usec;
    int timeout_count;
    bool psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    bool asim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    static const bcmdrd_sid_t ca_status_2_regs[TD4_X9_PBLKS_PER_PIPE] = {
        IDB_CA0_HW_STATUS_2r, IDB_CA1_HW_STATUS_2r,
        IDB_CA2_HW_STATUS_2r, IDB_CA3_HW_STATUS_2r,
        IDB_CA4_HW_STATUS_2r};
    static const bcmdrd_fid_t ca_status_2_regs_fifo_empty_fields[] = {
        FIFO_EMPTY_PORT0f, FIFO_EMPTY_PORT1f,
        FIFO_EMPTY_PORT2f, FIFO_EMPTY_PORT3f,
        FIFO_EMPTY_PORT4f, FIFO_EMPTY_PORT5f,
        FIFO_EMPTY_PORT6f, FIFO_EMPTY_PORT7f
    };

    SHR_FUNC_ENTER(unit);

    /* Skip the function if it is simulation. */
    if (psim || asim) {
        return SHR_E_NONE;
    }

    bcmtm_port_map_get(unit, &port_map);
    phy_port = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(phy_port);
    subp = bcm56780_a0_tm_get_subp_from_phy_port(phy_port);
    /* Timeout max 25 usec */
    timeout_in_usec = 25;
    timeout_count = 0;
    reg = ca_status_2_regs[pm_num];
    if (bcmtm_lport_is_cpu(unit, lport)  ||
               bcmtm_lport_is_mgmt(unit, lport) ||
               bcmtm_lport_is_lb(unit, lport)) {
        reg = IDB_CA_CPU_HW_STATUSr;
    }

    do {
        BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
        if (bcmtm_lport_is_cpu(unit, lport)  ||
               bcmtm_lport_is_mgmt(unit, lport) ||
               bcmtm_lport_is_lb(unit, lport)) {
            fid = FIFO_EMPTYf;
        } else {
            fid = ca_status_2_regs_fifo_empty_fields[subp];
        }
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, reg, fid, &ltmbuf, &ca_fifo_empty));
        if (ca_fifo_empty) {
            break;
        }

        sal_usleep(1 + (emul ? 1 : 0) * EMULATION_FACTOR);

        timeout_count++;
        if (timeout_count > timeout_in_usec) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "CA Polling timeout for lport %0d pipe %0d pm"
                                  " %0d subp %0d\n"),
                       lport, pipe_num, pm_num, subp));
        }
    } while (ca_fifo_empty == 0);
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_ca_lpbk_poll_buffer_empty(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t ca_fifo_empty;
    int pipe_num;
    int timeout_in_usec;
    int timeout_count;
    bool psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    bool asim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    SHR_FUNC_ENTER(unit);

    /* Skip the function if it is simulation. */
    if (psim || asim) {
        return SHR_E_NONE;
    }

    bcmtm_port_map_get(unit, &port_map);
    pipe_num = port_map->lport_map[lport].pipe;
    reg = IDB_CA_LPBK_HW_STATUSr;
    /* Timeout max 25 usec */
    timeout_in_usec = 25;
    timeout_count = 0;
    do {
        BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
        fid = FIFO_EMPTYf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, reg, fid, &ltmbuf, &ca_fifo_empty));
        if (ca_fifo_empty) {
            break;
        }

        sal_usleep(1 + (emul ? 1 : 0) * EMULATION_FACTOR);

        timeout_count++;
        if (timeout_count > timeout_in_usec) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LPBK CA Polling timeout for lport %0d pipe "
                                  "%0d\n"),
                       lport, pipe_num));
        }
    } while (!ca_fifo_empty);
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_ca_cpu_poll_buffer_empty(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0;
    uint32_t ca_fifo_empty;
    int pipe_num;
    int timeout_in_usec;
    int timeout_count;
    bool psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    bool asim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    SHR_FUNC_ENTER(unit);

    /* Skip the function if it is simulation. */
    if (psim || asim) {
        return SHR_E_NONE;
    }

    bcmtm_port_map_get(unit, &port_map);
    pipe_num = port_map->lport_map[lport].pipe;
    reg = IDB_CA_CPU_HW_STATUSr;
    /* Timeout max 25 usec */
    timeout_in_usec = 25;
    timeout_count = 0;
    do {
        BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, &ltmbuf));
        fid = FIFO_EMPTYf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, reg, fid, &ltmbuf, &ca_fifo_empty));
        if (ca_fifo_empty) {
            break;
        }

        sal_usleep(1 + (emul ? 1 : 0) * EMULATION_FACTOR);

        timeout_count++;
        if (timeout_count > timeout_in_usec) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "CPU CA Polling timeout for lport %0d pipe %0d\n"),
                       lport, pipe_num));
        }
    } while (!ca_fifo_empty);
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_reset_idb(int unit, int lport, int reset_value)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg;
    bcmdrd_sid_t ca_ctrl_reg;
    bcmdrd_sid_t ca_ctrl_reg_1;
    bcmdrd_sid_t obm_ctrl_reg;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t ltmbuf0 = 0;
    uint32_t ltmbuf1 = 0;
    uint32_t fval;
    int pipe_num;
    int pm_num;
    int pport;
    int subp;
    static const bcmltd_sid_t ca_control_regs[TD4_X9_PBLKS_PER_PIPE] = {
        IDB_CA0_CONTROLr, IDB_CA1_CONTROLr,
        IDB_CA2_CONTROLr, IDB_CA3_CONTROLr,
        IDB_CA4_CONTROLr
    };
    static const bcmdrd_sid_t obm_ctrl_regs[TD4_X9_PBLKS_PER_PIPE] = {
        IDB_OBM0_CONTROLr, IDB_OBM1_CONTROLr,
        IDB_OBM2_CONTROLr, IDB_OBM3_CONTROLr,
        IDB_OBM4_CONTROLr,
    };
    static const bcmdrd_fid_t idb_reg_reset_fields[] = {
        PORT0_RESETf, PORT1_RESETf, PORT2_RESETf, PORT3_RESETf,
        PORT4_RESETf, PORT5_RESETf, PORT6_RESETf, PORT7_RESETf
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    /* get the port macro number for the port. */
    pport = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;
    pm_num = bcm56780_a0_tm_get_pipe_pm_from_phy_port(pport);

    /* port is front panel port. */
    if (bcmtm_lport_is_fp(unit, lport)) {
        ca_ctrl_reg = ca_control_regs[pm_num];
        obm_ctrl_reg = obm_ctrl_regs[pm_num];
        ca_ctrl_reg_1 = IDB_CA_CONTROL_1r;

        BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
        ltmbuf0 = 0;
        ltmbuf1 = 0;
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, ca_ctrl_reg, ltid, &pt_info,
                                   &ltmbuf0));
        BCMTM_PT_DYN_INFO(pt_info, pm_num, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, ca_ctrl_reg_1, ltid, &pt_info,
                                   &ltmbuf1));
        BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, obm_ctrl_reg, ltid, &pt_info,
                                   ltmbuf));

        subp = bcm56780_a0_tm_get_subp_from_phy_port(pport);
        fid = idb_reg_reset_fields[subp];
        fval = reset_value;
        /*
         * CA wr domain reset.
         */
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, ca_ctrl_reg, fid, &ltmbuf0, &fval));
        /*
         * CA rd domain reset.
         */
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, ca_ctrl_reg_1, fid, &ltmbuf1, &fval));
        /*
         * OBM reset.
         */
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, obm_ctrl_reg, fid, ltmbuf, &fval));
        if (reset_value == 0){
            fid = OBM_RESETf;
            fval = 0;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, obm_ctrl_reg, fid, ltmbuf, &fval));
            BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, ca_ctrl_reg, ltid,
                                        &pt_info, &ltmbuf0));
            BCMTM_PT_DYN_INFO(pt_info, pm_num, pipe_num);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, ca_ctrl_reg_1, ltid,
                                        &pt_info, &ltmbuf1));
            BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, obm_ctrl_reg, ltid,
                                        &pt_info, ltmbuf));
            SHR_IF_ERR_EXIT
                (bcm56780_a0_tm_idb_pa_reset(unit, pm_num, pipe_num, 0));
            SHR_IF_ERR_EXIT
                (bcm56780_a0_tm_idb_obm_tdm_cfg(unit, pm_num, pipe_num));
        } else {
            BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, obm_ctrl_reg, ltid,
                                        &pt_info, ltmbuf));
            BCMTM_PT_DYN_INFO(pt_info, pm_num, pipe_num);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, ca_ctrl_reg_1, ltid,
                                        &pt_info, &ltmbuf1));
            BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, ca_ctrl_reg, ltid,
                                        &pt_info, &ltmbuf0));
        }
    } else if (bcmtm_lport_is_lb(unit, lport) ||
               bcmtm_lport_is_rdb(unit, lport)) {
        /*
         * Reset LPBK CA and RDB CA.
         */
        reg = IDB_CA_LPBK_CONTROLr;
        BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, ltmbuf));
        fid = PORT_RESETf;
        fval = reset_value;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));
        BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, ltmbuf));
    } else if (bcmtm_lport_is_cpu(unit, lport)) {
        /*
         * Reset CPU CA.
         */
        reg = IDB_CA_CPU_CONTROLr;
        BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, ltmbuf));
        fid = PORT_RESETf;
        fval = reset_value;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));
        BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, ltmbuf));
    } else if (bcmtm_lport_is_mgmt(unit, lport)) {
        /*
         * Reset MGMT CA.
         */
        if (bcmtm_lport_is_mgmt(unit, lport)){
            SHR_IF_ERR_EXIT(bcm56780_mgmt_obm_tdm_cfg(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_mgmt_obm_reset(unit, lport,reset_value));
        }

        reg = IDB_CA_CPU_CONTROLr;
        BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, reg, ltid, &pt_info, ltmbuf));
        fid = PORT_RESETf;
        fval = reset_value;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, reg, fid, ltmbuf, &fval));
        BCMTM_PT_DYN_INFO(pt_info, 0, pipe_num);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, reg, ltid, &pt_info, ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_idb_general_init(int unit, int pipe_num)
{
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);

    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_idb_sch_set_max_spacing(unit,pipe_num));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_idb_sch_set_feature_ctrl(unit,pipe_num));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_idb_sch_set_cbmg_value(unit,pipe_num));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_idb_sch_set_urg_cell_spacing(unit,pipe_num));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_idb_sch_set_min_spacing(unit,pipe_num));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_idb_set_pksch_pkt_credits(unit,pipe_num));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_idb_force_saf_duration_timer_cfg(unit,pipe_num));
    if (TD4_X9_PIPE_HAS_CPU_MGMT(pipe_num)) {
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_idb_cpu_ca_peek_depth_cfg(unit,pipe_num));
    }
    if (TD4_X9_PIPE_HAS_LPBK(pipe_num)) {
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_idb_lpbk_ca_peek_depth_cfg(unit,pipe_num));
    }
    if (TD4_X9_PIPE_HAS_RDB(pipe_num)) {
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_idb_lpbk_ca_peek_depth_cfg(unit,pipe_num));
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_idb_sch_cpu_mgmt_lb_cfg(unit, pipe_num));
    }


exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * END SDKLT-DV SHARED CODE
 */

/*******************************************************************************
 * BCMTM INTERNAL PUBLIC FUNCTIONS
 */
int
bcm56780_a0_tm_port_idb_down(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    lport = port_map->pport_map[pport].lport;
    if (lport != -1) {
        /* Poll until IDB buffers are empty */
        if (bcmtm_lport_is_fp(unit,lport)) {
            SHR_IF_ERR_EXIT
                (bcm56780_a0_tm_idb_obm_poll_buffer_empty(unit,lport));
            SHR_IF_ERR_EXIT
                (bcm56780_a0_tm_idb_ca_poll_buffer_empty(unit,lport));
        }
        else if (bcmtm_lport_is_lb(unit,lport)) {
            SHR_IF_ERR_EXIT
                (bcm56780_a0_tm_idb_ca_lpbk_poll_buffer_empty(unit,lport));
        }
        else if (bcmtm_lport_is_cpu(unit,lport)||
                bcmtm_lport_is_mgmt(unit,lport)) {
            SHR_IF_ERR_EXIT
                (bcm56780_a0_tm_idb_ca_cpu_poll_buffer_empty(unit,lport));
        }
        /* Hold IDB buffers in reset state */
        bcm56780_a0_tm_reset_idb(unit, lport, 1);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_tm_port_idb_delete(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    lport = port_map->pport_map[pport].lport;
    if (lport != -1 && bcmtm_lport_is_fp(unit, lport)) {
        SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_config_pnum_mapping_tbl(unit,lport,0));
        SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_config_idb_to_dev_pmap_tbl(unit,lport,0));
        SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_sch_set_cal_config_per_port(
            unit,TDM_IDB_PORT_SCHED,lport,1));
        SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_sch_set_cal_config_per_port(
            unit,TDM_IDB_PKT_SCHED,lport,1));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_tm_port_idb_up(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    lport = port_map->pport_map[pport].lport;
    if (lport != -1) {
        /* Release IDB buffers from reset */
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_reset_idb(unit, lport, 0));
        if (bcmtm_lport_is_fp(unit, lport)) {
            SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_idb_obm_port_type(unit, lport));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_tm_port_idb_add(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    lport = port_map->pport_map[pport].lport;
    if (lport != -1) {
        /* Common port init for FP/ANXL port */
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_idb_config_idb_to_dev_pmap_tbl(unit, lport, 1));
        /* Scheduler port init */
        SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_sch_set_cal_config_per_port(
                    unit,TDM_IDB_PORT_SCHED,lport,0));
        SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_sch_set_cal_config_per_port(
                    unit,TDM_IDB_PKT_SCHED,lport,0));

        /* FP port init */
        if (bcmtm_lport_is_fp(unit,lport)) {
            /* Port mapping init */
            SHR_IF_ERR_EXIT(
                bcm56780_a0_tm_idb_config_pnum_mapping_tbl(unit, lport,1));

            /* OBM port init */
            SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_wr_obm_shared_config(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_wr_obm_thresh(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_wr_obm_flow_ctrl_cfg(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_wr_obm_fc_threshold(unit ,lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_wr_obm_ct_thresh(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_wr_obm_port_config(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_wr_obm_force_saf_config(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_wr_obm_lag_detection_config(unit, lport));
            /*TM LT is available. Revisit for flex*/
            SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_obm_pri_map_set(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_obm_buffer_config(unit, lport));

            /* CA port init */
            SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_wr_ca_ct_thresh(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_ca_bmop_set(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_wr_ca_force_saf_config(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_wr_ca_lag_detection_config(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_ca_buffer_config(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_tm_idb_ca_peek_depth_cfg(unit, lport));
        }
        else if (bcmtm_lport_is_mgmt(unit,lport)){
            /* MGMT_OBM port init */
            SHR_IF_ERR_EXIT(bcm56780_a0_mgmt_obm_shared_config(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_mgmt_obm_thresh(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_mgmt_obm_flow_ctrl_cfg(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_mgmt_obm_fc_threshold(unit ,lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_mgmt_obm_port_config(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_mgmt_obm_monitor_stats_config(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_mgmt_obm_pri_map_set(unit, lport));
            SHR_IF_ERR_EXIT(bcm56780_a0_mgmt_obm_buffer_config(unit, lport));
        }
    }


exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_tm_idb_init(int unit)
{
    uint32_t pipe_map;
    int pipe;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(unit, &pipe_map));

    for (pipe = 0; pipe < TD4_X9_PIPES_PER_DEV; pipe++) {
        if (pipe_map & (1 << pipe)) {
            SHR_IF_ERR_EXIT
                (bcm56780_a0_tm_idb_general_init(unit, pipe));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

