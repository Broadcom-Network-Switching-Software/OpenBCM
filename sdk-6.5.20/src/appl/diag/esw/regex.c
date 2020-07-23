/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        txrx.c
 * Purpose:
 * Requires:    
 */

#ifdef INCLUDE_REGEX
#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/thread.h>
#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <shared/bsl.h>
#include <soc/types.h>
#include <soc/debug.h>

#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/in.h>
#define atoi _shr_ctoi 
#else
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#endif

#include <bcm/port.h>
#include <bcm/error.h>

#include <appl/diag/system.h>
#include <bcm/bregex.h>

#ifndef NO_FILEIO 
char cmd_regex_usage[] = 
"Parameters\n"
"\tcreate <file> -- load the patterns from file\n"
"\tdestroy [engine] -- destroy all engines or [specific engine]\n"
"\tmonitor [start|stop|verbose|silent] -- send reports for flows/matches\n"
"\tlist -- list configured regex engines\n"
"\tshow [engine [engineid] | dfa [engineid] | info [engineid] | policer [id] | policy [id] | session [stats] | match stats | axp]\n"
;

typedef struct xd_s {  
    pbmp_t      pbmp; 
    char        *xd_file;
} xd_t;

#define MAX_PATTERN_PER_ENGINE 128

typedef struct diag_regex_engine_info_s {
    char                filename[256];
    int                 num_match;
    char                app_name[MAX_PATTERN_PER_ENGINE][64];
    bcm_regex_match_t   match[MAX_PATTERN_PER_ENGINE];
    bcm_regex_engine_t  engid;
    struct diag_regex_engine_info_s *next;
} diag_regex_engine_info_t;

static diag_regex_engine_info_t *engine_info_list[BCM_MAX_NUM_UNITS];
#define MAX_ENGINE_PER_UNIT     64
static bcm_regex_engine_t re_engines[BCM_MAX_NUM_UNITS][MAX_ENGINE_PER_UNIT];
static int engine_count[BCM_MAX_NUM_UNITS];
static int unit_inited[BCM_MAX_NUM_UNITS];
static int re_inited = 0;

/*
Memory to store engines not created by the BCM command interface
*/
static bcm_regex_engine_t   l_re_engines[BCM_MAX_NUM_UNITS][MAX_ENGINE_PER_UNIT];
static int                  l_engine_count[BCM_MAX_NUM_UNITS];

static int regex_cmd_init(void)
{
    if (re_inited) {
        return 0;
    }
    
    sal_memset(engine_info_list, 0, sizeof(diag_regex_engine_info_t*)*BCM_MAX_NUM_UNITS);
    sal_memset(engine_count, 0, sizeof(int)*BCM_MAX_NUM_UNITS);
    re_inited = 1;
    return 0;
}

static int initialize_regex(int unit, int init)
{
    bcm_regex_config_t  recfg;
    int i, rv, port;
    soc_pbmp_t  pbmp_temp;
    
    /* Set default configuration */
    recfg.flags = BCM_REGEX_CONFIG_ENABLE | BCM_REGEX_CONFIG_IP4 |
                  BCM_REGEX_CONFIG_IP6 | BCM_REGEX_CONFIG_TCP_SESSION_DETECT;
    recfg.max_flows = -1; /* max flows */
    recfg.payload_depth = -1;
    recfg.inspect_num_pkt = -1;
    recfg.inactivity_timeout_usec = 100;
    recfg.report_flags = BCM_REGEX_REPORT_NEW | BCM_REGEX_REPORT_MATCHED | 
                         BCM_REGEX_REPORT_END;
    recfg.dst_mac[0] = 0x00;
    for (i=1;i<6;i++) {
        recfg.dst_mac[i] = 0xa0 + i;
    }
    recfg.src_mac[0] = 0x00;
    for (i=1;i<6;i++) {
        recfg.src_mac[i] = 0xb0 + i;
    }
    recfg.ethertype = 0x0531;
    rv = bcm_regex_config_set(unit, &recfg);
    if (rv) {
        return CMD_FAIL;
    }

    /* Enable regex on the ports */
    BCM_PBMP_ASSIGN(pbmp_temp, PBMP_GE_ALL(unit));
    BCM_PBMP_ITER(pbmp_temp, port) {
        bcm_port_control_set(unit, port, bcmPortControlRegex, init ? 1 : 0);
    }

    unit_inited[unit] = !!init;
    return 0;
}


