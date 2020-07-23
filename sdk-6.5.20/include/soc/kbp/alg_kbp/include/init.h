/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef __INIT_H
#define __INIT_H



#include <stdint.h>
#include <stdio.h>

#include "errors.h"

#ifdef __cplusplus
extern "C" {
#endif





enum kbp_device_type {
    KBP_DEVICE_UNKNOWN,       
    KBP_DEVICE_12K,           
    KBP_DEVICE_OP,            
    KBP_DEVICE_OP2,           
    KBP_DEVICE_KAPS,          
    KBP_DEVICE_INVALID        
};



enum kbp_device_flags {
    KBP_DEVICE_DEFAULT = 0,           
    KBP_DEVICE_SMT = 1,               
    KBP_DEVICE_DUAL_PORT = 2,         
    KBP_DEVICE_NBO = 4,               
    KBP_DEVICE_SUPPRESS_INDEX = 8,    
    KBP_DEVICE_ISSU = 16,             
    KBP_DEVICE_CASCADE = 32,          
    KBP_DEVICE_SKIP_INIT = 64,        
    KBP_DEVICE_SUPPRESS_PARITY = 128, 
    KBP_DEVICE_QUAD_PORT = 256,       
    KBP_DEVICE_HST_DIG_LPBK = 512,    
    KBP_DEVICE_THREAD_SAFETY = 1024   
};



enum kbp_device_lane_speed {
    KBP_INIT_LANE_12_5 = 0,     
    KBP_INIT_LANE_3_125 = 1,    
    KBP_INIT_LANE_6_25 = 2,     
    KBP_INIT_LANE_10_3 = 3,     
    KBP_INIT_LANE_1_25 = 4,     

    KBP_INIT_LANE_15 = 5,       
    KBP_INIT_LANE_25_78125 = 6, 
    KBP_INIT_LANE_27_34375 = 7, 
    KBP_INIT_LANE_28_125 = 8,   
    KBP_INIT_LANE_51_5625 = 9,  
    KBP_INIT_LANE_56_25 = 10,   
    KBP_INIT_LANE_30 = 11,      
    KBP_INIT_LANE_46 = 12,      
    KBP_INIT_LANE_53_125 = 13,  
    KBP_INIT_LANE_INVALID = 14
};



enum kbp_device_ila_burst_short {
    KBP_INIT_BURST_SHORT_8_BYTES = 0,  
    KBP_INIT_BURST_SHORT_16_BYTES = 1, 
    KBP_INIT_BURST_SHORT_INVALID
};


enum blackhawk_tsc_txfir_tap_enable_enum {
    NRZ_LP_3TAP  = 0,
    NRZ_6TAP     = 1,
    PAM4_LP_3TAP = 2,
    PAM4_6TAP    = 3
};


enum kbp_prbs_polynomial {
    KBP_PRBS_7  = 0,
    KBP_PRBS_9  = 1,
    KBP_PRBS_11 = 2,
    KBP_PRBS_15 = 3,
    KBP_PRBS_23 = 4,
    KBP_PRBS_31 = 5,
    KBP_PRBS_58 = 6
};

#define kbp_blackhawk_tsc_txfir_tap_enable_enum blackhawk_tsc_txfir_tap_enable_enum


struct kbp_serdes_tap_settings {
    int16_t enable_taps;
    int16_t pre2;
    int16_t pre1;
    int16_t mainx;
    int16_t post1;
    int16_t post2;
    int16_t post3;
};


struct kbp_device_interface_settings {
    struct kbp_serdes_tap_settings tap_settings;
    uint16_t user_tap_settings;
    uint16_t link_training_enable;
    uint16_t media_type;
    uint16_t dfe_mode;
    uint16_t dfe_lp_mode;
    uint16_t silent_logs;
    uint16_t disable_restart_linktrn;
    uint16_t hst_pcs_poll_delay;                
};



struct kbp_device_config {
    struct
    {
        uint32_t start_lane;   
        uint32_t num_lanes;    
    } port_map[4];             

    enum kbp_device_lane_speed speed;      
    enum kbp_device_ila_burst_short burst; 
    uint32_t meta_frame_len;               
    uint32_t reset_only;                   

    void *handle;                          
    void *misc_handle;                     
    uint32_t reverse_lanes;                

    

    uint32_t num_devices;
    

    int32_t (*assert_kbp_resets) (void *misc_handle, int32_t s_reset_low, int32_t c_reset_low);

    

    int32_t (*mdio_read) (void *handle, int32_t chip_no, uint8_t dev, uint16_t reg, uint16_t *value);

    

    int32_t (*mdio_write) (void *handle, int32_t chip_no, uint8_t dev, uint16_t reg, uint16_t value);

    
    int32_t (*usleep) (void *misc_handle, uint32_t value);

    
    int32_t (*pre_enable_link_callback) (void *misc_handle);

    
    int32_t (*ucode_dma_load) (void *handle, const uint8_t *ucode, uint32_t len);

    
    int32_t (*get_rx_tx_lanes_bmp) (uint64_t *rx_lanes_bmp, uint64_t *tx_lanes_bmp);

    
    int32_t (*get_polarity_invert_lanes_bmp) (uint64_t *lanes_bmp);

    
    uint32_t init_avs;                      
    int32_t max_avs_voltage;                
    int32_t min_avs_voltage;                
    FILE *debug_fp;                         

    uint32_t reset_done;                    
    uint16_t fec_frame_length;              
    struct kbp_device_interface_settings settings; 
};



#define KBP_DEVICE_CONFIG_DEFAULT                       \
    {                                                   \
        { { 0, 12 },                  \
          { 0, 0  },                  \
          { 0, 0  },                  \
          { 0, 0  }                   \
        },                                              \
        KBP_INIT_LANE_12_5,           \
        KBP_INIT_BURST_SHORT_8_BYTES,        \
        0x80,                           \
        0,                              \
        0,                  \
        0,                      \
        0,                           \
        1,                       \
        0,                          \
        0,                               \
        0,                              \
        0,                                  \
        0,                    \
        0,                          \
        0,                  \
        0,        \
        0,                           \
        0,                         \
        0,                         \
        NULL,               \
        0x1000                    \
    }



kbp_status kbp_device_interface_init(enum kbp_device_type type, uint32_t flags, struct kbp_device_config *config);



kbp_status kbp_device_interface_config_smt(enum kbp_device_type type, struct kbp_device_config *config, uint32_t device_no, uint16_t smt_sb_map);




kbp_status kbp_device_interface_print_regs(enum kbp_device_type type, uint32_t flags, struct kbp_device_config *config, FILE *fp);



kbp_status kbp_device_interface_prbs(enum kbp_device_type type, uint32_t flags, struct kbp_device_config *config,
                                     enum kbp_prbs_polynomial prbs_poly, uint32_t enable);



kbp_status kbp_device_interface_prbs_print(enum kbp_device_type type, uint32_t flags,
                                           struct kbp_device_config *config);



kbp_status kbp_device_interface_eyescan_print(enum kbp_device_type type, uint32_t flags, struct kbp_device_config *config);



kbp_status kbp_device_interface_per_lane_prbs(enum kbp_device_type type, uint32_t flags, struct kbp_device_config *config,
                                              enum kbp_prbs_polynomial prbs_poly, uint32_t enable, uint64_t lane_bitmap);



kbp_status kbp_device_interface_per_lane_prbs_print(enum kbp_device_type type, uint32_t flags, struct kbp_device_config *config, uint64_t lane_bitmap);



kbp_status kbp_device_interface_per_lane_eyescan_print(enum kbp_device_type type, uint32_t flags, struct kbp_device_config *config, uint64_t lane_bitmap);



kbp_status kbp_device_interface_tx_serdes_emphasize(enum kbp_device_type type, struct kbp_device_config *config, uint64_t lane_bmp,
                                                    int8_t pre, int8_t main, int8_t post1, int8_t post2, int8_t post3);



kbp_status kbp_device_interface_enable_tx_link(enum kbp_device_type type, struct kbp_device_config *config,
                                               uint8_t port_bmp, uint8_t enable);



kbp_status kbp_device_interface_enable_rx_link(enum kbp_device_type type, struct kbp_device_config *config,
                                               uint8_t port_bmp, uint8_t enable);


kbp_status kbp_device_interface_prbs_error(enum kbp_device_type type, struct kbp_device_config *config,
                                           uint32_t lane_no, uint32_t *prbs_err_cnt, uint8_t *lock_lost);



kbp_status kbp_device_interface_prbs_inject_error(enum kbp_device_type type, uint32_t flags, struct kbp_device_config *config,
                                                  uint32_t error_count);



enum kbp_serdes_loopback_type {
    KBP_SERDES_DIGITAL_LOOPBACK,
    KBP_SERDES_REMOTE_LOOPBACK
};



kbp_status kbp_device_interface_serdes_loopback(enum kbp_device_type type, struct kbp_device_config *config,
                                                enum kbp_serdes_loopback_type loopback_type, uint8_t enable,
                                                uint64_t lane_bmp);


kbp_status kbp_device_interface_read_die_temperature(enum kbp_device_type type, struct kbp_device_config *config,
                                                     int32_t *temperature);


kbp_status kbp_device_interface_read_die_voltage(enum kbp_device_type type, struct kbp_device_config *config,
                                                 int32_t *voltage);



kbp_status kbp_device_interface_diag_print(enum kbp_device_type type, uint32_t flags,
                                           struct kbp_device_config *config);



kbp_status kbp_device_interface_per_lane_diag_print(enum kbp_device_type type, uint32_t flags,
                                                    struct kbp_device_config *config, uint64_t lane_bmp);


kbp_status kbp_device_interface_reset(enum kbp_device_type type, struct kbp_device_config *config);


kbp_status kbp_device_interface_dfe_setting(enum kbp_device_type type, struct kbp_device_config *config, uint64_t lane_bmp,
                                            uint8_t dfe_on, uint8_t dfe_lp_mode);

kbp_status kbp_device_pm_interface_init(int unit, void *xpt, int32_t port_speed, int32_t port_bmp,
                                        int32_t is_nrz, int32_t link_training,
                                        struct kbp_serdes_tap_settings *emph_vals);


kbp_status kbp_op_tap_device_pm_info_get(int unit, void *xpt);


kbp_status kbp_device_interface_read_die_voltage_temperature(enum kbp_device_type type, struct kbp_device_config *config,
                                                             int32_t *data_out, int32_t is_voltage, int32_t delay_in_ms);



kbp_status kbp_device_interface_serdes_tap_settings(enum kbp_device_type type, struct kbp_device_config *config,
                                                    uint32_t lane_bmp, struct kbp_serdes_tap_settings *tap_settings);



kbp_status kbp_device_pm_interface_serdes_tap_settings(void *xpt, uint16_t lane_bmp,
                                                       struct kbp_serdes_tap_settings *tap_settings);



kbp_status kbp_device_pm_interface_serdes_lanestate(void *xpt, uint16_t lane_bmp);



kbp_status kbp_device_pm_interface_serdes_prbs(void *xpt, enum kbp_prbs_polynomial prbs_poly,
                                               uint32_t enable, uint16_t lane_bitmap);



kbp_status kbp_device_pm_interface_serdes_prbs_print(void *xpt, uint16_t lane_bitmap);



kbp_status kbp_device_pm_interface_serdes_eyescan(void *xpt, uint16_t lane_bmp);



kbp_status kbp_device_pm_interface_dump_port_regs(void *xpt_hdl, uint32_t port_id, FILE *fp);



kbp_status kbp_mdio_write(struct kbp_device_config *config, uint32_t size, uint32_t base_addr, uint8_t *data);



kbp_status kbp_mdio_read(struct kbp_device_config *config, uint32_t size, uint32_t base_addr, uint8_t *data);



kbp_status kbp_device_interface_program_PHMF_FIFO(enum kbp_device_type type, struct kbp_device_config *config,
                                                  uint64_t lane_bitmap, uint8_t reset);



kbp_status kbp_device_interface_PHMF_FIFO_toggle(enum kbp_device_type type, struct kbp_device_config *config, uint32_t flags);



kbp_status kbp_device_interface_TXPCS_soft_reset_toggle(enum kbp_device_type type, struct kbp_device_config *config);



kbp_status kbp_device_interface_tx_rx_toggle(struct kbp_device_config *config, uint32_t flags);

kbp_status kbp_device_interface_fix_errors(enum kbp_device_type type, struct kbp_device_config *config,
                                           uint64_t *error_counters);



#ifdef __cplusplus
}
#endif
#endif 
