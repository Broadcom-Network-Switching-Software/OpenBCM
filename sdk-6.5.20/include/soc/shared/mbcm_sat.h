/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mbcm_sat.h
 * Purpose:     Multiplexing of the bcm layer for PP
 *
 * Different chip families require such different implementations
 * of some basic BCM layer functionality that the functions are
 * multiplexed to allow a fast runtime decision as to which function
 * to call.  This file contains the basic declarations for this
 * process.
 * 
 *
 * See internal/design/soft_arch/xgs_plan.txt for more info.
 *
 * Conventions:
 *    MBCM_SAT is the multiplexed pp bcm prefix
 *    _f is the function type declaration postfix
 */

#ifndef _SOC_SAT_MBCM_H
#define _SOC_SAT_MBCM_H

#include <bcm/types.h>
#include <soc/error.h>
#include <soc/shared/sat.h>
#ifdef BCM_SAT_SUPPORT

/*
 * the basic SOC_SAND_RET for error
 */
#ifndef SOC_SAND_ERR 
#define SOC_SAND_ERR 1
#endif
/*
 * the basic SOC_SAND_RET for success
 */
#ifndef SOC_SAND_OK
#define SOC_SAND_OK  0
#endif

/****************************************************************
 *
 * SOC Family type:
 */
typedef enum soc_sat_chip_family_e {
    SOC_SAT_FAMILY_SABER2,
    SOC_SAT_FAMILY_JERICHO
} soc_sat_chip_family_t;


/****************************************************************
 *
 * Type definitions for multiplexed BCM functions.
 */
typedef soc_error_t
    (*mbcm_sat_config_set_f) (
        SOC_SAND_IN int unit,
        SOC_SAND_IN soc_sat_config_t* conf
    );

typedef soc_error_t
    (*mbcm_sat_config_get_f) (
        SOC_SAND_IN int unit,
        SOC_SAND_OUT soc_sat_config_t* conf
    );

typedef soc_error_t
    (*mbcm_sat_gtf_create_f) (
        SOC_SAND_IN         int            unit,
        SOC_SAND_IN         uint32         flags,
        SOC_SAND_INOUT      soc_sat_gtf_t *gtf_id
    );

typedef soc_error_t
    (*mbcm_sat_gtf_destroy_f) (
        SOC_SAND_IN int unit,
	    SOC_SAND_IN soc_sat_gtf_t gtf_id
	 );

typedef soc_error_t
    (*mbcm_sat_gtf_destroy_all_f) (
        SOC_SAND_IN int unit
    );

typedef soc_error_t
    (*mbcm_sat_general_cfg_init_f) (
     SOC_SAND_IN int unit,
     SOC_SAND_IN soc_sat_init_t *sat_init
);

typedef soc_error_t
    (*mbcm_sat_gtf_packet_gen_set_f) (
     SOC_SAND_IN int unit, 
	 SOC_SAND_IN soc_sat_gtf_t gtf_id,
     SOC_SAND_IN int priority,
	 SOC_SAND_IN int enable
);

typedef soc_error_t
    (*mbcm_sat_gtf_packet_gen_get_f) (
     SOC_SAND_IN int unit, 
	 SOC_SAND_IN soc_sat_gtf_t gtf_id,
     SOC_SAND_IN int priority,
	 SOC_SAND_OUT int *enable
);

typedef soc_error_t
    (*mbcm_sat_gtf_packet_config_f) (
        SOC_SAND_IN int unit,
	    SOC_SAND_IN soc_sat_gtf_t gtf_id, 
        SOC_SAND_IN soc_sat_gtf_packet_config_t *config
     );

typedef soc_error_t
    (*mbcm_sat_gtf_bandwidth_set_f) (
     SOC_SAND_IN int unit, 
	 SOC_SAND_IN soc_sat_gtf_t gtf_id,	 
	 SOC_SAND_IN int priority,
     SOC_SAND_IN soc_sat_gtf_bandwidth_t *bw
);

