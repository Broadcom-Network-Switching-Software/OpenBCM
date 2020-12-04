/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Counter Tests
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_COUNTER

#include <sal/types.h>

#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>


#include <soc/debug.h>
#include <soc/counter.h>

#include "testlist.h"

#if defined(BCM_DNX_SUPPORT)
#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/stat/mib/mib_stat_dbal.h>
#include <bcm_int/dnx/stat/mib/mib_stat.h>
#endif


#if defined BCM_PETRA_SUPPORT
/*
* Get counter type for input port
*/
static soc_ctr_type_t
get_port_ctype(int unit, soc_port_t port) {
    int blk = -1;
    int bindex = -1;
    int blktype = -1;
    int phy_port;
    soc_ctr_type_t ctype = -1;

    if (soc_feature(unit, soc_feature_logical_port_num)) {
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    } else {
        phy_port = port;
    }
    blk = SOC_PORT_BLOCK(unit, phy_port);
    bindex = SOC_PORT_BINDEX(unit, phy_port);

    if (blk < 0 && bindex < 0) {
        return ctype;
    }

    blktype = SOC_BLOCK_INFO(unit, blk).type;

    switch (blktype) {
        case SOC_BLK_EPIC:
            ctype = SOC_CTR_TYPE_FE;
            break;
        case SOC_BLK_GPIC:
        case SOC_BLK_GPORT:
        case SOC_BLK_QGPORT:
        case SOC_BLK_SPORT:
            ctype = SOC_CTR_TYPE_GE;
            break;
        case SOC_BLK_XQPORT:
        case SOC_BLK_XGPORT:
            if (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port)) {
                ctype = SOC_CTR_TYPE_XE;
            } else {
                ctype = SOC_CTR_TYPE_GE;
            }
            break;
        case SOC_BLK_IPIC:
        case SOC_BLK_HPIC:
            ctype = SOC_CTR_TYPE_HG;
            break;
        case SOC_BLK_XPIC:
        case SOC_BLK_XPORT:
        case SOC_BLK_GXPORT:
        case SOC_BLK_GXFPPORT:
        case SOC_BLK_XLPORT:
        case SOC_BLK_XLPORTB0:
        case SOC_BLK_XTPORT:
        case SOC_BLK_CLPORT:
        case SOC_BLK_MXQPORT:
        case SOC_BLK_XWPORT:
            if (SOC_IS_GREYHOUND(unit) && blktype == SOC_BLK_GXPORT){
                if (SOC_BLOCK_PORT(unit, blk) < 0){
                    ctype = SOC_CTR_TYPE_XE;
                } else {
                    /* Greyhound GE port with SOC_BLK_GXPORT case*/
                    ctype = SOC_CTR_TYPE_GE;
                }
            } else {
                ctype = SOC_CTR_TYPE_XE;
            }
            break;
        case SOC_BLK_CLG2PORT:
            ctype = SOC_CTR_TYPE_CE;
            break;
        case SOC_BLK_CLP:
        case SOC_BLK_XLP:
            if (IS_IL_PORT(unit, port)) {
            } else if (IS_CE_PORT(unit, port)) {
                ctype = SOC_CTR_TYPE_CE;
            } else if (IS_QSGMII_PORT(unit,port)) {
                ctype = SOC_CTR_TYPE_GE;
            } else {
                ctype = SOC_CTR_TYPE_XE;
            }
            break;
        case SOC_BLK_MXQ:
            ctype = SOC_CTR_TYPE_GE;
            break;
        case SOC_BLK_CMIC:
            if (soc_feature(unit, soc_feature_cpuport_stat_dma)) {
                ctype = SOC_CTR_TYPE_CPU;
                break;
            }
        case SOC_BLK_CPIC:
        default:
            break;
        }

    return ctype;
}
#endif

#if defined BCM_DNX_SUPPORT

/**
 * brief - Test counter rw.
 */
