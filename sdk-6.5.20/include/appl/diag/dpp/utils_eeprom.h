/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DPP eeprom utils
 */

/*
 * If LOCK_EEPROM is set 'TRUE' then physical writing to parallel
 * eeprom is protected with the internal 'software data
 * protection'.
 * Since our eeprom blocks are protected by CRC signature,
 * this feature is disabled here.
 * If LOCK_EEPROM is set 'TRUE' then it should be checked on
 * what happens if the unlock sequence is disturbed (say by
 * watchdog interrupt) by another user of the eeprom.
 */
#define LOCK_EEPROM FALSE
/*
 * Number of blocks in EEPROM area which are assigned
 * to boot time. Update when adding new boot-time blocks.
 */
#define NUM_B_EE_BLOCKS     2
/*
 * Number of blocks in EEPROM area which are assigned
 * to boot time and to run time.
 * Update when adding new blocks.
 */
#define NUM_EE_BLOCKS        7
#define SIZEOF_EE_BLOCK_01  30
#define SIZEOF_EE_BLOCK_02 126
#define SIZEOF_EE_BLOCK_03 126
#define SIZEOF_EE_BLOCK_04 142
#define SIZEOF_EE_BLOCK_05 4094
#define SIZEOF_EE_BLOCK_06 254
#define SIZEOF_EE_BLOCK_07 164


/*
 * Maximal number of characters in memory device to
 * contain downloaded application program.
 * This is a string which must be null terminated.
 */
#define MEM_DEVICE_NAME_LEN         10
/*
 * Default value for name of device to contain the
 * file of the application program.
 */
#define B_DEFAULT_MEM_DEVICE_NAME "DUNE_01:"
/*
 * Maximal number of characters in name of download
 * file (on downloading host) which contains the
 * application program.
 * This is a string which must be null terminated.
 */
#define DLD_FILE_NAME_LEN         16
#define B_DEFAULT_DLD_FILE_NAME   "RefDesign_02.zip"
/*
 * Maximal number of characters in name of target
 * board which runs the application program.
 * This is a string which must be null terminated.
 */
#define TARGET_NAME_LEN           10
#define B_DEFAULT_TARGET_NAME     "Dune_FEC"
/*
 * Default value for maximal number of bytes in
 * local ram file to contain the downloaded
 * application.
 * This is also the size reserved on Flash memory
 * for the application program file.
 * Note that the LS 17 bits must be zero
 * and, in any case, are ignored. Address must,
 * then, be a multiple of 128K.
 */
#define B_DEFAULT_MEM_DRV_LEN_4MEG       0x100000
/*1.5 Meg, support both kind of Flash (4 and 8)*/
/*2.5 Meg, support both only one kind of Flash of 8 Mega*/
#define B_DEFAULT_MEM_DRV_LEN_8MEG_VER_B 0x280000
/*4 Meg, not suitable for Flash less then 8 Mega*/
#define B_DEFAULT_MEM_DRV_LEN_8MEG_VER_C 0x400000

/*
 * Default value for maximal number of bytes in
 * local flash memory to contain all application
 * programs.
 * The maximal number of application programs
 * that can be stored in flash memory is
 * floor(app_flash_size/mem_drv_len).
 * Note that the LS 17 bits must be zero
 * and, in any case, are ignored. Address must,
 * then, be a multiple of 128K.
 */
#define B_DEFAULT_APP_FLASH_SIZE_4MEG         (2 * B_DEFAULT_MEM_DRV_LEN_4MEG)
#define B_DEFAULT_APP_FLASH_SIZE_8MEG         (2 * B_DEFAULT_MEM_DRV_LEN_8MEG_VER_B)
#define B_DEFAULT_APP_FLASH_SIZE_8MEG_VER_C   (B_DEFAULT_MEM_DRV_LEN_8MEG_VER_C)

/*
 * Default value for base address of
 * local flash memory that is assigned
 * to contain application programs.
 * Note that the LS 17 bits must be zero
 * and, in any case, are ignored. Address must,
 * then, be a multiple of 128K.
 */
#define B_DEFAULT_APP_FLASH_BASE_4MEG     0xFFC00000
#define B_DEFAULT_APP_FLASH_BASE_8MEG     0xFF800000
/*
 * Default value for where the application
 * program will be taken from.
 * A value of zero is reserved for downloading
 * host. Otherwise, this is the number of
 * Flash memory block. If the indicated block
 * is out of specified Flash bounds, the
 * first applicable value is taken, depending
 * on APP_LOAD_METHOD.
 */
#define B_DEFAULT_APP_SOURCE         0x01
#define METHOD_NEWEST_VERSION        0x00
#define METHOD_FILE_THEN_NEWEST      0x01
#define METHOD_FLASH_THEN_NEWEST     0x02
/*
 * Default value for method of application loading
 * in case indicated source does not contain
 * a legitimate file or in case special method
 * is indicated.
 * For METHOD_NEWEST_VERSION -
 *   All Flash blocks are searched and then Host
 *   file and the one with newwest version
 *   mark is taken.
 * For METHOD_FILE_THEN_NEWEST -
 *   First, indicated source is checked. If
 *   it contains a valid version then this version
 *   and source are used. If no valid file is
 *   found on Flash then Host file is checked and
 *   is taken if it can be found.
 * For METHOD_FLASH_THEN_NEWEST -
 *   First, all Flash fioles are checked. If
 *   any contains a valid version then this version
 *   and this file are used. If no valid file is
 *   found on indicated source then all Flash
 *   blocks are searched and then Host file and
 *   the one with newwest version mark is taken.
 */
#define B_DEFAULT_APP_LOAD_METHOD      METHOD_FLASH_THEN_NEWEST
/*
 * Default value for indication on whether Flash
 * should be automatically loaded from host file
 * if host file is more up to date than oldest
 * file on Flash.
 * The file to replace is always the oldest one
 * (or the first not loaded at all).
 * No Flash file is loaded if the version of the
 * host file is already on one of the Flash files.
 * On final version, set default to FALSE
 */
#define B_DEFAULT_AUTO_FLASH_LOAD      FALSE
/*
 * Default value for indication on whether to
 * load application after startup process or
 * go straight to kernel. Going straight to
 * kernel allows Tornado to communicate with
 * the board using WDB.
 * Values:
 *   SHELL_ONCE -
 *     Go to OS shell after startup but next
 *     startup, return to previously loaded value.
 *   SHELL_THEN_APPLICATION
 *     Go to OS shell after startup but after
 *     timeout, if user gives no indication, go to
 *     application.
 *   STRAIGHT_TO_APPLICATION
 *     Go straight to application after startup.
 * On final version, set default to STRAIGHT_TO_APPLICATION
 */
#define SHELL_ONCE                      BIT(1)
#define SHELL_THEN_APPLICATION          BIT(0)
#define STRAIGHT_TO_APPLICATION         0
#define B_DEFAULT_KERNEL_STARTUP        STRAIGHT_TO_APPLICATION
/*
 * Default value for indication on whether to
 * activate the various caches.
 * For debugging, set both defaults to FALSE.
 * On final version, set both defaults to TRUE
 */
