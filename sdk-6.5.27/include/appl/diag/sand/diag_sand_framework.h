/**
 * \file diag_sand_framework.h
 *
 * Framework utilities, structures and definitions
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_SAND_FRAMEWORK_H_INCLUDED
#define DIAG_SAND_FRAMEWORK_H_INCLUDED

#include <bcm/types.h>
#include <sal/types.h>
#include <sal/appl/field_types.h>
#include <shared/utilex/utilex_prime_flags.h>
#include <shared/utilex/utilex_rhlist.h>
#include <appl/diag/parse.h>
#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_utils.h>

/* Command flags */
/*
 * This flag represents the default flag where no flags are provided.
 * The reason the value is 1 and not 0 is because these flags are prime flags.
 * Therefore flag operations are multiplication and division instead of bitwise AND and OR.
 * Because of this, 1 is the neutral value (doesn't affect existing values), not 0.
 * That being said, the prime flags APIs are designed to handle both 0 and 1 as neutral (no flag) values.
 * No flag means that test will be included in general (not flag-specific) regression sessions
 */
#define SH_CMD_FLAGS_NONE                        1

/**
 * By default, ctests are part of the post-submission regression, unless they have the CTEST_WEEKEND or CTEST_RANDOM flags.
 * 1. If a static test invocation list exists - per test flags take precedence, command flags are not relevant
 * 2. IF there is only a default test - command flag is relevant
 * 3. For dynamic lists the check is performed to assign individual flags to each generated test using CTEST_PASS in the command flags and appropriate flags per test
 * This flag precedence paradigm is valid for all ctest related flags
 * Precommit list is hard-coded and cannot be modified without first consulting with the DevOps team's manager for these sessions
 */
/**
 * Weekend list is generated always by using boolean argument weekend on the command line for "export"
 * This weekend argument is used for creating a list of tests the will run in regression session
 */
#define CTEST_WEEKEND                            PRIME_FLAG_0
/**
 * Shell command willing to have proprietary argument processing, should use these command to skip framework one
 */
#define SH_CMD_LEGACY                            PRIME_FLAG_1
/**
 * Ctest - used to skip the command or specific test when running/export tests. These tests will be marked as skip on
 *         test lists
 * Shell command - skip example execution
 */
#define SH_CMD_SKIP_EXEC                         PRIME_FLAG_2
/**
 * Internal framework flag assigned when ctest command examples execution is used with LoGger argument
 * Prints full command before execution
 */
#define SH_CMD_LOG                               PRIME_FLAG_3
/**
 * There are certain test that should be processed differently. Tests marked by this flag are included in run/export
 * only when random argument is used
 */
#define CTEST_RANDOM                             PRIME_FLAG_4
/**
 * Used for root level commands in shell framework to signal strict keyword verification vs keyword dictionary
 */
#define SH_CMD_VERIFY                            PRIME_FLAG_5
/**
 * Reserved for framework use - signals that sub-commands of this specific root should be registered in common shell
 * dynamic list - allows e.g. to skip dnx keyword to invoke commands registered under dnx
 */
#define SH_CMD_SKIP_ROOT                         PRIME_FLAG_6
/**
 * Used to enable/disable command under success of conditional callback, that is supposed to be attached to command
 * E.g. - command has no meaning for Adapter or the opposite,
 *     or command is only enabled when certain soc property present
 */
#define SH_CMD_CONDITIONAL                       PRIME_FLAG_7
/**
 * Represents system command argument "all"
 * For internal framework usage only
 */
#define SH_CMD_ALL                               PRIME_FLAG_8
/**
 * Represents system command argument "force"
 * For internal framework usage only
 */
#define SH_CMD_FORCE                             PRIME_FLAG_9
/**
 * Represents system command argument "failure"
 * For internal framework usage only
 */
#define SH_CMD_FAILURE_ONLY                      PRIME_FLAG_10
/*
 * Tests that should not run after WB, should be marked by this flag
 */
#define SH_CMD_SKIP_TEST_WB                      PRIME_FLAG_11
/**
 * Represents system command argument "quiet", which deprecate all messages while executing shell examples
 * For internal framework usage only
 */
#define SH_CMD_SILENT                            PRIME_FLAG_12
/**
 * Flag allows to pass control of random/postcommit to flag per dynamically generated test.
 * See explanation to PRECOMMIT
 */
#define CTEST_PASS                               PRIME_FLAG_13
/**
 * Flag used in traverse to signal that the callback should be applied to branches as well and not only leafs.
 * For internal use only
 */
#define SH_CMD_TRAVERSE_ALL                      PRIME_FLAG_14
/**
 * Flag used to signal shell command to redirect output into XML with predefined file. Previous one will be overwritten
 */
#define SH_CMD_OUTPUT_XML                        PRIME_FLAG_15
/**
 * Roll back to the original (before running the test) DBAL and SW state configuration after the test is done running.
 */
#define SH_CMD_ROLLBACK                          PRIME_FLAG_16
/**
 * Flag used to skip memory check.
 * In default there is memory check for all shell commands. It check whether amount of memory allocated was equal to freed one and
 * in case of mismatch shell command will return error
 */
#define SH_CMD_SKIP_MEMORY_CHECK                 PRIME_FLAG_17
/**
 * Flag used to mark the command which XML output will not be verified vs gold result during testing
 * Stand alone comparison may be still performed
 */
#define SH_CMD_NO_XML_VERIFY                     PRIME_FLAG_18
/**
 * Flag used to mark the command which XML output will not be verified vs gold result during testing
 * Stand alone comparison may be still performed
 */
#define SH_CMD_EXEC_LIST_ONLY                    PRIME_FLAG_19
/**
 * Flag used to mark the command which XML output will not be verified vs gold result during testing
 * Stand alone comparison may be still performed
 */
#define SH_CMD_EXEC_PER_CORE                     PRIME_FLAG_20
/**
 * Flag used to mark the command which XML output will be verified vs CUSTOM gold result during testing.
 */
#define SH_CMD_CUSTOM_XML_VERIFY                 PRIME_FLAG_21
/**
 * Flag used to mark the command which test output will be in the user manual.
 */
#define CTEST_UM                                 PRIME_FLAG_22
/**
 * Flag used to mark if the command is not part of the release user manual
 */
#define SH_CMD_NOT_RELEASE                       PRIME_FLAG_23
/**
 * Flag used to ensure full traversal of command tree, regardless of soc properties or devices.
 * It is used to effectively make the the command tree static, with all commands being initialized (during the init stage),
 *  though those that are not available in the current configuration will not be accessible or shown at any time.
 * It is also used to mark the when Ctest export should include all tests,
 *  including those that are unsupported in the current configuration.
 *  Depending on the command, some tests may not be included as a result of internal callback invokes.
 */
#define SH_CMD_TRAVERSE_NO_CB                    PRIME_FLAG_24
/**
 * Flag used to mark tests and test groups to be skipped if export/list/run command invoked with param skippable
 */
#define CTEST_SKIPPABLE                          PRIME_FLAG_25
/**
 * Flag used to mark tests and test groups as unsupported in adapter.
 * Will be skipped in adapter when export is run and included only when run with option 'full'.
 */
#define CTEST_ADAPTER_UNSUPPORTED                PRIME_FLAG_26
/**
 * Flag used to continue running remaining tests after first failure
 */
#define CTEST_CONT_ON_FAIL                       PRIME_FLAG_27
/**
 * Flag to disable the prefix lookup in sh_sand_cmd_get
 */
#define SH_CMD_NO_PREFIX_NAME_LOOKUP             PRIME_FLAG_28
/*
 * Flag to indicate that command is required for initialization sequence.
 */
#define SH_CMD_INIT_REQUIRED                     PRIME_FLAG_29

#define CTEST_DEFAULT    "default"

/**
 * \brief
 *   Presence of this flag in argument state means that user explicitly used this option in shell command invocation
 */
#define SH_SAND_ARG_PRESENT      SAL_BIT(0)
/**
 * \brief
 *   Presence of this flag in argument state means that option has mask: name=value,mask
 */
#define SH_SAND_ARG_MASKED       SAL_BIT(1)
/**
 * \brief
 *   Presence of this flag in argument state means that option has range: name=start-end
 *   Range may be provided only for non-negative value
 */
#define SH_SAND_ARG_RANGE        SAL_BIT(2)
/**
 * \brief
 *   Presence of this flag in argument state means that option is dynamic one verified through callback
 *   Validation range is not enforced over dynamic options
 */
