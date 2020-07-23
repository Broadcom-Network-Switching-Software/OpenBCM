/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Benchmark tests
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <stdarg.h>

#include <sal/types.h>
#include <soc/mem.h>
#include <soc/cm.h>

#include <sal/appl/pci.h>
#include <sal/appl/sal.h>
#include <sal/core/boot.h>
#include <sal/core/time.h>

#include <soc/l2x.h>
#include <soc/l3x.h>
#include <soc/phyctrl.h>
#include <soc/phy.h>
#include <soc/error.h>

#include <bcm/error.h>
#include <bcm/link.h>

#if defined(BCM_ESW_SUPPORT)
#include <bcm_int/esw/mbcm.h>
#endif

#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/test.h>
#include <appl/diag/progress.h>

#include "testlist.h"
#ifdef BCM_DNX_SUPPORT
#include <shared/shrextend/shrextend_debug.h>
#include <soc/sand/sand_mem.h>
#endif

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_TESTS


/* Decimal point formatting */
#define INT_FRAC_2PT(x) (x) / 100, (x) % 100
#define TEST_MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct
{
    int blk_id;
    soc_mem_t mem;
    int array_index;
    int entry_index;
    int nof_entries;
} mem_indirect_cmd_t;

typedef struct benchmark_s {
    uint32	testMask;
    int		testNum;
    int		sizeMem;
    char	*bufMem;
    int		sizeDMA;
    char	*bufDMA;
    char	*testName;
    sal_usecs_t	stime, etime;
    uint32	dataSize;
    char	*dataUnit;
} benchmark_t;

#define BENCH_BUFMEM_SIZE	0x100000
#define BENCH_BUFDMA_SIZE	0x200000

int
benchmark_done(int u, void *pa)
{
    benchmark_t		*b = (benchmark_t *) pa;

    if (b) {
	if (b->bufMem)
	    sal_free(b->bufMem);
	if (b->bufDMA)
	    soc_cm_sfree(u, b->bufDMA);

	sal_free(b);
    }

    return 0;
}

int
benchmark_init(int u, args_t *a, void **pa)
{
    benchmark_t		*b = 0;
    int			rv = -1;
    parse_table_t       pt;

    *pa = NULL;

    if ((b = sal_alloc(sizeof (benchmark_t), "benchmark")) == 0)
	goto done;

    memset(b, 0, sizeof (*b));

    *pa = (void *) b;

    b->testMask = ~0;
    b->testNum = 0;

    b->sizeMem = BENCH_BUFMEM_SIZE;

    if ((b->bufMem = sal_alloc(b->sizeMem, "benchmark")) == 0) {
	cli_out("Not enough host memory\n");
	goto done;
    }

    b->sizeDMA = BENCH_BUFDMA_SIZE;

    if ((b->bufDMA = soc_cm_salloc(u, b->sizeDMA, "benchmark")) == 0) {
	cli_out("Not enough DMA memory\n");
	goto done;
    }

    parse_table_init(u, &pt);
    parse_table_add(&pt, "TestMask", PQ_HEX|PQ_DFL, 0,
                    &b->testMask, NULL);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
        test_error(u,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        parse_arg_eq_done(&pt);
	goto done;
    }

    parse_arg_eq_done(&pt);

    rv = 0;

 done:
    if (rv < 0)
	benchmark_done(u, *pa);

    return rv;
}

static int
benchmark_begin(benchmark_t *b,
		char *testName,
		char *dataUnit,
		uint32 dataSize)
{
    assert(b->testName == 0);

    if ((b->testMask & (1U << b->testNum)) == 0) {
	return 0;
    }

    b->testName = testName;
    b->dataSize = dataSize;
    b->dataUnit = dataUnit;
    b->stime = sal_time_usecs();

    return 1;
}

static void
benchmark_end(benchmark_t *b)
{
    if (b->testName) {
	int		per_sec, usec_per;
        sal_usecs_t     td;
	char            plus = ' ', lessthan = ' ';

        b->etime = sal_time_usecs();
        td = SAL_USECS_SUB(b->etime, b->stime);
	if (td == 0) {
	    td = 1; /* Non-zero value */
	    plus = '+';
	    lessthan = '<';
	}

        per_sec = _shr_div_exp10(b->dataSize, td, 8);
        usec_per = b->dataSize ? _shr_div_exp10(td, b->dataSize, 2) : 0;

	cli_out("%2d) %-28s%7d.%02d%c %5s/sec  ; %c%7d.%02d usec/%s\n",
                b->testNum,
                b->testName,
                INT_FRAC_2PT(per_sec),
                plus,
                b->dataUnit,
                lessthan,
                INT_FRAC_2PT(usec_per),
                b->dataUnit);

	b->testName = 0;
    }

    b->testNum++;
}

#if defined BCM_TRIUMPH_SUPPORT || defined BCM_SCORPION_SUPPORT
static int
_gen_l2x_insert(int unit, l2x_entry_t *entry)
{
    return soc_mem_insert(unit, L2Xm, MEM_BLOCK_ANY, (void *)entry);
}

static int
_gen_l2x_delete(int unit, l2x_entry_t *entry)
{
    return soc_mem_delete(unit, L2Xm, MEM_BLOCK_ANY, (void *)entry);
}

static int
_gen_l2x_lookup(int unit, l2x_entry_t *key,
                l2x_entry_t *result, int *index_ptr)
{
    return soc_mem_search(unit, L2Xm, MEM_BLOCK_ANY, index_ptr, 
                          (void *)key, (void *)result, 0);
}
#endif /* BCM_TRIUMPH_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
static int
_ism_l2_mem_insert(int unit, soc_mem_t mem, void *entry)
{
    return soc_mem_insert(unit, mem, MEM_BLOCK_ANY, (void *)entry);
}

