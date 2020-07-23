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

#ifndef _QOS_INTERNAL_H_INCLUDED__
#define _QOS_INTERNAL_H_INCLUDED__

#include <soc/dnx/dbal/dbal.h>
#include <bcm/qos.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

/**
 * Define function prototype for _is_allocated procedures
 */
typedef int (
    *dnx_qos_map_id_is_allocated_f) (
    int,
    int,
    uint8 *);

/**
 * \brief
 * Boundary check macro
 */
#define DNX_QOS_ERROR_IF_OUT_OF_RANGE(name, val, max_limit)                                            \
        if (((uint32)val) > max_limit) {                                                                             \
            SHR_ERR_EXIT(_SHR_E_PARAM, "%s value %d id out of range 0 ~ %d\n",name, val, max_limit);  \
        }

/**
 * invalid qos map id
 */
#define BCM_DNX_QOS_MAP_INVALID_ID              -1

/**
 * supported flags for ingress qos application types PHB/REMARK ....
 */
#define DNX_QOS_INGRESS_APPLICATIONS_SUPPORTED_FLAGS (BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK |\
                                              BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_POLICER | BCM_QOS_MAP_L2_VLAN_PCP)
/**
 * supported flags for egress qos application types REMARK..
 */
#define DNX_QOS_EGRESS_APPLICATIONS_SUPPORTED_FLAGS (BCM_QOS_MAP_REMARK | BCM_QOS_MAP_ECN | BCM_QOS_MAP_POLICER |\
                                                     BCM_QOS_MAP_OAM_PCP | BCM_QOS_MAP_MPLS_PHP | BCM_QOS_MAP_L3_L2 |\
                                                     BCM_QOS_MAP_ENCAP_INTPRI_COLOR)

#define DNX_QOS_APPLICATIONS_SUPPORTED_FLAGS  (DNX_QOS_INGRESS_APPLICATIONS_SUPPORTED_FLAGS | DNX_QOS_EGRESS_APPLICATIONS_SUPPORTED_FLAGS)

/**
 * supported flags for map types
 */
#define DNX_QOS_MAP_TYPES_SUPPORTED_FLAGS (BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_L2_INNER_TAG | BCM_QOS_MAP_L2_INNER_TAG | \
                                           BCM_QOS_MAP_L2_UNTAGGED  | BCM_QOS_MAP_IPV4         | BCM_QOS_MAP_IPV6         | \
                                           BCM_QOS_MAP_MPLS | BCM_QOS_MAP_PORT | BCM_QOS_MAP_MPLS_SECOND | BCM_QOS_MAP_L2_TWO_TAGS | \
                                           BCM_QOS_MAP_RCH)

/**
 * supported flags for map create function
 * Map create function support direction flags & ID flag
 */
#define DNX_QOS_MAP_CREATE_SUPPORTED_FLAGS (BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_WITH_ID|\
                                            DNX_QOS_APPLICATIONS_SUPPORTED_FLAGS)

/**
 * supported flags for map add function
 * Map add function support direction flags, application type flags & map types flags.
 */
#define DNX_QOS_MAP_ADD_SUPPORTED_FLAGS   (DNX_QOS_APPLICATIONS_SUPPORTED_FLAGS | DNX_QOS_MAP_TYPES_SUPPORTED_FLAGS)

/**
 * supported flags for qos control
 */
#define DNX_QOS_CONTROL_SUPPORTED_FLAGS    (BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_IP_MC)

/*
 * Init values for qos model mapping table
 */
#define QOS_DP_PIPE_O1                  0
#define QOS_DP_PIPE_O2                  1
#define NETWORK_QOS_PIPE_O1             0
#define NETWORK_QOS_PIPE_O2             1
#define MAX_NUM_QOS_MODEL               8

typedef struct
{
    uint8 dp_select;
    uint8 is_ecn_eligiable;
    uint8 dp_is_preserve;
    uint8 nwk_qos_is_preserve;
    uint8 fwd_plus_1_explicit_profile;
} qos_model_mapping_table_t;

