/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Packet Watcher
 */

#include <sal/types.h>
#include <shared/bsl.h>
#ifdef INCLUDE_MACSEC

#ifndef __KERNEL__
#include <sys/types.h>
#include <netinet/in.h>
#endif

#include <soc/debug.h>
#include <soc/cm.h>

#include <sal/appl/io.h>
#include <sal/appl/sal.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <sal/core/spl.h>
#include <sal/core/libc.h>

#include <appl/diag/shell.h>
#include <appl/diag/parse.h>
#include <appl/diag/system.h>
#include <appl/diag/decode.h>
#include <appl/diag/dport.h>

#include <bcm/macsec.h>
#include <bcm/error.h>

#include <bcm_int/common/macsec_cmn.h>
#include <bcm_int/esw/port.h>
#include <soc/macsecphy.h>

#include <bmacsec.h>
#include <bmacsec_cli.h>

#ifdef PLISIM
#define MACSEC_SIM 1
/*
#define MACSEC_SIM_DEBUG 1
*/
#endif

/*
 * This variable stores the current BCM unit.
 */
static int _macsec_bcm_unit = -1;

#ifdef MACSEC_SIM

typedef struct _macsec_dev_info_s {
    char                *name;
    bcm_macsec_core_t   core_type;
    int                 num_port;
} _macsec_dev_info_t;

static _macsec_dev_info_t macsec_dev_info_table[] = {
    {"BCM54580", BCM_MACSEC_CORE_BCM5458X, 8},
    {"BCM8729",  BCM_MACSEC_CORE_BCM8729,  4},
    {NULL,       0,  0},
};

typedef struct _macsec_sim_io_res_s {
    uint32 dev_addr;
    uint32 dev_port;
    uint32 io_addr;
    int    res_size;
#define MACSEC_SIM_MAX_DATA_SIZE 16
    uint32 data[MACSEC_SIM_MAX_DATA_SIZE];
    struct _macsec_sim_io_res_s *next;
} _macsec_sim_io_res_t;

static _macsec_sim_io_res_t *io_res_bucket[16];

int macsec_sim_init()
{
    int ii;

    for (ii = 0; ii < 16; ii++) {
        io_res_bucket[ii] = NULL;
    }
    return 0;
}

void _dump_res(int bucket)
{
    _macsec_sim_io_res_t **pentry;
    int loop_cnt = 0;

    pentry = &io_res_bucket[bucket];
    cli_out("Dumping bucket %d\n", bucket);
    while(*pentry) {
        cli_out("0x%08x : %d %d %d \n", (*pentry)->io_addr, 
                (*pentry)->res_size,
                (*pentry)->dev_addr,
                (*pentry)->dev_port);
        pentry = &(*pentry)->next;
        if (++loop_cnt > 10000) {
            break;
        }
    }
}

#define MACSEC_SIM_RES_BUCKET(io) (((io) >> 20) & 0xf)

_macsec_sim_io_res_t* _macsec_sim_find_res(int dev_addr, int dev_port,
                                           int wordSz, uint32 io_addr)
{
    _macsec_sim_io_res_t **pentry, *res;
    int     ii, loop_cnt = 0;

    res = io_res_bucket[MACSEC_SIM_RES_BUCKET(io_addr)];

    while(res && ((res->io_addr != io_addr) || 
                  (res->dev_addr != dev_addr))) {
        res = res->next;
        if (++loop_cnt > 1000000) {
            _dump_res(MACSEC_SIM_RES_BUCKET(io_addr));
            return NULL;
        }
    }

    if (res == NULL) {
        res = sal_alloc(sizeof(_macsec_sim_io_res_t), "macsec sim res");
        for (ii = 0; ii < MACSEC_SIM_MAX_DATA_SIZE; ii++) {
            res->data[ii] = 0xbabeadda;
        }
        res->io_addr = io_addr;
        res->res_size = wordSz;
        res->dev_addr = dev_addr;
        res->next = NULL;

        pentry = &io_res_bucket[MACSEC_SIM_RES_BUCKET(io_addr)];
        res->next = *pentry;
        *pentry = res;
    }


    return res;
}

static int _macsec_sim_do_io_op(_macsec_sim_io_res_t *res, 
                                bmacsec_io_op_t op, 
                                int wordSz, void *data)
{
    int ii, read;

    read = ((op == BMACSEC_IO_TBL_RD) || 
            (op == BMACSEC_IO_REG_RD)) ? 1 : 0;

    if (read) {
        for (ii = 0; ii < wordSz; ii++) {
            *((uint32*)data + ii) = res->data[ii];
        }
    } else {
        for (ii = 0; ii < wordSz; ii++) {
            res->data[ii] = *((uint32*)data + ii);
        }
    }
#if defined(MACSEC_SIM_DEBUG)
    cli_out("SIM %s (0x%08x): 0x", (read ? "READ" : "WRITE"), res->io_addr);
    for (ii = 0; ii < wordSz; ii++) {
        cli_out("%08x ", res->data[ii]);
    }
    cli_out("\n");
#endif
    return 0;
}

static int macsec_sim_inited = 0;

