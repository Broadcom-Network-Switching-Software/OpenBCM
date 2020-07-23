/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Rate - Broadcom StrataSwitch Rate Limiting API.
 */

#include <soc/drv.h>

#include <soc/mem.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/drv.h>

#include <bcm/rate.h>
#include <bcm/error.h>

#include <bcm_int/esw_dispatch.h> 

#include <bcm_int/esw/rate.h> 

#define TOKEN_BUCKET_SIZE       64      /* Size of one token bucket in kbits */
#define METERING_TOKEN_MAX      0x7FFFF  /* Max number of supported tokens */
#define METER_BUCKETSIZE_MAX    12
#define BITSINBYTE              8
#define KBYTES                  (1024)
#define MBYTES                  (1024) * KBYTES
#define KBITS_IN_API            1000

#ifdef BCM_TRX_SUPPORT
#define _BCM_TRX_RATE_BCAST_INDEX         0
#define _BCM_TRX_RATE_MCAST_INDEX         1
#define _BCM_TRX_RATE_UNKNOWN_MCAST_INDEX 2
#define _BCM_TRX_RATE_DLF_INDEX           3

#define _BCM_TRX_RATE_BCAST_INDEX_GET(unit, index) \
{ \
    if (soc_feature(unit, soc_feature_configurable_storm_control_meter_mapping)) {\
        index = soc_property_get(unit, spn_BCM_RATE_BCAST_INDEX, _BCM_TRX_RATE_BCAST_INDEX); \
        if (index > 3) { \
            index = _BCM_TRX_RATE_BCAST_INDEX; \
        } \
    } else { \
        index = _BCM_TRX_RATE_BCAST_INDEX; \
    } \
} \

#define _BCM_TRX_RATE_MCAST_INDEX_GET(unit, index) \
{ \
    if (soc_feature(unit, soc_feature_configurable_storm_control_meter_mapping)) {\
        index = soc_property_get(unit, spn_BCM_RATE_MCAST_INDEX, _BCM_TRX_RATE_MCAST_INDEX); \
        if (index > 3) { \
            index = _BCM_TRX_RATE_MCAST_INDEX; \
        } \
    } else { \
        index = _BCM_TRX_RATE_MCAST_INDEX; \
    } \
} \

#define _BCM_TRX_RATE_UNKNOWN_MCAST_INDEX_GET(unit, index) \
{ \
    if (soc_feature(unit, soc_feature_configurable_storm_control_meter_mapping)) {\
        index = soc_property_get(unit, spn_BCM_RATE_UNKNOWN_MCAST_INDEX, _BCM_TRX_RATE_UNKNOWN_MCAST_INDEX); \
        if (index > 3) { \
            index = _BCM_TRX_RATE_UNKNOWN_MCAST_INDEX; \
        } \
    } else { \
        index = _BCM_TRX_RATE_UNKNOWN_MCAST_INDEX; \
    } \
} \

#define _BCM_TRX_RATE_DLF_INDEX_GET(unit, index) \
{ \
    if (soc_feature(unit, soc_feature_configurable_storm_control_meter_mapping)) {\
        index = soc_property_get(unit, spn_BCM_RATE_DLF_INDEX, _BCM_TRX_RATE_DLF_INDEX); \
        if (index > 3) { \
            index = _BCM_TRX_RATE_DLF_INDEX; \
        } \
    } else { \
        index = _BCM_TRX_RATE_DLF_INDEX; \
    } \
} \

#define _BCM_TRX_RATE_BYTE_MODE         1
#define _BCM_TRX_RATE_PKT_MODE          0

#define _BCM_TRX_REFRESH_TO_RPS         64      /* see inline comments */
#define _BCM_TRX_RATE_MAX_REFRESH_COUNT 0x7ffff
#define _BCM_TRX_RATE_DFT_PKT_BUCKETSZ  7       /* max allow */

#define _BCM_TR3_REFRESH_TO_RPS         64
#define _BCM_TR3_RATE_MAX_REFRESH_COUNT 0x1fffff

/* Definition for Hurricane3 :
 * - FP_STORM_CONTROL_METERSm.REFRESHCOUNTf extended to 20 bits.
 *   (to support max port speed upto 42G)
 */
#define _BCM_HR3_RATE_MAX_REFRESH_COUNT 0xfffff 
#endif /* BCM_TRX_SUPPORTED */

#if defined(BCM_BANDWIDTH_RATE_METER)

static uint32 kbits_2_bucket_size[METER_BUCKETSIZE_MAX + 1] = {
      4 * KBYTES * BITSINBYTE / KBITS_IN_API,   /* 4 Kbytes bucket size */
      8 * KBYTES * BITSINBYTE / KBITS_IN_API,   /* 8 Kbytes bucket size */
     16 * KBYTES * BITSINBYTE / KBITS_IN_API,   /* 16 Kbytes bucket size */
     32 * KBYTES * BITSINBYTE / KBITS_IN_API,   /* 32 Kbytes bucket size */
     64 * KBYTES * BITSINBYTE / KBITS_IN_API,   /* 64 Kbytes bucket size */
    128 * KBYTES * BITSINBYTE / KBITS_IN_API,   /* 128 Kbytes bucket size */
    256 * KBYTES * BITSINBYTE / KBITS_IN_API,   /* 256 Kbytes bucket size */
    512 * KBYTES * BITSINBYTE / KBITS_IN_API,   /* 512 Kbytes bucket size */
      1 * MBYTES * BITSINBYTE / KBITS_IN_API,   /* 1 Mbytes bucket size */
      2 * MBYTES * BITSINBYTE / KBITS_IN_API,   /* 2 Mbytes bucket size */
      4 * MBYTES * BITSINBYTE / KBITS_IN_API,   /* 4 Mbytes bucket size */
      8 * MBYTES * BITSINBYTE / KBITS_IN_API,   /* 8 Mbytes bucket size */
     16 * MBYTES * BITSINBYTE / KBITS_IN_API    /* 16 Mbytes bucket size */
};

/*
 * Function:
 *      _bcm_kbits_to_bucket_size
 * Purpose:
 *      Translate kbits burst value into bucket size
 * Parameters:
 *      kbits_burst - Value to translate
 *      
 * Returns:
 *      Programable value 
 */
STATIC int 
_bcm_kbits_to_bucket_size(uint32 kbits_burst)
{
    int bckt_sz; 

    for (bckt_sz = 0; bckt_sz <= METER_BUCKETSIZE_MAX; bckt_sz++) {
        if (kbits_burst <= kbits_2_bucket_size[bckt_sz]) {
            break;
        }
    }
    if (bckt_sz > METER_BUCKETSIZE_MAX) {
        bckt_sz = METER_BUCKETSIZE_MAX;
    }

    return bckt_sz;
}

/*
 * Function:
 *      _bcm_bucket_size_to_kbits
 * Purpose:
 *      Translate bucket_size into kbits burst
 * Parameters:
 *      kbits_burst - Value to translate
 *      
 * Returns:
 *      Programable value 
 */
STATIC uint32
_bcm_bucket_size_to_kbits(uint32 bucket_size)
{
    assert(bucket_size <= METER_BUCKETSIZE_MAX);

    return(kbits_2_bucket_size[bucket_size]);
}
#endif /* BCM_BANDWIDTH_RATE_METER */
/*
 * Function:
 *      _bcm_rate_bw_set
 * Purpose:
 *      Helper function to bcm_esw_rate_bandwidth_set
 * Parameters:
 *      unit        - BCM unit
 *      port        - port number
 *      mem         - Memory to program
 *      refresh -   value to configure refresh count with
 *      bucket_size - value to configure bucket size with
 *      
 * Returns:
 *      Programable value 
 */
#if defined(BCM_BANDWIDTH_RATE_METER)
STATIC int
_bcm_rate_bw_set(int unit, int port, uint32 mem, uint32 refresh, 
                 int bucket_size)
{
    fp_sc_meter_table_entry_t   entryMeter;

    soc_mem_field32_set(unit, mem, &entryMeter, BUCKETSIZEf, bucket_size);
    soc_mem_field32_set(unit, mem, &entryMeter, REFRESHCOUNTf, refresh);
    soc_mem_field32_set(unit, mem, &entryMeter, BUCKETCOUNTf, 0);

    soc_mem_write(unit, mem, MEM_BLOCK_ALL,port, &entryMeter);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_rate_bw_get
 * Purpose:
 *      Helper function to bcm_esw_rate_bandwidth_get
 * Parameters:
 *      unit        - BCM unit
 *      port        - BCM port 
 *      mem         - Memory to read 
 *      kbits_sec   - Rate in kilobits (1000 bits) per second.
 *      kbits_burst - Maximum burst size in kilobits(1000 bits)
 *      
 * Returns:
 *      Programable value 
 */
STATIC int
_bcm_rate_bw_get(int unit, int port, uint32 mem, 
                 uint32 *kbits_sec, uint32 *kbits_burst) 
{

    fp_sc_meter_table_entry_t   entryMeter;
    uint32                      bucket_size;

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, &entryMeter));

    *kbits_sec = TOKEN_BUCKET_SIZE * 
        soc_mem_field32_get(unit, mem, &entryMeter, REFRESHCOUNTf);
    bucket_size = soc_mem_field32_get(unit, mem, &entryMeter, BUCKETSIZEf);
    *kbits_burst = _bcm_bucket_size_to_kbits(bucket_size);

    return BCM_E_NONE;
}
#endif

#ifdef BCM_TRX_SUPPORT
/*  
 * Like most other switching chips, Triumph and Scorpion provide meters 
 * to control Brocast/Multicast/DLF packet storm. And similar to Raptor, 
 * they allow the byte-mode or packet-mode meters. However, unlike Raptor,
 * both modes in Triumph and Scorpion can not be mixed for a given port.
 * Triumph and Scorpion supports a set of 4 meters per ingress port. 
 * By default, if metering enabled, Broadcast packets select the 0th meter,
 * known Muliticast packets (including L2MC and IPMC packets) select 1st 
 * meter, unknown Multicast packets (including L2MC and IPMC packets) select
 * 2nd meter, and DLF packets select 3rd meter. If one ingress port want to
 * change the metering mode, it shall first turn off all meters enabled with
 * existing metering mode, and then program the new mode. Different metering 
 * mode in different ports are allowed. 
 */

