/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    phy8040.c
 * Purpose: Phy Driver supports for Broadcom 8040 multiplexer/retimer
 * Note:    The driver is written in a way to support the mux configuration.
 *          It always assumes one port connects to the switch side.
 *          other three ports (called mux port) may or may not connect to a phy
 *          device on the line side. 
 *          Only one link(switch side port <---> line side port) is active in
 *          any given time. 
 */
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_8040)
#include "phyconfig.h"    /* Must be the first phy include after phydefs.h */

#include <sal/types.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "phyxehg.h"
#include "phy8040.h"

#define MAX_MPORTS      3  /* max number of mux ports on the device*/
#define MAX_8040_PORTS  4  /* number of total xgxs ports on the device */
#define PHY8040_MUX_PORT_INVALID   0x7f
#define PHY_ADDR_NONE  0xFF
#define PHY87X6_ID1_REVA   0x6034
#define PHY87X6_ID1_REVB   0x6035
#define PHY87X6_ID0        0x20

/* 8040 device specific data descriptor. */ 
#define PHY8040_DEV_DESC(pc)  ((phy8040_ctrl_t *)((pc)->driver_data))
 
#define PHYDRV_CALL_NOARG(pc,dc,name,rv) \
    do { \
       if ((dc)->lp[(dc)->inx].nxt_pc) { \
	phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE((pc)->unit, (pc)->port); \
	EXT_PHY_SW_STATE((pc)->unit,(pc)->port)=(dc)->lp[(dc)->inx].nxt_pc; \
        rv = name((dc)->lp[(dc)->inx].nxt_pc->pd,(pc)->unit,(pc)->port); \
	EXT_PHY_SW_STATE((pc)->unit, (pc)->port) = tmp_pc; \
       } \
    } while(0)

#define PHYDRV_CALL_ARG1(pc,dc,name,arg0,rv) \
    do { \
       if ((dc)->lp[(dc)->inx].nxt_pc) { \
        phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE((pc)->unit, (pc)->port); \
        EXT_PHY_SW_STATE((pc)->unit,(pc)->port)=(dc)->lp[(dc)->inx].nxt_pc; \
        rv = name((dc)->lp[(dc)->inx].nxt_pc->pd,(pc)->unit,(pc)->port,arg0); \
        EXT_PHY_SW_STATE((pc)->unit, (pc)->port) = tmp_pc; \
       } \
    } while(0)

#define PHYDRV_CALL_ARG2(pc,dc,name,arg0,arg1,rv) \
    do { \
       if ((dc)->lp[(dc)->inx].nxt_pc) { \
        phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE((pc)->unit, (pc)->port); \
        EXT_PHY_SW_STATE((pc)->unit,(pc)->port)=(dc)->lp[(dc)->inx].nxt_pc; \
        rv = name((dc)->lp[(dc)->inx].nxt_pc->pd,(pc)->unit,(pc)->port, \
			arg0,arg1); \
        EXT_PHY_SW_STATE((pc)->unit, (pc)->port) = tmp_pc; \
       } \
    } while(0)

#define PHYDRV_CALL_ARG3(pc,dc,name,arg0,arg1,arg2,rv) \
    do { \
       if ((dc)->lp[(dc)->inx].nxt_pc) { \
        phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE((pc)->unit, (pc)->port); \
        EXT_PHY_SW_STATE((pc)->unit,(pc)->port)=(dc)->lp[(dc)->inx].nxt_pc; \
        rv = name((dc)->lp[(dc)->inx].nxt_pc->pd,(pc)->unit,(pc)->port, \
                        arg0,arg1,arg2); \
        EXT_PHY_SW_STATE((pc)->unit, (pc)->port) = tmp_pc; \
       } \
    } while(0)

/* mux_port_desc_t describes each mux port configuration. The value of each 
 * element must match the actual hardware configuration. For example, if the
 * mux port 0 connects a 8726 phy device with the mdio address 0x56, then the
 * port should be 0 and phy_addr should be 0x56.
 * The port is configured through spn_PHY_8040_MUX_PORT[x], x=0-2. 
 * The phy_addr is configured through spn_PORT_PHY_ADDR1.
 * Make sure all the configuration values match up the actual hardware configuration.
 */ 
typedef struct {
    phy_ctrl_t *nxt_pc;  /* the driver to handle the connected phy device */
    uint16 port;             /* the 8040 port connecting to the phy device */
    uint8  phy_addr;         /* the mdio address of the phy device */
} mux_port_desc_t;
 
/* device specific info descriptor */

typedef struct {
    /* The logical port map. Array index represents the logic port number. 
     * Each entry describes each mux port configuration. See comment on
     * mux_port_desc_t. 
     */ 
    mux_port_desc_t lp[MAX_MPORTS];
    uint16 switch_port;     /* switch port */
    uint16 num_mports;      /* number of mux port used */
    uint8 devids[MAX_8040_PORTS];   /* device ids, index is physical port number */
    uint8 inx;    /* active logic port number */
    uint8 sp_10g[MAX_MPORTS]; /* logical port speed */ 
} phy8040_ctrl_t;

