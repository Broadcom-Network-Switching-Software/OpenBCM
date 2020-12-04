/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * PHY Simulator (requires PHYMOD library)
 */

#include <soc/error.h>

#ifndef PHY_NUM_SIMS
#define PHY_NUM_SIMS            90
#endif

#if defined(PHYMOD_SUPPORT) && (PHY_NUM_SIMS > 0)
#define PHY_SIM_SUPPORT
#endif

#ifdef PHY_SIM_SUPPORT

#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/core/spl.h>
#include <soc/drv.h>
#include <soc/scache.h>



#ifdef BCM_WARM_BOOT_SUPPORT
#define SOC_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_WB_DEFAULT_VERSION            SOC_WB_VERSION_1_0
#endif


#include <phymod/phymod.h>
#include <phymod/chip/eagle_sim.h>

#include <phymod/chip/falcon_sim.h>
#include <phymod/chip/tscf_sim.h>
#include <phymod/chip/tsce_sim.h>
#include <phymod/chip/qsgmiie_sim.h>
#include <phymod/chip/viper_sim.h>
#include <phymod/chip/tscf16_sim.h>
#include <phymod/chip/tsce16_sim.h>
#include <phymod/chip/qtce_sim.h>
#include <phymod/chip/tscbh_sim.h>
#include <phymod/chip/tscf_gen3_sim.h>
#include <phymod/chip/tsce_dpll_sim.h>
#include <phymod/chip/blackhawk_sim.h>
#include <phymod/chip/qtce16_sim.h>
#include <phymod/chip/tsce7_sim.h>
#include <phymod/chip/tscf16_gen3_sim.h>
#include <phymod/chip/tscbh_gen2_sim.h>
#include <phymod/chip/tscbh_flexe_sim.h>
#include <phymod/chip/blackhawk7_v1l8p1_sim.h>

#include <soc/phy/phymod_sim.h>

#ifndef PHY_NUM_SIM_ENTRIES
#define PHY_NUM_SIM_ENTRIES     400
#endif

typedef struct soc_phy_sim_s {
    phymod_sim_entry_t pms_entries[PHY_NUM_SIM_ENTRIES];
    phymod_sim_t pms;
    int unit;
    uint32 phy_id;
} soc_phy_sim_t;

static soc_phy_sim_t soc_phy_sim[PHY_NUM_SIMS];
static int soc_phy_sims_used;


STATIC soc_phy_sim_t *
soc_physim_find(int unit, uint32 phy_id)
{
    int idx;
    soc_phy_sim_t *psim;

    for (idx = 0; idx < soc_phy_sims_used; idx++) {
        psim = &soc_phy_sim[idx];
        if (psim->unit == unit && psim->phy_id == phy_id) {
            return psim;
        }
    }
    return NULL;
}

int
soc_physim_add(int unit, uint32 phy_id, phymod_sim_drv_t *pms_drv)
{
    int idx;
    soc_phy_sim_t *psim;

    /* Check if this simulator exists already */
    for (idx = 0; idx < soc_phy_sims_used; idx++) {
        psim = &soc_phy_sim[idx];
        if (psim->unit == unit && psim->phy_id == phy_id) {
            /* Reset simulator */
            SOC_IF_ERROR_RETURN(
                phymod_sim_reset(&psim->pms));
            return SOC_E_NONE;
        }
    }

    /* Any free simulators? */
    if (soc_phy_sims_used >= PHY_NUM_SIMS) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "soc_physim_add: Out of resources for"
                              " unit=%d phy_id=0x%x\n"), unit, phy_id));
        return SOC_E_RESOURCE;
    }

    /* Add new simulator entry */
    psim = &soc_phy_sim[soc_phy_sims_used++];
    psim->unit = unit;
    psim->phy_id = phy_id;
    psim->pms.drv = pms_drv;

    /* Initialize and reset simulator */
    SOC_IF_ERROR_RETURN(
        phymod_sim_init(&psim->pms,
                        COUNTOF(psim->pms_entries), psim->pms_entries));
    SOC_IF_ERROR_RETURN(
        phymod_sim_reset(&psim->pms));

    return SOC_E_NONE;
}

#endif /* PHY_SIM_SUPPORT */

