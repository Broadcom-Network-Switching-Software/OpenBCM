/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MULTICAST

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/multicast.h>
#include <soc/dcmn/error.h>





STATIC dpp_mcds_t *_dpp_mc_swds[SOC_SAND_MAX_DEVICE] = {0};


dpp_mcds_t *dpp_get_mcds(
    SOC_SAND_IN  int unit
  )
{
  return _dpp_mc_swds[unit];
}


uint32 dpp_alloc_mcds(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  unsigned   size_of_mcds, 
    SOC_SAND_OUT dpp_mcds_t **mcds_out    
)
{
    int need_cleaning = 0;
    dpp_mcds_t** mcds_p = _dpp_mc_swds + unit;
    SOCDNX_INIT_FUNC_DEFS;
    if (*mcds_p) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("multicast data structure already allocated")));
    }
    if (!(*mcds_p = sal_alloc(size_of_mcds, "multicast data structure"))) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("could not allocate multicast data structure")));
    }
    need_cleaning = 1;
    SOCDNX_IF_ERR_EXIT(soc_sand_os_memset(*mcds_p, 0, size_of_mcds));
    need_cleaning = 0;
    if (mcds_out) {
        *mcds_out = *mcds_p;
    }

exit:
    if (need_cleaning) {
        sal_free(*mcds_p);
        *mcds_p = NULL;
    }
    SOCDNX_FUNC_RETURN;
}


uint32 dpp_dealloc_mcds(
    SOC_SAND_IN  int        unit
)
{
    dpp_mcds_t** mcds_p = _dpp_mc_swds + unit;
    SOCDNX_INIT_FUNC_DEFS;
    if (*mcds_p == NULL) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("multicast data structure not allocated")));
    }
    sal_free(*mcds_p);
    *mcds_p = NULL;

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC uint32 nof_mc_asserts = 0;   
STATIC int8 mc_asserts_enabled = 0; 
#if defined(_ARAD_SWDB_TEST_ASSERTS_REAL_ASSERT) && !defined(__KERNEL__)
extern char *getenv(const char*);
STATIC int8 mc_asserts_real = getenv("GDB") ? 1 : 0; 
#else
STATIC int8 mc_asserts_real = 0;
#endif

uint32 dpp_mcds_get_nof_asserts(void) {
    return nof_mc_asserts;
}
uint8 dpp_mcds_get_mc_asserts_enabled(void) {
    return mc_asserts_enabled;
}
void dpp_mcds_set_mc_asserts_enabled(uint8 enabled) {
    mc_asserts_enabled = enabled;
}
uint8 dpp_mcds_get_mc_asserts_real(void) {
    return mc_asserts_real;
}
void dpp_mcds_set_mc_asserts_real(uint8 real) {
    mc_asserts_real = real;
}

void dpp_perform_mc_assert(const char *file_name, unsigned line_number) { 
    ++nof_mc_asserts;
    if (mc_asserts_enabled) {
        LOG_ERROR(BSL_LS_SOC_MULTICAST, (BSL_META("MCDS ASSERTED at %s:%u\n"), file_name, line_number));
#if !defined(__KERNEL__)
        if (mc_asserts_real) {
            assert(0);
        }
    #endif
    }
}

