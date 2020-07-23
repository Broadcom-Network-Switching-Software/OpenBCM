/*! \file bcm56780_a0_tm_thd_internal.h
 *
 * File containing macro defines used in mmu threshold init sequence for
 * bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_TM_THD_INTERNAL_H
#define BCM56880_A0_TM_THD_INTERNAL_H


/* Alpha values for dynamic threshold. */
typedef enum soc_td4_x9_drop_limit_alpha_value_e {
    SOC_TD4_X9_COSQ_DROP_LIMIT_ALPHA_1_128, /* 1/128 */
    SOC_TD4_X9_COSQ_DROP_LIMIT_ALPHA_1_64,  /* 1/64 */
    SOC_TD4_X9_COSQ_DROP_LIMIT_ALPHA_1_32,  /* 1/32 */
    SOC_TD4_X9_COSQ_DROP_LIMIT_ALPHA_1_16,  /* 1/16 */
    SOC_TD4_X9_COSQ_DROP_LIMIT_ALPHA_1_8,   /* 1/8 */
    SOC_TD4_X9_COSQ_DROP_LIMIT_ALPHA_1_4,   /* 1/4 */
    SOC_TD4_X9_COSQ_DROP_LIMIT_ALPHA_1_2,   /* 1/2 */
    SOC_TD4_X9_COSQ_DROP_LIMIT_ALPHA_1,     /* 1 */
    SOC_TD4_X9_COSQ_DROP_LIMIT_ALPHA_2,     /* 2 */
    SOC_TD4_X9_COSQ_DROP_LIMIT_ALPHA_4,     /* 4 */
    SOC_TD4_X9_COSQ_DROP_LIMIT_ALPHA_8,     /* 8 */
    SOC_TD4_X9_COSQ_DROP_LIMIT_ALPHA_COUNT  /* Must be last! */
} soc_td4_x9_drop_limit_alpha_value_t;

#define MMU_CFG_BUF_DYNAMIC_FLAG        (0x80000000)
#define MMU_CFG_BUF_PERCENT_FLAG        (0x40000000)

#define SOC_MMU_CFG_SERVICE_POOL_MAX    4
#define SOC_MMU_CFG_PORT_MAX            80
#define SOC_MMU_CFG_PRI_GROUP_MAX       8
#define SOC_MMU_CFG_INT_PRI_MAX         16
#define SOC_MMU_CFG_QGROUP_MAX          256
#define SOC_MMU_CFG_EQUEUE_MAX          512
#define SOC_MMU_CFG_RQE_QUEUE_MAX       11
#define SOC_MMU_CFG_XPE_MAX             4


#define SOC_MMU_CFG_ITM_MAX             1
#define SOC_MMU_CFG_NUM_PROFILES_MAX    8
#define TD4_X9_ITMS_PER_DEV 1

typedef struct soc_mmu_cfg_buf_mapping_profile_s
{
     int inpri_to_prigroup_uc[SOC_MMU_CFG_INT_PRI_MAX];
     int inpri_to_prigroup_mc[SOC_MMU_CFG_INT_PRI_MAX];
     int prigroup_to_servicepool[SOC_MMU_CFG_PRI_GROUP_MAX];
     int prigroup_to_headroompool[SOC_MMU_CFG_PRI_GROUP_MAX];
     int pfcpri_to_prigroup[SOC_MMU_CFG_PRI_GROUP_MAX];
     int valid;
} soc_mmu_cfg_buf_mapping_profile_t;

typedef struct soc_mmu_cfg_buf_prigroup_s {
    int pool_idx; /* from mapping variable */
    int guarantee;
    int headroom;
    int user_delay;
    int switch_delay;
    int pkt_size;
    int device_headroom_enable;
    int port_guarantee_enable;
    int port_max_enable;
    int pool_scale;
    int pool_limit;
    int pool_resume;
    int pool_floor;
    int flow_control_enable;
    int lossless;

} soc_mmu_cfg_buf_prigroup_t;

typedef struct soc_mmu_cfg_buf_qgroup_s
{
    int guarantee;
    int guarantee_mc;
    int discard_enable;
    int pool_scale;
    int pool_limit;
    int pool_resume;
    int yellow_limit;
    int red_limit;
    int yellow_resume;
    int red_resume;
} soc_mmu_cfg_buf_qgroup_t;