#define B_DEFAULT_ACTIVATE_INST_CACHE  FALSE
#define B_DEFAULT_ACTIVATE_DATA_CACHE  FALSE
/*
 * Default value for console baud rate.
 * Values up to 115200 are allowed.
 */
#define B_DEFAULT_CONSOLE_BAUD_RATE    38400
#define IP_FROM_NV      0
#define IP_FROM_SLOT_ID 1
#define IP_FROM_CRATE 2
/*
 * Default value mode of getting IP address at
 * startup.
 * See explanation for the various modes below.
 * For IP_FROM_NV -
 *   Read IP address from NVRAM.
 * For IP_FROM_SLOT_ID -
 *   Read LS byte of IP address from slot ID
 *   and take the rest as
 *   set_by_user.set_by_user.set_by_user.xx
 *   (xx is slot ID as read from EPLD).
 *   In stand alone mode, always read IP from NVRAM.
 * For IP_FROM_CRATE -
 *   Read LS byte of IP address from slot ID
 *   and take the rest as
 *   CRATE_TYPE.CRATE_ID_MSB.CRATE_ID_LSB.xx
 *   (xx is slot ID as read from EPLD).
 *   In stand alone mode, always read IP from NVRAM.
 */
#define B_DEFAULT_IP_SOURCE_MODE       IP_FROM_NV
#define TEST_IRQ6                      BIT(0)
#define TEST_NMI                       BIT(1)
/*
 * Default value mode for various tests at
 * startup.
 * For TEST_IRQ6 -
 *   Carry out IRQ6 injection and monitoring at startup.
 * For TEST_NMI -
 *   Carry out IRQ0 injection and monitoring at startup.
 * By default, do not
 */
#define B_DEFAULT_TEST_AT_STARTUP      0
/*
 * Default value for startup_counter variable in block
 * one of NVRAM.
 */
#define B_DEFAULT_STARTUP_COUNTER      0x00
/*
 * deafualt values for Base and Option Registesr 4 and 5
 */
#define B_DEFAULT_BASE_REGISTER_04             0x40007001
#define B_DEFAULT_BASE_REGISTER_05             0x50007401
#define B_DEFAULT_OPTION_REGISTER_04           0xFF8007FA
#define B_DEFAULT_OPTION_REGISTER_05           0xFF8007FA
/*
 * Default value for timeout on a live TCP connection
 * (in units of 0.5 seconds).
 * This is the time between two consecutive retries.
 */
#define B_DEFAULT_TCP_TIMEOUT 4
/*
 * Default value for retries on a live TCP connection.
 * After sending specified retries, connection is declared
 * 'Down'.
 */
#define B_DEFAULT_TCP_RETRIES 3
/*
 * Default value for clk_mirror_data is '0'.
 * when zero, the clocks are not affected by the NvRam settings.
 */
#define B_DEFAULT_CLK_MIRROR_DATA 0
/*
 * }
 */
/*
 * Default values for general nv ram fields
 * (block_03)
 * {
 */
/*
 * Default value for indication on whether to
 * use page mode or not.
 */
#define DEFAULT_PAGE_MODE       TRUE
/*
 * Default value for number of lines per page.
 * Meaningful only if 'page_mode' is set.
 */
#define DEFAULT_PAGE_LINES      25
/*
 * Default value for indication on whether to
 * use full line edit (wair for CR before processing) or
 * respond to every character entered.
 */
#define DEFAULT_LINE_MODE             FALSE
/*
 * Default value on whether to activate watchdog when
 * application becomes active.
 * On final version, set default to TRUE
 */
#define DEFAULT_ACTIVATE_WATCHDOG     FALSE
/*
 * Default value for time period for watchdog.
 * Units: 100 milli seconds.
 * Maximal value: 31
 * expiration. Meaningful only if 'ACTIVATE_WATCHDOG'
 * is set.
 */
#define DEFAULT_WATCHDOG_PERIOD       25
/*
 * Timeout on standard Telnet activity in seconds.
 * After this time of inactivity, telnet will
 * be disconnected.
 */
#define DEFEULT_TELNET_ACTIVITY_TIMEOUT_STD  20
/*
 * Timeout on Telnet activity, in seconds, in case
 * user interface system is in a 'continuous display'
 * state. Usually, this is a value larger than
 * the standard timeout.
 * After this time of inactivity, telnet will
 * be disconnected.
 */
#define DEFAULT_TELNET_ACTIVITY_TIMEOUT_CONT 100
/*
 * Event error level at and above which events are
 * displayed on the screen.
 */
#define DEFAULT_EVENT_DISPLAY_LEVEL          SVR_WRN
/*
 * Event error level at and above which events are
 * stored in NVRAM log.
 */
#define DEFAULT_EVENT_NVRAM_LEVEL            SVR_ERR
/*
 * Event error level at and above which events are
 * sent to a syslog server.
 */
#define DEFAULT_EVENT_SYSLOG_LEVEL           SVR_LAST
/*
 * Debug tool. Specified debug level for Marvell's DPSS.
 * Depending on its value, various display option will
 * be activated at runtime.
 * A value of '0' indicates 'no display option active'.
 */
#define DEFAULT_DPSS_DISPLAY_LEVEL           0
/*
 * Debug tool. Specified debug level for Marvell's DPSS.
 * Depending on its value, various debug option will
 * be activated at runtime.
 * A value of '0' indicates 'no debug option active'.
 */
#define DEFAULT_DPSS_DEBUG_LEVEL             0
/*
 * Demo tool. Tells tha application whether to start
 * in demo mode - and what type of demo. Currently,
 * only "negev_demo" is supported, but this nv val,
 * will allow future releases to support more demo
 * modes. '0' indicates - no demo.
 */
#define DEFAULT_START_APPLICATION_IN_DEMO_MODE  0
/*
 * Flag. If set 'true' (on) then at startup a default
 * cli file is run
 */
#define DEFAULT_RUN_UI_STARTUP_SCRIPT       FALSE
/*
 * Flag. If set 'true' (on) then Marvell's DPSS
 * data structures are initialized from a file on flash.
 */
#define DEFAULT_DPSS_LOAD_FROM_FILE         FALSE
/*
 * Flag indicating whether mibs should be read
 * from downloading host (using TFTP) at startup.
 */
#define DEFAULT_DOWNLOAD_MIBS                FALSE
/*
 * Flag indicating whether to load the Net-SNMP package at run time
 */
#define DEFAULT_LOAD_NETSNMP                 FALSE
/*
 * Size on Nvram of the NetSNMP command line
 */
#define SNMP_CMD_LINE_SIZE                   128
#define DEFAULT_SNMP_CMD_LINE                ""
/*
 * Default value on whether to background collect temperature.
 * Enable disable background collection temperature.
 * If disabled:
 *  1. No I2C transaction toward the temperature.
 *  2. No valid temperature is given when asked.
 *  3. Original intension is to reduced the I2C
 *     transactions for hardware debugging.
 */
