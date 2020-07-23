/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       phy8806x_pe_api.c
 */


#include <soc/error.h>
#include <phymod/phymod.h>
#include <phymod/phymod_util.h>
#include <soc/phy/phymod_ctrl.h>
#include "phy8806x_xmod_api.h"


extern int
phy_8806x_xmod_command(int unit, int port, int mode_opcode,  uint32_t arg[], int arg_size, uint32_t result[], int result_size);

#ifdef PHYMOD_PHY8806X_SUPPORT

#define XMOD_CMD_MODE_CORE(cmd)  (cmd)
#define XMOD_CMD_MODE_ETH(cmd)   (0x100 | cmd)
#define XMOD_CMD_MODE_FC(cmd)    (0x200 | cmd)
#define XMOD_CMD_MODE_CPRI(cmd)  (0x300 | cmd)


/* ! Byte swap unsigned short */
uint16_t swap_uint16( uint16_t val ) 
{
    return (val << 8) | (val >> 8 );
}

/* ! Byte swap short */
int16_t swap_int16( int16_t val ) 
{
    return (val << 8) | ((val >> 8) & 0xFF);
}

/* ! Byte swap unsigned int */
uint32_t swap_uint32( uint32_t val )
{
#ifdef BE_HOST
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | (val >> 16);
#else
    return val;
#endif
}

/* ! Byte swap int */
int32_t swap_int32( int32_t val )
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | ((val >> 16) & 0xFFFF);
}

/* ! Byte swap unsigned int */
uint64_t swap_uint64( uint64_t val )
{
#ifdef BE_HOST
    uint32_t val0 = COMPILER_64_HI(val);
    uint32_t val1 = COMPILER_64_LO(val);
    uint64_t ret_val;
    COMPILER_64_SET(ret_val, swap_uint32(val1), swap_uint32(val0));
    return (ret_val);
#else
    return val;
#endif
}

int pe_xmod_pe_init(int unit, int port, xmod_pe_config_t *config)
{
#ifdef INCLUDE_PHY_8806X
    xmod_pe_config_t x_config;
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen, rv, idx;

    LOG_CLI((BSL_META_U(0, "%s() enter; unit %d; port %d\n"), __FUNCTION__, unit, port));

    /* setting up xmod xmod_pe_init: */
    /* int xmod_pe_init(IN xmod_pe_config_t *config)  */

    LOG_CLI((BSL_META_U(0, "%s() mode %d\n"), __FUNCTION__, config->mode));
    LOG_CLI((BSL_META_U(0, "%s() tag %d\n"), __FUNCTION__, config->tag));
    LOG_CLI((BSL_META_U(0, "%s() pfc %d\n"), __FUNCTION__, config->pfc));

     /* Convert to Little endian */
    x_config.mode = config->mode;
    x_config.tag = config->tag;
    x_config.pfc = config->pfc;
    x_config.res = config->res;
    for (idx=0; idx<4; idx++) {
        x_config.pcid[idx] = swap_uint16(config->pcid[idx]);
    }

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_config, sizeof(xmod_pe_config_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PE_INIT);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, 
                                XMOD_CMD_MODE_ETH(XMOD_PE_INIT),
                                xmodtxbuff, xmodtxlen, NULL, 0);
    if (SOC_FAILURE(rv)) {
        LOG_CLI((BSL_META_U(0, "%s() phy_8806x_xmod_command() error %d\n"), __FUNCTION__, rv));
        return SOC_E_FAIL;
    }

    return rv;
#else
    return PHYMOD_E_NONE;
#endif
}

