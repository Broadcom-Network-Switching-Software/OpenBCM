/**
 * \file algo_port_pp.h
 * Internal DNX Port PP Managment APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_PORT_PP_H_INCLUDED
/*
 * {
 */
#define ALGO_PORT_PP_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>
#include <bcm_int/dnx/algo/port_pp/algo_port_tpid.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief
 * Template name defines for Port force forward.
 */
#define DNX_PORT_PP_FORCE_FORWARD_TEMPLATE "DNX_PORT_PP_FORCE_FORWARD_TEMPLATE"

/**
 * \brief
 * Template name defines for egress sit profile.
 * \see
 * dnx_port_sit_profile_alloc
 */
#define DNX_ALGO_EGRESS_PP_PORT_SIT_PROFILE_TEMPLATE "EGRESS PORT SIT PROFILE"

/**
 * \brief
 *  Template for managing ESEM access commands.
 * \see
 *  algo_port_esem_access_cmd_template_init
 */
#define DNX_ALGO_TEMPLATE_ESEM_ACCESS_CMD  "ESEM access command"

/**
 * \brief
 *  Resource for managing esem default result profiles.
 * \see
 *  dnx_port_lif_esem_default_result_profile_init
 */
#define DNX_ALGO_ESEM_DEFAULT_RESULT_PROFILE "Egress SEM default result profile IDs resource"

/**
 * \brief
 *  Template name defines for pp port lb profile enablers
 * \see
 *  algo_pp_port_lb_profile_template_init
 */
#define DNX_ALGO_PP_PORT_LB_LAYER_PROFILE_ENABLERS "pp_port_lb_profile"

/*
 * Number of tags used in ESEM access command management:
 * Not all result types have the same size for holding the full length of the ESEM CMD,
 * so they have a limited range of ESEM CMDsm the smallest range will be our tag size,
 * as this is our smallest unit.
 */
#define DNX_ALGO_PP_PORT_ESEM_CMD_RES_NOF_TAGS (1 << (dnx_data_esem.access_cmd.nof_cmds_size_in_bits_get(unit) - dnx_data_esem.access_cmd.min_size_for_esem_cmd_get(unit)))

/**
 * }
 */

/**
 * \brief
 * Structure used for both sit relevant module internal implementation and template MNGR SIT profile algo
 */

typedef struct
{
    /**
      * Storage for sit tag type
      */
    dbal_enum_value_field_sit_tag_type_e tag_type;
    /**
      * Tpid value of the sit tag
      */
    uint16 tpid;
    /**
      * pcp dei value source, see dbal_enum_value_field_sit_pcp_dei_src_e
      */
    dbal_enum_value_field_sit_pcp_dei_src_e pcp_dei_src;
    /**
      * vid value source, see dbal_enum_value_field_sit_vid_src_e
      */
    dbal_enum_value_field_sit_vid_src_e vid_src;
} dnx_sit_profile_t;

/**
 * \brief
 *  Enum for ESEM access interface
 */
typedef enum dnx_esem_access_if_e
{
    /*
     * ESEM access interface 1.
     */
    ESEM_ACCESS_IF_1 = 0,

    /*
     * ESEM access interface 2.
     */
    ESEM_ACCESS_IF_2 = 1,

    /*
     * ESEM access interface 3.
     */
    ESEM_ACCESS_IF_3 = 2,

    /*
     * Number of ESEM access interfaces.
     */
    ESEM_ACCESS_IF_COUNT = 3
} dnx_esem_access_if_t;

/**
 * \brief
 *  Structure used for exchanging esem default result profiles
 */
typedef struct
{
    /**
     * Enable ESEM access in the cmd: TRUE or FALSE.
     */
    uint8 valid;

    /**
     * Application key-type for the ESEM access.
     */
    uint8 app_db_id;

    /**
     * Offset for ESEM entry from the access, based on current ESS entry.
     */
    uint8 designated_offset;

    /**
     * Default ESEM entry index for the access, used in case of no match in esem.
     */
    uint8 default_result_profile;

} dnx_esem_cmd_if_data_t;
/**
 * \brief
 *  Structure used for exchanging ESEM CMD profiles
 */
typedef struct dnx_esem_cmd_data_s
{
    /*
     * ESEM access information.
     */
    dnx_esem_cmd_if_data_t esem[ESEM_ACCESS_IF_COUNT];
    /*
     * Range_unit is a counter for the times that the same dnx_esem_cmd_if_data_t object was saved to the allocation
     * manager before (default is zero); in the case that the same dnx_esem_cmd_if_data_t objects needs to be allocated
     * in different ranges the difference between them will be this range unit.
     * The difference above is needed because ESEM CMD data is managed by smart template that uses hash over the object
     * in order to determine data equality
     */
    uint8 range_unit;
} dnx_esem_cmd_data_t;

/**
 * \brief -
 *   Initialize Port PP algorithms.
 *   The function creates Port PP Templates:
 *   - PORT TPID Template profiles for both ingress and egress.
 *
 * \param [in] unit -
 *      Relevant unit.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  Please refer to DNX Init/Deinit sequence.
 */
shr_error_e dnx_algo_port_pp_init(
    int unit);

/**
 * \brief -
 *   Deintialize port TPID algorithm.
 *   The function destroys Port PP Tempaltes:
 *   - PORT TPID Template profiles for both ingress and egress.
 * \param [in] unit -
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  Please refer to DNX Init/Deinit sequence.
 */
shr_error_e dnx_algo_port_pp_deinit(
    int unit);

/**
 * Force forward
 * {
 */

/**
 * \brief -
 *   print Callback function for template manager.
 *   For details about the parameters see dnx_algo_template_print_data_cb
 */
void dnx_algo_port_pp_force_forward_template_cb(
    int unit,
    const void *data);

/**
 * \brief -
 *   Print the sit profile template content.
 * \param [in] unit - relevant unit
 * \param [in] data - templat data
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 */
void dnx_algo_egress_sit_profile_template_print_cb(
    int unit,
    const void *data);

/**
 * \brief -
 *   Print the esem access cmd template content.
 * \param [in] unit - relevant unit
 * \param [in] data - templat data
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 */
void dnx_algo_port_esem_access_cmd_template_print_cb(
    int unit,
    const void *data);

/**
 * \brief -
 *   Print the egress acceptable frame type profile template content.
 * \param [in] unit - relevant unit
 * \param [in] data - templat data
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 */
void dnx_algo_egress_acceptable_frame_type_profile_template_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Print a pp_port_lb_profile template entry.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] data - pointer to data of type uint32 which
 *       includes represents the layers per which lb profile is disabled/enabled
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   * None
 */
void dnx_algo_pp_port_lb_profile_print_entry_cb(
    int unit,
    const void *data);

/*
 * }
 */
#endif /* ALGO_PORT_PP_H_INCLUDED */
