/*! \file bcmtm_internal_tdm.h
 *
 * TM internal TDM calendar.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_INTERNAL_TDM_H
#define BCMTM_INTERNAL_TDM_H

#include <bcmtm/tdm/bcmtm_tdm_top.h>
#include <bcmtm/bcmtm_types.h>

/*! Number of pipes. */
#define SOC_MAX_NUM_PIPES           4
/*! Number of ports. */
#define SOC_MAX_NUM_PORTS           256
/*! Number of MMU ports. */
#define SOC_MAX_NUM_MMU_PORTS       136
/*! Maximum number of lanes. */
#define SOC_PORT_RESOURCE_LANES_MAX 4

/*! port bimap type. */
typedef bcmdrd_pbmp_t pbmp_t;
/*
 * Lane Information
 *
 * Contains information on a given lane for a port.
 *
 * A port that uses 'x' number of lanes will have 'x' soc_port_lane_info_t
 * elements, one for each of the lanes that the port will use.
 */
/*! port lane information. */
typedef struct soc_port_lane_info_s {
    /*! PGW instance where lane resides */
    int pgw;
    /*! XLP number within PGW */
    int xlp;
    /*! TSC number (Same as XLP) */
    int tsc;
    /*! Index within XLP */
    int port_index;
} soc_port_lane_info_t;


/*! Port resource configuration used for FlexPort operations. */
typedef struct soc_port_resource_s {
    /*! flags. */
    uint32_t flags;
    /*! BCMI_XGS5_PORT_RESOURCE_OP_XXX */
    uint32_t op;
    /*! Logical port associated to physical port */
    int logical_port;
    /*! Physical port associated logical port */
    int physical_port;
    /*! MMU port associated to logical port */
    int mmu_port;
    /*! IDB port associated to logical port */
    int idb_port;
    /*! Pipe number of the port */
    int pipe;
    /*! Initial speed after FlexPort operation */
    int speed;
    /*! Port mode: single, dual, quad, tri012, ... */
    int mode;
    /*! Number of PHY lanes */
    int num_lanes;
    /*! Lane information array */
    soc_port_lane_info_t *lane_info[SOC_PORT_RESOURCE_LANES_MAX];
    /*! Encapsulation mode for port */
    bcmtm_port_encap_mode_t encap;
    /*! Indicates if port has oversub enabled */
    int oversub;
    /*! Packet priority to priority group mapping mask */
    uint16_t prio_mask;
    /*! Indicates if port is high speed port */
    uint8_t hsp;
    /*! Indicates if port has CL91 enabled */
    uint32_t cl91_enabled;
} soc_port_resource_t;


/*! Maximum scheduler slices. */
#define MAX_SCH_SLICES        2
/*! Maximum flex ports. */
#define MAX_NUM_FLEXPORTS     256
/*! Maximum TDM calendar length. */
#define MAX_CAL_LEN           512
/*! Maximum oversub groups. */
#define MAX_NUM_OVS_GRPS      32
/*! Maximum oversub group length. */
#define MAX_OVS_GRP_LEN       64
/*! Maximum oversub special groups. */
#define MAX_NUM_OVS_SPC_GRP   32
/*! Maximum oversub special group length. */
#define MAX_OVS_SPC_LEN       160
/*! Maximum flex phases. */
#define MAX_FLEX_PHASES       25

/*
 * ASF (Cut-Through forwarding)
 */

/*! ASF Modes */
typedef enum {
    /*! store-and-forward */
    _SOC_ASF_MODE_SAF          = 0,
    /*! same speed CT  */
    _SOC_ASF_MODE_SAME_SPEED   = 1,
    /*! slow to fast CT   */
    _SOC_ASF_MODE_SLOW_TO_FAST = 2,
    /*! fast to slow CT   */
    _SOC_ASF_MODE_FAST_TO_SLOW = 3,
    /*! internal cfg updates */
    _SOC_ASF_MODE_CFG_UPDATE   = 4,
    /*! CT not supported */
    _SOC_ASF_MODE_UNSUPPORTED  = 5
} soc_asf_mode_e;