/* Verify the meter mode per port */
STATIC int 
_bcm_trx_rate_modeset_verify(int unit, int port, int mode)
{
    uint32 ctrlMeter;
    int  byte_mode;
    int bcast_enable = 0;
    int known_l2mc = 0, unknown_l2mc = 0;
    int known_ipmc = 0, unknown_ipmc = 0, unknown_dlf = 0;
    soc_field_t known_l2mc_field;
#if defined(BCM_FIREBOLT6_SUPPORT)
    storm_control_meter_config_entry_t entry;
#endif


    if (SOC_IS_SHADOW(unit)) {
        known_l2mc_field = MC_ENABLEf;
    } else {
        known_l2mc_field = KNOWN_L2MC_ENABLEf;
    }

    /* Get the SC enable controls. */
#if defined (BCM_FIREBOLT6_SUPPORT)
    if (soc_feature(unit, soc_feature_storm_control_meter_is_mem)) {
        /* Clear the entry before reading. */
        sal_memset(&entry, 0, sizeof(storm_control_meter_config_entry_t));

        SOC_IF_ERROR_RETURN(soc_mem_read(unit,
            STORM_CONTROL_METER_CONFIGm, MEM_BLOCK_ALL, port, &entry));

         byte_mode    = soc_mem_field32_get(unit, STORM_CONTROL_METER_CONFIGm,
                                (uint32 *)&entry, BYTE_MODEf);
         bcast_enable = soc_mem_field32_get(unit,
                                 STORM_CONTROL_METER_CONFIGm,
                                 (uint32 *)&entry, BCAST_ENABLEf);
         known_l2mc   = soc_mem_field32_get(unit,
                                 STORM_CONTROL_METER_CONFIGm,
                                 (uint32 *)&entry, KNOWN_L2MC_ENABLEf);
         unknown_l2mc = soc_mem_field32_get(unit,
                                 STORM_CONTROL_METER_CONFIGm,
                                 (uint32 *)&entry, UNKNOWN_L2MC_ENABLEf);
         known_ipmc   = soc_mem_field32_get(unit,
                                 STORM_CONTROL_METER_CONFIGm,
                                 (uint32 *)&entry, KNOWN_IPMC_ENABLEf);
         unknown_ipmc = soc_mem_field32_get(unit,
                                 STORM_CONTROL_METER_CONFIGm,
                                 (uint32 *)&entry, UNKNOWN_IPMC_ENABLEf);
         unknown_dlf  = soc_mem_field32_get(unit,
                                 STORM_CONTROL_METER_CONFIGm,
                                 (uint32 *)&entry, DLFBC_ENABLEf);
    } else
#endif
    {
        SOC_IF_ERROR_RETURN
            (READ_STORM_CONTROL_METER_CONFIGr(unit, port, &ctrlMeter));
        byte_mode    = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter, BYTE_MODEf);
        bcast_enable = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter, BCAST_ENABLEf);
        known_l2mc   = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter, known_l2mc_field);
        unknown_l2mc = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter, UNKNOWN_L2MC_ENABLEf);
        known_ipmc   = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter, KNOWN_IPMC_ENABLEf);
        unknown_ipmc = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter, UNKNOWN_IPMC_ENABLEf);
        unknown_dlf  = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter, DLFBC_ENABLEf);

    }

    if (byte_mode != mode) {

        /* Check whether any meter is enabled */
        if (SOC_IS_SHADOW(unit)) {
            if (bcast_enable || known_l2mc) {
                return BCM_E_RESOURCE;
            }
        } else {
            if (bcast_enable || known_l2mc || unknown_l2mc||
                 known_ipmc || unknown_ipmc || unknown_dlf) {
                return BCM_E_RESOURCE;
            }
        }
    }

    return BCM_E_NONE;
}

/* Table of Kbits to BucketSize */
static uint32 
_bcm_trx_rate_bucketSz_map [] = {
    ((4   * KBYTES * BITSINBYTE) / KBITS_IN_API),
    ((16  * KBYTES * BITSINBYTE) / KBITS_IN_API),
    ((64  * KBYTES * BITSINBYTE) / KBITS_IN_API),
    ((256 * KBYTES * BITSINBYTE) / KBITS_IN_API),
    ((1   * MBYTES * BITSINBYTE) / KBITS_IN_API),
    ((4   * MBYTES * BITSINBYTE) / KBITS_IN_API),
    ((8   * MBYTES * BITSINBYTE) / KBITS_IN_API),
    ((16  * MBYTES * BITSINBYTE) / KBITS_IN_API)
};   

/* Kbit_to_bucketSz conversion */
STATIC int
_bcm_trx_rate_kbit_to_bucketsz(uint32 kbits)
{
    int ix;

    for (ix = 0; ix < COUNTOF(_bcm_trx_rate_bucketSz_map); ix++) {
        if (kbits <= _bcm_trx_rate_bucketSz_map[ix]) {
            return ix;
        }
    }

    return (-1);
}

/* Program the FP_STORM_CONTROL_METERS */
STATIC int
_bcm_trx_rate_meter_rate_set(int unit, int port, int index, 
                             int bucketSz, uint32 refreshCnt)
{
    ifp_storm_control_meters_entry_t  entryMeter;
    int entryNum;
    soc_mem_t mem;
    int storm_control_ptr = 0;
    port_tab_entry_t ptab;

    /* Table name change fot TH style IFP devices */
    if (soc_feature(unit, soc_feature_multi_pipe_mapped_ports) ||
        SOC_IS_TOMAHAWKX(unit)) {
        mem = IFP_STORM_CONTROL_METERSm;
    } else {
        mem = FP_STORM_CONTROL_METERSm;
    }
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit) && 
        SOC_MEM_IS_VALID(unit, FP_STORM_CONTROL_METERS_Xm)) {
        if (SOC_PBMP_MEMBER(SOC_INFO(unit).xpipe_pbm, port)) {
            mem = FP_STORM_CONTROL_METERS_Xm;
        } else {
            mem = FP_STORM_CONTROL_METERS_Ym;
        }
    }
#endif /* BCM_TRIDENT_SUPPORT */

    /* program the FP_STORM_CONTROL_METERS table */
    sal_memset(&entryMeter, 0x0, sizeof(entryMeter));
    soc_mem_field32_set(unit, mem, &entryMeter, BUCKETSIZEf, bucketSz);
    soc_mem_field32_set(unit, mem, &entryMeter, REFRESHCOUNTf, refreshCnt);
    soc_mem_field32_set(unit, mem, &entryMeter, BUCKETCOUNTf, 0);

    if (soc_mem_field_valid (unit, PORT_TABm, STORM_CONTROL_PTRf)) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm,
                                    MEM_BLOCK_ANY, port, &ptab));
        storm_control_ptr = soc_PORT_TABm_field32_get(unit, &ptab,
                                                    STORM_CONTROL_PTRf);
        entryNum = (storm_control_ptr << 2) + index;
    } else {
        entryNum = (port << 2) + index;
    }
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, mem, MEM_BLOCK_ALL, entryNum, &entryMeter));

    return BCM_E_NONE;
}

STATIC int
_bcm_trx_tokens_to_bucket_size(int unit, uint32 tokens, uint32 quantum)
{
    static uint32 _bcm_trx_tokens_to_bucket_map[] = {
        64*KBYTES, 256*KBYTES, 1*MBYTES, 4*MBYTES, 16*MBYTES, 64*MBYTES,
        128*MBYTES, 256*MBYTES
    };
    uint32 value;
    int i, c = COUNTOF(_bcm_trx_tokens_to_bucket_map);

    /* For TR3 the mapping values are reduced by 1/8 */    
    for (i = 0; i < c; i++) {
        value = _bcm_trx_tokens_to_bucket_map[i];
        if ((SOC_IS_TRIUMPH(unit)) || (SOC_IS_TRIUMPH2(unit))) {
            value /= 16;
        }
        if (SOC_IS_TRIUMPH3(unit)) {
            value /= 8;
        }
        if (value >= tokens) {
#if defined(BCM_TRIUMPH3_SUPPORT)
            /* For TR3 the min burst size(in packets) is 1 packet */
            /* Each packet consumes 2 * PACKET_QUANTUM tokens */
            if ((value / (2 * quantum)) >= 1)
#endif
            {
                break;
            }
        }
    }
    return (i == c) ? i-1 : i;
}

STATIC int
_bcm_trx_rate_meter_rate_get(int unit, int port, int mode, int index, 
                             uint32 *pRps, uint32 *pBrs);

/* Set the metering mode */
STATIC int 
_bcm_trx_rate_meter_portmode_set(int unit, int port, int mode,
                                 uint32 flags, uint32 fmask, 
                                 int rps, int brs)
{
    soc_field_t known_l2mc_field;
    uint32 ctrlMeter=0;
    int    allDisable = 0;
    int    bitSet;
    int    byte_mode;
    int    bucketSz;
    uint32 bcast_rate, mcast_rate, unknown_mcast_rate, dlf_rate, max_rate;
    uint32 min_rate = 0;
    uint32 burst, burst_tokens;
    uint32 refreshCnt, updated_refresh_cnt;
    uint32 deltaRps = 0;
    uint32 max_quantum = 4000;
    uint32 max_refresh_count = _BCM_TRX_RATE_MAX_REFRESH_COUNT;
    uint32 quantum = 125, old_quantum = 0;
    uint64 rps_temp;
    uint32 bcast_index = _BCM_TRX_RATE_BCAST_INDEX;
    uint32 mcast_index = _BCM_TRX_RATE_MCAST_INDEX;
    uint32 unknown_mcast_index = _BCM_TRX_RATE_UNKNOWN_MCAST_INDEX;
    uint32 dlf_index = _BCM_TRX_RATE_DLF_INDEX;
    int bcast_enable = 0;
    int known_l2mc = 0, unknown_l2mc = 0;
    int unknown_dlf = 0;
#if defined(BCM_FIREBOLT6_SUPPORT)
    storm_control_meter_config_entry_t entry;
#endif

    if (soc_feature(unit, soc_feature_configurable_storm_control_meter_mapping)) {
        _BCM_TRX_RATE_BCAST_INDEX_GET(unit, bcast_index);
        _BCM_TRX_RATE_MCAST_INDEX_GET(unit, mcast_index);
        _BCM_TRX_RATE_UNKNOWN_MCAST_INDEX_GET(unit, unknown_mcast_index);
        _BCM_TRX_RATE_DLF_INDEX_GET(unit, dlf_index);
    }

    if (SOC_IS_SHADOW(unit)) {
        known_l2mc_field = MC_ENABLEf;
    } else {
        known_l2mc_field = KNOWN_L2MC_ENABLEf;
    }


#if defined (BCM_FIREBOLT6_SUPPORT)
    if (soc_feature(unit, soc_feature_storm_control_meter_is_mem)) {
        /* Clear the entry before reading. */
        sal_memset(&entry, 0, sizeof(storm_control_meter_config_entry_t));

        SOC_IF_ERROR_RETURN(soc_mem_read(unit,
            STORM_CONTROL_METER_CONFIGm, MEM_BLOCK_ALL, port, &entry));

        byte_mode    = soc_mem_field32_get(unit, STORM_CONTROL_METER_CONFIGm,
                                (uint32 *)&entry, BYTE_MODEf);
        old_quantum  = soc_mem_field32_get(unit, STORM_CONTROL_METER_CONFIGm,
                                (uint32 *)&entry, PACKET_QUANTUMf);
        bcast_enable = soc_mem_field32_get(unit,
                                 STORM_CONTROL_METER_CONFIGm,
                                 (uint32 *)&entry, BCAST_ENABLEf);
        known_l2mc   = soc_mem_field32_get(unit,
                                 STORM_CONTROL_METER_CONFIGm,
                                 (uint32 *)&entry, KNOWN_L2MC_ENABLEf);
        unknown_l2mc = soc_mem_field32_get(unit,
                                 STORM_CONTROL_METER_CONFIGm,
                                 (uint32 *)&entry, UNKNOWN_L2MC_ENABLEf);
        unknown_dlf  = soc_mem_field32_get(unit,
                                 STORM_CONTROL_METER_CONFIGm,
                                 (uint32 *)&entry, DLFBC_ENABLEf);

    } else
#endif
    {
       SOC_IF_ERROR_RETURN
            (READ_STORM_CONTROL_METER_CONFIGr(unit, port, &ctrlMeter));
        byte_mode    = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter, BYTE_MODEf);
        bcast_enable = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter, BCAST_ENABLEf);
        known_l2mc   = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter, known_l2mc_field);
        unknown_l2mc = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter, UNKNOWN_L2MC_ENABLEf);
        unknown_dlf  = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter, DLFBC_ENABLEf);
        old_quantum = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                   ctrlMeter, PACKET_QUANTUMf);

    }

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || \
    defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit)) {
        max_refresh_count = _BCM_TR3_RATE_MAX_REFRESH_COUNT;
        max_quantum = 256000;
    } else if (SOC_IS_HURRICANE3(unit)||SOC_IS_GREYHOUND2(unit)){
        /* only max_refresh_count need be override. */
        max_refresh_count = _BCM_HR3_RATE_MAX_REFRESH_COUNT;
    }
