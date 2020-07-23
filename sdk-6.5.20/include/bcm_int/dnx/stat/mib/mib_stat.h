/** \file bcm_int/dnx/stat/mib/mib_stat.h
 * 
 * Internal DNX STAT MIB 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_MIB_STAT_INCLUDED__
/*
 * { 
 */
#define _DNX_MIB_STAT_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * MACROs
 * {
 */
/**
 * \brief -
 * Maximal amount of HW counters (either for subtraction or addition operation) that are needed to calculate a single bcm counter.
 */
#define DNX_MIB_MAX_COUNTER_BUNDLE 4

/*
 * }
 */

/*
 * Typedefs
 * {
 */
typedef enum dnx_mib_counter
{
    /** fabric counters*/
    dnx_mib_counter_tx_control_cells,
    dnx_mib_counter_tx_data_cells,
    dnx_mib_counter_tx_data_bytes,
    dnx_mib_counter_rx_crc_errors_data_cells,
    dnx_mib_counter_rx_crc_errors_control_cells_nonbypass,
    dnx_mib_counter_rx_crc_errors_control_cells_bypass,
    dnx_mib_counter_rx_fec_correctable_error,
    dnx_mib_counter_rx_control_cells,
    dnx_mib_counter_rx_data_cells,
    dnx_mib_counter_rx_data_bytes,
    dnx_mib_counter_rx_dropped_retransmitted_control,
    dnx_mib_counter_tx_asyn_fifo_rate,
    dnx_mib_counter_rx_asyn_fifo_rate,
    dnx_mib_counter_rx_lfec_fec_uncorrrectable_errors,
    dnx_mib_counter_rx_llfc_primary_pipe,
    dnx_mib_counter_rx_llfc_second_pipe,
    dnx_mib_counter_rx_llfc_third_pipe,
    dnx_mib_counter_rx_kpcs_errors,
    dnx_mib_counter_rx_kpcs_bypass_errors,
    dnx_mib_counter_rx_rs_fec_bit_error,
    dnx_mib_counter_rx_rs_fec_symbol_error_rate,

    /** interlaken counters */
    dnx_mib_counter_ilkn_rx_pkt,
    dnx_mib_counter_ilkn_tx_pkt,
    dnx_mib_counter_ilkn_rx_bytes,
    dnx_mib_counter_ilkn_tx_bytes,
    dnx_mib_counter_ilkn_rx_err_pkt,
    dnx_mib_counter_ilkn_tx_err_pkt,

    /** nif counters */
    dnx_mib_counter_nif_rx_fec_correctable_errors,
    dnx_mib_counter_nif_rx_fec_uncorrectable_errors,
    dnx_mib_counter_nif_rx_bip_errors,
    dnx_mib_counter_nif_rx_stats_drop_events_sch_low,
    dnx_mib_counter_nif_rx_stats_drop_events_sch_high,
    dnx_mib_counter_nif_rx_stats_drop_events_sch_tdm,

    /** nif ethernet counters */
    dnx_mib_counter_nif_eth_r64,
    dnx_mib_counter_nif_eth_r127,
    dnx_mib_counter_nif_eth_r255,
    dnx_mib_counter_nif_eth_r511,
    dnx_mib_counter_nif_eth_r1023,
    dnx_mib_counter_nif_eth_r1518,
    dnx_mib_counter_nif_eth_rmgv,
    dnx_mib_counter_nif_eth_r2047,
    dnx_mib_counter_nif_eth_r4095,
    dnx_mib_counter_nif_eth_r9216,
    dnx_mib_counter_nif_eth_r16383,
    dnx_mib_counter_nif_eth_rbca,
    dnx_mib_counter_nif_eth_rprog0,
    dnx_mib_counter_nif_eth_rprog1,
    dnx_mib_counter_nif_eth_rprog2,
    dnx_mib_counter_nif_eth_rprog3,
    dnx_mib_counter_nif_eth_rpkt,
    dnx_mib_counter_nif_eth_rpok,
    dnx_mib_counter_nif_eth_ruca,
    dnx_mib_counter_nif_eth_rmca,
    dnx_mib_counter_nif_eth_rxpf,
    dnx_mib_counter_nif_eth_rxpp,
    dnx_mib_counter_nif_eth_rxcf,
    dnx_mib_counter_nif_eth_rfcs,
    dnx_mib_counter_nif_eth_rerpkt,
    dnx_mib_counter_nif_eth_rflr,
    dnx_mib_counter_nif_eth_rjbr,
    dnx_mib_counter_nif_eth_rmtue,
    dnx_mib_counter_nif_eth_rovr,
    dnx_mib_counter_nif_eth_rvln,
    dnx_mib_counter_nif_eth_rdvln,
    dnx_mib_counter_nif_eth_rxuo,
    dnx_mib_counter_nif_eth_rxuda,
    dnx_mib_counter_nif_eth_rxwsa,
    dnx_mib_counter_nif_eth_rprm,
    dnx_mib_counter_nif_eth_rpfc0,
    dnx_mib_counter_nif_eth_rpfcoff0,
    dnx_mib_counter_nif_eth_rpfc1,
    dnx_mib_counter_nif_eth_rpfcoff1,
    dnx_mib_counter_nif_eth_rpfc2,
    dnx_mib_counter_nif_eth_rpfcoff2,
    dnx_mib_counter_nif_eth_rpfc3,
    dnx_mib_counter_nif_eth_rpfcoff3,
    dnx_mib_counter_nif_eth_rpfc4,
    dnx_mib_counter_nif_eth_rpfcoff4,
    dnx_mib_counter_nif_eth_rpfc5,
    dnx_mib_counter_nif_eth_rpfcoff5,
    dnx_mib_counter_nif_eth_rpfc6,
    dnx_mib_counter_nif_eth_rpfcoff6,
    dnx_mib_counter_nif_eth_rpfc7,
    dnx_mib_counter_nif_eth_rpfcoff7,
    dnx_mib_counter_nif_eth_rund,
    dnx_mib_counter_nif_eth_rfrg,
    dnx_mib_counter_nif_eth_rrpkt,
    dnx_mib_counter_nif_eth_t64,
    dnx_mib_counter_nif_eth_t127,
    dnx_mib_counter_nif_eth_t255,
    dnx_mib_counter_nif_eth_t511,
    dnx_mib_counter_nif_eth_t1023,
    dnx_mib_counter_nif_eth_t1518,
    dnx_mib_counter_nif_eth_tmgv,
    dnx_mib_counter_nif_eth_t2047,
    dnx_mib_counter_nif_eth_t4095,
    dnx_mib_counter_nif_eth_t9216,
    dnx_mib_counter_nif_eth_t16383,
    dnx_mib_counter_nif_eth_tbca,
    dnx_mib_counter_nif_eth_tpfc0,
    dnx_mib_counter_nif_eth_tpfcoff0,
    dnx_mib_counter_nif_eth_tpfc1,
    dnx_mib_counter_nif_eth_tpfcoff1,
    dnx_mib_counter_nif_eth_tpfc2,
    dnx_mib_counter_nif_eth_tpfcoff2,
    dnx_mib_counter_nif_eth_tpfc3,
    dnx_mib_counter_nif_eth_tpfcoff3,
    dnx_mib_counter_nif_eth_tpfc4,
    dnx_mib_counter_nif_eth_tpfcoff4,
    dnx_mib_counter_nif_eth_tpfc5,
    dnx_mib_counter_nif_eth_tpfcoff5,
    dnx_mib_counter_nif_eth_tpfc6,
    dnx_mib_counter_nif_eth_tpfcoff6,
    dnx_mib_counter_nif_eth_tpfc7,
    dnx_mib_counter_nif_eth_tpfcoff7,
    dnx_mib_counter_nif_eth_tpkt,
    dnx_mib_counter_nif_eth_tpok,
    dnx_mib_counter_nif_eth_tuca,
    dnx_mib_counter_nif_eth_tufl,
    dnx_mib_counter_nif_eth_tmca,
    dnx_mib_counter_nif_eth_txpf,
    dnx_mib_counter_nif_eth_txpp,
    dnx_mib_counter_nif_eth_txcf,
    dnx_mib_counter_nif_eth_tfcs,
    dnx_mib_counter_nif_eth_terr,
    dnx_mib_counter_nif_eth_tovr,
    dnx_mib_counter_nif_eth_tjbr,
    dnx_mib_counter_nif_eth_trpkt,
    dnx_mib_counter_nif_eth_tfrg,
    dnx_mib_counter_nif_eth_tvln,
    dnx_mib_counter_nif_eth_tdvln,
    dnx_mib_counter_nif_eth_rbyt,
    dnx_mib_counter_nif_eth_rrbyt,
    dnx_mib_counter_nif_eth_tbyt,

    /** nif CLU counters */
    dnx_mib_counter_nif_eth_rverify,
    dnx_mib_counter_nif_eth_rreply,
    dnx_mib_counter_nif_eth_rmcrc,
    dnx_mib_counter_nif_eth_rtrfu,
    dnx_mib_counter_nif_eth_rschcrc,
    dnx_mib_counter_nif_eth_rx_eee_lpi_event,
    dnx_mib_counter_nif_eth_rx_eee_lpi_duration,
    dnx_mib_counter_nif_eth_rx_llfc_phy,
    dnx_mib_counter_nif_eth_rx_llfc_log,
    dnx_mib_counter_nif_eth_rx_llfc_crc,
    dnx_mib_counter_nif_eth_rx_hcfc,
    dnx_mib_counter_nif_eth_rx_hcfc_crc,
    dnx_mib_counter_nif_eth_tverify,
    dnx_mib_counter_nif_eth_treply,
    dnx_mib_counter_nif_eth_tscl,
    dnx_mib_counter_nif_eth_tmcl,
    dnx_mib_counter_nif_eth_tlcl,
    dnx_mib_counter_nif_eth_txcl,
    dnx_mib_counter_nif_eth_tx_eee_lpi_event,
    dnx_mib_counter_nif_eth_tx_eee_lpi_duration,
    dnx_mib_counter_nif_eth_tx_llfc_log,
    dnx_mib_counter_nif_eth_tx_hcfc,
    dnx_mib_counter_nif_eth_tncl,
    dnx_mib_counter_nif_eth_xthol,
    dnx_mib_counter_nif_eth_tspare0,
    dnx_mib_counter_nif_eth_tspare1,
    dnx_mib_counter_nif_eth_tspare2,
    dnx_mib_counter_nif_eth_tspare3,

    /** Must be last */
    dnx_mib_counter_nof
} dnx_mib_counter_t;

/*
 * }
 */

/*
 * Functions prototypes
 * {
 */

/**
* \brief
*   Get the specified statistics by counter_idx
* \param [in] unit - unit id
* \param [in] port - logic port
* \param [in] type - stat type
* \param [in] counter_idx - counter index
* \param [out] value - counter value
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_stat_get_by_counter_idx(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    int counter_idx,
    uint64 *value);

/**
* \brief
*   initialize dnx stat mib module
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mib_init(
    int unit);

/**
* \brief
*   deinitialize dnx mib module
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mib_deinit(
    int unit);

/**
 * \brief - Configure counter for the port addition
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_counter_port_add(
    int unit);

/**
 * \brief - Configure counter for the port removal
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_counter_port_remove(
    int unit);

/**
* \brief
*   initialize mib stat fabric by fmac instance
* \param [in] unit - unit id
* \param [in] fsrd_idx - fsrd instance
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e mib_stat_fabric_fsrd_init(
    int unit,
    int fsrd_idx);

/**
* \brief
*   start counter thread
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_init_mib_thread_start(
    int unit);

/**
* \brief
*   stop counter thread
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_init_mib_thread_stop(
    int unit);

/**
* \brief
*   Callback pointer to obtain list of counters (addition and subtraction) from bcm counter type
* \param [in] unit - unit id
* \param [in] port - logical port
* \param [in] type - bcm counter type \see bcm_stat_val_t
* \param [in] max_nof_counters_to_add - maximal counters_to_add array size
* \param [in] max_nof_counters_subtract - maximal counters_to_subtract array size
* \param [out] nof_counters_to_add - actual counters_to_add array size
* \param [out] counters_to_add - counters to add
* \param [out] nof_counters_to_subtract - actual counters_to_subtract array size
* \param [out] counters_to_subtract - counters to subtract
* \return
*   shr_error_e - Error Type
* \remark
*   * This callback is only required for special cases where regular dnx data mapping is not enough.
* \see
*   * None
*/
typedef shr_error_e(
    *dnx_mib_counters_map_get_cb_f) (
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    int max_nof_counters_to_add,
    int max_nof_counters_subtract,
    int *nof_counters_to_add,
    dnx_mib_counter_t * counters_to_add,
    int *nof_counters_to_subtract,
    dnx_mib_counter_t * counters_to_subtract);

/**
* \brief - general cb for getting a list of hw mib counters given a bcm counter type. \see dnx_mib_counters_to_get_cb_f.
*/
shr_error_e dnx_mib_counters_map_get_cb(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    int max_nof_counters_to_add,
    int max_nof_counters_subtract,
    int *nof_counters_to_add,
    dnx_mib_counter_t * counters_to_add,
    int *nof_counters_to_subtract,
    dnx_mib_counter_t * counters_to_subtract);

/**
* \brief - cb for getting an empty list of hw mib counters given a bcm counter type. \see dnx_mib_counters_to_get_cb_f.
* This callback is used to not fail bcm_stat_get API for bcm types which are legal for other ports than the given port.
* For the given port type instead of failing we just return a counter value of 0.
*/
shr_error_e dnx_mib_counters_map_get_dummy_cb(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    int max_nof_counters_to_add,
    int max_nof_counters_subtract,
    int *nof_counters_to_add,
    dnx_mib_counter_t * counters_to_add,
    int *nof_counters_to_subtract,
    dnx_mib_counter_t * counters_to_subtract);

/**
 * \brief - DNX callback to get the length of MIB counters.
 * \remark - Currently it's assumed that counter length is common among diferent counter_set types.
 *
 * \see soc_get_counter_length_f.
 */
int dnx_mib_counter_length_get(
    int unit,
    int counter_id,
    int *length);

/**
 * \brief - DNX callback to read fmac MIB counters. \see soc_get_counter_f.
 */
int dnx_mib_fmac_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read);

