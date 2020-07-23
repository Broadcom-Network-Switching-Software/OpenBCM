/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file bcm_int/dnx/cosq/ingress/ingress_congestion.h
 * Reserved.$ 
 */
#ifndef _DNX_INGRESS_CONGESTION_H_INCLUDED_
/*
 * { 
 */
#define _DNX_INGRESS_CONGESTION_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>/** for dnx_sw_state_pretty_print_t */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dbal/dbal.h>

/*
 * Defines:
 * {
 */

typedef enum
{
    /*
     *  VSQ group A - category
     */
    DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY = 0,
    /*
     *  VSQ group B - category and traffic class
     */
    DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY_TRAFFIC_CLS,
    /*
     *  VSQ group C - category and connection class
     */
    DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY_CNCTN_CLS,
    /*
     *  VSQ group D - statistics tag
     */
    DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG,
    /*
     *  VSQ group E - Link Level Flow Control
     */
    DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT,
    /*
     *  VSQ group F - Port Priority Flow Control
     */
    DNX_INGRESS_CONGESTION_VSQ_GROUP_PG,
    /*
     *  Global VSQ
     */
    DNX_INGRESS_CONGESTION_VSQ_GLOBAL,
    /*
     *  Must be the last value
     *  Global is not counted
     */
    DNX_INGRESS_CONGESTION_VSQ_GROUP_NOF = DNX_INGRESS_CONGESTION_VSQ_GLOBAL
} dnx_ingress_congestion_vsq_group_e;

/*
 * } 
 */

typedef enum
{
    DNX_INGRESS_CONGESTION_RESOURCE_INVALID = -1,
    DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES = 0,
    DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS,
    DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS,
    DNX_INGRESS_CONGESTION_RESOURCE_NOF
} dnx_ingress_congestion_resource_type_e;

/*
 * DRAM Bound resource types
 */
typedef enum
{
    DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_INVALID = -1,
    DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BYTES = 0,
    DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BUFFERS,
    DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_PDS,
    DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_NOF
} dnx_ingress_congestion_dram_bound_resource_type_e;

/*
 * DRAM Bound threshold types
 */
typedef enum
{
    DNX_INGRESS_CONGESTION_DRAM_BOUND = 0,
    DNX_INGRESS_CONGESTION_DRAM_BOUND_RECOVERY_FAILURE,
    DNX_INGRESS_CONGESTION_DRAM_BOUND_THR_TYPE_NOF
} dnx_ingress_congestion_dram_bound_threshold_type_e;

/*
 * ingress congestion management statistics types
 */
typedef enum
{
    DNX_INGRESS_CONGESTION_STATISTICS_BDBS = 0,
    DNX_INGRESS_CONGESTION_STATISTICS_SRAM_BUFFERS,
    DNX_INGRESS_CONGESTION_STATISTICS_SRAM_PDS,
    DNX_INGRESS_CONGESTION_STATISTICS_NOF
} dnx_ingress_congestion_statistics_types_t;

typedef enum
{
    /** VOQ Reject Mask */
    DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SRAM_PDS_TOTAL_SHARED = 0,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SRAM_PDS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SRAM_BUFFERS_TOTAL_SHARED,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SRAM_BUFFERS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_WORDS_TOTAL_SHARED,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_WORDS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SYSTEM_RED,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_LATENCY,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_WRED,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_DRAM_BLOCK,

    /** VSQ Reject Mask */
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_PDS_TOTAL_SHARED,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_PDS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_SRAM_PDS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_SRAM_PDS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_SRAM_PDS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_SRAM_PDS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_BUFFERS_TOTAL_SHARED,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_BUFFERS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_SRAM_BUFFERS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_SRAM_BUFFERS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_SRAM_BUFFERS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_SRAM_BUFFERS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_WORDS_TOTAL_SHARED,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_WORDS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_WORDS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_WORDS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_WORDS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_WORDS_MAX_SIZE,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_F_WORDS_WRED,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_WORDS_WRED,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_WORDS_WRED,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_WORDS_WRED,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_WORDS_WRED,
    DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_WORDS_WRED,

    /** Global Reject Mask */
    DNX_INGRESS_CONGESTION_REJECT_BIT_DRAM_BDBS_OCCUPANCY,
    DNX_INGRESS_CONGESTION_REJECT_BIT_SRAM_BUFFERS_OCCUPANCY,
    DNX_INGRESS_CONGESTION_REJECT_BIT_SRAM_PDS_OCCUPANCY,
    DNX_INGRESS_CONGESTION_REJECT_BIT_DP_LEVEL,

    /** total bits */
    DNX_INGRESS_CONGESTION_REJECT_BIT_NOF
} dnx_ingress_congestion_reject_bit_e;