/**
 * \brief  structures for Propagation profile init used to initiate
 * DBAL_TABLE_QOS_PROPAG_PROFILE_MAPPING table
 */
typedef struct
{
    uint32 compare_stength;
    uint32 propagation_strength;
} qos_ingress_propagation_model_t;

typedef enum dnx_routong_dscp_preserve_mode_e
{
    ROUTING_DSCP_PRESERVE_MODE_INLIF_OUTLIF = 1,
    ROUTING_DSCP_PRESERVE_MODE_OUTLIF = 2
} dnx_routong_dscp_preserve_mode_t;

/**
 * \brief
 * add Qos INGRESS PHB map
 * \param [in] unit - Relevant unit.
 * \param [in] flags - Relevant flags.
 * \param [in] map_opcode - map opcode for map type and ingress map.
 * \param [in] map_entry - relevant map entry.
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_ingress_phb_map_add(
    int unit,
    uint32 flags,
    int map_opcode,
    bcm_qos_map_t * map_entry);

/**
 * \brief
 * delete Qos INGRESS PHB map entry.
 * for each profile & app & map type we have a unique key
 * structure for example for ETH it is PCP/DEI, for IPV6 it is
 * TOS for IPV4 it is DSCP
 * \param [in] unit - Relevant unit.
 * \param [in] flags - Relevant flags
 * \param [in] map_opcode - encoded map opcode for map type and ingress map.
 * \param [in] map_entry - relevant map entry.
 *
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_ingress_phb_map_delete(
    int unit,
    uint32 flags,
    int map_opcode,
    bcm_qos_map_t * map_entry);

/**
 * \brief
 * add Qos INGRESS REMARK map
 * \param [in] unit - Relevant unit.
 * \param [in] flags - Relevant flags.
 * \param [in] map_opcode - encoded map opcode for map type and ingress map.
 * \param [in] map_entry - relevant map entry.
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_ingress_remark_map_add(
    int unit,
    uint32 flags,
    int map_opcode,
    bcm_qos_map_t * map_entry);

/**
 * \brief
 * delete Qos INGRESS REMARK map entry
 * \param [in] unit - Relevant unit.
 * \param [in] flags - Relevant flags
 * \param [in] map_opcode - encoded map opcode for map type and ingress map.
 * \param [in] map_entry - relevant map entry.
 *
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_ingress_remark_map_delete(
    int unit,
    uint32 flags,
    int map_opcode,
    bcm_qos_map_t * map_entry);

/**
 * \brief
 * add Qos EGRESS current layer marking map
 *     \param [in] unit - Relevant unit.
 *     \param [in] flags - Relevant flags.
 *     \param [in] map_entry - relevant map entry.
 *     \param [in] map_opcode - encoded map opcode of egress map for current layer marking
 *
 *  \return
 *     \retval Negative in case of an error
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_egress_mark_map_add(
    int unit,
    uint32 flags,
    int map_opcode,
    bcm_qos_map_t * map_entry);

/**
 * \brief
 * delete Qos EGRESS REMARK map entry
 *   \param [in] unit - Relevant unit.
 *   \param [in] flags - Relevant flags
 *   \param [in] map_entry - relevant map entry.
 *   \param [in] map_opcode -encoded map opcode of egress map for current layer marking
 *
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_egress_mark_map_delete(
    int unit,
    uint32 flags,
    int map_opcode,
    bcm_qos_map_t * map_entry);

/**
 * \brief
 * add Qos EGRESS REMARK map
 *     \param [in] unit - Relevant unit.
 *     \param [in] flags - Relevant flags.
 *     \param [in] map_entry - relevant map entry.
 *     \param [in] map_id - encoded map id.
 *
 *  \return
 *     \retval Negative in case of an error
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_egress_remark_map_add(
    int unit,
    uint32 flags,
    int map_id,
    bcm_qos_map_t * map_entry);

/**
 * \brief
 * delete Qos EGRESS REMARK map entry
 *   \param [in] unit - Relevant unit.
 *   \param [in] flags - Relevant flags
 *   \param [in] map_entry - relevant map entry.
 *   \param [in] map_id -encoded map opcode of egress map for current layer marking or
 *                       encoded map ID of the EGRESS MAP for inheritance to next layer mapping.
 *
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_egress_remark_map_delete(
    int unit,
    uint32 flags,
    int map_id,
    bcm_qos_map_t * map_entry);

/**
 * \brief
 * Initialize hw tables,
 *      phbTcDpContextSelectionCam.
 *      phbTcDpContextActionMem.
 *  \param [in] unit - Relevant unit
 *  \return
 *     \retval Negative in case of an error
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_ingress_tables_init(
    int unit);

/**
 * \brief
 * Initialize DBAL Table: DBAL_TABLE_QOS_MODEL_MAPPING_TABLE.
 *  for each QOS_MODEL an entry is set with all needed
 * parameters.
 * use qos_model_mapping_table_init for input values for table
 * init
 * Write to QOS Hardware: ETPPB_QOS_MODEL_MAPPING_TABLE
 *    \param [in] unit - Relevant unit.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_remark_qos_model_mapping_init(
    int unit);

/**
 * \brief
 * Initialize DBAL Table: DBAL_TABLE_QOS_INITIAL_NWK_QOS_SELECT
 * & DBAL_TABLE_QOS_INITIAL_NWK_QOS_SELECT.
 * When working only with Jericho 2 devices this table only
 * takes the ingress_nwq_qos. When working with Jericho1 devices
 * there is a need to hadnle with the ingress_nwq_qos, and there are cases
 * that this parameter is not passed to the egress. This table
 * should supply a solution for those cases.
 * Write to QOS Hardware:
 * ETPPA_INITIAL_NWK_QOS_SELECT,
 * ETPPA_INITIAL_NWK_QOS_SELECT_PLUS_1
 *    \param [in] unit - Relevant unit.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_remark_nwk_qos_init(
    int unit);

/**
 * \brief
 * Initialize DBAL Table: DBAL_TABLE_QOS_DP_TO_TYPE_FWD.
 * set the appropriate qos var type for each forwarding code and
 * ecn eligiable combination.
 *
 * Write to QOS Hardware:
 * ETPPC_MAP_FWD_QOS_DP_TO_TYPE_FWD.
 *
 *    \param [in] unit - Relevant unit.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_remark_qos_dp_to_type_fwd_init(
    int unit);

/**
 * \brief
 * Initialize qos egress tables
 *    \param [in] unit -
 *     Relevant unit.
 *
 * \return
 *   \retval Negative in case of an error
 *   \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_egress_tables_init(
    int unit);

/**
 * \brief
 *     create the ingress sw qos profile according to the flags
 * \param [in] unit - Relevant unit.
 * \param [in] flags - related flags BCM_QOS_MAP_XXX.
 * \param [in] map_id - ID of the MAP.  Pointer to module
 *     ID of the local device to be filled by the function.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_ingress_map_create(
    int unit,
    uint32 flags,
    int *map_id);

/**
 * \brief
 *     create the ingress sw qos opcode according to the flags
 * \param [in] unit - Relevant unit.
 * \param [in] flags - related flags BCM_QOS_MAP_XXX.
 * \param [in] map_opcode - opcode of the MAP.  Pointer to module
 *     ID of the local device to be filled by the function.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_ingress_opcode_create(
    int unit,
    uint32 flags,
    int *map_opcode);

/**
 * \brief
 *     create the egress sw qos profile according to the flags
 * \param [in] unit - Relevant unit.
 * \param [in] flags - related flags BCM_QOS_MAP_XXX.
 * \param [in] map_id - ID of the MAP.  Pointer to module
 *     ID of the local device to be filled by the function.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_egress_map_create(
    int unit,
    uint32 flags,
    int *map_id);

/**
 * \brief
 *     create the egress sw qos opcode according to the flags
 * \param [in] unit - Relevant unit.
 * \param [in] flags - related flags BCM_QOS_MAP_XXX.
 * \param [in] map_opcode - opcode of the MAP.  Pointer to module
 *     ID of the local device to be filled by the function.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_egress_opcode_create(
    int unit,
    uint32 flags,
    int *map_opcode);

/**
 * \brief
 *     add ingress map entries to an allocated QOS profile
 * \param [in] unit - Relevant unit.
 * \param [in] flags - relevant flags.
 * \param [in] map_entry - relevant map entry.
 * \param [in] map_id - map id for map type and ingress map.
 *
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_ingress_map_add(
    int unit,
    uint32 flags,
    int map_id,
    bcm_qos_map_t * map_entry);

/**
 * \brief
 *   add egress map entries to an allocated QOS profile
 * \param [in] unit - Relevant unit.
 * \param [in] flags - related flags.
 * \param [in] map_entry - relevant map entry.
 * \param [in] map_id - encoded map ID.
 *
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_egress_map_add(
    int unit,
    uint32 flags,
    int map_id,
    bcm_qos_map_t * map_entry);

/**
 * \brief
 *     delete ingress entries from an allocated QOS profile
 * \param [in] unit - Relevant unit.
 * \param [in] flags - relevant flags.
 * \param [in] map_entry - relevant map entry.
 * \param [in] map_id - ID of the EGRESS MAP.
 *
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */

