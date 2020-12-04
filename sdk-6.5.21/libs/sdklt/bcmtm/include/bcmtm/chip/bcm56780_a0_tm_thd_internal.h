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

#include <bcmtm/thd/bcmtm_thd_internal.h>

/*! Alpha values for dynamic threshold. */
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

/*! Number of service pools. */
#define TD4_X9_MAX_SERVICE_POOLS      4
/*! Number of logical ports. */
#define TD4_X9_MMU_NUM_LPORT         80
/*! Number of internal priority. */
#define TD4_X9_MMU_CFG_INT_PRI       16
/*! Number of queue groups. */
#define TD4_X9_MMU_NUM_QGROUP        80
/*! Number of profiles for MMU */
#define TD4_X9_MMU_NUM_PROFILES       8
/*! Number of ITMs */
#define TD4_X9_ITMS_PER_DEV           1

/*! Maximum packet size in Bytes. */
#define TD4_X9_MMU_MAX_PACKET_BYTES        9416

/*! Packet header in bytes. */
#define TD4_X9_MMU_PACKET_HEADER_BYTES     64

/*! Jumbo frame in bytes. */
#define TD4_X9_MMU_JUMBO_FRAME_BYTES       9216
/*! Default MTU. */
#define TD4_X9_MMU_DEFAULT_MTU_BYTES       1536

/*! Total number of cells per buffer pool. */
#define TD4_X9_MMU_PHYSICAL_CELLS_PER_ITM  278528
/*! Total number of advertised cells for buffer pool. */
#define TD4_X9_MMU_TOTAL_CELLS_PER_ITM     266100

/*! Total number of shared cells per buffer pool. */
#define TD4_X9_MMU_TOTAL_SHARED_PER_ITM    265236

/*! Total number of reserved cells per buffer pool. */
#define TD4_X9_MMU_RSVD_CELLS_CFAP_PER_ITM 408

/*! Total number of cells in service pool. */
#define TD4_X9_THDI_BUFFER_CELL_LIMIT_SP_MAX      TD4_X9_MMU_TOTAL_CELLS_PER_ITM
/*! Maximum number of cells in headroom pool. */
#define TD4_X9_THDI_HDRM_BUFFER_CELL_LIMIT_HP_MAX      TD4_X9_MMU_TOTAL_CELLS_PER_ITM


/*! Cell size in bytes. */
#define TD4_X9_MMU_BYTES_PER_CELL          318

/*! Number of priority groups. */
#define TD4_X9_MMU_NUM_PG                  8

/*! Number of service pools. */
#define TD4_X9_MMU_NUM_POOL                4

/*! Number of queues in RQE. */
#define TD4_X9_MMU_NUM_RQE_QUEUES          9

/*! Number of internal priority. */
#define TD4_X9_MMU_NUM_INT_PRI             16

/*! Number of cells in jumbo packet. */
#define TD4_X9_MMU_JUMBO_PACKET_SIZE MMU_CFG_MMU_BYTES_TO_CELLS( TD4_X9_MMU_JUMBO_FRAME_BYTES + \
                                                               TD4_X9_MMU_PACKET_HEADER_BYTES, \
                                                               TD4_X9_MMU_BYTES_PER_CELL)


/*! Number of MCQ entry per buffer pool. */
#define TD4_X9_MMU_MCQ_ENTRY_PER_ITM    20480


/* Default values for thresholds */
/*! CFAP bank full reset limit. */
#define TD4_X9_CFAP_BANK_FULL_LIMIT 4092
/*! Default last  packet accept mode. */
#define TD4_X9_LAST_PKT_ACCEPT_MODE_DEFAULT 1
/*! Default last packet accpet pool. */
#define TD4_X9_LAST_PKT_ACCEPT_MODE_POOL_DEFAULT 15
/*! Default color aware mode. */
#define TD4_X9_COLOR_AWARE_DEFAULT 0
/*! Default headroom limit mode. */
#define TD4_X9_HDRM_LIMIT_DEFAULT 0
/*! Default SPID override mode. */
#define TD4_X9_SPID_OVERRIDE_ENABLE_DEFAULT 0
/*! SPAP yellow offset. */
#define TD4_X9_SPAP_YELLOW_OFFSET_DEFAULT 0
/*! SPAP red offset. */
#define TD4_X9_SPAP_RED_OFFSET_DEFAULT 0


