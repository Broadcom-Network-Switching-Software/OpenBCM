/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef __DEVICE_H
#define __DEVICE_H



#include <stdint.h>
#include <stdio.h>

#include "errors.h"
#include "init.h"
#include "hw_limits.h"

#ifdef __cplusplus
extern "C" {
#endif







struct kbp_device;



struct kbp_allocator;



struct kbp_db;



struct kbp_entry;


struct kbp_stats_db;



#define KBP_SDK_VERSION "KBP SDK 1.5.13"

#define KBP_ERROR_LIST_VERSION (2)






enum kbp_db_type {
    KBP_DB_INVALID, 
    KBP_DB_ACL,     
    KBP_DB_LPM,     
    KBP_DB_EM,      
    KBP_DB_AD,      
    KBP_DB_DMA,     
    KBP_DB_HB,      
    KBP_DB_COUNTER, 
    KBP_DB_TAP,     
    KBP_DB_STATS,   
    KBP_DB_END      
};



enum kbp_hw_resource {
    KBP_HW_RESOURCE_DBA,  
    KBP_HW_RESOURCE_UDA,  
    KBP_HW_RESOURCE_DBA_CORE,  
    KBP_HW_RESOURCE_UDA_CORE,  
    KBP_HW_RESOURCE_RANGE_UNITS, 
    KBP_HW_RESOURCE_INVALID      
};



enum kbp_range_encoding_type {
    KBP_RANGE_3B_ENCODING,  
    KBP_RANGE_2B_ENCODING,  
    KBP_RANGE_NO_ENCODING   
};


enum kbp_device_flush_flags {
    KBP_DEVICE_FLUSH_DEVICE_RESET = 1,      
    KBP_DEVICE_FLUSH_DEVICE_HARD_RESET = 2, 
    KBP_DEVICE_FLUSH_CONFIG_RESOURCE = 4,   
    KBP_DEVICE_FLUSH_ENTRIES = 8,           
    KBP_DEVICE_FLUSH_LTRS = 16,             
    KBP_DEVICE_FLUSH_PARALLEL = 32          
};


enum kbp_device_parity_inject_type {
    KBP_DBA_ERR_INJECT,     
    KBP_UDA_ERR_INJECT,     
    KBP_UIT_ERR_INJECT,     
    KBP_RPT_ERR_INJECT,     
    KBP_ERR_INJECT_END
};




#define EMU_CORE_CLEAR_BIT(value, reg, bit)                         \
    if (value) {                                                    \
        reg &= ~ (1ULL << bit);                                     \
    }

#define EMU_INTF_CLEAR_BIT(val1, val2, val3, val4, reg, bit)        \
{                                                                   \
    if (val1)                                                       \
        reg &= ~(1ULL << bit);                                      \
    if (val2)                                                       \
        reg &= ~(1ULL << (bit + 16));                               \
    if (val3)                                                       \
        reg &= ~(1ULL << (bit + 32));                               \
    if (val4)                                                       \
        reg &= ~(1ULL << (bit + 48));                               \
}

#define CORE_SET_BIT(value, reg, bit)                               \
    if (value) {                                                    \
        reg |= (1ULL << bit);                                       \
    }

#define CORE_SET_VALUE(value, reg, bit)                             \
    if ((reg >> bit) & 1ULL) {                                      \
        value = 1;                                                  \
    }


#define INTF_SET_BIT(val1, val2, val3, val4, reg, bit)              \
{                                                                   \
    if (val1)                                                       \
        reg |= (1ULL << bit);                                       \
    if (val2)                                                       \
        reg |= (1ULL << (bit + 16));                                \
    if (val3)                                                       \
        reg |= (1ULL << (bit + 32));                                \
    if (val4)                                                       \
        reg |= (1ULL << (bit + 48));                                \
}

#define INTF_CHECK_AND_SET_BIT(val1, val2, val3, val4, reg, bit)    \
{                                                                   \
    if (val1 || val2 || val3 || val4)                               \
        reg |= (1ULL << bit);                                       \
}

#define INTF_CHECK_AND_SET_BIT_2(val1, val2, val3, val4, reg, op2, bit)     \
{                                                                           \
    if (val1 || val2 || val3 || val4) {                                     \
        if (op2)                                                          \
            reg |= (1ULL << bit);                                           \
        else                                                                \
            reg |= (1ULL << (bit + 2));                                     \
    }                                                                       \
}





struct kbp_status_errors {

    
    uint8_t dba_soft_error; 
    uint8_t uda_soft_error; 
    uint8_t uit_soft_error; 
    uint8_t rpt_soft_error; 

    
    uint8_t intf0_host_transmit;        
    uint8_t intf0_host_receive;         
    uint8_t intf0_instr_len_receive;    
    uint8_t intf0_illegal_instr;        
    uint8_t intf0_eop_receive_1;        
    uint8_t intf0_burst_max;            
    uint8_t intf0_burst_short;          
    uint8_t intf0_protocol;             
    uint8_t intf0_start_of_packet;      
    uint8_t intf0_mac_fifo_parity;      
    uint8_t intf0_eop_receive_2;        
    uint8_t intf0_framing;              
    uint8_t intf0_crc24;                

    uint8_t intf1_host_transmit;        
    uint8_t intf1_host_receive;
    uint8_t intf1_instr_len_receive;
    uint8_t intf1_illegal_instr;
    uint8_t intf1_eop_receive_1;
    uint8_t intf1_burst_max;
    uint8_t intf1_burst_short;
    uint8_t intf1_protocol;
    uint8_t intf1_start_of_packet;
    uint8_t intf1_mac_fifo_parity;
    uint8_t intf1_eop_receive_2;
    uint8_t intf1_framing;
    uint8_t intf1_crc24;                

    uint8_t intf2_host_transmit;        
    uint8_t intf2_host_receive;
    uint8_t intf2_instr_len_receive;
    uint8_t intf2_illegal_instr;
    uint8_t intf2_eop_receive_1;
    uint8_t intf2_burst_max;
    uint8_t intf2_burst_short;
    uint8_t intf2_protocol;
    uint8_t intf2_start_of_packet;
    uint8_t intf2_mac_fifo_parity;
    uint8_t intf2_eop_receive_2;
    uint8_t intf2_framing;
    uint8_t intf2_crc24;                

    uint8_t intf3_host_transmit;        
    uint8_t intf3_host_receive;
    uint8_t intf3_instr_len_receive;
    uint8_t intf3_illegal_instr;
    uint8_t intf3_eop_receive_1;
    uint8_t intf3_burst_max;
    uint8_t intf3_burst_short;
    uint8_t intf3_protocol;
    uint8_t intf3_start_of_packet;
    uint8_t intf3_mac_fifo_parity;
    uint8_t intf3_eop_receive_2;
    uint8_t intf3_framing;
    uint8_t intf3_crc24;                

    
    uint8_t rcb2emc_resp_fifo_err;       
    uint8_t pci_req_fifo_err;            
    uint8_t cmp_with_no_rslt;            
    uint8_t stats_non_pkt_err;           
    uint8_t stats_rec_extract_pkt_err;   
    uint8_t stats_cmd_extract_pkt_err;   
    uint8_t err_in_req_inst_to_cpu;      
    uint8_t prosr_area_pio_resp_fifo_err;
    uint8_t prosr_mem_read_err;          
    uint8_t icb_input_fifo_err;          
    uint8_t power_cntl_mem_par_err;      
    uint8_t key_cntl_ltr_reg_err;        
    uint8_t cb_parity_err;               
    uint8_t db_intf_ltr_par_err;         
    uint8_t user_data_if_rslt_fifo_err;  
    uint8_t user_data_if_rslt_err;       
    uint8_t age_hb_or_vbit_mem_par_err_on_read; 
    uint8_t uda_ltr_par_err;             
    uint8_t proc_err_fatal;              
    uint8_t proc_err_non_fatal;          
    uint8_t arm_attention0;              
    uint8_t block_cmp_timeout;           
    uint8_t type2_otp_ecc_err;           


    
    uint8_t crb_error_interrupt;         
};



kbp_status kbp_device_init(struct kbp_allocator *alloc, enum kbp_device_type type, uint32_t flags,
                           void *xpt, struct kbp_device_config *config, struct kbp_device **device);



kbp_status kbp_device_destroy(struct kbp_device *device);



kbp_status kbp_device_lock(struct kbp_device *device);


enum kbp_restore_status {
    KBP_RESTORE_NO_CHANGE,            
    KBP_RESTORE_CHANGES_COMMITTED,    
    KBP_RESTORE_CHANGES_ABORTED       
};



enum parity_row_status {
    KBP_ROW_INVALID,
    KBP_ROW_UNTOUCHED,          
    KBP_ROW_RE_WRITE,           
    KBP_ROW_REPAIRED,           
    KBP_ROW_REPAIRED_RE_WRITE   
};



enum kbp_device_properties {
    KBP_DEVICE_PROP_DESCRIPTION,                
    KBP_DEVICE_PROP_TOTAL_NUM_BB,               
    KBP_DEVICE_PROP_RESOURCE_FN,                
    KBP_DEVICE_PROP_LTR_SM_UPDATE,              
    KBP_DEVICE_PROP_INTERRUPT,                  
    KBP_DEVICE_PROP_IFSR_THRESHOLD,             
    KBP_DEVICE_PROP_LAST_PARITY_ERRORS,         
    KBP_DEVICE_PROP_LAST_PARITY_ERRORS_STATUS,  
    KBP_DEVICE_PROP_INJECT_PARITY_ERRORS,       
    KBP_DEVICE_PROP_NON_REPAIRABLE_ABS,         
    KBP_DEVICE_PROP_READ_TYPE,                  
    KBP_DEVICE_PROP_DEBUG_LEVEL,                
    KBP_DEVICE_PROP_INST_LATENCY,               
    KBP_DEVICE_PRE_CLEAR_ABS,                   
    KBP_DEVICE_PROP_LAST_UDA_PARITY_ERRORS,     
    KBP_DEVICE_ADD_BROADCAST,                   
    KBP_DEVICE_ADD_SELECTIVE_BROADCAST,         
    KBP_DEVICE_BROADCAST_AT_XPT,                
    KBP_DEVICE_PROP_CRASH_RECOVERY,             
    KBP_DEVICE_PROP_DEFER_LTR_WRITES,           
    KBP_DEVICE_PROP_CLR_DEV_ERR_BITS,           
    KBP_DEVICE_PROP_DUMP_ON_ASSERT,             
    KBP_DEVICE_PROP_CLOCK_RATE,                 
    KBP_DEVICE_PROP_ADV_UDA_WRITE,              
    KBP_DEVICE_PROP_RETURN_ERROR_ON_ASSERTS,    
    KBP_DEVICE_PROP_COUNTER_COMPRESSION,        
    KBP_DEVICE_PROP_STAT_RECORD_SIZE,           
    KBP_DEVICE_PROP_TAP_NULL_REC_SIZE,          
    KBP_DEVICE_PROP_TAP_INGRESS_REC_SIZE,       
    KBP_DEVICE_PROP_TAP_EGRESS_REC_SIZE,        
    KBP_DEVICE_PROP_TAP_INGRESS_OPCODE_EXT_START_LOC, 
    KBP_DEVICE_PROP_TAP_EGRESS_OPCODE_EXT_START_LOC, 
    KBP_DEVICE_PROP_TAP_INGRESS_OPCODE_EXT_LEN, 
    KBP_DEVICE_PROP_TAP_EGRESS_OPCODE_EXT_LEN,  
    KBP_DEVICE_PROP_TAP_RESERVE_UDM,            
    KBP_DEVICE_PROP_MIN_RESOURCE,               
    KBP_DEVICE_PROP_FORCE_INVALIDATE_LOC,       
    KBP_DEVICE_PROP_HANDLE_INTERFACE_ERRORS,    
    KBP_DEVICE_PROP_RESOURCE_PROFILE,                    
    KBP_DEVICE_PROP_INVALID                     
};



kbp_status kbp_device_set_property(struct kbp_device *device, enum kbp_device_properties property, ...);



kbp_status kbp_device_get_property(struct kbp_device *device, enum kbp_device_properties property, ...);



kbp_status kbp_device_thread_init(struct kbp_device *device, uint32_t tid, struct kbp_device **thr_device);



kbp_status kbp_device_print_html(struct kbp_device *device, FILE *fp);



kbp_status kbp_device_print_sw_state(struct kbp_device * device, FILE * fp);




kbp_status kbp_device_print(struct kbp_device *device, FILE *fp);



const char * kbp_device_get_sdk_version(void);



kbp_status kbp_device_fix_errors(struct kbp_device *device);


kbp_status kbp_device_inject_errors(struct kbp_device *device, struct kbp_status_errors *err, uint32_t emulate);


kbp_status kbp_device_get_errors(struct kbp_device *device, struct kbp_status_errors *o_errors);



kbp_status kbp_device_enable_interrupt(struct kbp_device *device, struct kbp_status_errors *en_err);



kbp_status kbp_device_clear_errors(struct kbp_device *device, struct kbp_status_errors *sel_clr);



kbp_status kbp_device_get_error_list(struct kbp_device *device, uint64_t *error_buffer, uint32_t buffer_size, uint32_t *filled_count);



kbp_status kbp_device_config_smt(struct kbp_device *device, uint16_t smt_sb_map);


kbp_status kbp_device_scrub_tap_dma_buffer(struct kbp_device *device);






typedef int32_t (*kbp_device_issu_read_fn) (void *handle, uint8_t *buffer, uint32_t size, uint32_t offset);


typedef int32_t (*kbp_device_issu_write_fn) (void *handle, uint8_t *buffer, uint32_t size, uint32_t offset);



kbp_status kbp_device_save_state(struct kbp_device *device, kbp_device_issu_read_fn read_fn, kbp_device_issu_write_fn write_fn, void *handle);



kbp_status kbp_device_save_state_and_continue(struct kbp_device *device, kbp_device_issu_read_fn read_fn, kbp_device_issu_write_fn write_fn, void *handle);




kbp_status kbp_device_restore_state(struct kbp_device *device, kbp_device_issu_read_fn read_fn, kbp_device_issu_write_fn write_fn, void *handle);


kbp_status kbp_device_reconcile_start(struct kbp_device *device);


kbp_status kbp_device_reconcile_end(struct kbp_device *device);





kbp_status kbp_device_lookup_index(struct kbp_device *device, uint32_t hw_index, struct kbp_db **db, struct kbp_entry **entry);



kbp_status kbp_device_dump(struct kbp_device * device, uint32_t dump_xml_data_with_entries, FILE *fp);





kbp_status kbp_device_start_transaction(struct kbp_device *device);



kbp_status kbp_device_end_transaction(struct kbp_device *device);


kbp_status kbp_device_query_restore_status(struct kbp_device *device, enum kbp_restore_status *status);


kbp_status kbp_device_clear_restore_status(struct kbp_device *device);




kbp_status kbp_device_cache_writes(struct kbp_device *device);


kbp_status kbp_device_flush_ltr_writes(struct kbp_device *device);


kbp_status kbp_device_selective_shutdown(struct kbp_device *device, uint8_t *dba_bmp, uint32_t length_1);


kbp_status kbp_device_flush_all_writes(struct kbp_device *device, uint32_t flags);



const char *kbp_device_get_last_error(struct kbp_device *device);


kbp_status kbp_device_dump_core_regs(struct kbp_device *device, FILE *fp);


kbp_status kbp_device_dump_interface_regs(struct kbp_device *device, FILE *fp);


kbp_status kbp_device_read_last_result(struct kbp_device *device, uint32_t only_reg_data, FILE *fp);


kbp_status kbp_device_read_die_temperature(struct kbp_device *device, int32_t *temperature);


kbp_status kbp_device_read_die_voltage(struct kbp_device *device, int32_t *voltage);


kbp_status kbp_device_read_die_voltage_temperature(struct kbp_device *device, int32_t *data_out, int32_t is_voltage, int32_t delay_in_ms);


kbp_status kbp_device_memory_stats(struct kbp_device *device, FILE *fp);


kbp_status kbp_device_pio_register_write(struct kbp_device * device, uint32_t address, uint64_t data);


kbp_status kbp_device_pio_register_read(struct kbp_device * device, uint32_t address, uint64_t *o_data);


kbp_status kbp_device_load_resource_profile(struct kbp_device * device, char *resource_str);


kbp_status kbp_device_get_resource_profile(struct kbp_device *device, char *resource_str, uint32_t resource_str_size);


#ifdef __cplusplus
}
#endif
#endif 

