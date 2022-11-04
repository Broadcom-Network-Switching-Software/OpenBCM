/** \file appl_ref_lsm.h
 *    lsm(live switch monitor) tool is a CPU debug tool for dropped packets or in the
 *    future for any unexpected issues that might happen.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
 *     when no event happend last event time should be invalid value
 */
#define LSM_NO_LAST_EVENT -1

/**
 * \brief
 *     delay time between LSM events
 */
#define LSM_TIME_DELAY 1

/**
 * \brief
 *     table ID of packet event
 */
#define LSM_PACKET_EVENT_TABLE_NUM 0

/**
 * \brief
 *     table ID of packet event
 */
#define LSM_INTERRUPT_EVENT_TABLE_NUM 1
/**
 * \brief
 *     the max size of the string representation of lsm operator enum e.g. 'eq', 'ne',...
 */
#define LSM_OPERATOR_STRING_SIZE 3
/**
 * \brief
 *     exact number of attributes in lsm_register_qualifier_command_t.
 */
#define LSM_REG_NOF_ATTRS 2
/**
 * \brief
 *     max char size of register name in lsm_register_qualifier_command_t including ending NULL.
 */
#define LSM_REG_NAME_STRING_SIZE 32
/**
 * \brief
 *     max char size of register's field name in lsm_register_qualifier_command_t including ending NULL.
 */
#define LSM_REG_FIELD_NAME_STRING_SIZE 32
/**
 * \brief
 *     exact number of attributes in lsm_signal_qualifier_command_t.
 */
#define LSM_SIG_NOF_ATTRS 4
/**
 * \brief
 *     max char size of signal_from field in lsm_signal_qualifier_command_t including ending NULL.
 */
#define LSM_SIG_FROM_STRING_SIZE 8
/**
 * \brief
 *     max char size of signal_to field in lsm_signal_qualifier_command_t including ending NULL.
 */
#define LSM_SIG_TO_STRING_SIZE 8
/**
 * \brief
 *     max char size of signal_block field in lsm_signal_qualifier_command_t including ending NULL.
 */
#define LSM_SIG_BLOCK_STRING_SIZE 8
/**
 * \brief
 *     max char size of signal_name field in lsm_signal_qualifier_command_t including ending NULL.
 */
#define LSM_SIG_NAME_STRING_SIZE 32
/**
 * \brief
 *     max char size of qualifier name string including ending NULL.
 */
#define LSM_MAX_CHAR_QUAL_NAME 32
/**
 * \brief
 *     max number of attribute in xml tag
 */
#define LSM_MAX_TAG_ATTR 10
/**
 * \brief
 *     max action priority value
 */
#define LSM_MAX_ACTION_PRIO 255
/**
 * \brief
 *     max number of traps to configure
 */
#define LSM_MAX_NOF_TRAPS 128
/**
 * \brief
 *     max number of interrupts to configure
 */
#define LSM_MAX_NOF_INTERRUPTS 128
/**
 * \brief
 *  lsm trap strentgh
 */
#define LSM_TRAP_STRENTGH 8
/**
 * \brief
 *  max char size of action object string
 */
#define LSM_MAX_ACTION_BUFFER 128
/**
 * \brief
 *  max number of action tables
 */
#define LSM_MAX_NOF_ACTION_TABLES 10
/**
 * \brief
 *  max char size of defualt strings in lsm tool
 */
#define LSM_MAX_SIZE_STR 64
/**
 * \brief
 *  max char size of file path string
 */
#define MAX_SIZE_PATH 100
/**
 * \brief
 *  max number of qualifier elements in a qualifier object
 */
#define LSM_MAX_NOF_QUAL 10
/**
 * \brief
 *  max number of different qualifier types in a qualifier fields database
 */
#define LSM_MAX_NOF_QUAL_TYPES 32
/**
 * \brief
 *  max number of action elements in an action object
 */
#define LSM_MAX_NOF_ACTIONS 10
/**
 * \brief
 *  max number of qualifier entries in the qualifier table. Also max number of action tables
 */
#define LSM_MAX_NOF_QUAL_ENTRIES 16
/**
 * \brief
 *  max number of action entries in an action table
 */
#define LSM_MAX_NOF_ACTION_ENTRIES 1024
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
#define LSM_CONFIG_XML_PATH "lsm_config.xml"
/**
 * \brief
 *  max number chars in "lsm show action" diagnostic in the key column
 */
#define LSM_SHOW_ACTIONS_KEY_SIZE 50
/**
 * \brief
 *  max number chars in "lsm show action" diagnostic in the key column
 */