static int
test_dnx_ctr_rw(
    int unit,
    int port,
    int counter_id)
{
    int rv;
    uint64 val, incr;
    uint64 mask, pattern,tmp;
    int length;
    dbal_fields_e field_id;
    uint32 clear_on_read;
    int loop;
    int res = 0;                /* test result - assume pass */

    rv = dnx_mib_counter_id_to_dbal_field_id_get(unit, port, counter_id, &field_id, &clear_on_read);
    if (rv != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - counter type %d is not supported for port %d"), counter_id, port));
        res = 1;
    }

    if (clear_on_read == TRUE)
    {
        return res;
    }

    rv = dnx_mib_counter_length_get(unit, counter_id, &length);
    if (rv != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - get length failed for counter type %d"), counter_id));
        res = 1;
    }

    COMPILER_64_MASK_CREATE(mask, length, 0);
    COMPILER_64_SET(incr, 0x55555555, 0x55555555);
    /*
     * The following loop sets pattern to:
     *
     *	0x00000000
     *	0x55555555
     *	0xaaaaaaaa
     *	0xffffffff
     */

    COMPILER_64_ZERO(pattern);
    for (loop = 0; loop < 4; loop++)
    {
        tmp = pattern;

        rv = dnx_mib_nif_eth_stat_dbal_set(unit, port, field_id, tmp);
        if (rv != 0)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - set value %08x%08x for counter type %d failed"),
                                      COMPILER_64_HI(tmp),COMPILER_64_LO(tmp), counter_id));
            res = 1;
        }
        COMPILER_64_ZERO(val);
        rv = dnx_mib_nif_eth_stat_dbal_get(unit, port, field_id, &val);
        if (rv != 0)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - get value failed for counter type %d"), counter_id));
            res = 1;
        }

	    tmp = val;
	    COMPILER_64_XOR(tmp, pattern);
	    COMPILER_64_AND(tmp, mask);

	    if (!COMPILER_64_IS_ZERO(tmp))
	    {
            COMPILER_64_AND(pattern, mask);
            COMPILER_64_AND(val, mask);
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                                        "Counter type %d miscompare:\n"
                                        "    wrote     0x%08x%08x\n"
                                        "    read      0x%08x%08x\n"),
                                        counter_id,
                                        COMPILER_64_HI(pattern),
                                        COMPILER_64_LO(pattern),
                                        COMPILER_64_HI(val),
                                        COMPILER_64_LO(val)));
            res = 1;
	    }

        COMPILER_64_ADD_64(pattern, incr);
    }

    return res;
}

int ctr_dnx_test_rw(int unit, args_t *a, void *pa)
{
    bcm_pbmp_t pbmp;
    int port;
    int counter_id;
    int rv;
    int res = 0;                /* test result - assume pass */

    BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
    BCM_PBMP_REMOVE(pbmp, PBMP_SFI_ALL(unit));
    BCM_PBMP_REMOVE(pbmp, PBMP_IL_ALL(unit));
    /*
     * Get first nif eth port
     */
    _SHR_PBMP_FIRST(pbmp, port);

    /*
     * Test Counter read/wirte
     */
    cli_out("Counter read/wirte test - Start\n");

    if (port == -1)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - Expecting one eth port at least\n")));
        res = 1;
    }
    else
    {
        for (counter_id = dnx_mib_counter_nif_eth_r64; counter_id <= dnx_mib_counter_nif_eth_tbyt; counter_id++)
        {
            rv = test_dnx_ctr_rw(unit, port, counter_id);
            if (rv != 0)
            {
                res = 1;  /*set failed flag*/
            }
        }
    }
    cli_out("Counter read/wirte test - Done\n");
    /*
     * Final Result
     */
    if (res == 0)
    {
        cli_out("Counter read/wirte test- Pass\n");
    }
    return res;

}

/**
 * brief - Test counter width.
 */
