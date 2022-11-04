/*! \file edk_int.h
 *
 * EDKHOST APIs.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef EDK_INT_H
#define EDK_INT_H

/*#include <sal/sal_types.h>*/
/* Only for stubs */

/*! __ERROR_H__ */
/*! Success */
#define EDK_ERR_NONE         (0)
/*! Generic Failure */
#define EDK_ERR_FAILURE      (-1)
/*! Insufficient Memory */
#define EDK_ERR_NOMEM        (-2)
/*! Timeout */
#define EDK_ERR_TIMEOUT      (-3)
/*! Access Fault */
#define EDK_ERR_FAULT        (-4)
/*! Invalid Arguments */
#define EDK_ERR_INVALID      (-5)
/*! I/O Error */
#define EDK_ERR_IO           (-6)
/*! Busy */
#define EDK_ERR_BUSY         (-7)
/*! Empty */
#define EDK_ERR_EMPTY        (-8)
/*! Full */
#define EDK_ERR_FULL         (-9)
/*! Insufficient Resources */
#define EDK_ERR_RESOURCE     (-10)
/*! Configuration Error */
#define EDK_ERR_CONFIG       (-11)
/*! Init Error */
#define EDK_ERR_INIT         (-12)
/*! Disabled Device */
#define EDK_ERR_DISABLED     (-13)
/*! Bad Descriptor */
#define EDK_ERR_BADDESC      (-14)
/*! Bad Parameters */
#define EDK_ERR_PARAMS       (-15)
/*! Overflow Error */
#define EDK_ERR_OVERFLOW     (-16)
/*! Alignment Error */
#define EDK_ERR_UNALIGNED    (-17)
/*! Not Found */
#define EDK_ERR_NOT_FOUND    (-18)
/*! Try again */
#define EDK_ERR_RETRY        (-19)
/*! Test failure */
#define EDK_TEST_FAILURE     (-20)

/*! Handle to various drivers in EDK */
typedef void* handle_t;

/*! Notification callback */
typedef void(*notify_callback_t) (int event, void *cookie, uint64_t params);

/*! Procmgr callback */
typedef void (*rproc_callback_t) (uint32_t eventid, void *cookie);

/*! RPC callback */
typedef void (*rpc_callback_t)(void *outp, uint32_t retval, void *cookie);

/*! A structure describing one node of the buffer-IO ring */
typedef struct {
    /*! Address of the buffer */
    uint8_t *addr;

    /*! Size of the buffer */
    int32_t size;

    /*! Unique parameter associated with the buffer */
    uint32_t param;
} bufferio_vector_t;

/*!
 * \brief Function to open the remote processor manager on a given unit.
 *
 * This function is used to open a remote processor manager instance, given
 * the unit number of the remote processor's core number. Returns a handle
 * to the remote processor instance.
 *
 * \param[in] unit Unit number
 * \param[in] core FW core number
 *
 * \retval handle to the remote processor instance.
 */
handle_t edk_procmgr_open(int unit, int core);

/*!
 * \brief Function to close the remote processor manager instance.
 *
 * This function is used to close the remote processor manager instance. The
 * argument passed is the handle to the remote processor manager instance. Does
 * not return anything.
 *
 * \param[in] rproc Handle to the remote processor manager.
 *
 * \retval none
 */
int edk_procmgr_close(handle_t rproc);

/*!
 * \brief Function to load FW to the remote processor.
 *
 * This function is used to load the FW to the remote processor. Arguments are
 * used to provide the location of the FW binary, resource file path if one
 * likes to override resource allocations, core file path for dumping core and
 * trace file path for trace log.
 *
 * \param[in] rproc Handle to the remote processor instance.
 * \param[in] exec_file FW exec file to be loaded.
 * \param[in] res_file Resource override file in YAML
 * \param[in] core_file Core dump file path.
 * \param[in] trace_file Trace log file path.
 * \param[in] bootargs FW boot arguments.
 * \param[in] appargs Application arguments.
 *
 * \retval EDK_ERR_xxx error values.
 */
int edk_procmgr_load(handle_t rproc, char *exec_file, char *res_file,
                     char *core_file, char *trace_file, char *bootargs,
                     char *appargs);
/*!
 * \brief Function to unload the remote processor.
 *
 * This function is used to unload the FW from the remote processor. Argument
 * is the remote processor instance.
 *
 * \param[in] rproc Handle to the remote processor manager.
 *
 * \retval EDK_ERR_xxx error values.
 */
int edk_procmgr_unload(handle_t rproc);

