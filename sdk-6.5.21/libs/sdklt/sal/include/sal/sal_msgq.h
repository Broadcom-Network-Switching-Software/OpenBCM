/*! \file sal_msgq.h
 *
 * Message queue API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_MSGQ_H
#define SAL_MSGQ_H

#include <sal/sal_types.h>

/*!
 * Use this value to disable timeout for \ref sal_msgq_recv and \ref
 * sal_msgq_post.
 */
#define SAL_MSGQ_FOREVER        (uint32_t)(-1)

/*!
 * Use this timeout value to return immediately from \ref
 * sal_msgq_post if the send queue is full or from \ref sal_msgq_recv
 * if no message is ready.
 */
#define SAL_MSGQ_NOWAIT         0

/*! Special return value to indicate that the operation timed out. */
#define SAL_MSGQ_E_TIMEOUT      -3

/*! Invalid parameter error code */
#define SAL_MSGQ_E_INVALID_PARAM    -4

/*! Supported message priorities. */
typedef enum {
    SAL_MSGQ_MIN_PRIORITY = 0,
    SAL_MSGQ_NORMAL_PRIORITY = SAL_MSGQ_MIN_PRIORITY,
    SAL_MSGQ_HIGH_PRIORITY = 1,
    SAL_MSGQ_MAX_PRIORITY = SAL_MSGQ_HIGH_PRIORITY
} sal_msgq_priority_t;

/*! Number of supported message priorities. */
#define SAL_MSGQ_PRIORITIES     (SAL_MSGQ_MAX_PRIORITY + 1)

/*! Opaque message queue handle. */
typedef struct sal_msgq_s *sal_msgq_t;

/*!
 * \brief Create message queue
 *
 * \param [in] element_size The size of elements in the queue.
 *              All elements must have the same size
 * \param [in] max_elements The maximum number of elements that can be
 *              queued.
 * \param [in] desc String to identify an object during debug.
 *
 * \return Message queue handle on success or NULL on failure.
 */
extern sal_msgq_t
sal_msgq_create(size_t element_size, size_t max_elements, const char *desc);

/*!
 * \brief Destroy a message queue
 *
 * \param [in] msgq_hdl Message queue handle.
 *
 * \return Nothing.
 */
extern void
sal_msgq_destroy(sal_msgq_t msgq_hdl);

/*!
 * \brief Post a message to the queue
 *
 * \param [in] msgq_hdl Message queue handle
 * \param [in] element Element to push onto the message queue.
 * \param [in] pri Message priority.
 * \param [in] usec Send timeout value. If the queue is full, the function
 *              will block for up to \c usec microseconds.
 *
 * \return 0 on success, SAL_MSGQ_E_TIMEOUT on timeout, non-zero value
 *              if other failure.
 */
extern int
sal_msgq_post(sal_msgq_t msgq_hdl,
              void *element,
              sal_msgq_priority_t pri,
              uint32_t usec);

/*!
 * \brief Receive a message from the queue
 *
 * \param [in] msgq_hdl Message queue handle
 * \param [in] element Buffer where the content of the element will be
 *              copied into. Note that the memory size must be larger
 *              or equal to the element size used in \ref sal_msgq_create.
 * \param [in] usec Receive timeout value. If the queue is empty the
 *              function will wait for up to \c usec microseconds.
 *
 * \return 0 on success, SAL_MSGQ_E_TIMEOUT on timeout, non-zero value
 *              if other failure.
 */
extern int
sal_msgq_recv(sal_msgq_t msgq_hdl, void *element, uint32_t usec);

/*!
 * \brief Reports the number of pending elements in the queue
 *
 * \param [in] msgq_hdl Message queue handle
 *
 * \return The number of elements that are currently pending in the queue.
 * Value of 0 indicates either empty queue or invalid queue handle.
 */
extern size_t
sal_msgq_count_get(sal_msgq_t msgq_hdl);

#endif /* SAL_MSGQ_H */