static int
_ism_l2_mem_delete(int unit, soc_mem_t mem, void *entry)
{
    return soc_mem_delete(unit, mem, MEM_BLOCK_ANY, (void *)entry);
}

static int
_ism_l2_mem_lookup(int unit, soc_mem_t mem, void *key,
                   void *result, int *index_ptr)
{
    return soc_mem_search(unit, mem, MEM_BLOCK_ANY, index_ptr, 
                          (void *)key, (void *)result, 0);
}
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_DNX_SUPPORT
/**
* \brief - reset memory using indirect mode
*
* \param [in] unit - unit #
* \param [in] mem - memory
* \return
*   See shr_error_e
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
mem_indirect_reset(
    int unit,
    soc_mem_t mem)
{
    mem_indirect_cmd_t *cmds[SOC_MAX_NUM_BLKS];
    int nof_cmds[SOC_MAX_NUM_BLKS];
    int blk_index_min, blk_index_max;
    int index_min, index_max;
    int array_index_min, array_index_max;
    int blk_index, array_index;
    int cmd_id;
    int index;
    uint32 mem_addr;
    uint8 access_type = 0;
    uint32 entry[SOC_MAX_MEM_WORDS] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /** init commands data structure */
    sal_memset(cmds, 0, sizeof(cmds));
    sal_memset(nof_cmds, 0, sizeof(nof_cmds));
    for (blk_index = 0; blk_index < SOC_MAX_NUM_BLKS; blk_index++) {
        cmds[blk_index] = sal_alloc(200 * sizeof(mem_indirect_cmd_t), "cmds");
        sal_memset(cmds[blk_index], 0, 200 * sizeof(mem_indirect_cmd_t));
    }

    /** get info about table */
    blk_index_min = SOC_MEM_BLOCK_MIN(unit, mem);
    blk_index_max = SOC_MEM_BLOCK_MAX(unit, mem);
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    if (SOC_MEM_IS_ARRAY(unit, mem)) {
        array_index_max = (SOC_MEM_NUMELS(unit, mem) - 1) + SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
        array_index_min = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
    } else {
        array_index_min = 0;
        array_index_max = 0;
    }

    /** add memory commands to database */
    for (blk_index = blk_index_min; blk_index <= blk_index_max; blk_index++) {
        for (array_index = array_index_min; array_index <= array_index_max; array_index++) {
            for (index = index_min; index <= index_max; index += 16 * 1024 - 1) {
                cmd_id = nof_cmds[blk_index];
                cmds[blk_index][cmd_id].mem = mem;
                cmds[blk_index][cmd_id].blk_id = blk_index;
                cmds[blk_index][cmd_id].array_index = array_index;
                cmds[blk_index][cmd_id].entry_index = index;
                cmds[blk_index][cmd_id].nof_entries = TEST_MIN(index_max - index + 1, 16 * 1024 - 1);

                nof_cmds[blk_index]++;
            }
        }
    }

    /** perform all commands  */
    for (cmd_id = 0; cmd_id < 200; cmd_id++) {
        for (blk_index = 0; blk_index <= blk_index_max; blk_index++) {
            if (nof_cmds[blk_index] > cmd_id) {
                mem_addr = soc_mem_addr_get(unit, cmds[blk_index][cmd_id].mem, cmds[blk_index][cmd_id].array_index, cmds[blk_index][cmd_id].blk_id, cmds[blk_index][cmd_id].entry_index, &access_type);
                SHR_IF_ERR_EXIT(sand_mem_indirect_reset_write(unit, cmds[blk_index][cmd_id].mem, cmds[blk_index][cmd_id].blk_id, mem_addr, cmds[blk_index][cmd_id].nof_entries, entry));
            }
        }
    }

    /** verify all operations done */
    for (blk_index = 0; blk_index < SOC_MAX_NUM_BLKS; blk_index++) {
        if (nof_cmds[blk_index] > 0) {
            SHR_IF_ERR_EXIT(sand_mem_indirect_poll_done(unit, cmds[blk_index][0].blk_id, 10000));
        }
    }

exit:
    for (blk_index = 0; blk_index < SOC_MAX_NUM_BLKS; blk_index++) {
        sal_free(cmds[blk_index]);
    }
    SHR_FUNC_EXIT;
}




/**
* \brief - reset memory using dma mode
*
* \param [in] unit - unit #
* \param [in] mem - memory
* \return
*   void
* \remark
*   * None
* \see
*   * None
*/
static int
mem_dma_reset(
    int unit,
    soc_mem_t mem)
{
    uint32 entry[SOC_MAX_MEM_WORDS] = { 0 };
    int array_min, array_max;
    int array_index;
    int index_min = soc_mem_index_min(unit, mem);
    int index_max = soc_mem_index_max(unit, mem);
    int blk;

    /** get info about table */
    if (SOC_MEM_IS_ARRAY(unit, mem)) {
        array_max = (SOC_MEM_NUMELS(unit, mem) - 1) + SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
        array_min = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
    } else {
        array_min = 0;
        array_max = 0;
    }

    SOC_MEM_BLOCK_ITER(unit, mem, blk) {
        for (array_index = array_min; array_index <= array_max; array_index++) {
            SOC_IF_ERROR_RETURN(soc_mem_array_write_range(unit, 0, mem, array_index, blk, index_min, index_max, entry));
        }
    }
    return 0;
}
#endif

