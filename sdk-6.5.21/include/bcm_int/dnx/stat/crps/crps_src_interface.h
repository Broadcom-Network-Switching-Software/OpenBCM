 /** \file crps_src_interface.h
 * 
 *  DNX CRPS SRC_INTERFACE H FILE. (INTERNAL CRPS FILE)
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_CRPS_SRC_INTERFACE_INCLUDED__
/*
 * { 
 */
#define _DNX_CRPS_SRC_INTERFACE_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
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

/**
 * \brief
 *      function return the field offset and the field size (in bits). It is depend on the source that requested.
 *      each source has different metadata format as follow:
 *      IRPP: 36 metadata format is: {pp-meta-data(16),In-DP(2), Final-DP(2), 3x(Intermidiate- DP(2),Accounted(1)),
 *                                             In-TC(3), Disposition(1), TM-Reject-Reason(3)}
 *      ITPP: 12 bits metadata format is:{Flow-ID.Profile.Meta-Data(3), VoQ-Quartet.Profile.Meta-Data(3), TC(3), Latency-Bin(3)} 
 *      ERPP:16 bits metadata format is: {2'b0, PP-Drop(2), TM-Action-Type(2), TM-Is-MC(1), TC(3), DP(2), Counter-Data(4)} 
 *      ITM:  10 bits metadata format is: {1'b0, DP(2),  TC(3), disposition(1), Drop-reason(3)} 
 *      ETM: 10 bits metadata format is: : {DP[1:0], TC(3), Is-Sys-MC, IRPP-Discard, TM-Discard, TM-Drop-Reason}  
 *      ETPP:24 bits metadata format is user configuration. (it is not constant like other sources). therefore, the function get it from another db.. 
 * \par DIRECT_INPUT:
 *   \param [in] unit    -  Unit-ID 
 *   \param [in] core_id    -  core-ID  
 *   \param [in] src  -  the source block
 *   \param [in] command_id  -  the object index. relevant for Etpp source only
 *   \param [in] field_type  -  the field that use wants to gets it's offset and size 
 *   \param [out] size  -  size of the field in bits
 *   \param [out] field_offset  -  bit number that the field starts. (in case of NULL, it will not be updated.)
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
shr_error_e dnx_crps_src_interface_data_field_params_get(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t src,
    int command_id,
    bcm_stat_expansion_types_t field_type,
    uint32 *size,
    uint32 *field_offset);

/**
* \brief  
*   handle the irpp expansion selection set calculation and writing to HW.
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] interface  -  this is the key -defined the interface by core, src, command_id and type
*   \param [in] expansion_select  -  configuration structure
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
shr_error_e dnx_crps_src_interface_irpp_expansion_select_set(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select);

/**
* \brief  
*   handle the itpp expansion selection set calculation and writing to HW.
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] interface  -  this is the key -defined the interface by core, src, command_id and type
*   \param [in] expansion_select  -  configuration structure
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
shr_error_e dnx_crps_src_interface_itpp_expansion_select_set(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select);

/**
* \brief  
*   handle the erpp expansion selection set calculation and writing to HW.
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] interface  -  this is the key -defined the interface by core, src, command_id and type
*   \param [in] expansion_select  -  configuration structure
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
shr_error_e dnx_crps_src_interface_erpp_expansion_select_set(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select);

/**
* \brief  
*   handle the etpp expansion selection set calculation and writing to HW.
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] interface  -  this is the key -defined the interface by core, src, command_id and type
*   \param [in] expansion_select  -  configuration structure
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
shr_error_e dnx_crps_src_interface_etpp_expansion_select_set(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select);

/**
* \brief  
*   fill the expansion selection strucutre, base on the HW expansion selection bitmap taken from HW .
* \par DIRECT INPUT: 
*   \param [in] unit              -  unid id
*   \param [in] core_id        -  core_id
*   \param [in] src               -  the source block
*   \param [in] command_id  -  the command_id
*   \param [in] selection_arr     -  the HW expansion selection bitmap
*   \param [in] valid_types       -  array of valid types that need to check for each source
*   \param [out] expansion_select  -  the structure to fill
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
shr_error_e dnx_crps_src_interface_expansion_select_get(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t src,
    int command_id,
    uint32 *selection_arr,
    const bcm_stat_expansion_types_t * valid_types,
    bcm_stat_expansion_select_t * expansion_select);

/**
* \brief  
*   handle the irpp expansion selection reading from HW.
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] interface  -  this is the key -defined the interface by core, src, command_id and type
*   \param [out] val_arr  -  hw expansion selection bitmap
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
shr_error_e dnx_crps_src_interface_irpp_expansion_select_hw_get(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    uint32 *val_arr);

/**
* \brief  
*   handle the itpp expansion selection reading from HW.
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] interface  -  this is the key -defined the interface by core, src, command_id and type
*   \param [out] selection  -  hw expansion selection bitmap
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
shr_error_e dnx_crps_src_interface_itpp_expansion_select_hw_get(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    uint32 *selection);

/**
* \brief  
*   handle the erpp expansion selection reading from HW.
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] interface  -  this is the key -defined the interface by core, src, command_id and type
*   \param [out] selection  -  hw expansion selection bitmap
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
shr_error_e dnx_crps_src_interface_erpp_expansion_select_hw_get(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    uint32 *selection);

/**
* \brief  
*   handle the etpp expansion selection reading from HW.
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] interface  -  this is the key -defined the interface by core, src, command_id and type
*   \param [out] selection  -  hw expansion selection bitmap
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
shr_error_e dnx_crps_src_interface_etpp_expansion_select_hw_get(
    int unit,
    bcm_stat_counter_interface_key_t * interface,
    uint32 *selection);

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
 *   \param [in]  data - the value of the HW expansion bit map.  
 *   \param [out] is_selected  - is the given field was part of the expansion format.
 *   \param [out] bitmap  - update the bitmap taken from the hw. NOt all bits must be set for a given type
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
    uint32 *data,
    int *is_selected,
    uint32 *bitmap);

/**
 * \brief
 *      init the etpp meta data format configuration. Init it to be empty. Meaning it will not include any data 
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
shr_error_e dnx_crps_src_interface_etpp_meta_data_selection_init(
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
shr_error_e dnx_crps_src_interface_etpp_metadata_format_config(
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
shr_error_e dnx_crps_src_interface_etpp_latency_ctr_set(
    int unit,
    int core_id,
    int enable);
/*
 * } 
 */
#endif/*_DNX_CRPS_SRC_INTERFACE_INCLUDED__*/
