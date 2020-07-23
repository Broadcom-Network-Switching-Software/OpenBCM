/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * PHY Simulator (requires PHYMOD library)
 */


#ifndef PHY_NUM_SIMS
#define PHY_NUM_SIMS            66
#endif

#if defined(PHYMOD_SUPPORT) && (PHY_NUM_SIMS > 0)
#define PHY_SIM_SUPPORT
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
#define SOC_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_WB_DEFAULT_VERSION            SOC_WB_VERSION_1_0
#endif

#ifdef PHY_SIM_SUPPORT
#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_config.h>
#ifdef PHYMOD_BLACKHAWK_SUPPORT
#include <phymod/chip/blackhawk_sim.h>
#endif
#ifdef PHYMOD_EAGLE_SUPPORT
#include <phymod/chip/eagle_sim.h>
#endif
#ifdef PHYMOD_FALCON_SUPPORT
#include <phymod/chip/falcon_sim.h>
#endif
#ifdef PHYMOD_TSCBH_SUPPORT
#include <phymod/chip/tscbh_sim.h>
#endif
#ifdef PHYMOD_TSCE16_SUPPORT
#include <phymod/chip/tsce16_sim.h>
#endif
#ifdef PHYMOD_TSCE_SUPPORT
#include <phymod/chip/tsce_sim.h>
#endif
#ifdef PHYMOD_TSCF_SUPPORT
#include <phymod/chip/tscf_sim.h>
#endif
#ifdef PHYMOD_TSCBH_GEN2_SUPPORT
#include <phymod/chip/tscbh_gen2_sim.h>
#endif
#ifdef PHYMOD_TSCE7_SUPPORT
#include <phymod/chip/tsce7_sim.h>
#endif
#ifdef PHYMOD_TSCBH_GEN3_SUPPORT
#include <phymod/chip/tscbh_gen3_sim.h>
#endif
#ifdef PHYMOD_TSCO_SUPPORT
#include <phymod/chip/tsco_sim.h>
#endif
#ifdef PHYMOD_TSCO_DPLL_SUPPORT
#include <phymod/chip/tsco_dpll_sim.h>
#endif

#include <phymod/phymod_sim_control.h>

#ifndef PHY_NUM_SIM_ENTRIES
#define PHY_NUM_SIM_ENTRIES     400
#endif

typedef struct phymod_control_phy_sim_s {
    phymod_sim_entry_t pms_entries[PHY_NUM_SIM_ENTRIES];
    phymod_sim_t pms;
    int unit;
    uint32_t phy_id;
} phymod_control_phy_sim_t;


static phymod_control_phy_sim_t phymod_control_phy_sim[PHY_NUM_SIMS];
static int phymod_control_phy_sims_used;


static phymod_control_phy_sim_t *
phymod_control_physim_find(int unit, uint32_t phy_id)
{
    int idx;
    phymod_control_phy_sim_t *psim;

    for (idx = 0; idx < phymod_control_phy_sims_used; idx++) {
        psim = &phymod_control_phy_sim[idx];
        if (psim->unit == unit && psim->phy_id == phy_id) {
            return psim;
        }
    }
    return NULL;
}

int
phymod_control_physim_drv_get(int unit, phymod_dispatch_type_t dispatch_type, phymod_sim_drv_t **drv)
{

    switch(dispatch_type) {
#ifdef PHYMOD_BLACKHAWK_SUPPORT
        case phymodDispatchTypeBlackhawk:
            *drv = &blackhawk_sim_drv;
            break;
#endif
#ifdef PHYMOD_EAGLE_SUPPORT
        case phymodDispatchTypeEagle:
            *drv = &eagle_sim_drv;
            break;
#endif
#ifdef PHYMOD_FALCON_SUPPORT
        case phymodDispatchTypeFalcon:
            *drv = &falcon_sim_drv;
            break;
#endif
#ifdef PHYMOD_TSCBH_SUPPORT
        case phymodDispatchTypeTscbh:
            *drv = &tscbh_sim_drv;
            break;
#endif
#ifdef PHYMOD_TSCE16_SUPPORT
        case phymodDispatchTypeTsce16:
            *drv = &tsce16_sim_drv;
            break;
#endif
#ifdef PHYMOD_TSCE_SUPPORT
        case phymodDispatchTypeTsce:
            *drv = &tsce_sim_drv;
            break;
#endif
#ifdef PHYMOD_TSCF_SUPPORT
        case phymodDispatchTypeTscf:
            *drv = &tscf_sim_drv;
            break;
#endif
#ifdef PHYMOD_TSCBH_GEN2_SUPPORT
        case phymodDispatchTypeTscbh_gen2:
            *drv = &tscbh_gen2_sim_drv;
            break;
#endif
#ifdef PHYMOD_TSCE7_SUPPORT
        case phymodDispatchTypeTsce7:
            *drv = &tsce7_sim_drv;
            break;
#endif
#ifdef PHYMOD_TSCBH_GEN3_SUPPORT
        case phymodDispatchTypeTscbh_gen3:
            *drv = &tscbh_gen3_sim_drv;
            break;
#endif
#ifdef PHYMOD_TSCO_SUPPORT
        case phymodDispatchTypeTsco:
            *drv = &tsco_sim_drv;
            break;
#endif
#ifdef PHYMOD_TSCO_DPLL_SUPPORT
        case phymodDispatchTypeTsco_dpll:
            *drv = &tsco_dpll_sim_drv;
            break;
#endif
        default:
            *drv = NULL;
            PHYMOD_DEBUG_ERROR(
                ("phymod_control_physim_drv_get: Request for un-supported PHYSIM driver\
                 unit=%d dispatch type %d\n", unit, dispatch_type)
           );
    }
    return PHYMOD_E_NONE;
}