/**
 * \brief - DNX callback to clear fmac MIB counters. \see soc_clear_counter_f.
 */
int dnx_mib_fmac_counter_clear(
    int unit,
    int counter_id,
    int port);

/**
 * \brief - DNX callback to read NIF (both ETH and ILKN) MIB counters. \see soc_get_counter_f.
 */
int dnx_mib_nif_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read);

/**
 * \brief - DNX callback dnx_mib_counter_tto clear NIF MIB (both ETH and ILKN) counters. \see soc_clear_counter_f.
 */
int dnx_mib_nif_counter_clear(
    int unit,
    int counter_id,
    int port);

/**
 * \brief - DNX callback to read ILKN MIB counters. \see soc_get_counter_f.
 */
int dnx_mib_ilkn_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read);

/**
 * \brief - DNX callback to clear ILKN MIB counters. \see soc_clear_counter_f.
 */
int dnx_mib_ilkn_counter_clear(
    int unit,
    int counter_id,
    int port);

/**
 * \brief - DNX callback to read NIF ETH MIB counters. \see soc_get_counter_f.
 */
int dnx_mib_nif_eth_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read);

/**
 * \brief - DNX callback to clear NIF ETH MIB counters. \see soc_clear_counter_f.
 */
int dnx_mib_nif_eth_counter_clear(
    int unit,
    int counter_id,
    int port);

