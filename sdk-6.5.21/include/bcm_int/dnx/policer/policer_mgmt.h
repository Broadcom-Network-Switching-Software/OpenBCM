 /** \file bcm_int/dnx/policer/policer_mgmt.h
 * 
 * Internal DNX POLICER MGMT
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_POLICER_MGMT_INCLUDED__
/*
 * { 
 */
#define _DNX_POLICER_MGMT_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <include/bcm/policer.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

/*
 * MACROs
 * {
 */
#define DNX_POLICER_INGRESS_DB0_TEMPLATE_NAME    "meter ingress database-0 profiles"
#define DNX_POLICER_INGRESS_DB1_TEMPLATE_NAME    "meter ingress database-1 profiles"
#define DNX_POLICER_INGRESS_DB2_TEMPLATE_NAME    "meter ingress database-2 profiles"
#define DNX_POLICER_EGRESS_DB0_TEMPLATE_NAME    "meter egress database-0 profiles"
#define DNX_POLICER_EGRESS_DB1_TEMPLATE_NAME    "meter egress database-1 profiles"

/** kbits to bytes, for burst BCM TO DNX */
#define DNX_POLICER_KBITS_TO_BYTES(__bcm_rate)\
        ((__bcm_rate == 0) ? dnx_data_meter.profile.burst_size_min_get(unit) :((__bcm_rate)*DNX_METER_UTILS_KBITS_TO_BYTES))
    /** bytes to kbits , for burst DNX TO BCM */
#define DNX_POLICER_BYTES_TO_KBITS(__bcm_rate) ((__bcm_rate)/DNX_METER_UTILS_KBITS_TO_BYTES)

#define DNX_POLICER_CASCADE_GROUP_SIZE (4)

#define DNX_POLICER_MGMT_DP_CMD_MAX (15)

#define POLICER_MNGR_TM_CB(is_ingress, database_id, exec) ( \
        (is_ingress && (database_id == 0))         ? dnx_meter_db.policer_templates.policer_ingress_db0.exec   :\
        ((is_ingress && (database_id == 1))        ? dnx_meter_db.policer_templates.policer_ingress_db1.exec   :\
        ((is_ingress && (database_id == 2))       ? dnx_meter_db.policer_templates.policer_ingress_db2.exec   :\
        ((!is_ingress && (database_id == 0))     ? dnx_meter_db.policer_templates.policer_egress_db0.exec    :\
                                                  dnx_meter_db.policer_templates.policer_egress_db1.exec    ))))

/*
 * }
 */

/*
 * structures
 * {
 */

typedef enum
{
    /*
     *  Ignore the incoming packet color (if any) when
     *  determining the (new) color to apply to the packet. (all
     *  packets are assumed Green)
     */
    DNX_MTR_COLOR_MODE_BLIND = 0,
    /*
     *  Consider the incoming packet color (if any) when
     *  determining the (new) color to apply to the packet.
     */
    DNX_MTR_COLOR_MODE_AWARE = 1,
    /*
     *  Number of types in SOC_PPC_MTR_COLOR_MODE
     */
    DNX_NOF_MTR_COLOR_MODES = 2
} dnx_meter_color_t;