static int
test_dnx_ctr_width(
    int unit,
    int port,
    int counter_id)
{
    int rv;
    int res = 0;                /* test result - assume pass */
    uint64 val;
    dbal_fields_e field_id;
    uint32 clear_on_read;
    int defined_width, actual_width;

    COMPILER_64_SET(val, 0xffffffff, 0xffffffff);

    rv = dnx_mib_counter_id_to_dbal_field_id_get(unit, port, counter_id, &field_id, &clear_on_read);
    if (rv != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - counter type %d is not supported for port %d"), counter_id, port));
        res = 1;
    }

    if (clear_on_read == TRUE)
    {
        return res;
    }

    rv = dnx_mib_nif_eth_stat_dbal_set(unit, port, field_id, val);
    if (rv != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - set value %08x%08x for counter type %d for port %d failed"),
                                  COMPILER_64_HI(val),COMPILER_64_LO(val), counter_id, port));
        res = 1;
    }

    COMPILER_64_ZERO(val);
    rv = dnx_mib_nif_eth_stat_dbal_get(unit, port, field_id, &val);
    if (rv != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - get value failed for counter type %d for port %d"), counter_id, port));
        res = 1;
    }

    rv = dnx_mib_counter_length_get(unit, counter_id, &defined_width);
    if (rv != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - get length failed for counter type %d for port %d"), counter_id, port));
        res = 1;
    }

    actual_width = (_shr_popcount(COMPILER_64_LO(val)) + _shr_popcount(COMPILER_64_HI(val)));
    if (defined_width != actual_width)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                                    "Counter type %d for port %d width mismatch"
                                    "defined width: %d bits, actual: %d bits\n"),
                                    counter_id, port, defined_width, actual_width));
        res = 1;
    }

    return res;
}

int ctr_dnx_test_width(int unit, args_t *a, void *pa)
{

    bcm_pbmp_t pbmp;
    int port;
    int counter_id;
    int rv;
    int res = 0;                /* test result - assume pass */

    BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
    BCM_PBMP_REMOVE(pbmp, PBMP_SFI_ALL(unit));
    BCM_PBMP_REMOVE(pbmp, PBMP_IL_ALL(unit));
    /*
     * Get first nif eth port
     */
    _SHR_PBMP_FIRST(pbmp, port);

    /*
     * Test Counter widths
     */
    cli_out("Counter widths test - Start\n");

    if (port == -1)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - Expecting one eth port at least\n")));
        res = 1;
    }
    else
    {
        for (counter_id = dnx_mib_counter_nif_eth_r64; counter_id <= dnx_mib_counter_nif_eth_tbyt; counter_id++)
        {
            rv = test_dnx_ctr_width(unit, port, counter_id);
            if (rv != 0)
            {
                res = 1;  /*set failed flag*/
            }
        }
    }
    cli_out("Counter widths test - Done\n");
    /*
     * Final Result
     */
    if (res == 0)
    {
        cli_out("Counter widths test- Pass\n");
    }
    return res;
}

#endif

/*
 * Given a counter type, return a port that can be used for it.
 * Actually returns the first port of the appropriate type
 */
static soc_port_t
find_ctype_port(int unit, soc_ctr_type_t ctype)
{
    switch (ctype) {
    case SOC_CTR_TYPE_FE:
	if (SOC_PORT_NUM(unit, fe) < 1) {
	    return -1;
	}
	return SOC_PORT(unit, fe, 0);
    case SOC_CTR_TYPE_GE:
    case SOC_CTR_TYPE_GFE:
	if (SOC_PORT_NUM(unit, ge) < 1) {
	    return -1;
	}
#ifdef BCM_GREYHOUND_SUPPORT
       if (SOC_IS_GREYHOUND(unit) && IS_XL_PORT(unit, SOC_PORT(unit, ge, 0))) {
           return -1;
       }
#endif /* BCM_GREYHOUND_SUPPORT */
#ifdef BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit)) {
        int i, port;
        for (i=0;i<SOC_PORT_NUM(unit, ge);i++) {
            port = SOC_PORT(unit, ge, i);
            if (IS_QSGMII_PORT(unit, port)) {
                return port;
            }
        }
        return -1;
    }
#endif /* BCM_FIRELIGHT_SUPPORT */
	return SOC_PORT(unit, ge, 0);
    case SOC_CTR_TYPE_XE:
	if (SOC_PORT_NUM(unit, xe) < 1) {
	    return -1;
	}
#if defined BCM_PETRA_SUPPORT
    /*For QUX,XE ports in Viper core use ge counter type*/
    if (SOC_IS_QUX(unit)) {
        int i, port;
        for (i=0;i<SOC_PORT_NUM(unit, xe);i++) {
            port = SOC_PORT(unit, xe, i);
            if (SOC_CTR_TYPE_XE == get_port_ctype(unit,port)) {
                return port;
            }
        }
        if (SOC_PORT_NUM(unit, xe) == i) {
            return -1;
        }
    } else
#endif
#if defined BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        int i, port;
        for (i=0;i<SOC_PORT_NUM(unit, xe);i++) {
            port = SOC_PORT(unit, xe, i);
            if (IS_XL_PORT(unit, port)) {
                return port;
            }
        }
        if (SOC_PORT_NUM(unit, xe) == i) {
            return -1;
        }
    } else
