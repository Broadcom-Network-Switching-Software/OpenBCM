/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.h
 * Purpose:     SOC Port definitions.
 */

#ifndef   _SOC_ESW_PORT_H_
#define   _SOC_ESW_PORT_H_
#include <soc/drv.h>
#include <soc/tdm/core/tdm_top.h>

/*
 * Internal flags
 *
 *   SOC_PORT_RESOURCE_I_MAP      Indicates legacy FlexPort API is used.
 *                                In this case, when flexing to a fewer
 *                                number of ports (4x10 -> 1x40), the 'old'
 *                                ports become inactive rather than
 *                                being detached and destroyed (which is
 *                                what happens with the new multi_set API).
 *                                To be used only by legacy API.
 *
 *   SOC_PORT_RESOURCE_INACTIVE   Indicates if port is inactive or active.
 *                                To be used only by legacy API.
 *
 *   SOC_PORT_RESOURCE_ATTACH     The port needs to go through the attach
 *                                sequence.
 *
 *   SOC_PORT_RESOURCE_DETACH     The port needs to go through the detach
 *                                sequence.
 *
 *   SOC_PORT_RESOURCE_NEW        A new logical port, which previously
 *                                didn't exist, is added as a result of
 *                                a FlexPort operation.
 *
 *   SOC_PORT_RESOURCE_DESTROY    The logical port is deleted and
 *                                no longer exists in the system after
 *                                FlexPort.
 *
 *   SOC_PORT_RESOURCE_REMAP      The logical port existed before FlexPort
 *                                but is mapped to a different physical port
 *                                after FlexPort.
 *   SOC_PORT_RESOURCE_SPEED      Only speed is changed. Logical-physical mapping,
 *                                lanes, encap remain the same
 */
#define SOC_PORT_RESOURCE_I_MAP      (1 << 31)
#define SOC_PORT_RESOURCE_INACTIVE   (1 << 30)
#define SOC_PORT_RESOURCE_ATTACH     (1 << 29)
#define SOC_PORT_RESOURCE_DETACH     (1 << 28)
#define SOC_PORT_RESOURCE_NEW        (1 << 27)
#define SOC_PORT_RESOURCE_DESTROY    (1 << 26)
#define SOC_PORT_RESOURCE_REMAP      (1 << 25)
#define SOC_PORT_RESOURCE_SPEED      (1 << 24)


/* Maximum number of lanes that a port can have */
#define  SOC_PORT_RESOURCE_LANES_MAX    12

/* Maximum number of auxiliary ports */
#define SOC_PORT_AUXILIARY_MAX          8

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


/* FEC types supported. This MUST match the _SHR_PORT_PHY_FEC_* values */
typedef enum _soc_port_phy_fec_e {
    socPortPhyFecInvalid = _SHR_PORT_PHY_FEC_INVALID,
    socPortPhyFecNone = _SHR_PORT_PHY_FEC_NONE,
    socPortPhyFecBaseR = _SHR_PORT_PHY_FEC_BASE_R,
    socPortPhyFecRsFec = _SHR_PORT_PHY_FEC_RS_FEC,
    socPortPhyFecRs544 = _SHR_PORT_PHY_FEC_RS_544,
    socPortPhyFecRs272 = _SHR_PORT_PHY_FEC_RS_272,
    socPortPhyFecRs206 = _SHR_PORT_PHY_FEC_RS_206,
    socPortPhyFecRs108 = _SHR_PORT_PHY_FEC_RS_108,
    socPortPhyFecRs545 = _SHR_PORT_PHY_FEC_RS_545,
    socPortPhyFecRs304 = _SHR_PORT_PHY_FEC_RS_304,
    socPortPhyFecRs544_2xN = _SHR_PORT_PHY_FEC_RS_544_2XN,
    socPortPhyFecRs272_2xN = _SHR_PORT_PHY_FEC_RS_272_2XN,
    socPortPhyFecCount = _SHR_PORT_PHY_FEC_COUNT
} soc_port_phy_fec_t;


/*
 * Port Resource
 *
 * Contains port resource configuration used for FlexPort operations.
 */
typedef struct soc_port_resource_s {
    uint32 flags;
    uint32 op;              /* BCMI_XGS5_PORT_RESOURCE_OP_XXX */
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
    soc_encap_mode_t encap; /* Encapsulation mode for port */
    int oversub;            /* Indicates if port has oversub enabled */
    uint16 prio_mask;       /* Packet priority to priority group mapping mask */
    uint8 hsp;              /* Indicates if port is high speed port */
    uint32 cl91_enabled;    /* Indicates if port has CL91 enabled */
    soc_port_phy_fec_t fec_type;    /* fec_type for port */
    int phy_lane_config;            /* serdes pmd lane config for port */
    int link_training;              /* link training on or off */
    int roe_compression;              /* roe_compression */
    int num_subports;       /* Number of subport for this cascaded ports.*/
    int coe_port;           /* this is cascaded port. */
    int port_lc_map;        /* line card number for this port. */
} soc_port_resource_t;


