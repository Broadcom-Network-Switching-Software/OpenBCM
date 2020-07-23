/*! \file bcmpkt_rxpmd.h
 *
 * RX Packet Meta Data (RXPMD, called EP_TO_CPU in hardware) access interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_RXPMD_H
#define BCMPKT_RXPMD_H

#ifdef PKTIO_IMPL
#include <pktio_dep.h>
#else
#include <shr/shr_pb.h>
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#endif
#include <bcmpkt/bcmpkt_rxpmd_defs.h>
#include <bcmdrd/bcmdrd_types.h>

/*! RX raw packet metadata maximum size (words). */
#define BCMPKT_RXPMD_SIZE_WORDS         18

/*! RX raw packet metadata maximum size (bytes). */
#define BCMPKT_RXPMD_SIZE_BYTES         (BCMPKT_RXPMD_SIZE_WORDS * 4)

/*! CELL Error status bitmap. */
#define BCMPKT_RXMETA_ST_CELL_ERROR     (0x1 << 18)

/*! \brief Packet reasons bitmap.
 * Set of "reasons" (\ref BCMPKT_RX_REASON_XXX) why a packet came to the CPU.
 */
typedef struct bcmpkt_rx_reasons_s {
    /*! Bitmap container */
    SHR_BITDCLNAME(pbits, BCMPKT_RX_REASON_COUNT);
} bcmpkt_rx_reasons_t;

/*!
 * \name RXPMD Dumping flags.
 * \anchor BCMPKT_RXPMD_DUMP_F_XXX
 */
/*! \{ */
/*!
 * Dump all fields contents.
 */
#define BCMPKT_RXPMD_DUMP_F_ALL         0
/*!
 * Dump none zero field content only.
 */
#define BCMPKT_RXPMD_DUMP_F_NONE_ZERO   1
/*! \} */

/*!
 * \name Packet RX reason utility macros.
 * \anchor BCMPKT_RX_REASON_OPS
 */
/*! \{ */
/*!
 * Macro to check if a reason (\ref BCMPKT_RX_REASON_XXX) is included in a
 * set of reasons (\ref bcmpkt_rx_reasons_t). Returns:
 *   zero     => reason is not included in the set
 *   non-zero => reason is included in the set
 */
#define BCMPKT_RX_REASON_GET(_reasons, _reason) \
        SHR_BITGET(((_reasons).pbits), (_reason))

/*!
 * Macro to add a reason (\ref BCMPKT_RX_REASON_XXX) to a set of
 * reasons (\ref bcmpkt_rx_reasons_t)
 */
#define BCMPKT_RX_REASON_SET(_reasons, _reason) \
        SHR_BITSET(((_reasons).pbits), (_reason))

/*!
 * Macro to add all reasons (\ref BCMPKT_RX_REASON_XXX) to a set of
 * reasons (\ref bcmpkt_rx_reasons_t)
 */
#define BCMPKT_RX_REASON_SET_ALL(_reasons) \
        SHR_BITSET_RANGE(((_reasons).pbits), 0, BCMPKT_RX_REASON_COUNT)

/*!
 * Macro to clear a reason (\ref BCMPKT_RX_REASON_XXX) from a set of
 * reasons (\ref bcmpkt_rx_reasons_t)
 */
#define BCMPKT_RX_REASON_CLEAR(_reasons, _reason) \
        SHR_BITCLR(((_reasons).pbits), (_reason))

/*!
 * Macro to clear a set of reasons (\ref bcmpkt_rx_reasons_t).
 */
#define BCMPKT_RX_REASON_CLEAR_ALL(_reasons) \
        SHR_BITCLR_RANGE(((_reasons).pbits), 0, BCMPKT_RX_REASON_COUNT)
/*!
 * Macro to check for no reason (\ref bcmpkt_rx_reasons_t).
 */
#define BCMPKT_RX_REASON_IS_NULL(_reasons) \
        SHR_BITNULL_RANGE(((_reasons).pbits), \
                          0, BCMPKT_RX_REASON_COUNT)

/*!
 * Macro to iterate every reason (\ref bcmpkt_rx_reasons_t).
 */
#define BCMPKT_RX_REASON_ITER(_reasons, reason) \
    for(reason = BCMPKT_RX_REASON_NONE; reason < (int)BCMPKT_RX_REASON_COUNT; reason++) \
        if(BCMPKT_RX_REASON_GET(_reasons, reason))

