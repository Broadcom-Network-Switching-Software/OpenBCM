/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file defines memory measurement tool constants and functions.
 */

#ifndef _MEMORY_MEASUREMENT_TOOL
#define _MEMORY_MEASUREMENT_TOOL

#include <sal/core/thread.h>
#include <sal/types.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#define MEMORY_MEASUREMENT_DIAGNOSTICS_PRINT 1

#define MEMORY_MEASUREMENT_ELEMENTS_MAX_NUM 100
#define MEMORY_MEASUREMENT_ID_MAX_LENGTH 256

#define MEMORY_MEASUREMENT_INITIALIZE do { \
    if(!memory_measurement_tool_initialized) { \
        memory_measurement_tool_init(); \
        memory_measurement_tool_initialized = 1; \
    } \
} while(0)

typedef struct memory_measurement_element_s {
    char            id[MEMORY_MEASUREMENT_ID_MAX_LENGTH];
    uint8           is_active;
    uint32          sal_size;
    uint32          sw_state_size;
    sal_thread_t    thread_id;
} memory_measurement_element_t;

typedef struct memory_measurement_tool_s {
    int                             count;
    memory_measurement_element_t    elements[MEMORY_MEASUREMENT_ELEMENTS_MAX_NUM];
} memory_measurement_tool_t;

typedef struct memory_measurement_element_dnx_s {
    /** ID of the measurment element, used as handle */
    char            id[MEMORY_MEASUREMENT_ID_MAX_LENGTH];
    /** Indicates weather the measurment element is currently active */
    uint8           is_active;
    /** Holds the size of the memory allocated for the current measurment element */
    uint32          dnx_sw_state_size_alloc;
    /** Holds the size of the memory freed for the current measurment element */
    uint32          dnx_sw_state_size_free;
    /** Holds the thread ID. Memory measurement is done only on the calling thread*/
    sal_thread_t    thread_id;
} memory_measurement_element_dnx_t;

typedef struct memory_measurement_tool_dnx_s {
    /** Number of the measurment elements */
    int                             count;
    memory_measurement_element_dnx_t    elements[MEMORY_MEASUREMENT_ELEMENTS_MAX_NUM];
} memory_measurement_tool_dnx_t;

extern int memory_measurement_tool_initialized;
extern memory_measurement_tool_t memory_measurement_tool;
extern int memory_consumption_start_measurement(char *str);
extern int memory_consumption_pause_measurement(char *str);
extern int memory_consumption_end_measurement(char *str);
extern uint32 memory_consumption_measurement_get_sal_size(char *str);
extern int memory_measurement_print(char *str);
extern void memory_measurement_tool_init(void);

/*
 * Static structure that holds the information from the memory measurement tool
 */
extern memory_measurement_tool_dnx_t memory_measurement_tool_dnx;

/**
 * \brief - Function to start a measurement in the memory measurement tool. The memory measurement tool works on a stop-watch basis, meaning that measurements can be started and ended.
 * The actual memory value is the amount allocated/freed for SW state inbetween the start and end functions.
 *
 * \par DIRECT_INPUT:
 *   \param [in] str -  Measurement element ID.
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
extern int memory_consumption_start_measurement_dnx(char *str);

/**
 * \brief - Function to end measurement in the memory measurement tool. The memory measurement tool works on a stop-watch basis, meaning that measurements can be started and ended.
 * The actual memory value is the amount allocated/freed for SW state inbetween the start and end functions.
 *
 * \par DIRECT_INPUT:
 *   \param [in] str -  Measurement element ID.
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
extern int memory_consumption_end_measurement_dnx(char *str);

/**
 * \brief - Function to cleat the measurement entry in the memory measurement tool. The memory measurement tool works on a stop-watch basis, meaning that measurements can be started and ended.
 * The actual memory value is the amount allocated/freed for SW state inbetween the start and end functions.
 *
 * \par DIRECT_INPUT:
 *   \param [in] str -  Measurement element ID.
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
extern int memory_consumption_clear_measurement_dnx(char *str);

/**
 * \brief - Function to get the value of a specific measurement from the memory measurement tool
 *
 * \par DIRECT_INPUT:
 *   \param [in] str -  Measurement element ID.
 *   \param [out] dnx_sw_state_size -  size of the memory allocated for SW state for the specified measurement element in bytes.
 *   \param [in] is_alloc -  indicates if we want to get the memory allocated or memory freed.
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
extern int memory_measurement_dnx_sw_state_get(char *str, uint32 *dnx_sw_state_size, int is_alloc);

/**
 * \brief - Function to sample the allocated size in bytes from the SW state memory allocation macro
 *
 * \par DIRECT_INPUT:
 *   \param [in] size -  size of the memory allocated for SW state in bytes.
 *   \param [in] is_alloc -  indicates if we want to get the memory allocated or memory freed.
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   None
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
extern void memory_measurement_dnx_sw_state_sample(uint32 size, int is_alloc);

#endif	/* _MEMORY_MEASUREMENT_TOOL */