int pe_xmod_pe_status_get(int unit, int port, xmod_pe_status_t *status)
{
#ifdef INCLUDE_PHY_8806X
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *bufrxptr;
    int xmodrxlen, rv;

    LOG_CLI((BSL_META_U(0, "%s() enter; unit %d; port %d\n"), __FUNCTION__, unit, port));

    /* setting up xmod xmod_pe_status_get:      */
    /* int xmod_pe_status_get(OUT xmod_pe_status_t status) */

    /* setup out args */
    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PE_STATUS_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port,
                                XMOD_CMD_MODE_ETH(XMOD_PE_STATUS_GET),
                                NULL, 0, xmodrxbuff, xmodrxlen);
    if (SOC_FAILURE(rv)) {
        LOG_CLI((BSL_META_U(0, "%s() phy_8806x_xmod_command() error %d\n"), __FUNCTION__, rv));
        return SOC_E_FAIL;
    }

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, status, sizeof(xmod_pe_status_t));

    LOG_CLI((BSL_META_U(0, "%s() up_port:  res %d; link %d\n"), __FUNCTION__, status->up_port.res, status->up_port.link_up));
    LOG_CLI((BSL_META_U(0, "%s() pe_port0: res %d; link %d\n"), __FUNCTION__, status->pe_port0.res, status->pe_port0.link_up));
    LOG_CLI((BSL_META_U(0, "%s() pe_port1: res %d; link %d\n"), __FUNCTION__, status->pe_port1.res, status->pe_port1.link_up));
    LOG_CLI((BSL_META_U(0, "%s() pe_port2: res %d; link %d\n"), __FUNCTION__, status->pe_port2.res, status->pe_port2.link_up));
    LOG_CLI((BSL_META_U(0, "%s() pe_port3: res %d; link %d\n"), __FUNCTION__, status->pe_port3.res, status->pe_port3.link_up));

    return rv;
#else
    return PHYMOD_E_NONE;
#endif
}

