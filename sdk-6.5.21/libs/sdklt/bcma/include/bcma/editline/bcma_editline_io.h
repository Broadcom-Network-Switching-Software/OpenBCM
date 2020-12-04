/*! \file bcma_editline_io.h
 *
 * Editline types for terminal I/O.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_EDITLINE_IO_H
#define BCMA_EDITLINE_IO_H

/*!
 * \brief Key codes for special terminal keys.
 *
 * This structure can be used to override the default behavior of
 * special readline control keys.
 *
 * It is (optionally) passed in as part of the library initialization.
 */
typedef struct bcma_editline_term_keys_s {

    /*! Key code for EOF (return EOF to caller). */
    int eof;

    /*! Key code for Erase (typically Delete key). */
    int erase;

    /*! Key code for Interrupt (return to caller). */
    int intr;

    /*! Key code for Kill (clear input line). */
    int kill;

    /*! Key code for Quit (return to caller) */
    int quit;

} bcma_editline_term_keys_t;

/*!
 * \brief Callbacks functions for terminal control.
 *
 * Only the read, write and tty_set functions are mandatory, but it is
 * recommended to implement all of these callbacks.
 */
typedef struct bcma_editline_io_cb_s {

    /*! Read max characters from the terminal. */
    int (*read)(void *buf, int max);

    /*! Write max characters to the terminal. */
    int (*write)(const void *buf, int count);

    /*! Disable echo and buffering on terminal (or restore settings). */
    int (*tty_set)(int reset);

    /*! Get the size of the terminal window. */
    int (*winsize_get)(int *cols, int *rows);

    /*! Send an INTR signal to the system. */
    int (*send_sigint)(void);

    /*! Send a QUIT signal to the system. */
    int (*send_sigquit)(void);

} bcma_editline_io_cb_t;

#endif /* BCMA_EDITLINE_IO_H */
