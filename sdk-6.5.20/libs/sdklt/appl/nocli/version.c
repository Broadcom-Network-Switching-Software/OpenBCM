/*! \file version.c
 *
 * Application version information.
 * This file should always be rebuilt.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <string.h>
#include <stdio.h>

#include <shr/shr_pb.h>

#include "version.h"

static char version_str[256];

void
version_init(void)
{
    shr_pb_t *pb;

    pb = shr_pb_create();
    shr_pb_printf(pb, "SDKLT NoCLI Application\n");
#ifdef VERSION_INFO
    shr_pb_printf(pb, "Release %s", VERSION_INFO);
#ifdef DATE_INFO
    shr_pb_printf(pb, " built on %s", DATE_INFO);
#endif
#ifdef SCM_INFO
    shr_pb_printf(pb, " (%s)", SCM_INFO);
#endif
    shr_pb_printf(pb, "\n");
#endif
    strncpy(version_str, shr_pb_str(pb), sizeof(version_str) - 1);
    shr_pb_destroy(pb);
}

void
version_signon(void)
{
    printf("%s", version_str);
}