#endif
#if defined BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit)) {
        int i, port;
        for (i=0;i<SOC_PORT_NUM(unit, hg);i++) {
            port = SOC_PORT(unit, hg, i);
            if (IS_XL_PORT(unit, port)) {
                return port;
            }
        }
        for (i=0;i<SOC_PORT_NUM(unit, xe);i++) {
            port = SOC_PORT(unit, xe, i);
            if (IS_XL_PORT(unit, port)) {
                return port;
            }
        }
        for (i=0;i<SOC_PORT_NUM(unit, ge);i++) {
            port = SOC_PORT(unit, ge, i);
            if (IS_XL_PORT(unit, port)) {
                return port;
            }
        }
        return -1;
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        return SOC_PORT(unit, xe, 0);
    }
    case SOC_CTR_TYPE_HG:
	if (SOC_PORT_NUM(unit, hg) < 1) {
	    return -1;
	}
#if defined BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit)) {
        return -1;
    }
#endif
#if defined BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        int i, port;
        bcm_port_t phy_port;
        for (i=0;i<SOC_PORT_NUM(unit, hg);i++) {
            port = SOC_PORT(unit, hg, i);
            if (soc_feature(unit, soc_feature_logical_port_num)) {
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
            } else {
                phy_port = port;
            }
            if (SOC_BLOCK_IS_CMP(unit,
                    SOC_PORT_BLOCK(unit, phy_port), SOC_BLK_XLPORT)) {
                return port;
            }
        }
        return -1;
    }
#endif
	return SOC_PORT(unit, hg, 0);
    case SOC_CTR_TYPE_CE:
#if defined BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        int i, port;
        bcm_port_t phy_port;
        for (i=0;i<SOC_PORT_NUM(unit, xe);i++) {
            port = SOC_PORT(unit, xe, i);
            if (IS_CL_PORT(unit, port)) {
                return port;
            }
        }
        for (i=0;i<SOC_PORT_NUM(unit, hg);i++) {
            port = SOC_PORT(unit, hg, i);
            if (soc_feature(unit, soc_feature_logical_port_num)) {
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
            } else {
                phy_port = port;
            }
            if (SOC_BLOCK_IS_CMP(unit,
                    SOC_PORT_BLOCK(unit, phy_port), SOC_BLK_CLPORT)) {
                return port;
            }
        }
        return -1;
    } else
#endif
#if defined BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit)) {
        int i, port;
        for (i=0;i<SOC_PORT_NUM(unit, hg);i++) {
            port = SOC_PORT(unit, hg, i);
            if (IS_CL_PORT(unit, port)) {
                return port;
            }
        }
        for (i=0;i<SOC_PORT_NUM(unit, ce);i++) {
            port = SOC_PORT(unit, ce, i);
            if (IS_CL_PORT(unit, port)) {
                return port;
            }
        }
        for (i=0;i<SOC_PORT_NUM(unit, xe);i++) {
            port = SOC_PORT(unit, xe, i);
            if (IS_CL_PORT(unit, port)) {
                return port;
            }
        }
        for (i=0;i<SOC_PORT_NUM(unit, ge);i++) {
            port = SOC_PORT(unit, ge, i);
            if (IS_CL_PORT(unit, port)) {
                return port;
            }
        }
        return -1;
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        return -1;
    }
    default:
	return -1;
    }
}