typedef soc_error_t
    (*mbcm_sat_gtf_bandwidth_get_f) (
     SOC_SAND_IN int unit, 
	 SOC_SAND_IN soc_sat_gtf_t gtf_id,	 
	 SOC_SAND_IN int priority,
     SOC_SAND_OUT soc_sat_gtf_bandwidth_t *bw
);

typedef soc_error_t
    (*mbcm_sat_gtf_rate_pattern_set_f) (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_gtf_t gtf_id,
	SOC_SAND_IN int priority,
	SOC_SAND_IN soc_sat_gtf_rate_pattern_t *config
);

typedef soc_error_t
    (*mbcm_sat_gtf_rate_pattern_get_f) (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_gtf_t gtf_id,
	SOC_SAND_IN int priority,
	SOC_SAND_OUT soc_sat_gtf_rate_pattern_t *config
);

typedef soc_error_t
    (*mbcm_sat_gtf_stat_get_f) (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_gtf_t gtf_id,
	SOC_SAND_IN int priority,
	SOC_SAND_IN uint32 flags,
	SOC_SAND_IN soc_sat_gtf_stat_counter_t type,
	SOC_SAND_OUT uint64* value
);

typedef soc_error_t
    (*mbcm_sat_ctf_packet_config_f) (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_t ctf_id,
	SOC_SAND_IN soc_sat_ctf_packet_info_t *packet_info
);

typedef soc_error_t
    (*mbcm_sat_ctf_identifier_map_f) (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_identifier_t *identifier,
	SOC_SAND_IN soc_sat_ctf_t ctf_id
);

typedef soc_error_t 
    (*mbcm_sat_ctf_identifier_unmap_f) (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_identifier_t *identifier
);

typedef soc_error_t
    (*mbcm_sat_ctf_trap_set_f) (
	SOC_SAND_IN int unit, 
	SOC_SAND_IN uint32 trap_id_cnt,
	SOC_SAND_IN uint32* trap_id
);

typedef soc_error_t 
    (*mbcm_sat_ctf_bin_limit_set_f)
    (
	SOC_SAND_IN int unit, 
	SOC_SAND_IN int bins_count, 
 	SOC_SAND_IN soc_sat_ctf_bin_limit_t* bins
);

typedef soc_error_t
    (*mbcm_sat_ctf_bin_limit_get_f)
    (
	SOC_SAND_IN int unit, 
	SOC_SAND_IN int max_bins_count,
	SOC_SAND_OUT int * bins_count, 
 	SOC_SAND_OUT soc_sat_ctf_bin_limit_t* bins
);

typedef soc_error_t
    (*mbcm_sat_ctf_stat_config_set_f)
    (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_t ctf_id,
	SOC_SAND_IN soc_sat_ctf_stat_config_t *stat
);

typedef soc_error_t
    (*mbcm_sat_ctf_stat_get_f) (
	SOC_SAND_IN int unit, 
	SOC_SAND_IN soc_sat_ctf_t ctf_id,
	SOC_SAND_IN uint32 flags,
	SOC_SAND_OUT soc_sat_ctf_stat_t* stat
);

typedef soc_error_t
    (*mbcm_sat_ctf_availability_config_set_f)
    (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_t ctf_id,
	SOC_SAND_IN soc_sat_ctf_availability_config_t *config
	);

typedef soc_error_t 
    (*mbcm_sat_ctf_trap_data_to_session_map_f) (
	SOC_SAND_IN int unit,
	SOC_SAND_IN uint32 trap_data, 
	SOC_SAND_IN uint32 trap_data_mask,
	SOC_SAND_IN uint32 entry_idx,
	SOC_SAND_IN uint32 session_id
);

typedef soc_error_t 
    (*mbcm_sat_ctf_trap_data_to_session_unmap_f) (
	SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 entry_idx
); 

typedef soc_error_t 
    (*mbcm_sat_ctf_reports_config_set_f) (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_t ctf_id,
	SOC_SAND_IN soc_sat_ctf_report_config_t *reports
);

typedef soc_error_t
    (*mbcm_sat_oamp_stat_event_control_f) (
	SOC_SAND_IN int unit,
	SOC_SAND_IN int enable
);

