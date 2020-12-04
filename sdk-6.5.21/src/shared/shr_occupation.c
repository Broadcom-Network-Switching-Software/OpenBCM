/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Global occupation management
 * 
 * Provides a mechanism to allocate and manipulate bits (henceforth "items")
 * in a bitmap (henceforth "map"), according to input constraints.
 * After the allocation, the user can get and set the items in the map,
 * referring to them only by their IDs.
 * 
 * Allowed constraints:
 *   An item may require to have a certain amount of bits.
 *   An item can specify which bits may be allocated for it.
 * 
 * 
 * Example and usage guidelines:
 * -----------------------------
 * 
 *   The challenge:
 *      We have a "bag" of size 4 bits [0-3].
 *   We want to put 3 "artifacts" in this bag:
 *      - notebook: requires one bit.
 *      - bottle: requires one of the two MSBs.
 *                That is, only bits 2 or 3 may be allocated for the bottle.
 *      - laptop: requires two bits of the three LSBs.
 * 
 *   The Solution:
 *      To solve this problem we will create a map of size 4 to represent the bag.
 *          functions in use: shr_occ_map_args_init(), shr_occ_map_create.
 *      Then we will add the 3 items to represent the artifcats.
 *          functions in use: shr_occ_item_init(), shr_occ_map_add_item().
 *      Then we can solve these constraints and allocate bits for the items in the map.
 *          functions in use: shr_occ_map_solve().
 * 
 *   This solution is not exposed for the user - the user doesn't know which bits are allocated
 *   for each item. He only knows that the constrains are maintained.
 * 
 *   Saving and Using the solution:
 *          In order to save and load the solution, the user passes "save" and "load" callbacks to
 *      the map_args_init function.
 *          The user can then access the items and set their values from 0 to 2^(b-1),
 *      where 'b' is the number of bits the item required.
 *          For that he can use the provided access function: shr_occ_mgmt_item_access().
 *      This function takes as argument a "bag" item that represents the current bag state (the values of its itmes).
 *      Then using the item's ID in the access function, only the item-relevant part of the
 *      bag will be changed (on "set") / returned (on "get").
 * 
 *   For a code reference, you can take a look at:
 *   sdk/src/soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.c
 *   functions: arad_pp_occ_mgmt_init_inlif_profile(), arad_pp_occ_mgmt_app_get(), arad_pp_occ_mgmt_app_set().
 */

/************ 
 * INCLUDES *
 */
#include <shared/bsl.h>

#include <sal/core/sync.h>
#include <shared/shr_template.h>
#include <shared/error.h>
#include <shared/bitop.h>
#include <shared/shr_occupation.h>

/********** 
 * MACROS *
 */
/* { */

/* go over all the non empty items of the map */
#define SHR_OCC_MAP_ITEM_ITER(map, item, idx) \
    for (idx = 0; idx < map->size; idx++) \
        if ((item = &map->items[idx])->id != SHR_OCC_ITEM_ID_EMPTY)

/* } */
/**********************
 * INTERNAL FUNCTIONS *
 */
/* { */

/* free item */
void
_shr_occ_item_destroy (shr_occ_item_t* item){
    if (item != NULL) {
        if (item->mask != NULL) {
            sal_free(item->mask);
        }
    }
    return;
}

/* 
 * free map memory 
 * note: shr_occ_map_solve already calls it.
 */
void
_shr_occ_map_destroy (shr_occ_map_t* map)
{
    if (map != NULL) {
        if (map->items != NULL) {
            int i;
            for (i=0; i < map->size; i++) {
                _shr_occ_item_destroy(&map->items[i]);
            }
            sal_free(map->items);
        }
        if (map->solution != NULL) {
            sal_free(map->solution);
        }     
    }
    return;
}

/* saves map's solution to SW state */
int
_shr_occ_mgmt_solution_save(int unit, shr_occ_map_t* map){
    int i = 0, res = _SHR_E_NONE;

    if (map->save == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Warning: map's save callback is empty, map not saved.\n")));
        goto exit;
    }

    /* cell 0 contains size */
    res = (*(map->save))(unit,0,map->size);
    if (res != _SHR_E_NONE) {
        goto exit;
    }

    for (i = 0; i < map->size; i++) {
        res = (*(map->save))(unit,i+1,map->solution[i]);
        if (res != _SHR_E_NONE) {
            goto exit;
        }
    }

exit:
    return res;
}