/*!
 * Macro to get reasons number (\ref bcmpkt_rx_reasons_t).
 */
#define BCMPKT_RX_REASONS_COUNT(_reasons, _count) \
        SHR_BITCOUNT_RANGE(((_reasons).pbits), _count, \
                           0, BCMPKT_RX_REASON_COUNT)

/*!
 * Macro to compare 2 reasons (\ref bcmpkt_rx_reasons_t), return 1 for exact match.
 */
#define BCMPKT_RX_REASON_EQ(_reasons1, _reasons2) \
        SHR_BITEQ_RANGE(((_reasons1).pbits), ((_reasons2).pbits), \
                        0, BCMPKT_RX_REASON_COUNT)
/*! \} */

/*! \brief RXPMD field ID supported bit array.
 * Array of bits indicating whether a RXPMD field ID is supported by a given
 * device type.
 */
typedef struct bcmpkt_rxpmd_fid_support_s {
    /*! Field ID bitmap container */
    SHR_BITDCLNAME(fbits, BCMPKT_RXPMD_FID_COUNT);
} bcmpkt_rxpmd_fid_support_t;

/*!
 * \name Utility macros for \ref bcmpkt_rxpmd_fid_support_t.
 * \anchor BCMPKT_RXPMD_SUPPORT_OPS
 */
/*! \{ */
/*!
 * Macro to get a field ID's supported status.
 *
 * \retval zero Not supported
 * \retval non-zero Supported
 */
#define BCMPKT_RXPMD_FID_SUPPORT_GET(_support, _fid) \
        SHR_BITGET(((_support).fbits), (_fid))

/*!
 * Iterate over all supported RXPMD field IDs in the \c _support.
 */
#define BCMPKT_RXPMD_FID_SUPPORT_ITER(_support, _fid) \
    for(_fid = 0; _fid < BCMPKT_RXPMD_FID_COUNT; _fid++) \
        if(BCMPKT_RXPMD_FID_SUPPORT_GET(_support, _fid))
/*! \} */

/*!
 * \brief Get RXPMD's size for a given device type.
 *
 * \param [in] dev_type Device type.
 * \param [out] len Bytes of RXPMD length.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Unsupported device type or bad \c len pointer.
 * \retval SHR_E_UNAVAIL Not support RXPMD get function.
 */
extern int
bcmpkt_rxpmd_len_get(bcmdrd_dev_type_t dev_type, uint32_t *len);

/*!
 * \brief Get value from an RXPMD field.
 *
 * \param [in] dev_type Device type.
 * \param [in] rxpmd RXPMD handle.
 * \param [in] fid RXPMD field ID, refer to \ref BCMPKT_RXPMD_XXX.
 * \param [out] val Field value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_rxpmd_field_get(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd,
                       int fid, uint32_t *val);

/*!
 * \brief Set value into an RXPMD field. (Internally used for filter config.)
 *
 * \param [in] dev_type Device type.
 * \param [in,out] rxpmd RXPMD handle.
 * \param [in] fid RXPMD field ID, refer to \ref BCMPKT_RXPMD_XXX.
 * \param [in] val Set value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_rxpmd_field_set(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd,
                       int fid, uint32_t val);

/*!
 * \brief Get module header's pointer of the RXPMD.
 *
 * This function is used for geting Module header's pointer in RXPMD.
 *
 * \param [in] dev_type Device type.
 * \param [in] rxpmd RXPMD handle.
 * \param [out] hg_hdr HiGig header handle.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support HiGig header.
 * \retval SHR_E_INTERNAL Internal issue.
 */
extern int
bcmpkt_rxpmd_mh_get(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd,
                    uint32_t **hg_hdr);

/*!
 * \brief Get flex data handle from the RXPMD.
 *
 * This function is used for geting flex data handle from the \c rxpmd.
 *
 * \param [in] dev_type Device type.
 * \param [in] rxpmd RXPMD handle.
 * \param [out] flexdata Flex data handle.
 * \param [out] len Flex data size in 4-bytes.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support flex data.
 * \retval SHR_E_INTERNAL Internal issue.
 */
extern int
bcmpkt_rxpmd_flexdata_get(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd,
                          uint32_t **flexdata, uint32_t *len);
