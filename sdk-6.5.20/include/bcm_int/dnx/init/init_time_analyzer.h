/** \file init_time_analyzer.h
 * 
 * Define the modules running time to be analyzed in order to get better decisions when working on time optimization.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DNX_INIT_TIME_ANALYZER_H_INCLUDED
#define DNX_INIT_TIME_ANALYZER_H_INCLUDED
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/time.h>
#include <shared/utilex/utilex_time_analyzer.h>

/*
 * }
 */
/*
 * MACROs
 * {
 */
/*
 * Time Measurement Tool
 * =====================
 * *Allows measuring the amount of time spent in each code section
 * *Allows to count the times we run a specific code section
 * *Currently tracking few basic operations such as: (1) sw state (2) dnx data (3) dbal (4) resource manager
 * *The time measurements can be done per init step, per any two points in the code (using debugger or BCM shell)
 *
 * Enable
 * =======
 * go to "include/bcm_int/dnx/init/init_time_analyzer.h"
 * Modify macro "DNX_INIT_TIME_ANALYZER_*_EN" to 1 according to the procedures you want to measure
 * Recompile
 * Set soc property to "custom_feature_time_analyze=PER_STEP" to statistics per step or "custom_feature_time_analyze=GLOBAL" for shared statistics for all steps.
 * Run bcm.user
 *
 * Add a New Code Section to Track
 * ===============================
 * 1) In "include/bcm_int/dnx/init/init_time_analyzer.h",  add new member to enum "dnx_init_time_analyzer_module_e"
 * 2) Create Enable flag DNX_INIT_TIME_ANALYZER_<module>_EN or use one of the existing ones
 * 3) If new flag created, create also start and stop macros as follow
 *    "
      #if DNX_INIT_TIME_ANALYZER_DBAL_EN
      #define DNX_INIT_TIME_ANALYZER_<module>_START(unit, module_id)       utilex_time_analyzer_module_start(unit, module_id)
      #define DNX_INIT_TIME_ANALYZER_<module>_STOP(unit, module_id)        utilex_time_analyzer_module_stop(unit, module_id)
      #else
      #define DNX_INIT_TIME_ANALYZER_<module>_START(unit, module_id)
      #define DNX_INIT_TIME_ANALYZER_<module>_STOP(unit, module_id)
      #endif
 *    "
 * 4) Add new entry in DNX_INIT_TIME_ANALYZER_OPS_INIT
 * 5) Wrap your code section (more than one section is also supported) with:
 *     DNX_INIT_TIME_ANALYZER_<module>_START(unit, <new enum member>)
 *     DNX_INIT_TIME_ANALYZER_<module>_STOP(unit, <new enum member>)
 *
 * Display Results
 * ===============
 * BCM Shell > "ta dump"
 * GDB > "p sh_process_command(0, "ta dump")
 * Restart Measurement
 * BCM Shell > "ta clear"
 * GDB > "p sh_process_command(0, "ta clear")
 * Example Output:
 *  ============================================================================
    | Time Analyzer                                                            |
    ============================================================================
    | Name             | Total Time         | Occurrences | Avg Time           |
    ============================================================================
    | DBAL_SET         | 3 [milliseconds]   | 16          | 193 [microseconds] |
    | DBAL_GET         | 128 [milliseconds] | 2311        | 55 [microseconds]  |
    | DBAL_REST        | 32 [milliseconds]  | 9887        | 3 [microseconds]   |
    | ALGO_PORT_BITMAP | 0 [milliseconds]   | 0           | 0 [microseconds]   |
    | SLEEP            | 0 [milliseconds]   | 0           | 0 [microseconds]   |
    | DNX_DATA         | 18 [milliseconds]  | 23238       | 0 [microseconds]   |
    | SW_STATE         | 0 [milliseconds]   | 0           | 0 [microseconds]   |
    ============================================================================
 */
/**
 * \brief - set to 1 to enable. disabled by a compilation to optimize running time
 */
