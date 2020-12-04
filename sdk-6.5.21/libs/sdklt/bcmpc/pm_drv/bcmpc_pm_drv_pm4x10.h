/*! \file bcmpc_pm_drv_pm4x10.h
 *
 * PM4X10 PortMacro Manager private definitions.
 *
 * Declaration of the structures, enumerations, and static tables for
 * the PM4x10 PortMacro Manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_PM_DRV_PM4X10_H
#define BCMPC_PM_DRV_PM4X10_H

/*!
 * Port Macro Receive Statistics Vector (RSV) definitions.
 */
#define PM4X10_RSV_MASK_WRONG_SA             (0x1 << (0))
#define PM4X10_RSV_MASK_STACK_VLAN_DETECT    (0x1 << (1))
/* unsupported PFC DA*/
#define PM4X10_RSV_MASK_PFC_DA_ERR           (0x1 << (2))
/* same bit as PFC */
#define PM4X10_RSV_MASK_PAUSE_DA_ERR         (0x1 << (2))
#define PM4X10_RSV_MASK_RCV_TERM_OR_CODE_ERR (0x1 << (3))
#define PM4X10_RSV_MASK_CRC_ERR              (0x1 << (4))
/* IEEE length check fail */
#define PM4X10_RSV_MASK_FRAME_LEN_ERR        (0x1 << (5))
/* truncated/out of range */
#define PM4X10_RSV_MASK_TRUNCATED_FRAME      (0x1 << (6))
#define PM4X10_RSV_MASK_FRAME_RCV_OK         (0x1 << (7))
#define PM4X10_RSV_MASK_MCAST_FRAME          (0x1 << (8))
#define PM4X10_RSV_MASK_BCAST_FRAME          (0x1 << (9))
#define PM4X10_RSV_MASK_PROMISCUOUS_FRAME    (0x1 << (10))
#define PM4X10_RSV_MASK_CONTROL_FRAME        (0x1 << (11))
#define PM4X10_RSV_MASK_PAUSE_FRAME          (0x1 << (12))
#define PM4X10_RSV_MASK_OPCODE_ERR           (0x1 << (13))
#define PM4X10_RSV_MASK_VLAN_TAG_DETECT      (0x1 << (14))
#define PM4X10_RSV_MASK_UCAST_FRAME          (0x1 << (15))
#define PM4X10_RSV_MASK_RUNT_FRAME           (0x1 << (16))
#define PM4X10_RSV_MASK_RX_FIFO_FULL         (0x1 << (17))
#define PM4X10_RSV_MASK_PFC_FRAME            (0x1 << (18))
#define PM4X10_RSV_MASK_SCH_CRC_ERR          (0x1 << (19))
#define PM4X10_RSV_MASK_MACSEC_FRAME         (0x1 << (20))
#define PM4X10_RSV_MASK_DRIBBLE_NIBBLE_ERR   (0x1 << (21))

#define PM4X10_RSV_MASK_MIN     PM4X10_RSV_MASK_WRONG_SA
#define PM4X10_RSV_MASK_MAX     PM4X10_RSV_MASK_DRIBBLE_NIBBLE_ERR
#define PM4X10_RSV_MASK_ALL     ((PM4X10_RSV_MASK_DRIBBLE_NIBBLE_ERR) | \
                                ((PM4X10_RSV_MASK_DRIBBLE_NIBBLE_ERR) - 1))

/*! Entries of the AN ability table.
 * Each entry specifies a unique AN
 * speed ability and its associated VCO rate.
 */
typedef struct pm4x10_an_ability_table_entry_s {
    /*! Port speed in Mbps. */
    uint32_t speed;

    /*! Number of lanes. */
    uint32_t num_lanes;

    /*! FEC type. */
    pm_port_fec_t fec;

    /*! AN mode, such as CL73, CL37, CL73BAM. */
    pm_port_autoneg_mode_t an_mode;

    /*! AN media type. */
    pm_port_medium_t medium;

    /*! VCO value in Mhz. */
    uint32_t vco;
} pm4x10_autoneg_ability_table_entry_t;

