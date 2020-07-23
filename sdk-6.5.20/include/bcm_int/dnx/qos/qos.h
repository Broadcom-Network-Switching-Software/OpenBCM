/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains dnx QOS module definitions internal to the BCM library.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef _QOS_H_INCLUDED__
#define _QOS_H_INCLUDED__

#include <bcm/qos.h>
#include <bcm/port.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/dbal/dbal_structures.h>

/**
 *  qos map id
 */
#define DNX_QOS_INVALID_MAP_ID              -1
#define DNX_QOS_INITIAL_MAP_ID              0

#define DNX_QOS_L3_MAX_PROFILE              15
#define DNX_QOS_ING_LIF_MAX_PROFILE         255
#define DNX_QOS_ING_AC_MAX_PROFILE          63
#define DNX_QOS_EGR_AC_MAX_PROFILE          31

#define DNX_QOS_ECN_BURN_OPCODE_NUM  4
#define DNX_QOS_EXPLICIT_IDX_FLAG    1

/*
 * The network qos pipe template data is constructed from network_qos_pipe, 
 * network_qos_pipe_mapped and dp_pipe_mapped fields. 
 * when calling dnx_qos_egress_network_qos_profile_update function if we 
 * want to keep the same value for a field we use this value.
 */
#define DNX_QOS_NETWORK_QOS_PROFILE_KEEP_VALUE                          0x800

/** 
 * \brief 
 * map id profile mask and shift definition 
 */
#define DNX_QOS_MAP_PROFILE_MASK            0xffff
#define DNX_QOS_MAP_PROFILE_SHIFT           0

#define DNX_QOS_MAP_PROFILE_GET(map_id)       \
        (((map_id) >> DNX_QOS_MAP_PROFILE_SHIFT) & DNX_QOS_MAP_PROFILE_MASK)

/** 
 * \brief 
 *  Ingress default QoS propagation profile(total 8).
 *  It defines a set of compare strength and propagation strength for PHB,REMARK,ECN,TTL. 
 *  profile "PIPE" means pipe model (compare strength = 1,propagation strength = 1) for all of them, as default profile
 *  profile "UNIFORM" means uniform model (compare strength = 0,propagation strength = 1) for all of them,
 *  profile "SHORT_PIPE" means short pipe model (compare strength = 1,propagation strength = 0) for all of them, 
 *  All the rest will be managed by user control
 */
#define DNX_QOS_INGRESS_PROPAGATION_PROFILE_PIPE                (0)
#define DNX_QOS_INGRESS_PROPAGATION_PROFILE_UNIFORM             (1)
#define DNX_QOS_INGRESS_PROPAGATION_PROFILE_SHORT_PIPE          (2)
#define DNX_QOS_INGRESS_DEFAULT_PROPAGATION_PROFILE    DNX_QOS_INGRESS_PROPAGATION_PROFILE_PIPE

/**
 *\brief
 * ECN mapping profile, 
 * Only support two ecn mapping profile, default 0 that ecn is not eligible and 
 * profile 1 that ecn is eligible
 */
#define DNX_QOS_ECN_MAPPING_PROFILE_DEFAULT    0
#define DNX_QOS_ECN_MAPPING_PROFILE_ELIGIBLE   1

/**
 * \brief  
 *    dp profile can be applied to ive for inner or outer tag.
 */
typedef enum
{
    QOS_DP_PROFILE_INNER,
    QOS_DP_PROFILE_OUTER
} qos_dp_profile_application_type_e;

/** 
 * bcm internal map flags for type encodig  
 */
#define DNX_QOS_MAP_PHB       0x1
#define DNX_QOS_MAP_REMARK    0x2
#define DNX_QOS_MAP_ECN       0x4
#define DNX_QOS_MAP_OPCODE    0x8
#define DNX_QOS_MAP_POLICER   0x10
#define DNX_QOS_MAP_VLAN_PCP  0x20
#define DNX_QOS_MAP_MPLS_PHP  0x40
#define DNX_QOS_MAP_INGRESS   0x1000
#define DNX_QOS_MAP_EGRESS    0x2000
#define DNX_QOS_MAP_OAM_PCP   0x4000
#define DNX_QOS_MAP_ENCAP_COS 0x8000