#endif /* TRIUMPH3 || TRIDENT2 || HURRICANE3 */

    /* Min rate is calculated using the equation given in comments in
     * below paragraphs for different devices. To compute the min value use
     * refreshCnt=1 and max value for the PACKET QUANTUM.
     */
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {
	min_rate = 1 * (256000 / max_quantum) / 2;  /* min_refresh_count=1 */
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
    {
	min_rate = 1 * (8000 / max_quantum);  /* min_refresh_count=1 */
    }

    /* rps value 0 is used to reset the meter */
    if (rps && (rps < min_rate)) {
	return BCM_E_PARAM;
    }

    bcast_rate = 0;
    mcast_rate = 0;
    unknown_mcast_rate = 0;
    dlf_rate = 0;
    if (mode == _BCM_TRX_RATE_PKT_MODE) {
        max_rate = rps;
        if (mode == byte_mode) {
            if (!(fmask & BCM_RATE_BCAST) 
                && (bcast_enable)) {
                _bcm_trx_rate_meter_rate_get(unit, port, mode,
                                             bcast_index,
                                             &bcast_rate, &burst);
                if (max_rate < bcast_rate) {
                    max_rate = bcast_rate;
                }
            }

            if (!(fmask & (BCM_RATE_MCAST | BCM_RATE_KNOWN_MCAST)) 
                && (known_l2mc)) {
                _bcm_trx_rate_meter_rate_get(unit, port, mode,
                                             mcast_index,
                                             &mcast_rate, &burst);
                if (max_rate < mcast_rate) {
                    max_rate = mcast_rate;
                }
            }
            if (!SOC_IS_SHADOW(unit)) {
                if (!(fmask & (BCM_RATE_MCAST | BCM_RATE_UNKNOWN_MCAST)) 
                    && (unknown_l2mc)) {
                    _bcm_trx_rate_meter_rate_get
                        (unit, port, mode, unknown_mcast_index,
                         &unknown_mcast_rate, &burst);
                    if (max_rate < unknown_mcast_rate) {
                        max_rate = unknown_mcast_rate;
                    }
                }
                if (!(fmask & BCM_RATE_DLF)
                    && (unknown_dlf)) {
                    _bcm_trx_rate_meter_rate_get(unit, port, mode,
                                                 dlf_index,
                                                 &dlf_rate, &burst);
                    if (max_rate < dlf_rate) {
                        max_rate = dlf_rate;
                    }
                }
            }
        }

        /*
         * Triumph3:
         *
         * Each meter is refreshed 16000 times/sec;
         * Each refresh will add refreshCnt tokens;
         * Each packet consumes 2 * PACKET_QUANTUM tokens;
         * refreshCnt = (rps * PACKET_QUANTUM * 2) / 16000;
         *            = rps * PACKET_QUANTUM / 8000;
         *
         * Trident2:
         * Each meter is refreshed 64000 times/sec;
         * Each refresh will add (refreshCnt * 2) tokens;
         * Each packet consumes PACKET_QUANTUM tokens;
         * refreshCnt = (rps * PACKET_QUANTUM ) / (64000 * 2);
         *            = rps * PACKET_QUANTUM / 128000;
         *
         * Other TRX devices:
         *
         * Each meter is refreshed 64000 times/sec;
         * Each refresh will add (refreshCnt * 2) tokens;
         * Each packet consumes (PACKET_QUANTUM * 16) tokens;
         * refreshCnt = rps * PACKET_QUANTUM * 16 / (64000 * 2);
         *            = rps * PACKET_QUANTUM / 8000;
         *
         * pick quantum which is a factor of 8000 to reduce error
         */
        for (quantum = max_quantum; quantum >= 125; quantum >>= 1) {
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                if (max_rate <= max_refresh_count * (256000 / quantum) / 2) {
                    break;
                }
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                if (max_rate <= max_refresh_count * (8000 / quantum)) {
                    break;
                }
            }
        }
        if (quantum < 125) {
            for (quantum = 80; quantum >= 5; quantum >>= 1) {
#if defined(BCM_TRIDENT2_SUPPORT)
                if (SOC_IS_TD2_TT2(unit)) {
                    if (max_rate <= max_refresh_count * (128000 / quantum)) {
                        break;
                    }
                } else
#endif /* BCM_TRIDENT2_SUPPORT */
                {
                    if (max_rate <= max_refresh_count * (8000 / quantum)) {
                        break;
                    }
                }
            }
            if (quantum < 5) {
                return BCM_E_PARAM;
            }
        }

#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TD2_TT2(unit)) {
            if (quantum <= 128000) {
                refreshCnt = rps / (128000 / quantum);
            } else {
                refreshCnt = rps * (quantum / 128000);
            }
        } else
#endif /* BCM_TRIDENT2_SUPPORT */
        {
           /* The refreshCnt at this point is the converted value of requested pps, */
           /* rouded off to nearest lower value. Choosen quantum would accomodate   */
           /* the requested pps. Compute deltaRps which is the difference between   */
           /* requested pps and computed pps with refreshCnt. If deltaRps is more   */
           /* than 0, then increment the refreshCnt by one, so that configured pps  */
           /* is more than requested pps.                                           */

            if (quantum <= 8000) {
                refreshCnt = rps / (8000 / quantum);
                deltaRps = rps - (refreshCnt * (8000 / quantum));
                if (deltaRps) {
                    refreshCnt++ ;
                }
                rps += refreshCnt * (8000 / quantum);
            } else {
                refreshCnt = rps * (quantum / 8000);
                deltaRps = rps - (refreshCnt / (quantum / 8000));
                if (deltaRps) {
                    refreshCnt++ ;
                }
                rps += refreshCnt / (quantum / 8000);
            }
        }
        /*
         * Triumph3:
         * burst size (in tokens) = rps * (62.5us/1000000) * 2 * packet_quantum;
         *                        = rps * 2 * packet_quantum / 16000;
         *                        = rps * packet_quantum / 8000;
         *
         * Other TRX devices:
         *
         * burst size (in tokens) = rps * (15.625us/1000000) * 16 * packet_quantum;
         *                        = rps * 16 * packet_quantum / 64000;
         *                        = rps * packet_quantum / 4000;
         *
         * Choose smallest bucket size that can support the burst size.
         */
        COMPILER_64_SET(rps_temp, 0, rps);
        COMPILER_64_UMUL_32(rps_temp, quantum); 
        if (SOC_IS_TRIUMPH3(unit)) {
            if (SOC_FAILURE(soc_esw_div64(rps_temp,8000,&burst_tokens))){
                return SOC_E_INTERNAL;
            }
        } else {
            if (SOC_FAILURE(soc_esw_div64(rps_temp,4000,&burst_tokens))) {
                return SOC_E_INTERNAL;
            }
        }
        if (brs != -1) {
            bucketSz = _bcm_trx_rate_kbit_to_bucketsz(brs);
        } else {
            bucketSz = _bcm_trx_tokens_to_bucket_size(unit, burst_tokens, quantum);
        }
    } else {
        /*
         * Triumph3:
         *
         * Each meter is refreshed 8000 times/sec;
         * Each refresh will add refreshCnt tokens;
         * Each token represents 8 bit data.
         * rps = (refreshCnt * 8000 * 8) / (KBITS_IN_API) ->
         * rps = refreshCnt * 64;
         *
         * Other TRX devices:
         *
         * Each meter is refreshed 64000 times/sec;
         * Each refresh will add 2 * refreshCnt tokens;
         * Each token represents 0.5 bit data.
         * rps = ((refreshCnt * 2 * 64000) / 2) / (KBITS_IN_API) ->
         * rps = refreshCnt * 64;
         */
        refreshCnt = rps / _BCM_TRX_REFRESH_TO_RPS;

#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_METROLITE(unit) || SOC_IS_SABER2(unit)) {
         /* For Metrolite and Saber2 rps = refreshCnt * 64*2 */;
            refreshCnt = rps / (2*_BCM_TRX_REFRESH_TO_RPS);
        }