/* Oversub XON/XOFF Threshold */
/*! OBM entry length. */
#define TD4_X9_OBM_ENTRY_LEN    64

/*! XON threshold 10G */
#define TD4_X9_XON_THRESHOLD_10G      73  * TD4_X9_OBM_ENTRY_LEN
/*! XON threshold 25G */
#define TD4_X9_XON_THRESHOLD_25G      73  * TD4_X9_OBM_ENTRY_LEN
/*! XON threshold 50G */
#define TD4_X9_XON_THRESHOLD_50G      73  * TD4_X9_OBM_ENTRY_LEN
/*! XON threshold 100G */
#define TD4_X9_XON_THRESHOLD_100G     186 * TD4_X9_OBM_ENTRY_LEN
/*! XON threshold 200G */
#define TD4_X9_XON_THRESHOLD_200G     376 * TD4_X9_OBM_ENTRY_LEN
/*! XON threshold 400G */
#define TD4_X9_XON_THRESHOLD_400G     760 * TD4_X9_OBM_ENTRY_LEN

/*! XOFF threshold 10G */
#define TD4_X9_XOFF_THRESHOLD_10G     81  * TD4_X9_OBM_ENTRY_LEN
/*! XOFF threshold 25G */
#define TD4_X9_XOFF_THRESHOLD_25G     81  * TD4_X9_OBM_ENTRY_LEN
/*! XOFF threshold 50G */
#define TD4_X9_XOFF_THRESHOLD_50G     81  * TD4_X9_OBM_ENTRY_LEN
/*! XOFF threshold 100G */
#define TD4_X9_XOFF_THRESHOLD_100G    194 * TD4_X9_OBM_ENTRY_LEN
/*! XOFF threshold 200G */
#define TD4_X9_XOFF_THRESHOLD_200G    384 * TD4_X9_OBM_ENTRY_LEN
/*! XOFF threshold 400G */
#define TD4_X9_XOFF_THRESHOLD_400G    768 * TD4_X9_OBM_ENTRY_LEN


/*PG headroom*/
/*! PG headroom linerate 10G. */
#define TD4_X9_PG_HEADROOM_LINERATE_10G   167
/*! PG headroom linerate 25G. */
#define TD4_X9_PG_HEADROOM_LINERATE_25G   256
/*! PG headroom linerate 50G. */
#define TD4_X9_PG_HEADROOM_LINERATE_50G   360
/*! PG headroom linerate 100G. */
#define TD4_X9_PG_HEADROOM_LINERATE_100G  580
/*! PG headroom linerate 200G. */
#define TD4_X9_PG_HEADROOM_LINERATE_200G  1061
/*! PG headroom linerate 400G. */
#define TD4_X9_PG_HEADROOM_LINERATE_400G  1962

/*! PG headroom Oversub 10G. */
#define TD4_X9_PG_HEADROOM_OVERSUB_10G    \
    ( TD4_X9_PG_HEADROOM_LINERATE_10G + ceiling( TD4_X9_XOFF_THRESHOLD_10G,64))
/*! PG headroom Oversub 25G. */
#define TD4_X9_PG_HEADROOM_OVERSUB_25G    \
    ( TD4_X9_PG_HEADROOM_LINERATE_25G + ceiling( TD4_X9_XOFF_THRESHOLD_25G,64))
/*! PG headroom Oversub 50G. */
#define TD4_X9_PG_HEADROOM_OVERSUB_50G    \
    ( TD4_X9_PG_HEADROOM_LINERATE_50G + ceiling( TD4_X9_XOFF_THRESHOLD_50G,64))
/*! PG headroom Oversub 100G. */
#define TD4_X9_PG_HEADROOM_OVERSUB_100G    \
    ( TD4_X9_PG_HEADROOM_LINERATE_100G + ceiling( TD4_X9_XOFF_THRESHOLD_100G,64))