/** 
 * \brief 
 * mask & shift definition 
 */
#define DNX_QOS_PRI_MASK         0x7
#define DNX_QOS_PRI_OFFSET       1
#define DNX_QOS_CFI_MASK         0x1
#define DNX_QOS_TC_MASK          0x7
#define DNX_QOS_TC_OFFSET        2
#define DNX_QOS_DP_MASK          0x3
#define DNX_QOS_DP_OFFSET        8
#define DNX_QOS_TOS_MASK         0xFF
#define DNX_QOS_EXP_MASK         0x7
#define DNX_QOS_EXP_OFFSET       3
#define DNX_QOS_ECN_MASK         0x3
#define DNX_QOS_ECN_OFFSET       2
#define DNX_QOS_PCP_DEI_MASK     0xF
#define DNX_QOS_PCP_DEI_OFFSET   4
#define DNX_QOS_EXPLICIT_IDX_OFFSET  10
#define DNX_QOS_MAP_INDEX_MASK   0x3F
/**exp offset in case two labels in single encap entry*/
#define DNX_QOS_SECOND_EXP_OFFSET 0x3

/**
 * explicit network qos index
 */

#define DNX_QOS_IDX_PCP_DEI_TO_NWK_QOS(pcp, cfi) \
      ((((pcp << DNX_QOS_PRI_OFFSET) | cfi) << DNX_QOS_PCP_DEI_OFFSET) | \
       ((pcp << DNX_QOS_PRI_OFFSET) | cfi))
#define DNX_QOS_IDX_NWK_QOS_PCP_GET(nwk_qos) (((nwk_qos) >> DNX_QOS_PRI_OFFSET) & DNX_QOS_PRI_MASK)
#define DNX_QOS_IDX_NWK_QOS_CFI_GET(nwk_qos) ((nwk_qos) & DNX_QOS_CFI_MASK)

#define DNX_QOS_EXPLICIT_IDX_L2(pcp, cfi) \
     ((DNX_QOS_EXPLICIT_IDX_FLAG << DNX_QOS_EXPLICIT_IDX_OFFSET) | \
      DNX_QOS_IDX_PCP_DEI_TO_NWK_QOS(pcp,cfi))
#define DNX_QOS_EXPLICIT_IDX_PCP_GET(nwk_qos_idx) (((nwk_qos_idx) >> DNX_QOS_PRI_OFFSET) & DNX_QOS_PRI_MASK)
#define DNX_QOS_EXPLICIT_IDX_CFI_GET(nwk_qos_idx) ((nwk_qos_idx) & DNX_QOS_CFI_MASK)

#define DNX_QOS_IDX_EXP_TO_NWK_QOS(exp0, exp1) \
      ((exp1 << DNX_QOS_SECOND_EXP_OFFSET) | exp0)
#define DNX_QOS_IDX_NWK_QOS_GET_EXP0(nwk_qos) ((nwk_qos) & DNX_QOS_EXP_MASK)
#define DNX_QOS_IDX_NWK_QOS_GET_EXP1(nwk_qos) ((nwk_qos >> DNX_QOS_SECOND_EXP_OFFSET) & DNX_QOS_EXP_MASK)

/** 
 * \brief
 * map type shift, map is 32 bits build of 16 bit map type and 
 * 16 bit profile this define indicate the map type shift in 
 * the 32 bit value 
 */
#define DNX_QOS_MAP_TYPE_SHIFT              16

#define DNX_QOS_MAP_IS_INGRESS(map_id)    \
  (((map_id) >> DNX_QOS_MAP_TYPE_SHIFT) & DNX_QOS_MAP_INGRESS)

#define DNX_QOS_MAP_IS_EGRESS(map_id)    \
  (((map_id) >> DNX_QOS_MAP_TYPE_SHIFT) & DNX_QOS_MAP_EGRESS)

#define DNX_QOS_MAP_IS_OPCODE(map_id)    \
  (((map_id) >> DNX_QOS_MAP_TYPE_SHIFT) & DNX_QOS_MAP_OPCODE)

#define DNX_QOS_MAP_IS_PHB(map_id)    \
  (((map_id) >> DNX_QOS_MAP_TYPE_SHIFT) & DNX_QOS_MAP_PHB)

