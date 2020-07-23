/**
 *  \file bcm_int/dnx/rx/rx.h
 * 
 *  Internal DNX RX APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _RX_API_INCLUDED__
/*
 * {
 */
#define _RX_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <include/bcm/rx.h>
#include <include/bcm_int/dnx/algo/rx/algo_rx.h>
#include <include/bcm_int/dnx/algo/algo_gpm.h>
#include <shared/shrextend/shrextend_error.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/bslenum.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>

#define DNX_RX_ERPP_PMF_COUNTER_NOF                       (0x2)
#define DNX_RX_MTU_PROFILE_LIF_COMPRESSED_LAYER_TYPES     (0x7)

/**
 * snoop command reserved for snoop all packet to CPU
 */
#define DNX_RX_SNOOP_CMD_TO_CPU (1)

/**
  *\brief - data of a ERPP trap action
  * Holds all ERPP trap actions configured by the user using
  * dnx_rx_trap_set()
 */
typedef struct dnx_rx_trap_erpp_config_s
{
    /** when set traffic class is valid */
    uint8 tc_valid;

    /** traffic class value */
    uint8 tc;

    /** when set, drop presedence is valid */
    uint8 dp_valid;

    /**  drop presedence value */
    uint8 dp;

    /** when set, quality of service is valid */
    uint8 qos_id_valid;

    /** quality of service value */
    uint8 qos_id;

    /** when set, PP DSP is valid */
    uint8 pp_dsp_valid;

    /** PP DSP value */
    uint32 pp_dsp;

    /** when set, packet is discarded */
    uint8 discard;

    /** When set, PP drop reason is valid*/
    uint8 pp_drop_reason_valid;

    /** PP drop reason value */
    int pp_drop_reason;

    /** When Snoop profile is valid*/
    uint8 snoop_profile_valid;

    /** Snoop profile */
    uint8 snoop_profile;

    /** When Mirror profile is valid*/
    uint8 mirror_profile_valid;

    /** Mirror profile */
    uint8 mirror_profile;

    /** When CUD Outlif_or_MCDB pointer is valid*/
    uint8 outlif_or_mcdb_ptr_valid;

    /** CUD Outlif_or_MCDB pointer */
    uint32 outlif_or_mcdb_ptr;

    /** When Counter_profile is valid*/
    uint8 counter_profile_valid[DNX_RX_ERPP_PMF_COUNTER_NOF];

    /** Counter_profile */
    uint8 counter_profile[DNX_RX_ERPP_PMF_COUNTER_NOF];

     /** When Counter_ptr is valid*/
    uint8 counter_ptr_valid[DNX_RX_ERPP_PMF_COUNTER_NOF];

    /** Counter_ptr */
    uint32 counter_ptr[DNX_RX_ERPP_PMF_COUNTER_NOF];

} dnx_rx_trap_erpp_config_t;

/**
 * \brief ETPP trap action data for template manager.
 */
typedef struct dnx_rx_trap_etpp_action_s
{
    /** When set packet is dropped */
    uint8 is_pkt_drop;

    /** When set, recycling packet is in high priority */
    uint8 is_recycle_high_priority;

    /** When set, only the first 128B of the Forward packet are copied to the recycle port */
    uint8 is_recycle_crop_pkt;

    /** When set, Aligner appends the original FTMH to the Recycle Packet */
    uint8 is_recycle_append_ftmh;

    /** forward recycle command. this field is used for providing additional information to IRPP */
    uint8 recycle_cmd;

    /** ingress trap id stamped on FHEI */
    dbal_enum_value_field_ingress_trap_id_e ingress_trap_id;

    /** Packet processing drop reason */
    int pp_drop_reason;

    /** When set recycle copy is dropped */
    uint8 is_rcy_copy_drop;

    /** When set profile is configured with keep original packet VR fields */
    uint8 is_pkt_keep_fabric_headers;
} dnx_rx_trap_etpp_action_t;

/**
 * \brief recycle trap configuration.
 */
typedef struct dnx_rx_trap_recycle_cmd_config_s
{
    /** forward recycle strength, used for providing the strength of recycle ingress trap */
    uint8 recycle_strength;

    /** ingress trap id to indicate the recycle ingress trap */
    dbal_enum_value_field_ingress_trap_id_e ingress_trap_id;
} dnx_rx_trap_recycle_cmd_config_t;

