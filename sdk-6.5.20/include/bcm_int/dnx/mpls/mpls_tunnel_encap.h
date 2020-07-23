/** \file mpls_tunnel_encap.h
 *  MPLS functionality for DNX tunnel encapsulation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _MPLS_TUNNEL_ENCAP_INCLUDED__
/*
 * {
 */
#define _MPLS_TUNNEL_ENCAP_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* BCM_DNX_SUPPORT */

#include <bcm_int/dnx/algo/mpls/algo_mpls.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>

/*
 * }
 */

/*
 * MPLS labels case
 */
 /**
 * mpls tunnel encapsulation type - based on the number of MPLS labels to encapsulate.
 */
typedef enum
{
    FIRST_DNX_MPLS_TUNNEL_ENCAP_TYPE = 0,
    DNX_MPLS_TUNNEL_ENCAP_NO_LABEL = FIRST_DNX_MPLS_TUNNEL_ENCAP_TYPE,
    DNX_MPLS_TUNNEL_ENCAP_ONE_LABELS,
    DNX_MPLS_TUNNEL_ENCAP_TWO_LABELS,
    NUM_DNX_MPLS_TUNNEL_ENCAP_TYPE
} dnx_mpls_tunnel_encap_type_e;
/**
 * Minimal and maximal number of labels supported for push action. used by mpls_tunnel APIs
 */
#define DNX_MPLS_PUSH_MIN_NUM_LABELS 1
#define DNX_MPLS_PUSH_MAX_NUM_LABELS 2

/*
  * default invalid lif value.
  * must be same as CFG_LIF_NULL_VALUE HW register, used as local define as no get/set API for that register supported.
  */

#define DNX_MPLS_CFG_LIF_NULL_VALUE 0

/*
 * {
 */
/**
 * \brief
 *   Generic parameter container for mpls encap set/get
 */
typedef struct
{
    /**
     * \brief
     *   Number of labels in the EEDB entry
     */
    int labels_nof;
    /**
     * \brief
     *   Label values array
     */
    bcm_mpls_egress_label_t *label_array;
    /**
     * \brief
     *   TTL PIPE profile
     *   Used only in case TTL is in pipe mode
     */
    int ttl_pipe_profile;
    /**
     * \brief
     *   ESEM extraction command.
     */
    int esem_command_profile;
    /**
     * \brief
     *   Network QOS ID
     */
    int nwk_qos_id;
    /**
     * \brief
     *   has control word.
     */
    int has_cw;
    /**
     * \brief
     *   indication for evpn iml.
     */
    int is_iml;
    /**
     * \brief
     *   outlif profile
     */
    int out_lif_profile;
    /**
     * \brief
     *   outlif llvp profile
     */
    int out_lif_llvp_profile;
    /**
     * \brief
     *   Phase of EEDB access
     */
    int outlif_phase;
    /**
     * \brief
     *   protection pointer
     */
    int protection_pointer;
    /**
     * \brief
     *   protection path
     */
    int protection_path;
    /**
     * \brief
     *   require esem_name_space in the entry
     */
    int require_name_space;
} dnx_mpls_encap_t;

/*
 * }
 */

/**
 * \brief
 *   Initializes the input/output structure for mpls encap set/get functions.
 * \param [in] unit - Unit #
 * \param [in] encap_info - Designated param
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_mpls_encap_t_init(
    int unit,
    dnx_mpls_encap_t * encap_info);

/**
* \brief
* Fill MPLS encap info according to local outlif
*   \param [in] unit         -  Relevant unit.
*   \param [in] entry_handle_id -  DBAL handle id of type superset
*   \param [in] outlif_info  - Local Outlif Information.
*   \param [out] encap_info  - MPLS encapsulation information.
*
* \return
*   Negative in case of an error. See \ref shr_error_e
* \remark
 */
shr_error_e dnx_mpls_lif_table_manager_to_encap_info(
    int unit,
    uint32 entry_handle_id,
    lif_table_mngr_outlif_info_t * outlif_info,
    dnx_mpls_encap_t * encap_info);

/**
* \brief
* Fill MPLS outlif info. This will be input to result type decision
*   \param [in] unit         -  Relevant unit.
*   \param [in] entry_handle_id -  DBAL handle id of type superset
*   \param [in] encap_info   - MPLS/MPLS PORT encapsulation information.
*   \param [out] outlif_info  - Local Outlif Information.
*
* \return
*   Negative in case of an error. See \ref shr_error_e
* \remark
 */
shr_error_e dnx_mpls_encap_to_lif_table_manager_info(
    int unit,
    uint32 entry_handle_id,
    dnx_mpls_encap_t * encap_info,
    lif_table_mngr_outlif_info_t * outlif_info);

