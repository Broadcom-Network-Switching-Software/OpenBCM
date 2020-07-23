/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:      asf.c
 * Purpose:   ASF feature support for Tomahawk3 SKUs
 * Requires:
 */


#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk3.h>
#include <soc/init/tomahawk3_idb_init.h>
#include <soc/init/tomahawk3_ep_init.h>
#include <appl/diag/system.h>
#include <soc/drv.h>
#include <soc/ll.h>
#include <soc/property.h>
#include <shared/bsl.h>
#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#include <soc/esw/portctrl.h>
#endif


#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#define _SOC_TH3_ASF_QUERY   0xfe
#define _SOC_TH3_ASF_RETRV   0xff

#define _SOC_TH3_ASF_MAX_FACTOR 4
#define _SOC_TH3_ASF_MAX_SPEED 400000

#define _SOC_TH3_SAF_COMMIT_ID_NUM 16

#define _SOC_TH3_CT_CLASS_VALID_NUM 8
#define _SOC_TH3_CT_CLASS_TOTAL_NUM 16
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define _TH3_PORT_SPEED(si, port) (si)->port_init_speed[(port)]

#define _SOC_ASF_TH3_UNIT_VALIDATE(unit)   \
        if ( !(((unit) >= 0) && ((unit) <= SOC_MAX_NUM_DEVICES)) ) \
            return SOC_E_UNIT;

#define _SOC_TH3_ASF_SPEED_CLASS_VALIDATE(class) \
        if(!(((class) >= 0 && (class) < _SOC_TH3_CT_CLASS_VALID_NUM))) \
            return SOC_E_PARAM;

#define _SOC_TH3_CT_CLASS_TO_SPEED_MAP(ct_class) \
            soc_th3_asf_config_tbl[ct_class].speed

typedef enum _soc_th3_pipe_bandwidth_config_e {

    _SOC_PIPE_BANDWIDTH_CONFIG_400G = 0,
    _SOC_PIPE_BANDWIDTH_CONFIG_50G_400G = 1,
    _SOC_PIPE_BADWIDTH_CONFIG_16x100G = 2,
    _SOC_PIPE_BANDWIDTH_CONFIG_OTHER = 3,
    _SOC_PIPE_BANDWIDTH_CONFIG_MAX = 4

} _soc_th3_pipe_bandwidth_config_t;

typedef struct _soc_th3_saf_commit_config_s {

    uint16 saf_commit_id[_SOC_TH3_SAF_COMMIT_ID_NUM];

} _soc_th3_saf_commit_config_t;

typedef enum _soc_th3_asf_speed_encoding_e {
    _SOC_TH3_SPEED_ID_RESERVED = 0,
    _SOC_TH3_SPEED_ID_10G,
    _SOC_TH3_SPEED_ID_25G,
    _SOC_TH3_SPEED_ID_40G,
    _SOC_TH3_SPEED_ID_50G,
    _SOC_TH3_SPEED_ID_100G,
    _SOC_TH3_SPEED_ID_200G,
    _SOC_TH3_SPEED_ID_400G,
    _SOC_TH3_SPEED_ID_ERROR
} _soc_th3_asf_speed_encoding_t;



static const
_soc_th3_saf_commit_config_t
  _soc_th3_saf_commit_tbl[_SOC_PIPE_BANDWIDTH_CONFIG_MAX] = {
    {{0, 0, 0, 4, 4, 4, 4, 8, 8, 8, 8, 12, 12, 12, 12, 16}},  /* 4X400GE */
    {{1, 2, 3, 3, 4, 5, 6, 6, 7, 9, 11, 13, 15, 17, 19, 21}}, /* Mixed 50GE, 400GE */
    {{4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 20}}, /* 16x100GE */
    /*{{1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 8, 9, 10, 11, 12, 13}}    others */
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}   /* others */
};
static const struct _soc_th3_bmop_settings_s {
    int speed;
    /*Bubble MOP disable.*/
    uint8 bmop_disable[3];
} _soc_th3_bmop_settings_tbl[] = {
    {    -1, {0, 0, 0} },       /* SAF     */
    { 10000, {1, 1, 1} },       /* 10GE    */
    { 25000, {0, 0, 0} },       /* 25GE    */
    { 40000, {0, 0, 0} },       /* 40GE    */
    { 50000, {0, 0, 0} },       /* 50GE    */
    {100000, {0, 0, 0} },       /* 100GE   */
    {200000, {0, 0, 0} },       /* 200GE   */
    {400000, {0, 0, 0} }        /* 400GE   */
};

/* ASF ctrl */
typedef struct _soc_th3_asf_ctrl_s {
    uint8                     init;
    pbmp_t                    asf_ports;
    pbmp_t                    pause_restore;
} _soc_th3_asf_ctrl_t;

/* ASF Control */
static _soc_th3_asf_ctrl_t*
_soc_th3_asf_ctrl[SOC_MAX_NUM_DEVICES] = {NULL};

#ifdef BCM_WARM_BOOT_SUPPORT
/* ASF Warmboot */
typedef struct _soc_th3_asf_wb_s {
    int unit;
    _soc_th3_asf_ctrl_t asf_ctrl;
} _soc_th3_asf_wb_t;
#endif  /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *    _soc_th3_asf_speed_encoding_get
 * Purpose:
 *   Return speed encoding given a speed in MB/S
 * Parameters:
 *     speed - (IN) speed in MB/s
 * Returns:
 *     SOC_E_XXX
 */

STATIC int
_soc_th3_asf_speed_encoding_get(int speed) {
    switch(speed) {
        case 10000:
            return _SOC_TH3_SPEED_ID_10G;
        case 25000:
            return _SOC_TH3_SPEED_ID_25G;
        case 40000:
            return _SOC_TH3_SPEED_ID_40G;
        case 50000:
            return _SOC_TH3_SPEED_ID_50G;
        case 100000:
        case 160000:
            return _SOC_TH3_SPEED_ID_100G;
        case 200000:
            return _SOC_TH3_SPEED_ID_200G;
        case 400000:
            return _SOC_TH3_SPEED_ID_400G;
        default:
            return _SOC_TH3_SPEED_ID_ERROR;
    }
}

STATIC int
_soc_th3_asf_speed_get (int speed_encode) {
    switch(speed_encode) {
        case _SOC_TH3_SPEED_ID_10G:
            return 10000;
        case _SOC_TH3_SPEED_ID_25G:
            return 25000;
        case _SOC_TH3_SPEED_ID_40G:
            return 40000;
        case _SOC_TH3_SPEED_ID_50G:
            return 50000;
        case _SOC_TH3_SPEED_ID_100G:
            return 100000;
        case _SOC_TH3_SPEED_ID_200G:
            return 200000;
        case _SOC_TH3_SPEED_ID_400G:
            return 400000;
        default:
            return -1;
    }
}

/*
 * Function:
 *    _soc_th3_port_asf_speed_limits_get
 * Purpose:
 *   Get the max and min src speed (encoding)
 *   given a dst port speed.
 * Parameters:
 *     unit       - (IN) unit number
 *     port_speed  - (IN)  dst port speed
 *     mode - (IN) forwarding mode
 *     min_sp (OUT) min src speed encoding
 *     max_sp (OUT) max src speed encoding
 * Returns:
 *     SOC_E_XXX
 */