typedef enum
{
    /** VOQ DRAM BLOCK Bit Mask */
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_DP_LEVEL = 0,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_GLBL_SRAM_PDBS,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_GLBL_SRAM_BUFFERS,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_GLBL_DRAM_BDBS,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VOQ_LATENCY_BIT,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VOQ_WORDS_MAX_QSIZE,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VOQ_WRED,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VOQ_SYS_RED_BIT,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VOQ_TOTAL_FREE_SHARED_WORDS,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VOQ_SRAM_BUFFERS_MAX_QSIZE,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VOQ_TOTAL_FREE_SHARED_SRAM_BUFFERS,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VOQ_SRAM_PDS_MAX_QSIZE,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VOQ_TOTAL_FREE_SHARED_SRAM_PDS,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VSQ_QUEUE_BASED_VSQ_WRED,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VSQ_PORT_BASED_VSQ_WRED,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VSQ_QUEUE_BASED_WORDS_MAX_QSIZE,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VSQ_PORT_BASED_WORDS_MAX_QSIZE,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VSQ_PORT_BASED_VSQ_TOTAL_FREE_SHARED_WORDS,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VSQ_QUEUE_BASED_SRAM_BUFFERS_MAX_QSIZE,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VSQ_PORT_BASED_SRAM_BUFFERS_MAX_QSIZE,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VSQ_PORT_BASED_VSQ_TOTAL_FREE_SHARED_SRAM_BUFFERS,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VSQ_QUEUE_BASED_SRAM_PDS_MAX_QSIZE,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VSQ_PORT_BASED_SRAM_PDS_MAX_QSIZE,
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_VSQ_PORT_BASED_VSQ_TOTAL_FREE_SHARED_SRAM_PDS,

    /** total bits */
    DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_NOF
} dnx_ingress_congestion_dram_block_bitmap_e;

typedef struct
{
    /*
     * Tail drop settings 
     */
    /*
     * Maximal VSQ size per type per DP 
     */
    uint32 max_size_th[DNX_INGRESS_CONGESTION_RESOURCE_NOF][DNX_COSQ_NOF_DP];
} dnx_ingress_congestion_vsq_rate_class_taildrop_info_t;

typedef struct
{
    /**
     *  Indicator:
     *  1 - enable WRED test
     *  0 - disable test
     *  For VSQ WRED, this indication is ignored.
     *  To enable VSQ WRED, use 'soc_jer2_tmcitm_vsq_wred_gen_set' API.
     */
    int wred_en;

    /**
     *  Relating to queue average size. Minimum Average Threshold
     *  - Below this threshold, packet is admitted into the
     *  queue. Resolution of bytes. The device resolution is of 16
     *  bytes. Has to be lower than max_avrg_th. 
     */
    uint32 min_avrg_th;

    /**
     *  Relating to queue average size. Maximum Average Threshold
     *  - Above this threshold, packet is discarded from the
     *  queue. Resolution of bytes. The device resolution is of 16
     *  bytes. Has to be higher than min_avrg_th.
     */
    uint32 max_avrg_th;

    /**
     *  The maximum probability of discarding a packet (when the
     *  queue reaches the maximum average size ('max_avrg_th')
     *  and the packet size is the maximum size
     *  ('max_packet_size')). 1% = 1, 23% = 23. Range: 0 - 100.
     */
    uint32 max_probability;
} dnx_ingress_congestion_wred_info_t;

typedef struct
{
    dnx_ingress_congestion_wred_info_t wred_params[DNX_COSQ_NOF_DP];

} dnx_ingress_congestion_vsq_rate_class_wred_info_t;

/**
 * PG parameters
 */
typedef struct
{
    int is_lossless;
    int pool_id;
    int admit_profile;
} dnx_ingress_congestion_pg_params_t;

typedef struct
{
    uint32 max_headroom;
    uint32 max_headroom_nominal;
    uint32 max_headroom_extension;
} dnx_ingress_congestion_vsq_rate_class_pg_headroom_info_t;

typedef struct
{
    dnx_ingress_congestion_vsq_rate_class_wred_info_t wred;
    /*
     * WRED exp. weight 
     */
    uint32 wred_exp_wq;

    dnx_ingress_congestion_vsq_rate_class_taildrop_info_t taildrop;
    dnx_cosq_hyst_threshold_t fc[DNX_INGRESS_CONGESTION_RESOURCE_NOF];
} dnx_ingress_congestion_queue_vsq_rate_class_info_t;

typedef struct
{
    dnx_ingress_congestion_vsq_rate_class_wred_info_t wred;

    dnx_cosq_hyst_threshold_t fc[DNX_INGRESS_CONGESTION_RESOURCE_NOF];

    uint32 guaranteed_size[DNX_INGRESS_CONGESTION_RESOURCE_NOF][DNX_COSQ_NOF_DP];
    uint32 shared_size[DNX_INGRESS_CONGESTION_RESOURCE_NOF][DNX_COSQ_NOF_DP];
    uint32 headroom_size[DNX_INGRESS_CONGESTION_RESOURCE_NOF];
} dnx_ingress_congestion_src_port_pool_vsq_rate_class_info_t;

typedef struct
{
    dnx_ingress_congestion_src_port_pool_vsq_rate_class_info_t pool[DNX_DATA_MAX_INGR_CONGESTION_VSQ_POOL_NOF];
    /*
     * WRED exp. weight 
     */
    uint32 wred_exp_wq;
} dnx_ingress_congestion_src_port_vsq_rate_class_info_t;

typedef struct
{
    dnx_ingress_congestion_vsq_rate_class_wred_info_t wred;

    /*
     * WRED exp. weight 
     */
    uint32 wred_exp_wq;

    dnx_cosq_fadt_hyst_threshold_t fadt_fc[DNX_INGRESS_CONGESTION_RESOURCE_NOF];

    uint32 guaranteed_size[DNX_INGRESS_CONGESTION_RESOURCE_NOF][DNX_COSQ_NOF_DP];

    dnx_cosq_fadt_threshold_t shared_size[DNX_INGRESS_CONGESTION_RESOURCE_NOF][DNX_COSQ_NOF_DP];

    dnx_ingress_congestion_vsq_rate_class_pg_headroom_info_t headroom_size[DNX_INGRESS_CONGESTION_RESOURCE_NOF];
} dnx_ingress_congestion_pg_vsq_rate_class_info_t;

