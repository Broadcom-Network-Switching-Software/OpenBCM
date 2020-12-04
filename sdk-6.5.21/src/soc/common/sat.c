/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Utility routines for managing Higig headers
 */

#include <shared/bsl.h>
#include <shared/bitop.h>
#include <assert.h>
#include <sal/types.h>
#include <soc/drv.h>
#include <soc/types.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <sal/limits.h>
#include <soc/shared/sat.h>
#include <appl/diag/system.h>
#include <soc/scache.h>
#if defined(BCM_DPP_SUPPORT)
#include <soc/dpp/ARAD/arad_chip_defines.h>
#endif

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#endif

#ifdef BCM_SAT_SUPPORT

#ifdef BCM_WARM_BOOT_SUPPORT
#define SOC_SAT_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_SAT_WB_CURRENT_VERSION            SOC_SAT_WB_VERSION_1_0
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_SAT_GTF_STAT_FIX
typedef struct _soc_sat_data_s {
    uint64 gtf_stat_value[16];
} _soc_sat_data_t;
#endif

#ifdef BCM_SAT_GTF_STAT_FIX
_soc_sat_data_t soc_sat_data[BCM_UNITS_MAX];
#endif

static soc_sat_handler_t _soc_sat_handler[BCM_UNITS_MAX];

typedef struct _soc_sat_gtf_pkt_hdr_tbl_info_s {
    uint32 pkt_hdr_lsb[SOC_SAT_PKT_HERDER_LSB_FIELD_SIZE];
    uint32 pkt_hdr_msb[SOC_SAT_PKT_HERDER_MSB_FIELD_SIZE];
    uint32 pkt_hdr_len;
}_soc_sat_gtf_pkt_hdr_tbl_info_t;

typedef struct _soc_sat_gtf_stamp_field_tbl_info_s {
    uint32 stamp_2_ena;
    uint32 stamp_2_offset;
    uint32 stamp_2_shift;
    uint32 stamp_2_value;
    uint32 counter_8_ena;
    uint32 counter_8_offset;
    uint32 counter_8_shift;
    uint32 counter_8_mask;
    uint32 counter_8_inc_step;
    uint32 counter_8_period;
    uint32 counter_16_ena;
    uint32 counter_16_offset;
    uint32 counter_16_shift;
    uint32 counter_16_mask;
    uint32 counter_16_inc_step;
    uint32 counter_16_period;
}_soc_sat_gtf_stamp_field_tbl_info_t;

STATIC int soc_sat_dynamic_memory_access_set(int unit, uint32 enable)
{
  int
    rv;
  uint32 access_enable;

  rv = SOC_E_NONE;
  access_enable = (enable ? 1:0);

  rv = soc_reg_field32_modify(unit, OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, ENABLE_DYNAMIC_MEMORY_ACCESSf, access_enable);
  if (SOC_FAILURE(rv)) {
      LOG_ERROR(BSL_LS_SOC_SAT,
                (BSL_META_U(unit,
                            "%s\n"), soc_errmsg(rv)));
  }

  return rv;
}
/*****************************************************
*NAME
*  soc_msb_bit_on
*TYPE:
*  PROC
*DATE:
*  13-Nov-02
*FUNCTION:
*  Given number - x.
*  Return the msb bit number that is on.
*CALLING SEQUENCE:
*   soc_msb_bit_on(x)
*INPUT:
*  SOC_SAND_DIRECT:
*    const uint32 x -
*     Number to evaluate
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*     the bit number
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*    soc_msb_bit_on(0) = 0 -- definition
*    soc_msb_bit_on(1) = 0
*    soc_msb_bit_on(2) = 1
*    soc_msb_bit_on(3) = 1
*    soc_msb_bit_on(4) = 2
*    soc_msb_bit_on(5) = 2
*    soc_msb_bit_on(0xFF121212) = 31
*SEE ALSO:
 */
uint32
  soc_msb_bit_on(
    SOC_SAND_IN uint32 x_in
  )
{
  uint32
    n,
    x;
  int
    i;
   x = x_in;

  if (x == 0)
  {
    return(0);
  }

  n = 0;
  for (i=16 ; i>0 ; i>>=1)
  {
    if (x & ((~((uint32)0))<<i))
    {
      n += i;
      x >>= i;
    }
  }
  return n;
}

/*****************************************************
*NAME
*   soc_break_to_mnt_exp_round_down
*TYPE:
*  PROC
*DATE:
*  13-Nov-02
*FUNCTION:
*  Given: 1. Number - x, .
*         2. Number of mantissa bits - mnt_nof_bits
*         3. Number of exponent bits - exp_nof_bits
*  Return mantissa (mnt) and exponent (exp).
*  Such that, y = (mnt+mnt_inc)<<exp - is the closest smaller equal number than x.
*CALLING SEQUENCE:
*   soc_break_to_mnt_exp_round_down(x, mnt_nof_bits, exp_nof_bits, mnt_inc, mnt, exp)
*INPUT:
*  SOC_SAND_DIRECT:
*    const uint32 x -
*     Number to break
*    const uint32 mnt_nof_bits -
*     Size in bits of mantissa.
*     Range 1:10
*    const uint32 exp_nof_bits -
*     Size in bits of exponent
*     Range 1:10
*    const uint32 mnt_inc -
*     A number added to the mantissa for the calculation
*     Range 0:1
*    uint32* mnt -
*     Buffer to load mantissa result
*    uint32* exp -
*     Buffer to load exponent result
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_RET -
*
*  SOC_SAND_INDIRECT:
*    mnt, exp.
*REMARKS:
* 1. the mantissa and exponnent number of bits
*     ranges from 1 to 10 each. This range may
*     be enlarge with some minor code changes
*     (if needed at all)
*SEE ALSO:
 */
int
  soc_break_to_mnt_exp_round_down(
    SOC_SAND_IN  uint32 x,
    SOC_SAND_IN  uint32  mnt_nof_bits,
    SOC_SAND_IN  uint32  exp_nof_bits,
    SOC_SAND_IN  uint32  mnt_inc,
    SOC_SAND_OUT uint32* mnt,
    SOC_SAND_OUT uint32* exp
  )
{
  uint32
    y;
  uint32
    mnt_max,
    exp_max,
    msb_bit_on;
  int
    msb_bit_diff;
  int
    rv ;

  rv = SOC_E_NONE;

  /*
   * Inputs checking
   */
  if ( (mnt == NULL) ||
       (exp == NULL) ||
       (mnt_nof_bits > 16) ||
       (exp_nof_bits > 10) ||
       (mnt_nof_bits==0) ||
       (exp_nof_bits==0) ||
       (mnt_inc > 1)
     )
  {
    /*
     * Invalid Inputs
     */
    rv = SOC_E_PARAM ;
    goto exit ;
  }

  mnt_max = (0x1<<mnt_nof_bits)-1 ;
  exp_max = (0x1<<exp_nof_bits)-1 ;
  msb_bit_on = 0 ;
  msb_bit_diff = 0 ;

  if(exp_max>= (sizeof(uint32)*8-1))
  {
    /*
     * Case we got exp_max>=31,
     * make it only 30.
     */
    exp_max = sizeof(uint32)*8-2;
  }


  *mnt = 0;
  *exp = 0;

  if (x <= mnt_inc)
  { /*
     * x is too small
     * Put the smallest number it represent
     * Actually we should have returned ERR for (x < mnt_inc), because
     * there is no solution that gives a smaller equal number than x,
     * but for backwards compitability we'll return the smallest solution here
     * as well.
     */
    goto exit ;
  }
  y = ((1 /* mnt_max+mnt_inc */)<<exp_max)*mnt_max;
  if (x > y)
  { /*
     * x is too big for our represantation
     * Put the biggest number it can
     */
    *mnt = mnt_max;
    *exp = exp_max;
    goto exit ;
  }

  msb_bit_on = soc_msb_bit_on(x) ;

  msb_bit_diff = (msb_bit_on+1) - mnt_nof_bits;
  if (msb_bit_diff<=0)
  {
    msb_bit_diff = 0 ;
  }

  *mnt = mnt_max << msb_bit_diff ;
  *mnt &= x ;

  /* decrease mnt_inc .. because the reverse calculation is (mnt+mnt_inc)<<exp */
  if ((*mnt) >= mnt_inc)
  {
    (*mnt) -= mnt_inc;
  }
  else
  {
    if ( (*exp) > 0)
    {
      (*exp) -- ;
    }
    else
    {
      /* The function does not reach here.*/
      *exp = 0 ;
    }
  }
  *mnt >>= msb_bit_diff;
  *exp = msb_bit_diff ;
exit:
  return rv ;
}

/*****************************************************
*NAME
*   soc_break_to_mnt_exp_round_up
*TYPE:
*  PROC
*DATE:
*  13-Nov-02
*FUNCTION:
*  Get a number, x.
*  Return mantissa (mnt) and exponent (exp).
*  Such that, y = (mnt+mnt_inc)<<exp is the closest bigger equal number than x.
*CALLING SEQUENCE:
*   soc_break_to_mnt_exp_round_up(x, mnt_nof_bits, exp_nof_bits, mnt_inc, mnt, exp)
*INPUT:
*  SOC_SAND_DIRECT:
*    const uint32 x -
*     Number to dis-assemble
*    const uint32 mnt_nof_bits -
*     Size in bits of mantissa.
*    const uint32 exp_nof_bits -
*     Size in bits of exponent
*    const uint32 mnt_inc -
*     A number added to the mantissa for the calculation
*     Range 0:1
*    uint32* mnt -
*     Buffer to load mantissa result
*    uint32* exp -
*     Buffer to load exponent result
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*
*  SOC_SAND_INDIRECT:
*    mnt, exp.
*REMARKS:
* None.
*SEE ALSO:
 */
int
  soc_break_to_mnt_exp_round_up(
    SOC_SAND_IN  uint32 x,
    SOC_SAND_IN  uint32  mnt_nof_bits,
    SOC_SAND_IN  uint32  exp_nof_bits,
    SOC_SAND_IN  uint32  mnt_inc,
    SOC_SAND_OUT uint32* mnt,
    SOC_SAND_OUT uint32* exp
  )
{
  uint32
    mnt_max,
    exp_max ;
  uint32
    num ;
  int
    rv ;

  mnt_max = (0x1<<mnt_nof_bits)-1,
  exp_max = (0x1<<exp_nof_bits)-1 ;
  rv = SOC_E_NONE;

  /*
   * soc_break_to_mnt_exp_round_down() is checking the valifdity of the inputs.
   */
  rv = soc_break_to_mnt_exp_round_down(x, mnt_nof_bits, exp_nof_bits, mnt_inc, mnt, exp);
  if (rv != SOC_E_NONE)
  {
    goto exit ;
  }
  if(exp_max>= (sizeof(uint32)*8-1))
  {
    exp_max = sizeof(uint32)*8-2;
  }

  num = (*mnt + mnt_inc) << (*exp);
  if (num<x)
  {
    /* Round it up.*/
    (*mnt) ++;
    if (*mnt>mnt_max)
    {
      *mnt = *mnt/2;
      (*exp) ++;
      if ((*exp)> exp_max)
      {
        *exp = exp_max ;
        *mnt = mnt_max ;
      }
    }
  }

exit:
  return rv ;
}

STATIC soc_error_t
  soc_sat_packet_config_exp_payload_uint8_to_long(
    SOC_SAND_IN  uint8           exp_payload_u8[SOC_SAT_PAYLOAD_MAX_PATTERN_NOF_U8],
    SOC_SAND_IN  uint8           num_of_bytes,
    SOC_SAND_IN  uint32          is_reverse,    
    SOC_SAND_OUT uint32          exp_payload_long[SOC_SAT_PAYLOAD_MAX_PATTERN_NOF_U32]
  )
{
  uint32
    tmp;
  uint32
    char_indx,
    long_indx,
    write_to;
  uint32
    first_word = 0,
    second_word = 1, 
    last_word = 0;
  
    if (num_of_bytes > SOC_SAT_PAYLOAD_MAX_PATTERN_NOF_U8) {
        return SOC_E_PARAM;
    }

    last_word = (num_of_bytes > (SOC_SAT_PAYLOAD_MAX_PATTERN_NOF_U8/2)) ? second_word : first_word;
    write_to = (is_reverse ? (sizeof(uint32) - 1): 0);
    exp_payload_long[first_word] = 0;
    exp_payload_long[second_word] = 0;
    long_indx = (is_reverse ? last_word: first_word);

    for (char_indx = 0; char_indx < num_of_bytes; ++char_indx)
    {
        if (char_indx == sizeof(uint32)) {
            long_indx = (is_reverse ? 0: 1);
            write_to = (is_reverse ? (sizeof(uint32) - 1) : 0);
        }

        tmp = exp_payload_u8[char_indx];
        exp_payload_long[long_indx] |= (tmp << (SAL_CHAR_BIT * write_to));
        write_to = is_reverse ? (write_to-1) : (write_to+1);            
    }

    return SOC_E_NONE;
}

STATIC int
_soc_sat_gtf_construct_packet_header(
    SOC_SAND_IN int unit,
    SOC_SAND_IN bcm_pkt_t *pkt_hdr,
    SOC_SAND_OUT _soc_sat_gtf_pkt_hdr_tbl_info_t *pkt_hdr_tbl_info
    )
{
    uint32 pkt_data = 0;
    uint32 pkt_hdr_lsb_bits = 0, pkt_hdr_msb_bits = 0, msb_bits_remainder = 0, lsb_bits_remainder = 0;
    uint32 bytes_copy, bytes_copied;
    uint32 offset = 0;
    int i = 0;
    int rv = SOC_E_NONE;

    if (pkt_hdr == NULL) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Parameter pkt_hdr is NULL\n")));
        rv = SOC_E_PARAM;
        goto exit;
    }

    if (pkt_hdr_tbl_info == NULL) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Parameter pkt_hdr_tbl_info is NULL\n")));
        rv = SOC_E_PARAM;
        goto exit;
    }

    if (SOC_IS_QAX(unit)) {
        pkt_hdr_lsb_bits = soc_mem_field_length(unit, OAMP_SAT_TX_EVC_PRM_ENTRY_1m, PKT_HEADER_LSBf);
        pkt_hdr_msb_bits = soc_mem_field_length(unit, OAMP_SAT_TX_EVC_PRM_ENTRY_2m, PKT_HEADER_MSBf);
    }else {
        pkt_hdr_lsb_bits = soc_mem_field_length(unit, OAMP_SAT_TX_EVC_PARAMS_ENTRY_1m, PKT_HEADER_LSBf);
        pkt_hdr_msb_bits = soc_mem_field_length(unit, OAMP_SAT_TX_EVC_PARAMS_ENTRY_2m, PKT_HEADER_MSBf);
    }

    offset = pkt_hdr_msb_bits - 8;
    bytes_copy = pkt_hdr_msb_bits/8;

    for (i = 0 ; (i < bytes_copy) && (i < pkt_hdr->pkt_data[0].len); i++){
        pkt_data = pkt_hdr->pkt_data[0].data[i];
        SHR_BITCOPY_RANGE(pkt_hdr_tbl_info->pkt_hdr_msb, offset, &pkt_data, 0, 8);
        offset -= 8;
    }
    bytes_copied = i;

    if (bytes_copied < pkt_hdr->pkt_data[0].len) {
        msb_bits_remainder = pkt_hdr_msb_bits%8;
        lsb_bits_remainder = 8 - msb_bits_remainder;
        if (msb_bits_remainder != 0) {
		    offset = 0;
            pkt_data = pkt_hdr->pkt_data[0].data[bytes_copied];
            SHR_BITCOPY_RANGE(pkt_hdr_tbl_info->pkt_hdr_msb, offset, &pkt_data, (8 - msb_bits_remainder), msb_bits_remainder);

            offset = pkt_hdr_lsb_bits - lsb_bits_remainder;
            SHR_BITCOPY_RANGE(pkt_hdr_tbl_info->pkt_hdr_lsb, offset, &pkt_data, 0, lsb_bits_remainder);

            bytes_copied += 1;
        }
    }

    if (bytes_copied < pkt_hdr->pkt_data[0].len) {
        offset = pkt_hdr_lsb_bits - lsb_bits_remainder - 8;
        bytes_copy = (pkt_hdr_lsb_bits - lsb_bits_remainder)/8;
        for (i = 0 ; i < bytes_copy; i++){
            pkt_data = pkt_hdr->pkt_data[0].data[bytes_copied + i];
            SHR_BITCOPY_RANGE(pkt_hdr_tbl_info->pkt_hdr_lsb, offset, &pkt_data, 0, 8);
            offset -= 8;
        }
        bytes_copied += i;
    }

    pkt_hdr_tbl_info->pkt_hdr_len = pkt_hdr->pkt_data[0].len;

