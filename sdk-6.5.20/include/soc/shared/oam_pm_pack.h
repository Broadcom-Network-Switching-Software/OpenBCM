/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    oam_pm_pack.h
 * Purpose: OAM_PM definitions common to SDK and uKernel
 *
 * Notes:   Definition changes should be avoided in order to
 *          maintain compatibility between SDK and uKernel since
 *          both images are built and loaded separately.
 */

#ifndef _SOC_SHARED_OAM_PM_PACK_H
#define _SOC_SHARED_OAM_PM_PACK_H

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif


#define _PM_UC_MSG_TIMEOUT_USECS 5000000 

/*****************************************
 * OAM_PM uController Error codes
 */
typedef enum shr_oam_pm_uc_error_e {
    SHR_OAM_PM_UC_E_NONE = 0,
    SHR_OAM_PM_UC_E_INTERNAL,
    SHR_OAM_PM_UC_E_MEMORY,
    SHR_OAM_PM_UC_E_PARAM,
    SHR_OAM_PM_UC_E_RESOURCE,
    SHR_OAM_PM_UC_E_EXISTS,
    SHR_OAM_PM_UC_E_NOT_FOUND,
    SHR_OAM_PM_UC_E_UNAVAIL,
    SHR_OAM_PM_UC_E_VERSION,
    SHR_OAM_PM_UC_E_INIT
} shr_oam_pm_uc_error_t;

/*
 * Macro:
 *      SHR_OAM_PM_IF_ERROR_RETURN
 * Purpose:
 *      Evaluate _op as an expression, and if an error, return.
 * Notes:
 *      This macro uses a do-while construct to maintain expected
 *      "C" blocking, and evaluates "op" ONLY ONCE so it may be
 *      a function call that has side affects.
 */

#define SHR_OAM_PM_IF_ERROR_RETURN(op)                                     \
    do {                                                                \
        int __rv__;                                                     \
        if ((__rv__ = (op)) != SHR_OAM_PM_UC_E_NONE) {                     \
            return(__rv__);                                             \
        }                                                               \
    } while(0)


#define SHR_OAM_PM_MAX_PM_BIN_EDGES    7
/*
 * OAM_PM PM stats get
 */
typedef struct shr_oam_pm_msg_ctrl_pm_stats_get_s {
    uint32 flags;
    uint32 sess_id;    /* OAM_PM session (endpoint) id */
    uint32 far_loss_min; /* Minimum Frame loss at far-end */
    uint32 far_tx_min; /* Minimum Frames transmitted at far-end */
    uint32 far_loss_max; /* Maximum Frame loss at far-end */
    uint32 far_tx_max; /* Maximum Frames transmitted at far-end*/
    uint32 far_loss; /* Total far loss at far-end */
    uint32 near_loss_min; /* Minimum Frame loss at at near-end */
    uint32 near_tx_min; /* Minimum Frames transmitted at near-end */
    uint32 near_loss_max; /* Maximum Frame loss at near-end */
    uint32 near_tx_max; /* Maximum Frames transmitted at near-end */
    uint32 near_loss; /* Total near loss at near-end */
    uint32 lm_tx_count; /* Count of LM packets transmitted from local node */
                        /* during sampling interval*/
    uint32 DM_min; /* Minimum Frame pm_stats */
    uint32 DM_max; /* Maximum Frame pm_stats */
    uint32 DM_avg; /* Frame pm_stats */
    uint32 dm_tx_count; /* Count of DM packets transmitted from local node */
                        /* during sampling interval*/
    uint8 profile_id; /* Profile number associated with this session */
                      /* to measure the DM frames Delay */
                      /* which will fall into associated bins pm_stats range*/
    uint32 bin_counters[SHR_OAM_PM_MAX_PM_BIN_EDGES+1]; /* Bins Count of */
                                                     /*associated profile */
    uint32 lm_rx_count;                 /* Count of LM packets received in local
                                           node during sampling interval */
    uint32 dm_rx_count;                 /* Count of DM packets received in local
                                           node during sampling interval */
    uint32 far_total_tx_pkt_count;      /* Total Tx data packets at far-end */
    uint32 near_total_tx_pkt_count;     /* Total Tx data packets at near-end */

} shr_oam_pm_msg_ctrl_pm_stats_get_t;

