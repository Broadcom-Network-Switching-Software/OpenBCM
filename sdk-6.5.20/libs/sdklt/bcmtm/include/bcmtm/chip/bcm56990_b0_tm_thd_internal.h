/*! \file bcm56990_b0_tm_thd_internal.h
 *
 * File containing MMU thresholds type/macro defines and internal public
 * functions for bcm56990_b0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_B0_TM_THD_INTERNAL_H
#define BCM56990_B0_TM_THD_INTERNAL_H


/*! Alpha values for dynamic threshold. */
typedef enum soc_th4_b0_drop_limit_alpha_value_e {
    SOC_TH4_B0_COSQ_DROP_LIMIT_ALPHA_1_128, /* 1/128 */
    SOC_TH4_B0_COSQ_DROP_LIMIT_ALPHA_1_64,  /* 1/64 */
    SOC_TH4_B0_COSQ_DROP_LIMIT_ALPHA_1_32,  /* 1/32 */
    SOC_TH4_B0_COSQ_DROP_LIMIT_ALPHA_1_16,  /* 1/16 */
    SOC_TH4_B0_COSQ_DROP_LIMIT_ALPHA_1_8,   /* 1/8 */
    SOC_TH4_B0_COSQ_DROP_LIMIT_ALPHA_1_4,   /* 1/4 */
    SOC_TH4_B0_COSQ_DROP_LIMIT_ALPHA_1_2,   /* 1/2 */
    SOC_TH4_B0_COSQ_DROP_LIMIT_ALPHA_1,     /* 1 */
    SOC_TH4_B0_COSQ_DROP_LIMIT_ALPHA_2,     /* 2 */
    SOC_TH4_B0_COSQ_DROP_LIMIT_ALPHA_4,     /* 4 */
    SOC_TH4_B0_COSQ_DROP_LIMIT_ALPHA_8,     /* 8 */
    SOC_TH4_B0_COSQ_DROP_LIMIT_ALPHA_COUNT  /* Must be last! */
} soc_th4_b0_drop_limit_alpha_value_t;

/*! MMU buffer dynamic flag. */
#define MMU_CFG_BUF_DYNAMIC_FLAG        (0x80000000)
/*! MMU buffer percent flag. */
#define MMU_CFG_BUF_PERCENT_FLAG        (0x40000000)

/*! MMU service pool max number. */
#define SOC_MMU_CFG_SERVICE_POOL_MAX    4
/*! MMU port max number. */
#define SOC_MMU_CFG_PORT_MAX            272
/*! MMU priority group max number. */
#define SOC_MMU_CFG_PRI_GROUP_MAX       8
/*! MMU internal priority max number. */
#define SOC_MMU_CFG_INT_PRI_MAX         16
/*! MMU queue group max number. */
#define SOC_MMU_CFG_QGROUP_MAX          272
/*! MMU egress queue max number. */
#define SOC_MMU_CFG_EQUEUE_MAX          512
/*! MMU rqe queue max number. */
#define SOC_MMU_CFG_RQE_QUEUE_MAX       11
/*! MMU xpe max number. */
#define SOC_MMU_CFG_XPE_MAX             4

/*! MMU itm max number. */
#define SOC_MMU_CFG_ITM_MAX             2
/*! MMU profile max number. */
#define SOC_MMU_CFG_NUM_PROFILES_MAX    8

/*! MMU buffer mapping profile structure. */
typedef struct soc_mmu_cfg_buf_mapping_profile_s
{
     int inpri_to_prigroup_uc[SOC_MMU_CFG_INT_PRI_MAX];
     int inpri_to_prigroup_mc[SOC_MMU_CFG_INT_PRI_MAX];
     int prigroup_to_servicepool[SOC_MMU_CFG_PRI_GROUP_MAX];
     int prigroup_to_headroompool[SOC_MMU_CFG_PRI_GROUP_MAX];
     int pfcpri_to_prigroup[SOC_MMU_CFG_PRI_GROUP_MAX];
     int valid;
} soc_mmu_cfg_buf_mapping_profile_t;

/*! MMU buffer priority group configuration structure. */
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

/*! MMU buffer queue group configuration structure. */
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

