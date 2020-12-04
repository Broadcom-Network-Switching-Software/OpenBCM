/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy84728.c
 * Purpose:     SDK PHY driver for BCM84728
 *
 * Supported BCM84XXX Family of PHY devices:
 *
 *      Device  Ports   Media                           MAC Interface
 *      84707    1       1 10G SFP+                     XAUI
 *      84073    2       2 KR                           XAUI
 *      84074    4       4 KR                           XAUI
 *      84728    2       2 10G SFP+/WAN                 XAUI
 *      84748    4       4 10G SFP+/WAN                 XAUI
 *   84727/84722 2       2 10G SFP+                     XAUI
 *   84747/84742 4       4 10G SFP+                     XAUI
 *      84762    2       2 10G SFP+                     RXAUI
 *      84764    4       4 10G SFP+                     RXAUI
 *      84042    2       2 KR                           RXAUI
 *      84044    4       4 KR                           RXAUI
 *
 * Workarounds:
 *
 * References:
 *     
 * Notes:
 */ 
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_84728)
#include "phyconfig.h"    /* Must be the first phy include after phydefs.h */

#include <sal/types.h>
#include <sal/core/spl.h>
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
#include "phyfege.h"
#include "phyxehg.h"

#include "phy84728_int.h"
#include "phy84728.h"

#define LAN_MODE 0 
#define WAN_MODE 1
STATIC int
_phy_84728_mode_set(int unit, soc_port_t port, uint32 mode);


#define ADVERT_ALL_COPPER \
        (SOC_PA_PAUSE | SOC_PA_SPEED_10MB | \
         SOC_PA_SPEED_100MB | SOC_PA_SPEED_1000MB)

#define ADVERT_ALL_FIBER \
        (SOC_PA_PAUSE | SOC_PA_SPEED_1000MB) 

#define ADVERT_ALL_10GFIBER \
        (SOC_PA_PAUSE | SOC_PA_SPEED_10GB) 

#define WRITE_PHY84728_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val) \
WRITE_PHY_REG((_unit), (_phy_ctrl), \
SOC_PHY_CLAUSE45_ADDR(1, (_reg)), (_val))

#define MODIFY_PHY84728_PMA_PMD_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
MODIFY_PHY_REG((_unit), (_phy_ctrl), \
SOC_PHY_CLAUSE45_ADDR(1, (_reg)), (_val), (_mask))


#define PHY84728_REG_READ(_unit, _phy_ctrl, _addr, _val) \
            READ_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))

#define READ_PHY84728_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY84728_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(1, (_reg)), (_val))

#define NXT_PC(pc) ((phy_ctrl_t *)((pc)->driver_data))
#define PHYDRV_CALL_NOARG(pc,name) \
     do { \
        if (pc->driver_data) { \
            int rv; \
            phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE(pc->unit, pc->port); \
            EXT_PHY_SW_STATE(pc->unit,pc->port) = (phy_ctrl_t *)(pc->driver_data); \
            rv = name(((phy_ctrl_t *)(pc->driver_data))->pd,(pc)->unit,(pc)->port); \
            EXT_PHY_SW_STATE(pc->unit, pc->port) = tmp_pc; \
            if (SOC_FAILURE(rv)) { \
                return rv; \
            } \
       } \
    } while(0)

#define PHYDRV_CALL_ARG1(pc,name,arg0) \
    do { \
       if (pc->driver_data) { \
           int rv; \
           phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE(pc->unit, pc->port); \
           EXT_PHY_SW_STATE(pc->unit, pc->port) = (phy_ctrl_t *)(pc->driver_data); \
           rv = name(((phy_ctrl_t *)(pc->driver_data))->pd,(pc)->unit, \
                (pc)->port,arg0); \
           EXT_PHY_SW_STATE(pc->unit, pc->port) = tmp_pc; \
           if (SOC_FAILURE(rv)) { \
               return rv; \
           } \
       } \
    } while(0)

#define PHYDRV_CALL_ARG2(pc,name,arg0,arg1) \
    do { \
       if (pc->driver_data) { \
           int rv; \
           phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE(pc->unit, pc->port); \
           EXT_PHY_SW_STATE(pc->unit, pc->port) = (phy_ctrl_t *)(pc->driver_data); \
           rv=name(((phy_ctrl_t *)(pc->driver_data))->pd,(pc)->unit,(pc)->port,arg0, \
                        arg1); \
           EXT_PHY_SW_STATE(pc->unit, pc->port) = tmp_pc; \
           if (SOC_FAILURE(rv)) { \
               return rv; \
           } \
       } \
    } while(0)

#define PHYDRV_CALL_ARG3(pc,name,arg0,arg1,arg2) \
    do { \
       if (pc->driver_data) { \
           int rv; \
           phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE(pc->unit, pc->port); \
           EXT_PHY_SW_STATE(pc->unit, pc->port) = (phy_ctrl_t *)(pc->driver_data); \
           rv=name(((phy_ctrl_t *)(pc->driver_data))->pd,(pc)->unit,(pc)->port,arg0, \
                        arg1,arg2); \
           EXT_PHY_SW_STATE(pc->unit, pc->port) = tmp_pc; \
           if (SOC_FAILURE(rv)) { \
               return rv; \
           } \
       } \
    } while(0)

#define BSDK_PHY84728_LINE_MAC_PORT(p)    (p)
#define BSDK_PHY84728_SWITCH_MAC_PORT(p)  ((p) + 1)
#define PHY84728_PDET_MASK   (1 << 0)

/*
 * PHY Map structure for mapping mac drivers
 */
typedef struct phy84728_map_s {
    bsdk_phy84728_intf_t line_mode;     /* configured line mode */
    uint32 phy_sys_xaui_tx_lane_map;
    uint32 phy_sys_xaui_rx_lane_map;
    uint32 devid;
    uint32 reverse_mode;
    uint32 link_down;

}phy84728_map_t;

#define PHY84728_MAP(_pc) ((phy84728_map_t *)((_pc) + 1))
#define REVERSE_MODE(_pc) ((PHY84728_MAP(_pc))->reverse_mode)

#define BSDK_PHY84728_IS_KR_VARIANT(devid) \
(  (devid) == 0x84073  ||\
   (devid) == 0x84074  ||\
   (devid) == 0x84042  ||\
   (devid) == 0x84044  )

#define BSDK_PHY84728_IS_84728(devid) \
(   (devid) == 0x84728  )

#define PHY_84727_EDC_MODE_MASK     0xff

/* OUI from ID0 and ID1 registers contents */
#define BSDK_PHY84728_PHY_OUI(id0, id1)                          \
    bsdk_phy84728_bit_rev_by_byte_word32((uint32)(id0) << 6 | \
                                           ((id1) >> 10 & 0x3f))

/* PHY model from ID0 and ID1 register Contents */
#define BSDK_PHY84728_PHY_MODEL(id0, id1) ((id1) >> 4 & 0x3f)

/* PHY revision from ID0 and ID1 register Contents */
#define BSDK_PHY84728_PHY_REV(id0, id1)   ((id1) & 0xf)

/*
 * Function:     
 *    bsdk_bit_rev_by_byte_word32
 * Purpose:    
 *    Reverse the bits in each byte of a 32 bit long 
 * Parameters:
 *    n - 32bit input
 * Notes: 
 */
STATIC uint32
bsdk_phy84728_bit_rev_by_byte_word32(uint32 n)
{
    n = (((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1));
    n = (((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2));
    n = (((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4));
    return n;
}

STATIC int phy_84728_init(int unit, soc_port_t port);
STATIC int phy_84728_link_get(int unit, soc_port_t port, int *link);
STATIC int phy_84728_enable_set(int unit, soc_port_t port, int enable);
STATIC int phy_84728_duplex_set(int unit, soc_port_t port, int duplex);
STATIC int phy_84728_duplex_get(int unit, soc_port_t port, int *duplex);
STATIC int phy_84728_speed_set(int unit, soc_port_t port, int speed);
STATIC int phy_84728_speed_get(int unit, soc_port_t port, int *speed);
STATIC int phy_84728_master_set(int unit, soc_port_t port, int master);
STATIC int phy_84728_master_get(int unit, soc_port_t port, int *master);
STATIC int phy_84728_an_set(int unit, soc_port_t port, int autoneg);
STATIC int phy_84728_an_get(int unit, soc_port_t port,
                                int *autoneg, int *autoneg_done);
STATIC int phy_84728_lb_set(int unit, soc_port_t port, int enable);
STATIC int phy_84728_lb_get(int unit, soc_port_t port, int *enable);
STATIC int phy_84728_medium_config_set(int unit, soc_port_t port, 
                                      soc_port_medium_t  medium,
                                      soc_phy_config_t  *cfg);
STATIC int phy_84728_medium_config_get(int unit, soc_port_t port, 
                                      soc_port_medium_t medium,
                                      soc_phy_config_t *cfg);
STATIC int phy_84728_ability_advert_set(int unit, soc_port_t port, 
                                       soc_port_ability_t *ability);
STATIC int phy_84728_ability_advert_get(int unit, soc_port_t port,
                                       soc_port_ability_t *ability);
STATIC int phy_84728_ability_remote_get(int unit, soc_port_t port,
                                       soc_port_ability_t *ability);
STATIC int phy_84728_ability_local_get(int unit, soc_port_t port, 
                                soc_port_ability_t *ability);
STATIC int phy_84728_firmware_set(int unit, int port, int offset, 
                                 uint8 *data, int len);
STATIC int phy_84728_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value);
STATIC int phy_84728_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value);
STATIC int phy_84728_link_up(int unit, soc_port_t port);
STATIC int bsdk_phy84728_line_mode_set(phy_ctrl_t *pc, int dev_port,
                     bsdk_phy84728_intf_t mode);
STATIC int bsdk_phy84728_sys_mode_set(phy_ctrl_t *pc, int dev_port, 
                     bsdk_phy84728_intf_t mode);

STATIC int
_bsdk_phy84728_system_sgmii_duplex_set(phy_ctrl_t *pc, int dev_port, int duplex);
STATIC int
_bsdk_phy84728_system_sgmii_sync(phy_ctrl_t *pc, int dev_port);

extern uint8 bcm84728_fw[];
extern uint32 bcm84728_fw_length;
extern uint8 bcm84748_fw[];
extern uint32 bcm84748_fw_length;

#define NEW_NAME_ARRAY_SIZE 25
STATIC int
_phy_84728_nxt_dev_probe(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    phy_ctrl_t phy_ctrl;
    soc_phy_info_t phy_info;
    extern int _ext_phy_probe(int unit, soc_port_t port,soc_phy_info_t *pi, 
                              phy_ctrl_t *ext_pc);
    soc_phy_info_t *pi;
    int offset = 0;
    char new_name[NEW_NAME_ARRAY_SIZE];

    pc = EXT_PHY_SW_STATE(unit, port);

    /* initialize the phy_ctrl for the next device connected to the XFI port */
    sal_memset(&phy_ctrl, 0, sizeof(phy_ctrl_t));
    phy_ctrl.unit = unit;
    phy_ctrl.port = port;
    phy_ctrl.speed_max = pc->speed_max;
    phy_ctrl.read = pc->read;    /* use same MDIO read routine as this device's */
    phy_ctrl.write = pc->write;  /* use same MDIO write routine as this device's */ 

    /* get the mdio address of the next device */
    phy_ctrl.phy_id = soc_property_port_get(unit, port,
                                     spn_PORT_PHY_ADDR1, 0xff);

    if (phy_ctrl.phy_id == 0xff) {
        return SOC_E_NOT_FOUND;
    }

    /* probe the next device at the given address */
    SOC_IF_ERROR_RETURN
        (_ext_phy_probe(unit, port, &phy_info, &phy_ctrl));

    /* If we have probed 84728/84764 then we won't allow 
     * to chain (physically its not possible) another of the same type.
     * The Probe depends on spn_PORT_PHY_ADDR1 to probe 2nd level PHYs.
     * It could so happen that 84728/84764 could be chained with 8481.
     * In such cases probing could end up in a loop.
     */
    if ((phy_ctrl.pd == NULL) || ((phy_ctrl.phy_model == pc->phy_model) && 
                                  (phy_ctrl.phy_oui == pc->phy_oui))) {
        pc->driver_data = NULL;
        /* device not found */
        return SOC_E_NOT_FOUND;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_84728_nxt_dev_probe: found phy device"
                         " u=%d p=%d id0=0x%x id1=0x%x\n"), 
              unit, port,phy_ctrl.phy_id0,phy_ctrl.phy_id1));

    /* set the chained flag */
    PHY_FLAGS_SET(phy_ctrl.unit, phy_ctrl.port, PHY_FLAGS_CHAINED);

    /* Found device, allocate control structure */
    pc->driver_data = (void *)sal_alloc (sizeof (phy_ctrl_t) + phy_ctrl.size, phy_ctrl.pd->drv_name);
    if (pc->driver_data == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memcpy(pc->driver_data, &phy_ctrl, sizeof(phy_ctrl_t));

    /* We need to append the (chanined) PHY's name to the parent */
    pi = &SOC_PHY_INFO(unit, pc->port);
    offset = sal_strlen(pi->phy_name);
    /* size of new_name is 25 which is large enough */
    /* coverity[secure_coding] */
    sal_strcpy(new_name, (pi->phy_name));
    /* coverity[secure_coding] */
    sal_strcpy(new_name+offset, "->");
    offset = sal_strlen(new_name);
    if ((offset + sal_strlen(phy_info.phy_name)) < NEW_NAME_ARRAY_SIZE) {
        /* coverity[secure_coding] */
        sal_strcpy(new_name + offset, phy_info.phy_name);
    } else {
        return SOC_E_MEMORY;
    }
    pi->phy_name = sal_strdup(new_name);

    return SOC_E_NONE;
}
#if defined(NEW_NAME_ARRAY_SIZE)
    #undef NEW_NAME_ARRAY_SIZE
#endif


/*
 * Function:
 *      _phy_84728_medium_config_update
 * Purpose:
 *      Update the PHY with config parameters
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      cfg - Config structure.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_84728_medium_config_update(int unit, soc_port_t port,
                                soc_phy_config_t *cfg)
{

    SOC_IF_ERROR_RETURN
        (phy_84728_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_84728_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_84728_ability_advert_set(unit, port, &cfg->advert_ability));
    SOC_IF_ERROR_RETURN
        (phy_84728_an_set(unit, port, cfg->autoneg_enable));

    return SOC_E_NONE;
}

/*
* Squelch enable/disable
* 1) 10G XAUI output squelching enable: 1.C842.12=1 AND 1.C842.11=0;
* 2) 1G XAUI output squelching enable: 1.C842.10=1
*/
STATIC int
_phy_84728_squelch_enable(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc;
    uint16 data16, mask16;
    int speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (REVERSE_MODE(pc)) {
        /* not squelch support in reverse mode - do nothing */
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (phy_84728_speed_get(unit, port, &speed));

    if (speed == 10000) {
        mask16 = (1<<12);
        data16 = enable ? mask16 : 0;
        mask16 = ((1<<12) | (1<<11));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84728_PMA_PMD_REG(unit, pc, 0xC842, data16, mask16));
    } else {
        mask16 = (1<<10);
        data16 = enable ? mask16 : 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84728_PMA_PMD_REG(unit, pc, 0xC842, data16, mask16));
    }

    return SOC_E_NONE;
}

/* Function:
 *    phy_84728_init
 * Purpose:    
 *    Initialize 84728 phys
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_84728_init(int unit, soc_port_t port)
{
    int         wan_mode;
    phy_ctrl_t *pc;
    int line_mode;
    int fiber_preferred;
    int phy_ext_rom_boot;
    int rv;
    char           *str;
    /*
    _phy84728_event_callback_data_t *tmp;
    */
    /*uint16          ctrl, tmp2;*/
    uint16 temp;
    uint16 devid;
    uint16 mask16;
    uint16 data16;
    phy_ctrl_t *int_pc;

    pc     = EXT_PHY_SW_STATE(unit, port);

    sal_memset(PHY84728_MAP(pc), 0, sizeof(phy84728_map_t));

    PHY84728_MAP(pc)->link_down = 1;

    if (READ_PHY84728_PMA_PMD_REG(unit, pc, 0xc802, &devid) < 0) {
        return SOC_E_FAIL;
    }

    PHY84728_MAP(pc)->devid = devid;

    if (READ_PHY84728_PMA_PMD_REG(unit, pc, 0xc803, &devid) < 0) {
        return SOC_E_FAIL;
    }

    PHY84728_MAP(pc)->devid |= ((devid & 0xf) << 16) ;

    if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) {
        phy_ext_rom_boot = soc_property_port_get(unit, port,
                                                 spn_PHY_EXT_ROM_BOOT, 1);
        if (!phy_ext_rom_boot) {
            pc->flags |= PHYCTRL_MDIO_BCST;
            pc->dev_name = "BCM84728";
        }
        /* indicate second pass of init is needed */
        PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS2);
        return SOC_E_NONE;
    }

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {
  
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_84728_init: u=%d p=%d\n"),unit, port));

        REVERSE_MODE(pc) = soc_property_port_suffix_num_get(unit, port, 84728,
                                       spn_PORT_PHY_MODE_REVERSE, "phy", 0);


        /* Preferred mode:
         * phy_fiber_pref = 1 ; Port is in SFI mode in 10G Default
         *                      when speed is changed to 1G, its 1000X
         * phy_fiber_pref = 0; Port is in SGMII mode, allows only 1G/100M/10M
         */
        fiber_preferred =
            soc_property_port_get(unit, port, spn_PHY_FIBER_PREF, 1);
        
        /* Initially configure for the preferred medium. */
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_PASSTHRU);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_100FX);
        
        if (fiber_preferred) {
            line_mode = BSDK_PHY84728_INTF_SFI;
            pc->copper.enable = FALSE;
            pc->fiber.enable = TRUE;
            pc->interface = SOC_PORT_IF_XFI;
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
        } else {
            pc->interface = SOC_PORT_IF_SGMII;
            line_mode = BSDK_PHY84728_INTF_SGMII;
            pc->copper.enable = TRUE;
            pc->fiber.enable = FALSE;
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
        }
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_C45);
        
        pc->copper.preferred = !fiber_preferred;
        pc->copper.autoneg_enable = TRUE;
        pc->copper.autoneg_advert = ADVERT_ALL_COPPER;
        pc->copper.force_speed = 1000;
        pc->copper.force_duplex = TRUE;
        pc->copper.master = SOC_PORT_MS_AUTO;
        pc->copper.mdix = SOC_PORT_MDIX_AUTO;
        
        pc->fiber.preferred = TRUE;
        pc->fiber.autoneg_enable = FALSE;
        pc->fiber.autoneg_advert = ADVERT_ALL_FIBER;
        pc->fiber.force_speed = 10000;
        pc->fiber.force_duplex = TRUE;
        pc->fiber.master = SOC_PORT_MS_NONE;
        pc->fiber.mdix = SOC_PORT_MDIX_NORMAL;
        
        /* config DAC mode and backplane mode for KR/LR4/SR4  */
        mask16 = BSDK_PHY84728_LN_DEV1_CHIP_MODE_DAC_MASK |
            BSDK_PHY84728_LN_DEV1_CHIP_MODE_BKPLANE_MASK ;
        if (BSDK_PHY84728_IS_KR_VARIANT(PHY84728_MAP(pc)->devid)) {
            data16 = BSDK_PHY84728_LN_DEV1_CHIP_MODE_BKPLANE_MASK;
        } else {
            data16 = 0;
        }
        
        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_LN_DEV1_CHIP_MODEr(pc, data16, mask16));
        
        
        SOC_IF_ERROR_RETURN(
            bsdk_phy84728_reset(pc));
        
        /* Check where to load ucode from again in pass2 */
        phy_ext_rom_boot = soc_property_port_get(unit, port,
                                                 spn_PHY_EXT_ROM_BOOT, 1);
        SOC_IF_ERROR_RETURN(
            bsdk_phy84728_no_reset_setup(pc,
                                         line_mode,
                                         phy_ext_rom_boot));
        
        /* Get Requested LED selectors (defaults are hardware defaults) */
        pc->ledmode[0] = soc_property_port_get(unit, port, spn_PHY_LED1_MODE, 0);
        pc->ledmode[1] = soc_property_port_get(unit, port, spn_PHY_LED2_MODE, 1);
        pc->ledmode[2] = soc_property_port_get(unit, port, spn_PHY_LED3_MODE, 3);
        pc->ledmode[3] = soc_property_port_get(unit, port, spn_PHY_LED4_MODE, 6);
        pc->ledctrl = soc_property_port_get(unit, port, spn_PHY_LED_CTRL, 0x8);
        pc->ledselect = soc_property_port_get(unit, port, spn_PHY_LED_SELECT, 0);
        
        /* WAN/LAN mode set */
        wan_mode = soc_property_port_get(unit, port, spn_PHY_WAN_MODE, FALSE);
        
        if (wan_mode) {
            SOC_IF_ERROR_RETURN
                (_phy_84728_mode_set(unit, port, WAN_MODE));
        } else {
            SOC_IF_ERROR_RETURN
                (_phy_84728_mode_set(unit, port, LAN_MODE));
        }
        
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "84728: u=%d port=%d mode=%s : init.\n"), 
                             unit, port, wan_mode ? "WAN" : "LAN"));
        
        
        /* XAUI RX Lane Swap (Lane A, B, C, D changes to Lane D, C, B, A) */
        str = soc_property_port_get_str(unit, port, spn_PHY_XAUI_RX_LANE_SWAP); 
        if (str != NULL) {
            temp = soc_property_port_get(unit, port,
                                         spn_PHY_XAUI_RX_LANE_SWAP, 0);
        } else {
            temp = 0;
        }
        PHY84728_MAP(pc)->phy_sys_xaui_rx_lane_map = (temp) ? 0x3210 : 0;
        
        
        /* XAUI TX Lane Swap (Lane A, B, C, D changes to Lane D, C, B, A) */
        str = soc_property_port_get_str(unit, port, spn_PHY_XAUI_TX_LANE_SWAP); 
        if (str != NULL) {
            temp = soc_property_port_get(unit, port,
                                         spn_PHY_XAUI_TX_LANE_SWAP, 0);
        } else {
            temp = 0;
        }
        PHY84728_MAP(pc)->phy_sys_xaui_tx_lane_map = (temp) ? 0x3210 : 0;
        
        
#if 0
        /* Use lane mapping */
        SOC_IF_ERROR_RETURN(
            bsdk_phy84728_lane_map(pc));
#endif
        
        
        if (line_mode != BSDK_PHY84728_INTF_SGMII) {
            SOC_IF_ERROR_RETURN
                (_phy_84728_medium_config_update(unit, port, &pc->fiber));
        }
        
        int_pc = INT_PHY_SW_STATE(unit, port);
        if (NULL != int_pc) {
            PHY_AUTO_NEGOTIATE_SET (int_pc->pd, unit, port, 0);
            PHY_SPEED_SET(int_pc->pd, unit, port, 10000);
        }
        
        if (IS_HG_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(
                BSDK_PHY84728_REG_WR(pc, 0x00, 0x0004, 0x8007, 0x0040));
            SOC_IF_ERROR_RETURN(
                BSDK_PHY84728_REG_MOD(pc, 0x00, 0x0001, 0xcd08, 0x0100, 0x0100));
        }
        
        /* Enable Squelch */
        SOC_IF_ERROR_RETURN(_phy_84728_squelch_enable(unit, port, TRUE));
        
        rv = _phy_84728_nxt_dev_probe(unit,port);
        if (SOC_SUCCESS(rv)) {
            PHYDRV_CALL_NOARG(pc,PHY_INIT);
        }
    }        
    return SOC_E_NONE;
}

