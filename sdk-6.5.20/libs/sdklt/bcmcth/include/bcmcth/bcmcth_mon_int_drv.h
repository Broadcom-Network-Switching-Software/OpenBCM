/*! \file bcmcth_mon_int_drv.h
 *
 * BCMCTH Monitor-Inband Telemetry driver object.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_INT_DRV_H
#define BCMCTH_MON_INT_DRV_H


#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_mon_collector.h>

/*!
 * Data structure for the read-only operational fields in
 * MON_INBAND_TELEMETRY_CONTROL LT.
 */
typedef struct bcmcth_mon_int_control_oper_fields_s {
    /*! Corresponds to MAX_EXPORT_PKT_LENGTH_OPER field in the LT. */
    uint16_t max_export_pkt_length;

    /*! Corresponds to MAX_RX_PKT_LENGTH_OPER field in the LT. */
    uint16_t max_rx_pkt_length;
} bcmcth_mon_int_control_oper_fields_t;

/*! Data structure for MON_INBAND_TELEMETRY_CONTROL LT. */
typedef struct bcmcth_mon_int_control_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_INBAND_TELEMETRY_CONTROLt_FIELD_COUNT);

    /*! Corresponds to INBAND_TELEMETRY field in the LT. */
    bool inband_telemetry;

    /*! Corresponds to MAX_EXPORT_PKT_LENGTH field in the LT. */
    uint16_t max_export_pkt_length;

    /*! Corresponds to MAX_RX_PKT_LENGTH field in the LT. */
    uint16_t max_rx_pkt_length;

    /*! Corresponds to DEVICE_IDENTIFIER field in the LT. */
    uint32_t device_id;

    /*! Corresponds to EXPORT field in the LT. */
    bool export;

    /*! Corresponds to RX_PKT_EXPORT_MAX_LENGTH field in the LT. */
    uint16_t rx_pkt_export_max_length;

    /*! Corresponds to COLLECTOR_TYPE field in the LT. */
    bcmltd_common_mon_inband_telemetry_control_collector_type_t_t collector_type;

    /*! Corresponds to MON_COLLECTOR_IPV4_ID in the LT. */
    uint32_t collector_ipv4_id;

    /*!
     * Indicates that the entry corresponding to collector_ipv4_id was found.
     */
    bool collector_ipv4_found;

    /*!
     * Structure containing the fields of the IPv4 collector entry specified by
     * ipv4_collector_id.
     */
    bcmcth_mon_collector_ipv4_t collector_ipv4;

    /*! Corresponds to MON_COLLECTOR_IPV6_ID in the LT. */
    uint32_t collector_ipv6_id;

    /*!
     * Indicates that the entry corresponding to collector_ipv6_id was found.
     */
    bool collector_ipv6_found;

    /*!
     * Structure containing the fields of the IPv6 collector entry specified by
     * ipv6_collector_id.
     */
    bcmcth_mon_collector_ipv6_t collector_ipv6;

    /*! Corresponds to MON_EXPORT_PROFILE_ID field in the LT */
    uint32_t export_profile_id;

    /*!
     * Indicates that the entry corresponding to export_profile_id was found.
     */
    bool export_profile_found;

    /*!
     * Structure containing the fields of the export profile entry specified by
     * the export_profile_id.
     */
    bcmcth_mon_export_profile_t export_profile;

    /*!
     * Structure containing the read-only fields in the LT. This structure
     * contains the the fields that are updated internally and sent back to IMM
     * during INSERT or UPDATE operation.
     */
    bcmcth_mon_int_control_oper_fields_t oper;
} bcmcth_mon_int_control_entry_t;

/*!
 * Data structure for EXPORT_ELEMENTS structure in
 * MON_INBAND_TELEMETRY_IPFIX_EXPORT LT.
 */
