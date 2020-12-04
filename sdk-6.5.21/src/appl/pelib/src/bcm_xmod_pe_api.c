/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       bcm_xmod_pe_api.c
 */

#include "types.h"
#include "error.h"
#include "bcm_utils.h"
#include "bcm_xmod_api.h"
#include "eyescan.h"
#include "dscdump.h"
#include "prbs.h"

extern int
bcm_device_init_leds(int unit, int module);
extern int
bcm_start_cpu(int unit, int module);
extern int
bcm_xmod_command(int unit, int module, int mode_opcode,  uint32 *arg, int arg_size, uint32 *result, int result_size);

int bcm_mode[NUM_MODULES];

int bcm_set_module_mode(int module, int mode)
{
    if (module<0 || module>=NUM_MODULES) {
        BCM_LOG_ERR("%s() invalid module number %d\n", __FUNCTION__, module);
        return -1;
    }
    if (mode!=XMOD_PE_MODE_4_PORTS && mode!=XMOD_PE_MODE_2_PORTS) {
        BCM_LOG_ERR("%s() invalid mode number %d\n", __FUNCTION__, mode);
        return -1;
    }

    bcm_mode[module] = mode;
    return 0;
}

int bcm_get_module_mode(int module, int *mode)
{
    if (module<0 || module>=NUM_MODULES) {
        BCM_LOG_ERR("%s() invalid module number %d\n", __FUNCTION__, module);
        return -1;
    }

    if (bcm_mode[module]!=XMOD_PE_MODE_4_PORTS && bcm_mode[module]!=XMOD_PE_MODE_2_PORTS) {
        BCM_LOG_ERR("%s() invalid mode number %d\n", __FUNCTION__, bcm_mode[module]);
        return -1;
    }

    *mode = bcm_mode[module];
    return 0;
}

int bcm_port_lane_mask(int unit, int module, int sys_port)
{
    int mode;
    int lane_mask = 1<<module;

    if (sys_port)
        /* system ports are one lane */
        return lane_mask;

    if (module)
        /* only module 0 can be 2 lanes */
        return lane_mask;

    bcm_get_module_mode(module, &mode);
    if (mode==XMOD_PE_MODE_4_PORTS) {
        /* this port uses 2 lanes */
        BCM_LOG_CLI("%s() port uses 2 lanes.\n", __FUNCTION__);
        lane_mask = 0x3;
    }

    return lane_mask;
}

/* ! Byte swap unsigned short */
uint16 swap_uint16( uint16 val ) 
{
#ifdef BE_HOST
    return (val << 8) | (val >> 8 );
#else
    return val;
#endif 
}

/* ! Byte swap short */
int16 swap_int16( int16 val ) 
{
#ifdef BE_HOST
    return (val << 8) | ((val >> 8) & 0xFF);
#else
    return val;
#endif 
}

/* ! Byte swap unsigned int */
uint32 swap_uint32( uint32 val )
{
#ifdef BE_HOST
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | (val >> 16);
#else
    return val;
#endif 
}

/* ! Byte swap int */
int32 swap_int32( int32 val )
{
#ifdef BE_HOST
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | ((val >> 16) & 0xFFFF);
#else
    return val;
#endif 
}

/* ! Byte swap unsigned int */
uint64 swap_uint64( uint64 val )
{
#ifdef BE_HOST
    uint32 val0 = val>>32;
    uint32 val1 = val & 0xffffffff;
    return ((uint64)swap_uint32(val1))<<32 | swap_uint32(val0);
#else
    return val;
#endif 
}