/*! PG headroom Oversub 200G. */
#define TD4_X9_PG_HEADROOM_OVERSUB_200G    \
    ( TD4_X9_PG_HEADROOM_LINERATE_200G + ceiling( TD4_X9_XOFF_THRESHOLD_200G,64))
/*! PG headroom Oversub 400G. */
#define TD4_X9_PG_HEADROOM_OVERSUB_400G    \
    ( TD4_X9_PG_HEADROOM_LINERATE_400G + ceiling( TD4_X9_XOFF_THRESHOLD_400G,64))



/*! Number of RQE queues. */
#define TD4_X9_MMU_RQE_QUEUE_NUM 9
/*! QE config 1. */
#define TD4_X9_THDR_QE_CONFIG1_VALUE 0x7
/*! QE config priority limit. */
#define TD4_X9_THDR_QE_CONIFG_PRI_LIMIT 8
/*! QE min limit. */
#define TD4_X9_THDR_QE_LIMIT_MIN 6
/*! THDR QE reset offset. */
#define TD4_X9_MMU_THDR_QE_RESET_OFFSET 0xc
/*! THDR QE service pool shared limit. */
#define TD4_X9_THDR_QE_SP_SHARED_LIMIT 0x16ca
/*! THDR QE service pool resume limit. */
#define TD4_X9_THDR_QE_SP_RESUME_LIMIT 0x16be

/*! TD4-X9 granularity . */
#define TD4_X9_GRANULARITY 8


/*! TD$ mmu buffer mapping profile. */
typedef struct soc_mmu_cfg_buf_mapping_profile_s
{
     /*! Input pri to PG mapping for UC packets. */
     int inpri_to_prigroup_uc[TD4_X9_MMU_NUM_INT_PRI];
     /*! Input pri to PG mapping for MC packets. */
     int inpri_to_prigroup_mc[TD4_X9_MMU_NUM_INT_PRI];
     /*! PG to service pool map. */
     int prigroup_to_servicepool[TD4_X9_MMU_NUM_PG];
     /*! PG to headroom pool map. */
     int prigroup_to_headroompool[TD4_X9_MMU_NUM_PG];
     /*! PFC priority to PG map. */
     int pfcpri_to_prigroup[TD4_X9_MMU_NUM_PG];
     /*! valid. */
     int valid;
} soc_mmu_cfg_buf_mapping_profile_t;

/*! TD4 MMU port buffer thresholds. */
typedef struct soc_mmu_cfg_buf_port_s
{
    /*! Priotiy to PG map profile index. */
    int pri_to_prigroup_profile_idx;
    /*! PG properties profile index. */
    int prigroup_profile_idx;
    /*! PG config associated with port. */
    soc_mmu_cfg_buf_prigroup_t prigroups[TD4_X9_MMU_NUM_PG];
    /*! Queue config associated with port. */
    soc_mmu_cfg_buf_queue_t *queues;
    /*! Priority to PG map. */
    int pri_to_prigroup[TD4_X9_MMU_NUM_INT_PRI]; /* from mapping variable */
    /*! Pool config associated with port. */
    soc_mmu_cfg_buf_port_pool_t pools[TD4_X9_MAX_SERVICE_POOLS];
} soc_mmu_cfg_buf_port_t;

/*! TD4 MMU buffer threhsolds. */
typedef struct soc_mmu_cfg_buf_s
{
    /*! Service pool config. */
    soc_mmu_cfg_buf_pool_t pools[TD4_X9_MAX_SERVICE_POOLS];
    /*! Service pool config per ITM. */
    soc_mmu_cfg_buf_pool_t pools_itm[TD4_X9_ITMS_PER_DEV][TD4_X9_MAX_SERVICE_POOLS];
    /*! Port config. */
    soc_mmu_cfg_buf_port_t ports[TD4_X9_MMU_NUM_LPORT];
    /*! Qgroup config. */
    soc_mmu_cfg_buf_qgroup_t qgroups[TD4_X9_MMU_NUM_QGROUP];
    /*! Mapping profiles. */
    soc_mmu_cfg_buf_mapping_profile_t mapprofiles[TD4_X9_MMU_NUM_PROFILES];
} soc_mmu_cfg_buf_t;

