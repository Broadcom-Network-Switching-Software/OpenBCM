/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Built-in Self Test for ARL, L3, and CBP memories
 */


#include <shared/bsl.h>

#include <sal/types.h>
#include <soc/mem.h>

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/shell.h>
#include "testlist.h"
#if defined (BCM_DNXF_SUPPORT) || defined (BCM_DNX_SUPPORT)
#include <soc/cm.h>
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/ramon/ramon_drv.h>
#endif
#if defined (BCM_DNX_SUPPORT)
#include <soc/dnx/drv.h>
#endif
#endif
#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/cm.h>
#endif
#ifdef BCM_PETRA_SUPPORT
    #include <soc/dpp/ARAD/arad_init.h>
    #include <bcm/init.h>
    #include <soc/dpp/drv.h>
    #include <appl/diag/dcmn/init_deinit.h>
#endif

#if defined (BCM_ESW_SUPPORT) ||  defined (BCM_DFE_SUPPORT) || defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
typedef struct bist_testdata_s {
    soc_mem_t    mems[NUM_SOC_MEM];
    int     num_mems;
} bist_testdata_t;

#ifdef BCM_PETRA_SUPPORT
STATIC appl_dcmn_init_param_t bist_init_deinit[SOC_MAX_NUM_DEVICES];
#endif

#ifdef BCM_DNX_SUPPORT
uint32 any_bist_performed[(SOC_MAX_NUM_DEVICES+31)/32] = {0};
#endif

#if defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
uint32 test_was_run_and_suceeded[(SOC_MAX_NUM_DEVICES+31)/32] = {0};
#endif

int
bist_test_init(int u, args_t *a, void **p)
{

    bist_testdata_t    *bd = 0;

    if (SOC_IS_DFE(u) || SOC_IS_DNXF(u) || SOC_IS_DNX(u)){
        char *arg0 = ARG_CUR(a);
        if (arg0 != NULL && sal_strcasecmp(arg0, "all") == 0) {
            ARG_NEXT(a); /* ignore the default argument for this TR */
        }
#ifdef BCM_DFE_SUPPORT
        if(SOC_IS_DFE(u))
        {
            return init_deinit_test_init(u, a, p);
        }
#endif
#if defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
        if(SOC_IS_DNXF(u) || SOC_IS_DNX(u))
        {
            test_was_run_and_suceeded[u/32] &= ~(((uint32)1) << (u % 32));
        if (SOC_IS_DNXF(u))
        {
            char command[256];
            sal_sprintf(command,"config add %s.%d=%d",spn_BIST_ENABLE,u,2);
            sh_process_command(u,command);
        }
            return test_dnxc_init_test_init(u, a, p);
        }
#endif
    }
#ifdef BCM_PETRA_SUPPORT
    else if (SOC_IS_ARAD(u)){
        appl_dcmn_init_param_t *init_param;
        char *arg0 = ARG_CUR(a);

        if (arg0 != NULL && sal_strcasecmp(arg0, "all") == 0) {
            ARG_NEXT(a); /* ignore the default argument for this TR */
        }

        init_param = &bist_init_deinit[u];
        sal_memset(init_param, 0x0, sizeof(appl_dcmn_init_param_t));
        init_param->no_attach=1;
        *p = init_param;
        /* set default modid & base_modid with non-valid values, which denotes */
        /* that previous values should be taken if those params were not supplied */
        init_param->modid = -1;
        init_param->base_modid = -1;

        return appl_dcmn_init_usage_parse(u, a, init_param);
        /*return init_deinit_test_init(u, a, init_param);*/
    }
#endif

    if ((bd = sal_alloc(sizeof (*bd), "bist-testdata")) == 0) {
    goto fail;
    }

#if defined (BCM_ESW_SUPPORT)
    if (bist_args_to_mems(u, a, bd->mems, &bd->num_mems) < 0) {
    goto fail;
    }
#endif
    if ((soc_reset_init(u)) < 0) {
        goto fail;
    }

    if (soc_mem_debug_set(u, 0) < 0) {
    goto fail;
    }

    *p = (void *) bd;

    return 0;

 fail:
    if (bd) {
    sal_free(bd);
    }

    return -1;
}