typedef struct soc_mmu_cfg_buf_queue_s
{
    int numq;
    int pool_idx; /* from mapping variable */
    int guarantee;
    int discard_enable;
    int pool_scale;
    int pool_limit;
    int pool_resume;
    int color_discard_enable;
    int yellow_limit;
    int red_limit;
    int yellow_resume;
    int red_resume;
    int qgroup_id;
    int qgroup_min_enable;
    int mcq_entry_guarantee;
    int mcq_entry_reserved;
    int mcqe_limit;
    int pkt_guarantee;
    int pkt_pool_scale;
    int use_dyn_limit;
    int color_limit_dyn;
} soc_mmu_cfg_buf_queue_t;

typedef struct soc_mmu_cfg_buf_pool_s {
    int size;
    int yellow_size;
    int red_size;
    int total; /* calculated value (not from config variable) */
    int port_guarantee; /* calculated value (not from config variable) */
    int prigroup_guarantee; /* calculated value (not from config variable) */
    int prigroup_headroom; /* calculated value (not from config variable) */
    int queue_guarantee; /* calculated value (not from config variable) */
    int queue_group_guarantee;/* calculated value (not from config variable) */
    int rqe_guarantee; /* calculated value (not from config variable) */
    int asf_reserved; /* calculated value (not from config variable) */
    int mcq_entry_reserved;
    int total_mcq_entry;
    int total_rqe_entry;
} soc_mmu_cfg_buf_pool_t;

typedef struct soc_mmu_cfg_buf_port_pool_s {
    int guarantee;
    int pool_limit;
    int pool_resume;
} soc_mmu_cfg_buf_port_pool_t;

typedef struct soc_mmu_cfg_buf_port_s
{
    int guarantee;
    int pool_limit;
    int pool_resume;
    int pkt_size;
    int pri_to_prigroup_profile_idx;
    int prigroup_profile_idx;
    soc_mmu_cfg_buf_prigroup_t prigroups[SOC_MMU_CFG_PRI_GROUP_MAX];
    soc_mmu_cfg_buf_queue_t *queues;
    int pri_to_prigroup[SOC_MMU_CFG_INT_PRI_MAX]; /* from mapping variable */
    soc_mmu_cfg_buf_port_pool_t pools[SOC_MMU_CFG_SERVICE_POOL_MAX];
} soc_mmu_cfg_buf_port_t;

typedef struct soc_mmu_cfg_buf_mcengine_queue_s {
    int pool_idx;
    int guarantee;
    int pool_scale;
    int pool_limit;
    int discard_enable;
    int yellow_limit;
    int red_limit;
    int ext_guarantee;
    int pkt_guarantee;
    int ext_pool_scale;
    int pkt_pool_scale;
} soc_mmu_cfg_buf_mcengine_queue_t;

typedef struct soc_th_mmu_rsvd_buffer_s {
    int ing_rsvd_cells;
    int egr_rsvd_cells;
    int asf_rsvd_cells;
    int mcq_entry_rsvd;
} soc_mmu_rsvd_buffer_t;

typedef struct soc_mmu_cfg_buf_s
{
    int headroom;
    soc_mmu_rsvd_buffer_t rsvd_buffers[4];
    soc_mmu_cfg_buf_pool_t pools[SOC_MMU_CFG_SERVICE_POOL_MAX];
    soc_mmu_cfg_buf_pool_t pools_itm[SOC_MMU_CFG_ITM_MAX][SOC_MMU_CFG_SERVICE_POOL_MAX];
    soc_mmu_cfg_buf_pool_t pools_xpe[SOC_MMU_CFG_XPE_MAX][SOC_MMU_CFG_SERVICE_POOL_MAX];
    soc_mmu_cfg_buf_port_t ports[SOC_MMU_CFG_PORT_MAX];
    soc_mmu_cfg_buf_qgroup_t qgroups[SOC_MMU_CFG_QGROUP_MAX];
    soc_mmu_cfg_buf_mcengine_queue_t rqe_queues[SOC_MMU_CFG_RQE_QUEUE_MAX];
    soc_mmu_cfg_buf_queue_t  equeues[SOC_MMU_CFG_EQUEUE_MAX];
    soc_mmu_cfg_buf_mapping_profile_t mapprofiles[SOC_MMU_CFG_NUM_PROFILES_MAX];
    /* equeues must be the last member of this strucutre */
} soc_mmu_cfg_buf_t;


