/*! \file bcmbd_simhook.c
 *
 * Function hooks for simulated hardware access.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmbd/bcmbd_simhook.h>

/* Global variables for performance reasons */
bcmbd_hmi_read_f bcmbd_simhook_read[BCMDRD_CONFIG_MAX_UNITS];
bcmbd_hmi_write_f bcmbd_simhook_write[BCMDRD_CONFIG_MAX_UNITS];
bcmbd_schan_op_f bcmbd_simhook_schan_op[BCMDRD_CONFIG_MAX_UNITS];
