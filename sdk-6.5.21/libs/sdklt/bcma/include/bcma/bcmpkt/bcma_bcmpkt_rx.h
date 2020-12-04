/*! \file bcma_bcmpkt_rx.h
 *
 * General functions for Packet I/O Rx dispatcher.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <bcmpkt/bcmpkt_net.h>

#ifndef BCMA_BCMPKT_RX_H
#define BCMA_BCMPKT_RX_H

/*! To indicate that the callback function will be registered onto all network interfaces */
#ifndef BCMA_BCMPKT_NETIF_ALL
#define BCMA_BCMPKT_NETIF_ALL         (-1)
#endif

/*! Reserved network interface ID. */
#ifndef BCMA_BCMPKT_NETIF_RESERVED
#define BCMA_BCMPKT_NETIF_RESERVED    (0)
#endif

/*! Default maximum network interface value. */
#ifndef BCMA_BCMPKT_DEFAULT_NETIF_MAX
#define BCMA_BCMPKT_DEFAULT_NETIF_MAX (128)
#endif

/*! The highest callback priority, reserved for transparent packet watcher. */
#ifndef BCMA_BCMPKT_CB_PRIO_HIGHEST
#define BCMA_BCMPKT_CB_PRIO_HIGHEST    0
#endif

/*! The high callback priority. */
#ifndef BCMA_BCMPKT_CB_PRIO_HIGH
#define BCMA_BCMPKT_CB_PRIO_HIGH       50
#endif

/*! The default callback priority. */
#ifndef BCMA_BCMPKT_CB_PRIO_DEFAULT
#define BCMA_BCMPKT_CB_PRIO_DEFAULT    100
#endif

/*! The low callback priority. */
#ifndef BCMA_BCMPKT_CB_PRIO_LOW
#define BCMA_BCMPKT_CB_PRIO_LOW        200
#endif

/*!
 * \brief Return values from application Rx callback functions.
 */
typedef enum bcma_bcmpkt_rx_result_e {

    /*!
    * Packet is not terminated by the callback.
    * Dispatcher will deliver it to the next callback.
    */
    BCMA_BCMPKT_RX_NOT_HANDLED = 0,

    /*!
    * Packet is terminated by the callback.
    * Dispatcher will skip next callbacks.
    */
    BCMA_BCMPKT_RX_HANDLED = 1,

} bcma_bcmpkt_rx_result_t;

/*!
 * \brief Application Rx callback function type.
 *
 * Application Rx callback function type for receiving packets.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [in] packet Packet handle.
 * \param [in] cb_data Application-provided context.
 *
 * \return BCMA_BCMPKT_RX_XXX.
 */
typedef int (*bcma_bcmpkt_rx_cb_f)(int unit, int netif_id,
                                   bcmpkt_packet_t *packet, void *cb_data);

/*!
 * \brief Application Rx callback information structure.
 */
typedef struct bcma_bcmpkt_rx_cb_info_s {

    /*! Next callback info handle. */
    struct bcma_bcmpkt_rx_cb_info_s *next;

    /*! Leave for future. */
    uint32_t flags;

    /*! Callback function's priority(0 is highest). */
    uint8_t priority;

    /*! Callback function name. */
    const char *name;

    /*! Callback function. */
    bcma_bcmpkt_rx_cb_f cb_func;

    /*! Callback application contex. */
    void *cb_data;

    /*! True: Pending in callback deregistering state. */
    bool cb_pending;

} bcma_bcmpkt_rx_cb_info_t;

/*!
 * \brief Rx dispatcher information structure.
 */
typedef struct bcma_bcmpkt_dispatch_info_s {

    /*! Next Rx dispatcher info handle. */
    struct bcma_bcmpkt_dispatch_info_s *next;

    /*! Network interface ID. */
    int netif_id;

    /*! Dispatcher function. */
    bcmpkt_rx_cb_f dispatch_func;

    /*! Status flags, leave for future. */
    uint32_t flags;

    /*! Received packets. */
    uint64_t rx_pkts;

    /*! Received bytes. */
    uint64_t rx_bytes;

    /*! Packets handled. */
    uint64_t pkts_handled;

    /*! Packets with no Rx handler. */
    uint64_t non_hndlr_pkts;

    /*! Packets with invalid value returned from Rx handler. */
    uint64_t bad_hndlr_pkts;

    /*! Total number of callback functions. */
    uint32_t hndlr_cnt;

    /*! Total number of global callback functions. */
    uint32_t glb_hndlr_cnt;

    /*! The linked list head of callback functions. */
    bcma_bcmpkt_rx_cb_info_t *cb_info;

}bcma_bcmpkt_dispatch_info_t;

