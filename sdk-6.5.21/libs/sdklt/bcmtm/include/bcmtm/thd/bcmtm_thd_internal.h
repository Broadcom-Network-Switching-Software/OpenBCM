/*! \file bcmtm_thd_internal.h
 *
 * APIs, definitions for THD internal usage.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_THD_INTERNAL_H
#define BCMTM_THD_INTERNAL_H

/*! MMU PG buffer config. */
typedef struct soc_mmu_cfg_buf_prigroup_s {
    /*! Pool mapped to PG. */
    int pool_idx; /* from mapping variable */
    /*! PG min limit. */
    int guarantee;
    /*! PG headroom limit. */
    int headroom;
    /*! Enable PG to use port min limit. */
    int port_guarantee_enable;
    /*! Enable port shared limit. */
    int port_max_enable;
    /*! Dynamic shared limit alpha. */
    int pool_scale;
    /*! Static shared limit. */
    int pool_limit;
    /*! Resume limit. */
    int pool_resume;
    /*! PG floor reset limit. */
    int pool_floor;
    /*! Enable flowcontrol. */
    int flow_control_enable;
    /*! PG is lossless. */
    int lossless;
} soc_mmu_cfg_buf_prigroup_t;

/*! MMU queue group buffer thresholds. */
typedef struct soc_mmu_cfg_buf_qgroup_s
{
    /*! UC queue group min limit. */
    int guarantee;
    /*! MC queue group min limit. */
    int guarantee_mc;
} soc_mmu_cfg_buf_qgroup_t;

/*! MMU queue buffer thresholds. */
typedef struct soc_mmu_cfg_buf_queue_s
{
    /*! Number of queues. */
    int numq;
    /*! Pool mapped to the queue. */
    int pool_idx; /* from mapping variable */
    /*! Min limit. */
    int guarantee;
    /*! Limit enable. */
    int discard_enable;
    /*! Dynamic shared limit/alpha. */
    int pool_scale;
    /*! Static shared limit. */
    int pool_limit;
    /*! Resume limit. */
    int pool_resume;
    /*! Color limit enable. */
    int color_discard_enable;
    /*! Yellow limit. */
    int yellow_limit;
    /*! Red limit. */
    int red_limit;
    /*! Yellow resume limit. */
    int yellow_resume;
    /*! Red resume limit. */
    int red_resume;
    /*! Enable qgroup min. */
    int qgroup_min_enable;
    /*! Enable dynamic limit. */
    int use_dyn_limit;
    /*! Enable color dynamic limit. */
    int color_limit_dyn;
} soc_mmu_cfg_buf_queue_t;

/*! MMU pool buffer thresholds. */
typedef struct soc_mmu_cfg_buf_pool_s {
    /*! Pool shared limit. */
    int size;
    /*! Total buffer space. */
    int total; /* calculated value (not from config variable) */
    /*! Sum of port mins. */
    int port_guarantee; /* calculated value (not from config variable) */
    /*! Sum of PG mins. */
    int prigroup_guarantee; /* calculated value (not from config variable) */
    /*! Sum of PG headroom. */
    int prigroup_headroom; /* calculated value (not from config variable) */
    /*! Sum of queue mins. */
    int queue_guarantee; /* calculated value (not from config variable) */
    /*! Sum of qgroup mins. */
    int queue_group_guarantee;/* calculated value (not from config variable) */
    /*! Sum of mc qgroup min. */
    int mcq_entry_reserved;
    /*! total MCqueue entry space. */
    int total_mcq_entry;
} soc_mmu_cfg_buf_pool_t;


/*! MMU port SP buffer thresholds. */
typedef struct soc_mmu_cfg_buf_port_pool_s {
    /*! Port SP min. */
    int guarantee;
    /*! Shared limit. */
    int pool_limit;
    /*! Resume limit. */
    int pool_resume;
} soc_mmu_cfg_buf_port_pool_t;

/*! MMU device information. */
typedef struct soc_mmu_device_info_s {
    /*! Flags. */
    int  flags;
    /*! Maximum packet size in bytes. */
    int  max_pkt_byte;
    /*! Packet header size in bytes. */
    int  mmu_hdr_byte;
    /*! Jumbo packet size in bytes. */
    int  jumbo_pkt_size;
    /*! Default MTU packet size in bytes. */
    int  default_mtu_size;
    /*! Total number of cells in MMU buffer. */
    int  mmu_total_cell;
    /*! MMU cell size in bytes. */
    int  mmu_cell_size;
    /*! Number of PGs. */
    int  num_pg;
    /*! Number of service pools. */
    int  num_service_pool;
    /*! Number of entries in MCQ engine. */
    int  total_mcq_entry;
    /*! Number of RQE queues. */
    int  rqe_queue_num;
} soc_mmu_device_info_t;

/*! MMU buffer update reason. */
typedef enum shared_buffer_update_reason_e {
    RESERVED_BUFFER_UPDATE  = 0,
    ING_SERVICE_POOL_UPDATE = 1,
    EGR_SERVICE_POOL_UPDATE = 2
} shared_buffer_update_reason_t;


#endif /* BCMTM_THD_INTERNAL_H */
