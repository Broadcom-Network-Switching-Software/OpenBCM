/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains the XMOD API prototypes, structres and defines.
 * This file is intended to be used by both the host software and device fw.
 */

#ifndef __PHY8806X_XMOD_API_H__
#define __PHY8806X_XMOD_API_H__

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_debug.h>
#include <phymod/phymod_util.h>


/* XMOD API parameter types */
#define IN          /* SDK to FW */
#define OUT         /* FW to SDK */
#define INOUT       /* SDK to FW to SDK */

#define XMOD_IN_BUFFER_AREA                         0x7FD00
#define XMOD_OUT_BUFFER_AREA                        0x7FE00
#define XMOD_BUFFER_MAX_LEN                         256

#ifndef COMPILER_ATTRIBUTE
#define COMPILER_ATTRIBUTE(_a)    __attribute__ (_a)
#endif /* COMPILER_ATTRIBUTE */

#define PORT_LINE_SIDE_CTRL (0)
#define PORT_SYS_SIDE_CTRL  (0x100)

typedef enum xmod_error_e {
    XMOD_E_NONE_PROCESSING  = 1,
    XMOD_E_NONE             = 0,
    XMOD_E_INTERNAL         = -1,
    XMOD_E_MEMORY           = -2,
    XMOD_E_IO               = -3,
    XMOD_E_PARAM            = -4,
    XMOD_E_CORE             = -5,
    XMOD_E_PHY              = -6,
    XMOD_E_BUSY             = -7,
    XMOD_E_FAIL             = -8,
    XMOD_E_TIMEOUT          = -9,
    XMOD_E_RESOURCE         = -10,
    XMOD_E_CONFIG           = -11,
    XMOD_E_UNAVAIL          = -12,
    XMOD_E_INIT             = -13,
    XMOD_E_LIMIT            = -14           /* Must come last */
} xmod_error_t;

/* XMOD category and command list */
#define XMOD_CORE_CMDS                      0
#define XMOD_CORE_IDENTIFY                          0x00
#define XMOD_CORE_INFO_GET                          0x02
#define XMOD_CORE_LANE_MAP_GET                      0x04
#define XMOD_CORE_LANE_MAP_SET                      0x05
#define XMOD_CORE_RESET_GET                         0x06
#define XMOD_CORE_RESET_SET                         0x07
#define XMOD_CORE_FIRMWARE_INFO_GET                 0x08
#define XMOD_PHY_FIRMWARE_CORE_CONFIG_GET           0x0A
#define XMOD_PHY_FIRMWARE_CORE_CONFIG_SET           0x0B
#define XMOD_CORE_INIT                              0x0D
#define XMOD_CORE_PLL_SEQUENCER_RESTART             0x0F
#define XMOD_CORE_WAIT_EVENT                        0x10
#define XMOD_DEV_REG_GET                            0x12
#define XMOD_DEV_REG_SET                            0x13
#define XMOD_DEV_DEBUG_BUF_GET                      0x14
#define XMOD_CMD_ACKNOWLEDGE                        0x17
#define XMOD_CMD_ABORT                              0x19
#define XMOD_DEV_DEBUG_CMD                          0x1b
#define XMOD_DEV_CTR_INTERVAL_SET                   0x1d
#define XMOD_DEV_DEBUG_ETHER_CMD                    0x1e
#define XMOD_DEV_I2C_GET                            0x20
#define XMOD_DEV_I2C_SET                            0x21
#define XMOD_CORE_CMDS_COUNT                        0x22    /* LAST ENTRY */

#define XMOD_ETHERNET_CMDS                  1
#define XMOD_PHY_FIRMWARE_LANE_CONFIG_GET           0x00
#define XMOD_PHY_FIRMWARE_LANE_CONFIG_SET           0x01
#define XMOD_PHY_INIT                               0x03
#define XMOD_PHY_RX_RESTART                         0x05
#define XMOD_PHY_POLARITY_GET                       0x06
#define XMOD_PHY_POLARITY_SET                       0x07
#define XMOD_PHY_TX_GET                             0x08
#define XMOD_PHY_TX_SET                             0x09
#define XMOD_PHY_MEDIA_TYPE_TX_GET                  0x0A
#define XMOD_PHY_TX_OVERRIDE_GET                    0x0C
#define XMOD_PHY_TX_OVERRIDE_SET                    0x0D
#define XMOD_PHY_RX_GET                             0x10
#define XMOD_PHY_RX_SET                             0x11
#define XMOD_PHY_RESET_GET                          0x12
#define XMOD_PHY_RESET_SET                          0x13
#define XMOD_PHY_POWER_GET                          0x14
#define XMOD_PHY_POWER_SET                          0x15
#define XMOD_PHY_TX_LANE_CONTROL_GET                0x16
#define XMOD_PHY_TX_LANE_CONTROL_SET                0x17
#define XMOD_PHY_RX_LANE_CONTROL_GET                0x18
#define XMOD_PHY_RX_LANE_CONTROL_SET                0x19
#define XMOD_PHY_FEC_ENABLE_GET                     0x1A
#define XMOD_PHY_FEC_ENABLE_SET                     0x1B
#define XMOD_PHY_INTERFACE_CONFIG_GET               0x1C
#define XMOD_PHY_INTERFACE_CONFIG_SET               0x1D
#define XMOD_PHY_CL72_GET                           0x1E
#define XMOD_PHY_CL72_SET                           0x1F
#define XMOD_PHY_CL72_STATUS_GET                    0x20
#define XMOD_PHY_AUTONEG_ABILITY_GET                0x22
#define XMOD_PHY_AUTONEG_ABILITY_SET                0x23
#define XMOD_PHY_AUTONEG_REMOTE_ABILITY_GET         0x24
#define XMOD_PHY_AUTONEG_GET                        0x26
#define XMOD_PHY_AUTONEG_SET                        0x27
#define XMOD_PHY_AUTONEG_STATUS_GET                 0x28
#define XMOD_PHY_LOOPBACK_GET                       0x2A
#define XMOD_PHY_LOOPBACK_SET                       0x2B
#define XMOD_PHY_RX_PMD_LOCKED_GET                  0x2C
#define XMOD_PHY_LINK_STATUS_GET                    0x2E
#define XMOD_PHY_STATUS_DUMP                        0x30
#define XMOD_PHY_PCS_USERSPEED_GET                  0x32
#define XMOD_PHY_PCS_USERSPEED_SET                  0x33
#define XMOD_PORT_INIT                              0x35
#define XMOD_PHY_AN_TRY_ENABLE                      0x37
#define XMOD_PE_INIT                                0x39
#define XMOD_PE_STATUS_GET                          0x3a
#define XMOD_PE_PORT_STATS_GET                      0x3c
#define XMOD_PE_PCID_SET                            0x3f
#define XMOD_PE_FW_VER_GET                          0x40
#define XMOD_PORT_ENABLE_GET                        0x42
#define XMOD_PORT_ENABLE_SET                        0x43
#define XMOD_PHY_FORCED_SPEED_LINE_SIDE_GET         0x44
#define XMOD_PHY_FORCED_SPEED_LINE_SIDE_SET         0x45
#define XMOD_PHY_FLOW_CONTROL_MODE_GET              0x46
#define XMOD_PHY_FLOW_CONTROL_MODE_SET              0x47
/* In EBE mode, this is used to get the USE_IP_COS setting in case of PFE*/
#define XMOD_PHY_PSM_COS_BMP_GET                    0x48
/* In EBE mode, this is used to set the USE_IP_COS in case of PFE */
#define XMOD_PHY_PSM_COS_BMP_SET                    0x49
#define XMOD_PHY_AUTONEG_LINE_SIDE_GET              0x4a
#define XMOD_PHY_AUTONEG_LINE_SIDE_SET              0x4b
#define XMOD_PHY_RESET_INTF                         0x4d
#define XMOD_PHY_FW_LANE_CFG_GET                    0x4e
#define XMOD_PHY_FW_LANE_CFG_SET                    0x4f
#define XMOD_ETHERNET_CMDS_COUNT                    0x50   /* LAST ENTRY */

#define XMOD_FC_CMDS                        2
#define XMOD_BFCMAP_PORT_CONFIG_GET                 0x00
#define XMOD_BFCMAP_PORT_CONFIG_SET                 0x01
#define XMOD_BFCMAP_PORT_CMD_SET                    0x03
#define XMOD_BFCMAP_PORT_SPEED_SET                  0x05
#define XMOD_BFCMAP_PORT_LINK_FAULT_TRIGGER_RC_GET  0x06
#define XMOD_BFCMAP_PORT_DIAG_GET                   0x08
#define XMOD_BFCMAP_PORT_ABIL_GET                   0x0A
#define XMOD_BFCMAP_PORT_ABIL_SET                   0x0B
#define XMOD_BFCMAP_VLAN_MAP_GET                    0x0C
#define XMOD_BFCMAP_VLAN_MAP_ADD                    0x0D
#define XMOD_BFCMAP_VLAN_MAP_DELETE                 0x0F
#define XMOD_BFCMAP_PORT_INIT                       0x11
#define XMOD_FC_CMDS_COUNT                          0x12    /* LAST ENTRY */


#define XMOD_CPRI_CMDS                      3
#define XMOD_CPRI_CMDS_COUNT                        0x00    /* LAST ENTRY */

#define XMOD_MAX_LANES_PER_CORE             8
#define XMOD_NUM_DFE_TAPS                   5 /* MAX num of DFE TAPS */

#define WRITE_XMOD_ARG_BUFF(_xmodbuf, _argbuff, _arglen) \
    { \
	PHYMOD_MEMCPY((void*)_xmodbuf, (void*)_argbuff, _arglen); \
	_xmodbuf+=_arglen; \
    }
#define READ_XMOD_ARG_BUFF(_xmodbuf, _argbuff, _arglen) \
    { \
	PHYMOD_MEMCPY((void*)_argbuff, (void*)_xmodbuf, _arglen); \
	_xmodbuf+=_arglen; \
    }
#define XMOD_ARG_BUFF_ADJUST(_xmodbuf, _arglen) \
    { \
	_xmodbuf+=_arglen; \
    }

#define XMOD_CMD_MODE_CORE(cmd)  (cmd)
#define XMOD_CMD_MODE_ETH(cmd)   (0x100 | cmd)
#define XMOD_CMD_MODE_FC(cmd)    (0x200 | cmd)
#define XMOD_CMD_MODE_CPRI(cmd)  (0x300 | cmd)

#define XMOD_FLAGS_SHIFT 11
#define XMOD_FLAGS_MASK  0xf
#define XMOD_SYS_FLAG    1
#define XMOD_LINE_FLAG   2

/*!
 * @enum xmod_loopback_mode_e
 * @brief loopback mode
 */
typedef enum xmod_loopback_mode_e {
    xmodLoopbackGlobal = 0,
    xmodLoopbackGlobalPMD,
    xmodLoopbackGlobalPCS,
    xmodLoopbackRemotePMD,
    xmodLoopbackRemotePCS,
    xmodLoopbackSysGlobal,
    xmodLoopbackSysGlobalPMD,
    xmodLoopbackSysGlobalPCS,
    xmodLoopbackSysRemotePMD,
    xmodLoopbackSysRemotePCS,
    xmodLoopbackCount
} xmod_loopback_mode_td;
typedef uint8_t xmod_loopback_mode_t;

/*!
 * @enum xmod_core_version_e
 * @brief core version
 */
typedef enum xmod_core_version_e {
    xmodCoreVersionFalconA0 = 0,
    xmodCoreVersionEagleA0,
    xmodCoreVersionQsgmiieA0,
    xmodCoreVersionTsce4A0,
    xmodCoreVersionTsce12A0,
    xmodCoreVersionTscfA0,
    xmodCoreVersionTscfB0,
    xmodCoreVersionMT2A0,
    xmodCoreVersionCount
} xmod_core_version_td;
typedef uint8_t xmod_core_version_t;

/*!
 * @enum xmod_reset_mode_e
 * @brief Reset modes
 */
typedef enum xmod_reset_mode_e {
    xmodResetModeHard = 0,  /* Hard Reset */
    xmodResetModeSoft,      /* Soft Reset */
    xmodResetModeCount
} xmod_reset_mode_td;
typedef uint8_t xmod_reset_mode_t;

/*!
 * @enum xmod_reset_direction_e
 * @brief Reset modes
 */
typedef enum xmod_reset_direction_e {
    xmodResetDirectionIn = 0,   /* In Reset */
    xmodResetDirectionOut,      /* Out of Reset */
    xmodResetDirectionInOut,    /* Toggle Reset */
    xmodResetDirectionCount
} xmod_reset_direction_td;
typedef uint8_t xmod_reset_direction_t;

/*!
 * @enum xmod_firmware_media_type_e
 * @brief media_type for firmware
 */
typedef enum xmod_firmware_media_type_e {
    xmodFirmwareMediaTypePcbTraceBackPlane = 0, /* back plane */
    xmodFirmwareMediaTypeCopperCable,           /* copper cable */
    xmodFirmwareMediaTypeOptics,                /* optical */
    xmodFirmwareMediaTypeCount
} xmod_firmware_media_type_td;
typedef uint8_t xmod_firmware_media_type_t;

/*!
 * @enum xmod_media_typed_e
 * @brief media types
 */
typedef enum xmod_media_typed_e {
    xmodMediaTypeChipToChip = 0,
    xmodMediaTypeShort,
    xmodMediaTypeMid,
    xmodMediaTypeLong,
    xmodMediaTypeCount
} xmod_media_typed_td;
typedef uint8_t xmod_media_typed_t;