/*
 * Routine to test the width of a counter in bits, as verified against
 * the width listed in the regsfile, for counters up to 32 bits.
 */

static int
test_width32(int unit, soc_reg_t ctr_reg, soc_port_t port)
{
    uint32		val = 0;
    char		*name;
    int			regsfile_width, actual_width;
#if  defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    int			r;
#endif  /* BCM_ESW_SUPPORT */

    name = NULL;
    regsfile_width = SOC_REG_INFO(unit, ctr_reg).fields[0].len;
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    if (soc_feature(unit, soc_feature_counter_parity) ||
        soc_reg_field_valid(unit, ctr_reg, PARITYf) ) {
        int i = 0;
        while((SOC_REG_INFO(unit, ctr_reg).fields + i) != NULL) { 
            if (SOC_REG_INFO(unit, ctr_reg).fields[i].field == COUNTf) {
                regsfile_width = SOC_REG_INFO(unit, ctr_reg).fields[i].len;
                break;
            }
            i++;
        }
    }
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    name = SOC_REG_NAME(unit, ctr_reg);

    if (soc_feature(unit, soc_feature_prime_ctr_writes)) {
        r = soc_counter_set32(unit, port, ctr_reg, 0, ~0);
    } else {
        r = soc_reg32_set(unit, ctr_reg, port, 0, ~0);
    }

    if ((r < 0) ||
        (r = soc_reg32_get(unit, ctr_reg, port, 0, &val)) < 0) {
        test_error(unit,
                   "Can't access counter %s.%s: %s\n",
                   name, SOC_PORT_NAME(unit, port), soc_errmsg(r));
        return -1;
    }
    if (soc_feature(unit, soc_feature_counter_parity) ||
        soc_reg_field_valid(unit, ctr_reg, PARITYf) ) {
        int bits;
        bits = soc_reg_field_length(unit, ctr_reg, COUNTf);
        val &= ((bits == 32 ? 0 : 1 << bits) - 1);
    }
#endif

    actual_width = _shr_popcount(val);

    if (regsfile_width != actual_width) {
	test_error(unit,
		   "Counter %s.%s width mismatch: "
		   "regsfile: %d bits, actual: %d bits\n",
		   name, SOC_PORT_NAME(unit, port), regsfile_width,
                   actual_width);
	return -1;
    }
    return 0;
}

/*
 * Routine to test the width of a counter in bits, as verified against
 * the width listed in the regsfile, for counters over 32 bits.
 */

static int
test_width64(int unit, soc_reg_t ctr_reg, soc_port_t port)
{
    uint64		val;
    char		*name;
    int			regsfile_width, actual_width;
#if  defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    int			r;
#endif

    name = NULL;
    regsfile_width = SOC_REG_INFO(unit, ctr_reg).fields[0].len;
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    if (soc_feature(unit, soc_feature_counter_parity) ||
        soc_reg_field_valid(unit, ctr_reg, PARITYf)) {
        int i = 0;
        while((SOC_REG_INFO(unit, ctr_reg).fields + i) != NULL) { 
            if (SOC_REG_INFO(unit, ctr_reg).fields[i].field == COUNTf) {
                regsfile_width = SOC_REG_INFO(unit, ctr_reg).fields[i].len;
                break;
            }
            i++;
        }
    }
#endif

    COMPILER_64_SET(val, 0xffffffff, 0xffffffff);

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    name = SOC_REG_NAME(unit, ctr_reg);
    if ((r = soc_reg64_set(unit, ctr_reg, port, 0, val)) < 0 ||
	(r = soc_reg64_get(unit, ctr_reg, port, 0, &val)) < 0) {
	test_error(unit,
		   "Can't access counter %s.%s: %s\n",
		   name, SOC_PORT_NAME(unit, port), soc_errmsg(r));
	return -1;
    }
    val = soc_reg64_field_get(unit, ctr_reg, val,
                              SOC_REG_INFO(unit, ctr_reg).fields[0].field);
#endif
    actual_width = (_shr_popcount(COMPILER_64_LO(val)) +
		    _shr_popcount(COMPILER_64_HI(val)));

    if (regsfile_width != actual_width) {
	test_error(unit,
		   "Counter %s.%s width mismatch: "
		   "regsfile: %d bits, actual: %d bits\n",
		   name, SOC_PORT_NAME(unit, port),
                   regsfile_width, actual_width);
	return -1;
    }
    return 0;
}

