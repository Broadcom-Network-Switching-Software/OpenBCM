 /** \file crps_src_interface.h
 * 
 *  DNX CRPS SRC_INTERFACE H FILE. (INTERNAL CRPS FILE)
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _DNX_CRPS_SRC_INTERFACE_INCLUDED__
/*
 * { 
 */
#define _DNX_CRPS_SRC_INTERFACE_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#define DNX_CRPS_EGRESS_COMPENSATION_DELTA_VALUE_MIN (-64)
#define DNX_CRPS_EGRESS_COMPENSATION_DELTA_VALUE_MAX (63)

/*
 * MACROs
 * {
 */

/*
 * }
 */

/** see .h file */
shr_error_e dnx_crps_src_interface_expansion_type_offset_get(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t src,
    int command_id,
    bcm_stat_expansion_types_t field_type,
    uint32 *offset);

/**
* \brief
*   handle the epp expansion selection set calculation and writing to HW.
* \par DIRECT INPUT:
*   \param [in] unit    -  Unit-ID
*   \param [in] interface  -  this is the key -defined the interface by core, src, command_id and type
*   \param [in] selection  -  expansion select bitmap
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_crps_src_interface_expansion_select_hw_set(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    uint32 *selection);

/**
* \brief
*   Convert selection array bitmap to expansion elements array
* \par DIRECT INPUT:
*   \param [in] unit    -  Unit-ID
*   \param [in] core_id    -  core id
*   \param [in] src  -  the source block
*   \param [in] command_id  - command id
*   \param [in] selection_bitmap - expansion selection bitmap
*   \param [out] expansion_select  -  array of expansion type structures
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_crps_src_interface_expansion_bitmap_convert(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t src,
    int command_id,
    uint32 *selection_bitmap,
    bcm_stat_expansion_select_t * expansion_select);

/** see .h file */
shr_error_e dnx_crps_src_interface_expansion_select_hw_get(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    uint32 *val_arr);
/**
 * \brief
 *   expansion selection calculation. go over all the expansion selection elements from API and build a bitmap of expansion selection
 *  need to select from x bits meta-data only max 10 bits.
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] core_id - core_id
 * \par INDIRECT INPUT:
 *   \param [in] src  -  the source block.
 *   \param [in] command_id  - command/object id
 *   \param [in] expansion_select  -  configuration structure
 *   \param [out] ret_val  -  the return value for the expansion selection bit map.  define which bits to consider when building the expansion.
 * \par INDIRECT INPUT:
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_crps_src_interface_expansion_select_calc(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t src,
    int command_id,
    bcm_stat_expansion_select_t * expansion_select,
    uint32 *ret_val);

/**
 * \brief 
 *  check if a given field is selected to be part of the expansion selection format.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit    -  Unit-ID 
 *   \param [in] core_id    -  core_id
 *   \param [in] src    -  the source block
 *   \param [in] command_id    -  the command_id
 *   \param [in]  field_type - the fiels that the user wants to check if selected 
 *   \param [in]  src_bitmap - the value of the HW expansion bit map.
 *   \param [out] dest_bitmap  - update the bitmap taken from the hw. NOt all bits must be set for a given type
 *   \param [out] is_selected  - is the given field was part of the expansion format.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_crps_src_interface_expansion_field_is_selected(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t src,
    int command_id,
    bcm_stat_expansion_types_t field_type,
    uint32 *src_bitmap,
    uint32 *dest_bitmap,
    int *is_selected);

/**
 * \brief
 *      init the etpp/epp meta data format configuration. Init it to be empty. Meaning it will not include any data
 * \param [in] unit -
 *     unit id
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_src_interface_dynamic_meta_data_selection_init(
    int unit);

/**
 * \brief
 *      configure the etpp metadata format, base on the expansion selection API
 * \param [in] unit -
 *     unit id
 * \param [in] interface - interface strucutre as given in the expansion selection API. 
 * \param [in] expansion_select - expansion_select strucutre as given in the expansion selection API.  
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_src_interface_dynamic_metadata_format_config(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select);

/**
 * \brief
 *      specific hw configuration that need to do if the source is egress queue
 * \param [in] unit -unit id
 * \param [in] core_id - core_id
 * \param [in] enable -enable or disable the counter pointer from the egress tm
 * \return
 *    shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_src_egress_queue_hw_set(
    int unit,
    bcm_core_t core_id,
    int enable);

/**
* \brief
*      wrapper for specific configuration per source. Called from API bcm_stat_counter_interface_set
* \param [in] unit -unit id
* \param [in] core_id - core_id
* \param [in] config -interface config strucutre as sent from API. will be used to know the src and command_id.
* \return
*      shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_crps_src_config_set(
    int unit,
    int core_id,
    bcm_stat_counter_interface_t * config);

/**
* \brief
*      for dbal table CRPS_INGRESS_COMPENSATION_MASKS set mask
*      depending on the select - 0 - set mask for IRPP-0, 1 -
*      set mask for IRPP-1, 2- set mask for ITM
* \param [in] unit -unit id
* \param [in] core_id - core_id 
* \param [in] select - select which mask to be set
* \param [in] mask - mask value to be set
* \return
*      shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_crps_src_ingress_compensation_mask_hw_set(
    int unit,
    bcm_core_t core_id,
    int select,
    uint32 mask);

/**
* \brief
*      for dbal table CRPS_INGRESS_COMPENSATION_MASKS get mask
*      from field depending on the select - 0 - get mask for
*      IRPP-0, 1 - get mask for IRPP-1, 2- get mask for ITM
* \param [in] unit -unit id
* \param [in] core_id - core_id 
* \param [in] select - select which mask value to get
* \param [out] mask - mask value to be set
* \return
*      shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_crps_src_ingress_compensation_mask_hw_get(
    int unit,
    bcm_core_t core_id,
    int select,
    uint32 *mask);

/**
* \brief
*      for dbal table CRPS_INGRESS_COMPENSATION_SELECT set field
*      CGM_IRPP_PACKET_SIZE_SELECT
* \param [in] unit -unit id
* \param [in] core_id - core_id
* \param [in] command_id - command_id
* \param [in] select - selected mask - 1 for selecting packet
*        size A, 0 for packet size B
* \return
*      shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_crps_src_irpp_compensation_mask_select_set(
    int unit,
    bcm_core_t core_id,
    int command_id,
    uint8 select);

/**
* \brief
*      for dbal table CRPS_INGRESS_COMPENSATION_SELECT set field
*      CGM_IRPP_PACKET_SIZE_SELECT
* \param [in] unit -unit id
* \param [in] core_id - core_id
* \param [in] command_id - command_id
* \param [out] select - selected mask - 1 for selecting packet
*        size A, 0 for packet size B
* \return
*      shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_crps_src_irpp_compensation_mask_select_get(
    int unit,
    bcm_core_t core_id,
    int command_id,
    uint8 *select);

/**
 * \brief
 *      init irpp and itm compensation masks values
 * \param [in] unit -
 *     unit id
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_src_interface_compensation_init(
    int unit);

/**
 * \brief -
 * for IRPP/ITM set which IRPP/ITM mask is used - enable/disable
 * header truncate compensation
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  flags
 * \param [in] key - core,source,command_id 
 * \param [in] select_type - header adjust select type
 * \param [in] enable - header-truncate enabled/disabled
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_crps_ingress_pkt_size_adjust_select_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type,
    int enable);

/**
 * \brief -
 * for IRPP/ITM get which IRPP/ITM mask is used - if
 * header-truncate is enabled or not
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  flags
 * \param [in] key - core,source,command_id 
 * \param [in] select_type - header adjust select type 
 * \param [out] enable - header-truncate enabled/disabled
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_crps_ingress_pkt_size_adjust_select_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type,
    int *enable);

/**
* \brief
*  set crps select latency value or packet size value. 
*  crps refer to this configuration only for engines connected to Etpp,commnd_id=5
* \param [in] unit -unit id
* \param [in] core_id - core id
* \param [in] enable - if TRUE, latency value is taken, else packet size value is taken by CRPS
* \return
*    shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_crps_src_interface_egress_latency_ctr_set(
    int unit,
    int core_id,
    int enable);
/*
 * } 
 */
#endif/*_DNX_CRPS_SRC_INTERFACE_INCLUDED__*/
