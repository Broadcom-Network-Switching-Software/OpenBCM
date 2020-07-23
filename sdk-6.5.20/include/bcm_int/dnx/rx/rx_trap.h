/**
 *  \file bcm_int/dnx/rx/rx_trap.h
 * 
 *
 * Internal DNX RX APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RX_TRAP_H_INCLUDED
/* { */
#define RX_TRAP_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <include/bcm_int/dnx/rx/rx.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_snif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>

/* } */

/*
  * Define and MACROs
  * {
  */

/**
 * Number of Ingress traps
 */
#define DNX_RX_TRAP_IS_LEGAL_INGRESS_TRAP_ID(trap_id) \
if(trap_id > DBAL_NOF_ENUM_INGRESS_TRAP_ID_VALUES || trap_id < 0) \
{ \
   SHR_ERR_EXIT(_SHR_E_PARAM, "The supplied Trap_ID(%d) is out of legal range 0 - %d \n", trap_id, DBAL_NOF_ENUM_INGRESS_TRAP_ID_VALUES); \
} \

/**
 * MTU Traps for PORT config, use hard-coded forwarding type 3'b1;
 */
#define DNX_RX_MTU_PORT_COMPRESSED_LAYER_TYPE       (0x7)

/**
 * MTU Default profile;
 */
#define DNX_RX_TRAP_MTU_DEFAULT_PROFILE                 (0x0)

/**
 * ERPP first configurable fwd action profile
 */
#define DNX_RX_TRAP_ERPP_FWD_ACT_PROFILE_1ST_USER_TRAP    (1)

/**
 * ERPP last configurable fwd action profile
 */
#define DNX_RX_TRAP_ERPP_FWD_ACT_PROFILE_LAST_USER_TRAP   (7)

/**
 * \brief - Trap BLOCK and CLASS are going to be encoded in trap_id
 *          Trap_class will be either UserDefined or OAM , 0/1 respectively.
 * (2b - trap_block | 1b -trap_class | 9b - trap_type) */
#define DNX_RX_TRAP_BLOCK_SHIFT     (10)
#define DNX_RX_TRAP_BLOCK_MASK      (0x3)

#define DNX_RX_TRAP_CLASS_SHIFT        (9)
#define DNX_RX_TRAP_CLASS_MASK      (0x1)

#define DNX_RX_TRAP_TYPE_MASK       (0x1FF)

#define DNX_RX_TRAP_ID_SET(trap_block, trap_class, trap_type)   ((trap_block << DNX_RX_TRAP_BLOCK_SHIFT)    \
                                                               | (trap_class << DNX_RX_TRAP_CLASS_SHIFT)    \
                                                               | (trap_type))

#define DNX_RX_TRAP_ID_BLOCK_GET(trap_id)   ((trap_id >> DNX_RX_TRAP_BLOCK_SHIFT) & DNX_RX_TRAP_BLOCK_MASK)
#define DNX_RX_TRAP_ID_CLASS_GET(trap_id)   ((trap_id >> DNX_RX_TRAP_CLASS_SHIFT) & DNX_RX_TRAP_CLASS_MASK)
#define DNX_RX_TRAP_ID_TYPE_GET(trap_id)    (trap_id & DNX_RX_TRAP_TYPE_MASK)

/**
 * \brief - Get trap_action_profile (3b) from egress trap_id
 */
#define DNX_RX_TRAP_EGRESS_ACTION_PROFILE_MASK (0x7)

#define DNX_RX_TRAP_EGRESS_ACTION_PROFILE_GET(trap_id)      (trap_id & DNX_RX_TRAP_EGRESS_ACTION_PROFILE_MASK)
/**
 * \brief - Get snoop_action_profile (2b) from egress trap_id
 */
#define DNX_RX_TRAP_EGRESS_SNOOP_PROFILE_MASK (0x3)

#define DNX_RX_TRAP_EGRESS_SNOOP_PROFILE_GET(trap_id)      (trap_id & DNX_RX_TRAP_EGRESS_SNOOP_PROFILE_MASK)