int
ctr_test_width(int unit, args_t *a, void *pa)
/*
 * Function: 	ctr_test_width
 * Purpose:	Test the widths of all counters as compared to regsfile
 * Parameters:	u - unit #.
 *		a - pointer to arguments.
 *		pa - ignored cookie.
 * Returns:	0
 */
{
    int			i;
    soc_ctr_type_t      ctype;
    soc_reg_t           reg;
    soc_port_t		port;

    COMPILER_REFERENCE(a);
    COMPILER_REFERENCE(pa);

#if defined BCM_DNX_SUPPORT
    return ctr_dnx_test_width(unit, a, pa);
#endif

#ifdef BCM_SHADOW_SUPPORT
    /* coverity[unreachable] */
    if (SOC_IS_SHADOW(unit)) {
        if (soc_reg_field32_modify(unit, ISW2_HW_CONTROLr, REG_PORT_ANY,
                                   XGS_COUNTER_COMPAT_MODEf, 1) < 0) {
            return -1;
        }
    }
#endif
    for (ctype = SOC_CTR_TYPE_FE; ctype < SOC_CTR_NUM_TYPES; ctype++) {
        if (SOC_HAS_CTR_TYPE(unit, ctype)) {
	    port = find_ctype_port(unit, ctype);
        if (port < 0) {
		continue;
	    }
            for (i = 0; i < SOC_CTR_MAP_SIZE(unit, ctype); i++) {
                reg = SOC_CTR_TO_REG(unit, ctype, i);
                if (!SOC_COUNTER_INVALID(unit, reg) && 
                      !(SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_RO)) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
                    soc_regaddrinfo_t   ainfo;
                    uint64		mask;
                    uint32              addr;
                    int                 block;
                    uint8               at;

                    if (!SOC_REG_PORT_VALID(unit, reg, port)) {
                        continue;
                    }
                    addr = soc_reg_addr_get(unit, reg, port, 0,
                                            SOC_REG_ADDR_OPTION_NONE,
                                            &block, &at);
                    soc_regaddrinfo_extended_get(unit, &ainfo, block, at, addr);
                    if (SOC_COUNTER_INVALID(unit, ainfo.reg)) {
                        continue;
                    }
                    if (reg_mask_subset(unit, &ainfo, &mask)) {
                        /* Unimplemented in HW, skip */
                        continue;
                    }
#endif

    	    if (SOC_REG_IS_COUNTER_TABLE(unit, reg) ||
                SOC_REG_IS_64(unit, reg)) {
			test_width64(unit, reg, port);
		    } else {
			test_width32(unit, reg, port);
		    }
                }
            }
        }
    }
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        if (soc_reg_field32_modify(unit, ISW2_HW_CONTROLr, REG_PORT_ANY,
                                   XGS_COUNTER_COMPAT_MODEf, 0) < 0) {
            return -1;
        }
    }
#endif

    return 0;
}

/*
 * Routine to write a counter with several patterns and compare.
 * Works on counters <= 32 bits.
 */

