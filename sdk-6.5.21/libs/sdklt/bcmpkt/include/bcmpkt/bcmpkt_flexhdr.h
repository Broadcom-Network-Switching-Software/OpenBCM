/*! \file bcmpkt_flexhdr.h
 *
 *  Flexhdr access interface.
 */

#ifndef BCMPKT_FLEXHDR_H
#define BCMPKT_FLEXHDR_H

#include <shr/shr_pb.h>
#include <shr/shr_bitop.h>
#include <sal/sal_types.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmpkt/bcmpkt_pmd.h>

/*! Invalid profile ID. */
#define BCMPKT_FLEXHDR_PROFILE_NONE    -1

/*! CELL Error status bitmap. */
#define BCMPKT_RXFLEXMETA_ST_CELL_ERROR (0x1 << 18)

/*!
 * \name Packet FLEX reason utility macros.
 * \anchor BCMPKT_RXPMD_FLEX_REASON_OPS
 */
/*! \{ */
/*!
 * Macro to check if a reason is included in a
 * set of reasons (\ref bcmpkt_bitmap_t). Returns:
 *   zero     => reason is not included in the set
 *   non-zero => reason is included in the set
 */
#define BCMPKT_RXPMD_FLEX_REASON_GET(_reasons, _reason) \
        SHR_BITGET(((_reasons).pbits), (_reason))

/*!
 * Macro to add a reason to a set of
 * reasons (\ref bcmpkt_bitmap_t)
 */
#define BCMPKT_RXPMD_FLEX_REASON_SET(_reasons, _reason) \
        SHR_BITSET(((_reasons).pbits), (_reason))

/*!
 * Macro to clear a reason from a set of
 * reasons (\ref bcmpkt_bitmap_t)
 */
#define BCMPKT_RXPMD_FLEX_REASON_CLEAR(_reasons, _reason) \
        SHR_BITCLR(((_reasons).pbits), (_reason))

/*!
 * Macro to add all reasons to a set of reasons.
 */
#define BCMPKT_RXPMD_FLEX_REASON_SET_ALL(_reasons, _count) \
        SHR_BITSET_RANGE(((_reasons).pbits), 0, _count)

/*!
 * Macro to check for no reason.
 */
#define BCMPKT_RXPMD_FLEX_REASON_IS_NULL(_reasons, _count) \
        SHR_BITNULL_RANGE(((_reasons).pbits), 0, _count)

/*!
 * Macro to get reasons number.
 */
#define BCMPKT_RXPMD_FLEX_REASONS_COUNT(_reasons, _count, _reason_count) \
        SHR_BITCOUNT_RANGE(((_reasons).pbits), _count, 0, _reason_count)

/*!
 * Macro to compare 2 reasons, return 1 for exact match.
 */
#define BCMPKT_RXPMD_FLEX_REASON_EQ(_reasons1, _reasons2, _count) \
        SHR_BITEQ_RANGE(((_reasons1).pbits), ((_reasons2).pbits), \
                        0, _count)
/*! \} */

/*!
 * Flex header field info structure.
 */
typedef struct bcmpkt_flex_field_info_s {

    /*! Number of header fields. */
    int num_fields;

    /*! Header field names. */
    shr_enum_map_t *field_names;

} bcmpkt_flex_field_info_t;

/*!
 * \brief Get Header ID for a given flexhdr name.
 *
 * \param [in] variant Variant type.
 * \param [in] name flexhdr name string.
 * \param [out] hid flexhdr ID.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_NOT_FOUND Not found the name.
 */
extern int
bcmpkt_flexhdr_header_id_get(bcmlrd_variant_t variant,
                             char* name, uint32_t *hid);


/*!
 * \brief Check if flexhdr is supported.
 *
 * \param [in] variant Variant type.
 * \param [in] hid flexhdr ID.
 * \param [out] is_supported Supported for flexhdr.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not supported.
 */
extern int
bcmpkt_flexhdr_is_supported(bcmlrd_variant_t variant, uint32_t hid,
                            bool *is_supported);

/*!
 * \brief Get value from a flexhdr field.
 *
 * \param [in] variant Variant type.
 * \param [in] hid flexhdr ID.
 * \param [in] flexhdr flexhdr handle.
 * \param [in] profile Flexible data profile.
 * \param [in] fid flexhdr field ID.
 * \param [out] val Field value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_flexhdr_field_get(bcmlrd_variant_t variant, uint32_t hid,
                         uint32_t *flexhdr, int profile, int fid, uint32_t *val);

/*!
 * \brief Set value into a flexhdr field.
 *
 * \param [in] variant Variant type.
 * \param [in] hid flexhdr ID.
 * \param [in,out] flexhdr flexhdr handle.
 * \param [in] profile Flexible data profile.
 * \param [in] fid flexhdr field ID.
 * \param [in] val Set value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_flexhdr_field_set(bcmlrd_variant_t variant, uint32_t hid,
                         uint32_t *flexhdr, int profile, int fid, uint32_t val);

/*!
 * \brief Get field name for a given flexhdr field ID.
 *
 * \param [in] variant Variant type.
 * \param [in] hid flexhdr ID.
 * \param [in] fid flexhdr field ID.
 * \param [out] name flexhdr field name string.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_flexhdr_field_name_get(bcmlrd_variant_t variant, uint32_t hid,
                              int fid, char **name);

/*!
 * \brief Get field ID for a given flexhdr field name.
 *
 * \param [in] variant Variant type.
 * \param [in] hid flexhdr ID.
 * \param [in] name flexhdr field name string.
 * \param [out] fid flexhdr Field ID.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_NOT_FOUND Not found the name.
 */