#define DNX_QOS_MAP_IS_REMARK(map_id)    \
  (((map_id) >> DNX_QOS_MAP_TYPE_SHIFT) & DNX_QOS_MAP_REMARK)

#define DNX_QOS_MAP_IS_MPLS_PHP(map_id)    \
  (((map_id) >> DNX_QOS_MAP_TYPE_SHIFT) & DNX_QOS_MAP_MPLS_PHP)

#define DNX_QOS_MAP_IS_ECN(map_id)    \
  (((map_id) >> DNX_QOS_MAP_TYPE_SHIFT) & DNX_QOS_MAP_ECN)

#define DNX_QOS_MAP_IS_POLICER(map_id)    \
  (((map_id) >> DNX_QOS_MAP_TYPE_SHIFT) & DNX_QOS_MAP_POLICER)

#define DNX_QOS_MAP_IS_VLAN_PCP(map_id)    \
  (((map_id) >> DNX_QOS_MAP_TYPE_SHIFT) & DNX_QOS_MAP_VLAN_PCP)

#define DNX_QOS_MAP_IS_OAM_PCP(map_id)    \
  (((map_id) >> DNX_QOS_MAP_TYPE_SHIFT) & DNX_QOS_MAP_OAM_PCP)

#define DNX_QOS_MAP_IS_ENCAP_COS(map_id)    \
  (((map_id) >> DNX_QOS_MAP_TYPE_SHIFT) & DNX_QOS_MAP_ENCAP_COS)

#define DNX_QOS_INGRESS_MAP_SET(map_id)   \
  (map_id) |= (((DNX_QOS_MAP_INGRESS) << DNX_QOS_MAP_TYPE_SHIFT))

#define DNX_QOS_EGRESS_MAP_SET(map_id)    \
  (map_id) |= (((DNX_QOS_MAP_EGRESS) << DNX_QOS_MAP_TYPE_SHIFT))

#define DNX_QOS_OPCODE_MAP_SET(map_id)    \
  (map_id) |= (((DNX_QOS_MAP_OPCODE) << DNX_QOS_MAP_TYPE_SHIFT))

#define DNX_QOS_PHB_MAP_SET(map_id)       \
  (map_id) |= (((DNX_QOS_MAP_PHB) << DNX_QOS_MAP_TYPE_SHIFT))

#define DNX_QOS_REMARK_MAP_SET(map_id)    \
  (map_id) |= (((DNX_QOS_MAP_REMARK) << DNX_QOS_MAP_TYPE_SHIFT))

#define DNX_QOS_MPLS_PHP_MAP_SET(map_id)    \
  (map_id) |= (((DNX_QOS_MAP_MPLS_PHP) << DNX_QOS_MAP_TYPE_SHIFT))

#define DNX_QOS_ECN_MAP_SET(map_id)       \
  (map_id) |= (((DNX_QOS_MAP_ECN) << DNX_QOS_MAP_TYPE_SHIFT))

#define DNX_QOS_POLICER_MAP_SET(map_id)   \
  (map_id) |= (((DNX_QOS_MAP_POLICER) << DNX_QOS_MAP_TYPE_SHIFT))

#define DNX_QOS_VLAN_PCP_MAP_SET(map_id)  \
  (map_id) |= (((DNX_QOS_MAP_VLAN_PCP) << DNX_QOS_MAP_TYPE_SHIFT))

#define DNX_QOS_OAM_PCP_MAP_SET(map_id)  \
  (map_id) |= (((DNX_QOS_MAP_OAM_PCP) << DNX_QOS_MAP_TYPE_SHIFT))

#define DNX_QOS_ENCAP_COS_SET(map_id)  \
  (map_id) |= (((DNX_QOS_MAP_ENCAP_COS) << DNX_QOS_MAP_TYPE_SHIFT))

typedef enum
{
    /** qos propagation type pipe  */
    DNX_QOS_PROPAGATION_TYPE_PIPE = 0,
    /** qos propagation type short pipe  */
    DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE,
     /** qos propagation type uniform  */
    DNX_QOS_PROPAGATION_TYPE_UNIFORM,
    /** qos propagation type struck  */
    DNX_QOS_PROPAGATION_TYPE_STUCK,
    /** Number of enum valid values for QOS_PROPAG_TYPE
     * Must be last
     */
    DNX_QOS_PROPAGATION_TYPE_NOF
} dnx_qos_propagation_type_e;