int
benchmark_test(int unit, args_t *a, void *pa)
{
    benchmark_t		*b = (benchmark_t *) pa;
    int                 rv = 0;
    uint16		phy_data;
    int			iter_mem, iter_cmic, iter_miim;
    int			quickturn, i, test_p;

#if defined(BCM_ESW_SUPPORT) || defined(BCM_DNX_SUPPORT)
    int         iter_dma, iter_pci;
    uint32		reg = 0;
    uint32		entry[SOC_MAX_MEM_WORDS];
    soc_reg_t   test_register;
    int         blk = 0;
#endif

#if defined(BCM_XGS_SWITCH_SUPPORT) 
    sal_mac_addr_t	mac = { 0x00, 0x01, 0x10, 0x55, 0x66, 0x77 };
    sal_mac_addr_t	maclkup = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
#endif /* BCM_XGS_SWITCH_SUPPORT */

#if defined(BCM_ESW_SUPPORT)
    int			port = 0, phy_port = 0;
#endif /* BCM_ESW_SUPPORT */


    quickturn = (SAL_BOOT_QUICKTURN != 0);

    if (quickturn) {
    	iter_mem = 100;
    	iter_cmic = 200;
    	iter_miim = 200;
#if defined(BCM_ESW_SUPPORT) || defined(BCM_DNX_SUPPORT)
    	iter_dma = 1;
    	iter_pci = 2000;
#endif
    } else if (NULL != SOC_CONTROL(unit)->soc_rcpu_schan_op){
        iter_mem = 10;
        iter_cmic = 5000; 
        iter_miim = 0;
#if defined(BCM_ESW_SUPPORT) || defined(BCM_DNX_SUPPORT)
        iter_dma = 0;
        iter_pci = 0; 
#endif
    } else {
    	iter_mem = 10;
    	iter_cmic = 5000;
    	iter_miim = 5000;
#if defined(BCM_ESW_SUPPORT) || defined(BCM_DNX_SUPPORT)
    	iter_dma = 100;
    	iter_pci = 50000;
#endif
    }

    SOC_MEM_TEST_SKIP_CACHE_SET(unit, TRUE);

    if (benchmark_begin(b, "memset sys mem", "MB",
			iter_mem * (b->sizeMem / 0x100000))) {
	for (i = 0; i < iter_mem; i++) {
	    memset(b->bufMem, 0, b->sizeMem);
        }
    }
    benchmark_end(b);

    if (benchmark_begin(b, "memcpy sys mem", "MB",
            iter_mem / 2 * (b->sizeMem / 0x100000))) {
        for (i = 0; i < iter_mem; i++) {
            memcpy(b->bufMem + b->sizeMem / 2, b->bufMem, b->sizeMem / 2);
        }
    }
    benchmark_end(b);


    if (benchmark_begin(b, "memset dma mem", "MB",
			iter_mem * (b->sizeDMA / 0x100000))) {
	for (i = 0; i < iter_mem; i++) {
	    memset(b->bufDMA, 0, b->sizeDMA);
        }
    }
    benchmark_end(b);

    if (benchmark_begin(b, "memcpy dma mem", "MB",
            iter_mem  / 2 * (b->sizeDMA / 0x100000 ))) {
        for (i = 0; i < iter_mem; i++) {
            memcpy(b->bufDMA + b->sizeDMA / 2, b->bufDMA, b->sizeDMA / 2);
        }
    }
    benchmark_end(b);

#if defined(BCM_ESW_SUPPORT) || defined(BCM_DNX_SUPPORT)
    if (SOC_IS_ESW(unit) || SOC_IS_DNX(unit)) {
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            test_register = SOC_REG_IS_VALID(unit, CMIC_TOP_SBUS_TIMEOUTr);
            test_register = SOC_REG_IS_VALID(unit, CMIC_TOP_SBUS_TIMEOUTr) ? \
                CMIC_TOP_SBUS_TIMEOUTr : 0;
        } else
#endif
        {
            test_register = SOC_REG_IS_VALID(unit, CMIC_DEV_REV_IDr) ? \
                CMIC_DEV_REV_IDr : CMIC_I2C_DATAr;
        }
        if (test_register) {
            if (benchmark_begin(b, "read pci reg", "read", iter_pci)) {
                for (i = 0; i < iter_pci; i++) {
                    soc_pci_getreg(unit, soc_reg_addr(unit, test_register,
                                                  REG_PORT_ANY, 0), &reg);
                }
            }

            benchmark_end(b);

            if (benchmark_begin(b, "write pci reg", "write", iter_pci)) {
                for (i = 0; i < iter_pci; i++) {
                    soc_pci_write(unit, soc_reg_addr(unit, test_register,
                                                 REG_PORT_ANY, 0), reg);
                }
            }
            benchmark_end(b);
        }
    }
#endif /* BCM_ESW_SUPPORT */

#ifdef BCM_ESW_SUPPORT
    if (SOC_IS_ESW(unit)) {
        /* find an appropriate block to read and write */
        if (SOC_PORT_NUM(unit, fe) > 0) {
	    port = SOC_PORT(unit, fe, 0);
        } else if (SOC_PORT_NUM(unit, ge) > 0) {
	    port = SOC_PORT(unit, ge, 0);
        } else if (SOC_PORT_NUM(unit, xe) > 0) {
	    port = SOC_PORT(unit, xe, 0);
        } else {
	    port = SOC_PORT(unit, hg, 0);
        }
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        } else {
            phy_port = port;
        }
        blk = SOC_PORT_BLOCK(unit, phy_port);
    }