STATIC int
_soc_th3_port_asf_speed_limits_get(
    int unit,
    int port_speed,
    int mode,
    int *min_sp,
    int *max_sp)
{
    if(SOC_TH3_ASF_MODE_SAF != mode) {
        *min_sp = _soc_th3_asf_speed_encoding_get(port_speed);
        *max_sp = MIN(port_speed*_SOC_TH3_ASF_MAX_FACTOR, _SOC_TH3_ASF_MAX_SPEED);
        *max_sp = _soc_th3_asf_speed_encoding_get(*max_sp);
        _SOC_TH3_ASF_SPEED_CLASS_VALIDATE(*min_sp);
        _SOC_TH3_ASF_SPEED_CLASS_VALIDATE(*max_sp);
    } else {
        *min_sp = -1;
        *max_sp = -1;
    }

    return SOC_E_NONE;

}

/*
 * Function:
 *    _soc_th3_port_asf_speed_limits_read
 * Purpose:
 *   Get the max and min src speed (encoding)
 *   given a dst port speed.
 * Parameters:
 *     unit       - (IN) unit number
 *     port  - (IN)  dst port
 *     min_sp (OUT) min src speed encoding
 *     max_sp (OUT) max src speed encoding
 * Returns:
 *     SOC_E_XXX
 */

STATIC int
_soc_th3_port_asf_speed_limits_read(
    int unit,
    soc_port_t port,
    int *min_sp,
    int *max_sp)
{
    soc_reg_t asf_config_reg = MMU_EBCTM_EPORT_CT_CFGr;
    uint32 rval = 0;
    soc_field_t min_sp_fld = DST_PORT_SPEEDf;
    soc_field_t max_sp_fld = MAX_SRC_PORT_SPEEDf;

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, asf_config_reg, port, 0, &rval));
    *min_sp = soc_reg_field_get(unit, asf_config_reg, rval, min_sp_fld);
    *max_sp = soc_reg_field_get(unit, asf_config_reg, rval, max_sp_fld);
    return SOC_E_NONE;

}

/*
 * Function:
 *    _soc_th3_idb_ca_bmop_set
 * Purpose:
 *   Config bubble MOP settings given a logic port.
 * Parameters:
 *     unit       - (IN) unit number
 *     port  - (IN) logic port
 *     port_speed - (IN) port_speed.
 *     mode  - (IN) ASF mode
 * Returns:
 *     SOC_E_XXX
 */

STATIC int
_soc_th3_idb_ca_bmop_set(int unit, soc_port_t port, int port_speed, int mode)
{
    soc_reg_t reg;
    uint64 rval64, fldval64;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    uint32 bmop_disable;
    int latency_mode;
    int speed_encoding;
    soc_info_t *si;

    static const soc_reg_t th3_idb_ca_control_2[_TH3_PIPES_PER_DEV] = {
        IDB_CA_CONTROL_2_PIPE0r, IDB_CA_CONTROL_2_PIPE1r,
        IDB_CA_CONTROL_2_PIPE2r, IDB_CA_CONTROL_2_PIPE3r,
        IDB_CA_CONTROL_2_PIPE4r, IDB_CA_CONTROL_2_PIPE5r,
        IDB_CA_CONTROL_2_PIPE6r, IDB_CA_CONTROL_2_PIPE7r
    };
    static const soc_field_t ca_bmop_disable_fields[] = {
        PORT0_BUBBLE_MOP_DISABLEf, PORT1_BUBBLE_MOP_DISABLEf,
        PORT2_BUBBLE_MOP_DISABLEf, PORT3_BUBBLE_MOP_DISABLEf,
        PORT4_BUBBLE_MOP_DISABLEf, PORT5_BUBBLE_MOP_DISABLEf,
        PORT6_BUBBLE_MOP_DISABLEf, PORT7_BUBBLE_MOP_DISABLEf
    };
    si = &(SOC_INFO(unit));
    if(si == NULL) {
        return SOC_E_INTERNAL;
    }
    phy_port = si->port_l2p_mapping[port];
    pipe_num = si->port_pipe[port];
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);

    SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &latency_mode));

    speed_encoding =
        (mode == SOC_TH3_ASF_MODE_SAF) ? 0 :
            soc_tomahawk3_speed_to_e2e_settings_class_map(port_speed);
    bmop_disable = _soc_th3_bmop_settings_tbl[speed_encoding].
        bmop_disable[latency_mode];
    reg = th3_idb_ca_control_2[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY,
                                            pm_num, &rval64));
    COMPILER_64_SET(fldval64, 0, bmop_disable);
    soc_reg64_field_set(unit, reg, &rval64, ca_bmop_disable_fields[subp],
                                                          fldval64);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    pm_num, rval64));
    return SOC_E_NONE;
}


/*
 * Function:
 *    _soc_th3_asf_pause_update
 * Purpose:
 *   Update PAUSE settings for a port
 * Parameters:
 *     unit   - (IN) unit number
 *     port  - (IN)  logical port number
 *     mode - (IN) forwarding mode
 * Returns:
 *     SOC_E_XXX
 */