#define DEFAULT_BCKG_TEMPERATURES_EN      TRUE
/*
 * Default value on whether to hook our software exception handler.
 * The exception includes:
 *  1. Software emulation (0x1000).
 *  2. Alignment (0x600).
 *  3. DSI (0x300).
 *  4. ISI (0x400).
 */
#define DEFAULT_HOOK_SOFTWARE_EXCEPTION   TRUE
/*
 * }
 */
/*
 * Default values for nv ram fields related to
 * DUNE chip set.
 * {
 */
/*
 * Default value for indication on whether to
 * load 'configuration' register of FE chip
 * from NV ram or leave it (to load from MD).
 */
#define DEFAULT_CONF_FROM_MD         TRUE
/*
 * Default value for 'configuration' register
 * of FE chip:
 * Chip id = 1
 * Mode FE2 = 1
 * Repeater = 0
 * Planes = one plane
 */
#define DEFAULT_FE_CONFIGURATION      0x1001
/*
 * }
 */
/*
 * Definitions related to NMI/reset/watchdog exception
 * block (block_04).
 * {
 */
/*
 * Default values for NV ram fields related to
 * NMI/reset/watchdog exception block.
 * {
 */
/*
 * Default value for 'new_n_valid' element:
 * If set to '0' then this block contains
 * no information (either there has not been
 * any exception or data of last exception
 * has been displayed on the screen and transferred
 * to the 'log' fifo.
 */
#define DEFAULT_NEW_N_VALID        0
#define DEFAULT_ERROR_IDENTIFIER   0

/*
 * HOST_EEPROM_FRONT_CARD_TYPE_TYPE enum.
 * The front line cards to work with the GFA.
 * The enum is valid only for front line cards
 * outside from dune.
 */
typedef enum
{
  HOST_EEPROM_FRONT_CARD_TYPE_NONE=0,
  HOST_EEPROM_FRONT_CARD_TYPE_X10,
  HOST_EEPROM_FRONT_CARD_TYPE_IXP2800,

  HOST_EEPROM_FRONT_CARD_TYPE_NOF
} HOST_EEPROM_FRONT_CARD_TYPE ;
/*
 * The default front card type is NONE.
 * Should only be changed when the GFA works
 * with non-Dune front line cards.
 */


/*
 * Definitions related to NVRAM event log
 * block (block_05).
 * {
 */
/*
 * Number of elements in event log. Make sure
 * size of event log array is not larger than size
 * assigned to nvram log file in all.
 */
#define NUM_EVENT_LOG_ITEMS       30
/*
 * }
 */