STATIC int
_phy_54942_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t *pc; 
    uint16      xgxsstatus1, status1000x1;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (BSDK_RD_PHY84728_SYS_DEV4_XGXSSTATUS1r(pc, &xgxsstatus1));

    SOC_IF_ERROR_RETURN
        (BSDK_RD_PHY84728_SYS_DEV4_XGXS_BASE1000X_STAT1r(pc, &status1000x1));

    if (xgxsstatus1 == 0xffff) {
      *link = 0 ;
    } else {
      *link =  ((xgxsstatus1 & (1U << 2)) | (status1000x1 & (1U << 1))) ? 1 : 0;
    }

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "_phy_54942_link_get: u=%d port%d: link:%s\n"),
                 unit, port, *link ? "Up": "Down"));

    return SOC_E_NONE;
}


/*
 * Function:
 *    phy_84728_link_get
 * Purpose:
 *    Get layer2 connection status.
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      link - address of memory to store link up/down state.
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_84728_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t *pc;
    int rv = SOC_E_NONE;
    bsdk_phy84728_intf_t bsdk_mode;
    int bsdk_link;
    uint16 wis_mii_stat;

    pc    = EXT_PHY_SW_STATE(unit, port);
    *link = FALSE;      /* Default return */


    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc, PHY_LINK_GET, link);
        return SOC_E_NONE;
    }

    rv = bsdk_phy84728_line_intf_get(pc, 0, &bsdk_mode);
    if (!SOC_SUCCESS(rv)) {
        return SOC_E_FAIL;
    }

    if ((bsdk_mode == BSDK_PHY84728_INTF_SGMII) && !PHY_FLAGS_TST(unit, port, PHY_FLAGS_COPPER)) {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
    } else if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_FIBER)) {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
    }

    if (REVERSE_MODE(pc)) {
        SOC_IF_ERROR_RETURN
            (_phy_54942_link_get(unit, port, link));
    } else {
        if (PHY_WAN_MODE(unit, port)) {
             /* WIS status */
            SOC_IF_ERROR_RETURN
                (BSDK_RD_PHY84728_WIS_STATr(pc, &wis_mii_stat));
            bsdk_link = wis_mii_stat == 0xffff ? 0 : (wis_mii_stat & 0x0004) ? 1 : 0 ;
        } else {
            rv = bsdk_phy84728_link_get(pc, &bsdk_link);
            if (!SOC_SUCCESS(rv)) {
                return SOC_E_FAIL;
            }
        }

        if (bsdk_link) {
            *link = TRUE;
        } else {
            *link = FALSE;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "phy_84728_link_get: u=%d p=%d link=%d\n"),
                 unit, port,
                 *link));

    return SOC_E_NONE;
}


STATIC int
_phy_54942_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc; 

    pc = EXT_PHY_SW_STATE(unit, port);

    /* disable the Tx */
    SOC_IF_ERROR_RETURN
        (BSDK_MOD_PHY84728_XGXSBLK0_MISCCONTROL1r(pc,
               enable? 0:XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK,
               XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK));

    /* disable the Rx */
    SOC_IF_ERROR_RETURN
        (BSDK_MOD_PHY84728_SYS_DEV4_XGXS_CTRLr(pc, enable?  BSDK_PHY84728_AN_XGXS_CTRL_START_SEQ : 0,
                                                            BSDK_PHY84728_AN_XGXS_CTRL_START_SEQ ));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84728_enable_set
 * Purpose:
 *      Enable or disable the physical interface.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - Boolean, true = enable PHY, false = disable.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_84728_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc;
    uint16 power;

    pc     = EXT_PHY_SW_STATE(unit, port);

    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    if (REVERSE_MODE(pc)) {
        SOC_IF_ERROR_RETURN
            (_phy_54942_enable_set(unit, port, enable));
    } else {

        /* PMA/PMD */
        power = (enable) ? 0 : 0x0001 ;
        SOC_IF_ERROR_RETURN(
        BSDK_MOD_PHY84728_LN_DEV1_PMD_XMIT_DISABLEr(pc, power, 0x0001));

    }

    if (enable) {
        sal_usleep(50);
        SOC_IF_ERROR_RETURN(_phy_84728_squelch_enable(unit, port, TRUE));
    }

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_ENABLE_SET,enable);
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84728_enable_set: "
                         "Power %s fiber medium\n"), (enable) ? "up" : "down"));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84728_duplex_set
 * Purpose:
 *      Set the current duplex mode
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, true indicates full duplex, false indicates half.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_UNAVAIL - Half duplex requested, and not supported.
 * Notes:
 */
STATIC int
phy_84728_duplex_set(int unit, soc_port_t port, int duplex_in)
{
    int                          rv = SOC_E_NONE ;
    phy_ctrl_t                   *pc;
    bsdk_phy84728_duplex_t    duplex;
    uint16 mii_ctrl;
    bsdk_phy84728_intf_t line_mode;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_DUPLEX_SET, duplex_in);
        return SOC_E_NONE;
    }

    if (duplex_in) {
        duplex = BSDK_PHY84728_FULL_DUPLEX;
    } else {
        duplex = BSDK_PHY84728_HALF_DUPLEX;
    }


    SOC_IF_ERROR_RETURN(
        bsdk_phy84728_line_intf_get(pc, 0, &line_mode));

    if ((line_mode == BSDK_PHY84728_INTF_1000X ) ||
        (line_mode == BSDK_PHY84728_INTF_SFI )) {

        if (duplex == BSDK_PHY84728_FULL_DUPLEX) {
            rv =  SOC_E_NONE;
            goto exit;
        } else {
            rv =  SOC_E_UNAVAIL;
            goto exit;
        }
    }
    SOC_IF_ERROR_RETURN(
        BSDK_RD_PHY84728_LN_DEV7_AN_MII_CTRLr(pc, &mii_ctrl));

    if (duplex == BSDK_PHY84728_FULL_DUPLEX) {
        mii_ctrl |= BSDK_PHY84728_AN_MII_CTRL_FD;
    } else if (duplex == BSDK_PHY84728_HALF_DUPLEX) {
        mii_ctrl &= ~BSDK_PHY84728_AN_MII_CTRL_FD;
    } 
    SOC_IF_ERROR_RETURN(
        BSDK_WR_PHY84728_LN_DEV7_AN_MII_CTRLr(pc, mii_ctrl));

exit:

    if(SOC_SUCCESS(rv)) {
        pc->copper.force_duplex = duplex_in;
    }


    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84728_duplex_set: u=%d p=%d d=%d rv=%d\n"),
              unit, port, duplex, rv));
    return rv;
}

/*
 * Function:
 *      phy_84728_duplex_get
 * Purpose:
 *      Get the current operating duplex mode. If autoneg is enabled,
 *      then operating mode is returned, otherwise forced mode is returned.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - (OUT) Boolean, true indicates full duplex, false
 *              indicates half.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The duplex is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level. Autonegotiation is
 *      not manipulated.
 */

STATIC int
phy_84728_duplex_get(int unit, soc_port_t port, int *duplex)
{

    phy_ctrl_t                   *pc;
    uint16	status;
    bsdk_phy84728_intf_t line_mode;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_DUPLEX_GET, duplex);
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(
        bsdk_phy84728_line_intf_get(pc, 0, &line_mode));

    if ((line_mode == BSDK_PHY84728_INTF_1000X ) ||
        (line_mode == BSDK_PHY84728_INTF_SFI )) {
        *duplex = TRUE;
    } else {
        SOC_IF_ERROR_RETURN(
            BSDK_RD_PHY84728_LN_DEV7_AN_BASE1000X_STAT1r(pc, &status));
        if (status & (1U << 2)) {
            *duplex = TRUE;
        } else {
            *duplex = FALSE;
        }
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_84728_speed_set
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
phy_84728_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t *pc;
    phy_ctrl_t  *int_pc;
    int            rv; 

    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if (speed > 10000) {
        return SOC_E_UNAVAIL;
    }

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_SPEED_SET,speed);
        return SOC_E_NONE;
    }

    rv = bsdk_phy84728_speed_set(pc, speed);
    if(SOC_SUCCESS(rv)) {
        /* need to set the internal phy's speed accordingly */
        if (NULL != int_pc) {
            if (REVERSE_MODE(pc)) {
                SOC_IF_ERROR_RETURN
                    (PHY_INTERFACE_SET(int_pc->pd, unit, port, SOC_PORT_IF_SFI));
            }
            PHY_AUTO_NEGOTIATE_SET (int_pc->pd, unit, port, 0);
            PHY_SPEED_SET(int_pc->pd, unit, port, speed);
        }
        if(PHY_COPPER_MODE(unit, port)) {
            pc->copper.force_speed = speed;
        } else {
            if(PHY_FIBER_MODE(unit, port)) {
                pc->fiber.force_speed = speed;
            }
        }
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84728_speed_set: u=%d p=%d s=%d fiber=%d rv=%d\n"),
              unit, port, speed, PHY_FIBER_MODE(unit, port), rv));

    return rv;
}

/*
 * Function:
 *      phy_84728_speed_get
 * Purpose:
 *      Get the current operating speed for a 84728 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - (OUT) Speed of the phy
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The speed is retrieved for the ACTIVE medium.
 */

STATIC int
phy_84728_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t    *pc;
    uint16 data = 0;
    bsdk_phy84728_intf_t line_mode;

    if (speed == NULL) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_SPEED_GET,speed);
        return SOC_E_NONE;
    }

    {
        SOC_IF_ERROR_RETURN(
            bsdk_phy84728_line_intf_get(pc, 0, &line_mode));

        if (line_mode == BSDK_PHY84728_INTF_SFI) {
            *speed = 10000;
            return SOC_E_NONE;
        }
        if (line_mode == BSDK_PHY84728_INTF_1000X) {
            *speed = 1000;
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(
            BSDK_RD_PHY84728_LN_DEV1_SPEED_LINK_DETECT_STATr(pc, &data));
        switch(data & BSDK_PHY84728_PMD_SPEED_LD_STAT_LN_PDM_SPEED_MASK) {
        case BSDK_PHY84728_PMD_SPEED_LD_STAT_LN_PDM_SPEED_10M:
            *speed = 10;
            break;
        case BSDK_PHY84728_PMD_SPEED_LD_STAT_LN_PDM_SPEED_100M:
            *speed = 100;
            break;
        case BSDK_PHY84728_PMD_SPEED_LD_STAT_LN_PDM_SPEED_1G:
            *speed = 1000;
            break;
        case BSDK_PHY84728_PMD_SPEED_LD_STAT_LN_PDM_SPEED_10G:
            *speed = 10000;
            break;
        default :
            *speed = 0;
            break;
        }
    }

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_84728_speed_get: u=%d p=%d speed=%d"),
                  unit, port, *speed));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84728_an_set
 * Purpose:
 *      Enable or disable auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      autoneg - Boolean, if true, auto-negotiation is enabled
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The autoneg mode is set only for the ACTIVE medium.
 */

STATIC int
phy_84728_an_set(int unit, soc_port_t port, int autoneg)
{
    int                 rv;
    phy_ctrl_t   *pc;
    uint16 data = 0;
    bsdk_phy84728_intf_t line_mode;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc, PHY_AUTO_NEGOTIATE_SET, autoneg);
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(
        bsdk_phy84728_line_intf_get(pc, 0, &line_mode));


    if (line_mode == BSDK_PHY84728_INTF_SGMII) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (BSDK_MOD_PHY84728_LN_DEV7_ANr(pc, 0x8308, autoneg ? 0x0 : 0x1c, 0x1f));

    SOC_IF_ERROR_RETURN
        (BSDK_MOD_PHY84728_LN_DEV7_AN_MSIC2r(pc,  autoneg ? 0x0 : 0x20, 0x20));


#if 0  /* needed for KR */
    SOC_IF_ERROR_RETURN(
        BSDK_RD_PHY84728_XFI_DEV7_AN_CTRLr(pc, &data));
    if (autoneg) {
        data |= (BSDK_PHY84728_AN_AN_CTRL_AE |
                BSDK_PHY84728_AN_AN_CTRL_RAN);
    } else {
        data &= ~(BSDK_PHY84728_AN_AN_CTRL_AE |
                  BSDK_PHY84728_AN_AN_CTRL_RAN);
    }
    SOC_IF_ERROR_RETURN(
        BSDK_WR_PHY84728_XFI_DEV7_AN_CTRLr(pc, data));
#endif


    SOC_IF_ERROR_RETURN(
        BSDK_RD_PHY84728_LN_DEV7_AN_MII_CTRLr(pc, &data));
    if (autoneg) {
        data |= (BSDK_PHY84728_AN_MII_CTRL_AE | 
                 BSDK_PHY84728_AN_MII_CTRL_RAN);
    } else {
        data &= ~(BSDK_PHY84728_AN_MII_CTRL_AE | 
                  BSDK_PHY84728_AN_MII_CTRL_RAN);
    }
    SOC_IF_ERROR_RETURN(
        BSDK_WR_PHY84728_LN_DEV7_AN_MII_CTRLr(pc, data));

    if(SOC_SUCCESS(rv)) {
        if(PHY_COPPER_MODE(unit, port)) {
            pc->copper.autoneg_enable = autoneg ? TRUE : FALSE;
        } else {
            if(PHY_FIBER_MODE(unit, port)) {
                pc->fiber.autoneg_enable = autoneg ? TRUE : FALSE;
            }
        }
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84728_an_set: u=%d p=%d autoneg=%d rv=%d\n"),
              unit, port, autoneg, rv));
    return rv;
}

/*
 * Function:
 *      phy_84728_an_get
 * Purpose:
 *      Get the current auto-negotiation status (enabled/busy).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      autoneg - (OUT) if true, auto-negotiation is enabled.
 *      autoneg_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if autoneg == FALSE.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The autoneg mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_84728_an_get(int unit, soc_port_t port,
                     int *autoneg, int *autoneg_done)
{
    int           rv;
    phy_ctrl_t   *pc;
#if 0
    uint16 an_ctrl = 0, an_stat = 0;
#endif
    uint16 an_mii_ctrl = 0, an_mii_stat = 0;
    bsdk_phy84728_intf_t line_mode;

    pc = EXT_PHY_SW_STATE(unit, port);

    rv            = SOC_E_NONE;
    *autoneg      = FALSE;
    *autoneg_done = FALSE;

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG2(pc, PHY_AUTO_NEGOTIATE_GET, autoneg, autoneg_done);
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(
        bsdk_phy84728_line_intf_get(pc, 0, &line_mode));

    /* revisit for KR */
#if 0
    if (line_mode == BSDK_PHY84728_INTF_SFI) {
        SOC_IF_ERROR_RETURN(
            BSDK_RD_PHY84728_LN_DEV7_AN_CTRLr(pc, &an_ctrl));
        SOC_IF_ERROR_RETURN(
            BSDK_RD_PHY84728_LN_DEV7_AN_STATr(pc, &an_stat));
        *autoneg = (an_ctrl & BSDK_PHY84728_AN_AN_CTRL_AE) ? 1 : 0;
        *autoneg_done = (an_stat & BSDK_PHY84728_AN_AN_STAT_AN_DONE) ? 1 : 0;
    } else {
#endif
    {
        SOC_IF_ERROR_RETURN(
            BSDK_RD_PHY84728_LN_DEV7_AN_MII_CTRLr(pc, &an_mii_ctrl));
        SOC_IF_ERROR_RETURN(
            BSDK_RD_PHY84728_LN_DEV7_AN_MII_STATr(pc, &an_mii_stat));
        *autoneg = (an_mii_ctrl & BSDK_PHY84728_AN_MII_CTRL_AE) ? 1 : 0;
        *autoneg_done = 
            (an_mii_stat & BSDK_PHY84728_AN_MII_STAT_AN_DONE) ? 1 : 0;
    }

    return rv;
}


STATIC int
_phy_54942_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc; 

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (BSDK_PHY84728_REG_MOD(pc,
               0x0, 0x4, 0, enable ? (1U << 14) : 0, (1U << 14)));

    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_84728_lb_set
 * Purpose:
 *      Set the local PHY loopback mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      loopback - Boolean: true = enable loopback, false = disable.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The loopback mode is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_84728_lb_set(int unit, soc_port_t port, int enable)
{
    int           rv;
    phy_ctrl_t    *pc;
    uint16 data = 0;
    bsdk_phy84728_intf_t line_mode;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_LOOPBACK_SET,enable);
        if (enable) {
            SOC_IF_ERROR_RETURN
                (phy_84728_link_up(unit, port));
        }
        return SOC_E_NONE;
    }

#if 1
    /* Disable(Enable) Squelch when we Enable(Disable) loopback */
    SOC_IF_ERROR_RETURN
        (_phy_84728_squelch_enable(unit, port, enable ? FALSE : TRUE));
#endif

    SOC_IF_ERROR_RETURN(
        bsdk_phy84728_line_intf_get(pc, 0, &line_mode));

    if (REVERSE_MODE(pc)) {
        SOC_IF_ERROR_RETURN
            (_phy_54942_lb_set(unit, port, enable));

        if (rv == SOC_E_NONE) {
            /* wait for link up when loopback is enabled */
            
            if (enable) {
                sal_usleep(2000000);
            }
        }
    } else {

        /* lock detect bypass */
        SOC_IF_ERROR_RETURN(
            BSDK_PHY84728_REG_MOD(pc, 0x00, 0x0001, 
                0xCA7B, enable ? (1<<14) : 0, (1<<14)));


        if (line_mode == BSDK_PHY84728_INTF_SFI) {
            SOC_IF_ERROR_RETURN(
                BSDK_RD_PHY84728_LN_DEV1_PMD_CTRLr(pc, &data));
            data &= ~(1);
            data |= (enable) ? 1 : 0x0000;
            SOC_IF_ERROR_RETURN(
                BSDK_WR_PHY84728_LN_DEV1_PMD_CTRLr(pc, data));
        } else {
            SOC_IF_ERROR_RETURN(
                BSDK_RD_PHY84728_LN_DEV7_AN_MII_CTRLr(pc, &data));
            data &= ~BSDK_PHY84728_AN_MII_CTRL_LE;
            data |= (enable) ? BSDK_PHY84728_AN_MII_CTRL_LE : 0;
            SOC_IF_ERROR_RETURN(
                BSDK_WR_PHY84728_LN_DEV7_AN_MII_CTRLr(pc, data));
            if (!enable) 
            { 
                SOC_IF_ERROR_RETURN( 
                BSDK_PHY84728_REG_MOD(pc, 0x00, 0x0001, 0xC8E4, (1 << 12), (1 << 12))); 
                SOC_IF_ERROR_RETURN( 
                BSDK_PHY84728_REG_MOD(pc, 0x00, 0x0001, 0xC8E4, (1 << 4), (1 << 4))); 
                SOC_IF_ERROR_RETURN( 
                BSDK_PHY84728_REG_MOD(pc, 0x00, 0x0001, 0xC8E4, 0, (1 << 12))); 
                SOC_IF_ERROR_RETURN( 
                BSDK_PHY84728_REG_MOD(pc, 0x00, 0x0001, 0xC8E4, 0, (1 << 4))); 
             } 
	}

        if (rv == SOC_E_NONE) {
            /* wait for link up when loopback is enabled */
            
            if (enable) {
                sal_usleep(2000000);
            }
        }
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84728_lb_set: u=%d p=%d en=%d rv=%d\n"), 
              unit, port, enable, rv));

    return rv; 
}

STATIC int
_phy_54942_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t *pc; 
    uint16 tmp;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (BSDK_PHY84728_REG_RD(pc, 0x0, 0x4, 0, &tmp));

    *enable = tmp & (1U << 14) ? TRUE : FALSE;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84728_lb_get
 * Purpose:
 *      Get the local PHY loopback mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      loopback - (OUT) Boolean: true = enable loopback, false = disable.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The loopback mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_84728_lb_get(int unit, soc_port_t port, int *enable)
{
    int                  rv;
    phy_ctrl_t    *pc;
    uint16 data = 0;
    bsdk_phy84728_intf_t line_mode;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_LOOPBACK_GET,enable);
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(
        bsdk_phy84728_line_intf_get(pc, 0, &line_mode));

    if (REVERSE_MODE(pc)) {
        SOC_IF_ERROR_RETURN
            (_phy_54942_lb_get(unit, port, enable));
    } else {
        if (line_mode == BSDK_PHY84728_INTF_SFI) {
            SOC_IF_ERROR_RETURN(
                BSDK_RD_PHY84728_LN_DEV1_PMD_CTRLr(pc, &data));
            *enable = (data & 1) ? 1 : 0;
        } else {
            SOC_IF_ERROR_RETURN(
                BSDK_RD_PHY84728_LN_DEV7_AN_MII_CTRLr(pc, &data));
            *enable = (data & BSDK_PHY84728_AN_MII_CTRL_LE) ? 1 : 0;
        }
    }

    return rv; 
}

STATIC int
_phy_84728_control_pdet_set(int unit, soc_port_t port, uint32 enable)
{
    uint16 mask;
    uint16 data;
    phy_ctrl_t *pc;   /* PHY software state */

    mask = PHY84728_PDET_MASK;
    data = enable ? mask : 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(
         BSDK_MOD_PHY84728_LN_DEV7_AN_BASE1000X_CTRL2r(pc, data, mask));
    
    return SOC_E_NONE;
}

STATIC int
_phy_84728_control_pdet_get(int unit, soc_port_t port, uint32 *value)
{
    uint16         data16;
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(
         BSDK_RD_PHY84728_LN_DEV7_AN_BASE1000X_CTRL2r(pc, &data16));

    if ((data16 & PHY84728_PDET_MASK) == PHY84728_PDET_MASK) {
        *value = 1; 
    } else {
        *value = 0; 
    }

    return SOC_E_NONE;
}


STATIC int
_phy_84728_control_edc_mode_get(int unit, soc_port_t port, uint32 *value)
{
    uint16         data16;
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    /* program EDC mode */
    SOC_IF_ERROR_RETURN
        (READ_PHY84728_PMA_PMD_REG(unit,pc, 0xCA1A,&data16));

    *value = data16 & PHY_84727_EDC_MODE_MASK;
    return SOC_E_NONE;
}

STATIC int
_phy_84728_control_edc_mode_set(int unit, soc_port_t port, uint32 value)
{
    uint16         data16;
    uint16         mask16;
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    /* EDC mode programming sequence*/
    mask16 = 1 << 9;

    /* induce LOS condition: toggle register bit 0xc800.9 */
    SOC_IF_ERROR_RETURN
        (READ_PHY84728_PMA_PMD_REG(unit,pc, 0xC800,&data16));
    /* only change toggled bit 9 */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84728_PMA_PMD_REG(unit,pc, 0xC800,~data16,mask16));
    
    /* program EDC mode */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84728_PMA_PMD_REG(unit,pc, 0xCA1A,(uint16)value,
                                    PHY_84727_EDC_MODE_MASK));
    
    /* remove LOS condition: restore back original value of bit 0xc800.9 */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84728_PMA_PMD_REG(unit,pc, 0xC800,data16,mask16));

    return SOC_E_NONE;
}