/*!
 * @enum xmod_sequencer_operation_e
 * @brief Sequencer operations
 */
typedef enum xmod_sequencer_operation_e {
    xmodSeqOpStop = 0,  /* Stop Sequencer */
    xmodSeqOpStart,     /* Start Sequencer */
    xmodSeqOpRestart,   /* Toggle Sequencer */
    xmodSeqOpCount
} xmod_sequencer_operation_td;
typedef uint8_t xmod_sequencer_operation_t;

/*!
 * @enum xmod_core_event_e
 * @brief core event
 */
typedef enum xmod_core_event_e {
    xmodCoreEventPllLock = 0, /* PLL locked */
    xmodCoreEventCount
} xmod_core_event_td;
typedef uint8_t xmod_core_event_t;

/*!
 * @enum xmod_power_e
 * @brief power modes
 */
typedef enum xmod_power_e {
    xmodPowerOff = 0,   /* turn off power */
    xmodPowerOn,        /* turn on power */
    xmodPowerOffOn,     /* toggle power */
    xmodPowerNoChange,  /* stay where you are */
    xmodPowerCount
} xmod_power_td;
typedef uint8_t xmod_power_t;

/*!
 * @enum xmod_interface_e
 * @brief phy interface
 */
typedef enum xmod_interface_e {
    xmodInterfaceBypass = 0,
    xmodInterfaceSR,
    xmodInterfaceSR4,
    xmodInterfaceKX,
    xmodInterfaceKX4,
    xmodInterfaceKR,
    xmodInterfaceKR2,
    xmodInterfaceKR4,
    xmodInterfaceCX,
    xmodInterfaceCX2,
    xmodInterfaceCX4,
    xmodInterfaceCR,
    xmodInterfaceCR2,
    xmodInterfaceCR4,
    xmodInterfaceCR10,
    xmodInterfaceXFI,
    xmodInterfaceSFI,
    xmodInterfaceSFPDAC,
    xmodInterfaceXGMII,
    xmodInterface1000X,
    xmodInterfaceSGMII,
    xmodInterfaceRXAUI,
    xmodInterfaceX2,
    xmodInterfaceXLAUI,
    xmodInterfaceXLAUI2,
    xmodInterfaceCAUI,
    xmodInterfaceQSGMII,
    xmodInterfaceCount
} xmod_interface_td;
typedef uint8_t xmod_interface_t;

/*!
 * @enum xmod_ref_clk_e
 * @brief Reference Clock
 */
typedef enum xmod_ref_clk_e {
    xmodRefClk156Mhz = 0,   /* 156.25MHz */
    xmodRefClk125Mhz,       /* 125MHz */
    xmodRefClkCount
} xmod_ref_clk_td;
typedef uint8_t xmod_ref_clk_t;

/*!
 * @enum xmod_an_mode_type_e
 * @brief an mode type
 */
typedef enum xmod_an_mode_type_e {
    xmod_AN_MODE_NONE = 0,
    xmod_AN_MODE_CL73,
    xmod_AN_MODE_CL37,
    xmod_AN_MODE_CL73BAM,
    xmod_AN_MODE_CL37BAM,
    xmod_AN_MODE_HPAM,
    xmod_AN_MODE_SGMII,
    xmod_AN_MODE_Count
} xmod_an_mode_type_td;
typedef uint8_t xmod_an_mode_type_t;

/*!
 * @enum xmod_cl37_sgmii_speed_e
 * @brief cl37 sgmii speed type
 */
typedef enum xmod_cl37_sgmii_speed_e {
    xmod_CL37_SGMII_10M = 0,
    xmod_CL37_SGMII_100M,
    xmod_CL37_SGMII_1000M,
    xmod_CL37_SGMII_Count
} xmod_cl37_sgmii_speed_td;
typedef uint8_t xmod_cl37_sgmii_speed_t;

/*!
 * @enum xmod_pcs_userspeed_mode_e
 * @brief modes for xmod_phy_pcs_userspeed API
 */
typedef enum xmod_pcs_userspeed_mode_e {
    xmodPcsUserSpeedModeST = 0, /* PCS Sw Table */
    xmodPcsUserSpeedModeHTO,    /* PCS Hw Table Override */
    xmodPcsUserSpeedModeCount
} xmod_pcs_userspeed_mode_td;
typedef uint8_t xmod_pcs_userspeed_mode_t;

/*!
 * @enum xmod_pcs_userspeed_param_e
 * @brief parameters for xmod_phy_pcs_userspeed API
 */
typedef enum xmod_pcs_userspeed_param_e {
    xmodPcsUserSpeedParamEntry = 0,     /* ST current entry */
    xmodPcsUserSpeedParamHCD,           /* ST HCD */
    xmodPcsUserSpeedParamClear,         /* HTO Clear */
    xmodPcsUserSpeedParamPllDiv,        /* ST/HTO PLL DIV */
    xmodPcsUserSpeedParamPmaOS,         /* ST/HTO PMA OS */
    xmodPcsUserSpeedParamScramble,      /* Scramble mode */
    xmodPcsUserSpeedParamEncode,        /* Encode mode */
    xmodPcsUserSpeedParamCl48CheckEnd,  /* CL48 Check end */
    xmodPcsUserSpeedParamBlkSync,       /* Block sync mode */
    xmodPcsUserSpeedParamReorder,       /* Reorder mode */
    xmodPcsUserSpeedParamCl36Enable,    /* CL36 enable */
    xmodPcsUserSpeedParamDescr1,        /* Descramble1 mode */
    xmodPcsUserSpeedParamDecode1,       /* Decode1 mode */
    xmodPcsUserSpeedParamDeskew,        /* Deskew mode */
    xmodPcsUserSpeedParamDescr2,        /* Descramble2 mode */
    xmodPcsUserSpeedParamDescr2ByteDel,
    xmodPcsUserSpeedParamBrcm64B66,     /* Drcm64/66 descramble */
    xmodPcsUserSpeedParamSgmii,         /* SGMII mode */
    xmodPcsUserSpeedParamClkcnt0,       /* clock count0 */
    xmodPcsUserSpeedParamClkcnt1,       /* clock count1 */
    xmodPcsUserSpeedParamLpcnt0,        /* Loop count0 */
    xmodPcsUserSpeedParamLpcnt1,        /* Loop count1 */
    xmodPcsUserSpeedParamMacCGC,        /* Mac CGC */
    xmodPcsUserSpeedParamRepcnt,        /* Repeat Count */
    xmodPcsUserSpeedParamCrdtEn,        /* Credit Enable */
    xmodPcsUserSpeedParamPcsClkcnt,     /* PCS clock count */
    xmodPcsUserSpeedParamPcsCGC,        /* PCS CGC */
    xmodPcsUserSpeedParamCl72En,        /* Cl72 enable */
    xmodPcsUserSpeedParamNumOfLanes,    /* Num of lanes */
    xmodPcsUserSpeedParamCount
} xmod_pcs_userspeed_param_td;
typedef uint8_t xmod_pcs_userspeed_param_t;

/*!
 * @enum xmod_bfcmap_port_cmd_e
 * @brief bfcmap port cmds
 */
typedef enum xmod_bfcmap_port_cmd_e {
    XMOD_BFCMAP_PORT_UNINIT,      /* uninitialize port */
    XMOD_BFCMAP_DEV_INIT,         /* initialize device */
    XMOD_BFCMAP_DEV_UNINIT,       /* uninitialize device */
    XMOD_BFCMAP_PORT_RESET,       /* reset port */
    XMOD_BFCMAP_PORT_SHUTDOWN,    /* shutdown port */
    XMOD_BFCMAP_PORT_LINK_ENABLE, /* enable link on port */
    XMOD_BFCMAP_PORT_BOUNCE,      /* bounce link on port */
    XMOD_BFCMAP_PORT_STAT_CLEAR   /* clear all stats on port */
} xmod_bfcmap_port_cmd_td;
typedef uint8_t xmod_bfcmap_port_cmd_t;

/*!
 * @enum xmod_bfcmap_port_mode_e
 * @brief bfcmap port mode
 */
typedef enum xmod_bfcmap_port_mode_e {
    XMOD_BFCMAP_FCOE_TO_FC_MODE,
    XMOD_BFCMAP_FCOE_TO_FCOE_MODE
} xmod_bfcmap_port_mode_td;
typedef uint8_t xmod_bfcmap_port_mode_t;

/*!
 * @enum xmod_bfcmap_port_speed_e
 * @brief bfcmap port speed
 */
typedef enum xmod_bfcmap_port_speed_e {
    XMOD_BFCMAP_PORT_SPEED_AN,
    XMOD_BFCMAP_PORT_SPEED_2GBPS,
    XMOD_BFCMAP_PORT_SPEED_4GBPS,
    XMOD_BFCMAP_PORT_SPEED_8GBPS,
    XMOD_BFCMAP_PORT_SPEED_16GBPS,
    XMOD_BFCMAP_PORT_SPEED_32GBPS,
    XMOD_BFCMAP_PORT_SPEED_AN_2GBPS,
    XMOD_BFCMAP_PORT_SPEED_AN_4GBPS,
    XMOD_BFCMAP_PORT_SPEED_AN_8GBPS,
    XMOD_BFCMAP_PORT_SPEED_AN_16GBPS,
    XMOD_BFCMAP_PORT_SPEED_AN_32GBPS,
    XMOD_BFCMAP_PORT_SPEED_MAX_COUNT
} xmod_bfcmap_port_speed_td;
typedef uint8_t xmod_bfcmap_port_speed_t;

/*!
 * @enum xmod_bfcmap_port_state_e
 * @brief bfcmap port state
 */
typedef enum xmod_bfcmap_port_state_e {
    XMOD_BFCMAP_PORT_STATE_INIT,        /* INITIALIZATION STATE */
    XMOD_BFCMAP_PORT_STATE_RESET,       /* PORT IN RESETING STATE */
    XMOD_BFCMAP_PORT_STATE_ACTIVE,      /* PORT IN ACTIVE(LINK UP) STATE */
    XMOD_BFCMAP_PORT_STATE_LINKDOWN,    /* PORT IN LINK DOWN STATE */
    XMOD_BFCMAP_PORT_STATE_DISABLE,     /* PORT IN DISABLED STATE */
    XMOD_BFCMAP_PORT_STATE_MAX_COUNT
} xmod_bfcmap_port_state_td;
typedef uint8_t xmod_bfcmap_port_state_t;

/*!
 * @enum xmod_bfcmap_8g_fw_on_active_e
 * @brief bfcmap 8G fw on active
 */
typedef enum xmod_bfcmap_8g_fw_on_active_e {
    XMOD_BFCMAP_8G_FW_ON_ACTIVE_ARBFF,       /* Use ARBFF as fillword on 8G Active state */
    XMOD_BFCMAP_8G_FW_ON_ACTIVE_IDLE         /* Use IDLE as fillword on 8G Active state */
} xmod_bfcmap_8g_fw_on_active_td;
typedef uint8_t xmod_bfcmap_8g_fw_on_active_t;

/*!
 * @enum xmod_bfcmap_encap_mac_address_e
 * @brief bfcmap enc pack addr
 */
typedef enum xmod_bfcmap_encap_mac_address_e {
    XMOD_BFCMAP_ENCAP_FCOE_FPMA,            /* FPMA, use prefix and Node
                                              ID to construct FCoE MAC Address */
    XMOD_BFCMAP_ENCAP_FCOE_ETH_ADDRESS_NULL,/* Null address for FCOE MAC */
    XMOD_BFCMAP_ENCAP_FCOE_ETH_ADDRESS_USER /* User Provided Address */
} xmod_bfcmap_encap_mac_address_td;
typedef uint8_t xmod_bfcmap_encap_mac_address_t;

/*!
 * @enum xmod_bfcmap_map_table_input_e
 * @brief bfcmap map table input
 */
typedef enum xmod_bfcmap_map_table_input_e {
    XMOD_BFCMAP_MAP_TABLE_INPUT_VID,         /* Use VID as input for FCMAPPER */
    XMOD_BFCMAP_MAP_TABLE_INPUT_VFID         /* Use VFID as input for FCMAPPER */
} xmod_bfcmap_map_table_input_td;
typedef uint8_t xmod_bfcmap_map_table_input_t;

/*!
 * @enum xmod_bfcmap_fc_crc_mode_e
 * @brief fc crc mode
 */
typedef enum xmod_bfcmap_fc_crc_mode_e {
    XMOD_BFCMAP_FC_CRC_MODE_NORMAL,           /* normal mode */
    XMOD_BFCMAP_FC_CRC_MODE_NO_CRC_CHECK      /* no CRC check mode */
} xmod_bfcmap_fc_crc_mode_td;
typedef uint8_t xmod_bfcmap_fc_crc_mode_t;

/*!
 * @enum xmod_bfcmap_vfthdr_proc_mode_e
 * @brief bfcmap vfthdr proc mode
 */
typedef enum xmod_bfcmap_vfthdr_proc_mode_e {
    XMOD_BFCMAP_VFTHDR_PRESERVE,   /* preserve vfthdr */
    XMOD_BFCMAP_VFTHDR_INSERT,     /* insert vfthdr */
    XMOD_BFCMAP_VFTHDR_DELETE      /* delete vfthdr */
} xmod_bfcmap_vfthdr_proc_mode_td;
typedef uint8_t xmod_bfcmap_vfthdr_proc_mode_t;

