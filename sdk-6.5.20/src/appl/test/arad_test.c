/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Built-in Self Test for ARAD
 */

#include <shared/bsl.h>
#include <appl/diag/system.h> /* for parse_small_integers */

#include <soc/debug.h>

#include <bcm/init.h>
#include <bcm/error.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm/switch.h>
#endif /*BCM_WARM_BOOT_SUPPORT*/

#include <sal/appl/io.h>
#include <sal/appl/sal.h>

#include <appl/diag/parse.h>
#include <appl/diag/test.h>
#include <appl/diag/shell.h>

#include <appl/diag/dcmn/init.h>
#include <soc/dpp/dpp_config_defs.h>
#ifdef BCM_PETRA_SUPPORT

#include <soc/dpp/TMC/tmc_api_dram.h>
#include <soc/dpp/ARAD/arad_dram.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>

#include <soc/dpp/DRC/drc_combo28_bist.h>
#include <soc/dpp/DRC/drc_combo28_cb.h>

#include <appl/dcmn/interrupts/interrupt_handler.h>

#include <soc/dpp/drv.h> /* for SOC_DPP_CONFIG(unit) */

#include <sal/core/time.h>

char arad_bist_test_usage[] = 
"Bist Memory Test Usage:\n"
"  DramNdx=<value>         Dram index to run test on, default is dram 0.\n"
"                          To run the test for all of the drams set this value to 0xff\n"
"  WriteWeight=<value>     Writing weight, default is WriteWeight=1.\n"                    
"  ReadWeight=<value>      Reading weight, default is ReadWeight=1.\n"
"  BistNumActions=<value>  How many times to run test, default: BistNumActions=0x1\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nFull documentation cannot be displayed with -pendantic compiler\n";
#else
"  StartAddr=<0xvalue>     Specifies starting memory address\n"
"  EndAddr=<0xvalue>       Sspecifies ending memory address\n"
"  PatternMode=<value>     Pattern mode. It can be one of the following values:\n"
"                    0:    Costum pattern. Use the pattern as is.\n"
"                    1:    The PRBS will be used to generate the pattern towards the DRAM.\n"
"                    2:    Fill the data to write by 1010101... (Bits).\n"
"                    3:    Fill the data to write by 11111111... (Bits).\n"
"                    4:    Fill the data to write by 00000000... (Bits).\n"
"                    5:    A different bit is selected from pattern0-7 in an incremental manner.\n"
"                          The selected bit is duplicated on all of the dram data bus.\n"
"                    6:    Data shift mode.\n"
"                          Every write/read command the data will be shifted 1 bit to the left in a cyclic manner.\n"
"                          The initial pattern is pattern0 which is duplicated 8 times.\n"
"                          This DATA_MODE create 8 consecutive dram transactions.\n"
"                    7:    Data address mode.\n"
"                          Every command data will be equal to the address it is written to.\n"
"  Pattern0=<0xvalue>      Pattern 0. In pattern modes 1-4, 7 This value is ignored.\n"
"  Pattern1=<0xvalue>      Pattern 1. In pattern modes 1-4, 6-7 This value is ignored.\n"
"  Pattern2=<0xvalue>      Pattern 2. only for pattern modes 0, 5.\n"
"  Pattern3=<0xvalue>      Pattern 3. only for pattern modes 0, 5.\n"
"  Pattern4=<0xvalue>      Pattern 4. only for pattern modes 0, 5.\n"
"  Pattern5=<0xvalue>      Pattern 5. only for pattern modes 0, 5.\n"
"  Pattern6=<0xvalue>      Pattern 6. only for pattern modes 0, 5.\n"
"  Pattern7=<0xvalue>      Pattern 7. only for pattern modes 0, 5.\n"
"\n"
"  DRC COMBO28 options:\n"
"    Seed0=<0xvalue> Seed1=<0xvalue> ... Seed7=<0xvalue>  - Bist PRBS data seed.\n"
"    UseRandomSeed =<0/1> -   If this flag is set, seed will be chosen randomly.\n"  
"\n"
"  Common Flags :\n"
"    Infinite=<0/1>          If this flag is set, the test will be run infinately.\n"
"    Stop=<0/1>              If this flag is set, it will stop the test.\n"
"    GetData=<0/1>           If this flag is set when the test run infinately, it will get the results\n"
"    AllAdress=<0/1>         Perform BIST on all address range. all other params are ignored. set to 1 by default.\n"
"    NoReset=<0/1>           If this flag is set will not do soft reset after the test, set to 0 by default.\n"   
"    AddressShidtMode=<0/1>  If this flag is set, the address will be shift by 1 bit (from msb) in a cyclic manner every read/write command.\n"
"    TwoAddrMode=<0/1>       If this flag is set, the address used by the BIST always alternate between BIST start address and BIST end address.\n"
"  Arad Flags :\n"
"    ConsAddr8Banks=<0/1>    If this flag is set, the BIST will send 4 consecutive commands to the same bank before incrementing the bank number. default: ConsAddr8Banks=0x1.\n"
"  DRC_COMBO28 Flags :\n"
"    BGInterleave=<0/1>      If this flag is set, address bits 3:0 will be swapped in a manner that will interleave between bank groups in consecutive commands to improve max BIST rate\n"
"    SingelBankTest=<0/1>    If this flag is set, BIST swap address bits 9:6 with address bits 5:0 resulting in 64 consecutive commands to the same bank and row (incremented column).\n" 
;
#endif   /*COMPILER_STRING_CONST_LIMIT*/