static cmd_result_t
regex_parse_args(int u, args_t *a, xd_t *xd)
{
    parse_table_t       pt;
    char                *xfile = NULL;

    parse_table_init(u, &pt);

    parse_table_add(&pt, "Filename", PQ_DFL|PQ_STRING,0, &xfile,NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        sal_printf("Error: Unknown option: %s\n", ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return(CMD_FAIL);
    }

    if (xfile) {
        xd->xd_file = sal_strdup(xfile);
    } else {
        xd->xd_file = NULL;
    }

    parse_arg_eq_done(&pt);
    return(0);
}

static int _read_line(FILE *fp, char *b, int *len)
{
    int c;
    int l = 0;  

    c = getc(fp);
    while (!((c == EOF) || (c == '\n') || (c == '\r'))) {
        b[l++] = c;
        c = getc(fp);
    }

    *len = l;
    b[l] = '\0';
    if (l) {
        return 0;
    }

    return (c == EOF) ? -1 : 0;
}

static char* _strip_front(char *l, int len)
{
    int i=0;

    /* skip leading spaces. */
    while (i < len) {
        if ((l[i] == ' ') || (l[i] == '\t')) {
            i++;
            continue;
        }
        return &l[i];
    }
    return NULL;
}

static int decode_app_name(char* s, diag_regex_engine_info_t *app)
{
    sal_strcpy(&app->app_name[app->num_match][0], s);
    return 0;
}

static int decode_match_id(char* s, diag_regex_engine_info_t *app)
{
    app->match[app->num_match].match_id = atoi(s);
    return 0;
}

static int decode_flow_timeout(char* s, diag_regex_engine_info_t *app)
{
    app->match[app->num_match].inactivity_timeout_usec = atoi(s);
    return 0;
}

static int decode_pattern(char* s, diag_regex_engine_info_t *app)
{
    int j = 0, esc = 0;

    if (s[0] != '/') {
        return -1;
    }

    s++;
    sal_memset(app->match[app->num_match].pattern, 0, BCM_REGEX_MAX_PATTERN_SIZE);
    while (*s) {
        if (!esc && (*s == '/')) {
            return 0;
        }
        app->match[app->num_match].pattern[j] = *s;
        if (*s == '\\') {
            esc = !!esc;
        }
        j++;
        s++;
    }

    if (app->match[app->num_match].pattern[j-1] == '/') {
        app->match[app->num_match].pattern[j-1] = '\0';
        return 0;
    }

    return 1;
}

typedef struct regex_cmd_str_to_flag_s {
    char *name;
    unsigned int flag;
} regex_cmd_str_to_flag_t;

static regex_cmd_str_to_flag_t *
regex_flag_get(char *str, regex_cmd_str_to_flag_t *tbl)
{
    while (tbl->name) {
        if (!sal_strcasecmp(str, tbl->name)) {
            return tbl;
        }
        tbl++;
    }
    return NULL;
}

static int decode_flags_cmn(char* s, diag_regex_engine_info_t *app, 
                            regex_cmd_str_to_flag_t *tbl, 
                            unsigned int *pflag)
{
    char *ts;
    int done = 0;
    regex_cmd_str_to_flag_t *ps2f;

    *pflag = 0;

    ts = s;
    while (!done) {
        /* skip leading spaces */
        while (*s && ((*s == ' ') || (*s == '\t'))) {
            s++;
        }
        if (!*s) {
            return 0;
        }
        ts = s;
        while (*s && (!((*s == ' ') || (*s == ',')))) {
            s++;
        }
        if (!*s) {
            done = 1;
        } else {
            *s = '\0';
            s++;
        }
        ps2f = regex_flag_get(ts, tbl);
        if (!ps2f) {
            cli_out("unknown regex action: %s\n", ts);
            return -1;
        }
        *pflag |= ps2f->flag;
    }
    return 0;
}

static int decode_enums(char* s, diag_regex_engine_info_t *app, 
                        regex_cmd_str_to_flag_t *tbl, 
                        unsigned int *value)
{
    char *ts;
    int done = 0;
    regex_cmd_str_to_flag_t *ps2f;

    ts = s;
    while (!done) {
        /* skip leading spaces */
        while (*s && ((*s == ' ') || (*s == '\t'))) {
            s++;
        }
        if (!*s) {
            return 0;
        }
        ts = s;
        while (*s && (!((*s == ' ') || (*s == ',')))) {
            s++;
        }
        if (!*s) {
            done = 1;
        } else {
            *s = '\0';
            s++;
        }
        ps2f = regex_flag_get(ts, tbl);
        if (!ps2f) {
            cli_out("unknown regex action: %s\n", ts);
            return -1;
        }
        *value = ps2f->flag;
    }
    return 0;
}
static regex_cmd_str_to_flag_t flow_options[] = {
    { "to_server", BCM_REGEX_MATCH_TO_SERVER },
    { "to_client", BCM_REGEX_MATCH_TO_CLIENT },
    { "case", BCM_REGEX_MATCH_CASE_INSENSITIVE },
    { "http", BCM_REGEX_MATCH_MULTI_FLOW },
    { "none", 0 },
    { NULL, 0 }
};

static int decode_flowopt(char* s, diag_regex_engine_info_t *app)
{
    int rv;
    
    rv = decode_flags_cmn( s, app, flow_options, 
                            &app->match[app->num_match].flags);
    if (rv) {
        return CMD_FAIL;
    }
    return 0;
}

static regex_cmd_str_to_flag_t flow_sequence[] = {
    { "first", 0},
    { "any", -1 },
    { NULL, 0 }
};

static int decode_flow_sequence(char* s, diag_regex_engine_info_t *app)
{
    unsigned int val = 0;
    
    decode_enums(s, app, flow_sequence, &val);
    app->match[app->num_match].sequence = val;
    return 0;
}

static regex_cmd_str_to_flag_t match_actions[] = {
    { "ignore", BCM_REGEX_MATCH_ACTION_IGNORE },
    { "to_cpu", BCM_REGEX_MATCH_ACTION_COPY_TO_CPU },
    { "drop", BCM_REGEX_MATCH_ACTION_DROP },
    { "none", 0 },
    { NULL, 0 }
};

static int decode_action(char* s, diag_regex_engine_info_t *app)
{
    int rv;
    
    rv = decode_flags_cmn( s, app, match_actions, 
                            &app->match[app->num_match].action_flags);
    if (rv) {
        return CMD_FAIL;
    }
    return 0;
}

static int decode_previous_tag(char* s, diag_regex_engine_info_t *app)
{
    app->match[app->num_match].requires = atoi(s);
    return 0;
}

static int decode_next_tag(char* s, diag_regex_engine_info_t *app)
{
    app->match[app->num_match].provides = atoi(s);
    return 0;
}

typedef int (*decode_tag_value)(char *buf, diag_regex_engine_info_t *app);

typedef struct regex_xml_data_s {
    char            *tag_name;
    decode_tag_value decode_value;
} regex_xml_data_t;

static regex_xml_data_t regex_match_info[] = {
    { "engine", NULL},
    { "signature", NULL},
    { "application", decode_app_name },
    { "matchid", decode_match_id },
    { "pattern", decode_pattern },
    { "flowopt", decode_flowopt },
    { "sequence", decode_flow_sequence },
    { "timeout", decode_flow_timeout},
    { "previous_match_tag", decode_previous_tag },
    { "next_match_tag", decode_next_tag },
    { "action", decode_action},
    { NULL, NULL },
};

static regex_xml_data_t *regex_cmd_get_xml_tag(char *name)
{
    regex_xml_data_t *ptag = regex_match_info;
    char *s = name;

    while(*s) {
        if (*s == '>') {
            *s = '\0';
        }
        s++;
    }

    while (ptag->tag_name) {
        if (!sal_strcasecmp(name, ptag->tag_name)) {
            return ptag;
        }
        ptag++;
    }
    return NULL;
}

/* each input line should be in the format:
 *  /pattern/id/action/
 */
static int _parse_a_xml_line(char *l, int len, 
                             diag_regex_engine_info_t *einfo,
                             regex_xml_data_t **pptag, int *tag_start)
{
    char *b;
    regex_xml_data_t *ptag;

    b = _strip_front(l, len);
    if (!b) { 
        return 0;
    }

    /* check if the line is commented */
    if (b[0] == '#') {
        return 0; /* line is commenetd */
    }

    if (b[0] == '<') {
        b++;
        *tag_start = 1;
        if (*b == '/') {
            *tag_start = 0;
            b++;
        }
        ptag = regex_cmd_get_xml_tag(b);
        if (!ptag) {
            cli_out("Unknown XML tag: %s\n", b);
            return -1;
        }
        *pptag = ptag;
        return 0;
    }

    ptag = *pptag;
    if (!ptag) {
        cli_out("Invalid tag");
        return -1;
    }

    /* decode the line accouding to provided tag */
    if (ptag->decode_value && (ptag->decode_value(b, einfo))) {
        cli_out("Failed to decode the line: %s\n", b);
        return -1;
    }

    return 0;
}

static cmd_result_t
cmd_create_regex_engine(int unit, args_t *a)
{
    FILE                *fp;
    char                line[512];
    int                 llen, rv, nume=0;
    xd_t                xd;
    diag_regex_engine_info_t    einfo[32], *papp;
    bcm_regex_match_t   match[64];
    regex_xml_data_t *ptag = NULL;
    int              tstart = 0, i, e, line_num = 0;
    bcm_regex_engine_t  reid;
    bcm_regex_engine_config_t eng_cfg;

    if (!SOC_UNIT_VALID(unit)) {
        return (CMD_FAIL);
    }
    sal_memset(&xd, 0, sizeof(xd_t));
    if (CMD_OK != (rv = regex_parse_args(unit, a, &xd))) {
        return(rv);
    }

    if (!xd.xd_file) {
        return CMD_FAIL;
    }

    if ((fp = sal_fopen(xd.xd_file, "r")) == NULL) {
        cli_out("Failed to open file: %s\n", xd.xd_file);
        return CMD_FAIL;
    }

    sal_memset(einfo, 0, sizeof(diag_regex_engine_info_t)*32);
    for (i = 0; i < sizeof(einfo)/sizeof(einfo[0]); i++) {
        for (e = 0; e < MAX_PATTERN_PER_ENGINE; e++) {
            bcm_regex_match_t_init(&einfo[i].match[e]);
        }
    }

    while(1) {
        /* parse the regex patterns from the file */
        line_num++;
        if (_read_line(fp, line, &llen)) {
            break;
        }
        if (_parse_a_xml_line(line, llen, &einfo[nume], &ptag, &tstart)) {
            cli_out("Error on line : %d\n", line_num);
            return CMD_FAIL;
        }
        if (ptag && (!sal_strcasecmp(ptag->tag_name, "signature")) &&
            tstart == 0) {
            sal_strcpy(einfo[nume].filename, xd.xd_file);
            einfo[nume].num_match++;
            ptag = NULL;
            tstart = 1;
        }
        if (ptag && (!sal_strcasecmp(ptag->tag_name, "engine")) &&
            tstart == 0) {
            nume++;
            ptag = NULL;
            tstart = 1;
        }
    }

    if (unit_inited[unit] == 0) {
        initialize_regex(unit, 1);
    }

    /* create an engine */
    eng_cfg.flags = 0;
    for (e=0; e<nume; e++) {
        rv = bcm_regex_engine_create(unit, &eng_cfg, &reid);
        if (rv) {
            cli_out("Failed to create regex engine : %d", rv);
            return CMD_FAIL;
        }

        re_engines[unit][engine_count[unit]++] = reid;

        /* Install the patterens */
        for (i = 0; i < einfo[e].num_match; i++) {
            sal_memcpy(&match[i], &einfo[e].match[i], sizeof(bcm_regex_match_t));
        }
        rv = bcm_regex_match_set(unit, reid, match, einfo[e].num_match);
        if (rv) {
            cli_out("Failed to install patterns : %d", rv);
            bcm_regex_engine_destroy(unit, reid);
            engine_count[unit]--;
            return CMD_FAIL;
        }

        einfo[e].engid = reid;

        /* store the match objects */
        for (i = 0; i< einfo[e].num_match; i++) {
            papp = sal_alloc(sizeof(diag_regex_engine_info_t), "einfo info");
            sal_memcpy(papp, &einfo[e], sizeof(diag_regex_engine_info_t));
            papp->engid = reid;
            papp->next = NULL;
            /* add to list  to corelate the event notifications */
            papp->next = engine_info_list[unit];
            engine_info_list[unit] = papp;
        }
    }

    if (xd.xd_file) {
        sal_free(xd.xd_file);
    }
    return (CMD_OK);
}

static int 
_regex_diag_remove_engine_from_db(int unit, bcm_regex_engine_t engid)
{
  int i, j;
    diag_regex_engine_info_t **ppapp, *papp;

    for (i = 0; i < engine_count[unit]; i++) {
        if (re_engines[unit][i] == engid) {
            for (j = i+1; j<engine_count[unit]; j++) {
                re_engines[unit][j-1] = re_engines[unit][j];
            }
            engine_count[unit]--;

            /* remove from applist */
            ppapp = &engine_info_list[unit];
            while(*ppapp) {
                papp = *ppapp;
                if (papp->engid == engid) {
                    *ppapp = papp->next;
                    sal_free(papp);
                    return 0;
                }
                ppapp = &(*ppapp)->next;
            }
        }
    }
    return -1;
}

static int 
_regex_destroy_engine_with_id(int unit, bcm_regex_engine_t engid,
                              bcm_regex_engine_config_t *config,
                              void *user_data)
{
    int rv;
    bcm_regex_engine_t del_id = *((bcm_regex_engine_t*) user_data);

    if ((del_id == -1) || (del_id == engid)) {
        rv = bcm_regex_engine_destroy(unit, engid);
        if (rv) {
            cli_out("Failed to destroy engine: %d\n", engid);
            return -1;
        }
        _regex_diag_remove_engine_from_db(unit, engid);
    }
    return CMD_OK;
}

static cmd_result_t
cmd_destroy_regex_engine(int unit, int engidx)
{
    int rv;
    bcm_regex_engine_t eng_id = -1;

    if (engidx >= 0) {
        if (engidx >= engine_count[unit]) {
            cli_out("Invalid engine index. Not found\n");
            return CMD_FAIL;
        }
        eng_id = re_engines[unit][engidx];
    }
   
    rv = bcm_regex_engine_traverse(unit, 
                                  _regex_destroy_engine_with_id, (void*) &eng_id);
    if (engine_count[unit] == 0) {
        initialize_regex(unit, 0);
    }
    return rv ? CMD_FAIL : CMD_OK;
}

static int verbose = 1;
static void bcm_regex_diag_report_cb(int unit, bcm_regex_report_t *report, 
                                    void *user_data)
{
    int valid_flag = 0;
    char flow_type[64], *pos = flow_type;

    if (!verbose) {
        return;
    }
    
    cli_out("-----------------------------------\n");
    if (report->flags & BCM_REGEX_REPORT_NEW)
    {
        strcpy(pos, "New ");
        valid_flag = 1;
        pos += 4;
    }
    if (report->flags & BCM_REGEX_REPORT_MATCHED)
    {
        strcpy(pos, "Matched ");
        valid_flag = 1;
        pos += 8;
    }
    if (report->flags & BCM_REGEX_REPORT_END)
    {
        strcpy(pos, "End ");
        valid_flag = 1;
        pos += 4;
    }
    if (0 == valid_flag)
    {
        strcpy(pos, "Unknown-Event");
        cli_out("Unknown-Event");
        pos += 13;
    }
    *pos = '\0';
    cli_out("Flow is: %s\n", flow_type);
    cli_out("\n");
    cli_out("matchID   = %d\n", report->match_id);
    cli_out("match_flags     =   0x%08x\n", report->match_flags);
    cli_out("flags           =   0x%08x\n", report->flags);
    cli_out("direction       =   %s\n", report->match_flags & BCM_REGEX_MATCH_TO_SERVER ? "To server" : "To client");
    cli_out("protocol        =   %d (0x%02x)\n", report->protocol, report->protocol);
    cli_out("src_port        =   %d (0x%04x)\n", report->src_port, report->src_port);
    cli_out("dst_port        =   %d (0x%04x)\n", report->dst_port, report->dst_port);
    cli_out("src_ip          =   %d.%d.%d.%d\n",
            
            (report->sip >> 24) & 0xff,
            (report->sip >> 16) & 0xff,
            (report->sip >> 8) & 0xff,
            (report->sip >> 0) & 0xff);
    cli_out("dst_ip          =   %d.%d.%d.%d\n",
            (report->dip >> 24) & 0xff,
            (report->dip >> 16) & 0xff,
            (report->dip >> 8) & 0xff,
            (report->dip >> 0) & 0xff);
    cli_out("sip6            =   %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
            report->sip6[0], report->sip6[1], report->sip6[2], report->sip6[3],
            report->sip6[4], report->sip6[5], report->sip6[6], report->sip6[7],
            report->sip6[8], report->sip6[9], report->sip6[10], report->sip6[11],
            report->sip6[12], report->sip6[13], report->sip6[14], report->sip6[15]);
    cli_out("dip6            =   %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
            report->dip6[0], report->dip6[1], report->dip6[2], report->dip6[3],
            report->dip6[4], report->dip6[5], report->dip6[6], report->dip6[7],
            report->dip6[8], report->dip6[9], report->dip6[10], report->dip6[11],
            report->dip6[12], report->dip6[13], report->dip6[14], report->dip6[15]);
    cli_out("start_timestamp =   %d, x10 ms\n", report->start_timestamp);
    cli_out("last_timestamp  =   %d, x10 ms\n", report->last_timestamp);
    cli_out("refresh_timestamp = %d, x10 ms\n", report->last_timestamp);
    return;
}

static int remon_registered = 0;

static cmd_result_t
cmd_regex_monitor(int unit, int start)
{
    int rv = 0;

    if (start) {
        if (!remon_registered) {
            rv = bcm_regex_report_register(unit,
                                   (BCM_REGEX_REPORT_NEW | \
                                    BCM_REGEX_REPORT_MATCHED | \
                                    BCM_REGEX_REPORT_END),
                                    bcm_regex_diag_report_cb, NULL);
            if (rv) {
                cli_out("Error(%d) Starting ReMon\n", rv);
            } else {
                remon_registered = 1;
            }
        }
    } else {
            rv = bcm_regex_report_unregister(unit,
                                   (BCM_REGEX_REPORT_NEW | \
                                    BCM_REGEX_REPORT_MATCHED | \
                                    BCM_REGEX_REPORT_END),
                                    bcm_regex_diag_report_cb, NULL);
            remon_registered = 0;
    }
    return CMD_OK;
}

extern void
_bcm_regex_sw_engine_hwid(int unit, bcm_regex_engine_t engid,
                            int32 *enghwid);

static int regex_cmd_engine_dump(int unit, 
                                 bcm_regex_engine_t engine, 
                                 bcm_regex_engine_config_t *config, 
                                 void *user_data)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    int enghwid;

    _bcm_regex_sw_engine_hwid(unit, engine, &enghwid);

    cli_out("------------------------------------------\n");
    cli_out("\tEngine-ID: 0x%x, HW index %d\n", engine, enghwid);
    if (config->flags & BCM_REGEX_ENGINE_CONFIG_MULTI_PACKET) {
        cli_out("\tMode : Multipacket");
    }
    cli_out("------------------------------------------\n");
#endif

    return 0;
}