exit:
    return rv;
}

STATIC int
_soc_sat_gtf_construct_stamp_field(
    SOC_SAND_IN int unit,
    SOC_SAND_IN soc_sat_gtf_packet_edit_t *pkt_edit,
    SOC_SAND_OUT _soc_sat_gtf_stamp_field_tbl_info_t *stamp_field_info
    )
{
    int rv = SOC_E_NONE;
    const uint32 field_2_cnt_8_mask[7] = {0xff, 0xe0, 0xc0, 0xff, 0xff, 0xff, 0xff};
    const uint32 field_2_cnt_16_mask[7] = {0xffff, 0xe000, 0xc000, 0xffff, 0xffff, 0xfff0, 0xfff0};
    const soc_sat_stamp_t *tmp_stamp;
    int i = 0;
    int tmp_stamp_offset_bits = 0;

    if (pkt_edit == NULL) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Parameter pkt_edit is NULL\n")));
        rv = SOC_E_PARAM;
        goto exit;
    }

    if (stamp_field_info == NULL) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Parameter stamp_field_info is NULL\n")));
        rv = SOC_E_PARAM;
        goto exit;
    }

    for (i = 0; i < pkt_edit->number_of_stamps; i++) {
        tmp_stamp = &(pkt_edit->stamps[i]);

        if (tmp_stamp->stamp_type == socSatStampConstant2Bit) {
            tmp_stamp_offset_bits = tmp_stamp->offset;
            stamp_field_info->stamp_2_ena = 1;
            if (tmp_stamp_offset_bits%8 == 7) {
                stamp_field_info->stamp_2_offset = tmp_stamp_offset_bits/8 + 1; 
                stamp_field_info->stamp_2_shift = 7; 
            }
            else {
                stamp_field_info->stamp_2_offset = tmp_stamp_offset_bits/8;
                stamp_field_info->stamp_2_shift = 8 - 2 - tmp_stamp_offset_bits%8;
            }
            stamp_field_info->stamp_2_value = tmp_stamp->value;
        }
        else if (tmp_stamp->stamp_type == socSatStampCounter8Bit) {
            tmp_stamp_offset_bits = tmp_stamp->offset;
            stamp_field_info->counter_8_ena = 1;
            stamp_field_info->counter_8_offset = (tmp_stamp_offset_bits/8) + ((tmp_stamp_offset_bits%8)?1:0);
            stamp_field_info->counter_8_shift = (8 - (tmp_stamp_offset_bits%8))%8;
            stamp_field_info->counter_8_mask = field_2_cnt_8_mask[tmp_stamp->field_type];
            stamp_field_info->counter_8_inc_step = tmp_stamp->inc_step;
            stamp_field_info->counter_8_period = tmp_stamp->inc_period_packets;
        }
        else if (tmp_stamp->stamp_type == socSatStampCounter16Bit) {
            tmp_stamp_offset_bits = tmp_stamp->offset;
            stamp_field_info->counter_16_ena = 1;
            stamp_field_info->counter_16_offset = (tmp_stamp_offset_bits/8) + ((tmp_stamp_offset_bits%8)?1:0); 
            stamp_field_info->counter_16_shift = (8 - (tmp_stamp_offset_bits%8))%8;
            stamp_field_info->counter_16_mask = field_2_cnt_16_mask[tmp_stamp->field_type];
            stamp_field_info->counter_16_inc_step = tmp_stamp->inc_step;
            stamp_field_info->counter_16_period = tmp_stamp->inc_period_packets;
        }
    }

exit:
    return rv;
}

STATIC int
_soc_sat_clock_per_cycle_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 clock_per_cycle)
{
    int rv = SOC_E_NONE;
    uint64 field64;

    rv = READ_OAMP_SAT_GEN_CONFIGr(unit, &field64);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
            (BSL_META_U(unit,
                "Fail(%s) in read OAMP_SAT_GEN_CONFIGr\n"), soc_errmsg(rv)));
        return rv;
    }

    soc_reg64_field32_set(unit,OAMP_SAT_GEN_CONFIGr, &field64, RATE_NUM_CLKS_CYCLEf, clock_per_cycle);

    rv = WRITE_OAMP_SAT_GEN_CONFIGr(unit, field64);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
            (BSL_META_U(unit,
                "Fail(%s) in write OAMP_SAT_GEN_CONFIGr\n"), soc_errmsg(rv)));
        return rv;
    }

    return rv;
}

STATIC int
_soc_sat_gtf_rate_to_internal_rate_convert(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 rate,
    SOC_SAND_IN  uint32 flags,
    SOC_SAND_IN  uint32 mnt_nof_bits,
    SOC_SAND_IN  uint32 exp_nof_bits,
    SOC_SAND_IN  uint32 pps_granularity,
    SOC_SAND_OUT uint32 *rate_mnt_val,
    SOC_SAND_OUT uint32 *rate_exp_val
    )
{
    int rv = SOC_E_NONE;
    uint32 fld_val = 0;
    uint64 field64;

    uint32 bytes_per_cycle = 0;
    uint32 cycles_per_sec = 0;
    uint32 clocks_per_sec = 0;
    uint32 clocks_per_cycle = 0;
    uint32 freq_hz = 0;
    int granularity = 1000;	/* 1 kbits per sec */

    if (rate_mnt_val == NULL) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Parameter rate_mnt_val is NULL\n")));
        rv = SOC_E_PARAM;
        return rv;
    }

    if (rate_exp_val == NULL) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Parameter rate_exp_val is NULL\n")));
        rv = SOC_E_PARAM;
        return rv;
    }

    if(SOC_IS_QAX(unit) && (flags & SOC_SAT_GTF_RATE_WITH_GRANULARITY) && (pps_granularity == 0)){
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Parameter granularity can't be zero \n")));
        rv = SOC_E_PARAM;
        return rv;
    }

    /* 1. caculate cycle per sec */
    rv = soc_reg_above_64_field64_read(unit, OAMP_TIMER_CONFIGr, REG_PORT_ANY, 0, NUM_CLOCKS_SECf, &field64);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in read OAMP_TIMER_CONFIGr\n"), soc_errmsg(rv)));
        return rv;
    }
    fld_val = COMPILER_64_LO(field64);
    clocks_per_sec = fld_val;

    rv = soc_reg_above_64_field64_read(unit, OAMP_SAT_GEN_CONFIGr, REG_PORT_ANY, 0, RATE_NUM_CLKS_CYCLEf, &field64);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in read OAMP_SAT_GEN_CONFIGr\n"), soc_errmsg(rv)));
        return rv;
    }
    fld_val = COMPILER_64_LO(field64);
    clocks_per_cycle = fld_val;

    if (SOC_IS_QAX(unit) && (flags & SOC_SAT_GTF_RATE_IN_PACKETS)) {
        clocks_per_cycle = clocks_per_sec;

        if (SOC_IS_QAX(unit) && (flags & SOC_SAT_GTF_RATE_WITH_GRANULARITY)) {
            clocks_per_cycle = clocks_per_sec/pps_granularity;
            bytes_per_cycle = rate /pps_granularity;
        }
        else{
            bytes_per_cycle = rate;
        }
        rv = _soc_sat_clock_per_cycle_set(unit,clocks_per_cycle);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                (BSL_META_U(unit,
                "Fail(%s) in _soc_sat_clock_per_cycle_set\n"), soc_errmsg(rv)));
        }
    }else {
        /* Time tickets */
        freq_hz = SOC_INFO(unit).frequency * 1000000;
#if defined(BCM_DPP_SUPPORT)
        if (SOC_IS_JERICHO(unit)) {
            freq_hz = arad_chip_kilo_ticks_per_sec_get(unit)*1000;
            granularity = 64000;	/* 64kbits per sec */
        }
#endif
        /* clocks/cycle = (clock/s) / (cycle/s) 
           bps mode: the minimum granularity is 1kpbs, and the minimum rate for BYTES_PER_CYCLE is 1byte (8bits),
           so the 1kpbs packet needs 125(1000/8) cycle to send.
        */
        rv = _soc_sat_clock_per_cycle_set(unit,(freq_hz/granularity)*8); /* 1kbits per sec(125 cycles per sec) */
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                (BSL_META_U(unit,
                "Fail(%s) in _soc_sat_clock_per_cycle_set\n"), soc_errmsg(rv)));
        }
        cycles_per_sec = clocks_per_sec/clocks_per_cycle;
        /* 2. caculate bytes per cycle, given rate(Units: kbps), so rate should *1000 / 8bits (125)*/ 
        bytes_per_cycle = rate * 125 / cycles_per_sec;
    }

    /* 3. caculate interval value for bytes per cycle */
    rv = soc_break_to_mnt_exp_round_up(
          bytes_per_cycle,
          mnt_nof_bits,
          exp_nof_bits,
          0,
          rate_mnt_val,
          rate_exp_val
        );
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in soc_break_to_mnt_exp_round_up\n"), soc_errmsg(rv)));
    }

    return rv;
}

STATIC int
_soc_sat_gtf_rate_from_internal_rate_convert(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 rate_mnt_val,
    SOC_SAND_IN  uint32 rate_exp_val,
    SOC_SAND_IN  uint32 flags,
    SOC_SAND_OUT uint32 *rate
    )
{
    int rv = SOC_E_NONE;
    uint32 fld_val = 0;
    uint64 field64;

    uint32 bytes_per_cycle = 0;
    uint32 cycles_per_sec = 0;
    uint32 clocks_per_sec = 0;
    uint32 clocks_per_cycle = 0;

    if (rate == NULL) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Parameter rate is NULL\n")));
        rv = SOC_E_PARAM;
        return rv;
    }

    /* 1. caculate cycle per sec */
    rv = soc_reg_above_64_field64_read(unit, OAMP_TIMER_CONFIGr, REG_PORT_ANY, 0, NUM_CLOCKS_SECf, &field64);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in read OAMP_TIMER_CONFIGr\n"), soc_errmsg(rv)));
        return rv;
    }
    fld_val = COMPILER_64_LO(field64);
    clocks_per_sec = fld_val;

    rv = soc_reg_above_64_field64_read(unit, OAMP_SAT_GEN_CONFIGr, REG_PORT_ANY, 0, RATE_NUM_CLKS_CYCLEf, &field64);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in read OAMP_SAT_GEN_CONFIGr\n"), soc_errmsg(rv)));
        return rv;
    }
    fld_val = COMPILER_64_LO(field64);
    clocks_per_cycle = fld_val;    

    cycles_per_sec = clocks_per_sec/clocks_per_cycle;

    /* 2. caculate bytes per cycle, given rate(Units: kbps) */
    bytes_per_cycle = (rate_mnt_val) * (1<<(rate_exp_val));

    if ((SOC_IS_QAX(unit)) && flags) {
        *rate = bytes_per_cycle;   
    }else {
        /* 3. get rate(Units: kbps) by bytes_per_cycle and cycles_per_sec */
        *rate = (bytes_per_cycle * cycles_per_sec) / 125;
    }

    return rv;
}

soc_error_t soc_sat_general_cfg_init (
    SOC_SAND_IN int unit,
    SOC_SAND_IN soc_sat_init_t *sat_init
    )
{
    int rv = SOC_E_NONE;
    uint64 data;
    uint32 flow_id_mapping_data = 0;
    uint32 entry_offset = 0;
#ifdef BCM_SAT_GTF_STAT_FIX
    int i;
#endif

#ifdef BCM_SAT_GTF_STAT_FIX
    for (i = 0; i < 16; i++) {
        COMPILER_64_ZERO(soc_sat_data[unit].gtf_stat_value[i]);
    }
#endif

    rv = READ_OAMP_SAT_GEN_CONFIGr(unit, &data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in read OAMP_SAT_GEN_CONFIGr\n"), soc_errmsg(rv)));
        return rv;
    }

    /* 1. number of clocks between pulses for leaky buckets */
    soc_reg64_field32_set(unit,OAMP_SAT_GEN_CONFIGr, &data, RATE_NUM_CLKS_CYCLEf, sat_init->rate_num_clks_cycle);

    /* 2. mode of cmic tod IF*/
    if (!SOC_IS_QAX(unit)) {
        soc_reg64_field32_set(unit,OAMP_SAT_GEN_CONFIGr, &data, CMIC_TOD_MODEf, sat_init->cmic_tod_mode);    
    }

    rv = WRITE_OAMP_SAT_GEN_CONFIGr(unit, data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in write OAMP_SAT_GEN_CONFIGr\n"), soc_errmsg(rv)));
        return rv;
    }

    rv = READ_OAMP_TIMER_CONFIGr(unit, &data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in read OAMP_TIMER_CONFIGr\n"), soc_errmsg(rv)));
        return rv;
    }

    /*3. Default value of OAMP_SAT_RX_FLOW_ID table is random, have to add codes to initialize it */
    for (entry_offset = 0; entry_offset < soc_mem_index_max(unit, OAMP_SAT_RX_FLOW_IDm);  entry_offset++) {
        rv = WRITE_OAMP_SAT_RX_FLOW_IDm(unit, MEM_BLOCK_ALL, entry_offset, &flow_id_mapping_data);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return rv;
        }
    }

    /* 4. number of system clock cycles in one second */
    soc_reg64_field32_set(unit,OAMP_TIMER_CONFIGr, &data, NUM_CLOCKS_SECf, sat_init->num_clks_sec);  
#ifdef BCM_SABER2_SUPPORT
    if(SOC_IS_SABER2(unit)) {
        uint32 clk_scan;
        /* 
           Number of system clock cycles in one MEP scan (1.66 ms), 
           Optimize calculation to avoid floating point arithmetic
         */
        clk_scan = (sat_init->num_clks_sec/SOC_SAT_100K_NUM) * SOC_SAT_MEP_100K_SCAN_IN_SEC;
        soc_reg64_field32_set(unit,OAMP_TIMER_CONFIGr, &data, NUM_CLOCKS_SCANf, clk_scan);
    }
