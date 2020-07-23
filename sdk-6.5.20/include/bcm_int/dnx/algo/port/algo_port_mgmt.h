/** \file algo_port_mgmt.h
 *  
 *  PORT manager, Port utilities, and Port verifiers.
 *  
 *  Algo port managment:
 *  * MGMT APIs
 *    - Used to configure algo port module
 *    - located in algo_port_mgmt.h (source code algo_port_mgmt.c)
 *  * Utility APIs
 *    - Provide utilities functions which will make the BCM module cleaner and simpler
 *    - located in algo_port_utils.h (source code algo_port_utils.c
 *  * Verify APIs
 *    - Provide a set of APIs which verify port attributes
 *    - These verifiers used to verify algo port functions input.
 *    - These verifiers can be used out side of the module, if requried.
 *    - located in algo_port_verify.h (source code algo_port_verify.c)
 *  * SOC APIs
 *    - Provide set of functions to set / restore  and remove soc info data
 *    - Only the port related members will be configured by this module (other modules should not set those memebers directly)
 *    - The relevant members are: 'port to block access', 'port names' and 'port bitmaps'
 *    - located in algo_port_soc.c (all the functions are module internals)
 *  * Module internal definitions - algo_port_internal.h
 *  * Data bases - use sw state mechanism - XML file is algo_port.xml
 *  
 *  No need to include each sub module seperately.
 *  Including: 'algo_port_mgmt.h' will includes all the sub modules
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef ALGO_PORT_MGMT_H_INCLUDED
/*
 * {
 */
#define ALGO_PORT_MGMT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Includes.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>

#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>

#include <bcm/port.h>

/*
 * }
 */
/*
 * Macros.
 * {
 */
/**
 * \brief get next master logical port. 
 * (the port that will be defined as the master if the current master will be removed) 
 */
#define DNX_ALGO_PORT_MASTER_F_NEXT     (0x1)
/**
 * \brief get TDM-bypass master port (TDM-bypass representative)
 */
#define DNX_ALGO_PORT_MASTER_F_TDM_BYPASS     (0x2)
/**
 * \brief get non TDM-bypass master port (non TDM-bypass representative)
 */
#define DNX_ALGO_PORT_MASTER_F_NON_TDM_BYPASS     (0x4)

/**
 * \brief get SCH master port
 */
#define DNX_ALGO_PORT_MASTER_F_SCH     (0x8)

/**
 * \brief mask of all the supported flags by API dnx_algo_port_master_get
 */
#define DNX_ALGO_PORT_MASTER_F_MASK     (DNX_ALGO_PORT_MASTER_F_NEXT | DNX_ALGO_PORT_MASTER_F_TDM_BYPASS | DNX_ALGO_PORT_MASTER_F_NON_TDM_BYPASS | DNX_ALGO_PORT_MASTER_F_SCH)

/**
 * \brief Mark allocated pp port as LAG 
 */
#define DNX_ALGO_PORT_PP_PORT_ALLOCATE_F_LAG     (0x1)
/**
 * \brief Allocate Specific PP port 
 *        Should be used for debug only 
 */
#define DNX_ALGO_PORT_PP_PORT_ALLOCATE_F_WITH_ID     (0x2)

/**
 * \brief invalid port value
 */
#define DNX_ALGO_PORT_INVALID           (-1)

/**
 * \brief invalid Egress Interface
 */
#define DNX_ALGO_PORT_EGR_IF_INVALID         (-1)

/**
 * \brief flag for port application
 */
#define DNX_ALGO_PORT_APP_FLAG_PON           0x1
#define DNX_ALGO_PORT_APP_FLAG_COE           0x2

/**
 * \brief flag for port speed
 */
#define DNX_ALGO_PORT_SPEED_F_MBPS           0x1
#define DNX_ALGO_PORT_SPEED_F_KBPS           0x2
#define DNX_ALGO_PORT_SPEED_F_RX             0x4
#define DNX_ALGO_PORT_SPEED_F_TX             0x8

#define DNX_ALGO_PORT_SPEED_RX_GET(flags) ((flags) & DNX_ALGO_PORT_SPEED_F_RX ? 1 : 0)
#define DNX_ALGO_PORT_SPEED_TX_GET(flags) ((flags) & DNX_ALGO_PORT_SPEED_F_TX ? 1 : 0)

#define DNX_ALGO_PORT_SPEED_RX_SET(flags) ((flags) |= DNX_ALGO_PORT_SPEED_F_RX)
#define DNX_ALGO_PORT_SPEED_TX_SET(flags) ((flags) |= DNX_ALGO_PORT_SPEED_F_TX)

#define DNX_ALGO_PORT_FLEXE_TX_GET(flags) ((flags) & BCM_PORT_FLEXE_TX ? 1 : 0)
#define DNX_ALGO_PORT_FLEXE_RX_GET(flags) ((flags) & BCM_PORT_FLEXE_RX ? 1 : 0)

#define DNX_ALGO_PORT_FLEXE_TX_SET(flags) ((flags) |= BCM_PORT_FLEXE_TX)
#define DNX_ALGO_PORT_FLEXE_RX_SET(flags) ((flags) |= BCM_PORT_FLEXE_RX)

/**
 * \brief Convert NIF phy id to core 
 *        Include dnx_data_nif from source file in order to use this macro. 
 */
#define DNX_ALGO_PORT_NIF_PHY_CORE_GET(unit, nif_phy)\
            (nif_phy / dnx_data_nif.phys.nof_phys_per_core_get(unit))
/**
 * \brief Convert NIF phy id to local phy (number from 0 to nof_phys_per_core  - 1) 
 *        Include dnx_data_nif from source file in order to use this macro. 
 */
#define DNX_ALGO_PORT_NIF_PHY_LOCAL_PHY_GET(unit, nif_phy)\
            (nif_phy % dnx_data_nif.phys.nof_phys_per_core_get(unit))

/**
 * \brief Convert NIF local phy id and core to NIF global phy id.
 *        Include dnx_data_nif from source file in order to use this macro. 
 */
#define DNX_ALGO_PORT_LOCAL_PHY_TO_PHY_GET(unit, core, nif_local_phy)\
            (core * dnx_data_nif.phys.nof_phys_per_core_get(unit) + nif_local_phy)

/**
 * \brief Creating TM interface id (key to TM interface DB) for NIF ports.
 */
#define DNX_ALGO_PORT_TM_INTERFACE_HANDLE_FROM_FIRST_PHY(unit, first_phy_port)  (first_phy_port)

#define DNX_ALGO_PORT_FIRST_HEADER_SIZE_PROFILE "Profile for skipping first header size before ETH, PTCH1 and PTCH2 header and after PTCH1."
/**
 * \brief - maximal amount of counter sets per port
 */
#define DNX_ALGO_PORT_COUNTER_SETS_MAX (4)