/*
 * VSQ rate class data
 * can hold rate class data of any vsq type
 */
typedef struct
{
    dnx_ingress_congestion_vsq_group_e vsq_type;
    union
    {
        dnx_ingress_congestion_queue_vsq_rate_class_info_t vsq_a_d_rate_class_info;
        dnx_ingress_congestion_src_port_vsq_rate_class_info_t vsq_e_rate_class_info;
        dnx_ingress_congestion_pg_vsq_rate_class_info_t vsq_f_rate_class_info;
    } data;
} dnx_ingress_congestion_vsq_rate_class_info_t;

typedef struct
{
    uint8 pg[DNX_COSQ_NOF_TC];
} dnx_ingress_congestion_vsq_tc_pg_mapping_t;

/*
 * ingress congestion management statistics.
 */
typedef struct
{
    uint32 free_count[DNX_INGRESS_CONGESTION_STATISTICS_NOF];
    uint32 min_free_count[DNX_INGRESS_CONGESTION_STATISTICS_NOF];
} dnx_ingress_congestion_statistics_info_t;

/**
 * \brief Ingress congestion voq occupancy statistics.
 */
typedef struct
{
    /**
     * ID of current queue
     */
    uint32 id;
    /**
     * Total size in Bytes of current VOQ
     */
    uint32 size;
    /**
     * Total size in Bytes of current VOQ in SRAM
     */
    uint32 size_sram;
    /**
     * Total size in buffers of current VOQ
     */
    uint32 buffer_size;
    /**
     * Total size in PDS of current VOQ
     */
    uint32 nof_pds_in_sram;
    /**
     * Average size size in words in both DRAM and SRAM
     */
    uint32 size_avrg;
} dnx_ingress_congestion_voq_occupancy_info_t;

/*
 * ingress congestion management statistics.
 */
/**
 * \brief - Structure for collecting VSQ and VOQ counter info
 */
typedef struct
{
    /**
     * NOF Enqueued packets
     */
    int sram_enq_pkt_ovf;
    uint32 sram_enq_pkt_cnt;
    /**
     * NOF Rejected packets
     */
    int sram_disc_pkt_ovf;
    uint32 sram_disc_pkt_cnt;
    /**
     * NOF Dequeued packets from SRAM to DRAM
     */
    int sram_to_dram_pkt_ovf;
    uint32 sram_to_dram_pkt_cnt;
    /**
     * NOF Dequeued packets from SRAM to Fabric
     */
    int sram_to_fabric_pkt_ovf;
    uint32 sram_to_fabric_pkt_cnt;
    /**
     * NOF Deleted packets in SRAM
     */
    int sram_del_pkt_ovf;
    uint32 sram_del_pkt_cnt;
    /**
     * NOF Dequeued bundles
     */
    int dram_deq_bundle_ovf;
    uint32 dram_deq_bundle_cnt;
    /**
     * NOF Deleted bundles
     */
    int dram_del_bundle_ovf;
    uint32 dram_del_bundle_cnt;
    /**
     * NOF Enqueued bytes
     */
    int sram_enq_byte_ovf;
    uint64 sram_enq_byte_cnt;
    /**
     * NOF Rejected bytes
     */
    int sram_disc_byte_ovf;
    uint64 sram_disc_byte_cnt;
    /**
     * NOF Deleted bytes in SRAM
     */
    int sram_del_byte_ovf;
    uint64 sram_del_byte_cnt;
    /**
     * NOF Dequeued bytes from SRAM to DRAM
     */
    int sram_to_dram_byte_ovf;
    uint64 sram_to_dram_byte_cnt;
    /**
     * NOF Dequeued bytes from SRAM to Fabric
     */
    int sram_to_fabric_byte_ovf;
    uint64 sram_to_fabric_byte_cnt;
} dnx_ingress_congestion_counter_info_t;

/*
 * ingress congestion voq occupancy statistics.
 */
/**
 * \brief - 
 */
typedef struct
{
    /**
     * Total
     */
    uint32 total;
    /**
     * Guaranteed
     */
    uint32 guaranteed[DNX_DATA_MAX_INGR_CONGESTION_VSQ_POOL_NOF];

    /**
     * Shared
     */
    uint32 shared[DNX_DATA_MAX_INGR_CONGESTION_VSQ_POOL_NOF];
    /**
     * Headroom
     */
    uint32 headroom[DNX_DATA_MAX_INGR_CONGESTION_VSQ_POOL_NOF];
    /**
     * Headroom Extension - used only for VSQ F, has only 1 pool
     */
    uint32 headroom_ex;

} dnx_ingress_congestion_vsq_resource_spaces_t;

/**
 * \brief - Structure for collecting VSQ occupancy info
 */
typedef struct
{
    /**
     * ID of current queue in the VSQ group
     */
    uint32 vsq_id;
    /**
     * Average size in Bytes of current VSQ in the VSQ group
     */
    uint32 size_avrg;
    /**
     * Total size in all buffers in Bytes, OCB Buffesrs and OCB PDS 
     * of current VSQ in the VSQ group 
     */
    dnx_ingress_congestion_vsq_resource_spaces_t resources[DNX_INGRESS_CONGESTION_RESOURCE_NOF];

} dnx_ingress_congestion_vsq_occupancy_info_t;