#define LSM_SHOW_ACTIONS_ACTION_SIZE 52


/*
 * }
 */

/*
 * \brief
 *  Opaque handle to a action command.
 */
typedef char lsm_action_buffer_t[LSM_MAX_ACTION_BUFFER];


/**
 * \brief signifies different types of actions.
 */
typedef enum
{
    /**
     * signifies an invalid action.
     */
    lsm_action_invalid = -1,
    /**
     * signifies first acrion enum.
     */
    lsm_action_first = 0,
    /**
     * an lsm action which runs a specific diagnostic bcm command.
     */
    lsm_action_diag = lsm_action_first,
    /**
     * an lsm action which prints a buffer.
     */
    lsm_action_print,
    /*
     * Last member, not a usable value.
     */
    lsm_action_nof
} lsm_action_e;

/**
 * \brief lsm event types, the reason the tool was triggered.
 */
typedef enum
{
    /**
     * signifies an invalid event.
     */
    lsm_event_invalid = -1,
    /**
     * signifies first event enum.
     */
    lsm_event_first = 0,
    /**
     * signifies a dropped packet event.
     */
    lsm_event_packet = lsm_event_first,
    /**
     * signifies an interrupt event.
     */
    lsm_event_interrupt,
    /**
     * Last member, not a usable value.
     */
    lsm_event_nof
} lsm_event_e;

/**
 * \brief signifies different types of qualifiers. LSM uses qualifiers to determine conditions.
 *          for example using a signal qualifier we can do some action in case some signal's value equals 2.
 */
typedef enum
{
    /**
     * signifies an invalid qualifier.
     */
    lsm_qualifier_invalid = -1,
    /**
     * signifies first qualifier enum.
     */
    lsm_qualifier_first = 0,
    /**
     * an lsm qualifier represent a signal in the device
     */
    lsm_qualifier_signal = lsm_qualifier_first,
    /**
     * an lsm qualifier represent  a register in the device
     */
    lsm_qualifier_register = 1,
    /**
     * an lsm qualifier represent an interrupt id in the device
     */
    lsm_qualifier_interrupt = 2,
    /*
     * Last member, not a usable value.
     */
    lsm_qualifier_nof
} lsm_qualifier_e;

/**
 * \brief signifies different types of operators supported for usage in LSM qualifier lookups.
 *          for example using a "lt" (<) operator lookup will return a "hit" when looked up value is lower
 *          than database value.
 */
typedef enum
{
    /**
     * signifies an invalid operator.
     */
    lsm_operator_invalid = -1,
    /**
     * signifies first lsm operator enum.
     */
    lsm_operator_first = 0,
    /**
     * an lsm operator for equals ==
     */
    lsm_operator_equals = lsm_operator_first,
    /**
     * an lsm operator for not equals !=
     */
    lsm_operator_not_equals = 1,
    /**
     * an lsm operator for lower than <
     */
    lsm_operator_lower = 2,
    /**
     * an lsm operator for greater than >
     */
    lsm_operator_greater = 3,
    /**
     * an lsm operator for lower than and equals <=
     */
    lsm_operator_lower_equals = 4,
    /**
     * an lsm operator for greater than and equals >=
     */
    lsm_operator_greater_equals = 5,
    /*
     * Last member, not a usable value.
     */
    lsm_operator_nof
} lsm_operator_e;

/**
 * A structure used in populating an array for mapping string name of lsm_qualifier_e to it's numeric value.
 */
typedef struct lsm_qualifier_enum_string {
    /**
     * the string representation (name) of an lsm qualifier type
     */
    char * lsm_qual_string;
    /**
     * the enum representation (name) of an lsm qualifier type
     */
    lsm_qualifier_e lsm_qual_enum;
    /**
     * for each lsm qualifier type the lsm parser will expect a number of specific tag attributes,
     * this list of strings will contain these tag names.
     */
    char * attrributes[LSM_MAX_TAG_ATTR];
    /**
     * for each lsm qualifier type the lsm parser will expect a this exact number of tag attributes.
     */
    uint32 nof_attrs;
    /**
     * for each lsm qualifier type the lsm parser will expect a number of specific tag attributes,
     * this list is the size limit of each of these tag attributes.
     */
    uint32 attr_value_limit[LSM_MAX_TAG_ATTR];
} lsm_signal_qualifier_enum_string_t;

/**
 * A structure used in populating an array for mapping string name of lsm_operator_e to it's numeric value.
 */
typedef struct lsm_operator_enum_string {
    char * string;
    lsm_operator_e lsm_enum;
} lsm_operator_enum_string_t;