/**
 * \brief 
 * Init for qos module. 
 * initialize DBAL_TABLE_QOS_PHB_CONTEXT_SELECTION table. 
 * \par DIRECT INPUT
 *    \param [in] unit - Relevant unit.
 * \par INDIRECT INPUT: 
 *    None. 
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. 
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT:
 *   * Write to QOS Hardware.
 *   * phbTcDpContextSelectionCam.
 *   * phbTcDpContextActionMem.
 */
shr_error_e dnx_qos_init(
    int unit);

/**
 * \brief 
 * map color to hw value DP
 * \param [in] unit - Relevant unit.
 * \param [in] color - color
 * \param [out] dp - hardware dp
 * \return
 *     \retval Negative in case of an error. 
 *     \retval Zero in case of NO ERROR
 */
int dnx_qos_color_encode(
    int unit,
    bcm_color_t color,
    int *dp);

/**
 * \brief 
 * map hw value DP to color
 * \param [in] unit - Relevant unit.
 * \param [in] dp - hardware dp
 * \param [out] color - color
 * \return
 *     \retval Negative in case of an error. 
 *     \retval Zero in case of NO ERROR
 */

int dnx_qos_color_decode(
    int unit,
    int dp,
    bcm_color_t * color);

/**
 * \brief 
 * convert ingress qos model to propagation type.
 * \param [in] unit - Relevant unit.
 * \param [in] qos_propag_model - ingress propagation model
 * \param [out] qos_propag_type - propagation type for PHB/REMARK/TTL
 * \return
 *     \retval Negative in case of an error. 
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_ingress_qos_model_to_propag_type(
    int unit,
    bcm_qos_ingress_model_type_t qos_propag_model,
    dnx_qos_propagation_type_e * qos_propag_type);

/**
 * \brief 
 * convert propagation type to ingress qos model. 
 * \param [in] unit - Relevant unit.
 * \param [in] qos_propag_type - propagation type for PHB/REMARK/TTL
 * \param [out] qos_propag_model - ingress propagation model
 * \return
 *     \retval Negative in case of an error. 
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_ingress_propag_type_to_qos_model(
    int unit,
    dnx_qos_propagation_type_e qos_propag_type,
    bcm_qos_ingress_model_type_t * qos_propag_model);

/**
 * \brief 
 * convert egress qos model type to model. 
 * \param [in] unit - Relevant unit.
 * \param [in] qos_model - bcm egress qos model 
 * \param [out] dbal_qos_model - egress qos model in dbal
 * \return
 *     \retval Negative in case of an error. 
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_egress_model_bcm_to_dbal(
    int unit,
    bcm_qos_egress_model_t * qos_model,
    dbal_enum_value_field_encap_qos_model_e * dbal_qos_model);

/**
 * \brief 
 * convert egress qos model to model type. 
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_qos_model - egress qos model in dbal
 * \param [out] qos_model - bcm egress qos model
 * \return
 *     \retval Negative in case of an error. 
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_egress_model_dbal_to_bcm(
    int unit,
    dbal_enum_value_field_encap_qos_model_e dbal_qos_model,
    bcm_qos_egress_model_t * qos_model);

/**
 * \brief
 * Write to HW the TTL-value mapped by TTL-PIPE-PROFILE.
 * Used by BCM APIs of IPv4 and IPv6 IP-Tunnels.
 * \param [in] unit - Relevant unit.
 * \param [in] ttl_pipe_profile - template managed TTL PIPE profile index.
 * \param [in] ttl_value - TTL PIPE value.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \remark
 *   * Write to TTL Hardware: DBAL_TABLE_EGRESS_QOS_TTL_PIPE_MAPPING.
 */
shr_error_e dnx_qos_egress_ttl_profile_hw_set(
    int unit,
    uint32 ttl_pipe_profile,
    uint32 ttl_value);

