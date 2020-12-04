/*! \file bcmpc_pm_drv_pm8x100.h
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

#ifndef BCMPC_PM_DRV_PM8X100_H
#define BCMPC_PM_DRV_PM8X100_H


/*!
 * Entries of the AN ability table.
 * Each entry specifies a unique AN speed
 * ability and its associated VCO rate.
 */
typedef struct pm8x100_an_ability_table_entry_s
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

} pm8x100_autoneg_ability_table_entry_t;

/*!
 * A comprehensive list of PM8X100 AN abilities
 * and their VCO rates.
 */
static const pm8x100_autoneg_ability_table_entry_t
                                      pm8x100_autoneg_ability_table[] =
{
    /*! Speed, number of lanes, FEC type, AN mode, VCO rate. */
    {
        20000,  1, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_41P25G
    },
    {
        40000,  2, PM_PORT_FEC_NONE,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
        PM_VCO_41P25G
    },
    /*
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
        100000,  1, PM_PORT_FEC_RS_544,
        PM_PORT_AUTONEG_MODE_CL73_BAM,
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

/*! \brief PM8X50 speed ability entry type.
 *
 * Each entry specifies a unique port speed ability and its associated VCO rate.
 */
typedef struct pm8x100_force_speed_ability_entry_s
{
    /*! Port speed in Mbps. */
    uint32_t speed;

    /*! Number of PMD lanes. */
    uint32_t num_lanes;

    /*! FEC type. */
    pm_port_fec_t fec;

    /*! VCO value. */
    pm_vco_t vco;

} pm8x100_force_speed_ability_entry_t;


/*! PM8X100 force speed abilities and their VCO rates. */
static const pm8x100_force_speed_ability_entry_t
                                    pm8x100_force_speed_ability_table[] =
{
    /* port_speed, num_lanes, FEC type, VCO rate */
    { 20000, 1, PM_PORT_FEC_NONE,       PM_VCO_41P25G },
    { 40000, 2, PM_PORT_FEC_NONE,       PM_VCO_41P25G },
    /*
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