/* MMU port */
#define TD4_X9_MMU_PORTS_OFFSET_PER_PIPE  32
#define TD4_X9_MMU_PORTS_PER_DEV          (32 * 8)

#define TD4_X9_SECTION_SIZE                32768

#define TD4_X9_MMU_MAX_PACKET_BYTES        9416
#define TD4_X9_MMU_PACKET_HEADER_BYTES     64
#define TD4_X9_MMU_JUMBO_FRAME_BYTES       9216
#define TD4_X9_MMU_DEFAULT_MTU_BYTES       1536

#define TD4_X9_MMU_PHYSICAL_CELLS_PER_ITM  278528
#define TD4_X9_MMU_TOTAL_CELLS_PER_ITM     272244
#define TD4_X9_MMU_RSVD_CELLS_CFAP_PER_ITM 408

#define TD4_X9_THDI_BUFFER_CELL_LIMIT_SP_MAX      TD4_X9_MMU_TOTAL_CELLS_PER_ITM
#define TD4_X9_THDI_HDRM_BUFFER_CELL_LIMIT_HP_MAX      TD4_X9_MMU_TOTAL_CELLS_PER_ITM


#define TD4_X9_MMU_BYTES_PER_CELL          318
#define TD4_X9_MMU_NUM_PG                  8
#define TD4_X9_MMU_NUM_POOL                4
#define TD4_X9_MMU_NUM_RQE_QUEUES          9
#define TD4_X9_MMU_NUM_INT_PRI             16
#define TD4_X9_MMU_NUM_MAX_PROFILES        8

#define SOC_TD4_X9_MMU_CFG_QGROUP_MAX      80

#define TD4_X9_MMU_JUMBO_PACKET_SIZE TD4_X9_MMU_CFG_MMU_BYTES_TO_CELLS( TD4_X9_MMU_JUMBO_FRAME_BYTES + \
                                                               TD4_X9_MMU_PACKET_HEADER_BYTES, \
                                                               TD4_X9_MMU_BYTES_PER_CELL)

#define SOC_TD4_X9_NUM_CPU_QUEUES           48


#define SOC_TD4_X9_MMU_MCQ_ENTRY_PER_ITM    20480
#define SOC_TD4_X9_MMU_RQE_ENTRY_PER_ITM    6144
#define SOC_TD4_X9_MMU_RQE_ENTRY_RSVD_PER_ITM 183

#define SOC_TD4_X9_MAX_PHY_PORTS_PER_PIPE 40
#define SOC_TD4_X9_MAX_DEV_PORTS_PER_PIPE 20

#define SOC_TD4_X9_PHY_PORTS_PER_DEV          \
    ( SOC_TD4_X9_MAX_PHY_PORTS_PER_PIPE * TD4_X9_PIPES_PER_DEV)
#define SOC_TD4_X9_DEV_PORTS_PER_DEV          \
    ( SOC_TD4_X9_MAX_DEV_PORTS_PER_PIPE * TD4_X9_PIPES_PER_DEV)
#define SOC_TD4_X9_MMU_PORTS_PER_DEV          \
    ( SOC_TD4_X9_MAX_DEV_PORTS_PER_PIPE * TD4_X9_PIPES_PER_DEV)

#define TD4_X9_MMU_TOTAL_MCQ_ENTRY(unit)   SOC_TD4_X9_MMU_MCQ_ENTRY_PER_ITM
#define TD4_X9_MMU_TOTAL_RQE_ENTRY(unit)   SOC_TD4_X9_MMU_RQE_ENTRY_PER_ITM