/*!
 * \brief Function to read the remote processor addressable memory space.
 *
 * This function is used to read the remote processor address space at a given
 * address for a given size and dump the read values into an user provided
 * buffer.
 *
 * \param[in] rproc Handle to the remote processor manager.
 * \param[in] src memory to be read on remote processors address space.
 * \param[out] dst Buffer used to dump the read remote processors memory.
 * \param[in] size Memory size in bytes to be read.
 *
 * \retval Number of bytes read.
 */
int edk_procmgr_read(handle_t rproc, uint32_t src, uint32_t *dst, int size);

/*!
 * \brief Function to write the remote processors addressable memory space.
 *
 * This function is used to write the remote processor address space with given
 * buffer at a given address and for a given size.
 *
 * \param[in] rproc Handle to the remote processors instance.
 * \param[in] src Buffer to be written to remote processor memory space.
 * \param[in] dst Remote processor addressable memory space.
 * \param[in] size Memory size in bytes to be written.
 *
 * \retval Number of bytes written.
 */
int edk_procmgr_write(handle_t rproc, uint32_t *src, uint32_t dst, int size);

/*!
 * \brief Function to register a callback on system event for a given remote
 *        processor.
 *
 * This function registers the callback for system events such as hang and
 * crash that can happen on a given remote processor.
 *
 * \param[in] rproc Handle to the remote processor manager instance.
 * \param[in] eventid  SYSTEM_EVENT_xxx System event id.
 * \param[in] callback Notification callback function for system event.
 * \param[in] cookie User defined parameter to be used with the callback function.
 *
 * \retval EDK_ERR_xxx error values.
 */
int edk_procmgr_register_event(handle_t rproc, uint32_t eventid,
                                rproc_callback_t callback, void *cookie);
/*!
 * \brief Function to unregister a system event.
 *
 * This function un-registers the callback for already registered system events
 * that can happen on a given remote processor.
 *
 * \param[in] rproc Handle to the remote processor manager.
 * \param[in] eventid  SYSTEM_EVENT_xxx System event id.
 *
 * \retval EDK_ERR_xxx error values.
 */
int edk_procmgr_unregister_event(handle_t rproc, uint32_t eventid);

/*!
 * \brief Function to register the event for notification callback on a given
 *        remote processor.
 *
 * This function registers a callback with the event notifier for a given event,
 * to be called on an event from Firmware running on the remote processor.
 * Events are application defined. Valid event id's range from 1-255 per core.
 *
 * \param[in] rproc Handle to the remote processor instance.
 * \param[in] event Event to be notified.
 * \param[in] callback Notification Timer callback function.
 * \param[in] cookie User defined parameter to be used with the callback
 *            function.
 *
 * \retval EDK_ERR_xxx error values.
 */
int edk_notify_register(handle_t rproc, int event, notify_callback_t callback,
                        void *cookie);
/*!
 * \brief Function to un-register an event.
 *
 * This function unregisters any event previously registered for notifications
 * on a given remote processor.
 *
 * \param[in] rproc Handle to the remote processor instance.
 * \param[in] event Event ID.
 *
 * \retval None.
 */
void edk_notify_unregister(handle_t rproc, int event);

/*!
 * \brief Function to send an event to Firmware.
 *
 * This function initiates the event transfer to Firmware running on the given
 * remote processor with the given parameters.
 *
 * \param[in] rproc Handle to the remote processor instance.
 * \param[in] event Event to be notified.
 * \param[in] params Parameters to be passed on a given event.
 *
 * \retval EDK_ERR_xxx error values.
 */
int edk_notify_send_event(handle_t rproc, int event, uint64_t params);

/*!
 * \brief Function for synchronous invocation of RPC on given remote processor.
 *
 * This function invokes the remote procedure on a remote processor. Additional
 * arguments are provided to pass arguments to the remote procedure and get
 * output from the remote procedure. Timeout also can be specified for the
 * expected completion of remote procedural call.
 *
 * \param[in] rproc Remote processor handle.
 * \param[in] fx_name Remote procedure name.
 * \param[in] inp Input parameters to the remote procedure.
 * \param[in] insize Size of the input parameters in bytes.
 * \param[out] outp Buffer pointer to capture return parameters of remote
 *                  procedure call.
 * \param[in] outsize Size of output parameters from RPC call in bytes.
 * \param[out] retval Buffer to capture return value of remote procedure call.
 * \param[in] timeoutms Timeout in ms.
 *
 * \retval EDK_ERR_xxx error values.
 */