int bcm_xmod_pe_init(int unit, int module, xmod_pe_config_t *config)
{
    xmod_pe_config_t x_config;
    uint32 xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8 *buftxptr;
    int xmodtxlen, rv=0, idx;

    BCM_LOG_DEBUG("%s() enter; unit %d; module %d\n", __FUNCTION__, unit, module);

    /* initialize board leds */
    /* bcm_device_init_leds(unit, module); */

    /* setting up xmod xmod_pe_init: */
    /* int xmod_pe_init(IN xmod_pe_config_t *config)  */

    BCM_LOG_CLI("%s() mode %d\n", __FUNCTION__, config->mode);
    BCM_LOG_CLI("%s() tag %d\n", __FUNCTION__, config->tag);
    BCM_LOG_CLI("%s() pfc %d\n", __FUNCTION__, config->pfc);
    BCM_LOG_CLI("%s() line10g %d\n", __FUNCTION__, config->line10g);
    BCM_LOG_CLI("%s() tagid[0] 0x%x\n", __FUNCTION__, config->tagid[0]);
    BCM_LOG_CLI("%s() tagid[1] 0x%x\n", __FUNCTION__, config->tagid[1]);
    BCM_LOG_CLI("%s() tagid[2] 0x%x\n", __FUNCTION__, config->tagid[2]);
    BCM_LOG_CLI("%s() tagid[3] 0x%x\n", __FUNCTION__, config->tagid[3]);
    BCM_LOG_CLI("%s() lt_up_port 0x%x\n", __FUNCTION__, config->lt_up_port);
    BCM_LOG_CLI("%s() lt_pe_port[0] 0x%x\n", __FUNCTION__, config->lt_pe_port[0]);
    BCM_LOG_CLI("%s() lt_pe_port[1] 0x%x\n", __FUNCTION__, config->lt_pe_port[1]);
    BCM_LOG_CLI("%s() lt_pe_port[2] 0x%x\n", __FUNCTION__, config->lt_pe_port[2]);
    BCM_LOG_CLI("%s() lt_pe_port[3] 0x%x\n", __FUNCTION__, config->lt_pe_port[3]);

    /* save mode */
    bcm_set_module_mode(unit, config->mode);
    x_config.mode = config->mode;
    x_config.tag = config->tag;
    x_config.pfc = config->pfc;
    x_config.line10g = config->line10g;
    for (idx=0; idx<4; idx++) {
        x_config.tagid[idx] = swap_uint16(config->tagid[idx]);
    }
    x_config.lt_up_port = config->lt_up_port;
    for (idx=0; idx<3; idx++) {
        x_config.res0[idx] = config->res0[idx];
    }
    for (idx=0; idx<4; idx++) {
        x_config.lt_pe_port[idx] = config->lt_pe_port[idx];
    }

    /* write args to xmod buffer */
    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_config, sizeof(xmod_pe_config_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PE_INIT);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = bcm_xmod_command(unit, module,
                                XMOD_CMD_MODE_ETH(XMOD_PE_INIT),
                                xmodtxbuff, xmodtxlen, NULL, 0);
    if (SOC_FAILURE(rv)) {
        BCM_LOG_ERR("%s() bcm_xmod_command() error %d\n", __FUNCTION__, rv);
        return SOC_E_FAIL;
    }
    
    return rv;
}

int bcm_xmod_pe_status_get(int unit, int module, xmod_pe_status_t *status)
{
    uint32 xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8 *bufrxptr;
    int xmodrxlen, rv;

    BCM_LOG_DEBUG("%s() enter; unit %d; module %d\n", __FUNCTION__, unit, module);

    /* setting up xmod xmod_pe_status_get:      */
    /* int xmod_pe_status_get(OUT xmod_pe_status_t status) */

    /* setup out args */
    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PE_STATUS_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = bcm_xmod_command(unit, module,
                                XMOD_CMD_MODE_ETH(XMOD_PE_STATUS_GET),
                                NULL, 0, xmodrxbuff, xmodrxlen);
    if (SOC_FAILURE(rv)) {
        BCM_LOG_ERR("%s() bcm_xmod_command() error %d\n", __FUNCTION__, rv);
        return SOC_E_FAIL;
    }

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8 *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, status, sizeof(xmod_pe_status_t));

    BCM_LOG_CLI("%s() up_port:  res %d; link %d\n", __FUNCTION__, status->up_port.res, status->up_port.link_up);
    BCM_LOG_CLI("%s() pe_port0: res %d; link %d\n", __FUNCTION__, status->pe_port0.res, status->pe_port0.link_up);
    BCM_LOG_CLI("%s() pe_port1: res %d; link %d\n", __FUNCTION__, status->pe_port1.res, status->pe_port1.link_up);
    BCM_LOG_CLI("%s() pe_port2: res %d; link %d\n", __FUNCTION__, status->pe_port2.res, status->pe_port2.link_up);
    BCM_LOG_CLI("%s() pe_port3: res %d; link %d\n", __FUNCTION__, status->pe_port3.res, status->pe_port3.link_up);
    BCM_LOG_CLI("%s() up_port: module %d\n", __FUNCTION__, status->up_port_mod);

    return rv;
}