#endif
        /* brs is in unit of Kbits */
        bucketSz = _bcm_trx_rate_kbit_to_bucketsz(brs);
        if (bucketSz == -1) {
            return BCM_E_PARAM;
        }
    }

    if (refreshCnt == 0 && rps != 0) {
        /* Use minimum value unless rps is zero */
        refreshCnt = 1;
    } else if (refreshCnt > max_refresh_count) {
        return BCM_E_PARAM;
    }

    if (byte_mode != mode) {
        if (SOC_IS_SHADOW(unit)) {
            /* if metering mode is changed, disable the storm control first */
            soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr, &ctrlMeter,
                          BCAST_ENABLEf, 0);
            soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr, &ctrlMeter,
                          MC_ENABLEf, 0);
        } else {

#if defined (BCM_FIREBOLT6_SUPPORT)
            if (soc_feature(unit, soc_feature_storm_control_meter_is_mem)) {

                soc_mem_field32_set(unit, STORM_CONTROL_METER_CONFIGm,
                    (uint32 *)&entry, BCAST_ENABLEf, 0);
                soc_mem_field32_set(unit, STORM_CONTROL_METER_CONFIGm,
                    (uint32 *)&entry, DLFBC_ENABLEf, 0);
                soc_mem_field32_set(unit, STORM_CONTROL_METER_CONFIGm,
                    (uint32 *)&entry, KNOWN_L2MC_ENABLEf, 0);
                soc_mem_field32_set(unit, STORM_CONTROL_METER_CONFIGm,
                    (uint32 *)&entry, UNKNOWN_L2MC_ENABLEf, 0);
                soc_mem_field32_set(unit, STORM_CONTROL_METER_CONFIGm,
                    (uint32 *)&entry, KNOWN_IPMC_ENABLEf, 0);
                soc_mem_field32_set(unit, STORM_CONTROL_METER_CONFIGm,
                    (uint32 *)&entry, UNKNOWN_IPMC_ENABLEf, 0);

                /* Program new metering mode. */
                soc_mem_field32_set(unit, STORM_CONTROL_METER_CONFIGm,
                    (uint32 *)&entry, BYTE_MODEf, mode);
                SOC_IF_ERROR_RETURN(
                    soc_mem_write(unit, STORM_CONTROL_METER_CONFIGm,
                    MEM_BLOCK_ALL, port, &entry));
            } else
#endif
            {
                /* if metering mode is changed, disable the storm control first */
                soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr, &ctrlMeter,
                     BCAST_ENABLEf, 0);
                soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr, &ctrlMeter,
                     DLFBC_ENABLEf, 0);
                soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr, &ctrlMeter,
                     KNOWN_L2MC_ENABLEf, 0);
                soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr, &ctrlMeter,
                     UNKNOWN_L2MC_ENABLEf, 0);
                soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr, &ctrlMeter,
                     KNOWN_IPMC_ENABLEf, 0);
                soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr, &ctrlMeter,
                     UNKNOWN_IPMC_ENABLEf, 0);

                /* program new metering mode */
                soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr, &ctrlMeter,
                     BYTE_MODEf, mode);
                SOC_IF_ERROR_RETURN
                    (WRITE_STORM_CONTROL_METER_CONFIGr(unit, port, ctrlMeter));
            }
            allDisable = 1;
        }
    }

    /* enable/disable BCAST counting */
    if (fmask & BCM_RATE_BCAST) {
        bitSet = 0;
        if (flags & BCM_RATE_BCAST) {
            _bcm_trx_rate_meter_rate_set(unit, port, 
                                         bcast_index,
                                         bucketSz, refreshCnt);
            bitSet = 1;
        }
        if (bitSet || !allDisable) {
#if defined (BCM_FIREBOLT6_SUPPORT)
            if (soc_feature(unit, soc_feature_storm_control_meter_is_mem)) {

                soc_mem_field32_set(unit, STORM_CONTROL_METER_CONFIGm,
                    (uint32 *)&entry, BCAST_ENABLEf, bitSet);
            } else
#endif
            {
                soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr, &ctrlMeter,
                    BCAST_ENABLEf, bitSet);
            }
        }
    } else if (mode == _BCM_TRX_RATE_PKT_MODE && bcast_rate != 0 &&
               quantum != old_quantum) {
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TD2_TT2(unit)) {
            if (quantum <= 128000) {
                updated_refresh_cnt = bcast_rate / (128000 / quantum);
            } else {
                updated_refresh_cnt = bcast_rate * (quantum / 128000);
            }
        } else
#endif /* BCM_TRIDENT2_SUPPORT */
        {
            if (quantum <= 8000) {
                updated_refresh_cnt = bcast_rate / (8000 / quantum);
                deltaRps = bcast_rate - (updated_refresh_cnt * (8000 / quantum));
            } else {
                updated_refresh_cnt = bcast_rate * (quantum / 8000);
                deltaRps = bcast_rate - (updated_refresh_cnt / (quantum / 8000));
            }
            if (deltaRps) {
                updated_refresh_cnt++ ;
            }
        }

        /* Use minimum value */
        if (updated_refresh_cnt == 0) {
           updated_refresh_cnt = 1;
        }

        _bcm_trx_rate_meter_rate_set(unit, port, bcast_index,
                                     bucketSz, updated_refresh_cnt);
    }

    /* enable/disable known MCAST counting */
    if (fmask & (BCM_RATE_MCAST | BCM_RATE_KNOWN_MCAST)) {
        bitSet = 0;
        if (flags & (BCM_RATE_MCAST | BCM_RATE_KNOWN_MCAST)) {
            _bcm_trx_rate_meter_rate_set(unit, port, 
                                         mcast_index,
                                         bucketSz, refreshCnt);
            bitSet = 1;
        }
        if (bitSet || !allDisable) { 
            if (SOC_IS_SHADOW(unit)) {
                soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr,
                                  &ctrlMeter, MC_ENABLEf, bitSet);
            } else {
#if defined (BCM_FIREBOLT6_SUPPORT)
                if (soc_feature(unit, soc_feature_storm_control_meter_is_mem)) {
                    soc_mem_field32_set(unit, STORM_CONTROL_METER_CONFIGm,
                        (uint32 *)&entry, KNOWN_L2MC_ENABLEf, bitSet);
                    soc_mem_field32_set(unit, STORM_CONTROL_METER_CONFIGm,
                        (uint32 *)&entry, KNOWN_IPMC_ENABLEf, bitSet);

                } else
#endif
                {
                    soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr,
                         &ctrlMeter, KNOWN_L2MC_ENABLEf, bitSet);
                    soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr,
                         &ctrlMeter, KNOWN_IPMC_ENABLEf, bitSet);
                }
            }
        }
    } else if (mode == _BCM_TRX_RATE_PKT_MODE && mcast_rate != 0 &&
               quantum != old_quantum) {
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TD2_TT2(unit)) {
            if (quantum <= 128000) {
                updated_refresh_cnt = mcast_rate / (128000 / quantum);
            } else {
                updated_refresh_cnt = mcast_rate * (quantum / 128000);
            }
        } else
#endif /* BCM_TRIDENT2_SUPPORT */
        {
            if (quantum <= 8000) {
                updated_refresh_cnt = mcast_rate / (8000 / quantum);
                deltaRps = mcast_rate - (updated_refresh_cnt * (8000 / quantum));
            } else {
                updated_refresh_cnt = mcast_rate * (quantum / 8000);
                deltaRps = mcast_rate - (updated_refresh_cnt / (quantum / 8000));
            }
            if (deltaRps) {
                updated_refresh_cnt++ ;
            }
        }

        /* Use minimum value */
        if (updated_refresh_cnt == 0) {
           updated_refresh_cnt = 1;
        }

        _bcm_trx_rate_meter_rate_set(unit, port, mcast_index,
                                     bucketSz, updated_refresh_cnt);
    }

    /* enable/disable unknown MCAST counting */
    if (!SOC_IS_SHADOW(unit)) {
        if (fmask & (BCM_RATE_MCAST | BCM_RATE_UNKNOWN_MCAST)) {
            bitSet = 0;
            if (flags & (BCM_RATE_MCAST | BCM_RATE_UNKNOWN_MCAST)) {
                _bcm_trx_rate_meter_rate_set
                    (unit, port, unknown_mcast_index, bucketSz,
                     refreshCnt);
                bitSet = 1;
            }
            if (bitSet || !allDisable) { 
#if defined (BCM_FIREBOLT6_SUPPORT)
                if (soc_feature(unit, soc_feature_storm_control_meter_is_mem)) {
                    soc_mem_field32_set(unit, STORM_CONTROL_METER_CONFIGm,
                        (uint32 *)&entry, UNKNOWN_L2MC_ENABLEf, bitSet);
                    soc_mem_field32_set(unit, STORM_CONTROL_METER_CONFIGm,
                        (uint32 *)&entry, UNKNOWN_IPMC_ENABLEf, bitSet);

                } else
#endif
                {
                    soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr,
                        &ctrlMeter, UNKNOWN_L2MC_ENABLEf, bitSet);
                    soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr,
                        &ctrlMeter, UNKNOWN_IPMC_ENABLEf, bitSet);
                }
            }
        } else if (mode == _BCM_TRX_RATE_PKT_MODE && unknown_mcast_rate != 0 &&
                   quantum != old_quantum) {
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                if (quantum <= 128000) {
                    updated_refresh_cnt = unknown_mcast_rate / (128000 / quantum);
                } else {
                    updated_refresh_cnt = unknown_mcast_rate * (quantum / 128000);
                }
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                if (quantum <= 8000) {
                    updated_refresh_cnt = unknown_mcast_rate / (8000 / quantum);
                    deltaRps = unknown_mcast_rate - (updated_refresh_cnt * (8000 / quantum));
                } else {
                    updated_refresh_cnt = unknown_mcast_rate * (quantum / 8000);
                    deltaRps = unknown_mcast_rate - (updated_refresh_cnt / (quantum / 8000));
                }
                if (deltaRps) {
                    updated_refresh_cnt++ ;
                }
            }
            _bcm_trx_rate_meter_rate_set(unit, port,
                                         unknown_mcast_index,
                                         bucketSz, updated_refresh_cnt);
        }

        /* enable/disable DLF counting */
        if (fmask & BCM_RATE_DLF) {
            bitSet = 0;
            if (flags & BCM_RATE_DLF) {
                _bcm_trx_rate_meter_rate_set(unit, port, 
                                             dlf_index, bucketSz,
                                             refreshCnt);
                bitSet = 1;
            }
            if (bitSet || !allDisable) {
#if defined (BCM_FIREBOLT6_SUPPORT)
                if (soc_feature(unit, soc_feature_storm_control_meter_is_mem)) {
                    soc_mem_field32_set(unit, STORM_CONTROL_METER_CONFIGm,
                        (uint32 *)&entry, DLFBC_ENABLEf, bitSet);

                } else
#endif
                {
                    soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr,
                        &ctrlMeter, DLFBC_ENABLEf, bitSet);
                }
            }
        } else if (mode == _BCM_TRX_RATE_PKT_MODE && dlf_rate != 0 &&
                   quantum != old_quantum) {
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                if (quantum <= 128000) {
                    updated_refresh_cnt = dlf_rate / (128000 / quantum);
                } else {
                    updated_refresh_cnt = dlf_rate * (quantum / 128000);
                }
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                if (quantum <= 8000) {
                    updated_refresh_cnt = dlf_rate / (8000 / quantum);
                    deltaRps = dlf_rate - (updated_refresh_cnt * (8000 / quantum));
                } else {
                    updated_refresh_cnt = dlf_rate * (quantum / 8000);
                    deltaRps = dlf_rate - (updated_refresh_cnt / (quantum / 8000));
                }
                if (deltaRps) {
                    updated_refresh_cnt++ ;
                }
            }

            /* Use minimum value */
            if (updated_refresh_cnt == 0) {
               updated_refresh_cnt = 1;
            }

            _bcm_trx_rate_meter_rate_set(unit, port, dlf_index,
                                         bucketSz, updated_refresh_cnt);
        }
    }

    /* Set PACKET_QUANTUM for optimal precision */ 
    if (mode == _BCM_TRX_RATE_PKT_MODE) {
#if defined (BCM_FIREBOLT6_SUPPORT)
        if (soc_feature(unit, soc_feature_storm_control_meter_is_mem)) {
            soc_mem_field32_set(unit, STORM_CONTROL_METER_CONFIGm,
                (uint32 *)&entry, PACKET_QUANTUMf, quantum);

        } else
#endif
        {
            soc_reg_field_set(unit, STORM_CONTROL_METER_CONFIGr,
                 &ctrlMeter, PACKET_QUANTUMf, quantum);
        }
    }

#if defined (BCM_FIREBOLT6_SUPPORT)
    if (soc_feature(unit, soc_feature_storm_control_meter_is_mem)) {
        SOC_IF_ERROR_RETURN(
            soc_mem_write(unit, STORM_CONTROL_METER_CONFIGm,
            MEM_BLOCK_ALL, port, &entry));
    } else