/**
 * \brief structure containing arguments used to retrieve qualifier values of type signal.
 *          see lsm_qualifier_e for qualifier types.
 */
typedef struct
{
    /**
     * a signal's name
     */
    char signal_name[LSM_SIG_NAME_STRING_SIZE];
    /**
     * the source stage of a signal
     */
    char signal_from[LSM_SIG_FROM_STRING_SIZE];
    /**
     * the destinataion stagr of a signal
     */
    char signal_to[LSM_SIG_TO_STRING_SIZE];
    /**
     * the block a signal is generated in
     */
    char signal_block[LSM_SIG_BLOCK_STRING_SIZE];
} lsm_signal_qualifier_command_t;

/**
 * \brief structure containing arguments used to retrieve qualifier values of type register.
 *          see lsm_qualifier_e for qualifier types.
 */
typedef struct
{
    /**
     * the register name
     */
    char register_name[LSM_REG_NAME_STRING_SIZE];
    /**
     * the register's field name
     */
    char field_name[LSM_REG_FIELD_NAME_STRING_SIZE];
} lsm_register_qualifier_command_t;

/**
 * \brief union containing structs holding needed info for retrieving qualifier values.
 * it is used order to abstract the qualifier command of different qualifier types. when alocating
 * memory for qualifier command (also called qulifier fields) it will assign max size from union staucts and
 * app will write the relevant data in the allocated space.
 */
typedef union lsm_qualifer_command {
    /**
     * struct holding needed info for retriving signal qualifier values.
     */
    lsm_signal_qualifier_command_t sig;
    /**
     * struct holding needed info for retriving register qualifier values.
     */
    lsm_register_qualifier_command_t reg;
} lsm_qualifier_command_t;

/**
 * \brief structure that holds information regarding a single qualifer
 */
typedef struct
{
    /**
     * qualifier name
     */
    char name[LSM_MAX_SIZE_STR];
    /**
     * a structure containing information on how collect qualifier values
     */
    lsm_qualifier_command_t qualifier_command;
    /**
     * the type of qualifier
     */
    lsm_qualifier_e qualifier_type;
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
    /**
     * the operator which will determine how to compare the database value to lookup value
     */
    lsm_operator_e lookup_operator;
} lsm_qualifier_info_t;


/**
 * \brief a list of qualifiers which signify a single qualifier entry
 */
typedef struct
{
    /**
     * a list of qualifiers
     */
    lsm_qualifier_info_t list_of_qualifiers[LSM_MAX_NOF_QUAL];
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
     * action type.
     */
    lsm_action_e action_type;
    /**
     * diag action, executes a bcm diag.
     */
    lsm_action_buffer_t action;
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
    lsm_action_results_t list_of_action_results[LSM_MAX_NOF_ACTIONS];
    /**
     * the number of actions in the action list.
     */
    uint32 nof_actions;
    /**
     * priopity of current entry.
     */
    uint32 priority;
} lsm_actions_entry_t;

/**
 * \brief a struct used to simulate an event that trigers the app.
 */