/**
 * \brief
 * Clear HW entry of TTL-value mapped by TTL-PIPE-PROFILE. Used
 * by BCM APIs of IPv4 and IPv6 IP-Tunnels. 
 * \param [in] unit - Relevant unit.
 * \param [in] ttl_pipe_profile - template managed TTL PIPE profile index. 
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \remark
 *   * Write to TTL Hardware: DBAL_TABLE_EGRESS_QOS_TTL_PIPE_MAPPING.
 */
shr_error_e dnx_qos_egress_ttl_profile_hw_clear(
    int unit,
    uint32 ttl_pipe_profile);

/**
 * \brief 
 * get qos propagation profile attributes for requested 
 * propagation profile ID.
 *  
 *    \param [in] unit - Relevant unit. 
 *    \param [in] propagation_profile - Relevant
 *           propagation_profile.
 *    \param [in] phb_propagation_type - PHB app propagation
 *           type PIPE/UNIFORM/STUCK/STUCK STRONG
 *    \param [in] remark_propagation_type - PHB app propagation
 *           type PIPE/UNIFORM/STUCK/STUCK STRONG
 *    \param [in] ecn_propagation_type - PHB app propagation
 *           type PIPE/UNIFORM/STUCK/STUCK STRONG
 *    \param [in] ttl_propagation_type - PHB app propagation
 *           type PIPE/UNIFORM/STUCK/STUCK STRONG
 *    
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR 
 */
shr_error_e dnx_qos_ingress_propagation_profile_attr_get(
    int unit,
    int propagation_profile,
    dnx_qos_propagation_type_e * phb_propagation_type,
    dnx_qos_propagation_type_e * remark_propagation_type,
    dnx_qos_propagation_type_e * ecn_propagation_type,
    dnx_qos_propagation_type_e * ttl_propagation_type);

/**
 * \brief -
 *  Allocate a propagation profile with the given PHB/REMARK/ECN/TTL propagation
 *  parameters by exchanging.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] phb_propagation_type - PHB propagation type.
 * \param [in] remark_propagation_type - REMARK propagation type.
 * \param [in] ecn_propagation_type - ECN propagation type.
 * \param [in] ttl_propagation_type - TTL propagation type.
 * \param [in] old_propagation_profile - The original propagation profile.
 * \param [out] new_propagation_profile - pointer to new propagation profile.
 * \param [out] is_first - pointer to indicator whether the propagation profile
 *              is first used or not.
 * \param [out] is_last - pointer to indicator whether the propagation profile
 *              is no longer used or not.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  If the given propagation profile is in use, do the exchange based on it
 *  and the inputs. Otherwise do the exchange based on the inputs only.
 *  In case the current propagation profile support the requested type it will
 *  remain unchanged except for the reference counter, otherwise a new qos
 *  propagation profile is allocated.
 *
 * \see
 *  None
 */
shr_error_e dnx_qos_ingress_propagation_profile_alloc(
    int unit,
    dnx_qos_propagation_type_e phb_propagation_type,
    dnx_qos_propagation_type_e remark_propagation_type,
    dnx_qos_propagation_type_e ecn_propagation_type,
    dnx_qos_propagation_type_e ttl_propagation_type,
    int old_propagation_profile,
    int *new_propagation_profile,
    uint8 *is_first,
    uint8 *is_last);

/**
 * \brief -
 *  Set the propagation profile attributes to Hardware.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] propagation_profile - The given propagation profile.
 * \param [in] phb_propagation_type - PHB propagation type.
 * \param [in] remark_propagation_type - REMARK propagation type.
 * \param [in] ecn_propagation_type - ECN propagation type.
 * \param [in] ttl_propagation_type - TTL propagation type. 
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  The attributes for the given application types is pairs of compare strength
 *  and propagation strength which are predefined.
 *
 * \see
 *  None
 */
shr_error_e dnx_qos_ingress_propagation_profile_hw_set(
    int unit,
    int propagation_profile,
    dnx_qos_propagation_type_e phb_propagation_type,
    dnx_qos_propagation_type_e remark_propagation_type,
    dnx_qos_propagation_type_e ecn_propagation_type,
    dnx_qos_propagation_type_e ttl_propagation_type);

