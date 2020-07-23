/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT


#include <shared/bsl.h>


#include <sal/appl/sal.h>


#include <soc/error.h>


#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/JER/jer_ocb_dram_buffers.h>

#include <soc/sand/sand_mem.h>



#define JER_OCB_DEFAULT_OCB_ONLY_MAX_TO_HIGH_TH_DIFFERENCE 800
#define JER_OCB_DEFAULT_OCB_ONLY_MAX_TO_LOW_TH_DIFFERENCE 900
#define JER_OCB_DEFAULT_OCB_MIXED_PERCENTAGE_OF_HIGH_TH_FROM_MAX 81/100
#define JER_OCB_DEFAULT_OCB_MIXED_PERCENTAGE_OF_LOW_TH_FROM_MAX 80/100
#define JER_MC_DEFAULT_MAX_TO_HIGH_TH_DIFFERENCE 2000
#define JER_MC_DEFAULT_PERCENTAGE_OF_LOW_TH_FROM_MAX 80/100

#define QAX_BUFFER_PTRS_PER_DATA_BUFFER 1536


#define OCB_FMC_BULK_SIZE (16*1024)

#define CALCULATE_NEEDED_BULKS(size, bulk_size)         ((size == 0) ? 1 : (size)/(bulk_size) + 1 * ((size)%(bulk_size) != 0))
#define MNMC_BANK_SIZE (512 * 1024)
#define MAX_NOF_FBC_RANGES 4




int soc_jer_str_prop_mc_nbr_full_dbuff_get(int unit, int *p_val)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_MULTICAST_NBR_FULL_DBUFF;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "JERICHO_INIT_FMC_64_REP_512K_DBUFF_MODE") == 0) {
            *p_val = JERICHO_INIT_FMC_64_REP_512K_DBUFF_MODE;
        } else if (sal_strcmp(propval, "JERICHO_INIT_FMC_4K_REP_256K_DBUFF_MODE") == 0) {
            *p_val = JERICHO_INIT_FMC_4K_REP_256K_DBUFF_MODE;
        } else if (sal_strcmp(propval, "JERICHO_INIT_FMC_NO_REP_DBUFF_MODE") == 0) {
            *p_val = JERICHO_INIT_FMC_NO_REP_DBUFF_MODE;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));  
        }
    } else {
        *p_val = JERICHO_INIT_FMC_4K_REP_256K_DBUFF_MODE;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_dram_buffer_conf_calc(int unit)
{

    int max_nof_dram_buffers;
    int needed_buffers_for_fbm = 0;
    int remaining_dram_buffers;
    int nof_drams;
    soc_jer_ocb_dram_dbuffs_bound_t  *dbuffs;

    SOCDNX_INIT_FUNC_DEFS;

    dbuffs = &SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries;
    nof_drams = SOC_DPP_CONFIG(unit)->arad->init.dram.nof_drams;

    max_nof_dram_buffers = SOC_DPP_CONFIG(unit)->jer->dbuffs.max_nof_dram_buffers;

    
    if ((nof_drams == 0) && (max_nof_dram_buffers > 0)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("misconfiguration, dram buffers available but system has no dram")));
    }

    
    if (max_nof_dram_buffers != 0) {
        needed_buffers_for_fbm = max_nof_dram_buffers / QAX_BUFFER_PTRS_PER_DATA_BUFFER + 1; 
    }
    remaining_dram_buffers = max_nof_dram_buffers - needed_buffers_for_fbm;

    
    if (nof_drams != 0) {
        dbuffs->mnmc_0.start = 0; 
        dbuffs->mnmc_0.end = SOC_SAND_RNG_LAST(dbuffs->mnmc_0.start, remaining_dram_buffers);; 
        dbuffs->mnmc_0.size = remaining_dram_buffers;
        dbuffs->fbc_mnmc_0.start = SOC_SAND_RNG_NEXT(dbuffs->mnmc_0.end);
        dbuffs->fbc_mnmc_0.end = SOC_SAND_RNG_LAST(dbuffs->fbc_mnmc_0.start, needed_buffers_for_fbm);
        dbuffs->fbc_mnmc_0.size = needed_buffers_for_fbm;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_dram_buffer_conf_set(int unit) 
{
    soc_reg_above_64_val_t reg_above_64_val;
    soc_jer_ocb_dram_dbuffs_bound_t  *dbuffs;

    SOCDNX_INIT_FUNC_DEFS;

    dbuffs = &SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries;

    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_get(unit, DDP_FBC_CONFIGURATIONr, REG_PORT_ANY, 0, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, DDP_FBC_CONFIGURATIONr, reg_above_64_val, DATA_FIRST_POINTERf, dbuffs->mnmc_0.start);
    soc_reg_above_64_field32_set(unit, DDP_FBC_CONFIGURATIONr, reg_above_64_val, DATA_LAST_POINTERf, dbuffs->mnmc_0.end);
    soc_reg_above_64_field32_set(unit, DDP_FBC_CONFIGURATIONr, reg_above_64_val, FREE_BUFF_START_POINTERf, dbuffs->fbc_mnmc_0.start);
    soc_reg_above_64_field32_set(unit, DDP_FBC_CONFIGURATIONr, reg_above_64_val, FREE_BUFF_LAST_POINTERf, dbuffs->fbc_mnmc_0.end);
    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_set(unit, DDP_FBC_CONFIGURATIONr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val); 
    soc_reg_above_64_field32_set(unit, DDP_FBC_CONTROLr, reg_above_64_val, FBC_INITf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DDP_FBC_CONTROLr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val); 
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DDP_FBC_CONTROLr, REG_PORT_ANY, 0, reg_above_64_val));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, DDP_DDP_GENERAL_CONFIGURATIONr,REG_PORT_ANY, 0, FC_LP_DEQUEUE_WORDS_THf,0xfa0));
      
      
    
exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jer_ocb_dram_buffer_conf_calc(int unit) 
{
    int                                 ocb_enabled;
    int                                 fmc_dbuff_mode;
    int                                 nof_active_cores;    
    
    uint32                              total_used_bulks = 0;
    uint32                              fmc_ocb_total_bulk_size = 0;
    uint32                              ocb_total_size = 0,       ocb_0_size = 0,       ocb_1_size = 0;
    uint32                              ocb_total_used_bulks = 0, ocb_0_used_bulks = 0, ocb_1_used_bulks = 0;
    uint32                              fmc_total_size = 0,       fmc_0_size = 0,       fmc_1_size = 0;
    uint32                              fmc_total_used_bulks = 0, fmc_0_used_bulks = 0, fmc_1_used_bulks = 0;
    uint32                              mnmc_total_size = 0,      mnmc_0_size = 0,      mnmc_1_size = 0;
    uint32                              fbc_total_size = 0,       fbc_fmc_0_size = 0,   fbc_fmc_1_size = 0,   fbc_mnmc_0_size = 0,    fbc_mnmc_1_size = 0;
    uint32                              nof_buffers_that_requires_fbc = 0;
    
    uint32                              fmc_ocb_remainder_to_512k_alignment = 0;
    uint32                              mnmc_without_remainder_size = 0;
    uint32                              mnmc_aligned_to_1024k_size = 0;

    uint32                              max_nof_dram_buffers = 0;
    uint32                              max_nof_buffers = 0;

    soc_jer_ocb_dram_dbuffs_bound_t     *dbuffs;
    ARAD_MGMT_INIT                      *arad_init;

    SOCDNX_INIT_FUNC_DEFS;

    dbuffs = &SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries;
    arad_init = &SOC_DPP_CONFIG(unit)->arad->init;
    nof_active_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    
    max_nof_dram_buffers = ((SOC_DPP_CONFIG(unit)->arad->init.dram.dram_size_total_mbyte * 1024) / SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size) * 1024;

    
    ocb_enabled = arad_init->ocb.ocb_enable;
    if(ocb_enabled == OCB_ENABLED)
    {
        
        ocb_total_size = (SOC_DPP_DEFS_GET(unit, ocb_memory_size) / 8) * (1024 * 1024 / arad_init->ocb.databuffer_size);
    } else {
        ocb_total_size = 0;
    }

    if(nof_active_cores > 1)
    {
        ocb_1_size = ocb_0_size = ocb_total_size / 2;
    } else {
        ocb_0_size = ocb_total_size;
        ocb_1_size = 0;
    }

    ocb_total_used_bulks += ocb_0_used_bulks = CALCULATE_NEEDED_BULKS(ocb_0_size, OCB_FMC_BULK_SIZE);
    total_used_bulks += ocb_total_used_bulks += ocb_1_used_bulks = CALCULATE_NEEDED_BULKS(ocb_1_size, OCB_FMC_BULK_SIZE);

    if (ocb_total_size > (64 * 1024)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Size of OCB should not exceed 64K buffers.\n"), ocb_total_size));
    }

    
    fmc_dbuff_mode = arad_init->dram.fmc_dbuff_mode;
    if (fmc_dbuff_mode == JERICHO_INIT_FMC_64_REP_512K_DBUFF_MODE) 
    {
        fmc_total_size = 512 * 1024;
    } else if (fmc_dbuff_mode == JERICHO_INIT_FMC_4K_REP_256K_DBUFF_MODE) {
        fmc_total_size = 256 * 1024;
    } else {  
        fmc_total_size = 0;
    }

    if(max_nof_dram_buffers == 0)
    {
        fmc_total_size = 0;
    }

    if(fmc_total_size != 0)
    {
        fmc_total_size -= OCB_FMC_BULK_SIZE * ocb_total_used_bulks;
    }

    if (nof_active_cores > 1)
    {
        fmc_1_size = fmc_0_size = fmc_total_size / 2 ;
    } else {
        fmc_0_size = fmc_total_size;
        fmc_1_size = 0;
    }

    fmc_total_used_bulks += fmc_0_used_bulks = CALCULATE_NEEDED_BULKS(fmc_0_size, OCB_FMC_BULK_SIZE);
    total_used_bulks += fmc_total_used_bulks += fmc_1_used_bulks = CALCULATE_NEEDED_BULKS(fmc_1_size, OCB_FMC_BULK_SIZE);

    
    if(max_nof_dram_buffers == 0) 
    {
        mnmc_total_size = mnmc_1_size = mnmc_0_size = 0;
    } else {
        SOCDNX_SAND_IF_ERR_EXIT( arad_init_pdm_nof_entries_calc(unit, arad_init->dram.pdm_mode, &max_nof_buffers));
        fmc_ocb_total_bulk_size = OCB_FMC_BULK_SIZE * total_used_bulks;
        
        if (max_nof_buffers < max_nof_dram_buffers)
        {
            
            mnmc_total_size = max_nof_buffers - fmc_ocb_total_bulk_size;
            
            nof_buffers_that_requires_fbc = max_nof_buffers - (OCB_FMC_BULK_SIZE * ocb_total_used_bulks);
            
            SOCDNX_SAND_IF_ERR_EXIT(arad_init_dram_fbc_buffs_get(nof_buffers_that_requires_fbc, arad_init->dram.dbuff_size, &fbc_total_size));
            
            fbc_total_size += MAX_NOF_FBC_RANGES; 
            
            if (max_nof_dram_buffers - max_nof_buffers < fbc_total_size)
            {
                
                mnmc_total_size = mnmc_total_size - (fbc_total_size - (max_nof_dram_buffers - max_nof_buffers));
            }
        } 
        else 
        {
            
            mnmc_total_size = max_nof_dram_buffers - fmc_ocb_total_bulk_size;
            
            nof_buffers_that_requires_fbc = max_nof_dram_buffers - (OCB_FMC_BULK_SIZE * ocb_total_used_bulks);
            
            SOCDNX_SAND_IF_ERR_EXIT(arad_init_dram_fbc_buffs_get(nof_buffers_that_requires_fbc, arad_init->dram.dbuff_size, &fbc_total_size));
            
            fbc_total_size += MAX_NOF_FBC_RANGES;
            
            mnmc_total_size -= fbc_total_size;
        }
                
        if(nof_active_cores > 1)
        {
            if (soc_property_get(unit, "ext_ram_dbuff_mmc_optimized_distribution_enable", 0))
            {
                
                
                
                
                
                
                uint32 mnmc_0_theoretical_fairest_size = mnmc_total_size / 2;
                uint32 diffrence_to_split = (fmc_ocb_total_bulk_size + mnmc_0_theoretical_fairest_size) % (512 * 1024);
                if (diffrence_to_split < (512 * 1024) - diffrence_to_split)
                {
                    mnmc_0_size = mnmc_0_theoretical_fairest_size - diffrence_to_split;
                }
                else
                {
                    mnmc_0_size = mnmc_0_theoretical_fairest_size + ((512 * 1024) - diffrence_to_split);
                }
                mnmc_1_size = mnmc_total_size - mnmc_0_size;
            }
            else
            {
                
                
                
                
                fmc_ocb_remainder_to_512k_alignment = (fmc_ocb_total_bulk_size % (512 * 1024)) ? (512 * 1024) - (fmc_ocb_total_bulk_size % (512 * 1024)) : 0 ;
                mnmc_without_remainder_size = mnmc_total_size - fmc_ocb_remainder_to_512k_alignment;
                mnmc_aligned_to_1024k_size = mnmc_without_remainder_size - (mnmc_without_remainder_size % (1024 * 1024));
                mnmc_0_size = mnmc_aligned_to_1024k_size / 2 + fmc_ocb_remainder_to_512k_alignment;
                mnmc_1_size = mnmc_total_size - mnmc_0_size;
            }

        } else {
            mnmc_0_size = mnmc_total_size;
            mnmc_1_size = 0;
        }
    }

    
    if (mnmc_0_size > mnmc_total_size || mnmc_1_size > mnmc_total_size)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Not enough memory for current configuration.")));
    }

       
    
    dbuffs->ocb_0.start = 0;
    dbuffs->ocb_0.end = (ocb_0_size == 0) ? dbuffs->ocb_0.start : SOC_SAND_RNG_LAST(dbuffs->ocb_0.start, ocb_0_size);
    dbuffs->ocb_0.size = ocb_0_size;

    
    dbuffs->fmc_0.start = dbuffs->ocb_0.start + (ocb_0_used_bulks * OCB_FMC_BULK_SIZE);
    dbuffs->fmc_0.end = (fmc_0_size == 0) ? dbuffs->fmc_0.start : SOC_SAND_RNG_LAST(dbuffs->fmc_0.start, fmc_0_size);
    dbuffs->fmc_0.size = fmc_0_size;

    
    dbuffs->ocb_1.start = dbuffs->fmc_0.start + (fmc_0_used_bulks * OCB_FMC_BULK_SIZE);
    dbuffs->ocb_1.end = (ocb_1_size == 0) ? dbuffs->ocb_1.start : SOC_SAND_RNG_LAST(dbuffs->ocb_1.start, ocb_1_size);
    dbuffs->ocb_1.size = ocb_1_size;

    
    dbuffs->fmc_1.start = dbuffs->ocb_1.start + (ocb_1_used_bulks * OCB_FMC_BULK_SIZE);
    dbuffs->fmc_1.end = (fmc_1_size == 0) ? dbuffs->fmc_1.start : SOC_SAND_RNG_LAST(dbuffs->fmc_1.start, fmc_1_size);
    dbuffs->fmc_1.size = fmc_1_size;

    
    dbuffs->mnmc_0.start = dbuffs->fmc_1.start + (fmc_1_used_bulks * OCB_FMC_BULK_SIZE);
    dbuffs->mnmc_0.end = SOC_SAND_RNG_LAST(dbuffs->mnmc_0.start, mnmc_0_size); 
    dbuffs->mnmc_0.size = mnmc_0_size;

    
    dbuffs->mnmc_1.start = SOC_SAND_RNG_NEXT(dbuffs->mnmc_0.end);
    dbuffs->mnmc_1.end = SOC_SAND_RNG_LAST(dbuffs->mnmc_1.start, mnmc_1_size); 
    dbuffs->mnmc_1.size = mnmc_1_size;

    
    
    if ((dbuffs->ocb_0.start % OCB_FMC_BULK_SIZE != 0) || (dbuffs->ocb_1.start % OCB_FMC_BULK_SIZE != 0) || (dbuffs->fmc_0.start % OCB_FMC_BULK_SIZE != 0) || (dbuffs->fmc_1.start % OCB_FMC_BULK_SIZE != 0))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Sizes of OCB-0(=%d), FMC-0(=%d), OCB-1(=%d) and FMC-1(=%d) should be on 16K alignments.\n"),
                                                                    ocb_0_size, fmc_0_size, ocb_1_size,    fmc_1_size ));
    }

    
    if (arad_init->dram.nof_drams != 0) 
    {
        
        if ( (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1) && (dbuffs->mnmc_1.start % (512 * 1024) != 0) ) 
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("MNMC-0 (=%d) and MNMC-1 (=%d) should be on 512K alignments.\n"), mnmc_0_size, mnmc_1_size));
        }
    }

    
    if(max_nof_dram_buffers == 0)
    {
        fbc_fmc_0_size = fbc_fmc_1_size = fbc_mnmc_0_size = fbc_mnmc_1_size = 0;
    } else {
        
        SOCDNX_SAND_IF_ERR_EXIT(arad_init_dram_fbc_buffs_get(SOC_SAND_RNG_COUNT(dbuffs->fmc_0.end, dbuffs->fmc_0.start), arad_init->dram.dbuff_size, &fbc_fmc_0_size));
        
        fbc_fmc_0_size = (fmc_0_size == 0) ? 0 : fbc_fmc_0_size ;

        SOCDNX_SAND_IF_ERR_EXIT(arad_init_dram_fbc_buffs_get(SOC_SAND_RNG_COUNT(dbuffs->fmc_1.end, dbuffs->fmc_1.start), arad_init->dram.dbuff_size, &fbc_fmc_1_size)); 
        
        fbc_fmc_1_size = (fmc_1_size == 0) ? 0 : fbc_fmc_1_size;

        SOCDNX_SAND_IF_ERR_EXIT(arad_init_dram_fbc_buffs_get(SOC_SAND_RNG_COUNT(dbuffs->mnmc_0.end, dbuffs->mnmc_0.start), arad_init->dram.dbuff_size, &fbc_mnmc_0_size));
        
        fbc_mnmc_0_size = (mnmc_0_size == 0) ? 0 : fbc_mnmc_0_size;
                
        SOCDNX_SAND_IF_ERR_EXIT(arad_init_dram_fbc_buffs_get(SOC_SAND_RNG_COUNT(dbuffs->mnmc_1.end, dbuffs->mnmc_1.start), arad_init->dram.dbuff_size, &fbc_mnmc_1_size));
        
        fbc_mnmc_1_size = (mnmc_1_size == 0) ? 0 : fbc_mnmc_1_size;
    }

    
    dbuffs->fbc_fmc_0.start = SOC_SAND_RNG_NEXT(dbuffs->mnmc_1.end);
    dbuffs->fbc_fmc_0.end = SOC_SAND_RNG_LAST(dbuffs->fbc_fmc_0.start, fbc_fmc_0_size);
    dbuffs->fbc_fmc_0.size = fbc_fmc_0_size;

    
    dbuffs->fbc_fmc_1.start = SOC_SAND_RNG_NEXT(dbuffs->fbc_fmc_0.end);
    dbuffs->fbc_fmc_1.end = SOC_SAND_RNG_LAST(dbuffs->fbc_fmc_1.start, fbc_fmc_1_size);
    dbuffs->fbc_fmc_1.size = fbc_fmc_1_size;

    
    dbuffs->fbc_mnmc_0.start = SOC_SAND_RNG_NEXT(dbuffs->fbc_fmc_1.end);
    dbuffs->fbc_mnmc_0.end = SOC_SAND_RNG_LAST(dbuffs->fbc_mnmc_0.start, fbc_mnmc_0_size);
    dbuffs->fbc_mnmc_0.size = fbc_mnmc_0_size;

    
    dbuffs->fbc_mnmc_1.start = SOC_SAND_RNG_NEXT(dbuffs->fbc_mnmc_0.end);
    dbuffs->fbc_mnmc_1.end = SOC_SAND_RNG_LAST(dbuffs->fbc_mnmc_1.start, fbc_mnmc_1_size);
    dbuffs->fbc_mnmc_1.size = fbc_mnmc_1_size;

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jer_ocb_dram_buffer_conf_set(int unit) 
{
    int                                 nof_banks_in_use = 0;
    int                                 current_bank_index = 0;
    uint32                              start_position = 0; 
    uint32                              current_bank_start_position = 0;
    uint32                              current_bank_end_position = 0;
    uint32                              fmc_bank_size = 0;
    uint32                              first_bank_for_core_1 = 0;
    uint32                              field_val;
    uint64                              reg64_val;
    soc_jer_ocb_dram_dbuffs_bound_t     *dbuffs_bdries;
    ARAD_MGMT_INIT                      *arad_init;
    ARAD_INIT_OCB                       *ocb;
    uint32                              max_nof_fmc_dbuffs = 0;
    uint32                              max_nof_mnmc_dbuffs = 0;
    uint32                              nof_cores = 0;
    uint32                              core = 0;
    soc_reg_above_64_val_t              reg_above_64;

    SOCDNX_INIT_FUNC_DEFS;

    arad_init = &SOC_DPP_CONFIG(unit)->arad->init;
    ocb = &arad_init->ocb;
    dbuffs_bdries = &SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries;
    nof_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    
    
    
    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_INTERNAL_REUSEf, 1));
    }

     
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_0r, REG_PORT_ANY, 0, OCB_0_BUFF_PTR_STARTf, dbuffs_bdries->ocb_0.start)); 
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_0r, REG_PORT_ANY, 0, OCB_0_BUFF_PTR_ENDf, dbuffs_bdries->ocb_0.end));

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_1r, REG_PORT_ANY, 0, OCB_1_BUFF_PTR_STARTf, dbuffs_bdries->ocb_1.start)); 
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_1r, REG_PORT_ANY, 0, OCB_1_BUFF_PTR_ENDf, dbuffs_bdries->ocb_1.end));

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_0r, REG_PORT_ANY, 0, FULL_MULTICAST_0_DB_PTR_STARTf, dbuffs_bdries->fmc_0.start));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_0r, REG_PORT_ANY, 0, FULL_MULTICAST_0_DB_PTR_ENDf, dbuffs_bdries->fmc_0.end));

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_1r, REG_PORT_ANY, 0, FULL_MULTICAST_1_DB_PTR_STARTf, dbuffs_bdries->fmc_1.start)); 
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_1r, REG_PORT_ANY, 0, FULL_MULTICAST_1_DB_PTR_ENDf, dbuffs_bdries->fmc_1.end));

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_0r, REG_PORT_ANY, 0, MINI_MULTICAST_0_DB_PTR_STARTf, dbuffs_bdries->mnmc_0.start)); 
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_0r, REG_PORT_ANY, 0, MINI_MULTICAST_0_DB_PTR_ENDf, dbuffs_bdries->mnmc_0.end));

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_1r, REG_PORT_ANY, 0, MINI_MULTICAST_1_DB_PTR_STARTf, dbuffs_bdries->mnmc_1.start)); 
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_1r, REG_PORT_ANY, 0, MINI_MULTICAST_1_DB_PTR_ENDf, dbuffs_bdries->mnmc_1.end));

    if (!SOC_IS_FLAIR(unit)) {
        if (dbuffs_bdries->fbc_fmc_0.size != 0) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_LOWf, dbuffs_bdries->fbc_fmc_0.start)); 
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_HIGHf, dbuffs_bdries->fbc_fmc_0.end));
        } else {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_LOWf, 0x3fffff)); 
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_HIGHf, 0x3fffff));
        }

        if (dbuffs_bdries->fbc_fmc_1.size != 0) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_LOWf, dbuffs_bdries->fbc_fmc_1.start)); 
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_HIGHf, dbuffs_bdries->fbc_fmc_1.end));
        } else {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_LOWf, 0x3fffff)); 
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_HIGHf, 0x3fffff));
        }

        if (dbuffs_bdries->fbc_mnmc_0.size != 0) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_LOWf, dbuffs_bdries->fbc_mnmc_0.start)); 
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_HIGHf, dbuffs_bdries->fbc_mnmc_0.end));
        } else {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_LOWf, 0x3fffff)); 
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_HIGHf, 0x3fffff));
        }

        if (dbuffs_bdries->fbc_mnmc_1.size != 0) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_LOWf, dbuffs_bdries->fbc_mnmc_1.start)); 
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_HIGHf, dbuffs_bdries->fbc_mnmc_1.end));
        } else {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_LOWf, 0x3fffff)); 
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_HIGHf, 0x3fffff));
        }

        
        COMPILER_64_ZERO(reg64_val);
        soc_reg64_field32_set(unit, IDR_DRAM_REJECT_MULTICAST_RANGEr, &reg64_val, DRAM_REJECT_MULTICAST_RANGE_N_LOWf, 1);
        soc_reg64_field32_set(unit, IDR_DRAM_REJECT_MULTICAST_RANGEr, &reg64_val, DRAM_REJECT_MULTICAST_RANGE_N_HIGHf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_IDR_DRAM_REJECT_MULTICAST_RANGEr(unit, 0, reg64_val));
        SOCDNX_IF_ERR_EXIT(WRITE_IDR_DRAM_REJECT_MULTICAST_RANGEr(unit, 1, reg64_val));
    }
    
    if (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1) {
        if( ocb->ocb_enable == OCB_ENABLED){
            if (ocb->databuffer_size == 256) {
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 0, FBC_BANK_IS_USED_BY_OCB_Nf, 0x00ff)); 
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 1, FBC_BANK_IS_USED_BY_OCB_Nf, 0xff00));

                for (current_bank_index = 0; current_bank_index < JER_OCB_BANK_NUM; current_bank_index++) {
                    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, &reg64_val));
                    start_position = (current_bank_index < (JER_OCB_BANK_NUM / 2)) ? dbuffs_bdries->ocb_0.start : dbuffs_bdries->ocb_1.start;
                    current_bank_start_position = start_position + ((current_bank_index % (JER_OCB_BANK_NUM / 2)) * JER_OCB_BANK_SIZE);
                    soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_START_BANK_Nf, current_bank_start_position);
                    current_bank_end_position = current_bank_start_position + JER_OCB_BANK_SIZE - 1;
                    soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_END_BANK_Nf, current_bank_end_position);
                    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, reg64_val));
                }
            } else {  
                
                
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 0, FBC_BANK_IS_USED_BY_OCB_Nf, 0x0055));
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 1, FBC_BANK_IS_USED_BY_OCB_Nf, 0x5500));

                for (nof_banks_in_use = 0, current_bank_index = 0; current_bank_index < JER_OCB_BANK_NUM; current_bank_index += 2, ++nof_banks_in_use) {
                    
                    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, &reg64_val));
                    start_position = (current_bank_index < (JER_OCB_BANK_NUM / 2)) ? dbuffs_bdries->ocb_0.start : dbuffs_bdries->ocb_1.start;
                    current_bank_start_position = start_position + ((nof_banks_in_use % (JER_OCB_BANK_NUM / 4)) * JER_OCB_BANK_SIZE);
                    soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_START_BANK_Nf, current_bank_start_position);
                    current_bank_end_position = current_bank_start_position + JER_OCB_BANK_SIZE - 1;
                    soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_END_BANK_Nf, current_bank_end_position);
                    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, reg64_val));
                }
            }
        } else {
            
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 0, FBC_BANK_IS_USED_BY_OCB_Nf, 0x0000)); 
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 1, FBC_BANK_IS_USED_BY_OCB_Nf, 0x0000));

            for (current_bank_index = 0; current_bank_index < JER_OCB_BANK_NUM; current_bank_index++) {
                SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, &reg64_val));
                soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_START_BANK_Nf, 0x3fffff);
                soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_END_BANK_Nf, 0x3fffff);
                SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, reg64_val));
            }
        }
        
        fmc_bank_size = (arad_init->dram.fmc_dbuff_mode == JERICHO_INIT_FMC_64_REP_512K_DBUFF_MODE) ? 32 * 1024 : 16 * 1024;
        first_bank_for_core_1 = dbuffs_bdries->ocb_1.start / fmc_bank_size;
        field_val = (0xffff >> first_bank_for_core_1) << first_bank_for_core_1;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQMT_MEM_BANK_SELECTr, REG_PORT_ANY, 0, FLUSCNT_BANK_SELECTf, field_val));

        first_bank_for_core_1 = dbuffs_bdries->mnmc_1.start / MNMC_BANK_SIZE;
        field_val = (0xff >> first_bank_for_core_1) << first_bank_for_core_1;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQMT_MEM_BANK_SELECTr, REG_PORT_ANY, 0, MNUSCNT_BANK_SELECTf, field_val));
    } else { 
        if( ocb->ocb_enable == OCB_ENABLED){
            
            if (!SOC_IS_FLAIR(unit)) {
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 0, FBC_BANK_IS_USED_BY_OCB_Nf, 0x5555));
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 1, FBC_BANK_IS_USED_BY_OCB_Nf, 0x0000));
            }

            for (nof_banks_in_use = 0, current_bank_index = 0; current_bank_index < JER_OCB_BANK_NUM; current_bank_index += 2, ++nof_banks_in_use) {
                
                SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, &reg64_val));
                field_val = dbuffs_bdries->ocb_0.start + (nof_banks_in_use * JER_OCB_BANK_SIZE);
                soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_START_BANK_Nf, field_val);
                field_val = dbuffs_bdries->ocb_0.start + ((nof_banks_in_use + 1) * JER_OCB_BANK_SIZE) - 1;
                soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_END_BANK_Nf, field_val);
                SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, reg64_val));
            }

            for (current_bank_index = 1; current_bank_index < JER_OCB_BANK_NUM; current_bank_index += 2) {
                
                SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, &reg64_val));
                soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_START_BANK_Nf, 0x3fffff);
                soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_END_BANK_Nf, 0x3fffff);
                SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, reg64_val));
            }
        } else {
            
            if (!SOC_IS_FLAIR(unit)) {
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 0, FBC_BANK_IS_USED_BY_OCB_Nf, 0x0000));
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 1, FBC_BANK_IS_USED_BY_OCB_Nf, 0x0000));
            }

            for (current_bank_index = 0; current_bank_index < JER_OCB_BANK_NUM; ++current_bank_index) {
                
                SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, &reg64_val));
                soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_START_BANK_Nf, 0x3fffff);
                soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_END_BANK_Nf, 0x3fffff);
                SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, reg64_val));
            }
        }
        if (!SOC_IS_FLAIR(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQMT_MEM_BANK_SELECTr, REG_PORT_ANY, 0, FLUSCNT_BANK_SELECTf, 0));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQMT_MEM_BANK_SELECTr, REG_PORT_ANY, 0, MNUSCNT_BANK_SELECTf, 0));
        }
    }

    if (!SOC_IS_FLAIR(unit)) {
        field_val = (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1) ? 0x38 : 0x0 ;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQMT_BANK_SELECTr, REG_PORT_ANY, 0, BDB_BANK_SELECTf, field_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQMT_BANK_SELECTr, REG_PORT_ANY, 0, PDM_BANK_SELECTf, field_val));
    }

    
    switch(arad_init->dram.dbuff_size){
    case SOC_TMC_ITM_DBUFF_SIZE_BYTES_256:
        field_val = 0;
        break;
    case SOC_TMC_ITM_DBUFF_SIZE_BYTES_512:
        field_val = 1;
        break;
    case SOC_TMC_ITM_DBUFF_SIZE_BYTES_1024:
        field_val = 2;
        break;
    case SOC_TMC_ITM_DBUFF_SIZE_BYTES_2048:
        field_val = 3;
        break;
    default:
        field_val = 2;
    }
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_GENERAL_CFG_3r, REG_PORT_ANY, 0, DRAM_BUFF_SIZEf, field_val));
    
    switch(arad_init->ocb.databuffer_size){
    case 256:
        field_val = 0;
        break;
    case 512:
        field_val = 1;
        break;
    default:
        field_val = 0;
    }
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_GENERAL_CFG_3r, REG_PORT_ANY, 0, OCB_BUFF_SIZEf, field_val));

    
    if (!SOC_IS_FLAIR(unit)) {
        for (core = 0; core < nof_cores; ++core) 
        {
            int conf_mnmc = 1;
            int conf_fmc = 1;

            max_nof_mnmc_dbuffs = (core == 0) ? dbuffs_bdries->mnmc_0.size : dbuffs_bdries->mnmc_1.size;
            if ((max_nof_mnmc_dbuffs < JER_MC_DEFAULT_MAX_TO_HIGH_TH_DIFFERENCE) ||
                (max_nof_mnmc_dbuffs * JER_MC_DEFAULT_PERCENTAGE_OF_LOW_TH_FROM_MAX > max_nof_mnmc_dbuffs - JER_MC_DEFAULT_MAX_TO_HIGH_TH_DIFFERENCE)) 
            {
                
                conf_mnmc = 0;
            }

            max_nof_fmc_dbuffs = (core == 0) ? dbuffs_bdries->fmc_0.size : dbuffs_bdries->fmc_1.size;
            if ((max_nof_fmc_dbuffs < JER_MC_DEFAULT_MAX_TO_HIGH_TH_DIFFERENCE) ||
                (max_nof_fmc_dbuffs * JER_MC_DEFAULT_PERCENTAGE_OF_LOW_TH_FROM_MAX > max_nof_fmc_dbuffs - JER_MC_DEFAULT_MAX_TO_HIGH_TH_DIFFERENCE)) 
            {
                
                conf_fmc = 0;
            }

            if (!conf_mnmc && !conf_fmc) {
                continue;
            }

            SOC_REG_ABOVE_64_CLEAR(reg_above_64);

            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, REG_PORT_ANY, core, reg_above_64)); 
            if (conf_mnmc) {
                soc_reg_above_64_field32_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, MINI_MULTICAST_N_DB_OCC_TH_Hf, max_nof_mnmc_dbuffs - JER_MC_DEFAULT_MAX_TO_HIGH_TH_DIFFERENCE); 
                soc_reg_above_64_field32_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, MINI_MULTICAST_N_DB_OCC_TH_Lf, max_nof_mnmc_dbuffs * JER_MC_DEFAULT_PERCENTAGE_OF_LOW_TH_FROM_MAX); 
            }
            if (conf_fmc) {
                soc_reg_above_64_field32_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, FULL_MULTICAST_N_DB_OCC_TH_Hf, max_nof_fmc_dbuffs - JER_MC_DEFAULT_MAX_TO_HIGH_TH_DIFFERENCE); 
                soc_reg_above_64_field32_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, FULL_MULTICAST_N_DB_OCC_TH_Lf, max_nof_fmc_dbuffs * JER_MC_DEFAULT_PERCENTAGE_OF_LOW_TH_FROM_MAX); 
            }
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, REG_PORT_ANY, core, reg_above_64)); 
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_ocb_dram_buffer_autogen_set(int unit) 
{
    
    uint32  field_val;
    ARAD_MGMT_INIT  *arad_init;
    soc_jer_ocb_dram_dbuffs_bound_t  *dbuffs_bdries;

    SOCDNX_INIT_FUNC_DEFS;

    arad_init = &SOC_DPP_CONFIG(unit)->arad->init;
    dbuffs_bdries = &SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries;

    
    if (arad_init->ocb.ocb_enable != OCB_DISABLED) {
        field_val = 0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_OCB_0_AUTOGEN_ENABLEf, field_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_OCB_1_AUTOGEN_ENABLEf, field_val));

        if (dbuffs_bdries->ocb_0.size != 0) {
            field_val = 0x1;
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_OCB_0_AUTOGEN_ENABLEf, field_val));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IDR_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, OCB_0_AUTOGEN_DONEf, 0x1));
        }

        if (dbuffs_bdries->ocb_1.size != 0) {
            field_val = 0x1;
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_OCB_1_AUTOGEN_ENABLEf, field_val));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IDR_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, OCB_1_AUTOGEN_DONEf, 0x1));
        }        
    }
    if (arad_init->drc_info.dram_num != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_MINI_MULTICAST_0_AUTOGEN_ENABLEf, 0x0));
        field_val = (dbuffs_bdries->mnmc_0.size != 0)? 0x1:0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_MINI_MULTICAST_0_AUTOGEN_ENABLEf, field_val));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_MINI_MULTICAST_1_AUTOGEN_ENABLEf, 0x0));
        field_val = (dbuffs_bdries->mnmc_1.size != 0)? 0x1:0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_MINI_MULTICAST_1_AUTOGEN_ENABLEf, field_val));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_FULL_MULTICAST_0_AUTOGEN_ENABLEf, 0x0));
        field_val = (dbuffs_bdries->fmc_0.size != 0)? 0x1:0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_FULL_MULTICAST_0_AUTOGEN_ENABLEf, field_val));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_FULL_MULTICAST_1_AUTOGEN_ENABLEf, 0x0));
        field_val = (dbuffs_bdries->fmc_1.size != 0)? 0x1:0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_FULL_MULTICAST_1_AUTOGEN_ENABLEf, field_val));
    } else {
        
        soc_reg_above_64_val_t r = {0};
        r[0] = r[1] = r[2] = r[3] = r[4] = r[5] = r[6] = r[7] = r[8] = r[9] = 0xffffffff;
        r[10]=0x3ff; 
        SOCDNX_IF_ERR_EXIT(WRITE_IDR_REASSEMBLY_CONTEXT_IS_OCB_ONLYr(unit, r));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_ocb_conf_set(int unit) 
{

    int i;
    uint64 reg64_val;
    uint32 table_entry[2] = {0};
    soc_reg_above_64_val_t reg_above_64;
    uint32 core = 0, nof_cores = 0, max_nof_ocb_dbuffs = 0;
    soc_jer_ocb_dram_dbuffs_bound_t  *dbuffs_bdries;
    ARAD_INIT_OCB *ocb;
    soc_dpp_drc_combo28_info_t *drc_info;

    SOCDNX_INIT_FUNC_DEFS;
    
    nof_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    dbuffs_bdries = &SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries;
    ocb = &SOC_DPP_CONFIG(unit)->arad->init.ocb;
    drc_info = &SOC_DPP_CONFIG(unit)->arad->init.drc_info;
        
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, OCB_GENERAL_CONFIGr, REG_PORT_ANY, 0, RD_ADDR_FIFO_ALMOST_FULL_THf, 0xa));

     
    soc_mem_field32_set(unit, IQM_OCBPRMm, table_entry, OCB_QUE_BUFF_SIZE_EN_TH_0f, 0x17f);
    soc_mem_field32_set(unit, IQM_OCBPRMm, table_entry, OCB_QUE_BUFF_SIZE_EN_TH_1f, 0x17f);
    soc_mem_field32_set(unit, IQM_OCBPRMm, table_entry, OCB_QUE_SIZE_EN_TH_0f, 0x17f);
    soc_mem_field32_set(unit, IQM_OCBPRMm, table_entry, OCB_QUE_SIZE_EN_TH_1f, 0x17f);
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IQM_OCBPRMm, MEM_BLOCK_ANY, table_entry));
    
    
    
    SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, IDR_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1));

    
    table_entry[1] = table_entry[0] = 0;
    soc_mem_field32_set(unit, IDR_QUEUE_IS_DRAM_ELIGIBLEm, table_entry, ELIGIBILITYf, 0xFFFFFFFF);
    SOCDNX_IF_ERR_EXIT(sand_fill_partial_table_with_entry(unit, IDR_QUEUE_IS_DRAM_ELIGIBLEm, 0, 1, MEM_BLOCK_ANY, 0, 3071, table_entry));

    table_entry[1] = table_entry[0] = 0;
    soc_mem_field32_set(unit, IDR_QUEUE_IS_OCB_ELIGIBLEm, table_entry, ELIGIBILITYf, 0xFFFFFFFF);
    SOCDNX_IF_ERR_EXIT(sand_fill_partial_table_with_entry(unit, IDR_QUEUE_IS_OCB_ELIGIBLEm, 0, 1, MEM_BLOCK_ANY, 0, 3071, table_entry));

    
    SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, IDR_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, ENABLE_DYNAMIC_MEMORY_ACCESSf, 0));

    
    for (i = 0; i < 2; i++) {
        
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, IDR_OCB_USE_SHAPERr, REG_PORT_ANY, i, &reg64_val));
        if (ocb->ocb_enable == OCB_ENABLED) {
            soc_reg64_field32_set(unit, IDR_OCB_USE_SHAPERr, &reg64_val, OCB_N_SHAPER_ENABLEf, 0x1);
            soc_reg64_field32_set(unit, IDR_OCB_USE_SHAPERr, &reg64_val, OCB_N_SHAPER_TIMER_CYCLESf, 0x0);
            soc_reg64_field32_set(unit, IDR_OCB_USE_SHAPERr, &reg64_val, OCB_N_SHAPER_GRANT_SIZEf, 0x100);
            soc_reg64_field32_set(unit, IDR_OCB_USE_SHAPERr, &reg64_val, OCB_N_SHAPER_MAX_BURSTf, 0xffff);
        } else {
            soc_reg64_field32_set(unit, IDR_OCB_USE_SHAPERr, &reg64_val, OCB_N_SHAPER_ENABLEf, 0x0);
        }
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, IDR_OCB_USE_SHAPERr, REG_PORT_ANY, i, reg64_val));
    }

    
    SOCDNX_IF_ERR_EXIT(READ_IDR_BUFFER_CHANGE_CONFIGURATIONr(unit, &reg64_val));
    soc_reg64_field32_set(unit, IDR_BUFFER_CHANGE_CONFIGURATIONr, &reg64_val, ENABLE_BUFFER_CHANGEf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_IDR_BUFFER_CHANGE_CONFIGURATIONr(unit, reg64_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, IDR_DRAM_FIFO_RECYCLE_TIMEOUT_CONFIGURATIONr, REG_PORT_ANY, RECYCLE_TIMEOUT_ENABLEf, 0));

    
    for (i = 0; i < 2; i++) {
        
        
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, REG_PORT_ANY, i, &reg64_val));
        soc_reg64_field32_set(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, &reg64_val, OCB_ELIGIBLE_MULTICAST_RANGE_N_LOWf, 0x0);
        soc_reg64_field32_set(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, &reg64_val, OCB_ELIGIBLE_MULTICAST_RANGE_N_HIGHf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, REG_PORT_ANY, i, reg64_val));

        
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, REG_PORT_ANY, i, &reg64_val));
        if (ocb->ocb_enable == OCB_ENABLED) {
            if (drc_info->dram_num != 0) {
                soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg64_val, OCB_COMMITTED_MULTICAST_RANGE_N_LOWf, 0x1); 
                soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg64_val, OCB_COMMITTED_MULTICAST_RANGE_N_HIGHf, 0x0);
            } else {
                soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg64_val, OCB_COMMITTED_MULTICAST_RANGE_N_LOWf, 0x0); 
                soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg64_val, OCB_COMMITTED_MULTICAST_RANGE_N_HIGHf, 0x17fff);
            }
        } else {
            
            soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg64_val, OCB_COMMITTED_MULTICAST_RANGE_N_LOWf, 0x1);
            soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg64_val, OCB_COMMITTED_MULTICAST_RANGE_N_HIGHf, 0x0);
        }
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, REG_PORT_ANY, i, reg64_val));
    }

    
    if( ocb->ocb_enable == OCB_ENABLED)
    {
        
        for (core = 0; core < nof_cores; ++core) 
        {
            max_nof_ocb_dbuffs = (core == 0) ? dbuffs_bdries->ocb_0.size : dbuffs_bdries->ocb_1.size;
            if (max_nof_ocb_dbuffs < JER_OCB_DEFAULT_OCB_ONLY_MAX_TO_LOW_TH_DIFFERENCE) 
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("available OCB size is to small to be configured\n")));
            }

            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, REG_PORT_ANY, core, reg_above_64)); 
            soc_reg_above_64_field32_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, OCB_N_ONLY_DB_OCC_TH_Hf, max_nof_ocb_dbuffs - JER_OCB_DEFAULT_OCB_ONLY_MAX_TO_HIGH_TH_DIFFERENCE); 
            soc_reg_above_64_field32_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, OCB_N_ONLY_DB_OCC_TH_Lf, max_nof_ocb_dbuffs - JER_OCB_DEFAULT_OCB_ONLY_MAX_TO_LOW_TH_DIFFERENCE); 
            soc_reg_above_64_field32_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, OCB_N_MIXED_DB_OCC_TH_Hf, max_nof_ocb_dbuffs * JER_OCB_DEFAULT_OCB_MIXED_PERCENTAGE_OF_HIGH_TH_FROM_MAX); 
            soc_reg_above_64_field32_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, OCB_N_MIXED_DB_OCC_TH_Lf, max_nof_ocb_dbuffs * JER_OCB_DEFAULT_OCB_MIXED_PERCENTAGE_OF_LOW_TH_FROM_MAX); 
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, REG_PORT_ANY, core, reg_above_64)); 
        }
    } else {
        SOC_REG_ABOVE_64_CLEAR(reg_above_64);
        WRITE_IDR_REASSEMBLY_CONTEXT_IS_OCB_ONLYr(unit, reg_above_64);
        WRITE_IDR_TRAFFIC_CLASS_IS_OCB_ONLYr(unit, 0, 0x0);
        WRITE_IDR_TRAFFIC_CLASS_IS_OCB_ONLYr(unit, 1, 0x0);
    }

