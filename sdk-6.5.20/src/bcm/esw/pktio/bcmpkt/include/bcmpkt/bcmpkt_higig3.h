/*! \file bcmpkt_higig3.h
 *
 * Higig3 access interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_HIGIG3_H
#define BCMPKT_HIGIG3_H

#ifdef PKTIO_IMPL
#include <pktio_dep.h>
#else
#include <sal/sal_types.h>
#include <shr/shr_pb.h>
#include <shr/shr_bitop.h>
#endif
#include <bcmpkt/bcmpkt_higig3_defs.h>
#include <bcmdrd/bcmdrd_types.h>

/* Note, ether type set to same value as reset value of R_GSH_ETHERTYPEr(700) */
/*! Ethernet type used for Higig3 header */
#define BCMPKT_HIGIG3_ETHER_TYPE                     0x2BC

/*! Higig3 base header size (bytes). */
#define BCMPKT_HIGIG3_BASE_HEADER_SIZE_BYTES         8
/*! Higig3 base header size (words). */
#define BCMPKT_HIGIG3_BASE_HEADER_SIZE_WORDS         2

/*! Higig3 extension 0 header size (bytes). */
#define BCMPKT_HIGIG3_EXT0_HEADER_SIZE_BYTES         8
/*! Higig3 extension 0 header size (words). */
#define BCMPKT_HIGIG3_EXT0_HEADER_SIZE_WORDS         2

/*! Higig3 header size (bytes). Includes base and ext0 header */
#define BCMPKT_HIGIG3_SIZE_BYTES       (BCMPKT_HIGIG3_BASE_HEADER_SIZE_BYTES + \
                                        BCMPKT_HIGIG3_EXT0_HEADER_SIZE_BYTES)
/*! Higig3 header size (words). Includes base and ext0 header */
#define BCMPKT_HIGIG3_SIZE_WORDS       (BCMPKT_HIGIG3_BASE_HEADER_SIZE_WORDS + \
                                        BCMPKT_HIGIG3_EXT0_HEADER_SIZE_WORDS)

/*!
 * \name HIGIG3 Dumping flags.
 * \anchor BCMPKT_HIGIG3_DUMP_F_XXX
 */
/*! \{ */
/*!
 * Dump all fields contents.
 */
#define BCMPKT_HIGIG3_DUMP_F_ALL         0
/*!
 * Dump none zero field content only.
 */
#define BCMPKT_HIGIG3_DUMP_F_NONE_ZERO   1
/*! \} */

/*! \brief HIGIG3 field ID supported bit array.
 * Array of bits indicating whether a HIGIG3 field ID is supported by a given
 * device type.
 */
typedef struct bcmpkt_higig3_fid_support_s {
    /*! Field ID bitmap container */
    SHR_BITDCLNAME(fbits, BCMPKT_HIGIG3_FID_COUNT);
} bcmpkt_higig3_fid_support_t;

/*!
 * \name Utility macros for \ref bcmpkt_higig3_fid_support_t.
 * \anchor BCMPKT_HIGIG3_SUPPORT_OPS
 */
/*! \{ */
/*!
 * Macro to get a field ID's supported status.
 *
 * \retval zero Not supported
 * \retval non-zero Supported
 */
#define BCMPKT_HIGIG3_FID_SUPPORT_GET(_support, _fid) \
        SHR_BITGET(((_support).fbits), (_fid))

/*!
 * Iterate over all supported HIGIG3 field IDs in the \c _support.
 */
#define BCMPKT_HIGIG3_FID_SUPPORT_ITER(_support, _fid) \
    for(_fid = 0; _fid < BCMPKT_HIGIG3_FID_COUNT; _fid++) \
        if(BCMPKT_HIGIG3_FID_SUPPORT_GET(_support, _fid))
/*! \} */

/*!
 * \brief Check if Higig3 is supported.
 *
 * \param [in] unit Device unit.
 * \param [in] dev_type Device type.
 * \param [in, out] is_supported: true if Higig3 is supported, false if not .
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not supported.
 */
extern int
bcmpkt_higig3_is_supported(int unit, bcmdrd_dev_type_t dev_type, bool *is_supported);

/*!
 * \brief Get value from a Higig3 field.
 *
 * \param [in] unit Device unit.
 * \param [in] dev_type Device type.
 * \param [in] higig3 Higig3 handle.
 * \param [in] fid Higig3 field ID, refer to \ref BCMPKT_HIGIG3_XXX.
 * \param [out] val Field value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_higig3_field_get(int unit, bcmdrd_dev_type_t dev_type, uint32_t *higig3,
                        int fid, uint32_t *val);

/*!
 * \brief Set value into a higig3 header field.
 *
 * \param [in] unit Device unit.
 * \param [in] dev_type Device type.
 * \param [in,out] higig3 Higig3 handle.
 * \param [in] fid Higig3 field ID, refer to \ref BCMPKT_HIGIG3_XXX.
 * \param [in] val Set value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_higig3_field_set(int unit, bcmdrd_dev_type_t dev_type, uint32_t *higig3,
                        int fid, uint32_t val);

/*!
 * \brief Get field name for a given Higig3 field ID.
 *
 * \param [in] fid Higig3 field ID, refer to \ref BCMPKT_HIGIG3_XXX.
 * \param [out] name Higig3 field name string.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_higig3_field_name_get(int fid, char **name);

/*!
 * \brief Get field ID for a given Higig3 field name.
 *
 * \param [in] name Higig3 name string.
 * \param [out] fid Higig3 Field ID.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_NOT_FOUND Not found the name.
 */
extern int
bcmpkt_higig3_field_id_get(char* name, int *fid);

/*!
 * \brief Get supported Higig3 field IDs for a given device type.
 *
 * This function returns a structure with information about the Higig3 field IDs
 * a given device type supports.
 *
 * Use \ref BCMPKT_HIGIG3_FID_SUPPORT_GET on the returned structure to get the
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
bcmpkt_higig3_fid_support_get(int unit, bcmdrd_dev_type_t dev_type,
                              bcmpkt_higig3_fid_support_t *support);

/*!
 * \brief Dump all supported HIGIG3 fields into \c pb.
 *
 * If view_name is given, dump common fields and the fields belonging to the
 * view. If view_name is NULL, dump common fields and the fields of all view's.
 * If view_name is unknown, only dump common fields.
 *
 * \param [in] unit Device unit.
 * \param [in] dev_type Device type.
 * \param [in] view_name HIGIG3 view name.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE success
 * \retval SHR_E_PARAM Check parameter failed
 * \retval SHR_E_INTERNAL API internal error.
 */
extern int
bcmpkt_higig3_field_list_dump(int unit, bcmdrd_dev_type_t dev_type,
                              char *view_name, shr_pb_t *pb);

/*!
 * \brief Dump Higig3 content into \c pb.
 *
 * This function is used for dumping the content of Higig3 header. If the
 * BCMPKT_HIGIG3_DUMP_F_NONE_ZERO is set, only dump none zero fields.
 *
 * \param [in] unit Device unit.
 * \param [in] dev_type Device type.
 * \param [in] higig3 Higig3 handle.
 * \param [in] flags Defined as \ref BCMPKT_HIGIG3_DUMP_F_XXX.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_higig3_dump(int unit, bcmdrd_dev_type_t dev_type, uint32_t *higig3,
                   uint32_t flags, shr_pb_t *pb);

#endif /* BCMPKT_HIGIG3_H */