#define SOC_SAND_FAILURE(_sand_ret) \
    ((handle_sand_result(_sand_ret)) < 0)


typedef struct arad_bist_test_params_s
{
    uint32 unit;
    SHR_BITDCLNAME(dram_ndx, SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX));
    uint32 no_reset;
    SOC_TMC_DRAM_BIST_INFO run_info;
}arad_bist_test_params_t;

static arad_bist_test_params_t      *arad_bist_test_params[SOC_MAX_NUM_DEVICES];

int
arad_bist_test_init(int unit, args_t *a, void **p)
{
    uint32 dram_ndx_max;
    uint32 dram_index;
    uint32 found_dram_interfaces = 0;
    arad_bist_test_params_t *abt;
    parse_table_t pt;
    char* dram_ndx_srt=NULL;
    uint32 cons_addr_8_banks, address_shidt_mode,
        infinite, all_adress, use_default_test, stop, get_data, two_addr_mode,
        bg_interleave, single_bank_test, address_prbs_mode, use_random_seed;
    int rv = -1;

    abt = arad_bist_test_params[unit];
    if (abt == NULL) {
        abt = sal_alloc(sizeof(arad_bist_test_params_t), "arad bist test");
        if (abt == NULL) 
        {
            LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "%s: cannot allocate memory test data\n"), ARG_CMD(a)));
            return -1;
        }
        sal_memset(abt, 0, sizeof(arad_bist_test_params_t));
        arad_bist_test_params[unit] = abt;
    }

    if(ARG_CNT(a) == 0 ){
        use_default_test = 1;
    } else {
        use_default_test = 0;
    }

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "DramNdx", PQ_STRING, (void *) (0),
            &(dram_ndx_srt), NULL);
    parse_table_add(&pt, "WriteWeight", PQ_INT, (void *) 1,
            &(abt->run_info.write_weight), NULL);
    parse_table_add(&pt, "ReadWeight", PQ_INT, (void*) 1,
            &(abt->run_info.read_weight), NULL);
    parse_table_add(&pt, "BistNumActions", PQ_INT, (void *) 1,
            &(abt->run_info.bist_num_actions), NULL);
    parse_table_add(&pt, "StartAddr", PQ_INT, (void *) 0,
            &(abt->run_info.bist_start_address), NULL);
    parse_table_add(&pt, "EndAddr", PQ_INT, (void *) (1),
            &(abt->run_info.bist_end_address), NULL);
    parse_table_add(&pt, "PatternMode", PQ_INT, (void *) (0),
            &(abt->run_info.pattern_mode), NULL);
    parse_table_add(&pt, "Pattern0", PQ_INT, (void *) (0),
            &(abt->run_info.pattern[0]), NULL);
    parse_table_add(&pt, "Pattern1", PQ_INT, (void *) (0),
            &(abt->run_info.pattern[1]), NULL);
    parse_table_add(&pt, "Pattern2", PQ_INT, (void *) (0),
            &(abt->run_info.pattern[2]), NULL);
    parse_table_add(&pt, "Pattern3", PQ_INT, (void *) (0),
            &(abt->run_info.pattern[3]), NULL);
    parse_table_add(&pt, "Pattern4", PQ_INT, (void *) (0),
            &(abt->run_info.pattern[4]), NULL);
    parse_table_add(&pt, "Pattern5", PQ_INT, (void *) (0),
            &(abt->run_info.pattern[5]), NULL);
    parse_table_add(&pt, "Pattern6", PQ_INT, (void *) (0),
            &(abt->run_info.pattern[6]), NULL);
    parse_table_add(&pt, "Pattern7", PQ_INT, (void *) (0),
            &(abt->run_info.pattern[7]), NULL);
    parse_table_add(&pt, "ConsAddr8Banks", PQ_INT, (void *) (1),
            &cons_addr_8_banks, NULL); 
    parse_table_add(&pt, "Seed0", PQ_INT, (void *) (0),
            &(abt->run_info.data_seed[0]), NULL); 
    parse_table_add(&pt, "Seed1", PQ_INT, (void *) (0),
            &(abt->run_info.data_seed[1]), NULL);
    parse_table_add(&pt, "Seed2", PQ_INT, (void *) (0),
            &(abt->run_info.data_seed[2]), NULL);
    parse_table_add(&pt, "Seed3", PQ_INT, (void *) (0),
            &(abt->run_info.data_seed[3]), NULL);
    parse_table_add(&pt, "Seed4", PQ_INT, (void *) (0),
            &(abt->run_info.data_seed[4]), NULL);
    parse_table_add(&pt, "Seed5", PQ_INT, (void *) (0),
            &(abt->run_info.data_seed[5]), NULL);
    parse_table_add(&pt, "Seed6", PQ_INT, (void *) (0),
            &(abt->run_info.data_seed[6]), NULL);  
    parse_table_add(&pt, "Seed7", PQ_INT, (void *) (0),
            &(abt->run_info.data_seed[7]), NULL); 
    parse_table_add(&pt, "UseRandomSeed", PQ_INT, (void *) (0),
            &use_random_seed, NULL);        
    parse_table_add(&pt, "AddressShidtMode", PQ_INT, (void *) (0),
            &address_shidt_mode, NULL);
    parse_table_add(&pt, "Infinite", PQ_INT, (void *) (0),
            &infinite, NULL);
    parse_table_add(&pt, "AllAdress", PQ_INT, (void *) (0),
            &all_adress, NULL);
    parse_table_add(&pt, "Stop", PQ_INT, (void *) (0),
            &stop, NULL);
    parse_table_add(&pt, "GetData", PQ_INT, (void *) (0),
            &get_data, NULL);
    parse_table_add(&pt, "TwoAddrMode", PQ_INT, (void *) (0),
            &two_addr_mode, NULL);
    parse_table_add(&pt, "NoReset", PQ_INT, (void *) (0),
            &(abt->no_reset), NULL);
    parse_table_add(&pt, "BGInterleave", PQ_INT, (void *) (1),
            &bg_interleave, NULL);
    parse_table_add(&pt, "SingelBankTest", PQ_INT, (void *) (0),
            &single_bank_test, NULL);                      
    parse_table_add(&pt, "AddressPrbsMode", PQ_INT, (void *) (0),
            &address_prbs_mode, NULL);            

    if (parse_arg_eq(a, &pt) < 0) 
    {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "%s: Invalid option: %s\n"), ARG_CMD(a), ARG_CUR(a)));
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "%s\n"),arad_bist_test_usage));
        goto done;
    }

    if (ARG_CNT(a) != 0) 
    {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "%s: extra options starting with \"%s\"\n"), ARG_CMD(a), ARG_CUR(a)));
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "%s\n"),arad_bist_test_usage));
        goto done;
    }

    if( use_default_test != 0) 
    {
        all_adress = 1;
    }

    abt->unit = unit;

    /** set flags **/
    abt->run_info.arad_bist_flags = 0;
    if(cons_addr_8_banks) 
    {
        abt->run_info.arad_bist_flags |= SOC_TMC_DRAM_BIST_FLAGS_CONS_ADDR_8_BANKS;
    }
    if(address_shidt_mode) 
    {
        abt->run_info.arad_bist_flags |= SOC_TMC_DRAM_BIST_FLAGS_ADDRESS_SHIFT_MODE;
    }
    if(infinite) 
    {
        abt->run_info.arad_bist_flags |= SOC_TMC_DRAM_BIST_FLAGS_INFINITE;
    }
    if(all_adress) 
    {
        abt->run_info.arad_bist_flags |= SOC_TMC_DRAM_BIST_FLAGS_ALL_ADDRESS;
    }
    if(stop) 
    {
        abt->run_info.arad_bist_flags |= SOC_TMC_DRAM_BIST_FLAGS_STOP;
    }
    if(get_data) 
    {
        abt->run_info.arad_bist_flags |= SOC_TMC_DRAM_BIST_FLAGS_GET_DATA;
    }
    if(two_addr_mode) 
    {
        abt->run_info.arad_bist_flags |= SOC_TMC_DRAM_BIST_FLAGS_TWO_ADDRESS_MODE;
    }              
    if(bg_interleave)
    {
        abt->run_info.arad_bist_flags |= SOC_TMC_DRAM_BIST_FLAGS_BG_INTERLEAVE;
    }
    if(single_bank_test)
    {
        abt->run_info.arad_bist_flags |= SOC_TMC_DRAM_BIST_FLAGS_SINGLE_BANK_TEST;
    }
    if(address_prbs_mode)
    {
        abt->run_info.arad_bist_flags |= SOC_TMC_DRAM_BIST_FLAGS_ADDRESS_PRBS_MODE;
    }
    if(use_random_seed)
    {
        abt->run_info.arad_bist_flags |= SOC_TMC_DRAM_BIST_FLAGS_USE_RANDOM_DATA_SEED;
    }

    
    /** configuration validation **/
    if((abt->run_info.write_weight == 0) && (abt->run_info.read_weight == 0)) 
    {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "WriteWeight=0 and ReadWeight=0 is illegal configuration\n")));
        return -1; 
    }

    if (*dram_ndx_srt == '\0') 
    {
        /* set by default - dram index = 0 */
        SHR_BITCLR_RANGE(abt->dram_ndx, 0, SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX));
        SHR_BITSET(abt->dram_ndx, 0);
        
    } else if (shr_bitop_str_decode((const char *)dram_ndx_srt, abt->dram_ndx, 1) < 0)  
    {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "DramNdx parameter was not enter correctly\n")));
        return -1;
    }

    /* make sure that has a valid interface to work on */
    dram_ndx_max = SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX);
    SHR_BIT_ITER(abt->dram_ndx, dram_ndx_max, dram_index)
    {
        ++found_dram_interfaces;
    }

    if (found_dram_interfaces == 0) {
        LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "DramNdx recieved is not valid\n")));
        return -1;
    }
    
    *p = abt;
    rv = 0;

    done:
       parse_arg_eq_done(&pt);
       return rv;
}