#endif /* BCM_ESW_SUPPORT */

    if (benchmark_begin(b, "read soc reg", "read",
			iter_cmic)) {
        if (SOC_IS_ESW(unit)) {
#ifdef BCM_ESW_SUPPORT
#ifdef BCM_HERCULES_SUPPORT
            if (SOC_IS_HERCULES(unit)) {
                for (i = 0; i < iter_cmic; i++) {
                    /*    coverity[unchecked_value]    */
                    READ_ING_COS_MAPr(unit, port, &reg);
                }
            } else
#endif /* BCM_HERCULES_SUPPORT */
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit)) {
                for (i = 0; i < iter_cmic; i++) {
                    /*    coverity[unchecked_value]    */
                    READ_EGR_CONFIGr(unit, &reg);
                }
            } else
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
            if (SOC_IS_FBX(unit)) {
                for (i = 0; i < iter_cmic; i++) {
                    /*    coverity[unchecked_value]    */
                    READ_EMIRROR_CONTROLr(unit, port, &reg);
                }
            } else
#endif /* BCM_FIREBOLT_SUPPORT */
            {
                /* Do Nothing */
            }
#endif /* BCM_ESW_SUPPORT */
        } else {
#ifdef BCM_DNX_SUPPORT
            if (SOC_IS_DNX(unit)) {
                for (i = 0; i < iter_cmic; i++) {
                    SOC_IF_ERROR_RETURN(READ_ECI_VERSION_REGISTERr(unit, &reg));
                }
            }
#endif
       }
    }
    benchmark_end(b);
    if (benchmark_begin(b, "write soc reg", "write",
			iter_cmic)) {
        if (SOC_IS_ESW(unit)) {
#ifdef BCM_ESW_SUPPORT
#ifdef BCM_HERCULES_SUPPORT
            if (SOC_IS_HERCULES(unit)) {
                for (i = 0; i < iter_cmic; i++) {
                    WRITE_ING_COS_MAPr(unit, port, reg);
                }
            } else
#endif /* BCM_HERCULES_SUPPORT */
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit)) {
                for (i = 0; i < iter_cmic; i++) {
                    WRITE_EGR_CONFIGr(unit, reg);
                }
            } else
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
            if (SOC_IS_FBX(unit)) {
                for (i = 0; i < iter_cmic; i++) {
                    WRITE_EMIRROR_CONTROLr(unit, port, reg);
                }
            } else
#endif /* BCM_FIREBOLT_SUPPORT */
            {
                /* Do Nothing */
            }
#endif /* BCM_ESW_SUPPORT */
        } else {
#ifdef BCM_DNX_SUPPORT
            if (SOC_IS_DNX(unit)) {
                for (i = 0; i < iter_cmic; i++) {
                    SOC_IF_ERROR_RETURN(WRITE_ECI_VERSION_REGISTERr(unit, reg));
                }
            }
#endif
        }
    }
    benchmark_end(b);

    if (SOC_IS_ESW(unit)) {
#ifdef BCM_ESW_SUPPORT
#ifdef BCM_HERCULES_SUPPORT
        if (SOC_IS_HERCULES(unit)) {
            if (benchmark_begin(b, "read mc table entry", "read",
                                iter_cmic)) {
                for (i = 0; i < iter_cmic; i++) {
                    /*    coverity[unchecked_value]    */
                    READ_MEM_MCm(unit, blk, 10, entry);
                }
            }
            benchmark_end(b);
            
            if (benchmark_begin(b, "write mc table entry", "write",
                                iter_cmic)) {
                for (i = 0; i < iter_cmic; i++) {
                    /*    coverity[unchecked_value]    */
                    WRITE_MEM_MCm(unit, blk, 10, entry);
                }
            }
            benchmark_end(b);
        } else
#endif /* BCM_HERCULES_SUPPORT */
#ifdef BCM_BRADLEY_SUPPORT
        if (SOC_IS_XGS3_FABRIC(unit)) {
            blk = SOC_MEM_BLOCK_ANY(unit, L2MCm);
            if (benchmark_begin(b, "read l2mc table entry", "read",
                                iter_cmic)) {
                for (i = 0; i < iter_cmic; i++) {
                    /*    coverity[unchecked_value]    */
                    READ_L2MCm(unit, blk, 10, entry);
                }
            }
            benchmark_end(b);
        
            if (benchmark_begin(b, "write l2mc table entry", "write",
                                iter_cmic)) {
                for (i = 0; i < iter_cmic; i++) {
                    WRITE_L2MCm(unit, blk, 10, entry);
                }
            }
            benchmark_end(b);
        } else
#endif /* BCM_BRADLEY_SUPPORT */
#ifdef BCM_RAPTOR_SUPPORT
        if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
            blk = SOC_MEM_BLOCK_ANY(unit, FP_POLICY_TABLEm);
            if (benchmark_begin(b, "read FP Policy table entry", "read",
                                iter_cmic)) {
                for (i = 0; i < iter_cmic; i++)
                    READ_FP_POLICY_TABLEm(unit, blk, 10, entry);
            }
            benchmark_end(b);
        
            if (benchmark_begin(b, "write FP Policy table entry", "write",
                                iter_cmic)) {
                for (i = 0; i < iter_cmic; i++) {
                    /*    coverity[unchecked_value]    */
                    WRITE_FP_POLICY_TABLEm(unit, blk, 10, entry);
                }
            }
            benchmark_end(b);
        } else