/** Maximum number of first header size profiles */
#define DNX_ALGO_PORT_MAX_FIRST_HEADER_SIZE_PROFILES 6
/** Default profile for template manager for skipping first header size */
#define DNX_ALGO_PORT_FIRST_HEADER_SIZE_DEFAULT_PROFILE (DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD1 + DNX_ALGO_PORT_MAX_FIRST_HEADER_SIZE_PROFILES)
/*
 * }
 */
/*
 * TypeDefs.
 * {
 */
/**
 * \brief 
 * Logical port type.
 */
typedef enum
{
    /**
     * Logical port is not used.
     */
    DNX_ALGO_PORT_TYPE_INVALID = -1,

    /**
     * Network interface port - ethernet interfaces  
     */
    DNX_ALGO_PORT_TYPE_NIF_ETH = 0,
    /**
     * Network interface port - interlaken port  
     */
    DNX_ALGO_PORT_TYPE_NIF_ILKN = 1,
    /**
     * CPU port
     */
    DNX_ALGO_PORT_TYPE_CPU = 2,
    /**
     * Recycle port  
     */
    DNX_ALGO_PORT_TYPE_RCY = 3,
    /**
     * Egress Replication Port  
     */
    DNX_ALGO_PORT_TYPE_ERP = 4,
    /**
     * Off Load Processor port
     */
    DNX_ALGO_PORT_TYPE_OLP = 5,
    /**
     * Fabric link port  
     */
    DNX_ALGO_PORT_TYPE_FABRIC = 6,
    /**
     * OAMP port
     */
    DNX_ALGO_PORT_TYPE_OAMP = 7,
    /**
     * SAT port  
     */
    DNX_ALGO_PORT_TYPE_SAT = 8,
    /**
     * EVENTOR port  
     */
    DNX_ALGO_PORT_TYPE_EVENTOR = 9,
    /**
     * RCY Mirror Port
     */
    DNX_ALGO_PORT_TYPE_RCY_MIRROR = 10,
    /**
     * IPSEC port  
     */
    DNX_ALGO_PORT_TYPE_IPSEC = 11,
    /**
     * FLEXE CLIENT (bus A) port
     */
    DNX_ALGO_PORT_TYPE_FLEXE_CLIENT = 12,
    /**
     * FLEXE MAC (bus B)
     */
    DNX_ALGO_PORT_TYPE_FLEXE_MAC = 13,
    /**
     * FLEXE SAR (bus C) port
     */
    DNX_ALGO_PORT_TYPE_FLEXE_SAR = 14,
    /**
     * FLEXE PHY port
     */
    DNX_ALGO_PORT_TYPE_FLEXE_PHY = 15,
    /**
     * SCH Only
     */
    DNX_ALGO_PORT_TYPE_SCH_ONLY = 16,
    /**
     * CRPS port
     */
    DNX_ALGO_PORT_TYPE_CRPS = 17,
    /**
     * Number of port types - must be last
     */
    DNX_ALGO_PORT_TYPE_NOF
} dnx_algo_port_type_e;

/**
 * \brief
 * Basic info about port.
 */
typedef struct
{
    dnx_algo_port_type_e port_type;

    unsigned int is_tm:1;
    unsigned int is_ing_tm:1;
    unsigned int is_egr_tm:1;
    unsigned int is_sch:1;
    unsigned int is_tdm_bypass:1;
    unsigned int is_tdm_packet:1;

    unsigned int is_imb:1;

    unsigned int is_nif:1;
    unsigned int is_nif_eth:1;
    unsigned int is_nif_ilkn:1;
    unsigned int is_elk:1;
    unsigned int is_stif:1;

    unsigned int is_ing_pp:1;
    unsigned int is_egr_pp:1;

    unsigned int is_l1:1;
    unsigned int is_flexe_phy:1;
    unsigned int is_flexe_client:1;

    unsigned int is_fabric:1;
} dnx_algo_port_info_s;

/**
 * \brief - TDM mode per each TM port.
 */
typedef enum
{
    /**
     * Invalid - i.e. tdm mode is not tdm and not non-tdm port :) 
     * This value will be used when removing a port 
     */
    DNX_ALGO_PORT_TDM_MODE_INVALID = -1,
    /**
     * TDM wasn't set. (i.e. non TDM port)
     */
    DNX_ALGO_PORT_TDM_MODE_NONE = 0,
    /**
     * Standard TDM mode. Bypass ingress TM.
     */
    DNX_ALGO_PORT_TDM_MODE_BYPASS = 1,
    /**
     * TDM. which sent through ingress TM.
     * Considered as a standard 
     */
    DNX_ALGO_PORT_TDM_MODE_PACKET = 2,
    /**
     * Number of TDM modes. Must be last!
     */
    DNX_ALGO_PORT_TDM_MODE_NOF
} dnx_algo_port_tdm_mode_e;

/**
 * \brief - Interface TDM mode
 */
typedef enum
{

    /**
     * No TDM ports on interface
     */
    DNX_ALGO_PORT_IF_NO_TDM = 0,
    /**
     * All ports on interface are TDM
     */
    DNX_ALGO_PORT_IF_TDM_ONLY = 1,
    /**
     * Interface might have both TDM and Non-TDM ports
     */
    DNX_ALGO_PORT_IF_TDM_HYBRID = 2,
    /**
     * Number of TDM modes. Must be last!
     */
    DNX_ALGO_PORT_IF_TDM_MODE
} dnx_algo_port_if_tdm_mode_e;

/**
 * \brief - Header modes
 */
typedef enum
{
    /** Header mode NON_PP */
    DNX_ALGO_PORT_HEADER_MODE_NON_PP = 0,
    /** Header mode Ethernet */
    DNX_ALGO_PORT_HEADER_MODE_ETH = 1,
    /** Header mode MPLS_RAW */
    DNX_ALGO_PORT_HEADER_MODE_MPLS_RAW = 2,
    /** Header mode INJECTED_2_PP */
    DNX_ALGO_PORT_HEADER_MODE_INJECTED_2_PP = 3
} dnx_algo_port_header_mode_e;

/**
 * \brief - MIB counter set
 */
typedef enum
{
    /**
     * CDU port mib counters
     */
    DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CDU,
    /**
     * CLU port mib counters
     */
    DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU,
    /**
     * Interlaken mib port counters
     */
    DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN,
    /**
     * Fabric port mib counters
     */
    DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC,
    /**
     * Interlaken over Fabric port mib counters
     */
    DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN_OVER_FABRIC,
    /**
     * NIF Ethernet (CDU/CLU)
     */
    DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH,
    /**
     * NIF (NIF_ETH and ILKN)
     */
    DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF,

    /**
     * FlexE Mac
     */
    DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC,
    /**
     * CPU
     */
    DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_CPU,
    /**
     * must be last
     */
    DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NOF
} dnx_algo_port_mib_counter_set_type_e;

/**
 * \brief 
 * Info required when adding TM port.
 */