#endif
    rv = WRITE_OAMP_TIMER_CONFIGr(unit, data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in write OAMP_TIMER_CONFIGr\n"), soc_errmsg(rv)));
    }

    return rv;
}

soc_error_t soc_sat_config_set  (
    SOC_SAND_IN int unit,
    SOC_SAND_IN soc_sat_config_t* conf
    )
{
    uint64 reg_val;
    uint32 field = 0;
    int rv = SOC_E_NONE;

    COMPILER_64_ZERO(reg_val);
    rv = READ_OAMP_SAT_GEN_CONFIGr(unit, &reg_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in read OAMP_SAT_GEN_CONFIGr\n"), soc_errmsg(rv)));
        return rv;
    }
    field = (conf->timestamp_format == socSATTimestampFormatNTP ? 1 : 0);
    soc_reg64_field32_set(unit, OAMP_SAT_GEN_CONFIGr, &reg_val, USE_NTP_TODf, field);
    field = (conf->config_flags & SOC_SAT_CONFIG_CRC_REVERSED_DATA ? 1 : 0);
    soc_reg64_field32_set(unit, OAMP_SAT_GEN_CONFIGr, &reg_val, REV_CRC_DATAf, field);
    field = (conf->config_flags & SOC_SAT_CONFIG_CRC_INVERT ? 1 : 0);
    soc_reg64_field32_set(unit, OAMP_SAT_GEN_CONFIGr, &reg_val, INV_CRC_RESf, field);
    field = (conf->config_flags & SOC_SAT_CONFIG_CRC_HIGH_RESET ? 1 : 0);
    soc_reg64_field32_set(unit, OAMP_SAT_GEN_CONFIGr, &reg_val, RST_CRC_F_1Z0f, field);
    field = (conf->config_flags & SOC_SAT_CONFIG_PRBS_USE_NXOR ? 1 : 0);
    soc_reg64_field32_set(unit, OAMP_SAT_GEN_CONFIGr, &reg_val, SAT_PRBS_USE_NXORf, field);
    if (!SOC_IS_QAX(unit)) { 
        field = (conf->timestamp_format == socSATTimestampFormatNTP ? 2 : 1);
        soc_reg64_field32_set(unit, OAMP_SAT_GEN_CONFIGr, &reg_val, CMIC_TOD_MODEf, field);
    }
    rv = WRITE_OAMP_SAT_GEN_CONFIGr(unit, reg_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in write OAMP_SAT_GEN_CONFIGr\n"), soc_errmsg(rv)));
    }

    return rv;
}

soc_error_t soc_sat_config_get  (
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT soc_sat_config_t* conf
    )
{
    int rv = SOC_E_NONE;
    uint64 reg_val;
    uint32 field = 0;

    COMPILER_64_ZERO(reg_val);
    rv = READ_OAMP_SAT_GEN_CONFIGr(unit, &reg_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in read OAMP_SAT_GEN_CONFIGr\n"), soc_errmsg(rv)));
        return rv;
    }
    field = soc_reg64_field32_get(unit, OAMP_SAT_GEN_CONFIGr, reg_val, USE_NTP_TODf);
    conf->timestamp_format = (field ? socSATTimestampFormatNTP : socSATTimestampFormatIEEE1588v1);

    field = soc_reg64_field32_get(unit, OAMP_SAT_GEN_CONFIGr, reg_val, REV_CRC_DATAf);
    if (field) {
        conf->config_flags |= SOC_SAT_CONFIG_CRC_REVERSED_DATA;
    }
    field = soc_reg64_field32_get(unit, OAMP_SAT_GEN_CONFIGr, reg_val, INV_CRC_RESf);
    if (field) {
        conf->config_flags |= SOC_SAT_CONFIG_CRC_INVERT;
    }
    field = soc_reg64_field32_get(unit, OAMP_SAT_GEN_CONFIGr, reg_val, RST_CRC_F_1Z0f);
    if (field) {
        conf->config_flags |= SOC_SAT_CONFIG_CRC_HIGH_RESET;
    }
    field = soc_reg64_field32_get(unit, OAMP_SAT_GEN_CONFIGr, reg_val, SAT_PRBS_USE_NXORf);
    if (field) {
        conf->config_flags |= SOC_SAT_CONFIG_PRBS_USE_NXOR;
    }

    return rv;
}

soc_error_t soc_sat_gtf_packet_gen_set (
    SOC_SAND_IN int unit, 
	SOC_SAND_IN soc_sat_gtf_t gtf_id,
    SOC_SAND_IN int priority,
	SOC_SAND_IN int enable
    )
{
    int rv = SOC_E_NONE;
    soc_reg_above_64_val_t data;
    int index;
    int pri = 0, start_pri = 0, stop_pri = 0;
#ifdef BCM_SAT_GTF_STAT_FIX
    uint32 stat_val;
#endif
    uint32 rate_pattern_mode = 0;

    SOC_REG_ABOVE_64_CLEAR(data);

    if (priority == SOC_SAT_GTF_OBJ_COMMON) {
        start_pri = SOC_SAT_GTF_OBJ_CIR;
        stop_pri = SOC_SAT_GTF_OBJ_EIR + 1;
    }
    else {
        start_pri = priority;
        stop_pri = start_pri + 1;
    }

    /* set packet gen for specified generator */
    for (pri = start_pri; pri < stop_pri; pri++) {
        index = SAT_TX_PER_GENERATOR_INDX(gtf_id, pri);

        rv = READ_OAMP_SAT_GEN_RATE_CONTROLr(unit, index, data);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in read OAMP_SAT_GEN_RATE_CONTROLr\n"), soc_errmsg(rv)));
            return rv;
        }
        rate_pattern_mode = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, BURST_MODE__Nf);

        /*Combined mode*/
        if (( rate_pattern_mode == 1)&&( enable == 1)) {
            /* Ratepattern mode need to reset the ITER CNT*/
            soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, STOP_ITER_CNT_RST__Nf, 1);
            rv = WRITE_OAMP_SAT_GEN_RATE_CONTROLr(unit, index, data);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SAT,
                    (BSL_META_U(unit,"Fail(%s) in write OAMP_SAT_GEN_RATE_CONTROLr\n"), soc_errmsg(rv)));
                return rv;
            }

            rv = READ_OAMP_SAT_GEN_RATE_CONTROLr(unit, index, data);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SAT,
                    (BSL_META_U(unit,
                    "Fail(%s) in read OAMP_SAT_GEN_RATE_CONTROLr\n"), soc_errmsg(rv)));
                return rv;
            }
            soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, STOP_ITER_CNT_RST__Nf, 0);
            rv = WRITE_OAMP_SAT_GEN_RATE_CONTROLr(unit, index, data);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SAT,
                   (BSL_META_U(unit,"Fail(%s) in write OAMP_SAT_GEN_RATE_CONTROLr\n"), soc_errmsg(rv)));
                return rv;
            }
        }

#ifdef BCM_SAT_GTF_STAT_FIX
        if (!SOC_IS_QAX(unit)) {
            stat_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, PACKET_COUNT__Nf);
            COMPILER_64_ADD_32(soc_sat_data[unit].gtf_stat_value[index], stat_val);
        }
#endif
        soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, PKT_GEN_EN__Nf, enable);    
        rv = WRITE_OAMP_SAT_GEN_RATE_CONTROLr(unit, index, data);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in write OAMP_SAT_GEN_RATE_CONTROLr\n"), soc_errmsg(rv)));
            return rv;
        }
    }

    return rv;
}

soc_error_t soc_sat_gtf_packet_gen_get (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_sat_gtf_t gtf_id,
    SOC_SAND_IN int priority,
    SOC_SAND_OUT int *enable
    )
{
    int rv = SOC_E_NONE;
    soc_reg_above_64_val_t data;
    int index;
#ifdef BCM_SAT_GTF_STAT_FIX
    uint32 stat_val;
#endif

    SOC_REG_ABOVE_64_CLEAR(data);

    if (priority == SOC_SAT_GTF_OBJ_COMMON) {
        rv = SOC_E_PARAM;
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s): priority for soc_sat_gtf_packet_gen_get should be one of %d and %d \n"), soc_errmsg(rv), SOC_SAT_GTF_OBJ_CIR, SOC_SAT_GTF_OBJ_EIR));
        return rv;       
    }

    /* get packet gen from specified generator */
    index = SAT_TX_PER_GENERATOR_INDX(gtf_id, priority);

    rv = READ_OAMP_SAT_GEN_RATE_CONTROLr(unit, index, data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in read OAMP_SAT_GEN_RATE_CONTROLr\n"), soc_errmsg(rv)));
        return rv;
    }
#ifdef BCM_SAT_GTF_STAT_FIX
    if (!SOC_IS_QAX(unit)) {
        stat_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, PACKET_COUNT__Nf);
        COMPILER_64_ADD_32(soc_sat_data[unit].gtf_stat_value[index], stat_val);
    }
#endif
    (*enable) = soc_reg_above_64_field32_get(unit,OAMP_SAT_GEN_RATE_CONTROLr, data, PKT_GEN_EN__Nf);

    return rv;
}