/**
 * \brief ETPP trap action profile configuration.
 */
typedef struct dnx_rx_trap_etpp_config_s
{
    /** ETPP trap action */
    dnx_rx_trap_etpp_action_t action;

    /** Recycle command configuration */
    dnx_rx_trap_recycle_cmd_config_t recycle_cmd_config;
} dnx_rx_trap_etpp_config_t;

/**
 * \brief MTU Profile configuration for Template manager.
 */

typedef struct dnx_rx_trap_mtu_data_s
{
    /** MTU value */
    int mtu;

    /** Forward trap strength value */
    uint8 fwd_strength;

    /** Snoop trap strength value */
    uint8 snp_strength;

    /** Trap Action profile */
    uint8 trap_action_profile;

} dnx_rx_trap_mtu_data_t;

typedef struct dnx_rx_trap_mtu_profile_config_s
{
    dnx_rx_trap_mtu_data_t mtu_data[DNX_RX_MTU_PROFILE_LIF_COMPRESSED_LAYER_TYPES];

} dnx_rx_trap_mtu_profile_config_t;

/**
 * \brief defination of DNX header ihformation
 */
typedef struct dnx_rx_parse_headers_s
{
    /** FTMH.TM-Action-Type */
    bcm_pkt_dnx_ftmh_action_type_t action_type;
    /** Headers stack */
    bcm_pkt_dnx_t dnx_header_stack[BCM_PKT_NOF_DNX_HEADERS];
    /** Number of DNX headers */
    uint8 dnx_header_count;
    /** Length of total headers */
    uint32 header_length;
    /** TRUE: then packet is trapped to CPU and trap-code/qualifier are valid */
    uint8 is_trapped;
    /** TRUE: packet was punted to CPU by OAMP */
    uint8 is_punted;
    /** FTMH.TM-Action-Is-MC */
    uint8 is_mc_traffic;
    /** INTERNAL.FHEI.Code */
    uint32 cpu_trap_code;
    /** INTERNAL.FHEI.Qualifier */
    uint32 cpu_trap_qualifier;
    /** FTMH.Source-System-Port-Aggregate */
    uint32 src_sys_port;
    /** INTERNAL.Forward-Domain */
    uint32 internal_inlif_inrif;
    /** FTMH.PP_DSP */
    uint32 otm_port;
    /** FTMH.Out-LIF (or MC-ID) */
    uint32 outlif_mcid;
    /** FTMH LB-Key Extension */
    uint32 lb_key;
    /** FTMH DSP Extension */
    uint32 dsp_ext;
    /** FTMH stacking Extension */
    uint32 stacking_ext;
    /** FTMH.TC */
    uint32 tc;
    /** FTMH.DP */
    uint32 dp;
    /** INTERNAL.Unknow_addr */
    uint8 unknown_addr;
    /** INTERNAL.fwd_code in JR mode */
    uint32 jr_mode_fwd_code;
} dnx_rx_parse_headers_t;

int dnx_rx_packet_parse(
    int unit,
    bcm_pkt_t * pkt,
    uint8 device_access_allowed);

int dnx_rx_packet_parser_etpp_mtu_trap(
    int unit,
    bcm_pkt_t * pkt,
    uint8 is_recycle_append_ftmh);

/**
 * \brief - Initialize dnx rx module
 * Will init all relevant info, such as Traps etc..
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *
 * \par INDIRECT INPUT:
 *   * DNX data related RX module information
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * Rx HW related regs/mems
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rx_init(
    int unit);

/**
 * \brief - Deinitialize dnx rx module
 *  Will deinit all relevant info, such as Traps etc..
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e - Error Type
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rx_deinit(
    int unit);

 /*
  * Procedures for rx_trap.c {
  */

/**
* \brief
*  Configure all trap related that is relevant for device initiation
* \par DIRECT INPUT:
*   \param [in] unit      -  Unit Id
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
shr_error_e dnx_rx_trap_init(
    int unit);

/*
  * } Procedures for rx_trap.c
 */

/*
 * }
 */
#endif/*_RX_API_INCLUDED__*/