int
arad_bist_test_done(int unit, void *p)
{
    int rv=0;
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Arad Bist Mem Tests Done\n")));

    if (((arad_bist_test_params_t*)p)->no_reset == 0) 
    {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Do soft reset\n")));        
        rv = soc_device_reset(unit, SOC_DPP_RESET_MODE_BLOCKS_SOFT_RESET, SOC_DPP_RESET_ACTION_INOUT_RESET);
        if(rv!=0) 
        {
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Error during soc_device_reset()\n")));
        }
    }

    sal_free(arad_bist_test_params[unit]);
    arad_bist_test_params[unit] = NULL;
    return rv;
}

int arad_bist_test(int unit, args_t *a, void *p)
{
    uint32
        dram_ndx,
        dram_ndx_max,
        bist_err_occur,
        bist_full_err_cnt,
        bist_single_err_cnt,
        bist_global_err_cnt,
        soc_sand_rc,
        is_gddr5=0,
        is_crc=0,
        arad_bist_fail = 0x0;
    int rv;
    combo28_bist_err_cnt_t combo28_bist_err_cnt;

    arad_bist_test_params_t *abt = p;
    CONST soc_dpp_drc_combo28_info_t *drc_info = NULL;

    drc_info = &(SOC_DPP_CONFIG(unit)->arad->init.drc_info);

    dram_ndx_max = SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max);
    SHR_BIT_ITER(abt->dram_ndx, dram_ndx_max, dram_ndx)
    {
        if (SOC_IS_DPP_DRC_COMBO28(unit)) 
        {
            /* sanity check for combo 28, check that Dram is valid */
            /* coverity [overrun-local:FALSE] */
            if ((SOC_IS_JERICHO(unit) || SOC_IS_ARDON(unit)) && (!SHR_BITGET(SOC_DPP_CONFIG(unit)->arad->init.drc_info.dram_bitmap, dram_ndx)))
            {
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "arad_bist_test: Disabled Interface=%d, skipping BIST\n"), dram_ndx));
                continue; 
            }
        }
        else
        {
            /* sanity check for Arad, check that Dram is valid */
            if (SOC_IS_ARAD(unit) && (!SOC_DPP_CONFIG(unit)->arad->init.dram.is_valid[dram_ndx])) 
            {
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "arad_bist_test: Disabled Interface=%d, skipping BIST\n"), dram_ndx));
                continue; 
            } 
        }

        if (abt->run_info.arad_bist_flags & SOC_TMC_DRAM_BIST_FLAGS_ALL_ADDRESS) {
            /* 
             * Setup all 
             */ 
    
            /* Remove all other flags */
            abt->run_info.arad_bist_flags = SOC_TMC_DRAM_BIST_FLAGS_ALL_ADDRESS | SOC_TMC_DRAM_BIST_FLAGS_CONS_ADDR_8_BANKS;
    
            abt->run_info.bist_start_address = 0x0;
#ifdef BCM_DDR3_SUPPORT
            if (SOC_IS_DPP_DRC_COMBO28(unit) == FALSE) {
                abt->run_info.bist_end_address =  SOC_DDR3_NUM_COLUMNS(unit) * SOC_DDR3_NUM_ROWS(unit) * SOC_DDR3_NUM_BANKS(unit) / 8 - 1;
            } else 
#endif
            {
                /*
                 *  bits bank * byte column * bits row 
                 */                
                abt->run_info.bist_end_address = (((drc_info->nof_columns) / 8) * (drc_info->nof_banks) * (drc_info->nof_rows)) - 1;
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "dram_ndx=%d, bist_end_address=0x%x\n"), dram_ndx, abt->run_info.bist_end_address));
            }
            abt->run_info.bist_num_actions =  abt->run_info.bist_end_address + 1;

            abt->run_info.write_weight = 1;
            abt->run_info.read_weight = 0;
    
            abt->run_info.pattern_mode = SOC_TMC_DRAM_BIST_DATA_PATTERN_ADDR_MODE;
    
            if(SOC_IS_DPP_DRC_COMBO28(unit) == FALSE){
                soc_sand_rc = arad_dram_bist_test_start(abt->unit, dram_ndx, &(abt->run_info));
                if (SOC_SAND_FAILURE(soc_sand_rc)) {
                    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Error: First arad_dram_bist_test_start () Failed:\n")));
                    arad_bist_fail = 0x1;
                    goto done;
                }
            } else {
            
                rv = soc_dpp_drc_combo28_bist_test_start(abt->unit, dram_ndx, &(abt->run_info), NULL);
                if (rv < 0) {
                    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Error: soc_dpp_drc_combo28_bist_test_start () Failed:\n")));
                    arad_bist_fail = 0x1;
                    goto done;
                }            
            }
    
            abt->run_info.write_weight = 0;
            abt->run_info.read_weight = 1;
            
            if (SOC_IS_DPP_DRC_COMBO28(unit) == FALSE) {
                soc_sand_rc = arad_dram_bist_test_start(abt->unit, dram_ndx, &(abt->run_info));
                if (SOC_SAND_FAILURE(soc_sand_rc)) {
                    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Error: Secound arad_dram_bist_test_start () Failed:\n")));
                    arad_bist_fail = 0x1;
                    goto done;
                }
            } else {
                rv = soc_dpp_drc_combo28_bist_test_start(abt->unit, dram_ndx, &(abt->run_info), NULL);
                if (rv < 0) {
                    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Error: soc_dpp_drc_combo28_bist_test_start () Failed:\n")));
                    arad_bist_fail = 0x1;
                    goto done;
                }
            
            }
    
        } else { /* Not All address mode */
            if (SOC_IS_DPP_DRC_COMBO28(unit) == FALSE) {
                soc_sand_rc = arad_dram_bist_test_start(abt->unit, dram_ndx, &(abt->run_info));
          
                if (SOC_SAND_FAILURE(soc_sand_rc)) {
                    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Error: arad_dram_bist_test_start () Failed:\n")));
                    arad_bist_fail = 0x1;
                    goto done;
                }
            } else {
                rv = soc_dpp_drc_combo28_bist_test_start(abt->unit, dram_ndx, &(abt->run_info), NULL);
                if (rv < 0) {
                    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Error: soc_dpp_drc_combo28_bist_test_start () Failed:\n")));
                    arad_bist_fail = 0x1;
                    goto done;
                }
            }
          }


        /* Get the results */
        if (SOC_IS_DPP_DRC_COMBO28(unit) == FALSE)
        {
            /* coverity [overrun-call:FALSE] */
            soc_sand_rc = arad_dram_read_bist_err_cnt(abt->unit, dram_ndx, &bist_err_occur, &bist_full_err_cnt, &bist_single_err_cnt, &bist_global_err_cnt);
            if (SOC_SAND_FAILURE(soc_sand_rc)) 
            {
               LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Error: arad_dram_read_bist_err_cnt () Failed:\n")));
               arad_bist_fail = 0x1;
               goto done;
            }

            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\nDRAM index: %u \nResults:\n"), dram_ndx));
            /* Mapping of bits with errors : 0x%x (bit N stands for an error in bits N, N+32, ... N+224) */
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Mapping of bits with errors : 0x%x\n"), bist_err_occur));
            /* Number of errors occoured in bits that are not masked by bist full mask: %d */
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Number of errors occoured (bist full mask): %d\n"), bist_full_err_cnt)); 
            /* Number of errors occoured in bits that are not masked by bist single bit mask: %d */
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Number of errors occoured (bist single mask): %d\n"), bist_single_err_cnt)); 
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Number of global errors: %d\n"), bist_global_err_cnt));

            if ((bist_err_occur != 0x0)  || (bist_full_err_cnt != 0x0)  || (bist_single_err_cnt != 0x0)  || (bist_global_err_cnt != 0x0))
            {
                arad_bist_fail = 0x1;
            }

        } else
        {
            rv = soc_dpp_drc_combo28_shmoo_drc_bist_err_cnt(abt->unit, dram_ndx, &combo28_bist_err_cnt);
            if (rv < 0) 
            {
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Error: arad_dram_read_bist_err_cnt () Failed:\n")));
                arad_bist_fail = 0x1;
                goto done;
            }

            is_gddr5 = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5);
            is_crc   = ((drc_info->write_crc == 1) && (drc_info->read_crc == 1));

            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\nDRAM index: %u \nResults:\n"), dram_ndx));
            /* Mapping of bits with errors : 0x%x (bit N stands for an error in bits N, N+32, ... N+224) */
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Mapping of bits with errors : 0x%x \n"), combo28_bist_err_cnt.bist_err_occur));
            /* Number of errors occoured in bits that are not masked by bist full mask: %d */
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Number of errors occoured (bist full mask): %d\n"), combo28_bist_err_cnt.bist_full_err_cnt)); 
            /* Number of errors occoured in bits that are not masked by bist single bit mask: %d */
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Number of errors occoured (bist single mask): %d\n"), combo28_bist_err_cnt.bist_single_err_cnt)); 
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Number of global errors: %d\n"), combo28_bist_err_cnt.bist_global_err_cnt));
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Number of DBI errors: %d\n"), combo28_bist_err_cnt.bist_dbi_global_err_cnt));
            /* Mapping of DBI bits with errors : 0x%x (bit N stands for an error in bits N, N+8, ... N+24) */
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Mapping of DBI bits with errors : 0x%x \n"), combo28_bist_err_cnt.bist_dbi_err_occur));
            /* EDC is relevant only for GDDR5 */
            if ((is_gddr5 == 1) && (is_crc == 1))
            {
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Number of EDC errors: %d\n"), combo28_bist_err_cnt.bist_edc_global_err_cnt)); 
                /* Mapping of EDC bits with errors : 0x%x (bit N stands for an error in bits N, N+8, ... N+24) */
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Mapping of EDC bits with errors : 0x%x \n"), combo28_bist_err_cnt.bist_edc_err_occur));
            }

            if ((combo28_bist_err_cnt.bist_err_occur != 0x0)          || (combo28_bist_err_cnt.bist_full_err_cnt != 0x0)   || 
                (combo28_bist_err_cnt.bist_single_err_cnt != 0x0)     || (combo28_bist_err_cnt.bist_global_err_cnt != 0x0) ||
                (combo28_bist_err_cnt.bist_dbi_global_err_cnt != 0x0) || (combo28_bist_err_cnt.bist_dbi_err_occur != 0x0)  || 
                ((is_gddr5 == 1) && (is_crc == 1) && ((combo28_bist_err_cnt.bist_edc_global_err_cnt != 0x0) || (combo28_bist_err_cnt.bist_edc_err_occur != 0x0))))
            {
                arad_bist_fail = 0x1;
            }
        }
    }