typedef struct
{
  unsigned long startup_counter ;
} __ATTRIBUTE_PACKED__ BLOCK_01  ;
typedef struct
{
  unsigned long  ip_addr ;
  unsigned long  ip_mask ;
  unsigned long  default_gateway_addr ;
  unsigned long  downloading_host_addr ;
  unsigned long  mem_drv_len ;
  unsigned long  app_flash_size ;
  char           *app_flash_base ;
  unsigned long  console_baud_rate ;
  unsigned char  app_source ;
  unsigned char  app_load_method ;
  unsigned char  auto_flash_load ;
  unsigned char  kernel_startup ;
  char           mem_device_name[MEM_DEVICE_NAME_LEN + 1] ;
  char           dld_file_name[DLD_FILE_NAME_LEN + 1] ;
  char           target_name[TARGET_NAME_LEN + 1] ;
  unsigned char  activate_inst_cache ;
  unsigned char  activate_data_cache ;
  unsigned char  ip_source_mode ;
  unsigned char  test_at_startup ;
  /* 
   * The following block of params are for the base / option regsiters needed for
   * the test chip board
   */
  unsigned long  base_register_04 ;
  unsigned long  base_register_05 ;
  unsigned long  option_register_04 ;
  unsigned long  option_register_05 ;
  unsigned char  tcp_timeout ;
  unsigned char  tcp_retries ;
  unsigned long  debug_register_01;
} __ATTRIBUTE_PACKED__ BLOCK_02  ;
typedef struct
{
  unsigned char  page_mode ;
  unsigned char  page_lines ;
  unsigned char  conf_from_md[NUM_FE_ON_BOARD] ;
  unsigned short fe_configuration[NUM_FE_ON_BOARD] ;
  unsigned char  line_mode ;
  unsigned char  activate_watchdog ;
  unsigned char  watchdog_period ;
    /*
     * Timeout on standard Telnet activity in seconds.
     * After this time of inactivity, telnet will
     * be disconnected.
     */
  unsigned char  telnet_activity_timeout_std ;
    /*
     * Timeout on Telnet activity, in seconds, in case
     * user interface system is in a 'continuous display'
     * state. Usually, this is a value larger than
     * the standard timeout.
     * After this time of inactivity, telnet will
     * be disconnected.
     */
  unsigned char  telnet_activity_timeout_cont ;
    /*
     * Minimal severity, above which events are displayed
     * (related to gen_err).
     */
  unsigned char  event_display_level ;
  unsigned char  event_nvram_level ;
    /*
     * Flag. If set to non-zero:
     * + SOC_SAND_FAP10M system: Marvell's DPSS is initialized and started at startup.
     * + SOC_SAND_FAP20V system: sweep application is initialized and started at startup.
     * Refer to enumerator 'D_APPLICATION_ACTIVATE' for different kinds of application flavors.
     */
  unsigned char  usr_app_flavor ;
    /*
     * Debug tool. Specified debug level for Marvell's DPSS.
     * Depending on its value, various display option will
     * be activated at runtime.
     */
  unsigned char  dpss_display_level ;
    /*
     * Debug tool. Specified debug level for Marvell's DPSS.
     * Depending on its value, various debug option will
     * be activated at runtime.
     */
  unsigned char  dpss_debug_level ;
    /*
     * Hook our exception handler regarding the 
     *  1. Software Emulation Exception
     *  2. Alignment (0x600).
     *  3. DSI (0x300).
     *  4. ISI (0x400).
     */
  unsigned char  hook_software_exception ;
    /*
     * Enable disable background collection temperature.
     * If disabled:
     *  1. No I2C transaction toward the temperature.
     *  2. No valid temperature is given when asked.
     *  3. Original intension is to reduced the I2C 
     *     transactions for hardware debugging.
     */
  unsigned char  bckg_temperatures_en ;
  /*
   * Enables running of default cli script at startup.
   */
  unsigned char  run_ui_startup_script;
  /*
   * Enables loading of dpss config file (dune_rpc.cfg)
   * from the flash instead of from tftp.
   */
  unsigned char  load_dpss_from_file_on_flash;
  /*
   * Enables starting the application in demo mode
   * Currently only "negev_demo" is supported.
   */
  unsigned char  start_application_in_demo_mode;
  /*
   * Determine if the SOC_SAND_FAP10M is accessing the device
   * via the ECI/PCI interface.
   */
  unsigned char  fap10m_run_mode;

  /*
   * Minimal severity, above which events are sent to
   * syslog server (related to gen_err).
   */
  unsigned char  event_syslog_level ;


} __ATTRIBUTE_PACKED__ BLOCK_03  ;
typedef struct
{
    /*
     * Internal error identifier of this exception.
     * If this value is '0' then no meaningful
     * exception description is contained in this
     * block.
     */
  unsigned long  error_identifier ;
    /*
     * Number of system startup at which this exception
     * occurred. Value taken from NV ram, block 01.
     */
  unsigned long  startup_count ;
    /*
     * Time from last startup (when reset exception
     * has occurred), in units of 10 milli seconds.
     */
  unsigned long  time_from_startup ;
    /*
     * Time of day (when reset exception
     * has occurred), in units T.B.D.
     */
  unsigned long  tod[2] ;
    /*
     * Name identifier of task (where reset exception
     * has occurred).
     */
  char           task_name[10] ;
    /*
     * Value of program counter where reset exception
     * has occurred.
     */
  unsigned long  program_counter ;
    /*
     * Number identifier of task (where reset exception
     * has occurred).
     */
  int            task_id ;
    /*
     * Free text description of exception. Null terminated
     * string.
     */
  char           nmi_text[80] ;
    /*
     * Flag. If non-zero then this exception should also
     * trigger an alarm.
     */
  unsigned char  trigger_alarm ;
    /*
     * Counter. If, somehow, a few reset exceptions
     * occur before actual hardware reset hits the
     * system then only information concerning the last one
     * is stored in this block. However, this counter
     * is incremented, in that case.
     */
  unsigned char  exception_counter ;
    /*
     * Flag. If non-zero then all fields in this block,
     * assigned to reset/watchdog exception, are valid
     * and should be displayed on the screen and moved
     * to the NV ram 'log'.
     * It is up to the writer of the exception handler
     * to fill all other elements with meaningful information
     * before asserting this field.
     */
  unsigned char  new_n_valid ;
    /*
     * Identifier of boot version used when exception
     * was detected.
     */
  unsigned short boot_ver ;
    /*
     * Identifier of application version used when
     * exception was detected.
     */
  unsigned short prog_ver ;
    /*
     * Time elapsed from restarting the watchdog
     * timer (until reset exception has occurred),
     * in units of milli seconds.
     */
  unsigned long  time_from_wd_restart ;
    /*
     * Time elapsed from start to end of watchdog/NMI
     * interrupt handler,
     * in units of milli seconds.
     */
  unsigned long  handler_processing_time ;
} __ATTRIBUTE_PACKED__ BLOCK_04  ;
typedef struct
{
    /*
     * Full identifier of the event
     */
  int event_id ;
    /*
     * Time from last startup when event was detected
     * (displayed/stored), in units of 10 milli seconds.
     */
  unsigned long  time_from_startup ;
    /*
     * Error reported by OS when event was first
     * reported.
     */
  int  vx_works_error ;
    /*
     * Identifier of boot version used when event
     * was detected.
     */
  unsigned short  boot_ver ;
    /*
     * Identifier of application version used when
     * event was detected.
     */
  unsigned short  prog_ver ;
    /*
     * Number of system startup at which this exception
     * occurred. Value taken from NV ram, block 01.
     */
  unsigned short startup_count ;
    /*
     * This byte contains an indication on whether
     * this event is an alarm and, if so, whether it is
     * a 'set alarm' or a 'reset alarm' event as
     * follows:
     *   SET_ALARM         BIT(0)
     *   RESET_ALARM       BIT(1)
     *   ALARM_EVENT       (SET_ALARM | RESET_ALARM)
     *   ALARM_TYPE_MASK   0x0000FF00
     * If ALARM_EVENT is zero then this is not an alarm event.
     * If it is an alarm event then ALARM_TYPE_MASK contains
     * an identifier of the alarm (in addition to event_id)
     */
  unsigned short   alarm_flags ;
    /*
     * Severity of event.
     * Values
     *   SVR_MSG 0
     *   SVR_WRN 1
     *   SVR_ERR 2
     *   SVR_FTL 3
     */
  char severity ;
    /*
     * Number of times event was found to have repeated
     * when it was .
     */
  unsigned char num_accumulated ;
    /*
     * Text to display when scanning of events (for aging) finds
     * an event which has non-zero num_accumulated element and
     * which is still pending.
     */
  char event_text[80] ;
    /*
     * Name of task at which event was detected.
     */
  char task_name[10] ;
    /*
     * Name of procedure at which event was detected.
     */
  char proc_name[16] ;
} __ATTRIBUTE_PACKED__ EVENT_LOG_ITEM  ;
typedef struct
{
    /*
     * Index in array 'event_log_item' of next item
     * to load.
     */
  unsigned char next_in_index ;
    /*
     * Number of meaningful events logged in array
     * 'event_log_item'. Events are stored cyclicly.
     * For example, if 'NUM_EVENT_LOG_ITEMS' is 20,
     * 'next_in_index' is 5 and 'num_events_logged'
     * is 7 then the index of the first meaningful
     * entry is 18. 
     */
  unsigned char num_events_logged ;
    /*
     * Array of events pushed into this log file.
     */
  EVENT_LOG_ITEM event_log_item[NUM_EVENT_LOG_ITEMS] ;
} __ATTRIBUTE_PACKED__ BLOCK_05  ;
typedef struct
{
  unsigned char  load_netsnmp ;
  unsigned char  download_mibs ;
  char           snmp_cmd_line[SNMP_CMD_LINE_SIZE] ;
  /*
   */
} __ATTRIBUTE_PACKED__ BLOCK_06  ;
typedef struct
{
    /*
     * Internal error identifier of this exception.
     * If this value is '0' then no meaningful
     * exception description is contained in this
     * block.
     */
  unsigned long  error_identifier ;
    /*
     * Number of system startup at which this exception
     * occurred. Value taken from NV ram, block 01.
     */
  unsigned long  startup_count ;
    /*
     * Time from last startup (when reset exception
     * has occurred), in units of 10 milli seconds.
     */
  unsigned long  time_from_startup ;
    /*
     * Time of day (when reset exception
     * has occurred), in units T.B.D.
     */
  unsigned long  tod[2] ;
    /*
     * Name identifier of task (where reset exception
     * has occurred).
     */
  char           task_name[10] ;
    /*
     * Value of program counter where reset exception
     * has occurred.
     */
  unsigned long  program_counter ;
    /*
     * Number identifier of task (where reset exception
     * has occurred).
     */
  int            task_id ;
    /*
     * Free text description of exception. Null terminated
     * string.
     */
  char           text[80] ;
    /*
     * Flag. If non-zero then this exception should also
     * trigger an alarm.
     */
  unsigned char  trigger_alarm ;
    /*
     * Counter. If, somehow, a few reset exceptions
     * occur before actual hardware reset hits the
     * system then only information concerning the last one
     * is stored in this block. However, this counter
     * is incremented, in that case.
     */
  unsigned char  exception_counter ;
    /*
     * Flag. If non-zero then all fields in this block,
     * assigned to exception, are valid
     * and should be displayed on the screen and moved
     * to the NV ram 'log'.
     * It is up to the writer of the exception handler
     * to fill all other elements with meaningful information
     * before asserting this field.
     */
  unsigned char  new_n_valid ;
    /*
     * Identifier of boot version used when exception
     * was detected.
     */
  unsigned short boot_ver ;
    /*
     * Identifier of application version used when
     * exception was detected.
     */
  unsigned short prog_ver ;
    /*
     * Time elapsed from start to end of exception
     * interrupt handler,
     * in units of milli seconds.
     */
  unsigned long  handler_processing_time ;
    /*
     * Value of 'Machine State Register' (MSR) at the 
     * time of the excpetion.
     */
  unsigned long  msr_register ;
    /*
     * Registers R0-R5
     */
  unsigned long  registers_0_to_4[5] ;
    /*
     * Registers R29-R31
     */
  unsigned long  registers_30_to_31[2] ;
} __ATTRIBUTE_PACKED__ BLOCK_07  ;