int
bist_test(int u, args_t *a, void *p)
{
#if defined (BCM_ESW_SUPPORT)
    bist_testdata_t    *bd = p;
#endif  
    int        rv = 0;
    COMPILER_REFERENCE(a);  
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(u))
    {
        any_bist_performed[u/32] |= ((uint32)1) << (u % 32);
    }
#endif
#if defined (BCM_DNXF_SUPPORT) || defined (BCM_DNX_SUPPORT)
    if(SOC_IS_DNXF(u) || SOC_IS_DNX(u))
    {
        if (((rv = test_dnxc_init_test(u, a, p))) && !(test_was_run_and_suceeded[u / 32] & (((uint32) 1) << (u % 32)))) { /* perform deinit and init */
            test_error(u, "BIST or deinint/init failed: %s\n", soc_errmsg(rv));
        }
    } else
#endif
#if defined (BCM_DFE_SUPPORT)
    if (SOC_IS_DFE(u)) {
        char command[256];
        uint16 dev_id;
        uint8 rev_id;

        soc_cm_get_id (u, &dev_id, &rev_id);
        sal_sprintf(command,"config delete %s.BCM8%x",spn_BIST_ENABLE,dev_id);
        sh_process_command(u,command);
        sal_sprintf(command,"config add %s.BCM8%x=%d",spn_BIST_ENABLE,dev_id,2);
        sh_process_command(u,command);
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
        if ((rv = init_deinit_test(u, a, p))) { /* perform deinit and init */
            test_error(u, "BIST or deinint/init failed: %s\n", soc_errmsg(rv));
        }
#endif
       /* rv = soc_dfe_drv_mbist(u, 0); */
    } else
#endif     
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(u)) {
        SOC_DPP_CONFIG(u)->tm.various_bm |= DPP_VARIOUS_BM_FORCE_MBIST_TEST; /* mark MBIST to be performed by the init */
        if ((rv = init_deinit_test(u, a, p))) { /* perform deinit and init */
            test_error(u, "BIST or deinint/init failed: %s\n", soc_errmsg(rv));
        }
        SOC_DPP_CONFIG(u)->tm.various_bm &= ~(uint8)DPP_VARIOUS_BM_FORCE_MBIST_TEST;
        return rv;
    } else
#endif     
#if defined (BCM_ESW_SUPPORT)
    {
        rv = soc_bist(u,
               bd->mems, bd->num_mems,
               SOC_CONTROL(u)->bistTimeout);
    }
#else
    {
        cli_out("device not supported\n");
    }
#endif
    if (rv < 0)
    {
#if defined (BCM_DNXF_SUPPORT) || defined (BCM_DNX_SUPPORT)
        if (!(test_was_run_and_suceeded[u / 32] & (((uint32) 1) << (u % 32)))) {
#endif
            test_error(u, "BIST failed: %s\n", soc_errmsg(rv));
            return -1;
#if defined (BCM_DNXF_SUPPORT) || defined (BCM_DNX_SUPPORT)
        }
#endif
    }
    return 0;
}

int
bist_test_done(int u, void *p)
{
    int        rv;
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(u))
    {
        return init_deinit_test_done(u, p);
    } else
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(u))
    {
        any_bist_performed[u/32] &= ~(((uint32)1) << (u % 32));
    }
#endif
#if defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
    if(SOC_IS_DNXF(u) || SOC_IS_DNX(u)){
        if (SOC_IS_DNXF(u))
        {
            char command[256];
            sal_sprintf(command,"config delete %s.%d",spn_BIST_ENABLE,u);
            sh_process_command(u,command);
        }
        return test_dnxc_init_test_done(u, p);
    } else 
#endif
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
    if(SOC_IS_DFE(u)){
        return init_deinit_test_done(u, p);
    } else 
#endif
    {
        rv = soc_reset_init(u) ;
    }
    sal_free(p);

    if (rv < 0)
    {
#if defined (BCM_DNXF_SUPPORT) || defined (BCM_DNX_SUPPORT)
        if (!(test_was_run_and_suceeded[u / 32] & (((uint32) 1) << (u % 32)))) {
#endif
            test_error(u, "Post-BIST reset failed: %s\n", soc_errmsg(rv));
            return -1;
#if defined (BCM_DNXF_SUPPORT) || defined (BCM_DNX_SUPPORT)
        }
#endif
    }
    return 0;
}
#endif /* BCM_ESW_SUPPORT */