#define SH_SAND_ARG_DYNAMIC      SAL_BIT(3)
/**
 * \brief
 *   Presence of this flag in argument state means that option may be provided without argument name
 */
#define SH_SAND_ARG_FREE         SAL_BIT(4)
/**
 * \brief
 *   Presence of this flag in argument state means that examples are not required to utilize this argument
 */
#define SH_SAND_ARG_QUIET        SAL_BIT(5)

/**
 * \brief Maximum string size for single token input
 */
#define SH_SAND_MAX_TOKEN_SIZE      1024
#define SH_SAND_MAX_LABELS          100
#define SH_SAND_MAX_ARRAY32_SIZE    16
#define SH_SAND_MAX_SHORTCUT_SIZE   12
#define SH_SAND_MAX_KEYWORD_SIZE    32
#define SH_SAND_MAX_RESOURCE_SIZE   SH_SAND_MAX_KEYWORD_SIZE + 3        /* Basic keyword plus 1 character for null term
                                                                         * and 2 for plural form */
#define SH_SAND_MAX_COMMAND_DEPTH   10
#define SH_SAND_MAX_TIME_SIZE       64
#define SH_SAND_MAX_OPTIONS         100
#define SH_SAND_MAX_ENUM_ENTRIES    200

#define SH_SAND_MAX_UINT32          0xFFFFFFFF

#define FAMILY_NAME(unit)           (SOC_IS_DNX(unit) ? ("dnx") : (SOC_IS_DNXF(unit) ? ("dnxf") : ("")))

struct sh_sand_cmd_s;

/**
* Enum that represent if it is a shell or ctest.
*/
typedef enum
{
    MANUAL_TYPE_CTEST,
    MANUAL_TYPE_SHELL,
    MANUAL_TYPE_SHELL_RELEASE
} sh_manual_type_e;

/**
 * \brief Typedef to construct structure that will hold all possible options of true and false
 */
typedef struct sh_sand_enum_s
{
    /**
     * String representing true or false statement
     */
    char *string;
    /**
     * Value that will be FALSE for negative, TRUE for positive answers
     */
    int value;
    /**
     *  Description of specific enum value
     */
    char *desc;
    /**
     * String allowing to obtain dynamic value,
     * e.g. if first token is DNX_DATA - values will be obtained from DNX Data module
     */
    char *plugin_str;
    /*
     * If not NULL, append these enum entries to the enum entries list
     */
    struct sh_sand_enum_s *inherited_enum;
} sh_sand_enum_t;

/**
 * \brief Typedef for shell leaf command
 * \par DIRECT INPUT:
 *     \param [in] keyword option name to be verified through the callback
 *     \param [in] id_p pointer to option identifier to be used by shell command, may be NULL
 * \par INDIRECT OUTPUT:
 *     \param [out] id option identifier to be used by shell command, transferred through id_p variable
 * \par DIRECT OUTPUT:
 *     \retval SAL_FIELD_TYPE_NONE for failure - option does not exist
 *     \retval SAL_FIELD_TYPE_* any other valid field type
 */
typedef shr_error_e(
    *sh_sand_option_cb_t) (
    int unit,
    char *keyword,
    sal_field_type_e * type,
    uint32 *id_p,
    /*
     * Void pointer for different kind os extensions
     */
    void **ext_ptr_p);

/**
 * \brief Typedef for init dynamic test/invocation list per leaf command
 * \param [in] keyword option name to be verified through the callback
 * \param [in] id_p pointer to option identifier to be used by shell command, may be NULL
 * \return
 *     \retval _SHR_E_NONE for success
 *     \retval shr_error_e for any error
 */
typedef shr_error_e(
    *sh_sand_invoke_cb_t) (
    int unit,
    rhlist_t * invoke_list);

/**
 * \brief Typedef for generating auto-complete values for options
 * \param [in] substring the user-provided substring to find matches for
 * \param [in] full_string - a string to be filled by the callback with the matching auto-complete values, space-separated
 *                               allocated and freed by the code calling the callback
 * \param [in] num_matches_found - an int to be filled by the number of matching auco-complete values
 * \return
 *     \retval _SHR_E_NONE for success
 *     \retval shr_error_e for any error
 */
typedef shr_error_e(
    *sh_sand_ac_option_cb_t) (
    int unit,
    char *substring,
    char *full_string,
    int *num_matches_found);

/*
 * Flags for examples generation
 */
/** Add to list auto generated examples as well - used mainly for test purposes */
#define SH_SAND_EXAMPLES_DYNAMIC    0x01
/** Add to list examples with full command, otherwise there will be only arguments */
#define SH_SAND_EXAMPLES_FULL       0x02

/**
 * \brief Typedef for init dynamic examples list  per leaf command
 * \param [in] unit - unit ID
 * \param [in] flags       - flags field is reserved for future use
 * \param [in] sh_sand_cmd - leaf command, which examples we need to generate
 * \param [in] examples_list - pointer to the example list used in add_exampel API
 *
 * \return
 *     \retval _SHR_E_NONE for success
 *     \retval shr_error_e for any error
 */
typedef shr_error_e(
    *sh_sand_example_generate_cb_t) (
    int unit,
    int flags,
    struct sh_sand_cmd_s * sh_sand_cmd,
    rhlist_t * examples_list);

/**
 * \brief Typedef for init/deinit configuration before/after examples invocation
 * \param [in] unit - unit id
 * \return
 *     \retval _SHR_E_NONE for success
 *     \retval shr_error_e for any error
 */
typedef shr_error_e(
    *sh_sand_simple_cb_t) (
    int unit);

/**
 * \brief Union allowing to handle all types of parameters through the same pointer
 */
typedef union
{
    /**
     * String, copied from input
     */
    char val_str[SH_SAND_MAX_TOKEN_SIZE];
    /**
     * It is plain int value, but we use separate to mark the boolean
     */
    int val_bool;
    /**
     * It is plain int value, but we use separate to mark the enum
     */
    int val_enum;
    /**
     * 32 bit signed value
     */
    int val_int32;
    /**
     * 32 bit unsigned value
     */
    uint32 val_uint32;
    /**
     * MAC address - array of 6 bytes
     */
    sal_mac_addr_t mac_addr;
    /**
     * IPv4 address - unsigned 32 bit value
     */
    sal_ip_addr_t ip4_addr;
    /**
     * IPv6 address - array of 16 bytes
     */
    sal_ip6_addr_t ip6_addr;
    /**
     * Array 32 bit unsigned value
     */
    uint32 array_uint32[SH_SAND_MAX_ARRAY32_SIZE];
    /**
     * Ports bitmap
     */
    bcm_pbmp_t ports_bitmap;
} sh_sand_param_u;

/**
 * \brief Union allowing to handle all types of parameters through the same pointer
 */
typedef union
{
    /**
     * 32 bit signed value
     */
    int val_int32;
    /**
     * 32 bit unsigned value
     */
    uint32 val_uint32;
} sh_sand_validation_u;

/*
 * Keyword Flags
 */
#define SH_SAND_KEYWORD_SINGLE_CHARACTER_SHORTCUT 0x0001
#define SH_SAND_KEYWORD_ALLOW_DOUBLE              0x0002

/**
 * \brief Control structure for command keyword definition, provided by framework
 */
typedef struct sh_sand_keyword_s
{
    /**
     * Keyword
     */
    char *keyword;
    /*
     *
     */
    uint32 flags;
    /**
     * Shortcut for the keyword obtained by capital letters
     */
    char short_key[SH_SAND_MAX_SHORTCUT_SIZE];
    /**
     * List for all alternatives, that should not be assigned separate keyword
     * 1. Plural form for the keyword, no plural being provided in the sh_sand_keywords means auto creation
     * 2. Any variation that is keyword_# or keyword#
     */
    rhlist_t *var_list[SOC_MAX_NUM_DEVICES];
    /**
     * How much times specific keyword was used
     */
    int count;
    /**
     * List of commands used this keyword
     */
    rhlist_t *cmd_list[SOC_MAX_NUM_DEVICES];
} sh_sand_keyword_t;

/**
 * \brief Control structure for cli option definition, provided by command developer
 */
