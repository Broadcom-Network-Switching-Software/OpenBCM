/*! \file bcm56880_a0_tm_thd_internal.h
 *
 * File containing macro defines used in mmu threshold init sequence for
 * bcm56880_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_TM_THD_INTERNAL_H
#define BCM56880_A0_TM_THD_INTERNAL_H

#include <bcmtm/chip/bcm56880_a0_tm.h>
#include <bcmtm/thd/bcmtm_thd_internal.h>

/*! Alpha values for dynamic threshold. */
typedef enum soc_td4_drop_limit_alpha_value_e {
    SOC_TD4_COSQ_DROP_LIMIT_ALPHA_1_128, /* 1/128 */
    SOC_TD4_COSQ_DROP_LIMIT_ALPHA_1_64,  /* 1/64 */
    SOC_TD4_COSQ_DROP_LIMIT_ALPHA_1_32,  /* 1/32 */
    SOC_TD4_COSQ_DROP_LIMIT_ALPHA_1_16,  /* 1/16 */
    SOC_TD4_COSQ_DROP_LIMIT_ALPHA_1_8,   /* 1/8 */
    SOC_TD4_COSQ_DROP_LIMIT_ALPHA_1_4,   /* 1/4 */
    SOC_TD4_COSQ_DROP_LIMIT_ALPHA_1_2,   /* 1/2 */
    SOC_TD4_COSQ_DROP_LIMIT_ALPHA_1,     /* 1 */
    SOC_TD4_COSQ_DROP_LIMIT_ALPHA_2,     /* 2 */
    SOC_TD4_COSQ_DROP_LIMIT_ALPHA_4,     /* 4 */
    SOC_TD4_COSQ_DROP_LIMIT_ALPHA_8,     /* 8 */
    SOC_TD4_COSQ_DROP_LIMIT_ALPHA_COUNT  /* Must be last! */
} soc_td4_drop_limit_alpha_value_t;

/*! TD4 MMU number of device ports. */
#define TD4_MMU_NUM_LPORT                       160
/*! TD4 MMU number of service pools. */
#define TD4_MMU_NUM_POOL                        4
/*! TD4 MMU number of priority groups. */
#define TD4_MMU_NUM_PG                          8
/*! TD4 MMU number of internal priorities. */
#define TD4_MMU_NUM_INT_PRI                     16
/*! TD4 MMU number of config profiles. */
#define TD4_MMU_NUM_PROFILES                    8
/*! TD4 MMU number of queue groups. */
#define TD4_MMU_NUM_QGROUP                      TD4_MMU_NUM_LPORT
/*! TD4 MMU number of RQE queues. */
#define TD4_MMU_NUM_RQE_QUEUES                  9

/*! TD4 MMU maximum packet size in bytes. */
#define TD4_MMU_MAX_PACKET_BYTES                9416
/*! TD4 MMU jumbo frame size in bytes. */
#define TD4_MMU_JUMBO_FRAME_BYTES               9216
/*! TD4 MMU default MTU frame size in bytes. */
#define TD4_MMU_DEFAULT_MTU_BYTES               1536
/*! TD4 MMU packet header size in bytes. */
#define TD4_MMU_PACKET_HEADER_BYTES             64
/*! TD4 MMU number of bytes per cell. */
#define TD4_MMU_BYTES_PER_CELL                  254
/*! TD4 MMU cell limit granularity. */
#define TD4_GRANULARITY                         8

/*! TD4 MMU available physical cells per ITM is 880 SKU. */
#define BCM56880_A0_MMU_PHYSICAL_CELLS_PER_ITM  278528
/*! TD4 MMU usable cells per ITM is 880 SKU. */
#define BCM56880_A0_MMU_TOTAL_CELLS_PER_ITM     265476
/*! TD4 MMU shared cells per ITM is 880 SKU. */
#define BCM56880_A0_MMU_TOTAL_SHARED_PER_ITM    264020

/*! TD4 MMU available physical cells per ITM is 883 SKU. */
#define BCM56883_A0_MMU_PHYSICAL_CELLS_PER_ITM  139264
/*! TD4 MMU usable cells per ITM is 883 SKU. */
#define BCM56883_A0_MMU_TOTAL_CELLS_PER_ITM     126212
/*! TD4 MMU shared cells per ITM is 883 SKU. */
#define BCM56883_A0_MMU_TOTAL_SHARED_PER_ITM    124084

/*! TD4 MMU CFAP reserved cells per ITM. */
#define TD4_MMU_RSVD_CELLS_CFAP_PER_ITM         408
/*! TD4 MMU MC queue entries per ITM. */
#define TD4_MMU_MCQ_ENTRY_PER_ITM               20480
/*! TD4 MMU RQE queue entries per ITM. */
#define TD4_MMU_RQE_ENTRY_PER_ITM               6144

/*! TD4 MMU MC queue entries per ITM. */
#define TD4_MMU_TOTAL_MCQ_ENTRY(unit)           TD4_MMU_MCQ_ENTRY_PER_ITM
/*! TD4 MMU RQE queue entries per ITM. */
#define TD4_MMU_TOTAL_RQE_ENTRY(unit)           TD4_MMU_RQE_ENTRY_PER_ITM