STATIC int
_soc_th3_asf_pause_update(
    int unit,
    soc_port_t port,
    soc_th3_asf_mode_t mode)
{

#ifdef PORTMOD_SUPPORT
    int rv = SOC_E_NONE;
    portmod_pause_control_t pause_control;

    SOC_IF_ERROR_RETURN(soc_esw_portctrl_init_check(unit));

    rv = portmod_port_pause_control_get(unit, port, &pause_control);
    if (!rv) {
        /* Retrieve current rx/tx pause enable for port */
        SOC_IF_ERROR_RETURN(
                portmod_port_pause_control_get(unit, port, &pause_control));
        if(mode != SOC_TH3_ASF_MODE_SAF) {
            if (pause_control.rx_enable == 1) {
                SOC_PBMP_PORT_ADD((_soc_th3_asf_ctrl[unit])->pause_restore, port);
                pause_control.rx_enable = 0;
                SOC_IF_ERROR_RETURN(
                        portmod_port_pause_control_set(unit, port,
                            &pause_control));
            }
        } else {
            if (SOC_PBMP_MEMBER((_soc_th3_asf_ctrl[unit])->pause_restore, port)) {
                pause_control.rx_enable = 1;
                SOC_IF_ERROR_RETURN(
                        portmod_port_pause_control_set(unit, port, &pause_control));
                SOC_PBMP_PORT_REMOVE(_soc_th3_asf_ctrl[unit]->pause_restore, port);
            }
        }

        SOC_IF_ERROR_RETURN(
                portmod_port_pause_control_set(unit, port, &pause_control));
    }


    return rv;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/**
 * Function:
 *     _soc_th3_asf_tct_config_set
 * Purpose:
 *      Configure Transitional CT.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      SOC_E_xxx
 */
STATIC int
_soc_th3_asf_tct_config_set(int unit)
{
#define _SOC_TH3_ASF_VALID_SPEED_NUM 7
    int i;
    soc_reg_t reg = INVALIDr;
    uint64 rval;
    static const struct _soc_th3_tct_config_s {
        uint8 itm_pkt_th;
        uint8 mmu_pkt_th;
        uint8 itm_cell_th;
        uint8 mmu_cell_th;
        uint32 cell_reduction_cycle;
        uint32 cell_reduction_num;
        uint32 exit_tct_timer;
        uint32 exit_cell_threshold;
    } config_t[_SOC_TH3_ASF_VALID_SPEED_NUM] = {
        {16, 16, 38, 38, 4096, 8, 32000, 76},
        {16, 16, 38, 38, 4096, 8, 32000, 76},
        {16, 16, 38, 38, 4096, 8, 32000, 76},
        {16, 16, 38, 38, 4096, 8, 32000, 76},
        {31, 31, 38, 38, 4096, 8, 32000, 76},
        {62, 62, 62, 62, 4096, 8, 16000, 100},
        {124, 124, 124, 124, 4096, 8, 8000, 162},
    };

    for(i = 0; i < _SOC_TH3_ASF_VALID_SPEED_NUM; i++){
        reg = MMU_EBCTM_TCT_ENTER_SPEED_CFGr;
        SOC_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg, REG_PORT_ANY, i, &rval));

        soc_reg64_field32_set(unit, reg, &rval, ENTER_TCT_ITM_PKT_THRESHOLDf,
            config_t[i].itm_pkt_th);
        soc_reg64_field32_set(unit, reg, &rval, ENTER_TCT_MMU_PKT_THRESHOLDf,
            config_t[i].mmu_pkt_th);
        soc_reg64_field32_set(unit, reg, &rval, ENTER_TCT_ITM_CELL_THRESHOLDf,
            config_t[i].itm_cell_th);
        soc_reg64_field32_set(unit, reg, &rval, ENTER_TCT_MMU_CELL_THRESHOLDf,
            config_t[i].mmu_cell_th);

        SOC_IF_ERROR_RETURN
            (soc_reg64_set(unit, reg, REG_PORT_ANY, i, rval));

        reg = MMU_EBCTM_TCT_EXIT_SPEED_CFGr;
        SOC_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg, REG_PORT_ANY, i, &rval));

        soc_reg64_field32_set(unit, reg, &rval, TCT_BUFFER_CELL_REDUCTION_CYCLESf,
            config_t[i].cell_reduction_cycle);
        soc_reg64_field32_set(unit, reg, &rval, TCT_BUFFER_CELL_REDUCTION_NUMBERf,
            config_t[i].cell_reduction_num);
        soc_reg64_field32_set(unit, reg, &rval, EXIT_TCT_TIMERf,
            config_t[i].exit_tct_timer);
        soc_reg64_field32_set(unit, reg, &rval, EXIT_TCT_MMU_CELL_THRESHOLDf,
            config_t[i].exit_cell_threshold);

        SOC_IF_ERROR_RETURN
            (soc_reg64_set(unit, reg, REG_PORT_ANY, i, rval));
    }

#undef _SOC_TH3_ASF_VALID_SPEED_NUM

    return SOC_E_NONE;
}

#if 0
/**
 * Function:
 *     _soc_th3_asf_port_enable
 * Purpose:
 *      Enable/Disable a given port for CT in IDB.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Logical port number.
 *      enable - (IN) 0: disable, 1: enable.
 * Returns:
 *      SOC_E_xxx
 */
STATIC int
_soc_th3_idb_ca_asf_enable(int unit, soc_port_t port, uint8 ct_enable)
{
    soc_reg_t reg;
    uint64 rval64;
    uint32 fval;
    int phy_port;
    int pm_num;
    int subp;
    int pipe_num;
    soc_info_t *si = &SOC_INFO(unit);
    static const soc_reg_t ca_control_2[_TH3_PIPES_PER_DEV] =
    {
        IDB_CA_CONTROL_2_PIPE0r, IDB_CA_CONTROL_2_PIPE1r,
        IDB_CA_CONTROL_2_PIPE2r, IDB_CA_CONTROL_2_PIPE3r,
        IDB_CA_CONTROL_2_PIPE4r, IDB_CA_CONTROL_2_PIPE5r,
        IDB_CA_CONTROL_2_PIPE6r, IDB_CA_CONTROL_2_PIPE7r
    };
    static const soc_field_t ca_ct_disable_fields[] = {
        PORT0_CT_DISABLEf, PORT1_CT_DISABLEf,
        PORT2_CT_DISABLEf, PORT3_CT_DISABLEf,
        PORT4_CT_DISABLEf, PORT5_CT_DISABLEf,
        PORT6_CT_DISABLEf, PORT7_CT_DISABLEf
    };

    if(si == NULL) {
        return SOC_E_INTERNAL;
    }
    phy_port = si->port_l2p_mapping[port];
    pipe_num = si->port_pipe[port];
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    reg = ca_control_2[pipe_num];
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
    SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, REG_PORT_ANY,
                                            pm_num, &rval64));
    fval = ct_enable ? 0: 1;
    soc_reg64_field32_set(unit, reg, &rval64, ca_ct_disable_fields[subp],
                                                          fval);
    SOC_IF_ERROR_RETURN(soc_reg64_set(unit, reg, REG_PORT_ANY,
                                                    pm_num, rval64));
    return SOC_E_NONE;
}
#endif

/**
 * Function:
 *     _soc_th3_asf_port_enable
 * Purpose:
 *      Enable/Disable a given port for CT.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Logical port number.
 *      port_speed (IN) port speed in MB/s.
 *      enable - (IN) 0: disable, 1: enable.
 * Returns:
 *      SOC_E_xxx
 */
STATIC int
_soc_th3_asf_port_enable(
    int unit,
    soc_port_t port,
    int port_speed,
    uint8 enable)
{
    
    int dst_speed_id, max_src_speed_id;
    uint32 rval;
    soc_reg_t reg = MMU_EBCTM_EPORT_CT_CFGr;
    dst_speed_id = _soc_th3_asf_speed_encoding_get(port_speed);
    max_src_speed_id = _soc_th3_asf_speed_encoding_get
            (MIN((port_speed*_SOC_TH3_ASF_MAX_FACTOR), _SOC_TH3_ASF_MAX_SPEED));

    _SOC_TH3_ASF_SPEED_CLASS_VALIDATE(dst_speed_id);
    _SOC_TH3_ASF_SPEED_CLASS_VALIDATE(max_src_speed_id);

    SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, port, 0, &rval));
    if(enable) {

        soc_reg_field_set(unit, reg, &rval, DST_PORT_SPEEDf, dst_speed_id);
        soc_reg_field_set(unit, reg, &rval, MAX_SRC_PORT_SPEEDf,
            max_src_speed_id);

    }

    soc_reg_field_set(unit, reg, &rval, CT_ENABLEf, enable);
    SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, port, 0, rval));

    /* disable/enable per port TCT */
    reg = MMU_EBCTM_EPORT_TCT_CFGr;
    SOC_IF_ERROR_RETURN
           (soc_reg32_get(unit, reg, port, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, TCT_ENABLEf, enable);
    SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, port, 0, rval));

