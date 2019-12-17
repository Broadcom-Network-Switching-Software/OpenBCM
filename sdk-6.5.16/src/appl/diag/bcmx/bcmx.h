/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bcmx.h
 * Purpose:     
 */

#ifndef   _DIAG_BCMX_H_
#define   _DIAG_BCMX_H_

#include <bcmx/lport.h>
#include <bcmx/l2.h>

#include <bcmx/lplist.h>
#include <bcmx/bcmx.h>
#include <appl/diag/diag.h>

#define BCMX_UNIT_ITER(unit, i)                                           \
    for (i = 0, unit = bcmx_unit_list[0];                                 \
         i < bcmx_unit_count;                                             \
         unit = bcmx_unit_list[++i])

extern int bcmx_cmd_cnt;
extern cmd_t bcmx_cmd_list[];
extern int bcmx_unit_list[];

/*
 * Uport
 */
#define BCMX_UPORT_STR_LENGTH_MIN      12
#define BCMX_UPORT_STR_LENGTH_DEFAULT  BCMX_UPORT_STR_LENGTH_MIN

extern char * bcmx_lport_to_uport_str(bcmx_lport_t lport);

typedef void (*bcmx_uport_to_str_f)(bcmx_uport_t uport, char *buf, int len);
extern bcmx_uport_to_str_f bcmx_uport_to_str;

typedef bcmx_uport_t (*bcmx_uport_parse_f)(char *buf, char **end);
extern bcmx_uport_parse_f bcmx_uport_parse;


/* In bcmxdiag/util.c */
extern int bcmx_lplist_parse(bcmx_lplist_t *list, char *buf);

#if defined(INCLUDE_ACL)
extern cmd_result_t bcmx_cmd_acl(int unit, args_t *args);
extern char         bcmx_cmd_acl_usage[];
#endif /* INCLUDE_ACL */

#ifdef BCM_FIELD_SUPPORT
extern cmd_result_t bcmx_cmd_field(int unit, args_t *args);
extern char         bcmx_cmd_field_usage[];
#endif

extern cmd_result_t bcmx_cmd_ipmc(int unit, args_t *args);
extern char         bcmx_cmd_ipmc_usage[];

extern cmd_result_t bcmx_cmd_l2(int unit, args_t *args);
extern char         bcmx_cmd_l2_usage[];

extern void         bcmx_dump_l2_addr(char *pfx, bcmx_l2_addr_t *l2addr,
                                      bcmx_lplist_t *port_block);

extern void         bcmx_dump_l2_cache_addr(char *pfx, bcmx_l2_cache_addr_t *l2addr);

extern cmd_result_t bcmx_cmd_l3(int unit, args_t *args);
extern char         bcmx_cmd_l3_usage[];

extern cmd_result_t bcmx_cmd_mcast(int unit, args_t *args);
extern char         bcmx_cmd_mcast_usage[];

extern cmd_result_t bcmx_cmd_mirror(int unit, args_t *args);
extern char         bcmx_cmd_mirror_usage[];

extern cmd_result_t bcmx_cmd_port(int unit, args_t *args);
extern char         bcmx_cmd_port_usage[];

extern cmd_result_t bcmx_cmd_stat(int unit, args_t *args);
extern char         bcmx_cmd_stat_usage[];

extern cmd_result_t bcmx_cmd_trunk(int unit, args_t *args);
extern char         bcmx_cmd_trunk_usage[];

extern cmd_result_t bcmx_cmd_vlan(int unit, args_t *args);
extern char         bcmx_cmd_vlan_usage[];

extern cmd_result_t bcmx_cmd_stg(int unit, args_t *args);
extern char         bcmx_cmd_stg_usage[];

extern cmd_result_t bcmx_cmd_rate(int unit, args_t *args);
extern char         bcmx_cmd_rate_usage[];

extern cmd_result_t bcmx_cmd_link(int unit, args_t *args);
extern char         bcmx_cmd_link_usage[];

extern char         bcmx_cmd_tx_usage[];
extern cmd_result_t bcmx_cmd_tx(int unit, args_t *args);

extern char         bcmx_cmd_rx_usage[];
extern cmd_result_t bcmx_cmd_rx(int unit, args_t *args);

#if 0

typedef int user_port_t;

/* For now, we support this many boards in sample configurations */
#define BCMX_BOARDS_MAX 4

typedef struct bcmx_config_desc_s {
    int num_entries;
    int stk_mode;                /* How should boards be added? */
    bcmx_cfg_params_t *params;
    bcmx_system_trx_t *trx_funs;    /* Translation functions (see above) */
    bcmx_dest_route_get_f dest_route_get;
    char *name;
    bcmx_cpu_info_t cpu_info[BCMX_BOARDS_MAX];
    int exit_units[BCMX_BOARDS_MAX];   /* How to get off board */
    int exit_ports[BCMX_BOARDS_MAX];
} bcmx_config_desc_t;

extern bcmx_config_desc_t bcmx_configs[];
extern int CFG_COUNT;

extern bcmx_lport_t general_uport_str_to_lport(char *uport_str, char **end);
extern bcmx_lport_t general_lport_next(bcmx_lport_t lport);

/* Commands */

extern cmd_result_t bcmx_cmd_mcpu_setup(int unit, args_t *args);
extern char bcmx_cmd_mcpu_setup_usage[];
extern cmd_result_t bcmx_cmd_mcpu_start(int unit, args_t *args);
extern char bcmx_cmd_mcpu_start_usage[];

/* RDP related functions */
extern int bcmx_rdp_client_tx(int cpuid, char *buf, int buf_len);
extern void bcmx_rdp_client_rx(uint32 cid, void *data, int length, int s_cpu);
extern int bcmx_rdp_server_tx(int cpu, char *buf, int buf_len);
extern void bcmx_rdp_server_rx(uint32 clientid, void *data, int length,
                                int s_cpu);
extern int bcmx_rdp_send(int cpuid, char *buf, int buf_len, int data_len);

/* BCMX in a stack */
extern void stack_simplex_bcmx_init(void);
extern void stack_duplex_bcmx_init(void);
#endif

#endif /* _DIAG_BCMX_H_ */