/*! MMU buffer queue configuration structure. */
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
} soc_mmu_cfg_buf_queue_t;

/*! MMU buffer pool configuration structure. */
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

/*! MMU buffer port pool configuration structure. */
typedef struct soc_mmu_cfg_buf_port_pool_s {
    int guarantee;
    int pool_limit;
    int pool_resume;
} soc_mmu_cfg_buf_port_pool_t;

/*! MMU buffer port configuration structure. */
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

/*! MMU buffer mc engine queue configuration structure. */
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

/*! MMU buffer reserved configuration structure. */
typedef struct soc_th_mmu_rsvd_buffer_s {
    int ing_rsvd_cells;
    int egr_rsvd_cells;
    int asf_rsvd_cells;
    int mcq_entry_rsvd;
} soc_mmu_rsvd_buffer_t;

/*! MMU buffer configuration structure. */
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


/*! MMU port offset per pipe. */
#define TH4_B0_MMU_PORTS_OFFSET_PER_PIPE   32
/*! MMU ports per device. */
#define TH4_B0_MMU_PORTS_PER_DEV           (32 * 16)
/*! MMU max packet length. */
#define TH4_B0_MMU_MAX_PACKET_BYTES        9416
/*! MMU packet header length. */
#define TH4_B0_MMU_PACKET_HEADER_BYTES     48
/*! MMU jumbo packet length. */
#define TH4_B0_MMU_JUMBO_FRAME_BYTES       9216
/*! MMU default MTU length. */
#define TH4_B0_MMU_DEFAULT_MTU_BYTES       1536

/*! MMU physical cells per itm. */
#define TH4_B0_MMU_PHYSICAL_CELLS_PER_ITM  243712
/*! MMU total cells per itm. */
#define TH4_B0_MMU_TOTAL_CELLS_PER_ITM     231446
/*! MMU reserved cfap cells per itm. */
#define TH4_B0_MMU_RSVD_CELLS_CFAP_PER_ITM 716

/*! MMU thdi buffer max sp cell limit . */
#define TH4_B0_THDI_BUFFER_CELL_LIMIT_SP_MAX           TH4_B0_MMU_TOTAL_CELLS_PER_ITM
/*! MMU thdi headroom buffer max cell limit . */
#define TH4_B0_THDI_HDRM_BUFFER_CELL_LIMIT_HP_MAX      TH4_B0_MMU_TOTAL_CELLS_PER_ITM

/*! MMU bytes per cell. */
#define TH4_B0_MMU_BYTES_PER_CELL          254
/*! MMU priority group number. */
#define TH4_B0_MMU_NUM_PG                  8
/*! MMU pool number. */
#define TH4_B0_MMU_NUM_POOL                4
/*! MMU rqe queue number. */
#define TH4_B0_MMU_NUM_RQE_QUEUES          9
/*! MMU internal priority number. */
#define TH4_B0_MMU_NUM_INT_PRI             16
/*! MMU profile max number. */
#define TH4_B0_MMU_NUM_MAX_PROFILES        8

/*! MMU queue group max number. */
#define SOC_TH4_B0_MMU_CFG_QGROUP_MAX      272

/*! MMU jumbo packet length. */
#define TH4_B0_MMU_JUMBO_PACKET_SIZE TH4_B0_MMU_CFG_MMU_BYTES_TO_CELLS( TH4_B0_MMU_JUMBO_FRAME_BYTES + \
                                                               TH4_B0_MMU_PACKET_HEADER_BYTES, \
                                                               TH4_B0_MMU_BYTES_PER_CELL)

/*! MMU cpu queue number. */
#define SOC_TH4_B0_NUM_CPU_QUEUES          48
/*! Num UC Q's Per pipe 16 * 12 */
#define SOC_TH4_B0_NUM_UC_QUEUES_PER_PIPE  192
/*! Num MC Q's Per pipe (16 * 6)  + 48 CPU queues */
#define SOC_TH4_B0_NUM_MC_QUEUES_PER_PIPE  144

/*! MMU mcq entry per itm. */
#define SOC_TH4_B0_MMU_MCQ_ENTRY_PER_ITM   16384
/*! MMU rqe entry per itm. */
#define SOC_TH4_B0_MMU_RQE_ENTRY_PER_ITM   3072
/*! MMU reserved rqe entry per itm. */
#define SOC_TH4_B0_MMU_RQE_ENTRY_RSVD_PER_ITM 215

