/** \file appl_ref_lsm.h
 *    lsm(live switch monitor) tool is a CPU debug tool for dropped packets or in the
 *    future for any unexpected issues that might happen.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 * 
 */

/*
* Include files.
* {
*/
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/pkt.h>
#include <shared/error.h>
#include <soc/sand/sand_signals.h>
#include <bcm/rx.h>

/*
 * }
 */

/*
* globals.
* {
*/

/*
 * }
 */

/*
* Defines.
* {
*/

/**
 * \brief
 *     max char size of qualifier name string
 */
#define MAX_SIZE_QAUL_NAME 300
/**
 * \brief
 *     max number of attribute in xml tag
 */
#define MAX_TAG_ATTR 2
/**
 * \brief
 max char size of print object string
 */
#define MAX_SIZE_PRINT 300
/**
 * \brief
 *  max char size of diag object string
 */
#define MAX_SIZE_DIAG 300
/**
 * \brief
 *  max char size of defualt strings in lsm tool
 */
#define MAX_SIZE_STR 30
/**
 * \brief
 *  max char size of file path string
 */
#define MAX_SIZE_PATH 100
/**
 * \brief
 *  max char size of command object string
 */
#define MAX_SIZE_CMD 150
/**
 * \brief
 *  max number of qualifier elements in a qualifier object
 */
#define MAX_NOF_QUAL 10
/**
 * \brief
 *  max number of action elements in an action object
 */
#define MAX_NOF_ACTIONS 10
/**
 * \brief
 *  max number of qualifier entries in the qualifier table
 */
#define MAX_NOF_QUAL_ENTRIES 10
/**
 * \brief
 *  max number of action entries in an action table
 */
#define MAX_NOF_ACTIONS_ENTRIES 10
/**
 * \brief
 *  max number of nibles in packet header
 */
#define MAX_PKT_HDR_SIZE 144
/**
 * \brief
 *  max number of ports
 */
#define MAX_NOF_PORTS 256
/**
 * \brief
 *  name of expected actions xml file
 */
#define LSM_ACTIONS_XML_PATH "lsm_actions.xml"
/**
 * \brief
 *  name of expected actions xml file
 */
#define LSM_QUALIFIERS_XML_PATH "lsm_qualifiers.xml"

/*
 * }
 */

/*
 * \brief
 *  Opaque handle to a print action command.
 */
typedef char print_info[MAX_SIZE_PRINT];
 /*
  * \brief
  *  Opaque handle to a run bcm diagnostic action command.
  */
typedef char bcm_diags[MAX_SIZE_DIAG];

/**
 * \brief structure containing arguments used to retrieve qualifier values.
 */
typedef struct
{
    /**
     * the source stage of a signal
     */
    char signal_from[MAX_SIZE_STR];
    /**
     * the destinataion stagr of a signal
     */
    char signal_to[MAX_SIZE_STR];
    /**
     * the block a signal is generated in
     */
    char signal_block[MAX_SIZE_STR];
    /**
     * a signal's name
     */
    char signal_name[MAX_SIZE_STR];
} lsm_qualifier_command_t;

/**
 * \brief signifies different types of events.
 */
typedef enum
{
    /**
     * a lsm event cuased by specific signal
     */
    lsm_signal = 0,
    /*
     * Last member, not a usable value.
     */
    lsm_nof
} lsm_qualifier_e;

/**
 * \brief structure that holds information regarding a single qualifer
 */
typedef struct
{
    /**
     * qualifier name
     */
    char name[MAX_SIZE_STR];
    /**
     * a structure containing information on how collect qualifier values
     */
    lsm_qualifier_command_t qualifier_command;
    /**
     * the type of qualifier
     */
    lsm_qualifier_e qualifier_type;
    /**
     * the size the qulifier value in bytes
     */
    uint32 qual_size;
} lsm_qualifiers_fields_t;

/**
 * \brief structure that holds information regarding a single qualifer and its value
 */