soc_error_t soc_sat_gtf_packet_config (
    SOC_SAND_IN int unit,
    SOC_SAND_IN soc_sat_gtf_t gtf_id, 
    SOC_SAND_IN soc_sat_gtf_packet_config_t *config
    )
{
    int rv = SOC_E_NONE;
    soc_reg_above_64_val_t tx_evc_params_entry_1_data, tx_evc_params_entry_2_data;
    soc_reg_above_64_val_t evc_rate_control_data;
    uint32 field_data;
    soc_field_t pkt_length_fields[SOC_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS] = {
        PKT_LENGTH_0f, 
        PKT_LENGTH_1f,
        PKT_LENGTH_2f,
        PKT_LENGTH_3f,
        PKT_LENGTH_4f,
        PKT_LENGTH_5f,
        PKT_LENGTH_6f,
        PKT_LENGTH_7f};
    _soc_sat_gtf_pkt_hdr_tbl_info_t pkt_hdr_tbl_info;
    uint32 payload_rpt_pattern[SOC_SAT_PAYLOAD_MAX_PATTERN_NOF_U32] = {0};
    uint32 payload_type;   
    uint32 length_pattern[SOC_SAT_PKT_LENGTH_PATTERN_FIELD_SIZE] = {0};
    _soc_sat_gtf_stamp_field_tbl_info_t stamp_field_info[SOC_SAT_GTF_NUM_OF_PRIORITIES];
    uint32 sequence_ena[SOC_SAT_GTF_NUM_OF_PRIORITIES] = {0}, sequence_offset[SOC_SAT_GTF_NUM_OF_PRIORITIES] = {0}, sequence_period[SOC_SAT_GTF_NUM_OF_PRIORITIES] = {0};
    uint32 pkt_add_crc[SOC_SAT_GTF_NUM_OF_PRIORITIES] = {0}, crc_offset[SOC_SAT_GTF_NUM_OF_PRIORITIES] = {0};
    uint32 pkt_add_tlv = 0;
    uint32 offset = 0;
    int i = 0;
    int priority = 0;
    soc_mem_t sat_tx_evc_prm1_mem;
    soc_mem_t sat_tx_evc_prm2_mem;

    /* 1.1 prepare packet header */
    sal_memset(&pkt_hdr_tbl_info, 0, sizeof(pkt_hdr_tbl_info));
    rv = _soc_sat_gtf_construct_packet_header(unit, &(config->header_info), &pkt_hdr_tbl_info);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in _soc_sat_gtf_construct_packet_header\n"), soc_errmsg(rv)));
        return rv;
    }

    /* 1.2 prepare payload */
    if (config->payload.payload_type == socSatPayloadConstant8Bytes) {
        rv = soc_sat_packet_config_exp_payload_uint8_to_long(config->payload.payload_pattern, SOC_SAT_PAYLOAD_MAX_PATTERN_NOF_U8, 0, payload_rpt_pattern);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in soc_sat_packet_config_exp_payload_uint8_to_long\n"), soc_errmsg(rv)));
            return rv;
        }
        payload_type = 0;
    }
    else if (config->payload.payload_type == socSatPayloadConstant4Bytes) {
        rv = soc_sat_packet_config_exp_payload_uint8_to_long(config->payload.payload_pattern, SOC_SAT_PAYLOAD_MAX_PATTERN_NOF_U8/2, 0, payload_rpt_pattern);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in soc_sat_packet_config_exp_payload_uint8_to_long\n"), soc_errmsg(rv)));
            return rv;
        }
        payload_type = 1;
    }
    else if (config->payload.payload_type == socSatPayloadPRBS) {
        payload_type = 2;
    }
    else  {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Invalid payload type(%d)\n"), config->payload.payload_type));
        return SOC_E_PARAM;
    }
    for (priority = 0; priority < SOC_SAT_GTF_NUM_OF_PRIORITIES; priority++) {
        /* 1.3 prepare stamp */  
        sal_memset(&(stamp_field_info[priority]), 0, sizeof(stamp_field_info[priority]));
        rv = _soc_sat_gtf_construct_stamp_field(unit, &(config->packet_edit[priority]), &(stamp_field_info[priority]));
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in _soc_sat_gtf_construct_stamp_field\n"), soc_errmsg(rv)));
            return rv;
        }

        /* 1.4 prepare sequence */
        if (config->offsets.seq_number_offset != SOC_SAT_GTF_SEQ_NUM_DISABLE) {
            sequence_ena[priority] = 1;
            sequence_offset[priority] = config->offsets.seq_number_offset;
            sequence_period[priority] = config->packet_edit[priority].number_of_ctfs - 1;
        }
        else {
            sequence_ena[priority] = 0;
        }

        /* 1.5 prepare tlv and crc */
        if (config->packet_edit[priority].flags & SOC_SAT_GTF_PACKET_EDIT_ADD_END_TLV) {
            pkt_add_tlv = 1;
        }
        else {
            pkt_add_tlv = 0;
        }

        if (config->packet_edit[priority].flags & SOC_SAT_GTF_PACKET_EDIT_ADD_CRC) {
            pkt_add_crc[priority] = 1;
            crc_offset[priority] = config->offsets.crc_byte_offset;
        }
        else {
            pkt_add_crc[priority] = 0;
        }
    }

    /* 2.1 set header and payload info */

   /* For writing on dynamic tables (in the OAMP), have to enable dynamic memory access */
    rv = soc_sat_dynamic_memory_access_set(unit, 1);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in soc_sat_dynamic_memory_access_set\n"), soc_errmsg(rv)));
        return rv;
    }

    sat_tx_evc_prm1_mem = SOC_IS_QAX(unit) ? OAMP_SAT_TX_EVC_PRM_ENTRY_1m : OAMP_SAT_TX_EVC_PARAMS_ENTRY_1m;
    sat_tx_evc_prm2_mem = SOC_IS_QAX(unit) ? OAMP_SAT_TX_EVC_PRM_ENTRY_2m : OAMP_SAT_TX_EVC_PARAMS_ENTRY_2m;
    /* Per evc parameters, two memory entries are needed: odd entries */
    rv = soc_mem_read(unit, sat_tx_evc_prm1_mem, MEM_BLOCK_ANY, SAT_TX_EVC_PARAMS_ENTRY_1_TBL_INDX(gtf_id), tx_evc_params_entry_1_data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in read sat_tx_evc_prm1_mem\n"), soc_errmsg(rv)));
        goto exit;
    }
    /* PKT_HEADER_LSBf */
    soc_mem_field_set(unit, sat_tx_evc_prm1_mem, tx_evc_params_entry_1_data, PKT_HEADER_LSBf, pkt_hdr_tbl_info.pkt_hdr_lsb);
    rv = soc_mem_write(unit, sat_tx_evc_prm1_mem, MEM_BLOCK_ANY, SAT_TX_EVC_PARAMS_ENTRY_1_TBL_INDX(gtf_id), tx_evc_params_entry_1_data); 
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in write sat_tx_evc_prm1_mem\n"), soc_errmsg(rv)));
        goto exit;
    }


    /* Per evc parameters, two memory entries are needed: odd entries */
    rv = soc_mem_read(unit, sat_tx_evc_prm2_mem, MEM_BLOCK_ANY, SAT_TX_EVC_PARAMS_ENTRY_2_TBL_INDX(gtf_id), tx_evc_params_entry_2_data); 
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in read sat_tx_evc_prm2_mem\n"), soc_errmsg(rv)));
        goto exit;
    }

    soc_mem_field_set(unit, sat_tx_evc_prm2_mem, tx_evc_params_entry_2_data, PKT_HEADER_MSBf, pkt_hdr_tbl_info.pkt_hdr_msb);
    soc_mem_field_set(unit, sat_tx_evc_prm2_mem, tx_evc_params_entry_2_data, PKT_HEADER_LENGTHf, &(pkt_hdr_tbl_info.pkt_hdr_len));
    soc_mem_field_set(unit, sat_tx_evc_prm2_mem, tx_evc_params_entry_2_data, RPT_PATTERNf, payload_rpt_pattern);
    soc_mem_field_set(unit, sat_tx_evc_prm2_mem, tx_evc_params_entry_2_data, PAYLOAD_TYPEf, &payload_type);

    rv = soc_mem_write(unit, sat_tx_evc_prm2_mem, MEM_BLOCK_ANY, SAT_TX_EVC_PARAMS_ENTRY_2_TBL_INDX(gtf_id), tx_evc_params_entry_2_data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in write sat_tx_evc_prm2_mem\n"), soc_errmsg(rv)));
        goto exit;
    }
    
    /* 2.2 set length, pattern, stamp, sequence, tlv and crc */
    if (SOC_IS_QAX(unit)) { 
        soc_reg_above_64_val_t tx_gen_prm_entry1_data;
        soc_reg_above_64_val_t tx_gen_prm_entry2_data;
        for (priority = 0; priority < SOC_SAT_GTF_NUM_OF_PRIORITIES; priority++) {
            rv = READ_OAMP_SAT_TX_GEN_PRM_ENTRY_1m(unit, MEM_BLOCK_ANY, SAT_TX_GEN_PRM_ENTRY_1_TBL_INDX(gtf_id, priority), tx_gen_prm_entry1_data);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SAT,
                          (BSL_META_U(unit,
                                      "Fail(%s) in read OAMP_SAT_TX_GEN_PRM_ENTRY_1m\n"), soc_errmsg(rv)));
                goto exit;
            }

            rv = READ_OAMP_SAT_TX_GEN_PRM_ENTRY_2m(unit, MEM_BLOCK_ANY, SAT_TX_GEN_PRM_ENTRY_2_TBL_INDX(gtf_id, priority), tx_gen_prm_entry2_data);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SAT,
                          (BSL_META_U(unit,
                                      "Fail(%s) in read OAMP_SAT_TX_GEN_PRM_ENTRY_2m\n"), soc_errmsg(rv)));
                goto exit;
            }

            /* PKT_LENGTH_XXXf */
            for (i = 0; i < SOC_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; i++) {
                field_data = config->packet_edit[priority].packet_length[i];
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, pkt_length_fields[i], &field_data);
            }
           soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_2m, tx_gen_prm_entry2_data, PKT_LENGTH_NEXTf, (uint32*)&(config->packet_edit[priority].packet_length[config->packet_edit[priority].packet_length_pattern[0]]));
           field_data = 0;
           soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_2m, tx_gen_prm_entry2_data, PKT_LENGTH_PATTERN_IDXf, &field_data);

            /* LENGTH_PATTERNf */   
            offset = 0;
            for (i = 0; i < config->packet_edit[priority].pattern_length; i++) { 
                SHR_BITCOPY_RANGE(length_pattern, offset, &(config->packet_edit[priority].packet_length_pattern[i]), 0, 3);
                offset += 3;
            }
            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, LENGTH_PATTERNf, length_pattern);
            field_data = config->packet_edit[priority].pattern_length - 1;
            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, LENGTH_PATTERN_SIZEf, &field_data);

            /* STMP_2_BIT_XXXf */
            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, STMP_2_BIT_ENABLEf, &(stamp_field_info[priority].stamp_2_ena));
            if (stamp_field_info[priority].stamp_2_ena != 0) {
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, STMP_2_BIT_OFFSETf, &(stamp_field_info[priority].stamp_2_offset));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, STMP_2_BIT_SHIFTf, &(stamp_field_info[priority].stamp_2_shift));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, STMP_2_BIT_VALUEf, &(stamp_field_info[priority].stamp_2_value));
            }
            /* CNTR_8_BIT_XXXf */
            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, CNTR_8_BIT_ENABLEf, &(stamp_field_info[priority].counter_8_ena));
            if (stamp_field_info[priority].counter_8_ena != 0) {
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, CNTR_8_BIT_OFFSETf, &(stamp_field_info[priority].counter_8_offset));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, CNTR_8_BIT_SHIFTf, &(stamp_field_info[priority].counter_8_shift));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, CNTR_8_BIT_MASKf, &(stamp_field_info[priority].counter_8_mask));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, CNTR_8_BIT_INC_STEPf, &(stamp_field_info[priority].counter_8_inc_step));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, CNTR_8_BIT_PERIODf, &(stamp_field_info[priority].counter_8_period));
                field_data = 0x0;
                soc_mem_field32_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_2m, tx_gen_prm_entry2_data, CNTR_8_BIT_NEXT_VALUEf, field_data); 
            }       
            /* CNTR_16_BIT_XXXf */
            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, CNTR_16_BIT_ENABLEf, &(stamp_field_info[priority].counter_16_ena));
            if (stamp_field_info[priority].counter_16_ena != 0) {
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, CNTR_16_BIT_OFFSETf, &(stamp_field_info[priority].counter_16_offset));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, CNTR_16_BIT_SHIFTf, &(stamp_field_info[priority].counter_16_shift));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, CNTR_16_BIT_MASKf, &(stamp_field_info[priority].counter_16_mask));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, CNTR_16_BIT_INC_STEPf, &(stamp_field_info[priority].counter_16_inc_step));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, CNTR_16_BIT_PERIODf, &(stamp_field_info[priority].counter_16_period));
                field_data = 0x0;
                soc_mem_field32_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_2m, tx_gen_prm_entry2_data, CNTR_16_BIT_NEXT_VALUEf, field_data); 
            }
            /* SEQ_NUM_XXXf */
            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, SEQ_NUM_ENABLEf, &(sequence_ena[priority]));
            if (sequence_ena[priority] == 1) {
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, SEQ_NUM_PERIODf, &(sequence_period[priority]));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, SEQ_NUM_OFFSETf, &(sequence_offset[priority]));
                field_data = 0;
                soc_mem_field32_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_2m, tx_gen_prm_entry2_data, SEQ_NUM_NXT_VALUEf, field_data);
            }

            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, ADD_END_TLVf, &pkt_add_tlv);
            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, CRC_32_ENABLEf, &(pkt_add_crc[priority]));
            if (pkt_add_crc[priority] == 1) {
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_1m, tx_gen_prm_entry1_data, CRC_32_OFFSETf, &(crc_offset[priority]));
            }

            /* set sat packet as non-TDM packet && set src channel to 0 */
            field_data = 0;
            soc_mem_field32_set(unit, OAMP_SAT_TX_GEN_PRM_ENTRY_2m, tx_gen_prm_entry2_data, CHAN_NUMf, field_data);

            rv = WRITE_OAMP_SAT_TX_GEN_PRM_ENTRY_1m(unit, MEM_BLOCK_ANY, SAT_TX_GEN_PRM_ENTRY_1_TBL_INDX(gtf_id, priority), tx_gen_prm_entry1_data);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SAT,
                          (BSL_META_U(unit,
                                      "Fail(%s) in write OAMP_SAT_TX_GEN_PRM_ENTRY_1m\n"), soc_errmsg(rv)));
                goto exit;
            }

            rv = WRITE_OAMP_SAT_TX_GEN_PRM_ENTRY_2m(unit, MEM_BLOCK_ANY, SAT_TX_GEN_PRM_ENTRY_2_TBL_INDX(gtf_id, priority), tx_gen_prm_entry2_data);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SAT,
                          (BSL_META_U(unit,
                                      "Fail(%s) in write OAMP_SAT_TX_GEN_PRM_ENTRY_2m\n"), soc_errmsg(rv)));
                goto exit;
            }
        }
    }else {
        soc_reg_above_64_val_t tx_gen_params_data;
        for (priority = 0; priority < SOC_SAT_GTF_NUM_OF_PRIORITIES; priority++) {
            rv = READ_OAMP_SAT_TX_GEN_PARAMSm(unit, MEM_BLOCK_ANY, SAT_TX_GEN_PARAMS_TBL_INDX(gtf_id, priority), tx_gen_params_data);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SAT,
                          (BSL_META_U(unit,
                                      "Fail(%s) in read OAMP_SAT_TX_GEN_PARAMSm\n"), soc_errmsg(rv)));
                goto exit;
            }

            /* PKT_LENGTH_XXXf */
            for (i = 0; i < SOC_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; i++) {
                field_data = config->packet_edit[priority].packet_length[i];
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, pkt_length_fields[i], &field_data);
            }
            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, PKT_LENGTH_NEXTf, (uint32*)&(config->packet_edit[priority].packet_length[config->packet_edit[priority].packet_length_pattern[0]]));
            field_data = 0;
            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, PKT_LENGTH_PATTERN_IDXf, &field_data);

            /* LENGTH_PATTERNf */   
            offset = 0;
            for (i = 0; i < config->packet_edit[priority].pattern_length; i++) { 
                SHR_BITCOPY_RANGE(length_pattern, offset, &(config->packet_edit[priority].packet_length_pattern[i]), 0, 3);
                offset += 3;
            }
            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, LENGTH_PATTERNf, length_pattern);
            field_data = config->packet_edit[priority].pattern_length - 1;
            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, LENGTH_PATTERN_SIZEf, &field_data);

            /* STMP_2_BIT_XXXf */
            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, STMP_2_BIT_ENABLEf, &(stamp_field_info[priority].stamp_2_ena));
            if (stamp_field_info[priority].stamp_2_ena != 0) {
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, STMP_2_BIT_OFFSETf, &(stamp_field_info[priority].stamp_2_offset));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, STMP_2_BIT_SHIFTf, &(stamp_field_info[priority].stamp_2_shift));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, STMP_2_BIT_VALUEf, &(stamp_field_info[priority].stamp_2_value));
            }
            /* CNTR_8_BIT_XXXf */
            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, CNTR_8_BIT_ENABLEf, &(stamp_field_info[priority].counter_8_ena));
            if (stamp_field_info[priority].counter_8_ena != 0) {
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, CNTR_8_BIT_OFFSETf, &(stamp_field_info[priority].counter_8_offset));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, CNTR_8_BIT_SHIFTf, &(stamp_field_info[priority].counter_8_shift));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, CNTR_8_BIT_MASKf, &(stamp_field_info[priority].counter_8_mask));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, CNTR_8_BIT_INC_STEPf, &(stamp_field_info[priority].counter_8_inc_step));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, CNTR_8_BIT_PERIODf, &(stamp_field_info[priority].counter_8_period));
                field_data = 0x0;
                soc_mem_field32_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, CNTR_8_BIT_NEXT_VALUEf, field_data); 
            }       
            /* CNTR_16_BIT_XXXf */
            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, CNTR_16_BIT_ENABLEf, &(stamp_field_info[priority].counter_16_ena));
            if (stamp_field_info[priority].counter_16_ena != 0) {
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, CNTR_16_BIT_OFFSETf, &(stamp_field_info[priority].counter_16_offset));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, CNTR_16_BIT_SHIFTf, &(stamp_field_info[priority].counter_16_shift));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, CNTR_16_BIT_MASKf, &(stamp_field_info[priority].counter_16_mask));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, CNTR_16_BIT_INC_STEPf, &(stamp_field_info[priority].counter_16_inc_step));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, CNTR_16_BIT_PERIODf, &(stamp_field_info[priority].counter_16_period));
                field_data = 0x0;
                soc_mem_field32_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, CNTR_16_BIT_NEXT_VALUEf, field_data); 
            }
            /* SEQ_NUM_XXXf */
            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, SEQ_NUM_ENABLEf, &(sequence_ena[priority]));
            if (sequence_ena[priority] == 1) {
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, SEQ_NUM_PERIODf, &(sequence_period[priority]));
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, SEQ_NUM_OFFSETf, &(sequence_offset[priority]));
                field_data = 0;
                soc_mem_field32_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, SEQ_NUM_NXT_VALUEf, field_data);
            }

            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, ADD_END_TLVf, &pkt_add_tlv);
            soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, CRC_32_ENABLEf, &(pkt_add_crc[priority]));
            if (pkt_add_crc[priority] == 1) {
                soc_mem_field_set(unit, OAMP_SAT_TX_GEN_PARAMSm, tx_gen_params_data, CRC_32_OFFSETf, &(crc_offset[priority]));
            }

            rv = WRITE_OAMP_SAT_TX_GEN_PARAMSm(unit, MEM_BLOCK_ANY, SAT_TX_GEN_PARAMS_TBL_INDX(gtf_id, priority), tx_gen_params_data);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SAT,
                          (BSL_META_U(unit,
                                      "Fail(%s) in write OAMP_SAT_TX_GEN_PARAMSm\n"), soc_errmsg(rv)));
                goto exit;
            }
        }
    }
    /* 2.3 set packet_context_id */
    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit) ) {   
        rv = soc_reg_above_64_get(unit, OAMP_SAT_EVC_RATE_CONTROLr, REG_PORT_ANY, SAT_TX_PER_EVC_INDX(gtf_id), evc_rate_control_data);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in read OAMP_SAT_EVC_RATE_CONTROLr\n"), soc_errmsg(rv)));
            goto exit;
        }
        soc_reg_above_64_field32_set(unit, OAMP_SAT_EVC_RATE_CONTROLr, evc_rate_control_data, PIPE_ID_EVC__Nf, config->packet_context_id);
        rv = soc_reg_above_64_set(unit, OAMP_SAT_EVC_RATE_CONTROLr, REG_PORT_ANY, SAT_TX_PER_EVC_INDX(gtf_id), evc_rate_control_data);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in write OAMP_SAT_EVC_RATE_CONTROLr\n"), soc_errmsg(rv)));
            goto exit;
        }
    }

