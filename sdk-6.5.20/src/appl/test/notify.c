/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Misc. tests
 */

#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/appl/io.h>

#include <bcm/link.h>
#include <bcm/error.h>

#include <soc/drv.h>
#include <soc/debug.h>

#include "testlist.h"

#if defined(BCM_STRATA_SUPPORT)
#define LS_MAX_RETRIES	10000

#define LS_TEST_PATTERNS ((2 * SOC_MAX_NUM_PORTS) + 4)

typedef struct link_scan_s {
    uint32          ls_irq_mask;
    pbmp_t	    ls_bmp[LS_TEST_PATTERNS];
    int             ls_linkscan;
} link_scan_t;

static link_scan_t *link_scan[SOC_MAX_NUM_DEVICES];

/*
 * Function:    not_link_done
 * Purpose:     Clean up after liststatus test.
 * Parameters:  u - unit #
 *              pa - cookie from init.
 * Returns:     0 - success, -1 - failed.
 */
int
not_link_done(int u, void *pa)
{
    link_scan_t    *ls = pa;
    int             rv;

    if (ls == NULL) {
	return 0;
    }
    soc_intr_enable(u, ls->ls_irq_mask & IRQ_LINK_STAT_MOD);

    if ((rv = bcm_linkscan_enable_set(u, ls->ls_linkscan)) < 0) {
	test_error(u, "ERROR: bcm_link_scan_enable_set failed: %s\n",
		   bcm_errmsg(rv));;
	rv = -1;
    }

    sal_free(ls);
    link_scan[u] = NULL;

    return rv;
}

/*
 * Function:    not_link_init
 * Purpose:     Initialize link status test.
 * Parameters:  u - unit #
 *              a - args
 *              pa - cookie used in later calls.
 * Returns:     0 - success, -1 failed.
 */
int
not_link_init(int u, args_t *a, void **pa)
{
    link_scan_t		*ls;
    int			i, rv;
    soc_port_t          port;

    if (ARG_CNT(a)) {
	test_error(u, "Arguments not supported\n");
	return -1;
    }

    if (link_scan[u] == NULL) {
	link_scan[u] = sal_alloc(sizeof(link_scan_t), "not_link");
	if (link_scan[u] == NULL) {
	    test_error(u, "ERROR: cannot allocate memory\n");
	    return -1;
	}
	sal_memset(link_scan[u], 0, sizeof(link_scan_t));
    }
    ls = link_scan[u];

    SOC_PBMP_ASSIGN(ls->ls_bmp[0], PBMP_PORT_ALL(u));
    SOC_PBMP_CLEAR(ls->ls_bmp[1]);
    SOC_PBMP_CLEAR(ls->ls_bmp[2]);
    SOC_PBMP_WORD_SET(ls->ls_bmp[2], 0, 0x55555555);
    SOC_PBMP_AND(ls->ls_bmp[2], PBMP_PORT_ALL(u));
    SOC_PBMP_CLEAR(ls->ls_bmp[3]);
    SOC_PBMP_WORD_SET(ls->ls_bmp[3], 0, 0xaaaaaaaa);
    SOC_PBMP_AND(ls->ls_bmp[3], PBMP_PORT_ALL(u));

    i = 4;
    PBMP_PORT_ITER(u, port) {
	SOC_PBMP_CLEAR(ls->ls_bmp[i]);
	SOC_PBMP_PORT_ADD(ls->ls_bmp[i], port);
	SOC_PBMP_ASSIGN(ls->ls_bmp[i + NUM_E_PORT(u)], PBMP_PORT_ALL(u));
	SOC_PBMP_REMOVE(ls->ls_bmp[i + NUM_E_PORT(u)], ls->ls_bmp[i]);
        i++;
    }

    if ((rv = bcm_linkscan_enable_get(u, &ls->ls_linkscan)) < 0) {
	test_error(u, "ERROR: bcm_linkscan_enable_get failed: %s",
		   bcm_errmsg(rv));
	sal_free(ls);
	link_scan[u] = NULL;
	return -1;
    }

    if ((rv = bcm_linkscan_enable_set(u, 0)) < 0) {
	test_error(u, "ERROR: bcm_link_scan_enable_set failed: %s\n",
		   bcm_errmsg(rv));
	sal_free(ls);
	link_scan[u] = NULL;
	return -1;
    }

    /* Verify Auto Link Scan was disabled */

    if ((soc_pci_read(u, CMIC_SCHAN_CTRL)) & SC_MIIM_LINK_SCAN_EN_TST) {
	test_error(u, "ERROR: Could not clear MIIM_LINK_SCAN_EN bit\n");
	not_link_done(u, ls);
	return -1;
    }

    ls->ls_irq_mask = soc_intr_disable(u, IRQ_LINK_STAT_MOD);

    *pa = ls;
    return 0;
}