#define DNX_INIT_TIME_ANALYZER_DBAL_EN          0
#define DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN     0
#define DNX_INIT_TIME_ANALYZER_DNX_DATA_EN      0
#define DNX_INIT_TIME_ANALYZER_SW_STATE_EN      0
#define DNX_INIT_TIME_ANALYZER_RES_MNGR_EN      0
#define DNX_INIT_TIME_ANALYZER_ACCESS_EN        0
#define DNX_INIT_TIME_ANALYZER_ALGO_PORT_EN     0
#define DNX_ERROR_RECOVERY_TIME_DIAGNOSTICS     0

/*
 * regular init time analyzer diagnostics are disabled when error recovery diagnostics are enabled
 */
/** DBAL */
#if DNX_INIT_TIME_ANALYZER_DBAL_EN
#define DNX_INIT_TIME_ANALYZER_DBAL_START(unit, module_id)       utilex_time_analyzer_module_start(unit, module_id)
#define DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, module_id)        utilex_time_analyzer_module_stop(unit, module_id)
#else
#define DNX_INIT_TIME_ANALYZER_DBAL_START(unit, module_id)
#define DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, module_id)
#endif
/** DBAL INIT */
#if DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN
#define DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, module_id)       utilex_time_analyzer_module_start(unit, module_id)
#define DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, module_id)        utilex_time_analyzer_module_stop(unit, module_id)
#else
#define DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, module_id)
#define DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, module_id)
#endif
/** DNX DATA */
#if DNX_INIT_TIME_ANALYZER_DNX_DATA_EN
#define DNX_INIT_TIME_ANALYZER_DNX_DATA_START(unit, module_id)       utilex_time_analyzer_module_start(unit, module_id)
#define DNX_INIT_TIME_ANALYZER_DNX_DATA_STOP(unit, module_id)        utilex_time_analyzer_module_stop(unit, module_id)
#else
#define DNX_INIT_TIME_ANALYZER_DNX_DATA_START(unit, module_id)
#define DNX_INIT_TIME_ANALYZER_DNX_DATA_STOP(unit, module_id)
#endif
/** SW_STATE */
#if DNX_INIT_TIME_ANALYZER_SW_STATE_EN
#define DNX_INIT_TIME_ANALYZER_SW_STATE_START(unit, module_id)       utilex_time_analyzer_module_start(unit, module_id)
#define DNX_INIT_TIME_ANALYZER_SW_STATE_STOP(unit, module_id)        utilex_time_analyzer_module_stop(unit, module_id)
#else
#define DNX_INIT_TIME_ANALYZER_SW_STATE_START(unit, module_id)
#define DNX_INIT_TIME_ANALYZER_SW_STATE_STOP(unit, module_id)
#endif
/** RES_MNGR */
#if DNX_INIT_TIME_ANALYZER_RES_MNGR_EN
#define DNX_INIT_TIME_ANALYZER_RES_MNGR_START(unit, module_id)       utilex_time_analyzer_module_start(unit, module_id)
#define DNX_INIT_TIME_ANALYZER_RES_MNGR_STOP(unit, module_id)        utilex_time_analyzer_module_stop(unit, module_id)
#else
#define DNX_INIT_TIME_ANALYZER_RES_MNGR_START(unit, module_id)
#define DNX_INIT_TIME_ANALYZER_RES_MNGR_STOP(unit, module_id)
#endif
/** ACCESS */
#if DNX_INIT_TIME_ANALYZER_ACCESS_EN
#define DNX_INIT_TIME_ANALYZER_ACCESS_START(unit, module_id)       utilex_time_analyzer_module_start(unit, module_id)
#define DNX_INIT_TIME_ANALYZER_ACCESS_STOP(unit, module_id)        utilex_time_analyzer_module_stop(unit, module_id)
#else
#define DNX_INIT_TIME_ANALYZER_ACCESS_START(unit, module_id)
#define DNX_INIT_TIME_ANALYZER_ACCESS_STOP(unit, module_id)
#endif
/** ALGO PORT*/
#if DNX_INIT_TIME_ANALYZER_ALGO_PORT_EN
#define DNX_INIT_TIME_ANALYZER_ALGO_PORT_START(unit, module_id)       utilex_time_analyzer_module_start(unit, module_id)
#define DNX_INIT_TIME_ANALYZER_ALGO_PORT_STOP(unit, module_id)        utilex_time_analyzer_module_stop(unit, module_id)
#else
#define DNX_INIT_TIME_ANALYZER_ALGO_PORT_START(unit, module_id)
#define DNX_INIT_TIME_ANALYZER_ALGO_PORT_STOP(unit, module_id)
#endif
/*
 * error recovery diagnostics require init time analyzer diagnostics
 */