static cmd_result_t dump_regex(int unit)
{
    bcm_regex_config_t recfg;

    if (bcm_regex_config_get(unit, &recfg)) {
        return CMD_FAIL;
    }

    /* display configuration */
    if (recfg.flags & BCM_REGEX_CONFIG_ENABLE) {
        cli_out("Regex : Enabled\n");
    } else {
        cli_out("Regex : Disabled\n");
        return CMD_OK;
    }

    cli_out("Max flow : %d\n", recfg.max_flows);
    cli_out("Max payload inspection size : %d\n", recfg.payload_depth);
    cli_out("Max packets to inspect: %d\n", recfg.inspect_num_pkt);
    cli_out("Report generation enabled for: ");
    if (recfg.report_flags & BCM_REGEX_REPORT_NEW) {
        cli_out("New Flow,");
    }
    if (recfg.report_flags & BCM_REGEX_REPORT_MATCHED) {
        cli_out("Flow match,");
    }
    if (recfg.report_flags & BCM_REGEX_REPORT_END) {
        cli_out("Flow terminated");
    }
    cli_out("%s\n", recfg.report_flags ? "" : "None");

    /* iterate over all the engines and display their configuration */
    bcm_regex_engine_traverse(unit, regex_cmd_engine_dump, NULL);
    return CMD_OK;
}