/*** START SDK API COMMON CODE ***/

#define MAX_SCH_SLICES        2
#define MAX_NUM_FLEXPORTS     288
#define MAX_CAL_LEN           512
#define MAX_NUM_OVS_GRPS      32
#define MAX_OVS_GRP_LEN       64
#define MAX_NUM_OVS_SPC_GRP   32
#define MAX_OVS_SPC_LEN       200
#define CLMAC_AVERAGE_IPG_DEFAULT 12
#define CLMAC_AVERAGE_IPG_HIGIG   8

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
    soc_asf_mode_e         asf_modes[SOC_MAX_NUM_PORTS]; /* Per-port CT mode. */
    soc_asf_mem_profile_e  asf_mem_prof; /* MMU cell buffer allocation profile. */
} soc_asf_prop_t;

typedef struct soc_port_map_type_s {
    enum port_speed_e log_port_speed[SOC_MAX_NUM_PORTS]; /* Logical port speed. */

    /* Physical-to-logical port map. From soc_info_t. */
    int     port_p2l_mapping[SOC_MAX_NUM_PORTS];
    /* Logical-to-physical port map. From soc_info_t. */
    int     port_l2p_mapping[SOC_MAX_NUM_PORTS];
    /* Physical-to-MMU port map. From soc_info_t. */
    int     port_p2m_mapping[SOC_MAX_NUM_PORTS];
    /* MMU-to-physical port map. From soc_info_t. */
    int     port_m2p_mapping[SOC_MAX_NUM_MMU_PORTS];
    /* Logical-to-IDB port map. From soc_info_t. */
    int     port_l2i_mapping[SOC_MAX_NUM_PORTS];
    /* Logical port to number of SERDES lanes information. From soc_info_t. */
    int     port_num_lanes[SOC_MAX_NUM_PORTS];
    /* Physical port to port-block (PBLK, a permutation of PM instances) */
    /* instance mapping for the full-chip. N PMs per pipe. (N=16 in TH2.) */
    int     port_p2PBLK_inst_mapping[SOC_MAX_NUM_PORTS];

    pbmp_t  physical_pbm;      /* Physical port bitmap. From soc_info_t. */
    pbmp_t  hg2_pbm;           /* HiGig2 port bitmap. From soc_info_t. */
    pbmp_t  management_pbm;    /* Management port bitmap. From soc_info_t. */
    pbmp_t  oversub_pbm;       /* Oversubscribed port bitmap. From soc_info_t. */

 /*Added Fields for Firebolt6 COE Port Mapping*/
  int         num_subports[SOC_MAX_NUM_PORTS];
                 /*!< num_subport_per_phy_port(COE ports). From soc_info_t. */
  int         port_lc_mapping[SOC_MAX_NUM_PORTS];
                 /*!< line card mapping for each coe enabled physical port. Else the value will be  -1. */
  pbmp_t      coe_port_pbm;       /*!< COE  port bitmap. From soc_info_t. */

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

    soc_tdm_schedule_pipe_t tdm_ingress_schedule_pipe[SOC_MAX_NUM_PIPES];
    soc_tdm_schedule_pipe_t tdm_egress_schedule_pipe[SOC_MAX_NUM_PIPES];
                                  /* Scratch pad locals. Caller must not allocate. */
                                  /* Callee allocates at entry and deallocates     */
                                  /* before return.                                */
    void *cookie;
} soc_port_schedule_state_t;

/*** END SDK API COMMON CODE ***/

extern int soc_port_speed_higig2eth(int speed);
extern int soc_port_speed_to_mask(int unit, int speed, uint32 *speed_mask,
                                  int *is_hg_speed);

/*** END SDK API COMMON CODE ***/
extern int soc_port_sister_validate(int unit, uint32 pm_port_num);
extern int soc_port_speed_encap_validate(int unit);

typedef enum soc_port_phy_tx_tap_mode_e {
    socPortPhyTxTapMode3Tap = _SHR_PORT_PHY_TX_TAP_MODE_3_TAP,
    socPortPhyTxTapMode6Tap = _SHR_PORT_PHY_TX_TAP_MODE_6_TAP
} soc_port_phy_tx_tap_mode_t;

typedef enum soc_port_phy_signalling_mode_e {
    socPortPhySignallingModeNRZ = _SHR_PORT_PHY_SIGNALLING_MODE_NRZ,
    socPortPhySignallingModePAM4 = _SHR_PORT_PHY_SIGNALLING_MODE_PAM4
} soc_port_phy_signalling_mode_t;

typedef _shr_port_phy_tx_t soc_port_phy_tx_t;
typedef _shr_port_rlm_config_t soc_port_rlm_config_t;
#endif /* _SOC_ESW_PORT_H_ */
