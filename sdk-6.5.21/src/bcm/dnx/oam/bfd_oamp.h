/** \file bfd_oamp.h
 * OAMP support - contains internal functions and definitions
 * for support of the OAM/BFD accelerator BFD functionality
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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

/** For IPv4 single hop, ip-ttl = (tunnel_mpls_enable)? 1 : 255
 * Also used by IPv6 */
#define DNX_OAMP_BFD_1HOP_MPLS_TUNNEL_ENABLE    SAL_BIT(9)

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
    dbal_enum_value_field_oamp_mep_type_e * bfd_dbal_mep_type,
    uint32 *out_flags,
    uint8 *ach_sel);

/*
 * }
 */
#endif /* BFD_OAMP_H_INCLUDED */