#endif
    {
        /* write to registers */
        SOC_IF_ERROR_RETURN
            (WRITE_STORM_CONTROL_METER_CONFIGr(unit, port, ctrlMeter));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trx_rate_set
 * Description:
 *      Triumph and Scopiton specific implementation for
 *      bcm_rate_xxx_set APIs
 * Parameters:
 *      unit  -  unit
 *      port  -  port number
 *      mode  -  packet- or byte-based mode
 *      flags -  flags for packet type
 *      fmask -  flags mask for <flags> valid bits
 *      rps   -  rate per second (pkt/sec for packet-based mode
 *                                Kbits/sec for byte-based mode)
 *      brs   -  burst size number (ignored in packet-based mode,
 *                                  Kbits/sec for byte-based mode)  
 *          
 * Return:
 *      BCM_E_XXX
 */
STATIC int
_bcm_trx_rate_set(int unit, int port, int mode, uint32 flags, 
                  uint32 fmask, uint32 rps, uint32 brs)
{
    if (!soc_feature(unit,soc_feature_storm_control)) {
        return BCM_E_UNAVAIL;
    }

    if (port != -1 && !SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    if (!(fmask & BCM_RATE_ALL)) {
        return BCM_E_PARAM;
    }

    /* Verify the configured mode is available */
    if (port == -1) { /* All ports */
        PBMP_ALL_ITER(unit, port) {
            if (IS_LB_PORT(unit, port) || !SOC_PORT_VALID(unit, port)) {
                if (!SOC_PORT_VALID_RANGE(unit, port)) {
                    break;
                }
                continue;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_trx_rate_modeset_verify(unit, port, mode));
        } 
        PBMP_ALL_ITER(unit, port) {
            if (IS_LB_PORT(unit, port) || !SOC_PORT_VALID(unit, port)) {
                if (!SOC_PORT_VALID_RANGE(unit, port)) {
                    break;
                }
                continue;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_trx_rate_meter_portmode_set(unit, port, mode, flags,
                                                  fmask, rps, brs));
        }
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_trx_rate_modeset_verify(unit, port, mode));

        BCM_IF_ERROR_RETURN
            (_bcm_trx_rate_meter_portmode_set(unit, port, mode, flags,
                                              fmask, rps, brs));
    }          
    return BCM_E_NONE;
}

STATIC int
_bcm_trx_rate_meter_rate_get(int unit, int port, int mode, int index, 
                             uint32 *pRps, uint32 *pBrs)
{
    uint32 ctrlMeter;
    ifp_storm_control_meters_entry_t entryMeter;
    int    entryNum;
    uint32 bucketSz;
    uint32 refreshCnt;
    uint32 quantum;
    soc_mem_t mem;
#if defined(BCM_FIREBOLT6_SUPPORT)
    storm_control_meter_config_entry_t entry;
#endif

    /* Table name change fot TH style IFP devices */
    if (soc_feature(unit, soc_feature_multi_pipe_mapped_ports) ||
        SOC_IS_TOMAHAWKX(unit)) {
        mem = IFP_STORM_CONTROL_METERSm;
    } else {
        mem = FP_STORM_CONTROL_METERSm;
    }
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit) && 
        SOC_MEM_IS_VALID(unit, FP_STORM_CONTROL_METERS_Xm)) {
        if (SOC_PBMP_MEMBER(SOC_INFO(unit).xpipe_pbm, port)) {
            mem = FP_STORM_CONTROL_METERS_Xm;
        } else {
            mem = FP_STORM_CONTROL_METERS_Ym;
        }
    }
#endif /* BCM_TRIDENT_SUPPORT */

    entryNum = (port << 2) + index;
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, entryNum, &entryMeter));

    refreshCnt = soc_mem_field32_get(unit, mem, &entryMeter, REFRESHCOUNTf);
    bucketSz   = soc_mem_field32_get(unit, mem, &entryMeter, BUCKETSIZEf);
    if (mode == _BCM_TRX_RATE_PKT_MODE) {
#if defined (BCM_FIREBOLT6_SUPPORT)
        if (soc_feature(unit, soc_feature_storm_control_meter_is_mem)) {
            sal_memset(&entry, 0, sizeof(storm_control_meter_config_entry_t));
            SOC_IF_ERROR_RETURN(soc_mem_read(unit,
                STORM_CONTROL_METER_CONFIGm, MEM_BLOCK_ALL, port, &entry));
             quantum = soc_mem_field32_get(unit, STORM_CONTROL_METER_CONFIGm,
                                    (uint32 *)&entry, PACKET_QUANTUMf);
        } else
#endif
        {
            SOC_IF_ERROR_RETURN
                (READ_STORM_CONTROL_METER_CONFIGr(unit, port, &ctrlMeter));
            quantum = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                    ctrlMeter, PACKET_QUANTUMf);
        }

        /* Please refer to calculations in _bcm_trx_rate_meter_portmode_set */
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TD2_TT2(unit)) {
            if (quantum <= 128000) {
                *pRps = refreshCnt * (128000 / quantum);
            } else {
                *pRps = refreshCnt / (quantum / 128000);
            }
        } else 
#endif /* BCM_TRIDENT2_SUPPORT */
        {
            if (quantum <= 8000) {
            {
                *pRps = refreshCnt * (8000 / quantum);
            }
            } else {
                *pRps = refreshCnt / (quantum / 8000);
            }
        }
        *pBrs = _bcm_trx_rate_bucketSz_map[bucketSz];
    } else if (mode == _BCM_TRX_RATE_BYTE_MODE) {

        /* Please refer to calculations in _bcm_trx_rate_meter_portmode_set */
        *pRps = refreshCnt * _BCM_TRX_REFRESH_TO_RPS; 
#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_METROLITE(unit) || SOC_IS_SABER2(unit)) {
         /* For Metrolite and Saber2 rps = refreshCnt * 64*2 */;
         *pRps = refreshCnt * (2*_BCM_TRX_REFRESH_TO_RPS);
        }
#endif
        *pBrs = _bcm_trx_rate_bucketSz_map[bucketSz]; 

    } else 
        return BCM_E_PARAM;
    
    return BCM_E_NONE;
}

STATIC int
_bcm_trx_rate_get(int unit, int port, int mode, int *pFlags,
                  uint32 fmask, uint32 *pRps, int *psRps, uint32 *pBrs)
{
    uint32 ctrlMeter;
    int    byte_mode;
    uint32 pktType = 0;
    uint32 rate    = 0;
    uint32 burstSz = 0;
    uint32 bcast_index = _BCM_TRX_RATE_BCAST_INDEX;
    uint32 mcast_index = _BCM_TRX_RATE_MCAST_INDEX;
    uint32 unknown_mcast_index = _BCM_TRX_RATE_UNKNOWN_MCAST_INDEX;
    uint32 dlf_index = _BCM_TRX_RATE_DLF_INDEX;
    int bcast_enable = 0;
    int known_l2mc = 0, unknown_l2mc = 0;
    int known_ipmc = 0, unknown_ipmc = 0, unknown_dlf = 0;
    soc_field_t known_l2mc_field;
#if defined(BCM_FIREBOLT6_SUPPORT)
    storm_control_meter_config_entry_t entry;
#endif

    if (SOC_IS_SHADOW(unit)) {
        known_l2mc_field = MC_ENABLEf;
    } else {
        known_l2mc_field = KNOWN_L2MC_ENABLEf;
    }

    if (soc_feature(unit, soc_feature_configurable_storm_control_meter_mapping)) {
        _BCM_TRX_RATE_BCAST_INDEX_GET(unit, bcast_index);
        _BCM_TRX_RATE_MCAST_INDEX_GET(unit, mcast_index);
        _BCM_TRX_RATE_UNKNOWN_MCAST_INDEX_GET(unit, unknown_mcast_index);
        _BCM_TRX_RATE_DLF_INDEX_GET(unit, dlf_index);
    }

    if (!soc_feature(unit,soc_feature_storm_control)) {
        if (pFlags != NULL) {
            *pFlags = pktType;
        }
        return BCM_E_UNAVAIL;
    }

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    assert ((mode == _BCM_TRX_RATE_PKT_MODE) ||
            (mode == _BCM_TRX_RATE_BYTE_MODE));

    /* Get the SC enable controls. */
#if defined (BCM_FIREBOLT6_SUPPORT)
    if (soc_feature(unit, soc_feature_storm_control_meter_is_mem)) {
        /* Clear the entry before reading. */
        sal_memset(&entry, 0, sizeof(storm_control_meter_config_entry_t));

        SOC_IF_ERROR_RETURN(soc_mem_read(unit,
            STORM_CONTROL_METER_CONFIGm, MEM_BLOCK_ALL, port, &entry));

         byte_mode    = soc_mem_field32_get(unit, STORM_CONTROL_METER_CONFIGm,
                                (uint32 *)&entry, BYTE_MODEf);
         bcast_enable = soc_mem_field32_get(unit,
                                 STORM_CONTROL_METER_CONFIGm,
                                 (uint32 *)&entry, BCAST_ENABLEf);
         known_l2mc   = soc_mem_field32_get(unit,
                                 STORM_CONTROL_METER_CONFIGm,
                                 (uint32 *)&entry, KNOWN_L2MC_ENABLEf);
         unknown_l2mc = soc_mem_field32_get(unit,
                                 STORM_CONTROL_METER_CONFIGm,
                                 (uint32 *)&entry, UNKNOWN_L2MC_ENABLEf);
         known_ipmc   = soc_mem_field32_get(unit,
                                 STORM_CONTROL_METER_CONFIGm,
                                 (uint32 *)&entry, KNOWN_IPMC_ENABLEf);
         unknown_ipmc = soc_mem_field32_get(unit,
                                 STORM_CONTROL_METER_CONFIGm,
                                 (uint32 *)&entry, UNKNOWN_IPMC_ENABLEf);
         unknown_dlf  = soc_mem_field32_get(unit,
                                 STORM_CONTROL_METER_CONFIGm,
                                 (uint32 *)&entry, DLFBC_ENABLEf);
    } else
#endif
    {
        SOC_IF_ERROR_RETURN
            (READ_STORM_CONTROL_METER_CONFIGr(unit, port, &ctrlMeter));
        byte_mode    = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter,
                                  BYTE_MODEf);
        bcast_enable = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr, ctrlMeter,
                                  BCAST_ENABLEf);
        known_l2mc   = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter, known_l2mc_field);
        unknown_l2mc = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter, UNKNOWN_L2MC_ENABLEf);
        known_ipmc   = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter, KNOWN_IPMC_ENABLEf);
        unknown_ipmc = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr,
                                  ctrlMeter, UNKNOWN_IPMC_ENABLEf);
        unknown_dlf  = soc_reg_field_get(unit, STORM_CONTROL_METER_CONFIGr, ctrlMeter,
                                      DLFBC_ENABLEf);
    }


    if (byte_mode == mode) {
        if (fmask & BCM_RATE_BCAST) {

            if (bcast_enable) {
                pktType |= BCM_RATE_BCAST;
                _bcm_trx_rate_meter_rate_get(unit, port, mode,
                                             bcast_index, &rate,
                                             &burstSz);
            }
        }
        if (fmask & BCM_RATE_MCAST) {
            if (SOC_IS_SHADOW(unit)) {
                if (known_l2mc) {
                    pktType |= BCM_RATE_MCAST;
                    _bcm_trx_rate_meter_rate_get (unit, port, mode,
                                                  mcast_index,
                                                  &rate, &burstSz);
                }
            } else {
                if (known_l2mc || unknown_l2mc || known_ipmc || unknown_ipmc) {
                    pktType |= BCM_RATE_MCAST;
                    _bcm_trx_rate_meter_rate_get(unit, port, mode,
                                                 mcast_index,
                                                 &rate, &burstSz);
                }
            }
        }
        if (!SOC_IS_SHADOW(unit)) {
            if (fmask & BCM_RATE_KNOWN_MCAST) {
                if (known_l2mc || known_ipmc) {
                    pktType |= BCM_RATE_KNOWN_MCAST;
                    _bcm_trx_rate_meter_rate_get(unit, port, mode,
                                                 mcast_index,
                                                 &rate, &burstSz);
                }
            }
            if (fmask & BCM_RATE_UNKNOWN_MCAST) {
                if (unknown_l2mc || known_ipmc) {
                    pktType |= BCM_RATE_UNKNOWN_MCAST;
                    _bcm_trx_rate_meter_rate_get(unit, port, mode,
                                                 unknown_mcast_index,
                                                 &rate, &burstSz);
                }
            }
            if (fmask & BCM_RATE_DLF) {

                if (unknown_dlf) {
                    pktType |= BCM_RATE_DLF;
                    _bcm_trx_rate_meter_rate_get(unit, port, mode,
                                                 dlf_index, &rate,
                                                 &burstSz);
                }
            }
        }
    }

    if (pFlags != NULL) {
        *pFlags = pktType;
    }
    if (pRps != NULL) {
        /* To accommodate uint32 pointers in API */
        *pRps = rate;
    }
    if (psRps != NULL) {
        /* To accommodate int pointers in API */
        *psRps = rate;
    }
    if (pBrs != NULL) {
        *pBrs = burstSz;
    }

    return BCM_E_NONE;
}
#endif /* BCM_TRX_SUPPORT */