exit:
    SOCDNX_FUNC_RETURN;
}




STATIC int jer_ocb_validate_value_fit_field_size(int unit, soc_reg_t reg, soc_field_t field, int value)
{
    return !(value >> soc_reg_field_length(unit, reg, field)) ;
}




int soc_jer_ocb_control_range_dram_mix_dbuff_threshold_set( SOC_SAND_IN int unit,   SOC_SAND_IN int core,   SOC_SAND_IN int range_max,   SOC_SAND_IN int range_min)
{
    soc_reg_above_64_val_t reg_above_64;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QAX(unit))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("function not supported in QAX")));
    }
    
    if( (0 == jer_ocb_validate_value_fit_field_size(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, OCB_N_MIXED_DB_OCC_TH_Hf, range_max)) ||
        (0 == jer_ocb_validate_value_fit_field_size(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, OCB_N_MIXED_DB_OCC_TH_Lf, range_min))    )
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("range_max = %d, range_min = %d, out of allowed range\n"), range_max, range_min));
    }

    
    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_get(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, REG_PORT_ANY, core, reg_above_64));
    soc_reg_above_64_field32_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, OCB_N_MIXED_DB_OCC_TH_Hf, range_max);
    soc_reg_above_64_field32_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, OCB_N_MIXED_DB_OCC_TH_Lf, range_min);
    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, REG_PORT_ANY, core, reg_above_64));

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_ocb_control_range_ocb_committed_multicast_set( SOC_SAND_IN int unit,   SOC_SAND_IN uint32 index,   SOC_SAND_IN int range_max,   SOC_SAND_IN int range_min)
{
    uint64 reg_64;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QAX(unit))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("function not supported in QAX")));
    }    
    
    
    if( (0 == jer_ocb_validate_value_fit_field_size(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, OCB_COMMITTED_MULTICAST_RANGE_N_HIGHf, range_max)) ||
        (0 == jer_ocb_validate_value_fit_field_size(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, OCB_COMMITTED_MULTICAST_RANGE_N_LOWf, range_min))    )
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("range_max = %d, range_min = %d, out of allowed range\n"), range_max, range_min));
    }

    
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, REG_PORT_ANY, index, &reg_64));
    soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg_64, OCB_COMMITTED_MULTICAST_RANGE_N_HIGHf, range_max);
    soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg_64, OCB_COMMITTED_MULTICAST_RANGE_N_LOWf, range_min);
    SOCDNX_IF_ERR_EXIT( soc_reg64_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, REG_PORT_ANY, index, reg_64));

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_ocb_control_range_ocb_eligible_multicast_set( SOC_SAND_IN int unit,   SOC_SAND_IN uint32 index,   SOC_SAND_IN int range_max,   SOC_SAND_IN int range_min)
{
    uint64 reg_64;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QAX(unit))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("function not supported in QAX")));
    }    
    
    
    if( (0 == jer_ocb_validate_value_fit_field_size(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, OCB_ELIGIBLE_MULTICAST_RANGE_N_HIGHf, range_max)) ||
        (0 == jer_ocb_validate_value_fit_field_size(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, OCB_ELIGIBLE_MULTICAST_RANGE_N_LOWf, range_min))    )
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("range_max = %d, range_min = %d, out of allowed range\n"), range_max, range_min));
    }

    
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, REG_PORT_ANY, index, &reg_64));
    soc_reg64_field32_set(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, &reg_64, OCB_ELIGIBLE_MULTICAST_RANGE_N_HIGHf, range_max);
    soc_reg64_field32_set(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, &reg_64, OCB_ELIGIBLE_MULTICAST_RANGE_N_LOWf, range_min);
    SOCDNX_IF_ERR_EXIT( soc_reg64_set(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, REG_PORT_ANY, index, reg_64));

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_ocb_control_range_dram_mix_dbuff_threshold_get( SOC_SAND_IN int unit,  SOC_SAND_IN int core,  SOC_SAND_OUT int* range_max,  SOC_SAND_OUT int* range_min)
{
    soc_reg_above_64_val_t reg_above_64;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QAX(unit))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("function not supported in QAX")));
    }    
    
    
    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_get(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, REG_PORT_ANY, core, reg_above_64));
    *range_max = soc_reg_above_64_field32_get(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, OCB_N_MIXED_DB_OCC_TH_Hf);
    *range_min = soc_reg_above_64_field32_get(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, OCB_N_MIXED_DB_OCC_TH_Lf);

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_ocb_control_range_ocb_committed_multicast_get( SOC_SAND_IN int unit,  SOC_SAND_IN uint32 index,  SOC_SAND_OUT int* range_max,  SOC_SAND_OUT int* range_min)
{
    uint64 reg_64;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QAX(unit))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("function not supported in QAX")));
    }
    
    
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, REG_PORT_ANY, index, &reg_64));
    *range_max = soc_reg64_field32_get(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, reg_64, OCB_COMMITTED_MULTICAST_RANGE_N_HIGHf);
    *range_min = soc_reg64_field32_get(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, reg_64, OCB_COMMITTED_MULTICAST_RANGE_N_LOWf);

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_ocb_control_range_ocb_eligible_multicast_get( SOC_SAND_IN int unit,  SOC_SAND_IN uint32 index,  SOC_SAND_OUT int* range_max,  SOC_SAND_OUT int* range_min)
{
    uint64 reg_64;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QAX(unit))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("function not supported in QAX")));
    }
        
    
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, REG_PORT_ANY, index, &reg_64));
    *range_max = soc_reg64_field32_get(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, reg_64, OCB_ELIGIBLE_MULTICAST_RANGE_N_HIGHf);
    *range_min = soc_reg64_field32_get(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, reg_64, OCB_ELIGIBLE_MULTICAST_RANGE_N_LOWf);

exit:
    SOCDNX_FUNC_RETURN;
}