/**
 * \brief - Structure for collecting the information about the last enqueued packet
 */
typedef struct
{
    /**
     *  Queue id of the last enqueued packed
     */
    uint32 qnum;
    /**
     * Packet size of the last enqueued packed
     */
    uint32 pkt_size;
    /**
     * Compensation
     */
    int compensation;
} dnx_ingress_congestion_last_enqueued_cmd_info_t;

/*
 * Exposing internal declarations -
 * the following functions are required in shell command "tm ingress queue map info" implementation.
 * see sh_dnx_tm_queue_mapping_info_cmd()
 * for information about these functions, see declaration in ingress_congestion_dbal.h
 */

/**
 * \brief - Get rate class profile for a VOQ
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id.
 * \param [in] voq - voq index
 * \param [out] rate_class - rate class profile
 *
 * \return shr_error_e
 */
extern shr_error_e dnx_ingress_congestion_dbal_voq_rate_class_get(
    int unit,
    int core,
    uint32 voq,
    uint32 *rate_class);

/**
 * \brief - Get traffic class for a VOQ
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id.
 * \param [in] voq - voq index
 * \param [out] traffic_class - traffic class profile
 *
 * \return shr_error_e
 */
extern shr_error_e dnx_ingress_congestion_dbal_voq_traffic_class_get(
    int unit,
    int core,
    uint32 voq,
    uint32 *traffic_class);

/**
 * \brief - Get connection class profile for a VOQ
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id.
 * \param [in] voq - voq index
 * \param [out] connection_class - connection class profile
 *
 * \return shr_error_e
 */
extern shr_error_e dnx_ingress_congestion_dbal_voq_connection_class_get(
    int unit,
    int core,
    uint32 voq,
    uint32 *connection_class);

/**
 * \brief - Callback to configure ingress congestion information
 *  for port to be added
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * port which is in process of being added
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   *  HW configuration of ingress port
 * \remark
 *   * Invoked during bcm_dnx_port_add
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_port_add_handle(
    int unit);

/**
 * \brief - Callback to clean ingress congestion data
 *  from port which is in process of removing
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * port which is in process of being removed
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   *  HW configuration of reassembly and port termination context
 * \remark
 *   * Invoked during bcm_dnx_port_remove
 *   * Must be invoked before Ingress Port handles port removing
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_port_remove_handle(
    int unit);

/**
 * \brief - allocate vsq-e id for non phy ports
 * 
 * \param [in] unit - chip unit id.
 * \param [in] src_port - source logical port
 * \param [out] vsq_e_id - allocated VSQ-E ID
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_vsq_e_id_alloc(
    int unit,
    bcm_port_t src_port,
    int *vsq_e_id);

/**
 * \brief - deallocate vsq-e id for non phy ports
 * 
 * \param [in] unit - chip unit id.
 * \param [in] src_port - source logical port
 * \param [in] vsq_e_id - VSQ-E ID
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_vsq_e_id_dealloc(
    int unit,
    bcm_port_t src_port,
    int vsq_e_id);

/**
 * \brief - allocate vsq-f id for non phy ports
 * 
 * \param [in] unit - chip unit id.
 * \param [in] src_port - source logical port
 * \param [in] numq - number of VSQ-Fs
 * \param [out] vsq_f_id - allocated VSQ-F base ID
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_vsq_f_id_alloc(
    int unit,
    bcm_port_t src_port,
    int numq,
    int *vsq_f_id);
/**
 * \brief - deallocate vsq-f id for non phy ports
 * 
 * \param [in] unit - chip unit id.
 * \param [in] src_port - source logical port
 * \param [in] numq - number of VSQ-Fs
 * \param [in] vsq_f_id - VSQ-F base ID
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_vsq_f_id_dealloc(
    int unit,
    bcm_port_t src_port,
    int numq,
    int vsq_f_id);

/**
 * \brief -
 * Get vsq to rate class mapping
 *
 * \param [in] unit - unit
 * \param [in] core - core
 * \param [in] vsq_type - VSQ type
 * \param [in] vsq_id - VSQ ID
 * \param [out] rt_cls - VSQ rate class
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_vsq_rate_class_mapping_get(
    int unit,
    int core,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    int vsq_id,
    int *rt_cls);

/*
 * brief - init configuration for VOQ programmable counters
 *
 * \param [in] unit - unit
 * \param [in] core - core
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_programmable_counter_voq_init_config_set(
    int unit,
    bcm_core_t core);

/*
 * brief - init configuration for VSQ programmable counters
 *
 * \param [in] unit - unit
 * \param [in] core - core
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_programmable_counter_vsq_init_config_set(
    int unit,
    bcm_core_t core);

/**
 * \brief - Function to configure the VOQ programmable counter 
 *        to track certain VOQ or VOQ bundle
 * 
 * \param [in] unit - unit
 * \param [in] core - core
 * \param [in] base_voq_id - base VOQ ID
 * \param [in] nof_voqs - number of VOQs
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_voq_programmable_counter_configuration_set(
    int unit,
    int core,
    uint32 base_voq_id,
    uint32 nof_voqs);

/**
 * \brief - Function to get all VOQ programmable counter 
 *        statistics
 * 
 * \param [in] unit - unit
 * \param [in] core - core
 * \param [out] counter_info - counter info
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_voq_programmable_counter_info_get(
    int unit,
    int core,
    dnx_ingress_congestion_counter_info_t * counter_info);

/** 
 *  \brief - Function to configure the VSQ programmable counter 
 *        to track certain VSQ and queue id
 * 
 * \param [in] unit - unit
 * \param [in] core - core
 * \param [in] vsq_group - VSQ type
 * \param [in] queue_id - VSQ id
 * \param [in] queue_id_provided - is VSQ ID provided
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_vsq_programmable_counter_configuration_set(
    int unit,
    int core,
    int vsq_group,
    int queue_id,
    int queue_id_provided);

/**
 * \brief - Function to get all VSQ programmable counter 
 *        statistics
 * 
 * \param [in] unit - unit index
 * \param [in] core - core index
 * \param [out] counter_info - counter info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_vsq_programmable_counter_info_get(
    int unit,
    int core,
    dnx_ingress_congestion_counter_info_t * counter_info);

/**
 * \brief - Function to get all the occupancy statistics for the 
 *        current VOQ defined by the dbal iterator.
 *      
 * \param [in] unit - unit index
 * \param [in] entry_handle_id - dbal entry handle
 * \param [out] voq_occupancy_info - occupancy info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_current_voq_occupancy_info_get(
    int unit,
    uint32 entry_handle_id,
    dnx_ingress_congestion_voq_occupancy_info_t * voq_occupancy_info);

/**
 * \brief - Function to get all the occupancy statistics for
 *        a specific VOQ.
 *
 * \param [in] unit - unit index
 * \param [in] core - core id
 * \param [in] queue_id - VOQ id
 * \param [out] voq_occupancy_info - occupancy info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_voq_occupancy_info_get(
    int unit,
    int core,
    int queue_id,
    dnx_ingress_congestion_voq_occupancy_info_t * voq_occupancy_info);

/**
 * \brief - Function to get all the occupancy statistics for the 
 *        current VSQ and its queue id defined by the dbal iterator.
 * 
 * \param [in] unit - unit index
 * \param [in] entry_handle_id - dbal entry handle
 * \param [in] vsq_group - VSQ type
 * \param [out] vsq_non_empty_info_array - info array
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_current_vsq_occupancy_info_get(
    int unit,
    uint32 entry_handle_id,
    int vsq_group,
    dnx_ingress_congestion_vsq_occupancy_info_t * vsq_non_empty_info_array);

/** print callbacks */