typedef struct sh_sand_option_s
{
    /**
     * Option Name
     */
    char *keyword;
    /**
     * Option type, used to scan from string into value and print the option
     */
    sal_field_type_e type;
    /**
     * Brief description, used in usage
     */
    char *desc;
    /**
     * Default string in the same format, as CLI user is supposed to enter
     */
    char *def;
    /*
     * Void pointer for different kind of extensions
     */
    void *ext_ptr;
    /*
     * String representing valid range for certain types of options:INT32, UINT32
     */
    char *valid_range;
    /*
     * Misc flags: See Shell Argument FLags above - SH_SAND_ARG_*
     */
    int flags;
    /*
     * On init/verify short key based on capital letters in sh_sand_keyword is assigned
     */
    char *short_key;
    /*
     * On init/verify put here pointer to resource for the keyword found
     */
    char *full_key;
    /*
     * If not NULL, append these options to the option list
     */
    struct sh_sand_option_s *inherited_options;
    /*
     * Callback allowing to dynamically auto-complete options
     */
    sh_sand_ac_option_cb_t ac_option_cb;
} sh_sand_option_t;

/**
 * \brief Control structure for processed CLI option, provided by framework to leaf command.
 * Structure is initialized once and then only param_buffer, param and present variables are updated per command invocation
 */
typedef struct sh_sand_args_s
{
    /**
     * Entry allows single element to be queued on argument list, provided as input parameter for leaf routine
     */
    rhentry_t entry;
    /**
     * Option type, used to scan from string into value and print the option
     */
    sal_field_type_e type;
    /**
     * Identify different info per option
     * whether specific parameter was present or not on command line
     * whether mask was present or not, range or not
     * See full list above SH_SAND_ARG_*
     */
    int state;
    /**
     * Identify whether specific parameter is requested to be present - aka must option.
     * It happens, when no default is defined. Pay attention that string option having empty one as default is valid default definition
     * Only NULL pointer for default is considered absence of default
     */
    int requested;
    /**
     * If option present
     *   1. If the option is single it will be here
     *   2. If it is range, start will be here
     *   3. If it is masked value, value will be here
     */
    sh_sand_param_u param1;
    /**
     * If option present
     *   1. If the option is single, nothing will be here - should not be accessed
     *   2. If it is range, end will be here
     *   3. If it is masked value, value will be here, if no mask provided will filled by 0xFF
     */
    sh_sand_param_u param2;
    /**
     * Identify different default info per option
     * whether specific parameter was present or not on command line
     * whether mask was present or not, range or not
     */
    int def_state;
    /**
     * Contains default value for param1 (see above)
     */
    sh_sand_param_u def_param1;
    /**
     * Contains default value for param2 (see above)
     */
    sh_sand_param_u def_param2;
    /**
     * Low end of valid range
     */
    sh_sand_validation_u low;
    /**
     * High end of valid range
     */
    sh_sand_validation_u high;
    /*
     * Void pointer for different kind of extensions
     */
    void *ext_ptr;
    /*
     * Shortcut for option
     */
    char *short_key;
} sh_sand_arg_t;

/*
 * \brief - describes ctest test type
 */
typedef enum
{
    SHR_SAND_CTEST_TYPE_UNKNOWN,
    SHR_SAND_CTEST_TYPE_SEMANTIC,
    SHR_SAND_CTEST_TYPE_FUNCTIONAL
} sh_sand_ctest_type_t;

/*
 * \brief - describes ctest documentation
 */
typedef struct
{
    /*
     * Number 1-3, 1 - most important
     */
    uint32 priority;

    /*
     * Test type. If NULL, the default is being used.
     */
    sh_sand_ctest_type_t type;

    /*
     * In case Test module contain several sub features, each sub-feature will be mentioned as sub-topic
     * Default is being used instead when NULL.
     */
    const char *sub_feature;

    /*
     * Functional Requirements - The user story, i.e. requirement from customer point of view.
     * Man's brief is used instead when NULL.
     */
    const char *brief;

    /*
     * CFD JIRA if any
     */
    const char *tracking_id;

    /*
     * Description of what is going on in the test.
     * FULL description from man is taken instead when NULL.
     */
    const char *procedures;

    /*
     * Result expected from running "Test Procedure".
     * Default is being used instead when NULL.
     */
    const char *result;

    /*
     * Additional notes, if any, free text.
     * Default is being used instead when NULL.
     */
    const char *note;

} sh_sand_ctest_doc_t;

/**
 * \brief Set of pointers to different info strings, which assembles into usage or man page
 */
typedef struct
{
    /**
     * brief command description not more than 80 characters
     */
    const char *brief;
    /**
     * Full command description limited by 1024 characters. May be increased through PRT_LONG_STR_SIZE
     */
    const char *full;
    /**
     * How command line should look like.
     * E.g. access list [name=str] [property={reg, mem, signal, array}]
     */
    char *synopsis;
    /**
     * Characteristic examples of command usage
     */
    char *examples;
    /**
     * Characteristic examples of command usage
     */
    char *compatibility;
    /*
     * Invoke this callback (if not NULL) before execution of examples
     * Usually serves to establish configuration that will allow command to work
     */
    sh_sand_simple_cb_t init_cb;
    /*
     * Invoke this callback (if not NULL) after execution of examples to clean the system
     */
    sh_sand_simple_cb_t deinit_cb;
    /*
     * Callback allowing to add dynamically list of examples/tests per command
     */
    sh_sand_example_generate_cb_t example_generate_cb;
} sh_sand_man_t;

/**
 * \brief Control structure for shell command arguments, generated by command init and updated on invoke
 */
typedef struct
{
    /**
     * Pointer to static arguments list processed by framework.
     */
    rhlist_t *stat_args_list;   /* List of actual arguments, initialized on verify */
    /**
     * Pointer to dynamic arguments list obtained through callback from user.
     * List is emptied on action completion, but not erased
     */
    rhlist_t *dyn_args_list;    /* List of actual arguments, approved dynamically by user */
    /**
     * String that will contain all command line options
     * It is set to EMPTY before new command options processing
     */
    char options_str[SH_SAND_MAX_TOKEN_SIZE];
    /*
     * Each command may generate number of tables, all the tables from 1 invocation should be placed under the same node
     *    1. time stamp will be nullified before command invocation
     *    2. 1st table will create command node
     *    3. rest of tables will be placed under the same command node
     */
    char time_str[SH_SAND_MAX_TIME_SIZE];
    /*
     * System commands require pointer to underlying command array
     */
    struct sh_sand_cmd_s *sh_sand_cmd_a;
    struct sh_sand_cmd_s *sh_sys_cmd_a;
    struct sh_sand_cmd_s *sh_sand_cmd;
} sh_sand_control_t;

/**
 * \brief Typedef for shell leaf command
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] args pointer standard bcm shell argument structure, used by parse.h MACROS and routines
 *     \param [in] ctr pointer to list of options processed by sand framework to be used with SH_SAND MACROS
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed
 *     \retval other errors for other failure type
 * \remark automatically frees the list
 */
typedef shr_error_e(
    *sh_sand_func_t) (
    int unit,
    args_t * args,
    sh_sand_control_t * ctr);

/**
 * \brief Control structure for full definition, provided by command developer
 */
typedef struct sh_sand_invoke_s
{
    /**
     * Pointer to test name
     */
    char *name;
    /**
     * Pointer to string with all parameters requested for specific command
     * e.g for "ctest mdb" - "table=LPM_PRIVATE", so that entire test line will be "ctest mdb table=LPM_PRIVATE"
     */
    char *params;
    /**
     * Flow execution flags, pointing to the stage test will take part in or other flow control
     */
    int flags;
    /*
     * If not NULL, append these invokes to the invokes list
     */
    struct sh_sand_invoke_s *inherited_invokes;
} sh_sand_invoke_t;

/**
 * \brief Control structure for test list
 */
typedef struct sh_sand_test_s
{
    /**
     * Entry allows single element to be queued on argument list, provided as input parameter for leaf routine
     */
    rhentry_t entry;
    /**
     * Pointer to string with all parameters requested for specific command
     * e.g for "ctest mdb" - "table=LPM_PRIVATE", so that entire test line will be "ctest mdb table=LPM_PRIVATE"
     */
    char *params;
    /**
     * Flow execution flags, pointing to the stage test will take part in or other flow control
     */
    int flags;
} sh_sand_test_t;
/**
 * \brief Control structure for shell command definition, provided by command developer
 */