/* Default values for thresholds */
#define TD4_X9_CFAP_BANK_FULL_LIMIT 4092
#define TD4_X9_LAST_PKT_ACCEPT_MODE_DEFAULT 1
#define TD4_X9_LAST_PKT_ACCEPT_MODE_POOL_DEFAULT 15
#define TD4_X9_COLOR_AWARE_DEFAULT 0
#define TD4_X9_HDRM_LIMIT_DEFAULT 0
#define TD4_X9_SPID_OVERRIDE_ENABLE_DEFAULT 0
#define TD4_X9_SPAP_YELLOW_OFFSET_DEFAULT 0
#define TD4_X9_SPAP_RED_OFFSET_DEFAULT 0
#define TD4_X9_MC_Q_MODE_DEFAULT 2
#define TD4_X9_OUTPUT_PORT_EN_DEFAULT 1048575
#define TD4_X9_PAUSE_EN_DEFAULT 0
#define TD4_X9_SPID_DEFAULT 0


/* Oversub XON/XOFF Threshold */
#define TD4_X9_OBM_ENTRY_LEN    64

#define TD4_X9_XON_THRESHOLD_10G      73  * TD4_X9_OBM_ENTRY_LEN
#define TD4_X9_XON_THRESHOLD_25G      73  * TD4_X9_OBM_ENTRY_LEN
#define TD4_X9_XON_THRESHOLD_50G      73  * TD4_X9_OBM_ENTRY_LEN
#define TD4_X9_XON_THRESHOLD_100G     186 * TD4_X9_OBM_ENTRY_LEN
#define TD4_X9_XON_THRESHOLD_200G     376 * TD4_X9_OBM_ENTRY_LEN
#define TD4_X9_XON_THRESHOLD_400G     760 * TD4_X9_OBM_ENTRY_LEN

#define TD4_X9_XOFF_THRESHOLD_10G     81  * TD4_X9_OBM_ENTRY_LEN
#define TD4_X9_XOFF_THRESHOLD_25G     81  * TD4_X9_OBM_ENTRY_LEN
#define TD4_X9_XOFF_THRESHOLD_50G     81  * TD4_X9_OBM_ENTRY_LEN
#define TD4_X9_XOFF_THRESHOLD_100G    194 * TD4_X9_OBM_ENTRY_LEN
#define TD4_X9_XOFF_THRESHOLD_200G    384 * TD4_X9_OBM_ENTRY_LEN
#define TD4_X9_XOFF_THRESHOLD_400G    768 * TD4_X9_OBM_ENTRY_LEN


/*PG headroom*/
#define TD4_X9_PG_HEADROOM_LINERATE_10G   167
#define TD4_X9_PG_HEADROOM_LINERATE_25G   256
#define TD4_X9_PG_HEADROOM_LINERATE_50G   360
#define TD4_X9_PG_HEADROOM_LINERATE_100G  580
#define TD4_X9_PG_HEADROOM_LINERATE_200G  1061
#define TD4_X9_PG_HEADROOM_LINERATE_400G  1962


#define TD4_X9_PG_HEADROOM_OVERSUB_10G    \
    ( TD4_X9_PG_HEADROOM_LINERATE_10G + ceiling( TD4_X9_XOFF_THRESHOLD_10G,64))
#define TD4_X9_PG_HEADROOM_OVERSUB_25G    \
    ( TD4_X9_PG_HEADROOM_LINERATE_25G + ceiling( TD4_X9_XOFF_THRESHOLD_25G,64))
#define TD4_X9_PG_HEADROOM_OVERSUB_50G    \
    ( TD4_X9_PG_HEADROOM_LINERATE_50G + ceiling( TD4_X9_XOFF_THRESHOLD_50G,64))
#define TD4_X9_PG_HEADROOM_OVERSUB_100G    \
    ( TD4_X9_PG_HEADROOM_LINERATE_100G + ceiling( TD4_X9_XOFF_THRESHOLD_100G,64))
#define TD4_X9_PG_HEADROOM_OVERSUB_200G    \
    ( TD4_X9_PG_HEADROOM_LINERATE_200G + ceiling( TD4_X9_XOFF_THRESHOLD_200G,64))
#define TD4_X9_PG_HEADROOM_OVERSUB_400G    \
    ( TD4_X9_PG_HEADROOM_LINERATE_400G + ceiling( TD4_X9_XOFF_THRESHOLD_400G,64))