extern int
bcmpkt_flexhdr_field_id_get(bcmlrd_variant_t variant, uint32_t hid,
                            char* name, int *fid);

/*!
 * \brief Get field info for a given flexhdr type.
 *
 * \param [in] variant Variant type.
 * \param [in] hid flexhdr ID.
 * \param [out] info field information.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_NOT_FOUND Not found the name.
 */
extern int
bcmpkt_flexhdr_field_info_get(bcmlrd_variant_t variant, uint32_t hid,
                              bcmpkt_flex_field_info_t *info);

/*!
 * \brief Dump all supported flexhdr fields into \c pb.
 *
 * If view_name is given, dump common fields and the fields belonging to the
 * view. If view_name is NULL, dump common fields and the fields of all view's.
 * If view_name is unknown, only dump common fields.
 *
 * \param [in] variant variant type.
 * \param [in] hid flexhdr ID.
 * \param [in] view_name flexhdr view name.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE success
 * \retval SHR_E_PARAM Check parameter failed
 * \retval SHR_E_INTERNAL API internal error.
 */
extern int
bcmpkt_flexhdr_field_list_dump(bcmlrd_variant_t variant, uint32_t hid,
                               char *view_name, shr_pb_t *pb);

/*!
 * \brief Dump flexhdr content into \c pb.
 *
 * This function is used for dumping the content of a flexhdr. If the
 * BCMPKT_DUMP_F_NONE_ZERO is set, only dump none zero fields.
 *
 * \param [in] variant Variant type.
 * \param [in] hid flexhdr ID.
 * \param [in] flexhdr flexhdr handle.
 * \param [in] flags Defined as \ref BCMPKT_DUMP_F_XXX.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_flexhdr_dump(bcmlrd_variant_t variant, uint32_t hid, uint32_t *flexhdr,
                    uint32_t flags, shr_pb_t *pb);

/*!
 * \brief Dump RXPMD_FLEX content into \c pb.
 *
 * This function is used for dumping the content of an RXPMD_FLEX. If the
 * BCMPKT_DUMP_F_NONE_ZERO is set, only dump none zero fields.
 *
 * \param [in] variant Variant type.
 * \param [in] rxpmd_flex RXPMD_FLEX handle.
 * \param [in] profile Flexible data profile.
 * \param [in] flags Refer to \ref BCMPKT_DUMP_F_XXX.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE success
 * \retval SHR_E_PARAM Check parameter failed
 * \retval SHR_E_INTERNAL API internal error.
 */
extern int
bcmpkt_rxpmd_flex_dump(bcmlrd_variant_t variant, uint32_t *rxpmd_flex,
                       uint32_t profile, uint32_t flags, shr_pb_t *pb);

/*!
 * \brief Get RX reasons from RXPMD_FLEX.
 *
 * Decode packet's RX reasons into "reasons". A received packet may have one RX
 * reason, multiple RX reasons, or none reason. RX reasons are in the format of
 * bitmap. Each bit means one reason type (refer to \ref BCMPKT_RX_REASON_XXX).
 *
 * User may use \ref BCMPKT_RXPMD_FLEX_REASON_OPS to parse each individual reason based
 * on this function's return value "reasons".
 *
 * \param [in] variant Variant type.
 * \param [in] rxpmd_flex RXPMD_FLEX handle.
 * \param [out] reasons RX reasons in bit array.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support Reason.
 * \retval SHR_E_INTERNAL Internal issue.
 */
extern int
bcmpkt_rxpmd_flex_reasons_get(bcmlrd_variant_t variant, uint32_t *rxpmd_flex,
                              bcmpkt_bitmap_t  *reasons);

/*!
 * \brief Set RX reasons into the RXPMD_FLEX. (Internally used for filter configuration.)
 *
 * Set RX reasons into RXPMD_FLEX data for packet filter purpose.
 *
 * \param [in] variant Variant type.
 * \param [in] reasons Reasons bit array.
 * \param [in,out] rxpmd_flex RXPMD_FLEX handle.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support Reason.
 * \retval SHR_E_INTERNAL Internal issue.
 */
extern int
bcmpkt_rxpmd_flex_reasons_set(bcmlrd_variant_t variant,
                              bcmpkt_bitmap_t *reasons, uint32_t *rxpmd_flex);

/*!
 * \brief Dump RX reasons into \c pb.
 *
 * \param [in] variant Variant type.
 * \param [in] rxpmd_flex RXPMD_FLEX handle.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE success
 * \retval SHR_E_PARAM Check parameter failed
 * \retval SHR_E_UNAVAIL Not support Reason.
 * \retval SHR_E_INTERNAL API internal error.
 */
extern int
bcmpkt_rxpmd_flex_reason_dump(bcmlrd_variant_t variant,
                              uint32_t *rxpmd_flex, shr_pb_t *pb);

/*!
 * \brief Get an RX reason's name.
 *
 * \param [in] variant Variant type.
 * \param [in] reason Reason ID.
 * \param [out] name Reason name string handle.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_rxpmd_flex_reason_name_get(bcmlrd_variant_t variant,
                                  int reason, char **name);

/*!
 * \brief Get max number of RX reason types.
 *
 * \param [in] variant Variant type.
 * \param [out] num Maximum RX reason types.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_rxpmd_flex_reason_max_get(bcmlrd_variant_t variant, uint32_t *num);

/*!
 * \brief Get reason ID for a given RX reason name.
 *
 * \param [in] variant Variant type.
 * \param [in] name Reason name string handle.
 * \param [out] rid Reason ID.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_NOT_FOUND Not found the name.
 */
extern int
bcmpkt_rxpmd_flex_reason_id_get(bcmlrd_variant_t variant,
                                char* name, int *rid);

#endif /* BCMPKT_FLEXHDR_H */