/*
 * Function:
 * Purpose:
 * Parameters:
 * Returns:
 */
int
not_link_test(int unit, args_t *a, void *pa)
{
    link_scan_t    *ls = (link_scan_t *) pa;
    schan_msg_t     schMsg;
    int             i, port, blk, rv;
    uint32          value;
    uint32          cnt = 0;
    uint64          val64;
    char	    pfmt[SOC_PBMP_FMT_LEN];
    int             bitmap_ext = SOC_IS_TUCANA(unit); 

    COMPILER_REFERENCE(a);

    /* Go through all forwarding port values */
    for (i = 0; i < LS_TEST_PATTERNS; i++) {
	LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Setting the Forwarding Port register to %s\n"),
                  SOC_PBMP_FMT(ls->ls_bmp[0], pfmt)));
	/* Update forwarding port register for all blocks/functional us */
	COMPILER_64_SET(val64, 0, SOC_PBMP_WORD_GET(ls->ls_bmp[0], 0));
	if (SOC_IS_TUCANA(unit)) {
            COMPILER_64_SET(val64, 1, SOC_PBMP_WORD_GET(ls->ls_bmp[0], 1));
        }
	soc_reg64_write_all_blocks(unit, EPC_LINKr, val64);

	(void)SOC_PBMP_FMT(ls->ls_bmp[i], pfmt);
	LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Sending Linkstat with value %s\n"), pfmt));
	/* Send a link status SChannel message */
	schan_msg_clear(&schMsg);
	schMsg.header.opcode = LINKSTAT_NOTIFY_MSG;

	/* Use bitmap structure */
	schMsg.bitmap.bitmap = SOC_PBMP_WORD_GET(ls->ls_bmp[i], 0);

        if (bitmap_ext) {
            schMsg.bitmap.bitmap_word1 = SOC_PBMP_WORD_GET(ls->ls_bmp[i], 1);
        }

	/* Write message out */
        if (bitmap_ext) {
            if ((rv = soc_schan_op(unit, &schMsg, 3, 0, 0)) < 0) {
                test_error(unit, "Error in soc_schan_op: %s\n", bcm_errmsg(rv));
                return (-1);
            }
        } else {       
            if ((rv = soc_schan_op(unit, &schMsg, 2, 0, 0)) < 0) {
                test_error(unit, "Error in soc_schan_op: %s\n", bcm_errmsg(rv));
                return (-1);
            }
        }

	/* Wait until a link up/down message is available for us */
	while (!((value = soc_pci_read(unit, CMIC_SCHAN_CTRL)) &
		 SC_LINK_STAT_MSG_TST) && (++cnt < LS_MAX_RETRIES)) {
	    LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "Waiting for message...\n")));
	}
	if (cnt >= LS_MAX_RETRIES) {
	    test_error(unit,
		       "Sent S-Channel link status message, no response!!\n");
	    return (-1);
	}
	/* Clear link up/down message */
	soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_LINK_STAT_MSG_CLR);

	/* Read PCI link status ... */
	value = soc_pci_read(unit, CMIC_LINK_STAT);
	if (value != SOC_PBMP_WORD_GET(ls->ls_bmp[i], 0)) {
	    test_error(unit, "ERROR: unexpected link status on cmic\n"
		       "\tread 0x%x, expecting 0x%x\n",
		       value, SOC_PBMP_WORD_GET(ls->ls_bmp[i], 0));
	    /* Just continue if we return. */
	}

        if (bitmap_ext) {
            /* Read PCI link status ... */
            value = soc_pci_read(unit, CMIC_LINK_STAT_MOD1);
            if (value != SOC_PBMP_WORD_GET(ls->ls_bmp[i], 1)) {
                test_error(unit, 
                           "ERROR: unexpected link status (mod1) on cmic\n"
                           "\tread 0x%x, expecting 0x%x\n",
                           value, SOC_PBMP_WORD_GET(ls->ls_bmp[i], 1));
                /* Just continue if we return. */
            }
        }

	SOC_BLOCK_ITER(unit, blk, SOC_BLK_ETHER) {
	    port = SOC_BLOCK_PORT(unit, blk);
	    rv = READ_EPC_LINKr(unit, port, &val64);
	    if (rv < 0) {
		test_error(unit,
			   "ERROR: EPC_LINK register (port %s) read failed: %s\n",
			   SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
		return -1;
	    }
	    value = COMPILER_64_LO(val64);
	    if (value != SOC_PBMP_WORD_GET(ls->ls_bmp[i], 0)) {
		test_error(unit, "ERROR: unexpected link status on port %s\n"
			   "\tread 0x%x, expecting 0x%x\n",
			   SOC_PORT_NAME(unit, port),
			   value, SOC_PBMP_WORD_GET(ls->ls_bmp[i], 0));
		return -1;
	    }
            if (bitmap_ext) {
                value = COMPILER_64_HI(val64);
                if (value != SOC_PBMP_WORD_GET(ls->ls_bmp[i], 1)) {
                    test_error(unit, 
                               "ERROR: unexpected link status (mod1) on port %s\n"
                               "\tread 0x%x, expecting 0x%x\n",
                               SOC_PORT_NAME(unit, port),
                               value, SOC_PBMP_WORD_GET(ls->ls_bmp[i], 1));
                    return -1;
                }
            }
	}
    }

    return 0;
}