done:
    if (arad_bist_fail == 0x1) {
        test_error(unit, "Arad BIST test failed!!!\n");
        return -1;
    }

    return 0; 
}

char arad_field_test_usage[] = 
"Field Tests Usage:\n"
"  Mode=<string>           The mode can be FAST, MEDIUM, SLOW or SCAN.\n"
"                          FAST will run the tests 10 times,\n"
"                          MEDIUM will run the test 100 times,\n"
"                          SLOW will run the test 10,000 times,\n"
"                          and SCAN will go over all the legal options of the tests.\n"
"                          The default is FAST.\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nFull documentation cannot be displayed with -pendantic compiler\n";
#else
"  X=<value>               Run the tests one time for specific key that is not zero.\n"
"  Deterministic=<0/1>     If set the test will be deterministic. Otherwise, the x-es will be choosed randomaly.\n"
"                          The default is TRUE.\n"
"  Srand=<srand value>     In desterministic test set the value to the function srand.\n"
"                          The default is 5.\n"
#ifdef BCM_WARM_BOOT_SUPPORT
"  Warmboot=<0/1>          If set the test will do warmboot after installing field groups and entries.\n"
"                          The default is FALSE.\n"
#endif
"  Ingress=<0/1>           If set the test will run the tests of the ingress stage.\n"
"                          The default is TRUE.\n"
"  Egress=<0/1>            If set the test will run the tests of the egress stage.\n"
"                          The default is FALSE.\n"
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
"  External=<0/1>          If set the test will run also the tests of the external stage.\n"
"                          The default is FALSE.\n"
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
;
#endif   /*COMPILER_STRING_CONST_LIMIT*/

