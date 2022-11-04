/** \file bcm_int/dnx/l3/l3_vrrp.h Internal DNX L3 APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef L3_VRRP_H_INCLUDED
/*
 * {
 */
#define L3_VRRP_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <bcm/switch.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include "bcm_int/dnx/init/init_pp.h"
/*
 * }
 */

/*
 * DEFINES
 * {
 */

#define DNX_L3_VRRP_LAYER_PROTOCOLS_SIZE_IN_UINT32 ((DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_LAYER_PROTOCOLS + (SAL_UINT32_NOF_BITS - 1)) / SAL_UINT32_NOF_BITS)

/*
 * }
 */
/*
 * Enumeration.
 * {
 */
/**
 * \brief enum to hold the index of each exem table to be used to access the mapping in dnx_l3_vrrp_dbal_table_exem_mapping
 */
typedef enum l3_vrrp_exem_dbal_table_indx_e
{
    L3_VRRP_TABLE_PER_PORT_MY_MAC,
    L3_VRRP_TABLE_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_0,
    L3_VRRP_TABLE_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_1,
    L3_VRRP_TABLE_VRID_MY_MAC_WITH_PROTOCOL,
    L3_VRRP_TABLE_PER_PORT_IGNORE_MAC,
    /** Number of vrrp exem dbal tables  */
    L3_VRRP_TABLE_NOF_TABLES,
} l3_vrrp_exem_dbal_table_indx_t;
/*
 * }
 */

/*
 * MACROs
 * {
 */

/**
 * \brief - list of supported L3 protocols for VRRP
 */

/*
 * }
 */
/*
 * Globals
 * {
 */

/*
 * }
 */

/*
 * Structures
 * {
 */

/*
 * }
 */

/*
 * Function declaration
 * {
 */

