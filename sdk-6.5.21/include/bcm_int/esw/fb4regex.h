/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       fb4regex.c
 * Purpose:    Internal regex function prototypes.
 */

#ifndef _BCM_INT_FIREBOLT4_REGEX_H_
#define _BCM_INT_FIREBOLT4_REGEX_H_
#if defined(INCLUDE_REGEX)
#include <bcm/bregex.h>
#include <bcm_int/esw/bregex.h>

int _bcm_tr3_regex_init(int unit, _bcm_esw_regex_functions_t **functions);
int _bcm_esw_tr3_regex_sync(int unit);
int _bcm_tr3_regex_config_set(int unit, bcm_regex_config_t *config);
int _bcm_tr3_regex_config_get(int unit, bcm_regex_config_t *config);

int _bcm_tr3_regex_exclude_add(int unit, uint8 protocol, 
                            uint16 l4_start, uint16 l4_end);

int _bcm_tr3_regex_exclude_get(int unit, int array_size, uint8 *protocol, 
                        uint16 *l4low, uint16 *l4high, int *array_count);

int _bcm_tr3_regex_exclude_delete(int unit, uint8 protocol, 
                            uint16 l4_start, uint16 l4_end);

int _bcm_tr3_regex_exclude_delete_all(int unit);
int _bcm_tr3_regex_engine_create(int unit, bcm_regex_engine_config_t *config, 
                            bcm_regex_engine_t *engid);

int _bcm_tr3_regex_engine_destroy(int unit, bcm_regex_engine_t engid);

int _bcm_tr3_regex_engine_traverse(int unit, bcm_regex_engine_traverse_cb cb, 
                                void *user_data);
int _bcm_tr3_regex_engine_get(int unit, bcm_regex_engine_t engid, 
                            bcm_regex_engine_config_t *config);

int _bcm_tr3_regex_match_check(int unit,  bcm_regex_match_t* match,
                                int count, int* metric);

int _bcm_tr3_regex_match_set(int unit, bcm_regex_engine_t engid,
                              bcm_regex_match_t* match, int count);

int _bcm_esw_regex_report_register(int unit, uint32 reports,
                       bcm_regex_report_cb callback, void *userdata);

int _bcm_esw_regex_report_unregister(int unit, uint32 reports,
                         bcm_regex_report_cb callback, void *userdata);

int _bcm_tr3_get_engine_size_info(int unit, int engine_id,
                                  bcm_regex_engine_info_t *engine_info);

int _bcm_tr3_get_match_id(int unit, int signature_id, int *match_id);

int _bcm_tr3_get_sig_id(int unit, int match_id, int *signature_id);

int _bcm_tr3_regex_info_get(int unit, bcm_regex_info_t *regex_info);

int _bcm_tr3_regex_stat_get(int unit, bcm_regex_stat_t type, uint64 *val);

int _bcm_tr3_regex_stat_set(int unit, bcm_regex_stat_t type, uint64 val);

int _bcm_tr3_regex_session_add(int unit, int flags,
                               bcm_regex_session_key_t *key,
                               bcm_regex_session_t *session);

int _bcm_tr3_regex_session_policy_update(int unit, int flags, int flow_index,
                                         bcm_regex_policy_t policy);

int _bcm_tr3_regex_session_delete_all(int unit);

int _bcm_tr3_regex_session_delete(int unit,
                                  bcm_regex_session_key_t *key);

int _bcm_tr3_regex_session_get(int unit, int flags,
                               bcm_regex_session_key_t *key,
                               bcm_regex_session_t *session);

int _bcm_tr3_regex_session_traverse(int unit,
                                    int flags,
                                    bcm_regex_session_traverse_cb cb,
                                    void *user_data);
int _bcm_tr3_regex_dump_axp(int unit);
#endif  /* INCLUDE_REGEX */

int _bcm_esw_regex_sync(int unit);

#endif /* _BCM_INT_FIREBOLT4_REGEX_H_ */