typedef struct
{
    /**
     * lsm event type
     */
    lsm_event_e event;
    /**
     * if packet event, the hex of the packet that triggers the event.
     */
    bcm_pkt_t *pkt;

    /**
     * if packet event, indication not to resend packet.
     * useful if user wants to trigger packet event but no need to resend packet
     * This will mean that LSM logic will work on last signals saved in device
     * user is responsible to send packet if needed
     */
    uint8 no_resend_packet_enable;

    /**
     * core id to collect information
     */
    uint8 core_id;

    /**
     * if interrupt event, this is the interrupt ID that triggers the event.
     */
    uint32 interrupt_id;

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
    /**
     * bool variable that signifies that rx callback should be disabled.
     */
    uint8 disable_rx_cb;
    /**
     * bool variable that signifies that interrupt callback should be disabled.
     */
    uint8 disable_interrupt_callback;
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
 * \brief struct to hold state of interrupts prior to app initialization in order to restore
 * configuration on deinit.
 */
typedef struct
{
    /**
     * interrupt ID
     */
	int interrupt_id;
	/**
     * interrupt name
     */
    char name[LSM_MAX_SIZE_STR];

    /**
     * status of bcmSwitchEventCorrActOverride configuration
     * 0 - SW callback only
     * 1 - SDK and user callback
     * 2 - user callback only
     */
    uint32 interrupt_corract_override_status;
     /**
     * status of bcmSwitchEventMask configuration
     */
    uint32 interrupt_masks_status;

} lsm_interrupt_t;

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
 * Following is a list of typedefs used in the appl_ref_lsm_db.c module to store needed information.
 */
/**
 * \brief a list of qualifiers which signify a single qualifier entry
 */
typedef struct
{
    /**
     * a list of qualifier fields pointers
     */
    lsm_qualifiers_fields_t *qualifiers[LSM_MAX_NOF_QUAL];
    /**
     * the number of qualifiers in the list
     */
    uint32 nof_qualifiers;
} lsm_db_qualifiers_entry_t;

/**
 * \brief structure that holds information regarding a single qualifer and its value
 */
typedef struct
{
    /**
     * a single qualifier's fields information
     */
    lsm_qualifiers_fields_t *qualifer_field;
    /**
     * the value of a single qualifier
     */
    int qualifier_value;
    /**
     * the operator which will determine how to compare the database value to lookup value
     */
    lsm_operator_e lookup_operator;
    /**
     * the mask of a single qualifier
     */
    uint32 qualifier_mask;
} lsm_db_qualifier_info_t;

/**
 * \brief The qualifier table (only one) is a list of all qualifiers entries.
 *  It is allocated on init.
 */
typedef struct
{
    /**
     * a list of qualifiers entries.
     */
    lsm_db_qualifiers_entry_t qualifier_entries[LSM_MAX_NOF_QUAL_ENTRIES];
    /**
     * the number of qualifier entries currently in the table.
     */
    uint32 nof_entries;
} lsm_db_qualifier_table_t;

/**
 * \brief an action entry is a mapping between a list of qualifiers and a list of actions.
 */
typedef struct
{
    /**
     * a list of qualifier elements and thier values.
     */
    lsm_db_qualifier_info_t qualifiers[LSM_MAX_NOF_QUAL];
    /**
     * the number of qualifiers in the qualifier list.
     */
    uint32 nof_qualifiers;
    /**
     * a list of action to executes in case of hit on all qualifiers in the list.
     */
    lsm_action_results_t list_of_action_results[LSM_MAX_NOF_ACTIONS];
    /**
     * the number of actions in the action list.
     */
    uint32 nof_actions;
    /**
     * priopity of current entry.
     */
    uint8 priority;
} lsm_db_actions_entry_t;

/**
 * \brief An action table is a list of action entries (see lsm_db_actions_entry_t).
 *  The LSM tool uses this tructure to map a list of qulifiers to a set of action to take.
 *  LSM_MAX_NOF_QUAL actions are allocated on init.
 */
typedef struct
{
    /**
     * a pointer t a list of action entries.
     */
    lsm_db_actions_entry_t *actions_entries;
    /**
     * the number of action entries currently in the table.
     */
    uint32 nof_entries;
} lsm_db_action_table_t;


/**
 * \brief - get the string representation of an lsm_operator_e enum
 *
 * \param [in] unit - Unit ID
 * \param [in] op_enum - an enum value to get string reperesentation of.
 * \param [out] op_string - string representation of recived op_enum.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e appl_ref_lsm_init_operator_enum_string_get(
    int unit,
    lsm_operator_e op_enum,
    char * op_string);
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
 *   diagnostics commands are verified with autogenerated ctests that run the command and verify that it
 *   does not create memory leaks or errors. in order to run some of the lsm diag commands we first
 *   need to initialize the tool itself. for that the autogenerated ctest get recieve a cb function to run prior
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
 * \brief - create the traps database, allocates needed memory. The trap table is static in size from creation until
 *          destruction. Updating the value in the table is possible with appl_ref_lsm_db_trap_table_update().
 *
 * \param [in] unit - Unit ID
 * \param [in] nof_traps - number of elements in traps below
 * \param [in] traps - trap list to insert into database
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None.
 * \see
 *   appl_ref_lsm_db_trap_table_update()
 */
shr_error_e
appl_ref_lsm_db_trap_table_create(
    int unit,
    uint32 nof_traps,
    lsm_init_trap_t * traps);
/**
 * \brief - create the interrupts database, allocates Lsm_interrupts dynamic memory. The trap table is static in size from creation until
 *          destruction. Updating the value in the table is possible with appl_ref_lsm_db_interrupt_table_update().
 *
 * \param [in] unit - Unit ID
 * \param [in] nof_interrupt - number of elements in interrupts below
 * \param [in] interrupt - interrupt list to insert into database
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   no verify on interrupt ID in this function.
 * \see
 *   appl_ref_lsm_db_interrupt_table_update()
 */
shr_error_e
appl_ref_lsm_db_interrupt_table_create(
    int unit,
    uint32 nof_interrupts,
	lsm_interrupt_t * interrupts);


/**
 * \brief - update the traps database, the number of traps in table is detemined in appl_ref_lsm_db_trap_table_create(),
 *          any traps in list after that number will be ignored.
 *
 * \param [in] unit - Unit ID
 * \param [in] nof_traps - number of elements in traps below
 * \param [in] traps - trap list to insert into database
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None.
 * \see
 *   appl_ref_lsm_db_trap_table_create()
 */
shr_error_e
appl_ref_lsm_db_trap_table_update(
    int unit,
    uint32 nof_traps,
    lsm_init_trap_t * traps);


/**
 * \brief - update all contects in the interrupt database, the number of interrupt in table is detemined in appl_ref_lsm_db_interrupt_table_create(),
 *          any interrupts in list after that number will be ignored.
 *
 * \param [in] unit - Unit ID
 * \param [in] nof_interrupts - number of elements in interrupt below
 * \param [in] interrupt - interrupt list to insert into database
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None.
 * \see
 *   appl_ref_lsm_db_trap_table_create()
 */
shr_error_e
appl_ref_lsm_db_interrupt_table_update(
    int unit,
    uint32 nof_interrupts,
    lsm_interrupt_t * interrept);

/**
 * \brief - get number of traps in table.
 *
 * \param [in] unit - Unit ID
 * \param [out] traps - number of traps in table
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_ref_lsm_db_nof_traps_get(
    int unit,
    uint32 * nof_traps);


/**
 * \brief - get number of interrupt in table.
 *
 * \param [in] unit - Unit ID
 * \param [out] nof_interrupts - number of interrupt in table
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_ref_lsm_db_nof_interrupts_get(
    int unit,
    uint32 * nof_interrupts);

/**
 * \brief - get copy of trap table.
 *
 * \param [in] unit - Unit ID
 *  param [in] nof_traps - number of elements in traps below
 * \param [out] traps - copy of traps table
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_ref_lsm_db_trap_table_get(
    int unit,
	uint32 *nof_traps,
    lsm_init_trap_t * traps);


/**
 * \brief - get copy of interrupt table.
 *
 * \param [in] unit - Unit ID
 *  param [in] nof_interrupts - number of elements in interrupts below
 * \param [out] interrupt - copy of interrupts table
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_ref_lsm_db_interrupt_table_get(
    int unit,
	uint32 *nof_interrupts,
    lsm_interrupt_t * interrupts);


/**
 * \brief - destroy the trap table in database
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
shr_error_e
appl_ref_lsm_db_trap_table_destroy(
    int unit);

/**
 * \brief - destroy the interrupt table in database
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
shr_error_e
appl_ref_lsm_db_interrupt_table_destroy(
    int unit);



/**
 * \brief - create the qualifier database, allocates needed memory for qualifier table.
 *
 * \param [in] unit - Unit ID
 * \param [in] nof_qualifiers - allocate's memory according to provided value
 *
 * \return
 *   shr_error_e

 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_db_qualifiers_table_create(
    int unit,
    uint32 nof_qualifiers);

/**
 * \brief - destroy the qalifier database
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   The database is being stored on host CPU's RAM.
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_db_qualifiers_table_destroy(
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
shr_error_e appl_ref_lsm_db_qualifiers_info_add(
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
 * \brief - verifies and allocates needed memory for nof_action_tables tables.
 *
 * \param [in] unit - Unit ID
 * \param [in] nof_action_tables - number of action tables to allocate
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_action_tables_create(
    int unit,
    uint32 nof_action_tables);

/**
 * \brief - verifies and allocates needed memory for one action table with nof_action_entries entrise.
 *
 * \param [in] unit - Unit ID
 * \param [in] action_table_id - action table id to be created.
 * \param [in] nof_action_entries - number of action entries to allocate for given action table
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
    uint32 action_table_id,
    uint32 nof_action_entries);

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
 * \brief - find a specific qualifier's attributes bt name.
 *
 * \param [in] unit - Unit ID
 * \param [in] qualifier_name - name of qualifier to find.
 * \param [out] qualifier_info - qualifier_info returned if found.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_ref_lsm_db_qualifiers_info_get(
    int unit,
    char * qualifier_name,
    lsm_qualifiers_fields_t * qualifier_info);

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
shr_error_e appl_ref_lsm_show_actions(
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
shr_error_e appl_ref_lsm_show_traps(
    int unit);

/**
 * \brief - print table showing all configured interrupts (only interrupts names).
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
shr_error_e appl_ref_lsm_show_interrupts(
    int unit);