/* default MDIO addresses of phy devices on the mux ports, per port based*/
static uint8 ext_phy_dft_addr1[][MAX_MPORTS] = {
    /* first phy     second phy     third phy       */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 0  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 1  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 2  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 3  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 4  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 5  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 6  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 7  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 8  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 9  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 10  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 11  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 12  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 13  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 14  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 15  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 16  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 17  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 18  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 19  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 20  */
    {0x56,          PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 21  */
    {0x57,          PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 22  */
    {0x58,          PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 23  */
    {0x59,          PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 24  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 25  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 26  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 27  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 28  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 29  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 30  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 31  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 32  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 33  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 34  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 35  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 36  */
    {PHY_ADDR_NONE, PHY_ADDR_NONE, PHY_ADDR_NONE},  /* 37  */
};

#define MAX_DFT_ADDR_ENTRY \
  ((sizeof(ext_phy_dft_addr1)/sizeof(ext_phy_dft_addr1[0][0]))/MAX_MPORTS)

/* local function prototypes */
STATIC int _phy8040_speed_set(int unit, soc_port_t port, int speed);
STATIC int _phy8040_speed_get(int unit, soc_port_t port, int *speed);


static int
_phy_8040_mux(int unit, soc_port_t port)
{
    int              switch_port;
    int              active_mport;
    phy8040_ctrl_t * dc;
    phy_ctrl_t     *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
                                                                                
    dc = PHY8040_DEV_DESC(pc);

    active_mport = dc->lp[dc->inx].port;
    switch_port  = dc->switch_port;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "8040: u=%d port=%d primary=%d secondary=%d.\n"), 
                         pc->unit, pc->port, switch_port, active_mport));

    /* Power down other two blocks. */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8040_PWR_CTRLr(pc->unit, pc, 
                            (PHY8040_PWRCTRL_RSVD| 
                             (0xf & 
                              ~((1<<switch_port)|
                                (1<<active_mport))))));
    
    /* Set source for primary <---> secondary ports */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8040_XGXSn_CTRLr(pc->unit, pc, switch_port,
                               PHY8040_XGXS_CTRL_SRC(active_mport),
                               PHY8040_XGXS_CTRL_SRC(0xf)));
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8040_XGXSn_CTRLr(pc->unit, pc, active_mport,
                               PHY8040_XGXS_CTRL_SRC(switch_port),
                               PHY8040_XGXS_CTRL_SRC(0xf)));
    
    /* Soft reset primary & secondary ports */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8040_XGXSn_REG(pc->unit, pc, dc->devids[switch_port],
                             MII_CTRL_REG, MII_CTRL_RESET, MII_CTRL_RESET));
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8040_XGXSn_REG(pc->unit, pc, dc->devids[active_mport],
                             MII_CTRL_REG, MII_CTRL_RESET, MII_CTRL_RESET));

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8040_init
 * Purpose:    
 *    Initialize 8040 phys. Configure each XGXS block with a unique device ID.
 *    The MDIO addressing is to use a single MDIO address and a different device
 *    id for each of the five device blocks. The top block's device ID is hardwired.
 *    The driver is written to only support the MDIO clause 45 protocol.
 *    Set up a valid link(switch port <---> mux port) and disable the unused mux 
 *    ports.
 * 
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 * Returns:    
 *    SOC_E_NONE
 */