/*! MMU max physical port per pipe. */
#define SOC_TH4_B0_MAX_PHY_PORTS_PER_PIPE  16
/*! MMU max device port per pipe. */
#define SOC_TH4_B0_MAX_DEV_PORTS_PER_PIPE  17
/*! MMU port stride per pipe. */
#define SOC_TH4_B0_MMU_PORT_STRIDE         32
/*! MMU physical port per device. */
#define SOC_TH4_B0_PHY_PORTS_PER_DEV          \
    ( SOC_TH4_B0_MAX_PHY_PORTS_PER_PIPE * TH4_B0_PIPES_PER_DEV)
/*! MMU device port per device. */
#define SOC_TH4_B0_DEV_PORTS_PER_DEV          \
    ( SOC_TH4_B0_MAX_DEV_PORTS_PER_PIPE * TH4_B0_PIPES_PER_DEV)
/*! MMU port per device. */
#define SOC_TH4_B0_MMU_PORTS_PER_DEV          \
    ( SOC_TH4_B0_MAX_DEV_PORTS_PER_PIPE * TH4_B0_PIPES_PER_DEV)

/*! MMU mcq entry per itm. */
#define TH4_B0_MMU_TOTAL_MCQ_ENTRY(unit)   SOC_TH4_B0_MMU_MCQ_ENTRY_PER_ITM
/*! MMU rqe entry per itm. */
#define TH4_B0_MMU_TOTAL_RQE_ENTRY(unit)   SOC_TH4_B0_MMU_RQE_ENTRY_PER_ITM

/*! Default values for thresholds */
/*! MMU cfap bank full limit. */
#define TH4_B0_CFAP_BANK_FULL_LIMIT                3580
/*! MMU last packet accept default mode. */
#define TH4_B0_LAST_PKT_ACCEPT_MODE_DEFAULT        1
/*! MMU pool last packet accept default mode. */
#define TH4_B0_LAST_PKT_ACCEPT_MODE_POOL_DEFAULT   15
/*! MMU default color aware. */
#define TH4_B0_COLOR_AWARE_DEFAULT                 0
/*! MMU default headroom limit. */
#define TH4_B0_HDRM_LIMIT_DEFAULT                  0
/*! MMU spid override enable default mode. */
#define TH4_B0_SPID_OVERRIDE_ENABLE_DEFAULT        0
/*! MMU default spap yellow offset. */
#define TH4_B0_SPAP_YELLOW_OFFSET_DEFAULT          0
/*! MMU default spap red offset. */
#define TH4_B0_SPAP_RED_OFFSET_DEFAULT             0
/*! MMU default mc queue mode. */
#define TH4_B0_MC_Q_MODE_DEFAULT                   2
/*! Default pause enable. */
#define TH4_B0_PAUSE_EN_DEFAULT                    0
/*! Default spid. */
#define TH4_B0_SPID_DEFAULT                        0

/*! Oversub XON/XOFF Threshold */
#define TH4_B0_OBM_ENTRY_LEN          64

/*! XON 10G Threshold */
#define TH4_B0_XON_THRESHOLD_10G      73  * TH4_B0_OBM_ENTRY_LEN
/*! XON 25G Threshold */
#define TH4_B0_XON_THRESHOLD_25G      73  * TH4_B0_OBM_ENTRY_LEN
/*! XON 50G Threshold */
#define TH4_B0_XON_THRESHOLD_50G      73  * TH4_B0_OBM_ENTRY_LEN
/*! XON 100G Threshold */
#define TH4_B0_XON_THRESHOLD_100G     186 * TH4_B0_OBM_ENTRY_LEN
/*! XON 200G Threshold */
#define TH4_B0_XON_THRESHOLD_200G     376 * TH4_B0_OBM_ENTRY_LEN
/*! XON 400G Threshold */
#define TH4_B0_XON_THRESHOLD_400G     760 * TH4_B0_OBM_ENTRY_LEN