int edk_rpc_call_sync(handle_t rproc, char *fx_name, void *inp, int insize,
                      void *outp, int outsize, int32_t *retval, int timeoutms);
/*!
 * \brief Function for asynchronous invocation of RPC on given remote processor
 *        with return over a callback.
 *
 * This function invokes the remote procedure on a remote processor. Additional
 * arguments are provided to pass arguments to the remote procedure and get
 * output from the remote procedure. Timeout also can be specified for the
 * expected completion of remote procedural call. The return value of the
 * remote procedure is asynchronously provided using callback.
 *
 * \param[in] rproc Remote processor handle.
 * \param[in] fx_name Remote procedure string.
 * \param[in] inp Pointer to the input parameters to be passed to remote
 *                procedure.
 * \param[in] insize Number of input parameters to the remote procedure.
 * \param[out] outp Buffer pointer to capture return parameters of remote
 *                  procedure call.
 * \param[in] outsize Number of output parameters from RPC call.
 * \param[in] callback Callback function to retrieve the return of RPC.
 * \param[in] cookie User defined parameter to be used with the callback
 *                   function.
 * \param[in] timeoutms Timeout in ms.
 *
 * \returns  EDK_ERR_xxx error values.
 */
int edk_rpc_call_async(handle_t rproc, char *fx_name, void *inp, int insize,
                       void *outp, int outsize, rpc_callback_t callback,
                       void *cookie, int timeoutms);
/*!
 * \brief Function to create a new Buffer-IO ring buffer.
 *
 * The function creates a new send or receive Ring Buffer towards the direction
 * and returns a handle to it.
 *
 * \param[in] rproc Handle to the remote processor instance.
 * \param[in] direction The direction of transfer  BUFFERIO_DIR_xxx
 *
 * \retval handle to the bufferio ring.
 *
 * \note When the direction is BUFFERIO_DIR_OUT, ensure that the firmware has
 * called uc_bufferio_alloc() before calling this API.
 */
handle_t edk_bufferio_open(handle_t rproc, int direction);

/*!
 * \brief Function to allocate buffers for the ring.
 *
 * The function allocates count buffers of size, if addr is NULL and add to the
 * ring of count elements.
 *
 * \param[in] bufferio Handle to the bufferio ring.
 * \param[in] addr Address of the first buffer. If the addr field set to NULL,
 * the buffer is allocated by the function, else, it is used as-is with address
 * incremented for upcoming buffers by the size of the slice.
 * \param[in] size Size of the buffer. Size will be rounded off to ensure word
 * alignment.
 * \param[in] count Number of buffers to be allocated.
 *
 * \retval EDK_ERR_xxx error values.
 */
int edk_bufferio_alloc(handle_t bufferio, uint32_t *addr, int size, int count);

/*!
 * \brief Function to retrieve a used/free buffer from the ring.
 *
 * The function retrieves a free buffer in case of a send ring and a full buffer
 * in case of a receive ring.
 *
 * \param[in] bufferio Handle to the bufferio ring.
 * \param[in] vector Pointer to a  bufferio_vector_t pointer containing the
 * information of the buffer.
 * \param[in] timeoutms Timeout in milliseconds.
 *
 * \retval EDK_ERR_xxx error values.
 */
int edk_bufferio_get(handle_t bufferio, bufferio_vector_t **vector,
                     int timeoutms);
/*!
 * \brief Function to queue a free/used buffer to the ring.
 *
 * The function queues a buffer in to the ring to be sent out in case of a send
 * ring and an empty buffer to receive data in case of a receive ring.
 *
 * \param[in] bufferio Handle to the bufferio ring.
 * \param[in] vector Pointer to a  bufferio_vector_t containing the information
 * of the buffer.
 *
 * \retval EDK_ERR_xxx error values.
 */
int edk_bufferio_put(handle_t bufferio, bufferio_vector_t *vector);

/*!
 * \brief Function to free the all buffers in the ring.
 *
 * The function de-allocates the memory buffers if allocated by buffer-IO.
 *
 * \param[in] bufferio Handle to the bufferio ring.
 *
 * \retval None.
 */
void edk_bufferio_free(handle_t bufferio);

/*!
 * \brief Function to close the buffer-IO ring.
 *
 * The function closes the buffer-IO ring.
 *
 * \param[in] bufferio Handle to the bufferio ring.
 *
 * \retval None.
 */
void edk_bufferio_close(handle_t bufferio);

#endif /* EDK_INT_H */
