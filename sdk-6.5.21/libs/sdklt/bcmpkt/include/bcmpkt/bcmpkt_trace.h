/*! \file bcmpkt_trace.h
 *
 * Packet trace API interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_TRACE_H
#define BCMPKT_TRACE_H

#include <shr/shr_pb.h>
#include <shr/shr_bitop.h>
#include <bcmpkt/bcmpkt_trace_defs.h>

/*! Maximum size of raw trace data (in bytes). */
#define BCMPKT_TRACE_SIZE_BYTES        112

/*!
 * \name Trace options.
 * \anchor BCMPKT_TRACE_OPT_xxx
 */

/*! \{ */

/*! Learning enable on trace packet. */
#define BCMPKT_TRACE_OPT_LEARN         0x1

/*! Disable ingress field processor lookup on trace packet. */
#define BCMPKT_TRACE_OPT_NO_IFP        0x2

/*! Drop trace packet to TM. */
#define BCMPKT_TRACE_OPT_DROP_TM       0x4

/*! \} */

/*! \brief Packet trace structure.
 *
 * Trace information used to construct trace packet.
 */
typedef struct bcmpkt_trace_pkt_s {

    /*! Trace options (\ref BCMPKT_TRACE_OPT_xxx). */
    uint32_t options;

    /*! Local front panel port that acts as a masquerade port. */
    int port;

    /*! Trace packet length. */
    int len;

    /*! Network interface ID. */
    int netif_id;

    /*! Trace packet data. */
    uint8_t *pkt;

} bcmpkt_trace_pkt_t;

/*! Trace data structure.
 *
 * Raw trace data.
 */
typedef struct bcmpkt_trace_data_s {

    /*! Amount of available trace data (may exceed buf_size). */
     uint32_t len;

    /*! Trace data buffer . */
    uint8_t *buf;

    /*! Size of trace data buffer. */
    uint32_t buf_size;

} bcmpkt_trace_data_t;

/*! Trace DOP info structure.
 *
 * DOP info.
 */
typedef struct bcmpkt_trace_dop_info_s {

    /*! DOP ID. */
    uint32_t    dop_id;

    /*! DOP Group ID. Ingress(0) or Egress(1). */
    uint32_t    group_id;

}bcmpkt_trace_dop_info_t;

