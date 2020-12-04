/*! \file bcmlu_unet.h
 *
 * Wrapper of bcmlu_socket for naming compatibility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLU_UNET_H
#define BCMLU_UNET_H

#include "./bcmlu_socket.h"

/*! Naming compatibility of function bcmlu_socket_attach. */
#define bcmlu_unet_attach \
            bcmlu_socket_attach

/*! Naming compatibility of function bcmlu_socket_detach. */
#define bcmlu_unet_detach \
            bcmlu_socket_detach

#endif /* BCMLU_UNET_H */
