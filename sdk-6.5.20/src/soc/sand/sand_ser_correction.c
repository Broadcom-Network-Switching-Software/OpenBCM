/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_SER

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/drv.h>
#include <soc/error.h>
 
#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <shared/bitop.h>
#include <bcm/error.h>

#include <soc/sand/sand_ser_correction.h>


#define WIDE_ECC_MAX_INTERVALS 10
#define WIDE_ECC_MAX_DATA_WIDTH 60
#define WIDE_ECC_MAX_ECC_WIDTH  10

static void copy_bits(
                      uint32 *src,
                      uint32  src_start,
                      uint32 *dst,
                      uint32  dst_start,
                      uint32  len
                      )
{
    int i,j;
    for (i=src_start,j=dst_start;i<len +src_start ;i++,j++) {
        int bit;
        
       SHR_BITTEST_RANGE(src, i, 1, bit);
       if (bit) {
            SHR_BITSET_RANGE(dst, j, 1) ;        

       }
       else {
           SHR_BITCLR_RANGE(dst, j, 1) ;        
       }
    }
}

static int is_eq_bits(
                      uint32 *src,
                      uint32  src_start,
                      uint32 *dst,
                      uint32  dst_start,
                      uint32  len
                      )
{
    int i,j;
    for (i=src_start,j=dst_start;i<len +src_start ;i++,j++) {
        int bit1,bit2;
        
        SHR_BITTEST_RANGE(src, i, 1, bit1);
        SHR_BITTEST_RANGE(dst, j, 1, bit2);
        if (bit1 != bit2) {
            return 0 ;          
       }
    }

    return 1;
}


static shr_error_e print_data(
                       int unit,
                       const uint32 len_bits,
                       const uint8 *data,
                       const char *data_name,
                       int debug)
{
    uint32 i;
    char *buf;

    SHR_FUNC_INIT_VARS(unit);
 
    if ((debug)) 
    {
        buf = sal_alloc(len_bits + 1, ""); 
        SHR_NULL_CHECK(buf, _SHR_E_PARAM, "buf");
        *buf ='\0';
        for (i=0;i<len_bits;i++) 
        {
            sal_strncat_s(buf, data[i] ? "1" : "0", len_bits+1);
        }

        cli_out("%s(l2m):%s\n",data_name,buf);
        sal_free(buf);
    }

exit:
    SHR_FUNC_EXIT;
}

static int get_ecc_bit_len(uint32 data_len_bits)
{
    uint32 ecc_len_bits;
    int pow;
    for (ecc_len_bits=0,pow=1;pow<(ecc_len_bits+data_len_bits+1);ecc_len_bits++,pow<<=1) 
    {
    }
    return ecc_len_bits;
}


static void data2bits(const uint32 data_len_bits,const uint32 *data,uint8 *bits)
{

    int j,i,start,end;
    int data_len_words = data_len_bits/(8*sizeof(uint32)) + (data_len_bits%(8*sizeof(uint32)) ? 1 : 0);

    for (j=0,i=0;i<data_len_words;i++) 
    {
        end = (i==(data_len_words-1) && (data_len_bits%(8*sizeof(uint32))) ? (data_len_bits%(8*sizeof(uint32))) : 8*sizeof(uint32));
        for (start=0;start<end;start++,j++)
        {
            bits[j] = (1<<start) & data[i] ? 1 : 0;
        }
    }
}


static void bits2data(const uint32 data_len_bits, uint32 *data,const uint8 *bits)
{

    int j,i,start,end;
    int data_len_words = data_len_bits/(8*sizeof(uint32)) + (data_len_bits%(8*sizeof(uint32)) ? 1 : 0);

    for (j=0,i=0;i<data_len_words;i++) 
    {
        data[i]=0;
        end = (i==(data_len_words-1) && (data_len_bits%(8*sizeof(uint32))) ? (data_len_bits%(8*sizeof(uint32))) : 8*sizeof(uint32));
        for (start=0;start<end;start++,j++) 
        {
            data[i] |= (bits[j] ? (1<<start) : 0);
        }
    }

}

