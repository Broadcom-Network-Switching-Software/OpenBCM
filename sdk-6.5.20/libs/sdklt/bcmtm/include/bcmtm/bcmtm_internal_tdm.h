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


#define SOC_MAX_NUM_PIPES           4
#define SOC_MAX_NUM_PORTS           256
#define SOC_MAX_NUM_MMU_PORTS       136
#define SOC_PORT_RESOURCE_LANES_MAX 4

typedef bcmdrd_pbmp_t pbmp_t;
/*
 * Lane Information
 *
 * Contains information on a given lane for a port.
 *
 * A port that uses 'x' number of lanes will have 'x' soc_port_lane_info_t
 * elements, one for each of the lanes that the port will use.
 */
typedef struct soc_port_lane_info_s {
    int pgw;           /* PGW instance where lane resides */
    int xlp;           /* XLP number within PGW */
    int tsc;           /*  TSC number (Same as XLP) */
    int port_index;    /* Index within XLP */
} soc_port_lane_info_t;


/*
 * Port Resource
 *
 * Contains port resource configuration used for FlexPort operations.
 */
typedef struct soc_port_resource_s {
    uint32_t flags;
    uint32_t op;              /* BCMI_XGS5_PORT_RESOURCE_OP_XXX */
    int logical_port;       /* Logical port associated to physical port */
    int physical_port;      /* Physical port associated logical port */
    int mmu_port;           /* MMU port associated to logical port */
    int idb_port;           /* IDB port associated to logical port */
    int pipe;               /* Pipe number of the port */
    int speed;              /* Initial speed after FlexPort operation */
    int mode;               /* Port mode: single, dual, quad, tri012, ... */
    int num_lanes;          /* Number of PHY lanes */
    soc_port_lane_info_t *lane_info[SOC_PORT_RESOURCE_LANES_MAX];
                            /* Lane information array */
    bcmtm_port_encap_mode_t encap; /* Encapsulation mode for port */
    int oversub;            /* Indicates if port has oversub enabled */
    uint16_t prio_mask;       /* Packet priority to priority group mapping mask */
    uint8_t hsp;              /* Indicates if port is high speed port */
    uint32_t cl91_enabled;    /* Indicates if port has CL91 enabled */
} soc_port_resource_t;



#define MAX_SCH_SLICES        2
#define MAX_NUM_FLEXPORTS     256
#define MAX_CAL_LEN           512
#define MAX_NUM_OVS_GRPS      32
#define MAX_OVS_GRP_LEN       64
#define MAX_NUM_OVS_SPC_GRP   32
#define MAX_OVS_SPC_LEN       160

#define MAX_FLEX_PHASES       25

/*
 * ASF (Cut-Through forwarding)
 */

/* ASF Modes */
typedef enum {
    _SOC_ASF_MODE_SAF          = 0,  /* store-and-forward */
    _SOC_ASF_MODE_SAME_SPEED   = 1,  /* same speed CT  */
    _SOC_ASF_MODE_SLOW_TO_FAST = 2,  /* slow to fast CT   */
    _SOC_ASF_MODE_FAST_TO_SLOW = 3,  /* fast to slow CT   */
    _SOC_ASF_MODE_CFG_UPDATE   = 4,  /* internal cfg updates */
    _SOC_ASF_MODE_UNSUPPORTED  = 5
} soc_asf_mode_e;

/* ASF MMU Cell Buffer Allocation Profiles */
typedef enum {
    _SOC_ASF_MEM_PROFILE_NONE      = 0,  /* No cut-through support */
    _SOC_ASF_MEM_PROFILE_SIMILAR   = 1,  /* Similar Speed Alloc Profile  */
    _SOC_ASF_MEM_PROFILE_EXTREME   = 2,  /* Extreme Speed Alloc Profile */
    _SOC_ASF_MEM_PROFILE_END       = 3
} soc_asf_mem_profile_e;

typedef struct soc_asf_prop_s {
    int                    switch_bypass_mode; /* Indicate latency modes. */
    soc_asf_mode_e         asf_modes[MAX_NUM_PORTS]; /* Per-port CT mode. */
    soc_asf_mem_profile_e  asf_mem_prof; /* MMU cell buffer allocation profile. */
} soc_asf_prop_t;


typedef struct soc_port_map_type_s {
    tdm_port_speed_t log_port_speed[MAX_NUM_PORTS]; /* Logical port speed. */

    /* Physical-to-logical port map. From soc_info_t. */
    int     port_p2l_mapping[MAX_NUM_PORTS];
    /* Logical-to-physical port map. From soc_info_t. */
    int     port_l2p_mapping[MAX_NUM_PORTS];
    /* Physical-to-MMU port map. From soc_info_t. */
    int     port_p2m_mapping[MAX_NUM_PORTS];
    /* MMU-to-physical port map. From soc_info_t. */
    int     port_m2p_mapping[MAX_NUM_PORTS];
    /* Logical-to-IDB port map. From soc_info_t. */
    int     port_l2i_mapping[MAX_NUM_PORTS];
    /* Logical port to number of SERDES lanes information. From soc_info_t. */
    int     port_num_lanes[MAX_NUM_PORTS];
    /* Physical port to port-block (PBLK, a permutation of PM instances) */
    /* instance mapping for the full-chip. N PMs per pipe. */
    int     port_p2PBLK_inst_mapping[MAX_NUM_PORTS];

    pbmp_t  physical_pbm;      /* Physical port bitmap. From soc_info_t. */
    pbmp_t  hg2_pbm;           /* HiGig2 port bitmap. From soc_info_t. */
    pbmp_t  management_pbm;    /* Management port bitmap. From soc_info_t. */
    pbmp_t  oversub_pbm;       /* Oversubscribed port bitmap. From soc_info_t. */

} soc_port_map_type_t;