STATIC int
phy_8040_init(int unit, soc_port_t port)
{
    phy8040_ctrl_t * dc;
    phy8040_ctrl_t * new_dc;
    int i;
    int rv = SOC_E_NONE;
    phy_ctrl_t     *pc;
    phy_ctrl_t *new_pc;
    uint32 mdio_adrs;
    void *tmp_holder;  /* temporary place holder */
    int size;
    int num_pc;
    soc_phy_info_t phy_info;
    extern int _ext_phy_probe(int unit, soc_port_t port,soc_phy_info_t *pi,
                              phy_ctrl_t *ext_pc);

    pc = EXT_PHY_SW_STATE(unit, port);

    /* allocate a temp storage to hold fixed number of control structures */
    size = sizeof(phy8040_ctrl_t) + MAX_MPORTS * sizeof (phy_ctrl_t);
    tmp_holder = sal_alloc(size, "PHY8040");
    if (tmp_holder == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(tmp_holder, 0, size);

    dc = (phy8040_ctrl_t *)tmp_holder;    
 
    dc->devids[0] =PHY8040_C45_DEV_XGXS0;
    dc->devids[1] =PHY8040_C45_DEV_XGXS1;
    dc->devids[2] =PHY8040_C45_DEV_XGXS2;
    dc->devids[3] =PHY8040_C45_DEV_XGXS3;

    dc->switch_port   = soc_property_port_get(pc->unit, pc->port,
                                           spn_PHY_8040_SWITCH_PORT,
                                           PHY8040_DFLT_SWITCH_PORT);
    dc->lp[0].port = soc_property_port_get(pc->unit, pc->port,
                                           spn_PHY_8040_MUX_PORT0,
                                           PHY8040_DFLT_MUX_PORT0);
    dc->lp[1].port = soc_property_port_get(pc->unit, pc->port,
                                           spn_PHY_8040_MUX_PORT1,
                                           PHY8040_DFLT_MUX_PORT1);
    dc->lp[2].port = soc_property_port_get(pc->unit, pc->port,
                                           spn_PHY_8040_MUX_PORT2,
                                           PHY8040_MUX_PORT_INVALID);

    /* spn_PORT_PHY_ADDR1 configures 3 additional MDIO addresses for
     * the mux port with 8040 type phy 
     * bit       31 -- 24 23 -- 16 15 -- 8 7 -- 0
     * mdioAdrs           phy 3    phy 2   phy 1
     * phy 1 --> mux port 0
     * phy 2 --> mux port 1
     * phy 3 --> mux port 2
     */ 
    mdio_adrs = soc_property_port_get(pc->unit, pc->port,
                                           spn_PORT_PHY_ADDR1, 0);

    for (i = 0; i < MAX_MPORTS; i++) {
        if ((mdio_adrs >> (i * 8)) & 0xFF) {  /* use configured value */
            dc->lp[i].phy_addr = (mdio_adrs >> (i * 8)) & 0xFF;
        } else { /* use default value */
            if (pc->port < MAX_DFT_ADDR_ENTRY) {
                dc->lp[i].phy_addr = ext_phy_dft_addr1[pc->port][i];
            }
        }
    }
    
    for (i = 0; i < MAX_MPORTS; i++) {
        if (dc->lp[i].port == PHY8040_MUX_PORT_INVALID) {
            break;
        }
    }
    dc->num_mports = i;
    
    /* probe the mux ports for the connected phys. */

    new_pc = (phy_ctrl_t *)(dc + 1);
    for (num_pc = 0,i = 0; i < dc->num_mports; i++) {
        new_pc->unit = unit;
        new_pc->port = port;
        new_pc->speed_max = pc->speed_max;
        new_pc->read = pc->read; /* use same MDIO read  as this device's */
        new_pc->write = pc->write; /*use same MDIO write as this device's */
        new_pc->phy_id = dc->lp[i].phy_addr;

        /* probe the next device at the given address */
        SOC_IF_ERROR_RETURN
            (_ext_phy_probe(unit, port, &phy_info, new_pc));

        if (new_pc->pd != NULL) {
            dc->lp[i].nxt_pc = new_pc;
            num_pc += 1;
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_8040_init: found next phy device"
                                 " u=%d p=%d id0=0x%x id1=0x%x\n"),
                      unit, port,new_pc->phy_id0,new_pc->phy_id1));
        }
        new_pc += 1;
    }

    /* device probe done, allocate memory for control structures */

    size = sizeof(phy8040_ctrl_t) + num_pc * sizeof (phy_ctrl_t);
    pc->driver_data = (void *)sal_alloc (size, pc->pd->drv_name);
    if (pc->driver_data == NULL) {
        sal_free(tmp_holder);
        return SOC_E_MEMORY;
    }

    /* copy control descriptors from temporary storage */
    new_dc = (phy8040_ctrl_t *)pc->driver_data;
    sal_memcpy(new_dc, dc, sizeof(phy8040_ctrl_t));
    new_pc = (phy_ctrl_t *)(new_dc + 1); 
    for (i = 0; i < dc->num_mports; i++) {
        if (dc->lp[i].nxt_pc) {
            sal_memcpy(new_pc, dc->lp[i].nxt_pc, sizeof(phy_ctrl_t));
            new_dc->lp[i].nxt_pc = new_pc;
            new_pc += 1;
        }
    }

    /* done copy, free temp storage */
    sal_free(tmp_holder);

    PHY_FLAGS_SET(pc->unit, pc->port,  PHY_FLAGS_FIBER | PHY_FLAGS_C45);

    /* Set our 4 internal XGXS block phy and dev addresses */
    for (i=0; i<MAX_8040_PORTS; i++)
      {
      SOC_IF_ERROR_RETURN
          (MODIFY_PHY8040_XGXSn_CTRLr(pc->unit, pc, i,
                    PHY8040_XGXS_CTRL_DEVID(new_dc->devids[i])|
                    PHY8040_XGXS_CTRL_PRTAD(pc->phy_id),
                    PHY8040_XGXS_CTRL_DEVID(0x1f)|
                    PHY8040_XGXS_CTRL_PRTAD(0x1f)));
      }

    /* determine the default active phy_ctrl desc */
    new_dc->inx = 0;
    
    _phy_8040_mux(pc->unit,pc->port);

    /* all logical ports default speed to 10G */
    for (i = 0; i < new_dc->num_mports; i++) {
        new_dc->sp_10g[i] = TRUE;
    }
    PHYDRV_CALL_NOARG(pc,new_dc,PHY_INIT,rv);
    return rv;
}