/**
* \brief
*  Calculates the number of additional headers required .
*   \param [in] has_cw     -  whether cw is resent.
*   \param [in] flags         -  label flags, whether EL or ELI required.
* \return
*   number of additional headers used, 0 if non used.
*/
int dnx_mpls_nof_additional_headers(
    int has_cw,
    uint32 flags);

/**
* \brief
*  Calculate the required result type to be used for the EEDB entry based on label flags and nof labels.
*   \param [in] unit     -  Relevant unit.
*   \param [in] flags     -  Label flags.
*   \param [in] tunnel_type     -  Tunnel type indicating on number of labels to be defined for the current EEDB entry (0, 1 or 2)
* \return
*   the relevant result type to be used
*/
dbal_enum_value_result_type_eedb_mpls_tunnel_e dnx_mpls_tunnel_egress_result_type_get(
    int unit,
    uint32 flags,
    int tunnel_type);

/**
* \brief
*  Set lif, TTL and QoS profiles on 'encap_info'.
*  In case REPLACE is set, update the old profiles from 'encap_info_orig' with new ones.
*   \param [in] unit -Relevant unit.
*   \param [in] update -if set perform REPLACE
*   \param [in] network_group_id - Split Horizon network group identifier.
*   \param [in] dbal_table_id - dbal table id indicating the application - PWE/MPLS/EVPN.
*   \param [in] encap_info_orig - original encapsulation information, used in case REPLACE flag is set
*   \param [out] encap_info - encapsulation parameters.
*   \param [out] ttl_pipe_profile_first_reference - indicate if the new ttl_pipe_profile is the first reference.
*   \param [out] ttl_pipe_profile_last_reference - indicate if the old ttl_pipe_profile is the last reference.
*   \param [out] qos_pipe_profile_first_reference - indicate if the new qos_pipe_profile is the first reference.
*   \param [out] qos_pipe_profile_last_reference - indicate if the old qos_pipe_profile is the last reference.
* \return
*   shr_error_e
*/
shr_error_e dnx_mpls_egress_lif_ttl_qos_profiles_sw_allocate(
    int unit,
    int update,
    bcm_switch_network_group_t network_group_id,
    dbal_tables_e dbal_table_id,
    dnx_mpls_encap_t encap_info_orig,
    dnx_mpls_encap_t * encap_info,
    uint8 *ttl_pipe_profile_first_reference,
    uint8 *ttl_pipe_profile_last_reference,
    uint8 *qos_pipe_profile_first_reference,
    uint8 *qos_pipe_profile_last_reference);

/**
 * \brief
 * Fill the dnx_jr_mode_mpls_push_profile_t structure
 * According to the information in bcm_mpls_tunnel_switch_t
 *
 * \param [in] unit - Relevant unit.
 * \param [in] label_entry - A pointer to a struct containing relevant
 *        information for the egress label.
 * \param [in] has_cw - Ctrl word.
 * \param [out] push_profile_info  -  A pointer to the struct representing the tunnel push profile entry.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mpls_label_info_to_push_profile(
    int unit,
    bcm_mpls_egress_label_t * label_entry,
    uint8 has_cw,
    dnx_jr_mode_mpls_push_profile_t * push_profile_info);

/**
 * \brief
 * convert push profile to mpls label entry information.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] push_cmd - push profile ID
 * \param [out] mpls_port - mpls label entry
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_push_profile_to_mpls_label_info(
    int unit,
    int push_cmd,
    bcm_mpls_port_t * mpls_port);
/**
* \brief
*  Create MPLS tunnels in the egress for push with EEI.
* \param [in] unit         - Relevant unit.
* \param [in] label_entry  - mpls push information.
* \param [in] has_cw       - whether carry control word
* \param [in] is_replace - exchange operation.
* \param [out] push_cmd  - push profile id used by EEI.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mpls_tunnel_create_push_entry_from_eei(
    int unit,
    bcm_mpls_egress_label_t * label_entry,
    uint8 has_cw,
    uint8 is_replace,
    int *push_cmd);

/**
* \brief
*  Delete MPLS tunnels in the egress for push with EEI.
* \param [in] unit         - Relevant unit.
* \param [in] push_cmd  - push profile id used by EEI.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mpls_tunnel_delete_push_entry_from_eei(
    int unit,
    int push_cmd);

/**
* \brief
*  Retrieve encap information from push command
* \param [in] unit         - Relevant unit.
* \param [in] push_cmd  - push profile id used by EEI.
* \param [out] encap_info  - mpls encapsulation information.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mpls_encap_get_from_push_cmd(
    int unit,
    int push_cmd,
    dnx_mpls_encap_t * encap_info);
#endif  /*_MPLS_TUNNEL_ENCAP_INCLUDED__*/