typedef struct sh_sand_cmd_s
{
    /**
     * Command name
     */
    char *keyword;
    /**
     * Pointer to leaf callback, if there is one
     */
    sh_sand_func_t action;
    /**
     * Pointer to next level command array, if there  is one
     */
    struct sh_sand_cmd_s *child_cmd_a;
    /**
     * Pointer to options list
     */
    sh_sand_option_t *options;
    /**
     * Pointer to man info structure, must be provided if command has leaf
     */
    sh_sand_man_t *man;         /* Manual structure */
    /*
     * When there is a need to accept dynamic options, callback need to be provided
     * Callback returns variable type and unique id that will allow to identify it inside without search
     */
    sh_sand_option_cb_t option_cb;
    /*
     * When there is a need to accept dynamic options, callback need to be provided
     * Callback returns variable type and unique id that will allow to identify it inside without search
     */
    sh_sand_invoke_t *invokes;
    /*
     * Misc flags, including
     * 1. Legacy mode serves to support legacy commands, do not enable with new/rewritten ones
     * 2. PRE/POST Commit indication for CTEST
     */
    int flags;
    /*
     * Callback allowing to add dynamically list of invocations/tests per command
     * For non-ctest shell commands is used as enable/disable callback.
     *    If callback returns FALSE - command is not active
     */
    sh_sand_invoke_cb_t invoke_cb;
    /*
     * From this place down - dynamic variable, changing in each command invocation
     */
    /*
     * On init/verify short key based on capital letters in sh_sand_keyword is assigned
     */
    char *short_key;
    /*
     * On init/verify put here pointer to resource for the keyword found
     */
    char *full_key;
    /*
     * String containing sequence of commands from root to the leaf
     */
    char cmd_only[SH_SAND_MAX_TOKEN_SIZE];
    /**
     * Pointer to legacy command list
     */
    rhlist_t *legacy_list;
    /**
     * Pointer to parent command, if it is not the root
     */
    struct sh_sand_cmd_s *parent_cmd;
    /*
     * String containing label.
     */
    char label[SH_SAND_MAX_TOKEN_SIZE];

    /*
     * CTest documentation
     */
    sh_sand_ctest_doc_t *doc;

} sh_sand_cmd_t;

typedef struct
{
    rhentry_t entry;
    /*
     * Short Key to utilize capital letters shortcuts in legacy commands
     */
    char short_key[SH_SAND_MAX_SHORTCUT_SIZE];
    /*
     * Pointer to next tokens list
     */
    rhlist_t *next_tokens_list;
    /**
     * Message to be printed when legacy command is used
     */
    char *message;
    /**
     * Substitute command to be invoked on behalf of legacy one, no command is NULL
     */
    char *substitute;
} sh_sand_legacy_entry_t;

typedef struct
{
    /**
     * Legacy command
     */
    const char *command;
    /**
     * Message to be printed when legacy command is used
     */
    const char *message;
    /**
     * Substitute command to be invoked on behalf of legacy one, no command is NULL
     */
    const char *substitute;
} sh_sand_legacy_cmd_t;

/**
 * \brief Iterate over command array looking for valid commands
 * \param [in] sh_sand_cmd_curr  - pointer to command that holds current
 * \param [in] sh_sand_cmd_array - pointer to command array that is being iterated
 * \param [in] flags             - pointer to CTEST and SH_CMD flags
 * \remark Valid commands are ones without CONDITIONAL flag or with null callback or callback returning success
 *  or when this iterator is invoked with SH_CMD_TRAVERSE_NO_CB flag
 */
#define SH_SAND_CMD_ITERATOR(sh_sand_cmd_curr, sh_sand_cmd_array, flags)                                               \
        for (sh_sand_cmd_curr = sh_sand_cmd_array;                                                                     \
                        sh_sand_cmd_curr != NULL && sh_sand_cmd_curr->keyword != NULL; sh_sand_cmd_curr++)             \
            if (sh_sand_cmd_availability_conds(unit, sh_sand_cmd_curr, flags) == _SHR_E_NONE)

/**
 * \brief Typedef for traverse callback command
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] args pointer standard bcm shell argument structure, used by parse.h MACROS and routines
 *     \param [in] cmd pointer to leaf command that traversing arrived to
 *     \param [in] prt_ctr pointer to PRT control structure for recursive output
 *     \param [in] depth recursion depth for different purposes, like print or recursion control
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed
 *     \retval other errors for other failure type
 * \remark automatically frees the list
 */
typedef shr_error_e(
    *sh_sand_traverse_cb_t) (
    int unit,
    args_t * args,
    sh_sand_cmd_t * cmd,
    void *flex_ptr,
    int depth);

/**
 * \brief Checks iterator conditions
 * \param [in] unit                 - unit id
 * \param [in] sh_sand_cmd_curr     - command to check conditions for
 * \param [in] flags                - CTEST and SH_CMD flags
 * \return
 *     \retval _SHR_E_NONE for success
 *     \retval other errors for other failure types
 * \remark - Iterator conditions are whether or not SH_CMD_CONDITIONAL or SH_CMD_TRAVERSE_NO_CB flags are present
 *           and whether or not the command has a callback - and if so what the return value of the callback is
 */
shr_error_e sh_sand_cmd_availability_conds(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_curr,
    uint32 flags);

/**
 * \brief Routine serves to invoke command from any level, it then acts recursively parsing command line
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] args pointer standard bcm shell argument structure, used by parse.h MACROS and routines
 *     \param [in] sh_sand_cmd_a pointer to command list(array) to start from
 *     \param [in] sh_sys_cmd_a pointer to system command list(array) for specified root
 *     \param [in] legacy_list - list in which we'll look for legacy commands if none will be found on sh_sand_cmd_a
 *     \param [in] flags - CTEST and SH_CMD flags
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e sh_sand_act(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_cmd_t * sh_sys_cmd_a,
    rhlist_t * legacy_list,
    uint32 flags);

/**
 * \brief
 *      Clean control structure from any allocation made in course on command processing
 * \param [in] unit
 * \param [in] sand_ctr - pointer to control structure used in command processing
 *
 * \remark
 *
 */
void sh_sand_option_list_clean(
    int unit,
    sh_sand_control_t * sand_ctr);

/**
 * \brief Remove option from a static or dynamic list and free the memory
 * \param [in] unit unit id
 * \param [in] sand_control pointer to sand control
 * \param [in] option_name - option name
 * \param [in] option_cb - option callback
 * \param [in] start_check - check if keyword starts from the string provided by option_name
 * \retval _SHR_E_NONE
 * \remark
 *    Is called from regular tree processing or from executing examples
 */
shr_error_e sh_sand_remove_option_dyn_stats_args(
    int unit,
    sh_sand_control_t * sand_control,
    char *option_name,
    sh_sand_option_cb_t option_cb,
    int start_check);

/**
 * \brief Fill args_list with options values or defaults
 * \param [in] unit unit id
 * \param [in] args pointer standard bcm shell argument structure, used by parse.h MACROS and routines
 * \param [in] sh_sand_cmd - leaf command to obtain full command for
 * \param [in] sand_ctr - pointer to control structure allocated by caller
 * \param [in] flags - misc flags for different options
 * \param [in] core - specify the core to execute. It will be used only if SH_CMD_EXEC_PER_CORE flag is set.
 * \retval _SHR_E_NONE
 * \remark
 *    Is called from regular tree processing or from executing examples
 */
shr_error_e sh_sand_option_list_process(
    int unit,
    int *core,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    sh_sand_control_t * sand_ctr,
    int flags);

/**
 * \brief Return enum string for value for specific command and option
 * \param [in] sand_control - control structure for command
 * \param [in] arg_keyword  - keyword for the command
 * \param [in] enum_value   - pointer to the string allocated by caller to be used as output location
 * \retval pointer to string which is name of enum or printed value or error message
 * \remark
 *    Routine (at least currently) does not return error, only success
 */
char *sh_sand_enum_value_text(
    sh_sand_control_t * sand_control,
    char *arg_keyword,
    int enum_value);

shr_error_e sh_sand_cmd_traverse(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_traverse_cb_t sh_sand_action,
    void *flex_ptr,
    int flags,
    char *label);

shr_error_e sys_usage_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e sys_manual_cmd_internal(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    char *introduction,
    sh_manual_type_e manual_type);

shr_error_e sys_manual_label_match(
    int unit,
    char *label,
    char **label_tokens,
    uint32 tokens_count,
    uint8 *match);