extern void 
_bcm_regex_sw_dump_engine(int unit, bcm_regex_engine_t engid,
                            uint32 f);
static int 
_regex_dump_engine_cb(int unit, bcm_regex_engine_t engid,
                      bcm_regex_engine_config_t *config,
                      void *user_data)
{
    bcm_regex_engine_t did = *((bcm_regex_engine_t*) user_data);

    if ((did == -1) || (did == engid)) {
#if defined(BCM_TRIUMPH3_SUPPORT)
        _bcm_regex_sw_dump_engine(unit, engid, 0);
#else
        return CMD_NOTIMPL;
#endif
    }
    return CMD_OK;
}

extern void
_bcm_regex_sw_load_engine(int unit, bcm_regex_engine_t engid,
                            uint32 f, int *eng_hwidx);
static int
_regex_load_engine_cb(int unit, bcm_regex_engine_t engid,
                      bcm_regex_engine_config_t *config,
                      void *user_data)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    int eng_hwidx = -1;

    _bcm_regex_sw_load_engine(unit, engid, 0, &eng_hwidx);
    if (eng_hwidx >= 0) {
        l_engine_count[unit]++;
        l_re_engines[unit][engid] = eng_hwidx;
        /*
        cli_out("Engine ID %d, hw index %d, engine count %d\n", engid, eng_hwidx, l_engine_count[unit]);
        */
    }
