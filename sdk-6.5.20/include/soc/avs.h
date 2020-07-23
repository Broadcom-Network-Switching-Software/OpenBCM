/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        avs.h
 * Purpose:
 */
#ifndef __SOC_AVS_H__
#define __SOC_AVS_H__

#ifdef INCLUDE_AVS
#include <soc/drv.h>
#include <soc/i2c.h>

#define SOC_AVS_NUM_CENT_XBMP 4
#define SOC_AVS_NUM_RMT_XBMP 8

typedef enum {
    SOC_AVS_BOOL_FALSE = 0,
    SOC_AVS_BOOL_TRUE = 1
} soc_avs_bool_t;


typedef enum {
 SOC_AVS_CTRL_OSC_INIT = 0,
 SOC_AVS_CTRL_OSC_THRESHOLD = 1,
 SOC_AVS_CTRL_SEQUENCER_INIT = 2,
 SOC_AVS_CTRL_PVTMON_INIT = 3,
 SOC_AVS_CTRL_OSC_MNTR_INIT = 4,
 SOC_AVS_CTRL_HW_MNTR_SW_CONTROL = 5,
 SOC_AVS_CTRL_QUERY_TRACK_STEP_SIZE = 6,
 SOC_AVS_CTRL_QUERY_NEW_THR_INC = 7,
 SOC_AVS_CTRL_LVM_MODE = 8,
 SOC_AVS_CTRL_PVTMON_TEMP_GET = 9,
 SOC_AVS_CTRL_PVTMON_VOLTAGE_GET = 10,
 SOC_AVS_CTRL_PVTMON_VOLTAGE_SET = 11
} soc_avs_ioctl_t;

typedef enum {
 SOC_AVS_PVT_TEMPERATURE = 0, /* Bit 0 - Temperature measurement */
 SOC_AVS_PVT_0P85V_0     = 1, /* Bit 1 - Voltage 0p85V<0> measurement */
 SOC_AVS_PVT_0P85V_1     = 2, /* Bit 2 - Voltage 0p85V<1> measurement */
 SOC_AVS_PVT_1V_0        = 3, /* Bit 3 - Voltage 1V<0> measurement */
 SOC_AVS_PVT_1V_1        = 4, /* Bit 4 - Voltage 1V<1> measurement */
 SOC_AVS_PVT_1P8V        = 5, /* Bit 5 - Voltage 1p8V measurement */
 SOC_AVS_PVT_3P3V        = 6, /* Bit 6 - Voltage 3p3V measurement */
 SOC_AVS_PVT_TESTMODE    = 7 /* Bit 7 - Testmode measurement */
} soc_avs_pvt_t;

typedef struct {
    soc_reg_t   reg_name;
    soc_field_t reg_field;
} soc_avs_reg_info_t;

typedef struct soc_avs_info_s {
    int first_cent;
    int num_centrals;
    uint32 cent_xbmp[SOC_AVS_NUM_CENT_XBMP];
    int first_rmt;
    int num_remotes;
    uint32 rmt_xbmp[SOC_AVS_NUM_RMT_XBMP];
    int vmin_avs;
    int vmax_avs;
    int vmargin_low;
    int vmargin_high;
    int rosc_count_mode;
    int ref_clk_freq; /* in MHz*/
    int measurement_time_control;
    soc_avs_reg_info_t *osc_cen_init_info;
    soc_avs_reg_info_t *osc_cen_thr_en_info;
    soc_avs_reg_info_t *osc_seq_reset_info;
    uint32 avs_flags;
    int rw_delta; /* in 0.1 mV */
#ifdef BCM_SBUSDMA_SUPPORT
    /* regitsers retrived from dma */
    soc_reg_t cen_osc_reg;
    soc_reg_t rmt_osc_reg;
#endif
} soc_avs_info_t;

typedef struct soc_avs_debug_xbmp_s{
    uint32 cent_xbmp[SOC_AVS_NUM_CENT_XBMP];
    uint32 rmt_xbmp[SOC_AVS_NUM_RMT_XBMP];
}soc_avs_debug_xbmp_t;

typedef struct soc_avs_debug_margin_s{
    int vmargin_low;
    int vmargin_high;
}soc_avs_debug_margin_t;

typedef struct soc_avs_functions_s{
    /* function required for close loop */
    int (*info_init)(int unit, soc_avs_info_t* avs_info ); /* initialization function */
    int (*info_deinit)(int unit); /* deinitialization function */
    int (*ioctl)(int unit, soc_avs_ioctl_t opcode, void* data, int len);
    /* function required for open loop */
    int (*openloop_voltage_get)(int unit, uint32 *voltage);
}soc_avs_functions_t;