/* insert "item_idx" into map's solution[solution_idx] */
int
_shr_occ_map_add_to_solution (int unit, shr_occ_map_t* map, int solution_idx, int item_idx){

    if (map->solution[solution_idx] != SHR_OCC_MAP_SOLUTION_BIT_EMPTY){
        LOG_ERROR(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit, "Error, map is overconstrained\n")));
        return _SHR_E_PARAM;
    }

    /* add to solution */
    map->bits_to_solve--;
    map->solution[solution_idx] = map->items[item_idx].id;

    /* mark item as solved */
    map->items[item_idx].id = SHR_OCC_ITEM_ID_EMPTY;
          
    return _SHR_E_NONE;
}

/* 
 * given an array 'arr' of size 'size' containing non negative integers, 
 * return value 'min_idx' is the index of the minimum positive element of the array. 
 */ 
void
_shr_occ_find_minimum_positive_idx(uint32* arr, int size, int* min_idx){
    uint32 
        i,
        min = 0xffffffff;
    for (i = 0; i < size; i++) {
        if (arr[i] > 0 && arr[i] < min) {
            min = arr[i];
            *min_idx = i;
        }
    }
}

/* 
 * given a mask 'mask' with size 'map->size', returns how many bits 
 * are set in this mask. 
 */
int
_shr_occ_nof_bits_in_mask(shr_occ_map_t* map, SHR_BITDCL* mask){
    int i,count = 0;
    if (mask == 0) {
        return 0;
    }
    
    SHR_BIT_ITER(mask, map->size, i) {
        count++;
    }
    return count;
}

/* 
 * If the mask has only one set bit, return its index.
 * Otherwise, return -1
 */
int 
_shr_occ_single_bit_in_mask(shr_occ_map_t* map, SHR_BITDCL* mask){
    int i;
    if (_shr_occ_nof_bits_in_mask(map,mask) == 1) {
        SHR_BIT_ITER(mask, map->size, i){
            return i;
        }
    }
    return -1;
}

/* 
 * trivial items are those who have a single possible solution. 
 * this function adds them to solution. 
 */
int
_shr_occ_map_solve_trivial_items (int unit, shr_occ_map_t* map){
    int rv = _SHR_E_NONE;
    int i, j, on_bit;
    shr_occ_item_t* item;

    SHR_OCC_MAP_ITEM_ITER(map,item,i) {
        /* 
         * Check if the item has only one allowed bit in mask.
         * In that case, we must allocate this bit to this item
         */
        if ((on_bit = _shr_occ_single_bit_in_mask(map, item->mask)) != -1) {

            /* Try to allocate this bit for this item */
            rv = _shr_occ_map_add_to_solution(unit,map,on_bit,i);
            if (rv != _SHR_E_NONE) {
                return rv;
            }

            /* turn this bit off in all of the remaining items */
            SHR_OCC_MAP_ITEM_ITER(map,item,j) {
                SHR_BITCLR(item->mask, on_bit);
            }

            /* 
             * Since the map has been changed, there may be new trivial items.
             * So start over. 
             */
            return _shr_occ_map_solve_trivial_items(unit,map);
        }
    }

    
    return rv;
}

/* returns the bit index that appears the least in items' masks */
int
_shr_occ_map_least_contended_bit(int unit, shr_occ_map_t* map){
    int i, j=0, lcb=0;
    shr_occ_item_t* item;
    uint32* contention_map = sal_alloc(sizeof(uint32) * map->size, "contention map");

    sal_memset(contention_map, 0, sizeof(uint32) * map->size);

    SHR_OCC_MAP_ITEM_ITER(map, item, i){
        j = 0;
        while (map->solution[j] != SHR_OCC_MAP_SOLUTION_END) {
            if (map->solution[j] == SHR_OCC_MAP_SOLUTION_BIT_EMPTY) {
                contention_map[j] += SHR_BITGET(item->mask, j) ? 1 : 0;
            }
            j++;
        }
    }

    _shr_occ_find_minimum_positive_idx(contention_map, map->size, &lcb);

    sal_free(contention_map);
    return lcb;
}

/* } */
/*************
 * FUNCTIONS *
 */
/* { */

/* 
 * initialize map's arguments. 
 * load/save should be callbacks to sw_state get/set functions.
 */
void
shr_occ_map_args_init(int unit, shr_occ_map_args_t* args, uint32 size, char* name, int (*load)(int, int, int*), int (*save)(int, int, int))
{
    args->size = size;
    args->name = name;
    args->load = load;
    args->save = save;
    return;
}

/* 
 * Initialize a map's item.
 */
void 
shr_occ_item_init(int unit, shr_occ_item_t* item, int id, SHR_BITDCL* mask, uint32 nof_bits)
{
    item->id = id;
    item->nof_bits = nof_bits;

    /* just a reference to mask at this point. later we will allocate a copy. */
    item->mask = mask;

    return;
}