typedef union
{
  BLOCK_01  block_01 ;
  unsigned char fill[SIZEOF_EE_BLOCK_01] ;
} __ATTRIBUTE_PACKED__ UN_EE_BLOCK_01 ;
typedef union
{
  BLOCK_02  block_02 ;
  unsigned char fill[SIZEOF_EE_BLOCK_02] ;
} __ATTRIBUTE_PACKED__ UN_EE_BLOCK_02 ;
typedef union
{
  BLOCK_03  block_03 ;
  unsigned char fill[SIZEOF_EE_BLOCK_03] ;
} __ATTRIBUTE_PACKED__ UN_EE_BLOCK_03 ;
typedef union
{
  BLOCK_04  block_04 ;
  unsigned char fill[SIZEOF_EE_BLOCK_04] ;
} __ATTRIBUTE_PACKED__ UN_EE_BLOCK_04 ;
typedef union
{
  BLOCK_05  block_05 ;
  unsigned char fill[SIZEOF_EE_BLOCK_05] ;
} __ATTRIBUTE_PACKED__ UN_EE_BLOCK_05 ;
typedef union
{
  BLOCK_06  block_06 ;
  unsigned char fill[SIZEOF_EE_BLOCK_06] ;
} __ATTRIBUTE_PACKED__ UN_EE_BLOCK_06 ;
typedef union
{
  BLOCK_07  block_07 ;
  unsigned char fill[SIZEOF_EE_BLOCK_07] ;
} __ATTRIBUTE_PACKED__ UN_EE_BLOCK_07 ;
typedef struct
{
  UN_EE_BLOCK_01 un_ee_block_01 ;
  unsigned short crc16 ;
} __ATTRIBUTE_PACKED__ EE_BLOCK_01  ;
typedef struct
{
  UN_EE_BLOCK_02 un_ee_block_02 ;
  unsigned short crc16 ;
} __ATTRIBUTE_PACKED__ EE_BLOCK_02  ;
typedef struct
{
  UN_EE_BLOCK_03 un_ee_block_03 ;
  unsigned short crc16 ;
} __ATTRIBUTE_PACKED__ EE_BLOCK_03  ;
typedef struct
{
  UN_EE_BLOCK_04 un_ee_block_04 ;
  unsigned short crc16 ;
} __ATTRIBUTE_PACKED__ EE_BLOCK_04  ;
typedef struct
{
  UN_EE_BLOCK_05 un_ee_block_05 ;
  unsigned short crc16 ;
} __ATTRIBUTE_PACKED__ EE_BLOCK_05  ;
typedef struct
{
  UN_EE_BLOCK_06 un_ee_block_06 ;
  unsigned short crc16 ;
} __ATTRIBUTE_PACKED__ EE_BLOCK_06  ;
typedef struct
{
  UN_EE_BLOCK_07 un_ee_block_07 ;
  unsigned short crc16 ;
} __ATTRIBUTE_PACKED__ EE_BLOCK_07  ;
typedef struct
{
  EE_BLOCK_01 ee_block_01 ;
  EE_BLOCK_02 ee_block_02 ;
  EE_BLOCK_03 ee_block_03 ;
  EE_BLOCK_04 ee_block_04 ;
  EE_BLOCK_05 ee_block_05 ;
  EE_BLOCK_06 ee_block_06 ;
  EE_BLOCK_07 ee_block_07 ;
} __ATTRIBUTE_PACKED__ EE_AREA  ;

/*
 * Definitions related to Serial EEPROM chip (AT24C04, I2C)
 * Chip has 512 non-volatile bytes.
 * {
 */
#define SER_EE_POWER_OF_SIZE                       9
#define SER_EPROM_I2C_ADDRESS_WRITE                0xA0
/*
 */
#define HOST_SER_EE_POWER_OF_SIZE                  9
#define HOST_SER_EPROM_I2C_ADDRESS_WRITE           0xA4
/*
 * I2C pasrameters for NVRAM on front end card.
 */
#define FRONT_SER_EE_POWER_OF_SIZE                 9
#define FRONT_SER_EPROM_I2C_ADDRESS_READ           0xA9
/*
 * I2C pasrameters for NVRAM on host_db card.
 */
#define HOST_DB_SER_EE_POWER_OF_SIZE               9
/*
 * Size of serial eeprom must be a multiple
 * of block size, SER_EPROM_BLOCK_SIZE.
 */
#define SIZEOF_SERIAL_EE                  B_BIT(SER_EE_POWER_OF_SIZE)
/*
 * Load high bits of offset in serial eeprom (src)
 * into address (dest). Note that src is at least two
 * bytes long while dest is one byte long.
 */
#define SER_EPROM_BLOCK_SIZE              16
/*
 * Number of blocks in serial EEPROM area which
 * are assigned to boot time and to run time.
 * Update when changing/adding new blocks.
 * Make sure blocks (with CRC) are multiples
 * of SER_EPROM_BLOCK_SIZE
 */
#define NUM_SERIAL_EE_BLOCKS         2
#define SIZEOF_SERIAL_EE_BLOCK_01    ((SER_EPROM_BLOCK_SIZE * 1) - 2)
#define SIZEOF_SERIAL_EE_BLOCK_02    ((SER_EPROM_BLOCK_SIZE * 5) - 2)
#define SIZEOF_SERIAL_EE_FILLER      \
          SIZEOF_SERIAL_EE - (SIZEOF_SERIAL_EE_BLOCK_01+2 + SIZEOF_SERIAL_EE_BLOCK_02+2)
/*
 * Number of blocks in HOST serial EEPROM area which
 * are assigned to boot time and to run time.
 * Update when changing/adding new blocks.
 * Make sure blocks (with CRC) are multiples
 * of SER_EPROM_BLOCK_SIZE
 */
#define SIZEOF_HOST_SERIAL_EE             B_BIT(HOST_SER_EE_POWER_OF_SIZE)
#define HOST_NUM_SERIAL_EE_BLOCKS         2
#define HOST_SIZEOF_SERIAL_EE_BLOCK_01    ((SER_EPROM_BLOCK_SIZE * 1) - 2)
#define HOST_SIZEOF_SERIAL_EE_BLOCK_02    ((SER_EPROM_BLOCK_SIZE * 20) - 2)
#define SIZEOF_HOST_SERIAL_EE_FILLER      \
          SIZEOF_HOST_SERIAL_EE -  \
          (HOST_SIZEOF_SERIAL_EE_BLOCK_01+ 2 + \
          HOST_SIZEOF_SERIAL_EE_BLOCK_02 + 2)
