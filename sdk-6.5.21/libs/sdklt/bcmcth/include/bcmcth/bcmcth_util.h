/*! \file bcmcth_util.h
 *
 * BCMCTH util driver objects.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_UTIL_H
#define BCMCTH_UTIL_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief This macro is used to set a Field present indicator bit status.
 *
 * Sets the Field \c _f present bit indicator in the Field Bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Nothing.
 */
#define BCMCTH_LT_FIELD_SET(_m, _f)    \
            do {                        \
                SHR_BITSET(_m, _f);;      \
            } while (0)

/*!
 * \brief This macro is used to get the Field present indicator bit status.
 *
 * Returns the Field \c _f present bit indicator status from the Field
 * Bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Field present indicator bit status from the Field bitmap.
 */
#define BCMCTH_LT_FIELD_GET(_m, _f)  (SHR_BITGET(_m, _f))

/*! Fill a MAC address into an uint32 array. */
#define BCMCTH_MAC_ADDR_TO_UINT32(mac, dst) do {  \
        (dst)[0] = (((uint32_t)(mac)[2]) << 24 |  \
                    ((uint32_t)(mac)[3]) << 16 |  \
                    ((uint32_t)(mac)[4]) << 8  |  \
                    ((uint32_t)(mac)[5]));        \
        (dst)[1] = (((uint32_t)(mac)[0]) << 8 |   \
                    ((uint32_t)(mac)[1]));        \
    } while (0)

/*! Get a MAC address from an uint32 array. */
#define BCMCTH_MAC_ADDR_FROM_UINT32(mac, src)            \
    do {                                                 \
        (mac)[0] = (uint8_t) (((src)[1] >> 8)  & 0xff);  \
        (mac)[1] = (uint8_t) ((src)[1]         & 0xff);  \
        (mac)[2] = (uint8_t) (((src)[0] >> 24) & 0xff);  \
        (mac)[3] = (uint8_t) (((src)[0] >> 16) & 0xff);  \
        (mac)[4] = (uint8_t) (((src)[0] >> 8)  & 0xff);  \
        (mac)[5] = (uint8_t) ((src)[0]         & 0xff);  \
    } while (0)

/*! Set the upper 8 bytes of a shr_ip6_t from an uint64_t. */
#define BCMCTH_IPV6_ADDR_UPPER_SET(_ip6_, _u64_)            \
    do {                                                    \
        (_ip6_)[0] = (uint8_t) (((_u64_) >> 56) & 0xff);    \
        (_ip6_)[1] = (uint8_t) (((_u64_) >> 48) & 0xff);    \
        (_ip6_)[2] = (uint8_t) (((_u64_) >> 40) & 0xff);    \
        (_ip6_)[3] = (uint8_t) (((_u64_) >> 32) & 0xff);    \
        (_ip6_)[4] = (uint8_t) (((_u64_) >> 24) & 0xff);    \
        (_ip6_)[5] = (uint8_t) (((_u64_) >> 16) & 0xff);    \
        (_ip6_)[6] = (uint8_t) (((_u64_) >>  8) & 0xff);    \
        (_ip6_)[7] = (uint8_t) (((_u64_) >>  0) & 0xff);    \
    } while (0)

/*! Get the upper 8 bytes of an IPv6 address. */
#define BCMCTH_IPV6_ADDR_UPPER_GET(_ip6_)                   \
    (((uint64_t) ((_ip6_)[0])) << 56) |                     \
    (((uint64_t) ((_ip6_)[1])) << 48) |                     \
    (((uint64_t) ((_ip6_)[2])) << 40) |                     \
    (((uint64_t) ((_ip6_)[3])) << 32) |                     \
    (((uint64_t) ((_ip6_)[4])) << 24) |                     \
    (((uint64_t) ((_ip6_)[5])) << 16) |                     \
    (((uint64_t) ((_ip6_)[6])) <<  8) |                     \
    (((uint64_t) ((_ip6_)[7])) <<  0)

/*! Set the lower 8 bytes of a shr_ip6_t from an uint64_t. */
#define BCMCTH_IPV6_ADDR_LOWER_SET(_ip6_, _u64_)             \
    do {                                                     \
        (_ip6_)[8]  = (uint8_t) (((_u64_) >> 56) & 0xff);    \
        (_ip6_)[9]  = (uint8_t) (((_u64_) >> 48) & 0xff);    \
        (_ip6_)[10] = (uint8_t) (((_u64_) >> 40) & 0xff);    \
        (_ip6_)[11] = (uint8_t) (((_u64_) >> 32) & 0xff);    \
        (_ip6_)[12] = (uint8_t) (((_u64_) >> 24) & 0xff);    \
        (_ip6_)[13] = (uint8_t) (((_u64_) >> 16) & 0xff);    \
        (_ip6_)[14] = (uint8_t) (((_u64_) >>  8) & 0xff);    \
        (_ip6_)[15] = (uint8_t) (((_u64_) >>  0) & 0xff);    \
    } while (0)