typedef struct soc_avs_vrm_access_s {
    int (*vrm_init)(int unit);
    int (*vrm_deinit)(int unit);
    int (*voltage_get)(int unit, uint32 *voltage);
    int (*voltage_set)(int unit, uint32 voltage);
} soc_avs_vrm_access_t;

/* Use PMBUS device driver */
typedef struct soc_avs_vrm_profile_s {
    char    mux_name[SOC_I2C_DEV_NAME_MAX];
    char    vrm_name[SOC_I2C_DEV_NAME_MAX];
    uint8   mux_enable_value;
} soc_avs_vrm_profile_t;

typedef struct soc_avs_vrm_info_s {
    soc_avs_vrm_profile_t profile;
    int mux_id;
    int vrm_id;
} soc_avs_vrm_info_t;

typedef struct soc_avs_control_s {
    soc_avs_functions_t     *avs_functions;
    soc_avs_info_t          *avs_info;
    soc_avs_vrm_access_t    *vrm_access; /* use customer vrm driver */
    soc_avs_vrm_info_t      *vrm_info; /* use i2c pmbus driver */
    sal_mutex_t             avsMutex;
    sal_thread_t            avs_track_pid;
    char                    avs_track_name[16];
    sal_usecs_t             avs_track_interval;
    sal_sem_t               avs_track_notify;
#ifdef BCM_SBUSDMA_SUPPORT
    int                     rmt_rosc_count_sync;
    int                     cent_rosc_count_sync;
    uint32                  *rmt_desc_buff;
    uint32                  *cent_desc_buff;
#endif
    int                     *cen_freq_thr;
    int                     *rmt_freq_thr;
    int                     *cen_fth_slope;
    uint32                  *saved_voltage;
    int32                   slope_margin;
    int32                   intercept_margin;
    soc_avs_debug_xbmp_t    *avs_debug_xbmp;
    soc_avs_debug_margin_t  *avs_debug_margin;
    uint32                  v1r_cache;
    uint32                  *cent_v1s_osc_cache;
    uint32                  *rmt_v1s_h_osc_cache;
    uint32                  *rmt_v1s_s_osc_cache;
    uint32                  *cent_lo_thr_cache;
    uint32                  *cent_hi_thr_cache;
    uint32                  *rmt_h_thr_cache;
    uint32                  *rmt_s_thr_cache;
    int                      track_step_size; /* in 0.1 mv */
    int                      new_thr_inc; /* in 0.1 mv */
    int                      flags;
}soc_avs_control_t;

extern int soc_avs_function_register(int unit,
    soc_avs_functions_t *avs_functions);

#define SOC_AVS_CONTROL(unit)       (soc_avs_ctrl[unit])
#define SOC_AVS_FUNCTIONS(unit)     (SOC_AVS_CONTROL(unit)->avs_functions)
#define SOC_AVS_VRM_ACCESS(unit)    (SOC_AVS_CONTROL(unit)->vrm_access)
#define SOC_AVS_VRM_INFO(unit)      (SOC_AVS_CONTROL(unit)->vrm_info)
#define SOC_AVS_INFO(unit)          (SOC_AVS_CONTROL(unit)->avs_info)
#define SOC_AVS_XBMP(unit)          (SOC_AVS_CONTROL(unit)->avs_debug_xbmp)
#define SOC_AVS_MARGIN(unit)        (SOC_AVS_CONTROL(unit)->avs_debug_margin)

#define SOC_AVS_LOCK(s)         sal_mutex_take(s->avsMutex, sal_mutex_FOREVER)
#define SOC_AVS_UNLOCK(s)       sal_mutex_give(s->avsMutex)

/* unit in 0.1 mv*/
#define SOC_AVS_MAX_VOLTAGE 10500
#define SOC_AVS_MIN_VOLTAGE 8500
#define SOC_AVS_DEFAULT_VMIN_AVS 8500   /* 0.85 V*/
#define SOC_AVS_DEFAULT_VMAX_AVS 10200  /* 1.02 V*/
#define SOC_AVS_DEFAULT_VMARGIN_HIGH 0
#define SOC_AVS_DEFAULT_VMARGIN_LOW 0

#define SOC_AVS_ALL_ONES 0xFFFFFFFF

/* make this larger to get more precision in
 * the numbers (but be careful of overflow) */
#define SOC_AVS_SCALING_FACTOR 10000
/* WARNING: DO NOT CHANGE THIS -- it MUST match the definition used
 * to build the library */
#define SOC_AVS_S1 1
#define SOC_AVS_S2 SOC_AVS_SCALING_FACTOR

#define SOC_AVS_UINT(_x_) ((uint32)((_x_)*SOC_AVS_S1))
#define SOC_AVS_INT(_x_) ((int)((_x_)*SOC_AVS_S1))