#endif /* BCM_RAPTOR_SUPPORT */
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        if (SOC_IS_FIREBOLT2(unit) || SOC_IS_TRX(unit)) {
            soc_mem_t mem = FP_POLICY_TABLEm;

            if (SOC_MEM_IS_VALID(unit, IFP_POLICY_TABLEm)) {
                mem = IFP_POLICY_TABLEm;
            }
            blk = SOC_MEM_BLOCK_ANY(unit, mem);
            if (benchmark_begin(b, "read FP Policy table entry", "read",
                                iter_cmic)) {
                for (i = 0; i < iter_cmic; i++) {
                    /*    coverity[unchecked_value]    */
                    soc_mem_read(unit, mem, blk, 10, entry);
                }
            }
            benchmark_end(b);
        
        
            /* No further testing for RCPU_ONLY units */
            if(SOC_IS_RCPU_ONLY(unit)) {
                SOC_MEM_TEST_SKIP_CACHE_SET(unit, FALSE);
                return rv; 
            }
        
#if defined(BCM_TRIUMPH3_SUPPORT)
            if (soc_feature(unit, soc_feature_ism_memory)) {
                if (benchmark_begin(b, "Clear L2_ENTRY_1 table", "clear",
                                    iter_mem)) {
                    for (i = 0; i < iter_mem; i++) {
                        /*    coverity[unchecked_value]    */
                        soc_mem_clear(unit, L2_ENTRY_1m, MEM_BLOCK_ALL, TRUE);
                    }
                }
                benchmark_end(b);
            } else 
#endif /* BCM_TRIUMPH3_SUPPORT */
            {
                if (benchmark_begin(b, "Clear L2X table", "clear",
                                    iter_mem)) {
                    for (i = 0; i < iter_mem; i++) {
                        /*    coverity[unchecked_value]    */
                        SOC_IF_ERROR_RETURN(soc_mem_clear(unit, L2Xm, MEM_BLOCK_ALL, TRUE));
                    }
                }
                benchmark_end(b);
            }

            if (benchmark_begin(b, "write FP Policy table entry", "write",
                                iter_cmic)) {
                soc_mem_t mem = FP_POLICY_TABLEm;

                if (SOC_MEM_IS_VALID(unit, IFP_POLICY_TABLEm)) {
                    mem = IFP_POLICY_TABLEm;
                }

                for (i = 0; i < iter_cmic; i++) {
                    /*    coverity[unchecked_value]    */
                    soc_mem_write(unit, mem, blk, 10, entry);
                }
            }
            benchmark_end(b);
        } else
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(unit)) {
            blk = SOC_MEM_BLOCK_ANY(unit, FP_TCAM_PLUS_POLICYm);
            if (benchmark_begin(b, "read FP Policy table entry", "read",
                                iter_cmic)) {
                for (i = 0; i < iter_cmic; i++) {
                    /*    coverity[unchecked_value]    */
                    READ_FP_TCAM_PLUS_POLICYm(unit, blk, 10, entry);
                }
            }
            benchmark_end(b);
        
            if (benchmark_begin(b, "write FP Policy table entry", "write",
                                iter_cmic)) {
                for (i = 0; i < iter_cmic; i++) {
                    /*    coverity[unchecked_value]    */
                    WRITE_FP_TCAM_PLUS_POLICYm(unit, blk, 10, entry);
                }
            }
            benchmark_end(b);
        } else
#endif /* BCM_FIREBOLT_SUPPORT */
        {
            /* Do Nothing */
        }
#endif /* BCM_ESW_SUPPORT */
    } else {
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit)) {
            blk = SOC_MEM_BLOCK_ANY(unit, CGM_DESTINATION_TABLEm);
            if (benchmark_begin(b, "read CGM Destination Table entry", "read", iter_cmic)) {
                for (i = 0; i < iter_cmic; i++) {
                    SOC_IF_ERROR_RETURN(READ_CGM_DESTINATION_TABLEm(unit, blk, 10, entry));
                }
                benchmark_end(b);
            }

            if (benchmark_begin(b, "write CGM Destination Table entry", "write", iter_cmic)) {
                for (i = 0; i < iter_cmic; i++) {
                    SOC_IF_ERROR_RETURN(WRITE_CGM_DESTINATION_TABLEm(unit, blk, 10, entry));
                }
                benchmark_end(b);
            }
        }