/* brief: internal strucuture that holds the profile configuration. use also for template manager!!! */
typedef struct
{
    /*
     * When set, disable the CIR. i.e., the Committed Leaky-Bucket will never give credit. The actual rate will be
     * zero. Relevant only to meter APIs; not relevant for Ethernet policers (has to be FALSE). 
     */
    uint32 disable_cir;
    /*
     * When set, disable the EIR. i.e., * the Excess Leaky-Bucket will never give credit. * The actual rate will be
     * zero. Relevant only to meter APIs; * not relevant for Ethernet policers (has to be FALSE). 
     */
    uint32 disable_eir;
    /*
     *  Committed Information Rate, in Kbps
     */
    uint32 cir;
    /*
     *  Excess Information Rate, in Kbps
     */
    uint32 eir;
    /*
     *  Max Committed Information Rate, in Kbps
     *  Used only when sharing is enabled
     */
    uint32 max_cir;
    /*
     *  Max Excess Information Rate, in Kbps
     */
    uint32 max_eir;
    /*
     *  Committed Burst Size, in bytes
     */
    uint32 cbs;
    /*
     *  Excess Burst Size, in bytes
     */
    uint32 ebs;
    /*
     *  Indicates whether the color-aware or color-blind mode is
     *  employed at the metered traffic. In T20E has to be color
     *  blind, error is returned otherwise.
     */

    dnx_meter_color_t color_mode;

    /*
     *  When TRUE, work in coupling mode. In coupling mode
     *  excess credits from the Committed Leaky Bucket is passed
     *  to the Excess Leaky Bucket. In non- coupling mode excess
     *  credits from the Committed Leaky Bucket are discarded
     *  (standard behavior). In T20E has to be FALSE, error is
     *  returned otherwise. Remarks:1. In coupling mode, the long
     *  term average bit rate of bytes in yellow service frames
     *  admitted to the network is bounded by EIR. 2. In
     *  non-coupling mode, the long term average bit rate of
     *  bytes in yellow Service Frames admitted to the network
     *  is bounded by CIR + EIR depending on volume of the
     *  offered green Service Frames.3. SrTcm (Single Rate Three
     *  color meter) is achieved by enable the coupling mode and
     *  set EIR to zero.
     */
    uint32 is_coupling_enabled;
    /*
     *  When TRUE, work in sharing mode. In sharing mode, meters
     *  are part of a hierarchical quad meters. Bucket will get
     *  credits only from higher hierarchy bucket with SharingFlag set.
     *  When FALSE, all extra credit from all shared buckets will be
     *  summed and spread according priority.
     *  ARAD only.
     */
    uint32 is_sharing_enabled;

    /*
     *  Enable packet mode,
     *  If TRUE then policing according to packet profile rate 
     *  If FALSE then policing according to bytes profile rate.
     *  Relevant only for Ethernet poilcers.
     */
    uint32 is_packet_mode;

    /*
     *  Enable packet adjust with header truncate,
     *  If TRUE then packet size will be considere of the header truncate size in irpp
     *  If FALSE then policing without header truncate size
     *  Relevant only for Ethernet poilcers and Meters.
     */
    uint32 is_pkt_truncate;

/*
    for cascade mode (sharing enable TRUE), need to have same rev exp for all buckets.
    therefore, we calculate the rev_exp in advance according to all buckets in the cascade and send it to the soc level.
    for other cases, it will be 0xFF.
*/
    uint32 ir_rev_exp;

    /*
     * data per profile that may represent something for the resolve table. for example: define if the color of the
     * meter will be black or red in case no committed or excess credits left (same as mark_black in QUX)
     */
    uint32 profile_data;

    /*
     * if set, meter is working in large bucket mode. otherwis, noraml bucket mode.
     */
    uint32 is_large_bucket_mode;

    /*
     * Indicate the Profile ID, used in order to allow two profiles with the same data
     */
    uint32 entropy_id;
} dnx_meter_profile_info_t;

/*
 * Functions prototypes
 * {
 */

/**
* \brief
*  Correct all rates in pol_cfg by the specified multiplier.
*  multiplier The amount to multiply or divide by.
*  divide If TRUE then divide the rates by multiplier.
*  Otherwise multiply by it.
* \param [in] unit -unit id
* \param [out] pol_cfg -strucutre which hold the api configuration
* \param [in] is_packets_to_kbits -If TRUE convert from packets to kbits
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_policer_correct_config_rates(
    int unit,
    bcm_policer_config_t * pol_cfg,
    int is_packets_to_kbits);

/**
 * \brief
 *      first initialization of the policer module
 * \param [in] unit -unit id
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_policer_init(
    int unit);

/**
 * \brief
 *      Deinitialize dnx policer module
 * \param [in] unit -unit id
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_policer_deinit(
    int unit);

/**
 * \brief
 *      callback print function for template manager. print the meter profile
 * \param [in] unit -unit id
 * \param [in] data -const pointer to profile data
 * \return
 *  void
 * \remark
 *   NONE
 * \see
 *   NONE
 */
void dnx_policer_mgmt_tm_meter_profile_print_cb(
    int unit,
    const void *data);

