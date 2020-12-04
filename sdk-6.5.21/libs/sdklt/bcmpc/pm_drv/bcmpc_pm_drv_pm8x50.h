/*! \file bcmpc_pm_drv_pm8x50.h
 *
 * PM8x50 PortMacro Manager private definitions.
 *
 * Declaration of the structures, enumerations, and static tables for
 * the PM8x50 PortMacro Manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_PM_DRV_PM8X50_H
#define BCMPC_PM_DRV_PM8X50_H

/*! Macros for PM8x50 used VCO bit map. */
/*! Require 20.625G VCO. */
#define PM8X50_VCO_BMP_20P625G 0x1
/*! Require 25.781G VCO. */
#define PM8X50_VCO_BMP_25P781G 0x2
/*! Require 26.562G VCO. */
#define PM8X50_VCO_BMP_26P562G 0x4

/*! Set the request for 20.625G VCO in VCO bitmap. */
#define PM8X50_VCO_BMP_20P625G_SET(vco_bm) ((vco_bm) |= PM8X50_VCO_BMP_20P625G)
/*! Set the request for 25.781G VCO in VCO bitmap. */
#define PM8X50_VCO_BMP_25P781G_SET(vco_bm) ((vco_bm) |= PM8X50_VCO_BMP_25P781G)
/*! Set the request for 26.562G VCO in VCO bitmap. */
#define PM8X50_VCO_BMP_26P562G_SET(vco_bm) ((vco_bm) |= PM8X50_VCO_BMP_26P562G)

/*! Clear the request for 20.625G VCO in VCO bitmap. */
#define PM8X50_VCO_BMP_20P625G_CLR(vco_bm) ((vco_bm) &= ~PM8X50_VCO_BMP_20P625G)
/*! Clear the request for 25.781G VCO in VCO bitmap. */
#define PM8X50_VCO_BMP_25P781G_CLR(vco_bm) ((vco_bm) &= ~PM8X50_VCO_BMP_25P781G)
/*! Clear the request for 26.562G VCO in VCO bitmap. */
#define PM8X50_VCO_BMP_26P562G_CLR(vco_bm) ((vco_bm) &= ~PM8X50_VCO_BMP_26P562G)

/*! Get the request for 20.625G VCO in VCO bitmap. */
#define PM8X50_VCO_BMP_20P625G_GET(vco_bm)\
        ((vco_bm) & PM8X50_VCO_BMP_20P625G ? 1: 0)
/*! Get the request for 25.781G VCO in VCO bitmap. */
#define PM8X50_VCO_BMP_25P781G_GET(vco_bm)\
        ((vco_bm) & PM8X50_VCO_BMP_25P781G ? 1: 0)
/*! Get the request for 26.562G VCO in VCO bitmap. */
#define PM8X50_VCO_BMP_26P562G_GET(vco_bm)\
        ((vco_bm) & PM8X50_VCO_BMP_26P562G ? 1: 0)
/*! Check if only 20.625G VCO is requested in VCO bitmap. */
#define PM8X50_VCO_BMP_20P625G_ONLY(vco_bm)\
        (vco_bm == PM8X50_VCO_BMP_20P625G ? 1: 0)

/*! Flags for VCO validation. */
/*! No PLL is allowed to switch during VCO validation. */
#define PM_VCO_VALIDATE_F_PLL_SWITCH_NOT_ALLOWED 0x1
/*! Only one PLL is allowed to switch during VCO validation. */
#define PM_VCO_VALIDATE_F_ONE_PLL_SWITCH_ALLOWED 0x2

/*! Macros for 400G autonegotiation (AN) workaround (WAR). */
/*! Register address for AN Software control status information. */
#define AN_X4_SW_CTRL_STS_REG 0xc1e6
/*! Mask for AN state: AN complete. */
#define AN_X4_SW_CTRL_STS_AN_COMPLETE_MASK 0x8000
/*! Register address for CL73 configure register. */
#define AN_X4_CL73_CFG_REG 0xc1c0
/*! Mask for AN disable. */
#define AN_X4_CL73_CFG_AN_DISABLE_MASK 0xfaff
/*! Register address for main setup register. */
#define MAIN0_SETUP_REG 0x9000
/*! Mask for clearing prot mode. */
#define MAIN0_SETUP_PORT_MODE_CLEAR 0xff8f
/*! Mask for setting single octal port mode. */
#define MAIN0_SETUP_PORT_MODE_OCTAL 0x50
/*! Register address for speed control register. */
#define SPEED_CTRL_REG  0xc050
/*! Mask for clearing speed ID. */
#define SPEED_CTRL_SPEED_ID_CLEAR 0xffc0
/*! 400G speed ID. */
#define SPEED_ID_400G    0xa
/*! Register address for AN LP base page. */
#define AN_X4_LP_BASE1r_REG 0xc1d3
/*! Mask for pause request in LP base page. */
#define AN_LP_BASE1r_PAGE_PAUSE_MASK   0x3
/*! Offset for pause request in LP base page. */
#define AN_LP_BASE1r_PAGE_PAUSE_OFFSET 0xa