/**
 * \brief 
 * free qos propagation profile reference according to the 
 * requested propagation type of PHB/REMARK/ECN/TTL combination.
 * the reference counter decrement (this is done using the 
 * template manager).
 * If it is the last reference, clear the hardware table.
 *  
 *    \param [in] unit - Relevant unit.
 *    \param [in] propagation_profile - Relevant
 *           propagation_profile.
 *    \param [out] last_reference - is this the profile last
 *           reference
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR  
 */
shr_error_e dnx_qos_ingress_propagation_profile_free_and_hw_clear(
    int unit,
    int propagation_profile,
    uint8 *last_reference);

/**
 * \brief
 *  Clear propagation profile attributes for a specific ingress 
 *  propagation profile.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] propagation_profile - The given propagation profile.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *
 * \see
 *  None
 */
shr_error_e dnx_qos_ingress_propagation_profile_hw_clear(
    int unit,
    int propagation_profile);

/**
 * \brief -
 *  Set ingress qos model for various qos application at the port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Device or logical port number.
 * \param [in] type - Port feature enumerator (for qos application type), 
 *                    see /bcm_port_control_t
 * \param [in] value - qos model
 *
 * \return
 *   bcm_error_t
 */
shr_error_e dnx_qos_ingress_port_model_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int value);

/**
 * \brief -
 *  Get ingress qos model for various qos application at the port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Device or logical port number.
 * \param [in] type - Port feature enumerator (for qos application type), 
 *                    see /bcm_port_control_t
 * \param [in] *value - Value of ingress qos model
 *
 * \return
 *     \retval Negative in case of an error. 
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_ingress_port_model_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int *value);

/**
 * \brief 
 * set network_qos profile attributes in hw. 
 *  
 *    \param [in] unit - Relevant unit.
 *    \param [in] network_qos_pipe_profile - Relevant network
 *           qos pipe profile.
 *    \param [in] network_qos_pipe - current layer network_qos
 *           value
 *    \param [in] network_qos_pipe_mapped - next layer
 *           network_qos value
 *    \param [in] dp_pipe_mapped - next layer dp value
 * 
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR   
 */
shr_error_e dnx_qos_egress_network_qos_pipe_profile_set(
    int unit,
    uint8 network_qos_pipe_profile,
    uint32 network_qos_pipe,
    uint32 network_qos_pipe_mapped,
    uint32 dp_pipe_mapped);

/**
 * \brief 
 * allocate qos egress network_qos profile for update 
 * outlif.NWK_QOS_IDX, the profile is allocated according to the
 * requested network_qos value for current and next layer and 
 * the next layer DP value. In case this is the first time this
 * network_qos value is requested a new profile is allocated and 
 * the hw is set accordingly with the propare value. In case 
 * this network_qos value already exist the profile is returned 
 * and the reference counter increment (this is done using the 
 * template manager). 
 *  
 *    \param [in] unit - Relevant unit. 
 *    \param [in] network_qos_pipe - current layer network_qos
 *           value
 *    \param [in] network_qos_pipe_mapped - next layer
 *           network_qos value
 *    \param [in] dp_pipe_mapped - next layer dp value
 *    \param [out] network_qos_pipe_profile -
 *           Relevant network_qos_pipe_profile.
 *    \param [out] first_reference - indicate if
 *         the allocated propagation_profile is the first
 *         reference.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR   
 */
shr_error_e dnx_qos_egress_network_qos_pipe_profile_allocate(
    int unit,
    uint8 network_qos_pipe,
    uint8 network_qos_pipe_mapped,
    uint8 dp_pipe_mapped,
    int *network_qos_pipe_profile,
    uint8 *first_reference);

