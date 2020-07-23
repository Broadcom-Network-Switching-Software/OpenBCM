/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>
#include <bcm/types.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/monterey.h>
#include <soc/mcm/memregs.h>
#include <soc/iproc.h>
#include <bcm/cpri.h>
#include <sal/compiler.h>

#ifdef CPRI_DIAG_SUPPORT
int
cpri_timesync_reset(int unit)
{
    uint32 rval=0;
    SOC_IF_ERROR_RETURN
        (READ_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr(unit, &rval));
    soc_reg_field_set(unit, NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr, &rval, LINCf, 0x0);
    SOC_IF_ERROR_RETURN
        (WRITE_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr(unit, rval));

    SOC_IF_ERROR_RETURN
        (READ_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, &rval));
    soc_reg_field_set(unit, NS_TIMESYNC_TS1_COUNTER_ENABLEr, &rval, ENABLEf, 1);
    SOC_IF_ERROR_RETURN
        (WRITE_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, rval));
    return SOC_E_NONE;

}

int sv_testing_cb(int unit,
                   bcm_port_t port,
                   bcm_cpri_interrupt_data_t *data,
                   void *userdata)

{
    uint64 offset ;
    uint32 lo, lo1, lo2;
    uint32 hi, hi1, hi2;
    bcm_cpri_tx_frame_tgen_config_t tgen_config;
    bcm_cpri_1588_timestamp_ts_config_t cmic_config;

    cpri_timesync_reset(unit);
    cmic_config.modulo_count = 1; 
    cmic_config.enable = 1; 
    bcm_cpri_port_1588_timestamp_ts_config_set(unit, port, BCM_CPRI_DIR_TX, BCM_CPRI_1588_TS_TYPE_CMIC, &cmic_config);
    bcm_cpri_port_1588_timestamp_ts_config_set(unit, port, BCM_CPRI_DIR_RX, BCM_CPRI_1588_TS_TYPE_CMIC, &cmic_config);

    hi2 = 0x25;
    lo2 = 0x373B3100;
    lo1 = COMPILER_64_LO(data->data64[0]);
    hi1 = COMPILER_64_HI(data->data64[0]);
    lo = lo1 + lo2;
    if (lo < lo1) {
    hi = hi1 + hi2 + 1;
    } else {
    hi = hi1 + hi2;
    }

    COMPILER_64_SET(offset, hi, lo);

    tgen_config.tx_tgen_ts_offset = offset ;
    bcm_cpri_port_tx_frame_tgen_config_set(unit, port, 1, &tgen_config);

    return 0;
}

int sv_testing_polling(int unit,
                        bcm_port_t port)

{
    bcm_cpri_1588_timestamp_ts_config_t cmic_config;
    bcm_cpri_1588_timestamp_t config;
    int count =0;

    while (count==0) {
        bcm_cpri_port_1588_timestamp_get(unit, port,BCM_CPRI_DIR_RX,
                                             BCM_CPRI_1588_TS_TYPE_SW,
                                             1, &count, &config);
    }
    cpri_timesync_reset(unit);
    cmic_config.modulo_count    = 1;
    cmic_config.enable          = 1;
    bcm_cpri_port_1588_timestamp_ts_config_set(unit, port, BCM_CPRI_DIR_RX, BCM_CPRI_1588_TS_TYPE_CMIC, &cmic_config);
    bcm_cpri_port_1588_timestamp_ts_config_set(unit, port, BCM_CPRI_DIR_TX, BCM_CPRI_1588_TS_TYPE_CMIC, &cmic_config);

    return 0;
}

void cpri_diag_test_interrupt_cb(int unit,
                   bcm_port_t port,
                   bcm_cpri_interrupt_data_t *data,
                   void *user_data)

{
    uint32  user_info = 0xDADADADA;

    if (user_data != NULL) {
        user_info = *(uint32*)user_data ;
    }
    LOG_CLI((" cpri_diag_test_interrupt_CN u=%d port=%d user_data=0x%x",unit,port,user_info));
    LOG_CLI(("\n  Interrupt is Served ID = %d %d %d 0x%x ....\n",data->cpri_intr_type, data->axc_id, data->queue_num, user_info));

    return ;
}

void cpri_diag_test_interrupt_cb2(int unit,
                   bcm_port_t port,
                   bcm_cpri_interrupt_data_t *data,
                   void *user_data)

{
    uint32  user_info = 0xDADADADA;

    if (user_data != NULL) {
        user_info = *(uint32*)user_data ;
    }
    LOG_CLI((" cpri_diag_test_interrupt_cb2 u=%d port=%d user_data=0x%x",unit,port,user_info));
    LOG_CLI(("\n  Interrupt is Served ID = %d %d %d 0x%x ....\n",data->cpri_intr_type, data->axc_id, data->queue_num, user_info));

    return ;
}