typedef struct arad_field_test_params_s
{
    uint32 unit;
    uint32 mode_id;
    uint32 x;
    uint32 is_deterministic;
    uint32 do_warmboot;
    uint32 srand_value;
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    uint32 external;
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
    uint32 ingress;
    uint32 egress;
}arad_field_test_params_t;

static arad_field_test_params_t      *arad_field_test_params[SOC_MAX_NUM_DEVICES];

int
arad_field_test_init(int unit, args_t *a, void **p)
{
    arad_field_test_params_t *abt;
    parse_table_t pt;
    char *mode = NULL;
    int rv = -1;

    abt = arad_field_test_params[unit];
    if (abt == NULL) {
        abt = sal_alloc(sizeof(arad_field_test_params_t), "arad field test");
        if (abt == NULL) {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "%s: cannot allocate memory test data\n"), ARG_CMD(a)));
            return -1;
        }
        sal_memset(abt, 0, sizeof(arad_field_test_params_t));
        arad_field_test_params[unit] = abt;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Deterministic", PQ_INT, (void *) (1),
          &abt->is_deterministic, NULL);
    parse_table_add(&pt, "Mode", PQ_STRING, (void *) "FAST",
          &mode, NULL);
    parse_table_add(&pt, "x", PQ_INT, (void *) (0),
          &abt->x, NULL);
    parse_table_add(&pt, "Srand", PQ_INT, (void *) (5),
          &abt->srand_value, NULL);