/*! ASF MMU Cell Buffer Allocation Profiles */
typedef enum {
    /*! No cut-through support */
    _SOC_ASF_MEM_PROFILE_NONE      = 0,
    /*! Similar Speed Alloc Profile  */
    _SOC_ASF_MEM_PROFILE_SIMILAR   = 1,
    /*! Extreme Speed Alloc Profile */
    _SOC_ASF_MEM_PROFILE_EXTREME   = 2,
    /*! Enum last value. */
    _SOC_ASF_MEM_PROFILE_END       = 3
} soc_asf_mem_profile_e;

/*! ASF property. */
typedef struct soc_asf_prop_s {
    /*! Indicate latency modes. */
    int                    switch_bypass_mode;
    /*! Per-port CT mode. */
    soc_asf_mode_e         asf_modes[MAX_NUM_PORTS];
    /*! MMU cell buffer allocation profile. */
    soc_asf_mem_profile_e  asf_mem_prof;
} soc_asf_prop_t;

/*! port map. */
typedef struct soc_port_map_type_s {
    /*! Logical port speed. */
    tdm_port_speed_t log_port_speed[MAX_NUM_PORTS];
    /*! Physical-to-logical port map. From soc_info_t. */
    int     port_p2l_mapping[MAX_NUM_PORTS];
    /*! Logical-to-physical port map. From soc_info_t. */
    int     port_l2p_mapping[MAX_NUM_PORTS];
    /*! Physical-to-MMU port map. From soc_info_t. */
    int     port_p2m_mapping[MAX_NUM_PORTS];
    /*! MMU-to-physical port map. From soc_info_t. */
    int     port_m2p_mapping[MAX_NUM_PORTS];
    /*! Logical-to-IDB port map. From soc_info_t. */
    int     port_l2i_mapping[MAX_NUM_PORTS];
    /*! Logical port to number of SERDES lanes information. From soc_info_t. */
    int     port_num_lanes[MAX_NUM_PORTS];
    /*! Physical port to port-block (PBLK, a permutation of PM instances) */
    /* instance mapping for the full-chip. N PMs per pipe. */
    int     port_p2PBLK_inst_mapping[MAX_NUM_PORTS];
    /*! Physical port bitmap. From soc_info_t. */
    pbmp_t  physical_pbm;
    /*! HiGig2 port bitmap. From soc_info_t. */
    pbmp_t  hg2_pbm;
    /*! Management port bitmap. From soc_info_t. */
    pbmp_t  management_pbm;
    /*! Oversubscribed port bitmap. From soc_info_t. */
    pbmp_t  oversub_pbm;

} soc_port_map_type_t;

/*! TDM schedule */
typedef struct soc_tdm_schedule_s {
    /*! Length of linerate TDM calendar. */
    int cal_len;
    /*! One array holding the TDM calendar. */
    int linerate_schedule[MAX_CAL_LEN];
    /*! Number of oversubscription groups. */
    int num_ovs_groups;
    /*! Oversubscription group length. */
    int ovs_group_len;
    /*!
     * Array holding the oversubscription schedule for multiple
     * oversubscription groups.
     */
    int oversub_schedule[MAX_NUM_OVS_GRPS][MAX_OVS_GRP_LEN];
    /*! Number of packet scheduler or spacing groups. */
    int num_pkt_sch_or_ovs_space;
    /*! Packet scheduler or spacing group length. */
    int pkt_sch_or_ovs_space_len;
    /*!
     * An array of arrays holding either the oversubscription spacings for
     * multiple oversubscription groups or the packet scheduling calendar
     * information.
     */
    int pkt_sch_or_ovs_space[MAX_NUM_OVS_SPC_GRP][MAX_OVS_SPC_LEN];
} soc_tdm_schedule_t;

/*! pipe tdm scheduler */
typedef struct soc_tdm_schedule_pipe_s {
    /*!
     * Set to 1 where the chip does not have separate physical tables for
     * half-pipes (or slices).
     */
    int num_slices;
    /*! TDM schedule per-slice. */
    soc_tdm_schedule_t   tdm_schedule_slice[MAX_SCH_SLICES];

} soc_tdm_schedule_pipe_t;