/* allocate and initialize a map */
void
shr_occ_map_create (int unit, shr_occ_map_t* map, shr_occ_map_args_t args, uint32 flags)
{
    int i;
    map->name = args.name;
    map->size = args.size;
    map->save = args.save;
    map->load = args.load;
    map->bits_to_solve = 0;
    map->items = sal_alloc(sizeof(shr_occ_item_t) * args.size, "items");
    for (i=0; i < args.size; i++) {
        shr_occ_item_init(unit, &(map->items[i]), SHR_OCC_ITEM_ID_EMPTY, 0, 1);
    }
    map->solution = sal_alloc(sizeof(int) * (map->size + 1), "solution");
    for (i=0; i < map->size; i++) {
        map->solution[i] = SHR_OCC_MAP_SOLUTION_BIT_EMPTY;
    }
    map->solution[i] = SHR_OCC_MAP_SOLUTION_END;
    return;
}

/* add an item and its constraints to map. */
int
shr_occ_map_add_item (int unit, shr_occ_map_t* map, shr_occ_item_t item, uint32 flags)
{
    int rv = _SHR_E_NONE, success = 0, i;

    for (i=0; i < map->size; i++)
    {
        if (map->items[i].id == SHR_OCC_ITEM_ID_EMPTY)
        {
            SHR_BITDCL* mask_cpy = sal_alloc(SHR_BITALLOCSIZE(map->size),"mask");
            sal_memset(mask_cpy, 0, SHR_BITALLOCSIZE(map->size));

            SHR_BITCOPY_RANGE(mask_cpy,0, item.mask,0, map->size);
            map->items[i].mask = mask_cpy;
            map->items[i].id = item.id;
            map->bits_to_solve++;
            /* break multi-bit items into 1-bit items */
            if (item.nof_bits > 1) {
                item.nof_bits--;
                continue;
            }
            success = 1;
            break;
        }
    }

    if (!success) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Cannot add item, map is full\n")));
        return _SHR_E_RESOURCE;
    }

    return rv;
}

/* 
 * solves the constraints introduced by the map's items. 
 * saves the solution to SW DB.
 * destroys the map (free memory).
 */
int
shr_occ_map_solve (int unit, shr_occ_map_t* map, uint32 flags)
{
    int rv = _SHR_E_NONE;
    shr_occ_item_t* item;
    uint32* mask_sizes = sal_alloc(sizeof(uint32) * map->size, "items masks sizes");
    

    /* loop as long as solution is not complete */
    while (map->bits_to_solve > 0) {
        int lcb, i;
        sal_memset(mask_sizes, 0, sizeof(uint32) * map->size);

        /* look for trivial items and solve them first */
        rv = _shr_occ_map_solve_trivial_items(unit, map);
        if (rv != _SHR_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Failed to solve items for map %s\n"), map->name));
            goto exit;
        }
        if (map->bits_to_solve == 0) {
            break;
        }
        
        lcb = _shr_occ_map_least_contended_bit(unit, map);
        
        /*choose the contending item that has the least options*/
        SHR_OCC_MAP_ITEM_ITER(map, item, i) {
            if (SHR_BITGET(item->mask, lcb)) {
                mask_sizes[i] = _shr_occ_nof_bits_in_mask(map, item->mask);
                /* this bit will be allocated, so it's no longer relevant */
                SHR_BITCLR(item->mask, lcb);
            }
        }
        _shr_occ_find_minimum_positive_idx(mask_sizes, map->size, &i);
        rv = _shr_occ_map_add_to_solution(unit, map, lcb, i);
        if (rv != _SHR_E_NONE) {
            goto exit;
        }
    }

    rv = _shr_occ_mgmt_solution_save(unit, map);
    if (rv != _SHR_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Error, couldn't save solution for map %s\n"), map->name));
        goto exit;
    }

exit:
    sal_free(mask_sizes);
    _shr_occ_map_destroy(map);
    return rv;
}


/* 
 * loads map's solution from SW DB 
 * load needs to be a 'get' callback to SW DB.
 */
int
shr_occ_mgmt_solution_load(int unit, int (*load)(int,int,int*), int *solution){
    int i, res = _SHR_E_NONE;

    /* cell 0 contains size */
    res = (*load)(unit,0,solution);
    if (res != _SHR_E_NONE) {
        goto exit;
    }
    
    for (i = 1; i <= solution[0]; i++) {
        res = (*load)(unit,i,(solution+i));
        if (res != _SHR_E_NONE) {
            goto exit;
        }
    }

exit:
    return res;
}