int macsec_sim_io_handler(bcm_macsec_dev_addr_t devaddr,/* device addr*/
                          int dev_port,        /* dev port index  */
                          bmacsec_io_op_t op,  /* I/O operation   */  
                          uint32 io_addr,      /* I/O address     */
                          int wordSz,         /* Word size       */
                          int num_entry,
                          uint32 *data)
{
    _macsec_sim_io_res_t *res;

    if (macsec_sim_inited == 0) {
        macsec_sim_init();
        macsec_sim_inited = 1;
    }

    res = _macsec_sim_find_res(devaddr, dev_port, wordSz, io_addr);

    if (res == NULL) {
        return -1;
    }
    _macsec_sim_do_io_op(res, op, wordSz, data);
    return 0;
}

static int _macsec_sim_port_create(char *dev_type)
{
    int                         dev, port, p, num_devs = 1, num_port;
    bcm_macsec_port_config_t    pCfg;
    _macsec_dev_info_t         *dev_info;
    int                         dev_addr, unit, sw_port = 1;

    /* lookup device info table */
    dev_info = macsec_dev_info_table;
    while (dev_info->name && sal_strcasecmp(dev_info->name, dev_type)) {
        dev_info++;
    }

    if (dev_info->name == NULL) {
        cli_out("Unknown device : %s\n", dev_type);
        return CMD_FAIL;
    }

    unit = _macsec_bcm_unit;
    sw_port = 1; /* Start using switch port 1 onwards, as CPU port is often zero */
    num_port = dev_info->num_port;
    for (dev = 0; dev < num_devs; dev++) {
        for (port = 0; port < num_port; port++, sw_port++) {
            /* create port itentifier */
            p = SOC_MACSEC_PORTID(unit, sw_port);
            dev_addr = soc_property_port_get(unit, port, 
                                        spn_MACSEC_DEV_ADDR, -1);
            if (dev_addr < 0) {
                cli_out("No %s property specified for port u=%d/p=%d\n",
                        spn_MACSEC_DEV_ADDR, unit, sw_port);
                continue;
            }
            bmacsec_port_destroy(p);
            bmacsec_port_create(p, dev_info->core_type,
                                dev_addr, port, macsec_sim_io_handler);
            cli_out("SIM: %d/%d is now MACSEC enabled.\n", unit, sw_port);


            bcm_macsec_port_config_get(unit, sw_port, &pCfg);
            pCfg.flags |= BCM_MACSEC_PORT_ENABLE;
            pCfg.max_frame = 1518;
            bcm_macsec_port_config_set(unit, sw_port, &pCfg);
        }
    }
    return 0;
}

#endif /* MACSEC_SIM */

char sh_macsec_usage[] =
    "Call macsec help for more information.\n"
    "\n";


static int
macsec_port_sym_parse(char *pname, bcm_macsec_dev_addr_t *dev_addr, 
                      int *dev_port, bmacsec_port_t *pid)
{
    bcm_port_config_t   pcfg;
    pbmp_t              pbm;
    soc_port_t          p, dport;
    int                 unit = _macsec_bcm_unit;

    if (unit < 0) {
        return -1;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        return CMD_FAIL;
    }

    BCM_PBMP_ASSIGN(pbm, pcfg.port);

    if (BCM_PBMP_IS_NULL(pbm)) {
        return -1;
    }

    DPORT_BCM_PBMP_ITER(unit, pbm, dport, p) {
        if (sal_strcasecmp(pname, BCM_PORT_NAME(unit, p)) == 0) {
            /*
             * Fill device address and device port info.
             */
            *dev_addr = soc_property_port_get(unit, p, 
                                        spn_MACSEC_DEV_ADDR, -1);
            *dev_port = soc_property_port_get(unit, p, 
                                     spn_MACSEC_PORT_INDEX, -1);
            *pid = SOC_MACSEC_PORTID(unit, p);
            return 0;
        }
    }

    return -1;
}


/*
 * Function: 	sh_macsec
 * Purpose:	MACSec comands
 * Parameters:	u - SOC unit #
 * Returns:	CMD_OK/CMD_FAIL/
 */
cmd_result_t
sh_macsec(int u, args_t *a)
{
    int rv = CMD_USAGE;
    bmacsec_cli_cfg_t bmacsec_cli_cfg;
#ifdef MACSEC_SIM
    char *arg;
    char  *dev_type;
#endif
    int num_a;
    static int macsec_cli_registered = 0;

    _macsec_bcm_unit = u;

    if(!macsec_cli_registered) {
        bmacsec_cli_cfg.port_sym_parse = macsec_port_sym_parse;
#ifdef MACSEC_SIM
        bmacsec_cli_cfg.dev_io_f = macsec_sim_io_handler;
#else
        bmacsec_cli_cfg.dev_io_f = NULL;
#endif
        rv = bmacsec_cli_register(&bmacsec_cli_cfg);
        if(rv < 0) {
            cli_out("Failed to register MACSEC CLI\n");
            return CMD_FAIL;
        }
   
        macsec_cli_registered = 1;
    }

#ifdef MACSEC_SIM
    arg = ARG_GET(a);
    if (arg != NULL && sal_strcasecmp(arg, "sim_init") == 0) {
        /* get the device to simulate */
        dev_type = ARG_GET(a);
        if (!dev_type) {
            dev_type = "BCM8729";
        }
        return _macsec_sim_port_create(dev_type);
    }
#endif
    num_a = a->a_argc - 1;
    num_a = (num_a < 0) ? 0 : num_a;
    rv = bmacsec_handle_cmd(num_a, &a->a_argv[1]);
    a->a_arg = a->a_argc; /* Consume all args */
    _macsec_bcm_unit = -1;
    return rv;
}

#endif /* INCLUDE_MACSEC */