#else
    return CMD_NOTIMPL;
#endif
    return CMD_OK;
}

static int dump_regex_engine(int unit, int engidx)
{
    bcm_regex_engine_t eng_id = -1;

    if (engidx >= 0) {
        if (engidx >= l_engine_count[unit]) {
            cli_out("Invalid engine index. Not found\n");
            return CMD_FAIL;
        }
        eng_id = l_re_engines[unit][engidx];
    }
    bcm_regex_engine_traverse(unit, _regex_dump_engine_cb, (void*) &eng_id);
    return CMD_OK;
}

static int dump_load_regex_engine(int unit)
{
    int i;
    bcm_regex_engine_t eng_id = -1;

    for (i = 0; i < MAX_ENGINE_PER_UNIT; i++) {
        l_re_engines[unit][i] = -1;
    }
    l_engine_count[unit] = 0;
    bcm_regex_engine_traverse(unit, _regex_load_engine_cb, (void*) &eng_id);

    return CMD_OK;
}

static int dump_regex_engine_info(int unit, int engid)
{
    bcm_regex_engine_info_t regex_engine_info;
    int rv;

    rv = bcm_regex_engine_info_get(unit, engid, &regex_engine_info);

    if (rv != 0) {
        cli_out("Invalid engine index. Not found\n");
        return CMD_FAIL;
    } else if (engid >= 0) {
        printf("Engine %d Size = 0x%x, %d\n", engid, regex_engine_info.size, regex_engine_info.size);
        printf("Engine %d Free Size = 0x%x, %d\n", engid, regex_engine_info.free_size, regex_engine_info.free_size);
    } else {
        cli_out("SME Size = 0x%x, %d\n", regex_engine_info.size, regex_engine_info.size);
        cli_out("SME Free Size = 0x%x, %d\n", regex_engine_info.free_size, regex_engine_info.free_size);
    }

    return CMD_OK;
}