/**
 * \brief Routine serves to execute certain framework command
 * \param [in] unit             - unit id
 * \param [in] command          - command to be executed under framework
 * \return
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e sh_sand_execute(
    int unit,
    char *command);

/**
 * \brief Add example(arguments string) for shell command testing
 * \param [in] unit          - unit id
 * \param [in] flags         - flags - currently empty
 * \param [in] sh_sand_cmd   - shell command, which examples we are adding, taken from generate callback
 * \param [in] examples_list - examples list, where all new examples will be added, there may be static examples already
 * \param [in] arguments_str - parameter string as if it appear on command line
 * \return
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e sh_sand_example_add(
    int unit,
    int flags,
    sh_sand_cmd_t * sh_sand_cmd,
    rhlist_t * examples_list,
    char *arguments_str);

/**
 * \brief Routine serves to initialize shell command tree and verify requested resources
 * \param [in] unit                 - unit id
 * \param [in] sh_sand_cmd_a        - array of commands under the root
 * \param [in] sh_sand_legacy_cmd_a - array of legacy commands under the root
 * \return
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e sh_sand_root_init(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_legacy_cmd_t * sh_sand_legacy_cmd_a);

/**
 * \brief Routine serves to deinitialize shell command tree and free resources
 * \param [in] unit                 - unit id
 * \return
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e sh_sand_root_deinit(
    int unit);

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
/**
 * \brief Recursively traverse shell command tree whose root is sh_sand_cmd, adding the SH_CMD_INIT_REQUIRED flag to all commands.
 * \param [in] unit        - unit id
 * \param [in] sh_sand_cmd - array of commands under the root
 * \return
 *   * None
 * \remark
 *     This is used to help calibrate shell command tree and autocompletion in case BCM init sequence did not complete successfully.
 */
void sh_sand_traverse_add_init_required_flag(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd);
#endif

/**
 * \brief Routine serves to calibrate shell command tree and verify requested resources after completion of init
 * \param [in] unit                 - unit id
 * \param [in] sh_sand_cmd_a        - array of commands under the root
 * \param [in] sh_sand_legacy_cmd_a - array of legacy commands under the root
 * \return
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e sh_sand_cmd_calibrate(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_legacy_cmd_t * sh_sand_legacy_cmd_a);

/**
 * \brief Return string for boolean value
 */
char *sh_sand_bool_str(
    int bool);

/**
 * \brief Return string for enum value
 * \param [in] enum_entry - pointer to enum array
 * \param [in] value - value which name we need
 * \return
 *   \retval SUCCESS - string associated to value
 *   \retval FAILURE - NULL
 */
char *sh_sand_enum_str(
    sh_sand_enum_t * enum_entry,
    int value);

/**
 * \brief Returns the number of cores for the device.
 * \param [in] unit unit id
 * \param [out] nof_cores - Number of cores
 * \retval _SHR_E_NONE
 */
shr_error_e sh_sand_nof_cores_get(
    int unit,
    int *nof_cores);

/**
 * \brief This macro is for local usage only to make external macros SH_SAND_GET* more transparent
 */

/*
 * Set of MACROS for obtaining regular option values, not ranges and not masked
 * {
 */
/*
 * \brief - Fetches sand_arg as per option name and assigns value union ptr to "param" variable
 * \param [in] mc_arg_keyword - keyword for the argument
 * \remark
 *    Strictly internal MACRO - to be used only by underlying ones, makes local assumptions
 */
#define _SH_SAND_GET(mc_arg_keyword)                                                                                   \
    sh_sand_param_u *param = NULL;                                                                                     \
    sal_field_type_e param_type = SAL_FIELD_TYPE_MAX;                                                                  \
    if(sand_control == NULL)                                                                                           \
    {                                                                                                                  \
        SHR_CLI_EXIT(_SHR_E_PARAM, "No command control for:%s\n", mc_arg_keyword);                                     \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        sh_sand_arg_t *sand_arg;                                                                                       \
        if((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->stat_args_list, mc_arg_keyword)) == NULL)         \
        {                                                                                                              \
            if((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->dyn_args_list, mc_arg_keyword)) == NULL)      \
            {                                                                                                          \
                SHR_CLI_EXIT(_SHR_E_PARAM, "command line option:%s is not supported\n", mc_arg_keyword);               \
            }                                                                                                          \
        }                                                                                                              \
        param_type = sand_arg->type;                                                                                   \
        if(sand_arg->state & SH_SAND_ARG_RANGE)                                                                        \
        {                                                                                                              \
            SHR_CLI_EXIT(_SHR_E_PARAM, "GET MACRO does not suit range input for option:%s, use RANGE ones\n",          \
                                                                                              mc_arg_keyword);         \
        }                                                                                                              \
        if(sand_arg->state & SH_SAND_ARG_MASKED)                                                                       \
        {                                                                                                              \
            SHR_CLI_EXIT(_SHR_E_PARAM, "GET MACRO does not suit masked input for option:%s, use MASKED ones\n",        \
                                                                                              mc_arg_keyword);         \
        }                                                                                                              \
        if(sand_arg->state & SH_SAND_ARG_PRESENT)                                                                      \
        {                                                                                                              \
            param = &sand_arg->param1;                                                                                 \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            param = &sand_arg->def_param1;                                                                             \
        }                                                                                                              \
    }

/*
 * \brief - Fetches state of option - present or not present
 * \param [in] mc_arg_keyword - keyword for the argument
 * \param [in] mc_is_prsent   - boolean value that will be assigned by TRUE if argument present
 * \remark
 *    Should be used only when there is necessity to figure out that default value was assigned by user or not.
 *    Otherwise default tells all the story
 */
#define SH_SAND_IS_PRESENT(mc_arg_keyword, mc_is_present)                                                              \
    if(sand_control == NULL)                                                                                           \
    {                                                                                                                  \
        SHR_CLI_EXIT(_SHR_E_PARAM, "No command control for:%s\n", mc_arg_keyword);                                     \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        sh_sand_arg_t *sand_arg;                                                                                       \
        if((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->stat_args_list, mc_arg_keyword)) == NULL)         \
        {                                                                                                              \
            if((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->dyn_args_list, mc_arg_keyword)) == NULL)      \
            {                                                                                                          \
                SHR_CLI_EXIT(_SHR_E_PARAM, "command line option:%s is not supported\n", mc_arg_keyword);               \
            }                                                                                                          \
        }                                                                                                              \
        mc_is_present = (sand_arg->state & SH_SAND_ARG_PRESENT) ? TRUE : FALSE;                                        \
    }

/*
 * \brief - Verifies that GET macro is consistent wipe of option it requests, based on previously obtained param_type
 * \remark
 *    Strictly internal MACRO - to be used only by underlying ones, makes local assumptions
 */
#define _SH_SAND_TYPE_VERIFY(arg_keyword, in_type)                                                                     \
        if(in_type != param_type)                                                                                      \
        {                                                                                                              \
            SHR_CLI_EXIT(_SHR_E_PARAM, "option:\"%s\" - GET macro of type:%s not matching definition type:%s\n",       \
                                            arg_keyword, sal_field_type_str(in_type), sal_field_type_str(param_type)); \
        }

/*
 * \brief - Fetches argument value of cli option
 * \param [in] arg_keyword - option name, exactly as defined in option list(sh_sand_option_t), case insensitive
 * \param [in] arg_value   - variable for value assignment
 * \remark
 *    1. Valid only in the context of shell command framework, sand_control need to be defined. either via initial
 *       framework callback or passed down as parameter
 *    2. Description valid for all SH_SAND_GET_* macros with the only difference being arg_value type, which will be
 *       Specified explicitly before each macro
 *    3. Value assignment may be by value or pointer (see details per macro
 */
/**
 * 1. char *arg_value
 * 2. by pointer, pointer to the value string is assigned to
 */
#define SH_SAND_GET_STR(arg_keyword, arg_value)                                                                        \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_STR)                                     \
                             arg_value = param->val_str;                                                               \
                         }