typedef struct
{
    /**
     * a single qualifier's fields information
     */
    lsm_qualifiers_fields_t qualifer_field;
    /**
     * the value of a single qualifier
     */
    uint32 qualifier_value;
    /**
     * the mask of a single qualifier
     */
    uint32 qualifier_mask;
} lsm_qualifier_info_t;

/**
 * \brief a list of qualifiers which signify a single qualifier entry
 */
typedef struct
{
    /**
     * a list of qualifiers
     */
    lsm_qualifier_info_t list_of_qualifiers[MAX_NOF_QUAL];
    /**
     * the number of qualifiers in the list
     */
    uint32 nof_qualifiers;
    /**
     * the list of qualifier's entry id (index) inside the qualifier table.
     */
    uint32 entry_id;
} lsm_qualifiers_entry_t;

/**
 * \brief a structure the contains the possible actions to be taken in case there is a hit on a qualifier.
 */
typedef struct
{
    /**
     * print action.
     */
    print_info prt_info;
    /**
     * diag action, executes a bcm diag.
     */
    bcm_diags diags;
} lsm_action_results_t;

/**
 * \brief an action entry is a mapping between a list of qualifiers and a list of actions.
 */
typedef struct
{
    /**
     * a list of qualifiers and thier values.
     */
    lsm_qualifiers_entry_t qualifiers;
    /**
     * a list of action to executes in case of hit on all qualifiers in the list.
     */
    lsm_action_results_t list_of_action_results[MAX_NOF_ACTIONS];
    /**
     * the number of actions in the action list.
     */
    uint32 nof_actions;
    /**
     * priopity of current entry.
     */
    int priority_id;
} lsm_actions_entry_t;

/**
 * \brief lsm event types, the reason the tool was triggered.
 */
typedef enum
{
    /**
     * signifies an invalid event.
     */
    lsm_invalid_event = 0,
    /**
     * signifies a dropped packet event.
     */
    lsm_packet_event = 1
} lsm_event_type_e;

/**
 * \brief a struct used to simulate an event that trigers the app.
 */
typedef struct
{
    /**
     * lsm event type
     */
    lsm_event_type_e event;
    /**
     * if packet event, the hex of the packet that trigers the app.
     */
    bcm_pkt_t *pkt;
} lsm_event_info_t;

/**
 * \brief information used in init of app
 */
typedef struct
{
    /**
     * the CPU port to resend packets from.
     */
    bcm_port_t port;
    /**
     * a path to look in for xml configure files
     */
    char *xml_dir;
    /**
     * the path for where to create log files in.
     */
    char *log_dir;
    /**
     * bool variable that signifies that logging should be disabled.
     */
    uint8 disable_log;
} lsm_init_info_t;

/**
 * \brief struct to hold state of traps prior to app initialization in order to restore
 * configuration on deinit.
 */
typedef struct
{
    /**
     * trap type.
     */
    bcm_rx_trap_t trap_type;
    /**
     * port the trap is configured on.
     */
    bcm_port_t port;
    /**
     * bool value to indicate if lsm app created trap type
     * configuration are modified the existing configuration.
     */
    uint8 is_new;
} lsm_init_trap_t;

/**
 * \brief struct to hold visibility state of port prior to app initialization in order to restore
 * configuration on deinit.
 */
typedef struct
{
    /**
     * bool value to indicate weather force visability enabled.
     */
    uint8 is_force_enable;
    /**
     * bool value to indicate weather visability enabled.
     */
    uint8 is_vis_enable;
} lsm_init_vis_t;