int
soc_physim_wrmask(int unit, uint32 phy_id,
                  uint32 phy_reg_addr, uint16 phy_wr_data, uint16 wr_mask)
{
#ifdef PHY_SIM_SUPPORT
    soc_phy_sim_t *psim = soc_physim_find(unit, phy_id);
    uint32 data32;

    if (psim == NULL) {
        return SOC_E_INIT;
    }

    data32 = wr_mask;
    data32 <<= 16;
    data32 |= phy_wr_data;

    SOC_IF_ERROR_RETURN(
        phymod_sim_write(&psim->pms, phy_reg_addr, data32));

    LOG_INFO(BSL_LS_SOC_PHYSIM,
             (BSL_META_U(unit,
                         "soc_physim_wrmask 0x%03x:0x%04x = 0x%04x/0x%04x\n"),
              phy_id, phy_reg_addr, phy_wr_data, wr_mask));
    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif
}

int
soc_physim_read(int unit, uint32 phy_id,
                uint32 phy_reg_addr, uint16 *phy_rd_data)
{
#ifdef PHY_SIM_SUPPORT
    soc_phy_sim_t *psim = soc_physim_find(unit, phy_id);
    uint32 data32;

    if (psim == NULL) {
        return SOC_E_INIT;
    }

    SOC_IF_ERROR_RETURN(
        phymod_sim_read(&psim->pms, phy_reg_addr, &data32));

    *phy_rd_data = data32;

    LOG_INFO(BSL_LS_SOC_PHYSIM,
             (BSL_META_U(unit,
                         "soc_physim_read 0x%03x:0x%04x = 0x%04x\n"),
              phy_id, phy_reg_addr, *phy_rd_data));
    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif
}

int
soc_physim_write(int unit, uint32 phy_id,
                 uint32 phy_reg_addr, uint16 phy_wr_data)
{
    return soc_physim_wrmask(unit, phy_id, phy_reg_addr, phy_wr_data, 0);
}

#ifdef PORTMOD_SUPPORT

#include <soc/portmod/portmod.h>

/* Default simulator bus */
STATIC
int sim_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t* val){
    portmod_default_user_access_t *user_data;
    uint16_t tmp=0;
    int rv;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    user_data = (portmod_default_user_access_t*)user_acc;

    (*val) = 0;
    rv = soc_physim_read(user_data->unit, core_addr, reg_addr, &tmp);
    (*val) = tmp;

    return rv;
}

STATIC
int sim_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val){
    portmod_default_user_access_t *user_data;
    uint16_t tmp = 0;
    uint16_t msk = 0;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    user_data = (portmod_default_user_access_t*)user_acc;

    tmp = (val & 0xFFFF);
    msk = ((val >> 16) & 0xFFFF);

    return soc_physim_wrmask(user_data->unit, core_addr, reg_addr, tmp, msk);
}

phymod_bus_t sim_bus = {
    "portmod_phy_sim",
    sim_bus_read,
    sim_bus_write,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0
};

void
soc_physim_enable_get(int unit, int* is_sim)
{
    int use_sim;

    use_sim = soc_property_get(unit, spn_PHY_SIMUL, 0) || soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0);

    if (use_sim || SAL_BOOT_PLISIM) {
        (*is_sim) = 1;
    } else {
        (*is_sim) = 0;
    }

    return ;
}