shr_error_e dnx_qos_ingress_map_delete(
    int unit,
    uint32 flags,
    int map_id,
    bcm_qos_map_t * map_entry);

/**
 * \brief
 *     delete egress entries from an allocated QOS profile
 * \param [in] unit - Relevant unit.
 * \param [in] flags - relevant flags.
 * \param [in] map_entry - relevant map entry.
 * \param [in] map_id - ID of the EGRESS MAP.
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_egress_map_delete(
    int unit,
    uint32 flags,
    int map_id,
    bcm_qos_map_t * map_entry);

/**
 * \brief
 * set the Ingress QOS profile to the port (LIF)
 *    \param [in] unit - Relevant unit.
 *    \param [in] port - ID of port.
 *    \param [in] ing_map - ID of the INGRESS MAP.
 * \return
 *    \retval Negative in case of an error.
 *    \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_ingress_port_map_set(
    int unit,
    bcm_gport_t port,
    int ing_map);

/**
 * \brief
 *    set the Ingress QOS profile to the port (LIF)
 * \param [in] unit - Relevant unit.
 * \param [in] port - ID of port.
 * \param [in] egr_map - ID of the EGRESS MAP.
 * \return
 *    \retval Negative in case of an error.
 *    \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_egress_port_map_set(
    int unit,
    bcm_gport_t port,
    int egr_map);

/**
 * \brief
 *     to get the Ingress QOS profile on the port (LIF)
 *     call dnx_algo_gpm_gport_to_hw_resources to receive local_in_lif,
 *     and get lif entry from corresponding dbal table ,
 *     then get ingress qos profile field of lif entry
 */
