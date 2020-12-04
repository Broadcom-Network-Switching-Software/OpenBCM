/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        shr_occupation.h
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
#ifndef _SHR_OCCUPATION_H_
#define _SHR_OCCUPATION_H_



/*************
 * INCLUDES  *
 */
/* { */
#include <sal/types.h>
#include <sal/core/alloc.h>
/* } */
/*************
 * DEFINES   *
 */
/* { */
#define SHR_OCC_ITEM_ID_EMPTY               (-1)
#define SHR_OCC_MAP_SOLUTION_BIT_EMPTY      (-1)
#define SHR_OCC_MAP_SOLUTION_END            (-2)
/* } */
/*************
 * MACROS    *
 */
/* { */

/* } */
/*************
 *  STRUCTS  *
 */
/* { */
typedef struct shr_occ_map_args_s{
    
    /* map size in bits */
    uint32 size;
    
    /* map name */
    char* name;

    /* callback for saving a map index in SW state */
    int (*save)(int unit, int idx, int val);

    /* callback for loading a map index from SW state */
    int (*load)(int unit, int idx, int* val);
    
} shr_occ_map_args_t;


typedef struct shr_occ_item_s{
    
    int id;
    
    /* which bits are allowed for this item */
    SHR_BITDCL* mask;
    
    /* number of bits required for this item */
    uint32 nof_bits;
    
} shr_occ_item_t;

typedef struct shr_occ_map_s{

    char* name;
    
    /* items added to map */
    shr_occ_item_t* items;
    
    /* map size in bits */
    uint32 size;

    /* solution for the items - will be saved to sw state
       when the map is solved */
    int* solution;

    /* remaining bits to solve */
    uint32 bits_to_solve;

    /* callback for saving a map index in SW state */
    int (*save)(int unit, int idx, int val);

    /* callback for loading a map index from SW state */
    int (*load)(int unit, int idx, int* val);
    
} shr_occ_map_t;
/* } */
/*************
 * GLOBALS   *
 */
/* { */

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
shr_occ_map_args_init(int unit, shr_occ_map_args_t* args, uint32 size, char* name, int (*load)(int, int, int*), int (*save)(int, int, int));

/* 
 * Initialize a map's item.
 */
void 
shr_occ_item_init(int unit, shr_occ_item_t* item, int id, SHR_BITDCL* mask, uint32 nof_bits);

/* allocate and initialize a map */
void
shr_occ_map_create (int unit, shr_occ_map_t* map, shr_occ_map_args_t args, uint32 flags);

/* add an item and its constraints to map. */
int
shr_occ_map_add_item (int unit, shr_occ_map_t* map, shr_occ_item_t item, uint32 flags);

/* 
 * solves the constraints introduced by the map's items. 
 * saves the solution to SW DB.
 * destroys the map (free memory).
 */
int
shr_occ_map_solve (int unit, shr_occ_map_t* map, uint32 flags);

/* 
 * loads map's solution from SW DB 
 * load needs to be a 'get' callback to SW DB.
 */
int
shr_occ_mgmt_solution_load(int unit, int (*load)(int,int,int*), int *solution);

/* will set mask to reflect the map bits used by item_id */
int
shr_occ_mgmt_item_mask_get(int unit, int (*load)(int,int,int*), int item_id, SHR_BITDCL *mask);

/* 
 * once a map is solved and saved to SW state, one can access its items and read/write their values. 
 * each item's value is bounded by the item's size (recall that an item is added to the map with "nof_bits" property) 
 *  
 * access_type = 0 will read the item's value from full_map, and return it inside val.
 * access_type = 1 will set the item's value in full_map to val.
 * access_type = -1 will use full_map to return the item's mask (= which map bits are used by item).
 */ 
int
shr_occ_mgmt_item_access(
   int          unit, 
   int          (*load)(int,int,int*), 
   int          (*save)(int,int,int), 
   int          item_id, 
   uint32       *val, 
   int          access_type, 
   SHR_BITDCL*  full_map);

/* a test function for the mechanism. to use, call it from anywhere */
int shr_occ_testing(int unit);

/* } */

#endif /* _SHR_OCCUPATION_H_ */
