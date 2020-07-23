/** \file algo_port_utils.h
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
#ifndef ALGO_PORT_UTILS_H_INCLUDED
/*
 * {
 */
#define ALGO_PORT_UTILS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
/*
 * Includes
 * {
 */
/*
 * }
 */
/*
 * MACROs
 * {
 */

/**
 * \brief "logicals bitmap get" flag to filter the result -
 * all non master logicals are filtered.
 */
#define DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY (0x1)
/**
 * \brief "logicals bitmap get" flag to filter the result -
 * all ELK ports are filtered.
 */
#define DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK (0x1 << 1)
/**
 * \brief "logicals bitmap get" flag to filter the result -
 * all STIF ports are filtered.
 */
#define DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_STIF (0x1 << 2)
/**
 * \brief "logicals bitmap get" flag to filter the result -
 * all L1 ports are filtered.
 */
#define DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_L1 (0x1 << 3)
/**
 * \brief "logicals bitmap get" flag to filter the result -
 * all ports in removed state are filtered.
 */
#define DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT (0x1 << 4)
/**
 * \brief "logicals bitmap get" flag to filter the result -
 * all ports in flexe phy are filtered.
 */
#define DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_FLEXE_PHY (0x1 << 5)

/**
 * \brief channels get flag - get tdm-bypass only channels
 */
#define DNX_ALGO_PORT_CHANNELS_F_TDM_BYPASS_ONLY   (0x1)
/**
 * \brief channels get flag - get non tdm-bypass only channels
 */
#define DNX_ALGO_PORT_CHANNELS_F_NON_TDM_BYPASS_ONLY   (0x2)
/**
 * \brief channels get flag - get tdm-bypass or l1 only channels
 */
#define DNX_ALGO_PORT_CHANNELS_F_TDM_BYPASS_OR_L1_ONLY   (0x4)
/**
 * \brief channels get flag - get non l1 channels
 */
#define DNX_ALGO_PORT_CHANNELS_F_NON_L1_ONLY   (0x8)
/**
 * \brief supported flags mask
 */
#define DNX_ALGO_PORT_CHANNELS_F_MASK (DNX_ALGO_PORT_CHANNELS_F_TDM_BYPASS_ONLY | DNX_ALGO_PORT_CHANNELS_F_NON_TDM_BYPASS_ONLY | DNX_ALGO_PORT_CHANNELS_F_TDM_BYPASS_OR_L1_ONLY)

/**
 * \brief return true iff port exist and port type is DNX_ALGO_PORT_TYPE_CPU
 */
#define DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_info)   (port_info.port_type == DNX_ALGO_PORT_TYPE_CPU)

/**
 * \brief return true iff port exist and port type is DNX_ALGO_PORT_TYPE_ERP
 */
#define DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_info)   (port_info.port_type == DNX_ALGO_PORT_TYPE_ERP)

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_SCH_ONLY
 */
#define DNX_ALGO_PORT_TYPE_IS_SCH_ONLY(unit, port_info)   (port_info.port_type == DNX_ALGO_PORT_TYPE_SCH_ONLY)

/**
 * \brief return true if port exist and represents fabric interface
 */
#define DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info)   (port_info.is_fabric)

/**
 * \brief return true if port exist and nif eth stif
 */
#define DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info)   (port_info.is_nif_eth && port_info.is_stif)
/**
 * \brief return true if port exist and represent nif eth l1
 */
#define DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info)   (port_info.is_nif_eth && port_info.is_l1)

/**
 * \brief return true if port exist and it represents nif eth
 *      not including stif if "include_stif"==0
 *      not including l1 if "include_l1"==0
 */
#define DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, include_stif, include_l1) \
        (port_info.is_nif_eth && !port_info.is_flexe_phy && (include_stif || !port_info.is_stif) && (include_l1 || !port_info.is_l1))

/**
 * \brief return true if port exist and port type is
 *        DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK
 */
#define DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info)   (port_info.is_nif_ilkn && port_info.is_elk)

/**
 * \brief return true if port exist and represents ILM
 *      not including elk if "include_elk"==0
 *      not including l1 if "include_l1"==0
 */
#define DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, include_elk, include_l1) \
        (port_info.is_nif_ilkn && (include_elk || !port_info.is_elk) && (include_l1 || !port_info.is_l1))

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_NIF_ILKN or DNX_ALGO_PORT_TYPE_IS_NIF_ETH
 */
#define DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, include_elk, include_stif, include_l1, include_flexe)   \
        (port_info.is_nif && \
        (include_elk || !port_info.is_elk) && (include_stif || !port_info.is_stif) && \
        (include_l1 || !port_info.is_l1) && (include_flexe || !port_info.is_flexe_phy))