shr_error_e dnx_qos_ingress_port_map_get(
    int unit,
    bcm_gport_t port,
    int *ing_map,
    uint32 flags);

/**
 * \brief
 *    to get the egress QOS profile on the port (LIF)
 *    call dnx_algo_gpm_gport_to_hw_resources to receive local_out_lif,
 *    and get lif entry from corresponding dbal table,
 *    then get egress qos profile field of lif entry
 */
shr_error_e dnx_qos_egress_port_map_get(
    int unit,
    bcm_gport_t port,
    int *egr_map,
    uint32 flags);

/**
 * \brief
 *    to get the ingress mapping entries
 *    return mapping entry array, and number of mapping.
 *    if array_size is zero, means only to get number of entries.
 */
int dnx_qos_ingress_map_multi_get(
    int unit,
    uint32 flags,
    int map_id,
    int array_size,
    bcm_qos_map_t * array,
    int *array_count);

/**
 * \brief
 *    to get the egress mapping entries
 *    return mapping entry array, and number of mapping.
 *    if array_size is zero, means only to get number of entries.
 */
int dnx_qos_egress_map_multi_get(
    int unit,
    uint32 flags,
    int map_id,
    int array_size,
    bcm_qos_map_t * array,
    int *array_count);

/**
 * \brief
 *   Set to HW the policer_offset based on the Qos profile and tc.
 *   Used by meter_ptr as follow: {Meter_Ptr = Meter_ID + policer_offset}
 */