typedef struct
{
    /**
     * TM port
     */
    uint32 tm_port;

    /**
     * Port channel for channelized ports (must)
     */
    int channel;
    /**
     * True iff the port is channelized
     */
    int is_channelized;
    /**
     * Egress base queue pair number
     */
    int base_q_pair;
    /**
     * Number of port priorities 
     */
    int num_priorities;
    /**
     * SCH base hr id
     */
    int base_hr;
    /**
     * Number of sch priorities 
     */
    int sch_priorities;
    /**
     * Enable sch priority propagation on the port
     */
    int sch_priority_propagation_enable;
    /**
     * Egress Queuing interface
     */
    int egress_interface;
    /**
     * Ingress interleaving
     */
    int is_ingress_interleave;
    /**
     * Egress interleaving
     */
    int is_egress_interleave;
    /**
     * port TDM Mode
     */
    dnx_algo_port_tdm_mode_e tdm_mode;
    /**
     * Interface TDM mode
     */
    dnx_algo_port_if_tdm_mode_e if_tdm_mode;
} dnx_algo_port_tm_add_t;

/**
 * \brief 
 * Info required when adding ILKN port. 
 */
typedef struct
{
    /**
     * ILKN lanes bitmap
     */
    bcm_pbmp_t ilkn_lanes;

    /**
     * is ILKN port used for ELK connection.
     */
    int is_elk_if;

    /**
     * is ILKN over fabric
     */
    int is_ilkn_over_fabric;
} dnx_algo_port_nif_ilkn_add_t;

/**
 * \brief 
 * Info required when adding NIF ETH port.
 */
typedef struct
{
    /**
     * is ETH port used for statistics interface
     */
    int is_stif;
    /**
     * is ETH port used for statistics interface and data
     */
    int is_stif_data;
    /**
     * is ETH port used for FlexE physical interface
     */
    int is_flexe_phy;
} dnx_algo_port_nif_eth_add_t;

/**
 * \brief
 * Info required when adding network interface port. 
 */
typedef struct
{
    /**
     * Port Core ID
     */
    bcm_core_t core;
    /**
     * Interface type 
     * NIF Only! interface
     */
    bcm_port_if_t interface;
    /**
     * Interface offset.
     */
    int interface_offset;
    /**
     * phys bitmap required to be allocated
     */
    bcm_pbmp_t phys;
    /**
     * TM port info
     */
    dnx_algo_port_tm_add_t tm_info;
    /**
     * ILKN port info - only relevant for ILKN
     */
    dnx_algo_port_nif_ilkn_add_t ilkn_info;
    /**
     * NIF ETH port info - only relevant for ETH
     */
    dnx_algo_port_nif_eth_add_t eth_info;
    /**
     * is L1 interface
     * NIF Only!
     */
    int is_cross_connect;
} dnx_algo_port_if_add_t;

/**
 * \brief 
 * Info required when adding FlexE client.
 */
typedef struct
{
    /**
     * is the virtual client is FlexE mac
     */
    int is_flexe_mac;
    /**
     * is the virtual client is FlexE SAR
     */
    int is_flexe_sar;
    /**
     * is the virtual client is cross connect
     */
    int is_cross_connet;
    /**
     * FlexE client channel - used just for FlexE client
     */
    int client_channel;
} dnx_algo_port_special_if_flexe_add_t;

/**
 * \brief 
 * Info required when adding special TM interface.
 */
typedef struct
{
    /**
     * Port Core ID
     */
    bcm_core_t core;
    /**
     * Interface type 
     * Any Special TM interface is relevant 
     * * BCM_PORT_IF_CPU 
     * * BCM_PORT_IF_RCY 
     * * BCM_PORT_IF_ERP 
     * * BCM_PORT_IF_OLP 
     * * BCM_PORT_IF_OAMP 
     * * BCM_PORT_IF_SAT 
     * * BCM_PORT_IF_EVENTOR 
     * * BCM_PORT_IF_RCY_MIRROR
     * * BCM_PORT_IF_FLEXE_CLIENT
     * * BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT
     */
    bcm_port_if_t interface;

    /**
     * TM port info
     */
    dnx_algo_port_tm_add_t tm_info;

    /**
     * interface offset - used just for recycle ports
     */
    int interface_offset;
    /**
     * FlexE Info
     */
    dnx_algo_port_special_if_flexe_add_t flexe_info;
} dnx_algo_port_special_interface_add_t;

/**
 * \brief 
 * Indicator for each port type
 */
typedef struct
{
    /*
     * ILKN over fabric port
     */
    uint8 is_ilkn_over_fabric;
    /*
     * ELK port
     */
    uint8 is_elk;
    /*
     * stat interface
     */
    uint8 is_stif;
    /*
     * stat and data mix interface
     */
    uint8 is_stif_data;
    /*
     * Flexe PHY port
     */
    uint8 is_flexe_phy;
    /*
     * Cross connect port
     */
    uint8 is_cross_connect;
    /*
     * Flexe MAC port
     */
    uint8 is_flexe_mac;
    /*
     * Flexe SAR port
     */
    uint8 is_flexe_sar;
} dnx_algo_port_info_indicator_t;

/**
 * \brief 
 * Info required when adding fabric link.
 */
typedef struct
{
    /**
     * Fabric link id
     */
    int link_id;
} dnx_algo_port_fabric_add_t;

/**
 * \brief -  Access Logical fifo info (used for dbal tables 
 *        access)
 */
typedef struct
{
    /**
     * RMC id
     */
    int rmc_id;
    /**
     * Scheduler priority
     */
    bcm_port_nif_scheduler_t sch_priority;
    /**
     * PRD priority. see BCM_PORT_F_PRIORITY_*
     */
    uint32 prd_priority;
    /**
     * first memory entry
     */
    uint32 first_entry;
    /**
     * last memory entry
     */
    uint32 last_entry;
    /**
     * Threshold after overflow
     */
    uint32 thr_after_ovf;

} dnx_algo_port_rmc_info_t;

/*
 * }
 */

/*
 * Module Init / Deinit 
 * { 
 */
/**
 * \brief - 
 * Init algo port mgmt module: 
 * * Allocate resources (if required) 
 * * In cold boot init algo port data bases (soc control + sw state) 
 * * In warm boot restore soc control data base (which is not stored in sw state)
 *           
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * internal algo port data base
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_init(
    int unit);

/**
 * \brief - 
 *  is_init is true iff the module is initilized 
 *           
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] is_init - 0 iff the is not initilized
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_is_init(
    int unit,
    int *is_init);

/*
 * }
 */

/*
 * Adding / Removing Port.
 * {
 */