/*!
 * @enum xmod_bfcmap_vlantag_proc_mode_e
 * @brief bfcmap vlantag proc mode
 */
typedef enum xmod_bfcmap_vlantag_proc_mode_e {
    XMOD_BFCMAP_VLANTAG_PRESERVE,   /* preserve vlantag */
    XMOD_BFCMAP_VLANTAG_INSERT,     /* insert vlantag */
    XMOD_BFCMAP_VLANTAG_DELETE      /* delete vlantag */
} xmod_bfcmap_vlantag_proc_mode_td;
typedef uint8_t xmod_bfcmap_vlantag_proc_mode_t;

/*!
 * @enum xmod_bfcmap_vfid_mapsrc_e
 * @brief bfcmap vfid mapsrc
 */
typedef enum xmod_bfcmap_vfid_mapsrc_e {
    XMOD_BFCMAP_VFID_MACSRC_PASSTHRU,       /* passthru the vfid */
    XMOD_BFCMAP_VFID_MACSRC_PORT_DEFAULT,   /* use port default vfid */
    XMOD_BFCMAP_VFID_MACSRC_VID,            /* use vid if exists*/
    XMOD_BFCMAP_VFID_MACSRC_MAPPER,         /* use FCMAPER ouput if exists */
    XMOD_BFCMAP_VFID_MACSRC_DISABLE         /* disable VFID processing */
} xmod_bfcmap_vfid_mapsrc_td;
typedef uint8_t xmod_bfcmap_vfid_mapsrc_t;

/*!
 * @enum xmod_bfcmap_vid_mapsrc_e
 * @brief bfcmap vid mapsrc
 */
typedef enum xmod_bfcmap_vid_mapsrc_e {
    XMOD_BFCMAP_VID_MACSRC_VFID,            /* use vfid if exists*/
    XMOD_BFCMAP_VID_MACSRC_PASSTHRU,        /* passthru the vid */
    XMOD_BFCMAP_VID_MACSRC_PORT_DEFAULT,    /* use port default vid */
    XMOD_BFCMAP_VID_MACSRC_MAPPER           /* use FCMAPER ouput if exists */
} xmod_bfcmap_vid_mapsrc_td;
typedef uint8_t xmod_bfcmap_vid_mapsrc_t;

/*!
 * @enum xmod_bfcmap_vlan_pri_map_mode_e
 * @brief bfcmap vlan pri map mode
 */
typedef enum xmod_bfcmap_vlan_pri_map_mode_e {
    XMOD_BFCMAP_VLAN_PRI_MAP_MODE_PORT_DEFAULT,     /* use port default priority */
    XMOD_BFCMAP_VLAN_PRI_MAP_MODE_PASSTHRU          /* passthru if exists, else use port default priority */
} xmod_bfcmap_vlan_pri_map_mode_td;
typedef uint8_t xmod_bfcmap_vlan_pri_map_mode_t;

/*!
 * @enum xmod_bfcmap_hopcnt_check_mode_e
 * @brief bfcmap hopcnt check mode
 */
typedef enum xmod_bfcmap_hopcnt_check_mode_e {
    XMOD_BFCMAP_HOPCNT_CHECK_MODE_NO_CHK,       /* no check on hop count at ingress */
    XMOD_BFCMAP_HOPCNT_CHECK_MODE_FAIL_DROP,    /* discard on hop count == 1 */
    XMOD_BFCMAP_HOPCNT_CHECK_MODE_FAIL_EOFNI    /* EOFni on hop count == 1 */
} xmod_bfcmap_hopcnt_check_mode_td;
typedef uint8_t xmod_bfcmap_hopcnt_check_mode_t;

/*!
 * @enum xmod_bfcmap_lf_tr_e
 * @brief bfcmap lf tr
 */
typedef enum xmod_bfcmap_lf_tr_e {
    XMOD_BFCMAP_LF_TR_NONE,
    XMOD_BFCMAP_LF_TR_PORT_INIT,
    XMOD_BFCMAP_LF_TR_OPEN_LINK,
    XMOD_BFCMAP_LF_TR_LINK_FAILURE,
    XMOD_BFCMAP_LF_TR_OLS_RCVD,
    XMOD_BFCMAP_LF_TR_NOS_RCVD,
    XMOD_BFCMAP_LF_TR_SYNC_LOSS,
    XMOD_BFCMAP_LF_TR_BOUCELINK_FROM_ADMIN,
    XMOD_BFCMAP_LF_TR_CHGSPEED_FROM_ADMIN,
    XMOD_BFCMAP_LF_TR_DISABLE_FROM_ADMIN,
    XMOD_BFCMAP_LF_TR_RESET_FROM_ADMIN,
    XMOD_BFCMAP_LF_TR_LR_RCVD,
    XMOD_BFCMAP_LF_TR_LRR_RCVD,
    XMOD_BFCMAP_LF_TR_ED_TOV
} xmod_bfcmap_lf_tr_td;
typedef uint8_t xmod_bfcmap_lf_tr_t;

/*!
 * @enum xmod_bfcmap_lf_rc_e
 * @brief bfcmap lf rc
 */
typedef enum xmod_bfcmap_lf_rc_e {
    XMOD_BFCMAP_LF_RC_NONE,
    XMOD_BFCMAP_LF_RC_PORT_INIT,
    XMOD_BFCMAP_LF_RC_OPEN_LINK,
    XMOD_BFCMAP_LF_RC_LINK_FAILURE,
    XMOD_BFCMAP_LF_RC_OLS_RCVD,
    XMOD_BFCMAP_LF_RC_NOS_RCVD,
    XMOD_BFCMAP_LF_RC_SYNC_LOSS,
    XMOD_BFCMAP_LF_RC_BOUCELINK_FROM_ADMIN,
    XMOD_BFCMAP_LF_RC_CHGSPEED_FROM_ADMIN,
    XMOD_BFCMAP_LF_RC_DISABLE_FROM_ADMIN,
    XMOD_BFCMAP_LF_RC_RESET_FAILURE
} xmod_bfcmap_lf_rc_td;
typedef uint8_t xmod_bfcmap_lf_rc_t;

/*!
 * @enum xmod_bfcmap_diag_code_e
 * @brief bfcmap diag code
 */
typedef enum xmod_bfcmap_diag_code_e {
    XMOD_BFCMAP_DIAG_OK,
    XMOD_BFCMAP_DIAG_PORT_INIT,
    XMOD_BFCMAP_DIAG_OPEN_LINK,
    XMOD_BFCMAP_DIAG_LINK_FAILURE,
    XMOD_BFCMAP_DIAG_OLS_RCVD,
    XMOD_BFCMAP_DIAG_NOS_RCVD,
    XMOD_BFCMAP_DIAG_SYNC_LOSS,
    XMOD_BFCMAP_DIAG_BOUCELINK_FROM_ADMIN,
    XMOD_BFCMAP_DIAG_CHGSPEED_FROM_ADMIN,
    XMOD_BFCMAP_DIAG_DISABLE_FROM_ADMIN,
    XMOD_BFCMAP_DIAG_AN_NO_SIGNAL,
    XMOD_BFCMAP_DIAG_AN_TIMEOUT,
    XMOD_BFCMAP_DIAG_PROTO_TIMEOUT
} xmod_bfcmap_diag_code_td;
typedef uint8_t xmod_bfcmap_diag_code_t;

/*!
 * @enum xmod_pe_mode_e
 * @brief port extender mode types
 */
typedef enum xmod_pe_mode_e {
    XMOD_PE_MODE_4_PORTS = 1,
    XMOD_PE_MODE_2_PORTS = 2
} xmod_pe_mode_td;
typedef uint8_t xmod_pe_mode_t;

/*!
 * @enum xmod_pe_tag_e
 * @brief port extender tag types
 */
typedef enum xmod_pe_tag_e {
    XMOD_PE_TAG_BR      = 1,
    XMOD_PE_TAG_VLAN    = 2
} xmod_pe_tag_td;
typedef uint8_t xmod_pe_tag_t;

/*!
 * @enum xmod_port_if_e
 * @brief port media type
 */
typedef enum xmod_port_if_e {
    XMOD_PORT_IF_NOCXN, /* No physical connection */
    XMOD_PORT_IF_NULL,  /* Pass-through connection without PHY */
    XMOD_PORT_IF_MII,
    XMOD_PORT_IF_GMII,
    XMOD_PORT_IF_SGMII,
    XMOD_PORT_IF_TBI,
    XMOD_PORT_IF_XGMII,
    XMOD_PORT_IF_RGMII,
    XMOD_PORT_IF_RvMII,
    XMOD_PORT_IF_SFI,
    XMOD_PORT_IF_XFI,
    XMOD_PORT_IF_KR,
    XMOD_PORT_IF_KR4,
    XMOD_PORT_IF_CR,
    XMOD_PORT_IF_CR4,
    XMOD_PORT_IF_XLAUI,
    XMOD_PORT_IF_SR,
    XMOD_PORT_IF_RXAUI,
    XMOD_PORT_IF_XAUI,
    XMOD_PORT_IF_SPAUI,
    XMOD_PORT_IF_QSGMII,
    XMOD_PORT_IF_ILKN,
    XMOD_PORT_IF_RCY,
    XMOD_PORT_IF_FAT_PIPE,
    XMOD_PORT_IF_CGMII,
    XMOD_PORT_IF_CAUI,
    XMOD_PORT_IF_LR,
    XMOD_PORT_IF_LR4,
    XMOD_PORT_IF_SR4,
    XMOD_PORT_IF_KX,
    XMOD_PORT_IF_ZR,
    XMOD_PORT_IF_SR10,
    XMOD_PORT_IF_OTL,
    XMOD_PORT_IF_CPU,
    XMOD_PORT_IF_OLP,
    XMOD_PORT_IF_OAMP,
    XMOD_PORT_IF_ERP,
    XMOD_PORT_IF_TM_INTERNAL_PKT,   
    XMOD_PORT_IF_SR2,
    XMOD_PORT_IF_KR2,
    XMOD_PORT_IF_CR2,
    XMOD_PORT_IF_XFI2,
    XMOD_PORT_IF_XLAUI2,
    XMOD_PORT_IF_COUNT /* last, please */
} xmod_port_if_td;
typedef uint8_t xmod_port_if_t;

/*!
 * @struct xmod_lane_map_s
 * @brief Core information
 */
typedef struct xmod_lane_map_s {
    uint32_t num_of_lanes;                         /**< Number of elements in lane_map_rx/tx arrays */
    uint32_t lane_map_rx[XMOD_MAX_LANES_PER_CORE]; /**< lane_map_rx[x]=y means that rx lane x is mapped to rx lane y */
    uint32_t lane_map_tx[XMOD_MAX_LANES_PER_CORE]; /**< lane_map_tx[x]=y means that tx lane x is mapped to tx lane y */
} COMPILER_ATTRIBUTE((packed)) xmod_lane_map_t;

/*!
 * @struct xmod_core_init_config_s
 * @brief Core configuration
 */
typedef struct xmod_core_init_config_s {
    uint32_t num_of_lanes;                          /**< Number of elements in lane_map_rx/tx arrays */
    uint32_t lane_map_rx[XMOD_MAX_LANES_PER_CORE];  /**< lane_map_rx[x]=y means that rx lane x is mapped to rx lane y */
    uint32_t lane_map_tx[XMOD_MAX_LANES_PER_CORE];  /**< lane_map_tx[x]=y means that tx lane x is mapped to tx lane y */
    uint32_t tx_polarity;
    uint32_t rx_polarity;
    uint32_t ref_clock;
    uint8_t  core_mode;
    uint8_t  trcvr_host_managed;
} COMPILER_ATTRIBUTE((packed)) xmod_core_init_config_t;

/*!
 * @struct xmod_port_config_s
 * @brief Port configuration
 */
typedef struct xmod_port_config_s {
    uint32_t speed;
    uint8_t  interface;
    uint8_t  sys_lane_count;
    uint8_t  ln_lane_count;
    uint8_t  an_mode;
    uint8_t  an_cl72;
    uint8_t  fs_cl72;
    uint8_t  fs_cl72_sys;
    uint8_t  an_fec;
    uint8_t  port_is_higig;
    uint8_t  fiber_pref;   /* line side */
    uint8_t  fiber_pref_sys;  /* Sys side */
    uint8_t  port_mode;
    uint8_t  is_bootmaster;
    uint8_t  fs_fec;
    uint8_t  fs_fec_sys;
    uint8_t  is_flex;
    uint8_t  is_strip_crc;
    uint8_t  quad_mode;
} COMPILER_ATTRIBUTE((packed)) xmod_port_config_t;

/*!
 * @struct phymod_core_info_s
 * @brief Core information
 */
typedef struct xmod_core_info_s {
    xmod_core_version_t core_version;   /**< core version */
    uint32_t            serdes_id;      /**< serdes_id */
    uint32_t            phy_id0;        /**< phy_id0 */
    uint32_t            phy_id1;        /**< phy_id1 */
    char                name[30];
} COMPILER_ATTRIBUTE((packed)) xmod_core_info_t;

/*!
 * @struct xmod_core_i2c_s
 * @brief I2C access
 */
typedef struct xmod_core_i2c_s {
    uint8_t            dev_addr;
    uint8_t            offset;
    uint8_t            size;
} COMPILER_ATTRIBUTE((packed)) xmod_core_i2c_t;

/*!
 * @struct xmod_core_firmware_info_s
 * @brief Firmware information
 */
