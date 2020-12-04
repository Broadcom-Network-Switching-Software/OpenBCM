/*! \file bcma_io_term_mode.c
 *
 * Helper function for command line editing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <unistd.h>
#include <termios.h>

#include <bcma/io/bcma_io_term.h>

int
bcma_io_term_mode_set(int reset)
{
    static struct termios orig, new;
    static int orig_saved;

    if (!isatty(0)) {
        return -1;
    }

    if (reset) {
        /* Restore TTY settings */
        if (orig_saved) {
            tcsetattr(0, TCSADRAIN, &orig);
        }
        return 0;
    }

    if (!orig_saved) {
        /* Save terminal settings */
        if (tcgetattr(0, &orig) < 0) {
            return -1;
        }
        orig_saved = 1;
    }

    /* Disable echo and buffering */
    new = orig;
    new.c_lflag &= ~(ECHO | ICANON | ISIG);
    new.c_iflag &= ~(ISTRIP | INPCK);
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSADRAIN, &new) < 0) {
        return -1;
    }

    return 0;
}