/*
 * Number of blocks in FRONT serial EEPROM area which
 * are assigned to boot time and to run time.
 * Update when changing/adding new blocks.
 * Make sure blocks (with CRC) are multiples
 * of SER_EPROM_BLOCK_SIZE
 */
#define SIZEOF_FRONT_SERIAL_EE             B_BIT(FRONT_SER_EE_POWER_OF_SIZE)
#define FRONT_NUM_SERIAL_EE_BLOCKS         2
#define FRONT_SIZEOF_SERIAL_EE_BLOCK_01    ((SER_EPROM_BLOCK_SIZE * 1) - 2)
#define FRONT_SIZEOF_SERIAL_EE_BLOCK_02    ((SER_EPROM_BLOCK_SIZE * 20) - 2)
#define SIZEOF_FRONT_SERIAL_EE_FILLER      \
          SIZEOF_FRONT_SERIAL_EE -  \
          (FRONT_SIZEOF_SERIAL_EE_BLOCK_01+ 2 + \
          FRONT_SIZEOF_SERIAL_EE_BLOCK_02 + 2)

/*
 * Number of blocks in HOST_DB serial EEPROM area which
 * are assigned to boot time and to run time.
 * Update when changing/adding new blocks.
 * Make sure blocks (with CRC) are multiples
 * of SER_EPROM_BLOCK_SIZE
 */
#define SIZEOF_HOST_DB_SERIAL_EE           B_BIT(HOST_DB_SER_EE_POWER_OF_SIZE)
#define HOST_DB_NUM_SERIAL_EE_BLOCKS         2
#define HOST_DB_SIZEOF_SERIAL_EE_BLOCK_01    ((SER_EPROM_BLOCK_SIZE * 1) - 2)
#define HOST_DB_SIZEOF_SERIAL_EE_BLOCK_02    ((SER_EPROM_BLOCK_SIZE * 20) - 2)
#define SIZEOF_HOST_DB_SERIAL_EE_FILLER      \
          SIZEOF_HOST_DB_SERIAL_EE -  \
          (HOST_DB_SIZEOF_SERIAL_EE_BLOCK_01+ 2 + \
          HOST_DB_SIZEOF_SERIAL_EE_BLOCK_02 + 2)
/*
 * for the standard nvram handling system, block identifiers
 * of serial eeprom start at 'SERIAL_EE_FIRST_BLOCK_ID' while
 * offest starts at 'SERIAL_EE_FIRST_OFFSET' (Make sure there is no
 * overlap with other NVRAM systems!).
 * Currently the only other system is the parallel eeprom.
 */
#define SERIAL_EE_FIRST_BLOCK_ID          100
#define SERIAL_EE_FIRST_OFFSET            NV_RAM_SIZE
#define HOST_SERIAL_EE_FIRST_BLOCK_ID     200
#define FRONT_SERIAL_EE_FIRST_BLOCK_ID    300
#define HOST_DB_SERIAL_EE_FIRST_BLOCK_ID   400

/*
 * Maximal number of characters on name of board type
 * (including ending null).
 */
#define B_MAX_BOARD_NAME_LEN         12
/*
 * Default value for name of this board type.
 * Suffix 'new' specifies new PCI board.
 */
#define B_DEFAULT_BOARD_NAME         "BRD0001A"
#define B_DEFAULT_BOARD_NAME_NEW     "BRD0011A"
/*
 * Maximal number of characters on description of board type
 * (including ending null).
 */
#define B_MAX_BOARD_DESCRIPTION_LEN  40
/*
 * Default value for name of this board type.
 * Suffix 'new' specifies new PCI board.
 */
#define B_DEFAULT_BOARD_DESCRIPTION      "CPU mezzanine"
#define B_DEFAULT_BOARD_DESCRIPTION_NEW  "CPU-PMC"
/*
 * Default value for identifier of hardware of this board.
 * Suffix 'new' specifies new PCI board.
 */
#define B_DEFAULT_BOARD_SERIAL_NUMBER 0x0001
#define B_DEFAULT_BOARD_SERIAL_NUMBER_NEW 0x0001
/*
 * Default value for hardware version of this board.
 * Suffix 'new' specifies new PCI board.
 */
#define B_DEFAULT_BOARD_VERSION       101
#define B_DEFAULT_BOARD_VERSION_NEW   102
/*
 * HOST serial eeprom - default values
 * {
 */
/*
 * Maximal number of characters on host catalaog number.
 * (including ending null).
 */
#define B_HOST_MAX_CATALOG_NUMBER_LEN     9
/*
 * Default value for host catalaog number.
 */
#define B_DEFAULT_HOST_CATALOG_NUMBER ""
/*
 * Maximal number of characters on description of host board type
 * (including ending null).
 */
#define B_HOST_MAX_BOARD_DESCRIPTION_LEN  50
/*
 * Default value for host board description.
 */
#define B_DEFAULT_HOST_BOARD_DESCRIPTION ""
/*
 * Maximal number of characters on host board version
 */
#define B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN  7
/*
 * Default value for host board description.
 */
#define B_DEFAULT_HOST_BOARD_VERSION ""
/*
 * Maximal number of characters on empty param
 */
#define B_HOST_MAX_PARAM_LEN  20
/*
 * Default value for host board description.
 */
#define B_DEFAULT_ENPTY_PARAM ""
/*
 * Default value for host board description.
 */
#define B_DEFAULT_HOST_BOARD_SN 0x0000
/*
 * Default value for host board type.
 */
#define B_DEFAULT_HOST_BOARD_TYPE 0x0000
/*
 * }
 */
/*
 * FRONT serial eeprom - default values
 * {
 */
/*
 * FRONT_CARD_TYPE:
 * Front line cards produced by Dune.
 * See definitions below:
 *   FRONT_END_FTG         2200
 *   FRONT_END_TGS         2300
 */
/*
 * Maximal number of characters on front catalaog number.
 * (including ending null).
 */
#define B_FRONT_MAX_CATALOG_NUMBER_LEN     9
/*
 * Default value for front catalaog number.
 */
#define B_DEFAULT_FRONT_CATALOG_NUMBER ""
/*
 * Maximal number of characters on description of front board type
 * (including ending null).
 */
#define B_FRONT_MAX_BOARD_DESCRIPTION_LEN      50
/*
 * Default value for front board description.
 */
#define B_DEFAULT_FRONT_BOARD_DESCRIPTION      ""
/*
 * Maximal number of characters on front board version
 */
#define B_FRONT_MAX_DEFAULT_BOARD_VERSION_LEN  7
/*
 * Default value for front board description.
 */
#define B_DEFAULT_FRONT_BOARD_VERSION          ""
/*
 * Maximal number of characters on empty param
 */
#define B_FRONT_MAX_PARAM_LEN      20
/*
 * Default value for front board description.
 */
#define B_DEFAULT_ENPTY_PARAM      ""
/*
 * Default value for front board description.
 */
#define B_DEFAULT_FRONT_BOARD_SN   0x0000
/*
 * Default value for front board type.
 */