exit:
    /* After done, disable dynamic memory access */
    soc_sat_dynamic_memory_access_set(unit, 0);
    return rv;
}

soc_error_t soc_sat_gtf_bandwidth_set (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_sat_gtf_t gtf_id,	 
    SOC_SAND_IN int priority,
    SOC_SAND_IN soc_sat_gtf_bandwidth_t *bw
    )
{
    int rv = SOC_E_NONE;
    soc_reg_above_64_val_t data;
    int index;
    uint32 mnt_nof_bits = 0, exp_nof_bits = 0;
    uint32 rate_mnt_val = 0, rate_exp_val = 0;
    uint32 burst_mnt_val = 0, burst_exp_val = 0;
#ifdef BCM_SAT_GTF_STAT_FIX
    uint32 stat_val;
#endif

    /* 1. caculate bytes per cycle for rate */
    mnt_nof_bits = soc_reg_field_length(unit, OAMP_SAT_GEN_RATE_CONTROLr, BYTES_PER_CYCLE_MNT__Nf);
    exp_nof_bits = soc_reg_field_length(unit, OAMP_SAT_GEN_RATE_CONTROLr, BYTES_PER_CYCLE_EXP__Nf);   
    rv = _soc_sat_gtf_rate_to_internal_rate_convert(unit, 
        bw->rate,
        bw->flags,
        mnt_nof_bits,
        exp_nof_bits,
        bw->granularity,
        &rate_mnt_val,
        &rate_exp_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in _soc_sat_gtf_rate_to_internal_rate_convert\n"), soc_errmsg(rv)));
        return rv;
    }

    /* 2. caculate max credits for max_burst */
    mnt_nof_bits = soc_reg_field_length(unit, OAMP_SAT_GEN_RATE_CONTROLr, MAX_CREDITS_MNT__Nf);
    exp_nof_bits = soc_reg_field_length(unit, OAMP_SAT_GEN_RATE_CONTROLr, MAX_CREDITS_EXP__Nf);

    if (SOC_IS_QAX(unit) && (bw->flags & SOC_SAT_GTF_RATE_IN_PACKETS)) {
        rv = soc_break_to_mnt_exp_round_up(
              bw->max_burst,
              mnt_nof_bits,
              exp_nof_bits,
              0,
              &burst_mnt_val,
              &burst_exp_val
            );
    }else {
        rv = soc_break_to_mnt_exp_round_up(
              bw->max_burst* 125,
              mnt_nof_bits,
              exp_nof_bits,
              0,
              &burst_mnt_val,
              &burst_exp_val
            );
    }
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in soc_break_to_mnt_exp_round_up\n"), soc_errmsg(rv)));
        return rv;
    }

    if ((priority == SOC_SAT_GTF_OBJ_COMMON)) {
        SOC_REG_ABOVE_64_CLEAR(data);

        index = SAT_TX_PER_EVC_INDX(gtf_id);

        rv = READ_OAMP_SAT_EVC_RATE_CONTROLr(unit, index, data);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in read OAMP_SAT_EVC_RATE_CONTROLr\n"), soc_errmsg(rv)));
            return rv;
        }

        soc_reg_above_64_field32_set(unit, OAMP_SAT_EVC_RATE_CONTROLr, data, BYTES_PER_CYCLE_EVC_MNT__Nf, rate_mnt_val);
        soc_reg_above_64_field32_set(unit, OAMP_SAT_EVC_RATE_CONTROLr, data, BYTES_PER_CYCLE_EVC_EXP__Nf, rate_exp_val);
        soc_reg_above_64_field32_set(unit, OAMP_SAT_EVC_RATE_CONTROLr, data, MAX_CREDITS_EVC_MNT__Nf, burst_mnt_val);
        soc_reg_above_64_field32_set(unit, OAMP_SAT_EVC_RATE_CONTROLr, data, MAX_CREDITS_EVC_EXP__Nf, burst_exp_val);
        if (SOC_IS_QAX(unit)) {
            soc_reg_above_64_field32_set(unit, OAMP_SAT_EVC_RATE_CONTROLr, data, CRDT_PER_PKT_EVC__Nf, (bw->flags & SOC_SAT_GTF_RATE_IN_PACKETS) ? 1 : 0);
        }

        rv = WRITE_OAMP_SAT_EVC_RATE_CONTROLr(unit, index, data);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in write OAMP_SAT_EVC_RATE_CONTROLr\n"), soc_errmsg(rv)));
            return rv;
        }
    }
    else {
        SOC_REG_ABOVE_64_CLEAR(data);

        index = SAT_TX_PER_GENERATOR_INDX(gtf_id, priority);

        rv = READ_OAMP_SAT_GEN_RATE_CONTROLr(unit, index, data);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in read OAMP_SAT_GEN_RATE_CONTROLr\n"), soc_errmsg(rv)));
            return rv;
        }
#ifdef BCM_SAT_GTF_STAT_FIX
        if (!SOC_IS_QAX(unit)) {
            stat_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, PACKET_COUNT__Nf);
            COMPILER_64_ADD_32(soc_sat_data[unit].gtf_stat_value[index], stat_val);
        }
#endif

        soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, BYTES_PER_CYCLE_MNT__Nf, rate_mnt_val);
        soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, BYTES_PER_CYCLE_EXP__Nf, rate_exp_val);
        soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, MAX_CREDITS_MNT__Nf, burst_mnt_val);
        soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, MAX_CREDITS_EXP__Nf, burst_exp_val);
        if (SOC_IS_QAX(unit)) {
            soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, CRDT_PER_PKT__Nf, (bw->flags & SOC_SAT_GTF_RATE_IN_PACKETS) ? 1 : 0);
        }

        rv = WRITE_OAMP_SAT_GEN_RATE_CONTROLr(unit, index, data);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in write OAMP_SAT_GEN_RATE_CONTROLr\n"), soc_errmsg(rv)));
            return rv;
        }
    }

    return rv;
}

soc_error_t soc_sat_gtf_bandwidth_get (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_sat_gtf_t gtf_id,	 
    SOC_SAND_IN int priority,
    SOC_SAND_OUT soc_sat_gtf_bandwidth_t *bw
    )
{
    int rv = SOC_E_NONE;
    soc_reg_above_64_val_t data;
    int index;
    uint32 rate_mnt_val = 0, rate_exp_val = 0;
    uint32 burst_mnt_val = 0, burst_exp_val = 0;
    uint32 crdt_per_pkt = 0;
#ifdef BCM_SAT_GTF_STAT_FIX
    uint32 stat_val;
#endif

    if ((priority == SOC_SAT_GTF_OBJ_COMMON)) {
        SOC_REG_ABOVE_64_CLEAR(data);

        index = SAT_TX_PER_EVC_INDX(gtf_id);

        rv = READ_OAMP_SAT_EVC_RATE_CONTROLr(unit, index, data);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in read OAMP_SAT_EVC_RATE_CONTROLr\n"), soc_errmsg(rv)));
            return rv;
        }

        rate_mnt_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_EVC_RATE_CONTROLr, data, BYTES_PER_CYCLE_EVC_MNT__Nf);
        rate_exp_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_EVC_RATE_CONTROLr, data, BYTES_PER_CYCLE_EVC_EXP__Nf);
        burst_mnt_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_EVC_RATE_CONTROLr, data, MAX_CREDITS_EVC_MNT__Nf);
        burst_exp_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_EVC_RATE_CONTROLr, data, MAX_CREDITS_EVC_EXP__Nf);

        if (SOC_IS_QAX(unit)) {
            crdt_per_pkt = soc_reg_above_64_field32_get(unit, OAMP_SAT_EVC_RATE_CONTROLr, data, CRDT_PER_PKT_EVC__Nf);
            bw->flags = crdt_per_pkt;
        }
    }
    else {
        SOC_REG_ABOVE_64_CLEAR(data);

        index = SAT_TX_PER_GENERATOR_INDX(gtf_id, priority);

        rv = READ_OAMP_SAT_GEN_RATE_CONTROLr(unit, index, data);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in read OAMP_SAT_GEN_RATE_CONTROLr\n"), soc_errmsg(rv)));
            return rv;
        }
#ifdef BCM_SAT_GTF_STAT_FIX
        if (!SOC_IS_QAX(unit)) {
            stat_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, PACKET_COUNT__Nf);
            COMPILER_64_ADD_32(soc_sat_data[unit].gtf_stat_value[index], stat_val);
        }
#endif
        rate_mnt_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, BYTES_PER_CYCLE_MNT__Nf);
        rate_exp_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, BYTES_PER_CYCLE_EXP__Nf);
        burst_mnt_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, MAX_CREDITS_MNT__Nf);
        burst_exp_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, MAX_CREDITS_EXP__Nf);

        if (SOC_IS_QAX(unit)) {
            crdt_per_pkt = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, CRDT_PER_PKT__Nf);
            bw->flags = crdt_per_pkt;
        }
    }

    rv = _soc_sat_gtf_rate_from_internal_rate_convert(unit, rate_mnt_val, rate_exp_val, crdt_per_pkt, &(bw->rate));
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in _soc_sat_gtf_rate_from_internal_rate_convert\n"), soc_errmsg(rv)));
        return rv;
    }

    if ((SOC_IS_QAX(unit)) && crdt_per_pkt) {
        bw->max_burst = (burst_mnt_val) * (1<<(burst_exp_val));
    }else {
        bw->max_burst = (burst_mnt_val) * (1<<(burst_exp_val)) / 125;
    }

    return rv;
}

soc_error_t soc_sat_gtf_rate_pattern_set (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_gtf_t gtf_id,
	SOC_SAND_IN int priority,
	SOC_SAND_IN soc_sat_gtf_rate_pattern_t *config
    )
{
    int rv = SOC_E_NONE;
    soc_reg_above_64_val_t data;
    int index;
    uint32 mnt_nof_bits = 0, exp_nof_bits = 0;
    uint32 high_mnt_val = 0, high_exp_val = 0, low_mnt_val = 0, low_exp_val = 0;
#ifdef BCM_SAT_GTF_STAT_FIX
    uint32 stat_val;
#endif
    uint32 field_data = 0;
    uint32 pkt_mode = 0;

    SOC_REG_ABOVE_64_CLEAR(data);

    index = SAT_TX_PER_GENERATOR_INDX(gtf_id, priority);

    if (SOC_IS_QAX(unit)) {
        rv = READ_OAMP_SAT_GEN_RATE_CONTROLr(unit, index, data);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                            (BSL_META_U(unit,
                            "Fail(%s) in read OAMP_SAT_GEN_RATE_CONTROLr\n"), soc_errmsg(rv)));
            return rv;
        }
        pkt_mode = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, CRDT_PER_PKT__Nf);
        SOC_REG_ABOVE_64_CLEAR(data);
    }

    rv = READ_OAMP_SAT_GEN_RATE_CONTROLr(unit, index, data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in read OAMP_SAT_GEN_RATE_CONTROLr\n"), soc_errmsg(rv)));
        return rv;
    }
#ifdef BCM_SAT_GTF_STAT_FIX
    if (!SOC_IS_QAX(unit)) {
        stat_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, PACKET_COUNT__Nf);
        COMPILER_64_ADD_32(soc_sat_data[unit].gtf_stat_value[index], stat_val);
    }
#endif


    /* 1. calculate hardware values for high threshold */
    mnt_nof_bits = soc_reg_field_length(unit, OAMP_SAT_GEN_RATE_CONTROLr, THRESHOLD_UPPER_MNT__Nf);
    exp_nof_bits = soc_reg_field_length(unit, OAMP_SAT_GEN_RATE_CONTROLr, THRESHOLD_UPPER_EXP__Nf);
    rv = soc_break_to_mnt_exp_round_up(
             pkt_mode ?   config->high_threshold : (config->high_threshold* 125),
             mnt_nof_bits,
             exp_nof_bits,
             0,
             &high_mnt_val,
             &high_exp_val
            );
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in soc_break_to_mnt_exp_round_up\n"), soc_errmsg(rv)));
        return rv;
    }
    rv = soc_break_to_mnt_exp_round_up(
             pkt_mode ? config->low_threshold : (config->low_threshold* 125),
             mnt_nof_bits,
             exp_nof_bits,
             0,
             &low_mnt_val,
             &low_exp_val
            );
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in soc_break_to_mnt_exp_round_up\n"), soc_errmsg(rv)));
        return rv;
    }

    if ((config->rate_pattern_mode == socSatGtfRatePatternContinues) || (config->rate_pattern_mode == socSatGtfRatePatternSimpleBurst)) {
        field_data = 0;
    }
    else if (config->rate_pattern_mode == socSatGtfRatePatternCombined) {
        field_data = 1;
    }
    else if (config->rate_pattern_mode == socSatGtfRatePatternInterval) {
        field_data = 3;
    }
    else {
        rv = SOC_E_PARAM;
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) rate_pattern_mode(%d) is not supported\n"), 
                              soc_errmsg(rv), config->rate_pattern_mode));

        return rv;
    }

    soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, BURST_MODE__Nf, field_data);
    soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, THRESHOLD_UPPER_MNT__Nf, high_mnt_val);
    soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, THRESHOLD_UPPER_EXP__Nf, high_exp_val);
    soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, THRESHOLD_LOWER_MNT__Nf, low_mnt_val);
    soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, THRESHOLD_LOWER_EXP__Nf, low_exp_val);

    soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, STOP_ITER_CNT__Nf, config->stop_iter_count);
    soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, STOP_BURST_CNT__Nf, config->stop_burst_count);
    soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, STOP_INTERVAL_CNT_CFG__Nf, config->stop_interval_count);
    soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, BURST_PKT_WEIGHT__Nf, config->burst_packet_weight);
    soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, INTERVAL_PKT_WEIGHT__Nf, config->interval_packet_weight);

    if (config->flags & SOC_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST) {
        soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, SET_INTRVAL_CNT_TO_CFG__Nf, 0x1);
    }
    else {
        soc_reg_above_64_field32_set(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, SET_INTRVAL_CNT_TO_CFG__Nf, 0);
    }

    rv = WRITE_OAMP_SAT_GEN_RATE_CONTROLr(unit, index, data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in write OAMP_SAT_GEN_RATE_CONTROLr\n"), soc_errmsg(rv)));
    }

    return rv;
}

soc_error_t soc_sat_gtf_rate_pattern_get (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_gtf_t gtf_id,
	SOC_SAND_IN int priority,
	SOC_SAND_OUT soc_sat_gtf_rate_pattern_t *config
    )
{
    int rv = SOC_E_NONE;  
    soc_reg_above_64_val_t data;
    int index;
    uint32 high_mnt_val = 0, high_exp_val = 0, low_mnt_val = 0, low_exp_val = 0;
    uint32 interval_cnt_to_cfg;
    uint32 rate_pattern_mode = 0;
#ifdef BCM_SAT_GTF_STAT_FIX
    uint32 stat_val;
#endif

    SOC_REG_ABOVE_64_CLEAR(data);

    index = SAT_TX_PER_GENERATOR_INDX(gtf_id, priority);

    rv = READ_OAMP_SAT_GEN_RATE_CONTROLr(unit, index, data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in read OAMP_SAT_GEN_RATE_CONTROLr\n"), soc_errmsg(rv)));
        return rv;
    }
#ifdef BCM_SAT_GTF_STAT_FIX
    if (!SOC_IS_QAX(unit)) {
        stat_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, PACKET_COUNT__Nf);
        COMPILER_64_ADD_32(soc_sat_data[unit].gtf_stat_value[index], stat_val);
    }