# define SHR_OAM_PM_RAW_DATA_MAX_SAMPLES 10 /* Maximum number of samples in each buffer in Raw sampling mode */ 

typedef struct oam_pm_raw_counter_s { 
    uint32 tx_fcf; /* Count of packets transmitted local node */ 
    uint32 rx_fcf; /* Count of packets received by remote node */ 
    uint32 tx_fcb; /* Count of packets transmitted by remote node */ 
    uint32 rx_fcb; /* Count of packets received by local node */ 
} oam_pm_raw_counter_t; 

typedef struct oam_pm_raw_timestamp_s { 
    uint32 tx_tsf_secs; /* Time stamp upper of DM packets transmitted local node */ 
    uint32 tx_tsf_nsecs; /* Time stamp lower of DM packets transmitted local node */ 
    uint32 rx_tsf_secs; /* Time stamp upper of DM packets received by remote node */ 
    uint32 rx_tsf_nsecs; /* Time stamp lower of DM packets received by remote node */ 
    uint32 tx_tsb_secs; /* Time stamp upper of DM packets transmitted by remote node */ 
    uint32 tx_tsb_nsecs; /* Time stamp lower of DM packets transmitted by remote node */ 
    uint32 rx_tsb_secs; /* Time stamp upper of DM packets received by local node */ 
    uint32 rx_tsb_nsecs; /* Time stamp lower of DM packets received by local node */ 
} oam_pm_raw_timestamp_t; 

typedef struct oam_pm_raw_samples_s { 
    uint32 pm_id; /* Performance measurement ID in BTE*/
    uint32 lm_count; /* Number of LM samples collected as part of this buffer*/ 
    uint32 dm_count; /* Number of DM samples collected as part of this buffer*/ 
    oam_pm_raw_counter_t raw_counter[SHR_OAM_PM_RAW_DATA_MAX_SAMPLES]; /* LM counters collected as part of this buffer*/ 
    oam_pm_raw_timestamp_t raw_timestamp[SHR_OAM_PM_RAW_DATA_MAX_SAMPLES]; /* DM Timestamps collected as part of this buffer*/ 
} oam_pm_raw_sample_t; 

# define SHR_OAM_MAX_PM_ENDPOINTS 256

/*
 * OAM PM Raw data buffer object
 */

typedef struct shr_oam_pm_msg_ctrl_raw_buffer_s { 
    uint32 seq_num; /* Seq No. of the data buffer */ 
    oam_pm_raw_sample_t raw_sample[SHR_OAM_MAX_PM_ENDPOINTS]; /* Raw data buffer */ 
} shr_oam_pm_msg_ctrl_raw_buffer_t;

/*
 * OAM PM control messages
 */
typedef union shr_oam_pm_msg_ctrl_s {
    shr_oam_pm_msg_ctrl_raw_buffer_t raw_buffer;
} shr_oam_pm_msg_ctrl_t;

    
/*
 * OAM PM profile attach control message
 */ 
typedef struct shr_oam_pm_msg_ctrl_profile_attach_s {
    uint32  sess_id;
    uint32  profile_flags;
    uint8   profile_id;
    uint32  profile_edges[SHR_OAM_PM_MAX_PM_BIN_EDGES];
} shr_oam_pm_msg_ctrl_profile_attach_t;

extern uint8 *
shr_oam_pm_msg_ctrl_pm_stats_get_pack(uint8 *buf,
                   shr_oam_pm_msg_ctrl_pm_stats_get_t *msg);
extern uint8 *
shr_oam_pm_msg_ctrl_pm_stats_get_unpack(uint8 *buf,
                     shr_oam_pm_msg_ctrl_pm_stats_get_t *msg,
                     uint8 pm_stat_extra_elem_feature);
extern uint8 *
shr_oam_pm_msg_ctrl_profile_attach_pack(uint8 *buf,
                                 shr_oam_pm_msg_ctrl_profile_attach_t *msg);
extern uint8 *
shr_oam_pm_msg_ctrl_profile_attach_unpack(uint8 *buf,
                                 shr_oam_pm_msg_ctrl_profile_attach_t *msg);
extern uint8 *
shr_oam_pm_msg_ctrl_raw_buffer_unpack(uint8 *buf,
                                 shr_oam_pm_msg_ctrl_raw_buffer_t *msg);
#endif /* _SOC_SHARED_OAM_PM_PACK_H */