/*! A comprehensive list of pm4x10 AN abilities
 * and their VCO rates. */
static const pm4x10_autoneg_ability_table_entry_t
                                          pm4x10_autoneg_ability_table[] =
{
    /*! Speed, number of lanes, FEC type, AN mode, VCO rate. */
    {
        40000, 4, PM_PORT_FEC_NONE,   PM_PORT_AUTONEG_MODE_CL73,
        PM_PORT_MEDIUM_BACKPLANE,     PM_VCO_10P3125G
    },
    {
        40000, 4, PM_PORT_FEC_NONE,   PM_PORT_AUTONEG_MODE_CL73,
        PM_PORT_MEDIUM_COPPER,        PM_VCO_10P3125G
    },
    {
        40000, 4, PM_PORT_FEC_BASE_R, PM_PORT_AUTONEG_MODE_CL73,
        PM_PORT_MEDIUM_BACKPLANE,     PM_VCO_10P3125G
    },
    {
        40000, 4, PM_PORT_FEC_BASE_R, PM_PORT_AUTONEG_MODE_CL73,
        PM_PORT_MEDIUM_COPPER,        PM_VCO_10P3125G
    },
    {
        10000, 1, PM_PORT_FEC_NONE,   PM_PORT_AUTONEG_MODE_CL73,
        PM_PORT_MEDIUM_BACKPLANE,     PM_VCO_10P3125G
    },
    {
        10000, 1, PM_PORT_FEC_BASE_R, PM_PORT_AUTONEG_MODE_CL73,
        PM_PORT_MEDIUM_BACKPLANE,     PM_VCO_10P3125G
    },
    {
        10000, 2, PM_PORT_FEC_NONE,   PM_PORT_AUTONEG_MODE_CL37_BAM,
        PM_PORT_MEDIUM_BACKPLANE,     PM_VCO_12P5G
    },
    {
        10000, 4, PM_PORT_FEC_NONE,   PM_PORT_AUTONEG_MODE_CL37_BAM,
        PM_PORT_MEDIUM_BACKPLANE,     PM_VCO_12P5G
    },
    {
        10000, 4, PM_PORT_FEC_NONE,   PM_PORT_AUTONEG_MODE_CL73,
        PM_PORT_MEDIUM_BACKPLANE,     PM_VCO_12P5G
    },
    {
        2500,  1, PM_PORT_FEC_NONE,   PM_PORT_AUTONEG_MODE_CL37_BAM,
        PM_PORT_MEDIUM_BACKPLANE,     PM_VCO_10P3125G
    },
    {
        1000,  1, PM_PORT_FEC_NONE,   PM_PORT_AUTONEG_MODE_CL73,
        PM_PORT_MEDIUM_BACKPLANE,     PM_VCO_10P3125G
    },
    {
        1000,  1, PM_PORT_FEC_NONE,   PM_PORT_AUTONEG_MODE_CL37,
        PM_PORT_MEDIUM_FIBER,         PM_VCO_10P3125G
    },
    {
        1000,  1, PM_PORT_FEC_NONE,   PM_PORT_AUTONEG_MODE_SGMII,
        PM_PORT_MEDIUM_BACKPLANE,     PM_VCO_10P3125G
    },
    {
        100,   1, PM_PORT_FEC_NONE,   PM_PORT_AUTONEG_MODE_SGMII,
        PM_PORT_MEDIUM_BACKPLANE,     PM_VCO_10P3125G
    },
    {
        10,    1, PM_PORT_FEC_NONE,   PM_PORT_AUTONEG_MODE_SGMII,
        PM_PORT_MEDIUM_BACKPLANE,     PM_VCO_10P3125G
   }
};

/*! Entries of the force speed ability table.
 */
