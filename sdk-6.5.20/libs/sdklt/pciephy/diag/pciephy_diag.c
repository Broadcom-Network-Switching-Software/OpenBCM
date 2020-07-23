/*! \file pciephy_diag.c
 *
 * PCIe diagnostic APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <pciephy/pciephy.h>
#include <pciephy/pciephy_driver.h>

extern pciephy_driver_t pciephy_merlin16_driver;
extern pciephy_driver_t pciephy_merlin7_driver;

/*
 * The order in this array should be same as pciephy_driver_type_t.
 */
static pciephy_driver_t *pciephy_driver_list[PCIEPHY_DRIVER_TYPE_COUNT] = {
    &pciephy_merlin16_driver,
    &pciephy_merlin7_driver,
};

int
pciephy_diag_reg_read(pciephy_access_t *sa, uint16_t address, uint16_t *pdata)
{
    pciephy_driver_type_t type;
    pciephy_driver_t *drv;

    type = sa->type;
    if (type >= PCIEPHY_DRIVER_TYPE_COUNT) {
        return PCIEPHY_E_PARAM;
    }

    drv = pciephy_driver_list[type];
    if (!drv || !drv->read) {
        return PCIEPHY_E_UNAVAIL;
    }

    return drv->read(sa, address, pdata);
}

int
pciephy_diag_reg_write(pciephy_access_t *sa, uint16_t address, uint16_t data)
{
    pciephy_driver_type_t type;
    pciephy_driver_t *drv;

    type = sa->type;
    if (type >= PCIEPHY_DRIVER_TYPE_COUNT) {
        return PCIEPHY_E_PARAM;
    }

    drv = pciephy_driver_list[type];
    if (!drv || !drv->write) {
        return PCIEPHY_E_UNAVAIL;
    }

    return drv->write(sa, address, data);
}

int
pciephy_diag_reg_mwr(pciephy_access_t *sa, uint16_t address, uint16_t mask,
                      uint8_t lsb, uint16_t val)
{
    pciephy_driver_type_t type;
    pciephy_driver_t *drv;

    type = sa->type;
    if (type >= PCIEPHY_DRIVER_TYPE_COUNT) {
        return PCIEPHY_E_PARAM;
    }

    drv = pciephy_driver_list[type];
    if (!drv || !drv->mwr) {
        return PCIEPHY_E_UNAVAIL;
    }

    return drv->mwr(sa, address, mask, lsb, val);
}

int
pciephy_diag_pram_read(pciephy_access_t *sa, uint32_t address, uint32_t size)
{
    pciephy_driver_type_t type;
    pciephy_driver_t *drv;

    type = sa->type;
    if (type >= PCIEPHY_DRIVER_TYPE_COUNT) {
        return PCIEPHY_E_PARAM;
    }

    drv = pciephy_driver_list[type];
    if (!drv || !drv->pram_read) {
        return PCIEPHY_E_UNAVAIL;
    }

    return drv->pram_read(sa, address, size);
}

int pciephy_diag_dsc(pciephy_access_t *sa, int array_size)
{
    pciephy_driver_type_t type;
    pciephy_driver_t *drv;

    type = sa->type;
    if (type >= PCIEPHY_DRIVER_TYPE_COUNT) {
        return PCIEPHY_E_PARAM;
    }

    drv = pciephy_driver_list[type];
    if (!drv || !drv->dsc) {
        return PCIEPHY_E_UNAVAIL;
    }

    return drv->dsc(sa, array_size);
}

int
pciephy_diag_state(pciephy_access_t *sa, int array_size)
{
    pciephy_driver_type_t type;
    pciephy_driver_t *drv;

    type = sa->type;
    if (type >= PCIEPHY_DRIVER_TYPE_COUNT) {
        return PCIEPHY_E_PARAM;
    }

    drv = pciephy_driver_list[type];
    if (!drv || !drv->state) {
        return PCIEPHY_E_UNAVAIL;
    }

    return drv->state(sa, array_size);
}

int
pciephy_diag_eyescan(pciephy_access_t *sa, int array_size)
{
    pciephy_driver_type_t type;
    pciephy_driver_t *drv;

    type = sa->type;
    if (type >= PCIEPHY_DRIVER_TYPE_COUNT) {
        return PCIEPHY_E_PARAM;
    }

    drv = pciephy_driver_list[type];
    if (!drv || !drv->eyescan) {
        return PCIEPHY_E_UNAVAIL;
    }

    return drv->eyescan(sa, array_size);
}
