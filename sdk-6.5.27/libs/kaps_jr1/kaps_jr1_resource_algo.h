

#ifndef __KAPS_JR1_RESOURCE_ALGO_H
#define __KAPS_JR1_RESOURCE_ALGO_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include "kaps_jr1_device.h"
#include "kaps_jr1_hw_limits.h"
#include "kaps_jr1_errors.h"
#include "kaps_jr1_list.h"
#include "kaps_jr1_device_alg.h"
#include "kaps_jr1_resource.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct kaps_jr1_db;
    struct kaps_jr1_instruction;
    struct kaps_jr1_device;
    struct kaps_jr1_key;
    struct kaps_jr1_uda_mgr;
    struct uda_mgr_playback_chunk;





#define MAX_LSN_WIDTH_FOR_GROW (8)



    enum resource_user_specified
    {
        RESOURCE_RPT_BMP,
        RESOURCE_DBA_BMP,
        RESOURCE_UDM_BMP,
        RESOURCE_USB_BMP,
        RESOURCE_NUM_DBA_DT,
        RESOURCE_NUM_SRAM_DT,
        RESOURCE_MAX_LSN_SIZE,

        RESOURCE_INVALID
    };



#define FAILED_FIT_LSN  4



#define FAILED_FIT_IT   8



#define FAILED_FIT_HITBIT   16

#define FAILED_FIT_UDA   32







    kaps_jr1_status kaps_jr1_initialize_mem_map(
    struct kaps_jr1_device *device);





    kaps_jr1_status kaps_jr1_resource_expand_uda_mgr_regions(
    struct kaps_jr1_uda_mgr *mgr);




    uint32_t kaps_jr1_resource_get_num_unused_uda_bricks(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db);


    uint32_t kaps_jr1_resource_get_num_unused_uda_bricks_op(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db);



    kaps_jr1_status kaps_jr1_resource_process(
    struct kaps_jr1_device *device);



    void kaps_jr1_resource_print_html(
    struct kaps_jr1_device *device,
    FILE * f);







    kaps_jr1_status kaps_jr1_resource_dynamic_uda_alloc(
    struct kaps_jr1_device *device,
    struct memory_map *mem_map,
    struct kaps_jr1_db *db,
    int8_t dt_index,
    int32_t * p_num_lpu,
    uint8_t udms[]);


    kaps_jr1_status kaps_jr1_resource_release_udm(
    struct kaps_jr1_device *device,
    struct memory_map *mem_map,
    struct kaps_jr1_db *db,
    int32_t dt_index,
    int32_t udc_no,
    int32_t udm_no);




#ifdef __cplusplus
}
#endif
#endif                          