#endif /* BCM_DNX_SUPPORT */
    }

    /* Since TH3 supports MDIO operation only for ATE,
       skipping the below phy read/write as it involves
       MDIO operation
     */
    if(!SOC_IS_TOMAHAWK3(unit)) {
        test_p = 2;
        if(!IS_E_PORT(unit, test_p)) {
            /* If port 2 is invalid, change the test port to port 0 */
            test_p = 0;
        }

        if (IS_E_PORT(unit, test_p)) {
            if (! quickturn) {
                if (benchmark_begin(b, "read phy reg", "read",
                        iter_miim)) {

                    for (i = 0; i < iter_miim; i++)
                        soc_miim_read(unit, PORT_TO_PHY_ADDR(unit, test_p),
                            MII_PHY_ID0_REG, &phy_data);
                }
                benchmark_end(b);

                if (benchmark_begin(b, "write phy reg", "write",
                        iter_miim)) {
                    for (i = 0; i < iter_miim; i++)
                        soc_miim_write(unit, PORT_TO_PHY_ADDR(unit, test_p),
                            MII_PHY_ID0_REG, phy_data);
                }
                benchmark_end(b);
            }
        }/* IS_E_PORT */
    }
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit)) {
        soc_mem_t mem = KAPS_BUCKET_MEMORYm;
        int array_min = 0;
        int array_max = 0;
        int array_index;
        int index_min = soc_mem_index_min(unit, mem);
        int index_max = soc_mem_index_max(unit, mem);
        int entry_words = soc_mem_entry_words(unit, mem);
        int blk_min = SOC_MEM_BLOCK_MIN(unit, mem);
        int blk_max = SOC_MEM_BLOCK_MAX(unit, mem);
        int size;
        char *buffer;

        if (SOC_MEM_IS_ARRAY(unit, mem)) {
            soc_mem_array_info_t *maip = SOC_MEM_ARRAY_INFOP(unit, mem);
            if (maip) {
                array_max = (maip->numels - 1) + maip->first_array_index;
                array_min = maip->first_array_index;
            }
        }

        size = entry_words * sizeof(int) * (index_max - index_min);
        if (size * (array_max - array_min + 1) * (blk_max - blk_min + 1) <= b->sizeDMA) {
            /* DMA read KapsBucketMemory */
            if (benchmark_begin(b, "Dma read Kaps Bucket Memory", "xfer", iter_dma)) {
                for (i = 0; i < iter_dma; i++) {
                    SOC_MEM_BLOCK_ITER(unit, mem, blk) {
                        for (array_index = array_min; array_index <= array_max; array_index++) {
                            buffer = b->bufDMA + size * ((blk - blk_min) * (array_max - array_min + 1)
                                      + (array_index - array_min));
                            /* coverity[unchecked_value] */
                            /* coverity[CHECKED_RETURN: FALSE] */
                            SOC_IF_ERROR_RETURN(soc_mem_array_read_range(unit, mem, array_index, blk, index_min, index_max, buffer));
                        }
                    }
                }
                benchmark_end(b);
            }
            /* DMA write KapsBucketMemory */
            if (benchmark_begin(b, "Dma write Kaps Bucket Memory", "xfer", iter_dma)) {
                for (i = 0; i < iter_dma; i++) {
                    SOC_MEM_BLOCK_ITER(unit, mem, blk) {
                        for (array_index = array_min; array_index <= array_max; array_index++) {
                            buffer = b->bufDMA + size * ((blk - blk_min) * (array_max - array_min + 1)
                                     + (array_index - array_min));
                            /* coverity[unchecked_value] */
                            /* coverity[CHECKED_RETURN: FALSE] */
                            SOC_IF_ERROR_RETURN(soc_mem_array_write_range(unit, 0, mem, array_index, blk, index_min, index_max, buffer));
                        }
                    }
                }
                benchmark_end(b);
            }

            /* reset memory indirect mode */
            if (benchmark_begin(b, "Indirect reset Kaps Bucket Memory", "clear", iter_dma)) {
                for (i = 0; i < iter_dma; i++) {
                   SOC_IF_ERROR_RETURN(mem_indirect_reset(unit, mem));
                }
                benchmark_end(b);
            }

            /* reset memory Dma mode */
            if (benchmark_begin(b, "Dma reset Kaps Bucket Memory", "clear", iter_dma)) {
                for (i = 0; i < iter_dma; i++) {
                    SOC_IF_ERROR_RETURN(mem_dma_reset(unit, mem));
                }
                benchmark_end(b);
            }

            /* recover memory value */
            SOC_MEM_BLOCK_ITER(unit, mem, blk) {
                for (array_index = array_min; array_index <= array_max; array_index++) {
                    buffer = b->bufDMA + size * ((blk - blk_min) * (array_max - array_min + 1)
                            + (array_index - array_min));
                    SOC_IF_ERROR_RETURN(soc_mem_array_write_range(unit, 0, mem, array_index, blk, index_min, index_max, buffer));
                }
            }
        }
    }
#endif