typedef struct bcmcth_mon_int_ipfix_export_elements_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_INBAND_TELEMETRY_IPFIX_EXPORTt_FIELD_COUNT);

    /*! Corresponds to EXPORT_ELEMENT field in the LT. */
    uint32_t element;

    /*! Corresponds to ENTERPRISE field in the LT. */
    bool enterprise;

    /*! Corresponds to INFORMATION_ELEMENT_IDENTIFIER field in the LT. */
    uint16_t info_element_id;
} bcmcth_mon_int_ipfix_export_elements_t;

/*! Data structure for MON_INBAND_TELEMETRY_IPFIX_EXPORT LT. */
typedef struct bcmcth_mon_int_ipfix_export_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_INBAND_TELEMETRY_IPFIX_EXPORTt_FIELD_COUNT);

    /*! Corresponds to SET_ID field in the LT. */
    uint16_t set_id;

    /*! Corresponds to NUM_EXPORT_ELEMENTS field in the LT. */
    uint32_t num_elements;

    /*! Corresponds to EXPORT_ELEMENT fields in the LT. */
    bcmcth_mon_int_ipfix_export_elements_t elements[INT_EXPORT_ELEMENTS_MAX];

    /*! Corresponds to TX_INTERVAL field in the LT. */
    uint32_t tx_interval;

    /*! Corresponds to INITIAL_BURST field in the LT. */
    uint32_t initial_burst;

    /*! Corresponds to OPERATIONAL_STATE field in the LT. */
    bcmltd_common_mon_inband_telemetry_ipfix_export_state_t_t oper_state;
} bcmcth_mon_int_ipfix_export_entry_t;

/*! Data strucuture for MON_INBAND_TELEMETRY_STATS LT. */
typedef struct bcmcth_mon_int_stats_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_INBAND_TELEMETRY_STATSt_FIELD_COUNT);

    /*! Corresponds to RX_PKT_CNT field in the LT. */
    uint64_t rx_pkt_cnt;

    /*! Corresponds to TX_PKT_CNT field in the LT. */
    uint64_t tx_pkt_cnt;

    /*! Corresponds to TX_RECORD_CNT field in the LT. */
    uint64_t tx_record_cnt;

    /*! Corresponds to RX_PKT_EXPORT_DISABLED_DISCARD field in the LT. */
    uint64_t rx_pkt_export_disabled_discard;

    /*! Corresponds to RX_PKT_NO_EXPORT_CONFIG_DISCARD field in the LT. */
    uint64_t rx_pkt_no_export_config_discard;

    /*! Corresponds to RX_PKT_NO_IPFIX_CONFIG_DISCARD field in the LT. */
    uint64_t rx_pkt_no_ipfix_config_discard;

    /*! Corresponds to RX_PKT_CURRENT_LENGTH_EXCEED_DISCARD field in the LT. */
    uint64_t rx_pkt_current_length_exceed_discard;

    /*! Corresponds to RX_PKT_LENGTH_EXCEED_MAX_DISCARD field in the LT. */
    uint64_t rx_pkt_length_exceed_max_discard;

    /*! Corresponds to RX_PKT_PARSE_ERROR_DISCARD field in the LT. */
    uint64_t rx_pkt_parse_error_discard;

    /*! Corresponds to RX_PKT_UNKNOWN_NAMESPACE_DISCARD field in the LT. */
    uint64_t rx_pkt_unknown_namespace_discard;

    /*! Corresponds to RX_PKT_EXCESS_RATE_DISCARD field in the LT. */
    uint64_t rx_pkt_excess_rate_discard;

    /*! Corresponds to RX_PKT_INCOMPLETE_METADATA_DISCARD field in the LT. */
    uint64_t rx_pkt_incomplete_metadata_discard;

    /*! Corresponds to TX_PKT_FAILURE_CNT field in the LT. */
    uint64_t tx_pkt_failure_cnt;
} bcmcth_mon_int_stats_t;