/*
 * Function:
 * Purpose:
 * Parameters:
 * Returns:
 */
int
cos_stat_test(int unit, args_t *a, void *pa)
{
    int             i, cos, port, npat, rv;
    schan_msg_t     schMsg;
    uint32          value, setval;
    pbmp_t          bmp[LS_TEST_PATTERNS];

    COMPILER_REFERENCE(pa);

    if (ARG_CNT(a)) {
	test_error(unit, "Arguments not supported\n");
	return (-1);
    }

    SOC_PBMP_ASSIGN(bmp[0], PBMP_PORT_ALL(unit));
    SOC_PBMP_CLEAR(bmp[1]);
    SOC_PBMP_CLEAR(bmp[2]);
    SOC_PBMP_WORD_SET(bmp[2], 0, 0x55555555);
    SOC_PBMP_AND(bmp[2], PBMP_PORT_ALL(unit));
    SOC_PBMP_CLEAR(bmp[3]);
    SOC_PBMP_WORD_SET(bmp[3], 0, 0xaaaaaaaa);
    SOC_PBMP_AND(bmp[3], PBMP_PORT_ALL(unit));

    i = 4;
    PBMP_PORT_ITER(unit, port) {
	SOC_PBMP_CLEAR(bmp[i]);
	SOC_PBMP_PORT_ADD(bmp[i], port);
	SOC_PBMP_ASSIGN(bmp[i + NUM_E_PORT(unit)], PBMP_PORT_ALL(unit));
	SOC_PBMP_REMOVE(bmp[i + NUM_E_PORT(unit)], bmp[i]);
        i++;
    }
    npat = i + NUM_E_PORT(unit);

    for (cos = 0; cos < NUM_COS(unit); cos++) {
	/* Go through all forwarding port values */
	for (i = 0; i < npat; i++) {
	    setval = SOC_PBMP_WORD_GET(bmp[0], 0);
	    LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "Setting the COS%d Status register to 0x%x\n"),
                      cos, setval));

	    /* Update forwarding port register all blocks/functional units */
	    PBMP_PORT_ITER(unit, port) {
		rv = WRITE_COS_ENABLEr(unit, port, cos, setval);
		if (rv < 0) {
		    test_error(unit, "ERROR: COS_ENABLE.%d register (port %s) write failed: %s\n",
			       cos, SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
		    return -1;
		}
	    }

	    /* Send status change SChannel message */

	    setval = SOC_PBMP_WORD_GET(bmp[i], 0);
	    LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "Sending COS%d status with value 0x%x\n"), cos, setval));
	    schan_msg_clear(&schMsg);
	    schMsg.header.opcode = COS_QSTAT_NOTIFY_MSG;

	    /* Use bitmap structure */
	    schMsg.bitmap.bitmap = setval;
	    schMsg.header.cos = cos;

	    /* Write message out */
	    if ((rv = soc_schan_op(unit, &schMsg, 2, 0, 0)) < 0) {
		test_error(unit, "ERROR: soc_schan_op failed: %s\n",
			   bcm_errmsg(rv));
		return -1;
	    }

	    /* Read CMIC register ... */

	    value = soc_pci_read(unit, CMIC_COS_ENABLE(cos));
	    value = ~value;		/* cmic register is inverted */

	    if (value != setval) {
		test_error(unit, "ERROR: COS_ENABLE.%d register (CMIC) unexpected value\n"
			   "\tread 0x%x, expected 0x%x\n",
			   cos,
			   value, setval);
		return -1;
	    }

	    PBMP_PORT_ITER(unit, port) {
		rv = READ_COS_ENABLEr(unit, port, cos, &value);
		if (rv < 0) {
		    test_error(unit, "ERROR: COS_ENABLE.%d register (port %s) read failed: %s\n",
			       cos, SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
		    return -1;
		}
		if (value != setval) {
		    test_error(unit, "ERROR: COS_ENABLE.%d register (port %s) unexpected value\n"
			       "\tread 0x%x, expected 0x%x\n",
			       cos, SOC_PORT_NAME(unit, port),
			       value, setval);
		    return -1;
		}
	    }
	}
    }

    return 0;
}

/*
 * Test various given register status with various patterns
 */