/**
 * \brief - print callback for VSQ A-D template manager
 * 
 * \param [in] unit - unit index
 * \param [in] data - template manager data
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
void dnx_ingress_congestion_alloc_vsq_a_d_rate_cls_print_cb(
    int unit,
    const void *data);
/**
 * \brief - print callback for VSQ E template manager
 * 
 * \param [in] unit - unit index
 * \param [in] data - template manager data
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
void dnx_ingress_congestion_alloc_vsq_e_rate_cls_print_cb(
    int unit,
    const void *data);
/**
 * \brief - print callback for VSQ F template manager
 * 
 * \param [in] unit - unit index
 * \param [in] data - template manager data
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
void dnx_ingress_congestion_alloc_vsq_f_rate_cls_print_cb(
    int unit,
    const void *data);

/**
 * \brief - print callback for TC->PG template manager
 * 
 * \param [in] unit - unit index
 * \param [in] data - template manager data
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
void dnx_ingress_congestion_alloc_vsq_pg_tc_mapping_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - stop usage of dram.
 *          future APIs will not update HW regarding dram bound rate class attributes until dram usage is restored
 *
 * \param [in] unit - unit index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * this function uses both legacy and new dnx mechanisms (mbcm and dbal)
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_dram_usage_stop(
    int unit);

/**
 * \brief - restore usage of dram,
 *          get from SW-State what should be the current state of all rate classes and set accordingly
 *
 * \param [in] unit - unit index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * this function uses both legacy and new dnx mechanisms (mbcm and dbal)
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_dram_usage_restore(
    int unit);

/**
 * \brief - get free resource count and minimum free resources count for all resource types
 *
 * \param [in] unit - unit index
 * \param [in] core - core id.
 * \param [out] stats - all counter values read from HW.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_free_resource_counter_get(
    int unit,
    int core,
    dnx_ingress_congestion_statistics_info_t * stats);

/**
 * \brief - get information for last enqueued packet
 *
 * \param [in] unit - unit index
 * \param [in] core - relevant core
 * \param [out] last_cmd_info - structure with information for the last enqueued packet
 *
 * \return
 *   shr_error_e
 *
 * \remark
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_dbal_last_enqueued_cmd_info_get(
    int unit,
    int core,
    dnx_ingress_congestion_last_enqueued_cmd_info_t * last_cmd_info);

/**
 * \brief
 *   Get voq dram bound state
 *
 * \param [in] unit - unit.
 * \param [in] core - core.
 * \param [in] queue_id - queue_id.
 * \param [out] dram_bound_status - dram_bound_status.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_dbal_voq_dram_bound_state_get(
    int unit,
    int core,
    int queue_id,
    dbal_enum_value_field_dram_bound_state_e * dram_bound_status);

/**
 * \brief
 *   Set VSQ global drop threshold
 *
 * \param [in] unit - unit.
 * \param [in] core_id - core id
 * \param [in] index_type - index type
 * \param [in] index_info - index info
 * \param [in] thresh_info - threshold info
 * \param [in] threshold - threshold
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ingress_congestion_vsq_gl_drop_threshold_set(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold);

/**
 * \brief
 *   Get VSQ global drop threshold
 *
 * \param [in] unit - unit.
 * \param [in] core_id - core id.
 * \param [in] index_type - index type
 * \param [in] index_info - index info
 * \param [in] thresh_info - threshold info
 * \param [out] threshold - threshold
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ingress_congestion_vsq_gl_drop_threshold_get(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold);

/**
 * \brief
 *   Set global threshold
 *
 * \param [in] unit - unit.
 * \param [in] flags - flags
 * \param [in] index_type - index type
 * \param [in] index_info - index info
 * \param [in] thresh_info - threshold info
 * \param [in] threshold - threshold
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ingress_congestion_global_device_static_threshold_set(
    int unit,
    uint32 flags,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold);

/**
 * \brief
 *   Get global threshold
 *
 * \param [in] unit - unit.
 * \param [in] flags - flags
 * \param [in] index_type - index type
 * \param [in] index_info - index info
 * \param [in] thresh_info - threshold info
 * \param [out] threshold - threshold
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ingress_congestion_global_device_static_threshold_get(
    int unit,
    uint32 flags,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold);

/**
 * \brief
 * set FC threshold for VSQ
 * 
 * \param [in] unit - unit.
 * \param [in] gport - gport
 * \param [in] cosq - cosq
 * \param [in] flags - flags
 * \param [in] pfc_threshold - FC threshold
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ingress_congestion_vsq_fc_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 flags,
    bcm_cosq_pfc_config_t * pfc_threshold);

/**
 * \brief
 * get FC threshold for VSQ
 * 
 * \param [in] unit - unit.
 * \param [in] gport - gport
 * \param [in] cosq - cosq
 * \param [in] flags - flags
 * \param [out] pfc_threshold - FC threshold
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ingress_congestion_vsq_fc_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 flags,
    bcm_cosq_pfc_config_t * pfc_threshold);

/**
 * \brief - set rate class profile to operate only on OCB.
 *
 * \param [in] unit - The unit number.
 * \param [in] gport - rate class profile gport
 * \param [in] cosq - irrelevant here. set to 0
 * \param [in] is_ocb_only - indication is rate class should opearte only on OCB or OCB + DRAM.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *    dnx_ingress_congestion_gport_threshold_set
 */