typedef struct xmod_core_firmware_info_s {
    uint32_t fw_version;
    uint32_t fw_crc;
} COMPILER_ATTRIBUTE((packed)) xmod_core_firmware_info_t;

/*!
 * @struct xmod_firmware_core_config_s
 * @brief Firmware core config
 */
typedef struct xmod_firmware_core_config_s {
    uint32_t CoreConfigFromPCS;
    uint32_t VcoRate;           /* vco rate */
} COMPILER_ATTRIBUTE((packed)) xmod_firmware_core_config_t;

/*!
 * @struct xmod_polarity_s
 * @brief polarity config
 */
typedef struct xmod_polarity_s {
    uint32_t rx_polarity;
    uint32_t tx_polarity;
} COMPILER_ATTRIBUTE((packed)) xmod_polarity_t;

/*!
 * @struct xmod_tx_s
 * @brief TX Parameters
 */
typedef struct xmod_tx_s {
    int8_t pre;
    int8_t main;
    int8_t post;
    int8_t post2;
    int8_t post3;
    int8_t amp;
    uint8_t sys_side;
} COMPILER_ATTRIBUTE((packed)) xmod_tx_t;

/*!
 * @struct xmod_phy_init_config_s
 * @brief phy init config
 */
typedef struct xmod_phy_init_config_s {
    xmod_polarity_t polarity;
    xmod_tx_t       tx[XMOD_MAX_LANES_PER_CORE];
    uint32_t        cl72_en;
    uint32_t        an_en;
} COMPILER_ATTRIBUTE((packed)) xmod_phy_init_config_t;

/*!
 * @struct xmod_value_override_s
 * @brief The value_override structure is used for values which can be auto-set (enable=0) or override (enable=1 + value)
 */
typedef struct xmod_value_override_s {
    uint32_t enable;
    int32_t  value;
} COMPILER_ATTRIBUTE((packed)) xmod_value_override_t;

/*!
 * @struct xmod_tx_override_s
 * @brief TX parameters which can be auto-set or override
 */
typedef struct xmod_tx_override_s {
    xmod_value_override_t phase_interpolator;
} COMPILER_ATTRIBUTE((packed)) xmod_tx_override_t;

/*!
 * @struct xmod_rx_s
 * @brief RX Parameters
 */
typedef struct xmod_rx_s {
    xmod_value_override_t   vga;
    uint32_t                num_of_dfe_taps; /* number of elements in DFE array */
    xmod_value_override_t   dfe[XMOD_NUM_DFE_TAPS];
    xmod_value_override_t   peaking_filter;
    xmod_value_override_t   low_freq_peaking_filter;
    uint32_t                rx_adaptation_on;
    uint8_t                 sys_side;
} COMPILER_ATTRIBUTE((packed)) xmod_rx_t;

/*!
 * @struct xmod_phy_reset_s
 * @brief Direction (In,Out,inOut) for RX/TX to reset
 */
typedef struct xmod_phy_reset_s {
    xmod_reset_direction_t rx;
    xmod_reset_direction_t tx;
} COMPILER_ATTRIBUTE((packed)) xmod_phy_reset_t;

/*!
 * @struct xmod_phy_power_s
 * @brief Power operation (On,Off,Off and On) for RX/TX
 */
typedef struct xmod_phy_power_s {
    xmod_power_t rx;
    xmod_power_t tx;
} COMPILER_ATTRIBUTE((packed)) xmod_phy_power_t;

/*!
 * @struct xmod_phy_inf_config_s
 * @brief Direction (In,Out,inOut) for RX/TX to reset
 */
typedef struct xmod_phy_inf_config_s {
    xmod_interface_t    interface_type;
    uint32_t            data_rate;
    uint32_t            interface_modes;
    xmod_ref_clk_t      ref_clock;          /* Core reference clock. */
    uint16_t            pll_divider_req;    /* Core pll divider request. */
    void*               device_aux_modes;   /* Device auxiliary modes. */
} COMPILER_ATTRIBUTE((packed)) xmod_phy_inf_config_t;

/*!
 * @struct xmod_cl72_status_s
 * @brief cl72 status
 */
typedef struct xmod_cl72_status_s {
    uint32_t enabled;
    uint32_t locked;
} COMPILER_ATTRIBUTE((packed)) xmod_cl72_status_t;

/*!
 * @struct xmod_autoneg_ability_s
 * @brief autoneg ability
 */
typedef struct xmod_autoneg_ability_s {
    uint32_t                an_cap;
    uint32_t                cl73bam_cap;
    uint32_t                cl37bam_cap;
    uint32_t                an_fec;
    uint32_t                an_cl72;
    uint32_t                an_hg2;
    uint32_t                capabilities;
    xmod_cl37_sgmii_speed_t sgmii_speed;
} COMPILER_ATTRIBUTE((packed)) xmod_autoneg_ability_t;

/*!
 * @struct xmod_autoneg_control_s
 * @brief autoneg control
 */
typedef struct xmod_autoneg_control_s {
    xmod_an_mode_type_t an_mode;
    uint32_t            num_lane_adv; /* The number of lanes the autoneg advert */
    uint32_t            flags; /* see AN_F */
    uint32_t            enable;
    uint8_t             an_done;
} COMPILER_ATTRIBUTE((packed)) xmod_autoneg_control_t;

/*!
 * @struct xmod_autoneg_status_s
 * @brief autoneg status
 */
typedef struct xmod_autoneg_status_s {
    uint32_t            enabled;
    uint32_t            locked;
    uint32_t            data_rate;
    xmod_interface_t    interface;
} COMPILER_ATTRIBUTE((packed)) xmod_autoneg_status_t;

/*!
 * @struct xmod_pcs_userspeed_config_s
 * @brief userspeed config
 */
typedef struct xmod_pcs_userspeed_config_s {
    xmod_pcs_userspeed_mode_t   mode;
    xmod_pcs_userspeed_param_t  param;
    uint32_t                    value;
    uint16_t                    current_entry;
} COMPILER_ATTRIBUTE((packed)) xmod_pcs_userspeed_config_t;

/*!
 * @struct xmod_port_enable_control_s
 * @brief port enable/disable
 */
typedef struct xmod_port_enable_control_s {
    uint32_t             enable;
} COMPILER_ATTRIBUTE((packed)) xmod_port_enable_control_t;

typedef struct xmod_firmware_lane_config_s {
    uint32_t LaneConfigFromPCS;
    uint32_t AnEnabled; /**< Autoneg */
    uint32_t DfeOn; /**< Enable DFE */
    uint32_t ForceBrDfe; /**< Force Baud rate DFE */
    uint32_t LpDfeOn; /**< Enable low power DFE */
    phymod_firmware_media_type_t MediaType; /**< Media Type */
    uint32_t UnreliableLos; /**< For optical use */
    uint32_t ScramblingDisable; /**< disable scrambling */
    uint32_t Cl72AutoPolEn; /**< Forced CL72 */
    uint32_t Cl72RestTO; /**< Forced CL72 */
    uint32_t ForceExtenedReach; /**< Forced extened reach mode */
    uint32_t ForceNormalReach; /**< Forced normal reach mode */
    uint32_t LpPrecoderEnabled; /**< linker partner has pre-coder on */
    uint32_t ForcePAM4Mode; /**< Force PAM4 mode */
    uint32_t ForceNRZMode; /**< Forced CL72 */
    uint32_t SystemSide; /**< System side port */
} COMPILER_ATTRIBUTE((packed)) xmod_firmware_lane_config_t;


typedef uint8_t xmod_bmac_addr_t[6];

typedef struct xmod_debug_ether_s {
    uint8_t            link_state_ln_side;
    uint32_t           link_speed_ln_side;
    uint8_t            link_state_sys_side;
    uint32_t           link_speed_sys_side;
    uint8_t            link_cl72_sys_side;
    uint8_t            link_cl72_ln_side;
    uint8_t            link_fec_sys_side;
    uint8_t            link_fec_ln_side;
}COMPILER_ATTRIBUTE((packed)) xmod_debug_ether_t;

/*!
 * @struct xmod_cos_to_pri_t
 * @brief Cos to Vlan priority mapping information
 */
typedef struct xmod_bfcmap_cos_to_pri_s {
    uint16_t            cos;    /* ingress */
    uint16_t            pri;       /* FCoE vlan Priority */
} COMPILER_ATTRIBUTE((packed)) xmod_bfcmap_cos_to_pri_t;

/*!
 * @struct xmod_bfcmap_port_config_s
 * @brief bfcmap port configuration
 */
typedef struct xmod_bfcmap_port_config_s {
    uint32_t                        action_mask;                /* XMOD_FCMAP_ATTR_xxx. */
    uint32_t                        action_mask2;               /* XMOD_FCMAP_ATTR2_xxx. */
    xmod_bfcmap_port_mode_t         port_mode;                  /* FC port Mode, FCoE mode */
    xmod_bfcmap_port_speed_t        speed;                      /* Speed, 2/4/8/16G */
    uint32_t                        tx_buffer_to_buffer_credits; /* Transit B2B credits */
    uint32_t                        rx_buffer_to_buffer_credits; /* Receive B2B Credits (read only), computed based on max_frame_length */
    uint32_t                        max_frame_length;           /* maximum FC frame length for ingress and egress (in unit of 32-bit word) */
    uint32_t                        bb_sc_n;                    /* bb credit recovery parameter */
    xmod_bfcmap_port_state_t        port_state;                 /* Port state, INIT, Reset, Link Up, Link down (read only) */
    uint32_t                        r_t_tov;                    /* Receive, transmit Timeout, default 100ms */
    uint32_t                        interrupt_enable;           /* Interrupt enable */
    xmod_bfcmap_8g_fw_on_active_t   fw_on_active_8g;            /* fillword on 8G active state */

    /* The following three fields should be valid when  XMOD_FCMAP_ATTR_SRC_MAC_CONSTRUCT_MASK is set */
    xmod_bfcmap_encap_mac_address_t src_mac_construct;          /* Encap Source MAC construct mode */
    xmod_bmac_addr_t                src_mac_addr;               /* Src MAC Addr for encapsulated frame */
    uint32_t                        src_fcmap_prefix;           /* Source FCMAP prefix for the FPMA */

    /* The following three fields should be valid when  XMOD_FCMAP_ATTR_DST_MAC_CONSTRUCT_MASK is set */
    xmod_bfcmap_encap_mac_address_t dst_mac_construct;          /* Encap Dest MAC construct mode */
    xmod_bmac_addr_t                dst_mac_addr;               /* Dest MAC Addr for encapsulated frame */
    uint32_t                        dst_fcmap_prefix;           /* Destination FCMAP prefix for the FPMA */

    /* FC Mapper Configuration */
    uint32_t                        vlan_tag;                     /* default vlan for the mapper */
    uint32_t                        vft_tag;                      /* default VFT tag for the mapper */
    uint32_t                        mapper_len;                   /* Size of VLAN-VSAN Mapper table (read only) */
    uint32_t                        ingress_mapper_bypass;        /* Bypass Mapper */
    uint32_t                        egress_mapper_bypass;         /* Bypass Mapper */
    xmod_bfcmap_map_table_input_t   ingress_map_table_input;      /* VID or VFID */
    xmod_bfcmap_map_table_input_t   egress_map_table_input;       /* VID or VFID */
    xmod_bfcmap_fc_crc_mode_t       ingress_fc_crc_mode;          /* Mapper FC-CRC handling */
    xmod_bfcmap_fc_crc_mode_t       egress_fc_crc_mode;           /* Mapper FC-CRC handling */
    xmod_bfcmap_vfthdr_proc_mode_t  ingress_vfthdr_proc_mode;     /* VFT header processing mode */
    xmod_bfcmap_vfthdr_proc_mode_t  egress_vfthdr_proc_mode;      /* VFT header processing mode */
    xmod_bfcmap_vlantag_proc_mode_t ingress_vlantag_proc_mode;    /* VLAN header processing mode */
    xmod_bfcmap_vlantag_proc_mode_t egress_vlantag_proc_mode;     /* VLAN header processing mode */
    xmod_bfcmap_vfid_mapsrc_t       ingress_vfid_mapsrc;          /* source of VFID */
    xmod_bfcmap_vfid_mapsrc_t       egress_vfid_mapsrc;           /* source of VFID */
    xmod_bfcmap_vid_mapsrc_t        ingress_vid_mapsrc;           /* source of VID */
    xmod_bfcmap_vid_mapsrc_t        egress_vid_mapsrc;            /* source of VID */
    xmod_bfcmap_vlan_pri_map_mode_t ingress_vlan_pri_map_mode;    /* processing mode for priority field of vlan tag */
    xmod_bfcmap_vlan_pri_map_mode_t egress_vlan_pri_map_mode;     /* processing mode for priority field of vlan tag */
    xmod_bfcmap_hopcnt_check_mode_t ingress_hopCnt_check_mode;    /* mode of VFT hopCnt check on ingress */
    uint32_t                         egress_hopCnt_dec_enable;     /* enable VFT hopCnt decrement on egress */

    uint32_t                         use_tts_pcs_16G;              /* during 16G speed negotiation, 0 -- Use TTS, 1 -- Use PCS  */
    uint32_t                         use_tts_pcs_32G;              /* during 32G speed negotiation, 0 -- Use TTS, 1 -- Use PCS  */ 
    uint32_t                         training_enable_16G;          /* enable/disable link training for 16G */ 
    uint32_t                         training_enable_32G;          /* enable/disable link training for 32G */
    uint32_t                         fec_enable_16G;               /* enable/disable FEC for 16G */ 
    uint32_t                         fec_enable_32G;               /* enable/disable FEC for 32G */

    /* Config Parameters for FC Mapper */
    uint32_t                         ingress_fcs_crrpt_eof_enable;    /* Enable FCS corruption on if FCoE pkt has EOFni or EOFa on ingress */ 
    uint32_t                         ingress_vlantag_presence_enable; /* VLAN header insert enable */
    xmod_bfcmap_hopcnt_check_mode_t  egress_hopcnt_check_mode;        /* mode of VFT hopCnt check on egress */
    uint32_t                         ingress_hopcnt_dec_enable;       /* enable VFT hopCnt decrement on ingress */
    uint32_t                         egress_pass_ctrl_frame_enable;   /* enable passing control frame on egress when mapper is in bypass mode */
    uint32_t                         egress_pass_pfc_frame_enable;    /* enable passing pfc frame on egress when mapper is in bypass mode */
    uint32_t                         egress_pass_pause_frame_enable;  /* enable passing pause frame on egress when mapper is in bypass mode */
    uint32_t                         egress_fcoe_version_chk_disable; /* disable FCOE header version field check on egress */
    uint32_t                         egress_default_cos_value;        /* specify default CoS value to use on egress */
    uint32_t                         egress_use_ip_cos_map;           /* use IP CoS map values on egress */
    uint32_t                         scrambling_enable_mask;          /* bitmap to enable/disable scrambling for 2/4/8/16/32G FC speed */
    uint32_t                         scrambling_enable_value;         /* 0 - disable, 1 - enable: bitmap value for corresponding speed */
    uint32_t                         egress_pause_enable;             /* 0 - disable, 1 - enable; legacy pause on egress */ 
    uint32_t                         egress_pfc_enable;               /* 0 - disable, 1 - enable; PFC on egress */ 
    uint32_t                         stat_interval;                   /* stats collection interval in milliseconds */ 
    uint32_t                         trcm_attribs;                    /* Tranceiver module configured attributes */
    xmod_bfcmap_cos_to_pri_t                cos_to_pri;                      /* Specifies the vlan priority the cos maps to. */

} COMPILER_ATTRIBUTE((packed)) xmod_bfcmap_port_config_t;