static shr_error_e interleave_data_ecc(
        int unit,
        const uint32 data_len_bits,
        const uint8 *data, 
        const uint8 *ecc,
        int debug)
{
    uint32 ecc_len_bits = get_ecc_bit_len(data_len_bits);
    uint32 total_len_bits = data_len_bits + ecc_len_bits;
    char *buf = NULL, *buf4 = NULL;
    char buf2[12],buf3[12];
    int count;
    uint32 i,j,k,pos[1];

    SHR_FUNC_INIT_VARS(unit);

    if (debug) 
    {
        int sizeof_buf;
        int sizeof_buf4;

        sizeof_buf = ecc_len_bits * 5 + data_len_bits * 3 + 1;
        buf = sal_alloc(sizeof_buf, "interleave_data_ecc.buf"); 
        sizeof_buf4 = ecc_len_bits*5 + data_len_bits*3+1;
        buf4 = sal_alloc(sizeof_buf4,"interleave_data_ecc.buf4");
        SHR_NULL_CHECK(buf, _SHR_E_PARAM, "buf");
        SHR_NULL_CHECK(buf4, _SHR_E_PARAM, "buf4");

        *buf = '\0';
        *buf4 = '\0';

        for (i=0,j=0,k=0;i<total_len_bits;i++) 
        {
            *pos = i+1;
            SHR_BITCOUNT_RANGE(pos,count,0,sizeof(uint32)*8);
            
            if (count==1) 
            {
                sal_sprintf(buf2,"-(%d)-",ecc[k]);
                sal_sprintf(buf3,"(%2d) ",*pos);
                k++;
            }
            else 
            {
                sal_sprintf(buf2,"-%d-",data[j]);
                sal_sprintf(buf3,"%2d ",*pos);
                j++;
            }
            sal_strncat_s(buf, buf2, sizeof_buf);
            sal_strncat_s(buf4, buf3, sizeof_buf4);

        }
        cli_out("interleave daya,ecc%s\n",buf);
        cli_out("                   %s\n",buf4);
    }
exit:
    SHR_FREE(buf); 
    SHR_FREE(buf4); 
    SHR_FUNC_EXIT;
}


uint8 calc_parity(
        const uint32 data_len_bits,
        const uint8 *data)
{
    uint8 parity=0;
    uint32 i;
    for (i=0;i<data_len_bits;i++) 
    {
        parity ^= data[i];
    }

    return parity;
}

shr_error_e calc_ecc(
        int unit,
        const uint32 data_len_bits,
        const uint32 *data,
        uint32 *ecc_data)
{
    uint32 ecc_len_bits = get_ecc_bit_len(data_len_bits);
    uint8 *data_bits = sal_alloc(data_len_bits,"");
    uint8 *calc_ecc_bits = sal_alloc(ecc_len_bits+1,"");
    uint32 i,j,k,l, pos[1];
    int count, parity_check = 0;
    uint32 total_len_bits = data_len_bits + ecc_len_bits;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(data_bits, _SHR_E_PARAM, "data_bits");
    SHR_NULL_CHECK(calc_ecc_bits, _SHR_E_PARAM, "calc_ecc_bits");

    sal_memset(calc_ecc_bits,0,ecc_len_bits+1);
    data2bits(data_len_bits, data, data_bits);

    
    for (i=0,j=0;i<total_len_bits;i++) {
        *pos = i+1;
        SHR_BITCOUNT_RANGE(pos,count,0,sizeof(uint32)*8);
        if (count==1) {
            continue;
        }

        if (data_bits[j]) {
            for (l=0,k=1;k<*pos;k<<=1,l++) {
                if (!(k & *pos)) {
                    continue;
                }
                calc_ecc_bits[l] ^= 1;
            }
        }

        j++;
    }

    for (i = 0; i < data_len_bits; i++) {
        if (data_bits[i] == 1) {
            parity_check++;
        }
    }

    for (i = 0; i < ecc_len_bits; i++) {
        if (calc_ecc_bits[i] == 1) {
            parity_check++;
        }
    }

    
    if (parity_check % 2 != 0) {
        calc_ecc_bits[ecc_len_bits] = 1;
    }
    bits2data(ecc_len_bits+1,ecc_data,calc_ecc_bits);

exit:
    SHR_FREE(calc_ecc_bits);
    SHR_FREE(data_bits);
    SHR_FUNC_EXIT;
}

