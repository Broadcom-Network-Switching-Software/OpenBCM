/*! \file bcmpkt_socket.h
 *
 * Interfaces for NET (BCMPKT TX and TX) management access.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_SOCKET_H
#define BCMPKT_SOCKET_H

#include <bcmpkt/bcmpkt_dev.h>
#include <shr/shr_pb.h>

/*!
 * SOCKET driver types.
 * BCMPKT_DEV_DRV_T_AUTO type is for attach function ONLY. When it's input type,
 * Packet I/O module will select an appropriate SOCKET driver.
 */
typedef enum bcmpkt_socket_drv_types_e {
    BCMPKT_SOCKET_DRV_T_NONE = BCMPKT_DEV_DRV_T_NONE, /*! No SOCKET driver. */
    BCMPKT_SOCKET_DRV_T_AUTO = BCMPKT_DEV_DRV_T_AUTO, /*! Automatically select driver. */
    BCMPKT_SOCKET_DRV_T_TPKT = BCMPKT_DEV_DRV_T_KNET, /*! Packet_mmap. */
    BCMPKT_SOCKET_DRV_T_RAWSOCK, /*! Raw socket. */
    BCMPKT_SOCKET_DRV_T_COUNT /*! Must be end. */
} bcmpkt_socket_drv_types_t;

/*!
 * \brief SOCKET configuration parameters.
 */
typedef struct bcmpkt_socket_cfg_s {

    /*! RX polling timeout value (ms). */
    int rx_poll_timeout;

    /*! RX configuration buffer size for mmap. Value 0 : use driver default value. */
    int rx_buf_size;

    /*! TX configuration buffer size for mmap. Value 0 : use driver default value. */
    int tx_buf_size;

} bcmpkt_socket_cfg_t;

/*!
 * \brief SOCKET Create function.
 *
 * Create SOCKET onto a netif. If \c cfg = NULL, default settings will be
 * used for SOCKET creation.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [in] cfg SOCKET configuration handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_MEMORY Allocate buffer failed.
 * \retval SHR_E_EXISTS SOCKET exists on the network interface.
 * \retval SHR_E_FAIL Other failure.
 */
typedef int (*bcmpkt_socket_create_f)(int unit, int netif_id,
                                      bcmpkt_socket_cfg_t *cfg);

/*!
 * \brief SOCKET destroy function.
 *
 * Destory SOCKET from a netif.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 */
typedef int (*bcmpkt_socket_destroy_f)(int unit, int netif_id);

/*!
 * \brief SOCKET created status get function.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [out] created SOCKET created status.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 */
typedef int (*bcmpkt_socket_created_f)(int unit, int netif_id, bool *created);

/*!
 * \brief Dump SOCKET statistics dump into \c pb.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 * \retval SHR_E_UNAVAIL Not support SOCKET.
 */
typedef int (*bcmpkt_socket_stats_dump_f) (int unit, int netif_id, shr_pb_t *pb);

/*!
 * \brief SOCKET statistics clear function.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 * \retval SHR_E_UNAVAIL Not support SOCKET.
 */
typedef int (*bcmpkt_socket_stats_clear_f) (int unit, int netif_id);

/*!
 * \brief SOCKET vectors.
 */
typedef struct bcmpkt_socket_s {

    /*! initialized flag: 0 - uninitialized 1 - initialized. */
    int initialized;

    /*! Driver name, such as "TPacket". */
    char driver_name[128];

    /*! SOCKET driver type. */
    bcmpkt_socket_drv_types_t driver_type;

    /*! Create SOCKET onto a netif. */
    bcmpkt_socket_create_f create;

    /*! Destroy SOCKET from a netif. */
    bcmpkt_socket_destroy_f destroy;

    /*! Get SOCKET created status. */
    bcmpkt_socket_created_f created;

    /*! Dump a SOCKET's statistics. */
    bcmpkt_socket_stats_dump_f stats_dump;

    /*! Clear a SOCKET's statistics. */
    bcmpkt_socket_stats_clear_f stats_clear;

} bcmpkt_socket_t;

/*!
 * \brief SOCKET driver register function.
 *
 * This function is used for registering a low level SOCKET driver.
 * For each device, the active SOCKET driver is set by \ref bcmpkt_socket_drv_attach.
 *
 * \param [in] socket_drv SOCKET driver handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_CONFIG Driver configuration mistake.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_socket_drv_register(bcmpkt_socket_t *socket_drv);

/*!
 * \brief Unregister a SOCKET driver.
 *
 * \param [in] type SOCKET driver type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid type.
 * \retval SHR_E_BUSY The driver is in using.
 */
extern int
bcmpkt_socket_drv_unregister(bcmpkt_socket_drv_types_t type);

/*!
 * \brief Attach a SOCKET drivre onto a device.
 *
 *
 * \param [in] unit Switch unit number.
 * \param [in] type SOCKET driver type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_PARAM Invalid type.
 * \retval SHR_E_EXISTS Driver exists.
 * \retval SHR_E_FAIL Attach failed.
 */
extern int
bcmpkt_socket_drv_attach(int unit, bcmpkt_socket_drv_types_t type);

/*!
 * \brief Detach SOCKET driver from a device.
 *
 * \param [in] unit Switch unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 */
extern int
bcmpkt_socket_drv_detach(int unit);

/*!
 * \brief Get SOCKET driver type.
 *
 * \param [in] unit Switch unit number.
 * \param [out] type SOCKET driver type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmpkt_socket_drv_type_get(int unit, bcmpkt_socket_drv_types_t *type);

/*!
 * \brief SOCKET Create function.
 *
 * Create a SOCKET on a netif. If \c cfg = NULL, default settings will be
 * used for SOCKET creation.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [in] cfg SOCKET configuration handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_MEMORY Allocate buffer failed.
 * \retval SHR_E_EXISTS SOCKET exists on the network interface.
 * \retval SHR_E_FAIL Other failure.
 */
extern int
bcmpkt_socket_create(int unit, int netif_id, bcmpkt_socket_cfg_t *cfg);

/*!
 * \brief SOCKET destroy function.
 *
 * Destory SOCKET from a netif.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 */
extern int
bcmpkt_socket_destroy(int unit, int netif_id);

/*!
 * \brief SOCKET created status get function.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [out] created SOCKET created status.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 */
extern int
bcmpkt_socket_created(int unit, int netif_id, bool *created);

/*!
 * \brief Dump SOCKET statistics into \c pb.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 * \retval SHR_E_UNAVAIL Not support SOCKET.
 */
extern int
bcmpkt_socket_stats_dump(int unit, int netif_id, shr_pb_t *pb);

/*!
 * \brief SOCKET statistics clear function.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 * \retval SHR_E_UNAVAIL Not support SOCKET.
 */
extern int
bcmpkt_socket_stats_clear(int unit, int netif_id);

#endif /* BCMPKT_SOCKET_H */