/*
addr_ident - in some cases the access->phy_id is the same for several PMs (which are accesses from diffrent blocks). 
In this case addr_ident is ORed with the phy_id to distinguish between the cores.
*/
int
soc_physim_check_sim(int unit, phymod_dispatch_type_t type, phymod_access_t* access, uint32 addr_ident, int* is_sim)
{
    int use_sim;
    phymod_sim_drv_t *sim = NULL;

    use_sim = soc_property_get(unit, spn_PHY_SIMUL, 0) || soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0);

    if(use_sim || SAL_BOOT_PLISIM) {
        switch(type) {
#if !defined (EXCLUDE_PHYMOD_EAGLE_SIM) && defined (PHYMOD_EAGLE_SUPPORT)
            case phymodDispatchTypeEagle:
                sim = &eagle_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_FALCON_SIM) &&  defined (PHYMOD_FALCON_SUPPORT)
            case phymodDispatchTypeFalcon:
                sim = &falcon_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_QSGMIIE_SIM) && defined (PHYMOD_QSGMIIE_SUPPORT) 
            case phymodDispatchTypeQsgmiie:
                sim = &qsgmiie_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_TSCE_SIM) && defined (PHYMOD_TSCE_SUPPORT)
            case phymodDispatchTypeTsce:
                sim = &tsce_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_TSCF_SIM) && defined (PHYMOD_TSCF_SUPPORT)
            case phymodDispatchTypeTscf:
                sim = &tscf_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_VIPER_SIM) && defined (PHYMOD_VIPER_SUPPORT)
            case phymodDispatchTypeViper:
                if (SOC_IS_GREYHOUND2(unit)) {
                    sim = &viper_sp2_sim_drv;
                }
                else {
                    sim = &viper_sim_drv;
                }
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_TSCF16_SIM) && defined (PHYMOD_TSCF16_SUPPORT)
            case phymodDispatchTypeTscf16:
                sim = &tscf16_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_TSCBH_SIM) && defined (PHYMOD_TSCBH_SUPPORT)
            case phymodDispatchTypeTscbh:
                sim = &tscbh_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_TSCE16_SIM) && defined (PHYMOD_TSCE16_SUPPORT)
            case phymodDispatchTypeTsce16:
                sim = &tsce16_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_QTCE_SIM) && defined (PHYMOD_QTCE_SUPPORT)
            case phymodDispatchTypeQtce:
                sim = &qtce_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_BLACKHAWK_SIM) && defined (PHYMOD_BLACKHAWK_SUPPORT)
            case phymodDispatchTypeBlackhawk:
                sim = &blackhawk_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_TSCF_GEN3_SIM) && defined (PHYMOD_TSCF_GEN3_SUPPORT)
            case phymodDispatchTypeTscf_gen3:
                sim = &tscf_gen3_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_TSCE_DPLL_SIM) && defined (PHYMOD_TSCE_DPLL_SUPPORT)
            case phymodDispatchTypeTsce_dpll:
                sim = &tsce_dpll_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_QTCE16_SIM) && defined (PHYMOD_QTCE16_SUPPORT)
            case phymodDispatchTypeQtce16:
                sim = &qtce16_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_TSCE7_SIM) && defined (PHYMOD_TSCE7_SUPPORT)
            case phymodDispatchTypeTsce7:
                sim = &tsce7_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_TSCF16_GEN3_SIM) && defined (PHYMOD_TSCF16_GEN3_SUPPORT)
            case phymodDispatchTypeTscf16_gen3:
                sim = &tscf16_gen3_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_TSCBH_GEN2_SIM) && defined (PHYMOD_TSCBH_GEN2_SUPPORT)
            case phymodDispatchTypeTscbh_gen2:
                sim = &tscbh_gen2_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_TSCBH_FLEXE_SIM) && defined (PHYMOD_TSCBH_FLEXE_SUPPORT)
            case phymodDispatchTypeTscbh_flexe:
                sim = &tscbh_flexe_sim_drv;
                break;
#endif
#if !defined (EXCLUDE_PHYMOD_BLACKHAWK7_V1L8P1_SIM) && defined (PHYMOD_BLACKHAWK7_V1L8P1_SUPPORT)
            case phymodDispatchTypeBlackhawk7_v1l8p1:
                sim = &blackhawk7_v1l8p1_sim_drv;
                break;
#endif
            default:
                LOG_ERROR(BSL_LS_SOC_PHYSIM,
                         (BSL_META_U(unit,
                                     "Invalid simulator %d\n"), type));
                return SOC_E_NOT_FOUND;
        }

        PHYMOD_ACC_ADDR(access) |= addr_ident;
        SOC_IF_ERROR_RETURN(soc_physim_add(unit, PHYMOD_ACC_ADDR(access), sim));

        PHYMOD_ACC_BUS(access) = &sim_bus;
        (*is_sim) = 1;

    } else {
        /* Not simulator - do nothing*/
        (*is_sim) = 0;
    }

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
soc_physim_scache_allocate(int unit)
{
    int rv = SOC_E_NONE;
    uint8 *physim_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_size = 0;
    int idx;
    int default_ver = SOC_WB_DEFAULT_VERSION;

    for (idx = 0; idx < soc_phy_sims_used; idx++) {
        alloc_size +=   sizeof(int) +   /* unit*/
               sizeof(int) +            /* phy_id */
               sizeof(int) +            /* num_entries */
               sizeof(int) +            /* entries_used */
               (sizeof(phymod_sim_entry_t) * PHY_NUM_SIM_ENTRIES); /* pms_entries */
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_PHYSIM_HANDLE, 0);

    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                          TRUE, &alloc_size,
                                          &physim_scache_ptr,
                                          default_ver,
                                          NULL);
    /* Proceed with Level 1 Warm Boot */
    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        return rv;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}