/*! XOFF 10G Threshold */
#define TH4_B0_XOFF_THRESHOLD_10G     81  * TH4_B0_OBM_ENTRY_LEN
/*! XOFF 25G Threshold */
#define TH4_B0_XOFF_THRESHOLD_25G     81  * TH4_B0_OBM_ENTRY_LEN
/*! XOFF 50G Threshold */
#define TH4_B0_XOFF_THRESHOLD_50G     81  * TH4_B0_OBM_ENTRY_LEN
/*! XOFF 100G Threshold */
#define TH4_B0_XOFF_THRESHOLD_100G    194 * TH4_B0_OBM_ENTRY_LEN
/*! XOFF 200G Threshold */
#define TH4_B0_XOFF_THRESHOLD_200G    384 * TH4_B0_OBM_ENTRY_LEN
/*! XOFF 400G Threshold */
#define TH4_B0_XOFF_THRESHOLD_400G    768 * TH4_B0_OBM_ENTRY_LEN

/*! PG headroom 10G linerate. */
#define TH4_B0_PG_HEADROOM_LINERATE_10G   173
/*! PG headroom 25G linerate. */
#define TH4_B0_PG_HEADROOM_LINERATE_25G   262
/*! PG headroom 50G linerate. */
#define TH4_B0_PG_HEADROOM_LINERATE_50G   366
/*! PG headroom 100G linerate. */
#define TH4_B0_PG_HEADROOM_LINERATE_100G  585
/*! PG headroom 200G linerate. */
#define TH4_B0_PG_HEADROOM_LINERATE_200G  1071
/*! PG headroom 400G linerate. */
#define TH4_B0_PG_HEADROOM_LINERATE_400G  1982


/*! PG headroom 10G oversub. */
#define TH4_B0_PG_HEADROOM_OVERSUB_10G    \
    ( TH4_B0_PG_HEADROOM_LINERATE_10G + ceiling( TH4_B0_XOFF_THRESHOLD_10G,64))
/*! PG headroom 25G oversub. */
#define TH4_B0_PG_HEADROOM_OVERSUB_25G    \
    ( TH4_B0_PG_HEADROOM_LINERATE_25G + ceiling( TH4_B0_XOFF_THRESHOLD_25G,64))
/*! PG headroom 50G oversub. */
#define TH4_B0_PG_HEADROOM_OVERSUB_50G    \
    ( TH4_B0_PG_HEADROOM_LINERATE_50G + ceiling( TH4_B0_XOFF_THRESHOLD_50G,64))
/*! PG headroom 100G oversub. */
#define TH4_B0_PG_HEADROOM_OVERSUB_100G    \
    ( TH4_B0_PG_HEADROOM_LINERATE_100G + ceiling( TH4_B0_XOFF_THRESHOLD_100G,64))
/*! PG headroom 200G oversub. */
#define TH4_B0_PG_HEADROOM_OVERSUB_200G    \
    ( TH4_B0_PG_HEADROOM_LINERATE_200G + ceiling( TH4_B0_XOFF_THRESHOLD_200G,64))
/*! PG headroom 400G oversub. */
#define TH4_B0_PG_HEADROOM_OVERSUB_400G    \
    ( TH4_B0_PG_HEADROOM_LINERATE_400G + ceiling( TH4_B0_XOFF_THRESHOLD_400G,64))



/*! Port min guarentee configuration flag. */
#define SOC_MMU_CFG_F_PORT_MIN              0x01

/*! Port pool min guarentee configuration flag. */
#define SOC_MMU_CFG_F_PORT_POOL_MIN         0x02
/*! RQE configuration flag. */
#define SOC_MMU_CFG_F_RQE                   0x04
/*! EGR mcq entry configuration flag. */
#define SOC_MMU_CFG_F_EGR_MCQ_ENTRY         0x08

/*! MMU device information structure. */
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

/*! Shared buffer udpate reason. */
typedef enum shared_buffer_update_reason_e {
    RESERVED_BUFFER_UPDATE  = 0,
    ING_SERVICE_POOL_UPDATE = 1,
    EGR_SERVICE_POOL_UPDATE = 2
} shared_buffer_update_reason_t;

