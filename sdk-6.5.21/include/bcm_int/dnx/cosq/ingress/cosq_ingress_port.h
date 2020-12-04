/** \file cosq_ingress_port.h
 * 
 * 
 * Internal API of COSQ control set / get
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * $Copyright:.$
 */

#ifndef _DNX_COSQ_INGRESS_PORT_H_INCLUDED_
/** { */
#define _DNX_COSQ_INGRESS_PORT_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

/*
 * Reset logic: in-reset = 1, out-of-reset = 0
 */

#define IMB_COSQ_PRD_TM_KEY_TC_GET(tm_key, tc)          (tc = (tm_key >> _SHR_PORT_PRD_TM_DP_KEY_SIZE) & _SHR_PORT_PRD_TM_TC_KEY_MASK)
#define IMB_COSQ_PRD_TM_KEY_DP_GET(tm_key, dp)          (dp = tm_key & _SHR_PORT_PRD_TM_DP_KEY_MASK)
#define IMB_COSQ_PRD_IP_KEY_DSCP_GET(ip_key, dscp)      (dscp = ip_key & _SHR_PORT_PRD_IP_DSCP_KEY_MASK)
#define IMB_COSQ_PRD_MPLS_KEY_EXP_GET(mpls_key, exp)    (exp = mpls_key & _SHR_PORT_PRD_MPLS_EXP_KEY_MASK)
#define IMB_COSQ_PRD_ETH_KEY_PCP_GET(eth_key, pcp)      (pcp = (eth_key >> _SHR_PORT_PRD_ETH_DEI_KEY_SIZE) & _SHR_PORT_PRD_ETH_PCP_KEY_MASK)
#define IMB_COSQ_PRD_ETH_KEY_DEI_GET(eth_key, dei)      (dei = eth_key & _SHR_PORT_PRD_ETH_DEI_KEY_MASK)

#define IMB_COSQ_ALL_RMCS                        (-1)
#define IMB_COSQ_ALL_SCH_PRIO                    (-1)
#define IMB_COSQ_RMC_INVALID                     (-1)

/** The number of pools which the polling is expected to complete within */
#define IMB_COSQ_RMC_COUNTER_POLLING_NOF_POLLS        (100)
/** Time-out for RMC counter polling [usec] */
#define IMB_COSQ_RMC_COUNTER_POLLING_TIME_OUT_USEC    (20)

/*
 * MACROs
 * {
 */

#define DNX_COSQ_PRD_TCAM_ENTRY_OFFSET_GET(offset_array, offset_index, offset) \
            SHR_BITCOPY_RANGE(&offset, 0, &offset_array, offset_index * 8, 8)

#define DNX_COSQ_PRD_TCAM_ENTRY_OFFSET_SET(offset, offset_index, offset_array) \
            SHR_BITCOPY_RANGE(&offset_array, offset_index * 8, &offset, 0, 8)
/*
 * Structs
 * {
 */
/*The struct fields match the fields in DBAL table NIF_PRD_ETHU_TCAM/NIF_PRD_ILU_TCAM.*/
typedef struct dnx_cosq_prd_tcam_entry_info_s
{
    uint32 ether_code_val;      /* ether type code value */
    uint32 ether_code_mask;     /* ether value mask (1->valid, 0->dont care) */
    uint32 offset_array_val;    /* offset array - all 4 offsets result fields concatenated in a single uint32 */
    uint32 offset_array_mask;   /* offset array mask (1->valid, 0->dont care) */
    uint32 priority;            /* priority for the tcam entry */
    uint32 tdm;                 /* tdm priority for the tcam entry */
    uint32 is_entry_valid;      /* is entry valid to compare */
} dnx_cosq_prd_tcam_entry_info_t;

