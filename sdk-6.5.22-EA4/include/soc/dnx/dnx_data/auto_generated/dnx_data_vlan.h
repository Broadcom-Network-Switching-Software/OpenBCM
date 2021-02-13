/** \file dnx_data_vlan.h
 * 
 * MODULE DATA INTERFACE - 
 * 
 * Device Data
 * SW component that maintains per device data
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h'
 *        
 *     
 * 
 * AUTO-GENERATED BY AUTOCODER!
 * DO NOT EDIT THIS FILE!
 */
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_VLAN_H_
/*{*/
#define _DNX_DATA_VLAN_H_
/*
 * INCLUDE FILES:
 * {
 */
#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_vlan.h>
/*
 * }
 */

/*!
* \brief This file is only used by DNX (JR2 family). Including it by
* software that is not specific to DNX is an error.
*/
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * MODULE FUNCTIONS:
 * {
 */
/**
 * \brief Init default data structure - dnx_data_if_vlan
 * 
 * \param [in] unit - Unit #
 * 
 * \return
 *     rv - see 'shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
shr_error_e dnx_data_if_vlan_init(
    int unit);

/*
 * }
 */

/*
 * SUBMODULE  - VLAN_TRANSLATE:
 * VLAN translate data
 * {
 */
/*
 * Table value structure
 */
/*
 * Feature enum
 */
/**
 * \brief 
 * Submodule features
 */
typedef enum
{

    /**
     * Must be last one!
     */
    _dnx_data_vlan_vlan_translate_feature_nof
} dnx_data_vlan_vlan_translate_feature_e;

/*
 * Submodule functions typedefs
 */
/**
 * \brief get submodule feature - disabled/enabled
 * 
 * \param [in] unit - Unit #
 * \param [in] feature - feature to get (see enum definition for more details
 * 
 * \return
 *     is_supported - returns 1 if and only if supported and 0 otherwise
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef int(
    *dnx_data_vlan_vlan_translate_feature_get_f) (
    int unit,
    dnx_data_vlan_vlan_translate_feature_e feature);

/**
 * \brief returns define data of nof_vlan_tag_formats
 * Module - 'vlan', Submodule - 'vlan_translate', data - 'nof_vlan_tag_formats'
 * Number of supported VLAN tag formats
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_vlan_tag_formats - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_vlan_vlan_translate_nof_vlan_tag_formats_get_f) (
    int unit);

/**
 * \brief returns define data of nof_ingress_vlan_edit_profiles
 * Module - 'vlan', Submodule - 'vlan_translate', data - 'nof_ingress_vlan_edit_profiles'
 * Number of supported VLAN edit profiles on Ingress
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_ingress_vlan_edit_profiles - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_profiles_get_f) (
    int unit);

/**
 * \brief returns define data of nof_egress_vlan_edit_profiles
 * Module - 'vlan', Submodule - 'vlan_translate', data - 'nof_egress_vlan_edit_profiles'
 * Number of supported VLAN edit profiles on Egress
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_egress_vlan_edit_profiles - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_profiles_get_f) (
    int unit);

/**
 * \brief returns define data of nof_vlan_range_entries
 * Module - 'vlan', Submodule - 'vlan_translate', data - 'nof_vlan_range_entries'
 * Number of Ingress VLAN Compression range entry
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_vlan_range_entries - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_vlan_vlan_translate_nof_vlan_range_entries_get_f) (
    int unit);

/**
 * \brief returns define data of nof_vlan_ranges_per_entry
 * Module - 'vlan', Submodule - 'vlan_translate', data - 'nof_vlan_ranges_per_entry'
 * Number of Ingress VLAN Compression ranges per entry
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_vlan_ranges_per_entry - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_vlan_vlan_translate_nof_vlan_ranges_per_entry_get_f) (
    int unit);

/**
 * \brief returns define data of egress_tpid_outer_packet_index
 * Module - 'vlan', Submodule - 'vlan_translate', data - 'egress_tpid_outer_packet_index'
 * EVE command - the TPID index indicating to use the packet's outer TPID
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     egress_tpid_outer_packet_index - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_vlan_vlan_translate_egress_tpid_outer_packet_index_get_f) (
    int unit);

/**
 * \brief returns define data of egress_tpid_inner_packet_index
 * Module - 'vlan', Submodule - 'vlan_translate', data - 'egress_tpid_inner_packet_index'
 * EVE command - the TPID index indicating to use the packet's inner TPID
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     egress_tpid_inner_packet_index - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_vlan_vlan_translate_egress_tpid_inner_packet_index_get_f) (
    int unit);

/**
 * \brief returns numeric data of nof_ingress_vlan_edit_action_ids
 * Module - 'vlan', Submodule - 'vlan_translate', data - 'nof_ingress_vlan_edit_action_ids'
 * Number of supported ingress VLAN edit action_ids
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_ingress_vlan_edit_action_ids - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_action_ids_get_f) (
    int unit);

/**
 * \brief returns numeric data of nof_egress_vlan_edit_action_ids
 * Module - 'vlan', Submodule - 'vlan_translate', data - 'nof_egress_vlan_edit_action_ids'
 * Number of supported egress VLAN edit action_ids
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_egress_vlan_edit_action_ids - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_action_ids_get_f) (
    int unit);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_VLAN - VLAN_TRANSLATE:
 * {
 */
/**
 * \brief Interface for vlan vlan_translate data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_vlan_vlan_translate_feature_get_f feature_get;
    /**
     * returns define data of nof_vlan_tag_formats
     */
    dnx_data_vlan_vlan_translate_nof_vlan_tag_formats_get_f nof_vlan_tag_formats_get;
    /**
     * returns define data of nof_ingress_vlan_edit_profiles
     */
    dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_profiles_get_f nof_ingress_vlan_edit_profiles_get;
    /**
     * returns define data of nof_egress_vlan_edit_profiles
     */
    dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_profiles_get_f nof_egress_vlan_edit_profiles_get;
    /**
     * returns define data of nof_vlan_range_entries
     */
    dnx_data_vlan_vlan_translate_nof_vlan_range_entries_get_f nof_vlan_range_entries_get;
    /**
     * returns define data of nof_vlan_ranges_per_entry
     */
    dnx_data_vlan_vlan_translate_nof_vlan_ranges_per_entry_get_f nof_vlan_ranges_per_entry_get;
    /**
     * returns define data of egress_tpid_outer_packet_index
     */
    dnx_data_vlan_vlan_translate_egress_tpid_outer_packet_index_get_f egress_tpid_outer_packet_index_get;
    /**
     * returns define data of egress_tpid_inner_packet_index
     */
    dnx_data_vlan_vlan_translate_egress_tpid_inner_packet_index_get_f egress_tpid_inner_packet_index_get;
    /**
     * returns numeric data of nof_ingress_vlan_edit_action_ids
     */
    dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_action_ids_get_f nof_ingress_vlan_edit_action_ids_get;
    /**
     * returns numeric data of nof_egress_vlan_edit_action_ids
     */
    dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_action_ids_get_f nof_egress_vlan_edit_action_ids_get;
} dnx_data_if_vlan_vlan_translate_t;