#if 0
    /*enable/diable CT in IDB */
    SOC_IF_ERROR_RETURN
        (_soc_th3_idb_ca_asf_enable(unit, port, enable));
#endif
    return SOC_E_NONE;
}

/**
 * Function:
 *     _soc_th3_mmu_rl_tile_config_set
 * Purpose:
 *      Configure Read launcher.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      SOC_E_xxx
 */
STATIC int
_soc_th3_mmu_rl_tile_config_set(int unit)
{
    soc_reg_t reg = INVALIDr;
    uint64 rval;

    static const struct _soc_th3_mmu_rl_tile_config_s {

        uint32 buffer_activity;
        uint32 high_th;
        uint16 ebq_backup_th;
        uint32 ebq_backup;
        uint32 stop_cooloff;

    } rl_config = {
        1024, 5120, 64, 512, 512
    };


    reg = MMU_RL_CT_TILE_ACTIVITYr;
    SOC_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval));

    soc_reg64_field32_set(unit, reg, &rval,
                            STOP_DUE_TO_EBQ_BACKUPf, rl_config.ebq_backup);
    soc_reg64_field32_set(unit, reg, &rval,
                    STOP_DUE_TO_BUFFER_ACTIVITYf, rl_config.buffer_activity);
    soc_reg64_field32_set(unit, reg, &rval,
                            EBQ_BACKUP_THRESHOLDf, rl_config.ebq_backup_th);

    SOC_IF_ERROR_RETURN
            (soc_reg64_set(unit, reg, REG_PORT_ANY, 0, rval));

    reg = MMU_RL_CT_TILE_ACTIVITY2r;
    COMPILER_64_ZERO(rval);

    SOC_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval));

    soc_reg64_field32_set(unit, reg, &rval,
                                STOP_COOLOFFf, rl_config.stop_cooloff);
    soc_reg64_field32_set(unit, reg, &rval,
                                HIGH_ACTIVITY_THf, rl_config.high_th);

    SOC_IF_ERROR_RETURN
            (soc_reg64_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

/**
 * Function:
 *     _soc_th3_port_asf_fifo_threshold_set
 * Purpose:
 *     Set CT FIFO threshold.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      SOC_E_xxx
 */
STATIC int
_soc_th3_port_asf_fifo_threshold_set(int unit)
{
    int i;
    soc_reg_t reg;
    soc_reg_t ct_cfg_regs[] = {
        MMU_EBCTM_CT_SPEED_6_CFGr, MMU_EBCTM_CT_SPEED_5_CFGr,
        MMU_EBCTM_CT_SPEED_4_CFGr, MMU_EBCTM_CT_SPEED_3_CFGr,
        MMU_EBCTM_CT_SPEED_2_CFGr, MMU_EBCTM_CT_SPEED_1_CFGr,
        MMU_EBCTM_CT_SPEED_0_CFGr};
    uint32 rval;
    uint32 fval[] = {66, 38, 24, 16, 16, 16, 16};
    soc_field_t field = CT_FIFO_THRESHOLDf;

    for(i = 0; i < sizeof(fval)/sizeof(uint32); i++){
        reg = ct_cfg_regs[i];
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, field, fval[i]);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}

/**
 * Function:
 *     _soc_th3_mmu_saf_commitment_config_set
 * Purpose:
 *     Set SAF commitment register based on pipe configuration.
 * Parameters:
 *      unit - (IN) Unit number.
 *      pipe - (IN) Pipe number.
 * Returns:
 *      SOC_E_xxx
 */
STATIC int
_soc_th3_mmu_saf_commitment_config_set(int unit, int pipe) {

    int i, pipe_cfg;
    uint32 rval;
    const _soc_th3_saf_commit_config_t *saf_commit_p;
    soc_reg_t reg = MMU_EBCTM_CT_BUDGET_SAF_COMMITMENT_CFGr;
    soc_field_t field = CTBUDGET_SAFCOMMITMENTf;

    if (pipe < 0 || pipe >= SOC_MAX_NUM_PIPES) {
        return SOC_E_PARAM;
    }

    pipe_cfg = _SOC_PIPE_BANDWIDTH_CONFIG_OTHER;

    saf_commit_p = &(_soc_th3_saf_commit_tbl[pipe_cfg]);

    for(i = 0; i < _SOC_TH3_SAF_COMMIT_ID_NUM; i++) {
        rval = 0;
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_eb_reg32_get(unit, reg, pipe, pipe, i, &rval));
        soc_reg_field_set(unit, reg, &rval, field, saf_commit_p->saf_commit_id[i]);
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_eb_reg32_set(unit, reg, pipe, pipe, i, rval));
    }
    return SOC_E_NONE;
}

STATIC int
_soc_th3_mmu_saf_speed_bucket_config_set(int unit) {
#define MOP_SPACE_200G_LOW_FREQ 4
    soc_reg_t speed_cfg_reg = MMU_EBCTM_MIN_MOP_SPACING_SPEED_BUCKET_1_CFGr;
    uint32 rval = 0;
    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_eb_reg32_get(unit, speed_cfg_reg, REG_PORT_ANY, 0, 0,
            &rval));
    soc_reg_field_set(unit, speed_cfg_reg, &rval, NUM_CLK_PER_MOP_CELLf,
            MOP_SPACE_200G_LOW_FREQ);
    SOC_IF_ERROR_RETURN
        (soc_tomahawk3_eb_reg32_set(unit, speed_cfg_reg, REG_PORT_ANY, 0, 0,
                                    rval));
#undef MOP_SPACE_200G_LOW_FREQ
    return SOC_E_NONE;
}

