/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef MACSEC_CMN_H
#define MACSEC_CMN_H

#if defined(INCLUDE_MACSEC)

#include <bcm/macsec.h>

extern int 
_bcm_common_macsec_init(int unit);

extern int 
bcm_common_macsec_port_traverse(int unit, 
                             bcm_macsec_port_traverse_cb callback, 
                             void *user_data);
extern int
bcm_common_macsec_port_config_set(int unit, bcm_port_t port, 
                               bcm_macsec_port_config_t *cfg);

extern int
bcm_common_macsec_port_config_get(int unit, bcm_port_t port, 
                               bcm_macsec_port_config_t *cfg);

extern int 
bcm_common_macsec_secure_chan_create(int unit, bcm_port_t port, 
                                  uint32 flags,
                                  bcm_macsec_secure_chan_t *chan, 
                                  int *chanId);

extern int 
bcm_common_macsec_secure_chan_get(int unit, bcm_port_t port, int chanId,
                               bcm_macsec_secure_chan_t *chan);

extern int 
bcm_common_macsec_secure_chan_destroy(int unit, bcm_port_t port, int chanId);

extern int 
bcm_common_macsec_secure_chan_traverse(int unit, bcm_port_t port, 
                                    bcm_macsec_chan_traverse_cb callback,
                                    void *user_data);
extern int
bcm_common_macsec_secure_assoc_create(int unit, bcm_port_t port, 
                                   uint32 flags, int chanId, 
                                   bcm_macsec_secure_assoc_t *assoc, 
                                   int *assocId);

extern int
bcm_common_macsec_secure_assoc_get(int unit, bcm_port_t port, int assocId, 
                        bcm_macsec_secure_assoc_t *assoc, int *chanId);

extern int 
bcm_common_macsec_secure_assoc_destroy(int unit, 
                                    bcm_port_t port, int assocId);

extern int 
bcm_common_macsec_secure_assoc_traverse(int unit, bcm_port_t port, 
                            int chanId, 
                            bcm_macsec_secure_assoc_traverse_cb callback,
                            void *user_data);

extern int
bcm_common_macsec_flow_create(int unit, bcm_port_t port, 
                           uint32 flags, bcm_macsec_flow_match_t *flow, 
                           bcm_macsec_flow_action_t *action, int *flowId);

extern int
bcm_common_macsec_flow_get(int unit, bcm_port_t port, int flowId, 
                bcm_macsec_flow_match_t *flow, 
                bcm_macsec_flow_action_t *action);

extern int 
bcm_common_macsec_flow_destroy(int unit, bcm_port_t port, int flowId);

extern int 
bcm_common_macsec_flow_traverse(int unit, bcm_port_t port, 
                             bcm_macsec_flow_traverse_cb callbk,
                             void *user_data);

extern int 
bcm_common_macsec_stat_clear(int unit, bcm_port_t port);

extern int 
bcm_common_macsec_stat_get(int unit, bcm_port_t port, 
                        bcm_macsec_stat_t stat, 
                        int chanId, int assocId, uint64 *val);

extern int 
bcm_common_macsec_stat_get32(int unit, bcm_port_t port, 
                          bcm_macsec_stat_t stat, int chanId, 
                          int assocId, uint32 *val);

extern int 
bcm_common_macsec_stat_set(int unit, bcm_port_t port, 
                        bcm_macsec_stat_t stat, 
                        int chanId, int assocId, uint64 val);

extern int 
bcm_common_macsec_stat_set32(int unit, bcm_port_t port, 
                          bcm_macsec_stat_t stat, 
                          int chanId, int assocId, uint32 val);

extern int 
bcm_common_macsec_event_register(int unit, bcm_macsec_event_cb cb, void *user_data);

extern int 
bcm_common_macsec_event_unregister(int unit, bcm_macsec_event_cb cb);

extern int
bcm_common_macsec_event_enable_set(int unit,
                                bcm_macsec_event_t event, int enable);

extern int
bcm_common_macsec_event_enable_get(int unit,
                                bcm_macsec_event_t event, int *enable);

/*
 * Debug/Print config set/get.
 */
extern int bcm_common_macsec_config_print(uint32 level);

#endif /* defined(INCLUDE_MACSEC) */

#endif /* MACSEC_CMN_H */