shr_error_e dnx_qos_egress_policer_map_set(
    int unit,
    int map_id,
    int int_pri,
    int policer_offset);

/**
 * \brief
 * allocate profile 0 as default ingress profile.
 *    \param [in] unit - Relevant unit.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_ingress_profiles_init(
    int unit);

/**
 * \brief
 * allocate profile 0 as default egress profile.
 *    \param [in] unit - Relevant unit.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_egress_profiles_init(
    int unit);

/**
 * \brief
 *  Set the QoS attributes(remark_profile, TTL_MODEL, ttl, etc.) used in egress
 *  encapsulation for the cases in which EEI is used for MPLS encapsulation.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] flags - QOS control flags,
 * \param [in] type - QOS control type.
 * \param [in] arg - The property for the control type.
 *
 * \return
 *  \retval Negative in case of an error
 *  \retval Zero in case of NO ERROR
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
shr_error_e dnx_qos_ingress_control_mpls_swap_qos_attributes_set(
    int unit,
    uint32 flags,
    bcm_qos_control_type_t type,
    int arg);

/**
 * \brief
 *  Get the QoS attributes(remark_profile, TTL_MODEL, ttl, etc.) used in egress
 *  encapsulation for the cases in which EEI is used for MPLS encapsulation.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] flags - QOS control flags,
 * \param [in] type - QOS control type. Available control types:
 *                    bcmQosControlMplsIngressSwapRemarkProfile
 * \param [out] arg - The property for the control type.
 *
 * \return
 *  \retval Negative in case of an error
 *  \retval Zero in case of NO ERROR
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
shr_error_e dnx_qos_ingress_control_mpls_swap_qos_attributes_get(
    int unit,
    uint32 flags,
    bcm_qos_control_type_t type,
    int *arg);

/**
 * \brief
 *  Set explicit null label qos attribute.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] flags - control flags
 *      BCM_QOS_MAP_IPV4
 *      BCM_QOS_MAP_IPV6.
 * \param [in] type - qos control type
 * \param [in] arg - qos profile or qos model
 *
 * \return
 *    \retval Negative in case of an error.
 *    \retval Zero in case of NO ERROR
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  * None.
 *
 * \see
 *   * None
 */
shr_error_e dnx_qos_ingress_control_explicit_null_label_qos_attributes_set(
    int unit,
    uint32 flags,
    bcm_qos_control_type_t type,
    int arg);

/**
 * \brief
 *  get explicit null label qos attribute.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] flags - control flags
 *      BCM_QOS_MAP_IPV4
 *      BCM_QOS_MAP_IPV6.
 * \param [in] type - qos control type
 * \param [out] arg - qos profile or qos model
 *
 * \return
 *    \retval Negative in case of an error.
 *    \retval Zero in case of NO ERROR
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  * None.
 *
 * \see
 *   * None
 */
shr_error_e dnx_qos_ingress_control_explicit_null_label_qos_attributes_get(
    int unit,
    uint32 flags,
    bcm_qos_control_type_t type,
    int *arg);

/**
 * \brief
 *  Set dscp preserve in MPLS PHP pop case
 * \param [in] unit - Relevant unit.
 * \param [in] flags - control flags
 *      BCM_QOS_MAP_IPV4
 *      BCM_QOS_MAP_IPV6
 *      BCM_QOS_MAP_IP_MC
 * \param [in] arg - preserve true or false
 * \return
 *    \retval Negative in case of an error.
 *    \retval Zero in case of NO ERROR
 * \return
 *   shr_error_e
 * \remark
 *  * None.
 * \see
 *   * None
 */