/**
 * \brief - 
 * Add new NIF port
 * Relevant for both ethrnet port and interlaken ports.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - required logical port.
 *   \param [in] info - info required for adding nif port (see struct definition)
 *   
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * internal algo port data base
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_add(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_if_add_t * info);

/**
 * \brief -
 * Allocate TM interface handle
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port.
 * \param [in] interface_type - see bcm_port_if_t
 * \param [in] phys - port phy bitmap
 * \param [in] interface_offset - inteface_offset as defined in bcm_dnx_port_add()
 * \param [out] tm_interface - TM interface handle
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_tm_interface_handle_allocate(
    int unit,
    bcm_port_t logical_port,
    int interface_type,
    bcm_pbmp_t phys,
    int interface_offset,
    int *tm_interface);

/**
 * \brief -
 * Get if a nif phy is associated with an active port.
 * 
 * \param [in] unit - unit #.
 * \param [in] logical_phy - index of the phy
 * \param [out] is_active - 1 if phy is associated with an active port, 0 if not 
 *  
 * \return 
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_phy_active_get(
    int unit,
    int logical_phy,
    int *is_active);

/**
 * \brief - 
 * Add new special interface port (CPU, RCY, OLP, ERP, OAMP, SAT, EVENTOR)  
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - required logical port.
 *   \param [in] info - info required for adding special interface port (see struct definition)
 *   
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * internal algo port data base
 * \remark
 *   * OAMP, SAT, EVENTOR are not yet implemented.
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_special_interface_add(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_special_interface_add_t * info);

/**
 * \brief - 
 * Add scheduler to existing port 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - required logical port.
 *   \param [in] tm_info - info required for adding scheduler to port (see struct definition)
 */
shr_error_e dnx_algo_port_sch_only_add(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_tm_add_t * tm_info);

/**
 * \brief - 
 * Getting the logical port from interface and channel ID.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] core_id - core ID # (BCM_CORE_ALL is not supported)
 *   \param [in] port_info - basic info about port.
 *   \param [in] channel_id - channel ID
 *   \param [out] logical_port - logical port found.
 *
 * \remark
 *  DNX_ALGO_PORT_INVALID is returned if no matching port was found for specified port type and channel
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_interface_and_channel_to_logical_get(
    int unit,
    int core_id,
    dnx_algo_port_info_s port_info,
    int channel_id,
    bcm_port_t * logical_port);

/**
 * \brief - get Database interface id for special interfaces
 * (used for both general interface database and tm interface database)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] port_info - basic info about port
 *   \param [in] core - core ID
 *   \param [in] phys - phys bitmap, ignored for non-nif ports
 *   \param [in] interface_offset - inteface_offset as defined in bcm_dnx_port_add()
 *   \param [in] flexe_client_channel - flexe client index. Ignored for other port types
 *   \param [out] if_id_h0 - DB interface ID - handle 0
 *   \param [out] if_id_h1 - DB interface ID - handle 0
 *
 * \remark
 *  DNX_ALGO_PORT_INVALID is returned if no matching port was found for specified port type and channel
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_db_interface_id_get(
    int unit,
    dnx_algo_port_info_s port_info,
    bcm_core_t core,
    int interface_offset,
    bcm_pbmp_t * phys,
    int flexe_client_channel,
    int *if_id_h0,
    int *if_id_h1);

/**
 * \brief -
 * Adding fabric port
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - required logical port.
 *   \param [in] info - info required for adding fabric port (see struct definition)
 *   
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * internal algo port data base
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_fabric_add(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_fabric_add_t * info);

/**
 * \brief -
 * Get if a fabric phy is active.
 * 
 * \param [in] unit - unit #.
 * \param [in] fabric_phy -  index of the phy
 * \param [out] is_active - 1 if phy is active, 0 if not 
 *  
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_fabric_phy_active_get(
    int unit,
    int fabric_phy,
    int *is_active);

/**
 * \brief - 
 * Remove logical port from DB. 
 * * If logical port is valid & master port - also per port type handles will be free. 
 * * Otherwise those resources shouled be removed explictly. 
 * * Init logical DB
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - required logical port.
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * internal algo port data base
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_remove(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief - 
 *  Remove scheduler part of a port and update DB
 *  
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_remove_sch_only(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief - 
 *  Mark the port as 'going to be removed'. 
 *  During the process of remove port. 
 *  Some APIs could get the port that going to be removed using 'dnx_algo_port_port_removed_port_get()'. 
 *  
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_remove_process_start(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief - 
 *  Unmark the port as 'new added port'. 
 *  During the process of add port, 
 *  Some APIs could get the port that in the proccess of add using 'dnx_algo_port_port_added_port_get()'. 
 *  
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_add_process_done(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief - 
 * Get the port that going to be removed. 
 *  
 * \param [in] unit - unit #.
 * \param [out] logical_port - removed logical port #.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_removed_port_get(
    int unit,
    bcm_port_t * logical_port);

/**
 * \brief - 
 * Get the port that in the proccess of add. 
 *  
 * \param [in] unit - unit #.
 * \param [out] logical_port - added logical port #.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_added_port_get(
    int unit,
    bcm_port_t * logical_port);

/**
 * \brief - 
 * Allocate PP port
 * Might be used to allocate PP port for LAG group.
 * Mapping logical_ports to pp port is done with dnx_algo_port_pp_port_set()
 *  
 * \param [in] unit - unit #
 * \param [in] flags - See DNX_ALGO_PORT_PP_PORT_ALLOCATE_F_*
 * \param [in] core - core ID # (BCM_CORE_ALL is not supported)
 * \param [in] lag_id - if the PP port is allocated for LAG holds the LAG ID
 * \param [out] pp_port - allocated pp port #
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_pp_port_allocate(
    int unit,
    uint32 flags,
    bcm_core_t core,
    bcm_trunk_t lag_id,
    uint32 *pp_port);

/**
 * \brief - 
 * Free pp port.
 * Might be used when freeing LAG pp port.
 * The API will throw an error in a case logical port linked to this pp port.
 *  
 * \param [in] unit - unit #
 * \param [in] core - core ID # (BCM_CORE_ALL is not supported)
 * \param [in] pp_port - allocated pp port #
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_pp_port_free(
    int unit,
    bcm_core_t core,
    uint32 pp_port);

/*
 * }
 */

/*
 * Mapping from / to logical port 
 * { 
 */