int
phymod_control_physim_add(int unit, uint32_t phy_id, phymod_sim_drv_t *pms_drv)
{
    int idx;
    phymod_control_phy_sim_t *psim;

    /* Check if this simulator exists already */
    for (idx = 0; idx < phymod_control_phy_sims_used; idx++) {
        psim = &phymod_control_phy_sim[idx];
        if (psim->unit == unit && psim->phy_id == phy_id) {
            /* Reset simulator */
            PHYMOD_IF_ERR_RETURN(
                phymod_sim_reset(&psim->pms));
            return PHYMOD_E_NONE;
        }
    }

    /* Any free simulators? */
    if (phymod_control_phy_sims_used >= PHY_NUM_SIMS) {
        PHYMOD_DEBUG_ERROR(
                ("phymod_control_physim_add: Out of resources for  unit=%d phy_id=0x%x\n",
                unit, phy_id)
       );
        return PHYMOD_E_RESOURCE;
    }

    /* Add new simulator entry */
    psim = &phymod_control_phy_sim[phymod_control_phy_sims_used++];
    psim->unit = unit;
    psim->phy_id = phy_id;
    psim->pms.drv = pms_drv;

    /* Initialize and reset simulator */
    PHYMOD_IF_ERR_RETURN(
        phymod_sim_init(&psim->pms,
                        COUNTOF(psim->pms_entries), psim->pms_entries));
    PHYMOD_IF_ERR_RETURN(
        phymod_sim_reset(&psim->pms));

    return PHYMOD_E_NONE;
}


int
phymod_control_physim_wrmask(int unit, uint32_t phy_id,
                  uint32_t phy_reg_addr, uint16_t phy_wr_data, uint16_t wr_mask)
{
    phymod_control_phy_sim_t *psim = phymod_control_physim_find(unit, phy_id);
    uint32_t data32;

    if (psim == NULL) {
        return PHYMOD_E_INIT;
    }

    data32 = wr_mask;
    data32 <<= 16;
    data32 |= phy_wr_data;

    PHYMOD_IF_ERR_RETURN(
        phymod_sim_write(&psim->pms, phy_reg_addr, data32));

    PHYMOD_DEBUG_VERBOSE(
            ("phymod_control_physim_wrmask 0x%03x:0x%04x = 0x%04x/0x%04x\n",
              phy_id, phy_reg_addr, phy_wr_data, wr_mask)
            );
    return PHYMOD_E_NONE;
}

int
phymod_control_physim_read(int unit, uint32_t phy_id,
                uint32_t phy_reg_addr, uint16_t *phy_rd_data)
{
    phymod_control_phy_sim_t *psim = phymod_control_physim_find(unit, phy_id);
    uint32_t data32;

    if (psim == NULL) {
        return PHYMOD_E_INIT;
    }

    PHYMOD_IF_ERR_RETURN(
        phymod_sim_read(&psim->pms, phy_reg_addr, &data32));

    *phy_rd_data = data32;

    PHYMOD_DEBUG_VERBOSE(
            ("phymod_control_physim_read 0x%03x:0x%04x = 0x%04x\n",
              phy_id, phy_reg_addr, *phy_rd_data)
   );
    return PHYMOD_E_NONE;
}

int
phymod_control_physim_write(int unit, uint32_t phy_id,
                 uint32_t phy_reg_addr, uint16_t phy_wr_data)
{
    return phymod_control_physim_wrmask(unit, phy_id, phy_reg_addr, phy_wr_data, 0);
}

/* Default simulator bus */
static
int sim_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t* val){
    uint16_t tmp=0;
    int rv;

    (*val) = 0;
    rv = phymod_control_physim_read(0, core_addr, reg_addr, &tmp);
    (*val) = tmp;

    return rv;
}

static
int sim_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val){
    uint16_t tmp = 0;
    uint16_t msk = 0;

    tmp = (val & 0xFFFF);
    msk = ((val >> 16) & 0xFFFF);

    return phymod_control_physim_wrmask(0, core_addr, reg_addr, tmp, msk);
}

static phymod_bus_t phymod_physim_bus = {
    "sdklt_phymod_physim",
    sim_bus_read,
    sim_bus_write,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0
};

int
phymod_control_physim_bus_get(int unit, phymod_bus_t **phymod_sim_bus)
{
    *phymod_sim_bus = &phymod_physim_bus;
    return PHYMOD_E_NONE;
}

#endif /* PHY_SIM_SUPPORT */