static int dump_regex_session_info(int unit)
{
    bcm_regex_info_t regex_info;
    int rv;

    rv = bcm_regex_info_get(unit, &regex_info);

    if (rv != 0) {
        cli_out("Session info not found\n");
    } else {
        cli_out("Session Table Size      = %d\n", regex_info.session_size);
        cli_out("Session Table IPv4 Free = %d\n", regex_info.session_free_size_ipv4);
        cli_out("Session Table IPv6 Free = %d\n", regex_info.session_free_size_ipv6);
        cli_out("Policy Table Size       = %d\n", regex_info.policy_size);
        cli_out("Policy Table Free Size  = %d\n", regex_info.policy_free_size);
    }

    return CMD_OK;
}

static int dump_regex_session_stats(int unit)
{
    int rv, i;
    uint64 val[bcmStatRegexSessionLast+1];

    for (i = 0; i <= bcmStatRegexSessionLast; i++)
    {
        rv = bcm_regex_stat_get(unit, i, &val[i]);
        if (rv != 0) {
            cli_out("Stat value %d not found\n", i);
            return CMD_OK;
        }
    }

    cli_out("Session entries in use      = %08x:%08x\n", COMPILER_64_HI(val[0]), COMPILER_64_LO(val[0]));
    cli_out("Session flows created       = %08x:%08x\n", COMPILER_64_HI(val[1]), COMPILER_64_LO(val[1]));
    cli_out("Session max flows in use    = %08x:%08x\n", COMPILER_64_HI(val[2]), COMPILER_64_LO(val[2]));
    cli_out("Session flows missed TCP    = %08x:%08x\n", COMPILER_64_HI(val[3]), COMPILER_64_LO(val[3]));
    cli_out("Session flows missed UDP    = %08x:%08x\n", COMPILER_64_HI(val[4]), COMPILER_64_LO(val[4]));
    cli_out("Session cmdwait timeouts    = %08x:%08x\n", COMPILER_64_HI(val[5]), COMPILER_64_LO(val[5]));
    cli_out("Session unused results      = %08x:%08x\n", COMPILER_64_HI(val[6]), COMPILER_64_LO(val[6]));
    cli_out("Session suppressed actions  = %08x:%08x\n", COMPILER_64_HI(val[7]), COMPILER_64_LO(val[7]));
    cli_out("Session TCP SYN data        = %08x:%08x\n", COMPILER_64_HI(val[8]), COMPILER_64_LO(val[8]));
    cli_out("Session L4 invalid          = %08x:%08x\n", COMPILER_64_HI(val[9]), COMPILER_64_LO(val[9]));
    cli_out("Session L4 ports excluded   = %08x:%08x\n", COMPILER_64_HI(val[10]), COMPILER_64_LO(val[10]));

    return CMD_OK;
}