/* Default values for thresholds */
/*! TD4 MMU default value for CFAP bank full limit. */
#define TD4_CFAP_BANK_FULL_LIMIT                4092
/*! TD4 MMU default value for last packet accept mode. */
#define TD4_LAST_PKT_ACCEPT_MODE_POOL_DEFAULT   15
/*! TD4 MMU default value for color enable. */
#define TD4_COLOR_AWARE_DEFAULT                 0
/*! TD4 MMU default value for SPID override. */
#define TD4_SPID_OVERRIDE_ENABLE_DEFAULT        0
/*! TD4 MMU default value for yellow offset. */
#define TD4_SPAP_YELLOW_OFFSET_DEFAULT          0
/*! TD4 MMU default value for red offset. */
#define TD4_SPAP_RED_OFFSET_DEFAULT             0
/*! TD4 MMU default value for RQE queue color and dynamic enables. */
#define TD4_THDR_QE_CONFIG1_VALUE               0x7
/*! TD4 MMU default value for RQE queue shared limit alpha. */
#define TD4_THDR_QE_CONIFG_PRI_LIMIT            8
/*! TD4 MMU default value for RQE queue min limit. */
#define TD4_THDR_QE_LIMIT_MIN                   7
/*! TD4 MMU default value for RQE queue reset offset. */
#define TD4_MMU_THDR_QE_RESET_OFFSET 0xe
/*! TD4 MMU default value for RQE pool shared limit. */
#define TD4_THDR_QE_SP_SHARED_LIMIT 0x16c1
/*! TD4 MMU default value for RQE pool resume limit. */
#define TD4_THDR_QE_SP_RESUME_LIMIT 0x16b3

/* Oversub XON/XOFF Threshold */
/*! TD4 OBM entry length. */
#define TD4_OBM_ENTRY_LEN                       64
/*! TD4 OBM XON threshold for 10G port. */
#define TD4_XON_THRESHOLD_10G                   73  * TD4_OBM_ENTRY_LEN
/*! TD4 OBM XON threshold for 25G port. */
#define TD4_XON_THRESHOLD_25G                   73  * TD4_OBM_ENTRY_LEN
/*! TD4 OBM XON threshold for 50G port. */
#define TD4_XON_THRESHOLD_50G                   73  * TD4_OBM_ENTRY_LEN
/*! TD4 OBM XON threshold for 100G port. */
#define TD4_XON_THRESHOLD_100G                  186 * TD4_OBM_ENTRY_LEN
/*! TD4 OBM XON threshold for 200G port. */
#define TD4_XON_THRESHOLD_200G                  376 * TD4_OBM_ENTRY_LEN
/*! TD4 OBM XON threshold for 400G port. */
#define TD4_XON_THRESHOLD_400G                  760 * TD4_OBM_ENTRY_LEN
/*! TD4 OBM XOFF threshold for 10G port. */
#define TD4_XOFF_THRESHOLD_10G                  81  * TD4_OBM_ENTRY_LEN
/*! TD4 OBM XOFF threshold for 25G port. */
#define TD4_XOFF_THRESHOLD_25G                  81  * TD4_OBM_ENTRY_LEN
/*! TD4 OBM XOFF threshold for 50G port. */
#define TD4_XOFF_THRESHOLD_50G                  81  * TD4_OBM_ENTRY_LEN
/*! TD4 OBM XOFF threshold for 100G port. */
#define TD4_XOFF_THRESHOLD_100G                 194 * TD4_OBM_ENTRY_LEN
/*! TD4 OBM XOFF threshold for 200G port. */
#define TD4_XOFF_THRESHOLD_200G                 384 * TD4_OBM_ENTRY_LEN
/*! TD4 OBM XOFF threshold for 400G port. */
#define TD4_XOFF_THRESHOLD_400G                 768 * TD4_OBM_ENTRY_LEN

/*PG headroom*/
/*! TD4 MMU PG headroom value for linerate 10G port. */
#define TD4_PG_HEADROOM_LINERATE_10G            167
/*! TD4 MMU PG headroom value for linerate 25G port. */
#define TD4_PG_HEADROOM_LINERATE_25G            256
/*! TD4 MMU PG headroom value for linerate 50G port. */
#define TD4_PG_HEADROOM_LINERATE_50G            360
/*! TD4 MMU PG headroom value for linerate 100G port. */
#define TD4_PG_HEADROOM_LINERATE_100G           580
/*! TD4 MMU PG headroom value for linerate 200G port. */
#define TD4_PG_HEADROOM_LINERATE_200G           1061
/*! TD4 MMU PG headroom value for linerate 400G port. */
#define TD4_PG_HEADROOM_LINERATE_400G           1962
/*! TD4 MMU PG headroom value for oversub 10G port. */
#define TD4_PG_HEADROOM_OVERSUB_10G    \
    ( TD4_PG_HEADROOM_LINERATE_10G + ceiling( TD4_XOFF_THRESHOLD_10G,64))
