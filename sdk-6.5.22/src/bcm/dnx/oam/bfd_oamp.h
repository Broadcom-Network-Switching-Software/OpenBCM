/** \file bfd_oamp.h
 * OAMP support - contains internal functions and definitions
 * for support of the OAM/BFD accelerator BFD functionality
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef BFD_OAMP_H_INCLUDED
/*
 * {
 */
#define BFD_OAMP_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include "include/bcm/oam.h"
#include "include/bcm_int/dnx/algo/oam/algo_oam.h"
#include <src/bcm/dnx/bfd/bfd_internal.h>
#include <src/bcm/dnx/oam/oam_oamp.h>

/** No flags are in use for the templates   */
#define NO_FLAGS 0

/** The discriminator range template has only one profile   */
#define DISCR_PROF 0

/** Required interval profiles   */
#define REQ_MIN_INT_RX       0
#define DES_MIN_INT_TX       1
#define NOF_BASIC_INT_FIELDS 2

/** Conversion of a 6 bit flags value to a 4 bit profile   */
#define BFD_FLAGS_VAL_TO_PROFILE(x) ((UTILEX_GET_BITS_RANGE(x, 5, 3) << 1) | UTILEX_GET_BIT(x, 1))

/** Conversion of a 4 bit profile to a 6 bit flags value   */
#define BFD_FLAGS_PROFILE_TO_VAL(x) ((UTILEX_GET_BITS_RANGE(x, 3, 1) << 3) | (UTILEX_GET_BIT(x, 0) << 1))

/** Macro to differentiate between PWE and MPLS/IP endpoints */
#define BFD_TYPE_NOT_PWE(x) ((x == bcmBFDTunnelTypeUdp) || (x == bcmBFDTunnelTypeMpls)  || (x == bcmBFDTunnelTypeMplsTpCcCv))

/** Flags relevant for accelerated BFD endpoints   */

/** Update flag - necessary only for 1/4 entry  */
#define DNX_OAMP_BFD_MEP_UPDATE                 SAL_BIT(0)

/** Indicates 1/4 entry mode   */
#define DNX_OAMP_BFD_MEP_Q_ENTRY                SAL_BIT(1)

/** 0 - UDP dest port 3784 supported; 1 - not supported   */
#define DNX_OAMP_BFD_MEP_MICRO_BFD              SAL_BIT(2)

/** PWE only: add ACH word   */
#define DNX_OAMP_BFD_MEP_ACH                    SAL_BIT(3)

/** PWE only: add GAL word   */
#define DNX_OAMP_BFD_MEP_GAL                    SAL_BIT(4)

/** PWE only: add router alert label   */
#define DNX_OAMP_BFD_MEP_ROUTER_ALERT           SAL_BIT(5)

/** Use FEC ID or global out-LIF?   */
#define DNX_OAMP_BFD_DESTINATION_IS_FEC         SAL_BIT(6)

/** BFD Echo   */
#define DNX_OAMP_BFD_ECHO                       SAL_BIT(7)

/** BFD over IPv6 */
#define DNX_OAMP_BFD_IPV6                       SAL_BIT(8)

/** FXIED DIP for VCCV BFD */
#define DNX_OAMP_BFD_VCCV_BFD_FIXED_DIP          SAL_BIT(9)

/** For IPv4 single hop, ip-ttl = (tunnel_mpls_enable)? 1 : 255
 * Also used by IPv6 */
#define DNX_OAMP_BFD_1HOP_MPLS_TUNNEL_ENABLE    SAL_BIT(9)

/**
 *  Bits that prevent overlapping in endpoint IDs
 *  the endpoint ID will either be the local discriminator (for
 *  endpoints whose packets are classified by discriminator) or
 *  LIF ID (for the rest.)  Also, MEP IDs 0 - 12k are reserved
 *  for accelerated endpoints.  These bits will be added to
 *  prevent endpoint IDs overlapping between the different
 *  groups.
 */