/**
 * \brief - 
 * Get core of specific logical port.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 * \par INDIRECT INPUT:
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] core - core id.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_core_get(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t * core);

/**
 * \brief - 
 * Get core and tm port of specific logical port.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 * \par INDIRECT INPUT:
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] core - core id.
 *   \param [out] tm_port - tm port id.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_tm_port_get(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t * core,
    uint32 *tm_port);

/**
 * \brief - 
 * check if port is part of a lag.
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [out] port_in_lag - indication if port is in lag.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_in_lag(
    int unit,
    bcm_port_t logical_port,
    int *port_in_lag);

/**
 * \brief -
 * set header type in per tm-port
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [out] header_type - see BCM_SWITCH_PORT_HEADER_TYPE_X
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_header_type_in_set(
    int unit,
    bcm_port_t logical_port,
    int header_type);

/**
 * \brief -
 * get header type in per tm-port
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [out] header_type - see BCM_SWITCH_PORT_HEADER_TYPE_X
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_header_type_in_get(
    int unit,
    bcm_port_t logical_port,
    int *header_type);

/**
 * \brief -
 * set header type out per tm-port
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [out] header_type - see BCM_SWITCH_PORT_HEADER_TYPE_X
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_header_type_out_set(
    int unit,
    bcm_port_t logical_port,
    int header_type);

/**
 * \brief -
 * get header type out per tm-port
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [out] header_type - see BCM_SWITCH_PORT_HEADER_TYPE_X
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_header_type_out_get(
    int unit,
    bcm_port_t logical_port,
    int *header_type);

/**
 * \brief -
 * get lag id that this port is associated with.
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [out] lag_id - lag_id that this port is part of.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_lag_id_get(
    int unit,
    bcm_port_t logical_port,
    bcm_trunk_t * lag_id);

/**
 * \brief -
 * Get core and pp port of specific logical port.
 *  
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [out] core - core id.
 * \param [out] pp_port - pp port id.
 *  
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_pp_port_get(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t * core,
    uint32 *pp_port);

/**
 * \brief - 
 * Link logical port to pp port (sw wise only of course).
 * In a case the previous pp port wasn't mark as LAG, the previous pp port will freed.
 *  
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [in] core - core id.
 * \param [in] pp_port - pp port id.
 *  
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_pp_port_modify(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t core,
    uint32 pp_port);

/**
 * \brief - 
 * Get fabric link.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 * \par INDIRECT INPUT:
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] fabric_link - fabric link id.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_fabric_link_get(
    int unit,
    bcm_port_t logical_port,
    int *fabric_link);

/**
 * \brief - 
 *   Set PRBS mode.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   \param [in] prbs_mode - prbs mode.
 * \par INDIRECT INPUT:
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_prbs_mode_set(
    int unit,
    bcm_port_t logical_port,
    portmod_prbs_mode_t prbs_mode);

/**
 * \brief - 
 *   Get PRBS mode.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 * \par INDIRECT INPUT:
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] prbs_mode - prbs mode.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_prbs_mode_get(
    int unit,
    bcm_port_t logical_port,
    portmod_prbs_mode_t * prbs_mode);

/**
 * \brief - 
 * Get logical port from core X tm port
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] core - core id.
 *   \param [in] tm_port - tm port id.
 * \par INDIRECT INPUT:
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] logical_port - logical port #.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_tm_to_logical_get(
    int unit,
    bcm_core_t core,
    uint32 tm_port,
    bcm_port_t * logical_port);

/**
 * \brief - 
 * Get logical port from core X pp port
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] core - core id.
 *   \param [in] pp_port - pp port id.
 * \par INDIRECT INPUT:
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] logical_port - logical port #.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_pp_to_logical_get(
    int unit,
    bcm_core_t core,
    uint32 pp_port,
    bcm_port_t * logical_port);

/**
 * \brief - 
 * Get logical port from core X HR id
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] core - core id.
 *   \param [in] hr_id - HR id.
 * \par INDIRECT INPUT:
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] logical_port - logical port #.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_hr_to_logical_get(
    int unit,
    bcm_core_t core,
    uint32 hr_id,
    bcm_port_t * logical_port);

/**
 * \brief - 
 * Get is_valid for specified port HR
 *           
 * \param [in] unit - Relevant unit.
 * \param [in] core - core #
 * \param [in] hr_id - HR id #
 * \param [out] is_valid - is valid
 * 
 * \return  
 *   see shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_hr_is_valid_get(
    int unit,
    bcm_core_t core,
    uint32 hr_id,
    int *is_valid);

/**
 * \brief - 
 * Get logical port from core X pp port
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] fabric_link - fabric link id.
 * \par INDIRECT INPUT:
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] logical_port - logical port #.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_fabric_link_to_logical_get(
    int unit,
    int fabric_link,
    bcm_port_t * logical_port);

/**
 * \brief -
 * Get Gport from core X pp port
 *
 *   \param [in] unit - unit #.
 *   \param [in] core - Core ID.
 *   \param [in] pp_port - PP port ID.
 *   \param [out] gport - System or Trunk gport.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_pp_to_gport_get(
    int unit,
    bcm_core_t core,
    uint32 pp_port,
    bcm_gport_t * gport);
/*
 * }
 */

/*
 * Set / Get  APIs 
 * { 
 */