STATIC int
_soc_th3_ct_override_settings (int unit) {
    uint32 rval = 0;
    soc_reg_t reg;
    soc_field_t field;
    int fld_val, pipe;
    soc_info_t *si = &SOC_INFO(unit);
    static const soc_reg_t idb_min_port_pick_regs[] = {
        IS_MIN_PORT_PICK_SPACING_WITHIN_PKT_PIPE0r, IS_MIN_PORT_PICK_SPACING_WITHIN_PKT_PIPE1r,
        IS_MIN_PORT_PICK_SPACING_WITHIN_PKT_PIPE2r, IS_MIN_PORT_PICK_SPACING_WITHIN_PKT_PIPE3r,
        IS_MIN_PORT_PICK_SPACING_WITHIN_PKT_PIPE4r, IS_MIN_PORT_PICK_SPACING_WITHIN_PKT_PIPE5r,
        IS_MIN_PORT_PICK_SPACING_WITHIN_PKT_PIPE6r, IS_MIN_PORT_PICK_SPACING_WITHIN_PKT_PIPE7r
    };

    static const soc_reg_t idb_min_cell_spacing_regs[] = {
        IS_MIN_CELL_SPACING_EOP_TO_SOP_PIPE0r, IS_MIN_CELL_SPACING_EOP_TO_SOP_PIPE1r,
        IS_MIN_CELL_SPACING_EOP_TO_SOP_PIPE2r, IS_MIN_CELL_SPACING_EOP_TO_SOP_PIPE3r,
        IS_MIN_CELL_SPACING_EOP_TO_SOP_PIPE4r, IS_MIN_CELL_SPACING_EOP_TO_SOP_PIPE5r,
        IS_MIN_CELL_SPACING_EOP_TO_SOP_PIPE6r, IS_MIN_CELL_SPACING_EOP_TO_SOP_PIPE7r
    };


    /* MMU_EBCTM_CT_BUDGET_CFG.CT_BUDGET = 'd16 */
    reg = MMU_EBCTM_CT_BUDGET_CFGr;
    field = CT_BUDGETf;
    fld_val = 16;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, field, fld_val);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    /* MMU_RL_EBP_OVRD.CT_BURST_TH = 'd12 */
    reg = MMU_RL_EBP_OVRDr;
    field = CT_BURST_THf;
    fld_val = 12;
    rval = 0x0;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, field, fld_val);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (!SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {

            /* MIN_PORT_PICK_SPACING_WITHIN_PKT.MID_PKT_400G_MIN_SPACING = 6 */
            reg = idb_min_port_pick_regs[pipe];
            field = MID_PKT_400G_MIN_SPACINGf;
            fld_val = 6;
            rval = 0x0;
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pipe, 0, &rval));
            soc_reg_field_set(unit, reg, &rval, field, fld_val);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pipe, 0, rval));

            /* IS_MIN_CELL_SPACING_EOP_TO_SOP.INTER_PKT_400G_MIN_SPACING = 2 */
            reg = idb_min_cell_spacing_regs[pipe];
            field = INTER_PKT_400G_MIN_SPACINGf;
            fld_val = 2;
            rval = 0x0;
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pipe, 0, &rval));
            soc_reg_field_set(unit, reg, &rval, field, fld_val);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pipe, 0, rval));
        }
    }

    /* For lower frequencies,
       MIN_PORT_PICK_SPACING_WITHIN_PKT.MID_PKT_200G_MIN_SPACING = 6 */
    if (si->frequency < 1325) {
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            if (!SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
                reg = idb_min_port_pick_regs[pipe];
                field = MID_PKT_200G_MIN_SPACINGf;
                fld_val = 6;
                rval = 0x0;
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pipe, 0, &rval));
                soc_reg_field_set(unit, reg, &rval, field, fld_val);
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pipe, 0, rval));
            }
        }
    }



    return SOC_E_NONE;
}

/*************************************
 * Init Start Routine                *
 *    - Allocates ASF Ctrl structure *
 *      for the given unit           *
 */
STATIC int
_soc_th3_asf_init_start(int unit)
{
    _SOC_ASF_TH3_UNIT_VALIDATE(unit);

    if (!(_soc_th3_asf_ctrl[unit] =
              sal_alloc(sizeof(_soc_th3_asf_ctrl_t), "TH3 ASF Ctrl Area"))) {
        return SOC_E_MEMORY;
    }
    sal_memset(_soc_th3_asf_ctrl[unit], 0, sizeof(_soc_th3_asf_ctrl_t));

    return SOC_E_NONE;
}

/**
 * Function:
 *     soc_th3_port_asf_valid
 * Purpose:
 *      Check if a port is eligible for CT.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Logical port number.
 * Returns:
 *      SOC_E_xxx
 */
int
soc_th3_port_asf_valid(
    int unit,
    soc_port_t port)
{
    if(IS_CPU_PORT(unit, port) || IS_MANAGEMENT_PORT(unit, port)
            || IS_LB_PORT(unit, port)) {
        return FALSE;
    }

    return TRUE;
}

/**
 * Function:
 *     soc_th3_port_asf_mode_get
 * Purpose:
 *      Get traffic forwarding mode for a specific port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Logical port number.
 *      port_speed - (IN) Port speed in MB/s.
 *      mode - (OUT) ASF mode
 * Returns:
 *      SOC_E_xxx
 */
int
soc_th3_port_asf_mode_get(
    int unit,
    soc_port_t port,
    int port_speed,
    soc_th3_asf_mode_t *mode)
{
    uint32 rval, enable;
    _SOC_ASF_TH3_UNIT_VALIDATE(unit);

    if(NULL == mode) {
        return SOC_E_INTERNAL;
    }
    if(soc_th3_port_asf_valid(unit, port)) {
        soc_reg_t reg = MMU_EBCTM_EPORT_CT_CFGr;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, port, 0, &rval));
        enable = soc_reg_field_get(unit, reg, rval, CT_ENABLEf);
        if(enable) {
            *mode = SOC_TH3_ASF_MODE_CT;
        }
        else {
            *mode = SOC_TH3_ASF_MODE_SAF;
        }
    } else {
        *mode = SOC_TH3_ASF_MODE_SAF;
    }

    return SOC_E_NONE;
}
/**
 * Function:
 *     soc_th3_port_asf_mode_set
 * Purpose:
 *      Set traffic forwarding mode for a specific port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Logical port number.
 *      port_speed - (IN) Port speed in MB/s.
 *      mode - (IN) ASF mode
 * Returns:
 *      SOC_E_xxx
 */
int
soc_th3_port_asf_mode_set(
    int                 unit,
    soc_port_t          port,
    int                 port_speed,
    soc_th3_asf_mode_t  mode)
{
    int init = 0;
    soc_info_t *si = &SOC_INFO(unit);
    _SOC_ASF_TH3_UNIT_VALIDATE(unit);

    if(SOC_TH3_ASF_MODE_INIT == mode) {
        init = 1;
        mode = SOC_TH3_ASF_MODE_SAF;
    }
    if ((!init) && (!_soc_th3_asf_ctrl[unit] ||
        !_soc_th3_asf_ctrl[unit]->init || !si)) {
        return SOC_E_INTERNAL;
    }
    if(!soc_th3_port_asf_valid(unit, port)) {
        if(SOC_TH3_ASF_MODE_SAF != mode) {
            return SOC_E_UNAVAIL;
        }
        return SOC_E_NONE;
    }

    if(SOC_TH3_ASF_MODE_SAF != mode) {
        /* CT settings */
        /* Bubble Mop setting */
        SOC_IF_ERROR_RETURN
            (_soc_th3_idb_ca_bmop_set(unit, port, port_speed, mode));


        /* Disable pause */
        SOC_IF_ERROR_RETURN
            (_soc_th3_asf_pause_update(unit, port, mode));

        /*Enable CT */
        SOC_IF_ERROR_RETURN
            (_soc_th3_asf_port_enable(unit, port, port_speed, 1));

        /*bookkeeping */
        SOC_PBMP_PORT_ADD(_soc_th3_asf_ctrl[unit]->asf_ports, port);

    } else {
        /* Bubble Mop setting */
        SOC_IF_ERROR_RETURN
            (_soc_th3_idb_ca_bmop_set(unit, port, port_speed, mode));
        /* Disable CT */
        SOC_IF_ERROR_RETURN
            (_soc_th3_asf_port_enable(unit, port, port_speed, 0));
        /* Restore Pause */
        SOC_IF_ERROR_RETURN
            (_soc_th3_asf_pause_update(unit, port, mode));

        /*bookkeeping */
        SOC_PBMP_PORT_REMOVE(_soc_th3_asf_ctrl[unit]->asf_ports, port);
    }

    return SOC_E_NONE;
}