/**
 * \brief return true if port exist and port  is dispachable
 *        filter flexe ports if "include_flexe"==True
 */
#define DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_info, include_flexe)   \
        (port_info.is_imb && (include_flexe || !port_info.is_flexe_client))

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_OLP
 */
#define DNX_ALGO_PORT_TYPE_IS_OLP(unit, port_info)  (port_info.port_type == DNX_ALGO_PORT_TYPE_OLP)

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_RCY
 */
#define DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info)   (port_info.port_type ==  DNX_ALGO_PORT_TYPE_RCY)

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_OAMP
 */
#define DNX_ALGO_PORT_TYPE_IS_OAMP(unit, port_info)   (port_info.port_type ==  DNX_ALGO_PORT_TYPE_OAMP)

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_EVENTOR
 */
#define DNX_ALGO_PORT_TYPE_IS_EVENTOR(unit, port_info)   (port_info.port_type == DNX_ALGO_PORT_TYPE_EVENTOR)

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_SAT
 */
#define DNX_ALGO_PORT_TYPE_IS_SAT(unit, port_info)   (port_info.port_type == DNX_ALGO_PORT_TYPE_SAT)

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_RCY_MIRROR
 */
#define DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, port_info)  (port_info.port_type == DNX_ALGO_PORT_TYPE_RCY_MIRROR)

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_CRPS
 */
#define DNX_ALGO_PORT_TYPE_IS_CRPS(unit, port_info)   (port_info.port_type == DNX_ALGO_PORT_TYPE_CRPS)

/**
 * \brief return true if port exist and port type represents tm port
 */
#define DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info)   (port_info.is_tm)
/**
 * \brief return true if port exist and port type represents pp port
 *                    currently all TM ports are also PP - will be changed when LAG SCH implemented.
 */
#define DNX_ALGO_PORT_TYPE_IS_PP(unit, port_info, is_lag)  ((port_info.is_ing_pp || port_info.is_egr_pp) && !is_lag)
/**
 * \brief return true if port exist and port type represents ingress pp port
 */
#define DNX_ALGO_PORT_TYPE_IS_ING_PP(unit, port_info, is_in_lag) (port_info.is_ing_pp && !is_in_lag)
/**
 * \brief return true if port exist and port type represents egress pp port
 */
#define DNX_ALGO_PORT_TYPE_IS_EGR_PP(unit, port_info, is_in_lag)   (port_info.is_egr_pp && !is_in_lag)
/**
 * \brief return true if port exist and port type represents ingress tm port
 */
#define DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info)   (port_info.is_ing_tm)

/**
 * \brief return true if port exist and port type represents egress tm port
 */
#define DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info)   (port_info.is_egr_tm)
/**
 * \brief return true port type represents e2e scheduler port
 */
#define DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info)   (port_info.is_sch)

/**
 * \brief return true if port type represents FLEXE PHY port
 */
#define DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info) (port_info.port_type == DNX_ALGO_PORT_TYPE_FLEXE_PHY)
/**
 * \brief return true if port type represents FLEXE MAC port
 */
#define DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, include_l1) ((port_info.port_type == DNX_ALGO_PORT_TYPE_FLEXE_MAC) && (include_l1 || !port_info.is_l1))
/**
 * \brief return true if port type represents FLEXE SAR port
 */
#define DNX_ALGO_PORT_TYPE_IS_FLEXE_SAR(unit, port_info) (port_info.port_type == DNX_ALGO_PORT_TYPE_FLEXE_SAR)
/**
 * \brief return true if port type represents FLEXE CLIENT port
 */
#define DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info) (port_info.port_type == DNX_ALGO_PORT_TYPE_FLEXE_CLIENT)

/**
 * \brief return true if port type represents FLEXE port, to filter FLEXE MAC use include_mac=FALSE,
 *      to cilter FLEXE_PHY use include_phy=False
 */
#define DNX_ALGO_PORT_TYPE_IS_FLEXE(unit, port_info, include_mac, include_phy) \
    ((port_info.is_flexe_phy || port_info.is_flexe_client) && \
    (include_mac || !DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, 1)) && \
    (include_phy || !DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info)))

/**
 * \brief return true if port type represents L1 port
 */
#define DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info) (port_info.is_l1)

/**
 * \brief return true if port exist and represnet NIF ETH L1
 */
#define DNX_ALGO_PORT_TYPE_IS_ETH_L1(unit, port_info) (port_info.is_nif_eth && port_info.is_l1)

/**
 * \brief return true if port exist and represent FLEX MAC L1
 */
#define DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC_L1(unit, port_info) ((port_info.port_type == DNX_ALGO_PORT_TYPE_FLEXE_MAC) && port_info.is_l1)

/**
 * \brief return true if port exist and represent ILKN L1
 */