#define XMOD_BFCMAP_ATTR_PORT_MODE_MASK                        (0x1 << 0) 
#define XMOD_BFCMAP_ATTR_SPEED_MASK                            (0x1 << 1)
#define XMOD_BFCMAP_ATTR_TX_BB_CREDITS_MASK                    (0x1 << 2)
#define XMOD_BFCMAP_ATTR_RX_BB_CREDITS_MASK                    (0x1 << 3)
#define XMOD_BFCMAP_ATTR_MAX_FRAME_LENGTH_MASK                 (0x1 << 4)
#define XMOD_BFCMAP_ATTR_BB_SC_N_MASK                          (0x1 << 5)
#define XMOD_BFCMAP_ATTR_PORT_STATE_MASK                       (0x1 << 6)
#define XMOD_BFCMAP_ATTR_R_T_TOV_MASK                          (0x1 << 7)
#define XMOD_BFCMAP_ATTR_INTERRUPT_ENABLE_MASK                 (0x1 << 8)
#define XMOD_BFCMAP_ATTR_FW_ON_ACTIVE_8G_MASK                  (0x1 << 9)
#define XMOD_BFCMAP_ATTR_SRC_MAC_CONSTRUCT_MASK                (0x1 << 10)
#define XMOD_BFCMAP_ATTR_DST_MAC_CONSTRUCT_MASK                (0x1 << 11)
#define XMOD_BFCMAP_ATTR_VLAN_TAG_MASK                         (0x1 << 12)
#define XMOD_BFCMAP_ATTR_VFT_TAG_MASK                          (0x1 << 13)
#define XMOD_BFCMAP_ATTR_MAPPER_LEN_MASK                       (0x1 << 14)
#define XMOD_BFCMAP_ATTR_INGRESS_MAPPER_BYPASS_MASK            (0x1 << 15)
#define XMOD_BFCMAP_ATTR_EGRESS_MAPPER_BYPASS_MASK             (0x1 << 16)
#define XMOD_BFCMAP_ATTR_INGRESS_MAP_TABLE_INPUT_MASK          (0x1 << 17)
#define XMOD_BFCMAP_ATTR_EGRESS_MAP_TABLE_INPUT_MASK           (0x1 << 18)
#define XMOD_BFCMAP_ATTR_INGRESS_FC_CRC_MODE_MASK              (0x1 << 19)
#define XMOD_BFCMAP_ATTR_EGRESS_FC_CRC_MODE_MASK               (0x1 << 20)
#define XMOD_BFCMAP_ATTR_INGRESS_VFTHDR_PROC_MODE_MASK         (0x1 << 21)
#define XMOD_BFCMAP_ATTR_EGRESS_VFTHDR_PROC_MODE_MASK          (0x1 << 22)
#define XMOD_BFCMAP_ATTR_INGRESS_VLANTAG_PROC_MODE_MASK        (0x1 << 23)
#define XMOD_BFCMAP_ATTR_EGRESS_VLANTAG_PROC_MODE_MASK         (0x1 << 24)
#define XMOD_BFCMAP_ATTR_INGRESS_VFID_MAPSRC_MASK              (0x1 << 25)
#define XMOD_BFCMAP_ATTR_EGRESS_VFID_MAPSRC_MASK               (0x1 << 26)
#define XMOD_BFCMAP_ATTR_INGRESS_VID_MAPSRC_MASK               (0x1 << 27)
#define XMOD_BFCMAP_ATTR_EGRESS_VID_MAPSRC_MASK                (0x1 << 28)
#define XMOD_BFCMAP_ATTR_INGRESS_VLAN_PRI_MAP_MODE_MASK        (0x1 << 29)
#define XMOD_BFCMAP_ATTR_EGRESS_VLAN_PRI_MAP_MODE_MASK         (0x1 << 30)
#define XMOD_BFCMAP_ATTR_INGRESS_HOPCNT_CHECK_MODE_MASK        (0x1 << 31)
#define XMOD_BFCMAP_ATTR2_EGRESS_HOPCNT_DEC_ENABLE_MASK        (0x1 << 0)
#define XMOD_BFCMAP_ATTR2_USE_TTS_PCS_16G_MASK                 (0x1 << 1)
#define XMOD_BFCMAP_ATTR2_USE_TTS_PCS_32G_MASK                 (0x1 << 2) 
#define XMOD_BFCMAP_ATTR2_TRAINING_ENABLE_16G_MASK             (0x1 << 3) 
#define XMOD_BFCMAP_ATTR2_TRAINING_ENABLE_32G_MASK             (0x1 << 4)
#define XMOD_BFCMAP_ATTR2_FEC_ENABLE_16G_MASK                  (0x1 << 5)
#define XMOD_BFCMAP_ATTR2_FEC_ENABLE_32G_MASK                  (0x1 << 6)
#define XMOD_BFCMAP_ATTR2_INGRESS_FCS_CRRPT_EOF_ENABLE_MASK    (0x1 << 7)
#define XMOD_BFCMAP_ATTR2_INGRESS_VLANTAG_PRESENCE_ENABLE_MASK (0x1 << 8)
#define XMOD_BFCMAP_ATTR2_EGRESS_HOPCNT_CHECK_MODE_MASK        (0x1 << 9) 
#define XMOD_BFCMAP_ATTR2_EGRESS_PASS_CTRL_FRAME_ENABLE_MASK   (0x1 << 10)
#define XMOD_BFCMAP_ATTR2_EGRESS_PASS_PFC_FRAME_ENABLE_MASK    (0x1 << 11)
#define XMOD_BFCMAP_ATTR2_EGRESS_PASS_PAUSE_FRAME_ENABLE_MASK  (0x1 << 12) 
#define XMOD_BFCMAP_ATTR2_EGRESS_FCOE_VER_CHK_DISABLE_MASK     (0x1 << 13)
#define XMOD_BFCMAP_ATTR2_EGRESS_DEFAULT_COS_VALUE_MASK        (0x1 << 14) 
#define XMOD_BFCMAP_ATTR2_EGRESS_USE_IP_COS_MAP_MASK           (0x1 << 15)
#define XMOD_BFCMAP_ATTR2_INGRESS_HOPCNT_DEC_ENABLE_MASK       (0x1 << 16)
#define XMOD_BFCMAP_ATTR2_SCRAMBLING_ENABLE_MASK               (0x1 << 17)
#define XMOD_BFCMAP_ATTR2_EGRESS_PAUSE_ENABLE_MASK             (0x1 << 18)
#define XMOD_BFCMAP_ATTR2_EGRESS_PFC_ENABLE_MASK               (0x1 << 19)
#define XMOD_BFCMAP_ATTR2_STAT_INTERVAL_MASK                   (0x1 << 20)
#define XMOD_BFCMAP_ATTR2_TRCM_ATTRIBS_MASK                    (0x1 << 21)
#define XMOD_BFCMAP_ATTR2_COS_TO_PRI_MASK                      (0x1 << 22)

#define XMOD_BFCMAP_ATTR_ALL_MASK                        0xFFFFFFFF
#define XMOD_BFCMAP_ATTR2_ALL_MASK                       0xFFFFFFFF

typedef uint32_t xmod_bfcmap_pa_speed_t;

/*!
 * @struct xmod_bfcmap_port_ability_t
 * @brief bfcmap port ability bitmap
 */
typedef struct xmod_bfcmap_port_ability_s {
    xmod_bfcmap_pa_speed_t port_ability_speed;
} xmod_bfcmap_port_ability_t;

#define XMOD_BFCMAP_PORT_ABILITY_SPEED_AN          (0x1 << 0)
#define XMOD_BFCMAP_PORT_ABILITY_SPEED_2GBPS       (0x1 << 1)
#define XMOD_BFCMAP_PORT_ABILITY_SPEED_4GBPS       (0x1 << 2)
#define XMOD_BFCMAP_PORT_ABILITY_SPEED_8GBPS       (0x1 << 3)
#define XMOD_BFCMAP_PORT_ABILITY_SPEED_16GBPS      (0x1 << 4)
#define XMOD_BFCMAP_PORT_ABILITY_SPEED_32GBPS      (0x1 << 5)

#define XMOD_FCMAP_PORT_SCRAMBLING_SPEED_2GBPS    (0x1 << 0) 
#define XMOD_FCMAP_PORT_SCRAMBLING_SPEED_4GBPS    (0x1 << 1) 
#define XMOD_FCMAP_PORT_SCRAMBLING_SPEED_8GBPS    (0x1 << 2) 
#define XMOD_FCMAP_PORT_SCRAMBLING_SPEED_16GBPS   (0x1 << 3) 
#define XMOD_FCMAP_PORT_SCRAMBLING_SPEED_32GBPS   (0x1 << 4) 

#define XMOD_FCMAP_PORT_MODULE_NO_FW_I2C          (0x1 << 0)
#define XMOD_FCMAP_PORT_MODULE_IS_COPPER          (0x1 << 1)
#define XMOD_FCMAP_PORT_MODULE_SPEED_CAP32G       (0x1 << 2)
#define XMOD_FCMAP_PORT_MODULE_SPEED_CAP16G       (0x1 << 3)
#define XMOD_FCMAP_PORT_MODULE_SPEED_CAP8G        (0x1 << 4)
#define XMOD_FCMAP_PORT_MODULE_SPEED_CAP4G        (0x1 << 5)
#define XMOD_FCMAP_PORT_MODULE_FEC_CAP32G         (0x1 << 6)
#define XMOD_FCMAP_PORT_MODULE_FEC_CAP16G         (0x1 << 7)
#define XMOD_FCMAP_PORT_MODULE_IS_QSFP            (0x1 << 8)
#define XMOD_FCMAP_PORT_MODULE_SUPP_RS            (0x1 << 9)
#define XMOD_FCMAP_PORT_MODULE_RS_HRD_RX_CUTOFF      (0x3 << 10)
#define XMOD_FCMAP_PORT_MODULE_RS_HRD_TX_CUTOFF      (0x3 << 12)

#define XMOD_USE_IP_COS_SHIFT                     16
#define XMOD_USE_IP_COS_CMD                       0x1

/*!
 * @struct xmod_bfcmap_vlan_vsan_map_t
 * @brief bfcmap vlan to vsan mapping
 */
typedef struct xmod_bfcmap_vlan_vsan_map_s {
	uint16_t   vlan_vid;
	uint16_t   vsan_vfid;
} xmod_bfcmap_vlan_vsan_map_t;

/*!
 * @struct xmod_an_status_s
 * @brief  Autoneg result
 */
typedef struct xmod_an_status_s {
    uint32_t hcd_speed; /**< negotiated speed */
    uint8_t  result; /**< Autoneg result. */
} COMPILER_ATTRIBUTE((packed)) xmod_an_status_t;

typedef struct xmod_an_try_enable_control_s {
    xmod_an_mode_type_t an_mode;
    uint32_t            num_lane_adv; /* The number of lanes the autoneg advert */
    uint32_t            flags; /* see AN_F */
    uint32_t            enable;
    uint32_t            speed;
} COMPILER_ATTRIBUTE((packed)) xmod_an_try_enable_control_t;

/*!
 * @struct xmod_pe_config_s
 * @brief PE initial configuration information
 */
