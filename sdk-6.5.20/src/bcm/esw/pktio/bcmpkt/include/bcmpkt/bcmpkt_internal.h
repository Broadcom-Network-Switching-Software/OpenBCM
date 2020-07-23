/*! \file bcmpkt_internal.h
 *
 * Internal common head file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_INTERNAL_H
#define BCMPKT_INTERNAL_H

#ifdef PKTIO_IMPL
#include <pktio_dep.h>
#else
#include <sal/sal_types.h>
#include <shr/shr_types.h>
#include <shr/shr_pb.h>
#endif

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmpkt/bcmpkt_net.h>

/*! Get a field from a PMD buffer. */
typedef uint32_t (*bcmpkt_field_get_f)(uint32_t *data);

/*! Set a field within a PMD buffer. */
typedef void (*bcmpkt_field_set_f)(uint32_t *data, uint32_t val);

/*! Get a flex field from a PMD buffer. */
typedef int32_t (*bcmpkt_flex_field_get_f)(uint32_t *data, uint32_t profile, uint32_t *val);

/*! Set a flex field within a PMD buffer. */
typedef int32_t (*bcmpkt_flex_field_set_f)(uint32_t *data, uint32_t profile, uint32_t val);

/*! Get a complex field pointer or other attributions. */
typedef uint32_t (*bcmpkt_ifield_get_f)(uint32_t *data, uint32_t **addr);

/*! Get a field from a DOP Trace buffer. */
typedef uint32_t (*bcmpkt_dop_field_get_f)(uint32_t *data_in,
                                           uint32_t *data_out,
                                           uint32_t data_out_len);

/*! Get a field from a DOP Trace buffer. */
typedef uint32_t (*bcmpkt_pt_to_dop_info_get_f)(int unit, const char *pt_name,
                                                uint32_t port_id,
                                                uint32_t *info);

/*! Get a DOP data buffer. */
typedef uint32_t (*bcmpkt_dop_fids_get_f)(uint32_t *fid_list,
                                          uint8_t *fid_count);

/*! Get a Trace DOP attributions. */
typedef uint32_t (*bcmpkt_dop_iget_f)(void);

/*!
 * \brief RX callback information structure.
 */
typedef struct bcmpkt_rx_cb_info_s {

    /*! Next callback info handle. */
    struct bcmpkt_rx_cb_info_s *cb_next;

    /*! Callback flags. */
    uint32_t flags;

    /*! Callback function. */
    bcmpkt_rx_cb_f cb_func;

    /*! Callback application contex. */
    void *cb_data;

    /*! True: Pending in callback unregistering state. */
    bool cb_pending;

} bcmpkt_rx_cb_info_t;

/*!
 * \brief Packet metadata information structure.
 */
typedef struct bcmpkt_pmd_view_info_s {

    /*! View type list. */
    shr_enum_map_t *view_types;

    /*!
     * Each field's view code.
     * -2 means unavailable field.
     * -1 means common field.
     * others are correspondent view codes defined in view types.
     */
    int *view_infos;

    /*! View type get function. */
    bcmpkt_field_get_f view_type_get;

} bcmpkt_pmd_view_info_t;

/*!
 * \brief Check if a registered device driver is being actively used.
 *
 * \param [in] type Device driver type.
 *
 * \retval 1 Some device is using the driver.
 * \retval 0 No device is using the driver.
 */
extern int
bcmpkt_dev_drv_inuse(bcmpkt_dev_drv_types_t type);

/*!
 * \brief Check if a registered SOCKET driver is being actively used
 *
 * \param [in] type SOCKET driver type.
 *
 * \retval 1 Some device is using the driver.
 * \retval 0 No device is using the driver.
 */
extern int
bcmpkt_socket_drv_inuse(bcmpkt_socket_drv_types_t type);

/*!
 * \brief Get device ID.
 *
 * \param [in] unit Unit number.
 * \param [out] id Device ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input len is too small.
 * \retval SHR_E_NOT_FOUND Devuce name not found.
 */
extern int
bcmpkt_dev_id_get(int unit, uint32_t *id);

/*!
 * \brief Get RCPU header's configuration handle.
 *
 * \param [in] unit Switch unit number.
 *
 * \retval RCPU header's configuration handle.
 */
extern bcmpkt_rcpu_hdr_t *
bcmpkt_rcpu_hdr(int unit);

/*!
 * \brief Dump data into \c pb.
 *
 * \param [in,out] pb Print buffer handle.
 * \param [in] data Data to be printed.
 * \param [in] size print bytes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid type.
 */
extern int
bcmpkt_data_dump(shr_pb_t *pb, const uint8_t *data, int size);

/*!
 * \brief Dump data buffer into \c pb.
 *
 * \param [in,out] pb Print buffer handle.
 * \param [in] dbuf Data buffer to be printed.
 *
 * \retval None.
 */
extern void
bcmpkt_data_buf_dump(bcmpkt_data_buf_t *dbuf, shr_pb_t *pb);

#endif /* BCMPKT_INTERNAL_H */