/************************
 * Init Complete Marker *
 */
STATIC int
_soc_th3_asf_init_done(int unit)
{
    _SOC_ASF_TH3_UNIT_VALIDATE(unit);


    if (_soc_th3_asf_ctrl[unit]) {
        _soc_th3_asf_ctrl[unit]->init = 1;

        return SOC_E_NONE;
    } else {
        return SOC_E_INTERNAL;
    }
}

/**
 * Function:
 *      soc_th3_asf_init
 * Purpose:
 *      Initialize ASF subsystem and configure all the applicable ports
 *      to the specified forwarding mode
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      SOC_E_xxx
 */

int
soc_th3_asf_init(int unit)
{
    soc_port_t port;
    int speed, pipe;
    uint32 pipe_map;
    soc_info_t *si;
    int core_clock;
    if (!(si = &SOC_INFO(unit))) {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN(_soc_th3_asf_init_start(unit));
    /* 1. chip/pipe-level init */

    /* 1.1 RL config */
    SOC_IF_ERROR_RETURN(_soc_th3_mmu_rl_tile_config_set(unit));
    /* 1.2 CT FIFO threshold config */
    SOC_IF_ERROR_RETURN(_soc_th3_port_asf_fifo_threshold_set(unit));
    /* 1.3 TCT config */
    SOC_IF_ERROR_RETURN
        (_soc_th3_asf_tct_config_set(unit));
    /* 1.4 SAF commitment config */
    soc_tomahawk3_pipe_map_get(unit, &pipe_map);
    for(pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        if(pipe_map & (1 << pipe)) {
            SOC_IF_ERROR_RETURN(
                _soc_th3_mmu_saf_commitment_config_set(unit, pipe));
        }
    }

    /* 1.5 MOP min spacing config for lower frequencies */
    core_clock = si->frequency;
    if (core_clock <= 1000) {
        _soc_th3_mmu_saf_speed_bucket_config_set(unit);
    }

    /* Additional settings for SDK-166459 */
    SOC_IF_ERROR_RETURN(_soc_th3_ct_override_settings(unit));

    /* Per-port config */
    PBMP_PORT_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)
                || IS_MANAGEMENT_PORT(unit, port)) {
            continue;
        }
        speed = _TH3_PORT_SPEED(si, port);
        SOC_IF_ERROR_RETURN
            (soc_th3_port_asf_mode_set(unit, port, speed, SOC_TH3_ASF_MODE_INIT));

    }

    SOC_IF_ERROR_RETURN(_soc_th3_asf_init_done(unit));
    return SOC_E_NONE;
}

/************************
 * Deinit Routine   *
 */