STATIC int
_phy_84728_mode_set(int unit, soc_port_t port, uint32 mode) {
    uint16      dev_reg, mask;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    switch(mode) {
        case WAN_MODE:
            if (PHY_WAN_MODE(unit, port)) {
                /* Already in WAN mode */ 
                return SOC_E_NONE;
            }
            SOC_IF_ERROR_RETURN
                (BSDK_RD_PHY84728_WIS_DEV_IN_PKGr(pc,  &dev_reg));
#if 0
            /* Check WIS present */ 
            if (!(dev_reg & 0x0004)) {
                return SOC_E_UNAVAIL;
            }
#endif
            /* To configure WAN mode clock, */
            mask = PMD_MISC_CTRL_WAN_MODE;
            SOC_IF_ERROR_RETURN
                (BSDK_MOD_PHY84728_PMD_MISC_CTRLr(pc, PMD_MISC_CTRL_WAN_MODE, mask));

            PHY_FLAGS_SET(unit, port, PHY_FLAGS_WAN);
            break;

        case LAN_MODE:
#if 0
            if (!PHY_WAN_MODE(unit, port)) {
                /* Already in LAN mode */
                return SOC_E_NONE;
            } 
#endif

            /* To configure LAN mode clock, */ 
            mask = PMD_MISC_CTRL_WAN_MODE;
           
            /* put device in LAN mode */
            SOC_IF_ERROR_RETURN
                (BSDK_MOD_PHY84728_PMD_MISC_CTRLr(pc, 0, mask));

            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_WAN);
            break;
        default:
            return SOC_E_UNAVAIL;
    }
 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84728_mode_set: u=%d port=%d mode=%d\n"),
              unit, port, mode));

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_84728_medium_config_set
 * Purpose:
 *      Set the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *      cfg - Operating parameters
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_84728_medium_config_set(int unit, soc_port_t port, 
                           soc_port_medium_t  medium,
                           soc_phy_config_t  *cfg)
{
    phy_ctrl_t    *pc;
#if 0
    soc_phy_config_t *active_medium;  /* Currently active medium */
#endif
    soc_phy_config_t *change_medium;  /* Requested medium */
    soc_phy_config_t *other_medium;   /* The other medium */

    soc_port_mode_t   advert_mask;

    if (NULL == cfg) {
        return SOC_E_PARAM;
    }

    pc            = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG2(pc, PHY_MEDIUM_CONFIG_SET, medium, cfg);
        return SOC_E_NONE;
    }

   switch (medium) {
    case SOC_PORT_MEDIUM_COPPER:
        if (!pc->automedium && !PHY_COPPER_MODE(unit, port)) {
            return SOC_E_UNAVAIL;
        }
        change_medium  = &pc->copper;
        other_medium   = &pc->fiber;
        advert_mask    = ADVERT_ALL_COPPER;
        break;
    case SOC_PORT_MEDIUM_FIBER:
        if (!pc->automedium && !PHY_FIBER_MODE(unit, port)) {
            return SOC_E_UNAVAIL;
        }
        change_medium  = &pc->fiber;
        other_medium   = &pc->copper;
        advert_mask    = ADVERT_ALL_FIBER;
        break;
    default:
        return SOC_E_PARAM;
    }

    /*
     * Changes take effect immediately if the target medium is active or
     * the preferred medium changes.
     */
    if (change_medium->preferred != cfg->preferred) {
        /* Make sure that only one medium is preferred */
        other_medium->preferred = !cfg->preferred;
    }

    sal_memcpy(change_medium, cfg, sizeof(*change_medium));
    change_medium->autoneg_advert &= advert_mask;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84728_medium_config_get
 * Purpose:
 *      Get the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *      cfg - (OUT) Operating parameters
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_84728_medium_config_get(int unit, soc_port_t port, 
                           soc_port_medium_t medium,
                           soc_phy_config_t *cfg)
{
    int            rv;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG2(pc, PHY_MEDIUM_CONFIG_GET, medium, cfg);
        return SOC_E_NONE;
    }

    switch (medium) {
    case SOC_PORT_MEDIUM_COPPER:
        if (pc->automedium || PHY_COPPER_MODE(unit, port)) {
            sal_memcpy(cfg, &pc->copper, sizeof (*cfg));
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    case SOC_PORT_MEDIUM_FIBER:
        if (pc->automedium || PHY_FIBER_MODE(unit, port)) {
            sal_memcpy(cfg, &pc->fiber, sizeof (*cfg));
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    default:
        rv = SOC_E_PARAM;
        break;
    }

    return rv;
}

/*
 * Function:
 *      phy_84728_ability_advert_set
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
phy_84728_ability_advert_set(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    int        rv;
    phy_ctrl_t *pc;
    bsdk_phy84728_port_ability_t local_ability;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_ABILITY_ADVERT_SET,ability);
        return SOC_E_NONE;
    }

    if (PHY_COPPER_MODE(unit, port)) {
        pc->copper.advert_ability = *ability;
    } else {
        pc->fiber.advert_ability = *ability;
    }

    local_ability = 0;

    if(ability->speed_full_duplex & SOC_PA_SPEED_1000MB) {
        local_ability |= BSDK_PHY84728_PA_1000MB_FD;
    }
    if(ability->speed_full_duplex & SOC_PA_SPEED_10GB) {
        local_ability |= BSDK_PHY84728_PA_10000MB_FD;
    }

    if(ability->pause & SOC_PA_PAUSE_TX) {
        local_ability |= BSDK_PHY84728_PA_PAUSE_TX;
    }
    if(ability->pause & SOC_PA_PAUSE_RX) {
        local_ability |= BSDK_PHY84728_PA_PAUSE_RX;
    }
    if(ability->pause & SOC_PA_PAUSE_ASYMM) {
        local_ability |= BSDK_PHY84728_PA_PAUSE_ASYMM;
    }

    if(ability->loopback & SOC_PA_LB_NONE) {
        local_ability |= BSDK_PHY84728_PA_LB_NONE;
    }
    if(ability->loopback & SOC_PA_LB_PHY) {
        local_ability |= BSDK_PHY84728_PA_LB_PHY;
    }
    if(ability->flags & SOC_PA_AUTONEG) {
        local_ability |= BSDK_PHY84728_PA_AN;
    }

    rv = bsdk_phy84728_ability_advert_set(pc, local_ability);

    return rv;
}

/*
 * Function:
 *      phy_84728_ability_advert_get
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
phy_84728_ability_advert_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    int        rv;
    bsdk_phy84728_port_ability_t local_ability;

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_ABILITY_ADVERT_GET,ability);
        return SOC_E_NONE;
    }

    local_ability = 0;

    rv = bsdk_phy84728_ability_advert_get(pc, &local_ability);
    if (rv != SOC_E_NONE) {
        return SOC_E_FAIL;
    }

    if(local_ability & BSDK_PHY84728_PA_1000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
    }
    if(local_ability & BSDK_PHY84728_PA_10000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
    }

    if(local_ability & BSDK_PHY84728_PA_PAUSE_TX) {
        ability->pause |= SOC_PA_PAUSE_TX;
    }
    if(local_ability & BSDK_PHY84728_PA_PAUSE_RX) {
        ability->pause |= SOC_PA_PAUSE_RX;
    }
    if(local_ability & BSDK_PHY84728_PA_PAUSE_ASYMM) {
        ability->pause |= SOC_PA_PAUSE_ASYMM;
    }

    if(local_ability & BSDK_PHY84728_PA_LB_NONE) {
        ability->loopback |= SOC_PA_LB_NONE;
    }
    if(local_ability & BSDK_PHY84728_PA_LB_PHY) {
        ability->loopback |= SOC_PA_LB_PHY;
    }
    if(local_ability & BSDK_PHY84728_PA_AN) {
        ability->flags = SOC_PA_AUTONEG;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84728_ability_remote_get
 * Purpose:
 *      Get the current remote advertisement for auto-negotiation.
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
phy_84728_ability_remote_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    int        rv;
    bsdk_phy84728_port_ability_t remote_ability;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_ABILITY_REMOTE_GET,ability);
        return SOC_E_NONE;
    }

    remote_ability = 0;

    rv = bsdk_phy84728_remote_ability_advert_get(pc,
                                                   &remote_ability);
    if (rv != SOC_E_NONE) {
        return SOC_E_FAIL;
    }

    if(remote_ability & BSDK_PHY84728_PA_1000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
    }
    if(remote_ability & BSDK_PHY84728_PA_10000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
    }

    if(remote_ability & BSDK_PHY84728_PA_PAUSE_TX) {
        ability->pause |= SOC_PA_PAUSE_TX;
    }
    if(remote_ability & BSDK_PHY84728_PA_PAUSE_RX) {
        ability->pause |= SOC_PA_PAUSE_RX;
    }
    if(remote_ability & BSDK_PHY84728_PA_PAUSE_ASYMM) {
        ability->pause |= SOC_PA_PAUSE_ASYMM;
    }

    if(remote_ability & BSDK_PHY84728_PA_LB_NONE) {
        ability->loopback |= SOC_PA_LB_NONE;
    }
    if(remote_ability & BSDK_PHY84728_PA_LB_PHY) {
        ability->loopback |= SOC_PA_LB_PHY;
    }
    if(remote_ability & BSDK_PHY84728_PA_AN) {
        ability->flags = SOC_PA_AUTONEG;
    }

    return SOC_E_NONE;
}



/*
 * Function:
 *      phy_84728_ability_local_get
 * Purpose:
 *      Get local abilities 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The ability is retrieved only for the ACTIVE medium.
 */

STATIC int
phy_84728_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    int         rv = SOC_E_NONE;
    phy_ctrl_t *pc;
    bsdk_phy84728_port_ability_t local_ability = 0;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_ABILITY_LOCAL_GET,ability);
        return SOC_E_NONE;
    }

    rv = bsdk_phy84728_ability_local_get(pc, &local_ability);
    if (rv != SOC_E_NONE) {
        return SOC_E_FAIL;
    }

    if(local_ability & BSDK_PHY84728_PA_1000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
    }
    if(local_ability & BSDK_PHY84728_PA_10000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
    }

    if(local_ability & BSDK_PHY84728_PA_PAUSE_TX) {
        ability->pause |= SOC_PA_PAUSE_TX;
    }
    if(local_ability & BSDK_PHY84728_PA_PAUSE_RX) {
        ability->pause |= SOC_PA_PAUSE_RX;
    }
    if(local_ability & BSDK_PHY84728_PA_PAUSE_ASYMM) {
        ability->pause |= SOC_PA_PAUSE_ASYMM;
    }
    if(local_ability & BSDK_PHY84728_PA_LB_NONE) {
        ability->loopback |= SOC_PA_LB_NONE;
    }
    if(local_ability & BSDK_PHY84728_PA_LB_PHY) {
        ability->loopback |= SOC_PA_LB_PHY;
    }
    if(local_ability & BSDK_PHY84728_PA_AN) {
        ability->flags = SOC_PA_AUTONEG;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_84728_init_ucode_bcst(int unit, int port, int cmd)
{
    uint16 data16;
    int j;
    uint16 num_words;
    uint8 *fw_ptr;
    int fw_length;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (cmd == PHYCTRL_UCODE_BCST_SETUP) {

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY84740 BCST start: u=%d p=%d\n"), unit, port));
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc8fe, 0xffff));
        return SOC_E_NONE;

    } else if (cmd == PHYCTRL_UCODE_BCST_uC_SETUP) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY84740 BCST1: u=%d p=%d\n"), unit, port));
        /* 0xc848[15]=0, MDIO downloading to RAM, 0xc848[14]=1, serial boot */
        /* 0xc848[13]=0, SPI-ROM downloading not done, 0xc848[2]=0, spi port enable */
        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc848,
                                           (0 << 15)|(1 << 14),
                                           (1 << 15)|(1 << 14)|((1 << 13) | (1 << 2))));

        /* apply bcst Reset to start download code from MDIO */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMD_CTRLr(pc, 0x8000));
        return SOC_E_NONE;
    } else if (cmd == PHYCTRL_UCODE_BCST_ENABLE) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY84740 BCST2: u=%d p=%d\n"), unit, port));
        /* reset clears bcst register */
        /* restore bcst register after reset */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc8fe, 0xffff));
        return SOC_E_NONE;

    } else if (cmd == PHYCTRL_UCODE_BCST_LOAD) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "firmware_bcst,device name %s: u=%d p=%d\n"),
                             pc->dev_name? pc->dev_name: "NULL", unit, port));
        /* find the right firmware */
        /* MDIO based firmware download to RAM */
        if (BSDK_PHY84728_IS_84728(PHY84728_MAP(pc)->devid)) {
            fw_ptr = bcm84728_fw;
            fw_length = bcm84728_fw_length;
        } else { /* non-84728 device */
            fw_ptr = bcm84748_fw;
            fw_length = bcm84748_fw_length;
        }

        /* wait for 2ms for M8051 start and 5ms to initialize the RAM */
        sal_usleep(10000); /* Wait for 10ms */

        /* Write Starting Address, where the Code will reside in SRAM */
        data16 = 0x8000;
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_M8501_MSGINr(pc, data16));

        /* make sure address word is read by the micro */
        sal_udelay(10); /* Wait for 10us */

        /* Write SPI SRAM Count Size */
        data16 = (fw_length)/2;
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_M8501_MSGINr(pc, data16));

        /* make sure read by the micro */
        sal_udelay(10); /* Wait for 10us */

        /* Fill in the SRAM */
        num_words = (fw_length - 1);
        for (j = 0; j < num_words; j+=2) {
            /* Make sure the word is read by the Micro */
            sal_udelay(10);
            data16 = (fw_ptr[j] << 8) | fw_ptr[j+1];
            SOC_IF_ERROR_RETURN
                (BSDK_WR_PHY84728_LN_DEV1_M8501_MSGINr(pc, data16));
        }
        return SOC_E_NONE;

    } else if (cmd == PHYCTRL_UCODE_BCST_END) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY84740 BCST end: u=%d p=%d\n"), unit, port));

        /* make sure last code word is read by the micro */
        sal_udelay(20);

        /* first disable bcst mode */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc8fe, 0));
        
        /* Read Hand-Shake message (Done) from Micro */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_M8501_MSGOUTr(pc, &data16));
        if (data16 != 0x4321 ) {
            /* Download done message */
            LOG_CLI((BSL_META_U(unit,
                                "MDIO firmware download failed. Message: 0x%x\n"),
                     data16));
            return SOC_E_FAIL;
        }

        /* Download done message */
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "u=%d p=%d MDIO firmware download done message: 0x%x\n"),
                             unit, port,data16));
        
        /* Clear LASI status */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_RX_ALARMr(pc, &data16));

        /* Wait for LASI to be asserted when M8051 writes checksum to MSG_OUTr */
        sal_udelay(100); /* Wait for 100 usecs */

        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_M8501_MSGOUTr(pc, &data16));

        /* Need to check if checksum is correct */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_CHECKSUMr(pc, &data16));

        if (data16 != 0x600D) {
            /* Bad CHECKSUM */
            LOG_CLI((BSL_META_U(unit,
                                "firmware_bcst downlad failure: port %d "
                                "Incorrect Checksum %x\n"), port, data16));
            return SOC_E_FAIL;
        }

        /* read Rev-ID */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xCA1A, &data16));
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "u=%d p=%d MDIO Firmware broadcast download revID: 0x%x\n"),
                              unit, port,data16));

        return SOC_E_NONE;

    } else {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "u=%d p=%d firmware_bcst: invalid cmd 0x%x\n"),
                             unit, port,cmd));
    }

    return SOC_E_FAIL;
}


/*
 * Function:
 *      phy_84728_firmware_set
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
phy_84728_firmware_set(int unit, int port, int offset, uint8 *data, int len)
{
    int            rv;
    phy_ctrl_t *pc;

    if (data == NULL) {
        return _phy_84728_init_ucode_bcst(unit, port, offset);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    rv = bsdk_phy84728_spi_firmware_update(pc, data, len);
    if (rv != SOC_E_NONE) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY84728 firmware upgrade possibly failed:"
                                 "u=%d p=%d\n"), unit, port));
        return (SOC_E_FAIL);
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY84728 firmware upgrade successful:"
                         "u=%d p=%d\n"), unit, port));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84728_medium_status
 * Purpose:
 *      Indicate the current active medium
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) One of:
 *              SOC_PORT_MEDIUM_COPPER
 *              SOC_PORT_MEDIUM_XAUI
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_84728_medium_status(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_MEDIUM_GET, medium);
        return SOC_E_NONE;
    }
    if (PHY_COPPER_MODE(unit, port)) {
        *medium = SOC_PORT_MEDIUM_COPPER;
    } else {
        *medium = SOC_PORT_MEDIUM_FIBER;
    }


    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_84728_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to update
 *      value - New setting for the control
 * Returns:    
 *      SOC_E_NONE
 */
STATIC int
phy_84728_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;
    phy_ctrl_t *pc;
    uint16 regval = 0;

    PHY_CONTROL_TYPE_CHECK(type);
    
    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_UNAVAIL;    

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG2(pc, PHY_CONTROL_SET, type, value);
        return SOC_E_NONE;
    }

    switch(type) {
        case SOC_PHY_CONTROL_WAN:
            rv = _phy_84728_mode_set(unit, port, 
                                       value ? WAN_MODE : LAN_MODE);
            break;

        case SOC_PHY_CONTROL_EDC_MODE:
            rv = _phy_84728_control_edc_mode_set(unit,port,value);
            break;    

        case SOC_PHY_CONTROL_CLOCK_ENABLE:

            if(value) {
                regval = (1 << 10);
            }
            SOC_IF_ERROR_RETURN
                (BSDK_PHY84728_REG_MOD(pc, 0x01,0x01,0xca23,regval,0x400));

            rv = SOC_E_NONE;
            break;

        case SOC_PHY_CONTROL_CLOCK_FREQUENCY:

            switch(value)
            {
                case 125000: /* for 125 MHz */
                    regval = 1;
                    break;
                case 19531: /* for 19.53125 MHz */
                    regval = 2;
                    break;
                case 156250:
                    regval = 3; /* for 156.25 MHz */
                    break;
                default:
                    return SOC_E_PARAM;
                    break;
            }
            regval = (regval << 14); /* programming the frequency */
            regval |= (1 << 13);

            SOC_IF_ERROR_RETURN
                (BSDK_PHY84728_REG_MOD(pc, 0x01,0x01,0xcace,regval, 0xe000));
            rv = SOC_E_NONE;
            break;
        case SOC_PHY_CONTROL_PARALLEL_DETECTION: 
            rv = _phy_84728_control_pdet_set(unit, port, value);
            break;

        default:
            break; 
    }
    return rv;
}

/*
 * Function:
 *      phy_84728_control_get
 * Purpose:
 *      Get current control settign of the PHY.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to update
 *      value - (OUT)Current setting for the control
 * Returns:
 *      SOC_E_NONE
 */
STATIC int 
phy_84728_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int    rv;
    uint16 temp16;
    phy_ctrl_t *pc;

    if (NULL == value) {
        return SOC_E_PARAM;
    }

    PHY_CONTROL_TYPE_CHECK(type);

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_UNAVAIL;

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG2(pc, PHY_CONTROL_GET, type, value);
        return SOC_E_NONE;
    }

    switch(type) {
        case SOC_PHY_CONTROL_WAN:
            *value = PHY_WAN_MODE(unit, port) ? 1 : 0;   
            rv = SOC_E_NONE;
            break;

        case SOC_PHY_CONTROL_EDC_MODE:
            rv = _phy_84728_control_edc_mode_get(unit,port,value);
            break;    

        case SOC_PHY_CONTROL_CLOCK_ENABLE:

            {
                SOC_IF_ERROR_RETURN
                (BSDK_PHY84728_REG_RD(pc, 0x01,0x01,0xca23,&temp16));
                *value = (temp16 & (0x1 << 10))? TRUE : FALSE;
            }
            rv = SOC_E_NONE;
            break;

        case SOC_PHY_CONTROL_CLOCK_FREQUENCY:


            SOC_IF_ERROR_RETURN
            (BSDK_PHY84728_REG_RD(pc, 0x01,0x01,0xcace,&temp16));

            if(temp16 & (0x1 << 13))
            {
                switch(temp16 >> 14)
                {
                    case 1: /* for 125 MHz */
                        *value = 125000;
                        break;
                    case 2: /* for 19.53125 MHz */
                        *value = 19531;
                        break;
                    case 3:
                        *value = 156250; /* for 156.25 MHz */
                        break;
                    default:
                        *value = 0;
                        break;
                }
                if(*value) {
                    rv = SOC_E_NONE;
                } else {
                    rv = SOC_E_UNAVAIL;
                }
            } else {
                rv = SOC_E_DISABLED;
            }
            break;
        case SOC_PHY_CONTROL_PARALLEL_DETECTION:
            rv = _phy_84728_control_pdet_get(unit, port, value);
            break; 

        default:
            break;
    }
    return rv;
}

/*
 * Function:
 *      phy_84728_link_up
 * Purpose:
 *      Link up handler
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int 
phy_84728_link_up(int unit, soc_port_t port)
{

    /* After link up configure the system side to operate at 10G */
    phy_ctrl_t *pc;
    bsdk_phy84728_intf_t line_mode;
    int speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_84728_speed_get(unit, port, &speed));

    if (NXT_PC(pc)) {
        PHYDRV_CALL_NOARG(pc, PHY_LINKUP_EVT);
        /* PHYDRV_CALL_ARG1(pc,PHY_SPEED_GET, &speed); */
        if (speed == 10000) {
            line_mode = BSDK_PHY84728_INTF_SFI;
            SOC_IF_ERROR_RETURN(
                bsdk_phy84728_sys_mode_set(pc, 0, 
                                 BSDK_PHY84728_INTF_XAUI));
        } else if (speed == 1000) {
            if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_COPPER)) {
                line_mode = BSDK_PHY84728_INTF_SGMII;
                SOC_IF_ERROR_RETURN(
                    bsdk_phy84728_sys_mode_set(pc, 0, 
                                     BSDK_PHY84728_INTF_SGMII));
            } else {
                line_mode = BSDK_PHY84728_INTF_1000X;
                SOC_IF_ERROR_RETURN(
                    bsdk_phy84728_sys_mode_set(pc, 0, 
                                     BSDK_PHY84728_INTF_1000X));
            }
        } else {
            line_mode = BSDK_PHY84728_INTF_SGMII;
            SOC_IF_ERROR_RETURN(
                bsdk_phy84728_sys_mode_set(pc, 0, 
                                 BSDK_PHY84728_INTF_SGMII));
        }
        SOC_IF_ERROR_RETURN
            (bsdk_phy84728_line_mode_set(pc, 0, line_mode));

        SOC_IF_ERROR_RETURN
            (bsdk_phy84728_speed_set(pc, speed));
    }

    SOC_IF_ERROR_RETURN(
        bsdk_phy84728_line_intf_get(pc, 0, &line_mode));

    if (line_mode == BSDK_PHY84728_INTF_SGMII) {
        SOC_IF_ERROR_RETURN(
            _bsdk_phy84728_system_sgmii_sync(pc, 0));
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventInterface, SOC_PORT_IF_SGMII));
    }
#if 0   /* don't need to notify if speed is 10G */
    else {
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventInterface, SOC_PORT_IF_XAUI));
    }