shr_error_e dnx_ingress_congestion_voq_ocb_only_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int is_ocb_only);

/**
 * \brief - Get rate class profile indication if operates only on OCB.
 *
 * \param [in] unit - The unit number.
 * \param [in] gport - rate class profile gport
 * \param [in] cosq - irrelevant here. set to 0
 * \param [out] is_ocb_only - indication is rate class opeartes only on OCB or OCB + DRAM.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *    dnx_ingress_congestion_gport_threshold_set
 */
shr_error_e dnx_ingress_congestion_voq_ocb_only_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *is_ocb_only);

/**
 * \brief - Configure global drop per DP
 *
 * \param [in] unit - The unit number.
 * \param [in] flags - flags to set which DP to discard. see BCM_COSQ_DISCARD_*
 * supported flags:
 *      BCM_COSQ_DISCARD_ENABLE
 *      BCM_COSQ_DISCARD_COLOR_ALL
 *      BCM_COSQ_DISCARD_COLOR_YELLOW
 *      BCM_COSQ_DISCARD_COLOR_RED
 *      BCM_COSQ_DISCARD_COLOR_BLACK
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 */
shr_error_e dnx_ingress_congestion_discard_set(
    int unit,
    uint32 flags);

/**
 * \brief - Configure global drop per DP
 *
 * \param [in] unit - The unit number.
 * \param [out] flags - indication for which DP is discarded.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *  dnx_ingress_congestion_discard_set
 */
shr_error_e dnx_ingress_congestion_discard_get(
    int unit,
    uint32 *flags);

/**
 * \brief - set threshold according to gport type.
 *
 * \param [in] unit - The unit number.
 * \param [in] gport - rate class profile gport
 * \param [in] cosq - irrelevant here. set to 0
 * \param [in] threshold - threshold info. relevant fields:
 *              - flags => threshold specification. currently supported only BCM_COSQ_THRESHOLD_QSIZE_RECOVERY
 *              - type => threshold type. currently supported only bcmCosqThresholdBytes.
 *              - value => threshold value to set.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 */
shr_error_e dnx_ingress_congestion_gport_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold);

/**
 * \brief - get threshold according to gport type.
 *
 * \param [in] unit - The unit number.
 * \param [in] gport - rate class profile gport
 * \param [in] cosq - irrelevant here. set to 0
 * \param [in,out] threshold - threshold info.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *    dnx_ingress_congestion_gport_threshold_set
 */
shr_error_e dnx_ingress_congestion_gport_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold);