#define DNX_ALGO_PORT_TYPE_IS_ILKN_L1(unit, port_info) (port_info.is_nif_ilkn && port_info.is_l1)

/**
 * \brief max length of port type string
 */
#define DNX_ALGO_PORT_TYPE_STR_LENGTH           (20)

/**
 * \brief max length of tdm mode string
 */
#define DNX_ALGO_PORT_TDM_MODE_STR_LENGTH        (20)
/**
 * \brief max length of interface tdm mode string
 */
#define DNX_ALGO_PORT_IF_TDM_MODE_STR_LENGTH        (20)

#define DNX_ALGO_PORT_ILKN_WORD_SIZE     (64)
#define DNX_ALGO_PORT_ILKN_WORD_SIZE_WITH_CTRL_BIT     (67)
#define DNX_ALGO_PORT_ILKN_MAX_BURST_SIZE     (256)
#define DNX_ALGO_PORT_ILKN_MAX_BURST_SIZE_WITH_CTRL_WORD     (264)

/**
 * \brief flags used for dnx_algo_port_ilkn_active_ids_get
 */
#define DNX_ALGO_PORT_ILKN_ID_GET_ILKN_DATA_ONLY        (0x1)
#define DNX_ALGO_PORT_ILKN_ID_GET_ILKN_ELK_ONLY         (0x2)
#define DNX_ALGO_PORT_ILKN_ID_GET_ALL                   (DNX_ALGO_PORT_ILKN_ID_GET_ILKN_ELK_ONLY | DNX_ALGO_PORT_ILKN_ID_GET_ILKN_DATA_ONLY)

/*
 * }
 */

/*
 * Typedefs
 * {
 */
/**
 * \brief
 * Group of port types - will be added upon requirement.
 */