/*!
 * \brief Initialize trace function.
 *
 * This function is used to attach the trace driver and initialize the trace
 * profile.
 *
 * \param [in] unit Device number.
 * \param [in] warm Set to true for warm-boot start-up sequence.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_trace_init(int unit, bool warm);

/*!
 * \brief Get value of an trace info field.
 *
 * \param [in] unit Device number.
 * \param [in] data Pointer to raw trace data.
 * \param [in] fid Trace field ID, refer to \ref BCMPKT_TRACE_XXX.
 * \param [out] val Field value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_trace_field_get(int unit, const uint8_t *data,
                       int fid, uint32_t *val);

/*!
 * \brief Send out a trace packet and get trace raw data.
 *
 * \param [in] unit Device number.
 * \param [in] trace_pkt Trace packet information.
 * \param [out] trace_data Raw trace data from trace packet.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_trace_data_collect(int unit, bcmpkt_trace_pkt_t *trace_pkt,
                          bcmpkt_trace_data_t *trace_data);

/*!
 * \brief Reads the raw DOP data for a given DOP ID and DOP group.
 *
 * \param [in]  unit        Device number.
 * \param [in]  port        Port number.
 * \param [in]  dop_num     BCMPKT_TRACE_DOP_ID_XXX DOP number.
 * \param [out] trace_data  Raw trace DOP data from trace packet.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_trace_dop_data_collect(int unit, int port,
                              uint32_t dop_num,
                              bcmpkt_trace_data_t *trace_data);

/*!
 * \brief Convert BCMPKT_TRACE_DOP_ID_XXX DOP number format to dop info.
 *
 * \param [in]      unit            Device number.
 * \param [in]      dop_num         BCMPKT_TRACE_DOP_ID_XXX DOP number.
 * \param [out]     dop_info        DOP info.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_trace_dop_num_to_dop_info_get (int unit,
                                      uint32_t dop_num,
                                      bcmpkt_trace_dop_info_t *dop_info);

/*!
 * \brief Output the list of DOP field IDs for a given DOP ID and DOP group.
 *
 * \param [in]  unit        Device number.
 * \param [in]  dop_num     BCMPKT_TRACE_DOP_ID_XXX DOP number.
 * \param [out] fid_list    List of DOP field Ids.
 * \param [inout] fid_count Input: fid_list size.
 *                          Output: Number of valid entries in fid_list.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_trace_dop_fids_get(int unit,
                          uint32_t dop_num,
                          uint32_t *fid_list, uint8_t *fid_count);

/*!
 * \brief Extracts DOP field value for a given DOP field ID.
 *
 * \param [in]  unit        Device number.
 * \param [in]  raw_data    Raw DOP data.
 * \param [in]  fid         DOP field Id.
 * \param [out] field_data  DOP field data.
 * \param [inout] field_data_len  DOP Field Data len.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_trace_dop_field_get(int unit, const uint8_t *raw_data,
                           int fid, uint32_t *field_data,
                           uint8_t *field_data_len);

/*!
 * \brief Output PT lookup info for a given PT ID.
 *
 * \param [in]      unit            Device number.
 * \param [in]      pt_name         PT Name.
 * \param [in]      data            DOP Data.
 * \param [in]      port            Ingress Port.
 * \param [out]     pt_lookup       PT lookup done or not.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_trace_pt_lookup_info_get (int unit, const char *pt_name,
                                 uint8_t *data,
                                 int port,
                                 uint32_t *pt_lookup);

/*!
 * \brief Output PT lookup hit/miss info for a given PT ID.
 *
 * \param [in]      unit            Device number.
 * \param [in]      pt_name         PT Name.
 * \param [in]      data            DOP Data.
 * \param [in]      port            Ingress Port.
 * \param [out]     pt_hit          PT lookup hit/miss
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_trace_pt_hit_info_get (int unit, const char *pt_name,
                              uint8_t *data,
                              int port,
                              uint32_t *pt_hit);

/*!
 * \brief Returns the count of egress DOPs.
 *
 * \param [in]      unit            Device number.
 * \param [out]     count           Egress DOP Count
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_trace_egr_dop_cnt_get(int unit, uint32_t *count);

/*!
 * \brief Returns the count of ingress DOPs.
 *
 * \param [in]      unit            Device number.
 * \param [out]     count           Ingress DOP Count
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_trace_ing_dop_cnt_get(int unit, uint32_t *count);

/*!
 * \brief Output is the size/phase (word count) of the largest DOP.
 *
 * \param [in]      unit            Device number.
 * \param [out]     size            Max Dop size in word count.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_trace_max_dop_phase_get(int unit, uint32_t *size);

/*!
 * \brief Output is the max DOP field count.
 *
 * \param [in]      unit            Device number.
 * \param [out]     count           Max Dop field count.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_trace_max_dop_field_count_get(int unit, uint32_t *count);

/*!
 * \brief Parse trace raw data and dump result into \c pb.
 *
 * This function is used for parsing trace raw data and dumping the
 * parse result of the raw data.
 *
 * \param [in] unit Unit number.
 * \param [in] data Trace raw data handle.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE success
 * \retval SHR_E_PARAM Check parameter failed
 * \retval SHR_E_INTERNAL API internal error.
 */
extern int
bcmpkt_trace_fields_dump(int unit, const uint8_t *data, shr_pb_t *pb);