shr_error_e dnx_qos_ingress_control_mpls_pop_dscp_preserve_set(
    int unit,
    uint32 flags,
    int arg);

/**
 * \brief
 *  Get if dscp preserve in MPLS PHP pop case
 * \param [in] unit - Relevant unit.
 * \param [in] flags - control flags
 *      BCM_QOS_MAP_IPV4
 *      BCM_QOS_MAP_IPV6
 *      BCM_QOS_MAP_IP_MC
 * \param [out] arg - preserve true or false
 * \return
 *    \retval Negative in case of an error.
 *    \retval Zero in case of NO ERROR
 * \return
 *   shr_error_e
 * \remark
 *  * None.
 * \see
 *   * None
 */
shr_error_e dnx_qos_ingress_control_mpls_pop_dscp_preserve_get(
    int unit,
    uint32 flags,
    int *arg);

/**
 * \brief
 *  Set exp preserve in MPLS egress forward
 * \param [in] unit - Relevant unit.
 * \param [in] flags - QOS control flags
 * \param [in] arg - preserve true or false
 * \return
 *    \retval Negative in case of an error.
 *    \retval Zero in case of NO ERROR
 * \return
 *   shr_error_e
 * \remark
 *  * None.
 * \see
 *   * None
 */
shr_error_e dnx_qos_egress_control_mpls_fwd_exp_preserve_set(
    int unit,
    uint32 flags,
    int arg);

/**
 * \brief
 *  Get if exp preserve in MPLS egress forward case
 * \param [in] unit - Relevant unit.
 * \param [in] flags - QOS control flags
 * \param [out] arg - preserve true or false
 * \return
 *    \retval Negative in case of an error.
 *    \retval Zero in case of NO ERROR
 * \return
 *   shr_error_e
 * \remark
 *  * None.
 * \see
 *   * None
 */
shr_error_e dnx_qos_egress_control_mpls_fwd_exp_preserve_get(
    int unit,
    uint32 flags,
    int *arg);

/**
 * \brief
 *  Set the QOS map control type.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] map_id - QOS map id
 * \param [in] flags - Relevant flags, BCM_QOS_MAP_PHB, BCM_QOS_MAP_REMARK
 * \param [in] type - The type for qos map control, see bcm_qos_map_control_type_t.
 * \param [in] arg - The value of map control type.
 *
 * \return
 *  \retval Negative in case of an error
 *  \retval Zero in case of NO ERROR
 */

int dnx_qos_ingress_map_control_set(
    int unit,
    uint32 map_id,
    uint32 flags,
    bcm_qos_map_control_type_t type,
    int arg);

/**
 * \brief
 *  Get the QOS map control type.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] map_id - QOS map id
 * \param [in] flags - Relevant flags, BCM_QOS_MAP_PHB, BCM_QOS_MAP_REMARK
 * \param [in] type - The type for qos map control, see bcm_qos_map_control_type_t.
 * \param [out] arg - The value of map control type.
 *
 * \return
 *  \retval Negative in case of an error
 *  \retval Zero in case of NO ERROR
 */
int dnx_qos_ingress_map_control_get(
    int unit,
    uint32 map_id,
    uint32 flags,
    bcm_qos_map_control_type_t type,
    int *arg);

/**
 * \brief
 *    clear qos profile CS profile mapping.
 * \param [in] unit - Relevant unit.
 * \param [in] profile - QOS ingress profile
 *
 * \return
 *  \retval Negative in case of an error
 *  \retval Zero in case of NO ERROR
 */
shr_error_e dnx_qos_ingress_map_control_clear(
    int unit,
    int profile);

#endif /* _QOS_INTERNAL_H_INCLUDED__ */