typedef enum
{
    /**
     * Unused  logical ports.
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_INVALID = -1,
    /**
     * All valid ports.
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_VALID = 0,
    /**
     * Network interface logical ports - both ethernet and interlaken interfaces
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_NIF = 1,
    /**
     * Network interface ethernet logical ports.
     * Not including Statistic inteface.
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH = 2,
    /**
     * Fabric logical ports
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_FABRIC = 3,
    /**
     * TM logical ports for egress SCH
     * Relevant for Egress SCH
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_TM_E2E_SCH = 4,
    /**
     * TM logical ports - not including ERP
     * Relevant for ingress TM
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_TM_ING = 5,
    /**
     * PP logical ports
     * Currently - same as DNX_ALGO_PORT_LOGICALS_TYPE_TM_WITHOUT_ERP
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_PP = 6,
    /**
     * TM logical ports - not including ERP
     * Relevant only for egress queuing
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING = 7,
    /**
      * Network interface ILKN logical ports.
      * Including ELK ports
      */
    DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN = 8,
    /**
      * NIF ELK ports
      */
    DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK = 9,
    /**
      * CPU ports
      */
    DNX_ALGO_PORT_LOGICALS_TYPE_CPU = 10,
   /**
     * TM logical ports
     * ports which have tm_port
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_TM = 11,
    /**
      * FlexE physical ports
      */
    DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_PHY = 12,
    /**
      * FlexE physical ports
      */
    DNX_ALGO_PORT_LOGICALS_TYPE_SAT = 13,
    /**
     * FlexE MAC client
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_MAC = 14,
    /**
     * L1 port
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_L1 = 15,
    /**
     * number of logicals types
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_NOF
} dnx_algo_port_logicals_type_e;

/**
 * \brief - significant info about logical port (used for diag command)
 */
typedef struct
{
    /**
     * Logical port #
     */
    bcm_port_t logical_port;
    /**
     * Port type
     */
    dnx_algo_port_info_s port_info;
    /**
     * core_id or BCM_CORE_ALL if relevant to both
     */
    bcm_core_t core_id;
    /**
     * phy_id - first_phy_port for nif or link id for fabric
     */
    int phy_id;
    /**
     * tm_port - tm port id if relevant or DNX_ALGO_PORT_INVALID.
     */
    uint32 tm_port;
    /**
     * channel - tm channel id if relevant or -1.
     */
    int channel_id;

} dnx_algo_port_diag_info_t;

/**
 * \brief -  logical port info (used for diag command)
 */
typedef struct
{
    /**
     * Logical port
     */
    bcm_port_t logical_port;
    /**
     * Port type
     */
    dnx_algo_port_info_s port_info;
    /**
     * Interface offset
     */
    int interface_offset;
    /**
     * Master port
     */
    bcm_port_t master_port;

} dnx_algo_port_diag_info_logical_t;

/**
 * \brief -  nif info (used for diag command)
 */
typedef struct
{
    /**
     * is this info valid
     */
    int valid;
    /**
     * Phy ports bitmap
     */
    bcm_pbmp_t phy_ports;
    /**
     * First phy port
     */
    int first_phy;
    /**
     * Logical Fifos ids
     */
    bcm_pbmp_t logical_fifos;

} dnx_algo_port_diag_info_nif_t;

/**
 * \brief -  ILKN info (used for diag command)
 */
typedef struct
{
    /**
     * is this info valid
     */
    int valid;
    /**
     * Nof Segments
     */
    int nof_segments;

    /*
     * Is ILKN interleaved
     */
    int is_interleaved;

} dnx_algo_port_diag_info_ilkn_t;

/**
 * \brief -  tm & pp info (used for diag command)
 */
typedef struct
{
    /**
     * is this info valid
     */
    int valid;
    /**
     * TM port
     */
    uint32 tm_port;
    /**
     * Channel ID
     */
    int channel_id;
    /**
     * Channelized port - Flag that set when port added to DB
     */
    int is_channelized;
    /**
     * Base queue pair
     */
    int base_q_pair;
    /**
     * Number of port priorities
     */
    int num_priorities;

    /**
     * is sch info valid
     */
    int sch_valid;
    /**
     * Base hr
     */
    int base_hr;
    /**
     * Number of scheduler priorities
     */
    int sch_priorities;
    /**
     * PP port
     */
    uint32 pp_port;
    /**
     * Egress interface id
     */
    int egr_if_id;
    /**
     * Egress interface interleaved
     */
    int egr_if_interleaved;
    /**
     * Scheduler interface id
     */
    int sch_if_id;
    /**
     * Logical ports with the same interface
     */
    bcm_pbmp_t channels_ports;
    /**
     * tdm mode
     */
    dnx_algo_port_tdm_mode_e tdm_mode;
    /**
     * interface tdm mode
     */
    dnx_algo_port_if_tdm_mode_e if_tdm_mode;
    /**
     * tdm bypass master
     */
    bcm_port_t tdm_bypass_master;
    /**
     * non tdm bypass master
     */
    bcm_port_t non_tdm_bypass_master;
} dnx_algo_port_diag_info_tm_pp_t;

/**
 * \brief -  tm & pp info (used for diag command)
 */
typedef struct
{
    /**
     * is this info valid
     */
    int valid;
    /**
     * Core id
     */
    bcm_core_t core_id;
    /**
     * Rx speed of the interface [kbps]
     */
    int interface_rx_speed;
    /**
     * Tx speed of the interface [kbps]
     */
    int interface_tx_speed;
} dnx_algo_port_diag_info_interface_t;

/**
 * \brief -  fabric info (used for diag command)
 */
typedef struct
{
    /**
     * is this info valid
     */
    int valid;
    /**
     * Port interface type
     */
    int link_id;

} dnx_algo_port_diag_info_fabric_t;

/**
 * \brief - all info about logical port (used for diag command)
 */
typedef struct
{
    /**
     * Logical port info
     */
    dnx_algo_port_diag_info_logical_t logical_port_info;
    /**
     * NIF info
     */
    dnx_algo_port_diag_info_nif_t nif_info;

    /**
     * TM and PP info
     */
    dnx_algo_port_diag_info_interface_t interface_info;

    /**
     * TM and PP info
     */
    dnx_algo_port_diag_info_tm_pp_t tm_pp_info;
    /**
     * Fabric info
     */
    dnx_algo_port_diag_info_fabric_t fabric_info;
    /**
     * ILKN info
     */
    dnx_algo_port_diag_info_ilkn_t ilkn_info;

} dnx_algo_port_diag_info_all_t;

/**
 * \brief -  Access port info (used for dbal tables access)
 */
typedef struct
{
    /**
     * Core
     */
    bcm_core_t core;
    /**
     * ETH unit index in device
     */
    int ethu_id;
    /**
     * IMB unit (CDU/CLU) type in device
     */
    int imb_type;
    /**
     * IMB unit (CDU/CLU) index in device
     */
    int imb_type_id;
    /**
     * ETHU index inside the core
     */
    int ethu_id_in_core;
    /**
      * Lane id inside the Core;
      */
    int lane_in_core;
    /**
     * first lane of the PORT. Holds the first lane in ETHU ID related to the port.
     */
    int first_lane_in_port;
    /**
     * first lane of the ETHU (Regardless the Port)
     */
    int first_lane_in_ethu;
    /**
     * number of lanes in the ETHU
     */
    int nof_lanes_in_ethu;
    /**
     * PM id in the ETHU
     */
    int pm_id_in_ethu;
    /**
     * MAC id inside the CDU;
     */
    uint32 mac_id;
    /**
     * MAC mode config id inside the CDU;
     */
    uint32 mac_mode_config_id;

} dnx_algo_port_ethu_access_info_t;

/**
 * \brief -  Access port info (used for dbal tables access)
 */
typedef struct
{

    /**
     * Ilkn index
     */
    int ilkn_id;
    /**
     * Ilkn unit index in device
     */
    int ilkn_core;
    /**
      * port id inside the Core;
      */
    int port_in_core;
    /**
     * PM IMB unit (CDU/CLU) type in device
     */
    int pm_imb_type;

} dnx_algo_port_ilkn_access_info_t;

/**
 * \brief -  PM info
 */
typedef struct
{

    /**
     * PHYs
     */
    bcm_pbmp_t phys;
    /**
     * PM dispatch type
     */
    portmod_dispatch_type_t dispatch_type;
} dnx_algo_pm_info_t;

/*
 * }
 */
/**
 * \brief -
 *   Given an egress intrface and a logical port, which resides on that interface,
 *   get the 'next' logical port on that interface. If input logical port is
 *   DNX_ALGO_PORT_INVALID ('-1') then the first logical port is required.
 *   If returned logical port is DNX_ALGO_PORT_INVALID ('-1') then it indicates
 *   there are no more logical ports on that interface.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Required core # for all logical ports.
 * \param [in] egr_if -
 *   The interface on which all logical ports reside.
 * \param [in,out] logical_port_p -
 *   As input,
 *     pointed memory is the logical port for which the 'next' is required.
 *     Caller must make sure this logical port is, indeed, on specified interface.
 *     If not, then DNX_ALGO_PORT_INVALID will be returned.
 *     If pointed memory is DNX_ALGO_PORT_INVALID then the first logical port is required.
 *   As output,
 *     this procedure loads pointed memory by the 'next' logical port on specified
 *     interface. If loaded value is DNX_ALGO_PORT_INVALID then there are no more
 *     logical ports on that interface.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * NDIRECT INPUT: internal algo port data base
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_next_logical_on_if_get(
    int unit,
    bcm_core_t core,
    int egr_if,
    bcm_port_t * logical_port_p);

/**
 * \brief -
 * Get bitmap of required logical ports
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] core - required core #, or BCM_CORE_ALL for all cores
 *   \param [in] logicals_type - type of logical ports to get (see dnx_algo_port_logical_type_t)
 *   \param [in] flags - see DNX_ALGO_PORT_LOGICALS_F_*
 *
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] logicals - required logicals bitmap
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_logicals_get(
    int unit,
    bcm_core_t core,
    dnx_algo_port_logicals_type_e logicals_type,
    uint32 flags,
    bcm_pbmp_t * logicals);

/**
 * \brief -
 * Get bitmap of all the members of interface
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port of one of the channels
 *   \param [in] flags - see DNX_ALGO_PORT_CHANNELS_F_*
 *
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] logical_port_channnels - all the logical ports with the same interface.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_channels_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    bcm_pbmp_t * logical_port_channnels);

/**
 * \brief -
 * Get the number of all the members of interface
 *
 * \param [in] unit - Relevant unit.
 * \param [in] logical_port - logical port of one of the channels
 * \param [out] nof_channels - number of channels
 *
 * \return
 *   see shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_channels_nof_get(
    int unit,
    bcm_port_t logical_port,
    int *nof_channels);

/**
 * \brief
 *   Get all channels in an ILKN core with certain scheduler priority
 *
 * \param [in] unit - Relevant unit.
 * \param [in] logical_port - logical port of one of the channels in
 *                            the ILKN core.
 * \param [in] sch_prio - the required scheduler priority
 * \param [out] ilkn_core_channels - all the channels in the ILKN core
 *                                   with the required scheduler priority
 *
 * \return
 *   see shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_ilkn_sch_prio_channels_in_core_get(
    int unit,
    bcm_port_t logical_port,
    bcm_port_nif_scheduler_t sch_prio,
    bcm_pbmp_t * ilkn_core_channels);

/*
 * }
 */

/**
 * \brief - Get all pp ports per core (including LAG pp ports)
 *
 * \param [in] unit - Relevant unit
 * \param [in] core - Core ID
 * \param [out] pp_pbmp - PP port bitmap
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_pp_pbmp_get(
    int unit,
    bcm_core_t core,
    bcm_pbmp_t * pp_pbmp);

/**
 * \brief - Utility provided to map logical port pbmp to PP port pbmp
 *       pbmp
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_pbmp - logical ports bitmap.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   * See shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] pbmp_pp_arr - An array of PP ports bitmap per core. Will be cleared and filled by the procedure.
 * \remark
 *   * None
 * \see
 *   * For PBMP usage and its types see include/bcm/types.h
 */
shr_error_e dnx_algo_port_logical_pbmp_to_pp_pbmp_get(
    int unit,
    bcm_pbmp_t logical_pbmp,
    bcm_pbmp_t * pbmp_pp_arr);

/*
 * }
 */

/**
 * \brief - Utility provided to map PP port to  logical pbmp
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] core - Device core.
 *   \param [in] pp_ports - PP ports of the core above
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   * See shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] logical_pbmp - logical ports bitmap.
 * \remark
 *   * Relies on a function that maps PP port to local port
 * \see
 *   * For PBMP usage and its types see include/bcm/types.h
 */
shr_error_e dnx_algo_port_pp_pbmp_to_logical_pbmp_get(
    int unit,
    bcm_core_t core,
    bcm_pbmp_t pp_ports,
    bcm_pbmp_t * logical_pbmp);

/**
 * \brief - Utility provided to map single PP port to tm pbmp
 *
 * \param [in] unit - Relevant unit.
 * \param [in] core - Device core.
 * \param [in] pp_port - PP port.
 * \param [out] pbmp_tm_ports - TM ports bitmap.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_pp_to_tm_pbmp_get(
    int unit,
    bcm_core_t core,
    bcm_port_t pp_port,
    bcm_pbmp_t * pbmp_tm_ports);

/**
 * \brief - Get interface rate:
 *          For ILKN: interface_rate = num_of_lanes * port_speed
 *          Otherwise: interface_rate = port_speed
 *
 * \param [in] unit - Relevant unit.
 * \param [in] logical_port - logical port #.
 * \param [in] flags - each bit represent a speed type, such as \
 *                  - DNX_ALGO_PORT_SPEED_F_MBPS \
 *                  - DNX_ALGO_PORT_SPEED_F_KBPS \
 * \param [out] rate - rate in Mbps or Kbps
 *
 * \return
 *   * See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_interface_rate_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int *rate);

/**
 * \brief - Get ILKN data rate from serdes rate:
 *          ilkn_data_rate = port_speed * num_of_lanes * coef
 *
 * \param [in] unit - Relevant unit.
 * \param [in] logical_port - logical port
 * \param [in] serdes_rate - speed for single serdes
 * \param [in] nof_lanes - number of lanes for port
 * \param [out] data_rate - data rate
 *
 * \return
 *   * See shr_error_e
 * \remark
 *   * This conversion doesn't take FEC into consideration when making
 *   * calculations for the coef.
 *   *  Formula with FEC:
 *   *      64 [bits in ILKN word] x { 4096 [FEC frames between Alignment Marker] x 5140 [data bit in FEC Frame] - 1240 [size of AM] }
 *   *        ---------------------------------------------------------------------------------------------------------------
 *   *                                 65 [ILKN word in bits with one control bit]
 *   * ---------------------------------------------------------------------------------------------------------------------------
 *   *           { 4096 [FEC frames between AM] x 5440 [size of FEC Frame including syndrome] } x 256 [max burst size]
 *   *        ----------------------------------------------------------------------------------------------------------
 *   *                                  264 [max burst size with one control word]
 *   *
 *   *  Formula without FEC:
 *   *                  64 [bits in ILKN word] x 256 [max burst size]
 *   *    -----------------------------------------------------------------------------------------------------
 *   *      67 [ILKN word in bits with one control bit] x 264 [max burst size with one control word]
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_ilkn_serdes_rate_to_data_rate_conv(
    int unit,
    bcm_port_t logical_port,
    int serdes_rate,
    int nof_lanes,
    int *data_rate);

/**
 * \brief - Relevant for NIF ports only.
 *          Return the number of phys used by logical_port.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] logical_port - logical port #.
 * \param [out] nof_phys - nof_phys
 *
 * \return
 *   * See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_phys_nof_get(
    int unit,
    bcm_port_t logical_port,
    int *nof_phys);

/**
 * \brief - Get significant info about logical port (used for diag command)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   * See shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] diag_info - significant info about logical port
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_diag_info_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_diag_info_t * diag_info);

/**
 * \brief - Get all info about logical port (used for diag command)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   * See shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] diag_info_all - all info about logical port
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_diag_info_all_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_diag_info_all_t * diag_info_all);

/**
 * \brief - Convert port type to string
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] port_type - port type.
 *   \param [out] port_type_str - buffer to hold the string.
 */
shr_error_e dnx_algo_port_type_str_get(
    int unit,
    dnx_algo_port_type_e port_type,
    char port_type_str[DNX_ALGO_PORT_TYPE_STR_LENGTH]);

/**
 * \brief - Convert tdm mode to string
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] tdm_mode - enum value
 *   \param [out] tdm_mode_str - buffer to hold the string.
 */
shr_error_e dnx_algo_port_tdm_mode_str_get(
    int unit,
    dnx_algo_port_tdm_mode_e tdm_mode,
    char tdm_mode_str[DNX_ALGO_PORT_TDM_MODE_STR_LENGTH]);
/**
 * \brief - Convert interface tdm mode to string
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] if_tdm_mode - enum value
 *   \param [out] if_tdm_mode_str - buffer to hold the string.
 */

shr_error_e dnx_algo_port_if_tdm_mode_str_get(
    int unit,
    dnx_algo_port_if_tdm_mode_e if_tdm_mode,
    char if_tdm_mode_str[DNX_ALGO_PORT_IF_TDM_MODE_STR_LENGTH]);

/**
 * \brief - Get all information required for CDU DABL access
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port number.
 * \param [out] cdu_access_info - cdu access info struct.
 */
shr_error_e dnx_algo_port_ethu_access_info_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_ethu_access_info_t * cdu_access_info);