/**
 * \brief - init procedure for l3_vrrp (multiple my mac) module.
 * * Set default groups members (IPV4, IPV6 and the rest)
 * * Init template and allocation managers for TCAM usage
 * * Activate MyMac EXEM table - Set FFCs, VMV
 *
 * \param [in] unit - The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_l3_vrrp_init(
    int unit);

/**
 * \brief Get l3 protocol group for multiple my mac (vrrp)
 *
 * \param [in] unit - Relevant unit
 * \param [out] group_members - combination (flags) of l3 protocols (can be either input or output).
 * \param [out] group_id - group ID (can be either input or output)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l3_vrrp_protocol_group_get(
    int unit,
    uint32 *group_members,
    bcm_l3_protocol_group_id_t * group_id);

/**
 * \brief Set l3 protocol group for multiple my mac (vrrp)
 *
 * \param [in] unit - Relevant unit
 * \param [in] group_members - combination (flags) of l3 protocols.
 * \param [in] group_id - group ID to set
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l3_vrrp_protocol_group_set(
    int unit,
    uint32 group_members,
    bcm_l3_protocol_group_id_t group_id);

/**
 * \brief Given protocol flags, returns the minimal group_id - group_mask combination that contains all flags.
 *
 * \param [in] unit - Relevant unit
 * \param [in] members_flags - combination (flags) of l3 protocols.
 * \param [in] exact_match - If set, returns error in case the groups contain more than just the asked protocols.
 * \param [out] group_id - Group_id containing the flags
 * \param [out] group_mask - Minimal mask required to contain the flags.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l3_vrrp_members_to_protocol_group_get(
    int unit,
    uint32 *members_flags,
    uint8 exact_match,
    uint32 *group_id,
    uint32 *group_mask);

/**
 * \brief - Allocate and set a TCAM entry according to the given tcam_info:
 * 1. Allocate either a single or for all VSIs a tcam entry
 * 2. Commit the entry to HW if it's new
 * 3. Update VSI VRID members bitmap with the new allocated member
 *
 * \param [in] unit - Relevant unit
 * \param [in] tcam_info - vrrp tcam data.
 * \param [in] vlan - Matching vsi.
 * \param [out] tcam_index - Returned tcam allcoated index (profile)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l3_vrrp_setting_add(
    int unit,
    algo_dnx_l3_vrrp_tcam_key_t * tcam_info,
    int vlan,
    uint32 *tcam_index);

/**
 * \brief - Deallocate and clear a TCAM entry according to the given tcam_info:
 * 1. Deallocate either a single or for all VSIs a tcam entry
 * 2. Clear the entry in HW if it's the last
 * 3. Update VSI VRID members bitmap without the deallocated member
 *
 * \param [in] unit - Relevant unit
 * \param [in] tcam_info - vrrp tcam data.
 * \param [in] vlan - Matching vsi.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l3_vrrp_setting_delete(
    int unit,
    algo_dnx_l3_vrrp_tcam_key_t * tcam_info,
    int vlan);

/**
 * \brief - Get protocol group (group id can be masked) members (set of l3 protocols). The members are represented as
 * a bitmap of protocols.
 * The mapping is maintained in a dedicated SW state db.
 *
 * \param [in] unit - Relevant unit
 * \param [in] group_id - protocol group id.
 * \param [in] group_mask -protocol group mask.
 * \param [out] members_bmp - protocol layer bitmap (\see dbal_enum_value_field_layer_types_e).
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l3_vrrp_protocol_group_members_sw_get(
    int unit,
    int group_id,
    int group_mask,
    uint32 *members_bmp);

/**
 * \brief - Clear and deallocate EXEM based TCAM profile according to its data
 *
 * \param [in] unit - Relevant unit
 * \param [in] tcam_info - tcam info to clear.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l3_vrrp_exem_based_tcam_info_clear(
    int unit,
    algo_dnx_l3_vrrp_tcam_key_t * tcam_info);

/**
 * \brief - add EXEM SOURCE-PORT Multiple MY-MAC entry
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - logical source port. relevant for src-port result type
 * \param [in] mac_da - destination mac address
 * \param [out] index - allocated EXEM sw index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l3_vrrp_exem_source_port_my_mac_add(
    int unit,
    bcm_port_t port,
    bcm_mac_t mac_da,
    int *index);

/**
 * \brief - add EXEM per port protocold entry
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - logical source port. relevant for src-port result type
 * \param [in] protocol - ethernet type
 * \param [out] index - allocated EXEM sw index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l3_vrrp_exem_source_port_protocol_add(
    int unit,
    bcm_port_t port,
    uint32 protocol,
    int *index);

/**
 * \brief - add EXEM VSI Multiple MY-MAC entry
 *
 * \param [in] unit - Relevant unit
 * \param [in] vlan - VSI. relevant for vsi result type
 * \param [in] mac_da - destination mac address
 * \param [out] index - allocated EXEM sw index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l3_vrrp_exem_vsi_my_mac_add(
    int unit,
    bcm_vlan_t vlan,
    bcm_mac_t mac_da,
    int *index);

/**
 * \brief - Remove EXEM Multiple MY-MAC entry from SW EXEM table and HW EXEM table and deallocate it from resource
 * Manager.
 *
 * \param [in] unit - Relevant unit
 * \param [in] index - index to sw db of the entry to remove
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l3_vrrp_exem_multi_my_mac_delete(
    int unit,
    int index);

/**
 * \brief - Get EXEM multiple my mac entry from SW DB according to provided index.
 * An entry is either SOURCE-PORT or VSI
 *
 * \param [in] unit - Relevant unit
 * \param [in] index - index to sw db
 * \param [out] result_type - type of the retrieved entry
 * \param [out] vlan - entry vsi id (relevant for VSI entry)
 * \param [out] core - entry core id (relevent for port entry)
 * \param [out] pp_port - entry pp_port (relevent for port entry)
 * \param [out] mac_da - entry mac address (relevant for both types)
 * \param [out] protocol - entry protocol (relevant for one types)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l3_vrrp_exem_multi_my_mac_get(
    int unit,
    int index,
    dbal_result_type_t * result_type,
    bcm_vlan_t * vlan,
    int *core,
    uint32 *pp_port,
    bcm_mac_t mac_da,
    dbal_enum_value_field_layer_types_e * protocol);

/**
 * \brief - Set (write to HW) valid VRRP TCAM entry.
 *
 * \param [in] unit - Unit ID
 * \param [in] index - TCAM index (entry ID)
 * \param [in] tcam_info - VRRP TCAM key (mac DA and protocol group)
 * \param [in] da_profile - da profile of vrrp entry
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
shr_error_e dnx_l3_vrrp_tcam_info_set(
    int unit,
    int index,
    algo_dnx_l3_vrrp_tcam_key_t * tcam_info,
    int da_profile);

/**
 * \brief - Set (write to HW) valid VRRP TCAM entry for port+protocol
 * ignore mac.
 *
 * \param [in] unit - Unit ID
 * \param [in] protocol - protocol
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
shr_error_e dnx_l3_vrrp_port_protocol_tcam_info_set(
    int unit,
    uint32 protocol);

/**
 * \brief - Clear (write to HW) valid VRRP TCAM entry.
 *
 * \param [in] unit - Unit ID
 * \param [in] index - TCAM index (entry ID)
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
shr_error_e dnx_l3_vrrp_tcam_info_clear(
    int unit,
    int index);

/*
 * }
 */
#endif /* L3_VRRP_H_INCLUDED */