/*
 * }
 */

/*
 * SUBMODULE  - LLVP:
 * Link Layer VLAN Processing
 * {
 */
/*
 * Table value structure
 */
/*
 * Feature enum
 */
/**
 * \brief 
 * Submodule features
 */
typedef enum
{

    /**
     * Must be last one!
     */
    _dnx_data_vlan_llvp_feature_nof
} dnx_data_vlan_llvp_feature_e;

/*
 * Submodule functions typedefs
 */
/**
 * \brief get submodule feature - disabled/enabled
 * 
 * \param [in] unit - Unit #
 * \param [in] feature - feature to get (see enum definition for more details
 * 
 * \return
 *     is_supported - returns 1 if and only if supported and 0 otherwise
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef int(
    *dnx_data_vlan_llvp_feature_get_f) (
    int unit,
    dnx_data_vlan_llvp_feature_e feature);

/**
 * \brief returns numeric data of ext_vcp_en
 * Module - 'vlan', Submodule - 'llvp', data - 'ext_vcp_en'
 * Extended Vlan Classification enable
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     ext_vcp_en - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_vlan_llvp_ext_vcp_en_get_f) (
    int unit);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_VLAN - LLVP:
 * {
 */
/**
 * \brief Interface for vlan llvp data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_vlan_llvp_feature_get_f feature_get;
    /**
     * returns numeric data of ext_vcp_en
     */
    dnx_data_vlan_llvp_ext_vcp_en_get_f ext_vcp_en_get;
} dnx_data_if_vlan_llvp_t;

/*
 * }
 */

/*
 * MODULE INTERFACE DNX_DATA_IF_VLAN:
 * {
 */
/**
 * \brief Interface for vlan data
 */
typedef struct
{
    /**
     * Interface for vlan vlan_translate data
     */
    dnx_data_if_vlan_vlan_translate_t vlan_translate;
    /**
     * Interface for vlan llvp data
     */
    dnx_data_if_vlan_llvp_t llvp;
} dnx_data_if_vlan_t;

/*
 * }
 */
/*
 * Extern:
 * {
 */
/**
 * \brief
 * Module structured interface - used to get the required data stored in dnx data
 */
extern dnx_data_if_vlan_t dnx_data_vlan;
/*
 * }
 */
/*}*/
#endif /*_DNX_DATA_VLAN_H_*/
/* *INDENT-ON* */
