/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        traverse.h
 * Purpose:     RLINK traverse client/server/control support functions
 */

#ifndef   _TRAVERSE_H_
#define   _TRAVERSE_H_

#include <bcm/types.h>
#include <bcm/pkt.h>
#include <appl/cpudb/cpudb.h>
#include "rlink.h"

typedef struct bcm_rlink_traverse_data_s {
    int                             unit;       /* traverse unit */
    uint32                          c_id;       /* client ID */
    uint32                          s_id;       /* server ID */
    bcm_pkt_t                       *rx_pkt;
    uint8                           *rx_buf;    /* receive buffer */
    uint8                           *rx_ptr;    /* rx buffer current */
    int                             rx_len;     /* rx buf length */
    uint8                           *tx_buf;    /* tx buffer */
    uint8                           *tx_ptr;    /* tx buffer current */
    int                             tx_len;     /* tx buffer length */
    void                            *parent;    /* parent client/server */
} bcm_rlink_traverse_data_t;

/*
  Called by a client traverse implementation to start a remote traverse.
  Initializes and registers a traverse request record.
  Does *not* send any RLINK messages to the RLINK server.
 */
extern int bcm_rlink_traverse_request_start(int unit,
                                            bcm_rlink_traverse_data_t *req,
                                            uint32 *key);

/*
  Gets a traverse reply. If there are no rxeplies, and the remote
  traverse has not started, or signaled completion, then issue a request
  for traverse data and wait for a response.
 */
extern int bcm_rlink_traverse_reply_get(int unit,
                                        bcm_rlink_traverse_data_t *req);

/*
  Indicate that this request is done. If the traverse server has not
  indicated completion, then issue a done message.
 */
extern int bcm_rlink_traverse_request_done(int unit,
                                           int rv,
                                           bcm_rlink_traverse_data_t *data);


/* Server side implementation APIs */

/*
  Check to see if there's enough room for a reply. If not,
  send out current data and wait for a resonse.
 */
extern int bcm_rlink_traverse_reply_check(bcm_rlink_traverse_data_t *data,
                                          int size);

/*
  Flush any outstanding callback data.
 */
extern int bcm_rlink_traverse_reply_done(bcm_rlink_traverse_data_t *data,
                                         int rv);

/*
  Handle a traverse request
 */
extern int bcm_rlink_traverse_request(rlink_type_t type,
                                      cpudb_key_t cpu,
                                      bcm_pkt_t *pkt, uint8 *data, int len);

extern int bcm_rlink_traverse_device_clear(int unit);
#endif /* _TRAVERSE_H_ */