STATIC int
regStatus_test(int unit, soc_reg_t reg)
{
    int		i, rv, npat;
    schan_msg_t	schMsg;
    uint32	value, setval;
    pbmp_t	bmp[LS_TEST_PATTERNS];
    char	*regname;
    int		blk, port;
    uint32	addr;

    SOC_PBMP_ASSIGN(bmp[0], PBMP_PORT_ALL(unit));
    SOC_PBMP_CLEAR(bmp[1]);
    SOC_PBMP_CLEAR(bmp[2]);
    SOC_PBMP_WORD_SET(bmp[2], 0, 0x55555555);
    SOC_PBMP_AND(bmp[2], PBMP_PORT_ALL(unit));
    SOC_PBMP_CLEAR(bmp[3]);
    SOC_PBMP_WORD_SET(bmp[3], 0, 0xaaaaaaaa);
    SOC_PBMP_AND(bmp[3], PBMP_PORT_ALL(unit));

    i = 4;
    PBMP_PORT_ITER(unit, port) {
	SOC_PBMP_CLEAR(bmp[i]);
	SOC_PBMP_PORT_ADD(bmp[i], port);
	SOC_PBMP_ASSIGN(bmp[i + NUM_E_PORT(unit)], PBMP_PORT_ALL(unit));
	SOC_PBMP_REMOVE(bmp[i + NUM_E_PORT(unit)], bmp[i]);
        i++;
    }
    npat = i + NUM_E_PORT(unit);

    if (!SOC_REG_IS_VALID(unit, reg)) {
	test_error(unit, "ERROR: register %d is not valid for unit %d\n",
		   reg, unit);
	return -1;
    }
    regname = SOC_REG_NAME(unit, reg);

    /* Go through all forwarding port values */
    for (i = 0; i < npat; i++) {
	setval = SOC_PBMP_WORD_GET(bmp[0], 0);
	LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Setting register %s to 0x%x\n"),
                  regname, setval));
	soc_reg_write_all_blocks(unit, reg, setval);

	schan_msg_clear(&schMsg);

	setval = SOC_PBMP_WORD_GET(bmp[i], 0);
	/* Send status change SChannel message */
	switch (reg) {
	case EPC_HOLr:
	    schMsg.header.opcode = HOL_STAT_NOTIFY_MSG;
	    break;
	case EGR_BKP_DISr:
	    schMsg.header.opcode = BP_DISCARD_STATUS_MSG;
	    break;
	default:
	    test_error(unit, "ERROR: unexpected register %s\n", regname);
	    return -1;
	}
	LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Sending schan message value 0x%x\n"), setval));

	/* Use bitmap structure */
	schMsg.bitmap.bitmap = setval;

	/* Write message out */
	if ((rv = soc_schan_op(unit, &schMsg, 2, 0, 0)) < 0) {
	    test_error(unit, "soc_schan_op failed: %s\n", bcm_errmsg(rv));
	    return -1;
	}

	/* Read CMIC register ... */

	switch (reg) {
	case EPC_HOLr:
	    value = soc_pci_read(unit, CMIC_HOL_STAT);
	    value = ~value;	       /* cmic register is inverted */
	    break;
	case EGR_BKP_DISr:
	    value = soc_pci_read(unit, CMIC_IGBP_DISCARD);
	    break;
	default:
	    test_error(unit, "ERROR: unexpected register %s\n", regname);
	    return -1;
	}
	if (value != setval) {
	    test_error(unit,
		       "ERROR: %s register (CMIC) unexpected value\n"
		       "   read=0x%x, expecting=0x%x\n",
		       regname,
		       value, setval);
	    return -1;
	}

	SOC_BLOCK_ITER(unit, blk, SOC_BLK_ETHER) {
	    port = SOC_BLOCK_PORT(unit, blk);
	    addr = soc_reg_addr(unit, reg, port, 0);
	    if ((rv = soc_reg32_read(unit, addr, &value)) < 0) {
		test_error(unit,
			   "ERROR: %s register (port %s) read failed: %s\n",
			   regname, SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
		return -1;
	    }
	    if (value != setval) {
		test_error(unit,
			   "ERROR: %s register (port %s) unexpected value\n"
			   "   read=0x%x, expecting=0x%x\n",
			   regname, SOC_PORT_NAME(unit, port),
			   value, setval);
		return -1;
	    }
	}
    }

    return 0;
}

/*ARGSUSED*/
int
hol_stat_test(int u, args_t *a, void *pa)
{
    COMPILER_REFERENCE(a);
    COMPILER_REFERENCE(pa);

    return regStatus_test(u, EPC_HOLr);
}

/*ARGSUSED*/
int
bkp_stat_test(int u, args_t *a, void *pa)
{
    COMPILER_REFERENCE(a);
    COMPILER_REFERENCE(pa);

    return regStatus_test(u, EGR_BKP_DISr);
}
#endif /* BCM_STRATA_SUPPORT */