#endif

    if (PHY84728_MAP(pc)->link_down) {      /* was link down */
        if ( speed < 10000 ) {    /* don't need to notify if speed is 10G */
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, phyEventSpeed, speed));
        }
        PHY84728_MAP(pc)->link_down = 0;    /* link is up now */
    }

    if ((line_mode == BSDK_PHY84728_INTF_SGMII) || (line_mode == BSDK_PHY84728_INTF_1000X)) {
        SOC_IF_ERROR_RETURN(
            bsdk_phy84728_lane_map(pc));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84728_link_down
 * Purpose:
 *      Link down handler
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_84728_link_down(int unit, soc_port_t port)
{

    /* After link up configure the system side to operate at 10G */
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY84728_MAP(pc)->link_down = 1;
    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_84728_interface_get
 * Purpose:
 *      Get interface on a given port.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      pif   - Interface.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_84728_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    phy_ctrl_t *pc;
    bsdk_phy84728_intf_t line_mode;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(
        bsdk_phy84728_line_intf_get(pc, 0, &line_mode));

    if (line_mode == BSDK_PHY84728_INTF_SGMII) {
        *pif =  SOC_PORT_IF_SGMII;
    } else {
        *pif =  SOC_PORT_IF_XGMII;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84728_probe
 * Purpose:
 *      Complement the generic phy probe routine to identify this phy when its
 *      phy id0 and id1 is same as some other phy's.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pc   - phy ctrl descriptor.
 * Returns:
 *      SOC_E_NONE,SOC_E_NOT_FOUND and SOC_E_<error>
 */
STATIC int
phy_84728_probe(int unit, phy_ctrl_t *pc)
{
    uint16  data1, data2;

    if (READ_PHY84728_PMA_PMD_REG(unit, pc, 0xc802, &data1) < 0) {
        return SOC_E_NOT_FOUND;
    }

    if (READ_PHY84728_PMA_PMD_REG(unit, pc, 0xc803, &data2) < 0) {
        return SOC_E_NOT_FOUND;
    }

    if ((data2 & 0xf) == 0x8) {

        switch (data1) {

            case 0x4707:
            case 0x4073:
            case 0x4074:
            case 0x4728:
            case 0x4748:
            case 0x4727:
            case 0x4747:
            case 0x4722:
            case 0x4742:
            case 0x4762:
            case 0x4764:
            case 0x4042:
            case 0x4044:

                pc->size = sizeof(phy84728_map_t);
                return SOC_E_NONE;
        }
    }

    if ((data2 & 0xf) == 0x5) {

        switch (data1) {

            case 0x4942:

                pc->size = sizeof(phy84728_map_t);
                return SOC_E_NONE;
        }
    }

    return SOC_E_NOT_FOUND;
}

/*
 * Function:     
 *    _phy84728_get_model_rev
 * Purpose:    
 *    Get OUI, Model and Revision of the PHY
 * Parameters:
 *    phy_dev_addr - PHY Device Address
 *    oui          - (OUT) Organization Unique Identifier
 *    model        - (OUT)Device Model number`
 *    rev          - (OUT)Device Revision number
 * Notes: 
 */
int 
_phy84728_get_model_rev(phy_ctrl_t *pc,
                       int *oui, int *model, int *rev)
{
    uint16  id0, id1;

    SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMD_ID0r(pc, &id0));

    SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMD_ID1r(pc, &id1));

    *oui   = BSDK_PHY84728_PHY_OUI(id0, id1);
    *model = BSDK_PHY84728_PHY_MODEL(id0, id1);
    *rev   = BSDK_PHY84728_PHY_REV(id0, id1);

    return SOC_E_NONE;
}



/*
 * Function:
 *     bsdk_phy84728_firmware_spi_download
 * Purpose:
 *     Download firmware via SPI
 *         - Check if firmware download is complete
 *         - Check if the checksum is good
 * Parameters:
 *    phy_id    - PHY Device Address
 * Returns:
 */
/* STATIC int */
int
bsdk_phy84728_firmware_spi_download(phy_ctrl_t *pc)
{
    uint16 data = 0;
    int i;

    /* 0xc848[15]=1, SPI-ROM downloading to RAM, 0xc848[14]=1, serial boot */
    /* 0xc848[13]=0, SPI-ROM downloading not done, 0xc848[2]=0, spi port enable */
    SOC_IF_ERROR_RETURN(
           BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc848, 
               (1 << 15)|(1 << 14), 
               ((1 << 15)|(1 << 14)|(1 << 13)|(1 << 2))));

    /* apply software reset to download code from SPI-ROM */
    /* Reset the PHY **/
    SOC_IF_ERROR_RETURN(
           BSDK_MOD_PHY84728_LN_DEV1_PMD_CTRLr(pc, 
               BSDK_PHY84728_PMD_CTRL_RESET, BSDK_PHY84728_PMD_CTRL_RESET));

    for (i = 0; i < 5; i++) {
        sal_usleep(100000);
        SOC_IF_ERROR_RETURN(
               BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc848, &data));
        if (data & 0x2000) {  /* Check for download complete */
            /* Need to check if checksum is correct */
            SOC_IF_ERROR_RETURN(
                   BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xca1c, &data));
            if (data == 0x600D) {
                break;
            }
        }
    }
    if (i >= 5) { /* Bad CHECKSUM */
        LOG_CLI((BSL_META_U(pc->unit,
                            "SPI-Download Firmware download failure:"
                            "Incorrect Checksum %x\n"), data));
        return SOC_E_FAIL;
    }

    SOC_IF_ERROR_RETURN(
           BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xca1a, &data));

    LOG_CLI((BSL_META_U(pc->unit,
                        "BCM84728: u=%d p=%d: SPI-ROM download complete: "
                        "Version: 0x%x\n"), 
             pc->unit, pc->port, data));

    return SOC_E_NONE;
}

/*
 * Function:     
 *    bsdk_phy84728_line_intf_get
 * Purpose:    
 *    Determine the Line side Interface
 * Parameters:
 *    phy_id    - PHY's device address
 *    dev_port  - Channel number 
 *    mode      - (OUT) Line Port mode status
 * Returns:    
 */
int 
bsdk_phy84728_line_intf_get(phy_ctrl_t *pc, int dev_port, 
                              bsdk_phy84728_intf_t *mode)
{

    phy84728_map_t      *phy_info;

    if (mode == NULL) {
        return SOC_E_PARAM;
    }

    phy_info = PHY84728_MAP(pc);

    *mode = phy_info->line_mode;
    return SOC_E_NONE;
}



/*
 * Function:     
 *    bsdk_phy84728_line_mode_set
 * Purpose:    
 *    Set the Line side mode
 * Parameters:
 *    phy_id    - PHY's device address
 *    dev_port  - Channel number 
 *    mode      - Desired mode (SGMII/1000x/SFI)
 * Returns:    
 */
int
bsdk_phy84728_line_mode_set(phy_ctrl_t *pc, int dev_port, 
                              bsdk_phy84728_intf_t mode)
{

    int rv = SOC_E_NONE;
    phy84728_map_t      *phy_info;

    phy_info = PHY84728_MAP(pc);

    /* Disable Autodetect */
    SOC_IF_ERROR_RETURN(
        BSDK_MOD_PHY84728_LN_DEV7_AN_BASE1000X_CTRL1r(pc, 0,
                       BSDK_PHY84728_AN_BASE1000X_CTRL1_AUTODETECT_EN));
    switch (mode) {

    case BSDK_PHY84728_INTF_SGMII :

        SOC_IF_ERROR_RETURN(
            BSDK_WR_PHY84728_LN_DEV7_AN_BASE1000X_CTRL1r(pc, BSDK_PHY84728_AN_BASE1000X_CTRL1_COMMA_DET_EN |
                                                             BSDK_PHY84728_AN_BASE1000X_CTRL1_CRC_CHK_DIS |
                                                             BSDK_PHY84728_AN_BASE1000X_CTRL1_SD_ENABLE));
        SOC_IF_ERROR_RETURN(
            BSDK_WR_PHY84728_LN_DEV7_AN_MISC1r(pc, 0x6000));
        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_LN_DEV7_AN_MSIC2r(pc, BSDK_PHY84728_AN_MISC2_RX_MII_GEN_SEL, 
                                                    BSDK_PHY84728_AN_MISC2_RX_MII_GEN_SEL |
                                                    BSDK_PHY84728_AN_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN));
        SOC_IF_ERROR_RETURN(
            BSDK_WR_PHY84728_LN_DEV7_AN_SGMII_BASEPAGEr(pc, 0x0801));
        SOC_IF_ERROR_RETURN(
            BSDK_WR_PHY84728_LN_DEV7_AN_MII_CTRLr(pc, BSDK_PHY84728_AN_MII_CTRL_AE |
                                                      BSDK_PHY84728_AN_MII_CTRL_FD |
                                                      BSDK_PHY84728_AN_MII_CTRL_SS_1000));
        break;
        
    case BSDK_PHY84728_INTF_1000X :
        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_LN_DEV7_AN_BASE1000X_CTRL1r(pc, BSDK_PHY84728_AN_BASE1000X_CTRL1_FIBER_MODE,
                     BSDK_PHY84728_AN_BASE1000X_CTRL1_FIBER_MODE));
            /* Set PMA speed */
            SOC_IF_ERROR_RETURN(
                BSDK_WR_PHY84728_LN_DEV1_PMD_CTRLr(pc, BSDK_PHY84728_PMD_CTRL_SS_1000));
        break;

    case BSDK_PHY84728_INTF_SFI :
        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_LN_DEV7_AN_BASE1000X_CTRL1r(pc, BSDK_PHY84728_AN_BASE1000X_CTRL1_FIBER_MODE,
                     BSDK_PHY84728_AN_BASE1000X_CTRL1_FIBER_MODE));
        /* Set PMA speed */
        SOC_IF_ERROR_RETURN(
            BSDK_WR_PHY84728_LN_DEV1_PMD_CTRLr(pc, BSDK_PHY84728_PMD_CTRL_SS_10000));
        break;

    default :
        rv = SOC_E_PARAM;
        break;
        
    }

    if (SOC_SUCCESS(rv)) {
        phy_info->line_mode = mode;
    }
    return rv;
}


/*
 * Function:     
 *    bsdk_phy84728_sys_intf_get
 * Purpose:    
 *    Determine the System side Interface
 * Parameters:
 *    phy_id    - PHY's device address
 *    dev_port  - Channel number 
 *    mode      - (OUT) System side mode status
 * Returns:    
 */
int 
bsdk_phy84728_sys_intf_get(phy_ctrl_t *pc, int dev_port, 
                             bsdk_phy84728_intf_t *mode)
{

    phy84728_map_t      *phy_info;

    if (mode == NULL) {
        return SOC_E_PARAM;
    }

    phy_info = PHY84728_MAP(pc);

    /* System will always follow line side interface */
    *mode = phy_info->line_mode;

    return SOC_E_NONE;
}



/*
 * Function:     
 *    bsdk_phy84728_sys_mode_set
 * Purpose:    
 *    Set the system side mode
 * Parameters:
 *    phy_id    - PHY's device address
 *    dev_port  - Channel number 
 *    mode      - Desired mode (SGMII/1000x/XFI)
 * Returns:    
 */
int
bsdk_phy84728_sys_mode_set(phy_ctrl_t *pc, int dev_port, 
                              bsdk_phy84728_intf_t mode)
{
    int rv = SOC_E_NONE;

    /* Disable Autodetect */
    SOC_IF_ERROR_RETURN(
        BSDK_MOD_PHY84728_SYS_DEV4_XGXS_BASE1000X_CTRL1r(pc, 0,
                       BSDK_PHY84728_AN_BASE1000X_CTRL1_AUTODETECT_EN));
    switch (mode) {
    case BSDK_PHY84728_INTF_SGMII :
        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_SYS_DEV4_XGXS_CTRLr(pc, 0x0000, BSDK_PHY84728_AN_XGXS_CTRL_START_SEQ));

        SOC_IF_ERROR_RETURN(
            BSDK_WR_PHY84728_SYS_DEV4_XGXS_BASE1000X_CTRL1r(pc, BSDK_PHY84728_AN_BASE1000X_CTRL1_COMMA_DET_EN|
                                                            BSDK_PHY84728_AN_BASE1000X_CTRL1_CRC_CHK_DIS |
                                                            BSDK_PHY84728_AN_BASE1000X_CTRL1_SGMII_MASTER));

        SOC_IF_ERROR_RETURN(
            BSDK_WR_PHY84728_SYS_DEV4_XGXS_MISC1r(pc, 0x6000));

        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_SYS_DEV4_XGXS_MISC2r(pc, BSDK_PHY84728_AN_MISC2_RX_MII_GEN_SEL, 
                                                   BSDK_PHY84728_AN_MISC2_RX_MII_GEN_SEL |
                                                   BSDK_PHY84728_AN_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN));

        SOC_IF_ERROR_RETURN(
            BSDK_WR_PHY84728_SYS_DEV4_XGXS_SGMII_BASEPAGEr(pc, 0x0001));

        SOC_IF_ERROR_RETURN(
            BSDK_WR_PHY84728_SYS_DEV4_XGXS_MII_CTRLr(pc, BSDK_PHY84728_AN_MII_CTRL_FD |
                                                         BSDK_PHY84728_AN_MII_CTRL_SS_1000));

        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_SYS_DEV4_XGXS_CTRLr(pc, BSDK_PHY84728_AN_XGXS_CTRL_START_SEQ,
                                                      BSDK_PHY84728_AN_XGXS_CTRL_START_SEQ));

        break;
        
    case BSDK_PHY84728_INTF_1000X :
        SOC_IF_ERROR_RETURN(
            BSDK_WR_PHY84728_SYS_DEV4_XGXS_BASE1000X_CTRL1r(pc, BSDK_PHY84728_AN_BASE1000X_CTRL1_COMMA_DET_EN|
                                                            BSDK_PHY84728_AN_BASE1000X_CTRL1_CRC_CHK_DIS |
                                                            BSDK_PHY84728_AN_BASE1000X_CTRL1_FIBER_MODE));
        break;

    case BSDK_PHY84728_INTF_XAUI :
        SOC_IF_ERROR_RETURN(
            BSDK_WR_PHY84728_SYS_DEV4_XGXS_BASE1000X_CTRL1r(pc, BSDK_PHY84728_AN_BASE1000X_CTRL1_COMMA_DET_EN|
                                                            BSDK_PHY84728_AN_BASE1000X_CTRL1_CRC_CHK_DIS |
                                                            BSDK_PHY84728_AN_BASE1000X_CTRL1_FIBER_MODE));
        break;

    default :
        rv = SOC_E_PARAM;
        break;
    }

    return rv;
}


/*
 * Function:     
 *    bsdk_phy84728_no_reset_setup
 * Purpose:    
 *    Initialize the PHY without reset.
 * Parameters:
 *    phy_dev_addr        - PHY Device Address
 *    line_mode           - Line side Mode (SGMII/1000X/SFI).
 *    fcmap_enable        - Fibre Channel enabled or disabled.
 * Notes: 
 */
int
bsdk_phy84728_no_reset_setup(phy_ctrl_t *pc, int line_mode,
                               int phy_ext_boot)
{
    if ((line_mode != BSDK_PHY84728_INTF_SGMII) &&
        (line_mode != BSDK_PHY84728_INTF_1000X) && 
        (line_mode != BSDK_PHY84728_INTF_SFI)) {
        return SOC_E_CONFIG;
    }

    /* Enable Power - PMA/PMD, PCS, AN */
    SOC_IF_ERROR_RETURN(
         BSDK_MOD_PHY84728_LN_DEV1_PMD_CTRLr(pc, 0,
                                        BSDK_PHY84728_PMD_CTRL_PD));
    SOC_IF_ERROR_RETURN(
        BSDK_MOD_PHY84728_LN_DEV3_PCS_CTRLr(pc, 0,
                                       BSDK_PHY84728_PCS_PCS_CTRL_PD));
    SOC_IF_ERROR_RETURN(
        BSDK_MOD_PHY84728_LN_DEV7_AN_MII_CTRLr(pc, 0,
                                       BSDK_PHY84728_AN_MII_CTRL_PD));
    if (phy_ext_boot) {
        /* Download firmware if via SPI */
        SOC_IF_ERROR_RETURN(bsdk_phy84728_firmware_spi_download(pc));
    }  else if (!(pc->flags & PHYCTRL_MDIO_BCST)) {
        /* MDIO based firmware download to RAM */
        if (BSDK_PHY84728_IS_84728(PHY84728_MAP(pc)->devid)) {
            SOC_IF_ERROR_RETURN(
                bsdk_phy84728_mdio_firmware_download(pc,
                                                    bcm84728_fw, bcm84728_fw_length));
        } else { /* non-84728 device */
            SOC_IF_ERROR_RETURN(
                bsdk_phy84728_mdio_firmware_download(pc,
                                                    bcm84748_fw, bcm84748_fw_length));
        }
    } 

    /* Set Line mode */
    SOC_IF_ERROR_RETURN(
        bsdk_phy84728_line_mode_set(pc, 0, line_mode));
    
    /* System side mode */
    if (line_mode == BSDK_PHY84728_INTF_SFI) {
        SOC_IF_ERROR_RETURN(
            bsdk_phy84728_sys_mode_set(pc, 0, 
                                       BSDK_PHY84728_INTF_XAUI));
        SOC_IF_ERROR_RETURN(
            bsdk_phy84728_speed_set(pc, 10000));
    } else if (line_mode == BSDK_PHY84728_INTF_1000X) {
        SOC_IF_ERROR_RETURN(
            bsdk_phy84728_sys_mode_set(pc, 0, 
                                       BSDK_PHY84728_INTF_1000X));
        /* Force Speed to 1G */
        SOC_IF_ERROR_RETURN(
            bsdk_phy84728_speed_set(pc, 1000));
    } else { /* SGMII MODE */
        SOC_IF_ERROR_RETURN(
            bsdk_phy84728_sys_mode_set(pc, 0, 
                                       BSDK_PHY84728_INTF_SGMII));
    }

    return SOC_E_NONE;
}

/*
 * Function:     
 *    bsdk_phy84728_reset 
 * Purpose:    
 *    Reset the PHY.
 * Parameters:
 *    phy_id - PHY Device Address of the PHY to reset
 */
int 
bsdk_phy84728_reset(phy_ctrl_t *pc)
{
    int         wan_mode;
    uint16          ctrl, tmp;


    /* Reset the PHY **/
    SOC_IF_ERROR_RETURN(
        BSDK_RD_PHY84728_LN_DEV1_PMD_CTRLr(pc, &ctrl));

    SOC_IF_ERROR_RETURN(
        BSDK_WR_PHY84728_LN_DEV1_PMD_CTRLr(pc, 
                               (ctrl | BSDK_PHY84728_PMD_CTRL_RESET)));

    wan_mode = soc_property_port_get(pc->unit, pc->port, spn_PHY_WAN_MODE, FALSE);

    if (wan_mode) {
        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_WIS_CTRLr(pc, 
                                     BSDK_PHY84728_WIS_CTRL_RESET,
                                     BSDK_PHY84728_WIS_CTRL_RESET));
    }

    /* Needs minimum of 5us to complete the reset */
    sal_usleep(30);

    SOC_IF_ERROR_RETURN(
        BSDK_RD_PHY84728_LN_DEV1_PMD_CTRLr(pc, &tmp));

    if ((tmp & BSDK_PHY84728_PMD_CTRL_RESET) != 0) {
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMD_CTRLr(pc, ctrl));
        return SOC_E_FAIL;
    }

    if (wan_mode) {
        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_WIS_CTRLr(pc, 
                                     0,
                                     BSDK_PHY84728_WIS_CTRL_RESET));
    }

    return SOC_E_NONE;
}
/*
 * Function:     
 *    bsdk_phy84728_lane_map
 * Purpose:    
 *    To set the PHY's lane map  
 * Parameters:
 *    phy_id    - PHY's device address
 * Returns:    
 */
int
bsdk_phy84728_lane_map(phy_ctrl_t *pc)
{
    uint32 phy_sys_xaui_tx_lane_map, phy_sys_xaui_rx_lane_map;
    uint16 temp;

    phy_sys_xaui_tx_lane_map = PHY84728_MAP(pc)->phy_sys_xaui_tx_lane_map ;

    if (phy_sys_xaui_tx_lane_map) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "phy_sys_xaui_tx_lane_map: u=%d p=%d val=0x%x\n"),
                  pc->unit, pc->port, phy_sys_xaui_tx_lane_map));
        temp  = ((phy_sys_xaui_tx_lane_map & (0x3<<12)) >> 6) |
                 ((phy_sys_xaui_tx_lane_map & (0x3<<8)) >> 4) |
                 ((phy_sys_xaui_tx_lane_map & (0x3<<4)) >> 2) |
                 (phy_sys_xaui_tx_lane_map & 0x3) | 0x0100;
        SOC_IF_ERROR_RETURN(
            BSDK_PHY84728_REG_MOD(pc, 0x00, 0x0004, 0x8169, temp, 0x01ff));
    }

    phy_sys_xaui_rx_lane_map = PHY84728_MAP(pc)->phy_sys_xaui_rx_lane_map ;

    if (phy_sys_xaui_rx_lane_map) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "phy_sys_xaui_rx_lane_map: u=%d p=%d val=0x%x\n"),
                  pc->unit, pc->port, phy_sys_xaui_rx_lane_map));
        temp  = ((phy_sys_xaui_rx_lane_map & (0x3<<12)) >> 6) |
                 ((phy_sys_xaui_rx_lane_map & (0x3<<8)) >> 4) |
                 ((phy_sys_xaui_rx_lane_map & (0x3<<4)) >> 2) |
                 (phy_sys_xaui_rx_lane_map & 0x3) | 0x0100;
        SOC_IF_ERROR_RETURN(
            BSDK_PHY84728_REG_MOD(pc, 0x00, 0x0004, 0x816b, temp, 0x01ff));
    }

    return SOC_E_NONE;
}


/*
 * Function:     
 *    bsdk_phy84728_speed_set
 * Purpose:    
 *    To set the PHY's speed  
 * Parameters:
 *    phy_id    - PHY's device address
 *    speed     - Speed to set
 *               10   = 10Mbps
 *               100  = 100Mbps
 *               1000 = 1000Mbps
 * Returns:    
 */
int
bsdk_phy84728_speed_set(phy_ctrl_t *pc, int speed)
{
    int value = 0;
    bsdk_phy84728_intf_t line_mode;

    if ((speed != 10) && (speed != 100) && 
         (speed != 1000) && (speed != 10000)) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(
        bsdk_phy84728_line_intf_get(pc, 0, &line_mode));

    switch (line_mode) {
    case BSDK_PHY84728_INTF_SFI:
    case BSDK_PHY84728_INTF_1000X:
        /* Disable Clause 37 Autoneg */
        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_LN_DEV7_AN_MII_CTRLr(pc, 0x0000,
                                   BSDK_PHY84728_AN_MII_CTRL_AE));
        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_LN_DEV7_AN_BASE1000X_CTRL1r(pc, BSDK_PHY84728_AN_BASE1000X_CTRL1_FIBER_MODE,
                                   BSDK_PHY84728_AN_BASE1000X_CTRL1_FIBER_MODE));
        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_LN_DEV7_AN_MSIC2r(pc, BSDK_PHY84728_AN_MISC2_RX_MII_GEN_SEL |  
                                                    BSDK_PHY84728_AN_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN,
                                                    BSDK_PHY84728_AN_MISC2_RX_MII_GEN_SEL |
                                                    BSDK_PHY84728_AN_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN));
        if (speed == 10000) {

            /* Set Line mode */
            SOC_IF_ERROR_RETURN(
                bsdk_phy84728_line_mode_set(pc, 0,
                BSDK_PHY84728_INTF_SFI));

            /* Set System side to match Line side */
            SOC_IF_ERROR_RETURN(
                bsdk_phy84728_sys_mode_set(pc, 0, 
                                         BSDK_PHY84728_INTF_XAUI));
            /* Force Speed in PMD */
            SOC_IF_ERROR_RETURN(
                BSDK_MOD_PHY84728_LN_DEV1_PMD_CTRLr(pc, 
                                     BSDK_PHY84728_PMD_CTRL_SS_10000,
                                     BSDK_PHY84728_PMD_CTRL_10GSS_MASK));
            /* Select 10G-LRM PMA */
            SOC_IF_ERROR_RETURN(
                BSDK_MOD_PHY84728_LN_DEV1_PMD_CTRL2r(pc,
                             BSDK_PHY84728_PMD_CTRL2_PMA_10GLRMPMD_TYPE,
                             BSDK_PHY84728_PMD_CTRL2_PMA_SELECT_MASK));

        } else if (speed == 1000) {
           
                /* Set Line mode */
                SOC_IF_ERROR_RETURN(
                    bsdk_phy84728_line_mode_set(pc, 0,
                                                   BSDK_PHY84728_INTF_1000X));
                /* Set System side to match Line side */
                SOC_IF_ERROR_RETURN(
                    bsdk_phy84728_sys_mode_set(pc, 0, 
                                             BSDK_PHY84728_INTF_1000X));
                /* Force Speed in PMD */
                SOC_IF_ERROR_RETURN(
                    BSDK_MOD_PHY84728_LN_DEV1_PMD_CTRLr(pc, 
                                         BSDK_PHY84728_PMD_CTRL_SS_1000,
                                         BSDK_PHY84728_PMD_CTRL_10GSS_MASK));
            } else {
                return SOC_E_CONFIG;
        }
    break;

    case BSDK_PHY84728_INTF_SGMII:
        if (speed > 1000) {
            return SOC_E_CONFIG;
        }
        /* Disable Clause 37 Autoneg */
        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_LN_DEV7_AN_MII_CTRLr(pc, 0x0000,
                                   0x1000));
        if (speed == 1000) {
            value = BSDK_PHY84728_PMD_CTRL_SS_1000;
        }
        if (speed == 100) {
            value = BSDK_PHY84728_PMD_CTRL_SS_100;
        }
        if (speed == 10) {
            value = BSDK_PHY84728_PMD_CTRL_SS_10;
        }
        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_LN_DEV7_XGXS_CTRLr(pc, 0x0000, BSDK_PHY84728_AN_XGXS_CTRL_START_SEQ));

        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_LN_DEV7_AN_MII_CTRLr(pc, value,
                     BSDK_PHY84728_AN_MII_CTRL_SS_MASK | BSDK_PHY84728_AN_MII_CTRL_AE));

        /* Force Speed in PMD */
        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_LN_DEV1_PMD_CTRLr(pc, 
                                 value,
                                 BSDK_PHY84728_PMD_CTRL_10GSS_MASK));
        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_LN_DEV7_XGXS_CTRLr(pc, BSDK_PHY84728_AN_XGXS_CTRL_START_SEQ,
                                                     BSDK_PHY84728_AN_XGXS_CTRL_START_SEQ));
    break;

    default:
        return SOC_E_CONFIG;
    }

    /* Enable Squelch */
    SOC_IF_ERROR_RETURN(_phy_84728_squelch_enable(pc->unit, pc->port, TRUE));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84728_mdix_set
 * Description:
 *      Set the Auto-MDIX mode of a port/PHY
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - One of:
 *              SOC_PORT_MDIX_AUTO
 *                      Enable auto-MDIX when autonegotiation is enabled
 *              SOC_PORT_MDIX_FORCE_AUTO
 *                      Enable auto-MDIX always
 *              SOC_PORT_MDIX_NORMAL
 *                      Disable auto-MDIX
 *              SOC_PORT_MDIX_XOVER
 *                      Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