/*
 * Function:
 *    phy_8040_link_get
 * Purpose:
 *    Get layer2 connection status.
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 *    link - address of memory to store link up/down state.
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8040_link_get(int unit, soc_port_t port, int *link)
{
    int              switch_port;
    int              active_mport;
    uint16           switch_port_stat, mux_port_stat;
    uint16           sp_sync_stat, mp_sync_stat,rx_status;
    phy8040_ctrl_t * dc;
    phy_ctrl_t     *pc;
    int rv = SOC_E_NONE;
    int an = 0;
    int an_done = 0;
    int speed = 0;
    int speed_local = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
                                                                                
    dc = PHY8040_DEV_DESC(pc);

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    switch_port = dc->switch_port;
    active_mport = dc->lp[dc->inx].port;

    *link=FALSE;

    if (NULL != dc->lp[dc->inx].nxt_pc) {
    /* When the connected phy is in AN mode, its negotiated speed needs to be 
     * propagated to this device. Sync up here since this function is periodically
     * called by a dedicated linkscan task.  
     */
        PHYDRV_CALL_ARG1(pc,dc,PHY_LINK_GET,link,rv);
   
        if (*link == TRUE) {
            /* check if in an mode */ 
            PHYDRV_CALL_ARG2(pc,dc,PHY_AUTO_NEGOTIATE_GET,&an,&an_done,rv);
     
            if (an == TRUE && an_done == TRUE) {
                /* get the speed */
                PHYDRV_CALL_ARG1(pc,dc,PHY_SPEED_GET,&speed,rv);
                _phy8040_speed_get(unit, port, &speed_local);
      
                if (speed && (speed != speed_local)) {
                    /* set this device's speed accordingly */
                    _phy8040_speed_set(unit,port,speed);
                }
            }
        } else {
            return rv;
        }
    }

    if (dc->sp_10g[dc->inx] == FALSE) {
        /* check 1G linkup status */
        SOC_IF_ERROR_RETURN
            (READ_PHY8040_XGXSn_REG(pc->unit, pc, dc->devids[active_mport],
                             PHY8040_RX_STATUS_REG,&rx_status));

        if ((rx_status & PHY8040_1G_LINKUP_MASK) == PHY8040_1G_LINKUP_MASK) {
            *link=TRUE;
        }
        SOC_IF_ERROR_RETURN
            (READ_PHY8040_XGXSn_REG(pc->unit, pc, dc->devids[switch_port],
                             PHY8040_RX_STATUS_REG,&rx_status));
        if ((rx_status & PHY8040_1G_LINKUP_MASK) == PHY8040_1G_LINKUP_MASK) {
            *link=TRUE;
        }
    } else {  
       /* check 10G link */
 
        SOC_IF_ERROR_RETURN
            (READ_PHY8040_XGXSn_REG(pc->unit, pc, 
                           dc->devids[switch_port],
                           MII_STAT_REG, &switch_port_stat));
        SOC_IF_ERROR_RETURN
            (READ_PHY8040_XGXSn_REG(pc->unit, pc, 
                           dc->devids[active_mport],
                           MII_STAT_REG, &mux_port_stat));
    
        *link = MII_STAT_LA & (switch_port_stat & mux_port_stat);
    
        SOC_IF_ERROR_RETURN
            (READ_PHY8040_XGXSn_REG(pc->unit, pc, 
                           dc->devids[switch_port],
                           PHY8040_LANE_STATUS_REG, 
                           &sp_sync_stat));
    
        SOC_IF_ERROR_RETURN
            (READ_PHY8040_XGXSn_REG(pc->unit, pc, 
                           dc->devids[active_mport],
                           PHY8040_LANE_STATUS_REG,
                           &mp_sync_stat));
        if (*link) {
            *link &= (((sp_sync_stat & mp_sync_stat) & 0xf) == 0xf) ? 
                  MII_STAT_LA : 0;
            if (!*link) {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "8040: Link set but no sync.\n"
                                     "      u=%d port=%d primary lane status=%X secondary lane"
                                     " status=%X.\n"), 
                          pc->unit, pc->port, sp_sync_stat, mp_sync_stat));
            }
        } else {
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "8040: u=%d port=%d switch port=%X mux port=%X.\n"), 
                         pc->unit, pc->port, switch_port_stat, mux_port_stat));
        }
    }

    /* check the connected device */
    if (*link) {
        PHYDRV_CALL_ARG1(pc,dc,PHY_LINK_GET,link,rv);
    }
    return rv;
}