/**
* \brief
*   handle the expansion group set functionality
* \param [in] unit  - unit data
* \param [in] flags - 
* \param [in] core_id  - allow ALL
* \param [in] expansion_group  -config structure 
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_expansion_groups_set(
    int unit,
    int flags,
    bcm_core_t core_id,
    bcm_policer_expansion_group_t * expansion_group);

/**
* \brief
*   handle the expansion group get functionality
* \param [in] unit  - unit data
* \param [in] flags - 
* \param [in] core_id  - allow ALL
* \param [out] expansion_group  -config structure 
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_expansion_groups_get(
    int unit,
    int flags,
    bcm_core_t core_id,
    bcm_policer_expansion_group_t * expansion_group);

/**
* \brief
*   enable/disable the policer database in hw
* \param [in] unit         - unit id
* \param [in] flags        - 
* \param [in] core_id      - allowed ALL
* \param [in] database_id  - database_id
* \param [in] is_ingress   - is_ingress
* \param [in] is_global    - is_global
* \param [in] enable       - enable/disanle
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_database_enable_set(
    int unit,
    int flags,
    int core_id,
    int database_id,
    int is_ingress,
    int is_global,
    int enable);

/**
* \brief
*   get if  policer database id enable/disable in hw
* \param [in] unit         - unit id
* \param [in] flags        - 
* \param [in] core_id      - allowed ALL
* \param [in] database_id  - database_id
* \param [in] is_ingress   - is_ingress
* \param [in] is_global    - is_global
* \param [out] enable       - enable/disanle
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_database_enable_get(
    int unit,
    int flags,
    int core_id,
    int database_id,
    int is_ingress,
    int is_global,
    int *enable);

/**
* \brief
*   set the policer profile. handle the tm and the hw set
* \param [in] unit         - unit is
* \param [in] flags        - api flags. ignore the flags inside the structure pol_cfg for generic meter.
* \param [in] core_id      - core id
* \param [in] is_global    - global or generic meter
* \param [in] is_ingress   - ingress or egrres (relevant for generic)
* \param [in] database_id  - db is (0-2 for ingress, 0-1 for egress)
* \param [in] meter_idx    - meter id
* \param [in] pol_cfg    - configuration structure.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_policer_set(
    int unit,
    uint32 flags,
    int core_id,
    int is_global,
    int is_ingress,
    int database_id,
    int meter_idx,
    bcm_policer_config_t * pol_cfg);

/**
* \brief
*   get the policer profile from HW.
* \param [in] unit         - unit id
* \param [out] pol_cfg      - API struct with of meter profile
* \param [in] core_id      - core id
* \param [in] is_global    - global or generic meter
* \param [in] is_ingress   - ingress or egrres (relevant for generic)
* \param [in] database_id  - db is (0-2 for ingress, 0-1 for egress)
* \param [in] meter_idx    - meter id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_policer_get(
    int unit,
    int core_id,
    int is_global,
    int is_ingress,
    int database_id,
    int meter_idx,
    bcm_policer_config_t * pol_cfg);

/**
* \brief
*   destory all template managers profiles for policer and clear HW configuration
* \param [in] unit         - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   It does not destroy global meter. (only generic meters) and it does not destroy the databases, just the meters.
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_destroy_all(
    int unit);

/**
* \brief
*   create generic policer mgmt. update sw state and hw registers
* \param [in] unit         - unit id
* \param [in] flags        - none
* \param [in] core_id      - core id
* \param [in] is_ingress   - ingress or egrres (relevant for generic)
* \param [in] database_id  - db is (0-2 for ingress, 0-1 for egress)
* \param [in] config    - configuration for the database
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_database_create(
    int unit,
    int flags,
    int core_id,
    int is_ingress,
    int database_id,
    bcm_policer_database_config_t * config);

/**
 * \brief
 *      get from hw the ingress expansion mapping and calculate the number of expansion groups
 * \param [in] unit -unit id
 * \param [in] flags -flags
 * \param [in] core_id -core_id
 * \param [out] nof_expansion_groups -nof expansion groups
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_policer_mgmt_nof_expansion_groups_get(
    int unit,
    int flags,
    int core_id,
    int *nof_expansion_groups);

/**
 * \brief
 *      handle the API bcm_policer_engine_database_attach
 * \param [in] unit -unit id
 * \param [in] flags -flags
 * \param [in] engine -holds the engine_id, core and memory section.
 * \param [in] config -holds the database to connect and the base object stat id.
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_policer_mgmt_engine_database_attach(
    int unit,
    int flags,
    bcm_policer_engine_t * engine,
    bcm_policer_database_attach_config_t * config);

/**
 * \brief
 *  find the database that the engine belong to. from the engine id and section, it find the bank id and search it in all databases.
 * \param [in] unit -unit id
 * \param [in] core_id  -specific core id 
 * \param [in] engine  -engine id and section. ignore the core_id
 * \param [out] is_ingress  -found in ingress or egress database
 * \param [out] database_id  -database id that the engine+section found in.
 * \param [out] bank_index  -the index (in the bank_arr sw state) that the bank was found in 
 * \param [out] found - if the engine was found or not 
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_policer_mgmt_engine_database_find(
    int unit,
    int core_id,
    bcm_policer_engine_t * engine,
    int *is_ingress,
    int *database_id,
    int *bank_index,
    int *found);

/**
 * \brief
 *      handle the API bcm_policer_engine_database_detach
 * \param [in] unit -unit id
 * \param [in] flags -flags
 * \param [in] engine -holds the engine_id, core and memory section.
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_policer_mgmt_engine_database_detach(
    int unit,
    int flags,
    bcm_policer_engine_t * engine);

/**
 * \brief
 *      handle the API bcm_policer_engine_database_get
 * \param [in] unit - 
 * \param [in] flags - 
 * \param [in] engine - holds the core_id, engine_id and section of memory to attach
 * \param [out] config - configuration to update in the API
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_policer_mgmt_engine_database_get(
    int unit,
    int flags,
    bcm_policer_engine_t * engine,
    bcm_policer_database_attach_config_t * config);

/**
 * \brief
 *      verify that the section is valid for APIs that use the section parameters.
 *      Some of the APIs do not get the database handle, so the verification will be made only after extructing the database parameters.
 * \param [in] unit -unit id 
 * \param [in] core_id - specific core_id
 * \param [in] is_ingress - ingress ot egress database
 * \param [in] database_id -database_id
 * \param [in] section - ALL/LOW/HIGH
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_policer_mgmt_section_verify(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    bcm_policer_engine_section_t section);

/**
 * \brief
 * get how many meters in this database. how many potential meters, based on the banks that were allocated for this database
 * \param [in] unit -unit id 
 * \param [in] core_id - specific core_id
 * \param [in] is_ingress - ingress ot egress database
 * \param [in] database_id - database_id
 * \param [in] nof_meters - nof_meters 
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_policer_mgmt_nof_meter_in_database_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int *nof_meters);

/**
 * \brief
 *      default policer configuration which use API strucutre. It configure the max rate and max burst (normal mode)
 * \param [in] unit -unit id
 * \param [in] pol_cfg -strucutre which hold the policer configuration
 * \param [in] is_single_bucket - is policer database is single bucket mode
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_policer_default(
    int unit,
    bcm_policer_config_t * pol_cfg,
    uint32 is_single_bucket);

/**
* \brief
*   handle bcm_policer_primary_color_resolution_set API. configure each time one entry in table
* \param [in] unit        - unit id
* \param [in] flags        - flags (none)
* \param [in] key  - key for the table to configure
* \param [in] config     - configured table
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_primary_color_resolution_set(
    int unit,
    int flags,
    bcm_policer_primary_color_resolution_key_t * key,
    bcm_policer_primary_color_resolution_config_t * config);

/**
* \brief
*   handle bcm_policer_primary_color_resolution_get API. get each time one entry from table
* \param [in] unit        - unit id
* \param [in] flags        - flags (none)
* \param [in] key  - key for the table to configure
* \param [out] config     - configured table
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_primary_color_resolution_get(
    int unit,
    int flags,
    bcm_policer_primary_color_resolution_key_t * key,
    bcm_policer_primary_color_resolution_config_t * config);

/**
* \brief
*   handle bcm_policer_color_resolution_set API. set each time one entry from final resolution table
* \param [in] unit        - unit id
* \param [in] policer_color_resolution        - strucutre which hold the key and value of the table. for egress the value is only egress_color.
* \return
*   shr_error_e - Error Type
* \remark
*   * configure all cores always
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_color_final_resolution_set(
    int unit,
    bcm_policer_color_resolution_t * policer_color_resolution);

/**
* \brief
*   handle bcm_policer_color_resolution_get API. set each time one entry from final resolution table
* \param [in] unit        - unit id
* \param [in] policer_color_resolution        - strucutre which hold the key and value of the table. for egress the value is only egress_color.
* \return
*   shr_error_e - Error Type
* \remark
*   * configure all cores always
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_color_final_resolution_get(
    int unit,
    bcm_policer_color_resolution_t * policer_color_resolution);

/**
* \brief
*   set the delta value in meter ingress header compensation per port table. 
* \param [in] unit        - unit id
* \param [in] logical_port   -logical_port
* \param [in] delta   - -128..127
* \return
*   shr_error_e - Error Type
* \remark
*   There is a table per meter database (o,1,2) and global. Dbal set the given delta for all of them. 
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_ingress_port_compensation_delta_set(
    int unit,
    bcm_port_t logical_port,
    int delta);

/**
* \brief
*   get the delta value from meter ingress header compensation per port table. 
* \param [in] unit        - unit id
* \param [in] logical_port   - logical port
* \param [out] delta   - -128..127
* \return
*   shr_error_e - Error Type
* \remark
*   There is a table per meter database (o,1,2) and global. Dbal set the given delta for all of them. And get one of them (which should be all the same).
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_ingress_port_compensation_delta_get(
    int unit,
    bcm_port_t logical_port,
    int *delta);

/**
* \brief
*   enable/disable MEF 10.3 mode in ingress and egress
* \param [in] unit     - unit id
* \param [in] enable   - enable/disable
* \return
*   shr_error_e - Error Type
* \remark
*   
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_mef_10_3_set(
    int unit,
    int enable);

/**
* \brief
*   get if MEF 10.3 mode is enabled/disabled
* \param [in] unit      - unit id
* \param [out] enable   - enable/disable
* \return
*   shr_error_e - Error Type
* \remark
*   
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_mef_10_3_get(
    int unit,
    int *enable);

/**
* \brief
*   set filter bitmap by DP command 
* \param [in] unit        - unit id
* \param [in] policer_database_handle   - database handle
* \param [in] filter_bitmap - filter bitmap to be set
* \return
*   shr_error_e - Error Type
* \remark
*   API bcm_policer_control_set with type control
*   bcmPolicerControlTypeFilterByDpCommand sets given filter
*   bitmap by DP command. There is a table per
*   ingress/egress/global meter and DBAL sets the given filter
*   to the relevant field.
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_filter_by_dp_command_set(
    int unit,
    int policer_database_handle,
    uint32 filter_bitmap);

/**
* \brief
*   get filter bitmap by DP command 
* \param [in] unit        - unit id
* \param [in] policer_database_handle   - database handle
* \param [out] filter_bitmap - filter bitmap
* \return
*   shr_error_e - Error Type
* \remark
*   API bcm_policer_control_get with type control
*   bcmPolicerControlTypeFilterByDpCommand gives the filter
*   bitmap by DP command.
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_filter_by_dp_command_get(
    int unit,
    int policer_database_handle,
    uint32 *filter_bitmap);

/**
* \brief
*   select to pass to table DP-CMD(0) or Meter-Profile(1)
* \param [in] unit        - unit id
* \param [in] policer_database_handle   - database handle
* \param [in] enable - true=DP-CMD / false=Meter-Profile
* \return
*   shr_error_e - Error Type
* \remark
*   API bcm_policer_control_get with type control
*   bcmPolicerControlTypeDpResolutionByDpCommand -
*   enable/disable DP-CMD
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_dp_resolution_by_dp_command_set(
    int unit,
    int policer_database_handle,
    uint32 enable);

/**
* \brief
*   select to pass to table DP-CMD(0) or Meter-Profile(1)
* \param [in] unit        - unit id
* \param [in] policer_database_handle   - database handle
* \param [out] enable - true=DP-CMD / false=Meter-Profile
* \return
*   shr_error_e - Error Type
* \remark
*   API bcm_policer_control_get with type control
*   bcmPolicerControlTypeDpResolutionByDpCommand -
*   true=DP-CMD / false=Meter-Profile
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_dp_resolution_by_dp_command_get(
    int unit,
    int policer_database_handle,
    uint32 *enable);

/**
* \brief
*   select to pass to table DP-CMD(0) or Meter-Profile(1)
* \param [in] unit        - unit id
* \param [in] cb   - policer traverse callback
* \param [out] user_data - user data
* \return
*   shr_error_e - Error Type
* \remark
* 
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_traverse(
    int unit,
    bcm_policer_traverse_cb cb,
    void *user_data);

/**
* \brief
*   enable/disable fairness mechasinm for all meters (ingress/egress/global).
*   The fairness mechanism is adding a random number for the bucket level in order to prevent adverse synchronization.
*   between packet flows and provide fairness between multiple flows using the same meter.
* \param [in] unit        - unit id
* \param [in] policer_database_handle   - database handle
* \param [in] enable   - if true enable fairmness mechanism. false for disable it.
* \return
*   shr_error_e - Error Type
* \remark
* 
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_fairness_set(
    int unit,
    int policer_database_handle,
    int enable);

/**
* \brief
*   get if meter fairness mechainsm is enabled or disabled
* \param [in] unit        - unit id
* \param [in] policer_database_handle   - database handle
* \param [out] enable   - if true enabled.
* \return
*   shr_error_e - Error Type
* \remark
* 
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_fairness_get(
    int unit,
    int policer_database_handle,
    uint32 *enable);

/**
* \brief
*   function verify that the meter_pointer is valid, which means that its database was created, 
*   and that It has corresponding entry in the HW memory that belong to the given database.
*   In addition, it give back the meter indexes (base and size) that belongs to the meter_pointer.
* \param [in] unit        - unit id
* \param [in] core_id     - core id
* \param [in] is_ingress     - ingress/egress meter
* \param [in] database_id - database id
* \param [in] meter_pointer - meter pointer is the object_stat_id that was given. 
*             [meter_pointer = is_ingress ? object-stat-id : (object-stat-id*nof_QoS+QoS_offset) ]
* \param [out] meter_valid   - True/valid
* \param [out] meter_idx_base   - the base meter index
* \param [out] meter_idx_size   - size of meter indexes corresponding to the meter pointer. In case of expansion exist,
*                                 the size is according to the expansion, otherwise, the size is 1.
* \return
*   shr_error_e - Error Type
* \remark
*  1. Function does not handle global meter.
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_meter_index_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int meter_pointer,
    int *meter_valid,
    int *meter_idx_base,
    int *meter_idx_size);

/**
* \brief
*   enable dynamic access to all meter tables
* \param [in] unit        - unit id
* \param [in] enable     - True - enable, false - disable.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_meter_dynamic_access_enable_set(
    int unit,
    int enable);

/**
* \brief
*   get if dynamic access to all meter tables enable or disable
* \param [in] unit        - unit id
* \param [out] enable     - True - enable, false - disable.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_meter_dynamic_access_enable_get(
    int unit,
    int *enable);

/**
* \brief
*   Function to set the global per TC threshold for TC sensitive meters
* \param [in] unit            - Unit ID
* \param [in] gport           - Core Gport or 0, BCM_CORE_ALL is allowed, 0 indicates to set all cores
* \param [in] cosq            - TC for which the threshold will be set
* \param [in] tcsm_threshold  - Threshold value in percentages of the bucket level
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_tcsm_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int tcsm_threshold);

/**
* \brief
*   Function to get the global per TC threshold for TC sensitive meters
* \param [in] unit            - Unit ID
* \param [in] gport           - Core Gport or 0, BCM_CORE_ALL is allowed, 0 indicates to set all cores
* \param [in] cosq            - TC for which to get the threshold
* \param [out] tcsm_threshold - Threshold value in percentages of the bucket level
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_tcsm_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *tcsm_threshold);

/**
* \brief
*   Function to set enabler for TC sensitive meters
* \param [in] unit - Unit ID
* \param [in] policer_database_handle - database handle
* \param [in] tcsm_enable - enable the TCSM
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_tcsm_enable_set(
    int unit,
    int policer_database_handle,
    uint32 tcsm_enable);

/**
* \brief
*   Function to get enabler for TC sensitive meters
* \param [in] unit - Unit ID
* \param [in] policer_database_handle - database handle
* \param [out] tcsm_enable - enable the TCSM
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_tcsm_enable_get(
    int unit,
    int policer_database_handle,
    uint32 *tcsm_enable);

/**
* \brief
*   Function to set which TC will determine the threshold for TC sensitive meters
* \param [in] unit - Unit ID
* \param [in] policer_database_handle - database handle
* \param [in] use_pp_tc - TRUE: use the PP TC; FALSE: use the offset from expansion groups
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_tcsm_use_pp_tc_set(
    int unit,
    int policer_database_handle,
    uint32 use_pp_tc);

/**
* \brief
*   Function to get which TC will determine the threshold for TC sensitive meters
* \param [in] unit - Unit ID
* \param [in] policer_database_handle - database handle
* \param [out] use_pp_tc - TRUE: use the PP TC; FALSE: use the offset from expansion groups
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_policer_mgmt_tcsm_use_pp_tc_get(
    int unit,
    int policer_database_handle,
    uint32 *use_pp_tc);

#endif/*_DNX_POLICER_MGMT_INCLUDED__*/