/**
 * \brief - 
 * Get port master. 
 * When a few logical port represent the same physical interface, one of them is marked as master.
 * Might be used as single representative of the physical interface. 
 * Use flags for more complex situations:
 *  * Additional (different) master port is required - for example TDM
 *  * Get next master port (used for dynamic port)
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   \param [in] flags - see DNX_ALGO_PORT_MASTER_F_*
 * \par INDIRECT INPUT:
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] master_logical_port - master logical port #.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_master_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    bcm_port_t * master_logical_port);

/**
 * \brief - 
 * Check if the port is master port. 
 * When a few logical port represent the same physical interface, one of them are marked as master. 
 * Might be used as single representative of the physical interface. 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 * \par INDIRECT INPUT:
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] is_master_port - true iff it is the master port
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_is_master_get(
    int unit,
    bcm_port_t logical_port,
    int *is_master_port);

/**
 * \brief - 
 * Get port master from tm interface id obtained by dnx_algo_port_db_interface_id_get().
 * When a few logical port represent the same physical interface, one of them are marked as master.
 * Might be used as single representative of the physical interface.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] interface_id_h0 - DB interface id obtained by dnx_algo_port_db_interface_id_get()
 *   \param [in] interface_id_h1 - DB interface id obtained by dnx_algo_port_db_interface_id_get()
 * \par INDIRECT INPUT:
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] master_port - master logical port #.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_tm_inteface_to_master_get(
    int unit,
    int interface_id_h0,
    int interface_id_h1,
    int *master_port);

/**
 * \brief - 
 * Get port type. 
 * May be used for any kind of logical port. 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - required logical port.
 * \par INDIRECT INPUT:
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] port_info - basic info about port.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_info_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s * port_info);

/**
 * \brief - 
 * Get the master port on the same interface for ILKN.
 * May be used for any kind of logical port.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - required logical port.
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] master_port - master logical port #.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_to_ilkn_master_get(
    int unit,
    bcm_port_t logical_port,
    int *master_port);

/**
 * \brief - 
 * Get interface offset.
 * May be used for any kind of logical port which support interface offset. 
 * (Currently just NIF ports) 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - required logical port.
 * \par INDIRECT INPUT:
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] interface_offset - interface offset # (zero based)
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_interface_offset_get(
    int unit,
    bcm_port_t logical_port,
    int *interface_offset);

/**
 * \brief - Get logical port gport from interface offset.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] port_type - port type of logical port.
 *   \param [in] interface_offset - interface offset # (zero based)
 *   \param [out] gport - logical port gport
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_interface_offset_logical_get(
    int unit,
    dnx_algo_port_type_e port_type,
    int interface_offset,
    bcm_gport_t * gport);
/**
 * \brief - 
 * \brief - Configure channel for
 *         TM port
 *         FlexE client
 *  
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [in] channel_id - channel ID 
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_channel_set(
    int unit,
    bcm_port_t logical_port,
    int channel_id);

/**
 * \brief - Get channel ID for
 *         TM port
 *         FlexE client
 *  
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [out] channel_id - channel ID 
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_channel_get(
    int unit,
    bcm_port_t logical_port,
    int *channel_id);

/**
 * \brief - 
 * Set app flags - might be used for any tm port.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   \param [in] app_flags - each bit represent a app type, such as \
 *                  - DNX_ALGO_PORT_APP_FLAG_PON \ 
 *                  - DNX_ALGO_PORT_APP_FLAG_COE \ 
 *   
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * internal algo port data base
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_app_flags_set(
    int unit,
    bcm_port_t logical_port,
    uint32 app_flags);

/**
 * \brief - 
 * Get app flags - might be used for any tm port.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] app_flags - app flags
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_app_flags_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *app_flags);

/**
 * \brief - 
 *  Get base_q_pair - might be used for any tm port. 
 *  Return the queue id of the first from a sequential egress queue-pairs assigned to a port. 
 *  To get the number of queue-pair ('dnx_algo_port_priorities_nof_get();) 
 *  Each queue-pair include queue for unicast and multicast.
 *  
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [out] base_q_pair - base_q_pair 
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_base_q_pair_get(
    int unit,
    bcm_port_t logical_port,
    int *base_q_pair);

/**
 * \brief - 
 *  Get port and COSQ - Given the queue ID of one of the egress queue-pairs,
 *  get the port and COSQ to which the Q-pair is assigned. 
 *  Returns logical port of type NIF and COSQ.
 *
 * \param [in] unit - unit #.
 * \param [in] q_pair - Q-pair
 * \param [in] core - Core ID
 * \param [out] logical_port - logical port #.
 * \param [out] cosq - cosq
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_q_pair_port_cosq_get(
    int unit,
    int q_pair,
    bcm_core_t core,
    bcm_port_t * logical_port,
    bcm_cos_queue_t * cosq);

/**
 * \brief - 
 *  Get base_q_pair - might be used for any tm port. 
 *  Return the HR id of the first from a sequential HRs assigned
 *  to a port. To get the
 *  number of HRs ('dnx_algo_port_sch_priorities_nof_get();).
 *  
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [out] base_hr - base_hr 
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_base_hr_get(
    int unit,
    bcm_port_t logical_port,
    int *base_hr);

/**
 * \brief -
 *  Get a certain HR (port shceduler priority).
 *  Return the HR id according to the provided scheduler priority.
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [in] sch_priority - required scheduler priority
 * \param [out] hr - returned HR ID
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_hr_get(
    int unit,
    bcm_port_t logical_port,
    int sch_priority,
    int *hr);

/**
 * \brief - 
 *      Set number of port priorities
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [in] num_priorities - number of sch priorities 
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_sch_priorities_nof_set(
    int unit,
    bcm_port_t logical_port,
    int num_priorities);

/**
 * \brief - 
 *      Get number of sch priorities
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [out] num_priorities - number of sch priorities 
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_sch_priorities_nof_get(
    int unit,
    bcm_port_t logical_port,
    int *num_priorities);

/**
 * \brief - 
 *      Get number of port priorities
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [in] num_priorities - number of port priorities 
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_priorities_nof_set(
    int unit,
    bcm_port_t logical_port,
    int num_priorities);

/**
 * \brief - 
 *      Set number of port priorities
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [out] num_priorities - number of port priorities
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_priorities_nof_get(
    int unit,
    bcm_port_t logical_port,
    int *num_priorities);

/**
 * \brief -
 * Get is_channelized attribute - might be used for any tm port.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] is_channelized - is_channelized attribute (configured when adding a tm port)
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_is_channelized_get(
    int unit,
    bcm_port_t logical_port,
    int *is_channelized);

/**
 * \brief -
 * Is ingress interface interleaved
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   \param [out] is_ingress_interleave - is ingress interface interleaved
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_is_ingress_interleave_get(
    int unit,
    bcm_port_t logical_port,
    int *is_ingress_interleave);

/**
 * \brief -
 * Get is_egress_interleave attribute - used for master port.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] is_egress_interleave - is_egress_interleave attribute (configured when adding a tm port)
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_is_egress_interleave_get(
    int unit,
    bcm_port_t logical_port,
    int *is_egress_interleave);

/**
 * \brief -
 * Get scheduler priority propagation enable
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   \param [out] sch_priority_propagation_enable - scheduler priority propagation enable
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_sch_priority_propagation_enable_get(
    int unit,
    bcm_port_t logical_port,
    int *sch_priority_propagation_enable);

/**
 * \brief - 
 * Set interface speed, need to specify the speed is
 * for Rx, Tx, or both Rx and Tx.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   \param [in] flags - speed flags, '0' is not supported,
 *              supported flags:
 *                  - DNX_ALGO_PORT_SPEED_F_RX \
 *                  - DNX_ALGO_PORT_SPEED_F_TX \
 *
 *   \param [in] speed - interface rate in kbps
 *   
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * internal algo port data base
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_speed_set(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int speed);

/**
 * \brief - 
 * Get interface speed
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   \param [in] flags - speed flags, value '0' means for both Rx and Tx and is
 *        only valid when the Rx speed == Tx speed.
 *        Each bit represent a speed type, such as \
 *           - DNX_ALGO_PORT_SPEED_F_MBPS \
 *           - DNX_ALGO_PORT_SPEED_F_KBPS \
 *           - DNX_ALGO_PORT_SPEED_F_RX \
 *           - DNX_ALGO_PORT_SPEED_F_TX \
 *   
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] speed - interface rate in Mbps or Kbps
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_speed_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int *speed);

/**
 * \brief - 
 * Has interface speed
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   \param [in] flags - speed flags, value '0' is only valid
 *        when the Rx speed == Tx speed. If one port may have
 *        asymmetric speed for Rx and Tx, please specify the
 *        flag.
 *        supported flags:
 *            - DNX_ALGO_PORT_SPEED_F_RX \
 *            - DNX_ALGO_PORT_SPEED_F_TX \
 *
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] has_speed - 1 iff interface speed is already set
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_has_speed(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int *has_speed);

/**
 * \brief - 
 * Get phys bitmap of NIF port
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   \param [in] flags - see DNX_ALGO_PORT_NIF_PHYS_F_*
 *                      Currently not implemented.
 *   
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] phys - required phys bitmap
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_phys_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    bcm_pbmp_t * phys);

/**
 * \brief - initialize dnx_algo_port_rmc_info_t struct to
 *        invalid values
 * 
 * \param [in] unit - chip unit id
 * \param [out] rmc_info - rmc_info to initialize
 * 
 * \return
 *   shr_error_e
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_rmc_info_t_init(
    int unit,
    dnx_algo_port_rmc_info_t * rmc_info);

/**
 * \brief - 
 * Get logical fifo information for a NIF port
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [in] flags - see DNX_ALGO_PORT_NIF_PHYS_F_*
 *                      Currently not implemented.
 * \param [in] logical_fifo_index - index in logical fifo
 *          array
 * \param [out] logical_fifo - address to retrieve logical fifo
 *          information.
 *
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_logical_fifo_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int logical_fifo_index,
    dnx_algo_port_rmc_info_t * logical_fifo);

/**
 * \brief - 
 * Set logical fifo information for a NIF port
 * \param [in] unit - Relevant unit.
 * \param [in] logical_port - logical port #.
 * \param [in] flags - see DNX_ALGO_PORT_NIF_PHYS_F_*
 *                      Currently not implemented.
 * \param [in] logical_fifo_index - index in logical fifo
 *          array
 * \param [out] logical_fifo - port's logical fifo information
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_logical_fifo_set(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int logical_fifo_index,
    dnx_algo_port_rmc_info_t * logical_fifo);
/**
 * \brief - get port's logical fifo ids in a bitmap 
 * 
 * \param [in] unit - chip unit id
 * \param [in] logical_port - logical port
 * \param [in] flags - not implemented yet
 * \param [in] logical_fifo_pbmp - output pbmp with all logical 
 *        fifo IDs.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_logical_fifo_pbmp_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    bcm_pbmp_t * logical_fifo_pbmp);

/**
 * \brief - 
 * Get first phy-id of NIF port 
 *  
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [in] flags - see DNX_ALGO_PORT_NIF_FIRST_PHY_F_*
 *                      Currently not implemented.
 * \param [out] first_phy_port - first phy id (0 based)
 *   
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_first_phy_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int *first_phy_port);
/**
 * \brief - 
 * Convert nif phy to logical port
 *  
 * \param [in] unit - unit #.
 * \param [in] phy - nif_phy (0 based)
 * \param [in] is_over_fabric - is over fabric (relevant only for ILKN)
 * \param [in] flags - see DNX_ALGO_PORT_NIF_PHY_TO_LOGICAL_F_*
 *                      Currently not implemented.
 * \param [out] logical_port - logical port #.
 *   
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_phy_to_logical_get(
    int unit,
    int phy,
    int is_over_fabric,
    uint32 flags,
    bcm_port_t * logical_port);

/**
 * \brief -
 * Set number of segments for the ILKN port
 *
 * \param [in] unit - chip unit id
 * \param [in] logical_port - logical port #
 * \param [in] ilkn_lanes - ILKN lanes bitmap (bits 0-23)
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_ilkn_lanes_get(
    int unit,
    bcm_port_t logical_port,
    bcm_pbmp_t * ilkn_lanes);

/**
 * \brief - 
 * Set number of segments for the ILKN port 
 * 
 * \param [in] unit - chip unit id
 * \param [in] logical_port - logical port #
 * \param [in] nof_segments - number of segments occupied by the 
 *        port (0/2/4)
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_ilkn_nof_segments_set(
    int unit,
    bcm_port_t logical_port,
    int nof_segments);

/**
 * \brief - 
 * Get number of segments for the ILKN port 
 * 
 * \param [in] unit - chip unit id
 * \param [in] logical_port - logical port #
 * \param [out] nof_segments - number of segments occupied by 
 *        the port (0/2/4)
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_ilkn_nof_segments_get(
    int unit,
    bcm_port_t logical_port,
    int *nof_segments);

/*
 * See .h file
 */