typedef struct pm4x10_force_speed_ability_table_entry_s {
    /*! Port speed in Mbps. */
    uint32_t speed;

    /*! Number of lanes. */
    uint32_t num_lanes;

    /*! FEC type. */
    pm_port_fec_t fec;

    /*! AN media type. */
    pm_port_medium_t medium;

    /*! VCO value. */
    pm_vco_t vco;
} pm4x10_force_speed_ability_table_entry_t;

/*! pm4x10 force speed abilities. */
static const pm4x10_force_speed_ability_table_entry_t
                                          pm4x10_fs_ability_table[] =
{
    /*! Speed, number of lanes, FEC type, medium, vco */
    {
        10, 1, PM_PORT_FEC_NONE, PM_PORT_MEDIUM_BACKPLANE, PM_VCO_10P3125G
    },
    {
        100, 1, PM_PORT_FEC_NONE, PM_PORT_MEDIUM_BACKPLANE, PM_VCO_10P3125G
    },
    {
        1000, 1, PM_PORT_FEC_NONE, PM_PORT_MEDIUM_BACKPLANE, PM_VCO_10P3125G
    },
    {
        1000, 1, PM_PORT_FEC_NONE, PM_PORT_MEDIUM_FIBER, PM_VCO_10P3125G
    },
    {
        2500, 1, PM_PORT_FEC_NONE, PM_PORT_MEDIUM_BACKPLANE, PM_VCO_10P3125G
    },
    {
        2500, 1, PM_PORT_FEC_NONE, PM_PORT_MEDIUM_FIBER, PM_VCO_10P3125G
    },
    {
        10000, 1, PM_PORT_FEC_NONE, PM_PORT_MEDIUM_COPPER, PM_VCO_10P3125G
    },
    {
        10000, 1, PM_PORT_FEC_BASE_R, PM_PORT_MEDIUM_COPPER, PM_VCO_10P3125G
    },
    {
        10000, 1, PM_PORT_FEC_NONE, PM_PORT_MEDIUM_FIBER, PM_VCO_10P3125G
    },
    {
        10000, 1, PM_PORT_FEC_BASE_R, PM_PORT_MEDIUM_FIBER, PM_VCO_10P3125G
    },
    {
        10000, 1, PM_PORT_FEC_NONE, PM_PORT_MEDIUM_BACKPLANE, PM_VCO_10P3125G
    },
    {
        10000, 1, PM_PORT_FEC_BASE_R, PM_PORT_MEDIUM_BACKPLANE, PM_VCO_10P3125G
    },
    {
        10000, 2, PM_PORT_FEC_NONE, PM_PORT_MEDIUM_COPPER, PM_VCO_12P5G
    },
    {
        10000, 2, PM_PORT_FEC_NONE, PM_PORT_MEDIUM_FIBER, PM_VCO_12P5G
    },
    {
        10000, 4, PM_PORT_FEC_NONE, PM_PORT_MEDIUM_COPPER, PM_VCO_12P5G
    },
    {
        10000, 4, PM_PORT_FEC_NONE, PM_PORT_MEDIUM_BACKPLANE, PM_VCO_12P5G
    },
    {
        40000, 4, PM_PORT_FEC_NONE, PM_PORT_MEDIUM_BACKPLANE, PM_VCO_10P3125G
    },
    {
        40000, 4, PM_PORT_FEC_NONE, PM_PORT_MEDIUM_COPPER, PM_VCO_10P3125G
    },
    {
        40000, 4, PM_PORT_FEC_NONE, PM_PORT_MEDIUM_FIBER, PM_VCO_10P3125G
    },
    {
        40000, 4, PM_PORT_FEC_BASE_R, PM_PORT_MEDIUM_BACKPLANE, PM_VCO_10P3125G
    },
    {
        40000, 4, PM_PORT_FEC_BASE_R, PM_PORT_MEDIUM_COPPER, PM_VCO_10P3125G
    },
    {
        40000, 4, PM_PORT_FEC_BASE_R, PM_PORT_MEDIUM_FIBER, PM_VCO_10P3125G
    }
};

#endif /* BCMPC_PM_DRV_PM4X10_H */