/**
 * \brief - DNX callback to read flexe MAC MIB counters. \see soc_get_counter_f.
 */
int dnx_mib_flexe_mac_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read);

/**
 * \brief - DNX callback to clear flexe MAC ETH MIB counters. \see soc_clear_counter_f.
 */
int dnx_mib_flexe_mac_counter_clear(
    int unit,
    int counter_id,
    int port);

/**
 * \brief -
 *   Indication whether the given counter is enabled for the given port.
 *   This function is used to filter coutners per port.
 *
 * \param [in] unit - unit id
 * \param [in] port - logical port
 * \param [in] index - index to controlled counter array
 * \param [out] enable - Indication whether the counter is enabled
 * \param [out] printable - Indication whether the counter is printable
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   None
 * \see
 *   * None
 */
shr_error_e dnx_mib_counter_enable_get(
    int unit,
    bcm_port_t port,
    int index,
    int *enable,
    int *printable);

/**
 * \brief - DNX callback to get MIB counters. \see soc_get_counter_f.
 */
int dnx_mib_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read);

/**
 * \brief - DNX callback to clear MIB counters. \see soc_clear_counter_f.
 */
int dnx_mib_counter_clear(
    int unit,
    int counter_id,
    int port);

/**
* \brief
*   map bcm stat type and port to a set of controlled hw counters
* \param [in] unit - unit id
* \param [in] port - logical port
* \param [in] type - bcm stat type \see bcm_stat_val_t
* \param [in] max_array_size - maximal array size
* \param [out] add_counters - list of controlled counters to add
* \param [out] add_array_size - number of controlled counters to add
* \param [out] sub_counters - list of controlled counters to subtract
* \param [out] sub_array_size - number of controlled counters to subtract
* \return
*   shr_error_e - Error Type
* \remark
*   Counters list can be extract either directly from dnx data or from a callback provided by dnx data
* \see
*   * None
*/
shr_error_e dnx_mib_mapping_stat_get(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    int max_array_size,
    dnx_mib_counter_t * add_counters,
    int *add_array_size,
    dnx_mib_counter_t * sub_counters,
    int *sub_array_size);

#endif/*_DNX_mib_MGMT_INCLUDED__*/
