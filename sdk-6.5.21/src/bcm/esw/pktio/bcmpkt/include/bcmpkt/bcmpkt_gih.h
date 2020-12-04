/*! \file bcmpkt_gih.h
 *
 *  Generic Internal header(GIH) access interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_GIH_H
#define BCMPKT_GIH_H

#ifdef PKTIO_IMPL
#include <pktio_dep.h>
#else
#include <shr/shr_pb.h>
#include <shr/shr_bitop.h>
#include <sal/sal_types.h>
#endif
#include <bcmpkt/bcmpkt_generic_loopback_t_defs.h>
#include <bcmdrd/bcmdrd_types.h>

/*! GIH size (words). */
#define BCMPKT_GIH_SIZE_WORDS         4
/*! GIH size (bytes). */
#define BCMPKT_GIH_SIZE_BYTES         (BCMPKT_GIH_SIZE_WORDS * 4)
/*! GIH start of frame indicator. */
#define BCMPKT_GIH_START_OF_FRAME     0x2
/*! GIH start shift. */
#define BCMPKT_GIH_SHIFT              6
/*! GIH header type. */
#define BCMPKT_GIH_TYPE               0x2
/*! GIH start code. */
#define BCMPKT_GIH_START_CODE         ((BCMPKT_GIH_START_OF_FRAME << BCMPKT_GIH_SHIFT) \
                                       | BCMPKT_GIH_TYPE)
/*! GIH start. */
#define BCMPKT_GIH_START              BCMPKT_GIH_RESERVED_0


/*!
 * \name GIH Dumping flags.
 * \anchor BCMPKT_GIH_DUMP_F_XXX
 */
/*! \{ */
/*!
 * Dump all fields contents.
 */
#define BCMPKT_GIH_DUMP_F_ALL         0
/*!
 * Dump none zero field content only.
 */
#define BCMPKT_GIH_DUMP_F_NONE_ZERO   1
/*! \} */

/*!
 * \name BCMPKT_GIH_START encodings.
 * \anchor BCMPKT_GIH_START_XXX
 */
/*! \{ */
/*!
 * GIH start of frame indicator's value.
 */
#define BCMPKT_GIH_START_IND                   251
/*! \} */

/*! \brief GIH field ID supported bit array.
 * Array of bits indicating whether a GIH field ID is supported by a given
 * device type.
 */
typedef struct bcmpkt_gih_fid_support_s {
    /*! Field ID bitmap container */
    SHR_BITDCLNAME(fbits, BCMPKT_GIH_FID_COUNT);
} bcmpkt_gih_fid_support_t;

/*!
 * \name Utility macros for \ref bcmpkt_gih_fid_support_t.
 * \anchor BCMPKT_GIH_SUPPORT_OPS
 */
/*! \{ */
/*!
 * Macro to get a field ID's supported status.
 *
 * \retval zero Not supported
 * \retval non-zero Supported
 */
#define BCMPKT_GIH_FID_SUPPORT_GET(_support, _fid) \
        SHR_BITGET(((_support).fbits), (_fid))

/*!
 * Iterate over all supported GIH field IDs in the \c _support.
 */
#define BCMPKT_GIH_FID_SUPPORT_ITER(_support, _fid) \
    for(_fid = 0; _fid < BCMPKT_GIH_FID_COUNT; _fid++) \
        if(BCMPKT_GIH_FID_SUPPORT_GET(_support, _fid))
/*! \} */

/*!
 * \brief Get value from a GIH field.
 *
 * \param [in] unit Device unit.
 * \param [in] dev_type Device type.
 * \param [in] gih GIH handle.
 * \param [in] fid GIH field ID, refer to \ref BCMPKT_GIH_XXX.
 * \param [out] val Field value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_gih_field_get(int unit, bcmdrd_dev_type_t dev_type, uint32_t *gih,
                     int fid, uint32_t *val);

/*!
 * \brief Set value into a GIH field.
 *
 * \param [in] unit Device unit.
 * \param [in] dev_type Device type.
 * \param [in,out] gih GIH handle.
 * \param [in] fid GIH field ID, refer to \ref BCMPKT_GIH_XXX.
 * \param [in] val Set value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_gih_field_set(int unit, bcmdrd_dev_type_t dev_type, uint32_t *gih,
                     int fid, uint32_t val);

/*!
 * \brief Check if GIH is supported.
 *
 * \param [in] unit Device unit.
 * \param [in] dev_type Device type.
 * \param [in, out] is_supported Supported for GIH.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not supported.
 */
extern int
bcmpkt_gih_is_supported(int unit, bcmdrd_dev_type_t dev_type, bool *is_supported);

/*!
 * \brief Get field name for a given GIH field ID.
 *
 * \param [in] fid GIH field ID, refer to \ref BCMPKT_GIH_XXX.
 * \param [out] name GIH field name string.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_gih_field_name_get(int fid, char **name);

/*!
 * \brief Get field ID for a given GIH field name.
 *
 * \param [in] name GIH field name string.
 * \param [out] fid GIH Field ID.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_NOT_FOUND Not found the name.
 */
extern int
bcmpkt_gih_field_id_get(char* name, int *fid);

/*!
 * \brief Get supported GIH field IDs for a given device type.
 *
 * This function returns a structure with information about the GIH field IDs
 * a given device type supports.
 *
 * Use \ref BCMPKT_GIH_FID_SUPPORT_GET on the returned structure to get the
 * supported status of a specific field ID.
 *
 * \param [in] unit Device unit.
 * \param [in] dev_type Device type.
 * \param [out] support Field ID supported status bitmap.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_INTERNAL API internal error.
 */
extern int
bcmpkt_gih_fid_support_get(int unit, bcmdrd_dev_type_t dev_type,
                           bcmpkt_gih_fid_support_t *support);

/*!
 * \brief Dump all supported GIH fields into \c pb.
 *
 * If view_name is given, dump common fields and the fields belonging to the
 * view. If view_name is NULL, dump common fields and the fields of all view's.
 * If view_name is unknown, only dump common fields.
 *
 * \param [in] unit Device unit.
 * \param [in] dev_type Device type.
 * \param [in] view_name GIH view name.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE success
 * \retval SHR_E_PARAM Check parameter failed
 * \retval SHR_E_INTERNAL API internal error.
 */
extern int
bcmpkt_gih_field_list_dump(int unit, bcmdrd_dev_type_t dev_type,
                           char *view_name, shr_pb_t *pb);

/*!
 * \brief Dump GIH content into \c pb.
 *
 * This function is used for dumping the content of a GIH. If the
 * BCMPKT_GIH_DUMP_F_NONE_ZERO is set, only dump none zero fields.
 *
 * \param [in] unit Device unit.
 * \param [in] dev_type Device type.
 * \param [in] gih GIH handle.
 * \param [in] flags Defined as \ref BCMPKT_GIH_DUMP_F_XXX.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_gih_dump(int unit, bcmdrd_dev_type_t dev_type, uint32_t *gih,
                uint32_t flags, shr_pb_t *pb);

#endif /* BCMPKT_GIH_H */