static
shr_error_e ecc_correction_std(
        int unit,
        const uint32 data_len_bits,
        int xor_flag,
        uint32 *data, 
        uint32 *ecc)
{
    uint32 ecc_len_bits = get_ecc_bit_len(data_len_bits);
    uint8 *data_bits = sal_alloc(data_len_bits,"");
    uint8 *ecc_bits = sal_alloc(ecc_len_bits+1,"");
    uint8 *calc_ecc_bits = sal_alloc(ecc_len_bits+1,"");
    uint32 total_len_bits = data_len_bits + ecc_len_bits;
    uint32 i,j,k,l;
    int count, parity_check;
    uint32 bit1;
    uint32 index=0;
    uint32 pos[1];
    uint32 bits_for_count[1];
    uint32 double_check_ecc[1];

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(data_bits, _SHR_E_PARAM, "data_bits");
    SHR_NULL_CHECK(ecc_bits, _SHR_E_PARAM, "ecc_bits");
    SHR_NULL_CHECK(calc_ecc_bits, _SHR_E_PARAM, "calc_ecc_bits");

    sal_memset(calc_ecc_bits,0,ecc_len_bits);

    data2bits(data_len_bits, data, data_bits); 
    data2bits(ecc_len_bits+1,ecc,ecc_bits);

    SHR_IF_ERR_EXIT(print_data(unit,data_len_bits, data_bits, "input    data", 0));
    SHR_IF_ERR_EXIT(print_data(unit, ecc_len_bits+1, ecc_bits,  "input    ecc", 0));
    
    for (i=0,j=0;i<total_len_bits;i++) {
        
        *pos = i+1;
        SHR_BITCOUNT_RANGE(pos,count,0,sizeof(uint32)*8);
        
        if (count==1) {
            continue;
        }
        
        bit1 = data_bits[j];
        if (bit1) {
            
            for (l=0,k=1;k<*pos;k<<=1,l++) {
                if (!(k & *pos)) {
                    continue;
                }
                
                calc_ecc_bits[l] ^= 1;
            }
        }

        
        j++;

    }
    SHR_IF_ERR_EXIT(interleave_data_ecc(unit, data_len_bits,data_bits,ecc_bits, 0));
    SHR_IF_ERR_EXIT(interleave_data_ecc(unit, data_len_bits,data_bits,calc_ecc_bits, 0));

    parity_check = 0;
    for (i = 0; i < data_len_bits; i++) {
        if (data_bits[i] == 1) {
            parity_check++;
        }
    }
    for (i = 0; i < ecc_len_bits; i++) {
        if (calc_ecc_bits[i] == 1) {
            parity_check++;
        }
    }
    
    calc_ecc_bits[ecc_len_bits] = 0;
    if (parity_check % 2 != 0) {
        calc_ecc_bits[ecc_len_bits] = 1;
    }

    
    for (i=0,count=0;i<ecc_len_bits+1;i++) {
        if (ecc_bits[i]!=calc_ecc_bits[i]) {
            *pos = i;
            count++;
        }
    }
    if (!count) 
    {
        cli_out("error was not found\n");
        SHR_IF_ERR_EXIT(print_data(unit, data_len_bits, data_bits, "corrcted data", 0));
        SHR_IF_ERR_EXIT(print_data(unit, ecc_len_bits+1, calc_ecc_bits, "calc     ecc", 0));

        SHR_EXIT();
    }

    if (count==1) {
        bits2data(ecc_len_bits+1,ecc,calc_ecc_bits);
        SHR_IF_ERR_EXIT(print_data(unit, ecc_len_bits, calc_ecc_bits, "correct  ecc", 0));

        cli_out("the error bit is in ecc field at pos %d, corrected ecc is %x\n", *pos, ecc[0]); 
        SHR_EXIT();
    }
    
    for (i=0;i<ecc_len_bits;i++) {
        if (ecc_bits[i] ==  calc_ecc_bits[i]) {
            continue;
        }
        index += (1<<i);
    }

    
    for (i=4,j=3;i<index;i<<=1,j++) {
    }

    j=-1;
    for (i=0;i<=index;i++) {
        *bits_for_count = i;
        SHR_BITCOUNT_RANGE(bits_for_count,count,0,sizeof(uint32)*8);
        
        if (count<=1) {
            continue;
        }
        j++;
    }
    index =j;
    
    if (index < data_len_bits) {
        data_bits[index] = !data_bits[index];
    }

    SHR_IF_ERR_EXIT(print_data(unit, data_len_bits, data_bits, "corrcted data", 0));
    SHR_IF_ERR_EXIT(print_data(unit, ecc_len_bits+1, calc_ecc_bits, "calc     ecc", 0));
    bits2data(data_len_bits,data,data_bits);
    cli_out("corrected data value %x\n",data[0]); 

exit:
    calc_ecc(unit, data_len_bits, data, double_check_ecc);
    
    if (double_check_ecc[0] != ecc[0])
    {
        SHR_IF_ERR_EXIT(print_data(unit, data_len_bits, data_bits, "corrcted data", 1));
        SHR_IF_ERR_EXIT(print_data(unit, ecc_len_bits+1, calc_ecc_bits, "calc     ecc", 1));
        
        SHR_IF_ERR_CONT(_SHR_E_MEMORY);
    }
    sal_free(calc_ecc_bits);
    sal_free(data_bits);
    SHR_FREE(ecc_bits);
    SHR_FUNC_EXIT;
}

