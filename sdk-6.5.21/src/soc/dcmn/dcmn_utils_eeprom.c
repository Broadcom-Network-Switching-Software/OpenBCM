/****************************************************************** 
*                                                                   
* FILENAME:       utils_eeprom.c                      
* 
*                                                                   
* MODULE PREFIX:  $prefix$                               
*                                                                   
* SYSTEM:           Broadcom LTD Proprietary information
*                                                                   
* CREATION DATE:  $date$                                  
*                                                                   
* LAST CHANGED:   $date$
*                                                                   
* REVISION:       $revision$                                       
*                                                                   
* FILE DESCRIPTION:                                                 
*                                                                   
* REMARKS:                                                          
*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */   
#if defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__)



#include <shared/bsl.h>

#include <sal/types.h>
#include <soc/drv.h>
#include <soc/i2c.h>

#include <soc/dcmn/dcmn_utils_eeprom.h>

#include <appl/diag/dcmn/bsp_cards_consts.h> 


int negev_chassis_eeprom_write(int addr, int size, int val)
{
    int ret = 0;
    int i;
    int eeprom_addr = 0, eeprom_int_addr = 0, cur_val = 0;

    if (addr >= NEGEV_CHASSIS_EEPROM_NUM_MAX_ADRESS) {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): Error: Adress is too high. addr=%d\n"), FUNCTION_NAME(), addr));
        ret = -1;
        goto exit;
    } else if (addr < NEGEV_CHASSIS_EEPROM_NUM_ADRESS_PER_BANK) {
        eeprom_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BANK_1;
        eeprom_int_addr = addr;
    } else {
        eeprom_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BANK_2;
        eeprom_int_addr = addr - NEGEV_CHASSIS_EEPROM_NUM_ADRESS_PER_BANK;
    }

    for(i=0 ; i < size ; i++) {
        cur_val = (val >> (i * 8)) & 0xff;
        
        

        ret = cpu_i2c_write(eeprom_addr, eeprom_int_addr, CPU_I2C_ALEN_BYTE_DLEN_BYTE, cur_val);
        if (ret != 0) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META("%s(): Error: cpu_i2c_write Failed!!! ret=%d\n"), FUNCTION_NAME(), ret));
            goto exit;
        }

        eeprom_int_addr++;

        
        sal_usleep(60000);
    }

exit:
    return ret;
}

int negev_chassis_eeprom_write_str(int addr, int size, char *str)
{
    int ret = 0;
    int i;
    int eeprom_addr = 0, eeprom_int_addr = 0, cur_val = 0;

    if (addr >= NEGEV_CHASSIS_EEPROM_NUM_MAX_ADRESS) {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): Error: Adress is too high. addr=%d\n"), FUNCTION_NAME(), addr));
        ret = -1;
        goto exit;
    } else if (addr < NEGEV_CHASSIS_EEPROM_NUM_ADRESS_PER_BANK) {
        eeprom_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BANK_1;
        eeprom_int_addr = addr;
    } else {
        eeprom_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BANK_2;
        eeprom_int_addr = addr - NEGEV_CHASSIS_EEPROM_NUM_ADRESS_PER_BANK;
    }

    for(i=0 ; i < size ; i++) {
        cur_val = str[i] & 0xff;
      

        ret = cpu_i2c_write(eeprom_addr, eeprom_int_addr, CPU_I2C_ALEN_BYTE_DLEN_BYTE, cur_val);
        if (ret != 0) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META("%s(): Error: cpu_i2c_write Failed!!! ret=%d\n"), FUNCTION_NAME(), ret));
            goto exit;
        }

        eeprom_int_addr++;

        
        sal_usleep(60000);
    }

exit:
    return ret;
}

int negev_chassis_eeprom_read(int addr, int size, int *val)
{
#ifndef __KERNEL__
    int ret = 0;
    int i;
    int eeprom_addr = 0, eeprom_int_addr = 0, cur_val = 0;

    if (addr >= NEGEV_CHASSIS_EEPROM_NUM_MAX_ADRESS) {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): Error: Adress is too high. addr=%d\n"), FUNCTION_NAME(), addr));
        ret = -1;
        goto exit;
    } else if (addr < NEGEV_CHASSIS_EEPROM_NUM_ADRESS_PER_BANK) {
        eeprom_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BANK_1;
        eeprom_int_addr = addr;
    } else {
        eeprom_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BANK_2;
        eeprom_int_addr = addr - NEGEV_CHASSIS_EEPROM_NUM_ADRESS_PER_BANK;
    }

    *val = 0x0;

    for(i=0 ; i < size ; i++) {

        ret = cpu_i2c_read(eeprom_addr, eeprom_int_addr, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &cur_val);
        if (ret != 0) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META("%s(): Error: cpu_i2c_read Failed!!! ret=%d\n"), FUNCTION_NAME(), ret));
            goto exit;
        }

        

        *val |= (cur_val & 0xff) << (i * 8);

        eeprom_int_addr++;

        
        sal_usleep(60000);
    }

exit:
    return ret;
#endif 
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("This function is unavailable in Kernel mode\n")));
    return -1;
}