/* 
 * once a map is solved and saved to SW state, one can access its items and read/write their values. 
 * each item's value is bounded by the item's size (recall that an item is added to the map with "nof_bits" property) 
 *  
 * access_type = 0 will read the item's value from full_map, and return it inside val.
 * access_type = 1 will set the item's value in full_map to val.
 * access_type = -1 will use full_map to return the item's mask (= which map bits are used by item).
 */ 
int
shr_occ_mgmt_item_access (
   int unit, 
   int (*load)(int,int,int*), 
   int (*save)(int,int,int), 
   int item_id, 
   uint32* val, 
   int access_type, 
   SHR_BITDCL* full_map)
{
    int i, size, res = _SHR_E_NONE;
    SHR_BITDCL* mask = NULL;

    /* cell 0 contains size */
    res = (*load)(unit,0,&size);
    if (res != _SHR_E_NONE) {
        goto exit;
    }

    /* obtain item's mask */
    mask = sal_alloc(SHR_BITALLOCSIZE(size),"mask");
    sal_memset(mask, 0, SHR_BITALLOCSIZE(size));
    
    for (i = 0; i < size; i++) {
        int id;
        res = (*load)(unit,i+1,&id);
        if (res != _SHR_E_NONE) {
            goto exit;
        }
        if (id == item_id) {
            SHR_BITSET(mask, i);
        }
    }

    /* return mask */
    if (access_type == -1) {
        SHR_BITCLR_RANGE(full_map,0,size);
        SHR_BITCOPY_RANGE(full_map, 0, mask, 0, size);
    }
    /* write */
    else if (access_type == 1) {
        /* set item's bits to zero */
        SHR_BITNEGATE_RANGE(mask,0,size,mask);
        SHR_BITAND_RANGE(mask,full_map,0,size,full_map);
        /* restore mask */
        SHR_BITNEGATE_RANGE(mask,0,size,mask);
        /* set value to the item's bits */
        SHR_BIT_ITER(mask,size,i){
            if (*val & 1){
                SHR_BITSET(full_map,i);
            }
            *val >>= 1;
        }
        if (*val > 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
                "Error, not enough bits for item (id=%d) in map for given value (%u)\n"),
                    item_id, *val));
            res = _SHR_E_PARAM;
            goto exit;
        }
    }
    /* read */
    else {
        int val_idx = 0;
        *val = 0;
        SHR_BIT_ITER(mask,size,i){
            if (SHR_BITGET(full_map,i)) {
                *val |= 1 << val_idx;
            }
            val_idx++;
        }
    }

exit:
    sal_free(mask);
    return res;
}

/* will set mask to reflect the map bits used by item_id */
int shr_occ_mgmt_item_mask_get(int unit, int (*load)(int,int,int*), int item_id, SHR_BITDCL *mask) {
    return shr_occ_mgmt_item_access(unit, load, NULL, item_id, NULL, -1, mask);
}

/* a test function for the mechanism. to use, call it from anywhere */
int shr_occ_testing(int unit){
    int                     res = _SHR_E_NONE;
    shr_occ_item_t    item;
    shr_occ_map_args_t      args;
    shr_occ_map_t           map;
    uint32 
        size = 256,
        i,j;

    /* Initialize map */
    shr_occ_map_args_init(unit, &args, size, "TEST_PROFILE", NULL, NULL);
    shr_occ_map_create(unit, &map, args, 0);

    /* Add items */
    LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit, "Solving:\n")));
    for (i = 0; i < size; i++) {
        SHR_BITDCL *random_mask = sal_alloc(SHR_BITALLOCSIZE(size),"random mask");
        for (j=0 ; j < size ; j++) {
            if (sal_rand() % 2) {
                SHR_BITSET(random_mask, j);
            } else {
                SHR_BITCLR(random_mask, j);
            }
        }
        SHR_BITSET(random_mask, i); /* makes sure map is feasible */
        shr_occ_item_init(unit, &item, i, random_mask, 1);
        res = shr_occ_map_add_item(unit, &map, item, 0);
        if (res != _SHR_E_NONE) {
            sal_free(random_mask);
            goto exit;
        }

        LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit, "\t\t%d: %x\n"), i, map.items[i].mask[0]));
        sal_free(random_mask);
    }

    /* Solve items */
    res = shr_occ_map_solve(unit, &map, 0);
    if (res != _SHR_E_NONE) {
        goto exit;
    }

    LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit, "\t\tsolution:\n")));
    for (i=0;i<size;i++) {
        LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit, "\t\t\t%d\n"), map.solution[i]));
    }

exit:
    return res;
}

/* } */