/*! 400G AN WAR state definitions. */
typedef enum pm8x50_port_400g_an_state_e {
    PM8X50_PORT_AN_CHECK_PAGE_COMPLETE,
    PM8X50_PORT_AN_CHECK_PCS_LINKUP,
    PM8X50_PORT_AN_CREDIT_RESET,
    PM8X50_PORT_AN_DISABLE,
    PM8X50_PORT_AN_RESTART,
    PM8X50_PORT_AN_COMPLETE,
    PM8X50_PORT_AN_STATE_COUNT
} pm8x50_port_400g_an_state_t;

/*! three CW bad recover state definitions. */
typedef enum pm8x50_port_3_cw_bad_state_e {
    PM8X50_PORT_3_CW_BAD_RECOVER,
    PM8X50_PORT_3_CW_BAD_CHECK,
    PM8X50_PORT_3_CW_BAD_LINK_WAIT,
    PM8X50_PORT_3_CW_BAD_COMPLETED,
    PM8X50_PORT_3_CW_BAD_STATE_COUNT
} pm8x50_port_3_cw_bad_state_t;


/*! VCOs bitmap type. */
typedef uint32_t pm8x50_vcos_bmp_t;

/*! Entries of the AN ability table.
 * Each entry specifies a unique AN
 * speed ability and its associated VCO rate.
 */
typedef struct pm8x50_an_ability_table_entry_s
{
    /*! Port speed in Mbps. */
    uint32_t speed;

    /*! Number of lanes. */
    uint32_t num_lanes;

    /*! FEC type. */
    pm_port_fec_t fec;

    /*! AN mode, such as CL73, CL73BAM, MSA. */
    pm_port_autoneg_mode_t an_mode;

    /*! VCO value in Mhz. */
    uint32_t vco;

} pm8x50_autoneg_ability_table_entry_t;

/*! A comprehensive list of PM8X50 GEN1 AN abilities
 * and their VCO rates. */
static const pm8x50_autoneg_ability_table_entry_t
                                          pm8x50_autoneg_ability_table[] =
{
    /*! Speed, number of lanes, FEC type, AN mode, VCO rate. */
    {
        10000,  1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_20P625G
    },
    {
        10000,  1, PM_PORT_FEC_BASE_R,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_20P625G
    },
    {
        20000,  1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_20P625G
    },
    {
        20000,  1, PM_PORT_FEC_BASE_R,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_20P625G
    },
    {
        40000,  2, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_20P625G
    },
    {
        40000,  4, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_20P625G
    },
    {
        40000,  4, PM_PORT_FEC_BASE_R,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_20P625G
    },
    {
        25000,  1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_25P781G
    },
    {
        25000,  1, PM_PORT_FEC_BASE_R,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_25P781G
    },
    {
        25000,  1, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_25P781G
    },
    {
        25000,  1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_25P781G
    },
    {
        25000,  1, PM_PORT_FEC_BASE_R,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_25P781G
    },
    {
        25000,  1, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_25P781G
    },
    {
        25000,  1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        25000,  1, PM_PORT_FEC_BASE_R,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        25000,  1, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        50000,  1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        50000,  1, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        50000,  2, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_25P781G
    },
    {
        50000,  2, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_25P781G
    },
    {
        50000,  2, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        50000,  2, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        100000, 2, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        100000, 2, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        100000, 4, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        100000, 4, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_25P781G
    },
    {
        200000, 4, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        50000,  1, PM_PORT_FEC_RS_544,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_26P562G
    },
    {
        50000,  2, PM_PORT_FEC_RS_544,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_26P562G
    },
    {
        100000, 2, PM_PORT_FEC_RS_544,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_26P562G
    },
    {
        100000, 4, PM_PORT_FEC_RS_544,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_26P562G
    },
    {
        200000, 4, PM_PORT_FEC_RS_544_2XN,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_26P562G
    },
    {
        200000, 4, PM_PORT_FEC_RS_544,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_26P562G
    },
    {
        400000, 8, PM_PORT_FEC_RS_544_2XN,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_26P562G
    }
};

/*! \brief PM8X50 speed ability entry type.
 *
 * Each entry specifies a unique port speed ability and its associated VCO rate.
 */
