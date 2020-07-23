/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy8481.c
 * Purpose:    PHY Driver support for Broadcom 8481/848xx Serial 10Gig
 *             transceiver with XAUI/XFI interface.
 */

#ifdef BCM_PLP_BASE_T_PHY  /* ePIL standalone driver */
#define  PHYMOD_SUPPORT
#define  INCLUDE_PHY_8481
#else    /* switch SDK driver */
#define  BCM848XX_DIAG_DSC_SUPPORT
#endif /* BCM_PLP_BASE_T_PHY */

#define  BCM848XX_STATE_MFG_DIAG_SUPPORT

#include <sal/types.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#ifdef BCM_PLP_BASE_T_PHY     /**  ePIL PLP standalone driver  **/
    #define  PHYMOD_SUPPORT
    #define  INCLUDE_PHY_8481
    #include "legacy.h"
    #include <soc/property.h>
#else                         /**  switch SDK driver           **/
    #define  BCM848XX_DIAG_DSC_SUPPORT
    #include <soc/drv.h>
    #include <soc/debug.h>
    #include "phydefs.h"      /* Must include before other phy related includes */
    #include "phyconfig.h"    /* Must be the first phy include after phydefs.h */
    #include "phyident.h"
    #include "phyreg.h"
    #include "phynull.h"
    #include "phyxehg.h"
#endif /* BCM_PLP_BASE_T_PHY */

#define  BCM848XX_STATE_MFG_DIAG_SUPPORT

#if defined(INCLUDE_PHY_8481)

#include "phy8481.h"

/*
#define PHY8481_ARM_MEM_DEBUG
#define PHY8481_DEBUG_AUTOMEDIUM
#define PHY8481_FW_DEBUG
#define PHY8185X_DEBUG_CMD_HANDLER
*/
extern uint8 bcm_8481_firmware[];
extern int bcm_8481_firmware_size;

extern uint8 bcm_8482A0_firmware[];
extern int bcm_8482A0_firmware_size;

extern uint8 bcm_8482B0_firmware[];
extern int bcm_8482B0_firmware_size;

extern uint8 bcm_8482B1_firmware[];
extern int bcm_8482B1_firmware_size;

extern uint8 bcm_84833_firmware[];
extern int bcm_84833_firmware_size;

extern uint8 bcm_84844_firmware[];
extern int bcm_84844_firmware_size;

extern uint8 bcm_84858_firmware[];
extern int bcm_84858_firmware_size;

extern uint8 bcm_8486x_firmware[];
extern int bcm_8486x_firmware_size;

extern uint8 bcm_84888A0_firmware[];
extern int bcm_84888A0_firmware_size;

extern uint8 bcm_84888B0_firmware[];
extern int bcm_84888B0_firmware_size;

extern void get_dsc_param_koi(void);
extern void get_dsc_param_orca(void);

extern void set_pc(phy_ctrl_t *pc);
extern void set_intpc(phy_ctrl_t *pc);

#define SOC_PORT_MEDIUM_XAUI      SOC_PORT_MEDIUM_FIBER
#define PHY_FLAGS_XAUI            PHY_FLAGS_FIBER
#define PHY_XAUI_MODE(unit, port) PHY_FIBER_MODE((unit), (port))
#define SOC_PA_MEDIUM_XAUI        SOC_PA_MEDIUM_FIBER

#define PHY8483X_MFG_TEST_SUPPORT   0x1
#define PHY8483X_STATE_TEST_SUPPORT 0x2
#define PHY8483X_FULL_SNR_SUPPORT   0x4
#define PHY8483X_TIMESYNC_ENABLED   0x8
#define PHY8483X_ATOMIC_ACCESS_SHD 0x10

/* PRBS */
#define PHY84834_FORTYG_PRBS_PATTERN_TESTING_CONTROL_STATUS         0x0135
#define PHY84834_40G_PRBS31             (1 << 7)               
#define PHY84834_40G_PRBS9              (1 << 6) 
#define PHY84834_40G_PRBS_TX_ENABLE     (1 << 3) 
#define PHY84834_40G_PRBS_RX_ENABLE     (1 << 0) 
#define PHY84834_40G_PRBS_ENABLE     (PHY84834_40G_PRBS_RX_ENABLE | \
	PHY84834_40G_PRBS_TX_ENABLE)

#define PHY84834_FORTYG_PRBS_RECEIVE_ERROR_COUNTER_LANE0            0x0140 
#define PHY84834_USER_PRBS_CONTROL_0_REGISTER                       0xCD14 
#define PHY84834_USER_PRBS_TX_INVERT   (1 << 4)
#define PHY84834_USER_PRBS_RX_INVERT   (1 << 15)
#define PHY84834_USER_PRBS_INVERT      (1 << 4 | 1 << 15)
#define PHY84834_USER_PRBS_ENABLE      (1 << 7)
#define PHY84834_USER_PRBS_TYPE_MASK   (0x7)

#define PHY84834_USER_PRBS_STATUS_0_REGISTER                        0xCD15 
#define PHY84834_GENSIG_8071_REGISTER                               0xCD16 
#define PHY84834_RESET_CONTROL_REGISTER                             0xCD17 

#define PHY8485X_MDIO_CONTROL_REGISTER                              0x4110
#define PHY8485X_TX_PRBS_REGISTER                                   0xD0E1
#define PHY848XX_TX_PRBS_ADDR32_REGISTER                            0xD203A1C2
#define PHY8485X_RX_PRBS_REGISTER                                   0xD0D1
#define PHY848XX_RX_PRBS_ADDR32_REGISTER                            0xD203A1A2
#define PHY8485X_PRBS_STATUS_REGISTER                               0xD0D9
#define PHY8485X_PRBS_ERR_COUNT_MSB_REGSTER                         0xD0DA
#define PHY8485X_PRBS_ERR_COUNT_LSB_REGSTER                         0xD0DB
#define PHY8488X_EN_CLK_MDC_XFI                                     0x8003

#define PHY8485X_USER_PRBS_TYPE_MASK                                0x1
#define PHY8485X_USER_PRBS_ENABLE                                   0x1

#define PHY8488X_MDIO_CLOCK_CONTROL_REGISTER         0x8003
#define PHY8488X_EN_CLK_MDC_XFI_SHIFT                0
#define PHY8488X_EN_CLK_MDC_XFI_MASK                 BIT(PHY8488X_EN_CLK_MDC_XFI_SHIFT)
#define PHY84834_DIAG_CMD_ACCESS_32BIT_XFI_ADDR      0xC020
#define PHY848XX_PRBS_TX_ADDR32_REGISTER             0xD203A1C2
#define PHY848XX_PRBS_RX_ADDR32_REGISTER             0xD203A1A2

#define ADDR32BIT_HIGH(_addr32)                      ((uint16) ((_addr32) >> 16))
#define ADDR32BIT_LOW(_addr32)                       ((uint16) ((_addr32) & 0xFFFF))
#define ADDR32BIT_WRITE                              0x0000
#define ADDR32BIT_READ                               0x0001

#define PHY848XX_PRBS_INVERT_SHFT                    4
#define PHY848XX_PRBS_POLYNOMIAL_SHFT                1
#define PHY848XX_PRBS_ENABLE_SHFT                    0
#define PHY848XX_PRBS_INVERT_MASK                    (0x1U << PHY848XX_PRBS_INVERT_SHFT)
#define PHY848XX_PRBS_POLYNOMIAL_MASK                (0x7U << PHY848XX_PRBS_POLYNOMIAL_SHFT)
#define PHY848XX_PRBS_ENABLE_MASK                    (0x1U << PHY848XX_PRBS_ENABLE_SHFT)
#define PHY848XX_PRBS_PARAMETER_MASK                 (PHY848XX_PRBS_INVERT_MASK |  \
                                                      PHY848XX_PRBS_POLYNOMIAL_MASK)



#define REG_GROUP_SHADOW18                                          0x18
#define REG_GROUP_SHADOW1c                                          0x1c
#define REG_GROUP_SHADOW_MISC_REG                                   0x17
#define REG_GROUP_SHADOW_EXP_REG                                    0xf

#define PHY8485X_PREEMPH_GET_PRE_TAP(_val) \
        ((_val) & 0xf)

#define PHY8485X_PREEMPH_CTRL_MAIN_TAP_SHFT     4
#define PHY8485X_PREEMPH_GET_MAIN_TAP(_val) \
        (((_val) >> PHY8485X_PREEMPH_CTRL_MAIN_TAP_SHFT) & 0x3f)

#define PHY8485X_PREEMPH_CTRL_POST_TAP1_SHFT    10
#define PHY8485X_PREEMPH_GET_POST_TAP1(_val) \
        (((_val) >> PHY8485X_PREEMPH_CTRL_POST_TAP1_SHFT) & 0x1f)

#define PHY8485X_PREEMPH_CTRL_POST_TAP2_SHFT    16
#define PHY8485X_PREEMPH_GET_POST_TAP2(_val) \
        (((_val) >> PHY8485X_PREEMPH_CTRL_POST_TAP2_SHFT) & 0x7)

#define PHY8485X_PREEMPH_CTRL_HPF_EN_SHFT       15
#define PHY8485X_PREEMPH_GET_HPF_EN(_val) \
        (((_val) >> PHY8485X_PREEMPH_CTRL_HPF_EN_SHFT) & 0x1)

/* 5 bits pre_tap for SOC_PHY_CONTROL_PREEMPHASIS */
#define PHY8488X_PREEMPH_CTL_PRE_TAP_MASK       0x1f
#define PHY8488X_PREEMPH_CTL_PRE_TAP_SHFT       0
#define PHY8488X_PREEMPH_GET_PRE_TAP(_val) \
        (((_val) >> PHY8488X_PREEMPH_CTL_PRE_TAP_SHFT) &  PHY8488X_PREEMPH_CTL_PRE_TAP_MASK)
#define PHY8488X_PREEMPH_SET_PRE_TAP(_val) \
        (((_val)  & PHY8488X_PREEMPH_CTL_PRE_TAP_MASK) << PHY8488X_PREEMPH_CTL_PRE_TAP_SHFT)
/* 7 bits main_tap for SOC_PHY_CONTROL_PREEMPHASIS */
#define PHY8488X_PREEMPH_CTL_MAIN_TAP_MASK      0x3f
#define PHY8488X_PREEMPH_CTL_MAIN_TAP_SHFT      5
#define PHY8488X_PREEMPH_GET_MAIN_TAP(_val) \
        (((_val) >> PHY8488X_PREEMPH_CTL_MAIN_TAP_SHFT) &  PHY8488X_PREEMPH_CTL_MAIN_TAP_MASK)
#define PHY8488X_PREEMPH_SET_MAIN_TAP(_val) \
        (((_val)  & PHY8488X_PREEMPH_CTL_MAIN_TAP_MASK) << PHY8488X_PREEMPH_CTL_MAIN_TAP_SHFT)
/* 6 bits post1_tap for SOC_PHY_CONTROL_PREEMPHASIS */
#define PHY8488X_PREEMPH_CTL_POST1_MASK         0x3f
#define PHY8488X_PREEMPH_CTL_POST1_SHFT         12
#define PHY8488X_PREEMPH_GET_POST1_TAP(_val) \
        (((_val) >> PHY8488X_PREEMPH_CTL_POST1_SHFT) &  PHY8488X_PREEMPH_CTL_POST1_MASK)
#define PHY8488X_PREEMPH_SET_POST1_TAP(_val) \
        (((_val)  & PHY8488X_PREEMPH_CTL_POST1_MASK) << PHY8488X_PREEMPH_CTL_POST1_SHFT)
/* 5 bits post2_tap for SOC_PHY_CONTROL_PREEMPHASIS  (signed) */
#define PHY8488X_PREEMPH_CTL_POST2_MASK         0x1f
#define PHY8488X_PREEMPH_CTL_POST2_SHFT         18
#define PHY8488X_PREEMPH_GET_POST2_TAP(_val) \
        (((_val) >> PHY8488X_PREEMPH_CTL_POST2_SHFT) &  PHY8488X_PREEMPH_CTL_POST2_MASK)
#define PHY8488X_PREEMPH_SET_POST2_TAP(_val) \
        (((_val)  & PHY8488X_PREEMPH_CTL_POST2_MASK) << PHY8488X_PREEMPH_CTL_POST2_SHFT)
/* 4 bits post3_tap for SOC_PHY_CONTROL_PREEMPHASIS  (signed) */
#define PHY8488X_PREEMPH_CTL_POST3_MASK         0x0f
#define PHY8488X_PREEMPH_CTL_POST3_SHFT         24
#define PHY8488X_PREEMPH_GET_POST3_TAP(_val) \
        (((_val) >> PHY8488X_PREEMPH_CTL_POST3_SHFT) &  PHY8488X_PREEMPH_CTL_POST3_MASK)
#define PHY8488X_PREEMPH_SET_POST3_TAP(_val) \
        (((_val)  & PHY8488X_PREEMPH_CTL_POST3_MASK) << PHY8488X_PREEMPH_CTL_POST3_SHFT)

/* typedefs */
typedef struct {
    int           flags;
    soc_timeout_t to;
    uint8         *firmware;
    int           firmware_len;
    int           dl_dividend;
    int           dl_divisor;
    uint32        phy_ext_rom_boot;
    int           sys_forced_cl72;
    int           link_down;
    int           lb_speed;    /* intended speed for PHY loopback */
} PHY8481_PRIV_t;

#define LINK_DOWN(_pc) (((PHY8481_PRIV_t *)((_pc) + 1))->link_down)
#define FLAGS(_pc) (((PHY8481_PRIV_t *)((_pc) + 1))->flags)
#define TO(_pc) (((PHY8481_PRIV_t *)((_pc) + 1))->to)
#define FIRMWARE(_pc) (((PHY8481_PRIV_t *)((_pc) + 1))->firmware)
#define FIRMWARE_LEN(_pc) (((PHY8481_PRIV_t *)((_pc) + 1))->firmware_len)
#define DL_DIVIDEND(_pc) (((PHY8481_PRIV_t *)((_pc) + 1))->dl_dividend)
#define DL_DIVISOR(_pc) (((PHY8481_PRIV_t *)((_pc) + 1))->dl_divisor)
#define PHY_EXT_ROM_BOOT(_pc) (((PHY8481_PRIV_t *)((_pc) + 1))->phy_ext_rom_boot)
#define SYS_FORCED_CL72(_pc) (((PHY8481_PRIV_t *)((_pc) + 1))->sys_forced_cl72)
#define LB_SPEED(_pc) (((PHY8481_PRIV_t *)((_pc) + 1))->lb_speed)

#define PHY_ADDR_NONE  0xFF
#define NXT_PC(pc) ((phy_ctrl_t *)((pc)->driver_data))
#define LAST_IN_CHAIN(pc) (PHY_IS_CHAINED((pc)->unit, (pc)->port) && !NXT_PC(pc))

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

#define PHYDRV_CALL_ARG4(pc,name,arg0,arg1,arg2,arg3) \
    do { \
       if (pc->driver_data) { \
           int rv; \
           phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE(pc->unit, pc->port); \
           EXT_PHY_SW_STATE(pc->unit, pc->port) = (phy_ctrl_t *)(pc->driver_data); \
           rv=name(((phy_ctrl_t *)(pc->driver_data))->pd,(pc)->unit,(pc)->port,arg0, \
                        arg1,arg2,arg3); \
           EXT_PHY_SW_STATE(pc->unit, pc->port) = tmp_pc; \
           if (SOC_FAILURE(rv)) { \
               return rv; \
           } \
       } \
    } while(0)

/* default MDIO addresses of phy device on the xaui port, per port based*/
static uint8 ext_phy_dft_addr1[] = {
    PHY_ADDR_NONE,  /* 0  */
    PHY_ADDR_NONE,  /* 1  */
    PHY_ADDR_NONE,  /* 2  */
    PHY_ADDR_NONE,  /* 3  */
    PHY_ADDR_NONE,  /* 4  */
    PHY_ADDR_NONE,  /* 5  */
    PHY_ADDR_NONE,  /* 6  */
    PHY_ADDR_NONE,  /* 7  */
    PHY_ADDR_NONE,  /* 8  */
    PHY_ADDR_NONE,  /* 9  */
    PHY_ADDR_NONE,  /* 10  */
    PHY_ADDR_NONE,  /* 11  */
    PHY_ADDR_NONE,  /* 12  */
    PHY_ADDR_NONE,  /* 13  */
    PHY_ADDR_NONE,  /* 14  */
    PHY_ADDR_NONE,  /* 15  */
    PHY_ADDR_NONE,  /* 16  */
    PHY_ADDR_NONE,  /* 17  */
    PHY_ADDR_NONE,  /* 18  */
    PHY_ADDR_NONE,  /* 19  */
    PHY_ADDR_NONE,  /* 20  */
    PHY_ADDR_NONE,  /* 21  */
    PHY_ADDR_NONE,  /* 22  */
    PHY_ADDR_NONE,  /* 23  */
    PHY_ADDR_NONE,  /* 24  */
    PHY_ADDR_NONE,  /* 25  */
    PHY_ADDR_NONE,  /* 26  */
    PHY_ADDR_NONE,  /* 27  */
    PHY_ADDR_NONE,  /* 28  */
    PHY_ADDR_NONE,  /* 29  */
    PHY_ADDR_NONE,  /* 30  */
    PHY_ADDR_NONE,  /* 31  */
    PHY_ADDR_NONE,  /* 32  */
    PHY_ADDR_NONE,  /* 33  */
    PHY_ADDR_NONE,  /* 34  */
    PHY_ADDR_NONE,  /* 35  */
    PHY_ADDR_NONE,  /* 36  */
    PHY_ADDR_NONE   /* 37  */
};

#define PHY_FW_HANDSHAKE(unit, port) \
    do { \
        if (PHY_HS_CAPABLE(unit, port)) { \
            soc_timeout_t  to; \
            uint16         status; \
            int            rv; \
            phy_ctrl_t    *pc = EXT_PHY_SW_STATE(unit, port); \
            sal_usleep(100); \
            soc_timeout_init(&to, 2000000, 0); \
            do { \
                rv = READ_PHY8481_TOPLVL1_REG(unit, pc, 0x400e, &status); \
                if (((status & 0x2) == 0) || SOC_FAILURE(rv)) { \
                    break; \
                } \
            } while (!soc_timeout_check(&to)); \
            if (((status & 0x2) != 0) && (status != 0xffff)) { \
                LOG_WARN(BSL_LS_SOC_PHY, \
                         (BSL_META_U(unit, \
                                     "PHY8481 firmware handshake failed: u=%d p=%d status=0x%04x\n"), \
                          unit, port, status)); \
            } \
        } else { \
            sal_usleep(700000); \
        } \
    } while (0)

#define PHY_IS_BCM84823(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                                 PHY_BCM84823_OUI, \
                                                 PHY_BCM84823_MODEL) )

#define PHY_IS_BCM84833(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                                 PHY_BCM84833_OUI, \
                                                 PHY_BCM84833_MODEL) )

#define PHY_IS_BCM84834(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                                 PHY_BCM84834_OUI, \
                                                 PHY_BCM84834_MODEL) )

#define PHY_IS_BCM84844(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                                 PHY_BCM84844_OUI, \
                                                 PHY_BCM84844_MODEL) )

#define PHY_IS_BCM8484X_A0(_pc) (PHY_IS_BCM8484X(_pc) \
                                   && ((_pc)->phy_rev == 0x8 ))


#define PHY_IS_BCM84836(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                                 PHY_BCM84836_OUI, \
                                                 PHY_BCM84836_MODEL) )

#define PHY_IS_BCM84846(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                                 PHY_BCM84846_OUI, \
                                                 PHY_BCM84846_MODEL) )

#define PHY_IS_BCM84848(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                                 PHY_BCM84848_OUI, \
                                                 PHY_BCM84848_MODEL) )

#define PHY_IS_BCM8483X(_pc) \
                               ( ((_pc)->phy_oui == PHY_BCM8483X_OUI) && \
                                 (((_pc)->phy_model == PHY_BCM84833_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84834_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84835_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84836_MODEL)) )

#define PHY_IS_BCM8484X(_pc) \
                               ( ((_pc)->phy_oui == PHY_BCM8484X_OUI) && \
                                 (((_pc)->phy_model == PHY_BCM84844_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84848_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84846_MODEL)) )

#define PHY_IS_BCM84858(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                                 PHY_BCM84858_OUI, \
                                                 PHY_BCM84858_MODEL) )

#define PHY_IS_BCM84856(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                                 PHY_BCM84856_OUI, \
                                                 PHY_BCM84856_MODEL) )

#define PHY_IS_BCM8485X(_pc) \
                               ( ((_pc)->phy_oui == PHY_BCM8485X_OUI) && \
                                 (((_pc)->phy_model == PHY_BCM84858_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84856_MODEL)) )
#define PHY_IS_BCM8486X(_pc) \
                               ( ((_pc)->phy_oui == PHY_BCM8486X_OUI) && \
                                 (((_pc)->phy_model == PHY_BCM84860_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84861_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84864_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84868_MODEL)) )

#define PHY_IS_BCM84888(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                 PHY_BCM84888_OUI, \
                                 PHY_BCM84888_MODEL) )

#define PHY_IS_BCM84884(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                 PHY_BCM84884_OUI, \
                                 PHY_BCM84884_MODEL) )

#define PHY_IS_BCM84888E(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                 PHY_BCM84888E_OUI, \
                                 PHY_BCM84888E_MODEL) )

#define PHY_IS_BCM84884E(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                 PHY_BCM84884E_OUI, \
                                 PHY_BCM84884E_MODEL) )

#define PHY_IS_BCM84881(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                 PHY_BCM84881_OUI, \
                                 PHY_BCM84881_MODEL) )

#define PHY_IS_BCM84880(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                 PHY_BCM84880_OUI, \
                                 PHY_BCM84880_MODEL) )

#define PHY_IS_BCM84888S(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                 PHY_BCM84888S_OUI, \
                                 PHY_BCM84888S_MODEL) )

#define PHY_IS_BCM84885(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                 PHY_BCM84885_OUI, \
                                 PHY_BCM84885_MODEL) )

#define PHY_IS_BCM84886(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                 PHY_BCM84886_OUI, \
                                 PHY_BCM84886_MODEL) )

#define PHY_IS_BCM84887(_pc)   ( PHY_MODEL_CHECK((_pc), \
                                 PHY_BCM84887_OUI, \
                                 PHY_BCM84887_MODEL) )

#define PHY_IS_BCM8488X(_pc) \
                               ( ((_pc)->phy_oui == PHY_BCM8488X_OUI) && \
                                 (((_pc)->phy_model == PHY_BCM84888_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84884_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84888E_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84881_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84880_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84884E_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84888S_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84885_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84886_MODEL) || \
                                  ((_pc)->phy_model == PHY_BCM84887_MODEL) ) )

#define PHY_IS_BCM8488X_A0(_pc) \
                               ( PHY_IS_BCM8488X(_pc) && \
                                     ((_pc)->phy_rev == 0x0) )

#define PHY_IS_BCM8486X_AND_UP(_pc) \
                               ( PHY_IS_BCM8486X((_pc)) || \
                                 PHY_IS_BCM8488X(_pc) )

#define PHY_IS_BCM8485X_AND_UP(_pc) \
                               ( PHY_IS_BCM8485X((_pc)) || \
                                 PHY_IS_BCM8486X((_pc)) || \
                                 PHY_IS_BCM8488X(_pc) )

#define PHY_IS_BCM8484X_AND_UP(_pc) \
                               ( PHY_IS_BCM8484X((_pc)) || \
                                 PHY_IS_BCM8485X((_pc)) || \
                                 PHY_IS_BCM8486X((_pc)) || \
                                 PHY_IS_BCM8488X(_pc) )

#define PHY_IS_BCM8483X_AND_UP(_pc) \
                               ( PHY_IS_BCM8483X((_pc)) || \
                                 PHY_IS_BCM8484X((_pc)) || \
                                 PHY_IS_BCM8485X((_pc)) || \
                                 PHY_IS_BCM8486X((_pc)) || \
                                 PHY_IS_BCM8488X(_pc) )
                                 
#define PHY8481_IS_READY_FOR_CMD(_pc, _status) \
                               ((PHY_IS_BCM8483X(_pc) || PHY_IS_BCM8484X(_pc)) \
                               ? ( _status & PHY84834_CMD_OPEN_FOR_CMDS)       \
                               : ((_status != PHY8485X_CMD_SYSTEM_BUSY) &&     \
                                  (_status != PHY8485X_CMD_IN_PROGRESS)) )

#define PHY8481_IS_OPEN_FOR_CMD(_pc, _status) \
                               ((PHY_IS_BCM8483X(_pc) || PHY_IS_BCM8484X(_pc)) \
                               ? ( _status &  PHY84834_CMD_OPEN_FOR_CMDS)      \
                               : ((_status == PHY8485X_CMD_OPEN_FOR_CMDS) ||   \
                                  (_status == PHY8485X_CMD_COMPLETE_ERROR)) )

#define PHY8481_IS_CMD_COMPLETE(_pc, _status) \
                               ((PHY_IS_BCM8483X(_pc) || PHY_IS_BCM8484X(_pc)) \
                               ? ((_status & PHY8481_CMD_COMPLETE_PASS) ||     \
                                  (_status & PHY8481_CMD_COMPLETE_ERROR))      \
                               : ((_status == PHY8485X_CMD_OPEN_FOR_CMDS) ||   \
                                  (_status == PHY8485X_CMD_COMPLETE_ERROR)) )

#define PHY8481_SET_FIBER_PREFERRED(unit, port, pc)\
                                do {    \
                                        int fiber_preferred; \
                                        uint16 data16;    \
                                        if (!PHY_FORCED_COPPER_MODE(unit, port)) {  \
                                        /* Read the strap value and set fiber pref accordingly. */  \
                                        SOC_IF_ERROR_RETURN \
                                        (READ_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc, &data16));\
                                        fiber_preferred = soc_property_port_get(unit, port, spn_PHY_FIBER_PREF, (data16 & (1U << 8)) ? 1 : 0);\
                                        pc->automedium = TRUE; /* Always TRUE for this PHY */ }  \
                                        else { fiber_preferred = FALSE; pc->automedium = FALSE; } \
                                        pc->copper.preferred = !fiber_preferred; \
                                        pc->fiber.preferred = fiber_preferred; \
                                   }while(0)

#define PHY8481_ENABLE_MEDIUM(_pc)\
                                do {    _pc->copper.enable         = TRUE;\
                                        _pc->copper.force_duplex    = TRUE;\
                                        _pc->copper.force_speed    = 10000;\
                                        _pc->copper.autoneg_enable = TRUE;\
                                        _pc->copper.master = SOC_PORT_MS_AUTO;\
                                        _pc->copper.mdix = SOC_PORT_MDIX_AUTO;\
                                        _pc->fiber.enable          = TRUE;\
                                        _pc->fiber.force_speed     = 10000;\
                                        _pc->fiber.force_duplex    = TRUE;\
                                        _pc->fiber.autoneg_enable = FALSE;\
                                    } while(0)                                  

STATIC int phy_8481_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int phy_8481_speed_get(int unit, soc_port_t port, int *speed);
STATIC int phy_8481_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int phy_8481_an_get(int unit, soc_port_t port, int *an, int *an_done);
STATIC int phy_8481_an_set(int unit, soc_port_t port, int an);
STATIC int phy_8481_speed_set(int unit, soc_port_t port, int speed);
STATIC int phy_8481_duplex_set(int unit, soc_port_t port, int duplex);
STATIC int phy_8481_link_up(int unit, soc_port_t port);
STATIC int phy_8481_enable_set(int unit, soc_port_t port, int enable);
STATIC int phy_8481_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode);

STATIC int _phy_8481_xaui_an_get(int unit, soc_port_t port, int *an, int *an_done);
STATIC int _phy_8481_xaui_an_set(int unit, soc_port_t port, int an);
STATIC int _phy_8481_medium_change(int unit, soc_port_t port, int force_update);
STATIC int _phy_8481_medium_config_update(int unit, soc_port_t port,soc_phy_config_t *cfg);
STATIC int _phy_8481_xaui_speed_get(int unit, soc_port_t port, int *speed);
STATIC int _phy_8481_xaui_speed_set(int unit, soc_port_t port, int speed);
STATIC int _phy_8481_medium_check(int unit, soc_port_t port, int *medium);
STATIC int _phy_8481_xaui_enable_set(int unit, soc_port_t port, int enable);
STATIC int _phy_8481_xaui_enable_get(int unit, soc_port_t port, int *enable);
STATIC int _phy_8481_copper_enable_set(int unit, soc_port_t port, int enable);
STATIC int _phy_8481_copper_enable_get(int unit, soc_port_t port, int *enable);
STATIC int _phy_8481_halt(int unit, phy_ctrl_t *pc);
STATIC int _phy_8481_write_to_arm(int unit, phy_ctrl_t *pc, uint32 addr, uint8 *data,int len);
STATIC int _phy_8481_restart(int unit, phy_ctrl_t *pc);
STATIC int _phy_8481_copper_an_set(int unit, soc_port_t port, int an);
STATIC int _phy_8481_copper_lb_get(int unit, soc_port_t port, int *enable);
STATIC int _phy_8481_xaui_nxt_dev_probe(int unit, soc_port_t port);
/* STATIC */ int _phy84834_top_level_cmd_set(int unit,phy_ctrl_t *pc, uint16 cmd, uint16 arg[], int size);
/* STATIC */ int _phy84834_top_level_cmd_get(int unit,phy_ctrl_t *pc, uint16 cmd, uint16 arg[], int size);
/* STATIC */ int _phy84834_top_level_cmd_set_v2(int unit,phy_ctrl_t *pc, uint16 cmd, uint16 arg[], int size);
/* STATIC */ int _phy84834_top_level_cmd_get_v2(int unit,phy_ctrl_t *pc, uint16 cmd, uint16 arg[], int size);
STATIC int _phy8481_adjust_xfi_tx_preemphasis(int unit, int port, phy_ctrl_t *pc, uint32 phy_long_xfi);
STATIC int _phy848xx_short_reach_enable_set(int unit,phy_ctrl_t *pc, uint16 enable );
STATIC int _phy_84834_power_mode_set (int unit, soc_port_t port, uint32 mode);
STATIC int _phy_84834_power_mode_get (int unit, soc_port_t port, uint32 *mode);
STATIC int _phy_8481_check_firmware(int unit,phy_ctrl_t *pc);
STATIC int _phy_8481_copper_speed_get(int unit, soc_port_t port, int *speed);
STATIC int _phy_8481_copper_duplex_get(int unit, soc_port_t port, int *duplex);
STATIC int _phy_8481_copper_ability_advert_get(int unit, soc_port_t port,soc_port_ability_t *ability);
STATIC int _phy_8481_copper_an_get(int unit, soc_port_t port, int *an, int *an_done);
STATIC int _phy_8481_xaui_duplex_get(int unit, soc_port_t port, int *duplex);
STATIC int _phy_8481_xaui_ability_advert_get(int unit, soc_port_t port,soc_port_ability_t *ability);
STATIC int _phy8485x_shadow_reg_read(int unit, phy_ctrl_t *pc, uint16 data1, uint16 data2, uint16 data3 , uint16 data4, uint16 *value);
STATIC int _phy8485x_shadow_reg_write(int unit, phy_ctrl_t *pc, uint16 reg_group, uint16 reg_addr, uint16 bit_mask, uint16 bit_shift, uint16 value);
STATIC int _phy84834_xfi_reg_set(int unit, phy_ctrl_t *pc, uint16 bank, uint16 offset, uint16 value);
STATIC int _phy_84834_control_set(int unit, soc_port_t port, int intf, int lane, soc_phy_control_t type, uint32 value);
STATIC int _phy_84834_control_get(int unit, soc_port_t port, int intf, int lane, soc_phy_control_t type, uint32 *value);

int
phy_8481_reg_xge_read(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                uint8 dev_addr, uint16 reg_addr, uint16 *data);
int
phy_8481_reg_xge_write(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                uint8 dev_addr, uint16 reg_addr, uint16 data);
int
phy_8481_reg_xge_modify(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                uint8 dev_addr, uint16 reg_addr, uint16 data, uint16 mask);
#ifdef PHY8185X_DEBUG_CMD_HANDLER
STATIC int _phy8485x_emi_mode_enable_get(int unit, phy_ctrl_t *pc, uint16 *status);
STATIC int _phy8485x_emi_mode_enable_set(int unit, phy_ctrl_t *pc, uint16 status);
STATIC int _phy8485x_sub_lf_rf_status_get(int unit, phy_ctrl_t *pc, uint16 *status);
STATIC int _phy8485x_sub_lf_rf_set(int unit, phy_ctrl_t *pc);
STATIC int _phy8485x_sub_lf_rf_clear(int unit, phy_ctrl_t *pc);
STATIC int _phy8485x_snr_get(int unit, phy_ctrl_t *pc);
STATIC int _phy8485x_current_temp_get(int unit, phy_ctrl_t *pc);
STATIC int _phy8485x_upper_temp_warn_level_set(int unit, phy_ctrl_t *pc, uint16 temp);
STATIC int _phy8485x_upper_temp_warn_level_get(int unit, phy_ctrl_t *pc, uint16 *temp);
STATIC int _phy8485x_lower_temp_warn_level_set(int unit, phy_ctrl_t *pc, uint16 temp);
STATIC int _phy8485x_lower_temp_warn_level_get(int unit, phy_ctrl_t *pc, uint16 *temp);
STATIC int _phy8486x_xfi_2p5g_mode_get(int unit, phy_ctrl_t *pc, uint16 *mode_2p5g);
STATIC int _phy84888_xfi_2p5g_mode_get(int unit, phy_ctrl_t *pc, uint16 *mode_2p5g, uint16 *mode_5g);
STATIC int _phy_848xx_led_type_get (int unit, soc_port_t port, uint32 *led_type, uint32 *led_mode);
STATIC int _phy_848xx_led_type_set (int unit, soc_port_t port, uint32 led_type, uint32 led_mode);
STATIC int _phy_848xx_pause_frame_mode_get (int unit, soc_port_t port, uint32 *enable); 
STATIC int _phy_848xx_pause_frame_mode_set (int unit, soc_port_t port, uint32 enable);
#endif /* PHY8185X_DEBUG_CMD_HANDLER */
STATIC int _phy_848xx_mgbase_802_3bz_type_get (int unit, phy_ctrl_t *pc, uint16 *frame_type, uint16 *frame_mode); 
STATIC int _phy_848xx_mgbase_802_3bz_type_set (int unit, phy_ctrl_t *pc, uint16 frame_type, uint16 frame_mode); 

STATIC int _phy848xx_xfi_2p5g_5g_mode_set(int unit, phy_ctrl_t *pc, uint16  mode_2p5g);
STATIC int _phy_8485x_force_cl72_config(int unit, phy_ctrl_t *pc, uint16 enable);
STATIC int _phy_8485x_force_cl72_status(int unit, phy_ctrl_t *pc, uint16 *value);
STATIC int _phy84834_jumbo_packet_set(int unit, phy_ctrl_t *pc, uint16 mode);
#if 0
STATIC int _phy84834_jumbo_packet_get(int unit, phy_ctrl_t *pc, uint16 *mode);
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int _phy84834_top_level_cmd_get_dispatch_v2(int unit,phy_ctrl_t *pc, uint16 cmd);
STATIC int _phy84834_top_level_cmd_get_retrive_v2(int unit,phy_ctrl_t *pc, uint16 cmd, uint16 arg[], int size);
STATIC int _phy_8481_reinit(int unit, soc_port_t port);
#endif

#if 1

#define PHY84834_TOP_LEVEL_CMD_SET  _phy84834_top_level_cmd_set_v2
#define PHY84834_TOP_LEVEL_CMD_GET  _phy84834_top_level_cmd_get_v2
#define PHY84834_DIAG_CMD_SET_EEE_MODE_GENERIC PHY84834_DIAG_CMD_SET_EEE_MODE_V2
#define PHY84834_DIAG_CMD_GET_EEE_MODE_GENERIC PHY84834_DIAG_CMD_GET_EEE_MODE_V2
#define PHY84834_DIAG_CMD_SET_PAIR_SWAP_GENERIC PHY84834_DIAG_CMD_SET_PAIR_SWAP_V2
#define PHY84834_DIAG_CMD_SET_1588_ENABLE_GENERIC PHY84834_DIAG_CMD_SET_1588_ENABLE_V2
#define PHY84834_DIAG_CMD_GET_1588_ENABLE_GENERIC PHY84834_DIAG_CMD_GET_1588_ENABLE_V2
#define PHY84834_DIAG_CMD_SET_XFI_M_REG_VALUES_GENERIC PHY84834_DIAG_CMD_SET_XFI_M_REG_VALUES_V2
#define PHY84834_DIAG_CMD_GET_XFI_M_REG_VALUES_GENERIC PHY84834_DIAG_CMD_GET_XFI_M_REG_VALUES_V2
#define PHY84834_DIAG_CMD_SET_XFI_TX_FILTERS_GENERIC PHY84834_DIAG_CMD_SET_XFI_TX_FILTERS
#define PHY84834_DIAG_CMD_GET_XFI_TX_FILTERS_GENERIC PHY84834_DIAG_CMD_GET_XFI_TX_FILTERS
#define PHY84834_DIAG_CMD_SET_SHORT_REACH_ENABLE_GENERIC PHY84834_DIAG_CMD_SET_SHORT_REACH_ENABLE_V2
#define PHY84834_DIAG_CMD_GET_SHORT_REACH_ENABLE_GENERIC PHY84834_DIAG_CMD_GET_SHORT_REACH_ENABLE_V2
#define PHY84834_DIAG_CMD_GET_SUB_LF_RF_STATUS_GENERIC PHY84834_DIAG_CMD_GET_SUB_LF_RF_STATUS_V2
#define PHY84834_DIAG_CMD_CLEAR_SUB_LF_RF_GENERIC PHY84834_DIAG_CMD_CLEAR_SUB_LF_RF_V2
#define PHY84834_DIAG_CMD_SET_SUB_LF_RF_GENERIC PHY84834_DIAG_CMD_SET_SUB_LF_RF_V2
#define PHY84834_DIAG_CMD_GET_SNR_GENERIC PHY84834_DIAG_CMD_GET_SNR_V2
#define PHY84834_DIAG_CMD_GET_CURRENT_TEMP_GENERIC PHY84834_DIAG_CMD_GET_CURRENT_TEMP_V2
#define PHY84834_DIAG_CMD_SET_XFI_L_REG_VALUES_GENERIC PHY84834_DIAG_CMD_SET_XFI_L_REG_VALUES_V2
#define PHY84834_DIAG_CMD_GET_XFI_L_REG_VALUES_GENERIC PHY84834_DIAG_CMD_GET_XFI_L_REG_VALUES_V2
#define PHY8485X_DIAG_CMD_WRITE_SHADOW_REG PHY8485X_DIAG_CMD_WRITE_SHADOW_REG_V2
#define PHY8485X_DIAG_CMD_READ_SHADOW_REG PHY8485X_DIAG_CMD_READ_SHADOW_REG_V2
#define PHY8486x_DIAG_CMD_GET_XFI_2P5G_MODE  PHY84834_DIAG_CMD_GET_XFI_2P5G_MODE_V2
#define PHY8486x_DIAG_CMD_SET_XFI_2P5G_MODE  PHY84834_DIAG_CMD_SET_XFI_2P5G_MODE_V2
#define PHY848x_DIAG_CMD_SET_JUMBO_FRAME_MODE  PHY848X_DIAG_CMD_WRITE_JUMBO_FRAME_REG_V2
#define PHY848x_DIAG_CMD_GET_JUMBO_FRAME_MODE  PHY848X_DIAG_CMD_READ_JUMBO_FRAME_REG_V2
#define PHY84888_DIAG_CMD_SET_USXGMII_MODE  PHY84834_DIAG_CMD_SET_USXGMII
#define PHY84888_DIAG_CMD_GET_USXGMII_MODE  PHY84834_DIAG_CMD_GET_USXGMII
#define PHY84888_DIAG_CMD_SET_PAUSE_FRAME_MODE  PHY84834_DIAG_CMD_SET_PAUSE_FRAME_MODE
#define PHY84888_DIAG_CMD_GET_PAUSE_FRAME_MODE  PHY84834_DIAG_CMD_GET_PAUSE_FRAME_MODE
#define PHY84888_DIAG_CMD_SET_LED_TYPE  PHY84834_DIAG_CMD_SET_LED_TYPE
#define PHY84888_DIAG_CMD_GET_LED_TYPE  PHY84834_DIAG_CMD_GET_LED_TYPE
#define PHY84888_DIAG_CMD_SET_MGBASET_802_3BZ_TYPE  PHY84834_DIAG_CMD_SET_MGBASET_802_3BZ_TYPE
#define PHY84888_DIAG_CMD_GET_MGBASET_802_3BZ_TYPE  PHY84834_DIAG_CMD_GET_MGBASET_802_3BZ_TYPE
#define PHY848XX_DIAG_CMD_SET_FIBER_SPEED           PHY84834_DIAG_CMD_SET_FIBER_SPEED
#define PHY848XX_DIAG_CMD_GET_FIBER_SPEED           PHY84834_DIAG_CMD_GET_FIBER_SPEED

#define PHY848XX_DIAG_CMD_FIBER_SPEED_10G           3
#define PHY848XX_DIAG_CMD_FIBER_SPEED_1G            2

#else

#define PHY84834_TOP_LEVEL_CMD_SET  _phy84834_top_level_cmd_set
#define PHY84834_TOP_LEVEL_CMD_GET  _phy84834_top_level_cmd_get
#define PHY84834_DIAG_CMD_SET_EEE_MODE_GENERIC PHY84834_DIAG_CMD_SET_EEE_MODE
#define PHY84834_DIAG_CMD_GET_EEE_MODE_GENERIC PHY84834_DIAG_CMD_GET_EEE_MODE
#define PHY84834_DIAG_CMD_SET_PAIR_SWAP_GENERIC PHY84834_DIAG_CMD_PAIR_SWAP_CHANGE
#define PHY84834_DIAG_CMD_SET_1588_ENABLE_GENERIC PHY84834_DIAG_CMD_SET_1588_ENABLE
#define PHY84834_DIAG_CMD_GET_1588_ENABLE_GENERIC PHY84834_DIAG_CMD_GET_1588_ENABLE

#endif

static char *dev_name_8481  = "BCM8481";
static char *dev_name_84823 = "BCM84823";
static char *dev_name_84834 = "BCM84834";
static char *dev_name_84844 = "BCM84844";
static char *dev_name_84858 = "BCM84858";
static char *dev_name_8486x = "BCM8486x";
static char *dev_name_84888 = "BCM84888";

/* Function:
 *    phy_8481_init
 * Purpose:    
 *    Initialize 8481 phys
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 * Returns:    
 *    SOC_E_NONE
 */
STATIC int
phy_8481_init(int unit, soc_port_t port)
{
    soc_timeout_t  to;
    phy_ctrl_t     *pc;
    uint16         fw_ver, data16 = 0;
    soc_port_ability_t  ability;
    int            rv, is_line = 0;
    int            medium;
    uint32         phy_mdi_pair_map;
    uint32         phy_long_xfi, sys_side = 0, priority = 0;
    uint8          *fw = NULL;
    int            fw_len = 0;
    uint16         cl72_value=0xff;   
    uint16         pair_swap_value;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

        PHY_FLAGS_SET(unit, port, PHY_FLAGS_C45);
        FLAGS(pc) = 0; /* device specific private flags */
        LINK_DOWN(pc) = TRUE;

        PHY_EXT_ROM_BOOT(pc) = soc_property_port_get(unit, port,
                                                spn_PHY_EXT_ROM_BOOT, 1);
        DL_DIVIDEND(pc) = soc_property_port_get(unit, port,
                                                spn_RATE_EXT_DOWNLOAD_MDIO_DIVIDEND, 1);
        DL_DIVISOR(pc) = soc_property_port_get(unit, port,
                                                spn_RATE_EXT_DOWNLOAD_MDIO_DIVISOR, 0);

        SYS_FORCED_CL72(pc) = soc_property_port_get(unit, port, spn_PORT_INIT_CL72, 0);

        sys_side = soc_property_port_get(unit, port, spn_PHY_SERDES, 0);
        /* Set Repeater flag according to config for Orca/Mako              *\
        \* (PHY_SERDES == 2500 or 5000) means system side is 2500X or 5000X */
        if ( PHY_IS_BCM8486X_AND_UP(pc) &&
                     (sys_side != 2500) && (sys_side != 5000) ) {
            /* As setting 5G/2.5G line side speed ( @ 5G/2.5G over XFI mode), *\
            |* phy_8481_link_up() will notify internal SerDes with 10G speed. *|
            \* set REPEATER flag to tell MAC driver don't notify speed again! */
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_REPEATER);
        } else {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_REPEATER);
        }

#ifdef BCM_WARM_BOOT_SUPPORT
        if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
            rv = _phy_8481_reinit(unit, port);
            if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) && PHY_IS_BCM8483X_AND_UP(pc)) {
                SOC_IF_ERROR_RETURN
                    (_phy84834_top_level_cmd_get_dispatch_v2(unit, pc, (uint16)PHY84834_DIAG_CMD_GET_EEE_MODE_V2));
                PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS2);
            } else {
                PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_DEFAULT);
            }
            return rv;
        }
#endif

#if 0 
        /* reset the GPHY core */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_MII_CTRLr(unit, pc,
                                          MII_CTRL_RESET, MII_CTRL_RESET));
        /* Wait for device to come out of reset */
        soc_timeout_init(&to, 2000000, 0);
        do {
            rv = READ_PHY8481_MII_CTRLr(unit, pc, &data16);
            if (((data16 & MII_CTRL_RESET) == 0) ||
                SOC_FAILURE(rv)) {
                break;
            }
        } while (!soc_timeout_check(&to));
        if ((data16 & MII_CTRL_RESET) != 0) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY8481 GPHY core reset failed: u=%d p=%d\n"),
                      unit, port));
        }
#endif

        SOC_IF_ERROR_RETURN
                (READ_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc, &data16));
        /* Compare config property with Config13 Strap pin (30.0x401a[13]) */
        if ( PHY_EXT_ROM_BOOT(pc) != ((data16 & (1U << 13)) >> 13) ) {
            LOG_WARN( BSL_LS_SOC_PHY, (BSL_META_U(unit,
                     "Cannot load firmware from MDIO/SPIROM: "
                     "Config13 strap mismatch with the config property "
                     "phy_ext_rom_boot (unit=%d port=%d\n"),  unit, port) );
        }

        if ( soc_property_port_get(unit, port, spn_PHY_FORCE_FIRMWARE_LOAD, TRUE) ) {
            fw_ver = 0;
        } else {
            /* Read the f/w version register,                         *\
            \* if != 0, that means f/w is running and no need to load */
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_TOPLVL1_REG(unit, pc, 0x400f, &fw_ver));
            
            /* Disable loopback */
            /* PCS Control 1 Reg. DEVAD=3 Addr=0, bit[14] PCS loopback mode  */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PCS_CTRLr(unit, pc,
                                0, PCS_IEEE_CTL1_LPBK));
            /* 1000BASE-T/100BASE-TX MII Contol Reg. DEVAD=7, Addr=0xFFE0 bit[14] */    
            SOC_IF_ERROR_RETURN               
                    (MODIFY_PHY8481_MII_CTRLr(unit, pc, 0, MII_CTRL_LE));

        }

        if ( (! fw_ver) && (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1)
                        && (PHY_EXT_ROM_BOOT(pc)) ) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY8481: SPI-ROM Attempting firmware download : u=%d p=%d\n"),
                      unit, port));
            /* Reset the device PMA/PMD and PCS */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PMAD_CTRLr(unit, pc, MII_CTRL_RESET,
                                                     MII_CTRL_RESET) );
            /* Wait for device to come out of reset */
            soc_timeout_init(&to, 2000000, 0);
            if ( PHY_IS_BCM8484X_A0(pc) ) {
                sal_usleep(300000);
            }

            do {
                rv = READ_PHY8481_PMAD_CTRLr(unit, pc, &data16);
                if ( ((data16 & MII_CTRL_RESET) == 0) || SOC_FAILURE(rv) ) {
                    break;
                }
            } while ( ! soc_timeout_check(&to) );
            if ( ( data16 & MII_CTRL_RESET) != 0 ) {
                LOG_WARN(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit, "PHY8481 reset failed: u=%d p=%d\n"),
                          unit, port));
                /* failed to load FW from SPIROM, try MDIO FW load */
                PHY_EXT_ROM_BOOT(pc) = 0;
            }
        }
    
        if ( (! fw_ver) && (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1)
                        && (! PHY_EXT_ROM_BOOT(pc))) {
            /* Attempting f/w download */
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY8481: Attempting firmware download : u=%d p=%d\n"),
                      unit, port));
            switch ( PHY_OUI_MODEL(  PHY_OUI(pc->phy_id0, pc->phy_id1),
                                   PHY_MODEL(pc->phy_id0, pc->phy_id1)) ) {
    
            case  PHY_OUI_MODEL(PHY_BCM8481X_OUI, PHY_BCM8481X_MODEL) :
                if ((PHY_REV(pc->phy_id0, pc->phy_id1) >= 0x2)) {
                    /* C0 and beyond only */
                    FIRMWARE(pc) = bcm_8481_firmware;
                    FIRMWARE_LEN(pc) = bcm_8481_firmware_size;
                    pc->dev_name = dev_name_8481;
                }
            break;
    
            case  PHY_OUI_MODEL(PHY_BCM84822_OUI, PHY_BCM84822_MODEL) :
            case  PHY_OUI_MODEL(PHY_BCM84823_OUI, PHY_BCM84823_MODEL) :
                if (PHY_REV(pc->phy_id0, pc->phy_id1) == 0x0)  {
                    FIRMWARE(pc) = bcm_8482A0_firmware;
                    FIRMWARE_LEN(pc) = bcm_8482A0_firmware_size;
                }
                if (PHY_REV(pc->phy_id0, pc->phy_id1) == 0x1)  {
                    FIRMWARE(pc) = bcm_8482B0_firmware;
                    FIRMWARE_LEN(pc) = bcm_8482B0_firmware_size;
                }
                if (PHY_REV(pc->phy_id0, pc->phy_id1) == 0x2)  {
                    FIRMWARE(pc) = bcm_8482B1_firmware;
                    FIRMWARE_LEN(pc) = bcm_8482B1_firmware_size;
                }
                pc->flags |= PHYCTRL_MDIO_BCST;
                pc->dev_name = dev_name_84823;
            break;
    
            case  PHY_OUI_MODEL(PHY_BCM84833_OUI, PHY_BCM84833_MODEL) :
            case  PHY_OUI_MODEL(PHY_BCM84834_OUI, PHY_BCM84834_MODEL) :
            case  PHY_OUI_MODEL(PHY_BCM84835_OUI, PHY_BCM84835_MODEL) :
            case  PHY_OUI_MODEL(PHY_BCM84836_OUI, PHY_BCM84836_MODEL) :
                FIRMWARE(pc) = bcm_84833_firmware;
                FIRMWARE_LEN(pc) = bcm_84833_firmware_size;
                pc->flags |= PHYCTRL_MDIO_BCST;
                pc->dev_name = dev_name_84834;
            break;

            case  PHY_OUI_MODEL(PHY_BCM84844_OUI, PHY_BCM84844_MODEL) :
            case  PHY_OUI_MODEL(PHY_BCM84846_OUI, PHY_BCM84846_MODEL) :
            case  PHY_OUI_MODEL(PHY_BCM84848_OUI, PHY_BCM84848_MODEL) :
                FIRMWARE(pc) = bcm_84844_firmware;
                FIRMWARE_LEN(pc) = bcm_84844_firmware_size;
                pc->flags |= PHYCTRL_MDIO_BCST;
                pc->dev_name = dev_name_84844;
            break;
    
            case  PHY_OUI_MODEL(PHY_BCM84858_OUI, PHY_BCM84858_MODEL) :
            case  PHY_OUI_MODEL(PHY_BCM84856_OUI, PHY_BCM84856_MODEL) :
                FIRMWARE(pc) = bcm_84858_firmware;
                FIRMWARE_LEN(pc) = bcm_84858_firmware_size;
                pc->flags |= PHYCTRL_MDIO_BCST;
                pc->dev_name = dev_name_84858;
            break;
    
            case  PHY_OUI_MODEL(PHY_BCM84860_OUI, PHY_BCM84860_MODEL) |
                  PHY_OUI_MODEL(PHY_BCM84861_OUI, PHY_BCM84861_MODEL) :
            case  PHY_OUI_MODEL(PHY_BCM84864_OUI, PHY_BCM84864_MODEL) |
                  PHY_OUI_MODEL(PHY_BCM84868_OUI, PHY_BCM84868_MODEL) :
                FIRMWARE(pc) = bcm_8486x_firmware;
                FIRMWARE_LEN(pc) = bcm_8486x_firmware_size;
                pc->flags |= PHYCTRL_MDIO_BCST;
                pc->dev_name = dev_name_8486x;
            break;

            case  PHY_OUI_MODEL(PHY_BCM84888_OUI, PHY_BCM84888_MODEL) |
                  PHY_OUI_MODEL(PHY_BCM84887_OUI, PHY_BCM84887_MODEL) |
                  PHY_OUI_MODEL(PHY_BCM84884_OUI, PHY_BCM84884_MODEL) :
            case  PHY_OUI_MODEL(PHY_BCM84888E_OUI, PHY_BCM84888E_MODEL) |
                  PHY_OUI_MODEL(PHY_BCM84884E_OUI, PHY_BCM84884E_MODEL) :
            case  PHY_OUI_MODEL(PHY_BCM84881_OUI, PHY_BCM84881_MODEL) |
                  PHY_OUI_MODEL(PHY_BCM84880_OUI, PHY_BCM84880_MODEL) :
            case  PHY_OUI_MODEL(PHY_BCM84885_OUI, PHY_BCM84885_MODEL) |
                  PHY_OUI_MODEL(PHY_BCM84888S_OUI, PHY_BCM84888S_MODEL) | 
                  PHY_OUI_MODEL(PHY_BCM84886_OUI, PHY_BCM84886_MODEL) :
                if ( (PHY_REV(pc->phy_id0, pc->phy_id1) == 0x0) || (PHY_REV(pc->phy_id0, pc->phy_id1) == 0x4) || (PHY_REV(pc->phy_id0, pc->phy_id1) == 0x8) )  {
                    FIRMWARE(pc) = bcm_84888A0_firmware;
                    FIRMWARE_LEN(pc) = bcm_84888A0_firmware_size;
                }
                if ( (PHY_REV(pc->phy_id0, pc->phy_id1) == 0x1) || (PHY_REV(pc->phy_id0, pc->phy_id1) == 0x5) || (PHY_REV(pc->phy_id0, pc->phy_id1) == 0x9) )  {
                    FIRMWARE(pc) = bcm_84888B0_firmware;
                    FIRMWARE_LEN(pc) = bcm_84888B0_firmware_size;
                } 
                pc->flags |= PHYCTRL_MDIO_BCST;
                pc->dev_name = dev_name_84888;
            break;

            default:
                FIRMWARE(pc) = NULL;
                FIRMWARE_LEN(pc) = 0;
            break;
            }

            /* Now check whether a newer version of firmware is available for this model */ 
            rv = soc_phy_fw_get(pc->dev_name, &fw, &fw_len);  
            if (SOC_SUCCESS(rv)) {
                FIRMWARE(pc) = fw;
                FIRMWARE_LEN(pc) = fw_len;
            }

            if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_CHAINED)) {
            /*  No broadcast mode download in chained PHY. */
                pc->flags &= ~PHYCTRL_MDIO_BCST;
            }
   
            if ( FIRMWARE(pc) ) {
                if (pc->flags & PHYCTRL_MDIO_BCST) {
                    /* indicate second pass of init is needed */
                    PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS2);
                    return SOC_E_NONE;
                }

                SOC_IF_ERROR_RETURN
                    (_phy_8481_halt(unit, pc));
                SOC_IF_ERROR_RETURN
                    (_phy_8481_write_to_arm(unit, pc, 0, FIRMWARE(pc), FIRMWARE_LEN(pc)));
    
                /* write_addr=0xc300_0000, data=0x0000_002c, enable bottom write protection 
                   and set VINITHI signal to 0 (reset vector at 0x0000_0000) */

                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 0x0000));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, 0x0000));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, 0xc300));
                if (PHY_IS_BCM8485X_AND_UP(pc)) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, 0x0000));
                } else {
                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, 0x002c));
                }
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, 0x0000));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 
                        (2U << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT)|(1U << MDIO2ARM_CTL_MDIO2ARM_WR_SHIFT)));

                /* Reset the processor to start execution of the code in the on-chip memory */
                if ( PHY_IS_BCM8486X_AND_UP(pc) ) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x8004,0x5555));
                    if ( PHY_IS_BCM8488X(pc) ) {
                        /* Set 1e.0x0009[0]  */
                        SOC_IF_ERROR_RETURN
                            (MODIFY_PHY8481_TOPLVL1_REG(unit, pc, 0x0009, 1, PHY8488X_SOFT_RESET_MASK));
                        sal_usleep(200);
                    }
                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY8481_PMAD_REG(unit, pc, 0x0000 , 0x8000));
                } else
                if ( PHY_IS_BCM8485X_AND_UP(pc) ) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4186,0x0004));
                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4186,0x0000));
                }

                /* check the strap pin Config13 */
                SOC_IF_ERROR_RETURN
                    (READ_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc, &data16));
                if (data16 & (1U << 13)) {
                    /* Now reset only the ARM core */
                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0040));
                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0000));
                } else {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY8481_PMAD_CTRLr(unit, pc,
                                              MII_CTRL_RESET, MII_CTRL_RESET));
                }

                if (PHY_IS_BCM8484X_A0(pc)) {
                    sal_usleep(300000);
                }
                soc_timeout_init(&TO(pc), 2000000, 0);
    
            } else {
                LOG_WARN(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "PHY8481: firmware download not possible with this model: u=%d p=%d\n"),
                          unit, port));
            }
        }

        /* Need to disable port here for 8484X firmware workaround */
        /* Workaround (SDK-45594 Part 1 of 2) */
        if (PHY_IS_BCM8484X(pc)) {
            SOC_IF_ERROR_RETURN(
                    MODIFY_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc,
                                                        TOP_CONFIG_XGPHY_STRAP1_XGPH_DISABLE,
                                                        TOP_CONFIG_XGPHY_STRAP1_XGPH_DISABLE) );
        }

        if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) {
            /* indicate second pass of init is needed */
            PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS2);
            return SOC_E_NONE;
        }
    }
    
    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

#ifdef BCM_WARM_BOOT_SUPPORT
        uint16 args[5];
        if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
            if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) && PHY_IS_BCM8483X_AND_UP(pc)) {
                SOC_IF_ERROR_RETURN
                    (_phy84834_top_level_cmd_get_retrive_v2(unit, pc, (uint16)PHY84834_DIAG_CMD_GET_EEE_MODE_V2, args, 4));
                if (!args[0]) {
                    PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_EEE_ENABLED);
                    PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_EEE_MODE);
                    LOG_INFO(BSL_LS_SOC_PHY,
                             (BSL_META_U(unit,
                                         "u=%d p=%d recovered EEE None\n"), unit, port));
                } else if (args[0] == 1) {
                    PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_EEE_ENABLED);
                    PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_EEE_MODE);
                    LOG_INFO(BSL_LS_SOC_PHY,
                             (BSL_META_U(unit,
                                         "u=%d p=%d recovered EEE Native\n"), unit, port));
                } else if (args[0] == 2) {
                    PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_EEE_ENABLED);
                    PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_EEE_MODE);
                    LOG_INFO(BSL_LS_SOC_PHY,
                             (BSL_META_U(unit,
                                         "u=%d p=%d recovered EEE AutogrEEEn\n"), unit, port));
                }
            }
            PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_DEFAULT);
        
            PHY8481_SET_FIBER_PREFERRED(unit, port, pc);    
            PHY8481_ENABLE_MEDIUM(pc);
            return SOC_E_NONE;
        }
#endif

        do {
            rv = READ_PHY8481_TOPLVL1_REG(unit, pc, 0x400f, &fw_ver);
            if ( SOC_FAILURE(rv) ) {
                data16 = MII_CTRL_RESET;
                break;
            }
            rv = READ_PHY8481_PMAD_CTRLr(unit, pc, &data16);
            if ( SOC_FAILURE(rv) ) {
                break;
            }
            if ( fw_ver && (!(data16 & MII_CTRL_RESET)) ) {
                /* version is set and the reset bit has cleared */
                break;
            }
        } while ( !soc_timeout_check(&TO(pc)) );
        if ( (data16 & MII_CTRL_RESET) || (! fw_ver) ) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY8481 : u=%d p=%d : Firmware might not be running\n"),
                      unit, port));
        }

        SOC_IF_ERROR_RETURN
            (_phy_8481_check_firmware(unit, pc));

        PHY8481_SET_FIBER_PREFERRED(unit, port, pc);
        /* software controlled medium selection is not available yet */
        PHY8481_ENABLE_MEDIUM(pc);
        pc->power_mode = SOC_PHY_CONTROL_POWER_FULL;

        /* Need to disable port here for 8484X firmware workaround */
        /* Workaround (SDK-45594 Part 2 of 2) */
        if (PHY_IS_BCM8484X(pc)) {
            SOC_IF_ERROR_RETURN(
                phy_8481_enable_set(unit,port, FALSE));
        }

        soc_timeout_init(&TO(pc), 700000, 0);

        if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) {
            /* indicate second pass of init is needed */
            PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS3);
            return SOC_E_NONE;
        }
    }
    
    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS3) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

        PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS4);

        while (!soc_timeout_check(&TO(pc))) {
            if (PHY_HS_CAPABLE(unit, port)) {
                rv = READ_PHY8481_TOPLVL1_REG(unit, pc, 0x400e, &data16);
                if (((data16 & 0x2) == 0) || SOC_FAILURE(rv)) {
                    break;
                }
            } else {
                sal_usleep(100);
            }
        }

        if ( PHY_IS_BCM8486X_AND_UP(pc) ) {    /* Mako and Orca supports 5G/2.5G speed */

            if ( PHY_IS_BCM8488X(pc) ) {
                /* Default priority IEEE 802.3bz 5GBase-T & 2.5GBase-T  */
                priority = soc_property_port_get(unit, port,
                                     spn_PHY_MGBASET_802P3BZ_PRIORITY, 1);
                _phy_848xx_mgbase_802_3bz_type_set(unit, pc,
                                                   (priority) ? 1 : 0, (priority) ? 1 : 0);                 
            }
            sys_side = soc_property_port_get(unit, port, spn_PHY_SERDES, 0);
            if ( (sys_side == 2500 && PHY_IS_BCM8486X(pc)) ||
                   (PHY_IS_BCM8488X(pc) && (sys_side == 2500 || sys_side == 5000)) ) {
                /* MAC interface defaults to 2500BASE-X mode    */
                data16 = PHY84834_DIAG_CMD_XFI_2P5G_MODE_2500X;
            } else {
                /* MAC interface is 5G/2.5G over XFI mode       */
                data16 = PHY84834_DIAG_CMD_XFI_2P5G_MODE_XFI;
            }
            /* command handler code SET_XFI_2P5G_MODE (0x8017) */
            SOC_IF_ERROR_RETURN(
                    _phy848xx_xfi_2p5g_5g_mode_set(unit, pc, data16) );
        }

        if (!PHY_FORCED_COPPER_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc,
                                pc->fiber.preferred ? 1U<<8 : 0, 1U<<8));
        }
    
        phy_mdi_pair_map = soc_property_port_get(unit, port,
                                                 spn_PHY_MDI_PAIR_MAP, 0);
        if ( phy_mdi_pair_map ) {
            /* convert  16'b_00wW_00xX_00yY_00zZ --> 16'b_0000_0000_wWxX_yYzZ */
            pair_swap_value = ((phy_mdi_pair_map & (0x3<<12)) >> 6) |
                              ((phy_mdi_pair_map & (0x3<<8 )) >> 4) | 
                              ((phy_mdi_pair_map & (0x3<<4 )) >> 2) |
                              ((phy_mdi_pair_map & (0x3<<0 )) >> 0) ;

            if ( PHY_IS_BCM8484X_AND_UP(pc) ) {
                SOC_IF_ERROR_RETURN
                    (PHY84834_TOP_LEVEL_CMD_SET(unit, pc,
                            (uint16) PHY84834_DIAG_CMD_SET_PAIR_SWAP_GENERIC,
                            &pair_swap_value, 1) );
            } else {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_TOPLVL1_REG(unit, pc, 0x4005, 0, (1U << 1)));
                sal_udelay(35000);

                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_TOPLVL1_REG(unit, pc, 0x4009, pair_swap_value, 0xff));
               
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_TOPLVL1_REG(unit, pc, 0x4005,
                                (1U << 6)|(1U << 4)|(1U << 1),
                                (1U << 6)|(1U << 4)|(1U << 1)));
                sal_udelay(35000);
            }
        }

        /* By default XFI cannot drive long distance cables. TX premphasis needs
         * to be adjusted if long cables are used on the XFI side
         */
        phy_long_xfi = soc_property_port_get(unit, port, 
                                             spn_PHY_LONG_XFI, 0);
        if (phy_long_xfi) {
            if (!(pc->flags & PHYCTRL_SYS_SIDE_CTRL)) {
                is_line = 1;
                pc->flags |= PHYCTRL_SYS_SIDE_CTRL;
            }

            SOC_IF_ERROR_RETURN(
              _phy8481_adjust_xfi_tx_preemphasis(unit, port, pc, phy_long_xfi));                

            if (is_line) {
                pc->flags &= ~PHYCTRL_SYS_SIDE_CTRL;
                is_line = 0;
            }
        }

        if (!PHY_FORCED_COPPER_MODE(unit, port)) {
            /* check if there is another PHY device connected to the XAUI port */
            rv = _phy_8481_xaui_nxt_dev_probe(unit,port);
            if (SOC_SUCCESS(rv)) {
                PHYDRV_CALL_NOARG(pc,PHY_INIT);
            }
        } else {
            pc->fiber.preferred = FALSE;
            pc->copper.preferred = TRUE;
        }

        /* Copper - Set Local Advertising Configuration */
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);

        if (!PHY_FORCED_COPPER_MODE(unit, port)) {
            PHY8481_SELECT_REGS_CU(unit, pc);
        }
        SOC_IF_ERROR_RETURN
            (phy_8481_ability_local_get(unit, port, &ability));
        SOC_IF_ERROR_RETURN
            (phy_8481_ability_advert_set(unit, port, &ability));
    
        /* Fiber - Set Local Advertising Configuration */
        if (!PHY_FORCED_COPPER_MODE(unit, port)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
            PHY8481_SELECT_REGS_XAUI(unit, pc);
            SOC_IF_ERROR_RETURN
                (phy_8481_ability_local_get(unit, port, &ability));
            SOC_IF_ERROR_RETURN
                (phy_8481_ability_advert_set(unit, port, &ability));
        }
    
    
        /* The following should be done only after the chained PHY is probed as
           the PHY flags should be determined by the base PHY.
         */
        SOC_IF_ERROR_RETURN
            (_phy_8481_medium_check(unit, port, &medium));

    
        if (medium == SOC_PORT_MEDIUM_COPPER) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
            PHY8481_SELECT_REGS_CU(unit, pc);
        } else {        /* Fiber */
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
            PHY8481_SELECT_REGS_XAUI(unit, pc);
        }

        /*
         * Configure Auxiliary 1000BASE-X control register to turn off
         * carrier extension.  The Intel 7131 NIC does not accept carrier
         * extension and gets CRC errors.
         */
        /* Disable carrier extension */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_AUX_1000X_CTRLr(unit, pc, 0x0040, 0x0040));

        if (PHY_IS_BCM8485X_AND_UP(pc) || PHY_IS_BCM8484X(pc)) {
            /* Enable Jumbo Packet */
            SOC_IF_ERROR_RETURN(
                    _phy84834_jumbo_packet_set(unit, pc, 1));
        } else {
           /* Enable 18K jumbo frame support in 8482x and 8483x */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_MII_AUX_CTRLr(unit, pc, (1U << 14), (1U << 14)));
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PHY_ECRr(unit, pc, (1U << 0), (1U << 0)));
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_EXP_PATTERN_GENr(unit, pc, (1U << 14), (1U << 14)));
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_MISC_1000X_CTRL2r(unit, pc, (1U << 0), (1U << 0)));
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_AUX_1000X_CTRLr(unit, pc, (1U << 1), (1U << 1))); 
        }

        if (PHY_IS_BCM8484X(pc) && IS_HG_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(
                WRITE_PHY8481_PMAD_REG(unit, pc, 0xa939, 0x0183));
        }

        PHY_FW_HANDSHAKE(unit, port);

        /* config property PHY_OPERATIONAL_MODE == 0x80 or 0x8000 or 0x8080 *\
        \*        means to keep Super Isolate enabled                       */
        if ( (soc_property_port_get(unit, port, spn_PHY_OPERATIONAL_MODE, 0)
              & TOP_CONFIG_XGPHY_DISABLE) == 0 ) {
            /* Remove  Super Isolate  and  XGPH Disable  by default */
            SOC_IF_ERROR_RETURN(
                MODIFY_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc,   0x00,
                                                    TOP_CONFIG_XGPHY_DISABLE) );
        }
        PHY_FW_HANDSHAKE(unit, port);

        if ( PHY_IS_BCM8485X_AND_UP(pc) ) {
            if (SYS_FORCED_CL72(pc)) {
                _phy_8485x_force_cl72_config(unit, pc, 1);
                (_phy_8485x_force_cl72_status(unit, pc, (uint16 *)&cl72_value));
            } else {
                (_phy_8485x_force_cl72_status(unit, pc, (uint16 *)&cl72_value));
            }
        }
    }

    return SOC_E_NONE;
}


#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int
_phy_8481_reinit(int unit,soc_port_t port)  {
    phy_ctrl_t     *pc;
    int fiber_preferred, medium; 
    uint16 data16;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_8481_check_firmware(unit, pc));

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc, &data16));
    fiber_preferred = data16 & (1U << 8) ? 1 : 0;

    pc->copper.preferred = !fiber_preferred;
    pc->fiber.preferred = fiber_preferred;

    pc->automedium = TRUE; /* Always TRUE for this PHY */

    SOC_IF_ERROR_RETURN
        (_phy_8481_medium_check(unit, port, &medium));

    if (medium == SOC_PORT_MEDIUM_COPPER) {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
        SOC_IF_ERROR_RETURN
            (_phy_8481_copper_speed_get(unit, port, &pc->copper.force_speed));
        SOC_IF_ERROR_RETURN
            (_phy_8481_copper_duplex_get(unit, port, &pc->copper.force_duplex));
        SOC_IF_ERROR_RETURN
            (_phy_8481_copper_ability_advert_get(unit, port, &pc->copper.advert_ability));
        SOC_IF_ERROR_RETURN
            (_phy_8481_copper_an_get(unit, port, &pc->copper.autoneg_enable, NULL));
    } else {        /* Fiber */
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);

        SOC_IF_ERROR_RETURN
            (_phy_8481_xaui_speed_get(unit, port, &pc->fiber.force_speed));
        SOC_IF_ERROR_RETURN
            (_phy_8481_xaui_duplex_get(unit, port, &pc->fiber.force_duplex));
        SOC_IF_ERROR_RETURN
            (_phy_8481_xaui_ability_advert_get(unit, port, &pc->fiber.advert_ability));
        SOC_IF_ERROR_RETURN
            (_phy_8481_xaui_an_get(unit, port, &pc->fiber.autoneg_enable, NULL));
    }

    pc->power_mode = SOC_PHY_CONTROL_POWER_FULL;

    return SOC_E_NONE;
}
#endif

STATIC int
_phy_8481_clear_eee_stats(int unit,phy_ctrl_t *pc) {
    uint16 temp;

   /* Clearing 10G EEE stats: Enable clear on Read  */
    SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_EEE_STATISTICS_CTLr(unit, pc, 0x0003, 0x000f));
    SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_EEE_STATISTICS_CTLr(unit, pc, 0x000f, 0x000f));
    SOC_IF_ERROR_RETURN
            (READ_PHY8481_EXP_EEE_TX_EVENTS_HIGHr(unit, pc, &temp));
    SOC_IF_ERROR_RETURN
            (READ_PHY8481_EXP_EEE_TX_EVENTS_LOWr(unit, pc, &temp));
    /* Clearing 10G EEE stats: Disable clear on Read  */
    SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_EEE_STATISTICS_CTLr(unit, pc, 0x0002, 0x000f));

    /* Clearning 1G EEE stats  */
    SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_1G_EEE_STATISTICS_CTLr(unit, pc, 0x8002, 0x8002));
    SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_1G_EEE_STATISTICS_CTLr(unit, pc, 0x8000, 0x8002));

    return SOC_E_NONE;
}

STATIC int
_phy_8481_check_firmware(int unit,phy_ctrl_t *pc) {
    uint16 tmp16;
    uint8  fw_ver_chip_rev, fw_ver_main, fw_ver_branch, fw_ver_sub_branch,
           fw_date_month, fw_date_day, fw_date_year, spirom_crc_check_status;
    
    SOC_IF_ERROR_RETURN
                (READ_PHY8481_TOPLVL1_REG(unit, pc, 0x400f, &tmp16));
    fw_ver_chip_rev = (tmp16 >> 12) & 0xf;
    fw_ver_main     = (tmp16 >> 7) & 0x1f;
    fw_ver_branch   =  tmp16 & 0x7f;
    
    if (PHY_IS_BCM8485X_AND_UP(pc)) {
        fw_ver_sub_branch = fw_ver_chip_rev;
    }
    else {    
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_TOPLVL1_REG(unit, pc, 0x4011,&tmp16));
            fw_ver_sub_branch =  (tmp16 >> 11) & 0x1f;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_TOPLVL1_REG(unit, pc, 0x4010,&tmp16));
    
        fw_date_month = (tmp16 >> 9) & 0xf;                      /* BIT[12:9] */
        fw_date_day   = (tmp16 >> 4) & 0x1f;                     /* BIT[9:4]  */
        fw_date_year  = (tmp16 & 0xf) | ((tmp16 & BIT(13)) >> 9); /* [13,3:0] */
    
    LOG_CLI((BSL_META_U(unit,
                        "BCM848xx: u=%d p=%d: init. Chip Rev = %02d "
                        "Version = %02d.%02d.%02d Date = %02d/%02d/20%02d (%s)\n"),
             unit, pc->port, fw_ver_chip_rev, fw_ver_main, fw_ver_branch, fw_ver_sub_branch,
             fw_date_month, fw_date_day, fw_date_year,
             PHY_EXT_ROM_BOOT(pc) ? "SPI" : "MDIO"));
   
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_TOPLVL1_REG(unit, pc, 0x400d, &tmp16));
    spirom_crc_check_status = (tmp16 >> 14) & 0x3;
    if(spirom_crc_check_status == 0x2) {
        LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit, "SPIROM bad CRC \n")));
    }

    if (PHY_IS_BCM84823(pc) && ((fw_ver_main > 3) || ((fw_ver_main == 3) && (fw_ver_branch >= 2)))) {
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_HS_CAPABLE);
    }
    if (PHY_IS_BCM84823(pc) && ((fw_ver_main > 4) || ((fw_ver_main == 4) && (fw_ver_branch >= 18)))) {
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_EEE_CAPABLE);
        SOC_IF_ERROR_RETURN
           (MODIFY_PHY8481_TOP_CONFIG_AUTOGREEN_CONFIG1r(unit, pc, 0x0002, 0x0003));
        /* Clearing the stats: Disable clear on Read  */
        SOC_IF_ERROR_RETURN
           (MODIFY_PHY8481_EEE_STATISTICS_CTLr(unit, pc,0x0000, 0x0001));
    }
    if (PHY_IS_BCM8483X(pc) && ((fw_ver_main > 1) || ((fw_ver_main == 1) && (fw_ver_branch >= 15)))) {
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_HS_CAPABLE);
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_EEE_CAPABLE);
        /* Clearing the stats: Disable clear on Read  */
        SOC_IF_ERROR_RETURN
           (MODIFY_PHY8481_EEE_STATISTICS_CTLr(unit, pc,0x0000, 0x0001));
    }
    if (PHY_IS_BCM8484X(pc) && ((fw_ver_main > 0) || ((fw_ver_main == 0) && (fw_ver_branch >= 3)))) {
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_HS_CAPABLE);
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_EEE_CAPABLE);

        /* Set flag for FW versions >= 1.08.00 */
        if ( ((fw_ver_main == 1) && (fw_ver_branch >= 8)) || (fw_ver_main > 1) ) {
             FLAGS(pc) |= PHY8483X_ATOMIC_ACCESS_SHD;
        }
    }
    if (PHY_IS_BCM8483X(pc) && ((fw_ver_main > 1) || ((fw_ver_main == 1) && (fw_ver_branch >= 40)))) {
        FLAGS(pc) |= PHY8483X_MFG_TEST_SUPPORT;
        FLAGS(pc) |= PHY8483X_STATE_TEST_SUPPORT;
    }
    if (PHY_IS_BCM8484X(pc) && ((fw_ver_main > 0) || ((fw_ver_main == 0) && (fw_ver_branch >= 3)))) {
        FLAGS(pc) |= PHY8483X_MFG_TEST_SUPPORT;
        FLAGS(pc) |= PHY8483X_STATE_TEST_SUPPORT;
        FLAGS(pc) |= PHY8483X_FULL_SNR_SUPPORT;
    }
    if (PHY_IS_BCM8483X(pc) && ((fw_ver_main > 1) || ((fw_ver_main == 1) && (fw_ver_branch >= 52)))) {
        FLAGS(pc) |= PHY8483X_FULL_SNR_SUPPORT;
    }
    if ( (PHY_IS_BCM8485X(pc) && ((fw_ver_main > 0) || ((fw_ver_main == 0) && (fw_ver_branch >= 3 )))) ||
         (PHY_IS_BCM8486X_AND_UP(pc)) ) {
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_HS_CAPABLE);
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_EEE_CAPABLE);
        FLAGS(pc) |= PHY8483X_FULL_SNR_SUPPORT;
        FLAGS(pc) |= PHY8483X_STATE_TEST_SUPPORT;
        FLAGS(pc) |= PHY8483X_MFG_TEST_SUPPORT;

        /* Set flag for FW versions >= 1.00.05 */
        if ( ((fw_ver_main == 1) && (fw_ver_branch >= 1)) ||
              (fw_ver_main >  1) || (PHY_IS_BCM8486X_AND_UP(pc)) ) {
            FLAGS(pc) |= PHY8483X_ATOMIC_ACCESS_SHD;
        }
    }

    return SOC_E_NONE;
}
    


STATIC int
_phy_8481_copper_link_get(int unit, soc_port_t port, int *link) {

    uint16  pcs_mii_stat, pcs_mii_stat2, pcs_mii_stat3;
    uint16  speed_val, ctrl;
    phy_ctrl_t     *pc;
    int an,an_done,cur_speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_CU(unit, pc);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_STATr(unit, pc, &pc->mii_stat));

    SOC_IF_ERROR_RETURN
        (phy_8481_an_get(unit,port,&an,&an_done));

    /* return link false if in the middle of autoneg */
    if (an == TRUE && an_done == FALSE) {
        *link = FALSE;
        return SOC_E_NONE;
    } 

    /* PCS link status , DEVAD=3 */
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PCS_STATr(unit, pc, &pcs_mii_stat));

    if (pcs_mii_stat == 0xffff) {
       /* mii_stat == 0xffff check is to handle removable PHY daughter cards */
        *link = FALSE;
        return SOC_E_NONE;
    } 

   /* check if the current speed is set to 10G */
    cur_speed = 0;
    if (an) { /* autoneg is enabled */
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_TENG_AN_CTRLr(unit, pc, &ctrl));
            if ((ctrl & TENG_IEEE_AN_CTRL_10GBT)&&(pcs_mii_stat & MII_STAT_LA)) {
                cur_speed = 10000;
            }
    } else { /* forced mode */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_CTRLr(unit, pc, &speed_val));
        if ((speed_val & (IEEE_CTRLr_SPEED_10G_MASK|PMAD_MII_CTRL_SS_MASK))==IEEE_CTRLr_SPEED_10G) {
            cur_speed = 10000;
        }
    }

    if (cur_speed == 10000) { /* check all 3 device's link status if 10G */
        if PHY_IS_BCM84823(pc) {
                SOC_IF_ERROR_RETURN
                    (READ_PHY8481_PCS_REG(unit, pc, 0x20, &pcs_mii_stat2));

                SOC_IF_ERROR_RETURN
                    (READ_PHY8481_PCS_REG(unit, pc, 0x21, &pcs_mii_stat3));

                *link = (pcs_mii_stat & MII_STAT_LA) && ((pcs_mii_stat2 & 0x1001) == 0x1001) &&
                        (pcs_mii_stat3 & 0x8000) ? TRUE : FALSE;
        } else {
                *link = pcs_mii_stat & MII_STAT_LA ?  TRUE : FALSE;
        }
    } else {
        *link = (pc->mii_stat & MII_STAT_LA) ? TRUE : FALSE;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_xaui_link_get(int unit, soc_port_t port, int *link) {

    phy_ctrl_t *pc;
    uint16  status;
    int     an_done;
    int     an;
    int     speed;
    int     speed_local;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ((!PHY_COPPER_MODE(unit, port)) && NXT_PC(pc)) {
        /* there is a PHY device connected to this XAUI port */
        /* When it is in AN mode, its negotiated speed needs to be
         * propagated to this device. Sync up here since this function is periodically
         * called by a dedicated linkscan task.
         */
        PHYDRV_CALL_ARG1(pc,PHY_LINK_GET,link);

        if (*link == TRUE) {
            /* check if in an mode */
            PHYDRV_CALL_ARG2(pc,PHY_AUTO_NEGOTIATE_GET,&an,&an_done);

            if (an == TRUE && an_done == TRUE) {
                /* get the speed */
                PHYDRV_CALL_ARG1(pc,PHY_SPEED_GET,&speed);
                SOC_IF_ERROR_RETURN
                    (_phy_8481_xaui_speed_get(unit, port, &speed_local));

                if (speed && (speed != speed_local)) {
                    /* set this device's speed accordingly */
                    SOC_IF_ERROR_RETURN
                        (_phy_8481_xaui_speed_set(unit,port,speed));
                }
            }
        }
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_TOP_CONFIG_LINK_STATUS_REG(unit, pc, &status));

   /* status != 0xffff check is to handle removable PHY daughter cards */
    *link =  ((status != 0xffff) && (status & TOP_CONFIG_LINK_STATUS_REG_FIBER_LINK)) ? TRUE : FALSE;

    return (SOC_E_NONE);
}


static int
_phy8481_auto_negotiate_gcd(int unit, soc_port_t port, int *speed, int *duplex)
{
    int        t_speed, t_duplex;
    uint16     mii_ana, mii_anp;
    uint16     mii_gb_stat, mii_esr, mii_gb_ctrl;
    phy_ctrl_t *pc;

    t_speed = 0;
    t_duplex = 0;
    mii_gb_stat = 0;            /* Start off 0 */
    mii_gb_ctrl = 0;            /* Start off 0 */

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_ANAr(unit, pc, &mii_ana));
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_ANPr(unit, pc, &mii_anp));

    if (pc->mii_stat & MII_STAT_ES) {    /* Supports extended status */
        /*
         * If the PHY supports extended status, check if it is 1000MB
         * capable.  If it is, check the 1000Base status register to see
         * if 1000MB negotiated.
         */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MII_ESRr(unit, pc, &mii_esr));

        if (mii_esr & (MII_ESR_1000_X_FD | MII_ESR_1000_X_HD | 
                       MII_ESR_1000_T_FD | MII_ESR_1000_T_HD)) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_MII_GB_STATr(unit, pc, &mii_gb_stat));
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_MII_GB_CTRLr(unit, pc, &mii_gb_ctrl));
        }
    }

    /*
     * At this point, if we did not see Gig status, one of mii_gb_stat or 
     * mii_gb_ctrl will be 0. This will cause the first 2 cases below to 
     * fail and fall into the default 10/100 cases.
     */

    mii_ana &= mii_anp;

    if ((mii_gb_ctrl & MII_GB_CTRL_ADV_1000FD) &&
        (mii_gb_stat & MII_GB_STAT_LP_1000FD)) {
        t_speed  = 1000;
        t_duplex = 1;
    } else if ((mii_gb_ctrl & MII_GB_CTRL_ADV_1000HD) &&
               (mii_gb_stat & MII_GB_STAT_LP_1000HD)) {
        t_speed  = 1000;
        t_duplex = 0;
    } else if (mii_ana & MII_ANA_FD_100) {         /* [a] */
        t_speed = 100;
        t_duplex = 1;
    } else if (mii_ana & MII_ANA_T4) {            /* [b] */
        t_speed = 100;
        t_duplex = 0;
    } else if (mii_ana & MII_ANA_HD_100) {        /* [c] */
        t_speed = 100;
        t_duplex = 0;
    } else if (mii_ana & MII_ANA_FD_10) {        /* [d] */
        t_speed = 10;
        t_duplex = 1 ;
    } else if (mii_ana & MII_ANA_HD_10) {        /* [e] */
        t_speed = 10;
        t_duplex = 0;
    }

    if (speed)  {
        *speed  = t_speed;
    }
    if (duplex) {
       *duplex = t_duplex;
    }

    return(SOC_E_NONE);
}

/*
 * Function:
 *    phy_8481_link_get
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
phy_8481_link_get(int unit, soc_port_t port, int *link)
{

    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

    if (link == NULL) {
        return SOC_E_NONE;
    }

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    if (pc->automedium) {
       SOC_IF_ERROR_RETURN
           (_phy_8481_medium_change(unit, port,FALSE));
    }

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_link_get(unit, port, link));
    } else {
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_link_get(unit, port, link));
    }

    LINK_DOWN(pc) =  ! (*link);
    return SOC_E_NONE;
}

STATIC int
_phy_8481_copper_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

    PHY_FW_HANDSHAKE(unit, port);
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc,
                            enable ? 0 : TOP_CONFIG_XGPHY_STRAP1_XGPH_DISABLE,
                                         TOP_CONFIG_XGPHY_STRAP1_XGPH_DISABLE) );
    PHY_FW_HANDSHAKE(unit, port);

    return (SOC_E_NONE);
}

STATIC int
_phy_8481_copper_enable_get(int unit, soc_port_t port, int *enable)
{
    uint16   data16 = 0;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(
            READ_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc, &data16) );
    if ( (data16 & TOP_CONFIG_XGPHY_STRAP1_XGPH_DISABLE) != 0 ) {
        *enable = FALSE;
    } else {
        *enable = TRUE;
    }

    return (SOC_E_NONE);
}

STATIC int
_phy_8481_xaui_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_IS_BCM84887(pc) ) {
        /* do nothing. 84887 fiber enabling/diabling is controled by firmware */
        COMPILER_REFERENCE(enable);
    } else {
        PHYDRV_CALL_ARG1(pc,PHY_ENABLE_SET,enable);
    }
    return (SOC_E_NONE);
}

STATIC int
_phy_8481_xaui_enable_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_IS_BCM84887(pc) ) {    /* 84887 line side fiber is disabled only if */
        uint16  priority, status;   /*   Copper medium is preferred and link-up  */
        SOC_IF_ERROR_RETURN(
                READ_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc, &priority) );
        SOC_IF_ERROR_RETURN(
                READ_PHY84834_CORE_CFG_LINK_STATUSr(     unit, pc, &status  ) );

        *enable = (  (status   & CORE_CFG_LINK_STAT_COPPER_LINK_STATUS)  &&
                    ((priority & TOP_CONFIG_XGPHY_STRAP1_MEDIA_PRIORITY) ==
                                 TOP_CONFIG_XGPHY_STRAP1_MEDIA_PRIORITY_COPPER) )
                ? FALSE : TRUE;
    } else {
        PHYDRV_CALL_ARG1(pc, PHY_ENABLE_GET, enable);
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *    phy_8481_enable_set
 * Purpose:
 *    Enable/Disable phy
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 *      enable - on/off state to set
 * Returns:
 *    SOC_E_NONE
 */
STATIC int
phy_8481_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

    /* Do not incorrectly Power up the interface if medium is disabled and
     * global enable = true
     */
    if (pc->copper.enable && (pc->automedium || PHY_COPPER_MODE(unit, port))) {
        SOC_IF_ERROR_RETURN
            (_phy_8481_copper_enable_set(unit, port, enable));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_8481_enable_set: "
                             "Power %s copper medium\n"), (enable) ? "up" : "down"));
    }

    if (pc->fiber.enable && (pc->automedium || PHY_FIBER_MODE(unit, port))) {
        SOC_IF_ERROR_RETURN
            (_phy_8481_xaui_enable_set(unit, port, enable));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_8481_enable_set: "
                             "Power %s fiber medium\n"), (enable) ? "up" : "down"));
    }

    /* Update software state */
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_enable_get
 * Purpose:
 *      Enable or disable the physical interface for a 8481 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - (OUT) Boolean, true = enable PHY, false = disable.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
phy_8481_enable_get(int unit, soc_port_t port, int *enable)
{
    int data16 = 0;
    *enable = FALSE;

    if (PHY_COPPER_MODE(unit, port)) { /* Copper mode  */
        SOC_IF_ERROR_RETURN
            (_phy_8481_copper_enable_get(unit, port, &data16));
        if(data16 == 1) {
            *enable = TRUE;
        }
    }
    else { /* Fiber mode  */
        SOC_IF_ERROR_RETURN
            (_phy_8481_xaui_enable_get(unit, port, &data16));
        if(data16 == 1) {
            *enable = TRUE;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8481_lb_set
 * Purpose:
 *    Put 8481 in PHY PCS/PMA/PMD loopback
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */
STATIC int
_phy_8481_copper_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc;
    uint16      data16 = 0;
    int         lbmood, speed, rv = SOC_E_NONE;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( enable ) {         /* enable loopback */
        SOC_IF_ERROR_RETURN(_phy_8481_copper_lb_get(unit, port, &lbmood));
        if ( lbmood ) {             /* already in PHY loopback mode */
            return SOC_E_NONE;      /* do nothing               */
        }

        /* try to get the intended loopback speed from LB_SPEED */
        if ( 0 == (speed = LB_SPEED(pc)) ) {    /* 0 means LB_SPEED not set */
            /* intended loopback speed was not set,                        *\
            \* get real line side speed to determine what registers to set */
            SOC_IF_ERROR_RETURN(_phy_8481_copper_speed_get(unit, port, &speed));
        }
        if ( speed >= 2500 ) {     /*  10G / 5G / 2.5G  */
            /* 1000BASE-T/100BASE-TX MII Contol Reg. DEVAD=7, Addr=0xFFe0    */
            if ( PHY_IS_BCM8484X_AND_UP(pc) ) {
                /* turn off MII Control Loopback, and enable AutoNeg */
                SOC_IF_ERROR_RETURN(
                    MODIFY_PHY8481_MII_CTRLr(unit, pc,
                                             MII_CTRL_AE,
                                             MII_CTRL_AE | MII_CTRL_LE) );
            }
            /* PCS Control 1 Reg. DEVAD=3 Addr=0, clear bit[14] PCS loopback mode  */
            if ( PHY_IS_BCM8488X(pc) ) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_PCS_CTRLr(unit, pc, 0, PCS_IEEE_CTL1_LPBK));
            }

            /* PCS control Reg.(3.0x0000), clear bits [5:2] */
            if ( PHY_IS_BCM8488X(pc) ) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_PCS_CTRLr(unit, pc, 0, MGBASE_AN_CTRL_SPEED_MASK_LB));
            }

            if ( PHY_IS_BCM8486X_AND_UP(pc) ) {
                switch ( speed ) {
                case  2500 :
                    PHY_FLAGS_SET(unit, port, PHY_FLAGS_NO_SYS_LINE_SPD_SYNC);
                    data16 = (PHY_IS_BCM8488X(pc)) ? (PCS_IEEE_CTL1_SPEED_SEL_2P5G << 2) : MGBASE_AN_CTRL_SPEED_2P5G;
                    break;
                case  5000 :
                    PHY_FLAGS_SET(unit, port, PHY_FLAGS_NO_SYS_LINE_SPD_SYNC);
                    data16 = (PHY_IS_BCM8488X(pc)) ? (PCS_IEEE_CTL1_SPEED_SEL_5G <<2) : MGBASE_AN_CTRL_SPEED_5G;
                    break;
                case 10000 :
                    if (PHY_IS_BCM8488X(pc)) {
                        data16 = 0;
                    }
                    break;
                default :
                    data16 = 0;
                }
                if ( PHY_IS_BCM8488X(pc) ) {
                    /* PCS control Reg.(3.0x0000), Speed 10G/5G/2.5G [5:2] */    
                    SOC_IF_ERROR_RETURN(
                            MODIFY_PHY8481_PCS_CTRLr(unit, pc,
                                        data16, MGBASE_AN_CTRL_SPEED_MASK_LB) );
                } else {
                /* MGBASE-T AN Control Reg.(30.0x0000), Speed_5G/2.5G[6:5] */
                SOC_IF_ERROR_RETURN(
                    MODIFY_PHY8481_MGBASET_AN_CTRLr(unit, pc,
                                    data16, MGBASE_AN_CTRL_SPEED_MASK) );
                }
            } 
            /* PCS Control 1 Reg. DEVAD=3 Addr=0, set bit[14] PCS loopback mode  */    
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PCS_CTRLr(unit, pc,
                                PCS_IEEE_CTL1_LPBK, PCS_IEEE_CTL1_LPBK));

        } else {                    /* 1G / 100M / 10M */
            /* 1000BASE-T/100BASE-TX MII Contol Reg. DEVAD=7, Addr=0xFFE0    */
            if ( PHY_IS_BCM8484X_AND_UP(pc) ) {
                 /* Clear PCS loopback bit  */
                 SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_PCS_CTRLr(unit, pc,
                                    0, PCS_IEEE_CTL1_LPBK));
                if ( speed == 100 ) {
                    SOC_IF_ERROR_RETURN(
                        WRITE_PHY8481_MII_CTRLr(unit, pc,
                                            PHY8481_MII_CTRL_LOOPBACK_100M) );
                } else {  /* speed = 1000 */
                    SOC_IF_ERROR_RETURN(
                        WRITE_PHY8481_MII_CTRLr(unit, pc,
                                            PHY8481_MII_CTRL_LOOPBACK_1G) );
                }
            } else {
                SOC_IF_ERROR_RETURN               /* bit[14] : Internal Loopback */
                    (MODIFY_PHY8481_MII_CTRLr(unit, pc, MII_CTRL_LE, MII_CTRL_LE));
            }
        }

    } else {                /* disable loopback */
        LB_SPEED(pc) = 0;   /* clear intended loopback speed */
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_NO_SYS_LINE_SPD_SYNC);
        /* PCS Control 1 Reg. DEVAD=3 Addr=0, bit[14] PCS loopback mode  */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PCS_CTRLr(unit, pc, &data16));
        if ( data16 & PCS_IEEE_CTL1_LPBK ) {
            data16 &= (~PCS_IEEE_CTL1_LPBK);   /* bit[14] : Internal Loopback */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PCS_CTRLr(unit, pc, data16));
        }
        /* 1000BASE-T/100BASE-TX MII Contol Reg. DEVAD=7, Addr=0xFFE0    */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MII_CTRLr(unit, pc, &data16));
        if ( data16 & MII_CTRL_LE ) {
            data16 &= (~MII_CTRL_LE);          /* bit[14] : Internal Loopback */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_MII_CTRLr(unit, pc, data16));
        }
    }
    sal_udelay(180);  /* wait for the loopback setting to take effect */

    /* wait for MAC side Link UP/down state while enabling/disabling loopback */
    if ( PHY_IS_BCM8483X_AND_UP(pc) ) {
        soc_timeout_init(&TO(pc), 1000000, 0);
        do {
            rv = READ_PHY84834_CORE_CFG_LINK_STATUSr(unit, pc, &data16);
            if ( (( enable) && ( (data16 & CORE_CFG_LINK_STAT_MAC_LINK_STATUS))) ||
                 ((!enable) && (!(data16 & CORE_CFG_LINK_STAT_MAC_LINK_STATUS))) ||
                 SOC_FAILURE(rv)  ) {
                break;
            }
        } while ( ! soc_timeout_check(&TO(pc)) );
    }

    return rv;
}
      
STATIC int
_phy_8481_xaui_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc; 

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PHYXS_L_CTRLr(unit, pc, (enable) ? MII_CTRL_LE : 0, MII_CTRL_LE));

    /* Configure Loopback in SerDes */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PHYXS_L_COMBO_IEEE0_MIICNTLr(unit, pc,
                                             (enable) ? MII_CTRL_LE : 0,
                                             MII_CTRL_LE));
    return SOC_E_NONE;
}

STATIC int
phy_8481_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc; 

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_lb_set(unit, port, enable));
    } else {
        if ( NXT_PC(pc) ) {
            PHYDRV_CALL_ARG1(pc,PHY_LOOPBACK_SET,enable);
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_lb_set(unit, port, enable));
    }

    if ( enable ) {
        /* set link-down flag in order to notify appropriate speed *\
        \* in phy_8481_link_up()                                   */
        LINK_DOWN(pc) = TRUE;
        /* PHY driver has to call phy_8481_link_up() when setting loopback */
        SOC_IF_ERROR_RETURN( phy_8481_link_up(unit, port) );
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8481_lb_get
 * Purpose:
 *    Get 8481 PHY loopback state
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
_phy_8481_copper_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t *pc;
    uint16      tmp;

    pc = EXT_PHY_SW_STATE(unit, port);

    /* PCS Control 1 Reg. DEVAD=3 Addr=0, bit[14] PCS loopback mode */
    SOC_IF_ERROR_RETURN
            (READ_PHY8481_PCS_CTRLr(unit, pc, &tmp));
    /* ctrl != 0xffff check is to handle removable PHY daughter cards */
    *enable = ((tmp != 0xffff) && (tmp & PCS_IEEE_CTL1_LPBK)) ? TRUE : FALSE;

    if ( ! (*enable) ) {
        /* 1000BASE-T/100BASE-TX MII Contol Reg. DEVAD=7, Addr=0xFFe0    */
        SOC_IF_ERROR_RETURN               /* bit[14] : Internal Loopback */
                (READ_PHY8481_MII_CTRLr(unit, pc, &tmp));
       /* ctrl != 0xffff check is to handle removable PHY daughter cards */
        *enable = ((tmp != 0xffff) && (tmp & MII_CTRL_LE)) ? TRUE : FALSE;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8481_lb_get: u=%d port%d: loopback:%s\n"),
              unit, port, *enable ? "Enabled": "Disabled"));


    return SOC_E_NONE;
}

STATIC int
_phy_8481_xaui_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      mii_ctrl;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_L_COMBO_IEEE0_MIICNTLr(unit, pc, &mii_ctrl));
    /* ctrl != 0xffff check is to handle removable PHY daughter cards */
    *enable = (mii_ctrl != 0xffff) && ((mii_ctrl & MII_CTRL_LE) == MII_CTRL_LE);

    return (SOC_E_NONE);
}

STATIC int
phy_8481_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_lb_get(unit, port, enable));
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG1(pc,PHY_LOOPBACK_GET,enable);
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_lb_get(unit, port, enable));
    }

    return SOC_E_NONE;
}

int
phy_8481_xehg_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    int  speed = 0;

    SOC_IF_ERROR_RETURN(phy_8481_speed_get(unit, port, &speed));
    switch (speed) {
        case 100  : /* fall through */
        case 1000 :
            *pif = SOC_PORT_IF_SGMII;
            break;
        case 10000:
            *pif = SOC_PORT_IF_XFI;
            break;
        case 2500 : /* fall through */
        case 5000 :
            if ( PHY_REPEATER(unit, port) ) {
                /* MAC interface is in XFI mode */
                *pif = SOC_PORT_IF_XFI;
            } else {
                /* MAC interface is in 2500/5000BASE-X mode */
                *pif = SOC_PORT_IF_SGMII;
            }
            break;
         case 0 :
                *pif = SOC_PORT_IF_XFI;    
            break;
        default:   
           return SOC_E_PARAM; 
    }
    
    return SOC_E_NONE;
}

#ifdef PHY8481_DEBUG_AUTOMEDIUM
int
show_phy_8481_clkrst_reg(int unit, soc_port_t port)
{
    phy_ctrl_t   *pc;
    uint16    status=0;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_TOP_CONFIG_LINK_STATUS_REG(unit, pc, &status));

    LOG_CLI((BSL_META_U(unit,
                        "TOP_CONFIG_LINK_STATUS_REG = %x\n"), status));

    return SOC_E_NONE;
}
#endif

STATIC int
_phy_8481_medium_check(int unit, soc_port_t port, int *medium)
{
    phy_ctrl_t   *pc;
    uint16    status;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_FORCED_COPPER_MODE(unit, port)) {
        *medium = SOC_PORT_MEDIUM_COPPER;
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_TOP_CONFIG_LINK_STATUS_REG(unit, pc, &status));

    if ( status & TOP_CONFIG_LINK_STATUS_REG_FIBER_PRIO ) {
        if ( status & TOP_CONFIG_LINK_STATUS_REG_FIBER_ACT ) {
            *medium = SOC_PORT_MEDIUM_FIBER;
        } else if ( status & TOP_CONFIG_LINK_STATUS_REG_COPPER_ACT ) {
                *medium = SOC_PORT_MEDIUM_COPPER;
        } else {
            *medium = SOC_PORT_MEDIUM_FIBER;
        }
    } else {
        if ( status & TOP_CONFIG_LINK_STATUS_REG_COPPER_ACT ) {
            *medium = SOC_PORT_MEDIUM_COPPER;
        } else  if ( status & TOP_CONFIG_LINK_STATUS_REG_FIBER_ACT ) {
            *medium = SOC_PORT_MEDIUM_FIBER;
        } else {
            *medium = SOC_PORT_MEDIUM_COPPER;
        }
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_medium_change(int unit, soc_port_t port, int force_update)
{
    phy_ctrl_t    *pc;
    int            medium;

    pc    = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_8481_medium_check(unit, port, &medium));

    if (medium == SOC_PORT_MEDIUM_COPPER) {
        if ((!PHY_COPPER_MODE(unit, port)) || force_update) { /* Was fiber */
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);
            PHY8481_SELECT_REGS_CU(unit, pc);

            if (pc->copper.enable) {
                /* PHYDRV_CALL_ARG1(pc,PHY_ENABLE_SET,FALSE); */
                SOC_IF_ERROR_RETURN
                    (_phy_8481_medium_config_update(unit, port, &pc->copper));
            }
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_8481_link_auto_detect: u=%d p=%d [F->X]\n"), unit, port));
        }
    } else {        /* Fiber */
        if ((!PHY_FIBER_MODE(unit, port)) || force_update) { /* Was copper */
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);
            PHY8481_SELECT_REGS_XAUI(unit, pc);

            if (pc->fiber.enable) {
                /* PHYDRV_CALL_ARG1(pc,PHY_ENABLE_SET,TRUE); */
                SOC_IF_ERROR_RETURN
                    (_phy_8481_medium_config_update(unit, port, &pc->fiber));
            }
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_8481_link_auto_detect: u=%d p=%d [C->X]\n"), unit, port));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_medium_status
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
phy_8481_medium_status(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    if (PHY_COPPER_MODE(unit, port)) {
        *medium = SOC_PORT_MEDIUM_COPPER;
    } else {
        *medium = SOC_PORT_MEDIUM_XAUI;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_8481_medium_config_update
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
_phy_8481_medium_config_update(int unit, soc_port_t port,
                                soc_phy_config_t *cfg)
{

    SOC_IF_ERROR_RETURN
        (phy_8481_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_8481_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_8481_ability_advert_set(unit, port, &cfg->advert_ability));
    SOC_IF_ERROR_RETURN
        (phy_8481_an_set(unit, port, cfg->autoneg_enable));
    SOC_IF_ERROR_RETURN
        (phy_8481_mdix_set(unit, port, cfg->mdix));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_medium_config_set
 * Purpose:
 *      Set the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/XAUI
 *      cfg - Operating parameters
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_8481_medium_config_set(int unit, soc_port_t port, 
                           soc_port_medium_t  medium,
                           soc_phy_config_t  *cfg)
{
    phy_ctrl_t    *pc;
    soc_phy_config_t *active_medium;  /* Currently active medium */
    soc_phy_config_t *change_medium;  /* Requested medium */
    soc_phy_config_t *other_medium;   /* The other medium */
    int               medium_update;

    if (NULL == cfg || cfg->enable != TRUE) {
        return SOC_E_PARAM;
    }

    pc            = EXT_PHY_SW_STATE(unit, port);
    medium_update = FALSE;

    switch (medium) {
    case SOC_PORT_MEDIUM_COPPER:
        if (!pc->automedium) {
            if (!PHY_COPPER_MODE(unit, port)) {
                return SOC_E_UNAVAIL;
            }
            /* check if device is fiber capable before switching */
            if (cfg->preferred == 0) {
                if (PHY_FORCED_COPPER_MODE(unit, port)) {
                    /* return if not fiber capable*/
                    return SOC_E_UNAVAIL;
                }
            }
        }
        change_medium  = &pc->copper;
        other_medium   = &pc->fiber;
        break;
    case SOC_PORT_MEDIUM_FIBER:
        if (!pc->automedium && !PHY_FIBER_MODE(unit, port)) {
            return SOC_E_UNAVAIL;
        }
        change_medium  = &pc->fiber;
        other_medium   = &pc->copper;
        break;
    default:
        return SOC_E_PARAM;
    }

    /*
     * Changes take effect immediately if the target medium is active or
     * the preferred medium changes.
     */
    if (change_medium->enable != cfg->enable) {
        medium_update = TRUE;
    }
    if (change_medium->preferred != cfg->preferred) {
        /* Make sure that only one medium is preferred */
        other_medium->preferred = !cfg->preferred;
        medium_update = TRUE;
    }

    sal_memcpy(change_medium, cfg, sizeof(*change_medium));

    if (!PHY_FORCED_COPPER_MODE(unit, port)) {
        PHY_FW_HANDSHAKE(unit, port);
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc, pc->fiber.preferred ? 1U<<8 : 0, 1U<<8));
        PHY_FW_HANDSHAKE(unit, port);
    }

    if (medium_update) {
        /* The new configuration may cause medium change. Check
         * and update medium.
         */
        SOC_IF_ERROR_RETURN
            (_phy_8481_medium_change(unit, port,TRUE));
    } else {
        active_medium = (PHY_COPPER_MODE(unit, port)) ?  
                            &pc->copper : &pc->fiber;
        if (active_medium == change_medium) {
            /* If the medium to update is active, update the configuration */
            SOC_IF_ERROR_RETURN
                (_phy_8481_medium_config_update(unit, port, change_medium));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_medium_config_get
 * Purpose:
 *      Get the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/XAUI
 *      cfg - (OUT) Operating parameters
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_8481_medium_config_get(int unit, soc_port_t port, 
                           soc_port_medium_t medium,
                           soc_phy_config_t *cfg)
{
    int            rv;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    switch (medium) {
    case SOC_PORT_MEDIUM_COPPER:
        if (pc->automedium || PHY_COPPER_MODE(unit, port)) {
            sal_memcpy(cfg, &pc->copper, sizeof (*cfg));
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    case SOC_PORT_MEDIUM_FIBER:
        if (pc->automedium || (!PHY_COPPER_MODE(unit, port))) {
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

/* STATIC */ int
_phy84834_top_level_cmd_set(int unit,phy_ctrl_t *pc, uint16 cmd, uint16 arg[], int size)
{
    soc_timeout_t  to;
    int            rv;
    uint16 status;

    if ((size < 1) || (size > 4)) {
        return (SOC_E_PARAM);
    }

    if (cmd == PHY84834_DIAG_CMD_PAIR_SWAP_CHANGE) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84834_TOP_CONFIG_SCRATCH_2r(unit, pc, PHY84834_CMD_OPEN_OVERRIDE));
    }

    soc_timeout_init(&to, 7000000, 0);
    do {
        rv = READ_PHY84834_TOP_CONFIG_SCRATCH_2r(unit, pc, &status);
        if (((status & PHY84834_CMD_OPEN_FOR_CMDS) != 0) ||
            SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));
    if ((status & PHY84834_CMD_OPEN_FOR_CMDS) == 0) {
        LOG_CLI((BSL_META_U(unit,
                            "PHY84834_TOP_LEVEL_CMD_SET failed: u=%d p=%d cmd=%08x\n"),
                 unit, pc->port, cmd));
        return (SOC_E_FAIL);
    }

    if (size-- > 0) {
        if (cmd == PHY84834_DIAG_CMD_PAIR_SWAP_CHANGE) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84834_TOP_CONFIG_SCRATCH_4r(unit, pc, arg[0]));
        } else {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84834_TOP_CONFIG_SCRATCH_3r(unit, pc, arg[0]));
        }
    }
    if (size-- > 0) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84834_TOP_CONFIG_SCRATCH_4r(unit, pc, arg[1]));
    }
    /* coverity[dead_error_condition:FALSE] */
    if (size-- > 0) {
        /* coverity[dead_error_begin] */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84834_TOP_CONFIG_SCRATCH_12r(unit, pc, arg[2]));
    }
    /* coverity[dead_error_condition:FALSE] */
    if (size-- > 0) {
        /* coverity[dead_error_begin] */ 
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84834_TOP_CONFIG_SCRATCH_13r(unit, pc, arg[3]));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY84834_TOP_CONFIG_SCRATCH_0r(unit, pc, cmd));

    soc_timeout_init(&to, 1000000, 0);
    do {
        rv = READ_PHY84834_TOP_CONFIG_SCRATCH_2r(unit, pc, &status);
        if (((status & (PHY84834_CMD_COMPLETE_PASS | PHY84834_CMD_COMPLETE_ERROR)) != 0) ||
            SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));
    if ((status & PHY84834_CMD_COMPLETE_PASS) == 0) {
        LOG_CLI((BSL_META_U(unit,
                            "PHY84834_TOP_LEVEL_CMD_SET failed: u=%d p=%d cmd=%08x\n"),
                 unit, pc->port, cmd));
        return (SOC_E_FAIL);
    }

    SOC_IF_ERROR_RETURN(
            WRITE_PHY84834_TOP_CONFIG_SCRATCH_2r(unit, pc, PHY84834_CMD_CLEAR_COMPLETE) );

    return (SOC_E_NONE);
}

/* STATIC */ int
_phy84834_top_level_cmd_get(int unit,phy_ctrl_t *pc, uint16 cmd, uint16 arg[], int size)
{
    soc_timeout_t  to;
    int            rv;
    uint16 status;

    if ((size < 1) || (size > 4)) {
        return (SOC_E_PARAM);
    }

    soc_timeout_init(&to, 7000000, 0);
    do {
        rv = READ_PHY84834_TOP_CONFIG_SCRATCH_2r(unit, pc, &status);
        if (((status & PHY84834_CMD_OPEN_FOR_CMDS) != 0) ||
            SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));
    if ((status & PHY84834_CMD_OPEN_FOR_CMDS) == 0) {
        LOG_CLI((BSL_META_U(unit,
                            "PHY84834_TOP_LEVEL_CMD_GET failed: u=%d p=%d cmd=%08x\n"),
                 unit, pc->port, cmd));
        return (SOC_E_FAIL);
    }
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84834_TOP_CONFIG_SCRATCH_0r(unit, pc, cmd));

    soc_timeout_init(&to, 1000000, 0);

    do {
        rv = READ_PHY84834_TOP_CONFIG_SCRATCH_2r(unit, pc, &status);
        if (((status & (PHY84834_CMD_COMPLETE_PASS | PHY84834_CMD_COMPLETE_ERROR)) != 0) ||
            SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));
    if ((status & PHY84834_CMD_COMPLETE_PASS) == 0) {
        LOG_CLI((BSL_META_U(unit,
                            "PHY84834_TOP_LEVEL_CMD_GET failed: u=%d p=%d cmd=%08x\n"),
                 unit, pc->port, cmd));
        return (SOC_E_FAIL);
    }
    if (size-- > 0) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84834_TOP_CONFIG_SCRATCH_3r(unit, pc, &arg[0]));
    }
    if (size-- > 0) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84834_TOP_CONFIG_SCRATCH_4r(unit, pc, &arg[1]));
    }
    /* coverity[dead_error_condition:FALSE] */
    if (size-- > 0) {
        /* coverity[dead_error_begin] */ 
        SOC_IF_ERROR_RETURN
            (READ_PHY84834_TOP_CONFIG_SCRATCH_12r(unit, pc, &arg[2]));
    }
    /* coverity[dead_error_condition:FALSE] */
    if (size-- > 0) {
        /* coverity[dead_error_begin] */
        SOC_IF_ERROR_RETURN
            (READ_PHY84834_TOP_CONFIG_SCRATCH_13r(unit, pc, &arg[3]));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY84834_TOP_CONFIG_SCRATCH_2r(unit, pc, PHY84834_CMD_CLEAR_COMPLETE));

    return (SOC_E_NONE);

}

/* STATIC */ int
_phy84834_top_level_cmd_set_v2(int unit,phy_ctrl_t *pc, uint16 cmd, uint16 arg[], int size)
{
    soc_timeout_t  to;
    int            rv;
    uint16 status;

    if ((size < 1) || (size > 5)) {
        return (SOC_E_PARAM);
    }

    soc_timeout_init(&to, 7000000, 0);
    do {
        rv = READ_PHY84834_TOP_CONFIG_SCRATCH_26r(unit, pc, &status);
        if (PHY8481_IS_READY_FOR_CMD(pc, status) || SOC_FAILURE(rv)) {    
            break;
        }
    } while (!soc_timeout_check(&to));
    if (!PHY8481_IS_OPEN_FOR_CMD(pc, status)) {
        LOG_CLI((BSL_META_U(unit,
                            "PHY84834_TOP_LEVEL_CMD_SET failed:<1> u=%d p=%d "
                            "cmd=%04x status=%04x\n"), unit, pc->port, cmd, status));
        return (SOC_E_FAIL);
    }

    if (size-- > 0) {
        if (cmd == PHY84834_DIAG_CMD_SET_PAIR_SWAP_V2) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84834_TOP_CONFIG_SCRATCH_28r(unit, pc, arg[0]));
        } else {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84834_TOP_CONFIG_SCRATCH_27r(unit, pc, arg[0]));
        }
    }
    if (size-- > 0) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84834_TOP_CONFIG_SCRATCH_28r(unit, pc, arg[1]));
    }
    /* coverity[dead_error_condition:FALSE] */ 
    if (size-- > 0) {
         /* coverity[dead_error_begin] */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84834_TOP_CONFIG_SCRATCH_29r(unit, pc, arg[2]));
    }
    /* coverity[dead_error_condition:FALSE] */
    if (size-- > 0) {
        /* coverity[dead_error_begin] */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84834_TOP_CONFIG_SCRATCH_30r(unit, pc, arg[3]));
    }
    /* coverity[dead_error_condition:FALSE] */
    if (size-- > 0) {
        /* coverity[dead_error_begin] */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84834_TOP_CONFIG_SCRATCH_31r(unit, pc, arg[4]));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY84834_TOP_CONFIG_SCRATCH_0r(unit, pc, cmd));

    soc_timeout_init(&to, 7000000, 0);
    do {
        rv = READ_PHY84834_TOP_CONFIG_SCRATCH_26r(unit, pc, &status);
        if (PHY8481_IS_CMD_COMPLETE(pc, status) || SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));
    if (status != PHY8481_CMD_COMPLETE_PASS) {
        LOG_CLI((BSL_META_U(unit,
                            "PHY84834_TOP_LEVEL_CMD_SET failed:<2> u=%d p=%d "
                            "cmd=%04x status=%04x\n"), unit, pc->port, cmd, status));
        return (SOC_E_FAIL);
    }

    if (PHY_IS_BCM8483X(pc) || PHY_IS_BCM8484X(pc)) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84834_TOP_CONFIG_SCRATCH_26r(unit, pc, PHY84834_CMD_CLEAR_COMPLETE));
    }
    return (SOC_E_NONE);
}

/* STATIC */ int
_phy84834_top_level_cmd_get_v2(int unit,phy_ctrl_t *pc, uint16 cmd, uint16 arg[], int size)
{
    soc_timeout_t  to;
    int            rv;
    uint16 status;

    if ((size < 1) || (size > 5)) {
        return (SOC_E_PARAM);
    }

    soc_timeout_init(&to, 7000000, 0);
    do {
        rv = READ_PHY84834_TOP_CONFIG_SCRATCH_26r(unit, pc, &status);
        if (PHY8481_IS_READY_FOR_CMD(pc, status) || SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));
    if (!PHY8481_IS_OPEN_FOR_CMD(pc, status)) {
        LOG_CLI((BSL_META_U(unit,
                            "PHY84834_TOP_LEVEL_CMD_GET failed:<1> u=%d p=%d "
                            "cmd=%04x status=%04x\n"), unit, pc->port, cmd, status));
        return (SOC_E_FAIL);
    }
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84834_TOP_CONFIG_SCRATCH_0r(unit, pc, cmd));

    soc_timeout_init(&to, 1000000, 0);
    do {
        rv = READ_PHY84834_TOP_CONFIG_SCRATCH_26r(unit, pc, &status);
        if (PHY8481_IS_CMD_COMPLETE(pc, status) || SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));
    if (status != PHY8481_CMD_COMPLETE_PASS) {
        LOG_CLI((BSL_META_U(unit,
                            "PHY84834_TOP_LEVEL_CMD_GET failed:<2> u=%d p=%d "
                            "cmd=%04x status=%04x\n"), unit, pc->port, cmd, status));
        return (SOC_E_FAIL);
    }

    if (size-- > 0) {
        if (cmd == PHY84834_DIAG_CMD_GET_PAIR_SWAP_V2 && !(PHY_IS_BCM8488X(pc)) ) {
            SOC_IF_ERROR_RETURN
                (READ_PHY84834_TOP_CONFIG_SCRATCH_28r(unit, pc, &arg[0]));
        } else {
            SOC_IF_ERROR_RETURN
                (READ_PHY84834_TOP_CONFIG_SCRATCH_27r(unit, pc, &arg[0]));
        }
    }
    if (size-- > 0) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84834_TOP_CONFIG_SCRATCH_28r(unit, pc, &arg[1]));
    }
    /* coverity[dead_error_condition:FALSE] */
    if (size-- > 0) {
        /* coverity[dead_error_begin] */ 
        SOC_IF_ERROR_RETURN
            (READ_PHY84834_TOP_CONFIG_SCRATCH_29r(unit, pc, &arg[2]));
    }
    /* coverity[dead_error_condition:FALSE] */
    if (size-- > 0) {
        /* coverity[dead_error_begin] */
        SOC_IF_ERROR_RETURN
            (READ_PHY84834_TOP_CONFIG_SCRATCH_30r(unit, pc, &arg[3]));
    }
    /* coverity[dead_error_condition:FALSE] */
    if (size-- > 0) {
        /* coverity[dead_error_begin] */
        SOC_IF_ERROR_RETURN
            (READ_PHY84834_TOP_CONFIG_SCRATCH_31r(unit, pc, &arg[4]));
    }

    if (PHY_IS_BCM8483X(pc) || PHY_IS_BCM8484X(pc)) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84834_TOP_CONFIG_SCRATCH_26r(unit, pc, PHY84834_CMD_CLEAR_COMPLETE));
    }

    return (SOC_E_NONE);

}

/* READ a register: 32-bit addressing mode */
int
_phy848xx_addr32_reg_read(int unit, phy_ctrl_t *pc, uint32 addr32, uint16 *val)
{
    uint16       cmd_args[3];

    cmd_args[0] = ADDR32BIT_HIGH(addr32);
    cmd_args[1] = ADDR32BIT_LOW( addr32);
    cmd_args[2] = ADDR32BIT_READ;
    SOC_IF_ERROR_RETURN(
        PHY84834_TOP_LEVEL_CMD_SET(unit, pc,
                                   PHY84834_DIAG_CMD_ACCESS_32BIT_XFI_ADDR,
                                   cmd_args, 3) );
    SOC_IF_ERROR_RETURN(    /* get the value from Command Handler Data 5 Reg. */
                READ_PHY84834_TOP_CONFIG_SCRATCH_31r(unit, pc, val) );
    return SOC_E_NONE;
}

/* WRITE a register: 32-bit addressing mode */
int
_phy848xx_addr32_reg_write(int unit, phy_ctrl_t *pc, uint32 addr32, uint16 val)
{
    uint16       cmd_args[4];

    cmd_args[0] = ADDR32BIT_HIGH(addr32);
    cmd_args[1] = ADDR32BIT_LOW( addr32);
    cmd_args[2] = ADDR32BIT_WRITE;
    cmd_args[3] = val;      /* 16-bit data to be written to the register */
    SOC_IF_ERROR_RETURN(
        PHY84834_TOP_LEVEL_CMD_SET(unit, pc,
                                   PHY84834_DIAG_CMD_ACCESS_32BIT_XFI_ADDR,
                                   cmd_args, 4) );
    return SOC_E_NONE;
}

/* MODIFY a register: 32-bit addressing mode */
int
_phy848xx_addr32_reg_modify(int unit, phy_ctrl_t *pc, uint32 addr32,
                            uint16 val, uint16 mask)
{
    uint16       data16;
    SOC_IF_ERROR_RETURN( _phy848xx_addr32_reg_read(unit, pc, addr32, &data16) );
    data16 =  (data16 & (~mask)) | val;
    SOC_IF_ERROR_RETURN( _phy848xx_addr32_reg_write(unit, pc, addr32, data16) );
    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int
_phy84834_top_level_cmd_get_dispatch_v2(int unit,phy_ctrl_t *pc, uint16 cmd)
{
    int            rv;
    uint16 status;

    soc_timeout_init(&TO(pc), 7000000, 0);

    do {
        rv = READ_PHY84834_TOP_CONFIG_SCRATCH_26r(unit, pc, &status);
        if (PHY8481_IS_READY_FOR_CMD(pc, status) ||SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&TO(pc)));
    if (!PHY8481_IS_OPEN_FOR_CMD(pc, status)) {
        LOG_CLI((BSL_META_U(unit,
                            "PHY84834_TOP_LEVEL_CMD_GET (dispatch) failed:<1>"
                            " u=%d p=%d cmd=%04x status=%04x\n"),
                 unit, pc->port, cmd, status));
        return SOC_E_FAIL;
    }
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84834_TOP_CONFIG_SCRATCH_0r(unit, pc, cmd));

    soc_timeout_init(&TO(pc), 1000000, 0);

    return SOC_E_NONE;
}

STATIC int
_phy84834_top_level_cmd_get_retrive_v2(int unit,phy_ctrl_t *pc, uint16 cmd, uint16 arg[], int size)
{
    int            rv;
    uint16 status;

    do {
        rv = READ_PHY84834_TOP_CONFIG_SCRATCH_26r(unit, pc, &status);
        if (PHY8481_IS_CMD_COMPLETE(pc, status) || SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&TO(pc)));
    if (status != PHY8481_CMD_COMPLETE_PASS) {
        LOG_CLI((BSL_META_U(unit,
                            "PHY84834_TOP_LEVEL_CMD_GET failed:<2> u=%d p=%d"
                            "cmd=%04x status=%04x\n"), unit, pc->port, cmd, status));
        return SOC_E_FAIL;
    }
    if (size-- > 0) {
        if (cmd == PHY84834_DIAG_CMD_GET_PAIR_SWAP_V2) {
            SOC_IF_ERROR_RETURN
                (READ_PHY84834_TOP_CONFIG_SCRATCH_28r(unit, pc, &arg[0]));
        } else {
            SOC_IF_ERROR_RETURN
                (READ_PHY84834_TOP_CONFIG_SCRATCH_27r(unit, pc, &arg[0]));
        }
    }

    if (size-- > 0) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84834_TOP_CONFIG_SCRATCH_28r(unit, pc, &arg[1]));
    }
    if (size-- > 0) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84834_TOP_CONFIG_SCRATCH_29r(unit, pc, &arg[2]));
    }
    if (size-- > 0) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84834_TOP_CONFIG_SCRATCH_30r(unit, pc, &arg[3]));
    }
    if (size-- > 0) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84834_TOP_CONFIG_SCRATCH_31r(unit, pc, &arg[4]));
    }

    if (PHY_IS_BCM8483X(pc) || PHY_IS_BCM8484X(pc)) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84834_TOP_CONFIG_SCRATCH_26r(unit, pc, PHY84834_CMD_CLEAR_COMPLETE));
    }

    return SOC_E_NONE;
}
#endif

STATIC int
_phy84834_eee_mode_set(int unit,phy_ctrl_t *pc, uint16 mode, uint16 ag_th_high, uint16 ag_th_low, uint16 latency )
{
    uint16 args[5];
    args[0] = mode;
    args[1] = ag_th_high;
    args[2] = ag_th_low;
    args[3] = latency;

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_SET(unit, pc, (uint16)PHY84834_DIAG_CMD_SET_EEE_MODE_GENERIC, args, 4));

    return (SOC_E_NONE);
}

STATIC int
_phy84834_eee_mode_get(int unit,phy_ctrl_t *pc, uint16 *mode, uint16 *ag_th_high, uint16 *ag_th_low, uint16 *latency )
{
    uint16 args[5];

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)PHY84834_DIAG_CMD_GET_EEE_MODE_GENERIC, args, 4));

    if (mode) {
        *mode = args[0];
    }
    if (ag_th_high) {
        *ag_th_high = args[1];
    }
    if (ag_th_low) {
        *ag_th_low = args[2];
    }
    if (latency) {
        *latency = args[3];
    }

    return (SOC_E_NONE);
}

STATIC int
_phy84834_jumbo_packet_set(int unit, phy_ctrl_t *pc, uint16 mode)
{
    uint16 args[2];
    args[0] = mode;

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_SET(unit, pc, (uint16)PHY848x_DIAG_CMD_SET_JUMBO_FRAME_MODE, args, 1));

    return (SOC_E_NONE);
}

#if 0
STATIC int
_phy84834_jumbo_packet_get(int unit,phy_ctrl_t *pc, uint16 *mode)
{
    uint16 args[2];

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)PHY848x_DIAG_CMD_GET_JUMBO_FRAME_MODE, args, 1));

    if (mode) {
        *mode = args[0];
    }

    return (SOC_E_NONE);
}
#endif

STATIC int
_phy84834_chip_current_temp_get(int unit,phy_ctrl_t *pc, uint16 *digital_temp, uint16 *analog_temp )
{
    uint16 args[3];

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)PHY84834_DIAG_CMD_GET_CURRENT_TEMP_GENERIC, args, 2));

    if (digital_temp) {
        *digital_temp = args[0];
    }
    if (analog_temp) {
        *analog_temp = args[1];
    }

    return (SOC_E_NONE);
}

STATIC int
_phy84834_xfi_reg_set(int unit,phy_ctrl_t *pc, uint16 bank, uint16 offset, uint16 value )
{
    uint16 args[5];
    uint16 cmd;
    args[0] = bank;
    args[1] = offset;
    args[2] = value;

    if (pc->flags & PHYCTRL_SYS_SIDE_CTRL)
        cmd = (uint16)PHY84834_DIAG_CMD_SET_XFI_M_REG_VALUES_GENERIC;
    else
        cmd = (uint16)PHY84834_DIAG_CMD_SET_XFI_L_REG_VALUES_GENERIC;
 
    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_SET(unit, pc, (uint16)cmd, args, 3));

    return (SOC_E_NONE);
}

STATIC int
_phy84834_xfi_reg_get(int unit,phy_ctrl_t *pc, uint16 bank, uint16 offset, uint16 *value )
{
    uint16 args[5];
    uint16 cmd;
    args[0] = bank;
    args[1] = offset;

    if (pc->flags & PHYCTRL_SYS_SIDE_CTRL)
        cmd = (uint16)PHY84834_DIAG_CMD_GET_XFI_M_REG_VALUES_GENERIC;
    else
        cmd = (uint16)PHY84834_DIAG_CMD_GET_XFI_L_REG_VALUES_GENERIC;
    
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84834_TOP_CONFIG_SCRATCH_27r(unit, pc, args[0]));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84834_TOP_CONFIG_SCRATCH_28r(unit, pc, args[1]));

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)cmd, args, 3));

    if (value) {
        *value = args[2];
    }

    return (SOC_E_NONE);
}


STATIC int
_phy84834_xfi_reg_set_v2(int unit, phy_ctrl_t *pc, uint16 pre_tap, uint16 main_tap, uint16 post1_tap, uint16 post2_tap, uint16 HPF_en)
{
    uint16  args[5];

    args[0] = pre_tap;
    args[1] = main_tap;
    args[2] = post1_tap;
    args[3] = post2_tap;
    args[4] = HPF_en;

    /* Koi BCM8485X only supports system side preemphasis  */
    if ( pc->flags & PHYCTRL_SYS_SIDE_CTRL ) {
        return  PHY84834_TOP_LEVEL_CMD_SET(unit, pc,
                    (uint16) PHY84834_DIAG_CMD_SET_XFI_TX_FILTERS_GENERIC, args, 5);
    }
    return (SOC_E_UNAVAIL);
}

STATIC int
_phy84834_xfi_reg_get_v2(int unit, phy_ctrl_t *pc, uint16 *pre_tap, uint16 *main_tap, uint16 *post1_tap, uint16 *post2_tap, uint16 *HPF_en )
{
    uint16 args[5];
    uint16 cmd = 0;

    /* Koi BCM8485X only supports system side preemphasis  */
    if (pc->flags & PHYCTRL_SYS_SIDE_CTRL)
        cmd = (uint16)PHY84834_DIAG_CMD_GET_XFI_TX_FILTERS_GENERIC;

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)cmd, args, 5));

    if (pre_tap) {
        *pre_tap = args[0];
    }
    if (main_tap) {
        *main_tap = args[1];
    }
    if (post1_tap) {
        *post1_tap = args[2];
    }
    if (post2_tap) {
        *post2_tap = args[3];
    }
    if (HPF_en) {
        *HPF_en = args[4];
    }

    return (SOC_E_NONE);
}


STATIC uint16
_sign_extend_16(uint16 val, uint8 ori_sz_in_bit)
{
    uint16  neg_max =  0x1 << (ori_sz_in_bit - 1);
    uint16  mask    =  (0x1 << ori_sz_in_bit) - 1;
    /*
     * example of ori_sz_in_bit = 5
     *   neg_max = 0x0010 =  0x1 << (5 - 1)
     *   mask    = 0x001f =  (0x1 << 5) - 1
     *  ~mask    = 0xffe0 = ~0x001f    [extension prefix for negative num]
     */
	val &= mask;
    if ( val & neg_max ) {    /* if a negative number  */
        val |= (~mask);       /* sign extension for 16-bit number */
    }

    return  val;
}

#define  BITMASK_7_0    (0xFF)
#define  XFIM           0
#define  XFIL           1

STATIC int
_phy8488x_xfi_tx_filter_set(int unit, phy_ctrl_t *pc, uint16 pre_tap, uint16 main_tap,
                            uint16 post1_tap, uint16 post2_tap, uint16 post3_tap)
{
    uint16  args[5];

    /* 8488x supports XFI Config Type = 0 (XFIM) only */
    args[0] =  0;       /* (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? XFIM : XFIL; */
    /* get the current values of XFI_TX_FILTERS first */
    PHY84834_TOP_LEVEL_CMD_GET(unit, pc,
                    (uint16) PHY84834_DIAG_CMD_GET_XFI_TX_FILTERS_GENERIC, args, 5);
    args[0]  =  0;
    args[1]  = (main_tap  << 8) | (pre_tap   & BITMASK_7_0);
    args[2]  = (post2_tap << 8) | (post1_tap & BITMASK_7_0);
    args[3] &=  ~BITMASK_7_0;              /* retain the bit[15:8] of DATA4 */
    args[3] |=  post3_tap & BITMASK_7_0;
    args[4]  =  0;       /* amp_ctrl & driver_mode are always 0 for 8488x */

    return  PHY84834_TOP_LEVEL_CMD_SET(unit, pc,
                    (uint16) PHY84834_DIAG_CMD_SET_XFI_TX_FILTERS_GENERIC, args, 5);
}

STATIC int
_phy8488x_xfi_tx_filter_get(int unit, phy_ctrl_t *pc, uint16 *pre_tap, uint16 *main_tap,
                            uint16 *post1_tap, uint16 *post2_tap, uint16 *post3_tap)
{
    uint16  args[5];

    /* 8488x supports XFI Config Type = 0 (XFIM) only */
    args[0] =  0;       /* (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? XFIM : XFIL; */
    SOC_IF_ERROR_RETURN(
        PHY84834_TOP_LEVEL_CMD_GET(unit, pc,
                    (uint16) PHY84834_DIAG_CMD_GET_XFI_TX_FILTERS_GENERIC, args, 5) );
    if ( pre_tap   ) {
        *pre_tap   = args[1] & BITMASK_7_0;
    }
    if ( main_tap  ) {
        *main_tap  = args[1] >> 8;
    }
    if ( post1_tap ) {
        *post1_tap = args[2] & BITMASK_7_0;
    }
    if ( post2_tap ) {      /* -15 <= Post2 <= 15 */
        *post2_tap = _sign_extend_16((args[2] >> 8), 8);
    }
    if ( post3_tap ) {      /*  -7 <= Post2 <= 7  */
        *post3_tap = _sign_extend_16((args[3] & BITMASK_7_0), 8);
    }

    return (SOC_E_NONE);
}

STATIC int
_phy8485x_shadow_reg_write(int unit, phy_ctrl_t *pc, uint16 reg_group, uint16 reg_addr, uint16 bit_mask, uint16 bit_shift, uint16 value) 
{
    uint16 args[5];
    uint16 cmd;
    args[0] = reg_group;
    args[1] = reg_addr;
    args[2] = bit_mask;
    args[3] = bit_shift;
    args[4] = value;

    cmd = (uint16)PHY8485X_DIAG_CMD_WRITE_SHADOW_REG;

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_SET(unit, pc, (uint16)cmd, args, 5));

    return (SOC_E_NONE);
}

STATIC int
_phy8485x_shadow_reg_read(int unit, phy_ctrl_t *pc, uint16 reg_group, uint16 reg_addr, uint16 bit_mask, uint16 bit_shift, uint16 *value) 
{
    uint16 args[5];
    uint16 cmd;
    args[0] = reg_group;
    args[1] = reg_addr;
    args[2] = bit_mask;
    args[3] = bit_shift;

    cmd = (uint16)PHY8485X_DIAG_CMD_READ_SHADOW_REG;

    SOC_IF_ERROR_RETURN
        (WRITE_PHY84834_TOP_CONFIG_SCRATCH_27r(unit, pc, args[0]));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84834_TOP_CONFIG_SCRATCH_28r(unit, pc, args[1]));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84834_TOP_CONFIG_SCRATCH_29r(unit, pc, args[2]));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84834_TOP_CONFIG_SCRATCH_30r(unit, pc, args[3]));

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)cmd, args, 5));

    if (value) {
        *value = args[4];
    }

    return (SOC_E_NONE);
}

STATIC int
_phy8481_adjust_xfi_tx_preemphasis(int unit, int port, phy_ctrl_t *pc, uint32 phy_long_xfi)
{
    uint16 pre_tap = 0;
    uint16 main_tap = 0;
    uint16 post_tap1 = 0;
    uint16 post_tap2 = 0;
    uint16 HPF_en = 0;

    switch(phy_long_xfi) {

        case 1:
            if (PHY_IS_BCM8485X_AND_UP(pc)) {

             /* The value format:
             * bit 16:18: post_tap2 value
             * bit 15:  1 enable forced preemphasis ctrl, 0 auto config
             * bit 14:10:  post_tap value
             * bit 09:04:  main tap value
             * bit 03:00:  pre_tap value
             */

            /* main_tap = 32d, post_tap = 12d, pre_tap=8d for long channel on KOI */    
            pre_tap = 0x8;
            main_tap = 0x20;
            post_tap1 = 0xC;
            post_tap2 = 0x0;
            HPF_en = 0x0;

                SOC_IF_ERROR_RETURN(
                    _phy84834_xfi_reg_set_v2(unit, pc, pre_tap, main_tap, post_tap1, post_tap2, HPF_en));
            
            } else {
               /* Register bank = 0x2  offset = 0xC value = 0x3AF7  */
                SOC_IF_ERROR_RETURN(
                    _phy84834_xfi_reg_set(unit, pc, 0x2, 0xC, 0x3AF7));

            }
            break;

        default:
            break;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_8481_control_tx_driver_set
 * Purpose:
 *      Configure PHY device specific control function.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to update
 *      value - New setting for the control
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
_phy_8481_control_tx_driver_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;
    phy_ctrl_t    *pc;       /* PHY software state */
    uint16       data;  /* Temporary holder of reg value to be written */
    uint16       tmp ;  /* Temporary holder of reg value read */
    uint16       mask;  /* Bit mask of reg value to be updated */
    uint16       HPF_en    = 0;
    uint16       pre_tap   = 0;
    uint16       main_tap  = 0;
    uint16       post1_tap = 0;
    uint16       post2_tap = 0;
    uint16       post3_tap = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

#if 0
    /* Support only system side in copper mode - for now */
    if (PHY_COPPER_MODE(unit, port) && !(pc->flags & PHYCTRL_SYS_SIDE_CTRL)) {
        return SOC_E_UNAVAIL;
    }
#endif

    rv = SOC_E_NONE;
    switch ( type ) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        if ( PHY_IS_BCM8488X(pc) ) {
           /* format of 'value':      Decimal   Hexadecimal
            *   bit 27:24 post3       [-7,7]    [0x9 ,0x7]
            *   bit 22:18 post2       [-15,15]  [0x11,0x0f]
            *   bit 17:12 post1       [0,63]    [0x00,0x3f]
            *   bit 11:05 main tap    [0,112]   [0x00,0x70]
            *   bit 04:00 pre_tap     [0,31]    [0x00,0x1f]
            * criterion A: ( main + 48 > pre + post1 + post2 + post3 )
            * criterion B: ( main + pre + post1 + |post2| + |post3| <= 112 )
            */
            pre_tap   = PHY8488X_PREEMPH_GET_PRE_TAP(value);
            main_tap  = PHY8488X_PREEMPH_GET_MAIN_TAP(value);
            post1_tap = PHY8488X_PREEMPH_GET_POST1_TAP(value);
            post2_tap = _sign_extend_16(PHY8488X_PREEMPH_GET_POST2_TAP(value), 5);
            post3_tap = _sign_extend_16(PHY8488X_PREEMPH_GET_POST3_TAP(value), 4);
            
            /* Set Data 1 - Data 5 values  */
            SOC_IF_ERROR_RETURN(
                _phy8488x_xfi_tx_filter_set(unit, pc, pre_tap, main_tap,
                                            post1_tap, post2_tap, post3_tap));

        } else
        if ( PHY_IS_BCM8485X_AND_UP(pc) ) {
            /* The value format:
            * bit 18:16: post_tap2 value
            * bit 15:  1 enable forced preemphasis ctrl, 0 auto config
            * bit 14:10:  post_tap value
            * bit 09:04:  main tap value
            * bit 03:00:  pre_tap value
            */
            pre_tap   = PHY8485X_PREEMPH_GET_PRE_TAP(value);
            main_tap  = PHY8485X_PREEMPH_GET_MAIN_TAP(value);
            post1_tap = PHY8485X_PREEMPH_GET_POST_TAP1(value);
            post2_tap = PHY8485X_PREEMPH_GET_POST_TAP2(value);
            HPF_en    = PHY8485X_PREEMPH_GET_HPF_EN(value);
            /* Set Data 1 - Data 5 values  */
            SOC_IF_ERROR_RETURN(
                _phy84834_xfi_reg_set_v2(unit, pc, pre_tap, main_tap, post1_tap, post2_tap, HPF_en));
        }
        else {
            mask     = 0xf800; /* Main Tap */
            /* Register bank = 0x2  offset = 0xB value */
            SOC_IF_ERROR_RETURN(
                _phy84834_xfi_reg_set(unit, pc, 0x2, 0xB, (value & mask)));
            mask     = 0x00f7; /* Pre/Post Cursor */
            /* Register bank = 0x2  offset = 0xC value */
            SOC_IF_ERROR_RETURN(
                _phy84834_xfi_reg_set(unit, pc, 0x2, 0xC, (value & mask)));
        }
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        if (PHY_IS_BCM8485X_AND_UP(pc)) {
            /* register bit fields idriver no longer exist in Merlin Core */
            rv = SOC_E_UNAVAIL;      
        }
        else {
            data = (uint16)(value & 0xf);
            data = data << 12;
            mask = 0xf000;
            data = data & mask; /* Mask off other bits */
            /* Register bank = 0x2  offset = 0xA value */
            SOC_IF_ERROR_RETURN(
                _phy84834_xfi_reg_get(unit, pc, 0x2, 0xA, &tmp));

            tmp &= ~(mask);
            tmp |= data;

            /* Register bank = 0x2  offset = 0xA value */
            SOC_IF_ERROR_RETURN(
                _phy84834_xfi_reg_set(unit, pc, 0x2, 0xA, tmp));
        }
        break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        if (PHY_IS_BCM8485X_AND_UP(pc)) {
            /* register bit fields ipredriver no longer exist in Merlin Core */
            rv = SOC_E_UNAVAIL;
        }
        else {
            data = (uint16)(value & 0xf);
            data = data << 8;
            mask = 0x0f00;
            data = data & mask; /* Mask off other bits */
            /* Register bank = 0x2  offset = 0xA value */
            SOC_IF_ERROR_RETURN(
                _phy84834_xfi_reg_get(unit, pc, 0x2, 0xA, &tmp));

            tmp &= ~(mask);
            tmp |= data;

            /* Register bank = 0x2  offset = 0xA value */
            SOC_IF_ERROR_RETURN(
                _phy84834_xfi_reg_set(unit, pc, 0x2, 0xA, tmp));
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
 *      _phy_8481_control_tx_driver_get
 * Purpose:
 *      Configure PHY device specific control function.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to update
 *      value - New setting for the control
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
_phy_8481_control_tx_driver_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    phy_ctrl_t  *pc;       /* PHY software state */
    uint16       data;
    uint32       temp = 0;
    uint16       HPF_en    = 0;
    uint16       pre_tap   = 0;
    uint16       main_tap  = 0;
    uint16       post1_tap = 0;
    uint16       post2_tap = 0;
    uint16       post3_tap = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
#if 0
    /* Support only system side in copper mode - for now */
    if (PHY_COPPER_MODE(unit, port) && !(pc->flags & PHYCTRL_SYS_SIDE_CTRL)) {
        return SOC_E_UNAVAIL;
    }
#endif
    rv = SOC_E_NONE;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        if ( PHY_IS_BCM8488X(pc) ) {
           /* format of 'value':      Decimal   Hexadecimal
            *   bit 27:24 post3       [-7,7]    [0x9 ,0x7]
            *   bit 22:18 post2       [-15,15]  [0x11,0x0f]
            *   bit 17:12 post1       [0,63]    [0x00,0x3f]
            *   bit 11:05 main tap    [0,112]   [0x00,0x70]
            *   bit 04:00 pre_tap     [0,31]    [0x00,0x1f]
            */
            SOC_IF_ERROR_RETURN(
                _phy8488x_xfi_tx_filter_get(unit, pc, &pre_tap, &main_tap,
                                            &post1_tap, &post2_tap, &post3_tap));
            *value = PHY8488X_PREEMPH_SET_PRE_TAP(pre_tap)
                   | PHY8488X_PREEMPH_SET_MAIN_TAP(main_tap)
                   | PHY8488X_PREEMPH_SET_POST1_TAP(post1_tap)
                   | PHY8488X_PREEMPH_SET_POST2_TAP(post2_tap)
                   | PHY8488X_PREEMPH_SET_POST3_TAP(post3_tap);

        } else
        if (PHY_IS_BCM8485X_AND_UP(pc)) {
            SOC_IF_ERROR_RETURN(
                _phy84834_xfi_reg_get_v2(unit, pc, &pre_tap, &main_tap, &post1_tap, &post2_tap, &HPF_en));
        
            *value = 0;
            *value |= pre_tap;
            main_tap = ((temp | main_tap) << 4);
            post1_tap = ((temp | post1_tap) << 10);
            HPF_en = ((temp | HPF_en) << 15);
            post2_tap = ((temp | post2_tap) << 16);

            *value |= (main_tap | post1_tap | HPF_en | post2_tap);
        }
        else {
            /* Register bank = 0x2  offset = 0xB value */
            SOC_IF_ERROR_RETURN(
                _phy84834_xfi_reg_get(unit, pc, 0x2, 0xB, &data));
            *value = data & 0xf800; /* Main */
            /* Register bank = 0x2  offset = 0xC value */
            SOC_IF_ERROR_RETURN(
                _phy84834_xfi_reg_get(unit, pc, 0x2, 0xC, &data));
            *value |= (data & 0x00f7); /* Post and Pre cursor */
        }
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        if (PHY_IS_BCM8485X_AND_UP(pc)) {
            /* register bit fields idriver no longer exist in Merlin Core */            
            rv = SOC_E_UNAVAIL;
        }
        else {
            /* Register bank = 0x2  offset = 0xA value */
            SOC_IF_ERROR_RETURN(
                _phy84834_xfi_reg_get(unit, pc, 0x2, 0xA, &data));
            *value = ((data & 0xf000) >> 12);    
        }
        break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        if (PHY_IS_BCM8485X_AND_UP(pc)) {
            /* register bit fields ipredriver no longer exist in Merlin Core */
            rv = SOC_E_UNAVAIL;        
        }
        else {
            /* Register bank = 0x2  offset = 0xA value */
            SOC_IF_ERROR_RETURN(
                _phy84834_xfi_reg_get(unit, pc, 0x2, 0xA, &data));
            *value = ((data & 0x0f00) >> 8);    
        }
        break;
    default:
        rv = SOC_E_PARAM;
        break;
    }
    return rv;
}

STATIC int
_phy_8485x_force_cl72_config(int unit, phy_ctrl_t *pc, uint16 enable)
{
    uint16 args[5]={0,};

    args[0] = enable;
    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_SET(unit, pc, (uint16)PHY84834_DIAG_CMD_SET_KR_MODE_ENABLE_V2, args, 1));

    return (SOC_E_NONE);
}

int
_phy_8481_copper_control_driver_set(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 value)
{
    phy_ctrl_t  *pc;    /* PHY software state */
    uint16 temp;
    int rv = SOC_E_NONE;
    uint16 mode;
    int intf;

    pc = EXT_PHY_SW_STATE(unit, port);

    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_8481_control_tx_driver_set(unit, port, type, value);
        break;
    case SOC_PHY_CONTROL_LOOPBACK_EXTERNAL:
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_MII_AUX_CTRLr(unit, pc, value ? 1U<<15 : 0, 1U<<15));
        break;
    case SOC_PHY_CONTROL_EEE:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            return SOC_E_UNAVAIL;
        }

        if (value) {
            /* Clearing 10G/1G EEE stats */
            SOC_IF_ERROR_RETURN( _phy_8481_clear_eee_stats(unit, pc) );

            if (PHY_IS_BCM8483X_AND_UP(pc)) {
                /* 1G IEEE EEE, 10G IEEE EEE */
                if (PHY_IS_BCM8484X_AND_UP(pc)) {
                    mode = 5;
                } else {
                    mode = 1;
                }
                SOC_IF_ERROR_RETURN
                    (_phy84834_eee_mode_set(unit, pc, mode, 0x0000, 0x0000, 0x0000 ));
            } else {
                /* Disable AUTO EEE if native is enabled */
                SOC_IF_ERROR_RETURN
                   (MODIFY_PHY8481_TOP_CONFIG_AUTOGREEN_CONFIG1r(unit, pc, 0, 0x1));
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_EEE_ADVr(unit, pc, 0x000e, 0x000e));

            pc->copper.advert_ability.eee |= (SOC_PA_EEE_10GB_BASET | SOC_PA_EEE_1GB_BASET | SOC_PA_EEE_100MB_BASETX);
            temp = pc->copper.autoneg_enable;
            /* Initiate AN */
            SOC_IF_ERROR_RETURN
                (_phy_8481_copper_an_set(unit, port, 1));
            pc->copper.autoneg_enable = temp;
            if (PHY_IS_BCM8483X_AND_UP(pc)) {
                SOC_IF_ERROR_RETURN
                    (_phy84834_eee_mode_get(unit, pc, &temp, NULL, NULL, NULL));
            } 
            PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_EEE_ENABLED);
            PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_EEE_MODE);
        } else {
            if (PHY_IS_BCM8483X_AND_UP(pc)) {
                SOC_IF_ERROR_RETURN
                    (_phy84834_eee_mode_set(unit, pc, 0, 0x0000, 0x0000, 0x0000 ));
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_EEE_ADVr(unit, pc, 0x0000, 0x000e));
            pc->copper.advert_ability.eee &= ~(SOC_PA_EEE_10GB_BASET | SOC_PA_EEE_1GB_BASET | SOC_PA_EEE_100MB_BASETX);
            SOC_IF_ERROR_RETURN
                (_phy_8481_copper_an_set(unit, port, pc->copper.autoneg_enable ? 1 : 0));
            PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_EEE_ENABLED);
        }
        break;

    case SOC_PHY_CONTROL_EEE_AUTO:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            return SOC_E_UNAVAIL;
        }

        if (value) {
            /* Clearing 10G/1G EEE stats */
            SOC_IF_ERROR_RETURN( _phy_8481_clear_eee_stats(unit, pc) );

            if (PHY_IS_BCM8483X_AND_UP(pc)) {
                /* 1G IEEE EEE, 10G IEEE EEE */
                if (PHY_IS_BCM8484X_AND_UP(pc)) {
                    mode = 0xa;
                } else {
                    mode = 2;
                }
                SOC_IF_ERROR_RETURN
                    (_phy84834_eee_mode_set(unit, pc, mode, 0x0000, 0x3d09, ((PHY_IS_BCM8488X(pc)) ? 0x0480 : 0x047e) ));

            } else {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_TOP_CONFIG_AUTOGREEN_CONFIG1r(unit, pc, 0x0001, 0x0001));
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_EEE_ADVr(unit, pc, 0x000e, 0x000e));

            pc->copper.advert_ability.eee |= (SOC_PA_EEE_10GB_BASET | SOC_PA_EEE_1GB_BASET | SOC_PA_EEE_100MB_BASETX);
            temp = pc->copper.autoneg_enable;
            /* Initiate AN */
            SOC_IF_ERROR_RETURN
                (_phy_8481_copper_an_set(unit, port, 1));
            pc->copper.autoneg_enable = temp;
            if (PHY_IS_BCM8483X_AND_UP(pc)) {
                SOC_IF_ERROR_RETURN
                    (_phy84834_eee_mode_get(unit, pc, &temp, NULL, NULL, NULL));
            } 
            PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_EEE_ENABLED);
            PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_EEE_MODE);
        } else {
            if (PHY_IS_BCM8483X_AND_UP(pc)) {
                SOC_IF_ERROR_RETURN
                    (_phy84834_eee_mode_set(unit, pc, 0, 0x0000, 0x0000, 0x0000 ));
            } else {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_TOP_CONFIG_AUTOGREEN_CONFIG1r(unit, pc, 0x0000, 0x0001));
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_EEE_ADVr(unit, pc, 0x0000, 0x000e));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PHYC_CTL_AI_LPI_CTLr(unit, pc, 0x8000));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PHYC_CTL_AI_LPI_CTLr(unit, pc, 0x0000));
            pc->copper.advert_ability.eee &= ~(SOC_PA_EEE_10GB_BASET | SOC_PA_EEE_1GB_BASET | SOC_PA_EEE_100MB_BASETX);
            SOC_IF_ERROR_RETURN
                (_phy_8481_copper_an_set(unit, port, pc->copper.autoneg_enable ? 1 : 0));
            PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_EEE_ENABLED);
        }
        break;

    case SOC_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            return SOC_E_UNAVAIL;
        }
        if (PHY_IS_BCM8483X_AND_UP(pc)) {
            uint16 mode=0, latency=0, th_high=0, th_low=0;
            SOC_IF_ERROR_RETURN
                (_phy84834_eee_mode_get(unit, pc, &mode, &th_high, &th_low, &latency ));
            /* Fixed latency can have only 0 or 1 value  */
            if ((value != FALSE) && (value != TRUE)) {
                 return SOC_E_PARAM;
            }
            if (value == FALSE) {
                /* Variable latency  */
                SOC_IF_ERROR_RETURN
                    (_phy84834_eee_mode_set(unit, pc, 3, th_high, th_low, latency ));
            } else {
                /* Fixed latency  */ 
                SOC_IF_ERROR_RETURN
                    (_phy84834_eee_mode_set(unit, pc, 2, th_high, th_low, latency ));
            }
        } else {
            SOC_IF_ERROR_RETURN
               (MODIFY_PHY8481_TOP_CONFIG_AUTOGREEN_CONFIG1r(unit, pc, value ? 1U<<1 : 0, 1U<<1));
        }
        break;

    case SOC_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            return SOC_E_UNAVAIL;
        }
        if (PHY_IS_BCM8483X_AND_UP(pc)) {
            uint16 mode=0, buffer_len=0;
            SOC_IF_ERROR_RETURN
                (_phy84834_eee_mode_get(unit, pc, &mode, NULL, NULL, &buffer_len ));
            SOC_IF_ERROR_RETURN
                (_phy84834_eee_mode_set(unit, pc, mode, (uint16)((value >> 16) & 0xffff), (uint16)(value & 0xffff), buffer_len ));
        } else {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_TOP_CONFIG_AUTOGREEN_THRESHOLD_LOWr(unit, pc, (uint16)(value & 0xffff)));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_TOP_CONFIG_AUTOGREEN_THRESHOLD_HIGHr(unit, pc, (uint16)((value >> 16) & 0xffff)));
        }

        break;
    case SOC_PHY_CONTROL_EEE_STATISTICS_CLEAR:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            return SOC_E_UNAVAIL;
        }

        /* Clearing 10G EEE stats: Enable clear on Read  */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_EEE_STATISTICS_CTLr(unit, pc,0x0003, 0x000f));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_EEE_STATISTICS_CTLr(unit, pc,0x000f, 0x000f));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_EXP_EEE_TX_EVENTS_HIGHr(unit, pc, &temp));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_EXP_EEE_TX_EVENTS_LOWr(unit, pc, &temp));
        /* Clearing 10G EEE stats: Disable clear on Read  */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_EEE_STATISTICS_CTLr(unit, pc,0x0002, 0x000f));
        
        /* Clearing 1G EEE stats  */       
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_1G_EEE_STATISTICS_CTLr(unit, pc, 0x8002, 0x8002));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_1G_EEE_STATISTICS_CTLr(unit, pc, 0x8000, 0x8002));
        break;
        
    case SOC_PHY_CONTROL_EEE_AUTO_BUFFER_LIMIT:
    case SOC_PHY_CONTROL_EEE_TRANSMIT_WAKE_TIME:
    case SOC_PHY_CONTROL_EEE_RECEIVE_WAKE_TIME:
    case SOC_PHY_CONTROL_EEE_TRANSMIT_SLEEP_TIME:
    case SOC_PHY_CONTROL_EEE_RECEIVE_SLEEP_TIME:
    case SOC_PHY_CONTROL_EEE_TRANSMIT_QUIET_TIME:
    case SOC_PHY_CONTROL_EEE_RECEIVE_QUIET_TIME:
    case SOC_PHY_CONTROL_EEE_TRANSMIT_REFRESH_TIME:
            return SOC_E_UNAVAIL;

    case SOC_PHY_CONTROL_POWER:
        if (PHY_IS_BCM8483X_AND_UP(pc)) {
            SOC_IF_ERROR_RETURN
                (_phy_84834_power_mode_set(unit,port,value));
        }
        break;

    case SOC_PHY_CONTROL_CL72:
        if ( PHY_IS_BCM8485X_AND_UP(pc) ) {
            rv = _phy_8485x_force_cl72_config(unit, pc, value);
            break;
        } else {
            return SOC_E_UNAVAIL;
        }

    case SOC_PHY_CONTROL_MGBASET_802P3BZ_PRIORITY:
        if ( PHY_IS_BCM8488X(pc) ) {
            rv = _phy_848xx_mgbase_802_3bz_type_set(unit, pc, value, value);
            break;
        } else {
            return SOC_E_UNAVAIL;
        }
    
    case SOC_PHY_CONTROL_SUPER_ISOLATE :
        rv = MODIFY_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc,
                                    (value) ? TOP_CONFIG_XGPHY_DISABLE : 0x0,
                                    TOP_CONFIG_XGPHY_DISABLE);
        break;

    default:
        intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY_DIAG_INTF_SYS : PHY_DIAG_INTF_LINE;
        SOC_IF_ERROR_RETURN
            (_phy_84834_control_set(unit, port, intf, PHY_DIAG_LN_DFLT, type, value));
        break; 
    }

    return rv;
}


STATIC int
_phy_8485x_force_cl72_status(int unit, phy_ctrl_t *pc, uint16 *value)
{
	uint16 args[5] = {0,};

    args[0] = 0;
    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)PHY84834_DIAG_CMD_GET_KR_MODE_ENABLE_V2, args, 1));
    *value = args[0];

    return SOC_E_NONE;
}

int
_phy_8481_copper_control_driver_get(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 *value)
{
    phy_ctrl_t    *pc;       /* PHY software state */
    uint16 temp, temp1;
    uint16 digital_temp = 0, analog_temp = 0, frame_priority = 0;
    int rv = SOC_E_NONE;
    int speed=0;
    int intf;
 
    pc = EXT_PHY_SW_STATE(unit, port);

    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_8481_control_tx_driver_get(unit, port, type, value);
        break;
    case SOC_PHY_CONTROL_LOOPBACK_EXTERNAL:
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
         SOC_IF_ERROR_RETURN
             (READ_PHY8481_MII_AUX_CTRLr(unit, pc, &temp));
         *value = temp & (1U<<15) ? 1 : 0;
         break;

    case SOC_PHY_CONTROL_EEE:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            return SOC_E_UNAVAIL;
        }
        *value = (PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_ENABLED) && 
                 !PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_MODE)) ? 1 : 0;
        break;

    case SOC_PHY_CONTROL_EEE_AUTO:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            return SOC_E_UNAVAIL;
        }
        *value = (PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_ENABLED) && 
                  PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_MODE)) ? 1 : 0;
        break;

    case SOC_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            return SOC_E_UNAVAIL;
        }
        if (PHY_IS_BCM8483X_AND_UP(pc)) {
            uint16 mode=0, latency=0;
            SOC_IF_ERROR_RETURN 
                (_phy84834_eee_mode_get(unit, pc, &mode, NULL, NULL, &latency ));
            if (mode == 2 || mode == 6 || mode == 0xa) {
                *value = 1;
            }
            else if (mode == 3 || mode == 7 || mode == 0xb) {
                *value = 0;
            }
        } else {
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_TOP_CONFIG_AUTOGREEN_CONFIG1r(unit, pc, &temp));
            *value = (temp>>1) & 0x1;
        }
        break;

    case SOC_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            return SOC_E_UNAVAIL;
        }
        if (PHY_IS_BCM8483X_AND_UP(pc)) {
            SOC_IF_ERROR_RETURN
                (_phy84834_eee_mode_get(unit, pc, NULL, &temp1, &temp, NULL));
        } else {
           SOC_IF_ERROR_RETURN
              (READ_PHY8481_TOP_CONFIG_AUTOGREEN_THRESHOLD_LOWr(unit, pc, &temp));
           SOC_IF_ERROR_RETURN
              (READ_PHY8481_TOP_CONFIG_AUTOGREEN_THRESHOLD_HIGHr(unit, pc, &temp1));
        }
        *value = (((uint32) temp1) << 16) | temp;
        break;

    case SOC_PHY_CONTROL_EEE_TRANSMIT_EVENTS:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            return SOC_E_UNAVAIL;
        }

        SOC_IF_ERROR_RETURN
            (phy_8481_speed_get(unit, port, &speed));

        if (PHY_IS_BCM8484X(pc) || PHY_IS_BCM8485X_AND_UP(pc)) {
            if (speed == 10000) {
                /* Latch the stats: Write 1 to bit 2 and 3 */
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_EEE_STATISTICS_CTLr(unit, pc,0x000c, 0x000c));
            }
            else 
                if (speed == 1000) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY8481_1G_EEE_STATISTICS_CTLr(unit, pc, 0x8001, 0x4001));
                }
        }

        /* Speed 1G */
        if (speed == 1000 ) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_EXP_1G_TX_EVENT(unit, pc,(uint16 *) value));
        }
        /* Speed 10G */
        else
            if (speed == 10000 ) {
                SOC_IF_ERROR_RETURN
                   (READ_PHY8481_EXP_EEE_TX_EVENTS_LOWr(unit, pc, &temp));
                SOC_IF_ERROR_RETURN
                   (READ_PHY8481_EXP_EEE_TX_EVENTS_HIGHr(unit, pc, &temp1));
                *value = (((uint32) temp1) << 16) | temp;
            }
        break;

    case SOC_PHY_CONTROL_EEE_TRANSMIT_DURATION:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            return SOC_E_UNAVAIL;
        }
        
        SOC_IF_ERROR_RETURN
            (phy_8481_speed_get(unit, port, &speed));

        if (PHY_IS_BCM8484X(pc) || PHY_IS_BCM8485X_AND_UP(pc)) {
            if (speed == 10000) {
                /* Latch the stats: Write 1 to bit 2 and 3 */
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_EEE_STATISTICS_CTLr(unit, pc,0x000c, 0x000c));
            }
            else
                if (speed == 1000) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY8481_1G_EEE_STATISTICS_CTLr(unit, pc, 0x8001, 0x4001));
               }
        }

        /* Speed 1G */
        if (speed == 1000 ) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_EXP_1G_TX_DURATION(unit, pc,(uint16 *) value));
        }    
        /* Speed 10G */
        else
            if (speed == 10000 ) {
                SOC_IF_ERROR_RETURN
                   (READ_PHY8481_EXP_EEE_TX_DURATION_LOWr(unit, pc, &temp));
                SOC_IF_ERROR_RETURN
                   (READ_PHY8481_EXP_EEE_TX_DURATION_HIGHr(unit, pc, &temp1));
                *value = (((uint32) temp1) << 16) | temp;
        }
        break;

    case SOC_PHY_CONTROL_EEE_RECEIVE_EVENTS:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            return SOC_E_UNAVAIL;
        }

        SOC_IF_ERROR_RETURN
            (phy_8481_speed_get(unit, port, &speed));

        if (PHY_IS_BCM8484X(pc) || PHY_IS_BCM8485X_AND_UP(pc)) {
            if (speed == 10000) {
                /* Latch the stats: Write 1 to bit 2 and 3 */
                SOC_IF_ERROR_RETURN
                   (MODIFY_PHY8481_EEE_STATISTICS_CTLr(unit, pc,0x000c, 0x000c));
            }   
            else
                if (speed == 1000) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY8481_1G_EEE_STATISTICS_CTLr(unit, pc, 0x8001, 0x4001));
               }
        }

        /* Speed 1G */
        if (speed == 1000 ) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_EXP_1G_RX_EVENT(unit, pc,(uint16 *) value));
        }
        /* Speed 10G */
        else
            if (speed == 10000 ) {
                SOC_IF_ERROR_RETURN
                   (READ_PHY8481_EXP_EEE_RX_EVENTS_LOWr(unit, pc, &temp));
                SOC_IF_ERROR_RETURN
                   (READ_PHY8481_EXP_EEE_RX_EVENTS_HIGHr(unit, pc, &temp1));
                *value = (((uint32) temp1) << 16) | temp;
            }
        break;

    case SOC_PHY_CONTROL_EEE_RECEIVE_DURATION:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            return SOC_E_UNAVAIL;
        }
  
        SOC_IF_ERROR_RETURN
            (phy_8481_speed_get(unit, port, &speed));
  
        if (PHY_IS_BCM8484X(pc) || PHY_IS_BCM8485X_AND_UP(pc)) {
            if (speed == 10000) {
                /* Latch the stats: Write 1 to bit 2 and 3 */
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_EEE_STATISTICS_CTLr(unit, pc,0x000c, 0x000c));
            }
            else
                if (speed == 1000) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY8481_1G_EEE_STATISTICS_CTLr(unit, pc, 0x8001, 0x4001));
               }
        }

        /* Speed 1G */
        if (speed == 1000 ) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_EXP_1G_RX_DURATION(unit, pc,(uint16 *) value));
        }
        /* Speed 10G */
        else
            if (speed == 10000 ) {
                SOC_IF_ERROR_RETURN
                   (READ_PHY8481_EXP_EEE_RX_DURATION_LOWr(unit, pc, &temp));
                SOC_IF_ERROR_RETURN
                   (READ_PHY8481_EXP_EEE_RX_DURATION_HIGHr(unit, pc, &temp1));
                *value = (((uint32) temp1) << 16) | temp;
            }
        break;

    case SOC_PHY_CONTROL_POWER:
        if (PHY_IS_BCM8483X_AND_UP(pc)) {
            SOC_IF_ERROR_RETURN
                (_phy_84834_power_mode_get(unit,port,value));
        } else {
            *value = pc->power_mode;
        }
        break;

    case SOC_PHY_CONTROL_DIGITAL_TEMP:
        if (PHY_IS_BCM8483X_AND_UP(pc)) {
            SOC_IF_ERROR_RETURN
                (_phy84834_chip_current_temp_get(unit, pc, &digital_temp, NULL));
        }
        *value = digital_temp;
        break;

    case SOC_PHY_CONTROL_ANALOG_TEMP:
        if (PHY_IS_BCM8483X_AND_UP(pc)) {
            SOC_IF_ERROR_RETURN
                (_phy84834_chip_current_temp_get(unit, pc, NULL, &analog_temp));
        }
        *value = analog_temp;
        /* Invalid analog temperature value is returned  */
        rv=SOC_E_UNAVAIL;
        break;
   
    case SOC_PHY_CONTROL_MGBASET_802P3BZ_PRIORITY:
        if ( PHY_IS_BCM8488X(pc) ) {
            rv = _phy_848xx_mgbase_802_3bz_type_get(unit, pc, NULL, &frame_priority);
            *value = frame_priority;
        } else {
            rv=SOC_E_UNAVAIL;
        }
        break;

    case SOC_PHY_CONTROL_SUPER_ISOLATE :
        SOC_IF_ERROR_RETURN(
                READ_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc, &temp) );
        *value = (temp & TOP_CONFIG_XGPHY_DISABLE) ? TRUE : FALSE;
        break;

    default:
        intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY_DIAG_INTF_SYS : PHY_DIAG_INTF_LINE;
        SOC_IF_ERROR_RETURN
            (_phy_84834_control_get(unit, port, intf, PHY_DIAG_LN_DFLT, type, value));
        break;
    }

    return rv;
}

int
_phy_8481_xaui_control_driver_set(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 value)
{
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         /*Fall through*/
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         /*Fall through*/
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         _phy_8481_control_tx_driver_set(unit, port, type, value);
         break;

    default:
         /* should never get here */
         return SOC_E_UNAVAIL;
    }



    return SOC_E_NONE;
}

int
_phy_8481_xaui_control_driver_get(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 *value)
{
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         /*Fall through*/
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         /*Fall through*/
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         _phy_8481_control_tx_driver_get(unit, port, type, value);
         break;

    default:
         /* should never get here */
         return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_control_set
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
phy_8481_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        return(_phy_8481_copper_control_driver_set(unit, port, type, value));
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG2(pc,PHY_CONTROL_SET,type,value);
            return SOC_E_NONE;
        }
        return(_phy_8481_xaui_control_driver_set(unit, port, type, value));
    }

}
/*
 * Function:
 *      phy_8481_control_get
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
phy_8481_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        return(_phy_8481_copper_control_driver_get(unit, port, type, value));
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG2(pc,PHY_CONTROL_GET,type,value);
            return SOC_E_NONE;
        }
        return(_phy_8481_xaui_control_driver_get(unit, port, type, value));
    }

}

/*
 * Function:
 *      phy_8481_probe
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
phy_8481_probe(int unit, phy_ctrl_t *pc)
{
    uint16 id0, id1;

    if ( READ_PHY8481_TOPLVL1_ID0r(unit, pc, &id0) < 0 ) {
        return SOC_E_NOT_FOUND;
    }

    if ( READ_PHY8481_TOPLVL1_ID1r(unit, pc, &id1) < 0 ) {
        return SOC_E_NOT_FOUND;
    }

    /* check OUI & MODEL ID to identify the PHY chip */
    switch ( PHY_OUI_MODEL(PHY_OUI(id0, id1), PHY_MODEL(id0, id1)) ) {

    case  PHY_OUI_MODEL(PHY_BCM8481X_OUI, PHY_BCM8481X_MODEL) :
        if ( PHY_REV(id0, id1) == 0x7)  {
            /* 84812 detected */
            PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_FORCED_COPPER);
            PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_XAUI);
            pc->dev_name = "BCM84812";
        }
    break;

    case  PHY_OUI_MODEL(PHY_BCM84822_OUI, PHY_BCM84822_MODEL) :
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_FORCED_COPPER);
        PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_XAUI);
        if ( PHY_REV(id0, id1) != 0x0)  {
            /* A0 does not support EEE */
            PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_EEE_CAPABLE);
        }
    break;

    case  PHY_OUI_MODEL(PHY_BCM84823_OUI, PHY_BCM84823_MODEL) :
        if ( PHY_REV(id0, id1) != 0x0)  {
            /* A0 does not support EEE */
            PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_EEE_CAPABLE);
        }
    break;

    case  PHY_OUI_MODEL(PHY_BCM84833_OUI, PHY_BCM84833_MODEL) :
        if ( id1 & 0x8 ) {
#ifdef INCLUDE_MACSEC
            if ( soc_property_port_get(unit, pc->port, spn_MACSEC_ENABLE, 0) ) {
                return SOC_E_NOT_FOUND;
            }
#endif
            pc->dev_name = "BCM84333";
        }
    break;

    case  PHY_OUI_MODEL(PHY_BCM84836_OUI, PHY_BCM84836_MODEL) :
        if ( id1 & 0x8 ) {
#ifdef INCLUDE_MACSEC
            if ( soc_property_port_get(unit, pc->port, spn_MACSEC_ENABLE, 0) ) {
                return SOC_E_NOT_FOUND;
            }
#endif
            pc->dev_name = "BCM84336";
        }
    break;

    case  PHY_OUI_MODEL(PHY_BCM84834_OUI, PHY_BCM84834_MODEL) :
        if ( id1 & 0x8 ) {
#ifdef INCLUDE_MACSEC
            if ( soc_property_port_get(unit, pc->port, spn_MACSEC_ENABLE, 0) ) {
                return SOC_E_NOT_FOUND;
            }
#endif
            pc->dev_name = "BCM84334";
        }
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_FORCED_COPPER);
        PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_FIBER);
    break;

    case  PHY_OUI_MODEL(PHY_BCM84835_OUI, PHY_BCM84835_MODEL) :
        if ( id1 & 0x8 ) {
            return SOC_E_NOT_FOUND;
        }
    break;

    case  PHY_OUI_MODEL(PHY_BCM84846_OUI, PHY_BCM84846_MODEL) :
#ifdef INCLUDE_MACSEC
        if ( soc_property_port_get(unit, pc->port, spn_MACSEC_ENABLE, 0) ) {
            return SOC_E_NOT_FOUND;
        }
#endif
    break;

    case  PHY_OUI_MODEL(PHY_BCM84844_OUI, PHY_BCM84844_MODEL) :
    case  PHY_OUI_MODEL(PHY_BCM84848_OUI, PHY_BCM84848_MODEL) :
#ifdef INCLUDE_MACSEC
        if ( soc_property_port_get(unit, pc->port, spn_MACSEC_ENABLE, 0) ) {
            return SOC_E_NOT_FOUND;
        }
#endif
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_FORCED_COPPER);
        PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_FIBER);
    break;

    case  PHY_OUI_MODEL(PHY_BCM84856_OUI, PHY_BCM84856_MODEL) :
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_FORCED_COPPER);
        PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_FIBER);
        pc->dev_name = "BCM84856";
    break;
    case  PHY_OUI_MODEL(PHY_BCM84858_OUI, PHY_BCM84858_MODEL) :
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_FORCED_COPPER);
        PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_FIBER);
        pc->dev_name = "BCM84858";
    break;

    case  PHY_OUI_MODEL(PHY_BCM84860_OUI, PHY_BCM84860_MODEL) |
          PHY_OUI_MODEL(PHY_BCM84861_OUI, PHY_BCM84861_MODEL) :
        if ( PHY_REV(id0, id1) == 0x0)  {
            pc->dev_name = "BCM84861";
        }
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_FORCED_COPPER);
        PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_FIBER);
    break;

    case  PHY_OUI_MODEL(PHY_BCM84864_OUI, PHY_BCM84864_MODEL) |
          PHY_OUI_MODEL(PHY_BCM84868_OUI, PHY_BCM84868_MODEL) :
        if ( PHY_REV(id0, id1) == 0x0)  {
            pc->dev_name = "BCM84868";
        }
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_FORCED_COPPER);
        PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_FIBER);
    break;
    
    case  PHY_OUI_MODEL(PHY_BCM84888_OUI, PHY_BCM84888_MODEL) |
          PHY_OUI_MODEL(PHY_BCM84887_OUI, PHY_BCM84887_MODEL) |
          PHY_OUI_MODEL(PHY_BCM84884_OUI, PHY_BCM84884_MODEL) :
        if ( (PHY_REV(id0, id1) == 0x0) || (PHY_REV(id0, id1) == 0x1) )  {
            pc->dev_name = "BCM84888";
        } else
        if ( (PHY_REV(id0, id1) == 0x8) || (PHY_REV(id0, id1) == 0x9) )  {
            pc->dev_name = "BCM84884";
        } else
        if ( (PHY_REV(id0, id1) == 0x4) || (PHY_REV(id0, id1) == 0x5) )  {
            pc->dev_name = "BCM84887";
        }
        PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_FIBER);
    break;

    case  PHY_OUI_MODEL(PHY_BCM84888E_OUI, PHY_BCM84888E_MODEL) |
          PHY_OUI_MODEL(PHY_BCM84884E_OUI, PHY_BCM84884E_MODEL) :
        if ( (PHY_REV(id0, id1) == 0x0) || (PHY_REV(id0, id1) == 0x1) )  {
            pc->dev_name = "BCM84888E";
        } else
        if ( (PHY_REV(id0, id1) == 0x8) || (PHY_REV(id0, id1) == 0x9) )  {
            pc->dev_name = "BCM84884E";
        }
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_FORCED_COPPER);
        PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_FIBER);
    break;       

    case  PHY_OUI_MODEL(PHY_BCM84881_OUI, PHY_BCM84881_MODEL) |
          PHY_OUI_MODEL(PHY_BCM84880_OUI, PHY_BCM84880_MODEL) :
        if ( (PHY_REV(id0, id1) == 0x0) || (PHY_REV(id0, id1) == 0x1) )  {
            pc->dev_name = "BCM84881";
        } else
        if ( (PHY_REV(id0, id1) == 0x8) || (PHY_REV(id0, id1) == 0x9) )  {
            pc->dev_name = "BCM84880";
        }
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_FORCED_COPPER);
        PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_FIBER);
    break;

    case  PHY_OUI_MODEL(PHY_BCM84888S_OUI, PHY_BCM84888S_MODEL) |
          PHY_OUI_MODEL(PHY_BCM84886_OUI, PHY_BCM84886_MODEL) |
          PHY_OUI_MODEL(PHY_BCM84885_OUI, PHY_BCM84885_MODEL) :
        if ( (PHY_REV(id0, id1) == 0x0) || (PHY_REV(id0, id1) == 0x1) )  {
            pc->dev_name = "BCM84886";
        } else
        if ( (PHY_REV(id0, id1) == 0x8) || (PHY_REV(id0, id1) == 0x9) )  {
            pc->dev_name = "BCM84885";
        } else
        if ( (PHY_REV(id0, id1) == 0x4) || (PHY_REV(id0, id1) == 0x5) )  {
            pc->dev_name = "BCM84888S";
        }
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_FORCED_COPPER);
        PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_FIBER);
    break;

    case  PHY_OUI_MODEL(PHY_BCM84812CE_OUI, PHY_BCM84812CE_MODEL) :
    case  PHY_OUI_MODEL(PHY_BCM84821_OUI, PHY_BCM84821_MODEL) :

    break;

    default:
        return SOC_E_NOT_FOUND;

    }
    pc->size = sizeof(PHY8481_PRIV_t);

    return SOC_E_NONE;

}

/*
 * Function:
 *      phy_8481_speed_set
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
_phy_8481_copper_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    phy_ctrl_t  *int_pc;
    uint16       speed_c45 = 0, speed_c22 = 0, speed_mg = 0;
    int          lb_enable = 0, rv = SOC_E_NONE;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit, "phy_8481_speed_set: u=%d p=%d speed=%d\n"),
                                unit, port, speed));
    pc = EXT_PHY_SW_STATE(unit, port);
    /* coverity[dead_error_condition:FALSE] */
    int_pc = INT_PHY_SW_STATE(unit, port);
    switch ( speed ) {
        case 5000:
            speed_mg = MGBASE_AN_CTRL_MG_ENABLE | MGBASE_AN_CTRL_SPEED_5G;
            break;

        case 2500:
            speed_mg = MGBASE_AN_CTRL_MG_ENABLE | MGBASE_AN_CTRL_SPEED_2P5G;
            break;

        case 10000:
            speed_c45 = IEEE_CTRLr_SPEED_10G;
            speed_c22 = PMAD_MII_CTRL_SS_MSB | PMAD_MII_CTRL_SS_LSB;
            break;

        case 1000:
            speed_c45 = 0;
            speed_c22 = PMAD_MII_CTRL_SS_1000;
            break;

        case 100:
            speed_c45 = 0;
            speed_c22 = PMAD_MII_CTRL_SS_100;
            break;

        case 10:
            speed_c45 = 0;
            speed_c22 = PMAD_MII_CTRL_SS_10;
            break;

        default:
            return SOC_E_PARAM;

    }

    if ( PHY_IS_BCM8486X_AND_UP(pc) ) {   /* Mako support 5G/2.5G */
        /* MGBASE-T AN Control Reg.(30.0x0000), Speed_Select_5G/2.5G[6:5] */
        SOC_IF_ERROR_RETURN(
            MODIFY_PHY8481_MGBASET_AN_CTRLr(unit, pc, speed_mg,
                (MGBASE_AN_CTRL_SPEED_MASK | MGBASE_AN_CTRL_ADV_MASK)) );
    }

    /* XFI PMA/PMD Control 1 Reg. (1.0x0000), speed_10G[5:2] speed_sel[13,6] */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PMAD_CTRLr(unit, pc,  speed_c45 | speed_c22, 
                        IEEE_CTRLr_SPEED_10G_MASK | PMAD_MII_CTRL_SS_MASK ));

    if ( ! PHY_IS_BCM8486X_AND_UP(pc)) {
        /* Only allow forced 100M in case of no loopback and AN off */
        SOC_IF_ERROR_RETURN(_phy_8481_copper_lb_get(unit, port, &lb_enable));
        if(!lb_enable) {
             speed_c22 = PMAD_MII_CTRL_SS_100;
        }
    }

    if ( PHY_IS_BCM8483X_AND_UP(pc) ) {
        /* 1000/100BASE-T MII Control Reg. (7.0xFFE0), speed_select[13,6] */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_MII_CTRLr(unit, pc, speed_c22, PMAD_MII_CTRL_SS_MASK));
    } else {
        /* Sync the speed bits with the GPHY MII CTRL reg. */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_AN_REG(unit, pc, 0x8000, (1U<<13), (1U << 13)));
    }
    /* coverity[dead_error_condition:FALSE] */
    /* need to set the internal phy's speed accordingly */
    if ( NULL != int_pc ) {
        /* coverity[dead_error_begin] */
        rv = PHY_SPEED_SET(int_pc->pd, unit, port, speed);
    }
    LB_SPEED(pc) = speed;

    return rv;
}

STATIC int
_phy_8481_xaui_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_IS_BCM84887(pc) ) {
        uint16  args[1] = { (speed > 1000) ? PHY848XX_DIAG_CMD_FIBER_SPEED_10G
                                           : PHY848XX_DIAG_CMD_FIBER_SPEED_1G };
        SOC_IF_ERROR_RETURN(
                PHY84834_TOP_LEVEL_CMD_SET(unit, pc,
                            (uint16) PHY848XX_DIAG_CMD_SET_FIBER_SPEED, args, 1) );
        /* call phy_8481_link_up() to notify switch's SerDes of speed & intf_type ..., etc. */
        SOC_IF_ERROR_RETURN( phy_8481_link_up(unit, port) );
    } else {
        uint16  strap1 = 0;
        /* Select 1G Fiber */
        if ( speed == 1000 ) {
            strap1 = (1U << 12);
        }
        PHY_FW_HANDSHAKE(unit, port);
        SOC_IF_ERROR_RETURN(
                MODIFY_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc, strap1, (1U << 12)) );
        PHY_FW_HANDSHAKE(unit, port);
    }

    return (SOC_E_NONE);
}

STATIC int
phy_8481_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    
    if ( PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN( _phy_8481_copper_speed_set(unit, port, speed) );
        pc->copper.force_speed = speed;
    } else {
        PHYDRV_CALL_ARG1(pc,PHY_SPEED_SET,speed);
        SOC_IF_ERROR_RETURN( _phy_8481_xaui_speed_set(  unit, port, speed) );
        pc->fiber.force_speed = speed;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_speed_get
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
_phy_8481_copper_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t *pc;
    uint16 ctrl, status, speed_c45;
    int an = 0;
    int an_done = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_IS_BCM8484X_AND_UP(pc) ) {
        /* Core Config Status Register (30.0x400D), Copper_Speed[2,4:3] */
        SOC_IF_ERROR_RETURN(
            READ_PHY84834_CORE_CFG_LINK_STATUSr(unit, pc, &status) );
        
        if (PHY_IS_BCM8484X(pc)) {
            /* Do not include Status Register bit 2 for speed determination
             * on BCM8484X, because bit 2 is priority bit other than speed MSB.*/
            status &= ~CORE_CFG_LINK_STAT_SPEED_MSB;
        }

        speed_c45 =  (status & CORE_CFG_LINK_STAT_SPEED_MSB) |
                    ((status & CORE_CFG_LINK_STAT_SPEED_LSB)
                            >> CORE_CFG_LINK_STAT_SPEED_LSB_SHIFT);
    	switch ( speed_c45 ) {
    	case 1 :
            *speed = 100;     break;
    	case 2 :
            *speed = 1000;    break;
    	case 3 :
            *speed = 10000;   break;
    	case 4 :
            *speed = 2500;    break;
    	case 5 :
            *speed = 5000;    break;
    	default:
            *speed = 0;       break;
    	}
        if( *speed != 0) {
            return SOC_E_NONE;
        }
    }

    /* legacy way for determining current link-down speed */
    SOC_IF_ERROR_RETURN
        (phy_8481_an_get(unit, port, &an, &an_done));

    if ( an ) { /* autoneg is enabled */
        if ( an_done ) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_TENG_AN_CTRLr(unit, pc, &ctrl));
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_TENG_AN_STATr(unit, pc, &status));
            if ( (ctrl   & TENG_IEEE_AN_CTRL_10GBT   ) &&
                 (status & TENG_IEEE_AN_STAT_LP_10GBT) ) {
                *speed = 10000;
            } else {
                /* look at the CL22 regs and determine the gcd */
                SOC_IF_ERROR_RETURN
                    (_phy8481_auto_negotiate_gcd(unit, port, speed, NULL));
            }
        } else {
            *speed = 0;
            return(SOC_E_NONE);
        }

    } else {   /* autoNeg is not enabled, forced speed */
        /* read the MII Ctrl reg. */

        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_CTRLr(unit, pc, &speed_c45));
        switch(MII_CTRL_SS(speed_c45)) {
        case PMAD_MII_CTRL_SS_10:
            *speed = 10;
            break;
        case PMAD_MII_CTRL_SS_100:
            *speed = 100;
            break;
        case PMAD_MII_CTRL_SS_1000:
            *speed = 1000;
            break;
        default:
            if ( (speed_c45 & (IEEE_CTRLr_SPEED_10G_MASK|PMAD_MII_CTRL_SS_MASK))
                            == IEEE_CTRLr_SPEED_10G ) {
                *speed = 10000;
                break;
            } else {
                *speed = 0;
                return(SOC_E_NONE);
            }
        }
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_xaui_speed_get(int unit, soc_port_t port, int *speed)
{
    uint16      args[1] = { 0 };
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_IS_BCM84887(pc) ) {
        SOC_IF_ERROR_RETURN(
                PHY84834_TOP_LEVEL_CMD_GET(unit, pc,
                            (uint16) PHY848XX_DIAG_CMD_GET_FIBER_SPEED, args, 1) );
        *speed = (args[0] == PHY848XX_DIAG_CMD_FIBER_SPEED_1G) ? 1000 : 10000;
    } else {
        SOC_IF_ERROR_RETURN(
                READ_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc, args) );
        *speed = (args[0] & (1U << 12)) ? 1000 : 10000;
    }

    return (SOC_E_NONE);
}

STATIC int
phy_8481_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_speed_get(unit, port, speed));
    } else {
        if ( NXT_PC(pc)) {
            PHYDRV_CALL_ARG1(pc,PHY_SPEED_GET,speed);
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_speed_get(unit, port, speed));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_copper_duplex_set(int unit, soc_port_t port, int duplex)
{
    uint16      mii_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_CTRLr(unit, pc, &mii_ctrl));

    if ( duplex) {
        mii_ctrl |= MII_CTRL_FD;
    } else {
        mii_ctrl &= ~MII_CTRL_FD;
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MII_CTRLr(unit, pc, mii_ctrl));

    pc->copper.force_duplex = duplex;

    return(SOC_E_NONE);
}

STATIC int
_phy_8481_xaui_duplex_set(int unit, soc_port_t port, int duplex)
{
    uint16      mii_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_L_COMBO_IEEE0_MIICNTLr(unit, pc, &mii_ctrl));

    if ( duplex) {
        mii_ctrl |= MII_CTRL_FD;
    } else {
        mii_ctrl &= ~MII_CTRL_FD;
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PHYXS_L_COMBO_IEEE0_MIICNTLr(unit, pc, mii_ctrl));

    pc->fiber.force_duplex = duplex;

    return SOC_E_NONE;
}

STATIC int
phy_8481_duplex_set(int unit, soc_port_t port, int duplex)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_duplex_set(unit, port, duplex));
    } else {
        PHYDRV_CALL_ARG1(pc,PHY_DUPLEX_SET,duplex);
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_duplex_set(unit, port, duplex));
    }

    return SOC_E_NONE;
}


STATIC int
_phy_8481_copper_duplex_get(int unit, soc_port_t port, int *duplex)
{
    int         rv, speed, lbmode;
    uint16      mii_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    /* check if it's under PHY loopback mode ? */
    SOC_IF_ERROR_RETURN(_phy_8481_copper_lb_get(unit, port, &lbmode));
    /* if under loopback & lb_speed is set, use the intended loopback speed */
    if ( (! lbmode) || (0 == (speed = LB_SPEED(pc))) ) {  /* 0 means LB_SPEED not set */
        /* not loopback mode or don't know the intended speed for loopback, *\
        \* call _phy_8481_copper_speed_get() to get the current speed       */
        SOC_IF_ERROR_RETURN(_phy_8481_copper_speed_get(unit, port, &speed));
    }

    if ( ( speed == 10000 ) || ( speed == 0 ) ||
         ( speed == 5000  ) || ( speed == 2500 ) ) {
        *duplex =  TRUE;
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_CTRLr(unit, pc, &mii_ctrl));

    rv = SOC_E_NONE;
    if ( mii_ctrl & MII_CTRL_AE) {     /* Auto-negotiation enabled */
        if ( !(pc->mii_stat & MII_STAT_AN_DONE)) { /* Auto-neg NOT complete */
            *duplex = FALSE;
        } else {
            rv = _phy8481_auto_negotiate_gcd(unit, port, NULL, duplex);
        }
    } else {                /* Auto-negotiation disabled */
        *duplex = (mii_ctrl & MII_CTRL_FD) ? TRUE : FALSE;
    }

    return(rv);
}

STATIC int
_phy_8481_xaui_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint16       reg0_16;
    uint16       mii_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    *duplex = TRUE;

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_L_GP_STATUS_STATUS1000X1r(unit, pc, &reg0_16));

    if ( reg0_16 & GP_STATUS_STATUS1000X1_SGMII_MODE_MASK) {

    /* retrieve the duplex setting in SGMII mode */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PHYXS_L_COMBO_IEEE0_MIICNTLr(unit, pc, &mii_ctrl));

        if ( mii_ctrl & MII_CTRL_AE) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_PHYXS_L_COMBO_IEEE0_AUTONEGLPABILr(unit,pc,&reg0_16));

            /* make sure link partner is also in SGMII mode
             * otherwise fall through to use the FD bit in MII_CTRL reg
             */
            if ( reg0_16 & MII_ANP_SGMII_MODE) {
                if ( reg0_16 & MII_ANP_SGMII_FD) {
                    *duplex = TRUE;
                } else {
                    *duplex = FALSE;
                }

                return SOC_E_NONE;
            }
        }
        *duplex = (mii_ctrl & MII_CTRL_FD) ? TRUE : FALSE;
    }

    return SOC_E_NONE;
}

STATIC int
phy_8481_duplex_get(int unit, soc_port_t port, int *duplex)
{
    phy_ctrl_t  *pc;
    int          tmp = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_duplex_get(unit, port, duplex));
    } else {
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_duplex_get(unit, port, duplex));
        if ( NXT_PC(pc)) {
            tmp = *duplex;
            PHYDRV_CALL_ARG1(pc,PHY_DUPLEX_GET,duplex);
            *duplex = (tmp == TRUE && *duplex == TRUE) ? TRUE : FALSE;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_an_get
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
_phy_8481_copper_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy_ctrl_t *pc;
    uint16 mii_ctrl;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_STATr(unit, pc, &pc->mii_stat));

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_CTRLr(unit, pc, &mii_ctrl));

    /* Looking at the MII regs would suffice */
    *an = (mii_ctrl & MII_CTRL_AE) ? TRUE : FALSE;

    if (NULL != an_done) {
        *an_done = (pc->mii_stat & MII_STAT_AN_DONE) ? TRUE : FALSE;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_xaui_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *an      = FALSE;
    if (NULL != an_done) {
        *an_done = FALSE;
    }

    return (SOC_E_NONE);
}

STATIC int
phy_8481_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_an_get(unit, port, an, an_done));
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG2(pc,PHY_AUTO_NEGOTIATE_GET,an,an_done);
            return SOC_E_NONE;
        }

        SOC_IF_ERROR_RETURN(_phy_8481_xaui_an_get(unit, port, an, an_done));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_an_set
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
_phy_8481_copper_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;
    int pan = 0;
    int pan_done = 0;

    SOC_IF_ERROR_RETURN
        (phy_8481_an_get(unit, port, &pan, &pan_done));

    pc = EXT_PHY_SW_STATE(unit, port);

    if (pan != 0 && an == 0) {
        /* Workaround to avoid  1000BASET force mode which is causing    *\
        \* the PHY to misbehave  (0x2100 puts PHY in 100BT force mode )  */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MII_CTRLr(unit, pc, 0x2100));
    }

    /* 1000T/100TX MII Control Reg.(7.0xFFE0), AN_Enable[12] Restart_AN[9] */
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY8481_MII_CTRLr(unit, pc,
                                 (an ? (MII_CTRL_AE | MII_CTRL_RAN) : 0),
                                        MII_CTRL_AE | MII_CTRL_RAN) );
    /* 10GBASE-T AutoNeg Control Reg.(7.0x0000), AN_Enable[12] Restart_AN[9] */
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY8481_AN_CTRLr( unit, pc, 
                                 (an ? (AN_IEEE_CTRL_REG_AN_ENA_RESTART) : 0),
                                        AN_IEEE_CTRL_REG_AN_ENA_RESTART) );
    pc->copper.autoneg_enable = an;
    return SOC_E_NONE;
}

STATIC int
_phy_8481_xaui_an_set(int unit, soc_port_t port, int an)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(an);

    return SOC_E_NONE;
}

STATIC int
phy_8481_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc, *int_pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);

    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_an_set(unit, port, an));
    #if 1
        if ( SYS_FORCED_CL72(pc) )  {
            SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_SET(int_pc->pd, unit, port, an));
        }
    #endif
    } else {
        if ( NXT_PC(pc) ) {
            PHYDRV_CALL_ARG1(pc,PHY_AUTO_NEGOTIATE_SET,an);
            SOC_IF_ERROR_RETURN(_phy_8481_xaui_an_set(unit, port, FALSE));
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_an_set(unit, port, an));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_8481_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ability - (OUT) Port ability masks indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */
STATIC int
_phy_8481_copper_ability_advert_get(int unit, soc_port_t port,
                                    soc_port_ability_t *ability)
{
    phy_ctrl_t  *pc;
    uint16       data16;
    int          rv;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    sal_memset(ability, 0, sizeof(*ability));

    /* Copper AutoNeg Advertisement Reg.(7.0xFFE4) */
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_ANAr(unit, pc, &data16));
    if ( data16 & MII_ANA_HD_100 ) { 
        ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
    }
    if ( data16 & MII_ANA_FD_100 ) {
        ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
    }

    if ( ! PHY_IS_BCM8483X_AND_UP(pc) ) {   /* only 8481/8482x support 10M */
        if ( data16 & MII_ANA_HD_10 ) { 
            ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
        }
        if ( data16 & MII_ANA_FD_10 ) {
            ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
        }
    }

    switch ( data16 & (MII_ANA_PAUSE | MII_ANA_ASYM_PAUSE) ) {
    case MII_ANA_PAUSE:
        ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
        break;
    case MII_ANA_ASYM_PAUSE:
        ability->pause = SOC_PA_PAUSE_TX;
        break;
    case MII_ANA_PAUSE | MII_ANA_ASYM_PAUSE:
        ability->pause = SOC_PA_PAUSE_RX;
        break;
    }

    /* 1000BASE-T Control Reg.(7.0xFFE9) */
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_GB_CTRLr(unit, pc, &data16));
    /* GE Specific values */
    if ( data16 & MII_GB_CTRL_ADV_1000HD ) {
       ability->speed_half_duplex |= SOC_PA_SPEED_1000MB;
    }
    if ( data16 & MII_GB_CTRL_ADV_1000FD ) {
       ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
    }

    /* 10GBASE-T AN Control Reg.(7.0x0020) */
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_TENG_AN_CTRLr(unit, pc, &data16));
    /* 10G Specific values */
    if ( data16 & TENG_IEEE_AN_CTRL_10GBT ) {
       ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
    }

    if (PHY_IS_BCM8486X_AND_UP(pc)) {
        if ( PHY_IS_BCM8488X(pc) ) {
            /* check bit[8:7] of 10GBASE-T AN Control Reg.(7.0x0020) *\
            \* for 5GBASE-T / 2.5GBASE-T support                     */
            if ( data16 & TENG_IEEE_AN_CTRL_2p5GBT ) {
                ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
            }
            if ( data16 & TENG_IEEE_AN_CTRL_5GBT   ) {
                ability->speed_full_duplex |= SOC_PA_SPEED_5000MB;
            }
        }/* if PHY_IS_BCM8488X */

        /* Broadcom MGBASE-T  5G / 2.5G Specific values                        *\
        \* MGBASE-T AN Control Reg.(30.0x0000), Advert_5G/2.5G_capability[2:1] */
        SOC_IF_ERROR_RETURN(
            READ_PHY8481_MGBASET_AN_CTRLr(unit, pc, &data16) );
        if ( data16 & MGBASE_AN_CTRL_ADV_2P5G ) {
            ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
        }
        if ( data16 & MGBASE_AN_CTRL_ADV_5G ) {
            ability->speed_full_duplex |= SOC_PA_SPEED_5000MB;
        }
    }/* if PHY_IS_BCM8486X_AND_UP */

    /* EEE settings */
    if ( PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_EEE_ADVr(unit, pc, &data16));
        if ( data16 & 0x08 ) {
            ability->eee |= SOC_PA_EEE_10GB_BASET;
        }
        if ( data16 & 0x04 ) {
            ability->eee |= SOC_PA_EEE_1GB_BASET;
        }
        if ( data16 & 0x02 ) {
            ability->eee |= SOC_PA_EEE_100MB_BASETX;
        }
    }

    return rv;
}

STATIC int
_phy_8481_xaui_ability_advert_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t  *pc;
    uint16      an_adv;
    uint16      adv_over1g;

    pc = EXT_PHY_SW_STATE(unit, port);

    sal_memset(ability, 0, sizeof(*ability));

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_L_COMBO_IEEE0_AUTONEGADVr(unit, pc, &an_adv));

    switch (an_adv & (MII_ANP_C37_ASYM_PAUSE | MII_ANP_C37_PAUSE)) {
    case MII_ANP_C37_ASYM_PAUSE:
        ability->pause |= SOC_PA_PAUSE_TX;
        break;
    case (MII_ANP_C37_ASYM_PAUSE | MII_ANP_C37_PAUSE):
        ability->pause |= SOC_PA_PAUSE_RX;
        break;
    case MII_ANP_C37_PAUSE:
        ability->pause |= SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
        break;
    }

    ability->speed_full_duplex |= (an_adv & MII_ANP_C37_FD) ? SOC_PA_SPEED_1000MB : 0;

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_L_OVER1G_UP1r(unit, pc, &adv_over1g));
    ability->speed_full_duplex |= (adv_over1g & OVER1G_UP1_DATARATE_10GCX4_MASK) ?
                                  SOC_PA_SPEED_10GB : 0;

    return SOC_E_NONE;
}

STATIC int
phy_8481_ability_advert_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_ability_advert_get(unit, port, ability));
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG1(pc,PHY_ABILITY_ADVERT_GET,ability);
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_ability_advert_get(unit, port, ability));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_8481_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ability - Port ability masks indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */


STATIC int
_phy_8481_copper_ability_advert_set(int unit, soc_port_t port,
                                    soc_port_ability_t *ability)
{
    phy_ctrl_t  *pc;
    uint16       mii_adv, mii_gb_ctrl, mask;
    int          rv;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    mii_adv     = MII_ANA_ASF_802_3;
    
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_GB_CTRLr(unit, pc,  &mii_gb_ctrl));

    mii_gb_ctrl &= ~(MII_GB_CTRL_ADV_1000HD | MII_GB_CTRL_ADV_1000FD);
    if ( PHY_IS_BCM8484X(pc) ) {
        mii_gb_ctrl &= ~MII_GB_CTRL_PT;  /* DTE device (JIRA PHY-1772) */
    } else {
        mii_gb_ctrl |=  MII_GB_CTRL_PT;  /* repeater / switch port */
    }

    if ( ! PHY_IS_BCM8483X_AND_UP(pc) ) {   /* only 8481/8482x support 10M */
        if ( ability->speed_half_duplex & SOC_PA_SPEED_10MB ) {
            mii_adv |= MII_ANA_HD_10;
            LB_SPEED(pc) = 10;
        }
        if ( ability->speed_full_duplex & SOC_PA_SPEED_10MB  ) {
            mii_adv |= MII_ANA_FD_10;
            LB_SPEED(pc) = 10;
        }
    }

    if ( ability->speed_half_duplex & SOC_PA_SPEED_100MB  ) {
        mii_adv |= MII_ANA_HD_100;
        LB_SPEED(pc) = 100;
    }
    if ( ability->speed_full_duplex & SOC_PA_SPEED_100MB  ) {
        mii_adv |= MII_ANA_FD_100;
        LB_SPEED(pc) = 100;
    }
    if ( ability->speed_half_duplex & SOC_PA_SPEED_1000MB ) {
        mii_gb_ctrl |= MII_GB_CTRL_ADV_1000HD;
        LB_SPEED(pc) = 1000;
    }
    if ( ability->speed_full_duplex & SOC_PA_SPEED_1000MB ) {
        mii_gb_ctrl |= MII_GB_CTRL_ADV_1000FD;
        LB_SPEED(pc) = 1000;
    }
    if ( (ability->speed_half_duplex & SOC_PA_SPEED_2500MB) ||
         (ability->speed_full_duplex & SOC_PA_SPEED_2500MB) ) {
        LB_SPEED(pc) = 2500;
    }
    if ( (ability->speed_half_duplex & SOC_PA_SPEED_5000MB) ||
         (ability->speed_full_duplex & SOC_PA_SPEED_5000MB) ) {
        LB_SPEED(pc) = 5000;
    }
    if ( (ability->speed_half_duplex & SOC_PA_SPEED_10GB) ||
         (ability->speed_full_duplex & SOC_PA_SPEED_10GB) ) {
        LB_SPEED(pc) = 10000;
    }

    if ( (ability->speed_half_duplex & SOC_PA_SPEED_10MB) ||
         (ability->speed_full_duplex & SOC_PA_SPEED_10MB) ) {
        if ( ! PHY_IS_BCM8483X_AND_UP(pc) ) {  /* only 8481/8482x support 10M */
            LB_SPEED(pc) = 10;
        }
    }

    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        mii_adv |= MII_ANA_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        mii_adv |= MII_ANA_PAUSE | MII_ANA_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        mii_adv |= MII_ANA_PAUSE;
        break;
    }

    mask = MII_ANA_PAUSE  | MII_ANA_ASYM_PAUSE |
           MII_ANA_FD_100 | MII_ANA_HD_100     | MII_ANA_ASF_802_3;
    if ( ! PHY_IS_BCM8483X_AND_UP(pc) ) {   /* only 8481/8482x support 10M */
        mask |=  (MII_ANA_FD_10 | MII_ANA_HD_10);
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_MII_ANAr(unit, pc, mii_adv, mask));

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_MII_GB_CTRLr(unit, pc, mii_gb_ctrl, 
             MII_GB_CTRL_PT | MII_GB_CTRL_ADV_1000FD | MII_GB_CTRL_ADV_1000HD));

    if ( PHY_IS_BCM8486X_AND_UP(pc) ) {
        /* 5G / 2.5G Specific values */
        mii_gb_ctrl  = (ability->speed_full_duplex & SOC_PA_SPEED_5000MB)
                     ? (MGBASE_AN_CTRL_ADV_5G   | MGBASE_AN_CTRL_MG_ENABLE) : 0;
        mii_gb_ctrl |= (ability->speed_full_duplex & SOC_PA_SPEED_2500MB)
                     ? (MGBASE_AN_CTRL_ADV_2P5G | MGBASE_AN_CTRL_MG_ENABLE) : 0;
        /* MGBASE-T AN Control Reg.(30.0x0000), Advert_5G/2.5G_capability[2:1] */
        SOC_IF_ERROR_RETURN(
            MODIFY_PHY8481_MGBASET_AN_CTRLr(unit, pc, mii_gb_ctrl,
                                                      MGBASE_AN_CTRL_ADV_MASK) );
    }

    /* 10G Specific values            ( 7.0x0020 bit[12] )  */
    mii_gb_ctrl      = (ability->speed_full_duplex & SOC_PA_SPEED_10GB)
                     ? TENG_IEEE_AN_CTRL_10GBT  : 0;
    if ( PHY_IS_BCM8488X(pc) ) {
        /* 5G / 2.5G Specific values  ( 7.0x0020 bit[8:7] ) */
        mii_gb_ctrl |= (ability->speed_full_duplex & SOC_PA_SPEED_5000MB)
                     ? TENG_IEEE_AN_CTRL_5GBT   : 0;
        mii_gb_ctrl |= (ability->speed_full_duplex & SOC_PA_SPEED_2500MB)
                     ? TENG_IEEE_AN_CTRL_2p5GBT : 0;
    }
    /* 10GBASE-T AN Control Reg.(7.0x0020), Advert_10G/5G/2.5G_capability */
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY8481_TENG_AN_CTRLr(unit, pc, mii_gb_ctrl,
                                     TENG_IEEE_AN_CTRL_10GBT |
                                     TENG_IEEE_AN_CTRL_5GBT  |
                                     TENG_IEEE_AN_CTRL_2p5GBT) );

    /* EEE settings */
    if ( PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
        uint16 eee_ability = 0;
        if (ability->eee & SOC_PA_EEE_10GB_BASET) {
            eee_ability |= 0x8;
        }
        if (ability->eee & SOC_PA_EEE_1GB_BASET) {
            eee_ability |= 0x4;
        }
        if (ability->eee & SOC_PA_EEE_100MB_BASETX) {
            eee_ability |= 0x2;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_EEE_ADVr(unit, pc, eee_ability, 0xe));
    }

    return rv;
}

STATIC int
_phy_8481_xaui_ability_advert_set(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t  *pc;
    uint16      an_adv;

    pc = EXT_PHY_SW_STATE(unit, port);

    /* Support only full duplex */
    an_adv = (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ? MII_ANP_C37_FD : 0;
    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        an_adv |= MII_ANP_C37_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        an_adv |= MII_ANP_C37_ASYM_PAUSE | MII_ANP_C37_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        an_adv |= MII_ANP_C37_PAUSE;
        break;
    }
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PHYXS_L_COMBO_IEEE0_AUTONEGADVr(unit, pc, an_adv));

    if (ability->speed_full_duplex & SOC_PA_SPEED_10GB) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PHYXS_L_OVER1G_UP1r(unit, pc, OVER1G_UP1_DATARATE_10GCX4_MASK));
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PHYXS_L_OVER1G_UP1r(unit, pc, 0));
    }

    return SOC_E_NONE;
}


STATIC int
phy_8481_ability_advert_set(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_ability_advert_set(unit, port, ability));
        pc->copper.advert_ability = *ability;
    } else {
        pc->fiber.advert_ability = *ability;
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG1(pc,PHY_ABILITY_ADVERT_SET,ability);
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_ability_advert_set(unit, port, ability));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_xaui_nxt_dev_probe(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    phy_ctrl_t phy_ctrl;
    soc_phy_info_t phy_info;
    extern int _ext_phy_probe(int unit, soc_port_t port,soc_phy_info_t *pi, 
                              phy_ctrl_t *ext_pc);

    pc = EXT_PHY_SW_STATE(unit, port);

    /* initialize the phy_ctrl for the next device connected to the xaui port */
    sal_memset(&phy_ctrl, 0, sizeof(phy_ctrl_t));
    phy_ctrl.unit = unit;
    phy_ctrl.port = port;
    phy_ctrl.speed_max = pc->speed_max;
    phy_ctrl.read = pc->read;    /* use same MDIO read routine as this device's */
    phy_ctrl.write = pc->write;  /* use same MDIO write routine as this device's */ 

    /* coverity[overrun-call] */
    /* get the mdio address of the next device */
    phy_ctrl.phy_id = soc_property_port_get(unit, port,
                                     spn_PORT_PHY_ADDR1, 
                        port < sizeof(ext_phy_dft_addr1)/sizeof(ext_phy_dft_addr1[0])?
                        ext_phy_dft_addr1[port]:0);

    /* probe the next device at the given address */
    SOC_IF_ERROR_RETURN
        (_ext_phy_probe(unit, port, &phy_info, &phy_ctrl));

    if ((phy_ctrl.pd == NULL) || ((phy_ctrl.phy_model == pc->phy_model) &&
                                  (phy_ctrl.phy_oui == pc->phy_oui))) {
        pc->driver_data = NULL;
        /* device not found */
        return SOC_E_NONE;
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_8481_xaui_nxt_dev_probe: found phy device"
                         " u=%d p=%d id0=0x%x id1=0x%x\n"), 
              unit, port,phy_ctrl.phy_id0,phy_ctrl.phy_id1));
    /* set the chained flag */
    PHY_FLAGS_SET(phy_ctrl.unit, phy_ctrl.port, PHY_FLAGS_CHAINED);

    /* Found device, allocate control structure */
    pc->driver_data = (void *)sal_alloc ((sizeof (phy_ctrl_t) + phy_ctrl.size), phy_ctrl.pd->drv_name);
    if (pc->driver_data == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memcpy(pc->driver_data, &phy_ctrl, sizeof(phy_ctrl_t));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_ability_remote_get
 * Purpose:
 *      Get the current remoteisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ability - (OUT) Port ability masks indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The remoteisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
_phy_8481_copper_ability_remote_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    uint16      mii_anp, mii_gb_stat, teng_an_stat;
    int rv;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    sal_memset(ability, 0, sizeof(*ability));

    if ( (pc->mii_stat & (MII_STAT_AN_DONE | MII_STAT_LA))
                      == (MII_STAT_AN_DONE | MII_STAT_LA)  ) {
        /* Decode remote advertisement only when link is up  *\
        \* and autoneg is completed.                         */

        /* Copper AutoNeg Link Partner ability (7.0xFFE5) */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MII_ANPr(unit, pc, &mii_anp));
        /* 1000BASE-T Status (7.0xFFEA) */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MII_GB_STATr(unit, pc, &mii_gb_stat));
        /* 10GBASE-T Status (7.0x0021), LP_10G_Capability[11] */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_TENG_AN_STATr(unit, pc, &teng_an_stat));

        /* 10M/100M Specific values */
        if (mii_anp & MII_ANA_HD_10) { 
            ability->speed_half_duplex  |= SOC_PA_SPEED_10MB;
        }
        if (mii_anp & MII_ANA_FD_10) {
            ability->speed_full_duplex  |= SOC_PA_SPEED_10MB;
        }
        if (mii_anp & MII_ANA_HD_100) { 
            ability->speed_half_duplex  |= SOC_PA_SPEED_100MB;
        }
        if (mii_anp & MII_ANA_FD_100) {
            ability->speed_full_duplex  |= SOC_PA_SPEED_100MB;
        }

        /* PAUSE abilities */
        ability->pause     = 0;
        switch (mii_anp & (MII_ANA_PAUSE | MII_ANA_ASYM_PAUSE)) {
            case MII_ANA_PAUSE:
                ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
                break;
            case MII_ANA_ASYM_PAUSE:
                ability->pause = SOC_PA_PAUSE_TX;
                break;
            case MII_ANA_PAUSE | MII_ANA_ASYM_PAUSE:
                ability->pause = SOC_PA_PAUSE_RX;
                break;
        }

        /* GE Specific values */
        if (mii_gb_stat & MII_GB_STAT_LP_1000HD) {
           ability->speed_half_duplex |= SOC_PA_SPEED_1000MB;
        }
        if (mii_gb_stat & MII_GB_STAT_LP_1000FD) {
           ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
        }

        /* 10G Specific values */
        if (teng_an_stat & TENG_IEEE_AN_STAT_LP_10GBT  ) {
           ability->speed_full_duplex |= SOC_PA_SPEED_10GB  ;
        }
        if (teng_an_stat & TENG_IEEE_AN_STAT_LP_5GBT   ) {
           ability->speed_full_duplex |= SOC_PA_SPEED_5000MB;
        }
        if (teng_an_stat & TENG_IEEE_AN_STAT_LP_2p5GBT ) {
           ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
        }

        /* EEE abilities */
        if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            uint16 eee_ability;
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_EEE_LPABILr(unit, pc, &eee_ability));
            if (eee_ability & 0x08) {
                ability->eee |= SOC_PA_EEE_10GB_BASET;
            }
            if (eee_ability & 0x04) {
                ability->eee |= SOC_PA_EEE_1GB_BASET;
            }
            if (eee_ability & 0x02) {
                ability->eee |= SOC_PA_EEE_100MB_BASETX;
            }
        }
    } else {
        return (SOC_E_FAIL);        
    }

    return rv;
}

STATIC int
_phy_8481_xaui_ability_remote_get(int unit, soc_port_t port,
                               soc_port_ability_t *ability)
{
    uint16            mii_stat_combo;
    uint16            link_stat_gp;
    uint16            an_adv;
    soc_port_mode_t   mode;
    phy_ctrl_t       *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    sal_memset(ability, 0, sizeof(*ability));

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_L_COMBO_IEEE0_MIISTATr(unit, pc, &mii_stat_combo));
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_L_GP_STATUS_XGXSSTATUS3r(unit, pc, &link_stat_gp));
     
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "u=%d p=%d mii_stat_combo=%04x link_stat_gp=%04x\n"),
              unit, port, mii_stat_combo, link_stat_gp));


    mode = 0;
    if ((link_stat_gp & GP_STATUS_XGXSSTATUS3_LINK_MASK) &&
        (mii_stat_combo & MII_STAT_AN_DONE)) {
        /* Decode remote advertisement only when link is up and autoneg is
         * completed.
         */

        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PHYXS_L_GP_STATUS_LP_UP1r(unit, pc, &an_adv));

        mode |= (an_adv & OVER1G_LP_UP1_DATARATE_10GCX4_MASK) ?
                 SOC_PA_SPEED_10GB : 0;

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "u=%d p=%d over1G an_adv=%04x\n"),
                  unit, port, an_adv));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PHYXS_L_COMBO_IEEE0_AUTONEGLPABILr(unit, pc, &an_adv));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "u=%d p=%d combo an_adv=%04x\n"),
                  unit, port, an_adv));

        mode |= (an_adv & MII_ANP_C37_FD) ? SOC_PA_SPEED_1000MB : 0;
        ability->speed_full_duplex = mode;

        mode = 0; 
        switch (an_adv & (MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE)) {
            case MII_ANP_C37_PAUSE:
                mode |= SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
                break;
            case MII_ANP_C37_ASYM_PAUSE:
                mode |= SOC_PA_PAUSE_TX;
                break;
            case MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE:
                mode |= SOC_PA_PAUSE_RX;
                break;
        }
        ability->pause = mode;

        if (PHY_CLAUSE73_MODE(unit, port)) {
            
        }
    } else {
        return (SOC_E_FAIL);
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8481_xaui_ability_remote_get:unit=%d p=%d pause=%08x sp=%08x\n"),
              unit, port, ability->pause, ability->speed_full_duplex));

    return (SOC_E_NONE);
}

STATIC int
phy_8481_ability_remote_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_ability_remote_get(unit, port, ability));
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG1(pc,PHY_ABILITY_REMOTE_GET,ability);
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_ability_remote_get(unit, port, ability));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_ability_local_get
 * Purpose:
 *      Get the device's complete abilities.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ability - return device's abilities.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_8481_copper_ability_local_get(int unit, soc_port_t port,
                                   soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_IS_BCM8483X_AND_UP(pc) ) {
        ability->speed_half_duplex  = SOC_PA_SPEED_100MB;
        ability->speed_full_duplex  = SOC_PA_SPEED_10GB  | SOC_PA_SPEED_1000MB |
                                      SOC_PA_SPEED_100MB;
    } else {
        ability->speed_half_duplex  = SOC_PA_SPEED_100MB | SOC_PA_SPEED_10MB;
        ability->speed_full_duplex  = SOC_PA_SPEED_10GB  | SOC_PA_SPEED_1000MB |
                                      SOC_PA_SPEED_100MB | SOC_PA_SPEED_10MB;
    }

    ability->interface = SOC_PA_INTF_XGMII;
    if ( PHY_IS_BCM8486X_AND_UP(pc) ) {    /* Mako supports 5G/2.5G speed     */
        if ( PHY_REPEATER(unit, port) ) {  /* check MAC interface mode */
            /* 5G/2.5G over XFI mode (XGMII) support both speed */
            ability->speed_full_duplex |= (SOC_PA_SPEED_5000MB |
                                           SOC_PA_SPEED_2500MB);
        } else {
            /* 2500BASE-X mode (SGMII) supports 2.5G only */
            ability->speed_full_duplex |= (SOC_PA_SPEED_2500MB | SOC_PA_SPEED_5000MB);
            ability->interface = SOC_PA_INTF_SGMII;

        }
    }

    if ( PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) &&
         PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_ENABLED) ) {
        ability->eee = SOC_PA_EEE_10GB_BASET | SOC_PA_EEE_1GB_BASET |
                       SOC_PA_EEE_100MB_BASETX;
    }

    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->medium    = SOC_PA_MEDIUM_COPPER;
    ability->loopback  = SOC_PA_LB_PHY;
    ability->flags     = SOC_PA_AUTONEG;

    return (SOC_E_NONE);
}

STATIC int
_phy_8481_xaui_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{

    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    ability->speed_full_duplex  = SOC_PA_SPEED_10GB | SOC_PA_SPEED_1000MB;

    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_XGMII;
    ability->medium    = SOC_PA_MEDIUM_XAUI;
    ability->loopback  = SOC_PA_LB_PHY;
    ability->flags     = SOC_PA_ABILITY_NONE;

    return (SOC_E_NONE);
}

STATIC int
phy_8481_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    sal_memset(ability, 0, sizeof(*ability));

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_ability_local_get(unit, port, ability));
    } else {
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_ability_local_get(unit, port, ability));
    }

    return (SOC_E_NONE);
}

STATIC int
phy_8481_link_up(int unit, soc_port_t port)
{
    int         lbmode, speed_line, speed_sys, an, an_done;
    phy_ctrl_t  *pc;
    phy_ctrl_t  *int_pc;

    int_pc = INT_PHY_SW_STATE(unit, port);
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_NO_SYS_LINE_SPD_SYNC);

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (phy_8481_an_get(unit,port,&an,&an_done));

    SOC_IF_ERROR_RETURN
        (_phy_8481_copper_lb_get(unit, port, &lbmode));
    /* if the intended loopback speed was set, use the LB_SPEED */
    if ( (! (lbmode) ) || (0 == (speed_line = LB_SPEED(pc))) ) { /* 0 means LB_SPEED not set */
        /* don't know the intended speed for loopback,        *\
        \* call phy_8481_speed_get() to get the current speed */
        SOC_IF_ERROR_RETURN( phy_8481_speed_get(unit, port, &speed_line) );
    }

    /* notify parameters to the system side */
    switch ( speed_line ) {

    case 10000:
    case 5000:
    case 2500:
        if ( ! LAST_IN_CHAIN(pc) ) {
            /* MAC side interface type -- XGMII: 5G/2.5G over XFI mode *\
            \*                         -- 2500X: 2500BASE-X mode       */
            SOC_IF_ERROR_RETURN(
                soc_phyctrl_notify(unit, port, phyEventInterface,
                            (PHY_REPEATER(unit, port)) ? SOC_PORT_IF_XFI  :
                                 ((speed_line == 2500) ? SOC_PORT_IF_2500X
                                                       : SOC_PORT_IF_5000X)) );
            /* if MAC side in 5G/2.5G over XFI mode, force it to 10G speed */
            if ( PHY_REPEATER(unit, port) ) {
                if ( 10000 != speed_line ) {
                    /* system side is in 5G/2.5G over XFI (idle stuffing) *\
                    |* mode when line side speed is 5G/2.5G, notify       *|
                    \* the system side of 10G speed and set this flag     */
                    PHY_FLAGS_SET(unit, port, PHY_FLAGS_NO_SYS_LINE_SPD_SYNC);
                }
                /* if MAC side in 5G/2.5G over XFI mode,  *\
                \* always notify system side of 10G speed */
                speed_sys = 10000;
            } else {
                /* not 5G/2.5G over XFI mode, system side speed is same as line side */
                speed_sys = speed_line;
            }
            /* notify internal SerDes of system side speed */
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, phyEventSpeed    , speed_sys));
            /* notify internal SerDes of line side speed */
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, phyEventSpeedLine, speed_line));
            /* notify internal SerDes of Resume event */
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, phyEventResume, 1));
        }
        break;

    case 1000:
        if ( ! LAST_IN_CHAIN(pc) ) {
            if ( PHY_COPPER_MODE(unit, port) || PHY_IS_BCM84887(pc) ) {
                /* for 84887 fiber/copper modes  or  other PHY's copper mode */
                SOC_IF_ERROR_RETURN
                    (soc_phyctrl_notify(unit, port, phyEventInterface, SOC_PORT_IF_SGMII));
                SOC_IF_ERROR_RETURN
                    (soc_phyctrl_notify(unit, port, phyEventSpeed    , 1000));
                SOC_IF_ERROR_RETURN
                    (soc_phyctrl_notify(unit, port, phyEventSpeedLine, 1000));
            } else {
                /* for the fiber mode of 10G PHYs other than 84887 */
                SOC_IF_ERROR_RETURN
                    (soc_phyctrl_notify(unit, port, phyEventInterface, SOC_PORT_IF_XAUI ));
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_MII_AUX_CTRLr(unit, pc, (1U << 14), (1U << 14)));
        }
        break;

    case 100:
    case 10:
        if ( ! LAST_IN_CHAIN(pc) ) {
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, phyEventInterface, SOC_PORT_IF_SGMII));
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, phyEventSpeed    , speed_line));
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, phyEventSpeedLine, speed_line));
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_MII_AUX_CTRLr(unit, pc, (1U << 14), (1U << 14)));
        break;

    default:
        break;
    }

    /* misc. settings */
    if ( PHY_IS_BCM84836(pc) || PHY_IS_BCM84846(pc) || PHY_IS_BCM84856(pc) ) {
        /* for fiber media */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PHYXS_M_REG(unit, pc, 0x8061, 0, (1U << 5)));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PHYXS_M_REG(unit, pc, 0x80ba, 0, (3U << 2)));
    }
    if ( PHY_IS_BCM84834(pc) || PHY_IS_BCM84836(pc) ||
         PHY_IS_BCM84844(pc) || PHY_IS_BCM84846(pc) || PHY_IS_BCM84848(pc) ) {
        /* XFI LPI pass-thru for native mode EEE */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PHYXS_M_REG(unit, pc, 0x8390, 0x007c));
    }
    #if 1
        if ( SYS_FORCED_CL72(pc) )  {
            SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_SET(int_pc->pd, unit, port, an));
        }
    #endif
   
    /* Workaround for PHY-1570 Runt pkts on 84844 ports */    
    if ( FLAGS(pc) & PHY8483X_ATOMIC_ACCESS_SHD ) {    
        if ( (speed_line == 100) && (an == FALSE) ) {
            _phy8485x_shadow_reg_write(unit, pc, REG_GROUP_SHADOW_MISC_REG, 0x0018, 0xffff, 0, 0x9c00);
            _phy8485x_shadow_reg_write(unit, pc, REG_GROUP_SHADOW_MISC_REG, 0x0019, 0xffff, 0, 0x1000);
            _phy8485x_shadow_reg_write(unit, pc, REG_GROUP_SHADOW_MISC_REG, 0x0018, 0xffff, 0, 0x0000);
            _phy8485x_shadow_reg_write(unit, pc, REG_GROUP_SHADOW_MISC_REG, 0x0019, 0xffff, 0, 0x0000);
        }
    }

    LINK_DOWN(pc) = FALSE;
    return (SOC_E_NONE);
}

STATIC int
phy_8481_link_down(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    LB_SPEED(pc)  = 0;      /* clear the intended speed for loopback */
    LINK_DOWN(pc) = TRUE;   /* set link-down flag */
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_8481_mdix_set
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
phy_8481_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{
    phy_ctrl_t    *pc;
    int            speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (!PHY_COPPER_MODE(unit, port)) {
            return SOC_E_NONE;
    }

    switch (mode) {
    case SOC_PORT_MDIX_AUTO:
        /* Clear bit 14 for automatic MDI crossover */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PHY_ECRr(unit, pc, 0, 0x4000));

        /*
         * Write the result in the register 0x18, shadow copy 7
         */
        /* Clear bit 9 to disable forced auto MDI xover */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_MII_MISC_CTRLr(unit, pc, 0, 0x0200));
        break;

    case SOC_PORT_MDIX_FORCE_AUTO:
        /* Clear bit 14 for automatic MDI crossover */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PHY_ECRr(unit, pc, 0, 0x4000));

        /*
         * Write the result in the register 0x18, shadow copy 7
         */
        /* Set bit 9 to force automatic MDI crossover */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_MII_MISC_CTRLr(unit, pc, 0x0200, 0x0200));
        break;

    case SOC_PORT_MDIX_NORMAL:
        SOC_IF_ERROR_RETURN(phy_8481_speed_get(unit, port, &speed));
        if (speed == 0 || speed == 10 || speed == 100) {
            /* Set bit 14 for manual MDI crossover */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PHY_ECRr(unit, pc, 0x4000, 0x4000));

            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_TEST1r(unit, pc, 0));
        } else {
            return SOC_E_UNAVAIL;
        }
        break;

    case SOC_PORT_MDIX_XOVER:
        SOC_IF_ERROR_RETURN(phy_8481_speed_get(unit, port, &speed));
        if (speed == 0 || speed == 10 || speed == 100) {
             /* Set bit 14 for manual MDI crossover */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PHY_ECRr(unit, pc, 0x4000, 0x4000));

            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_TEST1r(unit, pc, 0x0080));
        } else {
            return SOC_E_UNAVAIL;
        }
        break;

    default:
        return SOC_E_PARAM;
    }

    pc->copper.mdix = mode;
    return SOC_E_NONE;

}        

/*
 * Function:
 *      phy_8481_mdix_get
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
phy_8481_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    phy_ctrl_t    *pc;
    int            speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (!PHY_COPPER_MODE(unit, port)) {
        *mode = SOC_PORT_MDIX_NORMAL;
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(phy_8481_speed_get(unit, port, &speed));
    if (speed == 1000) {
       *mode = SOC_PORT_MDIX_AUTO;
    } else {
        *mode = pc->copper.mdix;
    }

    return SOC_E_NONE;
}    

/*
 * Function:
 *      phy_8481_mdix_status_get
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
phy_8481_mdix_status_get(int unit, soc_port_t port,
                         soc_port_mdix_status_t *status)
{
    phy_ctrl_t    *pc;
    uint16               tmp;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (!PHY_COPPER_MODE(unit, port)) {
        *status = SOC_PORT_MDIX_STATUS_NORMAL;
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHY_ESRr(unit, pc, &tmp));
    if (tmp & 0x2000) {
        *status = SOC_PORT_MDIX_STATUS_XOVER;
    } else {
        *status = SOC_PORT_MDIX_STATUS_NORMAL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_master_set
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
phy_8481_master_set(int unit, soc_port_t port, int master)
{
    uint16      mii_gb_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (!PHY_COPPER_MODE(unit, port)) {
            return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_GB_CTRLr(unit, pc, &mii_gb_ctrl));

    switch (master) {
    case SOC_PORT_MS_SLAVE:
        mii_gb_ctrl |=  MII_GB_CTRL_MS_MAN;
        mii_gb_ctrl &= ~MII_GB_CTRL_MS;
        break;
    case SOC_PORT_MS_MASTER:
        mii_gb_ctrl |= MII_GB_CTRL_MS_MAN;
        mii_gb_ctrl |= MII_GB_CTRL_MS;
        break;
    case SOC_PORT_MS_AUTO:
        mii_gb_ctrl &= ~MII_GB_CTRL_MS_MAN;
        break;
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MII_GB_CTRLr(unit, pc, mii_gb_ctrl));

    pc->copper.master = master;

    return(SOC_E_NONE);
}

/*
 * Function:
 *      phy_8481_master_get
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
phy_8481_master_get(int unit, soc_port_t port, int *master)
{
    uint16      mii_gb_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (!PHY_COPPER_MODE(unit, port)) {
        *master = SOC_PORT_MS_NONE;
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_GB_CTRLr(unit, pc, &mii_gb_ctrl));

    if (!(mii_gb_ctrl & MII_GB_CTRL_MS_MAN)) {
         *master = SOC_PORT_MS_AUTO;
    }
    else if (mii_gb_ctrl & MII_GB_CTRL_MS) {
        *master = SOC_PORT_MS_MASTER;
    } else {
        *master = SOC_PORT_MS_SLAVE;
    }

    return SOC_E_NONE;
}



STATIC int
_phy_8481_halt(int unit, phy_ctrl_t *pc)
{
    uint16 data16;

    /* LED control stuff */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa82c,0xffff));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa82d,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa82e,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa82f,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa830,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa831,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa832,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa833,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa834,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa835,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa836,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa837,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa838,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa839,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa83a,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa83b,0xb6db));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa83c,0xffff));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa83d,0x0000));
    
    if (PHY_IS_BCM8485X_AND_UP(pc)) {
        if ( PHY_EXT_ROM_BOOT(pc) ) {
            /* SPI ROM firmware download */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x418c, 0x0000));
        } else {
            if (PHY_IS_BCM8488X(pc)) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x418c, 0x0000));
            }
            /* MDIO    firmware download */
            data16 = (PHY_IS_BCM8486X_AND_UP(pc)) ? 0x48f0 : 0x0040;
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4188, data16));
        }
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x017c)); /* assert reset for the whole ARM system */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4186,0x8000)); /* enable global reset */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0040)); /* deassert reset for the whole ARM system 
                                                                 but the ARM processor */

    if (PHY_IS_BCM8488X(pc)) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4186,0x8000)); /* enable global reset */
    }
    /* write_addr=0xc300_0000, data=0x0000_001e, remove bottom write protection 
       and set VINITHI signal to 1 in order to have the ARM processor start executing 
       bootrom space 0xffff 0000 */

    if (!PHY_IS_BCM8485X_AND_UP(pc)) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, 0x0000));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, 0xc300));
    }
    else {
        if (PHY_IS_BCM8488X(pc)) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA819 , 0x0000)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81a , 0xc300)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81B , 0x0001));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81C , 0x0000)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA817 , 0x0009)); 
        } else {    
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA819 , 0x0000)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81a , 0xc300)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81B , 0x0010)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81C , 0x0000)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA817 , 0x0009)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA819 , 0x0000)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81a , 0xffff)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81B , 0x1018)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81C , 0xe59f)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA817 , 0x0009)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA819 , 0x0004)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81a , 0xffff)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81B , 0x1f11)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81C , 0xee09)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA817 , 0x0009)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA819 , 0x0008)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81a , 0xffff)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81B , 0x0000)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81C , 0xe3a0)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA817 , 0x0009)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA819 , 0x000c)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81a , 0xffff)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81B , 0x1806)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81C , 0xe3a0)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA817 , 0x0009)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA819 , 0x0010)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81a , 0xffff)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81B , 0x0002)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81C , 0xe8a0)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA817 , 0x0009)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA819 , 0x0014)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81a , 0xffff)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81B , 0x0001)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81C , 0xe150)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA817 , 0x0009)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA819 , 0x0018)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81a , 0xffff)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81B , 0xfffc)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81C , 0x3aff)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA817 , 0x0009)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA819 , 0x001c)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81a , 0xffff)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81B , 0xfffe)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81C , 0xeaff)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA817 , 0x0009)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA819 , 0x0020)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81a , 0xffff)); 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81B , 0x0021)); 
           SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA81C , 0x0004)); 
           SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA817 , 0x0009)); 
        }
    }
    if (!PHY_IS_BCM8488X(pc)) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc, &data16));
        if (data16 & (1U << 13)) {
            SOC_IF_ERROR_RETURN
               (WRITE_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, 0x001e));
        } else {
            /* Halt CPU if doing PMA/PMD reset. */ 
            if ( ! (PHY_IS_BCM8485X_AND_UP(pc)) ) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, 0x001f));
            }
        }
    }
    if ( ! (PHY_IS_BCM8485X_AND_UP(pc)) ) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, 0x0000));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 
                (2U << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT)|(1U << MDIO2ARM_CTL_MDIO2ARM_WR_SHIFT)));
    }

    /* write_addr=0xffff_0000, data=0xeaff_fffe, replace the first 32 bits of bootrom
       at 0xffff0000 with the instruction "B 0xffff0000" to loop the processor in the
       bootrom address space */
    if (!PHY_IS_BCM8485X_AND_UP(pc)) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, 0x0000));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, 0xffff));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, 0xfffe));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, 0xeaff));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 
                (2U << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT)|(1U << MDIO2ARM_CTL_MDIO2ARM_WR_SHIFT)));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0000)); /* deassert reset */ 

    return (SOC_E_NONE);
}

STATIC int
_phy_8481_prog_eeprom(int unit, phy_ctrl_t *pc)
{
    soc_timeout_t  to_prog;

    /* write_addr=0xc300_0000, data=0x0000_002c, enable bottom write protection 
       and set VINITHI signal to 0 */

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, 0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, 0xc300));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, 0x002c));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, 0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 
            (2U << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT)|(1U << MDIO2ARM_CTL_MDIO2ARM_WR_SHIFT)));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0040));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0000));

    soc_timeout_init(&to_prog, 10000000, 0);

    do {
        soc_timeout_t  to_status;
        int            rv;
        uint16 status, data_low, data_high;

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, 0x0010));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, 0xc300));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 
                (2U << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT)|(1U << MDIO2ARM_CTL_MDIO2ARM_RD_SHIFT)));

        soc_timeout_init(&to_status, 10000, 0);

        do {
            rv = READ_PHY8481_MDIO2ARM_STS_REG(unit, pc, &status);
            if (((status & MDIO2ARM_STS_MDIO2ARM_DONE_MASK) != 0) ||
                SOC_FAILURE(rv)) {
                break;
            }
        } while (!soc_timeout_check(&to_status));

        if ((status & MDIO2ARM_STS_MDIO2ARM_DONE_MASK) == 0) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_MDIO2ARM_CTL_REG( unit, pc, 0 )); 
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY8481 MDIO2ARM read failed: u=%d p=%d\n"), unit, pc->port ));
            return (SOC_E_FAIL);
        }

        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, &data_low));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, &data_high));

        if ( (data_low == 0x600d) || (data_high == 0x600d) ) {
            return (SOC_E_NONE);
        }

        if ( (data_low == 0xdead) || (data_high == 0xdead) ) {
            return (SOC_E_FAIL);
        }

      } while(!soc_timeout_check(&to_prog));

    LOG_WARN(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY8481 firmware programming timed out !.: u=%d p=%d\n"), unit, pc->port ));

    return (SOC_E_FAIL);
}

STATIC int
_phy_8481_restart(int unit, phy_ctrl_t *pc)
{
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PMAD_CTRLr(unit, pc, MII_CTRL_RESET, MII_CTRL_RESET));

    return (SOC_E_NONE);
}

#define WRITE_BLK_SIZE 4

/* This routine should be used only when the ARM CPU is halted/looping. */

STATIC int
_phy_8481_write_to_arm(int unit, phy_ctrl_t *pc, uint32 addr, uint8 *data,int len)
{
    int n_writes, i;

#ifdef PHY8481_MEASSURE_DOWNLOAD_TIME
    sal_usecs_t t;
    t = sal_time_usecs();
#endif

    n_writes = (len + WRITE_BLK_SIZE -1) / WRITE_BLK_SIZE;

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 
            (1U << MDIO2ARM_CTL_MDIO2ARM_SELF_INC_ADDR_SHIFT)
          | (1U << MDIO2ARM_CTL_MDIO2ARM_BURST_SHIFT)
          | (2U << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT)
          | (1U << MDIO2ARM_CTL_MDIO2ARM_WR_SHIFT)));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, ((addr>>16) & 0xffff)));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, (addr & 0xffff)));

    for ( i = 0; i < n_writes; i++ ) {

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, 
                ((*( data + WRITE_BLK_SIZE/2 + 1 ))<<8)|( *( data + WRITE_BLK_SIZE/2 ))));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, ((*( data + 1 ))<<8)|(*( data + 0 ))));

        data += WRITE_BLK_SIZE;

    }
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_CTL_REG( unit, pc, 0 )); 

#ifdef PHY8481_MEASSURE_DOWNLOAD_TIME
    LOG_CLI((BSL_META_U(unit,
                        "u=%d p=%d raw download took %u us\n"), unit, pc->port, SAL_USECS_SUB(sal_time_usecs(), t)));
#endif

    return (SOC_E_NONE);
}

#ifdef BCM848XX_STATE_MFG_DIAG_SUPPORT

#define READ_BLK_SIZE 4

STATIC int
_phy_8481_read_from_arm(int unit, phy_ctrl_t *pc, uint32 addr, uint8 *data, int len)
{
    soc_timeout_t  to;
    int            rv;
    int n_reads, i;
    uint16 data16_high, data16_low, status;

    n_reads = (len + READ_BLK_SIZE -1) / READ_BLK_SIZE;

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, (1U << MDIO2ARM_CTL_MDIO2ARM_SELF_INC_ADDR_SHIFT) |
                                                  (1U << MDIO2ARM_CTL_MDIO2ARM_BURST_SHIFT) |
                                                  (2U << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT) |
                                                  (1U << MDIO2ARM_CTL_MDIO2ARM_RD_SHIFT)));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, ((addr>>16) & 0xffff)));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, (addr & 0xffff)));

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, &data16_low)); /* dummy read */

    for ( i = 0; i < n_reads; i++ ) {

        soc_timeout_init(&to, 10000, 0);
        do {
            rv = READ_PHY8481_MDIO2ARM_STS_REG(unit, pc, &status);
            if (((status & MDIO2ARM_STS_MDIO2ARM_DONE_MASK) != 0) ||
                SOC_FAILURE(rv)) {
                break;
            }
        } while (!soc_timeout_check(&to));
        if ((status & MDIO2ARM_STS_MDIO2ARM_DONE_MASK) == 0) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY8481 MDIO2ARM read failed: u=%d p=%d addr=%08x\n"), unit, pc->port, addr));
            return (SOC_E_FAIL);
        }
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, &data16_high));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, &data16_low)); /* causes auto inc of addr */

        *(data++) = data16_low & 0xff;
        *(data++) = (data16_low>>8) & 0xff;
        *(data++) = data16_high & 0xff;
        *(data++) = (data16_high>>8) & 0xff;
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_CTL_REG( unit, pc, 0 )); 

    return (SOC_E_NONE);
}
#endif  /* BCM848XX_STATE_MFG_DIAG_SUPPORT */

/* This routine could be used while the ARM CPU is running. */

STATIC int
_phy_8481_mdio2arm_read(int unit, soc_port_t port, uint32 addr, uint32 *val, int access32)
{
    soc_timeout_t  to;
    int            rv;
    uint16 data_low = 0, data_high = 0, status;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, (addr & 0xffff)));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, ((addr>>16) & 0xffff)));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 
            ((access32 ? 2U : 1U) << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT) |
            (1U << MDIO2ARM_CTL_MDIO2ARM_RD_SHIFT)));

    soc_timeout_init(&to, 10000, 0);

    do {
        rv = READ_PHY8481_MDIO2ARM_STS_REG(unit, pc, &status);
        if (((status & MDIO2ARM_STS_MDIO2ARM_DONE_MASK) != 0) ||
            SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));
    if ((status & MDIO2ARM_STS_MDIO2ARM_DONE_MASK) == 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY8481 MDIO2ARM read failed: u=%d p=%d addr=%08x\n"), unit, port, addr));
        return (SOC_E_FAIL);
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, &data_low));

    if (access32) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, &data_high));
    }

    *val = (data_high<<16)|data_low; 

    return (SOC_E_NONE);

}

/* This routine could be used while the ARM CPU is running. */

STATIC int
_phy_8481_mdio2arm_write(int unit, soc_port_t port, uint32 addr, uint32 val, int access32)
{
    soc_timeout_t  to;
    int            rv;
    uint16 status;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, (addr & 0xffff)));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, ((addr>>16) & 0xffff)));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, (val & 0xffff)));

    if (access32) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, ((val>>16) & 0xffff)));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 
            ((access32 ? 2U : 1U) << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT)|
            (1U << MDIO2ARM_CTL_MDIO2ARM_WR_SHIFT)));

    soc_timeout_init(&to, 10000, 0);

    do {
        rv = READ_PHY8481_MDIO2ARM_STS_REG(unit, pc, &status);
        if (((status & MDIO2ARM_STS_MDIO2ARM_DONE_MASK) != 0) ||
            SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));
    if ((status & MDIO2ARM_STS_MDIO2ARM_DONE_MASK) == 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY8481 MDIO2ARM write failed: u=%d p=%d addr=%08x\n"), unit, port, addr));
        return (SOC_E_FAIL);
    }

    return (SOC_E_NONE);

}

int
_phy_8481_mdio2arm_read32(int unit, soc_port_t port, uint32 addr, uint32 *val)
{
    SOC_IF_ERROR_RETURN
        (_phy_8481_mdio2arm_read(unit, port, addr, val, TRUE));
    return (SOC_E_NONE);
}

int
_phy_8481_mdio2arm_write32(int unit, soc_port_t port, uint32 addr, uint32 val)
{
    SOC_IF_ERROR_RETURN
        (_phy_8481_mdio2arm_write(unit, port, addr, val, TRUE));
    return (SOC_E_NONE);
}

#ifdef PHY8481_ARM_MEM_DEBUG

int
_phy_8481_mdio2arm_read16(int unit, soc_port_t port, uint32 addr, uint16 *val)
{
    uint32 data;

    SOC_IF_ERROR_RETURN
        (_phy_8481_mdio2arm_read(unit, port, addr, &data, FALSE));
    *val = data;

    return (SOC_E_NONE);
}

int
_phy_8481_mdio2arm_write16(int unit, soc_port_t port, uint32 addr, uint16 val)
{
    SOC_IF_ERROR_RETURN
        (_phy_8481_mdio2arm_write(unit, port, addr, (uint32)val, FALSE));
    return (SOC_E_NONE);
}


int
_phy_8481_show_arm16(int unit, soc_port_t port, uint32 addr)
{
    uint16 data;

    SOC_IF_ERROR_RETURN
        (_phy_8481_mdio2arm_read16(unit, port, addr, &data));

    LOG_CLI((BSL_META_U(unit,
                        "Value = %04x\n"), data)); 

    return (SOC_E_NONE);
}

int
_phy_8481_show_arm32(int unit, soc_port_t port, uint32 addr)
{
    uint32 data;

    SOC_IF_ERROR_RETURN
        (_phy_8481_mdio2arm_read32(unit, port, addr, &data));

    LOG_CLI((BSL_META_U(unit,
                        "Value = %08x\n"), data)); 

    return (SOC_E_NONE);
}

#endif

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
phy_8481_firmware_set(int unit, int port, int offset, uint8 *data,int len)
{
    phy_ctrl_t *pc;
    uint16 saved_phy_addr, data16;
    int dividend, divisor, rv = SOC_E_FAIL;
    uint16 temp1, temp2;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (data) {
        if ((!PHY_COPPER_MODE(unit, port)) && NXT_PC(pc)) {
            /* there is a PHY device connected to this XAUI port. Always program the
             * outmost PHY device. To program the next external device in the chain,
             * detach/remove the outmost phy device first
             */
            PHYDRV_CALL_ARG3(pc,PHY_FIRMWARE_SET,offset,data,len);
            return SOC_E_NONE;
        }
        LOG_CLI((BSL_META_U(unit,
                            "Step 1: Halting the system.\n")));
    
        SOC_IF_ERROR_RETURN
            (_phy_8481_halt(unit, pc));
    
        LOG_CLI((BSL_META_U(unit,
                            "Step 2: Uploading the SPI loader and data"
                            " into main memory.\n")));
    
        SOC_IF_ERROR_RETURN
            (_phy_8481_write_to_arm(unit, pc, (uint32) offset, data, len));
    
        LOG_CLI((BSL_META_U(unit,
                            "Step 3: Waiting for the Programming phase"
                            " to be completed.\n")));
    
        SOC_IF_ERROR_RETURN
            (_phy_8481_prog_eeprom(unit, pc));
    
        LOG_CLI((BSL_META_U(unit,
                            "Step 4: Restarting system.\n")));
    
        SOC_IF_ERROR_RETURN
            (_phy_8481_restart(unit, pc));
    } else {
        switch (offset) {
        case PHYCTRL_UCODE_BCST_SETUP:
            if ( ! PHY_IS_BCM8485X_AND_UP(pc) ) {
                /* Turn on broadcast mode */
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x411e,0xf003));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x410e,0x0401));
            }
            /* halt the system */
            SOC_IF_ERROR_RETURN( _phy_8481_halt(unit, pc) );
            break;
        case PHYCTRL_UCODE_BCST_uC_SETUP:
            break;
        case PHYCTRL_UCODE_BCST_ENABLE:
            if ( PHY_IS_BCM8485X_AND_UP(pc) ) {
                /* Turn on broadcast mode after halting system */
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4117, 0xf003));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4107, 0x0401));
            }
            break;
        case PHYCTRL_UCODE_BCST_LOAD:
            saved_phy_addr = pc->phy_id;
            pc->phy_id &= ~0x1f;
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_PMAD_REG(unit, pc, 0x2, &temp1));
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_PMAD_REG(unit, pc, 0x3, &temp2));
            if(temp1 != 0xffff) {
                if((temp1 != pc->phy_id0) || (temp2 != pc->phy_id1)) {
                    LOG_WARN(BSL_LS_SOC_PHY,
                             (BSL_META_U(unit,
                                         "Another PHY already exist on same boradcast MDIO address.\n")));
                }    
            }

            if (DL_DIVIDEND(pc) && DL_DIVISOR(pc)) {
                 rv = soc_cmic_rate_param_get(unit, &dividend, &divisor);
                if (SOC_SUCCESS(rv)) {
                    rv = soc_cmic_rate_param_set(unit, DL_DIVIDEND(pc), DL_DIVISOR(pc));
                }
            }

            SOC_IF_ERROR_RETURN
                (_phy_8481_write_to_arm(unit, pc, 0, FIRMWARE(pc), FIRMWARE_LEN(pc)));
            /* write_addr=0xc300_0000, data=0x0000_000c, enable bottom write protection 
               and set VINITHI signal to 0 (reset vector at 0x0000_0000) */

            data16 = (PHY_IS_BCM8485X_AND_UP(pc)) ? 0x0000 : 0x000c;
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 0x0000));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, 0x0000));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, 0xc300));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, data16));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, 0x0000));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc,
                    (2U << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT)|(1U << MDIO2ARM_CTL_MDIO2ARM_WR_SHIFT)));

            if (SOC_SUCCESS(rv)) {
                /* rv contains the return value of previous soc_cmic_rate_param_set() */
                SOC_IF_ERROR_RETURN
                    (soc_cmic_rate_param_set(unit, dividend, divisor));
            }
            pc->phy_id = saved_phy_addr;
            break;
        case PHYCTRL_UCODE_BCST_END:
            /* Turn off broadcast mode */
            if (PHY_IS_BCM8485X_AND_UP(pc)) {
                /* Before reset the processor, add OSF setting for all ports  */
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA817 , 0x0000));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA008 , 0x0000));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x8004,0x5555));
                if ( PHY_IS_BCM8488X(pc) ) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xA003 , 0x0000));
                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY8481_PMAD_REG(unit, pc, 0x0000 , 0x8000));
                } else {
                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY8481_PMAD_REG(unit, pc, 0x0000 , 0x8000));
                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4107,0x0000));
                }
            } else {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x410e,0x0000));
            }

            SOC_IF_ERROR_RETURN
                (READ_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc, &data16));
            if (data16 & (1U << 13)) {
                /* Now reset only the ARM core */
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0040));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0000));
            } else {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_PMAD_CTRLr(unit, pc,
                                          MII_CTRL_RESET, MII_CTRL_RESET));
            }

            if (PHY_IS_BCM8484X_A0(pc)) {
                sal_usleep(300000);
            }
            soc_timeout_init(&TO(pc), 2000000, 0);
            break;
        default:
            break;
        }
    }

    return SOC_E_NONE;
}

STATIC int
_phy_84834_control_prbs_invert_data_set(int unit, soc_port_t port,
                                        int tx, int invert)
{
    uint16       data16;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

    data16 = (invert) ? PHY848XX_PRBS_INVERT_MASK : 0;
    if ( PHY_IS_BCM8488X_A0(pc) ) {
        /* Orca A0 workaround: must use 32-bit addressing mode for PRBS */
        uint32  reg32 = (tx) ? PHY848XX_TX_PRBS_ADDR32_REGISTER
                             : PHY848XX_RX_PRBS_ADDR32_REGISTER;
        SOC_IF_ERROR_RETURN(
            _phy848xx_addr32_reg_modify(unit, pc, reg32,
                                    data16, PHY848XX_PRBS_INVERT_MASK) );
    } else
    if ( PHY_IS_BCM8485X_AND_UP(pc) ) {
        uint32  reg   = (tx) ? PHY8485X_TX_PRBS_REGISTER
                             : PHY8485X_RX_PRBS_REGISTER;
        SOC_IF_ERROR_RETURN(
            MODIFY_PHY8481_PMAD_REG(unit, pc, reg,
                                    data16, PHY848XX_PRBS_INVERT_MASK) );
    }
    else {      /* 8483x / 8484x */
        uint16  mask16;
        if ( tx ) {
            data16 = (invert) ? PHY84834_USER_PRBS_TX_INVERT : 0;
            mask16 = PHY84834_USER_PRBS_TX_INVERT;
        } else {
            data16 = (invert) ? PHY84834_USER_PRBS_RX_INVERT : 0;
            mask16 = PHY84834_USER_PRBS_RX_INVERT;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PHYXS_M_REG(unit, pc,
                       PHY84834_USER_PRBS_CONTROL_0_REGISTER, data16, mask16) );
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84834_control_prbs_polynomial_set(int unit, soc_port_t port,
                                       int poly_ctrl, int tx)   /* tx or rx */
{
    phy_ctrl_t    *pc;       /* PHY software state */
    uint16 data16;
    uint16 mask16;

        /*
         * poly_ctrl:
         *  0x0 = prbs7
         *  0x1 = prbs15
         *  0x2 = prbs23
         *  0x3 = prbs31
         */
    pc = EXT_PHY_SW_STATE(unit, port);

    /* 001 : prbs7 
       010 : prbs9
       011 : prbs11
       100 : prbs15
       101 : prbs23
       110 : prbs31
     */

    if ( PHY_IS_BCM8485X_AND_UP(pc) ) {
        if ( PHY_IS_BCM8488X(pc) ) {
            /* Turn on MDC clock for XFI */
            SOC_IF_ERROR_RETURN(
                MODIFY_PHY8481_TOPLVL1_REG(unit, pc,
                                PHY8488X_MDIO_CLOCK_CONTROL_REGISTER,
                                PHY8488X_EN_CLK_MDC_XFI_MASK,
                                PHY8488X_EN_CLK_MDC_XFI_MASK) );
        }

        /* Change XGP table to point to XFI */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_TOPLVL1_REG(unit, pc,
                PHY8485X_MDIO_CONTROL_REGISTER, 0x2004));

        if ( (poly_ctrl >= 0) && (poly_ctrl <= 6) ) {
            data16 = poly_ctrl << 1;
        } else {
            return SOC_E_PARAM;     /* 8485x/6x/8x support polynominal 0-6 */
        }

        if ( PHY_IS_BCM8488X_A0(pc) ) {
            /* Orca A0 workaround: must use 32-bit addressing mode for PRBS */
            uint32 reg32 = (tx) ? PHY848XX_TX_PRBS_ADDR32_REGISTER
                                : PHY848XX_RX_PRBS_ADDR32_REGISTER;
            SOC_IF_ERROR_RETURN(
                _phy848xx_addr32_reg_write(unit, pc, reg32, data16) );
        } else {
            uint32  reg = (tx) ? PHY8485X_TX_PRBS_REGISTER
                               : PHY8485X_RX_PRBS_REGISTER;
            SOC_IF_ERROR_RETURN(
                WRITE_PHY8481_PMAD_REG(unit, pc, reg, data16) );
        }
    }
    else {     /* 8483x / 8484x */
        if (poly_ctrl == 0) {
            data16 = 1;
        } else if (poly_ctrl == 1) {
            data16 = 4;
        } else if (poly_ctrl == 2) {
            data16 = 5;
        } else if (poly_ctrl == 3) {
            data16 = 6;
        } else {
            return SOC_E_PARAM;
        } 
        mask16 = PHY84834_USER_PRBS_TYPE_MASK;

        /* default is tx */
        if (!tx) {  /* rx */
            data16 = data16 << 12;
            mask16 = mask16 << 12;
        }

        /* both tx/rx types */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PHYXS_M_REG(unit, pc,
                PHY84834_USER_PRBS_CONTROL_0_REGISTER,
                    data16, mask16));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_84834_control_prbs_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_IS_BCM8485X_AND_UP(pc) ) {
        /* clear PRBS control if disabled */
        if ( ! enable ) {       /* disable Tx/Rx PRBS */
            if ( PHY_IS_BCM8488X_A0(pc) ) {
                /* Orca A0 workaround: must use 32-bit addressing mode for PRBS */
                SOC_IF_ERROR_RETURN(
                    _phy848xx_addr32_reg_modify(unit, pc,
                                PHY848XX_TX_PRBS_ADDR32_REGISTER,
                                0,  PHY848XX_PRBS_ENABLE_MASK) );
                SOC_IF_ERROR_RETURN(
                    _phy848xx_addr32_reg_modify(unit, pc,
                                PHY848XX_RX_PRBS_ADDR32_REGISTER,
                                0,  PHY848XX_PRBS_ENABLE_MASK) );
            } else {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_PMAD_REG(unit, pc,
                        PHY8485X_TX_PRBS_REGISTER, 0, PHY8485X_USER_PRBS_TYPE_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_PMAD_REG(unit, pc,
                        PHY8485X_RX_PRBS_REGISTER, 0, PHY8485X_USER_PRBS_TYPE_MASK));
            }
            /* Restore XGP table to point to XFI */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_TOPLVL1_REG(unit, pc,
                    PHY8485X_MDIO_CONTROL_REGISTER, 0x3));
            if ( PHY_IS_BCM8488X(pc) ) {
            /* Turn off MDC clock for XFI */
            SOC_IF_ERROR_RETURN(
                MODIFY_PHY8481_TOPLVL1_REG(unit, pc,
                                PHY8488X_MDIO_CLOCK_CONTROL_REGISTER,
                                0, PHY8488X_EN_CLK_MDC_XFI_MASK) );
            }
        } else {                /* enable PRBS generator/checker */
            if ( PHY_IS_BCM8488X(pc) ) {
            /* Turn on MDC clock for XFI */
            SOC_IF_ERROR_RETURN(
                MODIFY_PHY8481_TOPLVL1_REG(unit, pc,
                                PHY8488X_MDIO_CLOCK_CONTROL_REGISTER,
                                PHY8488X_EN_CLK_MDC_XFI_MASK,
                                PHY8488X_EN_CLK_MDC_XFI_MASK) );
            }
            /* Change XGP table to point to XFI */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_TOPLVL1_REG(unit, pc,
                    PHY8485X_MDIO_CONTROL_REGISTER, 0x2004));

            if ( PHY_IS_BCM8488X_A0(pc) ) {
                /* Orca A0 workaround: must use 32-bit addressing mode for PRBS */
                SOC_IF_ERROR_RETURN(
                    _phy848xx_addr32_reg_modify(unit, pc,
                                PHY848XX_TX_PRBS_ADDR32_REGISTER,
                                PHY848XX_PRBS_ENABLE_MASK,
                                PHY848XX_PRBS_ENABLE_MASK) );
                SOC_IF_ERROR_RETURN(
                    _phy848xx_addr32_reg_modify(unit, pc,
                                PHY848XX_RX_PRBS_ADDR32_REGISTER,
                                PHY848XX_PRBS_ENABLE_MASK,
                                PHY848XX_PRBS_ENABLE_MASK) );
            } else {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_PMAD_REG(unit, pc,
                        PHY8485X_TX_PRBS_REGISTER, 1, PHY8485X_USER_PRBS_TYPE_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8481_PMAD_REG(unit, pc,
                        PHY8485X_RX_PRBS_REGISTER, 1, PHY8485X_USER_PRBS_TYPE_MASK));
            }
        }
    } else {
        /* clear PRBS control if disabled */
        if (!enable) {
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PHYXS_M_REG(unit, pc, 
                    PHY84834_USER_PRBS_CONTROL_0_REGISTER, 0,
                    PHY84834_USER_PRBS_TYPE_MASK |
                    PHY84834_USER_PRBS_TYPE_MASK << 12)); /* rx type*/
        } 

        /*  program the PRBS enable */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PHYXS_M_REG(unit, pc, 
                PHY84834_USER_PRBS_CONTROL_0_REGISTER, 
                enable? PHY84834_USER_PRBS_ENABLE: 0,PHY84834_USER_PRBS_ENABLE));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_84834_control_prbs_enable_get(int unit, soc_port_t port, uint32 *value)
{
    uint16 data16;
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_IS_BCM8485X_AND_UP(pc)) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                PHY8485X_TX_PRBS_REGISTER, &data16));
        if (data16 & PHY8485X_USER_PRBS_TYPE_MASK) {
            *value = TRUE;
        } else {
            *value = FALSE;
        }
    } else {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PHYXS_M_REG(unit, pc, 
                PHY84834_USER_PRBS_CONTROL_0_REGISTER, &data16));
        if (data16 & PHY84834_USER_PRBS_ENABLE) {
            *value = TRUE;
        } else {
            *value = FALSE;
        }
    }

    return SOC_E_NONE;
}

STATIC int
_phy_84834_control_prbs_polynomial_get(int unit, soc_port_t port, uint32 *value)
{
    phy_ctrl_t    *pc;       /* PHY software state */
    uint16 poly_ctrl;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_IS_BCM8485X_AND_UP(pc)) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                PHY8485X_TX_PRBS_REGISTER, &poly_ctrl));
        *value = (poly_ctrl & 0xE) >> 1;
    } else {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PHYXS_M_REG(unit, pc, 
                PHY84834_USER_PRBS_CONTROL_0_REGISTER, &poly_ctrl));
        poly_ctrl &= PHY84834_USER_PRBS_TYPE_MASK;

        if (poly_ctrl == 1) {
            *value = 0;
        } else if (poly_ctrl == 4) {
            *value = 1;
        } else if (poly_ctrl == 5) {
            *value = 2;
        } else if (poly_ctrl == 6) {
            *value = 3;
        } else {
            *value = poly_ctrl;
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84834_control_prbs_tx_invert_data_get(int unit, soc_port_t port, uint32 *value)
{
    phy_ctrl_t    *pc;       /* PHY software state */
    uint16 data16;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_IS_BCM8485X_AND_UP(pc)) {
        *value = FALSE;
        return SOC_E_UNAVAIL;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_M_REG(unit, pc, 
                  PHY84834_USER_PRBS_CONTROL_0_REGISTER, &data16));
    if (data16 & PHY84834_USER_PRBS_TX_INVERT) {
        *value = TRUE;
    } else {
        *value = FALSE;
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84834_control_prbs_rx_status_get(int unit, soc_port_t port, uint32 *value)
{
    phy_ctrl_t    *pc;       /* PHY software state */
    uint16        data16;
    uint16        err_cnt_low, err_cnt_high;
    uint32        err_cnt = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_IS_BCM8485X_AND_UP(pc)) {
        /* Read register 0xD0D9 twice */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                PHY8485X_PRBS_STATUS_REGISTER, &data16));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                PHY8485X_PRBS_STATUS_REGISTER, &data16));

        /* Read error count register twice */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                PHY8485X_PRBS_ERR_COUNT_LSB_REGSTER, &err_cnt_low));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                PHY8485X_PRBS_ERR_COUNT_LSB_REGSTER, &err_cnt_low));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                PHY8485X_PRBS_ERR_COUNT_MSB_REGSTER, &err_cnt_high));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                PHY8485X_PRBS_ERR_COUNT_MSB_REGSTER, &err_cnt_high));

        err_cnt = (err_cnt_high << 16) |  err_cnt_low;

        if ((!err_cnt) && (data16 & 0x1)) {
            *value = 0;
        } else if (err_cnt) {
            *value = err_cnt;
        } else {
            *value = -1;
        }
    } else {
        *value = 0;
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PHYXS_M_REG(unit, pc, 
                                      PHY84834_USER_PRBS_STATUS_0_REGISTER, &data16));
        if (data16 == 0x8000) { /* PRBS lock and error free */
            *value = 0;
        } else if (!(data16 & 0x8000)) {
            *value = -1;
        } else {
            *value = data16 & 0x7fff;
        }
    }

    return SOC_E_NONE;
}


STATIC int
_phy_84834_remote_loopback_set(int unit, soc_port_t port,int intf,uint32 enable)
{
    phy_ctrl_t    *pc;       /* PHY software state */
    
    pc = EXT_PHY_SW_STATE(unit, port);

    if (intf == PHY_DIAG_INTF_SYS) {
        SOC_IF_ERROR_RETURN
               (MODIFY_PHY8481_PHYXS_M_REG(unit,pc, 0x0000,
               enable ? (1<<14):0, (1<<14)));
    }
    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_84834_control_set
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
_phy_84834_control_set(int unit, soc_port_t port, int intf,int lane,
                     soc_phy_control_t type, uint32 value)
{
#if 0
    phy_ctrl_t    *pc;       /* PHY software state */
#endif
    int rv;

#if 0
    pc = EXT_PHY_SW_STATE(unit, port);
#endif

    PHY_CONTROL_TYPE_CHECK(type);

    rv = SOC_E_UNAVAIL;
#if 0
    if (intf == PHY_DIAG_INTF_SYS) {
        /* if is targeted to the system side */
        SOC_IF_ERROR_RETURN
            (PHY84740_XFI(unit,pc));
    }
#endif
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_8481_control_tx_driver_set(unit, port, type, value);
        break;
#if 0
    case SOC_PHY_CONTROL_EDC_MODE:
        rv = _phy_84740_control_edc_mode_set(unit,port,value);
        break;
    case SOC_PHY_CONTROL_CLOCK_ENABLE:
        rv = _phy_84740_control_recovery_clock_set(unit,port,value);
        break;
    case SOC_PHY_CONTROL_CLOCK_FREQUENCY:
        rv = _phy_84740_control_recovery_clock_freq_set(unit,port,value);
        break;
#endif
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        if (intf != PHY_DIAG_INTF_SYS) {
            /* PRBS line side not supported */
            rv = SOC_E_UNAVAIL;
        }
        else {
            /* on line side, rx poly type is auto-detected. On system side,
             * need to enable the rx type as well.
             */
            rv = _phy_84834_control_prbs_polynomial_set(unit, port, value, TRUE);
            if (intf == PHY_DIAG_INTF_SYS) {
                rv = _phy_84834_control_prbs_polynomial_set(unit, port,value,FALSE);
            }
        }
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        /* on line side, rx invertion is auto-detected. On system side,
         * need to enable the rx as well.
         */
        rv = _phy_84834_control_prbs_invert_data_set(unit, port, 2, value);
        if (intf == PHY_DIAG_INTF_SYS) {
            rv |= _phy_84834_control_prbs_invert_data_set(unit, port, 1, value);
        }
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        /* fall through */
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        /* tx/rx is enabled at the same time. no seperate control */
        rv = _phy_84834_control_prbs_enable_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        rv = _phy_84834_remote_loopback_set(unit, port,PHY_DIAG_INTF_SYS,value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
#if 0
    if (intf == PHY_DIAG_INTF_SYS) {
        /* if it is targeted to the system side, switch back */
        SOC_IF_ERROR_RETURN
            (PHY84740_MMF(unit,pc));
    }
#endif
    return rv;
}

/*
 * Function:
 *      phy_84834_control_get
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
_phy_84834_control_get(int unit, soc_port_t port, int intf,int lane,
                     soc_phy_control_t type, uint32 *value)
{
#if 0
    phy_ctrl_t    *pc;       /* PHY software state */
#endif
    int rv;
/*    uint16 data16; */

#if 0
    pc = EXT_PHY_SW_STATE(unit, port);
#endif

    PHY_CONTROL_TYPE_CHECK(type);

#if 0
    if (intf == PHY_DIAG_INTF_SYS) {
        /* targeted to the system side */
        SOC_IF_ERROR_RETURN
            (PHY84740_XFI(unit,pc));
    }
#endif
    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_8481_control_tx_driver_get(unit, port, type, value);
        break;
#if 0
    case SOC_PHY_CONTROL_EDC_MODE:
        rv = _phy_84740_control_edc_mode_get(unit,port,value);
        break;
    case SOC_PHY_CONTROL_CLOCK_ENABLE:
        SOC_IF_ERROR_RETURN
           (READ_PHY84740_MMF_PMA_PMD_REG(unit,pc,PHY84740_PMAD_RECOVERY_CLK_REG, 
                                          &data16));
        *value = (data16 & PHY84740_RECOVERY_CLK_ENABLE_MASK)? TRUE: FALSE; 
        rv = SOC_E_NONE;
        break;
#endif
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        if (intf == PHY_DIAG_INTF_SYS) {
            rv = _phy_84834_control_prbs_polynomial_get(unit, port, value);
        }
        else { /* PRBS line side not supported */
            rv = SOC_E_UNAVAIL;
        }
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = _phy_84834_control_prbs_tx_invert_data_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        /* fall through */
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = _phy_84834_control_prbs_enable_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
        rv = _phy_84834_control_prbs_rx_status_get(unit, port, value);
        break;
#if 0
    case SOC_PHY_CONTROL_CLOCK_FREQUENCY:
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit,pc,PHY84740_PMAD_RECOVERY_CLK_FREQ_REG, 
                   &data16));
        switch ((data16 >> PHY84740_RECOVERY_CLK_FREQ_SHIFT) & 0x3) {
            case 0x1:
                *value = 125000;
                break;
            case 0x2:
                *value = 195312;
                break;
            case 0x3:
                *value = 156250;
                break;
            default:
                *value = 0x0;
                break;
        }
        rv = SOC_E_NONE;
        break;
#endif

    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

#if 0
    if (intf == PHY_DIAG_INTF_SYS) {
        /* if it is targeted to the system side, switch back */
        SOC_IF_ERROR_RETURN
            (PHY84740_MMF(unit,pc));
    }
#endif
    return rv;
}


#ifdef BCM848XX_STATE_MFG_DIAG_SUPPORT

STATIC int
_phy_8481_mfg_test_fw_ready(int unit, phy_ctrl_t *pc, uint16 val, uint16 mask)
{
    int     rv;
    uint16  status;

    soc_timeout_init(&TO(pc), 25000000, 0);
    do {
        sal_usleep(50000);
        rv = READ_PHY8481_PHYC_CTL_SPARE_GRP5_REG(unit, pc, &status);
        if ( SOC_FAILURE(rv) ) {
            break;    /* failed to read register */
        }

        if ( (status & mask) == val ) {
            return SOC_E_NONE;      /* status check OK */
        }
    } while ( ! soc_timeout_check(&TO(pc)) );

    /* register read or status check failed */
    LOG_WARN(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY848X manufacturing diagnostics failed: unit=%d port=%d\n"),
              unit, pc->port));
    return SOC_E_FAIL;
}

STATIC int
_phy_8481_mfg_test_set(int unit, soc_port_t port, int op_cmd, int arg)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    if ( ! (FLAGS(pc) & PHY8483X_MFG_TEST_SUPPORT) ) {
        return SOC_E_UNAVAIL;
    }

    switch ( op_cmd ) {
    case PHY_DIAG_CTRL_MFG_HYB_CANC:
    case PHY_DIAG_CTRL_MFG_DENC:
        SOC_IF_ERROR_RETURN            /* MFG Test init */
            (WRITE_PHY8481_PHYC_CTL_SPARE_GRP5_REG(unit, pc, 0xf003));
        break;

    case PHY_DIAG_CTRL_MFG_TX_ON:
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PHYC_CTL_SPARE_GRP5_REG(unit, pc, (((arg & 0xf) << 12) | 0x3)));
        break;

    case PHY_DIAG_CTRL_MFG_EXIT:
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PHYC_CTL_SPARE_GRP5_REG(unit, pc, 0x7));
        return SOC_E_NONE;

    default:
        return SOC_E_UNAVAIL;
    }
    
    if(!(PHY_IS_BCM8485X_AND_UP(pc))) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMD_IEEE_TST_MODE_10G_REG(unit, pc, (0x5 << 13), (0x7 << 13)));
    } else {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMD_IEEE_TST_MODE_10G_REG(unit, pc, (0x1 << 13), (0x7 << 13)));
    }
    /* MFG Test init done. PHYC_CTL_SPARE_GRP5_REG should be reset to 0xF000 */
    SOC_IF_ERROR_RETURN
        (_phy_8481_mfg_test_fw_ready(unit, pc, 0xf000, 0xffff));

    LOG_WARN(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY848X manufacturing diagnostics init: unit=%d port=%d\n"), unit, port));

    return SOC_E_NONE;
}
#endif  /* BCM848XX_STATE_MFG_DIAG_SUPPORT */

#ifdef PHY8185X_DEBUG_CMD_HANDLER
STATIC int
_phy_8585x_generic_command_handler_test_get(int unit, phy_ctrl_t *pc)
{
    uint16 status;

    _phy8485x_emi_mode_enable_get(unit, pc, &status);
    _phy8485x_sub_lf_rf_status_get(unit, pc, &status);
    _phy8485x_snr_get(unit, pc);
    _phy8485x_current_temp_get(unit, pc);
    return SOC_E_NONE;
}

STATIC int
_phy_8585x_generic_command_handler_test_set(int unit, phy_ctrl_t *pc)
{
    uint16 status = 1;

    _phy8485x_emi_mode_enable_set(unit, pc, status);

    return SOC_E_NONE;
}
#endif

#ifdef BCM848XX_STATE_MFG_DIAG_SUPPORT
STATIC int
_phy_8481_mfg_test_get(int unit, soc_port_t port, int op_cmd, uint32 arg[])
{
    phy_ctrl_t *pc;
    uint16 status;
    int requested_cmd, data_len;

    pc = EXT_PHY_SW_STATE(unit, port);
    if ( ! (FLAGS(pc) & PHY8483X_MFG_TEST_SUPPORT) ) {
        return SOC_E_UNAVAIL;
    }

    switch ( op_cmd ) {
    case PHY_DIAG_CTRL_MFG_HYB_CANC:
       SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PHYC_CTL_SPARE_GRP5_REG(unit, pc, 0x1));
        break;
    case PHY_DIAG_CTRL_MFG_DENC:
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PHYC_CTL_SPARE_GRP5_REG(unit, pc, 0x2));
        break;

    case PHY_DIAG_CTRL_MFG_TX_ON:
        break;
    case PHY_DIAG_CTRL_MFG_EXIT:
        return SOC_E_NONE;
    default:
        return SOC_E_UNAVAIL;
    }

    /* MFG Test done. PHYC_CTL_SPARE_GRP5_REG[2:0] should be reset to 000 */
    SOC_IF_ERROR_RETURN
        (_phy_8481_mfg_test_fw_ready(unit, pc, 0x0, 0x0007));

    switch (op_cmd) {
    case PHY_DIAG_CTRL_MFG_HYB_CANC:
        requested_cmd = 0x1;
        data_len = 770*4;
        break;
    case PHY_DIAG_CTRL_MFG_DENC:
        requested_cmd = 0x2;
        data_len = 44*4;
        break;
    case PHY_DIAG_CTRL_MFG_TX_ON:
        requested_cmd = 0x3;
        data_len = 0;
        break;
    /*
     * COVERITY
     *
     * This default is unreachable. It is kept intentionally as a defensive default for future development.
     */
    /* coverity[dead_error_begin]  */
    default:
        return SOC_E_UNAVAIL;
    }

    /* ACK from FW, should be identical to MFG_CMD in PHYC_CTL_SPARE_GRP5[2:0] */
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYC_CTL_SPARE_GRP6_REG(unit, pc, &status));
    if ( (status & 0x7) != requested_cmd ) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY848X manufacturing diagnostics command mismatch: u=%d p=%d\n"), unit, port));
        return SOC_E_FAIL;
    }

    if ( data_len ) {
        uint32 addr;
        SOC_IF_ERROR_RETURN
            (_phy_8481_mdio2arm_read(unit, port, 0xc300003c, &addr, 1));
        SOC_IF_ERROR_RETURN
            (_phy_8481_read_from_arm(unit, pc, soc_ntohl_load(&addr), (uint8*)arg, data_len));
    }

    /* Turn off the transmission */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PHYC_CTL_SPARE_GRP5_REG(unit, pc, 0xf003));
    SOC_IF_ERROR_RETURN    /*  and wait for the operation done */
        (_phy_8481_mfg_test_fw_ready(unit, pc, 0xf000, 0xffff));

    LOG_WARN(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY848X manufacturing diagnostics OK: unit=%d port=%d\n"), unit, port));

    return SOC_E_NONE;
}

STATIC int
_phy_8481_diag_dsc_get(phy_ctrl_t *pc)
{
#ifdef BCM848XX_DIAG_DSC_SUPPORT
    if ( PHY_IS_BCM8488X(pc) ) {
        set_intpc(pc);
        get_dsc_param_orca();
    } else {
        set_pc(pc);
        get_dsc_param_koi();
    }
#else
    LOG_CLI((BSL_META_U(pc->unit, "*** Diag DSC not supported: "
                                  "unit=%d port=%d\n"), pc->unit, pc->port));
#endif

    return SOC_E_NONE;
}

STATIC int
_phy_8481_state_test_set(int unit, soc_port_t port, int op_cmd, int arg)
{
    phy_ctrl_t    *pc;
    uint16 status;
    int rv;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (!(FLAGS(pc) & PHY8483X_STATE_TEST_SUPPORT)) {
        return SOC_E_UNAVAIL;
    }
    switch (op_cmd) {
    case PHY_DIAG_CTRL_STATE_TRACE1:
        break;
    case PHY_DIAG_CTRL_STATE_TRACE2:
        break;
    case PHY_DIAG_CTRL_STATE_WHEREAMI:
    case PHY_DIAG_CTRL_STATE_TEMP:
        soc_timeout_init(&TO(pc), 7000000, 0);
        do {
            rv = READ_PHY84834_TOP_CONFIG_SCRATCH_26r(unit, pc, &status);
            if (PHY8481_IS_READY_FOR_CMD(pc, status) || SOC_FAILURE(rv)) {
                break;
            }
        } while (!soc_timeout_check(&TO(pc)));

        if (!PHY8481_IS_OPEN_FOR_CMD(pc, status)) {
            LOG_CLI((BSL_META_U(unit,
                                "_phy_8481_state_test_set not ready u=%d "
                                "p=%d status=%04x\n"),
                     unit, pc->port, status));
            return SOC_E_FAIL;
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84834_TOP_CONFIG_SCRATCH_0r(unit, pc, (op_cmd ==  PHY_DIAG_CTRL_STATE_WHEREAMI) ?
             PHY84834_DIAG_CMD_GET_SNR_V2 : PHY84834_DIAG_CMD_GET_CURRENT_TEMP_V2));

        soc_timeout_init(&TO(pc), 1000000, 0);
        break;
    case PHY_DIAG_CTRL_STATE_GENERIC:
#ifdef PHY8185X_DEBUG_CMD_HANDLER
        if (PHY_IS_BCM8485X_AND_UP(pc)) {
            _phy_8585x_generic_command_handler_test_set(unit, pc);
        } else {
            return SOC_E_NONE;
        }
#endif
        break;
    default:
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_state_test_get(int unit, soc_port_t port, int op_cmd, uint32 arg[])
{
    phy_ctrl_t *pc;
    uint32 addr = 0;
    int len = 0, rv;
    XGPHY_DIAG_DATA_t *data = (XGPHY_DIAG_DATA_t *) arg; 
    uint16 temp0, temp1, status;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (!(FLAGS(pc) & PHY8483X_STATE_TEST_SUPPORT)) {
        return SOC_E_UNAVAIL;
    }

    switch (op_cmd) {
    case PHY_DIAG_CTRL_STATE_TRACE1:
        addr = 0x50800;
        len = 2048*4;
        break;
    case PHY_DIAG_CTRL_STATE_TRACE2:
        addr = 0x53000;
        len = 1024*4;
        break;
    case PHY_DIAG_CTRL_STATE_WHEREAMI:
    case PHY_DIAG_CTRL_STATE_TEMP:
        do {
            rv = READ_PHY84834_TOP_CONFIG_SCRATCH_26r(unit, pc, &status);
            if (PHY8481_IS_CMD_COMPLETE(pc, status) || SOC_FAILURE(rv)) {
                break;
            }
        } while (!soc_timeout_check(&TO(pc)));
        if (PHY_IS_BCM8483X(pc) || PHY_IS_BCM8484X(pc)) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84834_TOP_CONFIG_SCRATCH_26r(unit, pc, PHY84834_CMD_CLEAR_COMPLETE));
        }
        if ((status & PHY84834_CMD_COMPLETE_PASS) == 0) {
            LOG_CLI((BSL_META_U(unit,
                                "_phy_8481_state_test_get failed: u=%d p=%d "
                                "status=%04x\n"), unit, pc->port, status));
            return SOC_E_FAIL;
        }
        break;
    case PHY_DIAG_CTRL_STATE_GENERIC:
#ifdef PHY8185X_DEBUG_CMD_HANDLER 
        if (PHY_IS_BCM8485X_AND_UP(pc)) {
            _phy_8585x_generic_command_handler_test_get(unit, pc);
        } else {
        return SOC_E_NONE;
        }
#endif
        break;
    default:
        return SOC_E_UNAVAIL;
    }

    if (op_cmd == PHY_DIAG_CTRL_STATE_WHEREAMI) {
        if (PHY_IS_BCM8485X_AND_UP(pc)) { 
            sal_memset(data->snr_block, 0, sizeof(data->snr_block));

            SOC_IF_ERROR_RETURN
                (READ_PHY84834_TOP_CONFIG_SCRATCH_28r(unit, pc, &temp1)); /* DATA2 */
            soc_htolel_store(&data->snr_block[16], temp1);
            soc_htolel_store(&data->snr_block[0], temp1);    /* SNR OK */

            SOC_IF_ERROR_RETURN
                (READ_PHY84834_TOP_CONFIG_SCRATCH_29r(unit, pc, &temp1)); /* DATA3 */
            soc_htolel_store(&data->snr_block[20], temp1);
            soc_htolel_store(&data->snr_block[4], temp1);    /* SNR OK */

            SOC_IF_ERROR_RETURN
                (READ_PHY84834_TOP_CONFIG_SCRATCH_30r(unit, pc, &temp1)); /* DATA4 */
            soc_htolel_store(&data->snr_block[24], temp1);
            soc_htolel_store(&data->snr_block[8], temp1);    /* SNR OK */

            SOC_IF_ERROR_RETURN
                (READ_PHY84834_TOP_CONFIG_SCRATCH_31r(unit, pc, &temp1)); /* DATA5 */
            soc_htolel_store(&data->snr_block[28], temp1);
            soc_htolel_store(&data->snr_block[12], temp1);    /* SNR OK */

            data->serr = data->cerr = data->block_lock = data->block_point_id = 0;
            data->flags = FLAGS(pc) & PHY8483X_FULL_SNR_SUPPORT ? PHY_DIAG_FULL_SNR_SUPPORT : 0;
        } else {
            /*
            SOC_IF_ERROR_RETURN
                (READ_PHY84834_TOP_CONFIG_SCRATCH_27r(unit, pc, &dr1));
             */
            SOC_IF_ERROR_RETURN
                (READ_PHY84834_TOP_CONFIG_SCRATCH_29r(unit, pc, &temp1)); /* DATA3 */
            SOC_IF_ERROR_RETURN
                (READ_PHY84834_TOP_CONFIG_SCRATCH_30r(unit, pc, &temp0)); /* DATA4 */

            SOC_IF_ERROR_RETURN
                (_phy_8481_read_from_arm(unit, pc, ((uint32)temp1) << 16 | temp0,
               data->snr_block, (FLAGS(pc) & PHY8483X_FULL_SNR_SUPPORT) ? sizeof(data->snr_block) : 16));

            temp1 = soc_letohl_load(&data->snr_block[16]) - 32768;
            soc_htolel_store(&data->snr_block[16], temp1);
            temp1 = soc_letohl_load(&data->snr_block[20]) - 32768;
            soc_htolel_store(&data->snr_block[20], temp1);
            temp1 = soc_letohl_load(&data->snr_block[24]) - 32768;
            soc_htolel_store(&data->snr_block[24], temp1);
            temp1 = soc_letohl_load(&data->snr_block[28]) - 32768;
            soc_htolel_store(&data->snr_block[28], temp1);

            SOC_IF_ERROR_RETURN
                (READ_PHY8481_PMAD_REG(unit, pc, 0xb089,&temp0));
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_PMAD_REG(unit, pc, 0xb08a,&temp1));
            data->serr = ((uint32)temp1) << 16 | temp0;

            SOC_IF_ERROR_RETURN
                (READ_PHY8481_PMAD_REG(unit, pc, 0xb08b,&temp0));
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_PMAD_REG(unit, pc, 0xb08c,&temp1));
            data->cerr = ((uint32)temp1) << 16 | temp0;

            SOC_IF_ERROR_RETURN
                (READ_PHY8481_TOPLVL1_REG(unit, pc, 0x405c,&temp0));
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_PCS_REG(unit, pc, 0x21, &temp1));
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_PCS_REG(unit, pc, 0x21, &temp1)); /* read twice to clear latch */
            data->block_point_id = temp0;
            data->block_lock = (temp1 >> 15) & 0x1;

            data->flags = FLAGS(pc) & PHY8483X_FULL_SNR_SUPPORT ? PHY_DIAG_FULL_SNR_SUPPORT : 0; 
        }

    } else if (op_cmd == PHY_DIAG_CTRL_STATE_TEMP) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84834_TOP_CONFIG_SCRATCH_27r(unit, pc, &data->digital_temp)); /* DATA1 */
        SOC_IF_ERROR_RETURN
            (READ_PHY84834_TOP_CONFIG_SCRATCH_28r(unit, pc, &data->analog_temp)); /* DATA2 */
    } else {
       SOC_IF_ERROR_RETURN
            (_phy_8481_read_from_arm(unit, pc, addr, (uint8 *)arg, len));
    }

    return SOC_E_NONE;
}
#endif  /* BCM848XX_STATE_MFG_DIAG_SUPPORT */

STATIC int
phy_8481_diag_ctrl(
   int unit, /* unit */
   soc_port_t port, /* port */
   uint32 inst, /* the specific device block the control action directs to */
   int op_type,  /* operation types: read,write or command sequence */
   int op_cmd,   /* command code */
   void *arg)     /* command argument  */
{
#ifdef BCM848XX_STATE_MFG_DIAG_SUPPORT
    int lane;
    int intf;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8481_diag_ctrl: u=%d p=%d ctrl=0x%x\n"),
                         unit, port,op_cmd));

    lane = PHY_DIAG_INST_LN(inst);
    intf = PHY_DIAG_INST_INTF(inst);
    if (intf == PHY_DIAG_INTF_DFLT) {
        intf = PHY_DIAG_INTF_LINE;
    }

    if (op_type == PHY_DIAG_CTRL_GET) {
        switch (op_cmd) {
        case PHY_DIAG_CTRL_MFG_HYB_CANC:
        case PHY_DIAG_CTRL_MFG_DENC:
        case PHY_DIAG_CTRL_MFG_TX_ON:
        case PHY_DIAG_CTRL_MFG_EXIT:
        SOC_IF_ERROR_RETURN
            (_phy_8481_mfg_test_get(unit, port,
                  op_cmd,(uint32 *)arg));
            break;
        case PHY_DIAG_CTRL_STATE_TRACE1:
        case PHY_DIAG_CTRL_STATE_TRACE2:
        case PHY_DIAG_CTRL_STATE_WHEREAMI:
        case PHY_DIAG_CTRL_STATE_TEMP:
        case PHY_DIAG_CTRL_STATE_GENERIC:
        SOC_IF_ERROR_RETURN
            (_phy_8481_state_test_get(unit, port,
                  op_cmd,(uint32 *)arg));
            break;
        default:
        SOC_IF_ERROR_RETURN
            (_phy_84834_control_get(unit, port, intf, lane,
                  op_cmd,(uint32 *)arg));
            break;
        }
    } else if (op_type == PHY_DIAG_CTRL_SET) {
        switch (op_cmd) {
        case PHY_DIAG_CTRL_MFG_HYB_CANC:
        case PHY_DIAG_CTRL_MFG_DENC:
        case PHY_DIAG_CTRL_MFG_TX_ON:
        case PHY_DIAG_CTRL_MFG_EXIT:
        SOC_IF_ERROR_RETURN
            (_phy_8481_mfg_test_set(unit, port,
                  op_cmd, PTR_TO_INT(arg)));
            break;
        case PHY_DIAG_CTRL_STATE_TRACE1:
        case PHY_DIAG_CTRL_STATE_TRACE2:
        case PHY_DIAG_CTRL_STATE_WHEREAMI:
        case PHY_DIAG_CTRL_STATE_TEMP:
        case PHY_DIAG_CTRL_STATE_GENERIC:
        SOC_IF_ERROR_RETURN
            (_phy_8481_state_test_set(unit, port,
                  op_cmd, PTR_TO_INT(arg)));
            break;
        default:
        SOC_IF_ERROR_RETURN
            (_phy_84834_control_set(unit, port, intf, lane,
                  op_cmd, PTR_TO_INT(arg)));
            break;
        }
    } else if (op_type == PHY_DIAG_CTRL_CMD) {
        switch (op_cmd) {
        case PHY_DIAG_CTRL_DSC:
            /* Diag DSC supported on system side of BCM8485X  */
            if (PHY_IS_BCM8485X_AND_UP(pc) && intf == PHY_DIAG_INTF_SYS) {
                SOC_IF_ERROR_RETURN
                     (_phy_8481_diag_dsc_get(pc));
            }
            else {
                return SOC_E_UNAVAIL;
            }
            break;
        }
    }
#endif  /* BCM848XX_STATE_MFG_DIAG_SUPPORT */
    return SOC_E_NONE;
}

STATIC int
_phy848xx_short_reach_enable_set(int unit,phy_ctrl_t *pc, uint16 enable )
{
    uint16 args[5];
    args[0] = enable;

    if (PHY_IS_BCM8488X(pc) && enable) {
        args[1] = enable;
    }
    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_SET(unit, pc, (uint16)PHY84834_DIAG_CMD_SET_SHORT_REACH_ENABLE_GENERIC, args, (PHY_IS_BCM8488X(pc) && enable) ? 2 : 1));
    
    return (SOC_E_NONE);
}

STATIC int
_phy848xx_short_reach_enable_get(int unit, phy_ctrl_t *pc, uint16 *enable, uint16 *fenable)
{
    uint16 args[5];
    *fenable = 0;

    if ( !PHY_IS_BCM8488X(pc) ) {
        SOC_IF_ERROR_RETURN
            (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)PHY84834_DIAG_CMD_GET_SHORT_REACH_ENABLE_GENERIC, args, 1));

        if (enable) {
            *enable = args[0];
        }
    }
    else { /* Orca  */
        SOC_IF_ERROR_RETURN
            (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)PHY84834_DIAG_CMD_GET_SHORT_REACH_ENABLE_GENERIC, args, 2));

        if (enable) {
            *enable = args[0];
        }

        if (args[0]) {
            *fenable = args[1];
        }
    }
    return (SOC_E_NONE);
}

STATIC int
_phy_84834_power_mode_set (int unit, soc_port_t port, uint32 mode)
{
    phy_ctrl_t    *pc;

    pc       = EXT_PHY_SW_STATE(unit, port);

    if (pc->power_mode == mode) {
        return SOC_E_NONE;
    }
    if (mode == SOC_PHY_CONTROL_POWER_LOW) {
        SOC_IF_ERROR_RETURN
            (_phy848xx_short_reach_enable_set(unit, pc,  1));
    } else if (mode == SOC_PHY_CONTROL_POWER_FULL) {
        SOC_IF_ERROR_RETURN
            (_phy848xx_short_reach_enable_set(unit, pc,  0));
    } else {
        return SOC_E_UNAVAIL;
    }
 
    pc->power_mode = mode;

    return SOC_E_NONE;
}

STATIC int
_phy_84834_power_mode_get (int unit, soc_port_t port, uint32 *mode)
{
    phy_ctrl_t    *pc;
    uint16       temp1=0, temp2=0;

    pc       = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy848xx_short_reach_enable_get(unit, pc, &temp1, &temp2));
    if (temp1) {
        if (PHY_IS_BCM8488X(pc)) {
            if (temp2) {
                *mode = SOC_PHY_CONTROL_POWER_LOW;
            } else {
                *mode = SOC_PHY_CONTROL_POWER_FULL;
            }
        } else { 
            *mode = SOC_PHY_CONTROL_POWER_LOW;
        }
    } else {
        *mode = SOC_PHY_CONTROL_POWER_FULL;
    }
    pc->power_mode = *mode;

    return SOC_E_NONE;
}

STATIC int
phy_8481_cable_diag(int unit, soc_port_t port,
                    soc_port_cable_diag_t *status)
{
    phy_ctrl_t    *pc;
    soc_timeout_t  to;
    int            i, rv, enable = 0, an = 0, an_done = 0, speed = 0;
    uint16         ctrl_status, result, length;


    pc = EXT_PHY_SW_STATE(unit, port);
    
    /* ECD not supported on disabled port for KOI and later PHYs  */
    if (PHY_IS_BCM8485X_AND_UP(pc)) {
        SOC_IF_ERROR_RETURN
            (phy_8481_enable_get(unit, port, &enable));
        if (!enable) {
           return SOC_E_UNAVAIL; 
        }

        SOC_IF_ERROR_RETURN
            (phy_8481_an_get(unit, port, &an, &an_done));
        SOC_IF_ERROR_RETURN
            (phy_8481_speed_get(unit, port, &speed));
        
        /* KOI does not support ECD for forced 100M  */
        if ( (speed == 100) && !(an) ) {
            return SOC_E_UNAVAIL;
        }
    }

    status->fuzz_len = 10;
    status->npairs = 4;
    status->state = SOC_PORT_CABLE_STATE_OK;

    /* run now, not at AN, enanle inter pair short check, don't break link, length in meters */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_TOP_CONFIG_CABLE_DIAG_CTRL_STAT_REG(unit, pc, (1U<<15)|(1U<<10), 
            (1U<<15)|(1U<<14)|(1U<<13)|(1U<<12)|(1U<<10)));

    sal_usleep(100);
    soc_timeout_init(&to, 50000000, 0);
    
    do {
        rv = READ_PHY8481_TOP_CONFIG_CABLE_DIAG_CTRL_STAT_REG(unit, pc, &ctrl_status);
        if (((ctrl_status & (1U<<11)) == 0) ||
            SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));

    if ((ctrl_status & (1U<<11)) != 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY848X Cable Diagnostics failed: u=%d p=%d\n"), unit, port ));
        return (SOC_E_FAIL);
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYC_CTL_CABLE_DIAG_RESULT_REG(unit, pc, &result));

    for( i=3; i>=0; i--) {

        switch (result & 0xf) {
        case 0x1:
            status->pair_state[i] = SOC_PORT_CABLE_STATE_OK;
            break;
        case 0x2:
            status->pair_state[i] = SOC_PORT_CABLE_STATE_OPEN;
            break;
        case 0x3:
            status->pair_state[i] = SOC_PORT_CABLE_STATE_SHORT;
            break;
        case 0x4:
            status->pair_state[i] = SOC_PORT_CABLE_STATE_CROSSTALK;
            break;
        default:
            status->pair_state[i] = SOC_PORT_CABLE_STATE_UNKNOWN;
            break;
        }

        if (status->pair_state[i] > status->state) {
            status->state = status->pair_state[i];
        }

        switch (i) {
        case 3:
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_PHYC_CTL_CABLE_DIAG_PAIR78_LEN_REG(unit, pc, &length));
            break;
        case 2:
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_PHYC_CTL_CABLE_DIAG_PAIR56_LEN_REG(unit, pc, &length));
            break;
        case 1:
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_PHYC_CTL_CABLE_DIAG_PAIR34_LEN_REG(unit, pc, &length));
            break;
        case 0:
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_PHYC_CTL_CABLE_DIAG_PAIR12_LEN_REG(unit, pc, &length));
            break;
        }
        status->pair_len[i] = length;
        result >>= 4;
    }

    return SOC_E_NONE;
}

STATIC int
_phy84834_timesync_enable_set(int unit,phy_ctrl_t *pc, uint16 enable )
{
    uint16 args[5];
    args[0] = enable;

    if ( enable ) {
        if ( ! (FLAGS(pc) & PHY8483X_TIMESYNC_ENABLED) ) {
            SOC_IF_ERROR_RETURN
                (PHY84834_TOP_LEVEL_CMD_SET(unit, pc,
                        (uint16) PHY84834_DIAG_CMD_SET_1588_ENABLE_GENERIC,
                        args, 1));
            FLAGS(pc) |= PHY8483X_TIMESYNC_ENABLED;
            /* PTP_PARSER_EG_CNTR_[ALL/PTP]_PKT_[EN/CLR] registers *\
            \* 1.0xda20,0xda30,0xda21,0xda31 ,  bit[0]             */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PMAD_REG(unit, pc,
                        PHY84834_PTP_PARSER_EG_CNTR_ALL_PKT_EN , 0x1, 0x1));
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PMAD_REG(unit, pc,
                        PHY84834_PTP_PARSER_EG_CNTR_PTP_PKT_EN , 0x1, 0x1));
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PMAD_REG(unit, pc,
                        PHY84834_PTP_PARSER_EG_CNTR_ALL_PKT_CLR, 0x1, 0x1));
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PMAD_REG(unit, pc,
                        PHY84834_PTP_PARSER_EG_CNTR_PTP_PKT_CLR, 0x1, 0x1));
            /* PTP_PARSER_IG_CNTR_[ALL/PTP]_PKT_[EN/CLR] registers *\
            \* 1.0xdaa0,0xdab0,0xdaa1,0xdab1 ,  bit[0]             */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PMAD_REG(unit, pc,
                        PHY84834_PTP_PARSER_IG_CNTR_ALL_PKT_EN , 0x1, 0x1));
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PMAD_REG(unit, pc,
                        PHY84834_PTP_PARSER_IG_CNTR_PTP_PKT_EN , 0x1, 0x1));
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PMAD_REG(unit, pc,
                        PHY84834_PTP_PARSER_IG_CNTR_ALL_PKT_CLR, 0x1, 0x1));
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PMAD_REG(unit, pc,
                        PHY84834_PTP_PARSER_IG_CNTR_PTP_PKT_CLR, 0x1, 0x1));
        }

    } else {      /* disable */
        if ((FLAGS(pc) & PHY8483X_TIMESYNC_ENABLED)) {
            SOC_IF_ERROR_RETURN
                (PHY84834_TOP_LEVEL_CMD_SET(unit, pc,
                        (uint16)PHY84834_DIAG_CMD_SET_1588_ENABLE_GENERIC,
                        args, 1));
            FLAGS(pc) &= ~PHY8483X_TIMESYNC_ENABLED;
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PMAD_REG(unit, pc,
                        PHY84834_PTP_PARSER_EG_CNTR_ALL_PKT_EN, 0x0, 0x1));
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_PMAD_REG(unit, pc,
                        PHY84834_PTP_PARSER_IG_CNTR_ALL_PKT_EN, 0x0, 0x1));
        }
    }
    SOC_IF_ERROR_RETURN
        (phy_8481_an_set(unit, pc->port, 1));

    return (SOC_E_NONE);
}

STATIC int
_phy84834_timesync_enable_get(int unit,phy_ctrl_t *pc, uint16 *enable )
{
    uint16 args[5];

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)PHY84834_DIAG_CMD_GET_1588_ENABLE_GENERIC, args, 1));

    if (enable) {
        *enable = args[0];
    }

    return (SOC_E_NONE);
}

void _phy84834_encode_egress_message_mode(soc_port_phy_timesync_event_message_egress_mode_t mode,
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
    default:
        break;
    }
}

void _phy84834_encode_ingress_message_mode(soc_port_phy_timesync_event_message_ingress_mode_t mode,
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
    default:
        break;
    }
}

void _phy84834_decode_egress_message_mode(uint16 value, int offset,
                                            soc_port_phy_timesync_event_message_egress_mode_t *mode)
{
    switch ((value >> offset) & 0x7) {
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
    default:
        break;
    }
}

void _phy84834_decode_ingress_message_mode(uint16 value, int offset,
                                            soc_port_phy_timesync_event_message_ingress_mode_t *mode)
{
    switch ((value >> offset) & 0x7) {
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
    default:
        break;
    }
}

void _phy84834_encode_gmode(soc_port_phy_timesync_global_mode_t mode,
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

void _phy84834_decode_gmode(uint16 value, soc_port_phy_timesync_global_mode_t *mode)
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


void _phy84834_encode_framesync_mode(soc_port_phy_timesync_framesync_mode_t mode,
                                            uint16 *value)
{
    switch (mode) {
    case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN0:
        *value = 1;
        break;
    case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN1:
        *value = 2;
        break;
    case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCOUT:
        *value = 3;
        break;
    case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_CPU:
        *value = 4;
        break;
    default:
        break;
    }
}

void _phy84834_decode_framesync_mode(uint16 value, soc_port_phy_timesync_framesync_mode_t *mode)
{
    switch (value & 0xf) {
    case 1:
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN0;
        break;
    case 2:
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN1;
        break;
    case 3:
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCOUT;
        break;
    case 4:
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_CPU;
        break;
    default:
        break;
    }
}

void _phy84834_encode_syncout_mode(soc_port_phy_timesync_syncout_mode_t mode,
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
    default:
        break;
    }
}

#ifdef PHY8185X_DEBUG_CMD_HANDLER
STATIC int
_phy8485x_emi_mode_enable_get(int unit, phy_ctrl_t *pc, uint16 *status)
{
    uint16 args[2];

   SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)PHY84834_DIAG_CMD_GET_EMI_MODE_ENABLE_V2, args, 1));

    *status = args[0];

    return (SOC_E_NONE);
}

STATIC int
_phy8485x_emi_mode_enable_set(int unit, phy_ctrl_t *pc, uint16 status)
{
    uint16 args[2];

    args[0] = status;
    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_SET(unit, pc, (uint16)PHY84834_DIAG_CMD_SET_EMI_MODE_ENABLE_V2, args, 1));

    return (SOC_E_NONE);
}

STATIC int
_phy8485x_sub_lf_rf_status_get(int unit, phy_ctrl_t *pc, uint16 *status)
{
    uint16 args[2];

    SOC_IF_ERROR_RETURN
       (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)PHY84834_DIAG_CMD_SET_SUB_LF_RF_V2, args, 1));

    *status = args[0];

    return (SOC_E_NONE);
}

STATIC int
_phy8485x_sub_lf_rf_set(int unit, phy_ctrl_t *pc)
{
    uint16 args[2];
    args[0]=1;

    SOC_IF_ERROR_RETURN
       (PHY84834_TOP_LEVEL_CMD_SET(unit, pc, (uint16)PHY84834_DIAG_CMD_SET_SUB_LF_RF_GENERIC, args, 1));

    return (SOC_E_NONE);
}

STATIC int
_phy8485x_sub_lf_rf_clear(int unit, phy_ctrl_t *pc)
{
   uint16 args[2];
   args[0]=1;

   SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_SET(unit, pc, (uint16)PHY84834_DIAG_CMD_CLEAR_SUB_LF_RF_GENERIC, args, 1));

    return (SOC_E_NONE);
}

STATIC int
_phy8485x_snr_get(int unit, phy_ctrl_t *pc)
{
    uint16 args[5];
    args[0]=0;

    SOC_IF_ERROR_RETURN
    (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)PHY84834_DIAG_CMD_GET_SNR_GENERIC,  args, 5));
    
    return (SOC_E_NONE);
}

STATIC int
_phy8485x_current_temp_get(int unit, phy_ctrl_t *pc)
{
    uint16 args[2];
    args[0]=0;

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)PHY84834_DIAG_CMD_GET_CURRENT_TEMP_GENERIC,  args, 1));

    return (SOC_E_NONE);
}

STATIC int
_phy8485x_upper_temp_warn_level_get(int unit, phy_ctrl_t *pc, uint16 *temp)
{
    uint16 args[2];
    args[0]=0;

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)PHY84834_DIAG_CMD_GET_UPPER_TEMP_WARNING_LEVEL_V2,  args, 1));

    *temp = args[0];

    return (SOC_E_NONE);
}

STATIC int
_phy8485x_upper_temp_warn_level_set(int unit, phy_ctrl_t *pc, uint16 temp)
{
    uint16 args[2];
    args[0]=temp;

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_SET(unit, pc, (uint16)PHY84834_DIAG_CMD_SET_UPPER_TEMP_WARNING_LEVEL_V2,  args, 1));

    return (SOC_E_NONE);
}

STATIC int
_phy8485x_lower_temp_warn_level_get(int unit, phy_ctrl_t *pc, uint16 *temp)
{
    uint16 args[2];
    args[0]=0;

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)PHY84834_DIAG_CMD_GET_LOWER_TEMP_WARNING_LEVEL_V2,  args, 1));

    *temp = args[0];

    return (SOC_E_NONE);
}

STATIC int
_phy8485x_lower_temp_warn_level_set(int unit, phy_ctrl_t *pc, uint16 temp)
{
    uint16 args[2];
    args[0]=temp;

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_SET(unit, pc, (uint16)PHY84834_DIAG_CMD_SET_LOWER_TEMP_WARNING_LEVEL_V2,  args, 1));

    return (SOC_E_NONE);
}

STATIC int
_phy8486x_xfi_2p5g_mode_get(int unit, phy_ctrl_t *pc, uint16 *mode_2p5g)
{
    uint16  args[2] = { 0, 0 };

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc,
                    (uint16) PHY84834_DIAG_CMD_GET_XFI_2P5G_MODE_V2, args, 1) );
    *mode_2p5g = args[0];

    return (SOC_E_NONE);
}

STATIC int
_phy848xx_xfi_2p5g_5g_mode_get(int unit, phy_ctrl_t *pc, uint16 *mode_2p5g, uint16 *mode_5g)
{
    uint16  args[2] = { 0, 0 };

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc,
                    (uint16) PHY84834_DIAG_CMD_GET_XFI_2P5G_MODE_V2, args, (PHY_IS_BCM8488X(pc) ? 2 : 1)) );
    *mode_2p5g = args[0];
    if (PHY_IS_BCM8488X(pc)) {
        *mode_5g = args[1];
    }
    return (SOC_E_NONE);
}

STATIC int
_phy_848xx_led_type_get (int unit, soc_port_t port, uint32 *led_type, uint32 *led_mode)
{
    uint16 args[5];
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)PHY84888_DIAG_CMD_GET_LED_TYPE, args, 2));

    if (led_type) {
        *led_type = args[0];
    }
    if ( PHY_IS_BCM8488X(pc) && (args[0] == 2) ) {
        *led_mode = args[1];
    }

    return (SOC_E_NONE);
}

STATIC int
_phy_848xx_led_type_set (int unit, soc_port_t port, uint32 led_type, uint32 led_mode)
{
    uint16 args[5];
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    args[0] = led_type;

    if (led_type == 2) {
        args[1] = led_mode;
    }

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_SET(unit, pc, (uint16)PHY84888_DIAG_CMD_SET_LED_TYPE, args, 2));

    return (SOC_E_NONE);
}

STATIC int
_phy_848xx_pause_frame_mode_get (int unit, soc_port_t port, uint32 *enable)
{
    uint16 args[2];
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)PHY84888_DIAG_CMD_GET_PAUSE_FRAME_MODE, args, 1));
    *enable = args[0];

    return (SOC_E_NONE);
}

STATIC int
_phy_848xx_pause_frame_mode_set (int unit, soc_port_t port, uint32 enable)
{
    uint16 args[2];
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    args[0] = enable;

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_SET(unit, pc, (uint16)PHY84888_DIAG_CMD_SET_PAUSE_FRAME_MODE, args, 1));

    return (SOC_E_NONE);
}
#endif /* PHY8185X_DEBUG_CMD_HANDLER */

STATIC int
_phy_848xx_mgbase_802_3bz_type_get (int unit, phy_ctrl_t *pc, uint16 *frame_type, uint16 *frame_mode)
{
    uint16        args[5];

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_GET(unit, pc, (uint16)PHY84888_DIAG_CMD_GET_MGBASET_802_3BZ_TYPE, args, 2));

    *frame_type = args[0];
    *frame_mode = args[1];

    return (SOC_E_NONE);
}

STATIC int
_phy_848xx_mgbase_802_3bz_type_set (int unit, phy_ctrl_t *pc, uint16 frame_type, uint16 frame_mode)
{
    uint16        args[5];

    args[0] = frame_type;
    args[1] = frame_mode;

    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_SET(unit, pc, (uint16)PHY84888_DIAG_CMD_SET_MGBASET_802_3BZ_TYPE, args, 2));

    return (SOC_E_NONE);
}

STATIC int
_phy848xx_xfi_2p5g_5g_mode_set(int unit, phy_ctrl_t *pc, uint16 mode_2p5g)
{
    uint16 args[2];

    args[0] = mode_2p5g;
    if ( PHY_IS_BCM8488X(pc) ) {
        args[1] = mode_2p5g;
    }
    SOC_IF_ERROR_RETURN
        (PHY84834_TOP_LEVEL_CMD_SET(unit, pc,
                    (uint16)PHY84834_DIAG_CMD_SET_XFI_2P5G_MODE_V2, args, (PHY_IS_BCM8488X(pc) ? 2 : 1)) );
    return (SOC_E_NONE);
}

void _phy84834_decode_syncout_mode(uint16 value,
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


/*
 * Function:
 *    phy84834_timesync_config_set
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
int
phy84834_timesync_config_set(int unit, soc_port_t port,
                             soc_port_phy_timesync_config_t *conf)
{
    uint16 eg_config= 0, ig_config = 0,
           nse_sc_cfg = 0, value = 0, nse_nco_cfg = 0, temp16 = 0,temp1 = 0,temp2 = 0,
           gmode = 0, syncout_mode = 0, framesync_mode = 0;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if(NULL == conf) {
        return SOC_E_PARAM;
    }
    if ( PHY_IS_BCM8488X(pc) || (! PHY_IS_BCM8483X_AND_UP(pc)) ) {
        return SOC_E_UNAVAIL;
    }
    
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FLAGS) {
        SOC_IF_ERROR_RETURN
            (_phy84834_timesync_enable_set(unit, pc,
                        (conf->flags & SOC_PORT_PHY_TIMESYNC_ENABLE) ? 1 : 0 ));

        /* Capture PKT INFO  - Rx & Tx*/
        temp1 = temp2 = 0;  
        if (conf->flags & SOC_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE) {
            temp1 |= PHY84834_PTP_EG_PTP_PKT_CFG_CAPT_PTP_ALL;
            temp2 |= PHY84834_PTP_IG_PTP_PKT_CFG_CAPT_PTP_ALL; 
        }
        /* Egress  Parser PTP Config Reg. 1.DA08[15,11,7,3] */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc,
                             PHY84834_PTP_EG_PTP_PKT_CFG_REG,
                             temp1, PHY84834_PTP_EG_PTP_PKT_CFG_CAPT_PTP_ALL));
        /* Ingress Parser PTP Config Reg. 1.DA88[15,11,7,3] */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                             PHY84834_PTP_IG_PTP_PKT_CFG_REG,
                             temp2, PHY84834_PTP_IG_PTP_PKT_CFG_CAPT_PTP_ALL));

        /* Timestamp/Source_port capturing configuration */
        /* Capture full 80-bit src_port or 16-bit src port + 64-bit IP/MAC */
        if (conf->flags & SOC_PORT_PHY_TIMESYNC_CAPTURE_FULL_SOURCE_PORT) {
            temp16 |= PHY8486X_PTP_PIC_CONFIG_FULL_SRC_PORT;
        }
        /* Capture packet's 48-bit countstamp or 64 LSB of 80-bit timestamp */
        if (conf->flags & SOC_PORT_PHY_TIMESYNC_CAPTURE_LONG_TIMESTAMP) {
            temp16 |= PHY8486X_PTP_PIC_CONFIG_LONG_TIMESTAMP;
        }
        /* PIC Configuration Reg. 1.DB01[1:0] */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                                     PHY8486X_PTP_PIC_CONFIG_REG,
                                     temp16, PHY8486X_PTP_PIC_CONFIG_MASK) );

        /* Enable timestamp capture on the next frame sync event */
        nse_sc_cfg |= PHY84834_PTP_NSE_SC_CFG_FSYNC_EN;
        if (conf->flags & SOC_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE) {
            nse_sc_cfg |= PHY84834_PTP_NSE_SC_CFG_FSYNC_TS_EN;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                                     PHY84834_PTP_NSE_SC_CFG_REG,
                                     nse_sc_cfg, 
                                     PHY84834_PTP_NSE_SC_CFG_FSYNC_EN |
                                     PHY84834_PTP_NSE_SC_CFG_FSYNC_TS_EN) );
       
        /* Enable the FCS/CRC check in PTP detection receiving side */
        temp16 = 0;
        if (conf->flags & SOC_PORT_PHY_TIMESYNC_RX_CRC_ENABLE) {
            temp16 |= PHY84834_PTP_IG_CFG_FCS_CHECK_EN;       
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                                     PHY84834_PTP_IG_CFG_REG,
                                     temp16, 
                                     PHY84834_PTP_IG_CFG_FCS_CHECK_EN));

        /* Enable different packet detections & checks in Rx & Tx direction */
        if ( (conf->flags & SOC_PORT_PHY_TIMESYNC_8021AS_ENABLE) ||
             (conf->flags & SOC_PORT_PHY_TIMESYNC_L2_ENABLE) ) {
            ig_config |= PHY84834_PTP_IG_CFG_L2_802AS_EN;
            eg_config |= PHY84834_PTP_EG_CFG_L2_802AS_EN;
        }
        if ( conf->flags & SOC_PORT_PHY_TIMESYNC_IP4_ENABLE ) {
            ig_config |= PHY84834_PTP_IG_CFG_L4_IP4_UDP_EN;
            eg_config |= PHY84834_PTP_EG_CFG_L4_IP4_UDP_EN;
        }
        if ( conf->flags & SOC_PORT_PHY_TIMESYNC_IP6_ENABLE ) {
            ig_config |= PHY84834_PTP_IG_CFG_L4_IP6_UDP_EN;
            eg_config |= PHY84834_PTP_EG_CFG_L4_IP6_UDP_EN;
        }
#if 0
        if (conf->flags & SOC_PORT_PHY_TIMESYNC_L4_IPV6_ADDR_ENABLE) {
            ig_config |= PHY84834_PTP_IG_CFG_L4_IP6_UDP_ACH_EN;
            eg_config |= PHY84834_PTP_EG_CFG_L4_IP6_UDP_ACH_EN;
        }
        if (conf->flags & SOC_PORT_PHY_TIMESYNC_L4_IP_ADDR_ENABLE) {
            ig_config |= PHY84834_PTP_IG_CFG_L4_IP4_UDP_ACH_EN;
            eg_config |= PHY84834_PTP_EG_CFG_L4_IP4_UDP_ACH_EN;
        }
        if ((conf->flags & SOC_PORT_PHY_TIMESYNC_L2_DS_ENABLE) 
           || (conf->flags & SOC_PORT_PHY_TIMESYNC_AS_DS_ENABLE)) { 
            ig_config |= PHY84834_PTP_IG_CFG_L2_802AS_ACH_EN;
            eg_config |= PHY84834_PTP_EG_CFG_L2_802AS_ACH_EN;
        } 
#endif
        /* IG Config */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, PHY84834_PTP_IG_CFG_REG, ig_config));
        /* EG Config */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, PHY84834_PTP_EG_CFG_REG, eg_config));
                                          

        /* Set input for NCO adder */
        if (conf->flags & SOC_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT) {
            nse_nco_cfg |= (uint16)  PHY84834_PTP_NSE_NCO_CFG_FCW_SEL;
        } else {
            nse_nco_cfg &= (uint16) ~PHY84834_PTP_NSE_NCO_CFG_FCW_SEL;
        }

        /* NSE NCO CFG register */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                                               PHY84834_PTP_NSE_NCO_CFG_REG, 
                                               nse_nco_cfg, 
                                               PHY84834_PTP_NSE_NCO_CFG_FCW_SEL));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ITPID) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_VLAN_1TAG_TPID_REG, conf->itpid));
    }
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_VLAN_2TAG_OTPID_REG, conf->otpid));
    }
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID2) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_VLAN_2TAG_OTPID_1_REG, conf->otpid2));
    }
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TS_DIVIDER) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SHDW_DPLL_SYNC_DIV_REG, 
                                              conf->ts_divider & 0x0fff));
        /* Enable loading shadow reg value to active copy on next framesync */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc,
                                               PHY84834_PTP_NSE_SHDW_CTL_REG,
                                               PHY84834_PTP_NSE_SHDW_CTL_LOAD_SYNC_DIV,
                                               PHY84834_PTP_NSE_SHDW_CTL_LOAD_SYNC_DIV));
    }
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_LINK_DELAY) {
        /* Do nothing
         *
         * In some chips there are two registers Rx timestamp offset & 
         * Rx link delay. These two registers do similar funtion. 
         * The offset was added to the internal timestamp, the 
         * link delay was subtracted from it. Rx link delay control not required 
         * in this chip. Tx/RX timestamp offset does the job. The signed value will
         * be added to the timestamp.
         */
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                     PHY84834_PTP_NSE_SHDW_ORG_TS_0_REG, 
                     (uint16)(conf->original_timecode.nanoseconds & 0xffff)));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                     PHY84834_PTP_NSE_SHDW_ORG_TS_1_REG, 
                     (uint16)((conf->original_timecode.nanoseconds >> 16) & 0xffff)));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                     PHY84834_PTP_NSE_SHDW_ORG_TS_2_REG, 
                     (uint16)(COMPILER_64_LO(conf->original_timecode.seconds) & 0xffff)));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                     PHY84834_PTP_NSE_SHDW_ORG_TS_3_REG, 
                     (uint16)((COMPILER_64_LO(conf->original_timecode.seconds) >> 16) 
                        & 0xffff)));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                     PHY84834_PTP_NSE_SHDW_ORG_TS_4_REG, 
                     (uint16)(COMPILER_64_HI(conf->original_timecode.seconds) & 0xffff)));
        /* Enable loading shadow reg value to active copy on next framesync */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc,
                                               PHY84834_PTP_NSE_SHDW_CTL_REG,
                                               PHY84834_PTP_NSE_SHDW_CTL_LOAD_ORIG_TS,
                                               PHY84834_PTP_NSE_SHDW_CTL_LOAD_ORIG_TS));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_GMODE) {
        _phy84834_encode_gmode(conf->gmode,&gmode);
    }
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE) {
        _phy84834_encode_framesync_mode(conf->framesync.mode, &framesync_mode);
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                        PHY84834_NSE_SYNCIN_FSYNC_LEN_0_REG, 
                                        conf->framesync.length_threshold & 0xffff));/*[15:0]*/
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                                        PHY84834_NSE_SYNCIN_FSYNC_LEN_1_REG, 
                                        (conf->framesync.length_threshold >> 16) & 0x7,
                                        0x7));/*[18:16]*/
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                        PHY84834_NSE_SYNCIN_FSYNC_DLY_0_REG, 
                                        conf->framesync.event_offset & 0xffff));/*[15:0]*/
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                                        PHY84834_NSE_SYNCIN_FSYNC_DLY_1_REG, 
                                        (conf->framesync.event_offset >> 16) & 0x7,
                                        0x7));/*[18:16]*/
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE) {
        _phy84834_encode_syncout_mode(conf->syncout.mode, &syncout_mode);

        /* Interval & pulse width of syncout pulse */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SHDW_SOUT_INTERVAL_0_REG, 
                                              conf->syncout.interval & 0xffff));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SHDW_SOUT_INTERVAL_1_REG,
                                              ((conf->syncout.interval >> 16) & 0xffff)));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SHDW_SOUT_PULSE1_LEN_REG, 
                                              conf->syncout.pulse_1_length & 0x0fff));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SHDW_SOUT_PULSE2_LEN_REG, 
                                              conf->syncout.pulse_2_length & 0x0fff));
        /* Local timer for SYNC_OUT */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                        PHY84834_PTP_NSE_SOUT_TS_CMP_0_REG,
                        (uint16)(COMPILER_64_LO(conf->syncout.syncout_ts) & 0xffff)));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                        PHY84834_PTP_NSE_SOUT_TS_CMP_1_REG,
                        (uint16)((COMPILER_64_LO(conf->syncout.syncout_ts) >> 16) & 0xffff)));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                        PHY84834_PTP_NSE_SOUT_TS_CMP_2_REG,
                        (uint16)(COMPILER_64_HI(conf->syncout.syncout_ts) & 0xffff)));

        /* Enable loading shadow reg value to active copy on next framesync */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc,
                                               PHY84834_PTP_NSE_SHDW_CTL_REG,
                                               PHY84834_PTP_NSE_SHDW_CTL_LOAD_SYNCOUT,
                                               PHY84834_PTP_NSE_SHDW_CTL_LOAD_SYNCOUT));
    }
 
    /* GMODE */ 
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PMAD_REG(unit, pc, PHY84834_PTP_NSE_SC_CFG_REG, 
                   (gmode << 0) | (framesync_mode << 4) | (syncout_mode << 8), 
                   ( PHY84834_PTP_NSE_SC_CFG_GS_MODE |
                     PHY84834_PTP_NSE_SC_CFG_SI_MODE |
                     PHY84834_PTP_NSE_SC_CFG_SO_MODE)) );
    /* TX TS OFFSET register */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_TS_EG_SOP_OFFSET_0_REG, 
                                              (uint16)(conf->tx_timestamp_offset & 0xffff)));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_TS_EG_SOP_OFFSET_1_REG, 
                                              (uint16)((conf->tx_timestamp_offset >> 16) & 0xf ),
                                              0xf));

    }

     /* RX TS OFFSET register */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_TS_IG_SOP_OFFSET_0_REG, 
                                              (uint16)(conf->rx_timestamp_offset & 0xffff)));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_TS_IG_SOP_OFFSET_1_REG, 
                                              (uint16)((conf->rx_timestamp_offset >> 16) & 0xf ),
                                              0xf));
    }

    /* PTP TX action */
    value = 0;
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE) {
        _phy84834_encode_egress_message_mode(conf->tx_sync_mode, 0, &value);
    }
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE) {
        _phy84834_encode_egress_message_mode(conf->tx_delay_request_mode, 4, &value);
    }
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE) {
        _phy84834_encode_egress_message_mode(conf->tx_pdelay_request_mode, 8, &value);
    }
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE) {
        _phy84834_encode_egress_message_mode(conf->tx_pdelay_response_mode, 12, &value);
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                           PHY84834_PTP_EG_PTP_PKT_CFG_REG, 
                           value, 
                           ( PHY84834_PTP_EG_PTP_PKT_CFG_ACTION_PTP_SYNC
                           | PHY84834_PTP_EG_PTP_PKT_CFG_ACTION_PTP_DREQ
                           | PHY84834_PTP_EG_PTP_PKT_CFG_ACTION_PTP_PDREQ
                           | PHY84834_PTP_EG_PTP_PKT_CFG_ACTION_PTP_PDRESP)) ); 
    /* PTP RX action */
    value = 0;
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE) {
        _phy84834_encode_ingress_message_mode(conf->rx_sync_mode, 0, &value);
    }
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE) {
        _phy84834_encode_ingress_message_mode(conf->rx_delay_request_mode, 4, &value);
    }
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE) {
        _phy84834_encode_ingress_message_mode(conf->rx_pdelay_request_mode, 8, &value);
    }
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE) {
        _phy84834_encode_ingress_message_mode(conf->rx_pdelay_response_mode, 12, &value);
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                           PHY84834_PTP_IG_PTP_PKT_CFG_REG, 
                           value, 
                           ( PHY84834_PTP_IG_PTP_PKT_CFG_ACTION_PTP_SYNC
                           | PHY84834_PTP_IG_PTP_PKT_CFG_ACTION_PTP_DREQ
                           | PHY84834_PTP_IG_PTP_PKT_CFG_ACTION_PTP_PDREQ
                           | PHY84834_PTP_IG_PTP_PKT_CFG_ACTION_PTP_PDRESP)) );                                    

    /* DPLL initial reference phase */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE) {
        /* [15:0] */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                        PHY84834_PTP_NSE_DPLL_SHDW_REF_PHASE_0_REG, 
                        (uint16)(COMPILER_64_LO(conf->phy_1588_dpll_ref_phase) & 0xffff)));
        /* [31:16]  */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                        PHY84834_PTP_NSE_DPLL_SHDW_REF_PHASE_1_REG, 
                        (uint16)((COMPILER_64_LO(conf->phy_1588_dpll_ref_phase) >> 16) & 0xffff)));
        /* [47:32] */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                        PHY84834_PTP_NSE_DPLL_SHDW_REF_PHASE_2_REG, 
                        (uint16)(COMPILER_64_HI(conf->phy_1588_dpll_ref_phase) & 0xffff)));
        /* Enable loading shadow reg value to active copy on next framesync */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc,
                                               PHY84834_PTP_NSE_SHDW_CTL_REG,
                                               PHY84834_PTP_NSE_SHDW_CTL_LOAD_REF_PHASE,
                                               PHY84834_PTP_NSE_SHDW_CTL_LOAD_REF_PHASE));
    }

    /* DPLL REF PHASE */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_DPLL_SHDW_REF_PERIOD_0_REG, 
                                              conf->phy_1588_dpll_ref_phase_delta & 0xffff));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_DPLL_SHDW_REF_PERIOD_1_REG, 
                                              (conf->phy_1588_dpll_ref_phase_delta >> 16) & 0xffff));
         /* Enable loading shadow reg value to active copy on next framesync */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc,
                                               PHY84834_PTP_NSE_SHDW_CTL_REG,
                                               PHY84834_PTP_NSE_SHDW_CTL_LOAD_REF_PERIOD,
                                               PHY84834_PTP_NSE_SHDW_CTL_LOAD_REF_PERIOD));
    }

    /* DPLL K1 */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_DPLL_SHDW_GAIN_1_REG, 
                                              conf->phy_1588_dpll_k1 & 0x3f));
       /* Enable loading shadow reg value to active copy on next framesync */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc,
                                               PHY84834_PTP_NSE_SHDW_CTL_REG,
                                               PHY84834_PTP_NSE_SHDW_CTL_LOAD_DPLL_GAIN1,
                                               PHY84834_PTP_NSE_SHDW_CTL_LOAD_DPLL_GAIN1));
    }

    /* DPLL K2 */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_DPLL_SHDW_GAIN_2_REG, 
                                              conf->phy_1588_dpll_k2 & 0x3f));
       /* Enable loading shadow reg value to active copy on next framesync */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc,
                                               PHY84834_PTP_NSE_SHDW_CTL_REG,
                                               PHY84834_PTP_NSE_SHDW_CTL_LOAD_DPLL_GAIN2,
                                               PHY84834_PTP_NSE_SHDW_CTL_LOAD_DPLL_GAIN2));
    }

    /* DPLL K3 */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_DPLL_SHDW_GAIN_3_REG, 
                                              conf->phy_1588_dpll_k3 & 0x3f));
       /* Enable loading shadow reg value to active copy on next framesync */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc,
                                               PHY84834_PTP_NSE_SHDW_CTL_REG,
                                               PHY84834_PTP_NSE_SHDW_CTL_LOAD_DPLL_GAIN3,
                                               PHY84834_PTP_NSE_SHDW_CTL_LOAD_DPLL_GAIN3));
    }

    /* IEEE-1588 Gen-2 Inband operation */
    if ( conf->validity_mask &
               SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL ) {
        /*
         *  Inband control parameters setting
         */
        temp16 = (conf->inband_control.resv0_id
                      << PHY848XX_PTP_INBAND_CNTL_RESV0_ID_SHIFT)
                       & PHY848XX_PTP_INBAND_CNTL_RESV0_ID;
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_SYNC_ENABLE ) {
            temp16 |= PHY848XX_PTP_INBAND_CNTL_SYNC_EN;
        }
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_DELAY_RQ_ENABLE ) {
            temp16 |= PHY848XX_PTP_INBAND_CNTL_DREQ_EN;
        }
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_PDELAY_RQ_ENABLE ) {
            temp16 |= PHY848XX_PTP_INBAND_CNTL_PREQ_EN;
        }
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_PDELAY_RESP_ENABLE ) {
            temp16 |= PHY848XX_PTP_INBAND_CNTL_PRSP_EN;
        }

        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_CHECK ) {
            temp16 |= PHY848XX_PTP_INBAND_CNTL_RESV0_ID_CHECK;
        }
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_UPDATE ) {
            temp16 |= PHY848XX_PTP_INBAND_CNTL_RESV0_ID_UPDATE;
        }
        if ( (conf->flags & SOC_PORT_PHY_TIMESYNC_FOLLOW_UP_ASSIST_ENABLE) ||
             (conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_FOLLOW_UP_ASSIST) ) {
            temp16 |= PHY848XX_PTP_INBAND_CNTL_TS_UPD_FOLLOW_UP |
                      PHY848XX_PTP_INBAND_CNTL_TS_CAP_MODE;
        }
        if ( (conf->flags & SOC_PORT_PHY_TIMESYNC_DELAY_RESP_ASSIST_ENABLE) ||
             (conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_DELAY_RESP_ASSIST) ) {
            temp16 |= PHY848XX_PTP_INBAND_CNTL_TS_UPD_DELAY_RSP |
                      PHY848XX_PTP_INBAND_CNTL_TS_CAP_MODE;
        }
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_TIMER_MODE_SELECT ) {
            temp16 |= PHY848XX_PTP_INBAND_CNTL_TS_SEL;
        }
        if ( conf->inband_control.timer_mode &
                        SOC_PORT_PHY_TIMESYNC_TIMER_MODE_80BIT ) {
            temp16 |= PHY848XX_PTP_INBAND_CNTL_TS_SEL;
}
        /* PTP Inband Control Register, 01.0xDC00  */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                    PHY848XX_PTP_INBAND_CNTL_REG, temp16) );

        /* config the comparison of IP, VLAN & Source_Port in SOP memory */
        temp16 = 0;
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_VLAN_ID) {
            temp16 |= PHY848XX_PTP_INBAND_CTL2_CMP_VLAN_ID;
        }
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_SRC_PORT_NUM) {
            temp16 |= PHY848XX_PTP_INBAND_CTL2_CMP_SRC_PORT;
        } 
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_IP_ADDR ) {
            temp16 |= PHY848XX_PTP_INBAND_CTL2_CMP_IP_ADDR; 
        }
        /* PTP Inband Control 2 Register, 01.0xDC01  */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                                     PHY848XX_PTP_INBAND_CTL2_REG, temp16,
                                     PHY848XX_PTP_INBAND_CTL2_CMP_MASK) );
    } /* TIMESYNC_VALID_PHY_1588_INBAND_CONTROL */

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy84834_timesync_config_get
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */

int
phy84834_timesync_config_get(int unit, soc_port_t port,
                             soc_port_phy_timesync_config_t *conf)
{
    uint16  eg_config = 0, value = 0,
            nse_sc_cfg = 0, nse_nco_cfg = 0, temp1, temp2, temp3, temp16 = 0 ;
    soc_port_phy_timesync_global_mode_t gmode = SOC_PORT_PHY_TIMESYNC_MODE_FREE;
    soc_port_phy_timesync_framesync_mode_t framesync_mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_NONE;
    soc_port_phy_timesync_syncout_mode_t syncout_mode = SOC_PORT_PHY_TIMESYNC_SYNCOUT_DISABLE;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_IS_BCM8488X(pc) || (! PHY_IS_BCM8483X_AND_UP(pc)) ) {
        return SOC_E_UNAVAIL;
    }
    conf->flags = 0;

    /* NSE SC CFG register */
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PMAD_REG(unit, pc, PHY84834_PTP_NSE_SC_CFG_REG, &nse_sc_cfg));

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FLAGS) {
        SOC_IF_ERROR_RETURN
            (_phy84834_timesync_enable_get(unit, pc, &value ));

        if (value) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_ENABLE;
        }

        /* Capture pkt info enabled 
         * Note: Both tx and rx pkt capture need to be enabled 
         * to consider this flag to be set
         */
        temp1 = temp2 = 0;
        SOC_IF_ERROR_RETURN
        (READ_PHY8481_PMAD_REG(unit, pc,
                               PHY84834_PTP_EG_PTP_PKT_CFG_REG, &temp1));
        SOC_IF_ERROR_RETURN
        (READ_PHY8481_PMAD_REG(unit, pc,
                               PHY84834_PTP_IG_PTP_PKT_CFG_REG, &temp2));

        if ( (temp1 & PHY84834_PTP_EG_PTP_PKT_CFG_CAPT_PTP_SYNC) &&
             (temp2 & PHY84834_PTP_IG_PTP_PKT_CFG_CAPT_PTP_SYNC)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE; 
        }

        /* The Heartbeat registers ensure that ToD is synchronized  *\
        \* across the whole system.                                 */
        if (nse_sc_cfg & PHY84834_PTP_NSE_SC_CFG_FSYNC_TS_EN) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE;
        }

        /* RX CRC -- Igress Parser Configuration Register, Reg. 1.DA81[0]  */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                                   PHY84834_PTP_IG_CFG_REG, &temp16));
        if ( temp16 & PHY84834_PTP_IG_CFG_FCS_CHECK_EN) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_RX_CRC_ENABLE;
        }

        /* EG Config: addr check enable & packet detection  *\
        \* Egress Parser Configuration Register  1.DA01     */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                                   PHY84834_PTP_EG_CFG_REG, &eg_config));
        if ( eg_config & PHY84834_PTP_IG_CFG_L2_802AS_EN ) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_L2_ENABLE |
                           SOC_PORT_PHY_TIMESYNC_8021AS_ENABLE;
        }
        if ( eg_config & PHY84834_PTP_IG_CFG_L4_IP4_UDP_EN ) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_IP4_ENABLE;
        }
        if ( eg_config & PHY84834_PTP_IG_CFG_L4_IP6_UDP_EN ) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_IP6_ENABLE;
        }
#if 0
        if (eg_config & PHY84834_PTP_EG_CFG_L4_IP6_UDP_ACH_EN) {
            eg_config |= SOC_PORT_PHY_TIMESYNC_L4_IPV6_ADDR_ENABLE;
        }
        if (eg_config & PHY84834_PTP_EG_CFG_L4_IP4_UDP_ACH_EN) {
            eg_config |= SOC_PORT_PHY_TIMESYNC_L4_IP_ADDR_ENABLE;
        }
        if (eg_config & PHY84834_PTP_IG_CFG_L2_802AS_ACH_EN) { 
            eg_config |= SOC_PORT_PHY_TIMESYNC_L2_DS_ENABLE 
                         | SOC_PORT_PHY_TIMESYNC_AS_DS_ENABLE;
        } 
#endif
        /* CLK src for NCO */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                   PHY84834_PTP_NSE_NCO_CFG_REG, &nse_nco_cfg));

        if ( nse_nco_cfg & PHY84834_PTP_NSE_NCO_CFG_FCW_SEL ) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT;
        }
    }


    /* Gmode */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_GMODE) {
        _phy84834_decode_gmode(nse_sc_cfg ,&gmode);
        conf->gmode = gmode;
    }

    /* Framesync */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE) {
        _phy84834_decode_framesync_mode((nse_sc_cfg >> 4), &framesync_mode);
        conf->framesync.mode = framesync_mode;
        /* Length threshold */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                                        PHY84834_NSE_SYNCIN_FSYNC_LEN_1_REG,
                                        &temp16)); /*[18:16]*/
        conf->framesync.length_threshold = (temp16 & 0x7); 
        conf->framesync.length_threshold <<= 16;
 
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                        PHY84834_NSE_SYNCIN_FSYNC_LEN_0_REG, 
                                        &temp16));/*[15:0]*/
        conf->framesync.length_threshold |= temp16;

        /* Event offset */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                                        PHY84834_NSE_SYNCIN_FSYNC_DLY_1_REG,
                                        &temp16)); /*[18:16]*/
        conf->framesync.event_offset = (temp16 & 0x7);
        conf->framesync.event_offset <<= 16;

        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                        PHY84834_NSE_SYNCIN_FSYNC_DLY_0_REG, 
                                        &temp16));/*[15:0]*/
        conf->framesync.event_offset |= temp16;

    }

    /* SYNC OUT */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE) {
        _phy84834_decode_syncout_mode((nse_sc_cfg >> 8), &syncout_mode);
        conf->syncout.mode = syncout_mode;
        
        /* Syncout interval */   
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                                              PHY84834_PTP_NSE_SHDW_SOUT_INTERVAL_1_REG,
                                              &temp16));
        conf->syncout.interval = temp16;
        conf->syncout.interval <<= 16;
 
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SHDW_SOUT_INTERVAL_0_REG, 
                                              &temp16));
        conf->syncout.interval |= temp16;
        
        /* Syncout pulse 1 */
        temp16 = 0;
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SHDW_SOUT_PULSE1_LEN_REG, 
                                              &temp16));
        conf->syncout.pulse_1_length =  temp16 & 0x0fff;
        
        /* Syncout pulse 2 */
        temp16 = 0;
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SHDW_SOUT_PULSE2_LEN_REG, 
                                              &temp16));
        conf->syncout.pulse_2_length =  temp16 & 0x0fff;

        /* Syncout timestamp match value */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SOUT_TS_CMP_0_REG, 
                                              &temp1));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SOUT_TS_CMP_1_REG, 
                                              &temp2));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SOUT_TS_CMP_2_REG, 
                                              &temp3));
        COMPILER_64_SET(conf->syncout.syncout_ts, ((uint32)temp3), (((uint32)temp2<<16)|((uint32)temp1)));
    }

    /* VLAN TAG register */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ITPID) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_VLAN_1TAG_TPID_REG,
                                              &conf->itpid));
    }

    /* OUTER VLAN TAG register */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_VLAN_2TAG_OTPID_REG, 
                                              &conf->otpid));
    }

    /* INNER VLAN TAG register */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID2) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_VLAN_2TAG_OTPID_1_REG,
                                              &conf->otpid2));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TS_DIVIDER) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SHDW_DPLL_SYNC_DIV_REG, 
                                              &conf->ts_divider));
        conf->ts_divider &= 0xfff;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_LINK_DELAY) {
         /* Do nothing
         *
         * In some chips there are two registers Rx timestamp offset & 
         * Rx link delay. These two registers do similar funtion. 
         * The offset is added to the internal timestamp, the 
         * link delay is subtracted from it. Rx link delay control not required 
         * in this chip. Tx/RX timestamp offset does the job. The signed value will
         * be added to the timestamp.
         */
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE) {
        /* TIME CODE 0 register */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SHDW_ORG_TS_0_REG, 
                                              &temp1));

        /* TIME CODE 1 register */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SHDW_ORG_TS_1_REG, 
                                              &temp2));

        conf->original_timecode.nanoseconds = ((uint32)temp2 << 16) | temp1; 

        /* TIME CODE 2 register */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SHDW_ORG_TS_2_REG, 
                                              &temp1));
        /* TIME CODE 3 register */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SHDW_ORG_TS_3_REG, 
                                              &temp2));
        /* TIME CODE 4 register */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SHDW_ORG_TS_4_REG, 
                                              &temp3));
        COMPILER_64_SET(conf->original_timecode.seconds, ((uint32)temp3),  (((uint32)temp2<<16)|((uint32)temp1)));
    }

    /* TX TS OFFSET register */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET) {

        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_TS_EG_SOP_OFFSET_0_REG, 
                                              &temp1));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_TS_EG_SOP_OFFSET_1_REG, 
                                              &temp2));
        conf->tx_timestamp_offset = ((uint32)temp2 & 0xf) << 16 | temp1;
    }
    
    /* RX TS OFFSET register */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_TS_IG_SOP_OFFSET_0_REG, 
                                              (uint16 *)&temp1));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_TS_IG_SOP_OFFSET_1_REG, 
                                              &temp2));
        conf->rx_timestamp_offset = ((uint32)temp2 & 0xf) << 16 | temp1;
    }

    /* PTP TX action */
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PMAD_REG(unit, pc, 
                               PHY84834_PTP_EG_PTP_PKT_CFG_REG, &value)); 

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE) {
        _phy84834_decode_egress_message_mode(value, 0, &conf->tx_sync_mode);
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE) {
        _phy84834_decode_egress_message_mode(value, 4, &conf->tx_delay_request_mode);
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE) {
        _phy84834_decode_egress_message_mode(value, 8, &conf->tx_pdelay_request_mode);
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE) {
        _phy84834_decode_egress_message_mode(value, 12, &conf->tx_pdelay_response_mode);
    }
                                            
    /* PTP RX action */
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PMAD_REG(unit, pc, 
                               PHY84834_PTP_IG_PTP_PKT_CFG_REG, &value)); 

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE) {
        _phy84834_decode_ingress_message_mode(value, 0, &conf->rx_sync_mode);
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE) {
        _phy84834_decode_ingress_message_mode(value, 4, &conf->rx_delay_request_mode);
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE) {
        _phy84834_decode_ingress_message_mode(value, 8, &conf->rx_pdelay_request_mode);
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE) {
        _phy84834_decode_ingress_message_mode(value, 12, &conf->rx_pdelay_response_mode);
    }

    /* DPLL initial reference phase */                                       
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE) {
        /* [15:0] */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_DPLL_SHDW_REF_PHASE_0_REG, 
                                              &temp1));

        /* [31:16] */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_DPLL_SHDW_REF_PHASE_1_REG, 
                                              &temp2));

        /* [47:32] */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_DPLL_SHDW_REF_PHASE_2_REG, 
                                              &temp3));

        COMPILER_64_SET(conf->phy_1588_dpll_ref_phase, ((uint32)temp3), (((uint32)temp2<<16)|((uint32)temp1)));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_DPLL_SHDW_REF_PERIOD_0_REG, 
                                              &temp1));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_DPLL_SHDW_REF_PERIOD_1_REG, 
                                              &temp2));

        conf->phy_1588_dpll_ref_phase_delta = ((uint32)temp2 << 16) | temp1; 
    }

    /* DPLL K1 */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_DPLL_SHDW_GAIN_1_REG, 
                                              &conf->phy_1588_dpll_k1)); 
        conf->phy_1588_dpll_k1 &= 0x3f;
    }

    /* DPLL K2 */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_DPLL_SHDW_GAIN_2_REG, 
                                              &conf->phy_1588_dpll_k2)); 
        conf->phy_1588_dpll_k2 &= 0x3f;
    }

    /* DPLL K3 */
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_DPLL_SHDW_GAIN_3_REG, 
                                              &conf->phy_1588_dpll_k3)); 
        conf->phy_1588_dpll_k3 &= 0x3f;
    }

    /* IEEE-1588 Gen-2 Inband operations */
    if ( (conf->validity_mask &
                SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL) ) {
        /* PTP Inband Control Register, 1.0xDC00  */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                   PHY848XX_PTP_INBAND_CNTL_REG, &temp16) );
        conf->inband_control.resv0_id =         /* Rev0_ID[7:4] */
                (temp16 & PHY848XX_PTP_INBAND_CNTL_RESV0_ID)
                 >> PHY848XX_PTP_INBAND_CNTL_RESV0_ID_SHIFT;

        if ( temp16 & PHY848XX_PTP_INBAND_CNTL_SYNC_EN ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_SYNC_ENABLE;
        }
        if ( temp16 & PHY848XX_PTP_INBAND_CNTL_DREQ_EN ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_DELAY_RQ_ENABLE;
        }
        if ( temp16 & PHY848XX_PTP_INBAND_CNTL_PREQ_EN ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_PDELAY_RQ_ENABLE;
        }
        if ( temp16 & PHY848XX_PTP_INBAND_CNTL_PRSP_EN ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_PDELAY_RESP_ENABLE;
        }

        if ( temp16 & PHY848XX_PTP_INBAND_CNTL_RESV0_ID_CHECK ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_CHECK;
        }
        if ( temp16 & PHY848XX_PTP_INBAND_CNTL_RESV0_ID_UPDATE ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_UPDATE;
        }
        if ( temp16 & PHY848XX_PTP_INBAND_CNTL_TS_UPD_FOLLOW_UP ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_FOLLOW_UP_ASSIST;
            conf->flags |=
                SOC_PORT_PHY_TIMESYNC_FOLLOW_UP_ASSIST_ENABLE;
        }
        if ( temp16 & PHY848XX_PTP_INBAND_CNTL_TS_UPD_DELAY_RSP ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_DELAY_RESP_ASSIST;
            conf->flags |=
                SOC_PORT_PHY_TIMESYNC_DELAY_RESP_ASSIST_ENABLE;
        }
        if ( temp16 & PHY848XX_PTP_INBAND_CNTL_TS_SEL ) {
            conf->inband_control.timer_mode |=
                      SOC_PORT_PHY_TIMESYNC_TIMER_MODE_80BIT;
        } else {
            conf->inband_control.timer_mode |=
                      SOC_PORT_PHY_TIMESYNC_TIMER_MODE_48BIT;
        }
        if ( temp16 & PHY848XX_PTP_INBAND_CNTL_IP_ADDR_EN ) {
            if ( temp16 & PHY848XX_PTP_INBAND_CNTL_IP_MAC_SEL ) {
                conf->inband_control.flags |=
                      SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_MAC_ADDR;
            } else {
                conf->inband_control.flags |=
                      SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_IP_ADDR;
            }
        }

        /* PTP Inband Control 2 Register, 01.0xDC01  */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                   PHY848XX_PTP_INBAND_CTL2_REG, &temp16) );
        /* comparison of IP, VLAN & Source_Port in SOP memory */
        if ( temp16 & PHY848XX_PTP_INBAND_CTL2_CMP_VLAN_ID ) {
            conf->inband_control.flags |=
                      SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_VLAN_ID;
        }
        if ( temp16 & PHY848XX_PTP_INBAND_CTL2_CMP_SRC_PORT ) {
            conf->inband_control.flags |=
                      SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_SRC_PORT_NUM;
        }
        if ( temp16 & PHY848XX_PTP_INBAND_CTL2_CMP_IP_ADDR ) {
            conf->inband_control.flags |=
                      SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_IP_ADDR;
        }
    } /* SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL */

    return SOC_E_NONE;
}


/*
 * Function:
 *    phy84834_timesync_control_set
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
int
phy84834_timesync_control_set(int unit, soc_port_t port,
                              soc_port_control_phy_timesync_t type,
                              uint64 value)
{
    uint16 temp1, temp2; 
    uint32 value0;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_IS_BCM8488X(pc) || (! PHY_IS_BCM8483X_AND_UP(pc)) ) {
        return SOC_E_UNAVAIL;
    }

    switch (type) {

    case SOC_PORT_CONTROL_PHY_TIMESYNC_CAPTURE_TIMESTAMP:
    case SOC_PORT_CONTROL_PHY_TIMESYNC_HEARTBEAT_TIMESTAMP:
        return SOC_E_UNAVAIL;


    case SOC_PORT_CONTROL_PHY_TIMESYNC_NCOADDEND:
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                          PHY84834_PTP_NSE_NCO_CFG_REG, 
                          NCO_SHADOW_REG_STATIC_FCW,
                          PHY84834_PTP_NSE_NCO_CFG_FCW_SEL));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                          PHY84834_NSE_NCO_SHADOW_FCW_0_REG, 
                          (uint16)(COMPILER_64_LO(value) & 0xffff)));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                          PHY84834_NSE_NCO_SHADOW_FCW_1_REG, 
                          (uint16)((COMPILER_64_LO(value) >> 16) & 0xffff)));
        /* Enable loading of shadow reg on next framesync */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                          PHY84834_PTP_NSE_SHDW_CTL_REG, 
                          PHY84834_PTP_NSE_SHDW_CTL_LOAD_NCO_FCW,
                          PHY84834_PTP_NSE_SHDW_CTL_LOAD_NCO_FCW));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_FRAMESYNC:
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, PHY84834_PTP_NSE_SC_CFG_REG,
                          (uint16) ((COMPILER_64_LO(value) & 0x07) << 4),
                          PHY84834_PTP_NSE_SC_CFG_SI_MODE));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_LOCAL_TIME:
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_NSE_NCO_SHADOW_TIME_HIGH_0_REG, 
                                              (uint16)(COMPILER_64_LO(value) & 0xffff)));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_NSE_NCO_SHADOW_TIME_HIGH_1_REG, 
                                              (uint16)((COMPILER_64_LO(value) >> 16) & 0xffff)));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                                               PHY84834_NSE_NCO_SHADOW_TIME_HIGH_2_REG, 
                                               (uint16)COMPILER_64_HI(value), 0x0fff));
        /* Enable loading of shadow reg on next framesync */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_NSE_SHDW_CTL_REG, 
                                              PHY84834_PTP_NSW_SHDW_CTL_LOAD_COUNT_HIGH,
                                              PHY84834_PTP_NSW_SHDW_CTL_LOAD_COUNT_HIGH));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_LOAD_CONTROL:
        temp1 = temp2 = 0;
        value0 = COMPILER_64_LO(value);

        if (value0 &  SOC_PORT_PHY_TIMESYNC_TIMECODE_LOAD) {
            temp1 |= PHY84834_PTP_NSE_SHDW_CTL_LOAD_ORIG_TS;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_TIMECODE_ALWAYS_LOAD) {
            temp2 |= PHY84834_PTP_NSE_SHDW_CLR_LOAD_ORIG_TS;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_SYNCOUT_LOAD) {
            temp1 |= PHY84834_PTP_NSE_SHDW_CTL_LOAD_SYNCOUT;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_SYNCOUT_ALWAYS_LOAD) {
            temp2 |= PHY84834_PTP_NSE_SHDW_CLR_LOAD_SYNCOUT;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD) {
            temp1 |= PHY84834_PTP_NSE_SHDW_CTL_LOAD_SYNC_DIV;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_ALWAYS_LOAD) {
            temp2 |= PHY84834_PTP_NSE_SHDW_CLR_LOAD_SYNC_DIV;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_TN_LOAD) {
            temp1 |= PHY84834_PTP_NSW_SHDW_CTL_LOAD_NTP_TIME;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_TN_ALWAYS_LOAD) {
            temp2 |= PHY84834_PTP_NSW_SHDW_CLR_LOAD_NTP_TIME;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD) {
            temp1 |= PHY84834_PTP_NSW_SHDW_CTL_LOAD_TIME_COUNT;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD) {
            temp2 |= PHY84834_PTP_NSW_SHDW_CLR_LOAD_TIME_COUNT;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD) {
            temp1 |= PHY84834_PTP_NSE_SHDW_CTL_LOAD_NCO_FCW;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_ALWAYS_LOAD) {
            temp2 |= PHY84834_PTP_NSE_SHDW_CLR_LOAD_NCO_FCW;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD) {
            temp1 |= PHY84834_PTP_NSE_SHDW_CTL_LOAD_DPLL_FCW;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_ALWAYS_LOAD) {
            temp2 |= PHY84834_PTP_NSE_SHDW_CLR_LOAD_DPLL_FCW;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD) {
            temp1 |= PHY84834_PTP_NSE_SHDW_CTL_LOAD_REF_PHASE;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD) {
            temp2 |= PHY84834_PTP_NSE_SHDW_CLR_LOAD_REF_PHASE;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD) {
            temp1 |= PHY84834_PTP_NSE_SHDW_CTL_LOAD_REF_PERIOD;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD) {
            temp2 |= PHY84834_PTP_NSE_SHDW_CLR_LOAD_REF_PERIOD;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K3_LOAD) {
            temp1 |= PHY84834_PTP_NSE_SHDW_CTL_LOAD_DPLL_GAIN3;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD) {
            temp2 |= PHY84834_PTP_NSE_SHDW_CLR_LOAD_DPLL_GAIN3;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K2_LOAD) {
            temp1 |= PHY84834_PTP_NSE_SHDW_CTL_LOAD_DPLL_GAIN2;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD) {
            temp2 |= PHY84834_PTP_NSE_SHDW_CLR_LOAD_DPLL_GAIN2;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K1_LOAD) {
            temp1 |= PHY84834_PTP_NSE_SHDW_CTL_LOAD_DPLL_GAIN1;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD) {
            temp2 |= PHY84834_PTP_NSE_SHDW_CLR_LOAD_DPLL_GAIN1;
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                    PHY84834_PTP_NSE_SHDW_CTL_REG, temp1));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                    PHY84834_PTP_NSE_SHDW_CLR_REG, temp2));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT_MASK:
        temp1 = 0;
        value0 = COMPILER_64_LO(value);

        if (value0 &  SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT) {
            temp1 |= PHY84834_PTP_INT_PIC_EN;
        }
        if (value0 &  SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT) {
            temp1 |= PHY84834_PTP_INT_FSYNC_EN;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_INT_EN_REG, 
                                              temp1,
                                              (PHY84834_PTP_INT_PIC_EN |
                                               PHY84834_PTP_INT_FSYNC_EN)));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_TX_TIMESTAMP_OFFSET:
        value0 = COMPILER_64_LO(value);
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                               PHY84834_PTP_TS_EG_SOP_OFFSET_0_REG, 
                                               (uint16)(value0 & 0xffff))); 
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                               PHY84834_PTP_TS_EG_SOP_OFFSET_1_REG, 
                                               (uint16)((value0 >> 16) & 0xf))); 
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_RX_TIMESTAMP_OFFSET:
        value0 = COMPILER_64_LO(value);
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                               PHY84834_PTP_TS_IG_SOP_OFFSET_0_REG, 
                                               (uint16)(value0 & 0xffff))); 
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PMAD_REG(unit, pc, 
                                               PHY84834_PTP_TS_IG_SOP_OFFSET_1_REG, 
                                               (uint16)((value0 >> 16) & 0xf)));
        break;

    default:
        return SOC_E_UNAVAIL;
        break;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy84834_timesync_control_get
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
int
phy84834_timesync_control_get(int unit, soc_port_t port,
                              soc_port_control_phy_timesync_t type,
                              uint64 *value)
{
    uint16 value0 = 0;
    uint16 value1 = 0;
    uint16 value2 = 0;
    uint16 value3 = 0;
    uint32 value32;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_IS_BCM8488X(pc) || (! PHY_IS_BCM8483X_AND_UP(pc)) ) {
        return SOC_E_UNAVAIL;
    }

    switch (type) {
    case SOC_PORT_CONTROL_PHY_TIMESYNC_HEARTBEAT_TIMESTAMP:

        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                          PHY84834_PTP_NSE_SC_COUNTSTAMP_FSYNC_0_REG,
                          &value0));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                          PHY84834_PTP_NSE_SC_COUNTSTAMP_FSYNC_1_REG,
                          &value1));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                          PHY84834_PTP_NSE_SC_COUNTSTAMP_FSYNC_2_REG,
                          &value2));
        COMPILER_64_SET((*value), (((uint32)value3<<16)|((uint32)value2)),
                                  (((uint32)value1<<16)|((uint32)value0)));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_CAPTURE_TIMESTAMP:

        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                          PHY84834_PTP_PIC_FIFO_RD_PKT_TS_0_REG,
                          &value0));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                          PHY84834_PTP_PIC_FIFO_RD_PKT_TS_1_REG,
                          &value1));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc,
                          PHY84834_PTP_PIC_FIFO_RD_PKT_TS_2_REG,
                          &value2));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc,
                          PHY84834_PTP_PIC_FIFO_CTL_REG, 0x0001, 0x0001)); 

        COMPILER_64_SET((*value), (((uint32)value3<<16)|((uint32)value2)),
                                  (((uint32)value1<<16)|((uint32)value0)));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_NCOADDEND:
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                          PHY84834_NSE_NCO_SHADOW_FCW_0_REG, 
                          &value0));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                          PHY84834_NSE_NCO_SHADOW_FCW_1_REG, 
                          &value1));
        COMPILER_64_SET((*value), (((uint32)value3<<16)|((uint32)value2)),
                                  (((uint32)value1<<16)|((uint32)value0)));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_FRAMESYNC:
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, PHY84834_PTP_NSE_SC_CFG_REG,
                                             &value0));
        COMPILER_64_SET((*value),  0,
                        (value0 & (PHY84834_PTP_NSE_SC_CFG_SI_MODE)) >> 4);
        LOG_WARN(BSL_LS_SOC_PHY, (BSL_META_U(unit, "u=%d p=%d v0=0x%x v=0x%x\n"), unit,pc->port,value0,(COMPILER_64_LO(*value))));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_LOCAL_TIME:
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                          PHY84834_NSE_NCO_SHADOW_TIME_HIGH_0_REG, 
                          &value0)); 
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                          PHY84834_NSE_NCO_SHADOW_TIME_HIGH_1_REG, 
                          &value1)); 
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                          PHY84834_NSE_NCO_SHADOW_TIME_HIGH_2_REG, 
                          &value2)); 

        COMPILER_64_SET((*value), ((uint32)(value2 & 0x0fff)),
                                  (((uint32)value1<<16)|((uint32)value0)));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_LOAD_CONTROL:
        value32 = 0;
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                   PHY84834_PTP_NSE_SHDW_CTL_REG, &value1));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                   PHY84834_PTP_NSE_SHDW_CLR_REG, &value2));
        if (value1 & PHY84834_PTP_NSE_SHDW_CTL_LOAD_ORIG_TS) {
            value32 |= SOC_PORT_PHY_TIMESYNC_TIMECODE_LOAD;
        }
        if (value2 & PHY84834_PTP_NSE_SHDW_CLR_LOAD_ORIG_TS) {
            value32 |= SOC_PORT_PHY_TIMESYNC_TIMECODE_ALWAYS_LOAD;
        }
        if (value1 & PHY84834_PTP_NSE_SHDW_CTL_LOAD_SYNCOUT) {
            value32 |= SOC_PORT_PHY_TIMESYNC_SYNCOUT_LOAD;
        }
        if (value2 & PHY84834_PTP_NSE_SHDW_CLR_LOAD_SYNCOUT) {
            value32 |= SOC_PORT_PHY_TIMESYNC_SYNCOUT_ALWAYS_LOAD;
        }
        if (value1 & PHY84834_PTP_NSE_SHDW_CTL_LOAD_SYNC_DIV) {
            value32 |= SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD;
        }
        if (value2 & PHY84834_PTP_NSE_SHDW_CLR_LOAD_SYNC_DIV) {
            value32 |= SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_ALWAYS_LOAD;
        }
        if (value1 & PHY84834_PTP_NSW_SHDW_CTL_LOAD_NTP_TIME) {
            value32 |= SOC_PORT_PHY_TIMESYNC_TN_LOAD;
        }
        if (value2 & PHY84834_PTP_NSW_SHDW_CLR_LOAD_NTP_TIME) {
            value32 |= SOC_PORT_PHY_TIMESYNC_TN_ALWAYS_LOAD;
        }
        if (value1 & PHY84834_PTP_NSW_SHDW_CTL_LOAD_TIME_COUNT) {
            value32 |= SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD;
        }
        if (value2 & PHY84834_PTP_NSW_SHDW_CLR_LOAD_TIME_COUNT) {
            value32 |= SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD;
        }
        if (value1 & PHY84834_PTP_NSE_SHDW_CTL_LOAD_NCO_FCW) {
            value32 |= SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD;
        }
        if (value2 & PHY84834_PTP_NSE_SHDW_CLR_LOAD_NCO_FCW) {
            value32 |= SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_ALWAYS_LOAD;
        }
        if (value1 & PHY84834_PTP_NSE_SHDW_CTL_LOAD_DPLL_FCW ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD;
        }
        if (value2 & PHY84834_PTP_NSE_SHDW_CLR_LOAD_DPLL_FCW ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_ALWAYS_LOAD;
        }
        if (value1 & PHY84834_PTP_NSE_SHDW_CTL_LOAD_REF_PHASE) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD;
        }
        if (value2 & PHY84834_PTP_NSE_SHDW_CLR_LOAD_REF_PHASE) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD;
        }
        if (value1 & PHY84834_PTP_NSE_SHDW_CTL_LOAD_REF_PERIOD) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD;
        }
        if (value2 & PHY84834_PTP_NSE_SHDW_CLR_LOAD_REF_PERIOD) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD;
        }
        if (value1 & PHY84834_PTP_NSE_SHDW_CTL_LOAD_DPLL_GAIN3) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_K3_LOAD;
        }
        if (value2 & PHY84834_PTP_NSE_SHDW_CLR_LOAD_DPLL_GAIN3) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD;
        }
        if (value1 & PHY84834_PTP_NSE_SHDW_CTL_LOAD_DPLL_GAIN2) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_K2_LOAD;
        }
        if (value2 & PHY84834_PTP_NSE_SHDW_CLR_LOAD_DPLL_GAIN2) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD;
        }
        if (value1 & PHY84834_PTP_NSE_SHDW_CTL_LOAD_DPLL_GAIN1) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_K1_LOAD;
        }
        if (value2 & PHY84834_PTP_NSE_SHDW_CLR_LOAD_DPLL_GAIN1) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD;
        }
        COMPILER_64_SET((*value), 0, (uint32)value32);
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT:
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_INT_STATUS_REG, 
                                              &value1));
        value32 = 0;
        if (value1 & PHY84834_PTP_INT_STATUS_PIC) {
            /* Clear the interrupt status. Not self clearing! */
            SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_INT_CLEAR_REG, 
                                              PHY84834_PTP_INT_CLEAR_PIC,
                                              PHY84834_PTP_INT_CLEAR_PIC));
            value32 |= SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT;
        }
        if (value1 & PHY84834_PTP_INT_CLEAR_FSYNC) {
            /* Clear the interrupt status. Not self clearing! */
            SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_REG(unit, pc, 
                                              PHY84834_PTP_INT_CLEAR_REG, 
                                              PHY84834_PTP_INT_CLEAR_FSYNC,
                                              PHY84834_PTP_INT_CLEAR_FSYNC));
            value32 |= SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT;
        }
        COMPILER_64_SET((*value), 0, (uint32)value32);
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_TX_TIMESTAMP_OFFSET:
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                               PHY84834_PTP_TS_EG_SOP_OFFSET_0_REG, 
                                               &value0)); 
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                               PHY84834_PTP_TS_EG_SOP_OFFSET_1_REG, 
                                               &value1)); 
        COMPILER_64_SET((*value), 0, ((uint32)(value1 & 0xf) << 16) | (uint32)(value0 & 0x0fff));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_RX_TIMESTAMP_OFFSET:
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                               PHY84834_PTP_TS_IG_SOP_OFFSET_0_REG, 
                                               &value0)); 
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(unit, pc, 
                                               PHY84834_PTP_TS_IG_SOP_OFFSET_1_REG, 
                                               &value1)); 
        COMPILER_64_SET((*value), 0, ((uint32)(value1 & 0xf) << 16) | (uint32)(value0 & 0x0fff));
        break;

    default:
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_reg_read
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      port         - port number
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (OUT) Value read from PHY register
 */
STATIC int
phy_8481_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    phy_ctrl_t    *pc;
    uint32         reg_flags;
    uint16         reg_bank;
    uint8          reg_addr;
    uint16         data16;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        if ((SOC_PHY_CLAUSE45_DEVAD(phy_reg_addr) == 0x7) && 
           (SOC_PHY_CLAUSE45_REGAD(phy_reg_addr) >= 0xffe0)) {
            reg_flags = SOC_PHY_REG_FLAGS(phy_reg_addr);
            reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
            reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);
            /* handle gphy registers */ 
            SOC_IF_ERROR_RETURN
                (phy_8481_reg_xge_read(unit, pc, reg_flags, reg_bank, PHY8481_C45_DEV_AN, reg_addr, &data16));
        } else {
           SOC_IF_ERROR_RETURN
               (READ_PHY_REG(unit, pc, phy_reg_addr, &data16));
       }
       *phy_data = (uint32)data16;
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG3(pc,PHY_REG_READ,flags,phy_reg_addr,phy_data);
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_reg_write
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      port         - port number
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (IN) Value written to PHY register
 */
STATIC int
phy_8481_reg_write(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 phy_data)
{
    phy_ctrl_t    *pc;       /* PHY software state */
    uint32         reg_flags;
    uint16         reg_bank;
    uint8          reg_addr;
    uint16         data16;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        data16 = (uint16)(phy_data & 0xffff);

        if ((SOC_PHY_CLAUSE45_DEVAD(phy_reg_addr) == 0x7) && 
           (SOC_PHY_CLAUSE45_REGAD(phy_reg_addr) >= 0xffe0)) {
            reg_flags = SOC_PHY_REG_FLAGS(phy_reg_addr);
            reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
            reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);
            /* handle gphy registers */ 
            SOC_IF_ERROR_RETURN
                (phy_8481_reg_xge_write(unit, pc, reg_flags, reg_bank, PHY8481_C45_DEV_AN, reg_addr, data16));
        } else {
           SOC_IF_ERROR_RETURN
               (WRITE_PHY_REG(unit, pc, phy_reg_addr, data16));
       }
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG3(pc, PHY_REG_WRITE, flags, phy_reg_addr, phy_data);
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_reg_modify
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      port         - port number
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (IN) Value written to PHY register
 *      phy_data_mask    - (IN) Mask used for writing
 */
STATIC int
phy_8481_reg_modify(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 phy_data, uint32 phy_data_mask)
{
    phy_ctrl_t    *pc;       /* PHY software state */
    uint32         reg_flags;
    uint16         reg_bank;
    uint8          reg_addr;
    uint16         data16;
    uint16         mask16;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        data16 = (uint16)(phy_data & 0xffff);
        mask16 = (uint16)(phy_data_mask & 0xffff);

        if ((SOC_PHY_CLAUSE45_DEVAD(phy_reg_addr) == 0x7) && 
           (SOC_PHY_CLAUSE45_REGAD(phy_reg_addr) >= 0xffe0)) {
            reg_flags = SOC_PHY_REG_FLAGS(phy_reg_addr);
            reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
            reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);
            /* handle gphy registers */ 
            SOC_IF_ERROR_RETURN
                (phy_8481_reg_xge_modify(unit, pc, reg_flags, reg_bank, PHY8481_C45_DEV_AN, reg_addr, data16, mask16));
        } else {
           SOC_IF_ERROR_RETURN
               (MODIFY_PHY_REG(unit, pc, phy_reg_addr, data16, mask16));
       }
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG4(pc, PHY_REG_MODIFY, flags, phy_reg_addr, phy_data, phy_data_mask);
        }
    }
    return SOC_E_NONE;
}

int
phy_8481_reg_xge_read(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                      uint8 dev_addr, uint16 reg_addr, uint16 *data)
{
    uint32  value;
    uint16  tmp, tmp2 = 0;
    uint16  reg_group, bit_shift;
    int     rv = SOC_E_NONE;
 
    /* Atomic read/write way - new FW */ 
    if ( FLAGS(pc) & PHY8483X_ATOMIC_ACCESS_SHD ) {
        switch(reg_addr) {
        /* Map shadow registers */
        case 0xfff5:
            reg_group = REG_GROUP_SHADOW_MISC_REG;
            bit_shift = 0x0;
            break;
        case 0xfff8:
            reg_group = REG_GROUP_SHADOW18;
            bit_shift = 0x0;
            break;
        case 0xfffc:
            reg_group = REG_GROUP_SHADOW1c;
            bit_shift = 0x0;
            break;
        case 0xfffd:
            reg_group = REG_GROUP_SHADOW_EXP_REG;
            bit_shift = 0x0;
            break;
        default:
            if (!(flags & SOC_PHY_REG_RESERVE_ACCESS)) {
                /* Must not read from reserved registers */
                if (reg_addr > 0xfffe) {
                   rv = SOC_E_PARAM;
                }
            }
            reg_group = REG_GROUP_SHADOW_EXP_REG;
            bit_shift = 0x0;
            if (SOC_SUCCESS(rv)) {
                rv = READ_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), data);
                return rv;
            }
            break;
        }
        if (SOC_SUCCESS(rv)) {
            if(!(flags & SOC_PHY_REG_DUMMY_ACCESS)) {
                rv =  _phy8485x_shadow_reg_read(unit, pc, reg_group, reg_bank, 0xffff , bit_shift, data);
            } else {

                reg_group = REG_GROUP_SHADOW_EXP_REG;
                bit_shift = 0x0;

                do {
                    rv =  _phy8485x_shadow_reg_read(unit, pc, reg_group, reg_bank & 0xff , 0xffff , bit_shift, &tmp);
                    if (SOC_FAILURE(rv)) {
                        break;
                    }
                    rv =  _phy8485x_shadow_reg_read(unit, pc, reg_group, reg_bank & 0xff , 0xffff , bit_shift, &tmp);
                    if (SOC_FAILURE(rv)) {
                        break;
                    }
                    rv =  _phy8485x_shadow_reg_read(unit, pc, reg_group, reg_bank & 0xff , 0xffff , bit_shift, &tmp);
                    if (SOC_FAILURE(rv)) {
                        break;
                    }
                    rv =  _phy8485x_shadow_reg_read(unit, pc, reg_group, reg_bank & 0xff , 0xffff , bit_shift, &tmp2);
                } while(0);

                value = tmp2;
                value <<= 16; /*upper count*/
                value |= tmp;
                * ((uint32 *) data) = value;
            }
        }
    }
    else {  /* (! PHY8483X_ATOMIC_ACCESS_SHD), old way if FW is old */
        rv = READ_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), data);
    }

    if ( SOC_FAILURE(rv) ) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                    "phy_8481_reg_xge_read failed:"
                    " u=%d phy_id=0x%2x dev_addr=0x%02x reg_bank=0x%04x reg_addr=0x%02x" 
                    " rv=%d\n"), unit, pc->phy_id, dev_addr, reg_bank, reg_addr, rv));
    }
    return rv;
}

int
phy_8481_reg_xge_write(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                       uint8 dev_addr, uint16 reg_addr, uint16 data)
{
    uint16  reg_group, bit_shift;
    int     rv = SOC_E_NONE;

    if ( SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit) ) {
        return rv;
    }

    /* Atomic read/write way - new FW */
    if ( FLAGS(pc) & PHY8483X_ATOMIC_ACCESS_SHD ) {
        /* Map shadow registers */
        switch(reg_addr) {
        case 0xfff5:
            reg_group = REG_GROUP_SHADOW_MISC_REG;
            bit_shift = 0x0;
            break;
        case 0xfff8:
            reg_group = REG_GROUP_SHADOW18;
            bit_shift = 0x0;
            break;
        case 0xfffc:
            reg_group = REG_GROUP_SHADOW1c;
            bit_shift = 0x0;
            break;
        case 0xfffd:
            reg_group = REG_GROUP_SHADOW_EXP_REG;
            bit_shift = 0x0;
            break;
        default:
            if ( ! (flags & SOC_PHY_REG_RESERVE_ACCESS) ) {
                /* Must not write to reserved registers */
                if (reg_addr > 0xfffe) {
                    rv = SOC_E_PARAM;
                }
            }
            reg_group = REG_GROUP_SHADOW_EXP_REG;
            bit_shift = 0x0;
            if ( SOC_SUCCESS(rv) ) {
                rv = WRITE_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), data);
                return rv;
            }
            break;
        }
        if (SOC_SUCCESS(rv)) {
            rv = _phy8485x_shadow_reg_write(unit, pc, reg_group, reg_bank, 0xffff , bit_shift, data);
        }
    }
    else {  /* (! PHY8483X_ATOMIC_ACCESS_SHD), old way if FW is old */
        rv = WRITE_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), data);
    }

    if (SOC_FAILURE(rv)) {
        if ( SOC_FAILURE(rv) ) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                    "phy_8481_reg_xge_write failed:"
                    " u=%d phy_id=0x%2x dev_addr=0x%02x reg_bank=0x%04x reg_addr=0x%02x"                        
                    " rv=%d\n"), unit, pc->phy_id, dev_addr, reg_bank, reg_addr, rv));
        }
    }

    PHY8481_D7_FFXX_REG_VERIFY(unit, pc, flags,  reg_bank, reg_addr);
    return rv;
}

int
phy_8481_reg_xge_modify(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                uint8 dev_addr, uint16 reg_addr, uint16 data, uint16 mask)
{
    int     rv;
    uint16 reg_group, bit_shift;

    rv       = SOC_E_NONE;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)) {
        return rv;
    }

    /* Atomic read/write way - new FW */
    if (FLAGS(pc) & PHY8483X_ATOMIC_ACCESS_SHD) {
        switch(reg_addr) {
        /* Map shadow registers */
        case 0xfff5:
            reg_group = REG_GROUP_SHADOW_MISC_REG;
            bit_shift = 0x0;
            break;
        case 0xfff8:
            reg_group = REG_GROUP_SHADOW18;
            bit_shift = 0x0;
            if (reg_bank == 0x0007) {
                data |= 0x8000;
                mask |= 0x8000;
            }
            break;
        case 0xfffc:
            reg_group = REG_GROUP_SHADOW1c;
            bit_shift = 0x0;
            break;
        case 0xfffd:
            reg_group = REG_GROUP_SHADOW_EXP_REG;
            bit_shift = 0x0;
            break;
        default:
            if (!(flags & SOC_PHY_REG_RESERVE_ACCESS)) {
                /* Must not write to reserved registers */
                if (reg_addr > 0xfffe) {
                    rv = SOC_E_PARAM;
                }
            }
            reg_group = REG_GROUP_SHADOW_EXP_REG;
            bit_shift = 0x0;
            if (SOC_SUCCESS(rv)) {
                rv = MODIFY_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), data, mask);
                return rv;
            }
            break;
        }
        if (SOC_SUCCESS(rv)) {
            rv = _phy8485x_shadow_reg_write(unit, pc, reg_group, reg_bank, mask , bit_shift, data);
        }
    }
    else {  /* (! PHY8483X_ATOMIC_ACCESS_SHD), old way if FW is old */
        rv = MODIFY_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), data, mask);
    }

    if (SOC_FAILURE(rv)) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                    "phy_8481_reg_xge_modify failed:"
                    " u=%d phy_id=0x%2x dev_addr=0x%02x reg_bank=0x%04x reg_addr=0x%02x"
                    " rv=%d\n"), unit, pc->phy_id, dev_addr, reg_bank, reg_addr, rv)); 
    }

    PHY8481_D7_FFXX_REG_VERIFY( unit, pc, flags,  reg_bank, reg_addr);
    return rv;
}


#ifdef PHY8481_FW_DEBUG
int
phy_8481_fw_reg_dump(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    uint16      config, status;
    char *mac_mode, *line_mode, *cu_speed, *fiber_speed, *line_device, *mac_device, *crc_status; 

    pc       = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_TOP_CONFIG_XGPHY_STRAP1_REG(unit, pc, &config));

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_TOP_CONFIG_LINK_STATUS_REG(unit, pc, &status));

    LOG_CLI((BSL_META_U(unit,
                        "TOP_CONFIG_XGPHY_STRAP1_REG = 0x%04x\n"), config));

    switch ((config >> 3) & 0x3) {
    case 0:
        mac_mode = "00 - XAUI_M - Disabled, XFI - Enabled";
        break;
    case 1:
        mac_mode = "01 - XAUI_M - Enabled, XFI - Disabled";
        break;
    case 2:
        mac_mode = "10 - RESERVED";
        break;
    case 3:
        mac_mode = "11 - RESERVED";
        break;
    default:
        mac_mode = '\0';
        break;
    }

    switch ((config >> 5) & 0x3) {
    case 0:
        line_mode = "00 - XAUI_L - Disabled, XFI - Disabled";
        break;
    case 1:
        line_mode = "01 - XAUI_L - Enabled (Autodetect with Cu), XFI - Disabled";
        break;
    case 2:
        line_mode = "10 - XAUI_L - Disabled, XFI - Enabled (Autodetect with Cu)";
        break;
    case 3:
        line_mode = "11 - RESERVED";
        break;
    default:
        line_mode = '\0';
        break;
    }

    /* Print the config results */
    LOG_CLI((BSL_META_U(unit,
                        "MAC  mode        = %s\n"), mac_mode));

    LOG_CLI((BSL_META_U(unit,
                        "LINE mode        = %s\n"), line_mode));

    LOG_CLI((BSL_META_U(unit,
                        "CU PWRDN         = %s\n"), (config & (1U<<7)) ? "Yes" : "No"));

    LOG_CLI((BSL_META_U(unit,
                        "FIBER Prio       = %s\n"), (config & (1U<<8)) ? "Yes" : "No"));

    LOG_CLI((BSL_META_U(unit,
                        "SD Invert        = %s\n"), (config & (1U<<9)) ? "Yes" : "No"));

    LOG_CLI((BSL_META_U(unit,
                        "RANGE            = %s\n"), (config & (1U<<10)) ? "30M" : "100M"));

    LOG_CLI((BSL_META_U(unit,
                        "LED User Control = %s\n"), (config & (1U<<11)) ? "Yes" : "No"));


    LOG_CLI((BSL_META_U(unit,
                        "\nTOP_CONFIG_LINK_STATUS (400d) = 0x%04x\n"), status));

    switch ((status >> 3) & 0x3) {
    case 0:
        cu_speed = "00 - 10M";
        break;
    case 1:
        cu_speed = "01 - 100M";
        break;
    case 2:
        cu_speed = "10 - 1G";
        break;
    case 3:
        cu_speed = "11 - 10G";
        break;
    default:
        cu_speed = '\0';
        break;
    }

    switch ((status >> 6) & 0x3) {
    case 0:
        fiber_speed = "00 - 10M";
        break;
    case 1:
        fiber_speed = "01 - 100M";
        break;
    case 2:
        fiber_speed = "10 - 1G";
        break;
    case 3:
        fiber_speed = "11 - 10G";
        break;
    default:
        fiber_speed = '\0';
        break;
    }

    switch ((status >> 9) & 0x3) {
    case 0:
        mac_device = "00 - Reserved";
        break;
    case 1:
        mac_device = "01 - XAUI";
        break;
    case 2:
        mac_device = "10 - XFI";
        break;
    case 3:
        mac_device = "11 - Reserved";
        break;
    default:
        mac_device = '\0';
        break;
    }

    switch ((status >> 11) & 0x3) {
    case 0:
        line_device = "00 - CU";
        break;
    case 1:
        line_device = "01 - XAUI";
        break;
    case 2:
        line_device = "10 - XFI";
        break;
    case 3:
        line_device = "11 - Reserved";
        break;
    default:
        line_device = '\0';
        break;
    }

    switch ((status >> 14) & 0x3) {
    case 0:
        crc_status = "00 - CRC check going on (May indicate ARM hang)";
        break;
    case 1:
        crc_status = "01 - Good CRC";
        break;
    case 2:
        crc_status = "10 - Bad CRC";
        break;
    case 3:
        crc_status = "11 - Reserved";
        break;
    default:
        crc_status = '\0';
        break;
    }

    /* Print the status results */

    LOG_CLI((BSL_META_U(unit,
                        "FIBER Detected   = %s\n"), (status & (1U<<0)) ? "Yes" : "No"));

    LOG_CLI((BSL_META_U(unit,
                        "CU Detected      = %s\n"), (status & (1U<<1)) ? "Yes" : "No"));

    LOG_CLI((BSL_META_U(unit,
                        "FIBER Prio       = %s\n"), (status & (1U<<2)) ? "Yes" : "No"));

    LOG_CLI((BSL_META_U(unit,
                        "CU  speed        = %s\n"), cu_speed));

    LOG_CLI((BSL_META_U(unit,
                        "CU Link          = %s\n"), (status & (1U<<5)) ? "Yes" : "No"));

    LOG_CLI((BSL_META_U(unit,
                        "Fiber  speed     = %s\n"), fiber_speed));

    LOG_CLI((BSL_META_U(unit,
                        "Fiber Link       = %s\n"), (status & (1U<<8)) ? "Yes" : "No"));

    LOG_CLI((BSL_META_U(unit,
                        "MAC  device      = %s\n"), mac_device));

    LOG_CLI((BSL_META_U(unit,
                        "LINE device      = %s\n"), line_device));

    LOG_CLI((BSL_META_U(unit,
                        "MAC Link Up      = %s\n"), (status & (1U<<13)) ? "Yes" : "No"));

    LOG_CLI((BSL_META_U(unit,
                        "CRC Status       = %s\n"), crc_status));

    return SOC_E_NONE;
}
#endif

/*
 * Variable:
 *    phy_8481_drv
 * Purpose:
 *    Phy Driver for 10G (XAUI x 4) Serdes PHY. 
 */

phy_driver_t phy_8481drv_xe = {
    "8481 10-Gigabit PHY Driver",
    phy_8481_init,        /* Init */
    phy_null_reset,       /* Reset */
    phy_8481_link_get,    /* Link get   */
    phy_8481_enable_set,  /* Enable set */
    phy_8481_enable_get,  /* Enable get */
    phy_8481_duplex_set,  /* Duplex set */
    phy_8481_duplex_get,  /* Duplex get */
    phy_8481_speed_set,   /* Speed set  */
    phy_8481_speed_get,   /* Speed get  */
    phy_8481_master_set,  /* Master set */
    phy_8481_master_get,  /* Master get */
    phy_8481_an_set,      /* ANA set */
    phy_8481_an_get,      /* ANA get */
    NULL,                 /* Local Advert set, deprecated */
    NULL,                 /* Local Advert get, deprecated */
    NULL,                 /* Remote Advert get, deprecated */
    phy_8481_lb_set,      /* PHY loopback set */
    phy_8481_lb_get,      /* PHY loopback set */
    phy_null_interface_set, /* IO Interface set */
    phy_8481_xehg_interface_get, /* IO Interface get */
    NULL,                   /* pd_ability, deprecated */
    phy_8481_link_up,
    phy_8481_link_down,
    phy_8481_mdix_set,
    phy_8481_mdix_get,
    phy_8481_mdix_status_get,
    phy_8481_medium_config_set, /* medium config setting set */
    phy_8481_medium_config_get, /* medium config setting get */
    phy_8481_medium_status,        /* active medium */
    phy_8481_cable_diag,           /* phy_cable_diag  */
    NULL,                    /* phy_link_change */
    phy_8481_control_set,    /* phy_control_set */
    phy_8481_control_get,    /* phy_control_get */
    phy_8481_reg_read,       /* phy_reg_read */
    phy_8481_reg_write,      /* phy_reg_write */
    phy_8481_reg_modify,     /* phy_reg_modify */
    NULL,                    /* phy_notify */
    phy_8481_probe,          /* pd_probe  */
    phy_8481_ability_advert_set,  /* pd_ability_advert_set */
    phy_8481_ability_advert_get,  /* pd_ability_advert_get */
    phy_8481_ability_remote_get,  /* pd_ability_remote_get */
    phy_8481_ability_local_get,   /* pd_ability_local_get  */
    phy_8481_firmware_set,
    phy84834_timesync_config_set,
    phy84834_timesync_config_get,
    phy84834_timesync_control_set,
    phy84834_timesync_control_get,
    phy_8481_diag_ctrl             /* pd_diag_ctrl */
};

#else /* INCLUDE_PHY_8481 */
typedef int _phy_8481_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_8481 */