#endif

    rate_pattern_mode = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, BURST_MODE__Nf);

    high_mnt_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, THRESHOLD_UPPER_MNT__Nf); 
    high_exp_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, THRESHOLD_UPPER_EXP__Nf); 
    low_mnt_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, THRESHOLD_LOWER_MNT__Nf); 
    low_exp_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, THRESHOLD_LOWER_EXP__Nf);

    config->stop_iter_count = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, STOP_ITER_CNT__Nf); 
    config->stop_burst_count = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, STOP_BURST_CNT__Nf);
    config->stop_interval_count = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, STOP_INTERVAL_CNT_CFG__Nf); 
    config->burst_packet_weight = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, BURST_PKT_WEIGHT__Nf);
    config->interval_packet_weight = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, INTERVAL_PKT_WEIGHT__Nf); 

    interval_cnt_to_cfg = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, SET_INTRVAL_CNT_TO_CFG__Nf);   
    if (interval_cnt_to_cfg == 0x1) {
        config->flags |= SOC_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST;
    }

    config->high_threshold = (high_mnt_val) * (1<<(high_exp_val)) / 125;
    config->low_threshold = (low_mnt_val) * (1<<(low_exp_val)) / 125;

    if (rate_pattern_mode == 0) {
        config->rate_pattern_mode = (config->high_threshold == config->low_threshold) ? 
            socSatGtfRatePatternContinues : socSatGtfRatePatternSimpleBurst;
    }
    else if (rate_pattern_mode == 1) {
        config->rate_pattern_mode = socSatGtfRatePatternCombined;
    }
    else if (rate_pattern_mode == 3) {
        config->rate_pattern_mode = socSatGtfRatePatternInterval;
    }
    else {
        rv = SOC_E_PARAM;
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in parsing rate_pattern_mode(%d)\n"), soc_errmsg(rv), rate_pattern_mode));        
    }

    return rv;
}

soc_error_t soc_sat_gtf_stat_get (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_gtf_t gtf_id,
	SOC_SAND_IN int priority,
	SOC_SAND_IN uint32 flags,
	SOC_SAND_IN soc_sat_gtf_stat_counter_t type,
	SOC_SAND_OUT uint64* value
    )
{
    int rv = SOC_E_NONE;
    soc_reg_above_64_val_t data;
    int index;
    uint32 stat_val = 0;

    if (type == socSatGtfStatPacketCount) {
        SOC_REG_ABOVE_64_CLEAR(data);
        index = SAT_TX_PER_GENERATOR_INDX(gtf_id, priority);

        if (!SOC_IS_QAX(unit)) {
            rv = soc_reg_above_64_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, REG_PORT_ANY, index, data);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SAT,
                          (BSL_META_U(unit,
                                      "Fail(%s) in read OAMP_SAT_GEN_RATE_CONTROLr\n"), soc_errmsg(rv)));
                return rv;
            }
 
            stat_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_GEN_RATE_CONTROLr, data, PACKET_COUNT__Nf);
        }else {
            rv = soc_reg_above_64_get(unit, OAMP_SAT_TX_GEN_CNTRr, REG_PORT_ANY, index, data);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SAT,
                          (BSL_META_U(unit,
                                      "Fail(%s) in read OAMP_SAT_TX_GEN_CNTRr\n"), soc_errmsg(rv)));
                return rv;
            }
       
            stat_val = soc_reg_above_64_field32_get(unit, OAMP_SAT_TX_GEN_CNTRr, data, SAT_TX_GEN_PKT_CNTR__Nf);
        }

        COMPILER_64_ZERO(*value);
        COMPILER_64_ADD_32(*value, stat_val);
#ifdef BCM_SAT_GTF_STAT_FIX
        if (!SOC_IS_QAX(unit)) {
            COMPILER_64_ADD_64(*value, soc_sat_data[unit].gtf_stat_value[index]);

            COMPILER_64_ZERO(soc_sat_data[unit].gtf_stat_value[index]);
        }
#endif
    }

    return rv;
}

/* CTF functions */
soc_error_t soc_sat_ctf_packet_config (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_t ctf_id,
	SOC_SAND_IN soc_sat_ctf_packet_info_t *packet_info
    )
{
    int rv = SOC_E_NONE;
    uint32 offset = 0, field = 0;
    uint32 exp_rpt_pattern[SOC_SAT_PAYLOAD_MAX_PATTERN_NOF_U32] = {0};
    soc_reg_above_64_val_t
        reg_above64_val;

    /* For writing on dynamic tables (in the OAMP), have to enable dynamic memory access */
    rv = soc_sat_dynamic_memory_access_set(unit, 1);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    rv = READ_OAMP_SAT_RX_FLOW_PARAMSm(unit, MEM_BLOCK_ANY, ctf_id, reg_above64_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        goto exit;
    }
    
    /* PAYLOAD_OFFSETf */
    offset = 0;
    field = packet_info->offsets.payload_offset;
    SHR_BITCOPY_RANGE(reg_above64_val, offset, &field, 0, 16);
    /* CRC_OFFSETf */
    offset = SOC_SAT_CRC_OFFSET;
    field = (packet_info->flags & SOC_SAT_CTF_PACKET_INFO_ADD_CRC) ? packet_info->offsets.crc_byte_offset : 0;
    SHR_BITCOPY_RANGE(reg_above64_val, offset, &field, 0, 16);    
    /* SEQ_NUM_OFFSETf */
    field = packet_info->offsets.seq_number_offset;
    offset = SOC_SAT_SEQ_NUM_OFFSET;
    SHR_BITCOPY_RANGE(reg_above64_val, offset, &field, 0, 16);
    /* EXP_RPT_PATTERNf */
    if (packet_info->payload.payload_type == socSatPayloadConstant4Bytes) { 
        rv = soc_sat_packet_config_exp_payload_uint8_to_long(packet_info->payload.payload_pattern, SOC_SAT_PAYLOAD_MAX_PATTERN_NOF_U8/2, 1, exp_rpt_pattern);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in soc_sat_packet_config_exp_payload_uint8_to_long\n"), soc_errmsg(rv)));
            goto exit;
        }
    }
    else if (packet_info->payload.payload_type == socSatPayloadConstant8Bytes) {
        rv = soc_sat_packet_config_exp_payload_uint8_to_long(packet_info->payload.payload_pattern, SOC_SAT_PAYLOAD_MAX_PATTERN_NOF_U8, 1, exp_rpt_pattern);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) in soc_sat_packet_config_exp_payload_uint8_to_long\n"), soc_errmsg(rv)));
            goto exit;
        }
    }
    offset = SOC_SAT_EXP_RPT_PATTERN_OFFSET;
    SHR_BITCOPY_RANGE(reg_above64_val, offset, exp_rpt_pattern, 0, 64);

    /* ADD_END_TLVf */
    field = (packet_info->flags & SOC_SAT_CTF_PACKET_INFO_ADD_END_TLV) ? 1:0;
    offset = SOC_SAT_ADD_END_TLV_OFFSET;
    SHR_BITCOPY_RANGE(reg_above64_val, offset, &field, 0, 1);

    /* ADD_CRCf */
    field = (packet_info->flags & SOC_SAT_CTF_PACKET_INFO_ADD_CRC) ? 1:0;
    offset  = SOC_SAT_ADD_CRC_OFFSET;
    SHR_BITCOPY_RANGE(reg_above64_val, offset, &field, 0, 1);

    /* TIME_STAMP_OFFSETf */
    offset = SOC_SAT_TIME_STAMP_OFFSET;
    field = packet_info->offsets.timestamp_offset;
    SHR_BITCOPY_RANGE(reg_above64_val, offset, &field, 0, 16);

    if (packet_info->payload.payload_type == socSatPayloadConstant4Bytes) {
        /* USE_RPT_PTRN_PAYLOADf */
        offset = SOC_SAT_USE_RPT_PTRN_PAYLOAD_OFFSET;
        field = 1;
        SHR_BITCOPY_RANGE(reg_above64_val, offset, &field, 0, 1);
        /* USE_4_BYTE_PATTERNf */
        offset = SOC_SAT_USE_4_BYTE_PATTERN_OFFSET;
        field = 1;
        SHR_BITCOPY_RANGE(reg_above64_val, offset, &field, 0, 1);
        /* USE_PRBS_PAYLOADf */
        offset = SOC_SAT_USE_PRBS_PAYLOAD_OFFSET;
        field = 0;
        SHR_BITCOPY_RANGE(reg_above64_val, offset, &field, 0, 1);
    }
    else if (packet_info->payload.payload_type == socSatPayloadPRBS) {
        /* USE_RPT_PTRN_PAYLOADf */
        offset = SOC_SAT_USE_RPT_PTRN_PAYLOAD_OFFSET;
        field = 0;
        SHR_BITCOPY_RANGE(reg_above64_val, offset, &field, 0, 1);
        /* USE_4_BYTE_PATTERNf */
        offset = SOC_SAT_USE_4_BYTE_PATTERN_OFFSET;
        field = 0;
        SHR_BITCOPY_RANGE(reg_above64_val, offset, &field, 0, 1);
        /* USE_PRBS_PAYLOADf */
        offset = SOC_SAT_USE_PRBS_PAYLOAD_OFFSET;
        field = 1;
        SHR_BITCOPY_RANGE(reg_above64_val, offset, &field, 0, 1);
    }
    else if (packet_info->payload.payload_type == socSatPayloadConstant8Bytes) {
        /* USE_RPT_PTRN_PAYLOADf */
        offset = SOC_SAT_USE_RPT_PTRN_PAYLOAD_OFFSET;
        field = 1;
        SHR_BITCOPY_RANGE(reg_above64_val, offset, &field, 0, 1);
        /* USE_4_BYTE_PATTERNf */
        offset = SOC_SAT_USE_4_BYTE_PATTERN_OFFSET;
        field = 0;
        SHR_BITCOPY_RANGE(reg_above64_val, offset, &field, 0, 1);
        /* USE_PRBS_PAYLOADf */
        offset = SOC_SAT_USE_PRBS_PAYLOAD_OFFSET;
        field = 0;
        SHR_BITCOPY_RANGE(reg_above64_val, offset, &field, 0, 1);
    }
    /* Reset ExpSeqNumber as 0 */
    offset = 224;
    field = 0;
    SHR_BITCOPY_RANGE(reg_above64_val, offset, &field, 0, 32);
       
    rv = WRITE_OAMP_SAT_RX_FLOW_PARAMSm(unit, MEM_BLOCK_ALL, ctf_id, reg_above64_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        goto exit;
    }

exit:
    /* After done, disable dynamic memory access */
    soc_sat_dynamic_memory_access_set(unit, 0);
    return rv;
}

soc_error_t soc_sat_ctf_identifier_map (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_identifier_t *identifier,
	SOC_SAND_IN soc_sat_ctf_t ctf_id
    )
{
    int rv = SOC_E_NONE;
    int index = 0;
    uint32 data = 0;
    uint8 identifier_valid = 0;
    uint32 valid_fld = 1;

    index = SAT_CTF_IDENTIFIER_TO_INDEX(identifier->session_id, 
                                        identifier->trap_id,
                                        identifier->color,
                                        identifier->tc);

    rv = READ_OAMP_SAT_RX_FLOW_IDm(unit, MEM_BLOCK_ANY, index, &data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }
    identifier_valid = soc_mem_field32_get(unit, OAMP_SAT_RX_FLOW_IDm, &data, FLOW_VALIDf);
    if (identifier_valid) {
        return SOC_E_EXISTS;
    }
    soc_mem_field32_set(unit, OAMP_SAT_RX_FLOW_IDm, &data, FLOW_IDf, ctf_id);
    soc_mem_field32_set(unit, OAMP_SAT_RX_FLOW_IDm, &data, FLOW_VALIDf, valid_fld);

    rv = WRITE_OAMP_SAT_RX_FLOW_IDm(unit, MEM_BLOCK_ALL, index, &data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
    }

    return rv;
}

soc_error_t soc_sat_ctf_identifier_unmap (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_identifier_t *identifier
    )
{
    int rv = SOC_E_NONE;
    int index = 0;
    uint32 data = 0;
    uint32 valid_fld = 0;
    uint32 flow_id = 0;

    index = SAT_CTF_IDENTIFIER_TO_INDEX(identifier->session_id, 
                                        identifier->trap_id,
                                        identifier->color,
                                        identifier->tc);

    rv = READ_OAMP_SAT_RX_FLOW_IDm(unit, MEM_BLOCK_ANY, index, &data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }

    soc_mem_field32_set(unit, OAMP_SAT_RX_FLOW_IDm, &data, FLOW_IDf, flow_id);
    soc_mem_field32_set(unit, OAMP_SAT_RX_FLOW_IDm, &data, FLOW_VALIDf, valid_fld);

    rv = WRITE_OAMP_SAT_RX_FLOW_IDm(unit, MEM_BLOCK_ALL, index, &data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
    }

    return rv;
}

soc_error_t soc_sat_ctf_trap_set (
	SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 trap_id_cnt,
	SOC_SAND_IN uint32* trap_id
)
{
    int rv = SOC_E_NONE;
    soc_reg_above_64_val_t
        reg_above64_val, fld_above64_val;

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    SOC_REG_ABOVE_64_CLEAR(fld_above64_val);
    if (trap_id_cnt != SOC_SAT_CTF_TRAP_ID_MAX_NUM) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return SOC_E_PARAM;
    }

    rv = READ_OAMP_SAT_RX_CONFIGr(unit, reg_above64_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }
    fld_above64_val[0] = trap_id[0];
    soc_reg_above_64_field_set(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_TRAP_CODE_0f, fld_above64_val);
    fld_above64_val[0] = trap_id[1];
    soc_reg_above_64_field_set(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_TRAP_CODE_1f, fld_above64_val);
    fld_above64_val[0] = trap_id[2];
    soc_reg_above_64_field_set(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_TRAP_CODE_2f, fld_above64_val);
    rv = WRITE_OAMP_SAT_RX_CONFIGr(unit, reg_above64_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
    }

    return rv;
}