/*
 * Function:
 *      _bcm_esw_rate_hw_clear
 * Description:
 *      Clear all registers and memories that related to rate limiting 
 * Parameters:
 *      unit - unit number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_rate_hw_clear(int unit)
{
    uint32 rval = 0;
    bcm_port_t  port;
    int         i;
    soc_reg_t   reg;
    soc_mem_t   _bcm_esw_rate_mems[] = {
        FP_SC_BCAST_METER_TABLEm,
        FP_SC_MCAST_METER_TABLEm,
        FP_SC_DLF_METER_TABLEm,
        FP_STORM_CONTROL_METERSm
    };
    soc_reg_t   _bcm_esw_rate_regs[] = {
        SC_BYTE_METER_CONFIGr,
        BCAST_STORM_CONTROLr,
        DLFBC_STORM_CONTROLr,
        MCAST_STORM_CONTROLr,
        STORM_CONTROL_METER_CONFIGr
    };

#ifdef BCM_TRX_SUPPORT
    uint32 bcast_index = _BCM_TRX_RATE_BCAST_INDEX;
    uint32 mcast_index = _BCM_TRX_RATE_MCAST_INDEX;
    uint32 unknown_mcast_index = _BCM_TRX_RATE_UNKNOWN_MCAST_INDEX;
    uint32 dlf_index = _BCM_TRX_RATE_DLF_INDEX;
    uint32 mapping = 0, old_mapping =  0;
#endif /* BCM_TRX_SUPPORT */

    if (SOC_WARM_BOOT(unit)) {
        return BCM_E_NONE;
    }

    /* Registers initialization */      
    for (i = 0; i < COUNTOF(_bcm_esw_rate_regs); i++) {
        if (SOC_REG_IS_VALID(unit, _bcm_esw_rate_regs[i])) {
            PBMP_PORT_ITER(unit, port) {
                reg = _bcm_esw_rate_regs[i];
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, port, 0, rval));
                /* special case handling */
                if (STORM_CONTROL_METER_CONFIGr == _bcm_esw_rate_regs[i]) {
                    soc_field_t fields[] = {PACKET_QUANTUMf, BYTE_MODEf};
                    uint32      values[] = {0x100, 1};

                    BCM_IF_ERROR_RETURN(
                        soc_reg_fields32_modify(unit,  
                                                STORM_CONTROL_METER_CONFIGr, 
                                                port, COUNTOF(fields), 
                                                fields, values));
                }
            }
        }
    }

#ifdef BCM_FIREBOLT6_SUPPORT
    if (soc_feature(unit, soc_feature_storm_control_meter_is_mem)) {
        PBMP_PORT_ITER(unit, port) {
            soc_field_t fields[] = {PACKET_QUANTUMf, BYTE_MODEf};
            uint32      values[] = {0x100, 1};

            SOC_IF_ERROR_RETURN(soc_mem_fields32_modify(unit,
                STORM_CONTROL_METER_CONFIGm, port, COUNTOF(fields),
                    fields, values));
        }
    }
#endif

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && !SOC_IS_SHADOW(unit)) {
        /* 
         * Set the per-chip STORM_CONTROL_METER_MAPPING register for now:
         * offset 0 meter - BCAST 
         * offset 1 meter - known MCAST (including l2MC and IPMC)
         * offset 2 meter - unknown MCAST (including L2MC and IPMC)
         * offset 3 meter - DLF
         */

        if (soc_feature(unit, soc_feature_configurable_storm_control_meter_mapping)) {
            _BCM_TRX_RATE_BCAST_INDEX_GET(unit, bcast_index);
            _BCM_TRX_RATE_MCAST_INDEX_GET(unit, mcast_index);
            _BCM_TRX_RATE_UNKNOWN_MCAST_INDEX_GET(unit, unknown_mcast_index);
            _BCM_TRX_RATE_DLF_INDEX_GET(unit, dlf_index);
        }

        (READ_STORM_CONTROL_METER_MAPPINGr(unit, &old_mapping));
        mapping = old_mapping;
        soc_reg_field_set(unit, STORM_CONTROL_METER_MAPPINGr,
                &mapping, BCAST_METER_INDEXf, bcast_index);
        soc_reg_field_set(unit, STORM_CONTROL_METER_MAPPINGr,
                &mapping, KNOWN_L2MC_METER_INDEXf, mcast_index);
        soc_reg_field_set(unit, STORM_CONTROL_METER_MAPPINGr,
                &mapping, UNKNOWN_L2MC_METER_INDEXf, unknown_mcast_index);
        soc_reg_field_set(unit, STORM_CONTROL_METER_MAPPINGr,
                &mapping, KNOWN_IPMC_METER_INDEXf, mcast_index);
        soc_reg_field_set(unit, STORM_CONTROL_METER_MAPPINGr,
                &mapping, UNKNOWN_IPMC_METER_INDEXf, unknown_mcast_index);
        soc_reg_field_set(unit, STORM_CONTROL_METER_MAPPINGr,
                &mapping, DLFBC_METER_INDEXf, dlf_index);
        if (old_mapping != mapping) {
            SOC_IF_ERROR_RETURN
                (WRITE_STORM_CONTROL_METER_MAPPINGr(unit, mapping));
        }

    }
#endif /* BCM_TRX_SUPPORT */

    /* Memories initialization */
    for (i = 0; i < COUNTOF(_bcm_esw_rate_mems); i++) {
        if (SOC_MEM_IS_VALID(unit, _bcm_esw_rate_mems[i])) {
            BCM_IF_ERROR_RETURN(
                soc_mem_clear(unit, _bcm_esw_rate_mems[i], COPYNO_ALL, TRUE));
        }
    }
    
    if (SOC_MEM_IS_VALID(unit, IFP_STORM_CONTROL_METERSm)) {
            BCM_IF_ERROR_RETURN(
                soc_mem_clear(unit, IFP_STORM_CONTROL_METERSm, 
                                           COPYNO_ALL, TRUE));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_rate_init
 * Description:
 *      Initialize the rate module
 * Parameters:
 *      unit - unit number
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_esw_rate_init(int unit)
{
    if(SOC_IS_RCPU_ONLY(unit)) {
        return BCM_E_NONE;
    }

    /* Currently no SW structure required for rate module so only clear HW */
    return _bcm_esw_rate_hw_clear(unit);
}

/*
 * Function:
 *      bcm_rate_type_set
 * Description:
 *      Front end to bcm_*cast_rate_set functions.
 *      Uses a single data structure to write into
 *      all the 3 rate control registers
 * Parameters:
 *      unit - unit number
 *      rl - data structure containing info to be written to the
 *           rate control registers
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_rate_type_set(int unit, bcm_rate_limit_t *rl)
{
    int port;

    PBMP_E_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_rate_bcast_set(unit, rl->br_bcast_rate, rl->flags, port));
        BCM_IF_ERROR_RETURN
            (bcm_esw_rate_mcast_set(unit, rl->br_mcast_rate, rl->flags, port));
        BCM_IF_ERROR_RETURN
            (bcm_esw_rate_dlfbc_set(unit, rl->br_dlfbc_rate, rl->flags, port));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_rate_set
 * Description:
 *      Configure rate limit and on/off state of
 *      DLF, MCAST, and BCAST limiting
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      pps - Rate limit value in packets/second
 *      flags - Bitmask with one or more of BCM_RATE_*
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 * Notes:
 *      StrataSwitch supports only one rate limit for all 3 types.
 */

int
bcm_esw_rate_set(int unit, int pps, int flags)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        uint32 fmask;
        fmask = BCM_RATE_DLF | BCM_RATE_MCAST | BCM_RATE_BCAST;
        if (SOC_IS_SHADOW(unit)) {
            if (flags & BCM_RATE_DLF) {
                return BCM_E_UNAVAIL;
            }
        }
        return (_bcm_trx_rate_set(unit, -1, _BCM_TRX_RATE_PKT_MODE, 
                                  flags, fmask, (uint32)pps, -1)); 
    }