#ifdef BCM_WARM_BOOT_SUPPORT
    parse_table_add(&pt, "Warmboot", PQ_INT, (void *) (0),
          &abt->do_warmboot, NULL);
#endif
    parse_table_add(&pt, "Ingress", PQ_INT, (void *) (1),
          &abt->ingress, NULL);
    parse_table_add(&pt, "Egress", PQ_INT, (void *) (1),
          &abt->egress, NULL);
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    parse_table_add(&pt, "External", PQ_INT, (void *) (0),
          &abt->external, NULL);
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
    if (parse_arg_eq(a, &pt) < 0) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "%s: Invalid option: %s\n"),
                             ARG_CMD(a), ARG_CUR(a)));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "%s\n"),arad_field_test_usage));
        goto done;
    }

    if (ARG_CNT(a) != 0) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "%s: extra options starting with \"%s\"\n"),
                             ARG_CMD(a), ARG_CUR(a)));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "%s\n"),arad_field_test_usage));
        goto done;
    }

    abt->unit = unit;
    if(! sal_strncasecmp(mode, "SUPER_FAST", strlen(mode)) ) {
        abt->mode_id = 0;
    } else if(! sal_strncasecmp(mode, "FAST", strlen(mode)) ) {
        abt->mode_id = 1;
    } else if(! sal_strncasecmp(mode, "MEDIUM", strlen(mode)) ) {
        abt->mode_id = 2;
    } else if(! sal_strncasecmp(mode, "SLOW", strlen(mode)) ) {
        abt->mode_id = 3;
    } else if(! sal_strncasecmp(mode, "SCAN", strlen(mode)) ) {
        abt->mode_id = 4;
    } else {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "%s: Invalid option: %s\n"),
                             ARG_CMD(a), ARG_CUR(a)));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "%s\n"),arad_field_test_usage));
        goto done;
    }
