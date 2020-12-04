/*! \file bcmpc_pm_drv_pm8x100_dpll.h
 *
 * PM8x50 Gen3 PortMacro Manager private definitions.
 *
 * Declaration of the structures, enumerations and static tables for
 * the PM8x100 PortMacro Manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_PM_DRV_PM8X100_DPLL_H
#define BCMPC_PM_DRV_PM8X100_DPLL_H

/*! Macros for PM8x100_dpll used VCO bit map. */
/*! Require 51.563G VCO. */
#define PM8X100_DPLL_VCO_BMP_51P563G 0x1
/*! Require 53.125G VCO. */
#define PM8X100_DPLL_VCO_BMP_53P125G 0x2

/*! Set the request for 51.563G VCO in VCO bitmap. */
#define PM8X100_DPLL_VCO_BMP_51P563G_SET(vco_bm) ((vco_bm) |= PM8X100_DPLL_VCO_BMP_51P563G)
/*! Set the request for 53.125G VCO in VCO bitmap. */
#define PM8X100_DPLL_VCO_BMP_53P125G_SET(vco_bm) ((vco_bm) |= PM8X100_DPLL_VCO_BMP_53P125G)

/*! Clear the request for 51.563G VCO in VCO bitmap. */
#define PM8X100_DPLL_VCO_BMP_51P563G_CLR(vco_bm) ((vco_bm) &= ~PM8X100_DPLL_VCO_BMP_51P563G)
/*! Clear the request for 53.125G VCO in VCO bitmap. */
#define PM8X100_DPLL_VCO_BMP_53P125G_CLR(vco_bm) ((vco_bm) &= ~PM8X100_DPLL_VCO_BMP_53P125G)

/*! Get the request for 51.563G VCO in VCO bitmap. */
#define PM8X100_DPLL_VCO_BMP_51P563G_GET(vco_bm)\
        ((vco_bm) & PM8X100_DPLL_VCO_BMP_51P563G ? 1: 0)
/*! Get the request for 53.125G VCO in VCO bitmap. */
#define PM8X100_DPLL_VCO_BMP_53P125G_GET(vco_bm)\
        ((vco_bm) & PM8X100_DPLL_VCO_BMP_53P125G ? 1: 0)

/*! Flags for VCO validation. */
/*! No PLL is allowed to switch during VCO validation. */
#define PM_VCO_VALIDATE_F_PLL_SWITCH_NOT_ALLOWED 0x1
/*! Only one PLL is allowed to switch during VCO validation. */
#define PM_VCO_VALIDATE_F_ONE_PLL_SWITCH_ALLOWED 0x2


/*! VCOs bitmap type. */
typedef uint32_t pm8x100_dpll_vcos_bmp_t;


/*!
 * Entries of the AN ability table.
 * Each entry specifies a unique AN speed
 * ability and its associated VCO rate.
 */
typedef struct pm8x100_dpll_an_ability_table_entry_s
{
    /*! Port speed in Mbps. */
    uint32_t speed;

    /*! Number of PMD lanes. */
    uint32_t num_lanes;

    /*! FEC type. */
    pm_port_fec_t fec;

    /*! AN mode, such as CL73, CL73BAM, MSA. */
    pm_port_autoneg_mode_t an_mode;

    /*! VCO value in Mhz. */
    uint32_t vco;

} pm8x100_dpll_autoneg_ability_table_entry_t;

/*!
 * A comprehensive list of PM8X100 AN abilities
 * and their VCO rates.
 */
static const pm8x100_dpll_autoneg_ability_table_entry_t
                                      pm8x100_dpll_autoneg_ability_table[] =
{
    /*! Speed, number of lanes, FEC type, AN mode, VCO rate.
     * 10G-1lane support both 41.25G and 51.562G VCO with different OS mode.
     * Here set 51.562G VCO as default.
     */
    {
        10000, 1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_51P562G
    },
    {
        25000,  1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_51P562G
    },
    {
        25000,  1, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_51P562G
    },
    {
        25000,  1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_51P562G
    },
    {
        25000,  1, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_51P562G
    },
    {
        25000,  1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_51P562G
    },
    {
        25000,  1, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_51P562G
    },
    /*
     * 40G-4lane support both 41.25G and 51.562G VCO with different OS mode.
     * Here set 51.562G VCO as default.
     */
    {
        40000,  4, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_51P562G
    },
    {
        50000,  1, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_51P562G
    },
    {
        50000,  2, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_51P562G
    },
    {
        50000,  2, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_51P562G
    },
    {
        50000,  2, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_51P562G
    },
    {
        50000,  2, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_51P562G
    },
    {
        100000, 2, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_51P562G
    },
    {
        100000, 2, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_51P562G
    },
    {
        100000, 4, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_51P562G
    },
    {
        100000, 4, PM_PORT_FEC_RS_528,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_51P562G
    },
    {
        200000, 4, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_51P562G
    },
    {
        50000,  1, PM_PORT_FEC_RS_544,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_53P125G
    },
    {
        50000,  1, PM_PORT_FEC_RS_272,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_53P125G
    },
    {
        50000,  2, PM_PORT_FEC_RS_544,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_53P125G
    },
    {
        100000,  1, PM_PORT_FEC_RS_544_2XN,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_53P125G
    },
    {
        100000, 2, PM_PORT_FEC_RS_544,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_53P125G
    },
    {
        100000, 2, PM_PORT_FEC_RS_272,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_53P125G
    },
    {
        200000, 2, PM_PORT_FEC_RS_544_2XN,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_53P125G
    },
    {
        200000, 4, PM_PORT_FEC_RS_544_2XN,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_53P125G
    },
    {
        200000, 4, PM_PORT_FEC_RS_544,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_53P125G
    },
    {
        200000, 4, PM_PORT_FEC_RS_272_2XN,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_53P125G
    },
    {
        400000, 4, PM_PORT_FEC_RS_544_2XN,
        PM_PORT_AUTONEG_MODE_CL73,
        PM_VCO_53P125G
    },
    {
        400000, 8, PM_PORT_FEC_RS_544_2XN,
        PM_PORT_AUTONEG_MODE_CL73_MSA,
        PM_VCO_53P125G
    }
};