/*!
 * \brief Calcuate memory index for per-pipe memories
 *
 * \param [in] unit Unit number.
 * \param [in] port port number.
 * \param [in] mem memory SID.
 * \param [in] arr_off array offset.
 */
extern uint32_t
td4_x9_piped_mem_index(int unit, int port, bcmdrd_sid_t mem, int arr_off);


/*!
 * \brief Get pool mapped to a port,pg.
 *
 * \param [in] unit Unit number.
 * \param [in] lport port number.
 * \param [in] pg priority group number.
 * \param [out] itm ITM mapped to the port.
 * \param [out] pool pool mapped to the port,pg.
 * \param [in] is_hdrm hdrm/service pool map.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcm56780_a0_tm_thdi_pg_to_pool_mapping_get(int unit, int lport, int pg,
                                           int *itm, int *pool, bool is_hdrm);

/*!
 * \brief Function to update shared buffer.
 *
 * \param [in] unit Unit number.
 * \param [in] itm ITM number.
 * \param [in] pool service pool number.
 * \param [in] delta difference to be adjusted in shared space.
 * \param [in] reason reason for buffer pool update.
 * \param [in] check_only purpose only to check if delta is applicable.
 *
 * \retval SHR_E_RESOURCE Shared space insufficient for delta adjustment.
 * \retval SHR_E_NONE Success.
 */
extern int
bcm56780_a0_tm_shared_buffer_update(int unit, int itm, int pool, long int delta,
                                    shared_buffer_update_reason_t reason,
                                    bool check_only);

/*!
 * \brief Internal LT update for ingress shared pool.
 *
 * \param [in] unit Unit number.
 * \param [in] itm ITM number.
 * \param [in] pool service pool number.
 * \param [in] fval shared limit value.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcm56780_a0_tm_ing_pool_shared_limit_lt_update(int unit, int itm, int pool,
                                               uint32_t fval);

/*!
 * \brief Internal LT update for egress shared pool.
 *
 * \param [in] unit Unit number.
 * \param [in] itm ITM number.
 * \param [in] pool service pool number.
 * \param [in] fval shared limit value.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcm56780_a0_tm_egr_pool_shared_limit_lt_update(int unit, int itm, int pool,
                                               uint32_t fval);

/*!
 * \brief Allocate MMU config buffer structure.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_MEMORY Insufficient memory.
 * \retval SHR_E_NONE Success.
 */
extern soc_mmu_cfg_buf_t*
bcm56780_a0_mmu_buf_cfg_alloc(int unit);

/*!
 * \brief THDI port configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] port port number.
 * \param [in] buf MMU buffer config.
 * \param [in] devcfg MMU device config.
 * \param [in] lossless lossy/lossless mode of operation.
 *
 * \retval SHR_E_NONE Success.
 */

extern void
td4_x9_mmu_config_thdi_set(int unit, int port, soc_mmu_cfg_buf_t *buf,
                        soc_mmu_device_info_t *devcfg,  int lossless);

/*!
 * \brief THDO port configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] port port number.
 * \param [in] buf MMU buffer config.
 * \param [in] devcfg MMU device config.
 * \param [in] lossless lossy/lossless mode of operation.
 *
 * \retval SHR_E_NONE Success.
 */
extern void
td4_x9_mmu_config_thdo_set(int unit, int port, soc_mmu_cfg_buf_t *buf,
                        soc_mmu_device_info_t *devcfg, int lossless);

/*!
 * \brief MMU device configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] devcfg MMU device config.
 * \param [in] lossless lossy/lossless mode of operation.
 *
 * \retval SHR_E_NONE Success.
 */
extern void
td4_x9_mmu_init_dev_config(int unit, soc_mmu_device_info_t *devcfg, int lossless);

/*!
 * \brief Get default PG headroom value for port.
 *
 * \param [in] unit Unit number.
 * \param [in] port port number.
 */
extern int
td4_x9_default_pg_headroom(int unit, int port);
#endif /* BCM56880_A0_TM_THD_INTERNAL_H */