/*
 * Function:
 *    phy_8040_enable_set
 * Purpose:
 *    Enable/Disable phy 
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 *    enable - on/off state to set
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8040_enable_set(int unit, soc_port_t port, int enable)
{
    phy8040_ctrl_t * dc;
    uint16 data;
    int rv = SOC_E_NONE;
    phy_ctrl_t     *pc;
  
    pc = EXT_PHY_SW_STATE(unit, port);
    dc = PHY8040_DEV_DESC(pc);

    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    data = enable ? 0:MII_CTRL_PD;

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8040_XGXSn_REG(pc->unit, pc, dc->devids[dc->lp[dc->inx].port],
                             MII_CTRL_REG, data, MII_CTRL_PD));

    PHYDRV_CALL_ARG1(pc,dc,PHY_ENABLE_SET,enable,rv);
    
    return (rv);
}

STATIC int
_phy8040_speed_set(int unit, soc_port_t port, int speed)
{
    phy8040_ctrl_t * dc;
    int rv = SOC_E_NONE;
    phy_ctrl_t     *pc;
    uint8 devid[2];
    int i;
 
    pc = EXT_PHY_SW_STATE(unit, port);
    dc = PHY8040_DEV_DESC(pc);

    devid[0] = dc->devids[dc->lp[dc->inx].port];
    devid[1] = dc->devids[dc->switch_port];

    if (speed == 10000) { /* 10G */
        dc->sp_10g[dc->inx] = TRUE;

        /* various register configuration for 10G speed, powerup default values */
        for (i = 0; i < 2; i++) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8040_XGXSn_REG(pc->unit, pc, devid[i], 
                                      PHY8040_TX0A_CTRL1_REG, 0x0dd0));
        
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8040_XGXSn_REG(pc->unit, pc, devid[i], 
                                      PHY8040_XGXS_CTRL_REG,0x202e));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8040_XGXSn_REG(pc->unit, pc, devid[i], 
                                      PHY8040_LANE_CTRL1_REG,0x0000));

            SOC_IF_ERROR_RETURN
                (WRITE_PHY8040_XGXSn_REG(pc->unit, pc, devid[i], 
                                      PHY8040_LANE_CTRL2_REG,0xf000));

            SOC_IF_ERROR_RETURN
                (WRITE_PHY8040_XGXSn_REG(pc->unit, pc, devid[i], 
                                      PHY8040_RX_CTRL_REG,0x1c0a));
        }
    } else if (speed == 1000) {  /* 1G */

        dc->sp_10g[dc->inx] = FALSE;
        /* various register configuration for 1G speed */
        for (i = 0; i < 2; i++) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8040_XGXSn_REG(pc->unit, pc, devid[i], 
                                        PHY8040_TX0A_CTRL1_REG,0x0dd0));
                                                                                     
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8040_XGXSn_REG(pc->unit, pc, devid[i], 
                                        PHY8040_XGXS_CTRL_REG, 0x263a));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8040_XGXSn_REG(pc->unit, pc, devid[i], 
                                        PHY8040_LANE_CTRL1_REG,0xffff));
                                                                                     
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8040_XGXSn_REG(pc->unit, pc, devid[i], 
                                        PHY8040_LANE_CTRL2_REG,0x0000));
                                                                                     
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8040_XGXSn_REG(pc->unit, pc, devid[i], 
                                        PHY8040_RX_CTRL_REG,0x1c00));
        }
    }

    return (rv);
}

/*
 * Function:
 *      phy_8040_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */
                                                                                     
STATIC int
phy_8040_speed_set(int unit, soc_port_t port, int speed)
{
    phy8040_ctrl_t * dc;
    int rv = SOC_E_NONE;
    phy_ctrl_t     *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    dc = PHY8040_DEV_DESC(pc);

    _phy8040_speed_set(unit,port,speed);

    rv = SOC_E_UNAVAIL;
    PHYDRV_CALL_ARG1(pc,dc,PHY_SPEED_SET,speed,rv);
                                                                                     
    if (rv == SOC_E_UNAVAIL) {
        phy_ctrl_t  *int_pc;
        int_pc = INT_PHY_SW_STATE(unit, port);
                                                                                     
        if (NULL != int_pc) {
        rv = PHY_SPEED_SET(int_pc->pd, unit, port, speed);
        }
    }
    return (rv);
}

STATIC int
_phy8040_speed_get(int unit, soc_port_t port, int *speed)
{
    phy8040_ctrl_t * dc;
    int rv = SOC_E_NONE;
    phy_ctrl_t     *pc;
                                                                                     
    pc = EXT_PHY_SW_STATE(unit, port);
    dc = PHY8040_DEV_DESC(pc);
    if (dc->sp_10g[dc->inx] == FALSE) {
        *speed = 1000;
    } else {
        *speed = 10000;
    }

    return (rv);
}