static int dump_regex_match_stats(int unit)
{
    int rv, i;
    uint64 val[bcmStatRegexLast+1];

    for (i = bcmStatRegexSigMatchFirst; i <= bcmStatRegexSigMatchLast; i++)
    {
        rv = bcm_regex_stat_get(unit, i, &val[i]);
        if (rv != 0) {
            cli_out("Stat value %d not found\n", i);
            return CMD_OK;
        }
    }

    i = bcmStatRegexSigMatchFirst;

    cli_out("Sig match packets received      = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;
    cli_out("Sig match packets sent          = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;
    cli_out("Sig match packets dropped       = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;
    cli_out("Sig match bytes matched         = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;
    cli_out("Sig match matched flows         = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;
    cli_out("Sig match unmatched flows       = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;
    cli_out("Sig match total match           = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;
    cli_out("Sig match cross sig flags       = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;
    cli_out("Sig match fragments received    = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;
    cli_out("Sig match in packet error       = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;
    cli_out("Sig match flow tracker error    = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;
    cli_out("Sig match packet length error   = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;
    cli_out("Sig match L4 checksum error     = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;
    cli_out("Sig match flow done packet drop = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;
    cli_out("Sig match flow timestamp error  = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;
    cli_out("Sig match flow packet num error = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;
    cli_out("Sig match ECC error             = %08x:%08x\n", COMPILER_64_HI(val[i]), COMPILER_64_LO(val[i])); i++;

    return CMD_OK;
}

extern int _bcm_tr3_regex_dump_axp(int unit);

static int
_regex_dump_axp(int unit)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    int rv;

    rv = _bcm_tr3_regex_dump_axp(unit);
    if (rv != 0) {
        rv = CMD_FAIL;
    }
#else
    return CMD_NOTIMPL;
#endif
    return CMD_OK;
}

static int 
_regex_dump_engine_dfa_cb(int unit, bcm_regex_engine_t engid,
                              bcm_regex_engine_config_t *config,
                              void *user_data)
{
    bcm_regex_engine_t did = *((bcm_regex_engine_t*) user_data);

    if ((did == -1) || (did == engid)) {
#if defined(BCM_TRIUMPH3_SUPPORT)
        _bcm_regex_sw_dump_engine(unit, engid,1);
#else
        return CMD_NOTIMPL;
#endif
    }
    return CMD_OK;
}

static int dump_regex_engine_dfa(int unit, int engidx)
{
    bcm_regex_engine_t eng_id = -1;

    if (engidx >= 0) {
        if (engidx >= l_engine_count[unit]) {
            cli_out("Invalid engine index. Not found\n");
            return CMD_FAIL;
        }
        eng_id = l_re_engines[unit][engidx];
    }
    bcm_regex_engine_traverse(unit, _regex_dump_engine_dfa_cb, (void*) &eng_id);
    return CMD_OK;
}

typedef struct re_diag_ud_s {
    bcm_regex_engine_t engid;
    int found;
} re_diag_ud_t;

#if 0
static int
_regex_engine_present_in_hw(int unit, bcm_regex_engine_t engid,
                              bcm_regex_engine_config_t *config,
                              void *user_data)
{
    re_diag_ud_t *ud = (re_diag_ud_t*) user_data;

    if (ud->engid == engid) {
        ud->found = 1;
    }

    return CMD_OK;
}

static int _regex_engine_sync(int unit, bcm_regex_engine_t engid)
{
    re_diag_ud_t ud;

    ud.engid = engid;
    ud.found = 0;
    bcm_regex_engine_traverse(unit, _regex_engine_present_in_hw, (void*)&ud);
    if (ud.found == 0) {
        _regex_diag_remove_engine_from_db(unit, engid);
    }
    return 0;
}
#endif /* 0 */

static int _regex_dump_app_info(int unit, bcm_regex_engine_t engid)
{
    diag_regex_engine_info_t *papp;
    int i;

    /* remove from applist */
    papp = engine_info_list[unit];
    while(papp) {
        if (papp->engid == engid) {
            cli_out("\t File: %s\n", papp->filename);
            cli_out("\t Number of patterns : %d\n", papp->num_match);
            for (i = 0; i < papp->num_match; i++) {
                cli_out("\t Pattern %d : %s\n", i, papp->match[i].pattern);
            }
            return 0;
        }
        papp = papp->next;
    }
    return -1;
}

extern void _bcm_esw_policer_dump(int unit, bcm_policer_t policer_id);
extern void _bcm_esw_policy_dump(int unit, bcm_regex_policy_t policy);

cmd_result_t cmd_regex(int unit, args_t *a)
{
    char		*subcmd, *param;
    int                 mon_start = 0, eindex, i;
    bcm_regex_policy_t  policy;
    bcm_policer_t       policer_id;

    if (!soc_feature(unit, soc_feature_regex)) {
        return CMD_NOTIMPL;
    }

    if (re_inited == 0) {
        regex_cmd_init();
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return dump_regex(unit);
    }

    if (sal_strcasecmp(subcmd, "create") == 0) {
        return cmd_create_regex_engine(unit, a);
    } else if (sal_strcasecmp(subcmd, "destroy") == 0) {
        eindex = -1;
        if ((subcmd = ARG_GET(a))) {
            eindex = atoi(subcmd);
        } else {
            eindex = -1;
        }
        return cmd_destroy_regex_engine(unit, eindex);
    } else if (sal_strcasecmp(subcmd, "monitor") == 0) {
        if ((subcmd = ARG_GET(a)) == NULL) {
	    mon_start = 1;
        } else {
            if (sal_strcasecmp(subcmd, "start") == 0) {
                mon_start = 1;
            } else if (sal_strcasecmp(subcmd, "stop") == 0) {
                mon_start = 0;
            } else if (sal_strcasecmp(subcmd, "verbose") == 0) {
                verbose = 1;
                return 0;
            } else if (sal_strcasecmp(subcmd, "silent") == 0) {
                verbose = 0;
                return 0;
            } else {
                mon_start = 0;
            }
        }
        return cmd_regex_monitor(unit, mon_start);
    } else if (sal_strcasecmp(subcmd, "list") == 0) {
        dump_load_regex_engine(unit);
        /* list all the engines on this unit */
        cli_out("Total %d engines installed.\n", l_engine_count[unit]);
        for (i = 0; i < l_engine_count[unit]; i++) {
            cli_out("----- engine %d hw idx %d --------\n", i, l_re_engines[unit][i]);
            _regex_dump_app_info(unit, l_re_engines[unit][i]);
        }
        return CMD_OK;
    } else if (sal_strcasecmp(subcmd, "show") == 0) {
        dump_load_regex_engine(unit);
        subcmd = ARG_GET(a);
        if (!subcmd) {
            return dump_regex(unit);
        }

        param = ARG_GET(a);
        if (sal_strcasecmp(subcmd, "engine") == 0) {
            eindex = -1;
            if (param) {
                eindex = atoi(param);
            }
            return dump_regex_engine(unit, eindex);
        } else if (sal_strcasecmp(subcmd, "dfa") == 0) {
            eindex = -1;
            if (param) {
                eindex = atoi(param);
            }
            return dump_regex_engine_dfa(unit, eindex);
        } else if (sal_strcasecmp(subcmd, "policer") == 0) {
            policer_id = -1;
            if (param) {
                policer_id = atoi(param);
            }
            _bcm_esw_policer_dump(unit, policer_id);
        } else if (sal_strcasecmp(subcmd, "policy") == 0) {
            policy = -1;
            if (param) {
                policy = atoi(param);
            }
            _bcm_esw_policy_dump(unit, policy);
        } else if (sal_strcasecmp(subcmd, "info") == 0) {
            eindex = -1;
            if (param) {
                eindex = atoi(param);
            }
            return dump_regex_engine_info(unit, eindex);
        } else if (sal_strcasecmp(subcmd, "session") == 0) {
            if (!param) {
                return dump_regex_session_info(unit);
            }
            if (sal_strcasecmp(param, "stats") == 0) {
                return dump_regex_session_stats(unit);
            }
        } else if (sal_strcasecmp(subcmd, "match") == 0) {
            if (!param) {
                return CMD_OK;
            }
            if (sal_strcasecmp(param, "stats") == 0) {
                return dump_regex_match_stats(unit);
            }
        } else if (sal_strcasecmp(subcmd, "axp") == 0) {
            return _regex_dump_axp(unit);
        }
        return CMD_OK;
    }
    return CMD_USAGE;
}
#endif 

#else
typedef int _diag_esw_regex_not_empty; /* Make ISO compilers happy. */
#endif