/**
 * \brief - Action Profile, and OAM Trap Type are going to be encoded in trap_type, which is then encoded into trap_id
 *          Currently the OAM trap_type is 3 bits, but we will reserve additional 3 for future use,
 *           which means that the 3 LSB hold the oam_trap_id, the following 3 are empty buffer for future use and
 *           3 MSB for action_profile.
 * (3b - action_profile | 6b - oam_trap_type(3b empty buffer | 3b oam_trap_id)) */

#define DNX_RX_TRAP_ETPP_OAM_ACTION_PROFILE_SHIFT    (6)
#define DNX_RX_TRAP_ETPP_OAM_ACTION_PROFILE_MASK     (0x7)

#define DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_MASK          (0x3F)

#define DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_SET(action_profile, oam_trap_type)     ((action_profile << DNX_RX_TRAP_ETPP_OAM_ACTION_PROFILE_SHIFT) \
                                                                            | (oam_trap_type))

#define DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_GET(trap_type)         (trap_type & DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_MASK)
#define DNX_RX_TRAP_ETPP_OAM_ACTION_PROFILE_GET(trap_type)    ((trap_type >> DNX_RX_TRAP_ETPP_OAM_ACTION_PROFILE_SHIFT) & DNX_RX_TRAP_ETPP_OAM_ACTION_PROFILE_MASK)

#define DNX_RX_TRAP_ID_INVALID       (-1)

/**
 * \brief - 3b strength is encoded in TM profile (2b) and forward strength (1b)
 * strength (3b) = (2b - TM profile | 1b - forward strength) */
#define DNX_RX_TRAP_FWD_COMPRESSED_STRENGTH_SHIFT     (2)
#define DNX_RX_TRAP_FWD_COMPRESSED_STRENGTH_MASK      (0x1)

#define DNX_RX_TRAP_TM_PROFILE_MASK      (0x3)

#define DNX_RX_TRAP_FWD_COMPRESSED_STRENGTH_GET(strength)   ((strength >> DNX_RX_TRAP_FWD_COMPRESSED_STRENGTH_SHIFT) \
                                                                        & DNX_RX_TRAP_FWD_COMPRESSED_STRENGTH_MASK)
#define DNX_RX_TRAP_TM_PROFILE_GET(strength)                (strength & DNX_RX_TRAP_TM_PROFILE_MASK)

/**
*STAT_OBJ_CLEAR field in the trap FWD_ACT_PROFILE table should be set at init to all 1's
* Since 1 is the default value for - "do nothing"
*/
#define DNX_RX_TRAP_ACTION_STAT_OBJ_CLEAR_DEFAULT (0x3FF)

/**
 * \brief - Snoop command defines
 * When PMF handle ITMH one of the fields is snoop command (5 bits).
 * In PMF a tranlation to snoop code (9 bits) is being done which in action resolution will be used to access HW.
 * DNX_RX_TRAP_CODE_SNIF_PROFILE_START_IND - start index of sniff in dabl table, finds the first available index
 * sniff profile is used for snooping, mirroring and sampling*/
#define DNX_RX_TRAP_CODE_SNIF_PROFILE_START_IND(unit) (dnx_data_trap.ingress.nof_entries_action_table_get(unit) \
                                                       - dnx_data_snif.ingress.nof_profiles_get(unit))

/**
 * \brief - check if a parameter is bool indication
 */
#define DNX_RX_TRAP_IS_BOOL_INDICATION(param)   ((param == TRUE) | (param == FALSE))

/**
 * \brief - check if ingress trap is DA NOT FOUND
 */
#define DNX_RX_TRAP_IS_DA_NOT_FOUND(trap_id)    ((trap_id == DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_DA_NOT_FOUND0) || \
                                                 (trap_id == DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_DA_NOT_FOUND1) || \
                                                 (trap_id == DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_DA_NOT_FOUND2) || \
                                                 (trap_id == DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_DA_NOT_FOUND3))

/**
 * \brief - check if etpp is user trap based on profile
 */
#define DNX_RX_TRAP_IS_ETPP_USER_TRAP(profile)   ((profile >= DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_1) &&  \
                                                  (profile <= DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_4))

/**
 * \brief - check if etpp oam trap is of UP-MEP type
 */
