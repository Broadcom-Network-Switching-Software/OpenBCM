

#ifndef __KAPS_JR1_UDA_MGR_H
#define __KAPS_JR1_UDA_MGR_H

#include "kaps_jr1_errors.h"
#include "kaps_jr1_hw_limits.h"
#include "kaps_jr1_pool.h"
#include "kaps_jr1_portable.h"
#include "kaps_jr1_algo_hw.h"

#ifdef __cplusplus
extern "C"
{
#endif



    struct kaps_jr1_device;


#define MAX_UDA_CHUNK_LEVELS_ARRAY_SIZE        (28)




#define MAX_UDA_REGIONS 48



#define FREE_CHUNK      0 
#define ALLOCATED_CHUNK 1 

#define UDM_NO_HALF     0
#define UDM_LOWER_HALF  1
#define UDM_HIGHER_HALF 2
#define UDM_BOTH_HALF   3



    struct uda_mem_chunk
    {
        struct kaps_jr1_db *db;     
        void *handle;           
        struct uda_mem_chunk *prev_neighbor;    
        struct uda_mem_chunk *next_neighbor;    
        struct uda_mem_chunk *prev_free_chunk;  
        struct uda_mem_chunk *next_free_chunk;  

        uint32_t offset:20;     
        uint32_t region_id:6;   
        uint32_t type:1;
                     
        uint32_t size:20;       
        struct uda_mem_chunk **prev_partial_free_chunk; 
        struct uda_mem_chunk **next_partial_free_chunk; 
    };


    struct chunk_info
    {
        struct uda_mem_chunk *chunk_p;
        uint32_t lsn_id;
    };


    struct region
    {
        uint8_t udms[MAX_UDA_CHUNK_LEVELS_ARRAY_SIZE];
        uint8_t region_powered_up;      
        uint16_t num_lpu;       
        uint32_t num_row;       
        uint32_t is_diff_bb_widths; 
    };

    typedef kaps_jr1_status(
    *UdaMgr__UdaSBC) (
    struct uda_mem_chunk * uda_chunk,
    int32_t to_region_id,
    int32_t to_offset);
    typedef kaps_jr1_status(
    *UdaMgr__UpdateIIT) (
    struct uda_mem_chunk * uda_chunk);
    typedef void (
    *UdaMgr__update_lsn_size) (
    struct kaps_jr1_db * db);

    struct uda_config
    {
        uint8_t joined_udcs[MAX_UDA_CHUNK_LEVELS_ARRAY_SIZE];  
    };

    struct kaps_jr1_uda_mgr
    {
        UdaMgr__UdaSBC uda_mgr_sbc;
        UdaMgr__UpdateIIT uda_mgr_update_iit;
        UdaMgr__update_lsn_size uda_mgr_update_lsn_size;

        struct kaps_jr1_db *db;
        struct kaps_jr1_device *device;

        struct uda_mem_chunk *free_list[MAX_UDA_CHUNK_LEVELS_ARRAY_SIZE];  

        struct kaps_jr1_allocator *alloc;   
        uint8_t alloc_udm[KAPS_JR1_MAX_NUM_CORES][KAPS_JR1_UDM_PER_UDC][KAPS_JR1_ALLOC_UDM_MAX_COLS];       

        struct region region_info[MAX_UDA_REGIONS];     
        struct region compacted_region; 
        uint8_t enable_compaction;      

        struct uda_mem_chunk *neighbor_list;    

        struct uda_config config;       

        int32_t max_lpus_in_a_chunk;    
        uint32_t num_allocated_lpu_bricks;      
        uint32_t num_allocated_lpu_bricks_per_region[MAX_UDA_REGIONS];  
        uint32_t total_lpu_bricks;      
        uint32_t available_big_chunk_bricks;    
        int32_t max_rows_in_udm;        
        int16_t dt_index;
        uint16_t no_of_regions;
        uint32_t max_num_uda_chunk_levels;

            POOL_DEF(
    uda_mem_chunk) uda_chunk_pool;      
        struct chunk_info **chunk_map[MAX_UDA_REGIONS]; 
        
        uint32_t unit_test_mode:1;      
        uint32_t enable_dynamic_allocation:1;   
        uint32_t is_finalized:1;        
        uint32_t which_udm_half:2;      
        uint32_t chunk_alloc_started:1; 
        uint32_t has_index_based_ad; 
    };



    struct uda_mgr_stats
    {
        uint32_t total_num_allocated_lpu_bricks;        
        uint32_t total_num_free_lpu_bricks;     
    };



    kaps_jr1_status kaps_jr1_uda_mgr_init(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    int8_t dt_index,
    struct kaps_jr1_allocator *alloc,
    uint8_t alloc_udm[][KAPS_JR1_UDM_PER_UDC][KAPS_JR1_ALLOC_UDM_MAX_COLS],
    struct kaps_jr1_uda_mgr **mgr_pp,
    uint8_t max_lpus_in_a_chunk,
    uint8_t is_warmboot,
    uint8_t which_udm_half,
    UdaMgr__UdaSBC,
    UdaMgr__UpdateIIT,
    UdaMgr__update_lsn_size,
    int8_t unit_test_mode);


    kaps_jr1_status kaps_jr1_uda_mgr_configure(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_config *config);


    kaps_jr1_status uda_mgr_remove_db_from_common_uda_mgr(
    struct kaps_jr1_db *db);



    kaps_jr1_status kaps_jr1_uda_mgr_expand_all_allocated_udms(
    struct kaps_jr1_uda_mgr *mgr);



    kaps_jr1_status kaps_jr1_uda_mgr_wb_alloc(
    struct kaps_jr1_uda_mgr *mgr,
    void *handle,
    uint32_t rqt_num_lpus,
    uint32_t region_id,
    uint32_t offset,
    uint32_t lsn_id,
    struct uda_mem_chunk **alloc_chunk_pp);



    kaps_jr1_status kaps_jr1_uda_mgr_alloc(
    struct kaps_jr1_uda_mgr *mgr_p,
    int32_t rqt_num_lpus,
    struct kaps_jr1_device *device,
    void *handle,
    struct uda_mem_chunk **chunk,
    struct kaps_jr1_db *db);






    kaps_jr1_status kaps_jr1_uda_mgr_check_grow_up(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *cur_uda_chunk,
    uint32_t * grow_up_size_p);


    kaps_jr1_status kaps_jr1_uda_mgr_grow_up(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *cur_uda_chunk,
    uint32_t grow_up_size);



    kaps_jr1_status kaps_jr1_uda_mgr_check_grow_down(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *cur_uda_chunk,
    uint32_t * grow_down_size_p);


    kaps_jr1_status kaps_jr1_uda_mgr_grow_down(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *cur_uda_chunk,
    uint32_t grow_down_size);




    kaps_jr1_status kaps_jr1_uda_mgr_grow(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *cur_uda_chunk,
    uint32_t num_bricks_to_grow,
    uint32_t * num_bricks_grown_p,
    int8_t * flag_grown_up_p);



    kaps_jr1_status kaps_jr1_uda_mgr_free(
    struct kaps_jr1_uda_mgr *mgr_p,
    struct uda_mem_chunk *chunk);


    kaps_jr1_status kaps_jr1_uda_mgr_destroy(
    struct kaps_jr1_uda_mgr *mgr_p);



    kaps_jr1_status kaps_jr1_uda_mgr_wb_finalize(
    struct kaps_jr1_allocator *alloc,
    struct kaps_jr1_uda_mgr *mgr);


    kaps_jr1_status kaps_jr1_uda_mgr_wb_restore_regions(
    struct kaps_jr1_uda_mgr *mgr,
    struct kaps_jr1_allocator *alloc,
    int32_t no_of_regions,
    struct region region_info[]);



    kaps_jr1_status kaps_jr1_uda_mgr_calc_stats(
    const struct kaps_jr1_uda_mgr *mgr_p,
    struct uda_mgr_stats *uda_stats);



    kaps_jr1_status kaps_jr1_uda_mgr_print_stats(
    struct uda_mgr_stats *uda_stats);



    kaps_jr1_status kaps_jr1_uda_mgr_print_detailed_stats(
    const struct kaps_jr1_uda_mgr *mgr);



    kaps_jr1_status kaps_jr1_uda_mgr_usage(
    const struct kaps_jr1_uda_mgr *mgr_p,
    double *usage);



    kaps_jr1_status kaps_jr1_uda_mgr_verify(
    struct kaps_jr1_uda_mgr *mgr);



    kaps_jr1_status kaps_jr1_uda_mgr_compact(
    struct kaps_jr1_uda_mgr *mgr,
    int32_t region_id,
    int32_t new_region_id);


    kaps_jr1_status kaps_jr1_uda_mgr_compact_all_regions(
    struct kaps_jr1_uda_mgr *mgr);


    kaps_jr1_status kaps_jr1_uda_mgr_release_all_regions(
    struct kaps_jr1_uda_mgr *mgr);


    kaps_jr1_status kaps_jr1_uda_mgr_release_last_allocated_regions(
    struct kaps_jr1_uda_mgr *mgr);


    kaps_jr1_status kaps_jr1_uda_mgr_enable_dynamic_allocation(
    struct kaps_jr1_uda_mgr *mgr);


    kaps_jr1_status kaps_jr1_uda_mgr_disble_dynamic_allocation(
    struct kaps_jr1_uda_mgr *mgr);


    kaps_jr1_status kaps_jr1_uda_mgr_dynamic_uda_alloc_n_lpu(
    struct kaps_jr1_uda_mgr *mgr,
    int32_t num_udms_to_fit);


    uint32_t kaps_jr1_uda_mgr_get_num_free_bricks(
    struct kaps_jr1_uda_mgr *mgr);

    void kaps_jr1_uda_mgr_add_to_free_list(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *cur_chunk);
    void kaps_jr1_uda_mgr_add_to_neighbor_list(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *prev_chunk,
    struct uda_mem_chunk *cur_chunk);



    uint32_t kaps_jr1_uda_mgr_compute_brick_udc(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *uda_chunk,
    int32_t brick_no);
    uint32_t kaps_jr1_uda_mgr_compute_brick_row(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *uda_chunk,
    int32_t brick_no);
    kaps_jr1_status kaps_jr1_uda_mgr_compute_abs_brick_udc_and_row(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *uda_chunk,
    int32_t brick_no,
    uint32_t * abs_udc,
    uint32_t * abs_row);
    kaps_jr1_status
    kaps_jr1_uda_mgr_compute_abs_brick_udc_and_row_from_offset(
    struct kaps_jr1_uda_mgr * mgr,
    int32_t region_id,
    int32_t offset,
    uint32_t * abs_udc,
    uint32_t * abs_row);
    uint32_t kaps_jr1_uda_mgr_compute_brick_row_relative(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *uda_chunk,
    int32_t brick_no);
    uint32_t kaps_jr1_uda_mgr_compute_brick_udm(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *uda_chunk,
    int32_t brick_no);
    kaps_jr1_status kaps_jr1_uda_mgr_get_no_of_contigous_bricks(
    struct kaps_jr1_uda_mgr *mgr,
    int32_t region_id,
    int32_t offset);
    uint32_t kaps_jr1_uda_mgr_max_region_width(
    struct kaps_jr1_uda_mgr *mgr);
    uint32_t kaps_jr1_uda_mgr_compute_brick_udc_from_region_id(
    struct kaps_jr1_uda_mgr *mgr,
    int32_t region_id,
    int32_t offset,
    int32_t brick_no);
    kaps_jr1_status kaps_jr1_uda_mgr_compute_brick_udc_and_row_from_region_id(
    struct kaps_jr1_uda_mgr *mgr,
    int32_t region_id,
    int32_t chunk_offset,
    int32_t brick_no,
    uint32_t * p_abs_lpu,
    uint32_t * p_abs_row);
    kaps_jr1_status kaps_jr1_uda_mgr_get_relative_params_from_abs(
    struct kaps_jr1_uda_mgr *mgr,
    uint32_t lpu_no,
    uint32_t row_no,
    uint32_t * p_region_id,
    uint32_t * p_offset);

#ifdef __cplusplus
}
#endif
#endif 