#define B_DEFAULT_FRONT_BOARD_TYPE SOC_UNKNOWN_CARD
/*
 * }
 */
/*
 * HOST_DB serial eeprom - default values
 * {
 */
/*
 * HOST_DB_CARD_TYPE:
 * Host_db line cards produced by Dune.
 * See definitions below:
 */
/*
 * Maximal number of characters on host_db catalaog number.
 * (including ending null).
 */
#define B_HOST_DB_MAX_CATALOG_NUMBER_LEN     9
/*
 * Default value for host_db catalaog number.
 */
#define B_DEFAULT_HOST_DB_CATALOG_NUMBER ""
/*
 * Maximal number of characters on description of host_db board type
 * (including ending null).
 */
#define B_HOST_DB_MAX_BOARD_DESCRIPTION_LEN      50
/*
 * Default value for host_db board description.
 */
#define B_DEFAULT_HOST_DB_BOARD_DESCRIPTION      ""
/*
 * Maximal number of characters on host_db board version
 */
#define B_HOST_DB_MAX_DEFAULT_BOARD_VERSION_LEN  7
/*
 * Default value for host_db board description.
 */
#define B_DEFAULT_HOST_DB_BOARD_VERSION          ""
/*
 * Maximal number of characters on empty param
 */
#define B_HOST_DB_MAX_PARAM_LEN      20
/*
 * Default value for host_db board description.
 */
#define B_DEFAULT_ENPTY_PARAM      ""
/*
 * Default value for host_db board description.
 */
#define B_DEFAULT_HOST_DB_BOARD_SN   0x0000
/*
 * Default value for host_db board type.
 */
#define B_DEFAULT_HOST_DB_BOARD_TYPE SOC_UNKNOWN_CARD
/*
 * }
 */

typedef struct
{
  char eth_addr[MAC_ADRS_LEN] ;
} __ATTRIBUTE_PACKED__ SERIAL_EE_BLOCK_01  ;
typedef struct
{
  unsigned short  board_serial_number ;
  unsigned short  board_version ;
  char            board_description[B_MAX_BOARD_DESCRIPTION_LEN] ;
  char            board_name[B_MAX_BOARD_NAME_LEN] ;
} __ATTRIBUTE_PACKED__ SERIAL_EE_BLOCK_02  ;
typedef union
{
  SERIAL_EE_BLOCK_01  serial_ee_block_01 ;
  unsigned char fill[SIZEOF_SERIAL_EE_BLOCK_01] ;
} __ATTRIBUTE_PACKED__ UN_SERIAL_EE_BLOCK_01 ;
typedef union
{
  SERIAL_EE_BLOCK_02  serial_ee_block_02 ;
  unsigned char fill[SIZEOF_SERIAL_EE_BLOCK_02] ;
} __ATTRIBUTE_PACKED__ UN_SERIAL_EE_BLOCK_02 ;
typedef struct
{
  UN_SERIAL_EE_BLOCK_01 un_serial_ee_block_01 ;
  unsigned short crc16 ;
} __ATTRIBUTE_PACKED__ SER_EE_BLOCK_01  ;
typedef struct
{
  UN_SERIAL_EE_BLOCK_02 un_serial_ee_block_02 ;
  unsigned short crc16 ;
} __ATTRIBUTE_PACKED__ SER_EE_BLOCK_02  ;
typedef struct
{
  unsigned char fill[SIZEOF_SERIAL_EE_FILLER];
} __ATTRIBUTE_PACKED__ SER_EE_BLOCK_03 ;
/*
 * Definitions related to HOST serial eeprom
 * {
 */