typedef struct xmod_pe_config_s {
    xmod_pe_mode_t      mode;
    xmod_pe_tag_t       tag;
    uint8_t             pfc;
    uint8_t             res;
    uint16_t            pcid[4];
} COMPILER_ATTRIBUTE((packed)) xmod_pe_config_t;

/*!
 * @struct xmod_pe_port_status_s
 * @brief PE port status information
 */
typedef struct xmod_pe_port_status_s {
    uint8_t             res;            /* reserved */
    uint8_t             link_up;        /* link_up */
} COMPILER_ATTRIBUTE((packed)) xmod_pe_port_status_t;

/*!
 * @struct xmod_pe_status_s
 * @brief PE port status information
 */
typedef struct xmod_pe_status_s {
    xmod_pe_port_status_t       up_port;        /* up stream port */
    xmod_pe_port_status_t       pe_port0;       /* port 0 extender */
    xmod_pe_port_status_t       pe_port1;       /* port 1 extender */
    xmod_pe_port_status_t       pe_port2;       /* port 2 extender */
    xmod_pe_port_status_t       pe_port3;       /* port 3 extender */
    xmod_port_if_t              up_port_mod;    /* up stream port module type */
} COMPILER_ATTRIBUTE((packed)) xmod_pe_status_t;

/*!
 * @struct xmod_pe_port_stats_s
 * @brief PE port statistics information
 */
typedef struct xmod_pe_port_stats_s {
    uint32_t            port_dn_cnt;        /* port down counter */

    uint64_t            rpkt;               /* Receive frame/packet Counter */
    uint64_t            rbyt;               /* Receive Byte Counter */
    uint64_t            tpkt;               /* Transmit Packet/Frame Counter */
    uint64_t            tbyt;               /* Transmit Byte Counter */

    uint64_t            rfcs;               /* Receive FCS Error Frame Counter */
    uint64_t            rfrg;               /* Receive Fragment Counter */
    uint64_t            rjbr;               /* Receive Jabber Frame Counter */
    uint64_t            rovr;               /* Receive Oversized Frame Counter */
    uint64_t            rrpkt;              /* Receive RUNT Frame Counter */
    uint64_t            raln;               /* Receive Alignment Error Frame Counter */
    uint64_t            rerpkt;             /* Receive Code Error Frame Counter */
    uint64_t            rmtue;              /* Receive MTU Check Error Frame Counter */
    uint64_t            rprm;               /* Receive Promiscuous Frame Counter */
    uint64_t            rschcrc;            /* Receive SCH CRC Error */
    uint64_t            rtrfu;              /* Receive Truncated Frame Counter (due to RX FIFO full) */
    uint64_t            rfcr;               /* Receive False Carrier Counter */
    uint64_t            rxpf;               /* Receive PAUSE Frame Counter */
    uint64_t            rflr;               /* Receive Length Out of Range Frame Counter */

    uint64_t            tfcs;               /* Transmit FCS Error Counter */
    uint64_t            tfrg;               /* Transmit Fragment Counter */
    uint64_t            tjbr;               /* Transmit Jabber Counter */
    uint64_t            tncl;               /* Transmit Total Collision Counter */
    uint64_t            tovr;               /* Transmit Oversize Packet Counter */
    uint64_t            trpkt;              /* Transmit RUNT Frame Counter */
    uint64_t            txpf;               /* Transmit Pause Control Frame Counter */

} COMPILER_ATTRIBUTE((packed)) xmod_pe_port_stats_t;

/*!
 * @struct xmod_pe_pcid_s
 * @brief PE port E-channel ID information
 */
typedef struct xmod_pe_pcid_s {
    uint16_t            pcid0;    /* port 0 extender channel id */
    uint16_t            pcid1;    /* port 1 extender channel id */
    uint16_t            pcid2;    /* port 2 extender channel id */
    uint16_t            pcid3;    /* port 3 extender channel id */
} COMPILER_ATTRIBUTE((packed)) xmod_pe_pcid_t;

/*!
 * @struct xmod_pe_fw_ver_s
 * @brief FW ver information
 */
typedef struct xmod_pe_fw_ver_s {
    uint16_t            major;    /* major version */
    uint16_t            minor;    /* minor version */
    uint16_t            build;    /* build version */
} COMPILER_ATTRIBUTE((packed)) xmod_pe_fw_ver_t;

/*!
 * @struct xmod_cl72_t
 * @brief CL72 training information
 */
typedef struct xmod_cl72_s {
    uint8_t            enable;      /* enable or disable */
    uint8_t            sys_side;    /* system side or line side */
} COMPILER_ATTRIBUTE((packed)) xmod_cl72_t;

/*! 
 * @struct xmod_fec_t
 * @brief FEC training information
 */
typedef struct xmod_fec_s {
    uint8_t            enable;      /* enable or disable */
    uint8_t            sys_side;    /* system side or line side */
} COMPILER_ATTRIBUTE((packed)) xmod_fec_t;

/*!
 * @struct xmod_flow_ctrl_t
 * @brief Flow control information
 */
typedef struct xmod_flow_ctrl_s {
    uint8_t            ingress;    /* ingress or egress */
    uint8_t            mode;       /* Flow Control modes */
} COMPILER_ATTRIBUTE((packed)) xmod_flow_ctrl_t;

/* XMOD_CORE_CMDS & LENGTHS */
/* XMOD_CORE_IDENTIFY */
extern int xmod_core_identify(OUT uint8_t is_identified);
#define XMOD_CORE_IDENTIFY_IN_LEN   (0)
#define XMOD_CORE_IDENTIFY_OUT_LEN  (sizeof(uint8_t))
/* XMOD_CORE_INFO_GET */
extern int xmod_core_info_get(OUT xmod_core_info_t info);
#define XMOD_CORE_INFO_GET_IN_LEN   (0)
#define XMOD_CORE_INFO_GET_OUT_LEN  (sizeof(xmod_core_info_t))
/* XMOD_CORE_LANE_MAP_GET */
extern int xmod_core_lane_map_get(OUT xmod_lane_map_t lane_map);
#define XMOD_CORE_LANE_MAP_GET_IN_LEN   (0)
#define XMOD_CORE_LANE_MAP_GET_OUT_LEN  (sizeof(xmod_lane_map_t))
/* XMOD_CORE_LANE_MAP_SET */
extern int xmod_core_lane_map_set(IN xmod_lane_map_t lane_map);
#define XMOD_CORE_LANE_MAP_SET_IN_LEN   (sizeof(xmod_lane_map_t))
#define XMOD_CORE_LANE_MAP_SET_OUT_LEN  (0)
/* XMOD_CORE_RESET_GET */
extern int xmod_core_reset_get(IN xmod_reset_mode_t reset_mode, OUT xmod_reset_direction_t direction);
#define XMOD_CORE_RESET_GET_IN_LEN   (sizeof(xmod_reset_mode_t))
#define XMOD_CORE_RESET_GET_OUT_LEN  (sizeof(xmod_reset_direction_t))
/* XMOD_CORE_RESET_SET */
extern int xmod_core_reset_set(IN xmod_reset_mode_t reset_mode, IN xmod_reset_direction_t direction);
#define XMOD_CORE_RESET_SET_IN_LEN   (sizeof(xmod_reset_mode_t) + sizeof(xmod_reset_direction_t))
#define XMOD_CORE_RESET_SET_OUT_LEN  (0)
/* XMOD_CORE_FIRMWARE_INFO_GET */
extern int xmod_core_firmware_info_get(OUT xmod_core_firmware_info_t fw_info);
#define XMOD_CORE_FIRMWARE_INFO_GET_IN_LEN   (0)
#define XMOD_CORE_FIRMWARE_INFO_GET_OUT_LEN  (sizeof(xmod_core_firmware_info_t))
/* XMOD_PHY_FIRMWARE_CORE_CONFIG_GET */
extern int xmod_phy_firmware_core_config_get(OUT xmod_firmware_core_config_t fw_core_config);
#define XMOD_PHY_FIRMWARE_CORE_CONFIG_GET_IN_LEN   (0)
#define XMOD_PHY_FIRMWARE_CORE_CONFIG_GET_OUT_LEN  (sizeof(xmod_firmware_core_config_t))
/* XMOD_PHY_FIRMWARE_CORE_CONFIG_SET */
extern int xmod_phy_firmware_core_config_set(IN xmod_firmware_core_config_t fw_core_config);
#define XMOD_PHY_FIRMWARE_CORE_CONFIG_SET_IN_LEN   (sizeof(xmod_firmware_core_config_t))
#define XMOD_PHY_FIRMWARE_CORE_CONFIG_SET_OUT_LEN  (0)
/* XMOD_CORE_INIT */
extern int xmod_core_init(IN xmod_core_init_config_t init_config);
#define XMOD_CORE_INIT_IN_LEN   (sizeof(xmod_core_init_config_t))
#define XMOD_CORE_INIT_OUT_LEN  (0)
/* XMOD_CORE_PLL_SEQUENCER_RESTART */
extern int xmod_core_pll_sequencer_restart(IN xmod_sequencer_operation_t operation);
#define XMOD_CORE_PLL_SEQUENCER_RESTART_IN_LEN   (sizeof(xmod_sequencer_operation_t))
#define XMOD_CORE_PLL_SEQUENCER_RESTART_OUT_LEN  (0)
/* XMOD_CORE_WAIT_EVENT */
extern int xmod_core_wait_event(IN xmod_core_event_t event, IN uint32_t timeout);
#define XMOD_CORE_WAIT_EVENT_IN_LEN   (sizeof(xmod_core_event_t) + sizeof(uint32_t))
#define XMOD_CORE_WAIT_EVENT_OUT_LEN  (0)
/* XMOD_DEV_REG_GET */
extern int xmod_dev_reg_get(IN uint32_t reg_addr, OUT uint32_t val);
#define XMOD_DEV_REG_GET_IN_LEN   (sizeof(uint32_t))
#define XMOD_DEV_REG_GET_OUT_LEN  (sizeof(uint32_t))
/* XMOD_DEV_REG_SET */
extern int xmod_dev_reg_set(IN uint32_t reg_addr, IN uint32_t val);
#define XMOD_DEV_REG_SET_IN_LEN   (sizeof(uint32_t) + sizeof(uint32_t))
#define XMOD_DEV_REG_SET_OUT_LEN  (0)
/* XMOD_DEV_DEBUG_BUF_GET */
extern int xmod_dev_debug_buf_get(IN uint32_t buf_size, OUT uint8_t buf);
#define XMOD_DEV_DEBUG_BUF_GET_IN_LEN   (sizeof(uint32_t))
#define XMOD_DEV_DEBUG_BUF_GET_OUT_LEN  (XMOD_BUFFER_MAX_LEN)
/* XMOD_CMD_ACKNOWLEDGE */
#define XMOD_CMD_ACKNOWLEDGE_IN_LEN   (0)
#define XMOD_CMD_ACKNOWLEDGE_OUT_LEN  (0)
/* XMOD_CMD_ABORT */
#define XMOD_CMD_ABORT_IN_LEN   (0)
#define XMOD_CMD_ABORT_OUT_LEN  (0)
/* XMOD_DEV_DEBUG_CMD */
extern int xmod_dev_debug_cmd(IN uint32_t cmd, OUT uint32_t response);
#define XMOD_DEV_DEBUG_CMD_IN_LEN   (XMOD_BUFFER_MAX_LEN)
#define XMOD_DEV_DEBUG_CMD_OUT_LEN  (XMOD_BUFFER_MAX_LEN)
/* XMOD_DEV_CTR_INTERVAL_SET */
extern int xmod_dev_ctr_interval_set(IN uint32_t int_ms);
#define XMOD_DEV_CTR_INTERVAL_SET_IN_LEN   (sizeof(uint32_t))
#define XMOD_DEV_CTR_INTERVAL_SET_OUT_LEN  (0)
#define XMOD_DEV_DEBUG_ETHER_CMD_IN_LEN   (0)
#define XMOD_DEV_DEBUG_ETHER_CMD_OUT_LEN  (sizeof(xmod_debug_ether_t))
/* XMOD_DEV_I2C_GET */
#define XMOD_DEV_I2C_GET_IN_LEN   (sizeof(xmod_core_i2c_t))
#define I2C_GET_OUT_DATA_WORDS 4
#define XMOD_DEV_I2C_GET_OUT_LEN  (sizeof(uint32_t) + sizeof(uint32_t) * I2C_GET_OUT_DATA_WORDS)
/* XMOD_DEV_I2C_SET */
#define XMOD_DEV_I2C_SET_IN_LEN   (sizeof(xmod_core_i2c_t) + sizeof(uint32_t))
#define XMOD_DEV_I2C_SET_OUT_LEN  (sizeof(uint32_t))