int
soc_physim_scache_sync(int unit)
{
    uint8 *physim_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_size = 0;
    int rv = 0;
    int idx, idx_pms;
    soc_phy_sim_t *psim;
    phymod_sim_entry_t *physim_entry_scache_p;
    uint32 *u32_scache_p;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_PHYSIM_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                        FALSE, &alloc_size,
                        &physim_scache_ptr,
                        SOC_WB_DEFAULT_VERSION,
                        NULL);
    /* Proceed with Level 1 Warm Boot */
    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        return rv;
    }

    u32_scache_p = (uint32 *)physim_scache_ptr;
    for (idx = 0; idx < soc_phy_sims_used; idx++) {
        psim = &soc_phy_sim[idx];

        *u32_scache_p = psim->unit;     /* unit*/
        u32_scache_p++;
        *u32_scache_p = psim->phy_id;   /* phy_id */
        u32_scache_p++;
        *u32_scache_p = psim->pms.data.num_entries; /* num_entries */
        u32_scache_p++;
        *u32_scache_p = psim->pms.data.entries_used;/* entries_used */
        u32_scache_p++;

        /* pms_entries */
        physim_entry_scache_p = (phymod_sim_entry_t *)u32_scache_p;
        for (idx_pms = 0; idx_pms < psim->pms.data.entries_used; idx_pms++) {
            physim_entry_scache_p->flags = psim->pms_entries[idx_pms].flags;
            physim_entry_scache_p->addr = psim->pms_entries[idx_pms].addr;
            physim_entry_scache_p->data = psim->pms_entries[idx_pms].data;
            physim_entry_scache_p++;
        }
        u32_scache_p = (uint32 *)physim_entry_scache_p;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}

int
soc_physim_scache_recovery(int unit)
{
    uint8 *physim_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_size = 0;
    int rv = 0;
    int idx, idx_pms;
    soc_phy_sim_t *psim;
    phymod_sim_entry_t *physim_entry_scache_p;
    uint32 *u32_scache_p;
    uint16  recovered_ver = 0;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_PHYSIM_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                        FALSE, &alloc_size,
                        &physim_scache_ptr,
                        SOC_WB_DEFAULT_VERSION,
                        &recovered_ver);
    /* Proceed with Level 1 Warm Boot */
    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        return rv;
    }

    if (recovered_ver >= SOC_WB_VERSION_1_0) {
        u32_scache_p = (uint32 *)physim_scache_ptr;
        for (idx = 0; idx < soc_phy_sims_used; idx++) {
            psim = &soc_phy_sim[idx];

            psim->unit = *u32_scache_p;     /* unit*/
            u32_scache_p++;
            psim->phy_id = *u32_scache_p;   /* phy_id */
            u32_scache_p++;
            psim->pms.data.num_entries = *u32_scache_p; /* num_entries */
            u32_scache_p++;
            psim->pms.data.entries_used = *u32_scache_p;/* entries_used */
            u32_scache_p++;

            /* pms_entries */
            physim_entry_scache_p = (phymod_sim_entry_t *)u32_scache_p;
            for (idx_pms = 0; idx_pms < psim->pms.data.entries_used; idx_pms++) {
                psim->pms_entries[idx_pms].flags = physim_entry_scache_p->flags;
                psim->pms_entries[idx_pms].addr = physim_entry_scache_p->addr;
                psim->pms_entries[idx_pms].data = physim_entry_scache_p->data;
                physim_entry_scache_p++;
            }
            u32_scache_p = (uint32 *)physim_entry_scache_p;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* PHY_SIM_SUPPORT */