shr_error_e dnx_algo_port_nif_ilkn_is_over_fabric_get(
    int unit,
    bcm_port_t logical_port,
    int *is_over_fabric);

/*
 * See .h file
 */
shr_error_e dnx_algo_port_nif_ilkn_is_over_fabric_set(
    int unit,
    bcm_port_t logical_port,
    int is_over_fabric);

/**
 * \brief
 *   Get scheduler priority of the ILKN port 
 * 
 * \param [in] unit - chip unit id
 * \param [in] logical_port - logical port #
 * \param [out] priority - scheduler priority of the ILKN port
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_ilkn_priority_get(
    int unit,
    bcm_port_t logical_port,
    bcm_port_nif_scheduler_t * priority);

/**
 * \brief
 *   Set scheduler priority of the ILKN port 
 * 
 * \param [in] unit - chip unit id
 * \param [in] logical_port - logical port #
 * \param [in] priority - scheduler priority of the ILKN port
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_ilkn_priority_set(
    int unit,
    bcm_port_t logical_port,
    bcm_port_nif_scheduler_t priority);

/**
 * \brief - 
 * Get ilkn core aggregated PM info of ILKN port
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   \param [in] is_over_fabric - indicated if port is nif over fabric
 *   \param [out] nof_aggregated_pms - number of aggregated PMs.
 *   \param [out] controlled_pms -aggregated PMs Info .
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_ilkn_core_aggregated_pms_get(
    int unit,
    bcm_port_t logical_port,
    int is_over_fabric,
    int *nof_aggregated_pms,
    portmod_pm_identifier_t * controlled_pms);

/**
 * \brief -
 * Get latch down indication of NIF port
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] is_latch_down - Boolean indication:
 *   0 => all lanes are aligned and deskewed
 *   1 => latch is down. alignment has failed at some point
 *   before this DB was updated.
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_algo_port_nif_latch_down_get(
    int unit,
    bcm_port_t logical_port,
    int *is_latch_down);

/**
 * \brief - 
 * Set latch down indication of NIF port
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [in] is_latch_down - Boolean indication:
 *   0 => all lanes are aligned and deskewed
 *   1 => latch is down. alignment has failed at some point
 *   before this DB was updated.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_latch_down_set(
    int unit,
    bcm_port_t logical_port,
    int is_latch_down);

/**
 * \brief -
 * Get the original enable state of link training of NIF port before setting loopback
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] link_training_enable - Boolean indication:
 *   0 => link training disabled before setting loopback
 *   1 => link training enabled before setting loopback
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_loopback_original_link_training_get(
    int unit,
    bcm_port_t logical_port,
    int *link_training_enable);

/**
 * \brief -
 * Set the original enable state of link training of NIF port before setting loopback
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   \param [in] link_training_enable - Boolean indication:
 *   0 => link training disabled before setting loopback
 *   1 => link training enabled before setting loopback
 *
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   N/A
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_loopback_original_link_training_set(
    int unit,
    bcm_port_t logical_port,
    int link_training_enable);

/**
 * \brief -
 * Get the original phy lane configuration of a port before setting loopback
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] phy_lane_config - original phy lane configuration value
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_loopback_original_phy_lane_config_get(
    int unit,
    bcm_port_t logical_port,
    int *phy_lane_config);

/**
 * \brief -
 * Store the original phy lane configuration of a port before setting loopback
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   \param [in] phy_lane_config - original phy lane configuration value
 *
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   N/A
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_loopback_original_phy_lane_config_set(
    int unit,
    bcm_port_t logical_port,
    int phy_lane_config);

/**
 * \brief - 
 * Set TDM mode.
 * See dnx_algo_port_tdm_mode_e for avalibale modes.
 * Configuration id per TM port.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 *   \param [in] tdm_mode - reqruied mode.
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT 
 *   * None 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_tdm_set(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_tdm_mode_e tdm_mode);

/**
 * \brief - 
 * Get TDM mode.
 * See dnx_algo_port_tdm_mode_e for avalibale modes.
 * Configuration id per TM port.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] tdm_mode - required mode.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_tdm_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_tdm_mode_e * tdm_mode);

/**
 * \brief - 
 * Get interface TDM mode.
 * See dnx_algo_port_if_tdm_mode_e for available modes.
 * Configuration id per TM interface.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] logical_port - logical port #.
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] if_tdm_mode - required mode.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_if_tdm_mode_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_if_tdm_mode_e * if_tdm_mode);

/**
 * \brief -
 * Set egress interface id (egress queuing).
 * Configuration is per TM interface.
 * 
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [in] if_id - interface id number.
 *  
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_egr_if_set(
    int unit,
    bcm_port_t logical_port,
    int if_id);

/**
 * \brief -
 * Clear egress interface id (egress queuing).
 * Configuration is per TM interface.
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_egr_if_unset(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief - 
 * Set egress interface id (egress queuing).
 * Configuration is per TM interface.
 * 
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [out] if_id - interface id number.
 *  
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_egr_if_get(
    int unit,
    bcm_port_t logical_port,
    int *if_id);

/**
 * \brief - 
 * Set scheduler interface id.
 * Configuration is per TM interface.
 * 
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [in] if_id - interface id number.
 *  
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_sch_if_set(
    int unit,
    bcm_port_t logical_port,
    int if_id);

/**
 * \brief - 
 * Set scheduler interface id.
 * Configuration is per TM interface.
 * 
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [out] if_id - interface id number.
 *  
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_sch_if_get(
    int unit,
    bcm_port_t logical_port,
    int *if_id);

/**
 * \brief - 
 *  Map interface type to port type.
 * 
 * \param [in] unit - unit #.
 * \param [in] interface_type - see bcm_port_if_t
 * \param [in] indicator - additional indicator for port type
 * \param [out] port_info - required port info
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_interface_to_port_info_get(
    int unit,
    bcm_port_if_t interface_type,
    dnx_algo_port_info_indicator_t * indicator,
    dnx_algo_port_info_s * port_info);
/**
 * \brief -
 *  Map port type to interface type.
 *  Note that all NIF Ethernet ports will be mapped to BCM_PORT_IF_NIF_ETH and to specific interface such as BCM_PORT_IF_XFI.
 *
 * \param [in] unit - unit #.
 * \param [in] port_info - basic info about port
 * \param [out] indicator - additional indicator for port type
 * \param [out] interface_type - see bcm_port_if_t
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_to_interface_type_get(
    int unit,
    dnx_algo_port_info_s port_info,
    dnx_algo_port_info_indicator_t * indicator,
    bcm_port_if_t * interface_type);
/**
 * \brief -
 *  Map interface legacy type to number of lanes
 *
 * \param [in] unit - unit #.
 * \param [in] interface_type - Interface type to map
 * \param [out] nof_lanes - number of lanes
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_legacy_interface_to_nof_lanes_get(
    int unit,
    bcm_port_if_t interface_type,
    int *nof_lanes);

/*
 * }
 */

