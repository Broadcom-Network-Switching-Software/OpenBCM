/*! \file bcmsec_pt_internal.h
 *
 * SEC module interaction with physical table functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMSEC_PT_INTERNAL_H
#define BCMSEC_PT_INTERNAL_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmbd/bcmbd.h>
#include <bcmltd/bcmltd_types.h>

/*! Notify PT fn that the entry is an update. */
#define BCMSEC_PT_DYN_FLAGS_UPDATE      0x1

/*! Max entry width size. */
#define BCMSEC_MAX_ENTRY_WSIZE 24

/*! Max field width size. */
#define BCMSEC_MAX_FIELD_WSIZE 8


/*! SEC physical table info. */
typedef struct
bcmsec_pt_info_s {
    /*! Physical table index. */
    int index;

    /*! Physical table instance. */
    int tbl_inst;

    /*! Flags for PT */
    uint64_t flags;
} bcmsec_pt_info_t;

/*! Configure dynamic info passed to SEC module. */
#define BCMSEC_PT_DYN_INFO(pt_dyn_info, id, inst, flags) \
    do { \
        pt_dyn_info.index = id; \
        pt_dyn_info.tbl_inst = inst; \
        pt_dyn_info.flags = flags; \
    } while(0); \

/*! Structure for read-only fields got from PT. */
typedef struct sa_policy_encrypt_info_s {
    /*! Next packet number. */
    uint64_t    next_packet_number;

    /*! SA is in use. */
    uint8_t     in_use;

    /*! Start time since the SA was in use. */
    uint32_t    start_time;

    /*! Last stop time when the SA was in use. */
    uint32_t    stop_time;
} sa_policy_info_t;

/*!
 * \brief writes to the corresponding physical SID for indexed table.
 *
 * based on the index and instance passed in py_dyn_info the physical table is
 * populated with the content of ltmbuf
 *
 * \param [in] unit         Unit number.
 * \param [in] sid          Physical table sid.
 * \param [in] lt_id        Logical table sid.
 * \param [in] pt_dyn_info  Physical Table index and instance information.
 * \param [in] ltmbuf       data to be written to the physical table.
 *
 * \retval SHR_E_PARAM      Hardware table not found.
 * \retval SHR_E_NONE       In case of no failure.
 */
extern int
bcmsec_pt_indexed_write(int unit,
                       bcmdrd_sid_t sid,
                       bcmltd_sid_t lt_id,
                       bcmsec_pt_info_t *pt_dyn_info,
                       uint32_t *ltmbuf);


/*!
 * \brief reads from the corresponding physical SID for indexed table.
 *
 *
 * \param [in] unit         Unit number.
 * \param [in] sid          Physical table sid.
 * \param [in] lt_id        Logical table sid.
 * \param [in] pt_dyn_info  Physical Table index and instance information.
 * \param [in] ltmbuf       data to be written to the physical table.
 *
 * \retval SHR_E_PARAM      Hardware table not found.
 * \retval SHR_E_NONE       In case of no failure.
 */
extern int
bcmsec_pt_indexed_read(int unit,
                      bcmdrd_sid_t sid,
                      bcmltd_sid_t lt_id,
                      bcmsec_pt_info_t *pt_dyn_info,
                      uint32_t *ltmbuf);


/*!
 * \brief get the physical table field size
 *
 * return the size of a field in the physical table.
 *
 * \param [in] unit         Unit number.
 * \param [in] sid          Physical table sid.
 * \param [in] fid          Physical table field id.
 * \param [in] size         Field width.
 *
 * \retval   returns the size of the field.
 */
extern uint32_t
bcmsec_pt_fid_size_get(int unit,
                      bcmdrd_sid_t sid,
                      bcmdrd_fid_t fid,
                      uint32_t *size);

/*!
 * \brief prepares buffer for the physical table
 *
 * set the field value to the buffer for physical table write
 *
 * \param [in] unit         Unit number.
 * \param [in] sid          Physical table sid.
 * \param [in] fid          Physical table field id.
 * \param [in] entbuf       buffer to be updated.
 * \param [in] fbuf         field value.
 *
 * \retval SHR_E_NOT_FOUND  finfo not found for sid and fid.
 */
extern int
bcmsec_field_set(int unit,
                bcmdrd_sid_t sid,
                bcmdrd_fid_t fid,
                uint32_t *entbuf,
                uint32_t *fbuf);

/*!
 * \brief get field values for the fields physical table
 *
 * get the field value to the buffer for physical table
 *
 * \param [in] unit         Unit number.
 * \param [in] sid          Physical table sid.
 * \param [in] fid          Physical table field id.
 * \param [in] entbuf       buffer to be updated.
 * \param [in] fbuf         field value.
 *
 * \retval SHR_E_NOT_FOUND  finfo not found for sid and fid.
 */
extern int
bcmsec_field_get(int unit,
                bcmdrd_sid_t sid,
                bcmdrd_fid_t fid,
                uint32_t *entbuf,
                uint32_t *fbuf);

#endif /* BCMSEC_PT_INTERNAL_H */