/**
 * \brief - Map VOQ to rate class profile.
 *
 * \param [in] unit - The unit number.
 * \param [in] core - device core id.
 * \param [in] voq - VOQ number to map.
 * \param [in] rate_class - rate class profile to map the VOQ to.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_voq_to_rate_class_profile_map_set(
    int unit,
    int core,
    int voq,
    int rate_class);

/**
 * \brief - get mapped rate class profile from VOQ.
 *
 * \param [in] unit - The unit number.
 * \param [in] core - device core id.
 * \param [in] voq - VOQ number.
 * \param [in] rate_class - rate class profile mapped to the given VOQ.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_voq_to_rate_class_profile_map_get(
    int unit,
    int core,
    int voq,
    int *rate_class);

/**
 * \brief -
 * retrieve VSQ information
 * 
 * \param [in] unit - unit.
 * \param [in] vsq - vsq gport
 * \param [out] vsq_info - VSQ info
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ingress_congestion_vsq_info_get(
    int unit,
    bcm_gport_t vsq,
    bcm_cosq_vsq_info_t * vsq_info);

/**
 * \brief - Set DRAM bound / recovery fail thresholds (FADT).
 *
 * \param [in] unit - The unit number.
 * \param [in] fadt_info - which FADT threshold to set:
 *             - gport => rate class profile gport
 *             - cosq => not relevant. is set to 0
 *             - resource_type => SRAM bytes / buffers / PDs
 *             - thresh_type => dram bound / dram recovery fail
 * \param [in] threshold - fadt values to set.
 *             - thresh_min => min value for FADT algorithm
 *             - thresh_max => max value for FADT algorithm
 *             - alpha - alpha fsctor for FADT algoritm
 *             - resource_range_min - apply FADT algorithm only when free_res >= resource_range_min
 *             - resource_range_max - apply FADT algorithm only when free_res <= resource_range_max.
 *             NOTE: resource_range is not supported in all resource types. if it is not supported these values will be set to 0.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_dram_bound_fadt_threshold_set(
    int unit,
    bcm_cosq_fadt_info_t * fadt_info,
    bcm_cosq_fadt_threshold_t * threshold);

/**
 * \brief - Get DRAM bound / recovery fail thresholds (FADT).
 *
 * \param [in] unit - The unit number.
 * \param [in] fadt_info - which FADT threshold to get
 * \param [out] threshold - fadt values to set.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_ingress_congestion_dram_bound_fadt_threshold_set
 */
shr_error_e dnx_ingress_congestion_dram_bound_fadt_threshold_get(
    int unit,
    bcm_cosq_fadt_info_t * fadt_info,
    bcm_cosq_fadt_threshold_t * threshold);

/**
 * \brief - Set VOQ congestion notification FADT threshold
 *
 * \param [in] unit - The unit number.
 * \param [in] flags - specific flags. currently no flags supported
 * \param [in] fadt_info - info abouth the threshold:
 *             - gport => rate class profile gport
 *             - cosq => not relevant. is set to 0
 *             - resource_type => bytes / SRAM buffers / SRAM PDs
 *             - thresh_type => congestion notification
 * \param [in] threshold - threshold info.
 *             - thresh_min => min value for FADT algorithm
 *             - thresh_max => max value for FADT algorithm
 *             - alpha - alpha fsctor for FADT algoritm
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 */
shr_error_e dnx_ingress_congestion_voq_congestion_notification_fadt_threshold_set(
    int unit,
    uint32 flags,
    bcm_cosq_fadt_info_t * fadt_info,
    bcm_cosq_fadt_threshold_t * threshold);

/**
 * \brief - Get VOQ congestion notification FADT threshold
 *
 * \param [in] unit - The unit number.
 * \param [in] flags - specific flags. currently no flags supported
 * \param [in] fadt_info - info about the threshold.
 * \param [in,out] threshold - threshold info.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *    dnx_ingress_congestion_voq_congestion_notification_fadt_threshold_set
 */
shr_error_e dnx_ingress_congestion_voq_congestion_notification_fadt_threshold_get(
    int unit,
    uint32 flags,
    bcm_cosq_fadt_info_t * fadt_info,
    bcm_cosq_fadt_threshold_t * threshold);