/*! Get the lower 8 bytes of an IPv6 address. */
#define BCMCTH_IPV6_ADDR_LOWER_GET(_ip6_)                    \
    (((uint64_t) ((_ip6_)[8]))  << 56) |                     \
    (((uint64_t) ((_ip6_)[9]))  << 48) |                     \
    (((uint64_t) ((_ip6_)[10])) << 40) |                     \
    (((uint64_t) ((_ip6_)[11])) << 32) |                     \
    (((uint64_t) ((_ip6_)[12])) << 24) |                     \
    (((uint64_t) ((_ip6_)[13])) << 16) |                     \
    (((uint64_t) ((_ip6_)[14])) <<  8) |                     \
    (((uint64_t) ((_ip6_)[15])) <<  0)

/*!
 * \brief Structure for CTH related physical table operation.
 *
 * This structure is used to supply PTM API attributes used during Port related logical
 * table entry Read/Write operations to hardware/physical tables.
 */
typedef struct bcmcth_pt_op_info_s {

    /*! Device Resource Database Symbol ID. */
    bcmdrd_sid_t drd_sid;

    /*! Request LT symbol ID information. */
    bcmltd_sid_t req_lt_sid;

    /*! Response LT symbol ID information. */
    bcmltd_sid_t rsp_lt_sid;

    /*! LT operation transaction ID. */
    uint32_t trans_id;

    /*! Request PTM API option flags. */
    uint64_t req_flags;

    /*! Response PTM API option flags. */
    uint32_t rsp_flags;

    /*! PTM operation type. */
    bcmptm_op_type_t op;

    /*! Physical Table Entry Size in number of words. */
    size_t wsize;

    /*!
     * Dynamic address information of the physical table.
     * Physical Table Entry Hardware Index and Pipe Instances to Install Info.
     */
    bcmbd_pt_dyn_info_t dyn_info;

} bcmcth_pt_op_info_t;

/*!
 * \brief Writes an entry to a hardware table at a specified index location.
 *
 * \param [in] unit Unit number.
 * \param [in] op_info Pointer to bcmcth_pt_op_info_t structure.
 * \param [in] buf Pointer to hardware table entry write data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
extern int
bcmcth_pt_write(int unit,
                bcmcth_pt_op_info_t *op_info,
                uint32_t *buf);

/*!
 * \brief Read an entry from a hardware table index location.
 *
 * \param [in] unit Unit number.
 * \param [in] op_info Pointer to bcmcth_pt_op_info_t structure.
 * \param [out] buf Pointer to hardware table entry read data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware table read operation unsuccessful.
 */
extern int
bcmcth_pt_read(int unit,
               bcmcth_pt_op_info_t *op_info,
               uint32_t *buf);

/*!
 * \brief Writes an entry to a hardware table at a specified index location.
 *
 * \param [in] unit Unit number.
 * \param [in] op_info Pointer to bcmcth_pt_op_info_t structure.
 * \param [in] buf Pointer to hardware table entry write data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
extern int
bcmcth_irq_pt_write(int unit,
                    bcmcth_pt_op_info_t *op_info,
                    uint32_t *buf);


/*!
 * \brief Reads the field value from field list given
 *        field ID.
 *
 * \param [in] unit Unit number.
 * \param [in] in Field list.
 * \param [in] fid Field ID.
 * \param [out] fval Field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Field not found.
 */
extern int
bcmcth_imm_field_get(int unit,
                     const bcmltd_field_t *in,
                     uint32_t fid,
                     uint64_t *fval);

/*!
 * \brief Writes an entry to a hardware table at a specified index location.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table ID.
 * \param [in] pt_id Physical table ID.
 * \param [in] index Index to write to.
 * \param [in] entry_data Pointer to hardware table entry write data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
extern int
bcmcth_ireq_write(int unit,
                  bcmltd_sid_t lt_id,
                  bcmdrd_sid_t pt_id,
                  int index,
                  void *entry_data);

/*!
 * \brief Convert uint64 values into internal uint32 array.
 *
 *
 * \param [in] field_val64 uint64_t data value.
 * \param [out] field_val uint32_t array large enough to hold the data value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 */
extern int
bcmcth_uint64_to_uint32_array(uint64_t field_val64,
                              uint32_t *field_val);

/*!
 * \brief Convert an internal uint32_t array into a uint64_t value.
 *
 * \param [in] field_val uint32_t array large enough to hold the data value.
 * \param [out] field_val64 uint64_t data value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 */
extern int
bcmcth_uint32_array_to_uint64(uint32_t *field_val,
                              uint64_t *field_val64);

#endif /* BCMCTH_UTIL_H */