int bcm_xmod_pe_port_stats_get(int unit, int module, int pe_port, xmod_pe_port_stats_t *stats, int reset)
{
    uint8 x_port = (uint8)pe_port;
    uint8 x_reset = (uint8)reset;
    uint32 xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8 *buftxptr;
    int xmodtxlen;
    uint32 xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8 *bufrxptr;
    int xmodrxlen;
    int rv;

    BCM_LOG_DEBUG("%s() enter; unit %d; module %d\n", __FUNCTION__, unit, module);

    /* setting up xmod xmod_pe_port_stats_get:                              */
    /* int xmod_pe_port_stats_get(IN uint8 port, OUT xmod_pe_port_stats_t stats) */

    /* setup in args - write args to xmod buffer */
    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_port, sizeof(uint8));
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_reset, sizeof(uint8));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PE_PORT_STATS_GET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* setup out args */
    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PE_PORT_STATS_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = bcm_xmod_command(unit, module,
                                XMOD_CMD_MODE_ETH(XMOD_PE_PORT_STATS_GET),
                                xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);
    if (SOC_FAILURE(rv)) {
        BCM_LOG_ERR("%s() bcm_xmod_command() error %d\n", __FUNCTION__, rv);
        return SOC_E_FAIL;
    }

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8 *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, stats, sizeof(xmod_pe_port_stats_t));
    stats->port_dn_cnt = swap_uint32(stats->port_dn_cnt);

    stats->rpkt = swap_uint64(stats->rpkt);
    stats->rbyt = swap_uint64(stats->rbyt);
    stats->tpkt = swap_uint64(stats->tpkt);
    stats->tbyt = swap_uint64(stats->tbyt);

    stats->rfcs = swap_uint64(stats->rfcs);
    stats->rfrg = swap_uint64(stats->rfrg);
    stats->rjbr = swap_uint64(stats->rjbr);
    stats->rovr = swap_uint64(stats->rovr);
    stats->rrpkt = swap_uint64(stats->rrpkt);
    stats->raln = swap_uint64(stats->raln);
    stats->rerpkt = swap_uint64(stats->rerpkt);
    stats->rmtue = swap_uint64(stats->rmtue);
    stats->rprm = swap_uint64(stats->rprm);
    stats->rschcrc = swap_uint64(stats->rschcrc);
    stats->rtrfu = swap_uint64(stats->rtrfu);
    stats->rfcr = swap_uint64(stats->rfcr);
    stats->rxpf = swap_uint64(stats->rxpf);
    stats->rflr = swap_uint64(stats->rflr);

    stats->tfcs = swap_uint64(stats->tfcs);
    stats->tfrg = swap_uint64(stats->tfrg);
    stats->tjbr = swap_uint64(stats->tjbr);
    stats->tncl = swap_uint64(stats->tncl);
    stats->tovr = swap_uint64(stats->tovr);
    stats->trpkt = swap_uint64(stats->trpkt);
    stats->txpf = swap_uint64(stats->txpf);

    BCM_LOG_CLI("%s(%d) port_dn_cnt %x\n", __FUNCTION__, x_port, stats->port_dn_cnt);

    BCM_LOG_CLI("%s(%d) rpkt %llx\n", __FUNCTION__, x_port, stats->rpkt);
    BCM_LOG_CLI("%s(%d) rbyt %llx\n", __FUNCTION__, x_port, stats->rbyt);
    BCM_LOG_CLI("%s(%d) tpkt %llx\n", __FUNCTION__, x_port, stats->tpkt);
    BCM_LOG_CLI("%s(%d) tbyt %llx\n", __FUNCTION__, x_port, stats->tbyt);

    BCM_LOG_CLI("%s(%d) rfcs %llx\n", __FUNCTION__, x_port, stats->rfcs);
    BCM_LOG_CLI("%s(%d) rfrg %llx\n", __FUNCTION__, x_port, stats->rfrg);
    BCM_LOG_CLI("%s(%d) rjbr %llx\n", __FUNCTION__, x_port, stats->rjbr);
    BCM_LOG_CLI("%s(%d) rovr %llx\n", __FUNCTION__, x_port, stats->rovr);
    BCM_LOG_CLI("%s(%d) rrpkt %llx\n", __FUNCTION__, x_port, stats->rrpkt);
    BCM_LOG_CLI("%s(%d) raln %llx\n", __FUNCTION__, x_port, stats->raln);
    BCM_LOG_CLI("%s(%d) rerpkt %llx\n", __FUNCTION__, x_port, stats->rerpkt);
    BCM_LOG_CLI("%s(%d) rmtue %llx\n", __FUNCTION__, x_port, stats->rmtue);
    BCM_LOG_CLI("%s(%d) rprm %llx\n", __FUNCTION__, x_port, stats->rprm);
    BCM_LOG_CLI("%s(%d) rschcrc %llx\n", __FUNCTION__, x_port, stats->rschcrc);
    BCM_LOG_CLI("%s(%d) rtrfu %llx\n", __FUNCTION__, x_port, stats->rtrfu);
    BCM_LOG_CLI("%s(%d) rfcr %llx\n", __FUNCTION__, x_port, stats->rfcr);
    BCM_LOG_CLI("%s(%d) rxpf %llx\n", __FUNCTION__, x_port, stats->rxpf);
    BCM_LOG_CLI("%s(%d) rflr %llx\n", __FUNCTION__, x_port, stats->rflr);

    BCM_LOG_CLI("%s(%d) tfcs %llx\n", __FUNCTION__, x_port, stats->tfcs);
    BCM_LOG_CLI("%s(%d) tfrg %llx\n", __FUNCTION__, x_port, stats->tfrg);
    BCM_LOG_CLI("%s(%d) tjbr %llx\n", __FUNCTION__, x_port, stats->tjbr);
    BCM_LOG_CLI("%s(%d) tncl %llx\n", __FUNCTION__, x_port, stats->tncl);
    BCM_LOG_CLI("%s(%d) tovr %llx\n", __FUNCTION__, x_port, stats->tovr);
    BCM_LOG_CLI("%s(%d) trpkt %llx\n", __FUNCTION__, x_port, stats->trpkt);
    BCM_LOG_CLI("%s(%d) txpf %llx\n", __FUNCTION__, x_port, stats->txpf);

    return rv;
}

