/*! \file shr_pb_local.h
 *
 * Local print buffer APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_PB_LOCAL_H
#define SHR_PB_LOCAL_H

#include <bsl/bsl.h>

#include <shr/shr_pb.h>

/*!
 * Default printf-like function for displaying the contents of the
 * local print buffer. By default this function is \c cli_out.
 *
 * For more information, please refer to the implementation of \ref
 * SHR_PB_LOCAL_DONE.
 */
#ifndef SHR_PB_LOCAL_PRINT
#define SHR_PB_LOCAL_PRINT cli_out
#endif

/*!
 * \brief Declare and initialize local print buffer.
 *
 * The purpose of this macro is to allow a dump function to work even
 * if a NULL pointer is passed in as the destination print buffer.
 *
 * The most typical use case is when a dump function is called from
 * within gdb. In this case no pre-allocated print buffer is available
 * by default, so it will be more convenient to use a NULL pointer.
 *
 * If the local print buffer is active, the \ref SHR_PB_LOCAL_DONE
 * macro will display the contents of the local print buffer before it
 * is destroyed.
 *
 * Example:
 *
 * \code{.c}
 * void my_dump_func(int unit, shr_pb_t *pb)
 * {
 *     SHR_PB_LOCAL_DECL(pb);
 *
 *     <populate print buffer>
 *
 *     SHR_PB_LOCAL_DONE();
 * }
 * \endcode
 *
 * \param [in] _pb Default destination print buffer.
 */
#define SHR_PB_LOCAL_DECL(_pb) \
    shr_pb_t *_pb_local = _pb ? NULL : (_pb = shr_pb_create())

/*!
 * \brief Destroy local print buffer.
 *
 * Destroy a local print buffer which was initialized using \ref
 * SHR_PB_LOCAL_DECL.
 *
 * If the local print buffer is active, its contents will be displayed
 * via printf-like \c _print_func before it is destroyed.
 *
 * This function is primarily used internally by \ref
 * SHR_PB_LOCAL_DONE, but it can be used in application code if there
 * is a desire to make the print function name explicit, or if a
 * print function other than \c cli_out should be used.
 *
 * \param [in] _print_func Function to display print buffer.
 */
#define SHR_PB_LOCAL_DONE_VIA_FUNC(_print_func)         \
    do {                                                \
        if (_pb_local) {                                \
            _print_func("%s", shr_pb_str(_pb_local));   \
            shr_pb_destroy(_pb_local);                  \
        }                                               \
    } while (0)

/*!
 * \brief Destroy local print buffer.
 *
 * Destroy a local print buffer which was initialized using \ref
 * SHR_PB_LOCAL_DECL.
 *
 * If the local print buffer is active, its contents will be displayed
 * via \ref SHR_PB_LOCAL_PRINT before it is destroyed.
 *
 * Please refer to \ref SHR_PB_LOCAL_DECL for usage example.
 */
#define SHR_PB_LOCAL_DONE() \
    SHR_PB_LOCAL_DONE_VIA_FUNC(SHR_PB_LOCAL_PRINT)

#endif  /* !SHR_PB_LOCAL_H */