#if DNX_ERROR_RECOVERY_TIME_DIAGNOSTICS
#define DNX_ERROR_RECOVERY_TIME_DIAG_START(unit, module_id)     utilex_time_analyzer_module_start(unit, module_id)
#define DNX_ERROR_RECOVERY_TIME_DIAG_END(unit, module_id)       utilex_time_analyzer_module_stop(unit, module_id)
#else
#define DNX_ERROR_RECOVERY_TIME_DIAG_START(unit, module_id)
#define DNX_ERROR_RECOVERY_TIME_DIAG_END(unit, module_id)
#endif

/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/**
 * \brief - Time Analyzer storage structure
 */

typedef struct
{
    int module_id;
    char *name;
    int enable_flag;
} dnx_init_time_analyzer_operations_t;

/**
 * \brief - all the modules / operation tracked
 * Note: must be synced with the switch case in dnx_init_time_analyzer_module_name_get()
 */
typedef enum
{
    DNX_INIT_TIME_ANALYZER_DBAL_SET,
    DNX_INIT_TIME_ANALYZER_DBAL_GET,
    DNX_INIT_TIME_ANALYZER_DBAL_CLEAR,
    DNX_INIT_TIME_ANALYZER_DBAL_ENTRY_CLEAR,
    DNX_INIT_TIME_ANALYZER_DBAL_ITER,
    DNX_INIT_TIME_ANALYZER_DBAL_FIELD_SET,
    DNX_INIT_TIME_ANALYZER_DBAL_FIELD_GET,
    DNX_INIT_TIME_ANALYZER_DBAL_FIELD_REQUEST,
    DNX_INIT_TIME_ANALYZER_CHECK_2,
    DNX_INIT_TIME_ANALYZER_CHECK_1,
    DNX_INIT_TIME_ANALYZER_ALGO_PORT_BITMAP,
    DNX_INIT_TIME_ANALYZER_DNX_DATA,
    DNX_INIT_TIME_ANALYZER_SW_STATE,
    DNX_INIT_TIME_ANALYZER_RES_MNGR,
    DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR,
    DNX_INIT_TIME_ANALYZER_MEM_INIT,
    DNX_INIT_TIME_ANALYZER_MEM_ZEROS,
    DNX_INIT_TIME_ANALYZER_ER_API_FULL,
    DNX_INIT_TIME_ANALYSER_DBAL_INIT,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT,
    DNX_INIT_TIME_ANALYSER_DBAL_L_DYNAMIC_INIT,
    DNX_INIT_TIME_ANALYSER_DBAL_L_ALLOC_INIT,
    DNX_INIT_TIME_ANALYSER_DBAL_L_GENERIC_INIT,
    DNX_INIT_TIME_ANALYSER_DBAL_L_VALIDATIONS,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_VALID,
    DNX_INIT_TIME_ANALYSER_DBAL_MDB_INIT,
    DNX_INIT_TIME_ANALYSER_DBAL_SW_INIT,
    DNX_INIT_TIME_ANALYSER_DBAL_PEMLA_INIT,

    DNX_INIT_TIME_ANALYSER_DBAL_L_HL_INIT,
    DNX_INIT_TIME_ANALYSER_DBAL_L_SW_STATE_INIT,
    DNX_INIT_TIME_ANALYSER_DBAL_L_TCAMCON_INIT,
    DNX_INIT_TIME_ANALYSER_DBAL_L_PEMLA_INIT,
    DNX_INIT_TIME_ANALYSER_DBAL_L_MDB_INIT,

    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_IF,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_HL,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_HL_RES_TYPE,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_MDB,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_FIELD_STRING_TO_ID,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_TCAM_CS,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_STRUCT_ACCESS_CLEAR,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_STRUCT_CLEAR,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ACCESS_REGISTER_MEMORY,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_GENERAL_INFO,

    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_HL_REG,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_HL_MEM,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_HL_MEM_HW_ENTITY_VALIDATE,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_DNX_DATA_PARSING,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_AG_INIT,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_HL_MDB_INIT,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_AG_INIT_CLEAR_XML_STRUCT,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_AG_INIT_GENERAL_INFO,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_AG_INIT_DB_INTERFACE,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_AG_INIT_APP_TO_PHY_DB,
    DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_AG_INIT_ADD_DBAL_TABLE,

    DNX_INIT_TIME_ANALYZER_NOF
} dnx_init_time_analyzer_module_e;