/**
 * 1. int arg_value
 * 2. by value, boolean value is assigned to
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_BOOL(arg_keyword, arg_value)                                                                       \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_BOOL)                                    \
                             arg_value = param->val_bool;                                                              \
                         }
/**
 * 1. int arg_value
 * 2. by value, enum value is assigned to
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_ENUM(arg_keyword, arg_value)                                                                       \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_ENUM)                                    \
                             arg_value = param->val_enum;                                                              \
                         }
/**
 * 1. int arg_value
 * 2. by value, enum value is assigned to
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_ENUM_STR(mc_arg_keyword, arg_value) sh_sand_enum_value_text(sand_control, mc_arg_keyword, arg_value)

/**
 * 1. int arg_value -
 * 2. by value
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_PORT(arg_keyword, arg_value)                                                                       \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_PORT)                                    \
                             arg_value = param->ports_bitmap;                                                          \
                         }
/**
 * 1. int arg_value
 * 2. by value
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_INT32(arg_keyword, arg_value)                                                                      \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_INT32)                                   \
                             arg_value = param->val_int32;                                                             \
                         }
/**
 * 1. uint32 arg_value
 * 2. by value
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_UINT32(arg_keyword, arg_value)                                                                     \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_UINT32)                                  \
                             arg_value = param->val_uint32;                                                            \
                         }
/**
 * 1. uint32 *arg_value
 * 2. by pointer
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_ARRAY32(arg_keyword, arg_value)                                                                    \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_ARRAY32)                                 \
                             arg_value = param->array_uint32;                                                          \
                         }
/**
 * 1. sal_ip_addr_t arg_value
 * 2. by value
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_IP4(arg_keyword, arg_value)                                                                        \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_IP4)                                     \
                             arg_value = param->ip4_addr;                                                              \
                         }
/**
 * 1. sal_ip6_addr_t arg_value
 * 2. by value
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_IP6(arg_keyword, arg_value)                                                                        \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_IP6)                                     \
                             memcpy(arg_value, param->ip6_addr, sizeof(sal_ip6_addr_t));                               \
                         }
/**
 * 1. sal_mac_addr_t arg_value
 * 2. by value
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_MAC(arg_keyword, arg_value)                                                                        \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_MAC)                                     \
                             memcpy(arg_value, param->mac_addr, sizeof(sal_mac_addr_t));                               \
                         }
/*
 * End of regular options fetching MACROS
 * }
 */

/*
 * Set of MACROS for obtaining range option values
 * {
 */
/*
 * \brief - Fetches sand_arg as per option name and assigns value union ptr to "param" variable, difference from regular
 *          _SH_SAND_GET is that 2 params are fetched, and if there is no second pne on command line, first is assigned
 *          to both
 * \param [in] mc_arg_keyword - keyword for the argument
 * \remark
 *    Strictly internal MACRO - to be used only by underlying ones, makes local assumptions
 *    Only certain FIELD_TYPES are eligible for range options, see below
 */
#define _SH_SAND_GET_RANGE(mc_arg_keyword)                                                                             \
    sh_sand_param_u *param_start = NULL;                                                                               \
    sh_sand_param_u *param_end   = NULL;                                                                               \
    sal_field_type_e param_type = SAL_FIELD_TYPE_MAX;                                                                  \
    if(sand_control == NULL)                                                                                           \
    {                                                                                                                  \
        SHR_CLI_EXIT(_SHR_E_PARAM, "No command control for:%s\n", mc_arg_keyword);                                     \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        sh_sand_arg_t *sand_arg;                                                                                       \
        if((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->stat_args_list, mc_arg_keyword)) == NULL)         \
        {                                                                                                              \
            if((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->dyn_args_list, mc_arg_keyword)) == NULL)      \
            {                                                                                                          \
                SHR_CLI_EXIT(_SHR_E_PARAM, "command line option:%s is not supported\n", mc_arg_keyword);               \
            }                                                                                                          \
        }                                                                                                              \
        param_type = sand_arg->type;                                                                                   \
        if(sand_arg->state & SH_SAND_ARG_MASKED)                                                                       \
        {                                                                                                              \
            SHR_CLI_EXIT(_SHR_E_PARAM, "RANGE MACRO does not suit masked input for option:%s, use MASKED ones\n",      \
                                                                                                       mc_arg_keyword);\
        }                                                                                                              \
        if(sand_arg->state & SH_SAND_ARG_PRESENT)                                                                      \
        {                                                                                                              \
            param_start = &sand_arg->param1;                                                                           \
            if(sand_arg->state & SH_SAND_ARG_RANGE)                                                                    \
                param_end = &sand_arg->param2;                                                                         \
            else                                                                                                       \
                param_end = &sand_arg->param1;                                                                         \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            param_start = &sand_arg->def_param1;                                                                       \
            if(sand_arg->def_state & SH_SAND_ARG_RANGE)                                                                \
                param_end = &sand_arg->def_param2;                                                                     \
            else                                                                                                       \
                param_end = &sand_arg->def_param1;                                                                     \
        }                                                                                                              \
    }
/**
 * All *_RANGE macros have in addition to first value, which will represent start of range second one that represents
 * end of range, if there is only one value on command line it will be considered range of one element start=end
 * Eligible types: INT32, UINT32, IP4, IP6, MAC
 */
#define SH_SAND_GET_INT32_RANGE(arg_keyword, arg_value_start, arg_value_end)                                           \
                         {                                                                                             \
                             _SH_SAND_GET_RANGE(arg_keyword)                                                           \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_INT32)                                   \
                             arg_value_start = param_start->val_int32;                                                 \
                             arg_value_end   = param_end->val_int32;                                                   \
                         }

#define SH_SAND_GET_UINT32_RANGE(arg_keyword, arg_value_start, arg_value_end)                                          \
                         {                                                                                             \
                             _SH_SAND_GET_RANGE(arg_keyword)                                                           \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_UINT32)                                  \
                             arg_value_start = param_start->val_uint32;                                                \
                             arg_value_end   = param_end->val_uint32;                                                  \
                         }

#define SH_SAND_GET_IP4_RANGE(arg_keyword, arg_value_start, arg_value_end)                                             \
                         {                                                                                             \
                             _SH_SAND_GET_RANGE(arg_keyword)                                                           \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_IP4)                                     \
                             arg_value_start = param_start->ip4_addr;                                                  \
                             arg_value_end   = param_end->ip4_addr;                                                    \
                         }

#define SH_SAND_GET_IP6_RANGE(arg_keyword, arg_value_start, arg_value_end)                                             \
                         {                                                                                             \
                             _SH_SAND_GET_RANGE(arg_keyword)                                                           \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_IP6)                                     \
                             memcpy(arg_value_start, param_start->ip6_addr, sizeof(sal_ip6_addr_t));                   \
                             memcpy(arg_value_end, param_end->ip6_addr, sizeof(sal_ip6_addr_t));                       \
                         }

#define SH_SAND_GET_MAC_RANGE(arg_keyword, arg_value_start, arg_value_end)                                             \
                         {                                                                                             \
                             _SH_SAND_GET_RANGE(arg_keyword)                                                           \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_MAC)                                     \
                             memcpy(arg_value_start, param_start->mac_addr, sizeof(sal_mac_addr_t));                   \
                             memcpy(arg_value_end, param_end->mac_addr, sizeof(sal_mac_addr_t));                       \
                         }
/*
 * End of range options fetching MACROS
 * }
 */
/*
 * Set of MACROS for obtaining masked option values
 * {
 */
/*
 * \brief - Fetches sand_arg as per option name and assigns value union ptr to "param" variable. It obtains 2 value,
 *          where first is considered to be data, second mask. If there is no second, default value of all FF is
 *          assigned to mask
 * \param [in] mc_arg_keyword - keyword for the argument
 * \remark
 *    Strictly internal MACRO - to be used only by underlying ones, makes local assumptions
 */
#define _SH_SAND_GET_MASKED(mc_arg_keyword)                                                                            \
    sh_sand_param_u *param_data = NULL;                                                                                \
    sh_sand_param_u *param_mask = NULL;                                                                                \
    sal_field_type_e param_type = SAL_FIELD_TYPE_MAX;                                                                  \
    if(sand_control == NULL)                                                                                           \
    {                                                                                                                  \
        SHR_CLI_EXIT(_SHR_E_PARAM, "No command control for:%s\n", mc_arg_keyword);                                     \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        sh_sand_arg_t *sand_arg;                                                                                       \
        if((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->stat_args_list, mc_arg_keyword)) == NULL)         \
        {                                                                                                              \
            if((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->dyn_args_list, mc_arg_keyword)) == NULL)      \
            {                                                                                                          \
                SHR_CLI_EXIT(_SHR_E_PARAM, "command line option:%s is not supported\n", mc_arg_keyword);               \
            }                                                                                                          \
        }                                                                                                              \
        param_type = sand_arg->type;                                                                                   \
        if(sand_arg->state & SH_SAND_ARG_RANGE)                                                                        \
        {                                                                                                              \
            SHR_CLI_EXIT(_SHR_E_PARAM, "MASKED MACRO does not suit range input for option:%s, use range ones\n",       \
                                                                                                       mc_arg_keyword);\
        }                                                                                                              \
        if(sand_arg->state & SH_SAND_ARG_PRESENT)                                                                      \
        {                                                                                                              \
            param_data = &sand_arg->param1;                                                                            \
            if(sand_arg->state & SH_SAND_ARG_MASKED)                                                                   \
                param_mask = &sand_arg->param2;                                                                        \
            else                                                                                                       \
                param_mask = &sand_arg->param1;                                                                        \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            param_data = &sand_arg->def_param1;                                                                        \
            if(sand_arg->def_state & SH_SAND_ARG_MASKED)                                                               \
                param_mask = &sand_arg->def_param2;                                                                    \
            else                                                                                                       \
                param_mask = &full_param_mask;                                                                         \
        }                                                                                                              \
    }