int
soc_th3_asf_deinit(int unit)
{
#if 0
    /* Unregister linkscan callback routine */
    bcm_esw_linkscan_unregister(unit, soc_th_asf_linkscan_callback);
#endif

    if (_soc_th3_asf_ctrl[unit]) {
        sal_free(_soc_th3_asf_ctrl[unit]);
        _soc_th3_asf_ctrl[unit] = NULL;
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_th3_port_asf_detach                                              *
 * Description:                                                               *
 *     Update ASF when port deleted during Flexport operation                 *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 */
int
soc_th3_port_asf_detach(int unit, soc_port_t port)
{
    _SOC_ASF_TH3_UNIT_VALIDATE(unit);

    if (_soc_th3_asf_ctrl[unit] == NULL) {
        return SOC_E_INIT;
    }

    SOC_IF_ERROR_RETURN
        (soc_th3_port_asf_mode_set(unit, port,
                                  SOC_INFO(unit).port_init_speed[port],
                                  SOC_TH3_ASF_MODE_SAF));

    if (soc_th3_port_asf_valid(unit, port)) {
        SOC_PBMP_PORT_REMOVE(_soc_th3_asf_ctrl[unit]->asf_ports, port);
        SOC_PBMP_PORT_REMOVE(_soc_th3_asf_ctrl[unit]->pause_restore, port);
    }
    return SOC_E_NONE;
}

int
soc_th3_asf_config_dump(int unit)
{
    _SOC_ASF_TH3_UNIT_VALIDATE(unit);
    if (!_soc_th3_asf_ctrl[unit] || !_soc_th3_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}

/**
 * Function:
 *     _soc_th3_port_asf_class_get
 * Purpose:
 *      Return CT class of a given port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) logical port number.
 *      class - (OUT) CT class
 * Returns:
 *      SOC_E_xxx
 */

STATIC int
_soc_th3_port_asf_class_get(
    int unit,
    soc_port_t port,
    int  *class)
{
    edb_ip_cut_thru_class_entry_t entry;

    if (NULL == class) {
        return SOC_E_PARAM;
    }

    sal_memset(&entry, 0, sizeof(edb_ip_cut_thru_class_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_EDB_IP_CUT_THRU_CLASSm(unit, MEM_BLOCK_ALL, port, &entry));
    *class = soc_mem_field32_get(unit, EDB_IP_CUT_THRU_CLASSm, &entry,
                                 CUT_THRU_CLASSf);

    return SOC_E_NONE;
}

/**
 * Function:
 *     _soc_th3_port_asf_fifo_threshold_get
 * Purpose:
 *      Get MMU CT FIFO Threshold based on port speed.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_speed - (IN) Port speed in MB/s.
 *      ct_thresh - (OUT) CT FIFO Threshold.
 * Returns:
 *      SOC_E_xxx
 */
STATIC int
_soc_th3_port_asf_fifo_threshold_get(
    int unit,
    int port_speed,
    int* ct_thresh)

{
    int speed_id;
    uint32 rval;
    soc_reg_t reg = INVALIDr;
    soc_reg_t ct_cfg_regs[] = {
        INVALIDr,                  MMU_EBCTM_CT_SPEED_6_CFGr,
        MMU_EBCTM_CT_SPEED_5_CFGr, MMU_EBCTM_CT_SPEED_4_CFGr,
        MMU_EBCTM_CT_SPEED_3_CFGr, MMU_EBCTM_CT_SPEED_2_CFGr,
        MMU_EBCTM_CT_SPEED_1_CFGr, MMU_EBCTM_CT_SPEED_0_CFGr};

    speed_id = _soc_th3_asf_speed_encoding_get(port_speed);
    reg = ct_cfg_regs[speed_id];
    _SOC_TH3_ASF_SPEED_CLASS_VALIDATE(speed_id);
    if(SOC_REG_IS_VALID(unit, reg)) {
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        *ct_thresh = soc_reg_field_get(unit, reg, rval, CT_FIFO_THRESHOLDf);
    }
    return SOC_E_NONE;
}

/**
 * Function:
 *     soc_th3_port_asf_config_dump
 * Purpose:
 *      Dump per-port asf configurations to diag Shell.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Logical port number.
 *      port_speed - (IN) Port speed in MB/s.
 * Returns:
 *      SOC_E_xxx
 */
int
soc_th3_port_asf_config_dump(int unit, soc_port_t port, int port_speed)
{
    int    ct_class = 0;
    uint8  src_class = 0, dst_class = 0;
    int  min_sp = 0, max_sp = 0, fifo_threshold = 0;
    uint8  xmit_start_cnt[_SOC_TH3_CT_CLASS_VALID_NUM] = {0};
    uint32 egr_mmu_cell_credits = 0;
    portmod_pause_control_t pause_control;
    int rxp;
    soc_th3_asf_mode_t mode = SOC_TH3_ASF_MODE_SAF;
    egr_mmu_cell_credit_entry_t credit_entry;
    int phy_port;
    int min_speed, max_speed, dst_port_speed;
    soc_info_t *si = &SOC_INFO(unit);

    if(si == NULL) {
        return SOC_E_INTERNAL;
    }
    phy_port = si->port_l2p_mapping[port];

    _SOC_ASF_TH3_UNIT_VALIDATE(unit);
    SOC_IF_ERROR_RETURN(soc_esw_portctrl_init_check(unit));

    if (!soc_th3_port_asf_valid(unit, port)) {
        return BCM_E_NONE;
    }

    if (!_soc_th3_asf_ctrl[unit] || !_soc_th3_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(
        soc_th3_port_asf_mode_get(unit, port, port_speed, &mode));

    SOC_IF_ERROR_RETURN(
        _soc_th3_port_asf_speed_limits_read(unit, port, &min_sp, &max_sp));

    min_speed = _soc_th3_asf_speed_get(min_sp);
    max_speed = _soc_th3_asf_speed_get(max_sp);

    if (mode != SOC_TH3_ASF_MODE_SAF) {
        dst_port_speed = min_speed;
    } else {
        dst_port_speed = port_speed;
    }

    SOC_IF_ERROR_RETURN(
        _soc_th3_port_asf_class_get(unit, port, &ct_class));

    SOC_IF_ERROR_RETURN(
        _soc_th3_port_asf_fifo_threshold_get(unit, dst_port_speed,
            &fifo_threshold));

    dst_class = soc_tomahawk3_get_ct_class(dst_port_speed);
    for (src_class = 0; src_class < _SOC_TH3_CT_CLASS_VALID_NUM; src_class++) {
        xmit_start_cnt[src_class] = _SOC_TH3_ASF_RETRV;
        SOC_IF_ERROR_RETURN(
            soc_th3_port_asf_xmit_start_count_get(unit, port, src_class,
                                                  dst_class, mode,
                                                  &xmit_start_cnt[src_class]));
    }

    LOG_CLI(("%-5s %-3d %-3d %-6d %-6d %-3d   ", SOC_PORT_NAME(unit, port),
             mode, ct_class, min_speed, max_speed, fifo_threshold));
    for (src_class = 0; src_class < _SOC_TH3_CT_CLASS_VALID_NUM; src_class++) {
        LOG_CLI(("%-3d ", xmit_start_cnt[src_class]));
    }


    SOC_IF_ERROR_RETURN(READ_EGR_MMU_CELL_CREDITm(unit, MEM_BLOCK_ANY,
                        phy_port, &credit_entry));
    egr_mmu_cell_credits = soc_EGR_MMU_CELL_CREDITm_field32_get(unit,
                           &credit_entry, CREDITf);

    SOC_IF_ERROR_RETURN(
            portmod_port_pause_control_get(unit, port, &pause_control));
    rxp = pause_control.rx_enable;
    LOG_CLI((" %-3d %-3d \n", egr_mmu_cell_credits, rxp));

    return SOC_E_NONE;
}

/**
 * Function:
 * soc_th3_port_asf_show
 * Purpose:
 *      Print per-port traffic forwarding mode in diag Shell.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Logical port number.
 *      port_speed - (IN) Port speed in MB/s.
 * Returns:
 *      SOC_E_xxx
 */
int
soc_th3_port_asf_show(
    int        unit,
    soc_port_t port,
    int        port_speed)
{
#define _STRING_LEN 5
#define _ASF_MODE_NUM 2

    char mode_str[_ASF_MODE_NUM][_STRING_LEN] = {"SAF", "CT"};
    char speed_str[_SOC_TH3_CT_CLASS_VALID_NUM][_STRING_LEN] = {
                              "SAF", "10G", "25G", "40G",
                              "50G", "100G", "200G", "400G"};
    soc_th3_asf_mode_t mode = SOC_TH3_ASF_MODE_SAF;
    int min_speed, max_speed;

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
        return BCM_E_NONE;
    }

    SOC_IF_ERROR_RETURN(
        soc_th3_port_asf_mode_get(unit, port, port_speed, &mode));


    LOG_CLI(("%-5s     %-11s      ", SOC_PORT_NAME(unit, port), mode_str[mode]));

    if (SOC_TH3_ASF_MODE_SAF == mode) {
        LOG_CLI(("    .. NA ..\n"));
    } else if (SOC_TH3_ASF_MODE_CT == mode) {
        SOC_IF_ERROR_RETURN(
            _soc_th3_port_asf_speed_limits_get(unit, port_speed, mode,
                                                &min_speed, &max_speed));
        LOG_CLI(("%s / %s\n", speed_str[max_speed], speed_str[min_speed]));
    }
#undef _STRING_LEN
#undef _ASF_MODE_NUM
    return SOC_E_NONE;
}

/**
 * Function:
 * soc_th3_asf_class_init
 * Purpose:
 *      Initialize CT class memory.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Logical port number.
 *      class_id (IN) CT class ID.
 * Returns:
 *      SOC_E_xxx
 */

int
soc_th3_asf_class_init(
    int unit,
    soc_port_t port,
    uint32 class_id)
{
    edb_ip_cut_thru_class_entry_t entry;
    sal_memset(&entry, 0, sizeof(edb_ip_cut_thru_class_entry_t));
    soc_mem_field32_set(unit, EDB_IP_CUT_THRU_CLASSm, &entry,
                        CUT_THRU_CLASSf, class_id);
    SOC_IF_ERROR_RETURN
        (WRITE_EDB_IP_CUT_THRU_CLASSm(unit, MEM_BLOCK_ALL, port, &entry));

    return SOC_E_NONE;
}


/******************************************************************************
 * Name: soc_th3_port_asf_xmit_start_count_get                                *
 * Description:                                                               *
 *     Query or retrieve the XMIT Start Count for the specified source class  *
 *     of the port depending on the magic number passed in xmit_cnt           *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Source Port's Class                                                  *
 *     - Destination Port's CT Class                                          *
 *     - ASF Mode                                                             *
 * IN/OUT params:                                                             *
 *     - xmit_cnt : IN:  carries magic number for query / retrieve            *
 *     - xmit_cnt : OUT: contains a valid XMIT START COUNT for the            *
 *       source port class, on success                                        *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 */
int
soc_th3_port_asf_xmit_start_count_get(
    int                      unit,
    soc_port_t               port,
    int                      sc,
    int                      dc,
    soc_th3_asf_mode_t        mode,
    uint8                    *xmit_cnt)
{
    soc_info_t *si;
    edb_xmit_start_count_entry_t entry;
    int port_idx;
    int pipe;
    soc_mem_t config_mem = INVALIDm;

    _SOC_ASF_TH3_UNIT_VALIDATE(unit);

    if (!(si = &SOC_INFO(unit))) {
        return SOC_E_INTERNAL;
    }

    pipe = si->port_pipe[port];
    config_mem = SOC_MEM_UNIQUE_ACC(unit, EDB_XMIT_START_COUNTm)[pipe];

    if (!xmit_cnt ||
        !((mode >= SOC_TH3_ASF_MODE_SAF) &&
          (mode <= SOC_TH3_ASF_MODE_CT))) {
        return SOC_E_PARAM;
    }
    _SOC_TH3_ASF_SPEED_CLASS_VALIDATE(sc);
    _SOC_TH3_ASF_SPEED_CLASS_VALIDATE(dc);


    if (_SOC_TH3_ASF_RETRV == *xmit_cnt) {
        *xmit_cnt = 0;
        port_idx = ((port % SOC_TH3_MAX_DEV_PORTS_PER_PIPE)
                    * _SOC_TH3_CT_CLASS_TOTAL_NUM) + sc;

        sal_memset(&entry, 0, sizeof(edb_xmit_start_count_entry_t));
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, config_mem, MEM_BLOCK_ALL,
                                                     port_idx, &entry));
        *xmit_cnt = soc_mem_field32_get(unit, config_mem,
                                        &entry, THRESHOLDf);
    } else if (_SOC_TH3_ASF_QUERY == *xmit_cnt) {
        /* TH3: EDB_XMIT_START_COUNT is programmed by
         * soc_tomahawk3_ep_set_xmit_start_cnt. This function should not be used
         * to query and program the memory*/
        return SOC_E_INTERNAL;

    }

    return SOC_E_NONE;
}
/******************************************************************************
 * Name: soc_th3_port_asf_xmit_start_count_set                                *
 * Description:                                                               *
 *     Initialize XMIT START COUNT memory for all the source class for the    *
 *     specified port                                                         *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 *     - Extra cells (if any, to be added to xmit start count)                *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 */
int
soc_th3_port_asf_xmit_start_count_set(
    int                  unit,
    soc_port_t           port,
    int                  port_speed,
    soc_th3_asf_mode_t    mode,
    uint8                extra_cells)
{
    edb_xmit_start_count_entry_t entry;
    soc_info_t *si = &SOC_INFO(unit);
    uint8                        xmit_cnt = 0;
    int                          src_class, dst_class = 0, port_idx;
    int                          pipe;
    soc_mem_t                    config_mem = INVALIDm;

    if (!((mode >= SOC_TH3_ASF_MODE_SAF) &&
          (mode <= SOC_TH3_ASF_MODE_CFG_UPDATE))) {
        return SOC_E_PARAM;
    }

    if (SOC_TH3_ASF_MODE_CFG_UPDATE != mode) {
        if (SOC_E_PARAM == (dst_class = soc_tomahawk3_get_ct_class(port_speed))) {
            /* port on a speed unsupported by CT - will resort to SAF */
            dst_class = 0;
        }
    }

    sal_memset(&entry, 0, sizeof(edb_xmit_start_count_entry_t));

    pipe = si->port_pipe[port];
    config_mem = SOC_MEM_UNIQUE_ACC(unit, EDB_XMIT_START_COUNTm)[pipe];

    for (src_class = 0; src_class < _SOC_TH3_CT_CLASS_VALID_NUM; src_class++) {
        if (SOC_TH3_ASF_MODE_CFG_UPDATE == mode) {
            xmit_cnt = _SOC_TH3_ASF_RETRV;
        } else {
            xmit_cnt = _SOC_TH3_ASF_QUERY;
        }
        SOC_IF_ERROR_RETURN(
            soc_th3_port_asf_xmit_start_count_get(unit, port, src_class,
                                                   dst_class, mode, &xmit_cnt));
        xmit_cnt += extra_cells;

        port_idx = ((port % SOC_TH3_MAX_DEV_PORTS_PER_PIPE)
                        * _SOC_TH3_CT_CLASS_TOTAL_NUM) + src_class;

        soc_mem_field32_set(unit, config_mem, &entry,
                            THRESHOLDf, xmit_cnt);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, config_mem, MEM_BLOCK_ALL,
                                          port_idx, &entry));
    }

    return SOC_E_NONE;
}