int negev_chassis_eeprom_read_str(int addr, int size, char *str)
{
#ifndef __KERNEL__
    int ret = 0;
    int i;
    int eeprom_addr = 0, eeprom_int_addr = 0, cur_val = 0;

    if (addr >= NEGEV_CHASSIS_EEPROM_NUM_MAX_ADRESS) {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): Error: Adress is too high. addr=%d\n"), FUNCTION_NAME(), addr));
        ret = -1;
        goto exit;
    } else if (addr < NEGEV_CHASSIS_EEPROM_NUM_ADRESS_PER_BANK) {
        eeprom_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BANK_1;
        eeprom_int_addr = addr;
    } else {
        eeprom_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BANK_2;
        eeprom_int_addr = addr - NEGEV_CHASSIS_EEPROM_NUM_ADRESS_PER_BANK;
    }

    for(i=0 ; i < size ; i++) {

        ret = cpu_i2c_read(eeprom_addr, eeprom_int_addr, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &cur_val);
        if (ret != 0) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META("%s(): Error: cpu_i2c_read Failed!!! ret=%d\n"), FUNCTION_NAME(), ret));
            goto exit;
        }

      

        str[i] = (cur_val & 0xff);

        eeprom_int_addr++;

        
        sal_usleep(60000);
    }

exit:
    return ret;
#endif 
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("This function is unavailable in Kernel mode\n")));
    return -1;
}

int eeprom_write(unsigned short card_type, int addr, int size, int val)
{

    int ret = 0;

    switch (card_type) {
    case NEGEV_CHASSIS_CARD_TYPE:
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN:
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN_2:
    case FABRIC_CARD_FE1600:
    case FABRIC_CARD_FE1600_BCM88754:
    case NEGEV2_MNG_CARD:
    case LINE_CARD_ARAD:
    case LINE_CARD_ARAD_DVT:
    case LINE_CARD_ARAD_NOACP:
    case FABRIC_CARD_FE3200:
        ret = negev_chassis_eeprom_write(addr, size, val);
        if (ret != 0) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META("%s(): Error: negev_chassis_eeprom_write Failed!!! ret=%d\n"), FUNCTION_NAME(), ret));
            goto exit;
        }
        break;
    default:
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): Error: UnKnown card_type=0x%x\n"), FUNCTION_NAME(), card_type));
        ret = -1;
        goto exit;
    }

exit:
    return ret;
}

int eeprom_write_str(unsigned short card_type, int addr, int size, char *str)
{

    int ret = 0;

    switch (card_type) {
    case NEGEV_CHASSIS_CARD_TYPE:
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN:
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN_2:
    case FABRIC_CARD_FE1600:
    case FABRIC_CARD_FE1600_BCM88754:
    case NEGEV2_MNG_CARD:
    case LINE_CARD_ARAD:
    case LINE_CARD_ARAD_DVT:
    case LINE_CARD_ARAD_NOACP:
    case FABRIC_CARD_FE3200:
        ret = negev_chassis_eeprom_write_str(addr, size, str);
        if (ret != 0) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META("%s(): Error: negev_chassis_eeprom_write_str Failed!!! ret=%d\n"), FUNCTION_NAME(), ret));
            goto exit;
        }
        break;
    default:
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): Error: UnKnown card_type=0x%x\n"), FUNCTION_NAME(), card_type));
        ret = -1;
        goto exit;
    }

exit:
    return ret;
}

int eeprom_read(unsigned short card_type, int addr, int size, int *val)
{

    int ret = 0;

    switch (card_type) {
    case NEGEV_CHASSIS_CARD_TYPE:
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN:
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN_2:
    case FABRIC_CARD_FE1600:
    case FABRIC_CARD_FE1600_BCM88754:
    case NEGEV2_MNG_CARD:
    case LINE_CARD_ARAD:
    case LINE_CARD_ARAD_DVT:
    case LINE_CARD_ARAD_NOACP:
    case FABRIC_CARD_FE3200:
        ret = negev_chassis_eeprom_read(addr, size, val);
        if (ret != 0) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META("%s(): Error: negev_chassis_eeprom_read Failed!!! ret=%d\n"), FUNCTION_NAME(), ret));
            goto exit;
        }
        break;
    default:
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): Error: UnKnown card_type=0x%x\n"), FUNCTION_NAME(), card_type));
        ret = -1;
        goto exit;
    }

exit:
    return ret;
}

int eeprom_read_str(unsigned short card_type, int addr, int size, char *str)
{

    int ret = 0;

    switch (card_type) {
    case NEGEV_CHASSIS_CARD_TYPE:
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN:
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN_2:
    case FABRIC_CARD_FE1600:
    case FABRIC_CARD_FE1600_BCM88754:
    case NEGEV2_MNG_CARD:
    case LINE_CARD_ARAD:
    case LINE_CARD_ARAD_DVT:
    case LINE_CARD_ARAD_NOACP:
    case FABRIC_CARD_FE3200:
        ret = negev_chassis_eeprom_read_str(addr, size, str);
        if (ret != 0) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META("%s(): Error: negev_chassis_eeprom_read_str Failed!!! ret=%d\n"), FUNCTION_NAME(), ret));
            goto exit;
        }
        break;
    default:
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): Error: UnKnown card_type=0x%x\n"), FUNCTION_NAME(), card_type));
        ret = -1;
        goto exit;
    }

exit:
    return ret;
}
#else

#include <sal/types.h>
#endif