static int
test_rw32(int unit, soc_reg_t ctr_reg, soc_port_t port)
{
    char              *name;
    volatile uint32   pattern;  /* SiByte1250 workaround */
    uint32            mask, val;
#if  defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    uint32            pattern_mask;
    int               r;
#endif
    int               bits, patno;

    name = NULL;
    bits = SOC_REG_INFO(unit, ctr_reg).fields[0].len;
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    if (soc_feature(unit, soc_feature_counter_parity) ||
        soc_reg_field_valid(unit, ctr_reg, PARITYf)) {
        int i = 0;
        while((SOC_REG_INFO(unit, ctr_reg).fields + i) != NULL) { 
            if (SOC_REG_INFO(unit, ctr_reg).fields[i].field == COUNTf) {
                bits = SOC_REG_INFO(unit, ctr_reg).fields[i].len;
                break;
            }
            i++;
        }
    }
#endif
    mask = (bits == 32 ? 0 : 1 << bits) - 1;

    /*
     * The following loop sets pattern to:
     *
     *	0x00000000
     *	0x55555555
     *	0xaaaaaaaa
     *	0xffffffff
     */

    pattern = 0x00000000;
    val = 0;

    for (patno = 0; patno < 4; patno++) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
        name = SOC_REG_NAME(unit, ctr_reg);
        pattern_mask = pattern & mask;
        if (soc_feature(unit, soc_feature_prime_ctr_writes)) {
            r = soc_counter_set32(unit, port, ctr_reg, 0, pattern_mask);
        } else {
            r = soc_reg32_set(unit, ctr_reg, port, 0, pattern_mask);
        }
        if (r < 0) {
            val = pattern_mask;
	    test_error(unit,
		       "Can't write counter %s.%s: %s\n",
		       name, SOC_PORT_NAME(unit, port), soc_errmsg(r));
        } else if ((r = soc_reg32_get(unit, ctr_reg, port, 0, &val)) < 0) {
	    test_error(unit,
		       "Can't access counter %s.%s: %s\n",
		       name, SOC_PORT_NAME(unit, port), soc_errmsg(r));
        }
#endif
        if (((val ^ pattern) & mask) != 0) {
	    uint32 reread = 0;

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
	   (void) soc_reg32_get(unit, ctr_reg, port, 0, &reread);
#endif

	    test_error(unit,
		       "Counter %s.%s miscompare: \n"
                       "    wrote     0x%08x\n"
                       "    read      0x%08x\n"
                       "    re-read   0x%08x\n",
		       name, SOC_PORT_NAME(unit, port),
		       pattern & mask,
		       val & mask,
		       reread & mask);
	}

	pattern += 0x55555555;
    }
    return 0;
}

/*
 * Routine to write a counter with several patterns and compare.
 * Works on counters over 32 bits.
 */

static int
test_rw64(int unit, soc_reg_t ctr_reg, soc_port_t port)
{
    char		*name;
    uint64		mask, pattern, tmp;
    uint64		val, incr;
    int			bits, patno;
#if  defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    int                 r;
#endif /* BCM_ESW_SUPPORT */

    name = NULL;
    bits = SOC_REG_INFO(unit, ctr_reg).fields[0].len;
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    if (soc_feature(unit, soc_feature_counter_parity) ||
        soc_reg_field_valid(unit, ctr_reg, PARITYf) ) {
        int i = 0;
        while((SOC_REG_INFO(unit, ctr_reg).fields + i) != NULL) { 
            if (SOC_REG_INFO(unit, ctr_reg).fields[i].field == COUNTf) {
                bits = SOC_REG_INFO(unit, ctr_reg).fields[i].len;
                break;
            }
            i++;
        }
    }
#endif

    COMPILER_64_MASK_CREATE(mask, bits, 0);
    COMPILER_64_SET(incr, 0x55555555, 0x55555555);
    COMPILER_64_SET(val, 0x0, 0x0);

    /*
     * The following loop sets pattern to:
     *
     *	0x00000000
     *	0x55555555
     *	0xaaaaaaaa
     *	0xffffffff
     */

    COMPILER_64_ZERO(pattern);

    for (patno = 0; patno < 4; patno++) {
	tmp = pattern;
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
        name = SOC_REG_NAME(unit, ctr_reg);
	if ((r = soc_reg64_set(unit, ctr_reg, port, 0, tmp)) < 0) {
            val = tmp;
	    test_error(unit,
		       "Can't write counter %s.%s: %s\n",
		       name, SOC_PORT_NAME(unit, port), soc_errmsg(r));
	} else if ((r = soc_reg64_get(unit, ctr_reg, port, 0, &val)) < 0) {
	    test_error(unit,
		       "Can't access counter %s.%s: %s\n",
		       name, SOC_PORT_NAME(unit, port), soc_errmsg(r));
        }
#endif
	    tmp = val;
	    COMPILER_64_XOR(tmp, pattern);
	    COMPILER_64_AND(tmp, mask);

	    if (!COMPILER_64_IS_ZERO(tmp)) {
		uint64 reread;

                COMPILER_64_ZERO(reread);

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
		(void) soc_reg64_get(unit, ctr_reg, port, 0, &reread);
#endif

		COMPILER_64_AND(pattern, mask);
		COMPILER_64_AND(val, mask);

		test_error(unit,
			   "Counter %s.%s miscompare:\n"
			   "    wrote     0x%08x%08x\n"
                           "    read      0x%08x%08x\n"
			   "    re-read   0x%08x%08x\n",
			   name, SOC_PORT_NAME(unit, port),
			   COMPILER_64_HI(pattern),
			   COMPILER_64_LO(pattern),
			   COMPILER_64_HI(val),
			   COMPILER_64_LO(val),
			   COMPILER_64_HI(reread),
			   COMPILER_64_LO(reread));
	    }

	COMPILER_64_ADD_64(pattern, incr);
    }
    return 0;
}

