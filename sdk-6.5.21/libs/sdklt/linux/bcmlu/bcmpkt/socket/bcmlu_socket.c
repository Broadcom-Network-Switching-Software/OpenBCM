/*! \file bcmlu_socket.c
 *
 * Linux user mode network driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmlu/bcmpkt/bcmlu_socket.h>
#include "bcmlu_tpacket.h"
#include "bcmlu_rawsock.h"

#define BSL_LOG_MODULE BSL_LS_SYS_SOCKET

int
bcmlu_socket_attach(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmlu_tpacket_attach());

    SHR_IF_ERR_EXIT
        (bcmlu_rawsock_attach());

exit:
    SHR_FUNC_EXIT();
}

int
bcmlu_socket_detach(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmlu_tpacket_detach());

    SHR_IF_ERR_EXIT
        (bcmlu_rawsock_detach());

exit:
    SHR_FUNC_EXIT();
}