static
shr_error_e ecc_correction_wide(
        int unit,
        const uint32 data_len_bits,
        int xor_flag,
        uint32 *data, 
        uint32 *ecc)
{
    uint32 data_portions_array[WIDE_ECC_MAX_INTERVALS]={0};

    uint32 tmp_data_portion[WIDE_ECC_MAX_DATA_WIDTH]={0};
    uint32 tmp_ecc_portion[WIDE_ECC_MAX_ECC_WIDTH]={0};
    uint32 tmp_data_portion_orig[WIDE_ECC_MAX_DATA_WIDTH]={0};
    uint32 tmp_ecc_portion_orig[WIDE_ECC_MAX_ECC_WIDTH]={0};

    uint32 nof_portions=0;
    uint32 total_width = data_len_bits;
    int min_portion = 247;
    int max_portion = 502;
    int i,j;
    int ecc_pos=0,data_pos=0;

    SHR_FUNC_INIT_VARS(unit);

    while (total_width > 0) {
          if (total_width > (max_portion+min_portion)) {

             data_portions_array[nof_portions++] = max_portion;
             total_width -= max_portion;
          } else if (total_width > max_portion) {
              if (SOC_IS_JERICHO_AND_BELOW(unit)) {
                  data_portions_array[nof_portions++] = (total_width - min_portion);
                  total_width -= (total_width-min_portion); 
              }
              else
              {
                  data_portions_array[nof_portions++] = min_portion;
                  total_width -= min_portion; 
              }
          } else {
              data_portions_array[nof_portions++] = total_width;
              total_width = 0;
          }
       }

    for (j=nof_portions-1,i=0;i<nof_portions;i++,j--) {
        int data_portion_len = data_portions_array[i];
        uint32 ecc_portion_len = get_ecc_bit_len(data_portion_len) + 1;

        copy_bits(data,data_pos,tmp_data_portion,0,data_portion_len);
        copy_bits(data,data_pos,tmp_data_portion_orig,0,data_portion_len);

        copy_bits(ecc,ecc_pos,tmp_ecc_portion,0,ecc_portion_len);
        copy_bits(ecc,ecc_pos,tmp_ecc_portion_orig,0,ecc_portion_len);

        SHR_IF_ERR_EXIT(ecc_correction_std(unit,data_portion_len,xor_flag, tmp_data_portion, tmp_ecc_portion));

        if (!is_eq_bits(tmp_data_portion,0,tmp_data_portion_orig,0,data_portion_len)) 
        {
            copy_bits(tmp_data_portion,0,data,data_pos,data_portion_len);
        }

        if (!is_eq_bits(tmp_ecc_portion,0,tmp_ecc_portion_orig,0,ecc_portion_len)) 
        {
            copy_bits(tmp_ecc_portion,0,ecc,ecc_pos,ecc_portion_len);
        }

        data_pos+=data_portion_len;
        ecc_pos+=ecc_portion_len;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e sand_mdb_ecc_correction(
    int unit,
    const uint32 data_len_bits,
    uint32* data,
    uint32* ecc)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ecc_correction_wide(unit, data_len_bits, 1, data, ecc));

exit:
    SHR_FUNC_EXIT;
}



shr_error_e ecc_correction(
        int unit,
        soc_mem_t mem,
        const uint32 data_len_bits,
        int xor_flag,
        uint32 *data, 
        uint32 *ecc)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_MEM_ECC_TYPE(unit,mem) ==SOC_MEM_ECC_TYPE_WIDE)
    {
        SHR_IF_ERR_EXIT(ecc_correction_wide(unit,data_len_bits,xor_flag,data, ecc));
    }
    else
    {
        if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit))
        {
            SHR_IF_ERR_EXIT(ecc_correction_std(unit,data_len_bits,xor_flag, data, ecc));
        }
        else
        {
            SHR_IF_ERR_EXIT(ecc_correction_wide(unit,data_len_bits,xor_flag,data, ecc));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