int
ctr_test_rw(int unit, args_t *a, void *pa)
/*
 * Function: 	ctr_test_rw
 * Purpose:	Run test values through all counter registers
 * Parameters:	u - unit #.
 *		a - pointer to arguments.
 *		pa - ignored cookie.
 * Returns:	0
 */
{
    int			i;
    soc_reg_t           reg;
    soc_ctr_type_t      ctype;
    soc_port_t		port;

    COMPILER_REFERENCE(a);
    COMPILER_REFERENCE(pa);

#if defined BCM_DNX_SUPPORT
    return ctr_dnx_test_rw(unit, a, pa);
#endif

#ifdef BCM_SHADOW_SUPPORT
    /* coverity[unreachable] */
    if (SOC_IS_SHADOW(unit)) {
        if (soc_reg_field32_modify(unit, ISW2_HW_CONTROLr, REG_PORT_ANY,
                                   XGS_COUNTER_COMPAT_MODEf, 1) < 0) {
            return -1;
        }
    }
#endif

    for (ctype = SOC_CTR_TYPE_FE; ctype < SOC_CTR_NUM_TYPES; ctype++) {
        if (SOC_HAS_CTR_TYPE(unit, ctype)) {
	    port = find_ctype_port(unit, ctype);
	    if (port < 0) {
		continue;
	    }
            for (i = 0; i < SOC_CTR_MAP_SIZE(unit, ctype); i++) {
                reg = SOC_CTR_TO_REG(unit, ctype, i);
                if (!SOC_COUNTER_INVALID(unit, reg) && 
                      !(SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_RO)) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
                    soc_regaddrinfo_t   ainfo;
                    uint64		mask;
                    uint32              addr;
                    int                 block;
                    uint8               at;
                    
                    if (!SOC_REG_PORT_VALID(unit, reg, port)) {
                        continue;
                    }
                    addr = soc_reg_addr_get(unit, reg, port, 0,
                                            SOC_REG_ADDR_OPTION_NONE,
                                            &block, &at);
                    soc_regaddrinfo_extended_get(unit, &ainfo, block, at, addr);
                    if (SOC_COUNTER_INVALID(unit, ainfo.reg)) {
                        continue;
                    }
                    if (reg_mask_subset(unit, &ainfo, &mask)) {
                        /* Unimplemented in HW, skip */
                        continue;
                    }
#endif
    	    if (SOC_REG_IS_COUNTER_TABLE(unit, reg) ||
                SOC_REG_IS_64(unit, reg)) {
			test_rw64(unit, reg, port);
		    } else {
			test_rw32(unit, reg, port);
		    }
                }
            }
        }
    }
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        if (soc_reg_field32_modify(unit, ISW2_HW_CONTROLr, REG_PORT_ANY,
                                   XGS_COUNTER_COMPAT_MODEf, 0) < 0) {
            return -1;
        }
    }
#endif

    return 0;
}
