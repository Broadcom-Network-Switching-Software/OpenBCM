/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(INCLUDE_LIB_CINT)

#if defined(PHYMOD_SUPPORT)

#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <soc/types.h>
#include <soc/error.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/phymod_ctrl.h>

int
port_cores_access_get(int unit, soc_port_t port, uint32 array_size, phymod_core_access_t *cores, uint32 *nof_cores){
    phy_ctrl_t                *pc; 
    soc_phymod_ctrl_t         *pmc;
    int i;
    
    pc = INT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;
    
    *nof_cores = 0;
    if(pmc->num_phys > array_size){
        return SOC_E_PARAM;
    }
    for(i = 0 ; i < pmc->num_phys; i++){
        if( pmc->phy[i] == NULL ){
            return SOC_E_INTERNAL;
        }
        if (pmc->phy[i]->core == NULL) {
            return SOC_E_INTERNAL;
        }
        cores[i] = pmc->phy[i]->core->pm_core;
        *nof_cores += 1;
    }
    return SOC_E_NONE;
}

int
port_phys_access_get(int unit, soc_port_t port, uint32 array_size, phymod_phy_access_t *phys, uint32 *nof_cores){
    phy_ctrl_t                *pc; 
    soc_phymod_ctrl_t         *pmc;
    int i;
    
    pc = INT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;
    
    *nof_cores = 0;
    if(pmc->num_phys > array_size){
        return SOC_E_PARAM; 
    }
    for(i = 0 ; i < pmc->num_phys; i++){
        if( pmc->phy[i] == NULL ){
            return SOC_E_INTERNAL;
        }
        phys[i] = pmc->phy[i]->pm_phy;
        *nof_cores += 1;
    }
    return SOC_E_NONE;
}   


CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         port_cores_access_get,
                         int, int, unit, 0, 0,
                         soc_port_t, soc_port_t, port, 0, 0,
                         uint32, uint32, array_size, 0, 0,
                         phymod_core_access_t*, phymod_core_access_t, cores, 1, 0,
                         uint32*, uint32, nof_cores, 1, 0
                         );

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         port_phys_access_get,
                         int, int, unit, 0, 0,
                         soc_port_t, soc_port_t, port, 0, 0,
                         uint32, uint32, array_size, 0, 0,
                         phymod_phy_access_t*, phymod_phy_access_t, phys, 1, 0,
                         uint32*, uint32, nof_cores, 1, 0
                         );


static cint_function_t __cint_functions[] =
{
    CINT_FWRAPPER_ENTRY(port_cores_access_get),
    CINT_FWRAPPER_ENTRY(port_phys_access_get),
    { NULL }
};


static cint_parameter_desc_t __cint_typedefs[] = 
{
    {
        "uint32",
        "uint32_t",
        0,
        0
    },
    { NULL }
};



cint_data_t phymod_access_cint_data =
{    
    NULL,
    __cint_functions,
    NULL,
    NULL,
    __cint_typedefs,
    NULL,
    NULL,
};

#endif /* defined(PHYMOD_SUPPORT) */

#endif /* defined(INCLUDE_LIB_CINT) */

typedef int _phymod_access_cint_data_not_empty; /* Make ISO compilers happy. */