/**
 * \brief- list (operations ids, operation names, operation enable flag)
 */
#define DNX_INIT_TIME_ANALYZER_OPS_INIT {\
        {DNX_INIT_TIME_ANALYZER_CHECK_1, "GENERIC CHECK 1", DNX_INIT_TIME_ANALYZER_DBAL_EN},\
        {DNX_INIT_TIME_ANALYZER_CHECK_2, "GENERIC CHECK 2", DNX_INIT_TIME_ANALYZER_DBAL_EN},\
        {DNX_INIT_TIME_ANALYZER_DBAL_SET, "DBAL_SET", DNX_INIT_TIME_ANALYZER_DBAL_EN},\
        {DNX_INIT_TIME_ANALYZER_DBAL_GET, "DBAL_GET", DNX_INIT_TIME_ANALYZER_DBAL_EN},\
        {DNX_INIT_TIME_ANALYZER_DBAL_CLEAR, "DBAL_CLEAR", DNX_INIT_TIME_ANALYZER_DBAL_EN},\
        {DNX_INIT_TIME_ANALYZER_DBAL_ENTRY_CLEAR, "DBAL_ENTRY_CLEAR", DNX_INIT_TIME_ANALYZER_DBAL_EN},\
        {DNX_INIT_TIME_ANALYZER_DBAL_ITER, "DBAL_ITER", DNX_INIT_TIME_ANALYZER_DBAL_EN},\
        {DNX_INIT_TIME_ANALYZER_DBAL_FIELD_SET, "DBAL_FIELD_SET", DNX_INIT_TIME_ANALYZER_DBAL_EN},\
        {DNX_INIT_TIME_ANALYZER_DBAL_FIELD_GET, "DBAL_FIELD_GET", DNX_INIT_TIME_ANALYZER_DBAL_EN},\
        {DNX_INIT_TIME_ANALYZER_DBAL_FIELD_REQUEST, "DBAL_FIELD_REQ", DNX_INIT_TIME_ANALYZER_DBAL_EN},\
        {DNX_INIT_TIME_ANALYZER_ALGO_PORT_BITMAP, "PORT_BITMAP", DNX_INIT_TIME_ANALYZER_ALGO_PORT_EN},\
        {DNX_INIT_TIME_ANALYZER_DNX_DATA, "DNX_DATA", DNX_INIT_TIME_ANALYZER_DNX_DATA_EN},\
        {DNX_INIT_TIME_ANALYZER_SW_STATE, "SW STATE", DNX_INIT_TIME_ANALYZER_SW_STATE_EN},\
        {DNX_INIT_TIME_ANALYZER_RES_MNGR, "RES MNGR", DNX_INIT_TIME_ANALYZER_RES_MNGR_EN},\
        {DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR, "TEMP MNGR", DNX_INIT_TIME_ANALYZER_RES_MNGR_EN},\
        {DNX_INIT_TIME_ANALYZER_MEM_INIT, "MEM_INIT", DNX_INIT_TIME_ANALYZER_ACCESS_EN},\
        {DNX_INIT_TIME_ANALYZER_MEM_ZEROS, "MEM_ZEROS", DNX_INIT_TIME_ANALYZER_ACCESS_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_INIT, "DBAL INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT, "DBAL L INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_GENERIC_INIT, "DBAL GENERIC INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_ALLOC_INIT, "DBAL ALLOC INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_DYNAMIC_INIT, "DBAL DYNAMIC INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_VALIDATIONS, "DBAL INIT VALIDATIONS", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_VALID, "DBAL L INIT VALID", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_MDB_INIT, "DBAL MDB INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_SW_INIT, "DBAL SW INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_PEMLA_INIT, "DBAL PEMLA INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_HL_INIT, "DBAL L HL INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_SW_STATE_INIT, "DBAL L SWSTATE INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_TCAMCON_INIT, "DBAL L TCAMCON INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_PEMLA_INIT, "DBAL L PEMLA INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_MDB_INIT, "DBAL L MDB INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_IF, "DBAL ADD TABLE IF INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_HL, "DBAL ADD TABLE MAP HL INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_MDB, "DBAL ADD TABLE MAP MDB INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_FIELD_STRING_TO_ID, "DBAL FIELD STRING TO ID", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_TCAM_CS, "DBAL ADD TABLE MAP TCAM CS INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_HL_RES_TYPE, "DBAL ADD TABLE MAP HL RES TYPE", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ACCESS_REGISTER_MEMORY, "DBAL ACCESS REGISTER MEMORY", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_STRUCT_ACCESS_CLEAR, "DBAL STRUCT ACCESS CLEAR", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_STRUCT_CLEAR, "DBAL STRUCT CLEAR", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_GENERAL_INFO, "DBAL GENERAL INFO SET", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_HL_REG, "DBAL ADD TABLE MAP HL REG INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_HL_MEM, "DBAL ADD TABLE MAP HL MEM INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_HL_MEM_HW_ENTITY_VALIDATE, "DBAL ADD TABLE MAP HL MEM HW INIT ENTITY VALIDATE", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_DNX_DATA_PARSING, "DBAL DNX DATA PARSING INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_HL_MDB_INIT, "DBAL MDB HL INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_AG_INIT, "DBAL AG INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_AG_INIT_CLEAR_XML_STRUCT, "DBAL AG CLEAR XML STRUCT INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_AG_INIT_GENERAL_INFO, "DBAL AG GENERAL INFO INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_AG_INIT_DB_INTERFACE, "DBAL AG DB INTERFACE INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_AG_INIT_APP_TO_PHY_DB, "DBAL AG APP TO PHY DB INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        {DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_AG_INIT_ADD_DBAL_TABLE, "DBAL AG ADD DBAL TABLE INIT", DNX_INIT_TIME_ANALYZER_DBAL_INIT_EN},\
        }
/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */
/**
 * \brief - allocate memory, initialize time analyzer tool and adds the required modules to be tracked
 * \param [in] unit - unit #.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_time_analyzer_init(
    int unit);
/**
 * \brief - free time analyzer tool resources
 * \param [in] unit - unit #.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_time_analyzer_deinit(
    int unit);
/**
 * \brief - flag cb for init sequence that allows to skip the module
 * \param [in] unit - unit #.
 * * \param [out] flags - init sequnce flags
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_time_analyzer_flag_cb(
    int unit,
    int *flags);
/*
 * }
 */

#endif /* DNX_INIT_TIME_ANALYZER_H_INCLUDED */