/**
 * \brief - check whether the app was initialized.
 *
 * \param [in] unit - Unit ID
 * \param [out] is_initizilazed - was lsm tool initialized
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_is_init(
    int unit,
    uint8 *is_initizilazed);

/**
 * \brief - initialize the application.
 *
 * \param [in] unit - Unit ID
 * \param [in] init_info - set of paramateres to configure how to initialize the app.
 *         port - the CPU port to resend packets from.
 *         xml_dir - a path to look in for xml configure files.
 *         log_dir - the path for where to create log files in.
 *         disable_log - bool variable that signifies that logging should be disabled.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_init(
    int unit,
    lsm_init_info_t * init_info);

/**
 * \brief - initialize the application with no params, used as callback for diagnostic testings.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   diagnostics commands are verified with autogenerated ctests that run the command and verify
 *   does not create memory leaks or errors. it order to run some of the lsm diag commands we first
 *   need to initialize the tool itself. for that the autogenerated ctest get recive a cb function to run prior
 *   to testing the diag command. The following function is used to align the init of the lsm tool to that cb
 *   function.
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_void_init(
    int unit);

/**
 * \brief - deinitialize the application.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_deinit(
    int unit);

/**
 * \brief - simulate an event according to a set of params.
 *
 * \param [in] unit - Unit ID
 * \param [in] info - a set of params to simulate an application event:
 *                  event - lsm event type
 *                  pkt - if packet event, the hex of the packet that trigers the app.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_run(
    int unit,
    lsm_event_info_t * info);

/**
 * \brief - create the qualifier database, allocates needed memory for qualifier table.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   May return SHR_E_MEMORY error.
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_qualifiers_db_table_create(
    int unit);

/**
 * \brief - destroy th qalifier database
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_qualifiers_db_table_destroy(
    int unit);

/**
 * \brief - update the qualifier entry of a specific action table
 *
 * \param [in] unit - Unit ID
 * \param [in] entry_id - qualifier entry id/action table id
 *
 * \param [in] qualifier - an entry to add to qualifier database
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_qualifiers_db_entry_update(
    int unit,
    uint32 entry_id,
    lsm_qualifiers_entry_t * qualifier);

/**
 * \brief - add a list of qualifier specific information to qualifier database.
 *
 * \param [in] unit - Unit ID
 * \param [in] nof_qual_info - number of elements in the bellow list
 * \param [in] qualifier - a list of elements containing qulifier specific information
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_qualifiers_db_info_add(
    int unit,
    int nof_qual_info,
    lsm_qualifiers_fields_t * qualifier);

/**
 * \brief - get qulifier information accoding to qualifier entry index. Qualifier index is configured
 *          by the user in the action.xml file in the <action_table id="0"> tag.
 *
 * \param [in] unit - Unit ID
 * \param [in] entry_id - qualifier entry id
 * \param [out] qualifier - requestes qualifier information.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_qualifiers_db_entry_get(
    int unit,
    uint32 entry_id,
    lsm_qualifiers_entry_t * qualifier);

/**
 * \brief - allocates memory for MAX_NOF_QUAL of action tables.
 *
 * \param [in] unit - Unit ID
 * \param [in] action_table_id - action table id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_action_table_create(
    int unit,
    uint32 action_table_id);

/**
 * \brief - destroy all action tables.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_action_table_destroy_all(
    int unit);

/**
 * \brief - add an action entry to a specific action table
 *
 * \param [in] unit - Unit ID
 * \param [in] entry_id - action table id
 * \param [in] action - action to add to action table.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_action_table_entry_add(
    int unit,
    uint32 entry_id,
    lsm_actions_entry_t * action);

/**
 * \brief - find a qulifier match in a specific action table.
 *
 * \param [in] unit - Unit ID
 * \param [in] entry_id - action table id
 * \param [in/out] qualifier - qualifier to find.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_action_table_find_match(
    int unit,
    uint32 entry_id,
    lsm_actions_entry_t * qualifier);

/**
 * \brief - print table showing all qualifiers.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_show_qualifiers(
    int unit);

/**
 * \brief - print table showing all action of a specific action table.
 *
 * \param [in] unit - Unit ID
 * \param [in] entry_id - action table id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
void appl_ref_lsm_show_actions(
    int unit,
    uint32 entry_id);

/**
 * \brief - print table showing all configured traps (only trap names).
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
void appl_ref_lsm_show_traps(
    int unit);
