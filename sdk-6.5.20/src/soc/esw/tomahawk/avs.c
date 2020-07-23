/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        avs.c
 */

#include <shared/bsl.h>

#ifdef INCLUDE_AVS
#include <soc/avs.h>
#include <soc/tomahawk.h>

#define SOC_AVS_TOMAHAWK_NUMBER_OF_CENTRALS 36
#define SOC_AVS_TOMAHAWK_CENTRAL_XBMP0 0xFF000000
#define SOC_AVS_TOMAHAWK_CENTRAL_XBMP1 0xFFFFFFFF
#define SOC_AVS_TOMAHAWK_FIRST_CENTRAL 0

#define SOC_AVS_TOMAHAWK_NUMBER_OF_REMOTES 80
#define SOC_AVS_TOMAHAWK_REMOTE_XBMP0 0x0
#define SOC_AVS_TOMAHAWK_REMOTE_XBMP1 0x0
#define SOC_AVS_TOMAHAWK_REMOTE_XBMP2 0xFFFF3000
#define SOC_AVS_TOMAHAWK_FIRST_REMOTE 0

#define SOC_AVS_TOMAHAWK_VMIN_AVS 9000
#define SOC_AVS_TOMAHAWK_VMAX_AVS 10500
#define SOC_AVS_TOMAHAWK_VMARGIN_HIGH SOC_AVS_DEFAULT_VMARGIN_HIGH
#define SOC_AVS_TOMAHAWK_VMARGIN_LOW SOC_AVS_DEFAULT_VMARGIN_LOW

static soc_avs_reg_info_t _th_osc_cen_init_list[]={
/* AVS_REG_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_0r */
{AVS_REG_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_0r,CEN_ROSC_ENABLE_DEFAULTf},
{AVS_REG_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_1r,CEN_ROSC_ENABLE_DEFAULTf},
{INVALIDr,INVALIDf}
};

static soc_avs_reg_info_t _th_osc_cen_thr_en_list[]={
{AVS_REG_ROSC_THRESHOLD_1_CEN_ROSC_THRESHOLD1_EN_0r,IRQ_ENf},
{AVS_REG_ROSC_THRESHOLD_1_CEN_ROSC_THRESHOLD1_EN_1r,IRQ_ENf},
{AVS_REG_ROSC_THRESHOLD_2_CEN_ROSC_THRESHOLD2_EN_0r,IRQ_ENf},
{AVS_REG_ROSC_THRESHOLD_2_CEN_ROSC_THRESHOLD2_EN_1r,IRQ_ENf},
{INVALIDr,INVALIDf}
};

/* reset osc measurement */
static soc_avs_reg_info_t _th_osc_seq_reset_list[]={
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_0r,M_INIT_CEN_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_1r,M_INIT_CEN_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_RMT_ROSC_0r,M_INIT_RMT_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_RMT_ROSC_1r,M_INIT_RMT_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_RMT_ROSC_2r,M_INIT_RMT_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_RMT_ROSC_3r,M_INIT_RMT_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_RMT_ROSC_4r,M_INIT_RMT_ROSCf},
{INVALIDr,INVALIDf}
};