/*!
 * \brief Register INT IMM handlers.
 *
 * Register the IMM handlers required by the MON_INBAND_TELEMETRY_XXX
 * LTs. This function is called as part of MON IMM init.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_imm_register(int unit, bool warm);

/*!
 * \brief INT handler for MON_COLLECTOR_IPV4 LT IMM events.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] event IMM event.
 * \param [in] collector Structure containing the collector fields that are
 *             being inserted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_imm_collector_ipv4_handler(int unit,
                                          bcmimm_entry_event_t event,
                                          bcmcth_mon_collector_ipv4_t *collector);

/*!
 * \brief INT handler for MON_COLLECTOR_IPV6 LT IMM events.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] event IMM event.
 * \param [in] collector Structure containing the collector fields that are
 *             being inserted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_imm_collector_ipv6_handler(int unit,
                                          bcmimm_entry_event_t event,
                                          bcmcth_mon_collector_ipv6_t *collector);

/*!
 * \brief INT handler for MON_EXPORT_PROFILE LT IMM events.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] event IMM event.
 * \param [in] export_profile Structure containing the export profile fields
 *             that are being inserted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_imm_export_profile_handler(int unit,
                                          bcmimm_entry_event_t event,
                                          bcmcth_mon_export_profile_t *export_profile);


/*!
 * \brief Get the details of the MD FIFO.
 *
 * This device specific function returns the  width and depth of the MD FIFO.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [out] width Width of the FIFO in bytes.
 * \param [out] depth Depth of the FIFO.
*/
typedef int (*bcmcth_mon_int_md_fifo_get_f) (int unit, bcmltd_sid_t lt_id,
                                             uint32_t trans_id, int *width,
                                             int *depth);

/*!
 * \brief Get the flex fields that need to be extracted from Rx PMD.
 *
 * Get the flex fields in the Rx PMD that is required by the EApp.
 *
 * \param [in] unit Unit number.
 * \param [in] max_fids Indicates the size of the \c fids array
 * \param [out] num_fids Number of fields that need to be extracted from
 *                         Rx PMD.
 * \param [out] fids Array of flex field ids.
 * \param [out] mcs_fids MCS equivalent of \c fids.
*/
typedef int (*bcmcth_mon_int_rxpmd_flex_fids_get_f) (int unit, int max_fids,
                                                     uint8_t *num_fids, int *fids,
                                                     uint8_t *mcs_fids);

/*!
 * \brief Get the MCS equivalent of the device specific IPFIX element.
 *
 * Get the MCS equivalent of the device specific IPFIX element.
 *
 * \param [in] unit Unit number.
 * \param [in] type Device specific IPFIX elemeent type.
 * \param [out] mcs_element MCS equivalent of the device specific element type.
 */
typedef int (*bcmcth_mon_int_ipfix_export_element_get_f) (int unit,
                                                          uint32_t type,
                                                          uint8_t *mcs_element);

/*!
 * \brief Get the size of an IPFIX element.
 *
 * Get the size of an IPFIX element. Set 0xFFFF for variable length elements.
 *
 * \param [in] unit Unit number.
 * \param [in] type Device specific IPFIX elemeent type.
 * \param [out] mcs_element MCS equivalent of the device specific element type.
 */
typedef int (*bcmcth_mon_int_ipfix_export_element_size_get_f) (int unit,
                                                               uint32_t type,
                                                               uint16_t *size);

/*!
 * \brief Inband Telemetry driver object.
 *
 * Device specific INT functions are implemented using this driver.
 */
typedef struct bcmcth_mon_int_drv_s {
    /*! Get details of the MD FIFO */
    bcmcth_mon_int_md_fifo_get_f fifo_get;

    /*! Get the Rx PMD flex field ids. */
    bcmcth_mon_int_rxpmd_flex_fids_get_f rxpmd_flex_fids_get;

    /*! Get the MCS equivalent of the template elements. */
    bcmcth_mon_int_ipfix_export_element_get_f ipfix_export_element_get;

    /*! Get the size of an IPFIX element. */
    bcmcth_mon_int_ipfix_export_element_size_get_f ipfix_export_element_size_get;
} bcmcth_mon_int_drv_t;