phy_84728_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port) && NXT_PC(pc)) {
            PHYDRV_CALL_ARG1(pc, PHY_MDIX_SET, mode);
            return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(
        phy_null_mdix_set(unit, port, mode));

    return SOC_E_NONE;

}        

/*
 * Function:
 *      phy_84728_mdix_get
 * Description:
 *      Get the Auto-MDIX mode of a port/PHY
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - (Out) One of:
 *              SOC_PORT_MDIX_AUTO
 *                      Enable auto-MDIX when autonegotiation is enabled
 *              SOC_PORT_MDIX_FORCE_AUTO
 *                      Enable auto-MDIX always
 *              SOC_PORT_MDIX_NORMAL
 *                      Disable auto-MDIX
 *              SOC_PORT_MDIX_XOVER
 *                      Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
phy_84728_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port) && NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc, PHY_MDIX_GET, mode);
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(
        phy_null_mdix_get(unit, port, mode));

    return SOC_E_NONE;
}    

/*
 * Function:
 *      phy_84728_mdix_status_get
 * Description:
 *      Get the current MDIX status on a port/PHY
 * Parameters:
 *      unit    - Device number
 *      port    - Port number
 *      status  - (OUT) One of:
 *              SOC_PORT_MDIX_STATUS_NORMAL
 *                      Straight connection
 *              SOC_PORT_MDIX_STATUS_XOVER
 *                      Crossover has been performed
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
phy_84728_mdix_status_get(int unit, soc_port_t port,
                         soc_port_mdix_status_t *status)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port) && NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc, PHY_MDIX_STATUS_GET, status);
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(
        phy_null_mdix_status_get(unit, port, status));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84728_master_set
 * Purpose:
 *      Set the master mode for the PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - SOC_PORT_MS_*
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      No synchronization performed at this level. Autonegotiation is
 *      not manipulated.
 */

STATIC int
phy_84728_master_set(int unit, soc_port_t port, int master)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port) && NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc, PHY_MASTER_SET, master);
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(
        phy_null_set(unit, port, master));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84728_master_get
 * Purpose:
 *      Get the master mode for the PHY.  If mode is forced, then
 *      forced mode is returned; otherwise operating mode is returned.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - (OUT) SOC_PORT_MS_*
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      No synchronization performed at this level. Autonegotiation is
 *      not manipulated.
 */

STATIC int
phy_84728_master_get(int unit, soc_port_t port, int *master)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port) && NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc, PHY_MASTER_GET, master);
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(
        phy_null_zero_get(unit, port, master));

    return SOC_E_NONE;
}

/*
 * Function:
 *      bsdk_phy84728_link_get
 * Purpose:
 *      Determine the current link up/down status for a 84728 device.
 * Parameters:
 *    phy_id    - PHY Device Address
 *    link      - (OUT) 
 *                1 = Indicates Link is established
 *                0 = No Link.
 * Returns:
 */
int
bsdk_phy84728_link_get(phy_ctrl_t *pc, int *link)
{

    uint16 data = 0, pma_mii_stat = 0, pcs_mii_stat = 0;
    int an, an_done;
    bsdk_phy84728_intf_t line_mode;

    *link = 0;
    SOC_IF_ERROR_RETURN(
        phy_84728_an_get(pc->unit, pc->port, &an, &an_done)); 

    /* Firmware issue: need to comment out as workaround */
#if 0
    if (an && (an_done == 0)) {
        /* Auto neg in progess, return no link */
        *link = 0;
        return SOC_E_NONE;
    }
#endif


     SOC_IF_ERROR_RETURN(
         bsdk_phy84728_line_intf_get(pc, 0, &line_mode));

     switch (line_mode) {
        case BSDK_PHY84728_INTF_SFI:

            SOC_IF_ERROR_RETURN(
                BSDK_RD_PHY84728_LN_DEV1_PMD_STATr(pc, &pma_mii_stat));

            SOC_IF_ERROR_RETURN(
                BSDK_RD_PHY84728_LN_DEV3_PCS_STATr(pc, &pcs_mii_stat));

            if (pma_mii_stat == 0xffff || pcs_mii_stat == 0xffff) {
                *link = 0;
                break;
            }

            *link = pma_mii_stat & pcs_mii_stat & BSDK_PHY84728_PMD_STAT_RX_LINK_STATUS;

            break;

        case BSDK_PHY84728_INTF_1000X:
        case BSDK_PHY84728_INTF_SGMII:
            SOC_IF_ERROR_RETURN(
                BSDK_RD_PHY84728_LN_DEV7_AN_MII_STATr(pc, &data));

            if (data == 0xffff) {
                *link = 0;
                break;
            }

            *link = data & BSDK_PHY84728_AN_MII_STAT_LINK_STATUS;
            break;

        default:
            break;
     }

     if (*link == 0) {
         if (line_mode != BSDK_PHY84728_INTF_SGMII && an && an_done) {
            SOC_IF_ERROR_RETURN(
                bsdk_phy84728_line_mode_set(pc, 0, BSDK_PHY84728_INTF_1000X));
         }
     }

    return SOC_E_NONE;
}


/*
 * Function:
 *     bsdk_phy84728_ability_advert_set 
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *    phy_id    - PHY Device Address
 *    ability   - Ability indicating supported options/speeds.
 *                For Speed & Duplex:
 *                BSDK_PM_10MB_HD    = 10Mb, Half Duplex
 *                BSDK_PM_10MB_FD    = 10Mb, Full Duplex
 *                BSDK_PM_100MB_HD   = 100Mb, Half Duplex
 *                BSDK_PM_100MB_FD   = 100Mb, Full Duplex
 *                BSDK_PM_1000MB_HD  = 1000Mb, Half Duplex
 *                BSDK_PM_1000MB_FD  = 1000Mb, Full Duplex
 *                BSDK_PM_PAUSE_TX   = TX Pause
 *                BSDK_PM_PAUSE_RX   = RX Pause
 *                BSDK_PM_PAUSE_ASYMM = Asymmetric Pause
 * Returns:
 */

int
bsdk_phy84728_ability_advert_set(phy_ctrl_t *pc,
                               bsdk_phy84728_port_ability_t ability)
{
    uint16 data = 0;
    bsdk_phy84728_intf_t line_mode;

    SOC_IF_ERROR_RETURN(
        bsdk_phy84728_line_intf_get(pc, 0, &line_mode));

    if (line_mode == BSDK_PHY84728_INTF_1000X) {
        /* Always advertise 1000X full duplex */
        data = BSDK_PHY84728_DEV7_1000X_ANA_C37_FD;  
        if ((ability & BSDK_PHY84728_PA_PAUSE) == 
                                             BSDK_PHY84728_PA_PAUSE) {
            data |= BSDK_PHY84728_DEV7_1000X_ANA_C37_PAUSE;
        } else {
            if (ability & BSDK_PHY84728_PA_PAUSE_TX) {
                data |= BSDK_PHY84728_DEV7_1000X_ANA_C37_ASYM_PAUSE;
            } else {
                if (ability & BSDK_PHY84728_PA_PAUSE_RX) {
                    data |= BSDK_PHY84728_DEV7_1000X_ANA_C37_ASYM_PAUSE;
                    data |= BSDK_PHY84728_DEV7_1000X_ANA_C37_PAUSE;
                }
            }
        }
        SOC_IF_ERROR_RETURN(
            BSDK_WR_PHY84728_LN_DEV7_AN_ANAr(pc, data));

    }
    if (line_mode == BSDK_PHY84728_INTF_SGMII) {
        
        
    } else {
        /* No Autoneg in SFI mode */
        /* return SOC_E_UNAVAIL; */
    }
    return SOC_E_NONE;
}


/*
 * Function:
 *      bsdk_phy84728_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *    phy_id    - PHY Device Address
 *    ability   - (OUT) Port ability mask indicating supported options/speeds.
 *                For Speed & Duplex:
 *                BSDK_PM_10MB_HD    = 10Mb, Half Duplex
 *                BSDK_PM_10MB_FD    = 10Mb, Full Duplex
 *                BSDK_PM_100MB_HD   = 100Mb, Half Duplex
 *                BSDK_PM_100MB_FD   = 100Mb, Full Duplex
 *                BSDK_PM_1000MB_HD  = 1000Mb, Half Duplex
 *                BSDK_PM_1000MB_FD  = 1000Mb, Full Duplex
 *                BSDK_PM_PAUSE_TX   = TX Pause
 *                BSDK_PM_PAUSE_RX   = RX Pause
 *                BSDK_PM_PAUSE_ASYMM = Asymmetric Pause
 * Returns:
 */

int
bsdk_phy84728_ability_advert_get(phy_ctrl_t *pc,
                               bsdk_phy84728_port_ability_t *ability)
{
    uint16 data = 0;
    bsdk_phy84728_intf_t line_mode;

    if (ability == NULL) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(
        bsdk_phy84728_line_intf_get(pc, 0, &line_mode));

    if (line_mode == BSDK_PHY84728_INTF_1000X) {
        SOC_IF_ERROR_RETURN(
            BSDK_RD_PHY84728_LN_DEV7_AN_ANAr(pc, &data));

        *ability |= (data & BSDK_PHY84728_DEV7_1000X_ANA_C37_FD) ? 
                  BSDK_PHY84728_PA_1000MB_FD : 0;
        *ability |= (data & BSDK_PHY84728_DEV7_1000X_ANA_C37_HD) ?
                  BSDK_PHY84728_PA_1000MB_HD : 0;
        switch (data & (BSDK_PHY84728_DEV7_1000X_ANA_C37_PAUSE | 
                        BSDK_PHY84728_DEV7_1000X_ANA_C37_ASYM_PAUSE)) {
        case BSDK_PHY84728_DEV7_1000X_ANA_C37_PAUSE:
            *ability |= BSDK_PHY84728_PA_PAUSE;
            break;
        case BSDK_PHY84728_DEV7_1000X_ANA_C37_ASYM_PAUSE:
            *ability |= BSDK_PHY84728_PA_PAUSE_TX;
            break;
        case (BSDK_PHY84728_DEV7_1000X_ANA_C37_PAUSE | 
              BSDK_PHY84728_DEV7_1000X_ANA_C37_ASYM_PAUSE):
            *ability |= BSDK_PHY84728_PA_PAUSE_RX;
            break;
        }
    } else {
        if (line_mode == BSDK_PHY84728_INTF_SGMII) {
            *ability = BSDK_PHY84728_PA_SPEED_ALL |
                       BSDK_PHY84728_PA_AN        |
                       BSDK_PHY84728_PA_PAUSE     |
                       BSDK_PHY84728_PA_PAUSE_ASYMM;
        
        } else {
            /* No Autoneg in SFI mode */
            *ability = BSDK_PHY84728_PA_10000MB_FD |
                       BSDK_PHY84728_PA_PAUSE;
        }
    }
    return SOC_E_NONE;
}


/*
 * Function:
 *      bsdk_phy84728_remote_ability_advert_get
 * Purpose:
 *      Get partners current advertisement for auto-negotiation.
 * Parameters:
 *    phy_id         - PHY Device Address
 *    remote_ability - (OUT) Port ability mask indicating supported options/speeds.
 *                For Speed & Duplex:
 *                BSDK_PM_10MB_HD    = 10Mb, Half Duplex
 *                BSDK_PM_10MB_FD    = 10Mb, Full Duplex
 *                BSDK_PM_100MB_HD   = 100Mb, Half Duplex
 *                BSDK_PM_100MB_FD   = 100Mb, Full Duplex
 *                BSDK_PM_1000MB_HD  = 1000Mb, Half Duplex
 *                BSDK_PM_1000MB_FD  = 1000Mb, Full Duplex
 *                BSDK_PM_PAUSE_TX   = TX Pause
 *                BSDK_PM_PAUSE_RX   = RX Pause
 *                BSDK_PM_PAUSE_ASYMM = Asymmetric Pause
 * Returns:
 */

int
bsdk_phy84728_remote_ability_advert_get(phy_ctrl_t *pc,
                                bsdk_phy84728_port_ability_t *remote_ability)
{
    uint16 data = 0;
    bsdk_phy84728_intf_t line_mode;

    if (remote_ability == NULL) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(
        bsdk_phy84728_line_intf_get(pc, 0, &line_mode));

    if (line_mode == BSDK_PHY84728_INTF_1000X) {
        SOC_IF_ERROR_RETURN(
            BSDK_RD_PHY84728_LN_DEV7_AN_ANPr(pc, &data));

        *remote_ability |= (data & BSDK_PHY84728_DEV7_1000X_ANA_C37_FD) ?
                  BSDK_PHY84728_PA_1000MB_FD : 0;
        *remote_ability |= (data & BSDK_PHY84728_DEV7_1000X_ANA_C37_HD) ?
                               BSDK_PHY84728_PA_1000MB_HD : 0;
        switch (data & (BSDK_PHY84728_DEV7_1000X_ANA_C37_PAUSE | 
                        BSDK_PHY84728_DEV7_1000X_ANA_C37_ASYM_PAUSE)) {
        case BSDK_PHY84728_DEV7_1000X_ANA_C37_PAUSE:
            *remote_ability |= BSDK_PHY84728_PA_PAUSE;
            break;
        case BSDK_PHY84728_DEV7_1000X_ANA_C37_ASYM_PAUSE:
            *remote_ability |= BSDK_PHY84728_PA_PAUSE_TX;
            break;
        case (BSDK_PHY84728_DEV7_1000X_ANA_C37_PAUSE | 
              BSDK_PHY84728_DEV7_1000X_ANA_C37_ASYM_PAUSE):
            *remote_ability |= BSDK_PHY84728_PA_PAUSE_RX;
            break;
        }
    } else if (line_mode == BSDK_PHY84728_INTF_SGMII) {
        
        
    } else {
        if (!REVERSE_MODE(pc)) {
        /* No Autoneg in SFI mode */
        return SOC_E_UNAVAIL;
        }
    }
    return SOC_E_NONE;
}


/*
 * Function:
 *      bsdk_phy84728_ability_local_get
 * Purpose:
 *      Get the PHY abilities
 * Parameters:
 *    phy_id    - PHY Device Address
 *    mode      - Mask indicating supported options/speeds.
 *                For Speed & Duplex:
 *                BSDK_PM_10MB_HD    = 10Mb, Half Duplex
 *                BSDK_PM_10MB_FD    = 10Mb, Full Duplex
 *                BSDK_PM_100MB_HD   = 100Mb, Half Duplex
 *                BSDK_PM_100MB_FD   = 100Mb, Full Duplex
 *                BSDK_PM_1000MB_HD  = 1000Mb, Half Duplex
 *                BSDK_PM_1000MB_FD  = 1000Mb, Full Duplex
 *                BSDK_PM_PAUSE_TX   = TX Pause
 *                BSDK_PM_PAUSE_RX   = RX Pause
 *                BSDK_PM_PAUSE_ASYMM = Asymmetric Pause
 *                BSDK_PM_SGMII       = SGMIII Supported
 *                BSDK_PM_XSGMII      = XSGMIII Supported
 * Returns:
 */
int
bsdk_phy84728_ability_local_get(phy_ctrl_t *pc,
                             bsdk_phy84728_port_ability_t *ability)
{
    bsdk_phy84728_intf_t line_mode;

    if (ability == NULL) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(
        bsdk_phy84728_line_intf_get(pc, 0, &line_mode));

    *ability = BSDK_PHY84728_PA_LB_PHY | BSDK_PHY84728_PA_PAUSE;
    *ability |= BSDK_PHY84728_PA_SGMII;

    if (line_mode == BSDK_PHY84728_INTF_SGMII) {
        *ability |= BSDK_PHY84728_PA_AN;
        *ability |= BSDK_PHY84728_PA_SPEED_ALL;
        
    } else {
        *ability |= BSDK_PHY84728_PA_XSGMII;
        *ability |= BSDK_PHY84728_PA_10000MB_FD;
        *ability |= BSDK_PHY84728_PA_1000MB_FD;
    }

    return(SOC_E_NONE);
}

STATIC int
_bsdk_phy84728_system_sgmii_speed_set(phy_ctrl_t *pc, int dev_port, int speed)
{
    uint16 mii_ctrl = 0;

    if (speed == 0) {
        return SOC_E_NONE;
    }

    {
        SOC_IF_ERROR_RETURN(
            BSDK_RD_PHY84728_SYS_DEV4_XGXS_MII_CTRLr(pc, &mii_ctrl));

        mii_ctrl &= ~(BSDK_PHY84728_AN_MII_CTRL_SS_MASK);
        switch(speed) {
        case 10:
            mii_ctrl |= BSDK_PHY84728_AN_MII_CTRL_SS_10;
            break;
        case 100:
            mii_ctrl |= BSDK_PHY84728_AN_MII_CTRL_SS_100;
            break;
        case 1000:
            mii_ctrl |= BSDK_PHY84728_AN_MII_CTRL_SS_1000;
            break;
        default:
            return SOC_E_CONFIG;
        }
        SOC_IF_ERROR_RETURN(
            BSDK_WR_PHY84728_SYS_DEV4_XGXS_MII_CTRLr(pc, mii_ctrl));
    }
    return SOC_E_NONE;
}

STATIC int
_bsdk_phy84728_system_sgmii_duplex_set(phy_ctrl_t *pc, int dev_port,
                                 int duplex)
{
    uint16 mii_ctrl;

    {
        SOC_IF_ERROR_RETURN(
            BSDK_RD_PHY84728_SYS_DEV4_XGXS_MII_CTRLr(pc, &mii_ctrl));

        if ( duplex ) {
            mii_ctrl |= BSDK_PHY84728_AN_MII_CTRL_FD;
        } else {
            mii_ctrl &= ~BSDK_PHY84728_AN_MII_CTRL_FD;
        }
        SOC_IF_ERROR_RETURN(
            BSDK_WR_PHY84728_SYS_DEV4_XGXS_MII_CTRLr(pc, mii_ctrl));
    }

    return SOC_E_NONE;
}

STATIC int
_bsdk_phy84728_system_sgmii_sync(phy_ctrl_t *pc, int dev_port)
{
    int speed;
    int duplex;

        SOC_IF_ERROR_RETURN(
            phy_84728_speed_get(pc->unit, pc->port, &speed));

        SOC_IF_ERROR_RETURN(
            phy_84728_duplex_get(pc->unit, pc->port, &duplex));

        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_SYS_DEV4_XGXS_CTRLr(pc, 0x0000, BSDK_PHY84728_AN_XGXS_CTRL_START_SEQ));

        SOC_IF_ERROR_RETURN(
            _bsdk_phy84728_system_sgmii_speed_set(pc, dev_port, speed));

        SOC_IF_ERROR_RETURN(
            _bsdk_phy84728_system_sgmii_duplex_set(pc, dev_port, duplex));

        SOC_IF_ERROR_RETURN(
            BSDK_MOD_PHY84728_SYS_DEV4_XGXS_CTRLr(pc, BSDK_PHY84728_AN_XGXS_CTRL_START_SEQ,
                                                      BSDK_PHY84728_AN_XGXS_CTRL_START_SEQ));

    return SOC_E_NONE;
}

/*
 * Function:
 *     bsdk_phy84728_mdio_firmware_download
 * Purpose:
 *     Download new firmware via MDIO.
 * Parameters:
 *    phy_id    - PHY Device Address
 *    new_fw    - Pointer to new firmware
 *    fw_length    - Length of the firmware
 * Returns:
 */
int
bsdk_phy84728_mdio_firmware_download(phy_ctrl_t *pc,
                                  uint8 *new_fw, uint32 fw_length)
{

    uint16 data16;
    int j;
    uint16 num_words;

    /* 0xc848[15]=0, MDIO downloading to RAM, 0xc848[14]=1, serial boot */
    /* 0xc848[13]=0, SPI-ROM downloading not done, 0xc848[2]=0, spi port enable */
    SOC_IF_ERROR_RETURN(
           BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc848, 
               (0 << 15)|(1 << 14),
               (1 << 15)|(1 << 14)|((1 << 13) | (1 << 2))));

    /* Reset, to download code from MDIO */
    SOC_IF_ERROR_RETURN(
           BSDK_MOD_PHY84728_LN_DEV1_PMD_CTRLr(pc, 
               BSDK_PHY84728_PMD_CTRL_RESET, BSDK_PHY84728_PMD_CTRL_RESET));

    sal_usleep(50000);

    /* Write Starting Address, where the Code will reside in SRAM */
    data16 = 0x8000;
    SOC_IF_ERROR_RETURN
        (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, BSDK_PHY84728_PMAD_M8051_MSGIN_REG, data16));

    /* make sure address word is read by the micro */
    sal_usleep(10); /* Wait for 10us */

    /* Write SPI SRAM Count Size */
    data16 = (fw_length)/2;
    SOC_IF_ERROR_RETURN
        (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, BSDK_PHY84728_PMAD_M8051_MSGIN_REG, data16));

    /* make sure read by the micro */
    sal_usleep(10); /* Wait for 10us */

    /* Fill in the SRAM */
    num_words = (fw_length - 1);
    for (j = 0; j < num_words; j+=2) {
        /* Make sure the word is read by the Micro */
        sal_usleep(10);

        data16 = (new_fw[j] << 8) | new_fw[j+1];
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, BSDK_PHY84728_PMAD_M8051_MSGIN_REG,
                 data16));
    }

    /* make sure last code word is read by the micro */
    sal_usleep(20);

    /* Read Hand-Shake message (Done) from Micro */
    SOC_IF_ERROR_RETURN
        (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc,BSDK_PHY84728_PMAD_M8051_MSGOUT_REG, &data16)); 

    if (data16 != 0x4321 ) {
        /* Download done message */
        LOG_CLI((BSL_META_U(pc->unit,
                            "MDIO firmware download failed. Message: 0x%x\n"), data16));
    }

    /* Clear LASI status */
    SOC_IF_ERROR_RETURN
        (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0x9003, &data16));

    /* Wait for LASI to be asserted when M8051 writes checksum to MSG_OUTr */
    sal_usleep(100); /* Wait for 100 usecs */

    SOC_IF_ERROR_RETURN
        (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc,BSDK_PHY84728_PMAD_M8051_MSGOUT_REG, &data16));
    
    /* Need to check if checksum is correct */
    SOC_IF_ERROR_RETURN
        (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xCA1C, &data16));

    if (data16 != 0x600D) {
        /* Bad CHECKSUM */
        LOG_CLI((BSL_META_U(pc->unit,
                            "MDIO Firmware downlad failure:"
                 "Incorrect Checksum %x\n"), data16));
        return SOC_E_FAIL;
    }

    /* read Rev-ID */
    SOC_IF_ERROR_RETURN
        (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xCA1A, &data16));
    LOG_CLI((BSL_META_U(pc->unit,
                        "MDIO Firmware download completed. Version : 0x%x\n"), data16));

    return SOC_E_NONE;
}