void cpri_diag_test_capture_interrupt_cb(int unit,
                   bcm_port_t port,
                   bcm_cpri_interrupt_data_t *data,
                   void *user_data)

{
#if 0
    uint32  user_info = 0xDADADADA;
    bcm_cpri_1588_timestamp_ts_config_t cmic_config;

    cpri_timesync_reset(unit);
    cmic_config.modulo_count    = 1;
    cmic_config.enable          = 1;
    bcm_cpri_port_1588_timestamp_ts_config_set(unit, port, BCM_CPRI_DIR_RX, BCM_CPRI_1588_TS_TYPE_CMIC, &cmic_config);
    bcm_cpri_port_1588_timestamp_ts_config_set(unit, port, BCM_CPRI_DIR_TX, BCM_CPRI_1588_TS_TYPE_CMIC, &cmic_config);
    if (user_data != NULL) {
        user_info = *(uint32*)user_data ;
    }
    LOG_CLI(("capture_interrupt_cb  u=%d port=%d user_data=0x%x\n",unit,port,user_info));
    LOG_CLI(("Interrupt is Served ID = %d ....\n",data->cpri_intr_type));
#endif
    LOG_CLI(("CAP %d 0x%x 0x%x\n",data->data,
                 COMPILER_64_HI(data->data64[0]),
                 COMPILER_64_LO(data->data64[0])));

    return ;
}
int  fifo_counter =0;
void cpri_diag_test_fifo_interrupt_cb(int unit,
                   bcm_port_t port,
                   bcm_cpri_interrupt_data_t *data, 
                   void *user_data)

{
    int   index;
#if 0
    uint32  user_info = 0xABABABAB;

    if (user_data != NULL) {
        user_info = *(uint32*)user_data ;
    }
    LOG_CLI((" fifo_interrupt_cb  u=%d port=%d user_data=0x%x",unit,port,user_info));
    LOG_CLI(("\n  Interrupt is Served ID = %d ....\n",data->cpri_intr_type));
#endif
    for (index = 0; index < data->data; index++) { 
        LOG_CLI(("FI %d 0x%x 0x%x\n",data->data,
                 COMPILER_64_HI(data->data64[index]),
                 COMPILER_64_LO(data->data64[index])));
    }
    if (fifo_counter++ > 10){
        bcm_cpri_port_interrupt_enable_set(0,port,
                                   BCM_CPRI_INTR_RX_1588_TS_FIFO,
                                   0,
                                   0); /* enable/disable */
        fifo_counter =0;
    }

    return ;
}

uint32  user_int_data = 0xDEADBEEF;
int cpri_diag_test_register_interrupt_callback( int unit, int port) 
{
    bcm_cpri_port_interrupt_callback_register(unit,
                                              BCM_CPRI_INTR_RX_PCS_LINK_STATUS_LH,
                                              cpri_diag_test_interrupt_cb,
                                              &user_int_data);

    bcm_cpri_port_interrupt_callback_register(0,
                                              BCM_CPRI_INTR_RX_ENCAP_CTRL_Q_OVERFLOW,
                                              cpri_diag_test_interrupt_cb2,
                                              &user_int_data);
#if 0
    bcm_cpri_port_interrupt_callback_register(unit,
                                              BCM_CPRI_INTR_RX_PCS_LINK_STATUS_LL,
                                              cpri_diag_test_interrupt_cb,
                                              &user_int_data);

    bcm_cpri_port_interrupt_callback_register(unit,
                                              BCM_CPRI_INTR_RX_1588_CAPTURED_TS,
                                              cpri_diag_test_capture_interrupt_cb,
                                              &user_int_data);

    bcm_cpri_port_interrupt_callback_register(unit,
                                              BCM_CPRI_INTR_TX_1588_CAPTURED_TS,
                                              cpri_diag_test_capture_interrupt_cb,
                                              &user_int_data);

    bcm_cpri_port_interrupt_callback_register(unit,
                                              BCM_CPRI_INTR_RX_1588_TS_FIFO,
                                              cpri_diag_test_fifo_interrupt_cb,
                                              &user_int_data);

    bcm_cpri_port_interrupt_callback_register(unit,
                                              BCM_CPRI_INTR_TX_1588_TS_FIFO,
                                              cpri_diag_test_fifo_interrupt_cb,
                                              &user_int_data);
#endif
    return 0;
}

int cpri_diag_test_callback_register(int unit, int port, int key)
{
    return BCM_E_NONE;
}

int cpri_diag_test_callback_user_data_set(int unit, int port, int key,
                                          int *user_data, int num_entries)
{
    return BCM_E_NONE;
}


#endif /* CPRI_DIAG_SUPPORT */
 
