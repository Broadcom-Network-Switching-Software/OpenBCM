/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cmicx_link.h
 * Purpose:     cmicx m0 linkscan management
 */

#ifndef __CMICX_LINK_H__
#define __CMICX_LINK_H__
#include <bcm/types.h>
#if defined(BCM_CMICX_SUPPORT) || defined(CMICX_FW_BUILD)
#include <shared/cmicfw/iproc_mbox.h>
#include <shared/cmicfw/cmicx_link_defs.h>

#define CMICX_LS_ALIVE    1
#define CMICX_LS_NOTALIVE 2

/* CMICx Linkscan shared memory define. */
#define CMICX_LS_SHMEM_OFFSET_DEFAULT     (512)
#define CMICX_LS_SHMEM_SIZE               (64)

typedef enum ls_msgtype_e {
   LS_HW_CONFIG = 1,
   LS_HW_HEARTBEAT,
   LS_PAUSE,
   LS_CONTINUE,
   LS_LINK_STATUS_CHANGE
} ls_msgtype_t;

typedef struct soc_ls_heartbeat_s {
    uint32 m0_fw_version;
    uint32 host_fw_version;
}__attribute__ ((packed)) soc_ls_heartbeat_t;

#define CMICX_LS_M0FW_SCHAN_DEF     3
#define CMICX_LS_M0FW_SCHAN_MAGIC   0xAFBECD

#ifndef CMICX_FW_BUILD
typedef int (*soc_iproc_linkscan_sm_t)(soc_iproc_mbox_info_t *chan, soc_iproc_mbox_msg_t *msg);
extern int soc_cmicx_linkscan_hw_init(int unit);
extern int soc_cmicx_linkscan_hw_deinit(int unit);
extern int soc_cmicx_linkscan_config(int unit, soc_pbmp_t *hw_mii_pbm);
extern int soc_cmicx_linkscan_heartbeat(int unit, soc_ls_heartbeat_t *ls_heartbeat);
extern int soc_cmicx_linkscan_hw_link_get(int unit, soc_pbmp_t *hw_link);
extern int soc_cmicx_linkscan_pause(int unit);
extern int soc_cmicx_linkscan_continue(int unit);
extern int soc_cmicx_link_fw_config_init(int unit);
extern void cmicx_esw_linkscan_hw_interrupt(int unit);
extern void cmicx_common_linkscan_hw_interrupt(int unit);
extern int soc_cmicx_linkscan_hw_link_cache_get(int unit, soc_pbmp_t *hw_link);
extern int soc_cmicx_linkscan_dump(int unit);
#endif
#endif /*defined(BCM_CMICX_SUPPORT) || defined(CMICX_FW_BUILD)*/

/*
   Define a maximun physical port number for link scan
   Reserve some headroom for future
*/
#define LS_PHY_PBMP_PORT_MAX     384

#define LS_PHY_PBMP_WORD_MAX ((LS_PHY_PBMP_PORT_MAX + 31) / 32)

typedef struct ls_phy_pbmp_s {
   uint32  pbits[LS_PHY_PBMP_WORD_MAX];
} ls_phy_pbmp_t;

#define LS_PHY_PBMP_WORD_GET(bm, word)        ((bm).pbits[(word)])
#define LS_PHY_PBMP_WORD_SET(bm, word, val)   ((bm).pbits[(word)] = (val))
#define LS_PHY_PBMP_PORT_ADD(bm, port)  do { \
        if ((port) < LS_PHY_PBMP_PORT_MAX) { \
            (LS_PHY_PBMP_WORD_GET((bm), ((port)/32)) |= (1 << ((port)%32))); \
        } \
    } while(0)

#define LS_PHY_PBMP_PORT_REMOVE(bm, port)  do { \
        if ((port) < LS_PHY_PBMP_PORT_MAX) { \
            (LS_PHY_PBMP_WORD_GET((bm), ((port)/32)) &= ~((uint32) (1 << ((port)%32)))); \
        } \
    } while(0)

#define LS_PHY_PBMP_CLEAR(bm)           do { \
        int _w; \
        for (_w = 0; _w < LS_PHY_PBMP_WORD_MAX; _w++) { \
            LS_PHY_PBMP_WORD_SET(bm, _w, 0); \
        } \
    } while(0)

#define LS_PHY_PBMP_MEMBER(bm, port)  \
    (LS_PHY_PBMP_WORD_GET((bm), ((port)/32)) & (1 << ((port)%32)))

#define LS_PHY_PBMP_ITER(bm, port)    \
    for ((port) = 0; (port) < LS_PHY_PBMP_PORT_MAX; (port)++) \
        if (LS_PHY_PBMP_MEMBER((bm), (port)))

#endif /* __CMICX_LINK_H__ */