/*
 * Function:
 *      phy_8040_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - current link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_8040_speed_get(int unit, soc_port_t port, int *speed)
{
    phy8040_ctrl_t * dc;
    int rv = SOC_E_NONE;
    phy_ctrl_t     *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    dc = PHY8040_DEV_DESC(pc);

    _phy8040_speed_get(unit, port, speed);
    PHYDRV_CALL_ARG1(pc,dc,PHY_SPEED_GET,speed,rv);

    return (rv);
}

/*
 * Function:
 *      phy_8040_an_get
 * Purpose:
 *      Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      an   - (OUT) if true, auto-negotiation is enabled.
 *      an_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if an == false.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_8040_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy8040_ctrl_t * dc;
    int rv = SOC_E_NONE;
    phy_ctrl_t     *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    dc = PHY8040_DEV_DESC(pc);
    *an= 0;
    *an_done = 0;
    PHYDRV_CALL_ARG2(pc,dc,PHY_AUTO_NEGOTIATE_GET,an,an_done,rv);
    return (rv); 
}

/*
 * Function:
 *      phy_8040_an_set
 * Purpose:
 *      Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      an   - Boolean, if true, auto-negotiation is enabled
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_8040_an_set(int unit, soc_port_t port, int an)
{
    phy8040_ctrl_t * dc;
    int rv = SOC_E_NONE;
    phy_ctrl_t     *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    dc = PHY8040_DEV_DESC(pc);
    PHYDRV_CALL_ARG1(pc,dc,PHY_AUTO_NEGOTIATE_SET,an,rv);
    return (rv);
}

/*
 * Function:
 *    phy_8040_lb_set
 * Purpose:
 *    Put 8040 in PHY PCS/PMA/PMD loopback
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8040_lb_set(int unit, soc_port_t port, int enable)
{
    uint16     tmp;
    phy8040_ctrl_t * dc;
    phy_ctrl_t     *pc;
    int rv = SOC_E_UNAVAIL;
    pc = EXT_PHY_SW_STATE(unit, port);
                                                                                
    dc = PHY8040_DEV_DESC(pc);

    /* do the loopback from outmost phy if available */
    PHYDRV_CALL_ARG1(pc,dc,PHY_LOOPBACK_SET,enable,rv);

    if (rv != SOC_E_NONE) {
        tmp = enable ? MII_CTRL_LE : 0;

        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8040_XGXSn_REG(pc->unit, pc, dc->devids[dc->lp[dc->inx].port],
                             MII_CTRL_REG, tmp, MII_CTRL_LE));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8040_lb_get
 * Purpose:
 *    Get 8040 PHY loopback state
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8040_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16 tmp;
    int rv = SOC_E_UNAVAIL;
    phy8040_ctrl_t * dc;
    phy_ctrl_t     *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    dc = PHY8040_DEV_DESC(pc);

    /* check the loopback from outmost phy if available */
    PHYDRV_CALL_ARG1(pc,dc,PHY_LOOPBACK_GET,enable,rv);

    if (rv != SOC_E_NONE) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8040_XGXSn_REG(pc->unit, pc, dc->devids[dc->lp[dc->inx].port],
                   MII_CTRL_REG,&tmp));

        *enable = (tmp & MII_CTRL_LE) ? TRUE : FALSE;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8040_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      type  - Control to update
 *      value - New setting for the control
 *      type SOC_PHY_CONTROL_INTERFACE controls which mux port to use. 
 *      The associated value specifies the logical mux port number 0,1 and 2 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_8040_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int         rv;
    phy8040_ctrl_t * dc;
    int logic_port;
    phy_ctrl_t     *pc;
    int speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    dc = PHY8040_DEV_DESC(pc);

    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }
                                                                                     
    rv = SOC_E_NONE;
    
    switch(type) {
        /* chose which physical port, value contains the selected 8040 port */
        case SOC_PHY_CONTROL_INTERFACE:
            logic_port = value;
            if (logic_port != dc->inx) {

                if (logic_port < dc->num_mports) {
                    /* before changing to the new selected port, disable the 
                     * the current active port to prevent the false link
                     */
                    phy_8040_enable_set(pc->unit, pc->port, FALSE);
                    speed = dc->sp_10g[dc->inx];  /* remember previous speed*/

                    /* activate the new mux port */
                    dc->inx = logic_port;

                    _phy_8040_mux(pc->unit,pc->port);
                    phy_8040_enable_set(pc->unit, pc->port, TRUE);
                 
                    /* the switch port speed may be changed in the previous
                     * link. Make sure it has the right speed
                     */
                    if (speed != dc->sp_10g[dc->inx]) {
                        phy_ctrl_t  *int_pc;

                        speed = dc->sp_10g[dc->inx]? 10000:1000;
                        _phy8040_speed_set(unit,port,speed);

                        int_pc = INT_PHY_SW_STATE(unit, port);
                        
                        /* in case the linkscan doesn't detect the linkup event
                         * and set the serdes speed accordingly. we do it here
                         */                                                  
                        if (NULL != int_pc) {
                            rv = PHY_SPEED_SET(int_pc->pd, unit, port, speed);
                        }
                    }
                }
            }
            break;

        case SOC_PHY_CONTROL_PREEMPHASIS:
            /* Write PRE-EMPHASIS */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8040_XGXSn_REG(pc->unit, pc, 
                                     dc->devids[dc->lp[dc->inx].port],
                                     PHY8040_PRE_EMPHASIS_REG,
                                     PHY8040_PRE_EMPHASIS(value),
                                     PHY8040_PRE_EMPHASIS(0xf)));
            break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
            /* Write DRIVER Current */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8040_XGXSn_REG(pc->unit, pc, 
                                     dc->devids[dc->lp[dc->inx].port],
                                     PHY8040_PRE_EMPHASIS_REG,
                                     PHY8040_DRIVER_STRENGTH(value),
                                     PHY8040_DRIVER_STRENGTH(0xf)));
            break;            
        default:
            rv = SOC_E_UNAVAIL;
            break;
    }
    if (((soc_phy_control_t)type != SOC_PHY_CONTROL_INTERFACE) && 
        ((soc_phy_control_t)type != SOC_PHY_CONTROL_INTERFACE_MAX)) {
        PHYDRV_CALL_ARG2(pc,dc,PHY_CONTROL_SET,type,value,rv);
    }
    return rv;
}
/*
 * Function:
 *      phy_8040_control_get
 * Purpose:
 *      Get current control settign of the PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      type  - Control to update
 *      value - (OUT)Current setting for the control
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_8040_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    uint16      val;
    phy8040_ctrl_t * dc;
    phy_ctrl_t     *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    dc = PHY8040_DEV_DESC(pc);
    
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    
    switch(type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
            /* Read PRE-EMPHASIS */
            SOC_IF_ERROR_RETURN
                (READ_PHY8040_XGXSn_REG(pc->unit, pc, 
                                   dc->devids[dc->lp[dc->inx].port],
                                   PHY8040_PRE_EMPHASIS_REG,
                                   &val));
            *value = (val >> 12) & 0xf;
            break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
            /* Read DRIVER Current */
            SOC_IF_ERROR_RETURN
                (READ_PHY8040_XGXSn_REG(pc->unit, pc, 
                                   dc->devids[dc->lp[dc->inx].port],
                                   PHY8040_PRE_EMPHASIS_REG,
                                   &val));
            *value = (val >> 8) & 0xf;
            break;            
        case SOC_PHY_CONTROL_INTERFACE:
            *value = dc->inx;
            break;

        case SOC_PHY_CONTROL_INTERFACE_MAX:
            *value = dc->num_mports - 1;
            break;
        default:
            rv = SOC_E_UNAVAIL;
            break;
    }

    if (((soc_phy_control_t)type != SOC_PHY_CONTROL_INTERFACE) && 
        ((soc_phy_control_t)type != SOC_PHY_CONTROL_INTERFACE_MAX)) {
        PHYDRV_CALL_ARG2(pc,dc,PHY_CONTROL_GET,type,value,rv);
    }
    return rv;
}