/*
 *  Function:
 *       _phy_84728_phy_84728_write_message
 *
 *  Purpose:
 *      Write into Message In and Read from MSG Out register.
 *  Input:
 *      phy_id
 *      wrdata
 *      rddata
 */

STATIC int
_phy_84728_write_message(phy_ctrl_t *pc, uint16 wrdata, uint16 *rddata)
{
    uint16 tmp_data = 0;
    int iter = 0;

    SOC_IF_ERROR_RETURN
        (BSDK_RD_PHY84728_LN_DEV1_M8501_MSGOUTr(pc, &tmp_data));
    SOC_IF_ERROR_RETURN         
        (BSDK_WR_PHY84728_LN_DEV1_M8501_MSGINr(pc, wrdata));

    do {
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_LASI_STATr(pc, &tmp_data));

        if (tmp_data & 0x4) {
            break;
        }
        sal_usleep(BSDK_WR_TIMEOUT);
        iter++;
    } while (iter < BSDK_WR_ITERATIONS);

    if (!(tmp_data & 0x4)) {
        LOG_CLI((BSL_META_U(pc->unit,
                            "write message failed due to timeout\n")));
        return SOC_E_FAIL;
    }

    SOC_IF_ERROR_RETURN
        (BSDK_RD_PHY84728_LN_DEV1_M8501_MSGOUTr(pc, &tmp_data));

    *rddata = tmp_data;
    SOC_IF_ERROR_RETURN
        (BSDK_RD_PHY84728_LN_DEV1_RX_ALARMr(pc, &tmp_data));

    return SOC_E_NONE;
}


/*
 *  Function:
 *       _phy_84728_rom_wait
 *
 *  Purpose:
 *      Wait for data to be written to the SPI-ROM.
 *  Input:
 *      phy_id
 */

STATIC int
_phy_84728_rom_wait(phy_ctrl_t *pc)
{
    uint16  rd_data = 0, wr_data;
    int        count;
    int        SPI_READY;
    int        iter = 0;

    do {
        count = 1;
        wr_data = ((BSDK_RD_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        wr_data = BSDK_SPI_CTRL_1_L;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        if (rd_data & 0x0100) {
            break;
        }
        sal_usleep(BSDK_WR_TIMEOUT);
        iter++;
    } while (iter < BSDK_WR_ITERATIONS);

    if (!(rd_data & 0x0100)) {
        LOG_CLI((BSL_META_U(pc->unit,
                            "_phy_84728_rom_wait: write timeout\n")));
        return SOC_E_TIMEOUT;
    }

     SPI_READY = 1;
     while (SPI_READY == 1) {
        /* Set-up SPI Controller To Receive SPI EEPROM Status. */
        count = 1;
        wr_data = ((BSDK_WR_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        wr_data = BSDK_SPI_CTRL_2_H;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        wr_data = 0x0100;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        /* Fill-up SPI Transmit Fifo To check SPI Status. */
        count = 2;
        wr_data = ((BSDK_WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        /* Write Tx Fifo Register Address. */
        wr_data = BSDK_SPI_TXFIFO;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-1. */
        wr_data = ((1 * 0x0100) | BSDK_MSGTYPE_HRD);
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        /* Write SPI Tx Fifo Control Word-2. */
        wr_data = BSDK_RDSR_OPCODE;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        /* Write SPI Control Register Write Command. */
        count = 2;
        wr_data = ((BSDK_WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Address. */
        wr_data = BSDK_SPI_CTRL_1_L;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Word-1. */
        wr_data = 0x0101;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Word-2. */
        wr_data = 0x0100;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        /* Write SPI Control Register Write Command. */
        count = 1;
        wr_data = ((BSDK_WR_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Address. */
        wr_data = BSDK_SPI_CTRL_1_H;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Word-2. */
        wr_data = 0x0103;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        /* Wait For 64 bytes To be written.   */
        rd_data = 0x0000;
        
        do {
            count = 1;
            wr_data = ((BSDK_RD_CPU_CTRL_REGS * 0x0100) | count);
            SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
            wr_data = BSDK_SPI_CTRL_1_L;
            SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
            if (rd_data & 0x0100) {
                break;
            }
            sal_usleep(BSDK_WR_TIMEOUT);
            iter ++;
        } while (iter < BSDK_WR_ITERATIONS); 

        if (!(rd_data & 0x0100)) {
            LOG_CLI((BSL_META_U(pc->unit,
                                "_phy_84728_rom_program:timeout 2\n")));
            return SOC_E_TIMEOUT;
        }
        /* Write SPI Control Register Read Command. */
        count = 1;
        wr_data = ((BSDK_RD_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Address. */
        wr_data = BSDK_SPI_RXFIFO;

        SOC_IF_ERROR_RETURN         
            (BSDK_WR_PHY84728_LN_DEV1_M8501_MSGINr(pc, wr_data));
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_M8501_MSGOUTr(pc, &rd_data));

        /* Clear LASI Message Out Status. */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_LASI_STATr(pc, &wr_data));

        if ((rd_data & 0x1) == 0) {
            SPI_READY = 0;
        }
     } /* SPI_READY */
     return SOC_E_NONE;
}


/*
 * Function:
 *      _phy_84728_rom_write_enable_set
 *
 * Purpose:
 *      Enable disable protection on SPI_EEPROM
 *
 * Input:
 *      phy_id
 * Output:
 *      SOC_E_xxx
 *
 * Notes:
 *          25AA256 256Kbit Serial EEPROM
 *          STATUS Register
 *          +------------------------------------------+
 *          | WPEN | x | x | x | BP1 | BP0 | WEL | WIP |
 *          +------------------------------------------+
 *      BP1 BP0  :   Protected Blocks
 *       0   0   :  Protect None
 *       1   1   :  Protect All
 *
 *      WEL : Write Latch Enable
 *       0  : Do not allow writes
 *       1  : Allow writes
 */
STATIC int
_phy_84728_rom_write_enable_set(phy_ctrl_t *pc, int enable)
{
    uint16  rd_data, wr_data;
    uint8   wrsr_data;
    int        count;

    /*
     * Write SPI Control Register Write Command.
     */
    count = 2;
    wr_data = ((BSDK_WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /*
     * Write SPI Control -2 Register Address.
     */
    wr_data = BSDK_SPI_CTRL_2_L;
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /*
     * Write SPI Control -2 Register Word-1.
     */
    wr_data = 0x8200;
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /*
     * Write SPI Control -2 Register Word-2.
     */
    wr_data = 0x0100;
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /*
     * Fill-up SPI Transmit Fifo With SPI EEPROM Messages.
     * Write SPI Control Register Write Command.
     */
    count = 4;
    wr_data = ((BSDK_WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /*
     * Write Tx Fifo Register Address.
     */
    wr_data = BSDK_SPI_TXFIFO;
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /*
     * Write SPI Tx Fifo Control Word-1.
     */
    wr_data = ((1 * 0x0100) | BSDK_MSGTYPE_HWR);
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /*
     * Write SPI Tx Fifo Control Word-2.
     */
    wr_data = ((BSDK_MSGTYPE_HWR * 0x0100) | BSDK_WREN_OPCODE);
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /*
     * Write SPI Tx Fifo Control Word-3.
     */
    wr_data = ((BSDK_WRSR_OPCODE * 0x100) | (0x2));
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /*
     * Write SPI Tx Fifo Control Word-4.
     */
    wrsr_data = enable ? 0x2 : 0xc;
    wr_data = ((wrsr_data * 0x0100) | wrsr_data);
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /*
     * Write SPI Control Register Write Command.
     */
    count = 2;
    wr_data = ((BSDK_WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /*
     * Write SPI Control -1 Register Address.
     */
    wr_data = BSDK_SPI_CTRL_1_L;
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /*
     * Write SPI Control -1 Register Word-1.
     */
    wr_data = 0x0101;
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /*
     * Write SPI Control -1 Register Word-2.
     */
    wr_data = 0x0003;
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /*
     * Wait For WRSR Command To be written.
     */
    SOC_IF_ERROR_RETURN(_phy_84728_rom_wait(pc));

    return SOC_E_NONE;
}

/*
 * Function:
 *     bsdk_phy84728_spi_firmware_update
 * Purpose:
 *     Update the firmware in SPI ROM 
 * Parameters:
 *    phy_id    - PHY Device Address
 *    new_fw    - Pointer to new firmware
 *    fw_length    - Length of the firmware
 * Returns:
 */
int
bsdk_phy84728_spi_firmware_update(phy_ctrl_t *pc,
                                  uint8 *array, uint32 datalen)
{

    uint16   data = 0;
    int      j;
    uint16   rd_data, wr_data;
    uint8    spi_values[BSDK_WR_BLOCK_SIZE];
    int count, i = 0;


    if (array == NULL) {
        return SOC_E_PARAM;
    }

    /* Set Bit 2 and Bit 0 in SPI Control */
    SOC_IF_ERROR_RETURN(
        BSDK_MOD_PHY84728_LN_DEV1_SPI_CTRL_STATr(pc, 0x80FD, 0xffff));

    /* ser_boot pin high */
    SOC_IF_ERROR_RETURN(
        BSDK_MOD_PHY84728_LN_DEV1_MISC_CNTL2r(pc, 0x1, 0x1));

    /* Read LASI Status */
    SOC_IF_ERROR_RETURN(
        BSDK_RD_PHY84728_LN_DEV1_RX_ALARM_STATr(pc, &data));
    SOC_IF_ERROR_RETURN(
        BSDK_RD_PHY84728_LN_DEV1_TX_ALARM_STATr(pc, &data));
    SOC_IF_ERROR_RETURN(
        BSDK_RD_PHY84728_LN_DEV1_LASI_STATr(pc, &data));

    /* Enable LASI for Message Out */
    SOC_IF_ERROR_RETURN(
        BSDK_MOD_PHY84728_LN_DEV1_RX_ALARM_CNTLr(pc, 0x4, 0x4));

    /* Enable RX Alarm in LASI */
    SOC_IF_ERROR_RETURN(
        BSDK_MOD_PHY84728_LN_DEV1_LASI_CNTLr(pc, 0x4, 0x4));

    /* Read any residual Message out register */
    SOC_IF_ERROR_RETURN(
        BSDK_RD_PHY84728_LN_DEV1_M8501_MSGOUTr(pc, &data));

    /* Clear LASI Message OUT status */
    SOC_IF_ERROR_RETURN(
        BSDK_RD_PHY84728_LN_DEV1_RX_ALARM_STATr(pc, &data));

    /* Set SPI-ROM write enable */
    SOC_IF_ERROR_RETURN(
        _phy_84728_rom_write_enable_set(pc, 1));


    for (j = 0; j < datalen; j +=BSDK_WR_BLOCK_SIZE) {
        /* Setup SPI Controller */
        count = 2;
        wr_data = ((BSDK_WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

        /* Write SPI Control -2 Register Address.*/
        wr_data = BSDK_SPI_CTRL_2_L;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

        /* Write SPI Control -2 Register Word-1. */
        wr_data = 0x8200;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

        /* Write SPI Control -2 Register Word-2. */
        wr_data = 0x0100;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
        
        /* Fill-up SPI Transmit Fifo.
         * Write SPI Control Register Write Command.
         */
        count = 4 + (BSDK_WR_BLOCK_SIZE / 2);
        wr_data = ((BSDK_WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

        /* Write Tx Fifo Register Address. */
        wr_data = BSDK_SPI_TXFIFO;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-1. */
        wr_data = ((1 * 0x0100) | BSDK_MSGTYPE_HWR);
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-2. */
        wr_data = ((BSDK_MSGTYPE_HWR * 0x0100) | BSDK_WREN_OPCODE);
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-3. */
        wr_data = ((BSDK_WR_OPCODE * 0x0100) | (0x3 + BSDK_WR_BLOCK_SIZE));
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-4. */
        wr_data = (((j & 0x00FF) * 0x0100) | ((j & 0xFF00) / 0x0100));
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

        if (datalen < (j + BSDK_WR_BLOCK_SIZE)) {   /* last block */ 
            sal_memset(spi_values,0,BSDK_WR_BLOCK_SIZE);
            sal_memcpy(spi_values,&array[j],datalen - j);

            for (i = 0; i < BSDK_WR_BLOCK_SIZE; i += 2) {
                /* Write SPI Tx Fifo Data Word-4. */
                wr_data = ((spi_values[i+1] * 0x0100) | spi_values[i]);
                SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
            }           
        } else {        
            for (i = 0; i < BSDK_WR_BLOCK_SIZE; i += 2) {
                /* Write SPI Tx Fifo Data Word-4. */
                wr_data = ((array[j+i+1] * 0x0100) | array[j+i]);
                SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));
            }
        }

        /* 
         * Set-up SPI Controller To Transmit.
         * Write SPI Control Register Write Command.
         */
        count = 2;
        wr_data = ((BSDK_WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

        wr_data = BSDK_SPI_CTRL_1_L;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

        /* Write SPI Control -1 Register Word-1. */
        wr_data = 0x0501;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

        /* Write SPI Control -1 Register Word-2. */
        wr_data = 0x0003;
        SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

        /* Wait For 64 bytes To be written.   */
        SOC_IF_ERROR_RETURN(_phy_84728_rom_wait(pc));
    }

    /* Clear SPI-ROM write enable */
    SOC_IF_ERROR_RETURN(
        _phy_84728_rom_write_enable_set(pc, 0));

    /* Disable SPI EEPROM. */
    count = 2;
    wr_data = ((BSDK_WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /* Write SPI Control -2 Register Address. */
    wr_data = BSDK_SPI_CTRL_2_L;
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /* Write SPI Control -2 Register Word-1. */
    wr_data = 0x8200;
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /* Write SPI Control -2 Register Word-2. */
    wr_data = 0x0100;
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));


    /* Fill-up SPI Transmit Fifo With SPI EEPROM Messages. */
    count = 2;
    wr_data = ((BSDK_WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /* Write Tx Fifo Register Address. */
    wr_data = BSDK_SPI_TXFIFO;
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /* Write SPI Tx Fifo Control Word-1. */
    wr_data = ((0x1*0x0100) | BSDK_MSGTYPE_HWR);
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /* Write SPI Tx Fifo Control Word-2. */
    wr_data = BSDK_WRDI_OPCODE;
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /* Write SPI Control Register Write Command. */
    count = 2;
    wr_data = ((BSDK_WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /* Write SPI Control -1 Register Address. */
    wr_data = BSDK_SPI_CTRL_1_L;
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /* Write SPI Control -1 Register Word-1. */
    wr_data = 0x0101;
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /* Write SPI Control -1 Register Word-2. */
    wr_data = 0x0003;
    SOC_IF_ERROR_RETURN(_phy_84728_write_message(pc, wr_data, &rd_data));

    /* ser_boot pin LOW */
    SOC_IF_ERROR_RETURN(
        BSDK_MOD_PHY84728_LN_DEV1_MISC_CNTL2r(pc, 0x0, 0x1));

    /* Disable Bit 2 and Bit 0 in SPI Control */
    SOC_IF_ERROR_RETURN(
        BSDK_MOD_PHY84728_LN_DEV1_SPI_CTRL_STATr(pc, 0xc0F9, 0xffff));

    return SOC_E_NONE;
}

STATIC int
_phy_84728_bsc_rw(phy_ctrl_t *pc, int dev_addr, int opr,
                    int addr, int count, void *data_array,uint32 ram_start)
{
    int rv = SOC_E_NONE;
    int iter = 0;
    uint16 data16;
    int i;
    int access_type;
    int data_type;

    if (!data_array) {
        return SOC_E_PARAM;
    }

    if (count > BSDK_PHY84728_BSC_XFER_MAX) {
        return SOC_E_PARAM;
    }

    data_type = BSDK_PHY84728_I2C_DATA_TYPE(opr);
    access_type = BSDK_PHY84728_I2C_ACCESS_TYPE(opr);

    if (access_type == BSDK_PHY84728_I2CDEV_WRITE) {
        for (i = 0; i < count; i++) {
            if (data_type == BSDK_PHY84728_I2C_8BIT) {
                SOC_IF_ERROR_RETURN
                    (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, ram_start + i,
                          ((uint8 *)data_array)[i]));
            } else {  /* 16 bit */
                SOC_IF_ERROR_RETURN
                    (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, ram_start + i,
                          ((uint16 *)data_array)[i]));
            }
        }
    }

    data16 = ram_start;
    SOC_IF_ERROR_RETURN
        (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0x8004, data16));
    SOC_IF_ERROR_RETURN
        (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0x8003, addr));
    SOC_IF_ERROR_RETURN
        (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0x8002, count));

    data16 = 1;
    data16 |= (dev_addr<<9);
    if (access_type == BSDK_PHY84728_I2CDEV_WRITE) {
        data16 |= BSDK_PHY84728_WR_FREQ_400KHZ;
    }

    SOC_IF_ERROR_RETURN
        (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0x8005,data16));

    if (access_type == BSDK_PHY84728_I2CDEV_WRITE) {
        data16 =  0x8000 | BSDK_PHY84728_BSC_WRITE_OP;
    } else {
        data16 =  0x8000 | BSDK_PHY84728_BSC_READ_OP;
    }

    if (data_type == BSDK_PHY84728_I2C_16BIT) {
        data16 |= (1 << 12);
    }

    /* for single port mode, there should be only one I2C interface active
     * from lane0. The 0x800x register block is bcst type registers. If writing
     * to 0x8000 directly, it will enable all four I2C masters. Use indirect access
     * to enable only the lane 0.
     */

    SOC_IF_ERROR_RETURN
        (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0x8000, data16));

    while (iter < 100000) {
        rv = BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0x8000, &data16);
        if (((data16 & BSDK_PHY84728_2W_STAT) == BSDK_PHY84728_2W_STAT_COMPLETE)) {
            break;
        }
        iter++;
    }

    /* need some delays */
    sal_usleep(10000);

    LOG_CLI((BSL_META_U(pc->unit,
                        "BSC command status %d\n"),(data16 & BSDK_PHY84728_2W_STAT)));

    if (access_type == BSDK_PHY84728_I2CDEV_WRITE) {
        return SOC_E_NONE;
    }

    if ((data16 & BSDK_PHY84728_2W_STAT) == BSDK_PHY84728_2W_STAT_COMPLETE) {
        for (i = 0; i < count; i++) {
            SOC_IF_ERROR_RETURN
                (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, (ram_start+i), &data16));
            if (data_type == BSDK_PHY84728_I2C_16BIT) {
                ((uint16 *)data_array)[i] = data16;
                LOG_CLI((BSL_META_U(pc->unit,
                                    "%04x "), data16));
            } else {
                ((uint8 *)data_array)[i] = (uint8)data16;
                LOG_CLI((BSL_META_U(pc->unit,
                                    "%02x "), data16));
            }
        }
    }
    return rv;
}

/*
 * Read a slave device such as NVRAM/EEPROM connected to the 84728's I2C
 * interface. This function will be mainly used for diagnostic or workaround
 * purpose.
 * Note:
 * The size of read_array buffer must be greater than or equal to the
 * parameter nbytes.
 * usage example:
 *   Retrieve the first 100 byte data of the non-volatile storage device with
 *   I2C bus device address 0x50(default SPF eeprom I2C address) on unit 0,
 *   port 2.
 *   uint8 data8[100];
 *   phy_84728_i2cdev_read(0,2,0x50,0,100,data8);
 */
int
bsdk_phy_84728_i2cdev_read(phy_ctrl_t *pc,
                     int dev_addr,  /* 7 bit I2C bus device address */
                     int offset,    /* starting data address to read */
                     int nbytes,    /* number of bytes to read */
                     uint8 *read_array)   /* buffer to hold retrieved data */
{
    return _phy_84728_bsc_rw(pc, dev_addr,BSDK_PHY84728_I2CDEV_READ,
             offset, nbytes, (void *)read_array,BSDK_PHY84728_READ_START_ADDR);

}

/*
 * Write to a slave device such as NVRAM/EEPROM connected to the 84728's I2C
 * interface. This function will be mainly used for diagnostic or workaround
 * purpose.
 * Note:
 * The size of write_array buffer should be equal to the parameter nbytes.
 * The EEPROM may limit the maximun write size to 16 bytes
 * usage example:
 *   Write to first 100 byte space of the non-volatile storage device with
 *   I2C bus device address 0x50(default SPF eeprom I2C address) on unit 0,
 *   port 2, with written data specified in array data8.
 *   uint8 data8[100];
 *   *** initialize the data8 array with written data ***
 *
 *   phy_84728_i2cdev_write(0,2,0x50,0,100,data8);
 */

int
bsdk_phy_84728_i2cdev_write(phy_ctrl_t *pc,
                     int dev_addr,  /* I2C bus device address */
                     int offset,    /* starting data address to write to */
                     int nbytes,    /* number of bytes to write */
                     uint8 *write_array)   /* buffer to hold written data */
{
    int j;
    int rv = SOC_E_NONE;

    for (j = 0; j < (nbytes/BSDK_PHY84728_BSC_WR_MAX); j++) {
        rv = _phy_84728_bsc_rw(pc, dev_addr,BSDK_PHY84728_I2CDEV_WRITE,
                    offset + j * BSDK_PHY84728_BSC_WR_MAX, BSDK_PHY84728_BSC_WR_MAX,
                    (void *)(write_array + j * BSDK_PHY84728_BSC_WR_MAX),
                    BSDK_PHY84728_WRITE_START_ADDR);
        if (rv != SOC_E_NONE) {
            return rv;
        }
        sal_usleep(20000);
    }
    if (nbytes%BSDK_PHY84728_BSC_WR_MAX) {
        rv = _phy_84728_bsc_rw(pc, dev_addr,BSDK_PHY84728_I2CDEV_WRITE,
                offset + j * BSDK_PHY84728_BSC_WR_MAX, nbytes%BSDK_PHY84728_BSC_WR_MAX,
                (void *)(write_array + j * BSDK_PHY84728_BSC_WR_MAX),
                BSDK_PHY84728_WRITE_START_ADDR);
    }
    return rv;
}

void _phy_84728_encode_egress_message_mode(soc_port_phy_timesync_event_message_egress_mode_t mode,
                                            int offset, uint16 *value)
{

    switch (mode) {
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_NONE:
        *value |= (0x0 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_UPDATE_CORRECTIONFIELD:
        *value |= (0x1 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_REPLACE_CORRECTIONFIELD_ORIGIN:
        *value |= (0x2 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_CAPTURE_TIMESTAMP:
        *value |= (0x3 << offset);
        break;
    }

}

void _phy_84728_encode_ingress_message_mode(soc_port_phy_timesync_event_message_ingress_mode_t mode,
                                            int offset, uint16 *value)
{

    switch (mode) {
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_NONE:
        *value |= (0x0 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_UPDATE_CORRECTIONFIELD:
        *value |= (0x1 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_TIMESTAMP:
        *value |= (0x2 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_DELAYTIME:
        *value |= (0x3 << offset);
        break;
    }

}

void _phy_84728_decode_egress_message_mode(uint16 value, int offset,
                                            soc_port_phy_timesync_event_message_egress_mode_t *mode)
{

    switch ((value >> offset) & 0x3) {
    case 0x0:
        *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_NONE;
        break;
    case 0x1:
        *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_UPDATE_CORRECTIONFIELD;
        break;
    case 0x2:
        *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_REPLACE_CORRECTIONFIELD_ORIGIN;
        break;
    case 0x3:
        *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_CAPTURE_TIMESTAMP;
        break;
    }

}

void _phy_84728_decode_ingress_message_mode(uint16 value, int offset,
                                            soc_port_phy_timesync_event_message_ingress_mode_t *mode)
{

    switch ((value >> offset) & 0x3) {
    case 0x0:
        *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_NONE;
        break;
    case 0x1:
        *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_UPDATE_CORRECTIONFIELD;
        break;
    case 0x2:
        *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_TIMESTAMP;
        break;
    case 0x3:
        *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_DELAYTIME;
        break;
    }

}

void _phy_84728_encode_gmode(soc_port_phy_timesync_global_mode_t mode,
                                            uint16 *value)
{

    switch (mode) {
    case SOC_PORT_PHY_TIMESYNC_MODE_FREE:
        *value = 0x1;
        break;
    case SOC_PORT_PHY_TIMESYNC_MODE_SYNCIN:
        *value = 0x2;
        break;
    case SOC_PORT_PHY_TIMESYNC_MODE_CPU:
        *value = 0x3;
        break;
    default:
        break;
    }

}

void _phy_84728_decode_gmode(uint16 value,
                                            soc_port_phy_timesync_global_mode_t *mode)
{

    switch (value & 0x3) {
    case 0x1:
        *mode = SOC_PORT_PHY_TIMESYNC_MODE_FREE;
        break;
    case 0x2:
        *mode = SOC_PORT_PHY_TIMESYNC_MODE_SYNCIN;
        break;
    case 0x3:
        *mode = SOC_PORT_PHY_TIMESYNC_MODE_CPU;
        break;
    default:
        break;
    }

}


void _phy_84728_encode_framesync_mode(soc_port_phy_timesync_framesync_mode_t mode,
                                            uint16 *value)
{

    switch (mode) {
    case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN0:
        *value = 1U << 0;
        break;
    case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN1:
        *value = 1U << 1;
        break;
    case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCOUT:
        *value = 1U << 2;
        break;
    case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_CPU:
        *value = 1U << 3;
        break;
    default:
        break;
    }

}

void _phy_84728_decode_framesync_mode(uint16 value,
                                            soc_port_phy_timesync_framesync_mode_t *mode)
{

    switch (value & 0xf) {
    case 1U << 0:
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN0;
        break;
    case 1U << 1:
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN1;
        break;
    case 1U << 2:
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCOUT;
        break;
    case 1U << 3:
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_CPU;
        break;
    default:
        break;
    }

}

void _phy_84728_encode_syncout_mode(soc_port_phy_timesync_syncout_mode_t mode,
                                            uint16 *value)
{

    switch (mode) {
    case SOC_PORT_PHY_TIMESYNC_SYNCOUT_DISABLE:
        *value = 0x0;
        break;
    case SOC_PORT_PHY_TIMESYNC_SYNCOUT_ONE_TIME:
        *value = 0x1;
        break;
    case SOC_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN:
        *value = 0x2;
        break;
    case SOC_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN_WITH_SYNC:
        *value = 0x3;
        break;
    }

}

void _phy_84728_decode_syncout_mode(uint16 value,
                                            soc_port_phy_timesync_syncout_mode_t *mode)
{

    switch (value & 0x3) {
    case 0x0:
        *mode = SOC_PORT_PHY_TIMESYNC_SYNCOUT_DISABLE;
        break;
    case 0x1:
        *mode = SOC_PORT_PHY_TIMESYNC_SYNCOUT_ONE_TIME;
        break;
    case 0x2:
        *mode = SOC_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN;
        break;
    case 0x3:
        *mode = SOC_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN_WITH_SYNC;
        break;
    }

}


int
phy_84728_timesync_config_set(int unit, soc_port_t port, soc_port_phy_timesync_config_t *conf)
{
    uint16 rx_control_reg = 0, tx_control_reg = 0, rx_tx_control_reg = 0,
           en_control_reg = 0, capture_en_reg = 0,
           nse_sc_8 = 0, nse_nco_3 = 0, value, mpls_control_reg = 0, temp = 0,
           gmode = 0, framesync_mode = 0, syncout_mode = 0, mask = 0,
           pulse_1_length, pulse_2_length, length_threshold, event_offset;
    uint32 interval;
    int i;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FLAGS) {

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_ENABLE) {
            SOC_IF_ERROR_RETURN
                (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc600, 0x0003));
#if 0
            SOC_IF_ERROR_RETURN
                (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc8f0, 0x0000));
#endif
            en_control_reg |= 3U;
            nse_sc_8 |= (1U << 12);
        }

        /* slice enable control reg */
        SOC_IF_ERROR_RETURN
            (BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc600, en_control_reg, 
                                           3U));

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE) {
            capture_en_reg |= 3U;
        }

        /* TXRX SOP TS CAPTURE ENABLE reg */
        SOC_IF_ERROR_RETURN
            (BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc607, capture_en_reg, 3U));

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE) {
            nse_sc_8 |= (1U << 13);
        }

        /* NSE SC 8 register */
        SOC_IF_ERROR_RETURN
            (BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc63a, nse_sc_8, 
                                           (1U << 12)| (1U << 13)));

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_RX_CRC_ENABLE) {
            rx_tx_control_reg |= (1U << 3);
        }

        /* RX TX CONTROL register */
        SOC_IF_ERROR_RETURN
            (BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc61c, rx_tx_control_reg, (1U << 3)));

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_8021AS_ENABLE) {
            rx_control_reg |= (1U << 3);
            tx_control_reg |= (1U << 3);
        }

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_L2_ENABLE) {
            rx_control_reg |= (1U << 2);
            tx_control_reg |= (1U << 2);
        }

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_IP4_ENABLE) {
            rx_control_reg |= (1U << 1);
            tx_control_reg |= (1U << 1);
        }

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_IP6_ENABLE) {
            rx_control_reg |= (1U << 0);
            tx_control_reg |= (1U << 0);
        }

        /* TX CONTROL register */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc618, tx_control_reg));
                                          
        /* RX CONTROL register */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc61a, rx_control_reg));
                                          
        if (conf->flags & SOC_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT) {
            nse_nco_3 &= (uint16) ~(1U << 14);
        } else {
            nse_nco_3 |= (1U << 14);
        }

        /* NSE NCO 3 register */
        SOC_IF_ERROR_RETURN
            (BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc630, nse_nco_3, 
                                           (1U << 14)));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ITPID) {
        /* VLAN TAG register */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc61d, conf->itpid));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID) {
        /* OUTER VLAN TAG register */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc61e, conf->otpid));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID2) {
        /* INNER VLAN TAG register */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc61f, conf->otpid2));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TS_DIVIDER) {
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc636, conf->ts_divider & 0xfff));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_LINK_DELAY) {
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc605, conf->rx_link_delay & 0xffff));
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc606, (conf->rx_link_delay >> 16) & 0xffff));
    }


    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE) {

        /* TIME CODE 5 register */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc610, (uint16)(conf->original_timecode.nanoseconds & 0xffff)));

        /* TIME CODE 4 register */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc60f, (uint16)((conf->original_timecode.nanoseconds >> 16) & 0xffff)));

        /* TIME CODE 3 register */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc60e, (uint16)(COMPILER_64_LO(conf->original_timecode.seconds) & 0xffff)));

        /* TIME CODE 2 register */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc60d, (uint16)((COMPILER_64_LO(conf->original_timecode.seconds) >> 16) & 0xffff)));

        /* TIME CODE 1 register */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc60c, (uint16)(COMPILER_64_HI(conf->original_timecode.seconds) & 0xffff)));
    }

    mask = 0;

    /* NSE SC 8 register */

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_GMODE) {
        _phy_84728_encode_gmode(conf->gmode,&gmode);
        mask |= (0x3 << 14);
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE) {
        _phy_84728_encode_framesync_mode(conf->framesync.mode, &framesync_mode);
        mask |= (0xf << 2) | (0x1 << 11);

        /* Divide by 6.4 without floating point */
        length_threshold = (conf->framesync.length_threshold * 10) / 64;
        event_offset = (conf->framesync.event_offset * 10) / 64;

        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc63c, length_threshold));
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc63b, event_offset));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE) {
        _phy_84728_encode_syncout_mode(conf->syncout.mode, &syncout_mode);
        mask |= (0x3 << 0);

        /* Divide by 6.4 without floating point */
        interval = (conf->syncout.interval * 10) / 64;
        pulse_1_length = (conf->syncout.pulse_1_length * 10) / 64;
        pulse_2_length = (conf->syncout.pulse_2_length * 10) / 64;

        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc635, interval & 0xffff));
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc634,
                ((pulse_2_length & 0x3) << 14) | ((interval >> 16) & 0x3fff)));
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc633,
                ((pulse_1_length & 0x1ff) << 7) | ((pulse_2_length >> 2) & 0x7f)));

        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc639,
                (uint16)(COMPILER_64_LO(conf->syncout.syncout_ts) & 0xfff0)));
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc638,
                (uint16)((COMPILER_64_LO(conf->syncout.syncout_ts) >> 16) & 0xffff)));
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc637,
                (uint16)(COMPILER_64_HI(conf->syncout.syncout_ts) & 0xffff)));
    }

    SOC_IF_ERROR_RETURN
        (BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc63a, (gmode << 14) | 
            ((framesync_mode & 0x2) ? (1U << 11) : (1U << 6)) | /* if bits 1 or 2 are set, turn off syncout */
            (framesync_mode << 2) | (syncout_mode << 0), mask));

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET) {
        /* RX_TS_Offset[19:0] = (1.0xC67B[7:4]) | (1.0xC60B[15:0]) */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr( pc, 0xc60b,
                       (uint16) (  conf->rx_timestamp_offset & 0x0ffff)) );
        SOC_IF_ERROR_RETURN
            (BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc67b,
                       (uint16) (((conf->rx_timestamp_offset & 0xf0000)) >> 12),
                       (0xf << 4)) );
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET) {
        /* TX_TS_Offset[19:0] = (1.0xC67B[3:0]) | (1.0xC60A[15:0]) */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr( pc, 0xc60a,
                       (uint16) (  conf->tx_timestamp_offset & 0x0ffff)) );
        SOC_IF_ERROR_RETURN
            (BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc67b,
                       (uint16) (((conf->tx_timestamp_offset & 0xf0000)) >> 16),
                       (0xf << 0)) );
    }

    value = mask =  0;
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE) {
        _phy_84728_encode_egress_message_mode(conf->tx_sync_mode, 0, &value);
        mask |= 0x3 << 0;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE) {
        _phy_84728_encode_egress_message_mode(conf->tx_delay_request_mode, 2, &value);
        mask |= 0x3 << 2;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE) {
        _phy_84728_encode_egress_message_mode(conf->tx_pdelay_request_mode, 4, &value);
        mask |= 0x3 << 4;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE) {
        _phy_84728_encode_egress_message_mode(conf->tx_pdelay_response_mode, 6, &value);
        mask |= 0x3 << 6;
    }
                                            
    /* TX EVENT MESSAGE MODE SEL register */
    SOC_IF_ERROR_RETURN
        (BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc601, value, mask)); 

    value = mask =  0;
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE) {
        _phy_84728_encode_ingress_message_mode(conf->rx_sync_mode, 0, &value);
        mask |= 0x3 << 0;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE) {
        _phy_84728_encode_ingress_message_mode(conf->rx_delay_request_mode, 2, &value);
        mask |= 0x3 << 2;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE) {
        _phy_84728_encode_ingress_message_mode(conf->rx_pdelay_request_mode, 4, &value);
        mask |= 0x3 << 4;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE) {
        _phy_84728_encode_ingress_message_mode(conf->rx_pdelay_response_mode, 6, &value);
        mask |= 0x3 << 6;
    }
                                            
    /* RX EVENT MESSAGE MODE SEL register */
    SOC_IF_ERROR_RETURN
        (BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc603, value, mask)); 

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE) {

        /* Initial ref phase [15:0] */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc623, (uint16)(COMPILER_64_LO(conf->phy_1588_dpll_ref_phase) & 0xffff)));

        /* Initial ref phase [31:16]  */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc622, (uint16)((COMPILER_64_LO(conf->phy_1588_dpll_ref_phase) >> 16) & 0xffff)));

        /*  Initial ref phase [47:32] */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc621, (uint16)(COMPILER_64_HI(conf->phy_1588_dpll_ref_phase) & 0xffff)));
    }


    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA) {
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc625, conf->phy_1588_dpll_ref_phase_delta & 0xffff));
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc624, (conf->phy_1588_dpll_ref_phase_delta >> 16) & 0xffff));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1) {
        /* DPLL K1 */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc626, conf->phy_1588_dpll_k1 & 0xff)); 
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2) {
        /* DPLL K2 */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc627, conf->phy_1588_dpll_k2 & 0xff)); 
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3) {
        /* DPLL K3 */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc628, conf->phy_1588_dpll_k3 & 0xff)); 
    }


    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL) {

        /* MPLS controls */
        if (conf->mpls_control.flags & SOC_PORT_PHY_TIMESYNC_MPLS_ENABLE) {
            mpls_control_reg |= (1U << 7) | (1U << 3);
        }

        if (conf->mpls_control.flags & SOC_PORT_PHY_TIMESYNC_MPLS_ENTROPY_ENABLE) {
            mpls_control_reg |= (1U << 6) | (1U << 2);
        }

        if (conf->mpls_control.flags & SOC_PORT_PHY_TIMESYNC_MPLS_SPECIAL_LABEL_ENABLE) {
            mpls_control_reg |= (1U << 4);
        }

        if (conf->mpls_control.flags & SOC_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE) {
            mpls_control_reg |= (1U << 5) | (1U << 1);
        }

        SOC_IF_ERROR_RETURN
            (BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc651, mpls_control_reg, 
                (1U << 7) | (1U << 3)| (1U << 6) | (1U << 2) | (1U << 4) | (1U << 5) | (1U << 1))); 

        /* special label [19:16] */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc653, (uint16)((conf->mpls_control.special_label >> 16 ) & 0xf)));
                                          
        /* special label [15:0] */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc652, (uint16)(conf->mpls_control.special_label & 0xffff)));

        for (i = 0; i < 10; i++ ) {
            SOC_IF_ERROR_RETURN
                (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc654 + i, (uint16)(conf->mpls_control.labels[i].value & 0xffff)));
            SOC_IF_ERROR_RETURN
                (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc661 + i, (uint16)(conf->mpls_control.labels[i].mask & 0xffff)));
        }
 
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc65e, (uint16)(
                ((conf->mpls_control.labels[3].value >> 4)  & 0xf000) | 
                ((conf->mpls_control.labels[2].value >> 8)  & 0x0f00) | 
                ((conf->mpls_control.labels[1].value >> 12) & 0x00f0) | 
                ((conf->mpls_control.labels[0].value >> 16) & 0x000f))));

        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc65f, (uint16)(
                ((conf->mpls_control.labels[7].value >> 4)  & 0xf000) | 
                ((conf->mpls_control.labels[6].value >> 8)  & 0x0f00) | 
                ((conf->mpls_control.labels[5].value >> 12) & 0x00f0) | 
                ((conf->mpls_control.labels[4].value >> 16) & 0x000f))));

        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc660, (uint16)(
                ((conf->mpls_control.labels[9].value >> 12) & 0x00f0) | 
                ((conf->mpls_control.labels[8].value >> 16) & 0x000f))));

        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc66b, (uint16)(
                ((conf->mpls_control.labels[3].mask >> 4)  & 0xf000) | 
                ((conf->mpls_control.labels[2].mask >> 8)  & 0x0f00) | 
                ((conf->mpls_control.labels[1].mask >> 12) & 0x00f0) | 
                ((conf->mpls_control.labels[0].mask >> 16) & 0x000f))));

        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc66c, (uint16)(
                ((conf->mpls_control.labels[7].mask >> 4)  & 0xf000) | 
                ((conf->mpls_control.labels[6].mask >> 8)  & 0x0f00) | 
                ((conf->mpls_control.labels[5].mask >> 12) & 0x00f0) | 
                ((conf->mpls_control.labels[4].mask >> 16) & 0x000f))));

        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc66d, (uint16)(
                ((conf->mpls_control.labels[9].mask >> 12) & 0x00f0) | 
                ((conf->mpls_control.labels[8].mask >> 16) & 0x000f))));

        /* label flags */
        temp = 0;
        for (i = 0; i < 8; i++ ) {
            temp |= conf->mpls_control.labels[i].flags & SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_OUT ? (0x2 << (i<<1)) : 0;
            temp |= (conf->mpls_control.labels[i].flags & SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_IN)  ? (0x1 << (i<<1)) : 0;
        }

        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc66e, temp)); 

        temp = 0;
        for (i = 0; i < 2; i++ ) {
            temp |= (conf->mpls_control.labels[i + 8].flags & SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_OUT) ? (0x2 << (i<<1)) : 0;
            temp |= (conf->mpls_control.labels[i + 8].flags & SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_IN)  ? (0x1 << (i<<1)) : 0;
        }

        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc66f, temp)); 
    }


    return SOC_E_NONE;
}