#endif /* BCM_TRX_SUPPORT */

    if (SOC_IS_XGS3_SWITCH(unit)) {
        uint32 rate;
        int port;
        int enable = 0;
        PBMP_ALL_ITER(unit, port) {
            rate = 0;
            enable = (flags & BCM_RATE_MCAST) ? 1 : 0;
            soc_reg_field_set(unit, MCAST_STORM_CONTROLr,
                              &rate, ENABLEf, enable);
            soc_reg_field_set(unit, MCAST_STORM_CONTROLr,
                              &rate, THRESHOLDf, pps);
            SOC_IF_ERROR_RETURN
                (WRITE_MCAST_STORM_CONTROLr(unit, port, rate));

            rate = 0;
            enable = (flags & BCM_RATE_BCAST) ? 1 : 0;
            soc_reg_field_set(unit, BCAST_STORM_CONTROLr,
                              &rate, ENABLEf, enable);
            soc_reg_field_set(unit, BCAST_STORM_CONTROLr,
                              &rate, THRESHOLDf, pps);
            SOC_IF_ERROR_RETURN
                (WRITE_BCAST_STORM_CONTROLr(unit, port, rate));

            rate = 0;
            enable = (flags & BCM_RATE_DLF) ? 1 : 0;
            soc_reg_field_set(unit, DLFBC_STORM_CONTROLr,
                              &rate, ENABLEf, enable);
            soc_reg_field_set(unit, DLFBC_STORM_CONTROLr,
                              &rate, THRESHOLDf, pps);
            SOC_IF_ERROR_RETURN
                (WRITE_DLFBC_STORM_CONTROLr(unit, port, rate));
        }
        return BCM_E_NONE;
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_rate_mcast_set
 * Description:
 *      Configure rate limit for MCAST packets for the given port
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      pps - Rate limit value in packets/second
 *      flags - Bitmask with one or more of BCM_RATE_*
 *      port - Port number for which MCAST limit needs to be set
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_UNAVAIL - Not supported.
 * Notes:
 *      Individually setting MCAST limit is supported only on 5690 and later.
 */

int
bcm_esw_rate_mcast_set(int unit, int pps, int flags, int port)
{
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &port));
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return (_bcm_trx_rate_set(unit, port, _BCM_TRX_RATE_PKT_MODE, 
                                  flags, BCM_RATE_MCAST, (uint32)pps, -1)); 
    }
#endif /* BCM_TRX_SUPPORT */

    if (SOC_IS_XGS3_SWITCH(unit)) {
        uint32 rate = 0;
        int enable, threshold = pps;
        enable = (flags & BCM_RATE_MCAST) ? 1 : 0;
        soc_reg_field_set(unit, MCAST_STORM_CONTROLr,
                          &rate, ENABLEf, enable);
        soc_reg_field_set(unit, MCAST_STORM_CONTROLr,
                          &rate, THRESHOLDf, threshold);
        SOC_IF_ERROR_RETURN(WRITE_MCAST_STORM_CONTROLr(unit, port, rate));
        return BCM_E_NONE;
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_rate_bcast_set
 * Description:
 *      Configure rate limit for BCAST packets
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      pps - Rate limit value in packets/second
 *      flags - Bitmask with one or more of BCM_RATE_*
 *      port - Port number for which BCAST limit needs to be set
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_UNAVAIL - Not supported.
 * Notes:
 *      Individually setting BCAST limit is supported only on 5690 and later.
 */

int
bcm_esw_rate_bcast_set(int unit, int pps, int flags, int port)
{
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &port));
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return (_bcm_trx_rate_set(unit, port, _BCM_TRX_RATE_PKT_MODE, 
                                  flags, BCM_RATE_BCAST, (uint32)pps, -1)); 
    }
#endif /* BCM_TRX_SUPPORT */

    if (SOC_IS_XGS3_SWITCH(unit)) {
        uint32 rate = 0;
        int enable, threshold = pps;
        enable = (flags & BCM_RATE_BCAST) ? 1 : 0;
        soc_reg_field_set(unit, BCAST_STORM_CONTROLr,
                          &rate, ENABLEf, enable);
        soc_reg_field_set(unit, BCAST_STORM_CONTROLr,
                          &rate, THRESHOLDf, threshold);
        SOC_IF_ERROR_RETURN(WRITE_BCAST_STORM_CONTROLr(unit, port, rate));
        return BCM_E_NONE;
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_rate_dlfbc_set
 * Description:
 *      Configure rate limit for DLFBC packets
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      pps - Rate limit value in packets/second
 *      flags - Bitmask with one or more of BCM_RATE_*
 *      port - Port number for which DLFBC limit needs to be set
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_UNAVAIL - Not supported.
 * Notes:
 *      DLFBC: Destination Lookup Failure Broadcast
 *      Individually setting DLFBC limit is supported only on 5690 and later.
 */

int
bcm_esw_rate_dlfbc_set(int unit, int pps, int flags, int port)
{
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &port));
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return (_bcm_trx_rate_set(unit, port, _BCM_TRX_RATE_PKT_MODE, 
                                  flags, BCM_RATE_DLF, (uint32)pps, -1)); 
    }
#endif /* BCM_TRX_SUPPORT */

    if (SOC_IS_XGS3_SWITCH(unit)) {
        uint32 rate = 0;
        int enable, threshold = pps;
        enable = (flags & BCM_RATE_DLF) ? 1 : 0;
        soc_reg_field_set(unit, DLFBC_STORM_CONTROLr,
                          &rate, ENABLEf, enable);
        soc_reg_field_set(unit, DLFBC_STORM_CONTROLr,
                          &rate, THRESHOLDf, threshold);
        SOC_IF_ERROR_RETURN(WRITE_DLFBC_STORM_CONTROLr(unit, port, rate));
        return BCM_E_NONE;
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_rate_mcast_get
 * Description:
 *      Get rate limit for MCAST packets
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      pps - (OUT) Rate limit value in packets/second
 *      flags - (OUT) Bitmask with one or more of BCM_RATE_*
 *      port - Port number for which MCAST limit is requested
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_rate_mcast_get(int unit, int *pps, int *flags, int port)
{
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &port));
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    /* Check for Storm Control feature */
    if (!soc_feature(unit,soc_feature_storm_control)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return _bcm_trx_rate_get(unit, port, _BCM_TRX_RATE_PKT_MODE, 
                                 flags, BCM_RATE_MCAST, NULL, pps, NULL);
    }
#endif /* BCM_TRX_SUPPORT */

    if (SOC_IS_XGS3_SWITCH(unit)) {
        uint32 rate;

        SOC_IF_ERROR_RETURN(READ_MCAST_STORM_CONTROLr(unit, port, &rate));
        *flags = 0;
        *pps = 0;
        if (soc_reg_field_get(unit, MCAST_STORM_CONTROLr, rate, ENABLEf)) {
            *flags |= BCM_RATE_MCAST;
            *pps = soc_reg_field_get(unit, MCAST_STORM_CONTROLr,
                                     rate, THRESHOLDf);
        } 
        return BCM_E_NONE;
    }
#endif

    return bcm_esw_rate_get(unit, pps, flags);
}

/*
 * Function:
 *      bcm_rate_dlfbc_get
 * Description:
 *      Get rate limit for DLFBC packets
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      pps - (OUT) Rate limit value in packets/second
 *      flags - (OUT) Bitmask with one or more of BCM_RATE_*
 *      port - Port number for which DLFBC limit is requested
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      DLFBC: Destination Lookup Failure Broadcast
 */

int
bcm_esw_rate_dlfbc_get(int unit, int *pps, int *flags, int port)
{
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &port));
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    /* Check for Storm Control feature */
    if (!soc_feature(unit,soc_feature_storm_control)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return _bcm_trx_rate_get(unit, port, _BCM_TRX_RATE_PKT_MODE, 
                                 flags, BCM_RATE_DLF, NULL, pps, NULL);
    }
#endif /* BCM_TRX_SUPPORT */
    if (SOC_IS_XGS3_SWITCH(unit)) {
        uint32 rate;

        SOC_IF_ERROR_RETURN(READ_DLFBC_STORM_CONTROLr(unit, port, &rate));
 
        *flags = 0;
        *pps = 0;
        if (soc_reg_field_get(unit, DLFBC_STORM_CONTROLr, rate, ENABLEf)) {
            *flags |= BCM_RATE_DLF;
            *pps = soc_reg_field_get(unit, DLFBC_STORM_CONTROLr,
                                     rate, THRESHOLDf);
        }
        return BCM_E_NONE;
    }
#endif

    return bcm_esw_rate_get(unit, pps, flags);
}

/*
 * Function:
 *      bcm_rate_bcast_get
 * Description:
 *      Get rate limit for BCAST packets
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      pps - (OUT) Rate limit value in packets/second
 *      flags - (OUT) Bitmask with one or more of BCM_RATE_*
 *      port - Port number for which BCAST limit is requested
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_rate_bcast_get(int unit, int *pps, int *flags, int port)
{
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &port));
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    /* Check for Storm Control feature */
    if (!soc_feature(unit,soc_feature_storm_control)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return _bcm_trx_rate_get(unit, port, _BCM_TRX_RATE_PKT_MODE, 
                                 flags, BCM_RATE_BCAST, NULL, pps, NULL);
    }
#endif /* BCM_TRX_SUPPORT */
    if (SOC_IS_XGS3_SWITCH(unit)) {
        uint32 rate;

        SOC_IF_ERROR_RETURN(READ_BCAST_STORM_CONTROLr(unit, port, &rate));
        *flags = 0;
        *pps = 0;
        if (soc_reg_field_get(unit, BCAST_STORM_CONTROLr, rate, ENABLEf)) {
            *flags |= BCM_RATE_BCAST;
            *pps = soc_reg_field_get(unit, BCAST_STORM_CONTROLr,
                                     rate, THRESHOLDf);
        }
        return BCM_E_NONE;
    }
#endif

    return bcm_esw_rate_get(unit, pps, flags);
}

/*
 * Function:
 *      bcm_rate_type_get
 * Description:
 *      Front end to bcm_*cast_rate_get functions.
 *      Uses a single data structure to read from
 *      all the 3 rate control registers
 * Parameters:
 *      unit - unit number
 *      rl - (OUT) data structure containing info to be acquired from the
 *           rate control registers
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_rate_type_get(int unit, bcm_rate_limit_t *rl)
{
    int port;
    int flags = 0;

    bcm_rate_limit_t_init(rl);

    /* get first enabled ethernet port */
    PBMP_E_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_rate_bcast_get(unit, &rl->br_bcast_rate, &flags, port));
        rl->flags |= flags;
        BCM_IF_ERROR_RETURN
            (bcm_esw_rate_mcast_get(unit, &rl->br_mcast_rate, &flags, port));
        rl->flags |= flags;
        BCM_IF_ERROR_RETURN
            (bcm_esw_rate_dlfbc_get(unit, &rl->br_dlfbc_rate, &flags, port));
        rl->flags |= flags;
        break;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_rate_get
 * Description:
 *      Get rate limit and on/off state of
 *      DLF, MCAST, and BCAST limiting
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      pps - (OUT) Place to store returned rate limit value
 *      flags - (OUT) Place to store returned flag bitmask with
 *              one or more of BCM_RATE_*
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 * Notes:
 *      Actually returns the rate for BCAST only, but assumes the
 *      bcm_rate_set call was used so DLF, MCAST and BCAST should be equal.
 */

int
bcm_esw_rate_get(int unit, int *pps, int *flags)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        uint32 rate;
        int port;
        PBMP_ALL_ITER(unit, port) {
            break;
        }
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            uint32 fmask;
            fmask = BCM_RATE_DLF | BCM_RATE_MCAST | BCM_RATE_BCAST;
            return _bcm_trx_rate_get(unit, port, _BCM_TRX_RATE_PKT_MODE, 
                                     flags, fmask, NULL, pps, NULL);
        }