int bcm_xmod_pe_fw_ver_get(int unit, int module, xmod_pe_fw_ver_t *ver)
{
    xmod_pe_fw_ver_t x_ver;
    uint32 xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8 *bufrxptr;
    int xmodrxlen, rv;

    BCM_LOG_DEBUG("%s() enter; unit %d; module %d\n", __FUNCTION__, unit, module);

    /* setting up xmod xmod_pe_fw_ver_get:      */
    /* int xmod_pe_fw_ver_get(OUT xmod_pe_fw_ver_t fw_ver) */

    /* setup out args */
    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PE_FW_VER_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = bcm_xmod_command(unit, module,
                                XMOD_CMD_MODE_ETH(XMOD_PE_FW_VER_GET),
                                NULL, 0, xmodrxbuff, xmodrxlen);
    if (SOC_FAILURE(rv)) {
        BCM_LOG_ERR("%s() bcm_xmod_command() error %d\n", __FUNCTION__, rv);
        return SOC_E_FAIL;
    }

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8 *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_ver, sizeof(xmod_pe_fw_ver_t));
    ver->major = swap_uint16(x_ver.major);
    ver->minor = swap_uint16(x_ver.minor);
    ver->build = swap_uint16(x_ver.build);

    BCM_LOG_CLI("%s() major %d\n", __FUNCTION__, ver->major);
    BCM_LOG_CLI("%s() minor %d\n", __FUNCTION__, ver->minor);
    BCM_LOG_CLI("%s() build %d\n", __FUNCTION__, ver->build);

    return rv;
}

int bcm_eyescan(int unit, int module, int sys_port)
{
    int rv = 0;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, module);

    BCM_LOG_CLI("Eyescan module: %d; sys_port: %d\n", module, sys_port);
    rv = bcm_do_eyescan(pc, sys_port);

    return rv;
}

int bcm_dscdump(int unit, int module, int sys_port)
{
    int rv = 0;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, module);

    BCM_LOG_CLI("DSC Dump module %x sys_port %d\n",module, sys_port);
    rv = bcm_do_dscdump(pc, sys_port);

    return rv;
}

int bcm_prbs(int unit, int module, int sys_port, int action, int dir, int poly, int invert)
{
    int rv = 0;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, module);

    BCM_LOG_CLI("PRBS module %x; sys_port %d; action %d; dir %d; poly %d; invert %d\n",
            module, sys_port, action, dir, poly, invert);
    rv = bcm_do_prbs(pc, sys_port, action, dir, poly, invert);

    return rv;
}

