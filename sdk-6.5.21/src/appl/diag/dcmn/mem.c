/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       mem.c
 * Purpose:    Diag shell memory commands for DPP
 */

#include <shared/bsl.h>
#include <ibde.h>

#include <sal/appl/pci.h>

#include <soc/mem.h>
#include <soc/cm.h>



#include <soc/dcmn/dcmn_mem.h>

#if defined (BCM_DFE_SUPPORT)
#include <soc/dfe/cmn/dfe_drv.h>
#endif

#include <soc/dcmn/dcmn_intr_corr_act_func.h>

#include <appl/diag/system.h>
#include <appl/diag/bslcons.h>
#include <appl/diag/bslfile.h>

#define DUMP_TABLE_RAW          0x01
#define DUMP_TABLE_HEX          0x02
#define DUMP_TABLE_ALL          0x04
#define DUMP_TABLE_CHANGED      0x08
#define DUMP_DISABLE_CACHE      0x10
#define ARAD_INDIRECT_SIXE      512

extern int dpp_all_reg_get(int unit, int is_debug);
extern  int     mem_test_default_init(int, soc_mem_t mem, void **);
extern  int     mem_test_rw_init(int, soc_mem_t mem, void **);
extern  int     mem_test(int, args_t *, void *);
extern  int     mem_test_done(int, void *);

void memtest_mask_get(int unit, soc_mem_t mem, uint32 *mask);
void soc_mem_entry_dump_field(int unit, soc_mem_t mem, void *buf, char* field_name);


/*
 * TYPE DEFS
 */

typedef struct test_run_memory_stat_s {

    int mem_valid_num;
    int mem_test_num;
    int mem_pass_num;
    int mem_fail_num;

} test_run_memory_stat_t;

test_run_memory_stat_t test_run_memory_stat;

/*
 * Utility routine to concatenate the first argument ("first"), with
 * the remaining arguments, with commas separating them.
 */
#define MAX_MEM_SIZE   100000

/*
 * Function:
 *    soc_mem_entry_dump
 * Purpose:
 *    Debug routine to dump a formatted table entry.
 *
 *    Note:  Prefix != NULL : Dump chg command
 *           Prefix == NULL : Dump     command
 *             (Actually should pass dump_chg flag but keeping for simplicity)
 */
void
soc_mem_entry_dump_field(int unit, soc_mem_t mem, void *buf, char* field_name)
{
    soc_field_info_t *fieldp;
    soc_mem_info_t *memp;
    int f;
    int field_found=0;
#if !defined(SOC_NO_NAMES)
    uint32 fval[SOC_MAX_MEM_FIELD_WORDS];
    char tmp[(SOC_MAX_MEM_FIELD_WORDS * 8) + 3];
#endif
             /* Max nybbles + "0x" + null terminator */

    memp = &SOC_MEM_INFO(unit, mem);

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Memory not valid for unit\n")));
    } else {
    for (f = memp->nFields - 1; f >= 0; f--) {
        fieldp = &memp->fields[f];
        if (sal_strcasecmp(SOC_FIELD_NAME(unit, fieldp->field),field_name)) {
            continue;
        } else if (field_found++ == 0) {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"<");
        }

#if !defined(SOC_NO_NAMES)
           {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%s=", soc_fieldnames[fieldp->field]);
                sal_memset(fval, 0, sizeof (fval));
                soc_mem_field_get(unit, mem, buf, fieldp->field, fval);
                _shr_format_long_integer(tmp, fval, BITS2BYTES(fieldp->len));
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%s%s", tmp, f > 0 ? "," : "");
            }

#endif
         }
    }

    if (field_found) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,">");
    } else {
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "field: \"%s\" didn't found\n"), field_name));
    }

}

void memtest_mask_get(int unit, soc_mem_t mem, uint32 *mask)
{
    uint32		*tcammask = NULL;
    uint32		*eccmask = NULL;
    uint32              accum_tcammask;
    int			dw, i;

    if ((tcammask = sal_alloc(CMIC_SCHAN_WORDS(unit)*sizeof(uint32), "tcammask")) == NULL) {
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "dpp_mem_test_flipflop_iter_read_compare_callback: tcammask : Out of memory\n")));
        goto done;
    }
    if ((eccmask = sal_alloc(CMIC_SCHAN_WORDS(unit)*sizeof(uint32), "eccmask")) == NULL) {
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "dpp_mem_test_flipflop_iter_read_compare_callback: eccmask : Out of memory\n")));
        goto done;
    }

    for (i=0;i<CMIC_SCHAN_WORDS(unit);i++) {
        mask[i] = 0;
        tcammask[i] = 0;
        eccmask[i] = 0;
    }
    dw = soc_mem_entry_words(unit, mem);
    if(SOC_IS_DPP(unit) || SOC_IS_DFE(unit)) {
        soc_mem_datamask_rw_get(unit, mem, mask);
    } else
    {
        soc_mem_datamask_get(unit, mem, mask);
    }
    soc_mem_tcammask_get(unit, mem, tcammask);
    soc_mem_eccmask_get(unit, mem, eccmask);
    accum_tcammask = 0;
    for (i = 0; i < dw; i++) {
        accum_tcammask |= tcammask[i];
    }

    for (i = 0; i < dw; i++) {
      mask[i] &= ~eccmask[i];
    }
done:
    if (tcammask != NULL) {
        sal_free(tcammask);
    }
    if (eccmask != NULL) {
        sal_free(eccmask);
    }

}




