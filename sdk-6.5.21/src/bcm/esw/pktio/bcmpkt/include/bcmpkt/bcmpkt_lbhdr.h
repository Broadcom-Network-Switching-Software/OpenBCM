/*! \file bcmpkt_lbhdr.h
 *
 * Loopback header (LBHDR, called LOOPBACK_MH in hardware) access interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_LBHDR_H
#define BCMPKT_LBHDR_H

#ifdef PKTIO_IMPL
#include <pktio_dep.h>
#else
#include <shr/shr_pb.h>
#include <shr/shr_bitop.h>
#include <sal/sal_types.h>
#endif
#include <bcmpkt/bcmpkt_lbhdr_defs.h>
#include <bcmdrd/bcmdrd_types.h>

/*! TX Packet MetaData size (bytes). */
#define BCMPKT_LBHDR_SIZE_BYTES         16
/*! TX Packet MetaData size (words). */
#define BCMPKT_LBHDR_SIZE_WORDS         4

/*!
 * \name LBHDR Dumping flags.
 * \anchor BCMPKT_LBHDR_DUMP_F_XXX
 */
/*! \{ */
/*!
 * Dump all fields contents.
 */
#define BCMPKT_LBHDR_DUMP_F_ALL         0
/*!
 * Dump none zero field content only.
 */
#define BCMPKT_LBHDR_DUMP_F_NONE_ZERO   1
/*! \} */

/*!
 * \name BCMPKT_LBHDR_START encodings.
 * \anchor BCMPKT_LBHDR_START_XXX
 */
/*! \{ */
/*!
 * Loopback header start of frame indicator's value.
 */
#define BCMPKT_LBHDR_START_IND                   251
/*! \} */

/*! \brief LBHDR field ID supported bit array.
 * Array of bits indicating whether a LBHDR field ID is supported by a given
 * device type.
 */
typedef struct bcmpkt_lbhdr_fid_support_s {
    /*! Field ID bitmap container */
    SHR_BITDCLNAME(fbits, BCMPKT_LBHDR_FID_COUNT);
} bcmpkt_lbhdr_fid_support_t;

/*!
 * \name Utility macros for \ref bcmpkt_lbhdr_fid_support_t.
 * \anchor BCMPKT_LBHDR_SUPPORT_OPS
 */
/*! \{ */
/*!
 * Macro to get a field ID's supported status.
 *
 * \retval zero Not supported
 * \retval non-zero Supported
 */
#define BCMPKT_LBHDR_FID_SUPPORT_GET(_support, _fid) \
        SHR_BITGET(((_support).fbits), (_fid))

/*!
 * Iterate over all supported LBHDR field IDs in the \c _support.
 */
#define BCMPKT_LBHDR_FID_SUPPORT_ITER(_support, _fid) \
    for(_fid = 0; _fid < BCMPKT_LBHDR_FID_COUNT; _fid++) \
        if(BCMPKT_LBHDR_FID_SUPPORT_GET(_support, _fid))
/*! \} */

/*!
 * \brief Get value from a LBHDR field.
 *
 * \param [in] dev_type Device type.
 * \param [in] lbhdr LBHDR handle.
 * \param [in] fid LBHDR field ID, refer to \ref BCMPKT_LBHDR_XXX.
 * \param [out] val Field value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_lbhdr_field_get(bcmdrd_dev_type_t dev_type, uint32_t *lbhdr,
                       int fid, uint32_t *val);

/*!
 * \brief Set value into a LBHDR field.
 *
 * \param [in] dev_type Device type.
 * \param [in,out] lbhdr LBHDR handle.
 * \param [in] fid LBHDR field ID, refer to \ref BCMPKT_LBHDR_XXX.
 * \param [in] val Set value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_lbhdr_field_set(bcmdrd_dev_type_t dev_type, uint32_t *lbhdr,
                       int fid, uint32_t val);

/*!
 * \brief Get field name for a given LBHDR field ID.
 *
 * \param [in] fid LBHDR field ID, refer to \ref BCMPKT_LBHDR_XXX.
 * \param [out] name LBHDR field name string.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_lbhdr_field_name_get(int fid, char **name);

/*!
 * \brief Get field ID for a given LBHDR field name.
 *
 * \param [in] name LBHDR field name string.
 * \param [out] fid LBHDR Field ID.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_NOT_FOUND Not found the name.
 */
extern int
bcmpkt_lbhdr_field_id_get(char* name, int *fid);

/*!
 * \brief Get supported LBHDR field IDs for a given device type.
 *
 * This function returns a structure with information about the LBHDR field IDs
 * a given device type supports.
 *
 * Use \ref BCMPKT_LBHDR_FID_SUPPORT_GET on the returned structure to get the
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
bcmpkt_lbhdr_fid_support_get(bcmdrd_dev_type_t dev_type,
                             bcmpkt_lbhdr_fid_support_t *support);

/*!
 * \brief Dump all supported LBHDR fields into \c pb.
 *
 * If view_name is given, dump common fields and the fields belonging to the
 * view. If view_name is NULL, dump common fields and the fields of all view's.
 * If view_name is unknown, only dump common fields.
 *
 * \param [in] dev_type Device type.
 * \param [in] view_name LBHDR view name.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE success
 * \retval SHR_E_PARAM Check parameter failed
 * \retval SHR_E_INTERNAL API internal error.
 */
extern int
bcmpkt_lbhdr_field_list_dump(bcmdrd_dev_type_t dev_type, char *view_name,
                             shr_pb_t *pb);

/*!
 * \brief Dump LBHDR content into \c pb.
 *
 * This function is used for dumping the content of a LBHDR. If the
 * BCMPKT_LBHDR_DUMP_F_NONE_ZERO is set, only dump none zero fields.
 *
 * \param [in] dev_type Device type.
 * \param [in] lbhdr LBHDR handle.
 * \param [in] flags Defined as \ref BCMPKT_LBHDR_DUMP_F_XXX.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_lbhdr_dump(bcmdrd_dev_type_t dev_type, uint32_t *lbhdr, uint32_t flags,
                  shr_pb_t *pb);

#endif /* BCMPKT_LBHDR_H */