/************************
 * ASF Warmboot Support *
 */
#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
#define BCM_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_WB_VERSION_1_4                SOC_SCACHE_VERSION(1,4)
#define BCM_WB_VERSION_1_5                SOC_SCACHE_VERSION(1,5)
#define BCM_WB_VERSION_1_6                SOC_SCACHE_VERSION(1,6)
#define BCM_WB_VERSION_1_7                SOC_SCACHE_VERSION(1,7)
#define BCM_WB_VERSION_1_8                SOC_SCACHE_VERSION(1,8)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_8

int
soc_th3_asf_wb_memsz_get(
    int               unit,
    OUT uint32* const mem_sz,
    uint16            scache_ver)
{
    if (!mem_sz) {
        return SOC_E_PARAM;
    }
    *mem_sz = 0;

    if (!SOC_WARM_BOOT(unit)) {
        if (!_soc_th3_asf_ctrl[unit] ) {
            return SOC_E_UNAVAIL;
        }
        if (!_soc_th3_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }
    }

    *mem_sz = sizeof(_soc_th3_asf_wb_t);

    if (scache_ver < BCM_WB_VERSION_1_8) {
        /* adjust wb area sz for lower versions */
        *mem_sz -= sizeof(int);
    }

    return SOC_E_NONE;
}

int
soc_th3_asf_wb_sync(
    int    unit,
    IN_OUT uint8* const wb_data)
{
    _soc_th3_asf_wb_t *wbd;

    if (!_soc_th3_asf_ctrl[unit]) {
        return SOC_E_UNAVAIL;
    }
    if (!_soc_th3_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }
    if (!wb_data) {
        return SOC_E_PARAM;
    }

    wbd = (_soc_th3_asf_wb_t *) wb_data;
    wbd->unit = unit;
    sal_memcpy(&wbd->asf_ctrl, _soc_th3_asf_ctrl[unit],
               sizeof(_soc_th3_asf_ctrl_t));

    return SOC_E_NONE;
}

int
soc_th3_asf_wb_recover(
    int          unit,
    uint8* const wb_data,
    uint16       scache_ver)
{
    _soc_th3_asf_wb_t *wbd;
    int adj = 0; /*scache ver adjustment */

    if (!SOC_WARM_BOOT(unit)) {
        return SOC_E_INTERNAL;
    }
    if (!wb_data) {
        return SOC_E_PARAM;
    }

    wbd = (_soc_th3_asf_wb_t *) wb_data;
    if (wbd->unit == unit) {
        if (!(_soc_th3_asf_ctrl[unit] =
                   sal_alloc(sizeof(_soc_th3_asf_ctrl_t), "TH ASF Ctrl Area"))) {
            return SOC_E_MEMORY;
        }

        sal_memcpy(_soc_th3_asf_ctrl[unit], &wbd->asf_ctrl,
                   (sizeof(_soc_th3_asf_ctrl_t) - adj));
    }

    LOG_CLI((BSL_META_U(unit, "*** unit %d: MMU-ASF subsystem warmbooted: \n"),
                     unit));

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#endif
