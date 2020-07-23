/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_INTER_H_
#define _SOC_INTER_H_

#include <soc/chip.h>
#include <sal/core/sync.h>
#include <shared/periodic_event.h>
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
#include <soc/mcm/allenum.h>
#endif
#include <soc/error.h>
#if defined (BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)
#define SOC_CMCS_NUM_MAX                (5)
#endif /* BCM_CMICM_SUPPORT || BCM_CMICX_SUPPORT  */

#ifdef BCM_PETRA_SUPPORT
#include <shared/swstate/sw_state.h>
#endif /* BCM_PETRA_SUPPORT */

/* 
 * Defines
 */

#define SOC_INTERRUPT_INTERRUPT_PER_REG_NUM_MAX 128 /* max number of interrupts at on register for Jericho */

#define SOC_INTERRUPT_BIT_FIELD_DONT_CARE                    -1
#define SOC_ACTIVE_INTERRUPTS_GET_UNMASKED_ONLY 0x1
#define SOC_ACTIVE_INTERRUPTS_GET_CONT_PREV 0x2

#define SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE              0 
#define SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE  1
/* priority bit 2 --5, 4 bits length */
#define SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LSB         2
#define SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LEN         4
#define SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE_BITS 6
#define SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS             7
#define SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB_BIT        8
#define SOC_INTERRUPT_DB_FLAGS_FORCE_MASK_BIT            9
#define SOC_INTERRUPT_DB_FLAGS_PRIORITY_MAX_VAL          15
#define SOC_INTERRUPT_DB_FLAGS_PRIORITY_MASK             (((1 << (SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LEN + SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LSB)) - 1) &(~(( 1 << SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LSB) -1))) 

#define SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE 0x40
#define SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK             0x80
#define SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB           0x100
#define SOC_INTERRUPT_DB_FLAGS_FORCE_MASK               0x200

/* 
 * Types Def
 */
 
 typedef struct soc_interrupt_clear_read_fifo_s {
    soc_reg_t fifo_reg;
    uint32    read_count;  
} soc_interrupt_clear_read_fifo_t;

typedef struct soc_interrupt_clear_reg_write_s {
    soc_reg_t status_reg;
    soc_reg_above_64_val_t data;  
} soc_interrupt_clear_reg_write_t;

typedef struct soc_interrupt_clear_array_index_s {
    soc_reg_t fifo_reg;
    uint32    read_count;  
    uint32    reg_index;
} soc_interrupt_clear_array_index_t;

typedef struct soc_interrupt_cause_s {
    int id;
    int index;
} soc_interrupt_cause_t;

typedef struct  soc_interrupt_tree_s {
    soc_reg_t int_reg;
    soc_reg_t int_mask_reg;
    int index;
    int int_id[SOC_INTERRUPT_INTERRUPT_PER_REG_NUM_MAX];
}soc_interrupt_tree_t;

typedef int (* clear_func)(int unit, int block_instance, int interrupt_id);

typedef int (* soc_handle_interrupt_func)(int unit, int block_instance, uint32 en_interrupt, char *msg);

typedef struct soc_interrupt_db_s {
#if !defined(SOC_NO_NAMES)
    char *name;
#endif
    int id;
    soc_reg_t reg;
    soc_reg_t reg_test;
    int reg_index;
    soc_field_t field;
    soc_reg_t mask_reg;
    int mask_reg_index;
    soc_reg_t cnt_reg;
    soc_field_t mask_field;
    int vector_id;
    CONST soc_interrupt_tree_t *vector_info;
    soc_block_type_t block_type;
    clear_func interrupt_clear;

    /* for clear function */
    void *interrupt_clear_param1;
    void *interrupt_clear_param2;
    
    /* per block instance*/
    uint32          *statistics_count;
    uint32          *storm_detection_occurrences;
    uint32          *storm_detection_start_time;
    uint32          *storm_nominal_count;

    int bit_in_field;

    soc_handle_interrupt_func func_arr;/*interrupt action,                                 always run it*/

    /* recurring action function/params */
    soc_handle_interrupt_func func_arr_recurring_action;/*interrupt action, run it only in a storm*/
    int recurring_action_counters;
    int recurring_action_time;                                                             
    int recurring_action_cycle_time;  /*time in seconds of                                 each cycle, -1 is without recurring_action*/
    int recurring_action_cycle_counting;  /*threshold in each cycle, -1 is without recurring_action*/
    char *msg;  /*used to give a guidness for manually  handled interrupts*/

} soc_interrupt_db_t;

typedef struct soc_interrupt_s {
    soc_interrupt_db_t *interrupt_db_info;
    soc_interrupt_tree_t *interrupt_tree_info;    /* interrupts array */
    sal_sem_t interrupt_sem;
} soc_interrupt_t;

typedef struct {
    int init_done;
    periodic_event_handler_t  handler;
    soc_interrupt_cause_t  *interrupts;
} soc_interrupt_event_control_t;