int
phy_84728_timesync_config_get(int unit, soc_port_t port, soc_port_phy_timesync_config_t *conf)
{
    uint16 rx_tx_control_reg = 0, tx_control_reg = 0,
              en_control_reg = 0, capture_en_reg = 0,
              nse_sc_8 = 0, nse_nco_3 = 0, temp1, temp2, temp3, value, mpls_control_reg;
    soc_port_phy_timesync_global_mode_t gmode = SOC_PORT_PHY_TIMESYNC_MODE_FREE;
    soc_port_phy_timesync_framesync_mode_t framesync_mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_NONE;
    soc_port_phy_timesync_syncout_mode_t syncout_mode = SOC_PORT_PHY_TIMESYNC_SYNCOUT_DISABLE;
    int i;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    conf->flags = 0;

    /* NSE SC 8 register */
    SOC_IF_ERROR_RETURN
        (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc63a, &nse_sc_8));

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FLAGS) {
        /* SLICE ENABLE CONTROL register */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc600, &en_control_reg)); 

        if (en_control_reg & 1U) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_ENABLE;
        }

        /* TXRX SOP TS CAPTURE ENABLE reg */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc607, &capture_en_reg)); 

        if (capture_en_reg & 1U) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;
        }

        /* NSE SC 8 register */
        /* SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc63a, &nse_sc_8));
         */

        if (nse_sc_8 & (1U << 13)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE;
        }

         /* RX TX CONTROL register */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc61c, &rx_tx_control_reg));

        if (rx_tx_control_reg & (1U << 3)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_RX_CRC_ENABLE;
        }

        /* TX CONTROL register */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc618, &tx_control_reg));
                                          
        if (tx_control_reg & (1U << 3)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_8021AS_ENABLE;
        }

        if (tx_control_reg & (1U << 2)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_L2_ENABLE;
        }

        if (tx_control_reg & (1U << 1)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_IP4_ENABLE;
        }

        if (tx_control_reg & (1U << 0)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_IP6_ENABLE;
        }

        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc630, &nse_nco_3));

        if (!(nse_nco_3 & (1U << 14))) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT;
        }
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_GMODE) {
        _phy_84728_decode_gmode((nse_sc_8 >> 14),&gmode);
        conf->gmode = gmode;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE) {
        _phy_84728_decode_framesync_mode((nse_sc_8 >> 2), &framesync_mode);
        conf->framesync.mode = framesync_mode;
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc63c, &temp1));
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc63b, &temp2));

        /* Multiply by 6.4 without floating point */
        conf->framesync.length_threshold = (temp1 * 64) / 10;
        conf->framesync.event_offset = (temp2 * 64) / 10;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE) {
        _phy_84728_decode_syncout_mode((nse_sc_8 >> 0), &syncout_mode);
        conf->syncout.mode = syncout_mode;

        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc635, &temp1));
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc634, &temp2));
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc633, &temp3));

        /* Multiply by 6.4 without floating point */
        conf->syncout.pulse_1_length = (((temp3 >> 7) & 0x1ff) * 64) / 10;
        conf->syncout.pulse_2_length = ((((temp3 & 0x7f) << 2) | ((temp2 >> 14) & 0x3)) * 64) / 10;
        conf->syncout.interval =  ((((temp2 & 0x3fff) << 16) | temp1) * 64) / 10;

        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc639, &temp1));
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc638, &temp2));
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc637, &temp3));

        COMPILER_64_SET(conf->syncout.syncout_ts, ((uint32)temp3), (((uint32)temp2<<16)|((uint32)temp1)));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ITPID) {
        /* VLAN TAG register */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc61d, &conf->itpid));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID) {
        /* OUTER VLAN TAG register */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc61e, &conf->otpid));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID2) {
        /* INNER VLAN TAG register */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc61f, &conf->otpid2));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TS_DIVIDER) {
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc636, &conf->ts_divider));
        conf->ts_divider &= 0xfff;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_LINK_DELAY) {
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc605, &temp1));
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc606, &temp2));

        conf->rx_link_delay = ((uint32)temp2 << 16) | temp1; 
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE) {
        /* TIME CODE 5 register */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc610, &temp1));

        /* TIME CODE 4 register */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc60f, &temp2));

        conf->original_timecode.nanoseconds = ((uint32)temp2 << 16) | temp1; 

        /* TIME CODE 3 register */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc60e, &temp1));

        /* TIME CODE 2 register */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc60d, &temp2));

        /* TIME CODE 1 register */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc60c, &temp3));

        /* conf->original_timecode.seconds = ((uint64)temp3 << 32) | ((uint32)temp2 << 16) | temp1; */

        COMPILER_64_SET(conf->original_timecode.seconds, ((uint32)temp3),  (((uint32)temp2<<16)|((uint32)temp1)));
    }

    /* Tx/Rx TS Offset */
    if (conf->validity_mask & (SOC_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET |
                               SOC_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET)) {
        /* MSB [19:16] of Tx/Rx TS Offset */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc67b, &temp1));
    }
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET) {
        /* RX_TS_Offset[19:0] = (1.0xC67B[7:4]) | (1.0xC60B[15:0]) */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc60b, &temp2));
        conf->rx_timestamp_offset = (((uint32)(temp1 & 0xf0)) << 12) | temp2;
    }
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET) {
        /* TX_TS_Offset[19:0] = (1.0xC67B[3:0]) | (1.0xC60A[15:0]) */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc60a, &temp2));
        conf->rx_timestamp_offset = (((uint32)(temp1 & 0x0f)) << 16) | temp2;
    }

    /* TX EVENT MESSAGE MODE SEL register */
    SOC_IF_ERROR_RETURN
        (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc601, &value)); 

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE) {
        _phy_84728_decode_egress_message_mode(value, 0, &conf->tx_sync_mode);
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE) {
        _phy_84728_decode_egress_message_mode(value, 2, &conf->tx_delay_request_mode);
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE) {
        _phy_84728_decode_egress_message_mode(value, 4, &conf->tx_pdelay_request_mode);
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE) {
        _phy_84728_decode_egress_message_mode(value, 6, &conf->tx_pdelay_response_mode);
    }
                                            
    /* RX EVENT MESSAGE MODE SEL register */
    SOC_IF_ERROR_RETURN
        (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc603, &value)); 

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE) {
        _phy_84728_decode_ingress_message_mode(value, 0, &conf->rx_sync_mode);
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE) {
        _phy_84728_decode_ingress_message_mode(value, 2, &conf->rx_delay_request_mode);
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE) {
        _phy_84728_decode_ingress_message_mode(value, 4, &conf->rx_pdelay_request_mode);
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE) {
        _phy_84728_decode_ingress_message_mode(value, 6, &conf->rx_pdelay_response_mode);
    }
                                            
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE) {
        /* Initial ref phase [15:0] */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc623, &temp1));

        /* Initial ref phase [31:16] */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc622, &temp2));

        /* Initial ref phase [47:32] */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc621, &temp3));

        /* conf->phy_1588_dpll_phase_initial = ((uint64)temp3 << 32) | ((uint32)temp2 << 16) | temp1; */

        COMPILER_64_SET(conf->phy_1588_dpll_ref_phase, ((uint32)temp3), (((uint32)temp2<<16)|((uint32)temp1)));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA) {
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc625, &temp1));
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc624, &temp2));

        conf->phy_1588_dpll_ref_phase_delta = ((uint32)temp2 << 16) | temp1; 
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1) {
        /* DPLL K1 */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc626, &conf->phy_1588_dpll_k1)); 
        conf->phy_1588_dpll_k1 &= 0xff;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2) {
        /* DPLL K2 */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc627, &conf->phy_1588_dpll_k2)); 
        conf->phy_1588_dpll_k2 &= 0xff;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3) {
        /* DPLL K3 */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc628, &conf->phy_1588_dpll_k3)); 
        conf->phy_1588_dpll_k3 &= 0xff;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL) {
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc651, &mpls_control_reg));
                                          
        conf->mpls_control.flags = 0;

        if (mpls_control_reg & (1U << 7)) {
            conf->mpls_control.flags |= SOC_PORT_PHY_TIMESYNC_MPLS_ENABLE;
        }

        if (mpls_control_reg & (1U << 6)) {
            conf->mpls_control.flags |= SOC_PORT_PHY_TIMESYNC_MPLS_ENTROPY_ENABLE;
        }

        if (mpls_control_reg & (1U << 4)) {
            conf->mpls_control.flags |= SOC_PORT_PHY_TIMESYNC_MPLS_SPECIAL_LABEL_ENABLE;
        }

        if (mpls_control_reg & (1U << 5)) {
            conf->mpls_control.flags |= SOC_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE;
        }

        /* special label [19:16] */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc653, &temp2));
                                          
        /* special label [15:0] */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc652, &temp1));

        conf->mpls_control.special_label = ((uint32)(temp2 & 0x0f) << 16) | temp1; 


        for (i = 0; i < 10; i++ ) {

            SOC_IF_ERROR_RETURN
                (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc654 + i, &temp1));
            conf->mpls_control.labels[i].value = temp1;

            SOC_IF_ERROR_RETURN
                (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc661 + i, &temp1));
            conf->mpls_control.labels[i].mask = temp1;
        }
 
        /* now get [19:16] of labels */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc65e, &temp1));

        conf->mpls_control.labels[0].value |= ((uint32)(temp1 & 0x000f) << 16); 
        conf->mpls_control.labels[1].value |= ((uint32)(temp1 & 0x00f0) << 12); 
        conf->mpls_control.labels[2].value |= ((uint32)(temp1 & 0x0f00) << 8); 
        conf->mpls_control.labels[3].value |= ((uint32)(temp1 & 0xf000) << 4); 


        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc65f, &temp1));

        conf->mpls_control.labels[4].value |= ((uint32)(temp1 & 0x000f) << 16); 
        conf->mpls_control.labels[5].value |= ((uint32)(temp1 & 0x00f0) << 12); 
        conf->mpls_control.labels[6].value |= ((uint32)(temp1 & 0x0f00) << 8); 
        conf->mpls_control.labels[7].value |= ((uint32)(temp1 & 0xf000) << 4); 


        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc660, &temp1));

        conf->mpls_control.labels[8].value |= ((uint32)(temp1 & 0x000f) << 16); 
        conf->mpls_control.labels[9].value |= ((uint32)(temp1 & 0x00f0) << 12); 

        /* now get [19:16] of masks */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc66b, &temp1));

        conf->mpls_control.labels[0].mask |= ((uint32)(temp1 & 0x000f) << 16); 
        conf->mpls_control.labels[1].mask |= ((uint32)(temp1 & 0x00f0) << 12); 
        conf->mpls_control.labels[2].mask |= ((uint32)(temp1 & 0x0f00) << 8); 
        conf->mpls_control.labels[3].mask |= ((uint32)(temp1 & 0xf000) << 4); 


        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc66c, &temp1));

        conf->mpls_control.labels[4].mask |= ((uint32)(temp1 & 0x000f) << 16); 
        conf->mpls_control.labels[5].mask |= ((uint32)(temp1 & 0x00f0) << 12); 
        conf->mpls_control.labels[6].mask |= ((uint32)(temp1 & 0x0f00) << 8); 
        conf->mpls_control.labels[7].mask |= ((uint32)(temp1 & 0xf000) << 4); 


        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc66d, &temp1));

        conf->mpls_control.labels[8].mask |= ((uint32)(temp1 & 0x000f) << 16); 
        conf->mpls_control.labels[9].mask |= ((uint32)(temp1 & 0x00f0) << 12); 


        /* label flags */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc66e, &temp1)); 
        for (i = 0; i < 8; i++ ) {
            conf->mpls_control.labels[i].flags |= temp1 & (0x2 << (i<<1)) ? SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_OUT : 0;
            conf->mpls_control.labels[i].flags |= temp1 & (0x1 << (i<<1)) ? SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_IN : 0;
        }
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc66f, &temp2)); 
        for (i = 0; i < 2; i++ ) {
            conf->mpls_control.labels[i + 8].flags |= temp2 & (0x2 << (i<<1)) ? SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_OUT : 0;
            conf->mpls_control.labels[i + 8].flags |= temp2 & (0x1 << (i<<1)) ? SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_IN : 0;
        }

    }
    return SOC_E_NONE;
}