#ifdef BCM_ESW_SUPPORT
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        uint32 l2_entry[SOC_MAX_MEM_WORDS], l2_result[SOC_MAX_MEM_WORDS];
        soc_mem_t       mem;
        int mem_ix, index;
        char buffer[100];

        for (mem_ix = 0; mem_ix < 4; mem_ix++) {
            switch(mem_ix) {
            case 0:
                mem = L2_ENTRY_1m;
                break;
            case 1:
                mem = L2_ENTRY_2m;
                break;
            case 2:
                if (soc_feature(unit, soc_feature_esm_support)) {
                    mem = EXT_L2_ENTRY_1m;
                    break;
                } else {
                    continue;
                }
            case 3:
                if (soc_feature(unit, soc_feature_esm_support)) {
                    mem = EXT_L2_ENTRY_2m;
                    break;
                } else {
                    continue;
                }
            default:
                rv = -1;
                SOC_MEM_TEST_SKIP_CACHE_SET(unit, FALSE);
                /* break; */ return rv;  
                
            }

            if (0 >= soc_mem_index_count(unit, mem)) {
                /* No entries for this memory type. */
                continue;
            }

            memset(&l2_entry, 0,
                   WORDS2BYTES(soc_mem_entry_words(unit, mem)));
            soc_mem_mac_addr_set(unit, mem, &l2_entry, MAC_ADDRf, mac);
            soc_mem_field32_set(unit, mem, &l2_entry, VLAN_IDf, 1);

            switch(mem_ix) {
            case 0:
                soc_mem_field32_set(unit, mem, &l2_entry, VALIDf, 1);
                soc_mem_field32_set(unit, mem, &l2_entry, KEY_TYPEf,
                                    SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE);
                break;
            case 1:
                soc_mem_field32_set(unit, mem, &l2_entry, KEY_TYPE_0f,
                                    SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE);
                soc_mem_field32_set(unit, mem, &l2_entry, KEY_TYPE_1f,
                                    SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE);
                soc_mem_field32_set(unit, mem, &l2_entry, WIDE_0f, 1);
                soc_mem_field32_set(unit, mem, &l2_entry, WIDE_1f, 1);
                soc_mem_field32_set(unit, mem, &l2_entry, VALID_0f, 1);
                soc_mem_field32_set(unit, mem, &l2_entry, VALID_1f, 1);
                break;
            default:
                soc_mem_field32_set(unit, mem, &l2_entry, KEY_TYPEf, 0);
                break;
            }

            /* Cache the entry for use in second step */
            memcpy(&l2_result, &l2_entry,
                   WORDS2BYTES(soc_mem_entry_words(unit, mem)));

            sal_sprintf(buffer, "ISM %s insert/delete",
                        SOC_MEM_NAME(unit, mem));
            if (benchmark_begin(b, buffer, "i+d",
                                iter_cmic)) {
                for (i = 0; i < iter_cmic; i++) {
                    _ism_l2_mem_insert(unit, mem, &l2_entry);
                    _ism_l2_mem_delete(unit, mem, &l2_entry);
                }
            }
            benchmark_end(b);

            memcpy(&l2_entry, &l2_result,
                   WORDS2BYTES(soc_mem_entry_words(unit, mem)));
            soc_mem_mac_addr_set(unit, mem, &l2_entry, MAC_ADDRf, maclkup);

            sal_sprintf(buffer, "ISM %s lookup",
                        SOC_MEM_NAME(unit, mem));
            if (benchmark_begin(b, buffer, "lkup", iter_cmic)) {
                for (i = 0; i < iter_cmic; i++) {
                    _ism_l2_mem_lookup(unit, mem, &l2_entry,
                                       &l2_result, &index);
                }
            }
            benchmark_end(b);

            sal_sprintf(buffer, "DMA ISM %s table",
                        SOC_MEM_NAME(unit, mem));
            if (benchmark_begin(b, buffer, "xfer",
                                iter_dma)) {
                for (i = 0; i < iter_dma; i++)
                    if (soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                           soc_mem_index_min(unit, mem),
                                           soc_mem_index_max(unit, mem),
                                           b->bufDMA) < 0) {
                        rv = -1;
                        break;
                    }
            }
            benchmark_end(b);
        }

        /* IP tests for ESM analysis*/
        mem = PORT_TABm;
        sal_sprintf(buffer, "DMA %s table", SOC_MEM_NAME(unit, mem));
        if (benchmark_begin(b, buffer, "xfer", iter_dma)) {
            for (i = 0; i < iter_dma; i++)
                if (soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                       soc_mem_index_min(unit, mem),
                                       soc_mem_index_max(unit, mem),
                                       b->bufDMA) < 0) {
                    rv = -1;
                    break;
                }
        }
        benchmark_end(b);

        if ((-1 == soc_mem_index_max(unit, VRFm)))
        {
            mem = VLAN_PROTOCOL_DATAm;
        }
        else
        {
            mem = VRFm;
        }
        
        sal_sprintf(buffer, "DMA %s table", SOC_MEM_NAME(unit, mem));
        if (benchmark_begin(b, buffer, "xfer", iter_dma)) {
            for (i = 0; i < iter_dma; i++)
                if (soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                       soc_mem_index_min(unit, mem),
                                       soc_mem_index_max(unit, mem),
                                       b->bufDMA) < 0) {
                    rv = -1;
                    break;
                }
        }
        benchmark_end(b);
        SOC_MEM_TEST_SKIP_CACHE_SET(unit, FALSE);
        return rv; /* END OF ISM BENCHMARKS */
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_XGS_SWITCH(unit)) {
        l2x_entry_t l2xent;
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
        char *l3_dma_buf;
        int  alloc_size;
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */
        int (*l2x_ins)(int unit, l2x_entry_t *entry);
        int (*l2x_del)(int unit, l2x_entry_t *entry);
        int (*l2x_lkup)(int unit, l2x_entry_t *key, l2x_entry_t *result,
                       int *index_ptr);

#ifdef BCM_TRX_SUPPORT
        if (soc_feature(unit, soc_feature_generic_table_ops)) {
            l2x_ins = _gen_l2x_insert;
            l2x_del = _gen_l2x_delete;
            l2x_lkup = _gen_l2x_lookup;
        } else 
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(unit)) {
            l2x_ins = soc_fb_l2x_insert;
            l2x_del = soc_fb_l2x_delete;
            l2x_lkup = soc_fb_l2x_lookup;
        } else 
#endif /* BCM_FIREBOLT_SUPPORT */
        {
            l2x_ins = NULL;
            l2x_del = NULL;
            l2x_lkup = NULL;
        }

        memset(&l2xent, 0, sizeof(l2xent));
        soc_L2Xm_mac_addr_set(unit, &l2xent, MAC_ADDRf, mac);
        soc_L2Xm_field32_set(unit, &l2xent, VLAN_IDf, 1);

        if (benchmark_begin(b, "L2 insert/delete", "i+d",
                            iter_cmic)) {
            for (i = 0; i < iter_cmic; i++) {
                l2x_ins(unit, &l2xent);
                l2x_del(unit, &l2xent);
            }
        }
        benchmark_end(b);

        memset(&l2xent, 0, sizeof(l2xent));
        soc_L2Xm_mac_addr_set(unit, &l2xent, MAC_ADDRf, maclkup);
        soc_L2Xm_field32_set(unit, &l2xent, VLAN_IDf, 1);

        if (benchmark_begin(b, "L2 lookup", "lkup",
                            iter_cmic)) {
            for (i = 0; i < iter_cmic; i++) {
                l2x_entry_t	l2xresult;
                int		index;
                l2x_lkup(unit, &l2xent, &l2xresult, &index);
            }
        }
        benchmark_end(b);

        if (benchmark_begin(b, "DMA L2 table", "xfer",
                    iter_dma)) {
            for (i = 0; i < iter_dma; i++) {
                int min, max, step;
                step = (b->sizeDMA / (sizeof(l2x_entry_t) * 1024)) * 1024;
                for (min = soc_mem_index_min(unit, L2Xm),
                     max = soc_mem_index_max(unit, L2Xm); 
                    min < max; min += step) {
                    if (soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY,
                        min,
                        (min + step - 1) < max ? (min + step - 1) : max,
                        b->bufDMA) < 0) {
                        rv = -1;
                        break;
                    }
                }
            }
        }
        benchmark_end(b);