/**
 * All *_RANGE macros have in addition to first value, which will represent data, second one that represents
 *       mask. if there is only one value on command line it will be considered FULLY unmasked or the value of
 *       0xFF filled in mask
 * Eligible types: INT32, UINT32, IP4, IP6, MAC
 */
#define SH_SAND_GET_INT32_MASKED(arg_keyword, arg_value_data, arg_value_mask)                                          \
                         {                                                                                             \
                             _SH_SAND_GET_MASKED(arg_keyword)                                                          \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_INT32)                                   \
                             arg_value_data = param_data->val_int32;                                                   \
                             arg_value_mask = param_mask->val_int32;                                                   \
                         }

#define SH_SAND_GET_UINT32_MASKED(arg_keyword, arg_value_data, arg_value_mask)                                         \
                         {                                                                                             \
                             _SH_SAND_GET_MASKED(arg_keyword)                                                          \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_UINT32)                                  \
                             arg_value_data = param_data->val_uint32;                                                  \
                             arg_value_mask = param_mask->val_uint32;                                                  \
                         }

#define SH_SAND_GET_IP4_MASKED(arg_keyword, arg_value_data, arg_value_mask)                                            \
                         {                                                                                             \
                             _SH_SAND_GET_MASKED(arg_keyword)                                                          \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_IP4)                                     \
                             arg_value_data = param_data->ip4_addr;                                                    \
                             arg_value_mask = param_mask->ip4_addr;                                                    \
                         }

#define SH_SAND_GET_IP6_MASKED(arg_keyword, arg_value_data, arg_value_mask)                                            \
                         {                                                                                             \
                             _SH_SAND_GET_MASKED(arg_keyword)                                                          \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_IP6)                                     \
                             memcpy(arg_value_data, param_data->ip6_addr, sizeof(sal_ip6_addr_t));                     \
                             memcpy(arg_value_mask, param_mask->ip6_addr, sizeof(sal_ip6_addr_t));                     \
                         }

#define SH_SAND_GET_MAC_MASKED(arg_keyword, arg_value_data, arg_value_mask)                                            \
                         {                                                                                             \
                             _SH_SAND_GET_MASKED(arg_keyword)                                                          \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_MAC)                                     \
                             memcpy(arg_value_data, param_data->mac_addr, sizeof(sal_mac_addr_t));                     \
                             memcpy(arg_value_mask, param_mask->mac_addr, sizeof(sal_mac_addr_t));                     \
                         }
/*
 * End of masked options fetching MACROS
 * }
 */
/*
 * Set of MACROS for handling dynamic options and their values
 * {
 */
#define _SH_SAND_GET_DYN(mc_arg_keyword, mc_is_present)                                                                \
            sh_sand_arg_t *sand_arg;                                                                                   \
            if((sand_control == NULL) || (sand_control->dyn_args_list == NULL) ||                                      \
                ((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->dyn_args_list, mc_arg_keyword)) == NULL))   \
            {                                                                                                          \
                mc_is_present = FALSE;                                                                                 \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                mc_is_present = TRUE;                                                                                  \
            }                                                                                                          \

#define SH_SAND_GET_STR_DYN(arg_keyword, arg_value, is_present)                                                        \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param1.val_str;                                                 \
                         }

#define SH_SAND_GET_BOOL_DYN(arg_keyword, arg_value, is_present)                                                       \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param1.val_bool;                                                \
                         }

#define SH_SAND_GET_ENUM_DYN(arg_keyword, arg_value, is_present)                                                       \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param1.val_enum;                                                \
                         }

#define SH_SAND_GET_INT32_DYN(arg_keyword, arg_value, is_present)                                                      \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param1.val_int32;                                               \
                         }

#define SH_SAND_GET_ARRAY32_DYN(arg_keyword, arg_value, is_present)                                                    \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param1.array_uint32;                                            \
                         }

#define SH_SAND_GET_UINT32_DYN(arg_keyword, arg_value, is_present)                                                     \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param1.val_uint32;                                              \
                         }

#define SH_SAND_GET_IP4_DYN(arg_keyword, arg_value, is_present)                                                        \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param1.ip4_addr;                                                \
                         }

#define SH_SAND_GET_IP6_DYN(arg_keyword, arg_value, is_present)                                                        \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 memcpy(arg_value, sand_arg->param1.ip6_addr, sizeof(sal_ip6_addr_t));                 \
                         }

#define SH_SAND_GET_MAC_DYN(arg_keyword, arg_value, is_present)                                                        \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 memcpy(arg_value, sand_arg->param1.mac_addr, sizeof(sal_mac_addr_t));                 \
                         }
/*
 * End of dynamic options fetching MACROS
 * }
 */

#define SH_SAND_GET_ITERATOR(mc_arg) \
    for(mc_arg = utilex_rhlist_entry_get_first(sand_control->dyn_args_list); mc_arg; mc_arg = utilex_rhlist_entry_get_next(mc_arg))

#define SH_SAND_GET_DYN_ARGS_NUM()       (sand_control->dyn_args_list == NULL) ? 0 : RHLNUM(sand_control->dyn_args_list)
#define SH_SAND_HAS_MASK(mc_arg)     (mc_arg->state & SH_SAND_ARG_MASKED)

#define SH_SAND_GET_NAME(mc_arg)         RHNAME(mc_arg)
#define SH_SAND_GET_ID(mc_arg)           RHID(mc_arg)
#define SH_SAND_GET_TYPE(mc_arg)         mc_arg->type
#define SH_SAND_ARG_STR(mc_arg)          mc_arg->param1.val_str
#define SH_SAND_ARG_UINT32_DATA(mc_arg)  mc_arg->param1.val_uint32
#define SH_SAND_ARG_UINT32_MASK(mc_arg)  mc_arg->param2.val_uint32
#define SH_SAND_ARG_ARRAY_DATA(mc_arg)   mc_arg->param1.array_uint32
#define SH_SAND_ARG_ARRAY_MASK(mc_arg)   mc_arg->param2.array_uint32
#define SH_SAND_ARG_MAC_DATA(mc_arg)     mc_arg->param1.mac_addr
#define SH_SAND_ARG_MAC_MASK(mc_arg)     mc_arg->param2.mac_addr
#define SH_SAND_ARG_IP6_DATA(mc_arg)     mc_arg->param1.ip6_addr
#define SH_SAND_ARG_IP6_MASK(mc_arg)     mc_arg->param2.ip6_addr

/**
 * \brief Callback in shell command framework allowing to enable command for DNX device
 * \param [in] unit             - unit id
 * \param [in] list             - unused here (keep callback type)
 * \return
 *   \retval _SHR_E_NONE      - If the device is DNX
 *   \retval _SHR_E_NOT_FOUND - Otherwise
 *   Flow goes to exit in any case of failure
 */
shr_error_e sh_cmd_is_dnx(
    int unit,
    rhlist_t * list);

/**
 * \brief Callback in shell command framework allowing to enable command for DNXF device
 * \param [in] unit             - unit id
 * \param [in] list             - unused here (keep callback type)
 * \return
 *   \retval _SHR_E_NONE      - If the device is DNX
 *   \retval _SHR_E_NOT_FOUND - Otherwise
 *   Flow goes to exit in any case of failure
 */
shr_error_e sh_cmd_is_dnxf(
    int unit,
    rhlist_t * list);

/**
 * \brief Callback in shell command framework allowing to enable command for device that supports FEC BER Projection
 * \param [in] unit             - unit id
 * \param [in] list             - unused here (keep callback type)
 * \return
 *   \retval _SHR_E_NONE      - If the device supports FEC BER Projection
 *   \retval _SHR_E_NOT_FOUND - If the device doesn't support FEC BER Projection
 *   Flow goes to exit in any case of failure
 */