#define SOC_AVS_ROSC_COUNT_MODE_1EDGE 1  /* 1 is for one-edge (only rise edge) */
#define SOC_AVS_ROSC_COUNT_MODE_2EDGES 0  /* 0 is for 2 edges (both rise, fall) */


#define SOC_AVS_F_INITED        (1 << 0)
#define SOC_AVS_F_TRACK_INITED  (1 << 1)
#define SOC_AVS_F_THRESHOLD_SET (1 << 2)
#define SOC_AVS_F_VRM_INITED    (1 << 3)

#define SOC_AVS_INFO_F_USE_SOFTWARE_TAKEOVER    (1 << 0)
#define SOC_AVS_INFO_F_USE_READ_PVTMON_VOLTAGE  (1 << 1)
#define SOC_AVS_INFO_F_USE_LVM_FLAG             (1 << 2)
#define SOC_AVS_INFO_F_USE_OTP_VTRAP_DATA       (1 << 3)
#define SOC_AVS_INFO_F_SET_THRESHOLDS           (1 << 4)
#define SOC_AVS_INFO_F_DO_NOT_ACCESS_XOSC       (1 << 5)
#define SOC_AVS_INFO_F_RSOC_COUNT_DMA           (1 << 6)
#define SOC_AVS_INFO_F_RSOC_THRESHOLD_CACHE     (1 << 7)

/* helper macro */
#define SOC_AVS_OSC_EXCLUDED(i,xbmp) (((((xbmp) >> ((i)%32)) & 0x1) == 0) ? \
        FALSE : TRUE)

#define SOC_AVS_FREE_IF_ERROR_RETURN(op,ptr) \
    do { int _rv; \
        if (SOC_FAILURE((_rv = (op)))) { \
            sal_free((ptr));\
            _SHR_ERROR_TRACE(_rv);\
            return(_rv);\
        }\
    } while(0)

#define SOC_AVS_IF_ERROR_NOT_UNAVAIL_RETURN(op)   \
    _SHR_E_IF_ERROR_NOT_UNAVAIL_RETURN(op)


#define SOC_AVS_MAX(a,b) ((a) > (b) ? (a) : (b))
#define SOC_AVS_MIN(a,b) ((a) < (b) ? (a) : (b))
#define SOC_AVS_ABS(x) (((x) >= 0)? (x) : -(x))
#define SOC_AVS_ASSERT(x) assert(x)

#define SOC_AVS_DAC_DELAY           100
#define SOC_AVS_REMOTE_DELAY        0
#define NUM_BITS_PER_XBMP           32
#define SOC_AVS_SEQUENCER_DELAY     32
#define SOC_AVS_PVT_DISABLE_DELAY  200
#define SOC_AVS_OPENLOOP_DELAY      100

#define SOC_AVS_ROSC_TYPE_CENTRAL   0
#define SOC_AVS_ROSC_TYPE_REMOTE    1
#define SOC_AVS_ROSC_TYPE_ALL       2

extern int soc_avs_init(int unit);
extern int soc_avs_deinit(int unit);
extern int soc_avs_start(int unit);
extern int soc_avs_xbmp_dump(int unit, int type);
extern int soc_avs_xbmp_set(int unit, int type, int start_osc,
    int num_osc, int value);
extern int soc_avs_voltage_set(int unit, uint32 voltage);
extern int soc_avs_voltage_get(int unit, uint32 *voltage);
extern int _soc_avs_predict_vpred(int unit, int pass, uint32 dac_code_low, uint32
        dac_code_high, uint32 *vpred, uint32 *vlow, uint32 *vhigh);
extern int _soc_avs_initialize_oscs(int unit);
extern int soc_avs_track_start(int unit, int interval);
extern int soc_avs_track_stop(int unit);
extern int soc_avs_track(int unit);
extern int soc_avs_info_dump(int unit);
extern int soc_avs_osc_count_dump(int unit, int type);
extern int soc_avs_debug_margin_get(int unit,
            soc_avs_debug_margin_t *avs_debug_margin);
extern int soc_avs_debug_margin_set(int unit,
            soc_avs_debug_margin_t *avs_debug_margin);
extern int soc_avs_temperature_get(int unit, int *temperature);
extern int soc_avs_pvtmon_voltage_get(int unit, uint32 *voltage);
extern int soc_avs_pvtmon_voltage_set(int unit, uint32 voltage);

extern int
soc_avs_vrm_access_func_register(int unit, soc_avs_vrm_access_t *vrm_access);
extern int soc_avs_openloop_main(int unit);
extern int soc_avs_vrm_profile_add(int unit, soc_avs_vrm_profile_t *vrm_profile);

extern int soc_avs_openloop_voltage_get(int unit, uint32 *voltage);
#endif /* INCLUDE_AVS */
#endif /* __SOC_AVS_H__ */