typedef struct pm8x50_force_speed_ability_entry_s
{
    /*! Port speed in Mbps. */
    uint32_t speed;

    /*! Number of lanes. */
    uint32_t num_lanes;

    /*! FEC type. */
    pm_port_fec_t fec;

    /*! VCO value. */
    pm_vco_t vco;

} pm8x50_force_speed_ability_entry_t;

/*! PM8X50 GEN1 force speed abilities and their VCO rates. */
static const pm8x50_force_speed_ability_entry_t
                                        pm8x50_force_speed_ability_table[] =
{
    /* port_speed, num_lanes, fec_type, vco */
    { 10000,  1, PM_PORT_FEC_NONE,       PM_VCO_20P625G },
    { 10000,  1, PM_PORT_FEC_BASE_R,     PM_VCO_20P625G },
    { 20000,  1, PM_PORT_FEC_NONE,       PM_VCO_20P625G },
    { 20000,  1, PM_PORT_FEC_BASE_R,     PM_VCO_20P625G },
    { 40000,  4, PM_PORT_FEC_NONE,       PM_VCO_20P625G },
    { 40000,  4, PM_PORT_FEC_BASE_R,     PM_VCO_20P625G },
    { 40000,  2, PM_PORT_FEC_NONE,       PM_VCO_20P625G },
    { 25000,  1, PM_PORT_FEC_NONE,       PM_VCO_25P781G },
    { 25000,  1, PM_PORT_FEC_BASE_R,     PM_VCO_25P781G },
    { 25000,  1, PM_PORT_FEC_RS_528,     PM_VCO_25P781G },
    { 50000,  1, PM_PORT_FEC_RS_528,     PM_VCO_25P781G },
    { 50000,  2, PM_PORT_FEC_NONE,       PM_VCO_25P781G },
    { 50000,  2, PM_PORT_FEC_RS_528,     PM_VCO_25P781G },
    { 100000, 2, PM_PORT_FEC_NONE,       PM_VCO_25P781G },
    { 100000, 2, PM_PORT_FEC_RS_528,     PM_VCO_25P781G },
    { 100000, 4, PM_PORT_FEC_NONE,       PM_VCO_25P781G },
    { 100000, 4, PM_PORT_FEC_RS_528,     PM_VCO_25P781G },
    { 200000, 4, PM_PORT_FEC_NONE,       PM_VCO_25P781G },
    { 50000,  1, PM_PORT_FEC_RS_544,     PM_VCO_26P562G },
    { 50000,  1, PM_PORT_FEC_RS_272,     PM_VCO_26P562G },
    { 50000,  2, PM_PORT_FEC_RS_544,     PM_VCO_26P562G },
    { 100000, 2, PM_PORT_FEC_RS_272,     PM_VCO_26P562G },
    { 100000, 2, PM_PORT_FEC_RS_544,     PM_VCO_26P562G },
    { 100000, 4, PM_PORT_FEC_RS_544,     PM_VCO_26P562G },
    { 200000, 4, PM_PORT_FEC_RS_272,     PM_VCO_26P562G },
    { 200000, 4, PM_PORT_FEC_RS_544,     PM_VCO_26P562G },
    { 200000, 4, PM_PORT_FEC_RS_544_2XN, PM_VCO_26P562G },
    { 400000, 8, PM_PORT_FEC_RS_544_2XN, PM_VCO_26P562G }
};

/*! A comprehensive list of PM8X50 GEN2 AN abilities
 * and their VCO rates. */
