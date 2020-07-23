/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __BCM_INT_FP_OAM_H__
#define __BCM_INT_FP_OAM_H__

#include "soc/shared/ccm.h"
#include "soc/shared/ccm_msg.h"

#include "bcm_int/esw/bhh.h"

#if defined(INCLUDE_MPLS_LM_DM)
#include "bcm_int/esw/mpls_lm_dm.h"
#endif /* INCLUDE_MPLS_LM_DM */

/* Direction + isLocal + Group ID + Endpoint Name + Level + Gport + 2VIDs */
#define _FP_OAM_HASH_KEY_SIZE ( sizeof(uint8)                   +   \
                                sizeof(uint8)                   +   \
                                sizeof(bcm_oam_group_t)         +   \
                                sizeof(uint16)                  +   \
                                sizeof(int)                     +   \
                                sizeof(bcm_gport_t)             +   \
                                sizeof(bcm_vlan_t)              +   \
                                sizeof(bcm_vlan_t) )

typedef uint8 _bcm_fp_oam_hash_key_t[_FP_OAM_HASH_KEY_SIZE];



/*
 * Typedef:
 *     _bcm_fp_oam_group_data_t
 * Purpose:
 *     Group information.
 */
typedef struct _bcm_fp_oam_group_data_s
{
    int                 in_use; /* Group status */
    ccm_group_data_t    group_data; /* Group data stores on UK */
} _bcm_fp_oam_group_data_t;


/*
 * Typedef:
 *     _bcm_fp_oam_hash_data_t
 * Purpose:
 *     Endpoint hash table data structure.
 */
typedef struct _bcm_fp_oam_mep_data_s
{
    int in_use;                         /* Endpoint status */
    ccm_mep_data_t        mep_data;/* CCM Data Stored on UK */
} _bcm_fp_oam_mep_data_t;


/*
 * Typedef:
 *     _bcm_fp_oam_control_t
 * Purpose:
 *     OAM module control structure. One structure for each XGS device.
 */
typedef struct _bcm_fp_oam_control_s {

    int                         init;           /* TRUE if OAM module has */
                                                /* been initialized */
    int                         unit;           /* oc unit number */
    sal_mutex_t                 oc_lock;        /* Protection mutex */

    uint32                      group_count;    /* Total groups count */
    uint32                      mep_count;      /* Total mep count */

    /* Group Data */
    shr_idxres_list_handle_t    group_pool;     /* Group indices pool */
    _bcm_fp_oam_group_data_t    *group_data;    /* Group Data */

    /* LMEP Data */
    shr_idxres_list_handle_t    mep_pool;       /* LMEP indices pool */
    _bcm_fp_oam_mep_data_t      *mep_data;      /* LMEP Data */
    shr_htb_hash_table_t        mep_htbl;       /* LMEP+RMEP hash table */

    /* CCM Event handling data elements */
    uint32                      event_mask;     /* Event mask */
    _bcm_oam_event_handler_t    *event_handler_list_p; /* Event handlers LL */
    int                         event_handler_cnt[bcmOAMEventCount];
    sal_thread_t                event_thread_id; /* Event thread Prio */

    /* CCM UC Messaging data elements */
    int                         ccm_uc_num;     /* UC Num for CCM App */
    uint8                       *dma_buffer;    /* DMA Buffer for F/w Msging */
    uint32                      dma_buffer_len; /* Max DMA Msg size */
    int                         ccm_ukernel_ready; /* UKERNEL ready or not */

#if defined(INCLUDE_BHH)
    shr_idxres_list_handle_t    bhh_group_pool; /* BHH Group indices pool */
    _bhh_fp_oam_group_data_t    *bhh_group_data;/* BHH Group Data */
    _bhh_fp_oam_ep_data_t       *bhh_ep_data;   /* BHH EP Data */
    shr_htb_hash_table_t        bhh_mep_htbl;   /* BHH MEP hash table */
    shr_idxres_list_handle_t    bhh_pool;       /* BHH endpoint indices pool. */
    int                         rx_channel;     /* Local RX DMA channel for   */
                                                /*                BHH packets */
    int                         bhh_uc_num;     /* uController number running */
                                                /*                   BHH appl */
    int                         bhh_dma_buffer_len;/* DMA max buffer size     */
    uint8*                      bhh_dma_buffer; /* DMA buffer                 */
    uint8*                      bhh_dmabuf_reply;/* DMA reply buffer          */
    int                         bhh_endpoint_count;/*Number of BHH sessions on*/
                                                   /*  FW*/
    int                         bhh_base_group_id; /* BHH base group ID*/ 
    int                         bhh_base_endpoint_id;/* BHH base endpoint ID*/  
    sal_thread_t                bhh_event_thread_id;/* Event handler thread id*/
    uint32                      bhh_event_mask;/*Events registered with uKernel*/
    int                         ukernel_not_ready;/* UKERNEL ready or not     */
    uint16                      bhh_max_encap_length;
    uint8                       pm_bhh_lmdm_data_collection_mode;/*PM mode */
    uint32                      pm_bhh_raw_data_num_buffers;/* Num of raw data*/
                                                            /* buffers */
    int                         pm_bhh_dma_buffer_len; /* DMA max buffer size */
    uint8*                      pm_bhh_dma_buffer;     /* DMA buffer */
    bcm_oam_pm_raw_data_t       pm_bhh_raw_data_info;
#endif /* INCLUDE_BHH */

#if defined(INCLUDE_MPLS_LM_DM)
    int                         mpls_lm_dm_dma_buffer_len;/* DMA max buffer size */
    uint8*                      mpls_lm_dm_dma_buffer; /* DMA buffer */
    uint8                       mpls_lm_dm_uc_num; /* uC running MPLS_LM_DM app */
    uint8                       mpls_lm_dm_ukernel_ready; /* UC state */
    uint8                       mpls_lm_dm_rx_channel;/* Local RX DMA channel
                                                       for MPLS_LM_DM packets */
    uint16                      mpls_lm_dm_max_ep_count; /* MAX number of EP */
    bcm_oam_endpoint_t          mpls_lm_dm_base_endpoint_id;/* MPLS LM/DM base
                                                               endpoint ID */
    shr_idxres_list_handle_t    mpls_lm_dm_pool; /* MPLS_LM_DM session-Id pool */
    _bcm_fp_oam_mpls_lm_dm_ep_data_t *mpls_lm_dm_ep_data; /* EP data ptr */
    uint8                       pm_mpls_lm_dm_data_collection_mode;/*PM mode */
    uint32                      pm_mpls_lm_dm_raw_data_num_buffers; /* Num of raw data buffers */
    bcm_oam_pm_raw_data_t       pm_mpls_lm_dm_raw_data_info;
    sal_thread_t                mpls_lm_dm_pm_event_thread_id;
#endif /* INCLUDE_MPLS_LM_DM */
#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
    _bcm_oam_pm_event_handler_t *pm_event_handler_list_p;
    int                         pm_read_done_index[3];
    int                         pm_write_done_index[3];
    int                         pm_num_free_raw_data_buffers[3];
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */
} _bcm_fp_oam_control_t;


#endif /* !__BCM_INT_FP_OAM_H__ */