/*!
 * \brief Get the INT driver APIs.
 *
 * This function returns a structure which contains the device specific APIs for
 * INT.
 *
 * \param [in] unit Unit number.
 *
 * \return INT driver API structure
 */
extern bcmcth_mon_int_drv_t *
bcmcth_mon_int_drv_get(int unit);

/*!
 * \brief Alloc memory required for the INT EApp.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Flag indicating whether it is a cold or a warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_uc_alloc(int unit, bool warm);

/*!
 * \brief Free memory used by the INT EApp.
 *
 * \param [in] unit Unit number.
 */
extern void
bcmcth_mon_int_uc_free(int unit);

/*!
 * \brief MON_INBAND_TELEMETRY_CONTROL LT insert handler.
 *
 * This function handles the INSERT opcode for the MON_INBAND_TELEMETRY_CONTROL
 * LT. This function is called from the stage callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] insert Structure containing the fields being inserted into IMM.
 * \param [out] oper Structure containing the operational values that must be
 *              updated in IMM.
 * \param [out] replay Replay configuration of other LTs if the flag is true.
 *              Usually implies that the app has initialized.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_control_insert(int unit,
                              uint32_t trans_id,
                              bcmcth_mon_int_control_entry_t *insert,
                              bcmcth_mon_int_control_oper_fields_t *oper,
                              bool *replay);

/*!
 * \brief MON_INBAND_TELEMETRY_CONTROL LT update handler.
 *
 * This function handles the UPDATE opcode for the MON_INBAND_TELEMETRY_CONTROL
 * LT. This function is called from the stage callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] cur Structure containing the fields that are currently
 *             present in the IMM.
 * \param [in] update Structure containing the fields being updated.
 * \param [out] oper Structure containing the operational values that must be
 *              updated in IMM.
 * \param [out] replay Replay configuration of other LTs if the flag is true.
 *              Usually implies that the app has initialized.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_control_update(int unit,
                              uint32_t trans_id,
                              bcmcth_mon_int_control_entry_t *cur,
                              bcmcth_mon_int_control_entry_t *update,
                              bcmcth_mon_int_control_oper_fields_t *oper,
                              bool *replay);

/*!
 * \brief Get the MON_INBAND_TELEMETRY_CONTROL oper state.
 *
 * Get the current highest priority operational state.
 *
 * \param [in] unit Unit number.
 *
 * \return Operational state.
 */
extern bcmltd_common_mon_inband_telemetry_control_state_t_t
bcmcth_mon_int_control_oper_state_get(int unit);