/*! MMU bytes to cells transform. */
#define TH4_B0_MMU_CFG_MMU_BYTES_TO_CELLS(byte,cellhdr) (((byte) + (cellhdr) - 1) / (cellhdr))

/*! MMU rqe queue number. */
#define TH4_B0_MMU_RQE_QUEUE_NUM 9
/*! MMU rqe configuration value. */
#define TH4_B0_THDR_QE_CONFIG1_VALUE 0x7
/*! MMU rqe priority limit. */
#define TH4_B0_THDR_QE_CONIFG_PRI_LIMIT 8
/*! MMU rqe min limit. */
#define TH4_B0_THDR_QE_LIMIT_MIN 7
/*! MMU rqe reset offset. */
#define TH4_B0_MMU_THDR_QE_RESET_OFFSET 0xe
/*! MMU rqe sp shared limit. */
#define TH4_B0_THDR_QE_SP_SHARED_LIMIT 0xbc1
/*! MMU rqe sp resume limit. */
#define TH4_B0_THDR_QE_SP_RESUME_LIMIT 0xbb3

/*! TH4_B0 granularity. */
#define TH4_B0_GRANULARITY 8
/*! Ceiling function. */
#define CEILING(a, b) ((a - 1 + b) / b)

/*! Piped memory index calculation function. */
extern uint32_t
th4_b0_piped_mem_index(int unit, int port, bcmdrd_sid_t mem, int arr_off);

/*! TM thdi port to itm mapping get. */
extern int
bcm56990_b0_tm_thdi_port_to_itm_mapping_get(int unit, int lport, int *itm);

/*! TM thdi pg to pool mapping get. */
extern int
bcm56990_b0_tm_thdi_pg_to_pool_mapping_get(int unit, int lport, int pg,
                                           int *itm, int *pool, bool is_hdrm);

/*! TM shared buffer update. */
extern int
bcm56990_b0_tm_shared_buffer_update(int unit, int itm, int pool, long int delta,
                                    shared_buffer_update_reason_t reason,
                                    bool check_only);

/*! TM ingress pool shared limit lt update. */
extern int
bcm56990_b0_tm_ing_pool_shared_limit_lt_update(int unit, int itm, int pool,
                                               uint32_t fval);

/*! TM egress pool shared limit lt update. */
extern int
bcm56990_b0_tm_egr_pool_shared_limit_lt_update(int unit, int itm, int pool,
                                               uint32_t fval);

/*! MMU buffer configuration allocation. */
extern soc_mmu_cfg_buf_t*
th4_b0_soc_mmu_buf_cfg_alloc(int unit);

/*! MMU thdi configuration set. */
extern void
th4_b0_mmu_config_thdi_set(int unit, int port, soc_mmu_cfg_buf_t *buf,
                        soc_mmu_device_info_t *devcfg,  int lossless);

/*! MMU thdo configuration set. */
extern void
th4_b0_mmu_config_thdo_set(int unit, int port, soc_mmu_cfg_buf_t *buf,
                        soc_mmu_device_info_t *devcfg, int lossless);

/*! MMU buf prigroup configuration get. */
extern void
th4_b0_mmu_config_buf_prigroup_get(int unit, int port, int pg,
                                soc_mmu_cfg_buf_prigroup_t *buf_prigroup,
                                int lossless);

/*! MMU buf queue configuration get. */
extern void
th4_b0_mmu_config_buf_queue_get(int unit, int port,
                             soc_mmu_cfg_buf_queue_t *buf_queue);

/*! MMU buf queue group configuration get. */
extern void
th4_b0_mmu_config_buf_qgroup_get(int unit, int port,
                              soc_mmu_cfg_buf_qgroup_t *queue_grp);

/*! MMU buf port pool configuration get. */
extern void
th4_b0_mmu_config_buf_pool_get(int unit, int pool,
                            soc_mmu_cfg_buf_port_pool_t *buf_port_pool);

/*! MMU configuration initialization. */
extern void
th4_b0_mmu_init_dev_config(int unit, soc_mmu_device_info_t *devcfg, int lossless);

/*! MMU default pg headroom configuration. */
extern int
th4_b0_default_pg_headroom(int unit, int port);
#endif /* BCM56990_B0_TM_THD_INTERNAL_H */