#define DNX_RX_TRAP_IS_ETPP_OAM_UP_MEP(trap_type)   ((trap_type == DNX_RX_TRAP_ETPP_OAM_UP_MEP_OAMP)    || \
                                                     (trap_type == DNX_RX_TRAP_ETPP_OAM_UP_MEP_DEST1)   || \
                                                     (trap_type == DNX_RX_TRAP_ETPP_OAM_UP_MEP_DEST2))
/**
 * \brief - check if etpp oam trap is of error type
 */
#define DNX_RX_TRAP_IS_ETPP_OAM_ERR(trap_type)   ((trap_type == DNX_RX_TRAP_ETPP_OAM_PASSIVE_ERR)    || \
                                                  (trap_type == DNX_RX_TRAP_ETPP_OAM_LEVEL_ERR))

/**
 * \brief - check if etpp is user trap based on profile
 */
#define DNX_RX_TRAP_BLOCK_IS_VALID(trap_block)   ((trap_block > DNX_RX_TRAP_BLOCK_INVALID) &&  \
                                                  (trap_block < DNX_RX_TRAP_BLOCK_NUM_OF))

/**
* Protocol traps default profiles
*/
#define ICMPV6_TRAP_PROFILE_DEFAULT (0)
#define ICMPV4_TRAP_PROFILE_DEFAULT (0)
#define L2CP_TRAP_PROFILE_DEFAULT (0)
#define ARP_TRAP_PROFILE_DEFAULT (0)
#define IGMP_TRAP_PROFILE_DEFAULT (0)
#define DHCP_TRAP_PROFILE_DEFAULT (0)
#define NON_AUTH_8021X_TRAP_PROFILE_DEFAULT (0)

/**
* Number of protocol trap arguments
*/
#define DNX_RX_TRAP_NOF_L2CP_PROTOCOL_TRAP_ARGS (64)
#define DNX_RX_TRAP_NOF_ICMP_PROTOCOL_TRAP_ARGS (256)

/**
* ICMP-NDP Neighbor Solicitation request is identified when the packet is ICMPoIPv6 and the ICMP type is 135
*/
#define DNX_RX_TRAP_NEIGHBOR_SOLICITATION_TYPE  (135)

/*
  * }
  */

/*
  * Types
  * {
  */

/**
 * \brief enum to hold the poissble blocks for traps.
 */
typedef enum
{
    DNX_RX_TRAP_BLOCK_INVALID = -1,
    DNX_RX_TRAP_BLOCK_FIRST = 0,
    /**
        * bcmRxTrapType.. is one of the ingress traps - predefined,user_defined.
        */
    DNX_RX_TRAP_BLOCK_INGRESS = DNX_RX_TRAP_BLOCK_FIRST,        /* Must be zero since ingress trap_id is used as index */
    /**
        *bcmRxTrapType.. is one of the OAMP traps - OAMP block
        */
    DNX_RX_TRAP_BLOCK_OAMP,
    /**
        *bcmRxTrapType.. is one of the egress traps - ERPP block
        */
    DNX_RX_TRAP_BLOCK_ERPP,
    /**
        *bcmRxTrapType.. is one of the egress traps - ETPP block
        */
    DNX_RX_TRAP_BLOCK_ETPP,
    DNX_RX_TRAP_BLOCK_NUM_OF
} dnx_rx_trap_block_e;

/**
 * \brief enum to hold the poissble classes for traps.
 */
typedef enum
{
    DNX_RX_TRAP_CLASS_INVALID = -1,
    DNX_RX_TRAP_CLASS_FIRST = 0,
    /**
    * bcmRxTrapType.. is one of user defined traps
    */
    DNX_RX_TRAP_CLASS_USER_DEFINED = DNX_RX_TRAP_CLASS_FIRST,   /* Must be zero since predefined trap_id is used as */
    /**
    * bcmRxTrapType.. is one of OAM traps
    */
    DNX_RX_TRAP_CLASS_OAM,
    /**
    * bcmRxTrapType.. is one of the predefined traps
    */
    DNX_RX_TRAP_CLASS_PREDEFINED,
    DNX_RX_TRAP_CLASS_NUM_OF
} dnx_rx_trap_class_e;

/**
 * \brief enum to hold possible same interface context profiles
 */