#ifndef BCM_WARM_BOOT_SUPPORT
    abt->do_warmboot = FALSE;
#endif
    *p = abt;
    rv = 0;

    done:
       parse_arg_eq_done(&pt);
       return rv;
}

int
arad_field_test_done(int unit, void *p)
{
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "Arad Field Tests Done\n")));
    sal_free(arad_field_test_params[unit]);
    arad_field_test_params[unit] = NULL;
    return 0;
}

extern cmd_result_t _bcm_petra_field_test_qualify_set(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_action_set(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_field_group(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
extern cmd_result_t _bcm_petra_field_test_field_group_2(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
extern cmd_result_t _bcm_petra_field_test_field_group_destroy(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_destroy_with_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_entry(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_entry_traffic_perf(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_entry_priority(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
extern cmd_result_t _bcm_petra_field_test_entry_priority_2(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
extern cmd_result_t _bcm_petra_field_test_shared_bank(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_priority(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_cascaded(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_presel(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_presel_set(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_presel(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_presel_1(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_data_qualifiers(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_data_qualifier_set(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_data_qualifiers_entry(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_data_qualifiers_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_predefined_data_qualifiers_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_direct_extraction(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_de_entry(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_de_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_de_entry_traffic_large(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_direct_table(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_direct_table_entry(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_direct_table_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_full_tcam(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_full_tcam_perf(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_full_tcam_diff_prio(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_compress(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_itmh_field_group(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_itmh_field_group_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_itmh_parsing_test(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_itmh_parsing_test_pb(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_user_header(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);

int arad_field_test(int unit, args_t *a, void *p)
{
    cmd_result_t(*all_tests_func_ingress[])(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot) = {
        _bcm_petra_field_test_qualify_set,
        _bcm_petra_field_test_action_set,
        _bcm_petra_field_test_field_group,
        _bcm_petra_field_test_field_group_destroy,
        _bcm_petra_field_test_field_group_destroy_with_traffic,
        _bcm_petra_field_test_entry,
        _bcm_petra_field_test_entry_traffic,
        _bcm_petra_field_test_entry_traffic_perf,
        _bcm_petra_field_test_entry_priority,
        _bcm_petra_field_test_shared_bank,
        _bcm_petra_field_test_field_group_priority,
        _bcm_petra_field_test_cascaded,
        _bcm_petra_field_test_presel,
        _bcm_petra_field_test_presel_set,
        _bcm_petra_field_test_field_group_presel,
        _bcm_petra_field_test_field_group_presel_1,
        _bcm_petra_field_test_data_qualifiers,
        _bcm_petra_field_test_data_qualifier_set,
        _bcm_petra_field_test_data_qualifiers_entry,
        _bcm_petra_field_test_data_qualifiers_entry_traffic,
        _bcm_petra_field_test_predefined_data_qualifiers_entry_traffic,
        _bcm_petra_field_test_field_group_direct_extraction,
        _bcm_petra_field_test_de_entry,
        _bcm_petra_field_test_de_entry_traffic,
        _bcm_petra_field_test_de_entry_traffic_large,
        _bcm_petra_field_test_field_group_direct_table,
        _bcm_petra_field_test_direct_table_entry,
        _bcm_petra_field_test_direct_table_entry_traffic,
        _bcm_petra_field_test_full_tcam,
        _bcm_petra_field_test_full_tcam_perf,
        _bcm_petra_field_test_full_tcam_diff_prio,
        _bcm_petra_field_test_compress,
        _bcm_petra_field_test_itmh_field_group,
        _bcm_petra_field_test_itmh_field_group_traffic,
        _bcm_petra_field_test_itmh_parsing_test,
        _bcm_petra_field_test_itmh_parsing_test_pb,
        _bcm_petra_field_test_user_header
    };

    cmd_result_t(*all_tests_func_egress[])(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot) = {
        _bcm_petra_field_test_qualify_set,
        _bcm_petra_field_test_action_set,
        _bcm_petra_field_test_field_group,
        _bcm_petra_field_test_field_group_destroy,
        _bcm_petra_field_test_entry,
        _bcm_petra_field_test_entry_traffic,
        _bcm_petra_field_test_entry_traffic_perf,
        _bcm_petra_field_test_presel,
        _bcm_petra_field_test_presel_set,
        _bcm_petra_field_test_data_qualifiers,
        _bcm_petra_field_test_data_qualifier_set,
        _bcm_petra_field_test_data_qualifiers_entry,
        _bcm_petra_field_test_predefined_data_qualifiers_entry_traffic
    };

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    cmd_result_t(*all_tests_func_external[])(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot) = {
        _bcm_petra_field_test_qualify_set,
        _bcm_petra_field_test_action_set,
        _bcm_petra_field_test_field_group,
        /*_bcm_petra_field_test_field_group_2,*/
        _bcm_petra_field_test_entry,
        _bcm_petra_field_test_entry_traffic,
        _bcm_petra_field_test_entry_traffic_perf,
        _bcm_petra_field_test_entry_priority,
        _bcm_petra_field_test_entry_priority_2,
        _bcm_petra_field_test_data_qualifiers,
        _bcm_petra_field_test_data_qualifier_set,
        _bcm_petra_field_test_data_qualifiers_entry,
        _bcm_petra_field_test_data_qualifiers_entry_traffic,
        _bcm_petra_field_test_predefined_data_qualifiers_entry_traffic,
        _bcm_petra_field_test_full_tcam,
        _bcm_petra_field_test_full_tcam_diff_prio
    };
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

    cmd_result_t result;    
    arad_field_test_params_t *abt = p;
    uint32 test_idx;
    uint32 srand_val;

    sal_usecs_t prev_time  = sal_time_usecs();
    sal_usecs_t cur_time;
    sal_usecs_t diff_time;    

    if(abt->is_deterministic) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "The test is deterministic\n")));
        sal_srand(abt->srand_value);
    } else {
        srand_val = sal_time();
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "The test is not deterministic. srand_value = %d.\n"), srand_val));
        sal_srand(srand_val);
    }
	
	/* ingress */
    if(abt->ingress) {
        for(test_idx = 0; test_idx < sizeof(all_tests_func_ingress)/sizeof(all_tests_func_ingress[0]); ++test_idx) {
            result = all_tests_func_ingress[test_idx](abt->unit, 0, abt->x, abt->mode_id, abt->do_warmboot);
            cur_time = sal_time_usecs();
            diff_time = cur_time - prev_time;
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "The test run %u:%.2u.%.6u m:s\n"), (diff_time / 1000000) / 60, (diff_time / 1000000) % 60, diff_time % 1000000));
            prev_time = cur_time;
            if(result != CMD_OK) {
                return -1;
            }
        }
    }

    /* egress */
    if(abt->egress) {
        for(test_idx = 0; test_idx < sizeof(all_tests_func_egress)/sizeof(all_tests_func_egress[0]); ++test_idx) {
            result = all_tests_func_egress[test_idx](abt->unit, 1, abt->x, abt->mode_id, abt->do_warmboot);
            cur_time = sal_time_usecs();
            diff_time = cur_time - prev_time;
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "The test run %u:%.2u.%.6u m:s\n"), (diff_time / 1000000) / 60, (diff_time / 1000000) % 60, diff_time % 1000000));
            prev_time = cur_time;
            if(result != CMD_OK) {
                return -1;
            }
        }
    }
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    /* external */
    if(abt->external && abt->mode_id > 0) {
        for(test_idx = 0; test_idx < sizeof(all_tests_func_external)/sizeof(all_tests_func_external[0]); ++test_idx) {
            result = all_tests_func_external[test_idx](abt->unit, 2, abt->x, abt->mode_id, abt->do_warmboot);
            cur_time = sal_time_usecs();
            diff_time = cur_time - prev_time;
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "The test run %u:%.2u.%.6u m:s\n"), (diff_time / 1000000) / 60, (diff_time / 1000000) % 60, diff_time % 1000000));
            prev_time = cur_time;
            if(result != CMD_OK) {
                return -1;
            }
        }
    }
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
    return 0; 
}

#endif