typedef struct
{
  unsigned short host_board_type ;
} __ATTRIBUTE_PACKED__ HOST_SERIAL_EE_BLOCK_01  ;
typedef struct
{
  char  host_board_catalog_number[B_HOST_MAX_CATALOG_NUMBER_LEN];
  char  host_board_description[B_HOST_MAX_BOARD_DESCRIPTION_LEN];
  char  host_board_version[B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN];
  unsigned short host_board_serial_number;
  /*
   * On BRD0012A this is the C2C Clock rate in MHZ
   */
  unsigned short fap_pp_interface_clk;
  /*
   * On BRD0012A this is an indicator, to load or not DFCDLs values.
   * 1 - use board EEPROM. Do not load in software.
   * 0 - Software do all.
   */
  unsigned short  fap_use_eeprom_vals;
  unsigned short  eeprom_front_card_type;
  char  host_board_param_1[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_2[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_3[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_4[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_5[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_6[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_7[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_8[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_9[B_HOST_MAX_PARAM_LEN];
  char  host_board_param_10[B_HOST_MAX_PARAM_LEN];
} __ATTRIBUTE_PACKED__ HOST_SERIAL_EE_BLOCK_02  ;
typedef union
{
  HOST_SERIAL_EE_BLOCK_01  host_serial_ee_block_01 ;
  unsigned char fill[HOST_SIZEOF_SERIAL_EE_BLOCK_01] ;
} __ATTRIBUTE_PACKED__ UN_HOST_SERIAL_EE_BLOCK_01 ;
typedef union
{
  HOST_SERIAL_EE_BLOCK_02  host_serial_ee_block_02 ;
  unsigned char fill[HOST_SIZEOF_SERIAL_EE_BLOCK_02] ;
} __ATTRIBUTE_PACKED__ UN_HOST_SERIAL_EE_BLOCK_02 ;
typedef struct
{
  UN_HOST_SERIAL_EE_BLOCK_01 un_host_serial_ee_block_01 ;
  unsigned short crc16 ;
} __ATTRIBUTE_PACKED__ HOST_SER_EE_BLOCK_01  ;
typedef struct
{
  UN_HOST_SERIAL_EE_BLOCK_02 un_host_serial_ee_block_02 ;
  unsigned short crc16 ;
} __ATTRIBUTE_PACKED__ HOST_SER_EE_BLOCK_02  ;
typedef struct
{
  unsigned char fill[SIZEOF_HOST_SERIAL_EE_FILLER];
} __ATTRIBUTE_PACKED__ HOST_SER_EE_BLOCK_03;
/*
 * }
 */
/*
 * Definitions related to FRONT serial eeprom
 * {
 */
typedef struct
{
  unsigned short front_board_type ;
} __ATTRIBUTE_PACKED__ FRONT_SERIAL_EE_BLOCK_01  ;
typedef struct
{
  char  front_board_catalog_number[B_FRONT_MAX_CATALOG_NUMBER_LEN] ;
  char  front_board_description[B_FRONT_MAX_BOARD_DESCRIPTION_LEN] ;
  char  front_board_version[B_FRONT_MAX_DEFAULT_BOARD_VERSION_LEN] ;
  unsigned short front_board_serial_number ;
  char  front_board_param_1[B_FRONT_MAX_PARAM_LEN] ;
  char  front_board_param_2[B_FRONT_MAX_PARAM_LEN] ;
  char  front_board_param_3[B_FRONT_MAX_PARAM_LEN] ;
  char  front_board_param_4[B_FRONT_MAX_PARAM_LEN] ;
  char  front_board_param_5[B_FRONT_MAX_PARAM_LEN] ;
  char  front_board_param_6[B_FRONT_MAX_PARAM_LEN] ;
  char  front_board_param_7[B_FRONT_MAX_PARAM_LEN] ;
  char  front_board_param_8[B_FRONT_MAX_PARAM_LEN] ;
  char  front_board_param_9[B_FRONT_MAX_PARAM_LEN] ;
  char  front_board_param_10[B_FRONT_MAX_PARAM_LEN] ;
} __ATTRIBUTE_PACKED__ FRONT_SERIAL_EE_BLOCK_02  ;
typedef union
{
  FRONT_SERIAL_EE_BLOCK_01  front_serial_ee_block_01 ;
  unsigned char fill[FRONT_SIZEOF_SERIAL_EE_BLOCK_01] ;
} __ATTRIBUTE_PACKED__ UN_FRONT_SERIAL_EE_BLOCK_01 ;
typedef union
{
  FRONT_SERIAL_EE_BLOCK_02  front_serial_ee_block_02 ;
  unsigned char fill[FRONT_SIZEOF_SERIAL_EE_BLOCK_02] ;
} __ATTRIBUTE_PACKED__ UN_FRONT_SERIAL_EE_BLOCK_02 ;
typedef struct
{
  UN_FRONT_SERIAL_EE_BLOCK_01 un_front_serial_ee_block_01 ;
  unsigned short crc16 ;
} __ATTRIBUTE_PACKED__ FRONT_SER_EE_BLOCK_01  ;
typedef struct
{
  UN_FRONT_SERIAL_EE_BLOCK_02 un_front_serial_ee_block_02 ;
  unsigned short crc16 ;
} __ATTRIBUTE_PACKED__ FRONT_SER_EE_BLOCK_02  ;
typedef struct
{
  unsigned char fill[SIZEOF_FRONT_SERIAL_EE_FILLER];
} __ATTRIBUTE_PACKED__ FRONT_SER_EE_BLOCK_03;
/*
 * }
 */
/*
 * Definitions related to HOST_DB serial eeprom
 * {
 */
typedef struct
{
  unsigned short host_db_board_type ;
} __ATTRIBUTE_PACKED__ HOST_DB_SERIAL_EE_BLOCK_01  ;
typedef struct
{
  char  host_db_board_catalog_number[B_HOST_DB_MAX_CATALOG_NUMBER_LEN] ;
  char  host_db_board_description[B_HOST_DB_MAX_BOARD_DESCRIPTION_LEN] ;
  char  host_db_board_version[B_HOST_DB_MAX_DEFAULT_BOARD_VERSION_LEN] ;
  unsigned short host_db_board_serial_number ;
  char  host_db_board_param_1[B_HOST_DB_MAX_PARAM_LEN] ;
  char  host_db_board_param_2[B_HOST_DB_MAX_PARAM_LEN] ;
  char  host_db_board_param_3[B_HOST_DB_MAX_PARAM_LEN] ;
  char  host_db_board_param_4[B_HOST_DB_MAX_PARAM_LEN] ;
  char  host_db_board_param_5[B_HOST_DB_MAX_PARAM_LEN] ;
  char  host_db_board_param_6[B_HOST_DB_MAX_PARAM_LEN] ;
  char  host_db_board_param_7[B_HOST_DB_MAX_PARAM_LEN] ;
  char  host_db_board_param_8[B_HOST_DB_MAX_PARAM_LEN] ;
  char  host_db_board_param_9[B_HOST_DB_MAX_PARAM_LEN] ;
  char  host_db_board_param_10[B_HOST_DB_MAX_PARAM_LEN] ;
} __ATTRIBUTE_PACKED__ HOST_DB_SERIAL_EE_BLOCK_02  ;
typedef union
{
  HOST_DB_SERIAL_EE_BLOCK_01  host_db_serial_ee_block_01 ;
  unsigned char fill[HOST_DB_SIZEOF_SERIAL_EE_BLOCK_01] ;
} __ATTRIBUTE_PACKED__ UN_HOST_DB_SERIAL_EE_BLOCK_01 ;
typedef union
{
  HOST_DB_SERIAL_EE_BLOCK_02  host_db_serial_ee_block_02 ;
  unsigned char fill[HOST_DB_SIZEOF_SERIAL_EE_BLOCK_02] ;
} __ATTRIBUTE_PACKED__ UN_HOST_DB_SERIAL_EE_BLOCK_02 ;
typedef struct
{
  UN_HOST_DB_SERIAL_EE_BLOCK_01 un_host_db_serial_ee_block_01 ;
  unsigned short crc16 ;
} __ATTRIBUTE_PACKED__ HOST_DB_SER_EE_BLOCK_01  ;
typedef struct
{
  UN_HOST_DB_SERIAL_EE_BLOCK_02 un_host_db_serial_ee_block_02 ;
  unsigned short crc16 ;
} __ATTRIBUTE_PACKED__ HOST_DB_SER_EE_BLOCK_02  ;
typedef struct
{
  unsigned char fill[SIZEOF_HOST_DB_SERIAL_EE_FILLER];
} __ATTRIBUTE_PACKED__ HOST_DB_SER_EE_BLOCK_03;
/*
 * }
 */

typedef struct
{
  SER_EE_BLOCK_01         ser_ee_block_01 ;
  SER_EE_BLOCK_02         ser_ee_block_02 ;
  SER_EE_BLOCK_03         ser_ee_block_03_filler ; 
  HOST_SER_EE_BLOCK_01    host_ser_ee_block_01 ;
  HOST_SER_EE_BLOCK_02    host_ser_ee_block_02 ;
  HOST_SER_EE_BLOCK_03    host_ser_ee_block_03 ;
  FRONT_SER_EE_BLOCK_01   front_ser_ee_block_01 ;
  FRONT_SER_EE_BLOCK_02   front_ser_ee_block_02 ;
  FRONT_SER_EE_BLOCK_03   front_ser_ee_block_03 ;
  HOST_DB_SER_EE_BLOCK_01  host_db_ser_ee_block_01 ;
  HOST_DB_SER_EE_BLOCK_02  host_db_ser_ee_block_02 ;
  HOST_DB_SER_EE_BLOCK_03  host_db_ser_ee_block_03 ;
} __ATTRIBUTE_PACKED__ SERIAL_EE_AREA  ;

unsigned char eepromReadByte
    (
    int offset
    );
STATUS
  eepromWriteBytes(
    int           offset,     /* Offset, in Eeprom, to write int0 */
    unsigned char *data_ptr,  /* Pointer to source data           */
    unsigned int  size        /* Number of bytes to read from data*/
                              /* and write into offset            */
    );

unsigned
  int
    ser_ee_high_image_read(
      unsigned int   offset,
      unsigned int   num,
      unsigned char  *dest,
      unsigned int   *err_flg,
      char           *err_msg
      );
unsigned
  int
    ser_ee_high_image_write(
      unsigned int   offset,
      unsigned int   num,
      unsigned char  *dest,
      unsigned int   *err_flg,
      char           *err_msg
      );