#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
        if (SOC_IS_FBX(unit) &&
            !SOC_IS_HAWKEYE(unit) &&
            SOC_MEM_IS_VALID(unit, L3_ENTRY_ONLYm) &&
            (soc_mem_index_count(unit,L3_ENTRY_ONLYm) > 0)) {

            if (benchmark_begin(b, "Read L3_ENTRY_V4 table ", "xfer",
                                iter_cmic)) {
                for (i = 0; i < iter_cmic; i++) {
                    /*    coverity[unchecked_value]    */
                    SOC_IF_ERROR_RETURN(READ_L3_ENTRY_ONLYm(unit, blk, 10, entry));
                }
            }
            benchmark_end(b);

            alloc_size = (SOC_L3X_BUCKET_SIZE(unit) / 2) *
                WORDS2BYTES(soc_mem_entry_words(unit, L3_ENTRY_ONLYm));
            l3_dma_buf =  soc_cm_salloc(unit, alloc_size, "L3 bucket dma");
            if (NULL != l3_dma_buf) {
                sal_memset(l3_dma_buf, 0, alloc_size);

                if (benchmark_begin(b, "Dma read L3_ENTRY bucket", "xfer",
                                    iter_cmic)) {
                    for (i = 0; i < iter_cmic; i++) {
                        if (soc_mem_read_range(unit, L3_ENTRY_ONLYm, MEM_BLOCK_ANY,
                                           10 * SOC_L3X_BUCKET_SIZE(unit), 
                                           10 * SOC_L3X_BUCKET_SIZE(unit) + 
                                           (SOC_L3X_BUCKET_SIZE(unit) / 2)  - 1,
                                           l3_dma_buf) < 0) {
                            rv = -1;
                            break;
                       }
                    }
                }
                benchmark_end(b);

                soc_cm_sfree(unit, l3_dma_buf);
            } else {
                    rv = -1;
            }
        }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */  
        SOC_MEM_TEST_SKIP_CACHE_SET(unit, FALSE);
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#endif /* BCM_ESW_SUPPORT */


    {
         sal_mutex_t	m;

         m = sal_mutex_create("bench");

         if (benchmark_begin(b, "mutex lock/unlock", "lock",
                             iter_cmic)) {
             for (i = 0; i < iter_cmic; i++) {
                 sal_mutex_take(m, sal_mutex_FOREVER);
                 sal_mutex_give(m);
             }
         }
         benchmark_end(b);
         sal_mutex_destroy(m);
    }
#ifdef BCM_ESW_SUPPORT
    if (SOC_IS_TRIDENT(unit)) { /* only For Trident or TD+ */
#if defined(INCLUDE_L3)
        int rv = 0, j = 0;
        bcm_mac_t  nh_mac = {0,0xbb,0,0,0,0};
        bcm_l3_intf_t intf;
        bcm_l3_route_t route;
        bcm_l3_info_t l3_info;
        int c;

        bcm_l3_info_t_init(&l3_info);
        rv = bcm_l3_info(unit, &l3_info);

        bcm_l3_intf_t_init(&intf);
        intf.l3a_vid      = 1;
        intf.l3a_flags    = BCM_L3_ADD_TO_ARL | BCM_L3_WITH_ID;
        sal_memcpy(intf.l3a_mac_addr, &nh_mac, sizeof(intf.l3a_mac_addr));

        rv = bcm_l3_intf_create(unit, &intf);
        bcm_l3_route_t_init(&route);

        route.l3a_vrf = 10;
        route.l3a_subnet = 0x00000000;
        route.l3a_ip_mask = 0xffff0000;
        route.l3a_nexthop_ip = 0x0b0b0b0b;
        route.l3a_intf = intf.l3a_intf_id;
        sal_memcpy(&route.l3a_nexthop_mac, b, sizeof(route.l3a_nexthop_mac));

        if (benchmark_begin(b, "L3 DEFIP IPV4 Routes Add", "i+d", l3_info.l3info_max_route)) {
            for (c = 1; c <= l3_info.l3info_max_route; c++) {
                route.l3a_subnet = 0x0000000a + (c << 16);
                rv = bcm_l3_route_add(unit, &route);
                if (BCM_FAILURE(rv)) {
                    cli_out("bcm_l3_route_add: c = %d %s\n", c, bcm_errmsg(rv));
                    break;
                }
                if (c % 4096 == 0) {
                    route.l3a_ip_mask = 0xfffff000;
                    route.l3a_subnet = 0xb000000a + (c << 16);
                    j = j + 1;
                    if (j == 2) {
                        route.l3a_ip_mask = 0xffffff00;
                        route.l3a_subnet = 0xc000000a + (c << 16);
                    }
                    if (j == 3) {
                        route.l3a_ip_mask = 0xfffffff0;
                        route.l3a_subnet = 0xd000000a + (c << 16);
                    }
                    if (j == 4) {
                        route.l3a_ip_mask = 0xffffffff;
                        route.l3a_subnet = 0xe000000a + (c << 16);
                    }

                    route.l3a_vrf += c % 10;
                }
            }
        }
        benchmark_end(b);
#endif /* INCLUDE_L3 */
    } /* SOC_IS_TD_TT() */
#endif /* BCM_ESW_SUPPORT */

    /* Time config var get */
    if (benchmark_begin(b, "ConfigVar Get", "get", iter_cmic)) {
        for (i = 0; i < iter_cmic; i++) {
            soc_cm_config_var_get(unit, "p_abracadabra");
        }
    }
    benchmark_end(b);
  
    SOC_MEM_TEST_SKIP_CACHE_SET(unit, FALSE);
    return rv;
}