typedef enum
{
    DNX_RX_TRAP_SAME_INTERFACE_CONTEXT_PROFILE_DEFAULT = 0,
    DNX_RX_TRAP_SAME_INTERFACE_CONTEXT_PROFILE_IP = 1,
    DNX_RX_TRAP_NOF_SAME_INTERFACE_CONTEXT_PROFILES
} dnx_rx_trap_same_interface_context_profile_e;

/**
 * \brief enum to hold the possible stat objects to overwrite
 */
typedef enum
{
    DNX_RX_TRAP_STAT_OBJ_TO_OW_0 = 0,
    DNX_RX_TRAP_STAT_OBJ_TO_OW_1 = 1,
    DNX_RX_TRAP_NOF_STAT_OBJS_TO_OW
} dnx_rx_trap_stat_obj_to_ow_e;

/**
 * \brief enum to hold the possible protocol trap configuration modes.
 */
typedef enum
{
    DNX_RX_TRAP_PROTOCOL_MODE_INVALID = -1,
    DNX_RX_TRAP_PROTOCOL_MODE_FIRST = 0,
    /**
    * Enabling the device to look-up protocol profiles per in-lif
    */
    DNX_RX_TRAP_PROTOCOL_MODE_IN_LIF = DNX_RX_TRAP_PROTOCOL_MODE_FIRST,
    /**
    * Enabling the device to look-up protocol profiles per in-port
    */
    DNX_RX_TRAP_PROTOCOL_MODE_IN_PORT,
    DNX_RX_TRAP_PROTOCOL_MODE_NUM_OF
} dnx_rx_trap_protocol_mode_e;

/**
 * \brief enum for protocol DHCP trap arguments.
 */
typedef enum
{
    DNX_RX_TRAP_PROTOCOL_DHCP_ARGS_IPV4_CLIENT = 0,
    DNX_RX_TRAP_PROTOCOL_DHCP_ARGS_IPV4_SERVER = 1,
    DNX_RX_TRAP_PROTOCOL_DHCP_ARGS_IPV6_CLIENT = 2,
    DNX_RX_TRAP_PROTOCOL_DHCP_ARGS_IPV6_SERVER = 3
} dnx_rx_trap_protocol_dhcp_args_e;

/**
 * \brief enum for protocol ARP trap arguments.
 */
typedef enum
{
    DNX_RX_TRAP_PROTOCOL_ARP_ARGS = 0,
    DNX_RX_TRAP_PROTOCOL_MY_ARP_ARGS = 1
} dnx_rx_trap_protocol_arp_args_e;

/**
 * \brief enum for protocol NDP trap arguments.
 */
typedef enum
{
    DNX_RX_TRAP_PROTOCOL_NDP_ARGS = 0,
    DNX_RX_TRAP_PROTOCOL_MY_NDP_ARGS = 1
} dnx_rx_trap_protocol_ndp_args_e;

/**
 * \brief enum for Ethernet trap action profiles (routing enable procedure).
 */
typedef enum
{
    DNX_RX_TRAP_ETH_PROFILE_DEFAULT = 0,
    DNX_RX_TRAP_ETH_PROFILE_VTT_MY_MAC_AND_IP_DISABLE = 1,
    DNX_RX_TRAP_ETH_PROFILE_VTT_MY_MAC_AND_MPLS_DISABLE = 2,
    DNX_RX_TRAP_ETH_PROFILE_VTT_MY_MAC_AND_ARP = 3,
    DNX_RX_TRAP_ETH_PROFILE_VTT_MY_MAC_AND_UNKNOWN_L3 = 4
} dnx_rx_trap_eth_profiles_e;

/**
 * \brief enum for etpp forward action profiles.
 */
typedef enum
{
    DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_DO_NOTHING = 0,
    DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_DROP = 1,
    DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_GENERAL_TRAP = 2,
    DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_1 = 3,
    DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_2 = 4,
    DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_3 = 5,
    DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_4 = 6,
    DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_VISIBILITY = 7,
    DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_KEEP_FABRIC_PACKET = 8,
    DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_5,
    DNX_RX_TRAP_ETPP_FWD_ACT_NOF_PROFILES = 10
} dnx_rx_trap_etpp_fwd_act_profiles_e;

/**
 * \brief enum for forward recycle profiles.
 */
