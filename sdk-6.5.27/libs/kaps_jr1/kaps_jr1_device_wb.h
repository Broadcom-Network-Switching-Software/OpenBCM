

#ifndef __KAPS_JR1_DEVICE_WB_H
#define __KAPS_JR1_DEVICE_WB_H

#include "kaps_jr1_errors.h"
#include "kaps_jr1_device.h"
#include "kaps_jr1_db_wb.h"

#ifdef __cplusplus
extern "C"
{
#endif



    kaps_jr1_status kaps_jr1_wb_save_device_info(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fn);

    kaps_jr1_status kaps_jr1_wb_restore_device_info(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fn);
    
    kaps_jr1_status kaps_jr1_wb_refresh_db_handle_in_map(
    struct kaps_jr1_device *main_device);
    
    kaps_jr1_status kaps_jr1_device_nv_memory_manager_init(
    struct kaps_jr1_device *device);
    
    kaps_jr1_status kaps_jr1_device_nv_memory_manager_cr_init(
    struct kaps_jr1_device *device,
    uint32_t db_count);


    kaps_jr1_status kaps_jr1_device_wb_save_dma(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fun);
    

    kaps_jr1_status kaps_jr1_device_wb_restore_dma(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fun);





#ifdef __cplusplus
}
#endif
#endif