/*! TD4 MMU PG headroom value for oversub 25G port. */
#define TD4_PG_HEADROOM_OVERSUB_25G    \
    ( TD4_PG_HEADROOM_LINERATE_25G + ceiling( TD4_XOFF_THRESHOLD_25G,64))
/*! TD4 MMU PG headroom value for oversub 50G port. */
#define TD4_PG_HEADROOM_OVERSUB_50G    \
    ( TD4_PG_HEADROOM_LINERATE_50G + ceiling( TD4_XOFF_THRESHOLD_50G,64))
/*! TD4 MMU PG headroom value for oversub 100G port. */
#define TD4_PG_HEADROOM_OVERSUB_100G    \
    ( TD4_PG_HEADROOM_LINERATE_100G + ceiling( TD4_XOFF_THRESHOLD_100G,64))
/*! TD4 MMU PG headroom value for oversub 200G port. */
#define TD4_PG_HEADROOM_OVERSUB_200G    \
    ( TD4_PG_HEADROOM_LINERATE_200G + ceiling( TD4_XOFF_THRESHOLD_200G,64))
/*! TD4 MMU PG headroom value for oversub 400G port. */
#define TD4_PG_HEADROOM_OVERSUB_400G    \
    ( TD4_PG_HEADROOM_LINERATE_400G + ceiling( TD4_XOFF_THRESHOLD_400G,64))

/*! TD4 MMU buffer mapping profile. */
typedef struct soc_mmu_cfg_buf_mapping_profile_s
{
     /*! Input pri to PG mapping for UC packets. */
     int inpri_to_prigroup_uc[TD4_MMU_NUM_INT_PRI];
     /*! Input pri to PG mapping for MC packets. */
     int inpri_to_prigroup_mc[TD4_MMU_NUM_INT_PRI];
     /*! PG to service pool map. */
     int prigroup_to_servicepool[TD4_MMU_NUM_PG];
     /*! PG to headroom pool map. */
     int prigroup_to_headroompool[TD4_MMU_NUM_PG];
     /*! PFC priority to PG map. */
     int pfcpri_to_prigroup[TD4_MMU_NUM_PG];
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
    soc_mmu_cfg_buf_prigroup_t prigroups[TD4_MMU_NUM_PG];
    /*! Queue config associated with port. */
    soc_mmu_cfg_buf_queue_t *queues;
    /*! Priority to PG map. */
    int pri_to_prigroup[TD4_MMU_NUM_INT_PRI]; /* from mapping variable */
    /*! Pool config associated with port. */
    soc_mmu_cfg_buf_port_pool_t pools[TD4_MAX_SERVICE_POOLS];
} soc_mmu_cfg_buf_port_t;

/*! TD4 MMU buffer threhsolds. */
typedef struct soc_mmu_cfg_buf_s
{
    /*! Service pool config. */
    soc_mmu_cfg_buf_pool_t pools[TD4_MAX_SERVICE_POOLS];
    /*! Service pool config per ITM. */
    soc_mmu_cfg_buf_pool_t pools_itm[TD4_ITMS_PER_DEV][TD4_MAX_SERVICE_POOLS];
    /*! Port config. */
    soc_mmu_cfg_buf_port_t ports[TD4_MMU_NUM_LPORT];
    /*! Qgroup config. */
    soc_mmu_cfg_buf_qgroup_t qgroups[TD4_MMU_NUM_QGROUP];
    /*! Mapping profiles. */
    soc_mmu_cfg_buf_mapping_profile_t mapprofiles[TD4_MMU_NUM_PROFILES];
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
td4_piped_mem_index(int unit, int port, bcmdrd_sid_t mem, int arr_off);

/*!
 * \brief Get ITM of ingress port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport port number.
 * \param [out] itm ITM mapped to the port.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcm56880_a0_tm_thdi_port_to_itm_mapping_get(int unit, int lport, int *itm);

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
bcm56880_a0_tm_thdi_pg_to_pool_mapping_get(int unit, int lport, int pg,
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
bcm56880_a0_tm_shared_buffer_update(int unit, int itm, int pool, long int delta,
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
bcm56880_a0_tm_ing_pool_shared_limit_lt_update(int unit, int itm, int pool,
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
bcm56880_a0_tm_egr_pool_shared_limit_lt_update(int unit, int itm, int pool,
                                               uint32_t fval);

/*!
 * \brief Allocate MMU config buffer structure.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_MEMORY Insufficient memory.
 * \retval SHR_E_NONE Success.
 */
extern soc_mmu_cfg_buf_t* soc_mmu_buf_cfg_alloc(int unit);

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
td4_mmu_config_thdi_set(int unit, int port, soc_mmu_cfg_buf_t *buf,
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
td4_mmu_config_thdo_set(int unit, int port, soc_mmu_cfg_buf_t *buf,
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
td4_mmu_init_dev_config(int unit, soc_mmu_device_info_t *devcfg, int lossless);

/*!
 * \brief Get default PG headroom value for port.
 *
 * \param [in] unit Unit number.
 * \param [in] port port number.
 */
extern int
td4_default_pg_headroom(int unit, int port);
#endif /* BCM56880_A0_TM_THD_INTERNAL_H */