/**
 * \brief 
 * get qos network_qos profile attributes for requested 
 * network_qos profile ID. 
 *  
 *    \param [in] unit - Relevant unit.
 *    \param [in] network_qos_pipe_profile - Relevant
 *           network_qos_pipe_profile.
 *    \param [out] network_qos_pipe - current layer network_qos
 *           value
 *    \param [out] network_qos_pipe_mapped - next layer
 *           network_qos value
 *    \param [out] dp_pipe_mapped - next layer dp value
 *    
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_egress_network_qos_pipe_profile_attr_get(
    int unit,
    int network_qos_pipe_profile,
    uint8 *network_qos_pipe,
    uint8 *network_qos_pipe_mapped,
    uint8 *dp_pipe_mapped);

/**
 * \brief 
 * update existing profile with new network_qos value. In case 
 * the current network_qos profile support the requested 
 * network_qos value it will remain unchanged, otherwise a new 
 * qos network_qos profile is allocated. 
 *    
 *    \param [in] unit - Relevant unit. 
 *    \param [in] network_qos_pipe - current layer network_qos
 *           value
 *    \param [in] network_qos_pipe_mapped - next layer
 *           network_qos value
 *    \param [in] dp_pipe_mapped - next layer dp value
 *    \param [out] network_qos_pipe_profile - Relevant
 *           network_qos_pipe_profile.
 *    \param [out] first_reference - indicate if
 *         the new propagation_profile is the first reference.
 *    \param [out] last_reference - indicate if
 *         the old propagation_profile is the last reference. 
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR  
 */
shr_error_e dnx_qos_egress_network_qos_pipe_profile_update(
    int unit,
    uint16 network_qos_pipe,
    uint16 network_qos_pipe_mapped,
    uint16 dp_pipe_mapped,
    int *network_qos_pipe_profile,
    uint8 *first_reference,
    uint8 *last_reference);

/**
 * \brief 
 * get qos egress network_qos profile with requested network_qos value
 * it will be nwk_qos_idx of OUTLIFs
 *  
 *    \param [in] unit - Relevant unit. 
 *    \param [in] network_qos_pipe - current layer network_qos
 *           value
 *    \param [in] network_qos_pipe_mapped - next layer
 *           network_qos value
 *    \param [in] dp_pipe_mapped - next layer dp value
 *    \param [out] network_qos_pipe_profile -
 *           Relevant network_qos_pipe_profile.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR   
 */
shr_error_e dnx_qos_egress_network_qos_pipe_profile_get(
    int unit,
    uint8 network_qos_pipe,
    uint8 network_qos_pipe_mapped,
    uint8 dp_pipe_mapped,
    int *network_qos_pipe_profile);

/**
 * \brief 
 * free qos egress network_qos pipe profile reference. the 
 * reference counter decrement (this is done using the template 
 * manager). if it is the last reference, it is required to 
 * clear the HW by using the new API
 *  
 *    \param [in] unit - Relevant unit.
 *    \param [in] network_qos_pipe_profile - Relevant
 *           network_qos_pipe_profile.
 *    \param [out] last_reference - indicate if
 *         the propagation_profile is the last reference.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR  
 
 */
shr_error_e dnx_qos_egress_network_qos_pipe_profile_free(
    int unit,
    int network_qos_pipe_profile,
    uint8 *last_reference);

/**
 * \brief
 * Clear HW entry of qos pipe attribute by qos pipe profile. Used
 * by BCM APIs of MPLS, IPv4 and IPv6 IP-Tunnels. 
 * \param [in] unit - Relevant unit.
 * \param [in] network_qos_pipe_profile - Relevant
 *           network_qos_pipe_profile. 
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \remark
 *   * Write to TTL Hardware: DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_PIPE_PROFILE_TABLE.
 */
shr_error_e dnx_qos_egress_network_qos_pipe_profile_hw_clear(
    int unit,
    uint8 network_qos_pipe_profile);

/**
 * \brief
 *  Get qos map context per bcm flags.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] flags - qos map flags, BCM_QOS_MAP_*
 * \param [out] map_index - qos map index, see dbal_enum_value_field_ingress_qos_map_index_e.
 *
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *   None.
 */
shr_error_e dnx_qos_ingress_map_index_get(
    int unit,
    uint32 flags,
    dbal_enum_value_field_ingress_qos_map_index_e * map_index);

/**
 * \brief
 *  Get qos map context profile.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] table_id - qos table mapping from profile to cs profile
 * \param [in] profile - qos profile
 * \param [out] cs_profile - qos context profile.
 *
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *   None.
 */

shr_error_e dnx_qos_ingress_cs_profile_map_get(
    int unit,
    int table_id,
    int profile,
    uint32 *cs_profile);

#endif /* _QOS_H_INCLUDED__ */