typedef enum
{
    DNX_RX_TRAP_FWD_RCY_PROFILE_DEFAULT = 0,
    DNX_RX_TRAP_FWD_RCY_PROFILE_EGRESS_TRAPPED = 1,
    DNX_RX_TRAP_FWD_RCY_PROFILE_OAM_UPMEP_REFLECTOR = 2,
    DNX_RX_TRAP_FWD_RCY_PROFILE_RESERVED_1 = 3,
    DNX_RX_TRAP_FWD_RCY_PROFILE_RESERVED_2 = 4,
    DNX_RX_TRAP_FWD_RCY_PROFILE_RESERVED_3 = 5,
    DNX_RX_TRAP_FWD_RCY_PROFILE_RESERVED_4 = 6,
    DNX_RX_TRAP_FWD_RCY_PROFILE_CONFIGURABLE_1 = 7,
    DNX_RX_TRAP_FWD_RCY_PROFILE_CONFIGURABLE_2 = 8,
    DNX_RX_TRAP_FWD_RCY_PROFILE_CONFIGURABLE_3 = 9,
    DNX_RX_TRAP_FWD_RCY_PROFILE_CONFIGURABLE_4 = 10,
    DNX_RX_TRAP_FWD_RCY_PROFILE_CONFIGURABLE_5 = 11,
    DNX_RX_TRAP_FWD_RCY_PROFILE_CONFIGURABLE_6 = 12,
    DNX_RX_TRAP_FWD_RCY_PROFILE_CONFIGURABLE_7 = 13,
    DNX_RX_TRAP_FWD_RCY_PROFILE_CONFIGURABLE_8 = 14,
    DNX_RX_TRAP_FWD_RCY_PROFILE_DROP = 15,
    DNX_RX_TRAP_FWD_RCY_NOF_PROFILES = 16
} dnx_rx_trap_fwd_rcy_profiles_e;

/**
 * \brief enum for lif profiles.
 */
typedef enum
{
    DNX_RX_TRAP_LIF_PROFILE_INVALID = -1,
    DNX_RX_TRAP_LIF_PROFILE_DEFAULT = 0,
    DNX_RX_TRAP_LIF_PROFILE_STACK_TRAP_1 = 1,
    DNX_RX_TRAP_LIF_PROFILE_STACK_TRAP_2 = 2,
    DNX_RX_TRAP_LIF_PROFILE_STACK_MIRROR = 3
} dnx_rx_trap_lif_profiles_e;

/**
 * \brief enum for etpp oam trap types.
 */
typedef enum
{
    DNX_RX_TRAP_ETPP_OAM_INVALID = -1,
    DNX_RX_TRAP_ETPP_OAM_FIRST = 0,

    /** OAM ETPP trap with destination OAMP*/
    DNX_RX_TRAP_ETPP_OAM_UP_MEP_OAMP = DNX_RX_TRAP_ETPP_OAM_FIRST,

    /** OAM ETPP trap with user configurable destination*/
    DNX_RX_TRAP_ETPP_OAM_UP_MEP_DEST1 = 1,

    /** OAM ETPP trap with user configurable destination*/
    DNX_RX_TRAP_ETPP_OAM_UP_MEP_DEST2 = 2,

    /** OAM ETPP level error trap*/
    DNX_RX_TRAP_ETPP_OAM_LEVEL_ERR = 3,

    /** OAM ETPP passive error trap*/
    DNX_RX_TRAP_ETPP_OAM_PASSIVE_ERR = 4,

    /** OAM ETPP reflector trap*/
    DNX_RX_TRAP_ETPP_OAM_REFLECTOR = 5,
    /** IFA ETPP trap*/
    DNX_RX_TRAP_ETPP_IFA2 = 6,
    DNX_RX_TRAP_ETPP_OAM_NOF_TRAPS
} dnx_rx_trap_etpp_oam_types_e;

typedef struct dnx_rx_trap_protocol_config_s
{
    /** trap id */
    int trap_id;

    /** forward strength 0-16 */
    int fwd_strength;

    /** snoop strength 0-7 */
    int snp_strength;
} dnx_rx_trap_protocol_config_t;