static const pm8x50_autoneg_ability_table_entry_t
                                         pm8x50_gen2_autoneg_ability_table[] =
{
    /*! Speed, number of lanes, FEC type, AN mode, VCO rate. */
    {
        10000,  1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_20P625G
    },
    {
        10000,  1, PM_PORT_FEC_BASE_R,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_20P625G
    },
    {
        20000,  1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_20P625G
    },
    {
        20000,  1, PM_PORT_FEC_BASE_R,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_20P625G
    },
    {
        40000,  2, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_20P625G
    },
    {
        40000,  4, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_20P625G
    },
    {
        40000,  4, PM_PORT_FEC_BASE_R,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_20P625G
    },
    {
        25000,  1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_25P781G
    },
    {
        25000,  1, PM_PORT_FEC_BASE_R,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_25P781G
    },
    {
        25000,  1, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_25P781G
    },
    {
        25000,  1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_25P781G
    },
    {
        25000,  1, PM_PORT_FEC_BASE_R,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_25P781G
    },
    {
        25000,  1, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_25P781G
    },
    {
        25000,  1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        25000,  1, PM_PORT_FEC_BASE_R,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        25000,  1, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        50000,  1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        50000,  1, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        50000,  2, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_25P781G
    },
    {
        50000,  2, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_25P781G
    },
    {
        50000,  2, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        50000,  2, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        100000, 2, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        100000, 2, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        100000, 4, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        100000, 4, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_25P781G
    },
    {
        200000, 4, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_25P781G
    },
    {
        50000,  1, PM_PORT_FEC_RS_544,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_26P562G
    },
    {
        50000,  1, PM_PORT_FEC_RS_272,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_26P562G
    },
    {
        50000,  2, PM_PORT_FEC_RS_544,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_26P562G
    },
    {
        100000, 2, PM_PORT_FEC_RS_544,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_26P562G
    },
    {
        100000, 2, PM_PORT_FEC_RS_272,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_26P562G
    },
    {
        100000, 4, PM_PORT_FEC_RS_544,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_26P562G
    },
    {
        200000, 4, PM_PORT_FEC_RS_544_2XN,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_26P562G
    },
    {
        200000, 4, PM_PORT_FEC_RS_544,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_26P562G
    },
    {
        200000, 4, PM_PORT_FEC_RS_272_2XN,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_26P562G
    },
    {
        400000, 8, PM_PORT_FEC_RS_544_2XN,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_26P562G
    }
};

/*! PM8X50 GEN2 force speed abilities and their VCO rates. */
static const pm8x50_force_speed_ability_entry_t
                                      pm8x50_gen2_force_speed_ability_table[] =
{
    /* port_speed, num_lanes, fec_type, vco */
    { 10000,  1, PM_PORT_FEC_NONE,       PM_VCO_20P625G },
    { 10000,  1, PM_PORT_FEC_BASE_R,     PM_VCO_20P625G },
    { 20000,  1, PM_PORT_FEC_NONE,       PM_VCO_20P625G },
    { 20000,  1, PM_PORT_FEC_BASE_R,     PM_VCO_20P625G },
    { 40000,  4, PM_PORT_FEC_NONE,       PM_VCO_20P625G },
    { 40000,  4, PM_PORT_FEC_BASE_R,     PM_VCO_20P625G },
    { 40000,  2, PM_PORT_FEC_NONE,       PM_VCO_20P625G },
    { 25000,  1, PM_PORT_FEC_NONE,       PM_VCO_25P781G },
    { 25000,  1, PM_PORT_FEC_BASE_R,     PM_VCO_25P781G },
    { 25000,  1, PM_PORT_FEC_RS_528,     PM_VCO_25P781G },
    { 50000,  1, PM_PORT_FEC_NONE,       PM_VCO_25P781G },
    { 50000,  1, PM_PORT_FEC_RS_528,     PM_VCO_25P781G },
    { 50000,  2, PM_PORT_FEC_NONE,       PM_VCO_25P781G },
    { 50000,  2, PM_PORT_FEC_RS_528,     PM_VCO_25P781G },
    { 100000, 2, PM_PORT_FEC_NONE,       PM_VCO_25P781G },
    { 100000, 2, PM_PORT_FEC_RS_528,     PM_VCO_25P781G },
    { 100000, 4, PM_PORT_FEC_NONE,       PM_VCO_25P781G },
    { 100000, 4, PM_PORT_FEC_RS_528,     PM_VCO_25P781G },
    { 200000, 4, PM_PORT_FEC_NONE,       PM_VCO_25P781G },
    { 50000,  1, PM_PORT_FEC_RS_544,     PM_VCO_26P562G },
    { 50000,  1, PM_PORT_FEC_RS_272,     PM_VCO_26P562G },
    { 50000,  2, PM_PORT_FEC_RS_544,     PM_VCO_26P562G },
    { 100000, 2, PM_PORT_FEC_RS_272,     PM_VCO_26P562G },
    { 100000, 2, PM_PORT_FEC_RS_544,     PM_VCO_26P562G },
    { 100000, 4, PM_PORT_FEC_RS_544,     PM_VCO_26P562G },
    { 200000, 4, PM_PORT_FEC_RS_544,     PM_VCO_26P562G },
    { 200000, 4, PM_PORT_FEC_RS_544_2XN, PM_VCO_26P562G },
    { 200000, 4, PM_PORT_FEC_RS_272,     PM_VCO_26P562G },
    { 200000, 4, PM_PORT_FEC_RS_272_2XN, PM_VCO_26P562G },
    { 400000, 8, PM_PORT_FEC_RS_544_2XN, PM_VCO_26P562G },
    { 400000, 8, PM_PORT_FEC_RS_272_2XN, PM_VCO_26P562G }
};

#endif /* BCMPC_PM_DRV_PM8X50_H */