/* XMOD_ETHERNET_CMDS & LENGTHS */
/* XMOD_PHY_FIRMWARE_LANE_CONFIG_GET */
extern int xmod_phy_firmware_lane_config_get(OUT xmod_firmware_lane_config_t fw_lane_config);
#define XMOD_PHY_FIRMWARE_LANE_CONFIG_GET_IN_LEN   (0)
#define XMOD_PHY_FIRMWARE_LANE_CONFIG_GET_OUT_LEN  (sizeof(xmod_firmware_lane_config_t))
/* XMOD_PHY_FIRMWARE_LANE_CONFIG_SET */
extern int xmod_phy_firmware_lane_config_set(IN xmod_firmware_lane_config_t fw_lane_config);
#define XMOD_PHY_FIRMWARE_LANE_CONFIG_SET_IN_LEN   (sizeof(xmod_firmware_lane_config_t))
#define XMOD_PHY_FIRMWARE_LANE_CONFIG_SET_OUT_LEN  (0)
/* XMOD_PHY_INIT */
extern int xmod_phy_init(IN xmod_phy_init_config_t init_config);
#define XMOD_PHY_INIT_IN_LEN   (sizeof(xmod_phy_init_config_t))
#define XMOD_PHY_INIT_OUT_LEN  (0)
/* XMOD_PHY_RX_RESTART */
extern int xmod_phy_rx_restart(void);
#define XMOD_PHY_RX_RESTART_IN_LEN   (0)
#define XMOD_PHY_RX_RESTART_OUT_LEN  (0)
/* XMOD_PHY_POLARITY_GET */
extern int xmod_phy_polarity_get(OUT xmod_polarity_t polarity);
#define XMOD_PHY_POLARITY_GET_IN_LEN   (0)
#define XMOD_PHY_POLARITY_GET_OUT_LEN  (sizeof(xmod_polarity_t))
/* XMOD_PHY_POLARITY_SET */
extern int xmod_phy_polarity_set(IN xmod_polarity_t polarity);
#define XMOD_PHY_POLARITY_SET_IN_LEN   (sizeof(xmod_polarity_t))
#define XMOD_PHY_POLARITY_SET_OUT_LEN  (0)
/* XMOD_PHY_TX_GET */
extern int xmod_phy_tx_get(IN xmod_tx_t txi, OUT xmod_tx_t txo);
#define XMOD_PHY_TX_GET_IN_LEN   (sizeof(xmod_tx_t))
#define XMOD_PHY_TX_GET_OUT_LEN  (sizeof(xmod_tx_t))
/* XMOD_PHY_TX_SET */
extern int xmod_phy_tx_set(IN xmod_tx_t tx);
#define XMOD_PHY_TX_SET_IN_LEN   (sizeof(xmod_tx_t))
#define XMOD_PHY_TX_SET_OUT_LEN  (0)
/* XMOD_PHY_MEDIA_TYPE_TX_GET */
extern int xmod_phy_media_type_tx_get(IN xmod_media_typed_t media, OUT xmod_tx_t tx);
#define XMOD_PHY_MEDIA_TYPE_TX_GET_IN_LEN   (sizeof(xmod_media_typed_t))
#define XMOD_PHY_MEDIA_TYPE_TX_GET_OUT_LEN  (sizeof(xmod_tx_t))
/* XMOD_PHY_TX_OVERRIDE_GET */
extern int xmod_phy_tx_override_get(OUT xmod_tx_override_t tx_override);
#define XMOD_PHY_TX_OVERRIDE_GET_IN_LEN   (0)
#define XMOD_PHY_TX_OVERRIDE_GET_OUT_LEN  (sizeof(xmod_tx_override_t))
/* XMOD_PHY_TX_OVERRIDE_SET */
extern int xmod_phy_tx_override_set(IN xmod_tx_override_t tx_override);
#define XMOD_PHY_TX_OVERRIDE_SET_IN_LEN   (sizeof(xmod_tx_override_t))
#define XMOD_PHY_TX_OVERRIDE_SET_OUT_LEN  (0)
/* XMOD_PHY_RX_GET */
extern int xmod_phy_rx_get(IN xmod_rx_t rxi, OUT xmod_rx_t rxo);
#define XMOD_PHY_RX_GET_IN_LEN   (sizeof(xmod_rx_t))
#define XMOD_PHY_RX_GET_OUT_LEN  (sizeof(xmod_rx_t))
/* XMOD_PHY_RX_SET */
extern int xmod_phy_rx_set(IN xmod_rx_t rx);
#define XMOD_PHY_RX_SET_IN_LEN   (sizeof(xmod_rx_t))
#define XMOD_PHY_RX_SET_OUT_LEN  (0)
/* XMOD_PHY_RESET_GET */
extern int xmod_phy_reset_get(OUT xmod_phy_reset_t reset);
#define XMOD_PHY_RESET_GET_IN_LEN   (0)
#define XMOD_PHY_RESET_GET_OUT_LEN  (sizeof(xmod_phy_reset_t))
/* XMOD_PHY_RESET_SET */
extern int xmod_phy_reset_set(IN xmod_phy_reset_t reset);
#define XMOD_PHY_RESET_SET_IN_LEN   (sizeof(xmod_phy_reset_t))
#define XMOD_PHY_RESET_SET_OUT_LEN  (0)
/* XMOD_PHY_POWER_GET */
extern int xmod_phy_power_get(OUT xmod_phy_power_t power);
#define XMOD_PHY_POWER_GET_IN_LEN   (0)
#define XMOD_PHY_POWER_GET_OUT_LEN  (sizeof(xmod_phy_power_t))
/* XMOD_PHY_POWER_SET */
extern int xmod_phy_power_set(IN xmod_phy_power_t power);
#define XMOD_PHY_POWER_SET_IN_LEN   (sizeof(xmod_phy_power_t))
#define XMOD_PHY_POWER_SET_OUT_LEN  (0)
/* XMOD_PHY_TX_LANE_CONTROL_GET */
extern int xmod_phy_tx_lane_control_get(OUT uint8_t enable);
#define XMOD_PHY_TX_LANE_CONTROL_GET_IN_LEN   (0)
#define XMOD_PHY_TX_LANE_CONTROL_GET_OUT_LEN  (sizeof(uint8_t))
/* XMOD_PHY_TX_LANE_CONTROL_SET */
extern int xmod_phy_tx_lane_control_set(IN uint8_t enable);
#define XMOD_PHY_TX_LANE_CONTROL_SET_IN_LEN   (sizeof(uint8_t))
#define XMOD_PHY_TX_LANE_CONTROL_SET_OUT_LEN  (0)
/* XMOD_PHY_RX_LANE_CONTROL_GET */
extern int xmod_phy_rx_lane_control_get(OUT uint8_t enable);
#define XMOD_PHY_RX_LANE_CONTROL_GET_IN_LEN   (0)
#define XMOD_PHY_RX_LANE_CONTROL_GET_OUT_LEN  (sizeof(uint8_t))
/* XMOD_PHY_RX_LANE_CONTROL_SET */
extern int xmod_phy_rx_lane_control_set(IN uint8_t enable);
#define XMOD_PHY_RX_LANE_CONTROL_SET_IN_LEN   (sizeof(uint8_t))
#define XMOD_PHY_RX_LANE_CONTROL_SET_OUT_LEN  (0)
/* XMOD_PHY_FEC_ENABLE_GET */
extern int xmod_phy_fec_enable_get(OUT uint32_t enable);
#define XMOD_PHY_FEC_ENABLE_GET_IN_LEN   (sizeof(uint32_t))
#define XMOD_PHY_FEC_ENABLE_GET_OUT_LEN  (sizeof(uint32_t))
/* XMOD_PHY_FEC_ENABLE_SET */
extern int xmod_phy_fec_enable_set(IN uint32_t enable);
#define XMOD_PHY_FEC_ENABLE_SET_IN_LEN   (sizeof(xmod_fec_t))
#define XMOD_PHY_FEC_ENABLE_SET_OUT_LEN  (0)
/* XMOD_PHY_INTERFACE_CONFIG_GET */
extern int xmod_phy_interface_config_get(IN uint8_t flags, OUT xmod_phy_inf_config_t config);
#define XMOD_PHY_INTERFACE_CONFIG_GET_IN_LEN   (sizeof(uint8_t))
#define XMOD_PHY_INTERFACE_CONFIG_GET_OUT_LEN  (sizeof(xmod_phy_inf_config_t))
/* XMOD_PHY_INTERFACE_CONFIG_SET */
extern int xmod_phy_interface_config_set(IN uint8_t flags, IN xmod_phy_inf_config_t config);
#define XMOD_PHY_INTERFACE_CONFIG_SET_IN_LEN   (sizeof(uint8_t) + sizeof(xmod_phy_inf_config_t))
#define XMOD_PHY_INTERFACE_CONFIG_SET_OUT_LEN  (0)
/* XMOD_PHY_CL72_GET */
extern int xmod_phy_cl72_get(OUT uint32_t cl72_en);
#define XMOD_PHY_CL72_GET_IN_LEN   (sizeof(uint32_t))
#define XMOD_PHY_CL72_GET_OUT_LEN  (sizeof(uint32_t))
/* XMOD_PHY_CL72_SET */
extern int xmod_phy_cl72_set(IN uint32_t cl72_en);
#define XMOD_PHY_CL72_SET_IN_LEN   (sizeof(xmod_cl72_t))
#define XMOD_PHY_CL72_SET_OUT_LEN  (0)
/* XMOD_PHY_CL72_STATUS_GET */
extern int xmod_phy_cl72_status_get(OUT xmod_cl72_status_t status);
#define XMOD_PHY_CL72_STATUS_GET_IN_LEN   (sizeof(uint32_t))
#define XMOD_PHY_CL72_STATUS_GET_OUT_LEN  (sizeof(xmod_cl72_status_t))
/* XMOD_PHY_AUTONEG_ABILITY_GET */
extern int xmod_phy_autoneg_ability_get(OUT xmod_autoneg_ability_t an_ability_get_type);
#define XMOD_PHY_AUTONEG_ABILITY_GET_IN_LEN   (0)
#define XMOD_PHY_AUTONEG_ABILITY_GET_OUT_LEN  (sizeof(xmod_autoneg_ability_t))
/* XMOD_PHY_AUTONEG_ABILITY_SET */
extern int xmod_phy_autoneg_ability_set(IN xmod_autoneg_ability_t an_ability_set_type);
#define XMOD_PHY_AUTONEG_ABILITY_SET_IN_LEN   (sizeof(xmod_autoneg_ability_t))
#define XMOD_PHY_AUTONEG_ABILITY_SET_OUT_LEN  (0)
/* XMOD_PHY_AUTONEG_REMOTE_ABILITY_GET */
extern int xmod_phy_autoneg_remote_ability_get(OUT xmod_autoneg_ability_t an_ability_get_type);
#define XMOD_PHY_AUTONEG_REMOTE_ABILITY_GET_IN_LEN   (0)
#define XMOD_PHY_AUTONEG_REMOTE_ABILITY_GET_OUT_LEN  (sizeof(xmod_autoneg_ability_t))
/* XMOD_PHY_AUTONEG_GET */
extern int xmod_phy_autoneg_get(OUT xmod_autoneg_control_t an, OUT uint8_t an_done);
#define XMOD_PHY_AUTONEG_GET_IN_LEN   (0)
#define XMOD_PHY_AUTONEG_GET_OUT_LEN  (sizeof(xmod_autoneg_control_t) + sizeof(uint8_t))
/* XMOD_PHY_AUTONEG_SET */
extern int xmod_phy_autoneg_set(IN xmod_autoneg_control_t an);
#define XMOD_PHY_AUTONEG_SET_IN_LEN   (sizeof(xmod_autoneg_control_t))
#define XMOD_PHY_AUTONEG_SET_OUT_LEN  (0)
/* XMOD_PHY_AUTONEG_STATUS_GET */
extern int xmod_phy_autoneg_status_get(OUT xmod_autoneg_status_t status);
#define XMOD_PHY_AUTONEG_STATUS_GET_IN_LEN   (0)
#define XMOD_PHY_AUTONEG_STATUS_GET_OUT_LEN  (sizeof(xmod_autoneg_status_t))
/* XMOD_PHY_LOOPBACK_GET */
extern int xmod_phy_loopback_get(IN xmod_loopback_mode_t loopback_type, OUT uint8_t enable);
#define XMOD_PHY_LOOPBACK_GET_IN_LEN   (sizeof(xmod_loopback_mode_t))
#define XMOD_PHY_LOOPBACK_GET_OUT_LEN  (sizeof(uint8_t))
/* XMOD_PHY_LOOPBACK_SET */
extern int xmod_phy_loopback_set(IN xmod_loopback_mode_t loopback_type, IN uint8_t enable);
#define XMOD_PHY_LOOPBACK_SET_IN_LEN   (sizeof(xmod_loopback_mode_t) + sizeof(uint8_t))
#define XMOD_PHY_LOOPBACK_SET_OUT_LEN  (0)
/* XMOD_PHY_RX_PMD_LOCKED_GET */
extern int xmod_phy_rx_pmd_locked_get(OUT uint8_t rx_pmd_locked);
#define XMOD_PHY_RX_PMD_LOCKED_GET_IN_LEN   (0)
#define XMOD_PHY_RX_PMD_LOCKED_GET_OUT_LEN  (sizeof(uint8_t))
/* XMOD_PHY_LINK_STATUS_GET */
extern int xmod_phy_link_status_get(OUT uint8_t link_status);
#define XMOD_PHY_LINK_STATUS_GET_IN_LEN   (0)
#define XMOD_PHY_LINK_STATUS_GET_OUT_LEN  (sizeof(uint8_t))
/* XMOD_PHY_STATUS_DUMP */
extern int xmod_phy_status_dump(void);
#define XMOD_PHY_STATUS_DUMP_IN_LEN   (0)
#define XMOD_PHY_STATUS_DUMP_OUT_LEN  (0)
/* XMOD_PHY_PCS_USERSPEED_GET */
extern int xmod_phy_pcs_userspeed_get(OUT xmod_pcs_userspeed_config_t config);
#define XMOD_PHY_PCS_USERSPEED_GET_IN_LEN   (0)
#define XMOD_PHY_PCS_USERSPEED_GET_OUT_LEN  (sizeof(xmod_pcs_userspeed_config_t))
/* XMOD_PHY_PCS_USERSPEED_SET */
extern int xmod_phy_pcs_userspeed_set(IN xmod_pcs_userspeed_config_t config);
#define XMOD_PHY_PCS_USERSPEED_SET_IN_LEN   (sizeof(xmod_pcs_userspeed_config_t))
#define XMOD_PHY_PCS_USERSPEED_SET_OUT_LEN  (0)
/* XMOD_PORT_INIT */
extern int xmod_port_init(IN xmod_port_config_t port_config);
#define XMOD_PORT_INIT_IN_LEN   (sizeof(xmod_port_config_t))
#define XMOD_PORT_INIT_OUT_LEN  (0)
/* XMOD_PHY_AN_TRY_ENABLE */
extern int xmod_phy_an_try_enable(IN xmod_an_try_enable_control_t an_config, OUT xmod_an_status_t an_status);
#define XMOD_PHY_AN_TRY_ENABLE_IN_LEN   (sizeof(xmod_an_try_enable_control_t))
#define XMOD_PHY_AN_TRY_ENABLE_OUT_LEN  (sizeof(xmod_an_status_t))
/* XMOD_PE_INIT */
extern int xmod_pe_init(IN xmod_pe_config_t config);
#define XMOD_PE_INIT_IN_LEN   (sizeof(xmod_pe_config_t))
#define XMOD_PE_INIT_OUT_LEN  (0)
/* XMOD_PE_STATUS_GET */
extern int xmod_pe_status_get(OUT xmod_pe_status_t status);
#define XMOD_PE_STATUS_GET_IN_LEN   (0)
#define XMOD_PE_STATUS_GET_OUT_LEN  (sizeof(xmod_pe_status_t))
/* XMOD_PE_PORT_STATS_GET */
extern int xmod_pe_port_stats_get(IN uint8_t port, OUT xmod_pe_port_stats_t stats, IN uint8_t reset);
#define XMOD_PE_PORT_STATS_GET_IN_LEN   (sizeof(uint8_t)+sizeof(uint8_t))
#define XMOD_PE_PORT_STATS_GET_OUT_LEN  (sizeof(xmod_pe_port_stats_t))
/* XMOD_PE_PCID_SET */
extern int xmod_pe_pcid_set(IN xmod_pe_pcid_t pcid);
#define XMOD_PE_PCID_SET_IN_LEN   (sizeof(xmod_pe_pcid_t))
#define XMOD_PE_PCID_SET_OUT_LEN  (0)
/* XMOD_PE_FW_VER_GET */
extern int xmod_pe_fw_ver_get(OUT xmod_pe_fw_ver_t fw_ver);
#define XMOD_PE_FW_VER_GET_IN_LEN   (0)
#define XMOD_PE_FW_VER_GET_OUT_LEN  (sizeof(xmod_pe_fw_ver_t))
extern int xmod_port_enable_set(IN xmod_port_enable_control_t);
#define XMOD_PORT_ENABLE_SET_OUT_LEN (0)
#define XMOD_PORT_ENABLE_SET_IN_LEN  (sizeof(xmod_port_enable_control_t))
extern int xmod_port_enable_get(OUT xmod_port_enable_control_t);
#define XMOD_PORT_ENABLE_GET_OUT_LEN (sizeof(xmod_port_enable_control_t))
#define XMOD_PORT_ENABLE_GET_IN_LEN  (0)
extern int xmod_port_forced_speed_line_side_set(IN uint32_t enable);
#define XMOD_PHY_FORCED_SPEED_LINE_SIDE_SET_OUT_LEN (0)
#define XMOD_PHY_FORCED_SPEED_LINE_SIDE_SET_IN_LEN  (sizeof(uint32_t))
extern int xmod_port_forced_speed_line_side_get(IN uint32_t enable);
#define XMOD_PHY_FORCED_SPEED_LINE_SIDE_GET_OUT_LEN (sizeof(uint32_t))
#define XMOD_PHY_FORCED_SPEED_LINE_SIDE_GET_IN_LEN  (0)
#define XMOD_PHY_FLOW_CONTROL_MODE_SET_OUT_LEN      (0)
#define XMOD_PHY_FLOW_CONTROL_MODE_SET_IN_LEN       (sizeof(xmod_flow_ctrl_t))
#define XMOD_PHY_FLOW_CONTROL_MODE_GET_OUT_LEN      (sizeof(uint32_t))
#define XMOD_PHY_FLOW_CONTROL_MODE_GET_IN_LEN       (sizeof(uint32_t))
#define XMOD_PHY_PSM_COS_BMP_SET_OUT_LEN            (0)
#define XMOD_PHY_PSM_COS_BMP_SET_IN_LEN             (sizeof(uint32_t))
#define XMOD_PHY_PSM_COS_BMP_GET_OUT_LEN            (sizeof(uint32_t))
#define XMOD_PHY_PSM_COS_BMP_GET_IN_LEN             (0)
extern int xmod_port_autoneg_line_side_set(IN uint32_t enable);
#define XMOD_PHY_AUTONEG_LINE_SIDE_SET_OUT_LEN      (0)
#define XMOD_PHY_AUTONEG_LINE_SIDE_SET_IN_LEN       (sizeof(uint32_t))
extern int xmod_port_autoneg_line_side_get(IN uint32_t enable);
#define XMOD_PHY_AUTONEG_LINE_SIDE_GET_OUT_LEN      (sizeof(uint32_t))
#define XMOD_PHY_AUTONEG_LINE_SIDE_GET_IN_LEN       (0)
#define XMOD_PHY_RESET_INTF_OUT_LEN                 (0)
#define XMOD_PHY_RESET_INTF_IN_LEN                  (0)
#define XMOD_PHY_FW_LANE_CFG_GET_OUT_LEN            (sizeof(xmod_firmware_lane_config_t))
#define XMOD_PHY_FW_LANE_CFG_GET_IN_LEN             (sizeof(uint32_t))
#define XMOD_PHY_FW_LANE_CFG_SET_OUT_LEN            (0)
#define XMOD_PHY_FW_LANE_CFG_SET_IN_LEN             (sizeof(xmod_firmware_lane_config_t))