/**
 * \brief - Get all information required for ILKN DABL access
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port number.
 * \param [out] ilkn_access_info - ilkn access info struct.
 */
shr_error_e dnx_algo_port_ilkn_access_info_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_ilkn_access_info_t * ilkn_access_info);

/**
 * \brief - Get first logical_port with the requested port type and core
 *
 * \param [in] unit - unit #.
 * \param [in] core - requested core or BCM_CORE_ALL for both
 * \param [in] port_type - requested port_type
 * \param [out] logical_port - the first logical port or DNX_ALGO_PORT_INVALID if not found
 */
shr_error_e dnx_algo_port_first_port_type_get(
    int unit,
    bcm_core_t core,
    dnx_algo_port_type_e port_type,
    bcm_port_t * logical_port);

/**
 * \brief - Get the PLL index for given logical port
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port#
 * \param [out] pll_index - pll index
 */
shr_error_e dnx_algo_port_pll_index_get(
    int unit,
    bcm_port_t logical_port,
    int *pll_index);

/**
 * \brief -
 *  Return NIF phys (0-95) from ILKN lanes
 *
 * \param [in] unit - unit #.
 * \param [in] interface_id - ILKN Interface Id
 * \param [in] ilkn_lanes - ILKN lanes bitmap (0-23)
 * \param [out] phys - NIF phys bitmap (0-95)
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_ilkn_lanes_to_nif_phys_get(
    int unit,
    int interface_id,
    bcm_pbmp_t * ilkn_lanes,
    bcm_pbmp_t * phys);

/**
 * \brief -
 *  Return ILKN lanes (0-23) from NIF phys
 *
 * \param [in] unit - unit #.
 * \param [in] interface_id - ILKN Interface Id
 * \param [in] nif_phys - NIF phys bitmap (0-95)
 * \param [out] ilkn_lanes - ILKN lanes bitmap (0-23)
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_phys_to_ilkn_lanes_get(
    int unit,
    int interface_id,
    bcm_pbmp_t * nif_phys,
    bcm_pbmp_t * ilkn_lanes);

/**
 * \brief -
 *  Get ETHU ID from NIF PHY
 *
 * \param [in] unit - unit #.
 * \param [in] phy - NIF Phy
 * \param [out] ethu_id - Ethernet Unit ID
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_phy_to_ethu_id_get(
    int unit,
    int phy,
    int *ethu_id);

/**
 * \brief -
 *  Get first NIF PHY of ETHU
 *
 * \param [in] unit - unit #.
 * \param [in] core - requested core.
 * \param [in] ethu_id_in_core - Ethernet Unit ID
 * \param [out] first_phy - First Phy in Ethernet Unit
 * \param [out] nof_lanes - Number of lanes in Ethernet Unit
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_ethu_id_lane_info_get(
    int unit,
    int core,
    int ethu_id_in_core,
    int *first_phy,
    int *nof_lanes);

/**
 * \brief - Relevant for NIF ports only.
 *          Return core and NIF interface ID
 *
 * \param [in] unit - Relevant unit.
 * \param [in] logical_port - logical port #.
 * \param [in] flags - flags.
 * \param [out] core - core ID
 * \param [out] nif_interface_id - NIF interface ID
 *
 * \return
 *   * See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_interface_id_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int *core,
    int *nif_interface_id);

/**
 * \brief -
 *  Get lane info for ilkn according to phy
 *
 * \param [in] unit - unit #.
 * \param [in] core - requested core.
 * \param [in] phy - phy id of  the ilkn lane.
 * \param [out] ethu_id_in_core - Ethernet Unit ID
 * \param [out] lane - lane in Ethernet Unit
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_ilkn_lane_info_get(
    int unit,
    int core,
    int phy,
    int *ethu_id_in_core,
    int *lane);

/**
 * \brief -
 *  Get lane info from first phy id.
 *
 * \param [in] unit - unit #.
 * \param [in] first_phy - first phy
 * \param [out] first_lane_in_ethu - first lane id in Ethu
 * \param [out] nof_lanes_in_ethu - nof lanes in Ethu
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_phy_ethu_lane_info_get(
    int unit,
    int first_phy,
    int *first_lane_in_ethu,
    int *nof_lanes_in_ethu);

/**
 * \brief -
 *  Get PM info from phy id on this PM
 *
 * \param [in] unit - unit #.
 * \param [in] phy_id - PHY id
 * \param [out] pm_info - returned info about PM
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_phy_pm_info_get(
    int unit,
    int phy_id,
    dnx_algo_pm_info_t * pm_info);

/**
 * \brief -
 *  Map Nif Port to Nif unit id and internal offset inside the nif unit .
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - nif port
 * \param [out] nif_unit_id - NIF unit id
 * \param [out] internal_offset - internal offset inside the nif unit
 *
 * \return
 *   See shr_error_e
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_unit_id_get(
    int unit,
    bcm_port_t logical_port,
    int *nif_unit_id,
    int *internal_offset);
 /**
 * \brief - get all ILKN active IDs. Each set bit in active_ilkn_ids represent active ILKN ID.
 *
 * \param [in] unit - chip unit id.
 * \param [in] flags - used to specify which ILKN IDs should be returned. Flags parameter valid options are:
 *  DNX_ALGO_PORT_ILKN_ID_GET_ILKN_DATA_ONLY
 *  DNX_ALGO_PORT_ILKN_ID_GET_ILKN_ELK_ONLY
 *  DNX_ALGO_PORT_ILKN_ID_GET_ALL
 * \param [out] active_ilkn_ids - ILKN active IDs. Each set bit in active_ilkn_ids represent active ILKN ID.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_ilkn_active_ids_get(
    int unit,
    uint32 flags,
    uint32 *active_ilkn_ids);
 /**
 * \brief - get all ilkn ports on the same core
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] ilkn_ports - bitmap of ports on the same core
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_ilkn_ports_in_core_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * ilkn_ports);

/**
 * \brief - get number of ilkn ports on the same core
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] ilkn_nof_ports - number of ports on the same core
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_algo_port_ilkn_nof_ports_get(
    int unit,
    bcm_port_t port,
    int *ilkn_nof_ports);

/**
 * \brief - get number of prd ports profiles for the given port
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] nof_profiles - number of port profiles if PRD port-to-profile-map is enabled for this port.
 *                             Error if port-to-profile-map is not enabled
 *
 * \return
 *   int - Return Error if port-to-profile-map is not enabled for the given port
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_prd_nof_profiles_get(
    int unit,
    bcm_port_t port,
    int *nof_profiles);

/**
 * \brief - get the prd profile for the given port
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] prd_profile - prd profile id if PRD port-to-profile-map is enabled for this port.
 *                            internal lane id in case port-to-profile-map is not enabled
 *
 * \return
 *   int - Return Error if port-to-profile-map is not enabled for the given port
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_prd_profile_get(
    int unit,
    bcm_port_t port,
    uint32 *prd_profile);

/**
 * \brief - Get HRF size for ILKN port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] hrf_size - hrf size
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e imb_ilu_prd_hrf_size_get(
    int unit,
    bcm_port_t port,
    int *hrf_size);

/**
 * \brief - Map IMB type to generic PM dispatch type
 *
 * \param [in] unit - chip unit id.
 * \param [in] imb_type - IMB dispatch type
 * \param [in] is_over_fabric - is over fabric
 * \param [out] pm_type -PM dispatch type
 *
 * \return
 *   int - Return Error if port-to-profile-map is not enabled for the given port
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_imb_type_to_pm_type_get(
    int unit,
    int imb_type,
    int is_over_fabric,
    int *pm_type);

/**
 * \brief - Indicate if port supports precoder feature
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] support - 0:no support, 1:support
 *
 * \return
 *   int - Return Error if port-to-profile-map is not enabled for the given port
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_is_precoder_supported_get(
    int unit,
    bcm_port_t port,
    int *support);

/**
 * \brief - Returns all the configured ports on the provided absolute mac mode config id
 * (NOT the internal to the PM but the MAC ID of the device)
 *
 * \param [in] unit - chip unit id.
 * \param [in] mac_mode_config_id - mac mode config id of the device
 * \param [out] ports -pbmp of ports configured on the relevant mac
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_mac_mode_config_id_ports_get(
    int unit,
    int mac_mode_config_id,
    bcm_pbmp_t * ports);

#endif /* !ALGO_PORT_UTILS_H_INCLUDED */