#endif /* BCM_TRX_SUPPORT */

        *flags = 0;
        SOC_IF_ERROR_RETURN(READ_BCAST_STORM_CONTROLr(unit, port, &rate));
        if (soc_reg_field_get(unit, BCAST_STORM_CONTROLr, rate, ENABLEf)) {
            *pps = soc_reg_field_get(unit, BCAST_STORM_CONTROLr, rate,
                                     THRESHOLDf);
            *flags |= BCM_RATE_BCAST;
        }
        SOC_IF_ERROR_RETURN(READ_MCAST_STORM_CONTROLr(unit, port, &rate));
        if (soc_reg_field_get(unit, MCAST_STORM_CONTROLr, rate, ENABLEf)) {
            *pps = soc_reg_field_get(unit, MCAST_STORM_CONTROLr, rate,
                                     THRESHOLDf);
            *flags |= BCM_RATE_MCAST;
        }
        SOC_IF_ERROR_RETURN(READ_DLFBC_STORM_CONTROLr(unit, port, &rate));
        if (soc_reg_field_get(unit, DLFBC_STORM_CONTROLr, rate, ENABLEf)) {
            *pps = soc_reg_field_get(unit, DLFBC_STORM_CONTROLr, rate,
                                     THRESHOLDf);
            *flags |= BCM_RATE_DLF;
        }
        return BCM_E_NONE;
    }
#endif

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_esw_rate_bandwidth_get
 * Description:
 *      Get rate bandwidth limiting parameters
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Bitmask with one of the following:
 *              BCM_RATE_BCAST
 *              BCM_RATE_MCAST
 *              BCM_RATE_DLF
 *              BCM_RATE_MODE_PACKETS: pps vs bps mode.
 *      kbits_sec - Rate in kilobits (1000 bits) per second.
 *                  Rate of 0 disabled rate limiting.
 *      kbits_burst - Maximum burst size in kilobits(1000 bits)
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_UNAVAIL - Not supported
 *      BCM_E_XXX - Error.
 */

int 
bcm_esw_rate_bandwidth_get(int unit, bcm_port_t port, int flags, 
                           uint32 *kbits_sec, uint32 *kbits_burst)
{
#ifdef BCM_TRX_SUPPORT
    int mode;
#endif /* BCM_TRX_SUPPORT */

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &port));
    }
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        mode = ((flags & BCM_RATE_MODE_PACKETS) ?
                _BCM_TRX_RATE_PKT_MODE : _BCM_TRX_RATE_BYTE_MODE);
        flags &= ~BCM_RATE_MODE_PACKETS;
        return _bcm_trx_rate_get(unit, port, mode, 
                                 NULL, flags, kbits_sec, NULL,
                                 kbits_burst);
    } 
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_BANDWIDTH_RATE_METER)
    if (soc_feature(unit,soc_feature_storm_control)) {
        uint32  enable_entry;

        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }

        *kbits_sec = *kbits_burst = 0;
        BCM_IF_ERROR_RETURN(
            READ_SC_BYTE_METER_CONFIGr(unit, port, &enable_entry));

        if (flags & BCM_RATE_BCAST) {
            if (soc_reg_field_get(unit, SC_BYTE_METER_CONFIGr, enable_entry,
                                  BCAST_ENABLEf)) {
                _bcm_rate_bw_get(unit, port, FP_SC_BCAST_METER_TABLEm, 
                                 kbits_sec, kbits_burst);
            }
        } else if (flags & BCM_RATE_MCAST) {
            if (soc_reg_field_get(unit, SC_BYTE_METER_CONFIGr, enable_entry,
                                  MCAST_ENABLEf)) {
                _bcm_rate_bw_get(unit, port, FP_SC_MCAST_METER_TABLEm, 
                                 kbits_sec, kbits_burst);
            }
        } else if (flags & BCM_RATE_DLF) {
            if (!SOC_IS_SHADOW(unit)) {
                if (soc_reg_field_get(unit, SC_BYTE_METER_CONFIGr, enable_entry,
                                      DLFBC_ENABLEf)) {
                    _bcm_rate_bw_get(unit, port, FP_SC_DLF_METER_TABLEm,
                                     kbits_sec, kbits_burst);
                }
            }
        }

        return BCM_E_NONE;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_rate_bandwidth_set
 * Description:
 *      Set rate bandwidth limiting parameters
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Bitmask with one of the following:
 *              BCM_RATE_BCAST
 *              BCM_RATE_MCAST
 *              BCM_RATE_DLF
 *              BCM_RATE_MODE_PACKETS: pps vs bps mode.
 *      kbits_sec - Rate in kilobits (1000 bits) per second.
 *                  Rate of 0 disables rate limiting.
 *      kbits_burst - Maximum burst size in kilobits(1000 bits)
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_UNAVAIL - Not supported
 *      BCM_E_XXX - Error.
 */

int 
bcm_esw_rate_bandwidth_set(int unit, bcm_port_t port, int flags, 
                           uint32 kbits_sec, uint32 kbits_burst)
{
#ifdef BCM_TRX_SUPPORT
    int mode;
#endif /* BCM_TRX_SUPPORT */

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &port));
    }
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        uint32 fmask;
        mode = ((flags & BCM_RATE_MODE_PACKETS) ?
                _BCM_TRX_RATE_PKT_MODE : _BCM_TRX_RATE_BYTE_MODE);
        flags &= ~BCM_RATE_MODE_PACKETS;
        fmask = flags;

        if (kbits_sec == BCM_RATE_DISABLE) { /* disable storm control */
            flags = 0;
        }
        if (kbits_sec == BCM_RATE_BLOCK) {
            kbits_sec = 0;
            kbits_burst = 0;
        }
        return (_bcm_trx_rate_set(unit, port, mode, 
                                  flags, fmask, kbits_sec, kbits_burst)); 
    } 
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_BANDWIDTH_RATE_METER)

    if (soc_feature(unit,soc_feature_storm_control)) {
        uint32      enable_entry, old_entry;
        uint32      refresh;

        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        if (kbits_sec > METERING_TOKEN_MAX * TOKEN_BUCKET_SIZE) {
            return BCM_E_PARAM;
        }
        if (kbits_burst > kbits_2_bucket_size[METER_BUCKETSIZE_MAX]) {
            return BCM_E_PARAM;
        }

        if (!(flags & BCM_RATE_ALL)) {
            return BCM_E_PARAM;
        }

        /*    coverity[equality_cond]    */
        if (!kbits_sec || !kbits_burst) { 
            /* metering is disabled on port - program tables to max values */
            BCM_IF_ERROR_RETURN(
                READ_SC_BYTE_METER_CONFIGr(unit, port, &enable_entry));
            
            old_entry = enable_entry;
            if (flags & BCM_RATE_BCAST) {
                soc_reg_field_set(unit, SC_BYTE_METER_CONFIGr, &enable_entry,
                                  BCAST_ENABLEf, 0);

                _bcm_rate_bw_set(unit, port, FP_SC_BCAST_METER_TABLEm, 
                                 METERING_TOKEN_MAX, METER_BUCKETSIZE_MAX); 
            }
            if (flags & BCM_RATE_MCAST) {
                soc_reg_field_set(unit, SC_BYTE_METER_CONFIGr, &enable_entry,
                                  MCAST_ENABLEf, 0);

                _bcm_rate_bw_set(unit, port, FP_SC_MCAST_METER_TABLEm, 
                                 METERING_TOKEN_MAX, METER_BUCKETSIZE_MAX); 
            }
            if (flags & BCM_RATE_DLF) {
                if (!SOC_IS_SHADOW(unit)) {
                    soc_reg_field_set(unit, SC_BYTE_METER_CONFIGr, &enable_entry,
                                      DLFBC_ENABLEf, 0);
                    _bcm_rate_bw_set(unit, port, FP_SC_DLF_METER_TABLEm, 
                                     METERING_TOKEN_MAX, METER_BUCKETSIZE_MAX); 
                }
            }
            if (old_entry != enable_entry) {
                BCM_IF_ERROR_RETURN(
                    WRITE_SC_BYTE_METER_CONFIGr(unit, port, enable_entry));
            }

            return BCM_E_NONE; 
        }

        refresh = (kbits_sec + TOKEN_BUCKET_SIZE - 1) / TOKEN_BUCKET_SIZE ;
        if ( refresh >= METERING_TOKEN_MAX){
            refresh = METERING_TOKEN_MAX; 
        }

        BCM_IF_ERROR_RETURN(
            READ_SC_BYTE_METER_CONFIGr(unit, port, &enable_entry));

        old_entry = enable_entry;

        if (flags & BCM_RATE_BCAST) {
            soc_reg_field_set(unit, SC_BYTE_METER_CONFIGr, &enable_entry,
                              BCAST_ENABLEf, 1);

            _bcm_rate_bw_set(unit, port, FP_SC_BCAST_METER_TABLEm, refresh, 
                             _bcm_kbits_to_bucket_size(kbits_burst)); 
        }
        if (flags & BCM_RATE_MCAST) {
            soc_reg_field_set(unit, SC_BYTE_METER_CONFIGr, &enable_entry,
                              MCAST_ENABLEf, 1);

            _bcm_rate_bw_set(unit, port, FP_SC_MCAST_METER_TABLEm, refresh, 
                             _bcm_kbits_to_bucket_size(kbits_burst)); 
        }
        if (flags & BCM_RATE_DLF) {
            if (!SOC_IS_SHADOW(unit)) {
                soc_reg_field_set(unit, SC_BYTE_METER_CONFIGr, &enable_entry,
                              DLFBC_ENABLEf, 1);

                _bcm_rate_bw_set(unit, port, FP_SC_DLF_METER_TABLEm, refresh, 
                                 _bcm_kbits_to_bucket_size(kbits_burst)); 
            }
        }
        if (old_entry != enable_entry) {
            BCM_IF_ERROR_RETURN(
                WRITE_SC_BYTE_METER_CONFIGr(unit, port, enable_entry));
        }

        return BCM_E_NONE;
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_rate_packet_get
 * Description:
 *      Get rate packet limiting parameters
 * Parameters:
 *      unit      - Device number
 *      pkt_rate  - packet rate information
 * Returns:
 *      BCM_E_NONE    - Success.
 *      BCM_E_UNAVAIL - Not supported
 *      BCM_E_XXX     - Error.
 */

int 
bcm_esw_rate_packet_get(int unit, bcm_port_t port, bcm_rate_packet_t *pkt_rate)
{
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &port));
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return _bcm_trx_rate_get(unit, port, _BCM_TRX_RATE_PKT_MODE, 
                                 (int *)&pkt_rate->flags, pkt_rate->flags, NULL, 
                                 &pkt_rate->pps, (uint32 *)(&pkt_rate->kbits_burst));
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_rate_packet_set
 * Description:
 *      Set rate packet limiting parameters
 * Parameters:
 *      unit      - Device number
 *      pkt_rate  - packet rate information
 * Returns:
 *      BCM_E_NONE    - Success.
 *      BCM_E_UNAVAIL - Not supported
 *      BCM_E_XXX     - Error.
 */

int 
bcm_esw_rate_packet_set(int unit, bcm_port_t port, bcm_rate_packet_t *pkt_rate)
{
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &port));
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return (_bcm_trx_rate_set(unit, port, _BCM_TRX_RATE_PKT_MODE, 
                                  pkt_rate->flags, pkt_rate->flags, pkt_rate->pps, 
                                  (uint32)(pkt_rate->kbits_burst))); 
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}