/*! \brief PM8X100 dpll speed ability entry type.
 *
 * Each entry specifies a unique port speed ability and its associated VCO rate.
 */
typedef struct pm8x100_dpll_force_speed_ability_entry_s
{
    /*! Port speed in Mbps. */
    uint32_t speed;

    /*! Number of PMD lanes. */
    uint32_t num_lanes;

    /*! FEC type. */
    pm_port_fec_t fec;

    /*! VCO value. */
    pm_vco_t vco;

} pm8x100_dpll_force_speed_ability_entry_t;


/*! PM8X100 force speed abilities and their VCO rates. */
static const pm8x100_dpll_force_speed_ability_entry_t
                                    pm8x100_dpll_force_speed_ability_table[] =
{
    /* port_speed, num_lanes, FEC type, VCO rate
     * 10G-1lane support both 41.25G and 51.562G VCO with different
     * OS mode. Here set 51.562G VCO as default.
     */
    { 10000, 1, PM_PORT_FEC_NONE,       PM_VCO_51P562G },
    { 25000, 1, PM_PORT_FEC_NONE,       PM_VCO_51P562G },
    { 25000, 1, PM_PORT_FEC_RS_528,     PM_VCO_51P562G },
    /*
     * 40G-4lane support both 41.25G and 51.562G VCO with different
     * OS mode. Here set 51.562G VCO as default.
     */
    { 40000, 4, PM_PORT_FEC_NONE,       PM_VCO_51P562G },
    /*{ 50000, 1, PM_PORT_FEC_NONE,       PM_VCO_51P562G }, Only in NRZ mode */
    { 50000, 1, PM_PORT_FEC_RS_528,     PM_VCO_51P562G },
    { 50000, 2, PM_PORT_FEC_NONE,       PM_VCO_51P562G },
    { 50000, 2, PM_PORT_FEC_RS_528,     PM_VCO_51P562G },
    {100000, 2, PM_PORT_FEC_NONE,       PM_VCO_51P562G },
    {100000, 2, PM_PORT_FEC_RS_528,     PM_VCO_51P562G },
    {100000, 4, PM_PORT_FEC_NONE,       PM_VCO_51P562G },
    {100000, 4, PM_PORT_FEC_RS_528,     PM_VCO_51P562G },
    {200000, 4, PM_PORT_FEC_NONE,       PM_VCO_51P562G },
    /*{400000, 8, PM_PORT_FEC_NONE,       PM_VCO_51P562G }, Only in NRZ mode */
    { 50000, 1, PM_PORT_FEC_RS_544,     PM_VCO_53P125G },
    { 50000, 1, PM_PORT_FEC_RS_272,     PM_VCO_53P125G },
    { 50000, 2, PM_PORT_FEC_RS_544,     PM_VCO_53P125G },
    {100000, 1, PM_PORT_FEC_RS_544,     PM_VCO_53P125G },
    {100000, 1, PM_PORT_FEC_RS_544_2XN, PM_VCO_53P125G },
    {100000, 1, PM_PORT_FEC_RS_272,     PM_VCO_53P125G },
    {100000, 2, PM_PORT_FEC_RS_544,     PM_VCO_53P125G },
    {100000, 2, PM_PORT_FEC_RS_272,     PM_VCO_53P125G },
    {200000, 2, PM_PORT_FEC_RS_544,     PM_VCO_53P125G },
    {200000, 2, PM_PORT_FEC_RS_544_2XN, PM_VCO_53P125G },
    {200000, 2, PM_PORT_FEC_RS_272,     PM_VCO_53P125G },
    {200000, 2, PM_PORT_FEC_RS_272_2XN, PM_VCO_53P125G },
    {200000, 4, PM_PORT_FEC_RS_544,     PM_VCO_53P125G },
    {200000, 4, PM_PORT_FEC_RS_544_2XN, PM_VCO_53P125G },
    {200000, 4, PM_PORT_FEC_RS_272,     PM_VCO_53P125G },
    {200000, 4, PM_PORT_FEC_RS_272_2XN, PM_VCO_53P125G },
    {200000, 8, PM_PORT_FEC_RS_544_2XN, PM_VCO_53P125G },
    {400000, 4, PM_PORT_FEC_RS_544_2XN, PM_VCO_53P125G },
    {400000, 4, PM_PORT_FEC_RS_272_2XN, PM_VCO_53P125G },
    {400000, 8, PM_PORT_FEC_RS_544_2XN, PM_VCO_53P125G },
    {400000, 8, PM_PORT_FEC_RS_272_2XN, PM_VCO_53P125G }
};


#endif /* BCMPC_PM_DRV_PM8X100_H */