/*! port schedule state. */
typedef struct soc_port_schedule_state_s {
    /*! Number of ports to be flexed. From BCM. RO. */
    int                  nport;
    /*! Port reconfiguration information. From BCM. RO. */
    soc_port_resource_t  resource[MAX_NUM_FLEXPORTS];
    /*! Core clock frequency. From soc_info_t. RO. */
    int                  frequency;
    /*! Maximum core bandwidth. From soc_info_t. RO. */
    int                  bandwidth;
    /*! Maximum IO bandwidth. From soc_info_t. RO. */
    int                  io_bandwidth;
    /*! Configurations - 1: Lossless; 0: Lossy. RO. */
    int                  lossless;
    /*! Call context - 1: FlexPort; 0: Initial TDM. RO. */
    int                  is_flexport;
    /*! TDM calendar type - 0: Universal; 1: Ethernet optimized*/
    int                  calendar_type;
    /*! Cut-Through setting */
    soc_asf_prop_t       cutthru_prop;
    /*! Input port map. From soc_info_t. RO. */
    soc_port_map_type_t  in_port_map;
    /*! Output port map. From soc_info_t. */
    soc_port_map_type_t  out_port_map;
    /*! TDM ingress pipe scheduler. */
    soc_tdm_schedule_pipe_t tdm_ingress_schedule_pipe[MAX_NUM_PIPES];
    /*! TDM egress pipe scheduler. */
    soc_tdm_schedule_pipe_t tdm_egress_schedule_pipe[MAX_NUM_PIPES];
    /*!
     * Scratch pad locals. Caller must not allocate. Callee allocates at entry
     * and deallocates before return.
     */
    void *cookie;
} soc_port_schedule_state_t;

/*! TDM length. */
#define BCMTM_INT_TDM_LENGTH                 256
/*! oversub half pipe count. */
#define BCMTM_INT_OVS_HPIPE_COUNT_PER_PIPE   1
/*! oversub group count per half pipe. */
#define BCMTM_INT_OVS_GROUP_COUNT_PER_HPIPE  6
/*! oversub group tdm length. */
#define BCMTM_INT_OVS_GROUP_TDM_LENGTH       12
/*! packet scheduler length. */
#define BCMTM_INT_PKT_SCH_LENGTH             160


/*  General Physical port */
/*! Number of ports per port block. */
#define BCMTM_INT_PORTS_PER_PBLK             4
/*! Number of port blocks per pipe. */
#define BCMTM_INT_PBLKS_PER_PIPE             8
/*! Number of pipes per device. */
#define BCMTM_INT_PIPES_PER_DEV              4
/*! Number of general physical ports per pipe. */
#define BCMTM_INT_GPHY_PORTS_PER_PIPE        \
    (BCMTM_INT_PORTS_PER_PBLK * BCMTM_INT_PBLKS_PER_PIPE)
/*! Number of physical ports per pipe. */
#define BCMTM_INT_PHY_PORTS_PER_PIPE         (BCMTM_INT_GPHY_PORTS_PER_PIPE + 2)
/*! Number of port blocks per device. */
#define BCMTM_INT_PBLKS_PER_DEV              \
    (BCMTM_INT_PBLKS_PER_PIPE * BCMTM_INT_PIPES_PER_DEV)
/*! Number of physical ports per device. */
#define BCMTM_INT_PHY_PORTS_PER_DEV          \
    (BCMTM_INT_PHY_PORTS_PER_PIPE * BCMTM_INT_PIPES_PER_DEV)


/*  Device port
 *   * 32 General device port + 1 CPU/Mng + 1 Loopback*/
/*! Number of general device ports per pipe. */
#define BCMTM_INT_GDEV_PORTS_PER_PIPE        32
/*! Number of device ports per pipe. */
#define BCMTM_INT_DEV_PORTS_PER_PIPE         (BCMTM_INT_GDEV_PORTS_PER_PIPE + 2)
/*! Number of device ports per device. */
#define BCMTM_INT_DEV_PORTS_PER_DEV          \
    (BCMTM_INT_DEV_PORTS_PER_PIPE * BCMTM_INT_PIPES_PER_DEV)

#endif /* BCMTM_INTERNAL_TDM_H */