shr_error_e sh_cmd_is_fec_ber_proj_supported(
    int unit,
    rhlist_t * list);

/**
 * \brief Callback in shell command framework allowing to enable command for device doesn't support FEC BER Projection
 * \param [in] unit             - unit id
 * \param [in] list             - unused here (keep callback type)
 * \return
 *   \retval _SHR_E_NONE      - If the device doesn't support FEC BER Projection
 *   \retval _SHR_E_NOT_FOUND - If the device supports FEC BER Projection
 *   Flow goes to exit in any case of failure
 */
shr_error_e sh_cmd_is_fec_ber_proj_not_supported(
    int unit,
    rhlist_t * list);

/**
 * \brief Callback in shell command framework allowing to enable command for DXNF device with a single core
 * \param [in] unit             - unit id
 * \param [in] list             - unused here (keep callback type)
 * \return
 *   \retval _SHR_E_NONE      - If the device has a single core
 *   \retval _SHR_E_NOT_FOUND - Otherwise
 *   Flow goes to exit in any case of failure
 */
shr_error_e sh_cmd_is_dnxf_single_core(
    int unit,
    rhlist_t * list);

/**
 * \brief Callback in shell command framework allowing to enable command for DXNF device with multiple cores
 * \param [in] unit             - unit id
 * \param [in] list             - unused here (keep callback type)
 * \return
 *   \retval _SHR_E_NONE      - If the device has multiple cores
 *   \retval _SHR_E_NOT_FOUND - Otherwise
 *   Flow goes to exit in any case of failure
 */
shr_error_e sh_cmd_is_dnxf_not_single_core(
    int unit,
    rhlist_t * list);
/**
 * \brief Callback in shell command framework allowing to disable command for ADAPTER mode
 * \param [in] unit             - unit id
 * \param [in] list             - unused here (keep callback type)
 * \return
 *   \retval _SHR_E_NONE      - If the device is DNX
 *   \retval _SHR_E_NOT_FOUND - Otherwise
 *   Flow goes to exit in any case of failure
 */
shr_error_e sh_cmd_is_device(
    int unit,
    rhlist_t * list);

/**
 * \brief Initialize deinit_cb to this routine to avoid SW Recovery invocation
 * \param [in] unit             - unit id
 * \return
 *   \retval standard shr_error_e
 */
shr_error_e sh_deinit_cb_void(
    int unit);

/**
 * \brief Set root shell command pointer
 * \param [in] unit             - unit id
 * \param [out] sh_sand_cmd_a_p - pointer to place were to assign root shell command pointer
 * \return
 *   \retval _SHR_E_NONE      - If the device is DNX
 *   \retval _SHR_E_PARAM     - If place were pointer should be assigned is NULL
 *   \retval _SHR_E_INTERNAL  - If root is NULL
 */
shr_error_e sh_sand_cmd_root_get(
    int unit,
    sh_sand_cmd_t ** sh_sand_cmd_a_p);

extern sh_sand_enum_t sh_enum_table_lag_pool[];
extern sh_sand_enum_t sh_enum_table_flow_agg_id[];

extern sh_sand_man_t sh_sand_shell_man;
extern sh_sand_cmd_t sh_sand_shell_cmds[];

extern sh_sand_cmd_t sh_sand_sys_cmds[];

extern sh_sand_man_t sys_usage_man;
extern sh_sand_option_t sys_usage_arguments[];

extern sh_sand_keyword_t sh_sand_keywords[];
extern sh_sand_param_u full_param_mask;

extern sh_sand_option_t sys_manual_arguments[];

/**
 * \brief
 *      This function recursively builds a list of enum entries, including inherited enum entries
 * \param [in] enum_entries         - the enum entries to build the list with
 * \param [in,out] enum_entries_cnt - the running number of enum entries added to all_enum_entries
 * \param [in,out] all_enum_entries - the list of all enum entries in enum_entries, including inherited enum entries
 */
void sh_sand_enum_entries_get_recursive(
    sh_sand_enum_t * enum_entries,
    int *enum_entries_cnt,
    sh_sand_enum_t ** all_enum_entries);

/**
 * \brief
 *      This function recursively builds a list of options, including inherited options
 * \param [in] options         - the options to build the list with
 * \param [in,out] options_cnt - the running number of options added to all_options
 * \param [in,out] all_options - the list of all options in options, including inherited options
 */
void sh_sand_options_get_recursive(
    sh_sand_option_t * options,
    int *options_cnt,
    sh_sand_option_t ** all_options);

/**
 * \brief Routine serves to verify correctness of shell command tree and init requested resources
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] sh_sand_cmd_parent - parent command to sh_sand_cmd_a array
 *     \param [in] sh_sand_cmd_a pointer to command list to start from
 *     \param [in] sh_sys_cmd_a pointer to array of system commands assigned per root command
 *     \param [in] command accumulated from shell tree traversing, usually starts from NULL
 *     \param [in] flags verification flags for shell command initialization
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e sh_sand_init(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_parent,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_cmd_t * sh_sys_cmd_a,
    char *command,
    int flags);

/**
 * \brief Parse string to any given type
 * \param [in] unit - unit id
 * \param [in] type - The output type of the value
 * \param [in] source - input value to parse (string)
 * \param [in] target - Union of all kind of things allowing to get any format
 * \param [in] ext_ptr - use for ENUM type translation
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval Other            - Other errors as per shr_error_e
 * \remark
 *
 */
shr_error_e sh_sand_value_get(
    int unit,
    sal_field_type_e type,
    char *source,
    sh_sand_param_u * target,
    void *ext_ptr);

/**
 * \brief * Checks existence of gold result for command
 * \param [in] unit        - device id - important because path will depend on it
 * \param [in] command     - command for which existence of gold is checked
 *
 * \remark
 *
 */
shr_error_e sh_sand_gold_exists(
    int unit,
    char *command);

/**
 * \brief Recursive verification procedure for sand command array
 * \param [in] root_cmd     - root command name for sh_sand_cmds, we need it to have full command line in output
 * \param [in] sh_sand_cmds - sh_sand_cmd_t * - pointer to command array for specific command
 * \param [out] cmd_result - variable that return status value will be saved in
 * \return
 *   Flow goes to exit in any case of failure
 * \remark
 *   Routine should have exit point
 *   Should not be used in JR2
 * \see
 *   sh_sand_init
 */
#define SH_SAND_VERIFY(root_cmd, sh_sand_cmds, cmd_result) \
        {                                                                                           \
            static int sh_sand_legacy_command_verified = FALSE;                                     \
            if(sh_sand_legacy_command_verified == FALSE)                                            \
            {                                                                                       \
                char *command = sal_alloc(SH_SAND_MAX_TOKEN_SIZE, "sh_sand_command_verify");        \
                if(command == NULL)                                                                 \
                {                                                                                   \
                    cli_out("Memory allocation failure\n");                                         \
                    cmd_result = CMD_FAIL;                                                          \
                    goto exit;                                                                      \
                }                                                                                   \
                sal_strncpy(command, root_cmd, SH_SAND_MAX_TOKEN_SIZE -1);                          \
                cmd_result =  diag_sand_error_get(sh_sand_init(unit, NULL, sh_sand_cmds,            \
                                                                   sh_sand_sys_cmds, command, 0));  \
                if(cmd_result == CMD_OK)                                                            \
                {                                                                                   \
                    command[0] = 0;                                                                 \
                    sh_sand_init(unit, NULL, sh_sand_sys_cmds, NULL, command, 0);                   \
                    sh_sand_legacy_command_verified = TRUE;                                         \
                }                                                                                   \
                sal_free(command);                                                                  \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                cmd_result = CMD_OK;                                                                \
            }                                                                                       \
        }

/**
 * \brief Fills string with formated time, oriented for log purposes mainly
 * \param [in,out] time_str - pointer to string where formatted time presentation will be copied
 * \remark
 *   time_str should be at least SH_SAND_MAX_TIME_SIZE
 */
void sh_sand_time_get(
    char *time_str);

/**
 * \brief add option to dynamic argument list of sand control
 * \param [in] unit - unit ID
 * \param [in,out] sand_control - pointer for control structure for command
 * \param [in] option_name -  the name of the option
 * \param [in] option_cb - callback for dynamic option
 */
shr_error_e sh_sand_add_option_dyn_args(
    int unit,
    sh_sand_control_t * sand_control,
    char *option_name,
    sh_sand_option_cb_t option_cb);

#endif /* DIAG_SAND_FRAMEWORK_H_INCLUDED */