typedef soc_error_t
    (*mbcm_sat_oamp_stat_event_clear_f) (
	SOC_SAND_IN int unit
);

typedef int (*mbcm_sat_session_inuse_f)(
    SOC_SAND_IN int unit,
               SOC_SAND_IN int session_id);


/****************************************************************
 *
 * Multiplexed BCM SOC Driver Structure
 */
typedef struct mbcm_sat_functions_s {
    mbcm_sat_config_set_f                     mbcm_sat_config_set;
    mbcm_sat_config_get_f                     mbcm_sat_config_get;
    /* GTF functions */
    mbcm_sat_general_cfg_init_f               mbcm_sat_general_cfg_init;
    mbcm_sat_gtf_packet_gen_set_f             mbcm_sat_gtf_packet_gen_set;
    mbcm_sat_gtf_packet_gen_get_f             mbcm_sat_gtf_packet_gen_get;
    mbcm_sat_gtf_packet_config_f              mbcm_sat_gtf_packet_config;
    mbcm_sat_gtf_bandwidth_set_f              mbcm_sat_gtf_bandwidth_set;
    mbcm_sat_gtf_bandwidth_get_f              mbcm_sat_gtf_bandwidth_get;
    mbcm_sat_gtf_rate_pattern_set_f           mbcm_sat_gtf_rate_pattern_set;
    mbcm_sat_gtf_rate_pattern_get_f           mbcm_sat_gtf_rate_pattern_get;
    mbcm_sat_gtf_stat_get_f                   mbcm_sat_gtf_stat_get;
    /* CTF functions */
    mbcm_sat_ctf_packet_config_f              mbcm_sat_ctf_packet_config;
    mbcm_sat_ctf_identifier_map_f             mbcm_sat_ctf_identifier_map;
    mbcm_sat_ctf_identifier_unmap_f           mbcm_sat_ctf_identifier_unmap;
    mbcm_sat_ctf_trap_set_f                   mbcm_sat_ctf_trap_set;
    mbcm_sat_ctf_bin_limit_set_f              mbcm_sat_ctf_bin_limit_set;
    mbcm_sat_ctf_bin_limit_get_f              mbcm_sat_ctf_bin_limit_get;
    mbcm_sat_ctf_stat_config_set_f            mbcm_sat_ctf_stat_config_set;
    mbcm_sat_ctf_stat_get_f                   mbcm_sat_ctf_stat_get;
    mbcm_sat_ctf_availability_config_set_f    mbcm_sat_ctf_availability_config_set;
    mbcm_sat_ctf_trap_data_to_session_map_f   mbcm_sat_ctf_trap_data_to_session_map;
    mbcm_sat_ctf_trap_data_to_session_unmap_f mbcm_sat_ctf_trap_data_to_session_unmap;
    mbcm_sat_ctf_reports_config_set_f         mbcm_sat_ctf_reports_config_set;
    /* OAMP inerrupt */
    mbcm_sat_oamp_stat_event_control_f        mbcm_sat_oamp_stat_event_control;
    mbcm_sat_oamp_stat_event_clear_f          mbcm_sat_oamp_stat_event_clear;
    mbcm_sat_session_inuse_f                  mbcm_sat_session_inuse;
} mbcm_sat_functions_t; 


extern CONST mbcm_sat_functions_t *mbcm_sat_driver[BCM_MAX_NUM_UNITS]; /* USE _CALL MACRO FOR DISPATCH */
extern soc_sat_chip_family_t mbcm_sat_family[BCM_MAX_NUM_UNITS];

extern int mbcm_sat_init(int unit);

/* The argumnet should be in parentheses and to include the unit, i.e. (unit, arg1, arg2, ...) */
#define MBCM_SAT_DRIVER_CALL(unit, function, args) (((unit < BCM_MAX_NUM_UNITS) && mbcm_sat_driver[unit] && mbcm_sat_driver[unit]->function)?mbcm_sat_driver[unit]->function args:BCM_E_UNAVAIL)
#endif /* BCM_SAT_SUPPORT */
#endif /*_SOC_SAT_MBCM_H*/
