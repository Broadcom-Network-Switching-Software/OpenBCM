/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * EYESCAN H
 */


#ifndef _SOC_EYESCAN_H_
#define _SOC_EYESCAN_H_

#include <soc/types.h>

#define SOC_PORT_PHY_EYESCAN_MAX_HORIZINAL_RESOLUTION 63
#define SOC_PORT_PHY_EYESCAN_MAX_VERTICAL_RESOLUTION 63 

#define SOC_PORT_PHY_EYESCAN_H_INDEX SOC_PORT_PHY_EYESCAN_MAX_HORIZINAL_RESOLUTION / 2
#define SOC_PORT_PHY_EYESCAN_V_INDEX SOC_PORT_PHY_EYESCAN_MAX_VERTICAL_RESOLUTION / 2

#define SRD_EYESCAN_INVALID 0xffffffff
#define SRD_EYESCAN_INVALID_VOFFSET 0x7fffffff

#define SRD_EYESCAN_FLAG_VERTICAL_ONLY 0x1
#define SRD_EYESCAN_FLAG_AVOID_EXTRAPOLATION 0x2
#define SRD_EYESCAN_FLAG_DSC_PRINT 0x4

#define TIME_MULTIPLIER 2

/* #define BER_EXTRAP_SPD_UP */
#define MAX_EYE_LOOPS 49
#define HI_CONFIDENCE_ERR_CNT 100    /* bit errors to determine high confidence */
#define LO_CONFIDENCE_MIN_ERR_CNT 10 /* bit errors, exit condition for low confidence */
#define HI_CONFIDENCE_MIN_ERR_CNT 20 /* bit errors exit condition for high confidence */
#define INDEX_UNINITIALIZED  (-1)
#define VEYE_UNIT 1.75
#define EYESCAN_UTIL_MAX_ROUND_DIGITS (8) 

/*Eyescan test bounds*/
typedef struct soc_stat_resolution_bounds_e { 
    int horizontal_min; /* Typically 0 */
    int horizontal_max; 
    int vertical_min; /* Ignored */
    int vertical_max; /* Actual movement is [-max;max]*/
} soc_port_phy_eye_bounds_t; 

/*Select eyescan errors counting method*/
typedef enum soc_stat_eyscan_counter_e { 
    socPortPhyEyescanCounterRelativePhy = 0, 
    socPortPhyEyescanCounterPrbsPhy,
    socPortPhyEyescanCounterPrbsMac, 
    socPortPhyEyescanCounterCrcMac, 
    socPortPhyEyescanCounterBerMac, 
    socPortPhyEyescanCounterCustom,
    socPortPhyEyescanNofCounters /*last*/
} soc_port_phy_eyscan_counter_t;

/*Eyescan test parameters*/
typedef struct soc_stat_eyescan_params_s { 
    int sample_time; /*milisec*/ 
    int sample_resolution; /*interval between two samples*/ 
    soc_port_phy_eye_bounds_t bounds; 
    soc_port_phy_eyscan_counter_t counter;
    int error_threshold; /*minimum error threshold required, otherwise increase sample time*/
    int time_upper_bound; /*maximum sample time*/
    int nof_threshold_links; /*num of links that should be below error_threshold before increasing time*/
    int type;
    int ber_proj_scan_mode;  /* BER proj Controls the direction and polarity of the test. */
    int ber_proj_timer_cnt;  /* BER proj total measurement time in units of ~1.3 seconds. */
    int ber_proj_err_cnt;    /* BER proj the error threshold it uses to step to next measurement in units of 16 */
} soc_port_phy_eyescan_params_t;

/*Eyescan test results*/
typedef struct soc_stat_eyescan_results_s { 
    uint32 error_count[SOC_PORT_PHY_EYESCAN_MAX_HORIZINAL_RESOLUTION][SOC_PORT_PHY_EYESCAN_MAX_VERTICAL_RESOLUTION];   
    uint32 run_time[SOC_PORT_PHY_EYESCAN_MAX_HORIZINAL_RESOLUTION][SOC_PORT_PHY_EYESCAN_MAX_VERTICAL_RESOLUTION]; 
    int ext_results_int;
    int ext_results_remainder;
    int ext_better;     /* 1 for better than, 0 for exact, -1 for worse than */
    int ext_done;
} soc_port_phy_eyescan_results_t;

int soc_port_phy_eyescan_run( 
    int unit, 
	uint32 inst,
    int flags, 
    soc_port_phy_eyescan_params_t* params, 
    uint32 nof_ports, 
    soc_port_t* ports,
    int* lane_num,
    soc_port_phy_eyescan_results_t* results /*array of result per port*/);

typedef int (*port_phy_eyescan_counter_clear_f) (int unit, soc_port_t port);
typedef int (*port_phy_eyescan_counter_get_f) (int unit, soc_port_t port, uint32* err_count);

typedef struct soc_port_phy_eyescan_counter_cb_s {
    port_phy_eyescan_counter_clear_f clear_func;
    port_phy_eyescan_counter_get_f  get_func;
}soc_port_phy_eyescan_counter_cb_t;

typedef struct {
    int total_errs[MAX_EYE_LOOPS];
    int total_elapsed_time[MAX_EYE_LOOPS];
    int mono_flags[MAX_EYE_LOOPS];
    int offset_max;
    int veye_cnt;
    uint32 rate;      /* frequency in KHZ */
    int first_good_ber_idx;
    int first_small_errcnt_idx;
} EYE_DIAG_INFOt;

int 
soc_port_phy_eyescan_counter_register(
    int unit, 
    soc_port_phy_eyscan_counter_t counter, 
    soc_port_phy_eyescan_counter_cb_t* cf);

int soc_port_phy_eyescan_extrapolate(
    int unit, 
    int flags, 
    soc_port_phy_eyescan_params_t* params, 
    uint32 nof_ports, 
    soc_port_t* ports,
    soc_port_phy_eyescan_results_t* results /*array of result per port*/);

#ifdef PHYMOD_SUPPORT
extern int is_eyescan_algorithm_legacy_mode(int unit, uint32 nof_ports, soc_port_t* ports, uint32 inst);
extern int is_eyescan_algorithm_legacy_rpt_mode(int unit, uint32 nof_ports, soc_port_t* ports, uint32 inst);
#endif /* PHYMOD_SUPPORT */
#endif   /* _SOC_EYESCAN_H_  */