int pe_xmod_pe_port_stats_get(int unit, int port, int pe_port, xmod_pe_port_stats_t *stats, int reset)
{
#ifdef INCLUDE_PHY_8806X
    uint8_t x_port = (uint8_t)pe_port;
    uint8_t x_reset = (uint8_t)reset;
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen;
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *bufrxptr;
    int xmodrxlen;
    int rv;

    LOG_CLI((BSL_META_U(0, "%s() enter; unit %d; port %d\n"), __FUNCTION__, unit, port));

    /* setting up xmod xmod_pe_port_stats_get:                              */
    /* int xmod_pe_port_stats_get(IN uint8_t port, OUT xmod_pe_port_stats_t stats) */

    /* setup in args - write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_port, sizeof(uint8_t));
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_reset, sizeof(uint8_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PE_PORT_STATS_GET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* setup out args */
    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PE_PORT_STATS_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port,
                                XMOD_CMD_MODE_ETH(XMOD_PE_PORT_STATS_GET),
                                xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);
    if (SOC_FAILURE(rv)) {
        LOG_CLI((BSL_META_U(0, "%s() phy_8806x_xmod_command() error %d\n"), __FUNCTION__, rv));
        return SOC_E_FAIL;
    }

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
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

    LOG_CLI((BSL_META_U(0, "%s(%d) port_dn_cnt %x\n"),
                __FUNCTION__, x_port, stats->port_dn_cnt));

    LOG_CLI((BSL_META_U(0, "%s(%d) rpkt 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->rpkt), COMPILER_64_LO(stats->rpkt)));
    LOG_CLI((BSL_META_U(0, "%s(%d) rbyt 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->rbyt), COMPILER_64_LO(stats->rbyt)));
    LOG_CLI((BSL_META_U(0, "%s(%d) tpkt 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->tpkt), COMPILER_64_LO(stats->tpkt)));
    LOG_CLI((BSL_META_U(0, "%s(%d) tbyt 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->tbyt), COMPILER_64_LO(stats->tbyt)));

    LOG_CLI((BSL_META_U(0, "%s(%d) rfcs 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->rfcs), COMPILER_64_LO(stats->rfcs)));
    LOG_CLI((BSL_META_U(0, "%s(%d) rfrg 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->rfrg), COMPILER_64_LO(stats->rfrg)));
    LOG_CLI((BSL_META_U(0, "%s(%d) rjbr 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->rjbr), COMPILER_64_LO(stats->rjbr)));
    LOG_CLI((BSL_META_U(0, "%s(%d) rovr 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->rovr), COMPILER_64_LO(stats->rovr)));
    LOG_CLI((BSL_META_U(0, "%s(%d) rrpkt 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->rrpkt), COMPILER_64_LO(stats->rrpkt)));
    LOG_CLI((BSL_META_U(0, "%s(%d) raln 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->raln), COMPILER_64_LO(stats->raln)));
    LOG_CLI((BSL_META_U(0, "%s(%d) rerpkt 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->rerpkt), COMPILER_64_LO(stats->rerpkt)));
    LOG_CLI((BSL_META_U(0, "%s(%d) rmtue 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->rmtue), COMPILER_64_LO(stats->rmtue)));
    LOG_CLI((BSL_META_U(0, "%s(%d) rprm 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->rprm), COMPILER_64_LO(stats->rprm)));
    LOG_CLI((BSL_META_U(0, "%s(%d) rschcrc 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->rschcrc), COMPILER_64_LO(stats->rschcrc)));
    LOG_CLI((BSL_META_U(0, "%s(%d) rtrfu 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->rtrfu), COMPILER_64_LO(stats->rtrfu)));
    LOG_CLI((BSL_META_U(0, "%s(%d) rfcr 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->rfcr), COMPILER_64_LO(stats->rfcr)));
    LOG_CLI((BSL_META_U(0, "%s(%d) rxpf 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->rxpf), COMPILER_64_LO(stats->rxpf)));
    LOG_CLI((BSL_META_U(0, "%s(%d) rflr 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->rflr), COMPILER_64_LO(stats->rflr)));

    LOG_CLI((BSL_META_U(0, "%s(%d) tfcs 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->tfcs), COMPILER_64_LO(stats->tfcs)));
    LOG_CLI((BSL_META_U(0, "%s(%d) tfrg 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->tfrg), COMPILER_64_LO(stats->tfrg)));
    LOG_CLI((BSL_META_U(0, "%s(%d) tjbr 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->tjbr), COMPILER_64_LO(stats->tjbr)));
    LOG_CLI((BSL_META_U(0, "%s(%d) tncl 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->tncl), COMPILER_64_LO(stats->tncl)));
    LOG_CLI((BSL_META_U(0, "%s(%d) tovr 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->tovr), COMPILER_64_LO(stats->tovr)));
    LOG_CLI((BSL_META_U(0, "%s(%d) trpkt 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->trpkt), COMPILER_64_LO(stats->trpkt)));
    LOG_CLI((BSL_META_U(0, "%s(%d) txpf 0x%08x%08x\n"), __FUNCTION__, x_port,
                COMPILER_64_HI(stats->txpf), COMPILER_64_LO(stats->txpf)));

    return rv;
#else
    return PHYMOD_E_NONE;
#endif
}

int pe_xmod_pe_pcid_set(int unit, int port, xmod_pe_pcid_t *pcids)
{
#ifdef INCLUDE_PHY_8806X
    xmod_pe_pcid_t x_pcids;
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen, rv;

    LOG_CLI((BSL_META_U(0, "%s() enter; unit %d; port %d\n"), __FUNCTION__, unit, port));

    /* setting up xmod xmod_pe_pcid_set: */
    /* int xmod_pe_pcid_set(IN xmod_pe_pcid_t pcid)   */

    LOG_CLI((BSL_META_U(0, "%s() pcid0 %d\n"), __FUNCTION__, pcids->pcid0));
    LOG_CLI((BSL_META_U(0, "%s() pcid1 %d\n"), __FUNCTION__, pcids->pcid1));
    LOG_CLI((BSL_META_U(0, "%s() pcid2 %d\n"), __FUNCTION__, pcids->pcid2));
    LOG_CLI((BSL_META_U(0, "%s() pcid3 %d\n"), __FUNCTION__, pcids->pcid3));
    x_pcids.pcid0 = swap_uint16(pcids->pcid0);
    x_pcids.pcid1 = swap_uint16(pcids->pcid1);
    x_pcids.pcid2 = swap_uint16(pcids->pcid2);
    x_pcids.pcid3 = swap_uint16(pcids->pcid3);

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_pcids, sizeof(xmod_pe_pcid_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PE_PCID_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, 
                                XMOD_CMD_MODE_ETH(XMOD_PE_PCID_SET),
                                xmodtxbuff, xmodtxlen, NULL, 0);
    if (SOC_FAILURE(rv)) {
        LOG_CLI((BSL_META_U(0, "%s() phy_8806x_xmod_command() error %d\n"), __FUNCTION__, rv));
        return SOC_E_FAIL;
    }

    return rv;
#else
    return PHYMOD_E_NONE;
#endif
}

int pe_xmod_pe_fw_ver_get(int unit, int port, xmod_pe_fw_ver_t *ver)
{
#ifdef INCLUDE_PHY_8806X
    xmod_pe_fw_ver_t x_ver;
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *bufrxptr;
    int xmodrxlen, rv;

    LOG_CLI((BSL_META_U(0, "%s() enter; unit %d; port %d\n"), __FUNCTION__, unit, port));

    /* setting up xmod xmod_pe_fw_ver_get:      */
    /* int xmod_pe_fw_ver_get(OUT xmod_pe_fw_ver_t fw_ver) */

    /* setup out args */
    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PE_FW_VER_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port,
                                XMOD_CMD_MODE_ETH(XMOD_PE_FW_VER_GET),
                                NULL, 0, xmodrxbuff, xmodrxlen);
    if (SOC_FAILURE(rv)) {
        LOG_CLI((BSL_META_U(0, "%s() phy_8806x_xmod_command() error %d\n"), __FUNCTION__, rv));
        return SOC_E_FAIL;
    }

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_ver, sizeof(xmod_pe_fw_ver_t));
    ver->major = swap_uint16(x_ver.major);
    ver->minor = swap_uint16(x_ver.minor);
    ver->build = swap_uint16(x_ver.build);

    LOG_CLI((BSL_META_U(0, "%s() major %d\n"), __FUNCTION__, ver->major));
    LOG_CLI((BSL_META_U(0, "%s() minor %d\n"), __FUNCTION__, ver->minor));
    LOG_CLI((BSL_META_U(0, "%s() build %d\n"), __FUNCTION__, ver->build));

    return rv;
#else
    return PHYMOD_E_NONE;
#endif
}

int pe_xmod_pe_test(int unit, int port)
{
    xmod_pe_config_t    config;
    xmod_pe_status_t    status;
    xmod_pe_port_stats_t stats;
    xmod_pe_pcid_t      pcids;
    xmod_pe_fw_ver_t    ver;
    int                 pe_port;
    int                 rc;

    config.mode = XMOD_PE_MODE_4_PORTS;
    config.tag = XMOD_PE_TAG_BR;
    config.pfc = 1;
    config.res = 0;
    config.pcid[0] = 1;
    config.pcid[1] = 2;
    config.pcid[2] = 3;
    config.pcid[3] = 4;
    LOG_CLI((BSL_META_U(0, "%s() unit %d port %d\n"), __FUNCTION__, unit, port));
    LOG_CLI((BSL_META_U(0, "%s() calling pe_xmod_pe_init()\n"), __FUNCTION__));
    rc = pe_xmod_pe_init(unit, port, &config);
    if (SOC_FAILURE(rc)) {
        LOG_CLI((BSL_META_U(0, "%s() pe_xmod_pe_init() error %d\n"), __FUNCTION__, rc));
    }
        
    LOG_CLI((BSL_META_U(0, "%s() calling pe_xmod_pe_status_get()\n"), __FUNCTION__));
    rc = pe_xmod_pe_status_get(unit, port, &status);
    if (SOC_FAILURE(rc)) {
        LOG_CLI((BSL_META_U(0, "%s() pe_xmod_pe_status_get() error %d\n"), __FUNCTION__, rc));
    }

    for (pe_port=0; pe_port<=4; pe_port++) {
        LOG_CLI((BSL_META_U(0, "%s() calling pe_xmod_pe_port_stats_get()\n"), __FUNCTION__));
        rc = pe_xmod_pe_port_stats_get(unit, port, pe_port, &stats, 0);
        if (SOC_FAILURE(rc)) {
            LOG_CLI((BSL_META_U(0, "%s() pe_xmod_pe_status_get() error %d\n"), __FUNCTION__, rc));
        }
    }

    pcids.pcid0 = 0x0123;
    pcids.pcid1 = 0x4567;
    pcids.pcid2 = 0x89ab;
    pcids.pcid3 = 0xcdef;
    LOG_CLI((BSL_META_U(0, "%s() calling pe_xmod_pe_pcid_set()\n"), __FUNCTION__));
    rc = pe_xmod_pe_pcid_set(unit, port, &pcids);
    if (SOC_FAILURE(rc)) {
        LOG_CLI((BSL_META_U(0, "%s() pe_xmod_pe_pcid_set() error %d\n"), __FUNCTION__, rc));
    }

    LOG_CLI((BSL_META_U(0, "%s() calling pe_xmod_pe_fw_ver_get()\n"), __FUNCTION__));
    rc = pe_xmod_pe_fw_ver_get(unit, port, &ver);
    if (SOC_FAILURE(rc)) {
        LOG_CLI((BSL_META_U(0, "%s() pe_xmod_pe_fw_ver_get() error %d\n"), __FUNCTION__, rc));
    }

    return PHYMOD_E_NONE;
}

#endif /* PHYMOD_PHY8806X_SUPPORT */