/**
 *  This bit makes sure that if one endpoint has x as its LIF
 *  and another has x as its discriminator, the endpoints
 *  will not have the same MEP ID
 */
#define BFD_MEP_ID_DISCR_CLASS 23

/**
 *  This bit makes sure non-accelerated endpoints don't use IDs
 *  reserved for accelerated endpoints
 */
#define BFD_MEP_ID_NON_ACC_BIT 22

/** This bit identifies Client MEP on BFD Server unit, so we can
 *  allocate classification entry on the same id.
 *  Endpoint ID with this bit set is the Client MEP of the BFD
 *  server.
 */
#define BFD_MEP_ID_SERVER_CLIENT 24

/**
 *  This struct is used to store data temporarily for accessing
 *  the OAMP RMEP DB for BFD endpoints - the data needed to fill
 *  an entry, including necessary templates and profile tables.
 */
typedef struct
{
    /** Data for writing punt profile */
    uint32 punt_profile_index;
    dnx_oam_oamp_punt_event_profile_t punt_profile;
    uint32 punt_profile_write;

    dnx_oam_oamp_punt_good_profile_temp_data_t punt_good_profile;
    /** ID of endpoint */
    uint32 endpoint_id;
} bfd_temp_oamp_rmep_db_data_t;

/**
 * \brief - This function sets the RMEP index constant,
 *          which will be used as a field to write to
 *          two RMEP index tables - SW state and MDB.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_type - type of BFD endpoint (OAMP MEP DB enum)
 * \param [out] rmep_index_const - Resulting RMEP index constant.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_endpoint_db_get_const_for_oamp_rmep_index_db(
    int unit,
    dbal_enum_value_field_oamp_mep_type_e mep_type,
    uint16 *rmep_index_const);

/**
 * \brief - This function builds the structure needed to call
 *        the API that writes the entry(ies) for accelerated
 *        BFD endpoints into the hardware OAMP RMEP DB, and MDB
 *        entry, if necessary.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [in] rmep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables has been placed.  Here, this
 *        data is used to write the proper profile indexes to
 *        the RMEP DB entries (whether a new profile was added or
 *        an existing one was found.)
 * \param [out] entry_values - Pointing to resulting structure data
 *        that will be used later to call OAMP RMEP DB API.
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_bfd_endpoint_db_build_param_for_oamp_rmep_db(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_rmep_db_data_t * rmep_hw_write_data,
    dnx_oam_oamp_rmep_db_entry_t * entry_values);

/**
 * \brief - This function classified the mep by its type.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] hw_write_data - Pointer to struct where data
 *        for writing to HW tables should be placed (see struct
 *        definition for reason.)
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_mep_type_classification_set(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_endpoint_data_t * hw_write_data);

/**
 * \brief - This function converts the data read from the
 *          OAMP RMEP DB into fields in the API struct, or
 *          template indexes to look up more fields.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] rmep_sw_read_data - Pointer to struct that will
 *        hold indexes for templates where data will be read
 *        later.
 * \param [in] entry_values - Data read from the relevant
 *        accelerated BFD endpoint's OAMP RMEP DB entry.
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_bfd_endpoint_db_analyze_param_from_oamp_rmep_db(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_rmep_db_data_t * rmep_sw_read_data,
    const dnx_oam_oamp_rmep_db_entry_t * entry_values);

/**
 * \brief - This function calculates the correct bfd_period
 *          for a given endpoint ID, Tx rate and ccm_count.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_id - ID of endpoint
 * \param [in] tx_rate - 10 bit value that is written to
 *                       TX_PERIOD in OAMP_MEP_DB
 * \param [in] ccm_count - when the OAMP's CCM counter reaches
 *                         this value, a CCM is sent for this
 *                         endpoint.
 *
 * \return
 *   Calculated bfd_period value
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_bfd_get_bfd_period(
    int unit,
    uint16 endpoint_id,
    uint16 tx_rate,
    uint32 ccm_count);

/**
 * \brief - This function writes an entry to the OAMP_BFD_TX_RATES table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_index - index of entry to write
 * \param [in] tx_period_value - value to write in entry
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_endpoint_db_write_tx_rate_to_oamp_hw(
    int unit,
    uint32 entry_index,
    uint32 tx_period_value);

/**
 * \brief - This is a utility function that converts the MEP type
 *          used by the main BFD APIs and converts it to the
 *          relevant MEP type stored in the OAMP dbal tables, and
 *          also sets any flags necessary to support this type..
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] bfd_api_mep_type - MEP type used by main BFD APIs.
 * \param [in] in_flags - Flags field from main BFD APIs.
 * \param [in] flags2 - Flags2 field from main BFD APIs.
 * \param [in] label_ttl - TTL Value from main BFD API.
 * \param [in] dst_ip_addr - dst_ip_addr from main BFD API.
 * \param [out] bfd_dbal_mep_type - Pointer to MEP type that will
 *                 be stored in the OAMP hardware tables or MDB tables
 *                 for this accelerated endpoint.
 * \param [out] out_flags - Pointer to BFD flags used in OAMP MEP DB
 *              API.
 * \param [out] ach_sel - Pointer to flag that is mapped to the
 *              ACH_SEL field in the OAMP MEP DB entry.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_mep_type_parsing(
    int unit,
    bcm_bfd_tunnel_type_t bfd_api_mep_type,
    uint32 in_flags,
    uint32 flags2,
    uint32 label_ttl,
    bcm_ip_t dst_ip_addr,
    dbal_enum_value_field_oamp_mep_type_e * bfd_dbal_mep_type,
    uint32 *out_flags,
    uint8 *ach_sel);

/**
 * \brief - This function reads the OAMP BFD local
 *          minimum echo value from the template
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - see definition of struct for
 *        description of fields
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_oamp_local_min_echo_sw_read(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info);

/**
 * \brief - This function writes data related to the newly
 *        created BFD endpoint that is accelerated to the local
 *        OAMP to hardware tables and registers with names that
 *        start with "OAMP_"
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] rmep_hw_write_data - Pointer to struct where data
 *        for writing to OAMP RMEP DB related HW tables has been
 *        placed (see struct definition for reason.)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_oamp_endpoint_rmep_db_write_to_hw(
    int unit,
    const bfd_temp_oamp_rmep_db_data_t * rmep_hw_write_data);

/**
 * \brief - This function calls the functions that access
 *          the different templates associated with oamp
 *          profiles.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] rmep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP RMEP DB.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_oamp_write_rmep_data_to_templates(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_rmep_db_data_t * rmep_hw_write_data);

/**
 * \brief - This function, as part of the process of deleting
 *          an accelerated BFD endpoint, decrements the
 *          reference count of every profile that was used by
 *          that endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_entry_values - Pointer to struct containing
 *             Values read from OAMP_RMEP_DB entry that was used
 *             for this endpoint.
 * \param [out] punt_profile_delete - if the refernce count of
 *             a profile reaches 0, that profile is deleted from
 *             the relevant profile hardware table.
 * \param [out] punt_good_hw_write_data - Punt good profile HW info
 *              for the requested MEP.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_oamp_delete_rmep_data_from_templates(
    int unit,
    const dnx_oam_oamp_rmep_db_entry_t * rmep_entry_values,
    uint8 *punt_profile_delete,
    dnx_oam_oamp_punt_good_profile_temp_data_t * punt_good_hw_write_data);

/**
 * \brief - This function, as part of the process of deleting
 *          an accelerated BFD endpoint, decrements the
 *          that was used by that endpoint and reached a
 *          reference count of 0 when it was deleted.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_entry_values - Pointer to struct containing
 *             Values read from OAMP_RMEP_DB entry that was used
 *             for this endpoint.
 * \param [in] punt_profile_delete - if the refernce count of
 *             a profile reaches 0, that profile is deleted from
 *             the relevant profile hardware table.
 * \param [out] punt_good_hw_write_data - Punt good profile HW info
 *              for the requested MEP.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_oamp_endpoint_rmep_profiles_delete(
    int unit,
    const dnx_oam_oamp_rmep_db_entry_t * rmep_entry_values,
    uint8 punt_profile_delete,
    dnx_oam_oamp_punt_good_profile_temp_data_t * punt_good_hw_write_data);

/**
 * \brief - This function prepares to add an OAMP punt profile
 *        entry to the punt profile hardware table, or if that
 *        entry already exists, increases the relevant counter.
 *        This is achieved by using the template "OAM punt
 *        profile."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] rmep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP RMEP DB.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_oamp_punt_profile_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_rmep_db_data_t * rmep_hw_write_data);

/**
 * \brief - This function, as part of the process of modifying
 *          an accelerated BFD endpoint, decrements the
 *          reference count for every profile that is no longer
 *          used, and increments the reference counts for the
 *          new profiles.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] new_endpoint_info - API parameters specifying
 *        how to modify the endpoint.
 * \param [in] existing_endpoint_info - the original API
 *        parameters used to create the endpoint.
 * \param [in] rmep_entry_values - Pointer to struct containing
 *        Values read from OAMP_RMEP_DB entry that was used
 *        for this endpoint.
 * \param [out] punt_profile_delete - if the refernce count of
 *             a profile reaches 0, that profile is deleted from
 *             the relevant profile hardware table.
 * \param [out] rmep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP RMEP DB.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_oamp_modify_rmep_data_in_templates(
    int unit,
    const bcm_bfd_endpoint_info_t * new_endpoint_info,
    const bcm_bfd_endpoint_info_t * existing_endpoint_info,
    const dnx_oam_oamp_rmep_db_entry_t * rmep_entry_values,
    uint8 *punt_profile_delete,
    bfd_temp_oamp_rmep_db_data_t * rmep_hw_write_data);

/**
 * \brief - This function applies the minimum echo interval value
 *        configured for a new accelerated BFD.  One value is used
 *        for all Tx packets generated by the OAMP.
 *        The API will update the template manager with the
 *        configured OAMP minimum echo interval and update the hw_data
 *        values for later HW write.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] hw_data_oamp_local_min_echo_write - enbale HW modification for BFD min echo.
 * \param [out] hw_data_oamp_local_min_echo - BFD min echo interval.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_oamp_local_min_echo_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    uint32 *hw_data_oamp_local_min_echo_write,
    uint32 *hw_data_oamp_local_min_echo);

/**
 * \brief - This function calls functions that access
 *          the different templates associated with oamp
 *          profiles, and read data from those templates.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - see definition of struct for
 *        description of fields
 * \param [in] rmep_sw_read_data - Pointer to struct that now
 *        holds indexes for templates.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_oamp_read_rmep_data_from_templates(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_rmep_db_data_t * rmep_sw_read_data);

/**
 * \brief - This function reads TC and DP values from
 *        the template "OAM TC/DP."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [in] itmh_pph_priority_index - priority profile index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_oamp_itmh_pph_priority_profile_sw_read(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    uint32 itmh_pph_priority_index);

/**
 * \brief - This function prepares to set TC and DP values to
 *        the OAMP_PR2_FW_DTC and OAMP_PR2_FWDDP hardware
 *        registers respectively, or if those values already
 *        exist, increases the relevant counter. This is
 *        achieved by using the template "OAM TC/DP."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of structure for
 *        description of fields
 * \param [out] itmh_pph_priority_write - set to 1 if require write to the HW
 * \param [out] itmh_pph_priority_index - priority profile index
 * \param [out] tc - traffic class value
 * \param [out] dp - enable drop value
 * \param [out] qos - quality of service value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bfd_oamp_itmh_pph_priority_profile_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    uint32 *itmh_pph_priority_write,
    uint32 *itmh_pph_priority_index,
    uint32 *tc,
    uint32 *dp,
    uint32 *qos);
/*
 * }
 */
#endif /* BFD_OAMP_H_INCLUDED */