/*
 * Function:
 *      phy_8040_interface_get
 * Purpose:
 *      Get the device's interface.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - return device's interface.
 * Returns:
 *      SOC_E_XXX
 */ 
STATIC int
phy_8040_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    int rv = SOC_E_UNAVAIL;
    phy8040_ctrl_t * dc;
    phy_ctrl_t     *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    dc = PHY8040_DEV_DESC(pc);

    /* check the outmost phy first */
    PHYDRV_CALL_ARG1(pc,dc,PHY_INTERFACE_GET,pif,rv);
    if (rv != SOC_E_NONE) {
        *pif = SOC_PORT_IF_XAUI;
    }                                                                                
    return SOC_E_NONE;
}
                                                                                
/*
 * Function:
 *      phy_8040_ability_local_get
 * Purpose:  
 *      Get the device's complete abilities.
 * Parameters: 
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ability - return device's abilities. 
 * Returns: 
 *      SOC_E_XXX
 */                                                                                      
STATIC 
int phy_8040_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    int rv = SOC_E_UNAVAIL;
    phy8040_ctrl_t * dc;
    phy_ctrl_t     *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    dc = PHY8040_DEV_DESC(pc);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8040_ability_local_get: u=%d p=%d\n"),
              pc->unit, pc->port));                                                                                      
    if (NULL == ability) {    
        return SOC_E_PARAM;     
    }                                                                                      
    /* check the outmost phy first */
    PHYDRV_CALL_ARG1(pc,dc,PHY_ABILITY_LOCAL_GET,ability,rv);
    if (rv != SOC_E_NONE) {
        ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
        ability->speed_full_duplex  = SOC_PA_SPEED_10GB | SOC_PA_SPEED_1000MB;
        ability->pause     = 0;
        ability->interface = SOC_PA_INTF_XGMII;
        ability->medium    = SOC_PA_MEDIUM_FIBER;
        ability->loopback  = SOC_PA_LB_PHY;
        ability->flags     = 0;
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_8040_ability_local_get: u=%d p=%d speed=0x%x\n"),
                  pc->unit, pc->port, ability->speed_full_duplex));
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_8040_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_8040_ability_advert_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    int rv = SOC_E_UNAVAIL;
    phy8040_ctrl_t * dc;
    phy_ctrl_t     *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    dc = PHY8040_DEV_DESC(pc);

    PHYDRV_CALL_ARG1(pc,dc,PHY_ABILITY_ADVERT_GET,ability,rv);
    if (rv != SOC_E_NONE) {
        ability->speed_full_duplex = 0;
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_8040_ability_advert_remote_get
 * Purpose:
 *      Get the remote device's current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_8040_ability_advert_remote_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    int rv = SOC_E_UNAVAIL;
    phy8040_ctrl_t * dc;
    phy_ctrl_t     *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    dc = PHY8040_DEV_DESC(pc);

    PHYDRV_CALL_ARG1(pc,dc,PHY_ABILITY_REMOTE_GET,ability,rv);
    if (rv != SOC_E_NONE) {
        ability->speed_full_duplex = 0;
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_8040_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_8040_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    int rv = SOC_E_NONE;
    phy8040_ctrl_t * dc;
    phy_ctrl_t     *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    dc = PHY8040_DEV_DESC(pc);

    PHYDRV_CALL_ARG1(pc,dc,PHY_ABILITY_ADVERT_SET,ability,rv);
    return (rv);
}

/*
 * Function:
 *      phy_8481_firmware_set
 * Purpose:
 *      program the given firmware into the SPI-ROM
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      offset - offset to the data stream
 *      array  - the given data
 *      datalen- the data length
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_8040_firmware_set(int unit, int port, int offset, uint8 *data,int len)
{
    phy8040_ctrl_t * dc;
    phy_ctrl_t     *pc;
    int rv = SOC_E_NONE;

    pc = EXT_PHY_SW_STATE(unit, port);
    dc = PHY8040_DEV_DESC(pc);

    if (NULL != dc->lp[dc->inx].nxt_pc) {
        PHYDRV_CALL_ARG3(pc,dc,PHY_FIRMWARE_SET,offset,data,len,rv);
        return rv;
    }
    return SOC_E_UNAVAIL;
}

/*
 * Variable:
 *    phy_8040_drv
 * Purpose:
 *    Phy Driver for 10G (XAUI x 4) Serdes PHY. 
 */

phy_driver_t phy_8040drv_xe = {
    "8040 10-Gigabit PHY Driver",
    phy_8040_init,        /* Init */
    phy_null_reset,       /* Reset (dummy) */
    phy_8040_link_get,    /* Link get: XXX: talk to Bob */
    phy_8040_enable_set,  /* Enable set: : XXX: talk to Bob    */
    phy_null_enable_get,  /* Enable get */
    phy_null_set,         /* Duplex set */
    phy_null_one_get,     /* Duplex get */
    phy_8040_speed_set,   /* Speed set  */
    phy_8040_speed_get,   /* Speed get  */
    phy_null_set,         /* Master set */
    phy_null_zero_get,    /* Master get */
    phy_8040_an_set,         /* ANA set */
    phy_8040_an_get,      /* ANA get */
    phy_null_mode_set,    /* Local Advert set */
    phy_null_mode_get,    /* Local Advert get */
    phy_null_mode_get,    /* Remote Advert get */
    phy_8040_lb_set,      /* PHY loopback set: XXX: talk to Bob    */
    phy_8040_lb_get,      /* PHY loopback set: XXX: talk to Bob    */
    phy_null_interface_set, /* IO Interface set */
    phy_8040_interface_get, /* IO Interface get */
    NULL,   /* PHY abilities mask */
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,
    NULL,
    phy_null_medium_get,
    NULL,                    /* phy_cable_diag  */
    NULL,                    /* phy_link_change */
    phy_8040_control_set,    /* phy_control_set */
    phy_8040_control_get,    /* phy_control_get */
    NULL,                    /* phy_reg_read */
    NULL,                    /* phy_reg_write */
    NULL,                    /* phy_reg_modify */
    NULL,                    /* phy_notify */
    NULL,                    /* pd_probe */
    phy_8040_ability_advert_set,  /* pd_ability_advert_set */
    phy_8040_ability_advert_get,  /* pd_ability_advert_get */
    phy_8040_ability_advert_remote_get,     /* pd_ability_remote_get */
    phy_8040_ability_local_get,   /* pd_ability_local_get  */
    phy_8040_firmware_set 
};
#else /* INCLUDE_PHY_8040 */
typedef int _phy_8040_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_8040 */