soc_error_t soc_sat_ctf_bin_limit_set (
	SOC_SAND_IN int unit, 
	SOC_SAND_IN int bins_count, 
 	SOC_SAND_IN soc_sat_ctf_bin_limit_t* bins
    )
{
    int rv = SOC_E_NONE;
    soc_reg_above_64_val_t
        reg_above64_val, fld_above64_val;
    int idx = 0;

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    SOC_REG_ABOVE_64_CLEAR(fld_above64_val);

    rv = READ_OAMP_SAT_RX_CONFIGr(unit, reg_above64_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }
    for (idx = 0; idx < bins_count; idx++) {
        fld_above64_val[0] = bins[idx].bin_limit;
        switch(bins[idx].bin_select){
        case socSATRxFLVBinLimit0: 
            soc_reg_above_64_field_set(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_0f, fld_above64_val);
            break;
        case socSATRxFLVBinLimit1:
            soc_reg_above_64_field_set(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_1f, fld_above64_val);
            break;
        case socSATRxFLVBinLimit2:
            soc_reg_above_64_field_set(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_2f, fld_above64_val);
            break;
        case socSATRxFLVBinLimit3:
            soc_reg_above_64_field_set(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_3f, fld_above64_val);
            break;
        case socSATRxFLVBinLimit4:
            soc_reg_above_64_field_set(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_4f, fld_above64_val);
            break;
        case socSATRxFLVBinLimit5:
            soc_reg_above_64_field_set(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_5f, fld_above64_val);
            break;
        case socSATRxFLVBinLimit6:
            soc_reg_above_64_field_set(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_6f, fld_above64_val);
            break;
        case socSATRxFLVBinLimit7:
            soc_reg_above_64_field_set(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_7f, fld_above64_val);
            break;
        case socSATRxFLVBinLimit8:
            soc_reg_above_64_field_set(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_8f, fld_above64_val);
            break;    
        default:
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return SOC_E_PARAM;
        }
    }
    rv = WRITE_OAMP_SAT_RX_CONFIGr(unit, reg_above64_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
    }

    return rv;
}

soc_error_t soc_sat_ctf_bin_limit_get (
	SOC_SAND_IN int unit, 
	SOC_SAND_IN int max_bins_count,
	SOC_SAND_OUT int * bins_count, 
 	SOC_SAND_OUT soc_sat_ctf_bin_limit_t* bins
    )
{
    int rv = SOC_E_NONE;
    soc_reg_above_64_val_t
        reg_above64_val, fld_above64_val;
    int idx = 0;

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    SOC_REG_ABOVE_64_CLEAR(fld_above64_val);
    *bins_count = 0;

    rv = READ_OAMP_SAT_RX_CONFIGr(unit, reg_above64_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }
    for (idx = 0; idx < max_bins_count; idx++) {
        switch(idx){
        case socSATRxFLVBinLimit0: 
            soc_reg_above_64_field_get(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_0f, fld_above64_val);
            break;
        case socSATRxFLVBinLimit1:
            soc_reg_above_64_field_get(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_1f, fld_above64_val);
            break;
        case socSATRxFLVBinLimit2:
            soc_reg_above_64_field_get(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_2f, fld_above64_val);
            break;
        case socSATRxFLVBinLimit3:
            soc_reg_above_64_field_get(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_3f, fld_above64_val);
            break;
        case socSATRxFLVBinLimit4:
            soc_reg_above_64_field_get(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_4f, fld_above64_val);
            break;
        case socSATRxFLVBinLimit5:
            soc_reg_above_64_field_get(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_5f, fld_above64_val);
            break;
        case socSATRxFLVBinLimit6:
            soc_reg_above_64_field_get(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_6f, fld_above64_val);
            break;
        case socSATRxFLVBinLimit7:
            soc_reg_above_64_field_get(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_7f, fld_above64_val);
            break;
        case socSATRxFLVBinLimit8:
            soc_reg_above_64_field_get(unit, OAMP_SAT_RX_CONFIGr, reg_above64_val, RX_FLV_BIN_LIMIT_8f, fld_above64_val);
            break;    
        default:
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return SOC_E_PARAM;
        }
        if (fld_above64_val[0] == 0){
            break;
        }
        bins[idx].bin_select = idx;
        bins[idx].bin_limit = fld_above64_val[0];
        (*bins_count)++;
    }

    return rv;
}

soc_error_t soc_sat_ctf_stat_config_set (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_t ctf_id,
	SOC_SAND_IN soc_sat_ctf_stat_config_t *stat
    )
{
    int rv = SOC_E_NONE;
    uint32
      data[SOC_SAT_CTF_RX_FLOW_PARAMS_ENTRY_SIZE];
    uint64 reg_val;

    COMPILER_64_ZERO(reg_val);
    /* For writing on dynamic tables (in the OAMP), have to enable dynamic memory access */
    rv = soc_sat_dynamic_memory_access_set(unit, 1);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }

    sal_memset(data, 0, sizeof(data));
    rv = READ_OAMP_SAT_RX_FLOW_PARAMSm(unit, MEM_BLOCK_ANY, ctf_id, data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        goto exit;
    }
    soc_mem_field32_set(unit, OAMP_SAT_RX_FLOW_PARAMSm, &data, CALC_EQUAL_FDV_BINSf, (stat->use_global_bin_config ? 0 : 1));
    soc_mem_field32_set(unit, OAMP_SAT_RX_FLOW_PARAMSm, &data, FDV_BIN_MIN_DELAYf, stat->bin_min_delay);
    soc_mem_field32_set(unit, OAMP_SAT_RX_FLOW_PARAMSm, &data, FDV_BIN_STEPf, stat->bin_step);
    rv = WRITE_OAMP_SAT_RX_FLOW_PARAMSm(unit, MEM_BLOCK_ALL, ctf_id, data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        goto exit;
    }

    rv = READ_OAMP_SAT_RX_STATr(unit, &reg_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        goto  exit;
    }
    soc_reg64_field32_set(unit, OAMP_SAT_RX_STATr, &reg_val, RX_STATS_FLOW_IDf, ctf_id);
    soc_reg64_field32_set(unit, OAMP_SAT_RX_STATr, &reg_val, RX_STATS_UPDT_CNTR_ALWSf, stat->update_counters_in_unvavail_state);
    rv = WRITE_OAMP_SAT_RX_STATr(unit, reg_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        goto exit;
    }

    soc_reg64_field32_set(unit, OAMP_SAT_RX_STATr, &reg_val, RX_STATS_RST_STATE_MCHNf, 1);
    rv = WRITE_OAMP_SAT_RX_STATr(unit, reg_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                (BSL_META_U(unit,
                            "%s\n"), soc_errmsg(rv)));
        goto exit;
    }
    sal_udelay(1000);
    soc_reg64_field32_set(unit, OAMP_SAT_RX_STATr, &reg_val, RX_STATS_RST_STATE_MCHNf, 0);
    rv = WRITE_OAMP_SAT_RX_STATr(unit, reg_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                (BSL_META_U(unit,
                            "%s\n"), soc_errmsg(rv)));
        goto exit;
    }

exit:
    /* After done, disable dynamic memory access */
    soc_sat_dynamic_memory_access_set(unit, 0);
    return rv;
}

soc_error_t soc_sat_ctf_stat_get (
	SOC_SAND_IN int unit, 
	SOC_SAND_IN soc_sat_ctf_t ctf_id,
	SOC_SAND_IN uint32 flags,
	SOC_SAND_OUT soc_sat_ctf_stat_t* stat
    )
{
    int rv = SOC_E_NONE;
    uint64 reg_val;
    uint32 field = 0;
    soc_reg_above_64_val_t
        reg_above64_val_3, reg_above64_val_4;
    int offset_base = 0;
    int len = 0;
    uint32 fld_64[2];
    uint32 bin_delay_counters_5_3lsbs = 0;
    uint32 bin_delay_counters_5_low   = 0;
    uint32 bin_delay_counters_5_high  = 0;
    uint32
      data[SOC_SAT_CTF_RX_FLOW_PARAMS_ENTRY_SIZE];

    sal_memset(data, 0, sizeof(data));

    COMPILER_64_ZERO(reg_val);
    SOC_REG_ABOVE_64_CLEAR(reg_above64_val_3);
    SOC_REG_ABOVE_64_CLEAR(reg_above64_val_4);
    rv = READ_OAMP_SAT_RX_STATr(unit, &reg_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }
    field = (flags & SOC_SAT_CTF_STAT_DO_NOT_CLR_ON_READ ? 0 : 1);
    soc_reg64_field32_set(unit, OAMP_SAT_RX_STATr, &reg_val, RX_STATS_CLR_ON_RDf, field);
    soc_reg64_field32_set(unit, OAMP_SAT_RX_STATr, &reg_val, RX_STATS_FLOW_IDf, ctf_id);

#ifdef CRASH_RECOVERY_SUPPORT
    soc_hw_set_immediate_hw_access(unit);
#endif
    rv = WRITE_OAMP_SAT_RX_STATr(unit, reg_val);
#ifdef CRASH_RECOVERY_SUPPORT
    soc_hw_restore_immediate_hw_access(unit);
#endif

    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }

    rv = READ_OAMP_RX_STATS_DATA_3r(unit, reg_above64_val_3);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }
    rv = READ_OAMP_RX_STATS_DATA_4r(unit, reg_above64_val_4);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }

    /* 
     * Accumulated_statistics[999:0] -- RxStatsData_3 bits[639:80], RxStatsData_4 bits[439:0] 
     */
    /* received_packet_cnt  FlowStatAccumEntry1     RcvdPktCntrAccum bits[83:42] */
    offset_base = (80+42);
    len = 42;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_3, offset_base, len);
    COMPILER_64_SET(stat->received_packet_cnt, fld_64[1], fld_64[0]);
    /* out_of_order_packet_cnt  FlowStatAccumEntry1     OooPktCntrAccum bits[125:84] */
    offset_base += len;
    len = 42;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_3, offset_base, len);
    COMPILER_64_SET(stat->out_of_order_packet_cnt, fld_64[1], fld_64[0]);
    /* err_packet_cnt   FlowStatAccumEntry1 ErrPktCntrAccum bits[167:126] */
    offset_base += len;
    len = 42;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_3, offset_base, len);
    COMPILER_64_SET(stat->err_packet_cnt, fld_64[1], fld_64[0]);
    /* received_octet_cnt   FlowStatAccumEntry1 RcvdOctetCntrAccum bits[216:168] */
    offset_base += len;
    len = 49;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_3, offset_base, len);
    COMPILER_64_SET(stat->received_octet_cnt, fld_64[1], fld_64[0]);
    /* out_of_order_octet_cnt    FlowStatAccumEntry1    OooOctetCntrAccumLsb bits[249:217]
     *                           FlowStatAccumEntry2    OooOctetCntrAccumMsb bits[15:0]
     */
    offset_base += len;
    len = 49;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_3, offset_base, len);
    COMPILER_64_SET(stat->out_of_order_octet_cnt, fld_64[1], fld_64[0]);
    /* err_octet_cnt    FlowStatAccumEntry2 ErrOctetCntrAccum bits[64:16] */
    offset_base += len;
    len = 49;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_3, offset_base, len);
    COMPILER_64_SET(stat->err_octet_cnt, fld_64[1], fld_64[0]);
    /* uint64 last_packet_delay    FlowStatAccumEntry2 LastPktDelayAccum bits[96:65] */
    offset_base += len;
    len = 32;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_3, offset_base, len);
    COMPILER_64_SET(stat->last_packet_delay, fld_64[1], fld_64[0]);
    /* bin_delay_counters[0]    FlowStatAccumEntry2 FdvBin0PktCntrAccum bits[138:97] */
    offset_base += len;
    len = 42;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_3, offset_base, len);
    COMPILER_64_SET(stat->bin_delay_counters[0], fld_64[1], fld_64[0]);
    /* bin_delay_counters[1]    FlowStatAccumEntry2 FdvBin1PktCntrAccum bits[180:139] */
    offset_base += len;
    len = 42;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_3, offset_base, len);
    COMPILER_64_SET(stat->bin_delay_counters[1], fld_64[1], fld_64[0]);
    /* bin_delay_counters[2]    FlowStatAccumEntry2 FdvBin2PktCntrAccum bits[222:181] */
    offset_base += len;
    len = 42;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_3, offset_base, len);
    COMPILER_64_SET(stat->bin_delay_counters[2], fld_64[1], fld_64[0]);
    /* bin_delay_counters[3]     FlowStatAccumEntry2      FdvBin3PktCntrAccumLsb bits[249:223]
     *                           FlowStatAccumEntry3      FdvBin3PktCntrAccumMsb bits[14:0]
     */
    offset_base += len;
    len = 42;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_3, offset_base, len);
    COMPILER_64_SET(stat->bin_delay_counters[3], fld_64[1], fld_64[0]);
    /* bin_delay_counters[4]    FlowStatAccumEntry3 FdvBin4PktCntrAccum bits[56:15] */
    offset_base += len;
    len = 42;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_3, offset_base, len);
    COMPILER_64_SET(stat->bin_delay_counters[4], fld_64[1], fld_64[0]);

    /* OAMP_RX_STATS_DATA_3 and OAMP_RX_STATS_DATA_4
     * uint64 bin_delay_counters[5]    FlowStatAccumEntry3 FdvBin5PktCntrAccum bits[98:57] 
     */
    offset_base += len;
    /* Get bits[59:57] from OAMP_RX_STATS_DATA_3 */
    len = 3;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(&bin_delay_counters_5_3lsbs, 0, reg_above64_val_3, offset_base, len);
    offset_base = 0;
    len = 39;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_4, offset_base, len);
    bin_delay_counters_5_low =  (((fld_64[0] & 0x1FFFFFFF) << 3) | bin_delay_counters_5_3lsbs);
    bin_delay_counters_5_high = (((fld_64[1] & 0x7F) << 3) | (fld_64[0] >> 29));
    COMPILER_64_SET(stat->bin_delay_counters[5], bin_delay_counters_5_high, bin_delay_counters_5_low);
    /* bin_delay_counters[6]    FlowStatAccumEntry3 FdvBin6PktCntrAccum bits[140:99] */
    offset_base += len;
    len = 42;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_4, offset_base, len);
    COMPILER_64_SET(stat->bin_delay_counters[6], fld_64[1], fld_64[0]);
    /* bin_delay_counters[7]    FlowStatAccumEntry3 FdvBin7PktCntrAccum bits[182:141] */
    offset_base += len;
    len = 42;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_4, offset_base, len);
    COMPILER_64_SET(stat->bin_delay_counters[7], fld_64[1], fld_64[0]);
    /* bin_delay_counters[8]    FlowStatAccumEntry3 FdvBin8PktCntrAccum bits[224:183]
    */
    offset_base += len;
    len = 42;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_4, offset_base, len);
    COMPILER_64_SET(stat->bin_delay_counters[8], fld_64[1], fld_64[0]);
    /* bin_delay_counters[9]    FlowStatAccumEntry3    FdvBin9PktCntrAccumLsb bits[249:225]
     *                          FlowStatAccumEntry4    FdvBin9PktCntrAccumMsb bits[16:0]
     */
    offset_base += len;
    len = 42;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_4, offset_base, len);
    COMPILER_64_SET(stat->bin_delay_counters[9], fld_64[1], fld_64[0]);
    /* accumulated_delay_count	FlowStatAccumEntry4	AccumDelayCntrAccum bits[80:17] */
    offset_base += len;
    len = 64;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_4, offset_base, len);
    COMPILER_64_SET(stat->accumulated_delay_count, fld_64[1], fld_64[0]);
    /* max_packet_delay	FlowStatAccumEntry4	MaxPktDelayAccum bits [112:81] */
    offset_base += len;
    len = 32;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_4, offset_base, len);
    COMPILER_64_SET(stat->max_packet_delay, fld_64[1], fld_64[0]);
    /* min_packet_delay	FlowStatAccumEntry4	MinPktDelayAccum bits [144:113] */
    offset_base += len;
    len = 32;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_4, offset_base, len);
    COMPILER_64_SET(stat->min_packet_delay, fld_64[1], fld_64[0]);
    /* time_of_day_stamp	FlowStatAccumEntry4	TodStampAccum bits [208:145] */
    offset_base += len;
    len = 64;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_4, offset_base, len);
    COMPILER_64_SET(stat->time_of_day_stamp, fld_64[1], fld_64[0]);
    /* sec_in_avail_state_counter	FlowStatAccumEntry4	SecInAvailStateCntr bits[226:209] */
    offset_base += len;
    len = 16;
    fld_64[0] = fld_64[1] = 0;
    SHR_BITCOPY_RANGE(fld_64, 0, reg_above64_val_4, offset_base, len);
    COMPILER_64_SET(stat->sec_in_avail_state_counter, fld_64[1], fld_64[0]);

    /* last_packet_seq_number */
    rv = READ_OAMP_SAT_RX_FLOW_PARAMSm(unit, MEM_BLOCK_ANY, ctf_id, data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }

    /* Expected sequence number always increase 1 based on received packet.
     * So last packet sequence number will be expected sequence number -1.
     */
    fld_64[0] = fld_64[1] = 0;
    fld_64[0] = soc_mem_field32_get(unit, OAMP_SAT_RX_FLOW_PARAMSm, &data, EXP_SEQ_NUMBERf);
    fld_64[0]--;
    COMPILER_64_SET(stat->last_packet_seq_number, fld_64[1], fld_64[0]);

    return rv;
}