typedef struct dnx_rx_trap_protocol_my_arp_ndp_config_s
{
    /** two ip addresses defineing My-ARP/My-NDP trap */
    uint32 ip_1;
    uint32 ip_2;

    /** relevent yo My-ARP only */
    uint8 ignore_da;
} dnx_rx_trap_protocol_my_arp_ndp_config_t;

typedef struct dnx_rx_trap_action_profile_s
{
    /** action profile 0-7 */
    int profile;

    /** forward strength 0-15 */
    int fwd_strength;

    /** snoop strength 0-7 */
    int snp_strength;
} dnx_rx_trap_action_profile_t;

/*
  * }
  */

/**
* \brief
*  Check if specific trap was already allocated
* \par DIRECT INPUT:
*   \param [in] unit            -  Unit Id
*   \param [in] trap_id         -  trap id to check
*   \param [in] is_allocated_p  - Pointer type of int \n
*               \b As \b output - \n
*               Pointer will be filled by TRUE in case the trap was already created
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_is_trap_id_allocated(
    int unit,
    int trap_id,
    uint8 *is_allocated_p);

/**
* \brief
*  Returns the ingress class (predefined, oam, user define) of trap based on trap type value
* \par DIRECT INPUT:
*   \param [in] unit       -   Unit Id
*   \param [in] trap_type    -  trap type value to check
*   \param [out] trap_class_p  -  Pointer to a memory type of dnx_rx_trap_class_e. \n
*           \b As \b output - \n
*           contains the trap class.
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_ingress_class_get(
    int unit,
    int trap_type,
    dnx_rx_trap_class_e * trap_class_p);

/**
* \brief
*  Retrun a list of trap types supported per block and class
* \param [in] unit              - Device ID
* \param [in] trap_block        - Stage to get trap ID for
* \param [in] trap_class        -  Trap class to get Id of
* \param [out] trap_type_list_p - List of trap ids
* \param [out] trap_type_nof_p  - number of trap ids in the list
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_type_list(
    int unit,
    dnx_rx_trap_block_e trap_block,
    dnx_rx_trap_class_e trap_class,
    uint32 **trap_type_list_p,
    int *trap_type_nof_p);

/**
* \brief
*  Retrun trap name(string) of trap type
* \param [in] unit            - Device ID
* \param [in] trap_type     - Trap type to get its name
* \param [out] trap_name  - Trap name
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_type_to_name(
    int unit,
    bcm_rx_trap_t trap_type,
    char *trap_name);

/**
* \brief
* Initialize error (invalid_dest and SER) handling configuration:
* Configure destination to Redirect to cpu
* Stamp given trap code
* \param [in] unit  - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_ingress_err_handling_init(
    int unit);

/**
 * \brief -
 *   Initialize SER action profile
 *
 * \param [in] unit - Unit ID
 * \param [in] trap_id - Trap id
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 * \see
 *  None.
 */
shr_error_e dnx_rx_trap_erpp_ser_handling_init(
    int unit,
    int trap_id);