typedef struct soc_tdm_schedule_s {
    int cal_len;                        /* Length of linerate TDM calendar. */
    int linerate_schedule[MAX_CAL_LEN]; /* One array holding the TDM calendar. */

    int num_ovs_groups;                 /* Number of oversubscription groups. */
    int ovs_group_len;                  /* Oversubscription group length. */
                                        /* An array of arrays holding the oversubscription        */
                                        /* schedule for multiple oversubscription groups.         */
    int oversub_schedule[MAX_NUM_OVS_GRPS][MAX_OVS_GRP_LEN];

    int num_pkt_sch_or_ovs_space;       /* Number of packet scheduler or spacing groups.          */
    int pkt_sch_or_ovs_space_len;       /* Packet scheduler or spacing group length.              */
                                        /* An array of arrays holding either the oversubscription */
                                        /* spacings for multiple oversubscription groups or       */
                                        /* the packet scheduling calendar information.            */
    int pkt_sch_or_ovs_space[MAX_NUM_OVS_SPC_GRP][MAX_OVS_SPC_LEN];
} soc_tdm_schedule_t;


typedef struct soc_tdm_schedule_pipe_s {
    int num_slices;     /* Set to 1 where the chip does not have separate */
                        /* physical tables for half-pipes (or slices). */
    soc_tdm_schedule_t   tdm_schedule_slice[MAX_SCH_SLICES]; /* TDM schedule per-slice. */

} soc_tdm_schedule_pipe_t;


typedef struct soc_port_schedule_state_s {
    int                  nport;         /* Number of ports to be flexed. From BCM. RO. */
    soc_port_resource_t  resource[MAX_NUM_FLEXPORTS]; /* Port reconfiguration information. From BCM. RO. */
    int                  frequency;     /* Core clock frequency. From soc_info_t. RO. */
    int                  bandwidth;     /* Maximum core bandwidth. From soc_info_t. RO. */
    int                  io_bandwidth;  /* Maximum IO bandwidth. From soc_info_t. RO. */
    int                  lossless;      /* Configurations - 1: Lossless; 0: Lossy. RO. */
    int                  is_flexport;   /* Call context - 1: FlexPort; 0: Initial TDM. RO. */
    int                  calendar_type; /* TDM calendar type - 0: Universal; 1: Ethernet optimized*/

    soc_asf_prop_t       cutthru_prop;  /* Cut-Through setting */
    soc_port_map_type_t  in_port_map;   /* Input port map. From soc_info_t. RO. */
    soc_port_map_type_t  out_port_map;  /* Output port map. From soc_info_t. */

    soc_tdm_schedule_pipe_t tdm_ingress_schedule_pipe[MAX_NUM_PIPES];
    soc_tdm_schedule_pipe_t tdm_egress_schedule_pipe[MAX_NUM_PIPES];
                                  /* Scratch pad locals. Caller must not allocate. */
                                  /* Callee allocates at entry and deallocates     */
                                  /* before return.                                */
    void *cookie;
} soc_port_schedule_state_t;


#define BCMTM_INT_TDM_LENGTH                 256
#define BCMTM_INT_OVS_HPIPE_COUNT_PER_PIPE   1
#define BCMTM_INT_OVS_GROUP_COUNT_PER_HPIPE  6
#define BCMTM_INT_OVS_GROUP_TDM_LENGTH       12
#define BCMTM_INT_PKT_SCH_LENGTH             160


/*  General Physical port */
#define BCMTM_INT_PORTS_PER_PBLK             4
#define BCMTM_INT_PBLKS_PER_PIPE             8
#define BCMTM_INT_PIPES_PER_DEV              4
#define BCMTM_INT_GPHY_PORTS_PER_PIPE        \
    (BCMTM_INT_PORTS_PER_PBLK * BCMTM_INT_PBLKS_PER_PIPE)
#define BCMTM_INT_PHY_PORTS_PER_PIPE         (BCMTM_INT_GPHY_PORTS_PER_PIPE + 2)
#define BCMTM_INT_PBLKS_PER_DEV              \
    (BCMTM_INT_PBLKS_PER_PIPE * BCMTM_INT_PIPES_PER_DEV)
#define BCMTM_INT_PHY_PORTS_PER_DEV          \
    (BCMTM_INT_PHY_PORTS_PER_PIPE * BCMTM_INT_PIPES_PER_DEV)


/*  Device port
 *   * 32 General device port + 1 CPU/Mng + 1 Loopback*/
#define BCMTM_INT_GDEV_PORTS_PER_PIPE        32
#define BCMTM_INT_DEV_PORTS_PER_PIPE         (BCMTM_INT_GDEV_PORTS_PER_PIPE + 2)
#define BCMTM_INT_DEV_PORTS_PER_DEV          \
    (BCMTM_INT_DEV_PORTS_PER_PIPE * BCMTM_INT_PIPES_PER_DEV)

#endif /* BCMTM_INTERNAL_TDM_H */