soc_error_t soc_sat_ctf_availability_config_set (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_t ctf_id,
	SOC_SAND_IN soc_sat_ctf_availability_config_t *config
	)
{
    int rv = SOC_E_NONE;
    uint64 reg_val;

    COMPILER_64_ZERO(reg_val);
    rv = READ_OAMP_SAT_RX_STATr(unit, &reg_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }
    soc_reg64_field32_set(unit, OAMP_SAT_RX_STATr, &reg_val, RX_STATS_FLOW_IDf, ctf_id);
    soc_reg64_field32_set(unit, OAMP_SAT_RX_STATr, &reg_val, RX_STATS_NEW_NUM_SLTSf, config->switch_state_num_of_slots);
    soc_reg64_field32_set(unit, OAMP_SAT_RX_STATr, &reg_val, RX_STATS_NEW_THRSHf , config->switch_state_threshold_per_slot);
    rv = WRITE_OAMP_SAT_RX_STATr(unit, reg_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
    }

    soc_reg64_field32_set(unit, OAMP_SAT_RX_STATr, &reg_val, RX_STATS_RST_STATE_MCHNf, 1);
    rv = WRITE_OAMP_SAT_RX_STATr(unit, reg_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                (BSL_META_U(unit,
                            "%s\n"), soc_errmsg(rv)));
    }
    sal_udelay(1000);
    soc_reg64_field32_set(unit, OAMP_SAT_RX_STATr, &reg_val, RX_STATS_RST_STATE_MCHNf, 0);
    rv = WRITE_OAMP_SAT_RX_STATr(unit, reg_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                (BSL_META_U(unit,
                            "%s\n"), soc_errmsg(rv)));
    }
    
    return rv;
}

soc_error_t soc_sat_ctf_trap_data_to_session_map (
	SOC_SAND_IN int unit,
	SOC_SAND_IN uint32 trap_data, 
	SOC_SAND_IN uint32 trap_data_mask,
	SOC_SAND_IN uint32 entry_idx,
	SOC_SAND_IN uint32 session_id
    )
{
    int rv = SOC_E_NONE;
    uint64 data; /* Entry include 33 bits - Key bits[13:0], Mask bits[27:14] Dat bits[31:28] Valid[32] */
    int valid_fld = 1;

    COMPILER_64_ZERO(data);
    rv = READ_OAMP_RX_OAM_ID_TCAMm(unit, MEM_BLOCK_ANY, entry_idx, &data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }

    soc_mem_field32_set(unit, OAMP_RX_OAM_ID_TCAMm, &data, KEYf, trap_data);
    soc_mem_field32_set(unit, OAMP_RX_OAM_ID_TCAMm, &data, MASKf, trap_data_mask);
    soc_mem_field32_set(unit, OAMP_RX_OAM_ID_TCAMm, &data, DATf, session_id);
    soc_mem_field32_set(unit, OAMP_RX_OAM_ID_TCAMm, &data, VALIDf, valid_fld);


    if (SOC_IS_QUX(unit)) {
        rv = soc_sat_dynamic_memory_access_set(unit, 1);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in soc_sat_dynamic_memory_access_set\n"), soc_errmsg(rv)));
            return rv;
        }
    }

    rv = WRITE_OAMP_RX_OAM_ID_TCAMm(unit, MEM_BLOCK_ALL, entry_idx, &data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
    }

    if (SOC_IS_QUX(unit)) {
        rv = soc_sat_dynamic_memory_access_set(unit, 0);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in soc_sat_dynamic_memory_access_set\n"), soc_errmsg(rv)));
            return rv;
        }
    }

    return rv;
}

soc_error_t soc_sat_ctf_trap_data_to_session_unmap (
	SOC_SAND_IN int unit,
	SOC_SAND_IN uint32 entry_idx
    )
{
    int rv = SOC_E_NONE;
    uint64 data;/* Entry include 33 bits - Key bits[13:0], Mask bits[27:14] Dat bits[31:28] Valid[32] */
    int valid_fld = 0;
    uint32 zero = 0;

    COMPILER_64_ZERO(data);
    rv = READ_OAMP_RX_OAM_ID_TCAMm(unit, MEM_BLOCK_ANY, entry_idx, &data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }

    soc_mem_field32_set(unit, OAMP_RX_OAM_ID_TCAMm, &data, KEYf, zero);
    soc_mem_field32_set(unit, OAMP_RX_OAM_ID_TCAMm, &data, MASKf, zero);
    soc_mem_field32_set(unit, OAMP_RX_OAM_ID_TCAMm, &data, DATf, zero);
    soc_mem_field32_set(unit, OAMP_RX_OAM_ID_TCAMm, &data, VALIDf, valid_fld);

    if (SOC_IS_QUX(unit)) {
        rv = soc_sat_dynamic_memory_access_set(unit, 1);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in soc_sat_dynamic_memory_access_set\n"), soc_errmsg(rv)));
            return rv;
        }
    }

    rv = WRITE_OAMP_RX_OAM_ID_TCAMm(unit, MEM_BLOCK_ALL, entry_idx, &data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
    }

    if (SOC_IS_QUX(unit)) {
        rv = soc_sat_dynamic_memory_access_set(unit, 0);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in soc_sat_dynamic_memory_access_set\n"), soc_errmsg(rv)));
            return rv;
        }
    }

    return rv;
}

soc_error_t soc_sat_ctf_reports_config_set (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_t ctf_id,
	SOC_SAND_IN soc_sat_ctf_report_config_t *reports
    )
{
    int rv = SOC_E_NONE;
    uint32 field  = 0;
    uint32
      data[SOC_SAT_CTF_RX_FLOW_PARAMS_ENTRY_SIZE];
    
    /* For writing on dynamic tables (in the OAMP), have to enable dynamic memory access */
    rv = soc_sat_dynamic_memory_access_set(unit, 1);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }

    sal_memset(data, 0, sizeof(data));
    rv = READ_OAMP_SAT_RX_FLOW_PARAMSm(unit, MEM_BLOCK_ANY, ctf_id, data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        goto exit;
    }
    /* TIME_STAMP_OFFSETf */
    if (reports->report_sampling_percent == 0){
        /* never report */
        field = 255;
    }
    else if (reports->report_sampling_percent == SOC_SAT_CTF_REPORT_SAMPLING_PER_MAX) {
        /* always report */
        field = 0;
    }
    else {
        field = 255 - (reports->report_sampling_percent*255/100);
    }

    soc_mem_field32_set(unit, OAMP_SAT_RX_FLOW_PARAMSm, &data, RPRT_LFSR_THREHOLDf, field);
    if (reports->flags & SOC_SAT_CTF_REPORT_ADD_SEQ_NUM) {
        field = 1;
    } else {
        field = 0;
    }
    soc_mem_field32_set(unit, OAMP_SAT_RX_FLOW_PARAMSm, &data, RPRT_PKT_SEQ_NUMf, field);
    if (reports->flags & SOC_SAT_CTF_REPORT_ADD_DELAY) {
        field = 1;
    } else {
        field = 0;
    }
    soc_mem_field32_set(unit, OAMP_SAT_RX_FLOW_PARAMSm, &data, RPRT_PKT_DELAYf, field);

    rv = WRITE_OAMP_SAT_RX_FLOW_PARAMSm(unit, MEM_BLOCK_ALL, ctf_id, data);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        goto exit;
    }
exit:
    /* After done, disable dynamic memory access */
    soc_sat_dynamic_memory_access_set(unit, 0);
    return rv;
}

soc_error_t soc_sat_oamp_stat_event_control(
	SOC_SAND_IN int unit,
	SOC_SAND_IN int enable)
{
    uint32 state, rval;

    if (enable) {
        state = 1;
    } else {
        state = 0;
    }

    SOC_IF_ERROR_RETURN(READ_OAMP_INTERRUPT_MASK_REGISTERr(unit, &rval));
    soc_reg_field_set(unit, OAMP_INTERRUPT_MASK_REGISTERr, &rval,
                      STAT_PENDING_EVENT_MASKf, state);
    SOC_IF_ERROR_RETURN(WRITE_OAMP_INTERRUPT_MASK_REGISTERr(unit, rval));

    return SOC_E_NONE;
}

soc_error_t soc_sat_oamp_stat_event_clear(
	SOC_SAND_IN int unit)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_OAMP_INTERRUPT_REGISTERr(unit, &rval));
    soc_reg_field_set(unit, OAMP_INTERRUPT_REGISTERr, &rval,
                      STAT_PENDING_EVENTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_OAMP_INTERRUPT_REGISTERr(unit, rval));

    return SOC_E_NONE;
}

soc_error_t soc_common_sat_handler_register(
    int unit,
    soc_sat_handler_t handler)
{
    int rv = SOC_E_NONE;

    _soc_sat_handler[unit] = handler;

    return rv;
}

soc_error_t soc_common_sat_handler_get(
    int unit,
    soc_sat_handler_t *handler)
{
    int rv = SOC_E_NONE;

    *handler = _soc_sat_handler[unit];

    return rv;
}

int soc_sat_session_inuse(
    SOC_SAND_IN int unit,
	SOC_SAND_IN int session_id)
{
    int rv = SOC_E_NONE;
    int idx;
    uint64 data;
    uint8 valid_fld = 0;
    uint32 session;

    COMPILER_64_ZERO(data);  
    for (idx = 0; idx < 16; idx++) {
        rv = READ_OAMP_RX_OAM_ID_TCAMm(unit, MEM_BLOCK_ANY, idx, &data);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "soc_common_sat_session_inuse: %s\n"), soc_errmsg(rv)));
            return TRUE;
        }

        valid_fld = soc_mem_field32_get(unit, OAMP_RX_OAM_ID_TCAMm, &data, VALIDf);
        session = soc_mem_field32_get(unit, OAMP_RX_OAM_ID_TCAMm, &data, DATf);

        if (valid_fld && (session_id == session)) {
            return TRUE;
        }
    }

    return FALSE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
soc_sat_wb_init(int unit)
{
    _soc_sat_data_t   *psat_data;
    int             rv = BCM_E_NONE;
    uint32          size;
    uint32          scache_handle;
    uint8           *scache_ptr = NULL;
    uint16          default_ver = SOC_SAT_WB_CURRENT_VERSION;
    uint16          recovered_ver = SOC_SAT_WB_CURRENT_VERSION;
    int             stable_size;
    int             create;

    if (SOC_IS_QAX(unit)) {
        return rv;
    }
    psat_data = &(soc_sat_data[unit]);

    /* check to see if an scache table has been configured */
    rv = soc_stable_size_get(unit, &stable_size);
    if (SOC_FAILURE(rv) || stable_size <= 0) {
        return rv;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_SOC_SAT_HANDLE, 0);
    create = (SOC_WARM_BOOT(unit) ? FALSE : TRUE);
    size = (SOC_WARM_BOOT(unit) ? 0 : sizeof(_soc_sat_data_t));

    /* on cold boot, setup scache */
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      create,
                                      &size, &scache_ptr,
                                      default_ver, &recovered_ver);
    if (BCM_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Error(%s) reading scache. scache_ptr:%p and len:%d\n"),
                   soc_errmsg(rv), scache_ptr, size));
        /* If warmboot initialization fails, skip using warmboot for sat */
        rv = SOC_E_NOT_FOUND;
        return rv;
    }

    LOG_VERBOSE(BSL_LS_SOC_SAT,
                (BSL_META_U(unit,
                            "SAT: allocating 0x%x (%d) bytes of scache:"),
                 size, size));
    
    if (SOC_WARM_BOOT(unit)) {
        soc_scache_handle_used_set(unit, scache_handle, size);
        /* if supporting warmboot, use scache */
        if (scache_ptr) {
            sal_memcpy(psat_data, scache_ptr, sizeof(_soc_sat_data_t));
        } 
    }

    return BCM_E_NONE;
}

int
soc_sat_wb_sync(int unit, int sync)
{
    int                     rv = BCM_E_NONE;
    uint8                   *scache_ptr = NULL;
    uint32                  scache_len = 0;
    soc_scache_handle_t     scache_handle;
    uint16                  default_ver = SOC_SAT_WB_CURRENT_VERSION;
    uint16                  recovered_ver = SOC_SAT_WB_CURRENT_VERSION;
    _soc_sat_data_t         *psat_data;
    int                     stable_size;

    if (SOC_IS_QAX(unit)) {
        return rv;
    }

    psat_data = &(soc_sat_data[unit]);

    if (SOC_WARM_BOOT(unit)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Cannot write to SCACHE during WarmBoot\n")));
        return SOC_E_INTERNAL;
    }
    /* check to see if an scache table has been configured */
    rv = soc_stable_size_get(unit, &stable_size);
    if (SOC_FAILURE(rv) || stable_size <= 0) {
        return rv;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_SOC_SAT_HANDLE, 0);

    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      FALSE, &scache_len, &scache_ptr,
                                      default_ver, &recovered_ver);
    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        LOG_ERROR(BSL_LS_SOC_SAT,
                  (BSL_META_U(unit,
                              "Error(%s) reading scache. scache_ptr:%p and len:%d\n"),
                   soc_errmsg(rv), scache_ptr, scache_len));
        return rv;
    }

    sal_memcpy(scache_ptr, psat_data, sizeof(_soc_sat_data_t));

    rv = soc_scache_handle_used_set(unit, scache_handle, sizeof(_soc_sat_data_t));

    if (sync) {
        rv = soc_scache_commit(unit);
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_SAT,
                      (BSL_META_U(unit,
                                  "Error(%s) sync'ing scache to Persistent memory. \n"),
                       soc_errmsg(rv)));
            return rv;
        }
    }

    return BCM_E_NONE;
}

int soc_sat_wb_deinit(int unit)
{
    int rv = SOC_E_NONE;
    _soc_sat_data_t *psat_data;

    if (SOC_IS_QAX(unit)) {
        return rv;
    }

    psat_data = &(soc_sat_data[unit]);
    sal_memset(psat_data, 0, sizeof(_soc_sat_data_t));

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */


#endif /* BCM_SAT_SUPPORT */