/**
 * \brief -
 * convert BCM API cosq resource to internal resource
 * 
 * \param [in] unit - unit.
 * \param [in] cosq_resource -  BCM API cosq resource
 * \param [out] ingress_resource - internal resource
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ingress_congestion_cosq_resource_to_ingress_resource(
    int unit,
    bcm_cosq_resource_t cosq_resource,
    dnx_ingress_congestion_resource_type_e * ingress_resource);

/**
 * \brief - Set mirror on drop aging time per group type
 *
 * \param [in] unit - unit.
 * \param [in] type - Group type (bcmCosqControlMirrorOnDropAgingGlobal, bcmCosqControlMirrorOnDropAgingVoq,
 *                     bcmCosqControlMirrorOnDropAgingVsq)
 * \param [in] arg - Aging time in microseconds units
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_mirror_on_drop_aging_set(
    int unit,
    bcm_cosq_control_t type,
    int arg);

/**
 * \brief - Get mirror on drop aging time per group type
 *
 * \param [in] unit - unit.
 * \param [in] type - Group type (bcmCosqControlMirrorOnDropAgingGlobal, bcmCosqControlMirrorOnDropAgingVoq,
 *                     bcmCosqControlMirrorOnDropAgingVsq)
 * \param [out] arg - Aging time in microseconds units
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_mirror_on_drop_aging_get(
    int unit,
    bcm_cosq_control_t type,
    int *arg);

/**
 * \brief -
 * Gets FADT drop and guaranteed parameters - per rate-class and drop precedence.
 * 
 * \param [in] unit - unit.
 * \param [in] gport - VOQ rate class gport
 * \param [in] color - DP
 * \param [in] flags - API flags
 * \param [in] get_hw - should get be done from HW
 * \param [out] gport_size - FADT drop and guaranteed parameters
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_voq_rate_class_color_size_dual_get(
    int unit,
    bcm_gport_t gport,
    bcm_color_t color,
    uint32 flags,
    int get_hw,
    bcm_cosq_gport_size_t * gport_size);

/**
 * \brief - Get DRAM bound threshold (FADT) from HW. per rate class profile
 *
 * \param [in] unit - The unit number.
 * \param [in] resource - resource type to set the thresholds (SRAM bytes/ buffers/ PDs).
 * \param [in] rt_cls - rate class profile ID
 * \param [out] fadt - fadt thresholds. note that for some resources, there can be a free-resources limit on the FADT algorithm.
 *             it means the FADT algorithm will be used only when the number of free resources is between the specified MIN and MAX values.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_voq_rate_class_fadt_dram_bound_get(
    int unit,
    dnx_ingress_congestion_dram_bound_resource_type_e resource,
    uint32 rt_cls,
    bcm_cosq_fadt_threshold_t * fadt);

/**
 * \brief -Get DRAM recovery fail threshold (FADT) from HW. per rate class profile
 *
 * \param [in] unit - The unit number.
 * \param [in] resource - resource type to set the thresholds (SRAM bytes/ buffers/ PDs).
 * \param [in] rt_cls - rate class profile ID
 * \param [out] fadt - fadt thresholds. note that for some resources, there can be a free-resources limit on the FADT algorithm.
 *             it means the FADT algorithm will be used only when the number of free resources is between the specified MIN and MAX values.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_voq_rate_class_fadt_dram_recovery_fail_get(
    int unit,
    dnx_ingress_congestion_dram_bound_resource_type_e resource,
    uint32 rt_cls,
    bcm_cosq_fadt_threshold_t * fadt);

/**
 * \brief - Get DRAM recovery threshold from HW. per rate class profile
 *
 * \param [in] unit - The unit number.
 * \param [in] rt_cls - rate class profile ID
 * \param [out] recovery_threshold - recovery threshold in bytes
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_voq_rate_class_dram_recovery_get(
    int unit,
    uint32 rt_cls,
    uint32 *recovery_threshold);

/**
 * \brief -
 * get VOQ state - is the VOQ in DRAM
 *
 * \param [in] unit - unit.
 * \param [in] voq_gport -  VOQ group gport
 * \param [in] cosq -  queue offset
 * \param [out] is_in_dram - indication if the VOQ is in DRAM
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_voq_is_in_dram_get(
    int unit,
    bcm_gport_t voq_gport,
    bcm_cos_queue_t cosq,
    int *is_in_dram);

/**
 * \brief -
 * get PG params
 */
shr_error_e dnx_ingress_congestion_dbal_pg_params_get(
    int unit,
    int core,
    int pg,
    dnx_ingress_congestion_pg_params_t * pg_params);

/**
 * \brief - Function to get all the current occupancy statistics per VSQ ID.
 *
 * \param [in] unit - unit index
 * \param [in] core - device core
 * \param [in] vsq_group - VSQ type
 * \param [in] vsq_id - VSQ index
 * \param [in] pool_id - Shared memory pool id (relevant for VSQE)
 * \param [out] info - occupancy info array
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_vsq_id_current_occupancy_info_get(
    int unit,
    int core,
    int vsq_group,
    int vsq_id,
    int pool_id,
    dnx_ingress_congestion_vsq_occupancy_info_t * info);

/**
 * \brief - Function to get all the current occupancy statistics per VSQ ID.
 *
 * \param [in] unit - unit index
 * \param [in] core - device core
 * \param [in] vsq_group - VSQ type
 * \param [in] vsq_id - VSQ index
 * \param [in] pool_id - Shared memory pool id (relevant for VSQE)
 * \param [out] info - max occupancy info array
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_vsq_id_max_occupancy_info_get(
    int unit,
    int core,
    int vsq_group,
    int vsq_id,
    int pool_id,
    dnx_ingress_congestion_vsq_occupancy_info_t * info);

/**
 * \brief -  Function get the appropriate DBAL table and fields from  the COSQ resource format
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] cosq_resource - COSQ resource
 *   \param [out] table - DBAL table
 *   \param [out] thresh_field - DBAL field for SET threshold, CLEAR threshold is the same
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_voq_gl_ecn_resource_to_dbal_info(
        int unit,
        bcm_cosq_resource_t cosq_resource,
        dbal_tables_e * table,
        dbal_fields_e * thresh_field);

/**
 * \brief -  Function set the thresholds for ECN marking from global resources.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] index_info - additional information derived from the index
 *   \param [in] thresh_info - complementary information describing the threshold to be set
 *   \param [in] threshold - threshold value
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_global_ecn_marking_threshold_set(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold);

/**
 * \brief -  Function get the thresholds for ECN marking from global resources.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] index_info - additional information derived from the index
 *   \param [in] thresh_info - complementary information describing the threshold to be set
 *   \param [out] threshold - threshold value
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_global_ecn_marking_threshold_get(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold);

/** } */
#endif /*_DNX_INGRESS_CONGESTION_H_INCLUDED_*/