/*!
 * \brief Get RX reasons from RXPMD.
 *
 * Decode packet's RX reasons into "reasons". A received packet may have one RX
 * reason, multiple RX reasons, or none reason. RX reasons are in the format of
 * bitmap. Each bit means one reason type (refer to \ref BCMPKT_RX_REASON_XXX).
 *
 * User may use \ref BCMPKT_RX_REASON_OPS to parse each individual reason based
 * on this function's return value "reasons".
 *
 * \param [in] dev_type Device type.
 * \param [in] rxpmd RXPMD handle.
 * \param [out] reasons RX reasons in bit array.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support Reason.
 * \retval SHR_E_INTERNAL Internal issue.
 */
extern int
bcmpkt_rxpmd_reasons_get(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd,
                         bcmpkt_rx_reasons_t  *reasons);

/*!
 * \brief Set RX reasons into the RXPMD. (Internally used for filter configuration.)
 *
 * Set RX reasons into RXPMD data for packet filter purpose.
 *
 * \param [in] dev_type Device type.
 * \param [in] reasons Reasons bit array.
 * \param [in,out] rxpmd RXPMD handle.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support Reason.
 * \retval SHR_E_INTERNAL Internal issue.
 */
extern int
bcmpkt_rxpmd_reasons_set(bcmdrd_dev_type_t dev_type,
                         bcmpkt_rx_reasons_t *reasons, uint32_t *rxpmd);

/*!
 * \brief Get field name for a given RXPMD field ID.
 *
 * \param [in] fid RXPMD field ID, refer to \ref BCMPKT_RXPMD_XXX.
 * \param [out] name RXPMD field name string.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_rxpmd_field_name_get(int fid, char **name);

/*!
 * \brief Get field ID for a given RXPMD field name.
 *
 * \param [in] name RXPMD field name string.
 * \param [out] fid RXPMD Field ID.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_NOT_FOUND Not found the name.
 */
extern int
bcmpkt_rxpmd_field_id_get(char* name, int *fid);

/*!
 * \brief Get supported RXPMD field IDs for a given device type.
 *
 * This function returns a structure with information about the RXPMD field IDs
 * a given device type supports.
 *
 * Use \ref BCMPKT_RXPMD_FID_SUPPORT_GET on the returned structure to get the
 * supported status of a specific field ID.
 *
 * \param [in] dev_type Device type.
 * \param [out] support Field ID supported status bitmap.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_INTERNAL API internal error.
 */
extern int
bcmpkt_rxpmd_fid_support_get(bcmdrd_dev_type_t dev_type,
                             bcmpkt_rxpmd_fid_support_t *support);

/*!
 * \brief Dump all supported RXPMD fields into \c pb.
 *
 * If view_name is given, dump common fields and the fields belonging to the
 * view. If view_name is NULL, dump common fields and the fields of all view's.
 * If view_name is unknown, only dump common fields.
 *
 * \param [in] dev_type Device type.
 * \param [in] view_name RXPMD view name.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE success
 * \retval SHR_E_PARAM Check parameter failed
 * \retval SHR_E_INTERNAL API internal error.
 */
extern int
bcmpkt_rxpmd_field_list_dump(bcmdrd_dev_type_t dev_type, char *view_name,
                             shr_pb_t *pb);

/*!
 * \brief Dump RXPMD content into \c pb.
 *
 * This function is used for dumping the content of an RXPMD. If the
 * BCMPKT_RXPMD_DUMP_F_NONE_ZERO is set, only dump none zero fields.
 *
 * \param [in] dev_type Device type.
 * \param [in] rxpmd RXPMD handle.
 * \param [in] flags Refer to \ref BCMPKT_RXPMD_DUMP_F_XXX.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE success
 * \retval SHR_E_PARAM Check parameter failed
 * \retval SHR_E_INTERNAL API internal error.
 */
extern int
bcmpkt_rxpmd_dump(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd, uint32_t flags,
                  shr_pb_t *pb);

/*!
 * \brief Dump RX reasons into \c pb.
 *
 * \param [in] dev_type Device type.
 * \param [in] rxpmd RXPMD handle.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE success
 * \retval SHR_E_PARAM Check parameter failed
 * \retval SHR_E_UNAVAIL Not support Reason.
 * \retval SHR_E_INTERNAL API internal error.
 */
extern int
bcmpkt_rx_reason_dump(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd,
                      shr_pb_t *pb);

/*!
 * \brief Get an RX reason's name.
 *
 * \param [in] reason Reason ID.
 * \param [out] name Reason name string handle.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_rx_reason_name_get(int reason, char **name);

#endif /* BCMPKT_RXPMD_H */