/*!
 * \brief MON_INBAND_TELEMETRY_CONTROL LT delete handler.
 *
 * This function handles the DELETE opcode for the MON_INBAND_TELEMETRY_CONTROL
 * LT. This function is called from the stage callback handler for the LT.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_control_delete(int unit);

/*!
 * \brief MON_INBAND_TELEMETRY_IPFIX_EXPORT LT insert handler.
 *
 * This function handles the INSERT opcode for the
 * MON_INBAND_TELEMETRY_IPFIX_EXPORT LT. This function is called from the stage
 * callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [in] insert Structure containing the fields being inserted into IMM.
 * \param [out] oper_state Operational state of the LT entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_ipfix_export_insert(int unit,
                                   bcmcth_mon_int_ipfix_export_entry_t *insert,
                                   bcmltd_common_mon_inband_telemetry_ipfix_export_state_t_t *oper_state);

/*!
 * \brief MON_INBAND_TELEMETRY_IPFIX_EXPORT LT update handler.
 *
 * This function handles the UPDATE opcode for the
 * MON_INBAND_TELEMETRY_IPFIX_EXPORT LT. This function is called from the stage
 * callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [in] cur Structure containing the fields that are currently
 *             present in the IMM.
 * \param [in] update Structure containing the fields being updated.
 * \param [out] oper_state Operational state of the LT entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_ipfix_export_update(int unit,
                                   bcmcth_mon_int_ipfix_export_entry_t *cur,
                                   bcmcth_mon_int_ipfix_export_entry_t *update,
                                   bcmltd_common_mon_inband_telemetry_ipfix_export_state_t_t *oper_state);

/*!
 * \brief MON_INBAND_TELEMETRY_IPFIX_EXPORT LT delete handler.
 *
 * This function handles the DELETE opcode for the
 * MON_INBAND_TELEMETRY_IPFIX_EXPORT LT. This function is called from the stage
 * callback handler for the LT.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_ipfix_export_delete(int unit);

/*!
 * \brief Replay an MON_INBAND_TELEMETRY_IPFIX_EXPORT entry.
 *
 * This function replays a MON_INBAND_TELEMETRY_IPFIX_EXPORT entry that has
 * been already inserted.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Structure containing the fields that need to be replayed.
 * \param [out] oper_state Operational state of the LT entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_ipfix_export_replay(int unit,
                                   bcmcth_mon_int_ipfix_export_entry_t *entry,
                                   bcmltd_common_mon_inband_telemetry_ipfix_export_state_t_t *oper_state);

/*!
 * \brief MON_INBAND_TELEMETRY_STATS LT update handler.
 *
 * This function handles the UPDATE opcode for the MON_INBAND_TELEMETRY_STATS
 * LT. This function is called from the stage callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Structure containing the fields to be updated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_stats_update(int unit, bcmcth_mon_int_stats_t *entry);

/*!
 * \brief MON_INBAND_TELEMETRY_STATS LT lookup handler.
 *
 * This function handles the LOOKUP opcode for the MON_INBAND_TELEMETRY_STATS
 * LT. This function is called from the lookup callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [out] entry Structure containing the fields that are retreived.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_stats_lookup(int unit, bcmcth_mon_int_stats_t *entry);


/*!
 * \brief INT handler for MON_COLLECTOR_IPV4 LT insert.
 *
 * This function handles the INSERT opcode for MON_COLLECTOR_IPV4 LT. The
 * MON_INBAND_TELEMETRY_CONTROL.OPERATIONAL_STATE field may get updated as part
 * of this function.
 *
 * \param [in] unit Unit number.
 * \param [in] int_entry MON_INBAND_TELEMETRY_CONTROL entry which uses the
 *             collector.
 * \param [in] insert Structure containing the collector fields that are
 *              being inserted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_collector_ipv4_insert(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     bcmcth_mon_collector_ipv4_t *insert);

/*!
 * \brief INT handler for MON_COLLECTOR_IPV4 LT update.
 *
 * This function handles the UPDATE opcode for MON_COLLECTOR_IPV4 LT. The
 * MON_INBAND_TELEMETRY_CONTROL.OPERATIONAL_STATE field may get updated as part
 * of this function.
 *
 * \param [in] unit Unit number.
 * \param [in] int_entry MON_INBAND_TELEMETRY_CONTROL entry which uses the
 *             collector.
 * \param [in] update Structure containing the collector fields that are
 *              being updated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_collector_ipv4_update(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     bcmcth_mon_collector_ipv4_t *update);

/*!
 * \brief INT handler for MON_COLLECTOR_IPV4 LT delete.
 *
 * This function handles the DELETE opcode for MON_COLLECTOR_IPV4 LT. The
 * MON_INBAND_TELEMETRY_CONTROL.OPERATIONAL_STATE field may get updated as part
 * of this function.
 *
 * \param [in] unit Unit number.
 * \param [in] int_entry MON_INBAND_TELEMETRY_CONTROL entry which uses the
 *             collector.
 * \param [in] collector_id Id of the collector being deleted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mon_int_collector_ipv4_delete(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     uint32_t collector_id);

/*!
 * \brief INT handler for MON_COLLECTOR_IPV6 LT insert.
 *
 * This function handles the INSERT opcode for MON_COLLECTOR_IPV6 LT. The
 * MON_INBAND_TELEMETRY_CONTROL.OPERATIONAL_STATE field may get updated as part
 * of this function.
 *
 * \param [in] unit Unit number.
 * \param [in] int_entry MON_INBAND_TELEMETRY_CONTROL entry which uses the
 *             collector.
 * \param [in] insert Structure containing the collector fields that are
 *              being inserted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_collector_ipv6_insert(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     bcmcth_mon_collector_ipv6_t *insert);

/*!
 * \brief INT handler for MON_COLLECTOR_IPV6 LT update.
 *
 * This function handles the UPDATE opcode for MON_COLLECTOR_IPV6 LT. The
 * MON_INBAND_TELEMETRY_CONTROL.OPERATIONAL_STATE field may get updated as part
 * of this function.
 *
 * \param [in] unit Unit number.
 * \param [in] int_entry MON_INBAND_TELEMETRY_CONTROL entry which uses the
 *             collector.
 * \param [in] update Structure containing the collector fields that are
 *              being updated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_collector_ipv6_update(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     bcmcth_mon_collector_ipv6_t *update);

/*!
 * \brief INT handler for MON_COLLECTOR_IPV6 LT delete.
 *
 * This function handles the DELETE opcode for MON_COLLECTOR_IPV6 LT. The
 * MON_INBAND_TELEMETRY_CONTROL.OPERATIONAL_STATE field may get updated as part
 * of this function.
 *
 * \param [in] unit Unit number.
 * \param [in] int_entry MON_INBAND_TELEMETRY_CONTROL entry which uses the
 *             collector.
 * \param [in] collector_id Id of the collector being deleted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mon_int_collector_ipv6_delete(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     uint32_t collector_id);

/*!
 * \brief INT handler for MON_EXPORT_PROFILE LT insert.
 *
 * This function handles the INSERT opcode for MON_EXPORT_PROFILE LT. The
 * MON_INBAND_TELEMETRY_CONTROL.OPERATIONAL_STATE field may get updated as part
 * of this function.
 *
 * \param [in] unit Unit number.
 * \param [in] int_entry MON_INBAND_TELEMETRY_CONTROL entry which uses the
 *             export profile.
 * \param [in] insert Structure containing the export profile fields that are
 *              being inserted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_export_profile_insert(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     bcmcth_mon_export_profile_t *insert);

/*!
 * \brief INT handler for MON_EXPORT_PROFILE LT update.
 *
 * This function handles the UPDATE opcode for MON_EXPORT_PROFILE LT. The
 * MON_INBAND_TELEMETRY_CONTROL.OPERATIONAL_STATE field may get updated as part
 * of this function.
 *
 * \param [in] unit Unit number.
 * \param [in] int_entry MON_INBAND_TELEMETRY_CONTROL entry which uses the
 *             export profile.
 * \param [in] update Structure containing the export profile fields that are
 *              being updated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_int_export_profile_update(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     bcmcth_mon_export_profile_t *update);

/*!
 * \brief INT handler for MON_EXPORT_PROFILE LT delete.
 *
 * This function handles the DELETE opcode for MON_EXPORT_PROFILE LT. The
 * MON_INBAND_TELEMETRY_CONTROL.OPERATIONAL_STATE field may get updated as part
 * of this function.
 *
 * \param [in] unit Unit number.
 * \param [in] int_entry MON_INBAND_TELEMETRY_CONTROL entry which uses the
 *             export profile.
 * \param [in] export_profile_id Id of the export profile being deleted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mon_int_export_profile_delete(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     uint32_t export_profile_id);
#endif /* BCMCTH_MON_INT_DRV_H */
