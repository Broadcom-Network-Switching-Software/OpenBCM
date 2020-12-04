/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        rlink.h
 * Purpose:     RLINK protocol interfaces
 */

#ifndef   _BCM_SRC_RLINK_H
#define   _BCM_SRC_RLINK_H

#include <shared/idents.h>

#define	RLINK_CLIENT_ID		SHARED_CLIENT_ID_RLINK

typedef enum {
    RLINK_MSG_ADD,		/* scan add (client to server) */
    RLINK_MSG_DELETE,		/* scan delete (client to server) */
    RLINK_MSG_NOTIFY,		/* run handlers (server to client) */
    RLINK_MSG_TRAVERSE          /* traverse processing */
} rlink_msg_t;

typedef enum {
    RLINK_TYPE_LINK = 1,        /* linkscan */
    RLINK_TYPE_AUTH,            /* auth unauth callback */
    RLINK_TYPE_L2,              /* l2 notify */
    RLINK_TYPE_RX,              /* RX tunnelling */
    RLINK_TYPE_ADD,		/* Msg add processing */
    RLINK_TYPE_DELETE,		/* Msg delete processing */
    RLINK_TYPE_OAM,             /* oam event */
    RLINK_TYPE_BFD,             /* bfd event */
    RLINK_TYPE_FABRIC,          /* fabric event */
    RLINK_TYPE_START,           /* traverse start c>s */
    RLINK_TYPE_NEXT,            /* traverse next c>s */
    RLINK_TYPE_QUIT,            /* traverse quit c>s */
    RLINK_TYPE_ERROR,           /* traverse error s>c */
    RLINK_TYPE_MORE,            /* traverse more s>c */
    RLINK_TYPE_DONE             /* traverse done s>c */
} rlink_type_t;


extern uint8 *bcm_rlink_decode(uint8 *buf,
                               rlink_msg_t *msgp,
                               rlink_type_t *typep,
                               int *unitp);

extern uint8 *bcm_rlink_encode(uint8 *buf,
                               rlink_msg_t msg,
                               rlink_type_t type,
                               int unit);


#endif /* _BCM_SRC_RLINK_H */