/* XMOD_FC_CMDS & LENGTHS */
/* XMOD_BFCMAP_PORT_CONFIG_GET */
extern int xmod_bfcmap_port_config_get(OUT xmod_bfcmap_port_config_t port_conf);
#define XMOD_BFCMAP_PORT_CONFIG_GET_IN_LEN   (sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t))
#define XMOD_BFCMAP_PORT_CONFIG_GET_OUT_LEN  (sizeof(xmod_bfcmap_port_config_t))
/* XMOD_BFCMAP_PORT_CONFIG_SET */
extern int xmod_bfcmap_port_config_set(IN xmod_bfcmap_port_config_t port_conf);
#define XMOD_BFCMAP_PORT_CONFIG_SET_IN_LEN   (sizeof(xmod_bfcmap_port_config_t))
#define XMOD_BFCMAP_PORT_CONFIG_SET_OUT_LEN  (0)
/* XMOD_BFCMAP_PORT_CMD_SET */
extern int xmod_bfcmap_port_cmd_set(IN xmod_bfcmap_port_cmd_t cmd);
#define XMOD_BFCMAP_PORT_CMD_SET_IN_LEN   (sizeof(xmod_bfcmap_port_cmd_t))
#define XMOD_BFCMAP_PORT_CMD_SET_OUT_LEN  (0)
/* XMOD_BFCMAP_PORT_SPEED_SET */
extern int xmod_bfcmap_port_speed_set(IN xmod_bfcmap_port_speed_t port_speed);
#define XMOD_BFCMAP_PORT_SPEED_SET_IN_LEN   (sizeof(xmod_bfcmap_port_speed_t))
#define XMOD_BFCMAP_PORT_SPEED_SET_OUT_LEN  (0)
/* XMOD_BFCMAP_PORT_LINK_FAULT_TRIGGER_RC_GET */
extern int xmod_bfcmap_port_link_fault_trigger_rc_get(OUT xmod_bfcmap_lf_tr_t lf_trigger, OUT xmod_bfcmap_lf_rc_t lf_rc);
#define XMOD_BFCMAP_PORT_LINK_FAULT_TRIGGER_RC_GET_IN_LEN   (0)
#define XMOD_BFCMAP_PORT_LINK_FAULT_TRIGGER_RC_GET_OUT_LEN  (sizeof(xmod_bfcmap_lf_tr_t) + sizeof(xmod_bfcmap_lf_rc_t))
/* XMOD_BFCMAP_PORT_DIAG_GET */
extern int xmod_bfcmap_port_diag_get(OUT xmod_bfcmap_diag_code_t diag_code);
#define XMOD_BFCMAP_PORT_DIAG_GET_IN_LEN   (0)
#define XMOD_BFCMAP_PORT_DIAG_GET_OUT_LEN  (sizeof(xmod_bfcmap_diag_code_t))
/* XMOD_BFCMAP_PORT_ABIL_GET */
extern int xmod_bfcmap_port_abil_get(OUT xmod_bfcmap_port_ability_t abil_mask);
#define XMOD_BFCMAP_PORT_ABIL_GET_IN_LEN   (0)
#define XMOD_BFCMAP_PORT_ABIL_GET_OUT_LEN  (sizeof(xmod_bfcmap_port_ability_t))
/* XMOD_BFCMAP_PORT_ABIL_SET */
extern int xmod_bfcmap_port_abil_set(IN xmod_bfcmap_port_ability_t abil_mask);
#define XMOD_BFCMAP_PORT_ABIL_SET_IN_LEN   (sizeof(xmod_bfcmap_port_ability_t))
#define XMOD_BFCMAP_PORT_ABIL_SET_OUT_LEN  (0)
/* XMOD_BFCMAP_VLAN_MAP_GET */
extern int xmod_bfcmap_vlan_map_get(OUT xmod_bfcmap_vlan_vsan_map_t vlan);
#define XMOD_BFCMAP_VLAN_MAP_GET_IN_LEN   (sizeof(xmod_bfcmap_vlan_vsan_map_t))
#define XMOD_BFCMAP_VLAN_MAP_GET_OUT_LEN  (sizeof(xmod_bfcmap_vlan_vsan_map_t))
/* XMOD_BFCMAP_VLAN_MAP_ADD */
extern int xmod_bfcmap_vlan_map_add(IN xmod_bfcmap_vlan_vsan_map_t vlan);
#define XMOD_BFCMAP_VLAN_MAP_ADD_IN_LEN   (sizeof(xmod_bfcmap_vlan_vsan_map_t))
#define XMOD_BFCMAP_VLAN_MAP_ADD_OUT_LEN  (0)
/* XMOD_BFCMAP_VLAN_MAP_DELETE */
extern int xmod_bfcmap_vlan_map_delete(IN xmod_bfcmap_vlan_vsan_map_t vlan);
#define XMOD_BFCMAP_VLAN_MAP_DELETE_IN_LEN   (sizeof(xmod_bfcmap_vlan_vsan_map_t))
#define XMOD_BFCMAP_VLAN_MAP_DELETE_OUT_LEN  (0)
/* XMOD_BFCMAP_PORT_INIT */
extern int xmod_bfcmap_port_init(IN xmod_port_config_t port_cfg);
#define XMOD_BFCMAP_PORT_INIT_IN_LEN   (sizeof(xmod_port_config_t))
#define XMOD_BFCMAP_PORT_INIT_OUT_LEN  (0)

typedef struct xmod_api_length_s
{
  int in_len;
  int out_len;
} xmod_api_length_t;

extern const xmod_api_length_t xmod_core_cmds_arg_len_tbl[];
#define GET_XMOD_CORE_CMD_IN_LEN(idx)   (xmod_core_cmds_arg_len_tbl[idx].in_len)
#define GET_XMOD_CORE_CMD_OUT_LEN(idx)  (xmod_core_cmds_arg_len_tbl[idx].out_len)

extern const xmod_api_length_t xmod_ethernet_cmds_arg_len_tbl[];
#define GET_XMOD_ETHERNET_CMD_IN_LEN(idx)   (xmod_ethernet_cmds_arg_len_tbl[idx].in_len)
#define GET_XMOD_ETHERNET_CMD_OUT_LEN(idx)  (xmod_ethernet_cmds_arg_len_tbl[idx].out_len)

extern const xmod_api_length_t xmod_fc_cmds_arg_len_tbl[];
#define GET_XMOD_FC_CMD_IN_LEN(idx)   (xmod_fc_cmds_arg_len_tbl[idx].in_len)
#define GET_XMOD_FC_CMD_OUT_LEN(idx)  (xmod_fc_cmds_arg_len_tbl[idx].out_len)

#define GET_XMOD_BUF_WORD_LEN(len)   ((len+3)>>2)


/*************************************************************************
 * fw_stat is the alternate to xmod to speed up things..where some operations
 * are done by the FW periodically and stored in a fixed buffer, which the
 * SDK can read via AXI access.
 * Note : This is Word offset. Max 63 (256 bytes per core)
 */
#define FWSTAT_PHY_AUTONEG_GET_OFFSET                   0
#define FWSTAT_PHY_INTERFACE_CONFIG_GET_OFFSET          4

/* xmod desc  */
typedef struct _xmod_buf_desc {
    const phymod_access_t *pa;
    int mode_opcode;
    uint32_t *xmodtxbuff;
    int xmodtxlen;
    uint32_t *xmodrxbuff;
    int xmodrxlen;
} xmod_buf_desc;

#endif /* __PHY8806X_XMOD_API_H__ */