STATIC int
_soc_th_avs_ioctl(int unit, soc_avs_ioctl_t opcode, void* data, int len)
{
    uint32 value;
    int rv = SOC_E_NONE;
    switch (opcode) {
    case SOC_AVS_CTRL_PVTMON_VOLTAGE_GET:
        value = *(uint32 *)data; /* code */
        *(uint32 *)data = 10U*(value * 8800U) / 7168U;
        /* = ((code/1024)*880.0/0.7)/1000 */
        break;
    case SOC_AVS_CTRL_PVTMON_TEMP_GET:
        value = *(uint32 *)data; /* code */
        *(int32 *)data = (41004000 - (signed)(value*48705))/100;
        /* temper = 410.04 - (code * 0.48705);
         *        = (100000*410.04 - code * 48705)/100000
         *        = (41004000 - code*48705)/100000
         *        Above number represents Temperature in degrees
         *        To express temper in 1000th so multiply by 1000
         * temper = (41004000 - code*48705)/100; //in 1000 milli-Degree
         *
         *        When code = 0, temper = 410.04
         *        When code = 1023, temper = 41004000 - 49632891
         *                                 = -88.21
         */
        break;
    case SOC_AVS_CTRL_QUERY_TRACK_STEP_SIZE:
        /* 5 mV */
        *(uint32 *)data = 50;
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    return rv;
}

STATIC soc_error_t
_soc_th_avs_vcore_init(int unit)
{
    /* defined in avs_vctl.c */
    return soc_th_avs_vcore_init(unit);
}

STATIC soc_error_t
_soc_th_avs_vcore_deinit(int unit)
{
    /* defined in avs_vctl.c */
    return soc_th_avs_vcore_deinit(unit);
}

STATIC soc_error_t
_soc_th_avs_vcore_get(int unit, uint32 *voltage)
{
    /* defined in avs_vctl.c */
    return soc_th_avs_vcore_get(unit, voltage);
}

STATIC soc_error_t
_soc_th_avs_vcore_set(int unit, uint32 voltage)
{
    /* defined in avs_vctl.c */
    return soc_th_avs_vcore_set(unit, voltage);
}

STATIC soc_error_t
_soc_th_avs_init(int unit, soc_avs_info_t *avs_info)
{
    if (avs_info == NULL) {
        return (SOC_E_PARAM);
    }

    avs_info->num_centrals = SOC_AVS_TOMAHAWK_NUMBER_OF_CENTRALS;
    avs_info->cent_xbmp[0] = SOC_AVS_TOMAHAWK_CENTRAL_XBMP0;
    avs_info->cent_xbmp[1] = SOC_AVS_TOMAHAWK_CENTRAL_XBMP1;
    avs_info->first_cent = SOC_AVS_TOMAHAWK_FIRST_CENTRAL;

    avs_info->num_remotes = SOC_AVS_TOMAHAWK_NUMBER_OF_REMOTES;
    avs_info->rmt_xbmp[0] = SOC_AVS_TOMAHAWK_REMOTE_XBMP0;
    avs_info->rmt_xbmp[1] = SOC_AVS_TOMAHAWK_REMOTE_XBMP1;
    avs_info->rmt_xbmp[2] = SOC_AVS_TOMAHAWK_REMOTE_XBMP2;
    avs_info->first_rmt = SOC_AVS_TOMAHAWK_FIRST_REMOTE;


    avs_info->vmargin_low  = SOC_AVS_INT(SOC_AVS_TOMAHAWK_VMARGIN_LOW);
    avs_info->vmargin_high = SOC_AVS_INT(SOC_AVS_TOMAHAWK_VMARGIN_HIGH);
    
    avs_info->rosc_count_mode = SOC_AVS_ROSC_COUNT_MODE_1EDGE;
    avs_info->ref_clk_freq = 25;
    avs_info->measurement_time_control = 127;
    avs_info->osc_cen_init_info = _th_osc_cen_init_list;
    avs_info->osc_cen_thr_en_info = _th_osc_cen_thr_en_list;
    avs_info->osc_seq_reset_info = _th_osc_seq_reset_list;

    avs_info->avs_flags = SOC_AVS_INFO_F_SET_THRESHOLDS |
                          SOC_AVS_INFO_F_RSOC_THRESHOLD_CACHE ; 
        
	/* vmin_avs used for floor check */
    avs_info->vmin_avs = SOC_AVS_INT(SOC_AVS_TOMAHAWK_VMIN_AVS);
	/* vmax_avs used for ceiling check */
    avs_info->vmax_avs = SOC_AVS_INT(SOC_AVS_TOMAHAWK_VMAX_AVS);

#ifdef BCM_SBUSDMA_SUPPORT
    if (soc_feature(unit, soc_feature_sbusdma)) {
        avs_info->avs_flags |= SOC_AVS_INFO_F_RSOC_COUNT_DMA;
        avs_info->cen_osc_reg = AVS_REG_RO_REGISTERS_0_CEN_ROSC_STATUSr;
        avs_info->rmt_osc_reg = AVS_REG_PMB_SLAVE_AVS_ROSC_COUNTr;
    }
#endif

    return SOC_E_NONE;
}

STATIC soc_error_t
_soc_th_avs_deinit(int unit)
{
    /* voltage related deinit, if any */
    SOC_IF_ERROR_RETURN(
        _soc_th_avs_vcore_deinit(unit));

    return SOC_E_NONE;
}

soc_avs_functions_t _th_avs_functions;
soc_avs_vrm_access_t _th_avs_vrm_access;

soc_error_t
soc_th_avs_init(int unit)
{
    /* register avs functions*/
    memset(&_th_avs_functions, 0, sizeof(soc_avs_functions_t));
    _th_avs_functions.info_init = &_soc_th_avs_init;
    _th_avs_functions.info_deinit = &_soc_th_avs_deinit;
    _th_avs_functions.ioctl = &_soc_th_avs_ioctl;
    soc_avs_function_register(unit, &_th_avs_functions);

    memset(&_th_avs_vrm_access, 0, sizeof(soc_avs_vrm_access_t));
    _th_avs_vrm_access.vrm_init = &_soc_th_avs_vcore_init;
    _th_avs_vrm_access.voltage_set = &_soc_th_avs_vcore_set;
    _th_avs_vrm_access.voltage_get = &_soc_th_avs_vcore_get;
    _th_avs_vrm_access.vrm_deinit = &_soc_th_avs_vcore_deinit;
    soc_avs_vrm_access_func_register(unit, &_th_avs_vrm_access);

    return SOC_E_NONE;
}

#endif /* INCLUDE_AVS */