/*!
 * \brief Get Trace data's size.
 *
 * \param [in] unit Unit number.
 * \param [out] len Bytes of trace data length.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Unsupported device type or bad \c len pointer.
 * \retval SHR_E_UNAVAIL Not support trace data length get function.
 */
extern int
bcmpkt_trace_data_len_get(int unit, uint32_t *len);

/*!
 * \brief Packet trace drop reasons bitmap.
 * Set of "reasons" (\ref BCMPKT_TRACE_DROP_REASON_XXX) why a trace
 * packet is dropped.
 */
typedef struct bcmpkt_trace_drop_reasons_s {
    /*! Trace drop reason bitmap. */
    SHR_BITDCLNAME(pbits, BCMPKT_TRACE_DROP_REASON_COUNT);
} bcmpkt_trace_drop_reasons_t;

/*!
 * \name Packet trace drop reason utility macros.
 * \anchor BCMPKT_TRACE_DROP_REASON_OPS
 */
/*! \{ */
/*!
 * Macro to check if a drop reason (\ref BCMPKT_TRACE_DROP_REASON_XXX) is
 * included in a set of drop reasons (\ref bcmpkt_trace_drop_reasons_t).
 * Returns:
 *   zero     => Drop reason is not included in the set
 *   non-zero => Drop reason is included in the set
 */
#define BCMPKT_TRACE_DROP_REASON_GET(_reasons, _reason) \
        SHR_BITGET(((_reasons).pbits), (_reason))

/*!
 * Macro to add a drop reason (\ref BCMPKT_TRACE_DROP_REASON_XXX) to a
 * set of drop reasons (\ref bcmpkt_trace_drop_reasons_t)
 */
#define BCMPKT_TRACE_DROP_REASON_SET(_reasons, _reason) \
        SHR_BITSET(((_reasons).pbits), (_reason))

/*!
 * Macro to clear a set of drop reasons (\ref BCMPKT_TRACE_DROP_REASON_XXX)
 */
#define BCMPKT_TRACE_DROP_REASON_CLEAR_ALL(_reasons) \
        SHR_BITCLR_RANGE(((_reasons).pbits), 0, BCMPKT_TRACE_DROP_REASON_COUNT)

/*!
 * \brief Packet trace counter bitmap.
 * Set of "counter" (\ref BCMPKT_TRACE_COUNTER_XXX) where a trace
 * packet is counted.
 */
typedef struct bcmpkt_trace_counters_s {
    /*! Trace counter bitmap. */
    SHR_BITDCLNAME(pbits, BCMPKT_TRACE_COUNTER_COUNT);
} bcmpkt_trace_counters_t;

#define BCMPKT_TRACE_COUNTER_GET(_reasons, _reason) \
        SHR_BITGET(((_reasons).pbits), (_reason))

#define BCMPKT_TRACE_COUNTER_SET(_reasons, _reason) \
        SHR_BITSET(((_reasons).pbits), (_reason))

#define BCMPKT_TRACE_COUNTER_CLEAR_ALL(_reasons) \
        SHR_BITCLR_RANGE(((_reasons).pbits), 0, BCMPKT_TRACE_COUNTER_COUNT)

/*!
 * \brief Get drop reason of trace info.
 *
 * \param [in] unit Device number.
 * \param [in] data Pointer to raw trace data.
 * \param [out] bitmap Drop reason bitmap, refer to \ref BCMPKT_TRACE_DROP_REASON_XXX.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_trace_drop_reasons_get(int unit, const uint8_t *data,
                              bcmpkt_trace_drop_reasons_t *bitmap);

/*!
 * \brief Get counter of trace info.
 *
 * \param [in] unit Device number.
 * \param [in] data Pointer to raw trace data.
 * \param [out] bitmap Counter bitmap, refer to \ref BCMPKT_TRACE_COUNTER_XXX.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_trace_counter_get(int unit, const uint8_t *data,
                         bcmpkt_trace_counters_t *bitmap);

#endif /* BCMPKT_TRACE_H */