#ifdef BCM_PETRA_SUPPORT
typedef struct{
  
    uint32 flags;
    uint32 storm_timed_count;
    uint32 storm_timed_period;
  
} ARAD_INTERRUPT_DATA;

typedef struct
{  
/* The following 3 variables are not used and remain for future use */
  uint32      cmc_irq2_mask[SOC_CMCS_NUM_MAX]; /* Cached copy of CMIC_CMCx_PCIE_IRQ_MASK2 */
  uint32      cmc_irq3_mask[SOC_CMCS_NUM_MAX]; /* Cached copy of CMIC_CMCx_PCIE_IRQ_MASK3 */
  uint32      cmc_irq4_mask[SOC_CMCS_NUM_MAX]; /* Cached copy of CMIC_CMCx_PCIE_IRQ_MASK4 */
    
  PARSER_HINT_ARR ARAD_INTERRUPT_DATA* interrupt_data;
  
} ARAD_INTERRUPTS;
#endif /* BCM_PETRA_SUPPORT */

#define SOC_INTR_IS_SUPPORTED(unit) \
    (NULL != SOC_CONTROL(unit)->interrupts_info) 

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(PORTMOD_SUPPORT)
/* 
 * Functions
 */

/* Main interrupt handling SOC function */
void soc_cmn_block_error(void *unit_vp, void *d1, void *d2, void *d3, void *d4);
void soc_cmn_error(void *unit_vp, void *d1, void *d2, void *d3, void *d4);

/* Per interrupt functions*/
int soc_interrupt_is_valid(int unit, const soc_block_info_t* bi, const soc_interrupt_db_t* inter, int* is_valid /*out*/);
int soc_interrupt_get(int unit, int block_instance, const soc_interrupt_db_t* inter, int* inter_val /*out*/);
int soc_interrupt_force_get (int unit, int block_instance, const soc_interrupt_db_t* inter, int* inter_val);
int soc_interrupt_enable(int unit, int block_instance, const soc_interrupt_db_t* inter);
int soc_interrupt_force(int unit, int block_instance, const soc_interrupt_db_t* inter, int action);
int soc_interrupt_disable(int unit, int block_instance, const soc_interrupt_db_t* inter);
int soc_interrupt_is_enabled(int unit, int block_instance, const soc_interrupt_db_t* inter, int* is_enabled /*out*/);
int soc_interrupt_is_supported(int unit, int block_instance, int inter_id);
int soc_interrupt_has_func_arr(int unit, int inter_id);
int soc_interrupt_event_cb_none(
    int unit,
    soc_switch_event_t event,
    uint32 arg1,
    uint32 arg2,
    uint32 arg3,
    void *data);

int soc_interrupt_stat_cnt_increase(int unit, int bi, int interrupt_id);

int soc_interrupt_flags_set(int unit, int interrupt_id, uint32 flags);
int soc_interrupt_flags_get(int unit, int interrupt_id, uint32* flags);

/* Global interrupt functions */
int soc_nof_interrupts(int unit, int* nof_interrupts);
int soc_interrupt_is_all_mask(int unit, int *is_all_mask);

/* get the index of the block from the blocks port and interrupt id*/
int soc_interrupt_get_block_index_from_port(int unit, int interrupt_id, int port);
/* get the port of  a interrupt from interrupt ID and block index. */
int soc_interrupt_get_intr_port_from_index(int unit, int interrupt_id, int block_instance);

/* Storm functions */
int soc_interrupt_storm_timed_period_set(int unit, int interrupt_id, uint32 storm_timed_period);
int soc_interrupt_storm_timed_period_get(int unit, int intrrupt_id, uint32* storm_timed_period);

int soc_interrupt_storm_timed_count_set(int unit, int interrupt_id, uint32 storm_timed_count);
int soc_interrupt_storm_timed_count_get(int unit, int interrupt_id, uint32* storm_timed_count);

int soc_interrupt_update_storm_detection(int unit, int block_instance, soc_interrupt_db_t *inter);
int soc_interrupt_is_storm(int unit, int block_instance, soc_interrupt_db_t *inter, int *is_storm_count_period,int *is_storm_nominal);

/* Clear functions */
int soc_interrupt_clear_on_write(int unit, int block_instance, int interrupt_id);
int soc_interrupt_clear_on_reg_write(int unit, int block_instance, int interrupt_id);
int soc_interrupt_clear_on_clear(int unit, int block_instance, int interrupt_id);
int soc_interrupt_clear_on_read_fifo(int unit, int block_instance, int interrupt_id);
int soc_interrupt_clear_on_read_array_index(int unit, int block_instance, int interrupt_id);
int soc_interrupt_clear_all(int unit);
int soc_interrupt_is_all_clear(int unit, int *is_all_clear);