/* supports port min guarentee */
#define SOC_MMU_CFG_F_PORT_MIN              0x01

/* supports per port per pool min guarentee */
#define SOC_MMU_CFG_F_PORT_POOL_MIN         0x02
#define SOC_MMU_CFG_F_RQE                   0x04
#define SOC_MMU_CFG_F_EGR_MCQ_ENTRY         0x08

typedef struct soc_mmu_device_info_s {
    int  flags;
    int  max_pkt_byte;
    int  mmu_hdr_byte;
    int  jumbo_pkt_size;
    int  default_mtu_size;
    int  mmu_total_cell;
    int  mmu_cell_size;
    int  num_pg;
    int  num_service_pool;
    int  total_mcq_entry;
    int  total_rqe_queue_entry;
    int  rqe_queue_num;
} soc_mmu_device_info_t;

typedef enum shared_buffer_update_reason_e {
    RESERVED_BUFFER_UPDATE  = 0,
    ING_SERVICE_POOL_UPDATE = 1,
    EGR_SERVICE_POOL_UPDATE = 2
} shared_buffer_update_reason_t;

#define TD4_X9_MMU_CFG_MMU_BYTES_TO_CELLS(byte,cellhdr) (((byte) + (cellhdr) - 1) / (cellhdr))

#define SOC_MMU_CFG_DEV_POOL_NUM(dcfg) ((dcfg)->num_service_pool)
#define SOC_MMU_CFG_DEV_PG_NUM(dcfg) ((dcfg)->num_pg)
#define SOC_MMU_CFG_MMU_TOTAL_CELLS(dcfg)  ((dcfg)->mmu_total_cell)

#define SOC_TD4_X9_MMU_PIPED_MEM_INDEX(u,p,m,off) \
                    td4_x9_piped_mem_index((u),(p),(m),(off))

#define TD4_X9_MMU_RQE_QUEUE_NUM 9
#define TD4_X9_THDR_QE_CONFIG1_VALUE 0x7
#define TD4_X9_THDR_QE_CONIFG_PRI_LIMIT 8
#define TD4_X9_THDR_QE_LIMIT_MIN 6
#define TD4_X9_MMU_THDR_QE_RESET_OFFSET 0xc
#define TD4_X9_THDR_QE_SP_SHARED_LIMIT 0x16ca
#define TD4_X9_THDR_QE_SP_RESUME_LIMIT 0x16be

#define TD4_X9_GRANULARITY 8
#define CEILING(a, b) (((a - 1) / b) + 1)

extern uint32_t
td4_x9_piped_mem_index(int unit, int port, bcmdrd_sid_t mem, int arr_off);


extern int
bcm56780_a0_tm_thdi_pg_to_pool_mapping_get(int unit, int lport, int pg,
                                           int *itm, int *pool, bool is_hdrm);

extern int
bcm56780_a0_tm_shared_buffer_update(int unit, int itm, int pool, long int delta,
                                    shared_buffer_update_reason_t reason,
                                    bool check_only);

extern int
bcm56780_a0_tm_ing_pool_shared_limit_lt_update(int unit, int itm, int pool,
                                               uint32_t fval);

extern int
bcm56780_a0_tm_egr_pool_shared_limit_lt_update(int unit, int itm, int pool,
                                               uint32_t fval);

extern void
td4_x9_mmu_config_thdi_set(int unit, int port, soc_mmu_cfg_buf_t *buf,
                        soc_mmu_device_info_t *devcfg,  int lossless);

extern void
td4_x9_mmu_config_thdo_set(int unit, int port, soc_mmu_cfg_buf_t *buf,
                        soc_mmu_device_info_t *devcfg, int lossless);
extern void
td4_x9_mmu_init_dev_config(int unit, soc_mmu_device_info_t *devcfg, int lossless);
extern int
td4_x9_default_pg_headroom(int unit, int port);

extern soc_mmu_cfg_buf_t*
bcm56780_a0_mmu_buf_cfg_alloc(int unit);
#endif /* BCM56880_A0_TM_THD_INTERNAL_H */