/**
 * \brief -
 * Get port flexe client ID
 * Configuration is per flexe client index X (CLIENT, MAC, BUS).
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [in] client_id - client ID.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_flexe_client_id_get(
    int unit,
    bcm_port_t logical_port,
    int *client_id);
/**
 * \brief -
 * Get logical port from client index X (CLIENT, MAC, BUS)
 *
 * \param [in] unit - unit #.
 * \param [in] port_type - flexe port type.
 * \param [in] client_index - client index.
 * \param [out] logical_port - logical port #.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_flexe_client_index_to_port_get(
    int unit,
    dnx_algo_port_type_e port_type,
    int client_index,
    bcm_port_t * logical_port);

/**
 * \brief -
 * Get port flexe client bypass mode
 * Configuration is per flexe client index
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [in] is_bypass - is bypass.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_flexe_client_is_bypass_get(
    int unit,
    bcm_port_t logical_port,
    int *is_bypass);

/**
 * \brief -
 * Set port flexe client bypass mode
 * Configuration is per flexe client index
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [in] is_bypass - is bypass.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_flexe_client_is_bypass_set(
    int unit,
    bcm_port_t logical_port,
    int is_bypass);

/**
 * \brief -
 * Get port flexe phy core port
 * Configuration is per flex first phy
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [in] core_port - core port #.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_flexe_core_port_get(
    int unit,
    bcm_port_t logical_port,
    int *core_port);

/**
 * \brief -
 * Set port flexe phy core port
 * Configuration is per flexe first phy
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [in] core_port - core port #.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_flexe_core_port_set(
    int unit,
    bcm_port_t logical_port,
    int core_port);
/**
 * \brief -
 * Get port flexe phy logical_phy_id
 * Configuration is per flexe first phy
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [in] logical_phy_id - logical phy #.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_flexe_logical_phy_id_get(
    int unit,
    bcm_port_t logical_port,
    int *logical_phy_id);

/**
 * \brief -
 * Set port flexe phy logical_phy_id
 * Configuration is per flexe first phy
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [in] logical_phy_id - logical phy #.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_algo_port_flexe_logical_phy_id_set(
    int unit,
    bcm_port_t logical_port,
    int logical_phy_id);

/**
 * \brief - Printing callback for skipping first header size template
 *
 * \param [in] unit - Relevant unit
 * \param [in] data - Pointer to template profile data
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_algo_port_db_skip_first_ud_header_size_print_cb(
    int unit,
    const void *data);

/*
 * INCLUDE FILES: 
 * additional files to simplify port mgmt include 
 * {
 */

/**
 * \brief - Get mib counter-set type for a given logical port.
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [out] nof_sets - Actual size of counter_set_type array.
 * \param [out] counter_sets - List of counter-set types .
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * MIB counter-set is a set of HW counters dedicated for the specific NIF port.
 * \see
 *   * None
 */

shr_error_e dnx_algo_port_counter_sets_get(
    int unit,
    bcm_port_t logical_port,
    int *nof_sets,
    dnx_algo_port_mib_counter_set_type_e counter_sets[DNX_ALGO_PORT_COUNTER_SETS_MAX]);

/**
 * \brief - Get valid indication for a given port
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #.
 * \param [out] is_valid - Port valid indcation.
 *
 * \return
 *   see shr_error_e
 * \remark
 *
 * \see
 *   * None
 */

shr_error_e dnx_algo_port_is_valid_get(
    int unit,
    bcm_port_t logical_port,
    int *is_valid);

#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/algo/port/algo_port_verify.h>

/*
 * }
 */

/*
 * }
 */
#endif /*_ALGO_PORT_MGMT_API_INCLUDED__*/