/**
* \brief
*  Set additional configuration of My-NDP protocol trap.
* \par DIRECT INPUT:
*   \param [in] unit        - unit id
*   \param [in] word_index  - NDP IPv6 32b piece index
*   \param [in] config_p    - My-ARP/My-NDP protocol traps additional configuration
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_protocol_my_ndp_set(
    int unit,
    int word_index,
    dnx_rx_trap_protocol_my_arp_ndp_config_t * config_p);

/**
* \brief
*  Set additional configuration of My-ARP protocol trap.
* \par DIRECT INPUT:
*   \param [in] unit      - unit id
*   \param [in] config_p  - My-ARP/My-NDP protocol traps additional configuration
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_protocol_my_arp_set(
    int unit,
    dnx_rx_trap_protocol_my_arp_ndp_config_t * config_p);
/**
* \brief
* Get additional configuration of My-NDP protocol trap.
* \par DIRECT INPUT:
*   \param [in] unit        -  unit id
*   \param [in] word_index  - NDP IPv6 32b piece index
*   \param [out] config_p   -   My-ARP/My-NDP protocol traps additional configuration
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_protocol_my_ndp_get(
    int unit,
    int word_index,
    dnx_rx_trap_protocol_my_arp_ndp_config_t * config_p);

/**
* \brief
* Get additional configuration of My-ARP protocol trap.
* \par DIRECT INPUT:
*   \param [in] unit           -  unit id
*   \param [out] config_p   -   My-ARP/My-NDP protocol traps additional configuration
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_protocol_my_arp_get(
    int unit,
    dnx_rx_trap_protocol_my_arp_ndp_config_t * config_p);

/**
* \brief
*  This function enables or disables the Ignore DA ICMP filter field,
*  based on the enable field value.
*
* \par DIRECT INPUT:
*   \param [in] unit    - Unit Id
*   \param [in] enable  - Enable/Disable for ICMP Ignore DA
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_protocol_icmp_ignore_da_hw_set(
    int unit,
    int enable);

/**
* \brief
*  This function gets the value of the Ignore DA ICMP filter field.
*
* \par DIRECT INPUT:
*   \param [in] unit     - Unit Id
*   \param [out] enable  - Enable/Disable for ICMP Ignore DA
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_protocol_icmp_ignore_da_hw_get(
    int unit,
    int *enable);

/**
* \brief
*  Sets the Compressed ICMP type in the ICMP type compression table.
* \par DIRECT INPUT:
*   \param [in] unit                    - unit Id
*   \param [in] icmp_type               - ICMP type
*   \param [out] compressed_icmp_type    - Compressed ICMP type
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_icmp_compressed_type_hw_get(
    int unit,
    int icmp_type,
    uint32 *compressed_icmp_type);

/**
* \brief
*  Retrieves the MTU profile data which has been set for the allocated context.
* \par DIRECT INPUT:
*   \param [in] unit                    - unit Id
*   \param [in] mtu_profile             - MTU Profile
*   \param [out] mtu_profile_config_p   - MTU Profile Config data
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_mtu_profile_data_get(
    int unit,
    int mtu_profile,
    dnx_rx_trap_mtu_profile_config_t * mtu_profile_config_p);

/**
* \brief
*  Check if the supplied ingress trap_type(ID) requires a trap strength to be set.
* \par DIRECT INPUT:
*   \param [in] unit              -  Unit Id
*   \param [in] trap_type         -  Ingress trap type (HW trap id value) to configure
*   \param [out] is_strengthless  -  Strengthless trap indicator.
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_ingress_strengthless_trap_check(
    int unit,
    int trap_type,
    int *is_strengthless);

/**
* \brief
*  This function allocates a recycle command according to given configuration.
* \par DIRECT INPUT:
*   \param [in] unit      -  unit id
*   \param [in] old_recycle_cmd  -old recycle command configured for trap (4b)
*   \param [in] recycle_cmd_config_p      -  recycle command configuration
*   \param [in] new_recycle_cmd_p      -  new recycle command allocated
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_etpp_recycle_cmd_profile_set(
    int unit,
    uint8 old_recycle_cmd,
    dnx_rx_trap_recycle_cmd_config_t * recycle_cmd_config_p,
    int *new_recycle_cmd_p);

/**
* \brief
*  This function gets the recycle command from HW .
* \par DIRECT INPUT:
*   \param [in] unit      -  unit id
*   \param [in] core_id      -  core id
*   \param [in] recycle_cmd  - recycle command (4b)
*   \param [out] recycle_cmd_config      -  recycle command configuration: trap_id and strength
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_etpp_recycle_cmd_hw_get(
    int unit,
    uint32 core_id,
    uint8 recycle_cmd,
    dnx_rx_trap_recycle_cmd_config_t * recycle_cmd_config);

/**
* \brief
*  Allocate and Set the ETPP LIF trap Profile (Trap or Mirror)
* \par DIRECT INPUT:
*   \param [in] unit        - unit Id
*   \param [in] action_gport - action gport encoding Trap-Action-Profile
*   \param [in] old_lif_trap_profile - old trap action profile from the lif
*   \param [out] new_lif_trap_profile_p - - new trap action profile from the lif
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_lif_etpp_trap_profile_allocate_and_hw_update(
    int unit,
    uint32 old_lif_trap_profile,
    bcm_gport_t action_gport,
    uint32 *new_lif_trap_profile_p);

#endif/*_RX_API_INCLUDED__*/