/**
 * \brief - sub function of bcm_cosq_control_set, to handle 
 *        types where the port is not GPORT (port = local port)
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] type - type of command to perform (see 
 *        bcm_cosq_control_t)
 * \param [in] arg - argument to set in the given command
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_ingress_port_set(
    int unit,
    bcm_gport_t port,
    bcm_cosq_control_t type,
    int arg);
/**
 * \brief - sub function of bcm_cosq_contol_get, to handle types 
 *        of commands where the expected port is not GPORT (port
 *        = local port)
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] type - type of command to perform
 * \param [in] arg - returned value from given command
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_ingress_port_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_control_t type,
    int *arg);

/**
 * \brief - get outer ITMH header TC (Traffic Class) and DP (Drop Precedence) field offsets
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] tc_offset - TC (Traffic Class) field offset
 * \param [out] dp_offset - DP (Drop Precedence) field offset
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

int dnx_cosq_prd_itmh_offsets_get(
    int unit,
    bcm_port_t port,
    int *tc_offset,
    int *dp_offset);
/**
 * \brief - get outer FTMH header TC (Traffic Class) and DP (Drop Precedence) field offsets
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] tc_offset - TC (Traffic Class) field offset
 * \param [out] dp_offset - DP (Drop Precedence) field offset
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_prd_ftmh_offsets_get(
    int unit,
    bcm_port_t port,
    int *tc_offset,
    int *dp_offset);

/*Set/Get PRD TCAM key per Ether type (for the soft stage).*/
/**
 * \brief - build the key to the TCAM table. the key is composed
 *        of 36 bit:
 *  0-31: 4 offsets from the packet header given by the user.
 *        from each offset 8 bits are taken. the offsets start
 *        from a configurable offset base:
 *        - start of packet
 *        - end of eth header
 *        - end of header after eth header
 *  32-35: ether type code. a different key can be
 *        composed per each ether type code.
 *
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code to build the
 *        key.
 * \param [in] flex_key_config - key info to config (array of 4
 *        offsets and offsets base).
 *        in case offset base is "end of header after eth
 *        header" and the ether type code is of configurable
 *        type (1-6) it is also required to provide ether type
 *        size (in bytes)
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_prd_flex_key_construct_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    const bcm_cosq_ingress_drop_flex_key_construct_t * flex_key_config);
/**
 * \brief - get key constructed per ether type code
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code of the key to
 *        retrieve
 * \param [in] flex_key_config - returned key info
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_prd_flex_key_construct_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    bcm_cosq_ingress_drop_flex_key_construct_t * flex_key_config);

/**
 * \brief - set all fixed ether types sizes. (in bytes). this
 *        function is called only once per port, when the PRD is
 *        enabled (fixed ether types should always have the
 *        same size). the configurable ether types sizes are set
 *        by the user, see API
 *        imb_ethu_prd_flex_key_construct_set
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_prd_ether_type_size_set(
    int unit,
    bcm_port_t port);

/**
 * \brief - get outer tag size
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] outer_tag_size - Other tag size
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_prd_outer_tag_size_get(
    int unit,
    bcm_port_t port,
    uint32 *outer_tag_size);

/**
 * \brief - get PRD port type from port header type. in addition
 *        get TM/Eth port indication, depending on the header
 *        type of the port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] prd_port_type - PRD port type. see
 *        NIF_PRD_PORT_TYPE in DBAL
 * \param [in] is_eth_port - is eth port type
 * \param [in] is_tm_port - is TM port type
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_prd_port_type_get(
    int unit,
    bcm_port_t port,
    uint32 *prd_port_type,
    uint32 *is_eth_port,
    uint32 *is_tm_port);

/**
 * \brief - get the correct RMC of the port to which the PRD
 *        priority is mapped
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority - priority to look for
 * \param [in] rmc - returned RMC for the Priority
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_prd_threshold_rmc_get(
    int unit,
    bcm_port_t port,
    uint32 priority,
    dnx_algo_port_rmc_info_t * rmc);

/**
 * \brief - return true if the ether type code is configurable
 *        to the user. the PRD parser recognizes several ether
 *        types - some are fixed values, and some are open for
 *        user preference.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code to check
 * \param [in] is_custom_ether_type - returned indication
 *        whether the ether type is configurable
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_prd_is_custom_ether_type_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *is_custom_ether_type);

#endif /*_DNX_COSQ_INGRESS_PORT_H_INCLUDED_*/
