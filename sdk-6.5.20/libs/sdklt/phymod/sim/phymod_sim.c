/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod_sim.h>

int
phymod_sim_init(phymod_sim_t *pms, int num_ent, phymod_sim_entry_t *ent)
{
    if (pms == NULL || pms->drv == NULL || pms->drv->init == NULL) {
        return PHYMOD_E_INIT;
    }
    return pms->drv->init(&pms->data, num_ent, ent);
}

int
phymod_sim_reset(phymod_sim_t *pms)
{
    if (pms == NULL || pms->drv == NULL || pms->drv->reset == NULL) {
        return PHYMOD_E_INIT;
    }
    return pms->drv->reset(&pms->data);
}

int
phymod_sim_read(phymod_sim_t *pms, uint32_t addr, uint32_t *data)
{
    if (pms == NULL || pms->drv == NULL || pms->drv->read == NULL) {
        return PHYMOD_E_INIT;
    }
    return pms->drv->read(&pms->data, addr, data);
}

int
phymod_sim_write(phymod_sim_t *pms, uint32_t addr, uint32_t data)
{
    if (pms == NULL || pms->drv == NULL || pms->drv->write == NULL) {
        return PHYMOD_E_INIT;
    }
    return pms->drv->write(&pms->data, addr, data);
}

int
phymod_sim_event(phymod_sim_t *pms, phymod_sim_event_t event)
{
    if (pms == NULL || pms->drv == NULL || pms->drv->event == NULL) {
        return PHYMOD_E_INIT;
    }
    return pms->drv->event(&pms->data, event);
}