/*
 *flags available:
 *      SOC_ACTIVE_INTERRUPTS_GET_ALL - returns all active interrupts
 *      SOC_ACTIVE_INTERRUPTS_GET_UNMASKED_ONLY - returns just active and unmasked interrupts.
 *      SOC_ACTIVE_INTERRUPTS_GET_CONT_PREV - continue checking from th place the function stopped last time.
 * 
 * output:
 *      total_interrupts - the number of active interrupts (all or unmasked).
 *      interrupts[1..min(max_interrupts_size, *total_interrupts) - 1] - <cause, index> of each active interrupts.
 *      if *total_interrupts > max_interrupts_size interrupts do not include all active interrupts -
 *          for more interrupts run again with the flag SOC_ACTIVE_INTERRUPTS_GET_CONT_PREV.
 */
extern int
soc_active_interrupts_get(int unit, int flags, int max_interrupts_size,
                          soc_interrupt_cause_t *interrupts,
                          int *total_interrupts);

/*
 * returns: inter which togther with block_instance identifies the bits of the interrupt. 
 */
extern int
soc_interrupt_info_get(int unit, int interrupt_id, soc_interrupt_db_t *inter);

/* find the interupt ID of the register, register index and field*/
extern int 
soc_get_interrupt_id(int unit, soc_reg_t reg, int reg_index, soc_field_t field, int bit_in_field, int* interrupt_id);
/* find the interrupt ID of register address, block and bit index */
extern int
soc_get_interrupt_id_specific(int unit, int reg_adress, int reg_block, int field_bit, int* interrupt_id);
/* Order interrupts according to priority */
extern int 
soc_sort_interrupts_according_to_priority(int unit, soc_interrupt_cause_t *interrupts, uint32 interrupts_size);

#endif  /* #if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(PORTMOD_SUPPORT)*/

/*
 * Following is the CMIC interrupt framework
 * The frame work will be applicable for CMICX onwards
 */

#ifdef BCM_CMICX_SUPPORT

#define INTR_NAME_MAX    32
typedef unsigned int intr_num_t;

/* Interrupt handler structure used during registration */
typedef struct soc_cmic_intr_handler_s {
    intr_num_t    num;
    void    (*intr_fn)(int unit, void *data);
    void    *intr_data;
    char    intr_name[INTR_NAME_MAX];
} soc_cmic_intr_handler_t;

typedef void (*intr_fn_t)(int unit, void *data);

/*
 * CMIC operation structure to hold CMIC sprecific
 * function pointer table
 */
typedef struct soc_cmic_intr_op_s {
    int (*soc_cmic_intr_enable)(int unit, intr_num_t intr);
    int (*soc_cmic_intr_disable)(int unit, intr_num_t intr);
    int (*soc_cmic_intr_dump)(int unit, intr_num_t intr);
    int (*soc_cmic_intr_all_enable)(int unit);
    int (*soc_cmic_intr_all_disable)(int unit);
    int (*soc_cmic_intr_is_mask)(int unit, intr_num_t intr, int *mask);
    int (*soc_cmic_intr_register)(int unit, soc_cmic_intr_handler_t *handle,
                                   int size);
} soc_cmic_intr_op_t;


/*******************************************
* @function soc_cmic_intr_enable
* purpose Enable a particular interrupt
*
* @param unit [in] unit
* @param param [in] intr_num_t, Interrupt Number
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
soc_cmic_intr_enable(int unit, intr_num_t intr);

/*******************************************
* @function soc_cmic_intr_disable
* purpose Enable a particular interrupt
*
* @param unit [in] unit
* @param param [in] intr_num_t, Interrupt Number
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
soc_cmic_intr_disable(int unit, intr_num_t intr);


/*******************************************
* @function soc_cmic_intr_dump
* purpose dump registers particular interrupt
*
* @param unit [in] unit
* @param param [in] intr_num_t, Interrupt Number
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
soc_cmic_intr_dump(int unit, intr_num_t intr);



/*******************************************
* @function soc_cmic_intr_all_enable
* purpose Enable a particular interrupt
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
soc_cmic_intr_all_enable(int unit);

/*******************************************
* @function soc_cmic_all_intr_disable
* purpose Enable a particular interrupt
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
soc_cmic_intr_all_disable(int unit);

/*******************************************
* @function soc_cmic_intr_is_mask
* purpose Get a particular interrupt mask
*
* @param unit [in] unit
* @param param [in] intr_num_t, Interrupt Number
* @praram param [out] int, mask
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
soc_cmic_intr_is_mask(int unit, intr_num_t intr, int *mask);

/*******************************************
* @function soc_cmic_intr_register
* purpose Register the interrupt handler
*
* @param unit [in] unit
* @param param [in] soc_cmic_intr_handler_t pointer
* @param param [in] int, size of the array elements
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
soc_cmic_intr_register(int unit, soc_cmic_intr_handler_t *handle, int size);

/*******************************************
* @function soc_cmic_intr_init
* purpose initialize CMIC interrupt framework
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
soc_cmic_intr_init(int unit);

#endif  /* BCM_CMICX_SUPPORT */

#endif /* _SOC_INTER_H_ */  