/*!
 * \brief Rx dispatcher traverse callback function type.
 *
 * \param [in] unit Switch unit number.
 * \param [in] dispatcher Rx dispatcher.
 * \param [in] cb_data Application-provided context.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_FAIL Failed.
 */
typedef int (*bcma_bcmpkt_dispatcher_traverse_cb_f) (int unit,
                                           const bcma_bcmpkt_dispatch_info_t *dispatcher,
                                           void *cb_data);

/*!
 * \brief Register an application callback to a network interface's dispatcher.
 *
 * User defines its application callback function and registeres it to a network interface's
 * dispatcher.
 *
 * The \c netif_id = \ref BCMA_BCMPKT_NETIF_ALL means to register the callback to all
 * interfaces' dispatchers. If application would like to deregister the global callback, \c
 * bcma_bcmpkt_rx_unregister(unit, \c BCMA_BCMPKT_NETIF_ALL, ...) should be used.
 *
 * The \c priority is used to determine the order in which these callback functions are
 *  called. The highest \c priority is 0 which is usually for transparent packet watcher.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [in] flags Reserved for future.
 * \param [in] priority Priority of callback function.
 * \param [in] name Name of callback function.
 * \param [in] cb_func Application callback function.
 * \param [in] cb_data Application-provided context.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID The netif_id is invalid or doesn't support SOCKET.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_MEMORY Allocate buffer failed.
 * \retval SHR_E_EXISTS Callback already exists.
 */
extern int
bcma_bcmpkt_rx_register(int unit, int netif_id, uint32_t flags,
                        uint8_t priority, const char *name,
                        bcma_bcmpkt_rx_cb_f cb_func, void *cb_data);

/*!
 *\brief deregister an application callback from a network interface's dispatcher.
 *
 * Removing callback function from a network interface's dispatcher.
 *
 * The \c netif_id = \ref BCMA_BCMPKT_NETIF_ALL is used to deregister the global
 * callback which was registered by calling
 * \c bcma_bcmpkt_rx_register(unit, \c BCMA_BCMPKT_NETIF_ALL, ...).
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [in] priority Priority of callback function.
 * \param [in] cb_func Application callback function.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Netif_id is invalid or doesn't support SOCKET.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcma_bcmpkt_rx_unregister(int unit, int netif_id, uint8_t priority,
                          bcma_bcmpkt_rx_cb_f cb_func);

/*!
 * \brief Rx packet dispatcher sample function.
 *
 * This function is used for receiving packets from network interface and
 * dispatching packets among upper layer user applications.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [in] packet Packet handle.
 * \param [in] cookie Application-provided context.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcma_bcmpkt_packet_process(int unit, int netif_id, bcmpkt_packet_t *packet,
                           void *cookie);

/*!
 *\brief Rx packet dispatcher creation function.
 *
 * Create Rx packet dispatcher onto a netif's socket.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [in] flags Reserved for future.
 * \param [in] dispatch_func Rx packet dispatcher function.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Netif_id is invalid or doesn't support SOCKET.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_MEMORY Allocate buffer failed.
 */
extern int
bcma_bcmpkt_rx_dispatcher_create(int unit, int netif_id, uint32_t flags,
                                 bcmpkt_rx_cb_f dispatch_func);

/*!
 *\brief Rx packet dispatcher destroy function.
 *
 * Destroy Rx packet dispatcher from a netif's socket.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Netif_id is invalid or doesn't support SOCKET.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcma_bcmpkt_rx_dispatcher_destroy(int unit, int netif_id);

/*!
 * \brief Rx dispatcher traverse function.
 *
 * This function is for traversing all netifs' dispatchers.
 * The callback function will be called for each present dispatcher. Refer to
 * \ref bcma_bcmpkt_dispatcher_traverse_cb_f for callback function type.
 *
 * \param [in] unit Switch unit number.
 * \param [in] cb_func  Application callback function.
 * \param [in] cb_data Application-provided context for callback function.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcma_bcmpkt_rx_dispatcher_traverse(int unit,
                                   bcma_bcmpkt_dispatcher_traverse_cb_f cb_func,
                                   void *cb_data);

/*!
 * \brief Validate network interface ID.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID to be checked.
 *
 * \retval TURE Supported ID.
 * \retval FALSE Unsupported ID.
 */
extern bool
bcma_bcmpkt_rx_netif_valid(int unit, int netif_id);

#endif /* BCMA_BCMPKT_RX_H */