int
phy_84728_timesync_control_set(int unit, soc_port_t port, soc_port_control_phy_timesync_t type, uint64 value)
{
    uint16 temp1, temp2; 
    uint32 value0;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    switch (type) {

    case SOC_PORT_CONTROL_PHY_TIMESYNC_CAPTURE_TIMESTAMP:
    case SOC_PORT_CONTROL_PHY_TIMESYNC_HEARTBEAT_TIMESTAMP:
        return SOC_E_FAIL;


    case SOC_PORT_CONTROL_PHY_TIMESYNC_NCOADDEND:
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc62f, (uint16)(COMPILER_64_LO(value) & 0xffff)));
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc62e, (uint16)((COMPILER_64_LO(value) >> 16) & 0xffff)));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_FRAMESYNC:

        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc63a, &temp1));
        temp2 = ((temp1 | (0x3 << 14) | (0x1 << 12)) & ~(0xf << 2)) | (0x1 << 5);
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc63a, temp2));
        sal_usleep(1);
        temp2 &= ~((0x1 << 5) | (0x1 << 12));
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc63a, temp2));
        sal_usleep(1);
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc63a, temp1));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_LOCAL_TIME:
        COMPILER_64_SHR(value, 4);
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc632, (uint16)(COMPILER_64_LO(value) & 0xffff)));
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc631, (uint16)((COMPILER_64_LO(value) >> 16) & 0xffff)));
        SOC_IF_ERROR_RETURN
            (BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc630, (uint16)(COMPILER_64_HI(value) & 0xfff), 0x0fff));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_LOAD_CONTROL:
        temp1 = 0;
        temp2 = 0;

        value0 = COMPILER_64_LO(value);

        if (value0 &  SOC_PORT_PHY_TIMESYNC_TN_LOAD) {
            temp1 |= 1U << 11;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_TN_ALWAYS_LOAD) {
            temp2 |= 1U << 11;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_TIMECODE_LOAD) {
            temp1 |= 1U << 10;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_TIMECODE_ALWAYS_LOAD) {
            temp2 |= 1U << 10;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_SYNCOUT_LOAD) {
            temp1 |= 1U << 9;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_SYNCOUT_ALWAYS_LOAD) {
            temp2 |= 1U << 9;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD) {
            temp1 |= 1U << 8;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_ALWAYS_LOAD) {
            temp2 |= 1U << 8;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD) {
            temp1 |= 1U << 7;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD) {
            temp2 |= 1U << 7;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD) {
            temp1 |= 1U << 6;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_ALWAYS_LOAD) {
            temp2 |= 1U << 6;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD) {
            temp1 |= 1U << 5;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_ALWAYS_LOAD) {
            temp2 |= 1U << 5;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD) {
            temp1 |= 1U << 4;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD) {
            temp2 |= 1U << 4;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD) {
            temp1 |= 1U << 3;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD) {
            temp2 |= 1U << 3;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K3_LOAD) {
            temp1 |= 1U << 2;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD) {
            temp2 |= 1U << 2;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K2_LOAD) {
            temp1 |= 1U << 1;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD) {
            temp2 |= 1U << 1;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K1_LOAD) {
            temp1 |= 1U << 0;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD) {
            temp2 |= 1U << 0;
        }
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc615, temp1));

        /* always load not supported at the moment */
        /*
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc615, temp2));
         */

        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT:
        temp1 = 0;

        value0 = COMPILER_64_LO(value);

        if (value0 &  SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT) {
            temp1 |= 1U << 1;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT) {
            temp1 |= 1U << 0;
        }
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc617, temp1));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT_MASK:
        temp1 = 0;

        value0 = COMPILER_64_LO(value);

        if (value0 &  SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_MASK) {
            temp1 |= 1U << 1;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT_MASK) {
            temp1 |= 1U << 0;
        }
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc616, temp1));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_TX_TIMESTAMP_OFFSET:
        value0 = COMPILER_64_LO(value);
        /* TX_TS_Offset[19:0] = (1.0xC67B[3:0]) | (1.0xC60A[15:0]) */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr( pc, 0xc60a,
                       (uint16) (  value0 & 0x0ffff)) );
        SOC_IF_ERROR_RETURN
            (BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc67b,
                       (uint16) (((value0 & 0xf0000)) >> 16), (0xf << 0)) );
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_RX_TIMESTAMP_OFFSET:
        value0 = COMPILER_64_LO(value);
        /* RX_TS_Offset[19:0] = (1.0xC67B[7:4]) | (1.0xC60B[15:0]) */
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr( pc, 0xc60b,
                       (uint16) (  value0 & 0x0ffff)) );
        SOC_IF_ERROR_RETURN
            (BSDK_MOD_PHY84728_LN_DEV1_PMDr(pc, 0xc67b,
                       (uint16) (((value0 & 0xf0000)) >> 12), (0xf << 4)) );
        break;

    default:
        return SOC_E_NONE;
        break;
    }

    return SOC_E_NONE;
}

int
phy_84728_timesync_control_get(int unit, soc_port_t port, soc_port_control_phy_timesync_t type, uint64 *value)
{
    uint16 value0 = 0;
    uint16 value1 = 0;
    uint16 value2 = 0;
    uint32 value4 = 0;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    switch (type) {
    case SOC_PORT_CONTROL_PHY_TIMESYNC_HEARTBEAT_TIMESTAMP:

        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc63d, 0x4));

        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc647, &value0)); 
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc646, &value1)); 
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc645, &value2)); 

        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc63d, 0x8));
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc63d, 0x0));

    /*    *value = (((uint64)value3) << 48) | (((uint64)value2) << 32) | (((uint64)value1) << 16) | ((uint64)value0); */
        COMPILER_64_SET((*value), ((uint32)value2),  (((uint32)value1<<16)|((uint32)value0)));

        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_CAPTURE_TIMESTAMP:

        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc63d, 0x1));

        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc640, &value0)); 
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc63f, &value1)); 
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc63e, &value2)); 

        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc63d, 0x2));
        SOC_IF_ERROR_RETURN
            (BSDK_WR_PHY84728_LN_DEV1_PMDr(pc, 0xc63d, 0x0));

    /*   *value = (((uint64)value3) << 48) | (((uint64)value2) << 32) | (((uint64)value1) << 16) | ((uint64)value0); */
         COMPILER_64_SET((*value), ((uint32)value2),  (((uint32)value1<<16)|((uint32)value0)));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_NCOADDEND:
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc62f, &value0));
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc62e, &value1));
        COMPILER_64_SET((*value), 0, (((uint32)value1<<16)|((uint32)value0)));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_LOCAL_TIME:
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc632, &value0)); 
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc631, &value1)); 
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc630, &value2)); 
        COMPILER_64_SET((*value), ((uint32)(value2 & 0x0fff)),  (((uint32)value1<<16)|((uint32)value0)));
        COMPILER_64_SHL((*value), 4);
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_LOAD_CONTROL:

        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc615, &value1));
        /* always load not supported at the moment */
        /*
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc615, &value2));
         */

        value4 = 0;

        if (value1 & (1U << 11)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_TN_LOAD;
        }
#if 0
        if (value2 & (1U << 11)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_TN_ALWAYS_LOAD;
        }
#endif
        if (value1 & (1U << 10)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_TIMECODE_LOAD;
        }
#if 0
        if (value2 & (1U << 10)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_TIMECODE_ALWAYS_LOAD;
        }
#endif
        if (value1 & (1U << 9)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_SYNCOUT_LOAD;
        }
#if 0
        if (value2 & (1U << 9)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_SYNCOUT_ALWAYS_LOAD;
        }
#endif
        if (value1 & (1U << 8)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD;
        }
        if (value1 & (1U << 8)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_ALWAYS_LOAD;
        }
        if (value1 & (1U << 7)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD;
        }
        if (value1 & (1U << 7)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD;
        }
        if (value1 & (1U << 6)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD;
        }
#if 0
        if (value2 & (1U << 6)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_ALWAYS_LOAD;
        }
#endif
        if (value1 & (1U << 5)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD;
        }
#if 0
        if (value2 & (1U << 5)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_ALWAYS_LOAD;
        }
#endif
        if (value1 & (1U << 4)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD;
        }
#if 0
        if (value2 & (1U << 4)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD;
        }
#endif
        if (value1 & (1U << 3)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD;
        }
#if 0
        if (value2 & (1U << 3)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD;
        }
#endif
        if (value1 & (1U << 2)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_K3_LOAD;
        }
#if 0
        if (value2 & (1U << 2)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD;
        }
#endif
        if (value1 & (1U << 1)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_K2_LOAD;
        }
#if 0
        if (value2 & (1U << 1)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD;
        }
#endif
        if (value1 & (1U << 0)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_K1_LOAD;
        }
#if 0
        if (value2 & (1U << 0)) {
            value4 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD;
        }
#endif
        COMPILER_64_SET((*value), 0, value4);

        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT:
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc617, &value1));

        value0 = 0;

        if (value1 & (1U << 1)) {
            value0 |= SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT;
        }
        if (value1 & (1U << 0)) {
            value0 |= SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT;
        }
        COMPILER_64_SET((*value), 0, (uint32)value0);

        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT_MASK:
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc616, &value1));

        value0 = 0;

        if (value1 & (1U << 1)) {
            value0 |= SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_MASK;
        }
        if (value1 & (1U << 0)) {
            value0 |= SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT_MASK;
        }
        COMPILER_64_SET((*value), 0, (uint32)value0);

        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_TX_TIMESTAMP_OFFSET:
        /* MSB [19:16] of Tx TS Offset */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc67b, &value1));
        /* TX_TS_Offset[19:0] = (1.0xC67B[3:0]) | (1.0xC60A[15:0]) */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc60a, &value0));
        COMPILER_64_SET((*value),  0,
                       (((uint32)(value1 & 0x0f)) << 16) | value0);
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_RX_TIMESTAMP_OFFSET:
        /* MSB [19:16] of Rx TS Offset */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc67b, &value1));
        /* RX_TS_Offset[19:0] = (1.0xC67B[7:4]) | (1.0xC60B[15:0]) */
        SOC_IF_ERROR_RETURN
            (BSDK_RD_PHY84728_LN_DEV1_PMDr(pc, 0xc60b, &value0));
        COMPILER_64_SET((*value),  0,
                       (((uint32)(value1 & 0xf0)) << 12) | value0);
        break;

    default:
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

int
bsdk_phy84728_reg_read(phy_ctrl_t *  pc, uint32 flags, uint8 reg_bank,
                          uint16 reg_addr, uint16 *data)
{
    int rv;
#if 0
    uint16 reg_data, xpmd_reg_sel = 0xffff;
#endif
    uint32 cl45_reg_addr;

    rv = SOC_E_NONE;

#if 0
    if (flags & BSDK_PHY84728_SYS_SIDE) {
        /* Select System side registers */
        cl45_reg_addr = SOC_PHY_CLAUSE45_ADDR(1, xpmd_reg_sel);
        reg_data = 0x01;
        SOC_IF_ERROR_RETURN(
            BSDK_PHY84728_IO_MDIO_WRITE(pc, cl45_reg_addr, reg_data));

        cl45_reg_addr = SOC_PHY_CLAUSE45_ADDR(reg_bank, reg_addr);
        rv = BSDK_PHY84728_IO_MDIO_READ(pc, cl45_reg_addr, data);

        /* De-Select System side registers */
        cl45_reg_addr = SOC_PHY_CLAUSE45_ADDR(1, xpmd_reg_sel);
        reg_data = 0x0;
        SOC_IF_ERROR_RETURN(
            BSDK_PHY84728_IO_MDIO_WRITE(pc, cl45_reg_addr, reg_data));

    } else {
#endif
    {
        cl45_reg_addr = SOC_PHY_CLAUSE45_ADDR(reg_bank, reg_addr);
        rv = BSDK_PHY84728_IO_MDIO_READ(pc, cl45_reg_addr, data);
    }

    return rv;
}

int
bsdk_phy84728_reg_write(phy_ctrl_t *  pc, uint32 flags, uint8 reg_bank,
                           uint16 reg_addr, uint16 data)
{
    int rv;
#if 0
    uint16 reg_data, xpmd_reg_sel = 0xffff;
#endif
    uint32 cl45_reg_addr;

    rv = SOC_E_NONE;

#if 0
    if (flags & BSDK_PHY84728_SYS_SIDE) {
        /* Select System side registers */
        cl45_reg_addr = SOC_PHY_CLAUSE45_ADDR(1, xpmd_reg_sel);
        reg_data = 0x01;
        SOC_IF_ERROR_RETURN(
            BSDK_PHY84728_IO_MDIO_WRITE(pc, cl45_reg_addr, reg_data));

        cl45_reg_addr = SOC_PHY_CLAUSE45_ADDR(reg_bank, reg_addr);
        rv = BSDK_PHY84728_IO_MDIO_WRITE(pc, cl45_reg_addr, data);

        /* De-Select System side registers */
        cl45_reg_addr = SOC_PHY_CLAUSE45_ADDR(1, xpmd_reg_sel);
        reg_data = 0x00;
        SOC_IF_ERROR_RETURN(
            BSDK_PHY84728_IO_MDIO_WRITE(pc, cl45_reg_addr, reg_data));

    } else {
#endif
    {
        cl45_reg_addr = SOC_PHY_CLAUSE45_ADDR(reg_bank, reg_addr);
        rv = BSDK_PHY84728_IO_MDIO_WRITE(pc, cl45_reg_addr, data);
    }

    return rv;
}


int
bsdk_phy84728_reg_modify(phy_ctrl_t *  pc, uint32 flags, uint8 reg_bank,
                            uint16 reg_addr, uint16 data, uint16 mask)
{
    uint16  tmp, otmp, reg_data;
    uint32 cl45_reg_addr = 0;
#if 0
    uint16 xpmd_reg_sel = 0xffff;
#endif

#if 0
    if (flags & BSDK_PHY84728_SYS_SIDE) {
        /* Select System side registers */
        cl45_reg_addr = SOC_PHY_CLAUSE45_ADDR(1, xpmd_reg_sel);
        reg_data = 0x01;
        SOC_IF_ERROR_RETURN(
            BSDK_PHY84728_IO_MDIO_WRITE(pc, cl45_reg_addr, reg_data));

        cl45_reg_addr = SOC_PHY_CLAUSE45_ADDR(reg_bank, reg_addr);

        SOC_IF_ERROR_RETURN
            (BSDK_PHY84728_IO_MDIO_READ(pc, cl45_reg_addr, &tmp));

        reg_data = data & mask; /* Mask off other bits */
        otmp = tmp;
        tmp &= ~(mask);
        tmp |= reg_data;

        if (otmp != tmp) {
            SOC_IF_ERROR_RETURN
                (BSDK_PHY84728_IO_MDIO_WRITE(pc, cl45_reg_addr, tmp));
        }

        /* De-Select System side registers */
        cl45_reg_addr = SOC_PHY_CLAUSE45_ADDR(1, xpmd_reg_sel);
        data = 0x00;
        SOC_IF_ERROR_RETURN(
            BSDK_PHY84728_IO_MDIO_WRITE(pc, cl45_reg_addr, data));

    } else {
#endif
    {
        cl45_reg_addr = SOC_PHY_CLAUSE45_ADDR(reg_bank, reg_addr);

        SOC_IF_ERROR_RETURN
            (BSDK_PHY84728_IO_MDIO_READ(pc, cl45_reg_addr, &tmp));

        reg_data = data & mask; /* Mask off other bits */
        otmp = tmp;
        tmp &= ~(mask);
        tmp |= reg_data;

        if (otmp != tmp) {
            SOC_IF_ERROR_RETURN
                (BSDK_PHY84728_IO_MDIO_WRITE(pc, cl45_reg_addr, tmp));
        }
    }
    return SOC_E_NONE;
}

/*
 * Variable:
 *    phy_84728_drv
 * Purpose:
 *    Phy Driver for BCM84728 PHY
 */

phy_driver_t phy_84728drv_xe = {
    "84728 1G/10-Gigabit PHY Driver",
    phy_84728_init,        /* Init */
    phy_null_reset,       /* Reset */
    phy_84728_link_get,    /* Link get   */
    phy_84728_enable_set,  /* Enable set */
    phy_null_enable_get,  /* Enable get */
    phy_84728_duplex_set,  /* Duplex set */
    phy_84728_duplex_get,  /* Duplex get */
    phy_84728_speed_set,   /* Speed set  */
    phy_84728_speed_get,   /* Speed get  */
    phy_84728_master_set,  /* Master set */
    phy_84728_master_get,  /* Master get */
    phy_84728_an_set,      /* ANA set */
    phy_84728_an_get,      /* ANA get */
    NULL,                 /* Local Advert set, deprecated */
    NULL,                 /* Local Advert get, deprecated */
    NULL,                 /* Remote Advert get, deprecated */
    phy_84728_lb_set,      /* PHY loopback set */
    phy_84728_lb_get,      /* PHY loopback set */
    phy_null_interface_set,  /* IO Interface set */
    phy_84728_interface_get, /* IO Interface get */
    NULL,                   /* pd_ability, deprecated */
    phy_84728_link_up,
    phy_84728_link_down,
    phy_84728_mdix_set,        /* phy_84728_mdix_set */
    phy_84728_mdix_get,        /* phy_84728_mdix_get */
    phy_84728_mdix_status_get, /* phy_84728_mdix_status_get */
    phy_84728_medium_config_set, /* medium config setting set */
    phy_84728_medium_config_get, /* medium config setting get */
    phy_84728_medium_status,        /* active medium */
    NULL,                    /* phy_cable_diag  */
    NULL,                    /* phy_link_change */
    phy_84728_control_set,    /* phy_control_set */
    phy_84728_control_get,    /* phy_control_get */
    NULL,                    /* phy_reg_read */
    NULL,                    /* phy_reg_write */
    NULL,                    /* phy_reg_modify */
    NULL,                    /* phy_notify */
    phy_84728_probe,         /* pd_probe  */
    phy_84728_ability_advert_set,  /* pd_ability_advert_set */
    phy_84728_ability_advert_get,  /* pd_ability_advert_get */
    phy_84728_ability_remote_get,  /* pd_ability_remote_get */
    phy_84728_ability_local_get,   /* pd_ability_local_get  */
    phy_84728_firmware_set,
    phy_84728_timesync_config_set,
    phy_84728_timesync_config_get,
    phy_84728_timesync_control_set,
    phy_84728_timesync_control_get
};
#else /* INCLUDE_PHY_84728 */
typedef int _phy_84728_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_84728 */
