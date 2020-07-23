/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_DEFX_INCLUDED
/* { */
#define UI_DEFX_INCLUDED

#ifdef  __cplusplus
extern "C" {
#endif

#include <appl/diag/dpp/ref_sys.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>

#if !DUNE_BCM
#if !(defined(LINUX) || defined(UNIX))
/* { */
  #include "../../../../H/drv/mem/eeprom.h"
/* } */
#endif
#endif

#include "utils_defx.h"

/*
 * PROTOTYPES
 * {
 */

#define CLI_INVALID_DEFAULT_DEVICE_ID ((unsigned long)-1)

#if LINK_TIMNA_LIBRARIES
  void
    timna_set_default_unit(uint32 dev_id);
  uint32                                                        
    timna_get_default_unit(void);
#endif

#if LINK_PPD_LIBRARIES
  void
    soc_petra_set_default_unit(uint32 dev_id);
  uint32
    soc_petra_get_default_unit(void);

  void
    soc_petra_set_default_units(uint32 dev_ids[SOC_SAND_MAX_DEVICE], uint32 nof_devices);
  void
    soc_petra_get_default_units(uint32 dev_ids[SOC_SAND_MAX_DEVICE], uint32 *nof_devices);
#endif

#if LINK_T20E_LIBRARIES
  void
    t20e_set_default_unit(uint32 dev_id);
  void
    oam_set_default_unit(uint32 dev_id);
  uint32                                                        
    t20e_get_default_unit(void);
#endif

#if LINK_TMD_LIBRARIES
  void
    tmd_set_default_unit(uint32 dev_id);
  uint32                                                        
    tmd_get_default_unit(void);
#endif

void
  display_cli_prompt(
    void
  ) ;
void
  init_ui_module(
    void
  ) ;
void
  ui_proc(
    void
  ) ;
#if !(defined(LINUX) || defined(UNIX))
/* { */

int
  index_num_from_nv(
    unsigned char  *next_in_index_ptr,
    unsigned char  *num_events_logged_ptr
  ) ;
int
  index_num_to_nv(
    unsigned char  next_in_index,
    unsigned char  num_events_logged
  ) ;
int
  event_item_from_nv(
    EVENT_LOG_ITEM  *event_item_ptr,
    unsigned char   event_item_index
  ) ;
int
  event_item_to_nv(
    EVENT_LOG_ITEM  *event_item_ptr,
    unsigned char   event_item_index
  ) ;
int
  next_event_item_to_nv(
    EVENT_LOG_ITEM  *event_item_ptr
  ) ;
int
  next_event_item_from_nv(
    EVENT_LOG_ITEM  *event_item_ptr,
    int             *index_ptr
  ) ;

int
  block_05_from_nv(
    BLOCK_05  *block_05_ptr
  ) ;
int
  block_05_to_nv(
    BLOCK_05  *block_05_ptr
  ) ;

#endif /*} LINUX */

int
  handler_processing_time_from_nv(
    unsigned long  *handler_processing_time_ptr
    ) ;
int
  handler_processing_time_to_nv(
    unsigned long  handler_processing_time
    ) ;
int
  new_n_valid_from_nv(
    unsigned char  *new_n_valid_ptr
  ) ;
int
  new_n_valid_to_nv(
    unsigned char  new_n_valid
  ) ;
int
  error_identifier_from_nv(
    unsigned long  *error_identifier_ptr
  ) ;
int
  error_identifier_to_nv(
    unsigned long  error_identifier
  ) ;
int
  block_04_from_nv(
    void  *block_04_ptr
  ) ;
int
  block_04_to_nv(
    void  *block_04_ptr
  ) ;
int
  software_exception_new_n_valid_from_nv(
    unsigned char  *new_n_valid_ptr
    );
int
  software_exception_new_n_valid_to_nv(
    unsigned char  new_n_valid
    );
int
  software_exception_handler_processing_time_from_nv(
    unsigned long  *handler_processing_time_ptr
    );
int
  software_exception_handler_processing_time_to_nv(
    unsigned long  handler_processing_time
    );
int
  software_exception_error_identifier_from_nv(
    unsigned long  *error_identifier_ptr
  );
int
  software_exception_error_identifier_to_nv(
    unsigned long  error_identifier
  );
int
  software_exception_exception_counter_from_nv(
    unsigned char  *exception_counter_ptr
    );
int
  block_07_from_nv(
    void  *block_07_ptr
  );
int
  block_07_to_nv(
    void  *block_07_ptr
  );
int
  get_err_display_level(
    unsigned char *level_ptr
  ) ;
int
  get_nv_store_level(
    unsigned char *level_ptr
  ) ;
int
  get_err_syslog_level(
    unsigned char *level_ptr
  ) ;
int
  get_dpss_display_level(
    unsigned char *level_ptr
  ) ;
int
  get_dpss_debug_level(
    unsigned char *level_ptr
  ) ;
int
  get_run_cli_script_at_stratup(
    unsigned char *val_ptr
  );
int
  get_load_dpss_config_file_from_flash(
    unsigned char *val_ptr
  );
int
  get_start_application_in_demo_mode(
    unsigned char *val_ptr
  );
int
  get_fap10m_run_mode(
    unsigned char *val_ptr
  );
int
  get_download_mibs(
    unsigned char *flag_ptr
  ) ;
int
  get_load_netsnmp_flag(
    unsigned char *flag_ptr
  ) ;
int
  get_net_snmp_cmd_line(
    char *line
  ) ;
void
  nice_printed_snmp_cmd_line(
    char *out,
    char *in1,
    char *in2,
    char *in3,
    int   out_len
  );
int
  exception_counter_from_nv(
    unsigned char  *exception_counter_ptr
  ) ;
int
  get_watchdog_period(
    unsigned char *watchdog_period_ptr
  ) ;
int
  is_watchdog_active(
    void
  ) ;
int
  nv_is_watchdog_active(
    void
  ) ;
int
  nv_is_hook_software_exception_active(
    void
  ) ;
int
  nv_is_bckg_temperatures_en_active(
    void
  ) ;
void
  par_nv_poll_init(
    void
  ) ;
unsigned
  long
    get_par_nv_poll_cycles(
      void
  ) ;
unsigned
  long
    get_par_nv_poll_starts(
      void
  ) ;
unsigned
  long
    get_par_nv_poll_avg_byte(
      void
  ) ;
unsigned
  long
    get_par_nv_poll_avg_num(
      void
  ) ;
unsigned
  long
    get_par_nv_poll_avg_time(
      void
  ) ;
unsigned
  short
    get_par_nv_poll_min_time(
      void
  ) ;
unsigned
  short
    get_par_nv_poll_max_time(
      void
  ) ;
int
  parallel_nv_end(
    unsigned long address
  ) ;
int
  parallel_nv_start(
    unsigned long address
  ) ;
int
  parallel_nv_poll(
    unsigned long delay
  ) ;
void
  display_poll_statistics(
    void
  ) ;

STATUS
  nvRamGet(
    char *string,    /* where to copy non-volatile RAM    */
    int strLen,      /* maximum number of bytes to copy   */
    int offset       /* byte offset into non-volatile RAM */
  );
STATUS
  nvRamSet(
    char *string,     /* string to be copied into non-volatile RAM */
    int strLen,       /* maximum number of bytes to copy           */
    int offset        /* byte offset into non-volatile RAM         */
    );

unsigned
  int
    get_use_par_nv_sem(
      void
    ) ;
void
  set_use_par_nv_sem(
    unsigned int val
  ) ;
int
  init_par_nv_sem(
    void
  ) ;
int
  take_par_nv_sem(
    void
  ) ;
int
  give_back_par_nv_sem(
    void
  ) ;
/*
 * }
 */
/*
 * OBJECT:
 *   handling of inactivity timeout for Telnet.
 * {
 */
/*
 * Last time activity was detected on telnet. Time is
 * measured from startup in units of 10 milliseconds.
 */
unsigned
  int
    is_telnet_active(
      void
    ) ;
void
  set_telnet_active(
    unsigned int val
  ) ;
unsigned
  long
    get_telnet_timeout_val(
      void
    ) ;
void
  set_telnet_timeout_val(
    unsigned long val
  ) ;
void
  set_telnet_activity_time_mark(
    void
  ) ;
unsigned
  int
    has_telnet_inactivity_time_elapsed(
      void
  ) ;
/*
 * }
 */
/*
 * OBJECT:
 *   Timeout on suspended ui task.
 * {
 */
void
  inc_ui_suspend_timer(
    void
  );

void
  clear_ui_suspend_timer(
    void
  );

int
  is_ui_suspend_timer_overflown(
    void
  );

/*
 * }
 */

/*
 * Utilities related to telnet activity items and actions
 * (block_03)
 * {
 */
int
  get_telnet_activity_timeout_std(
    unsigned char *period_ptr
  ) ;
int
  get_telnet_activity_timeout_cont(
    unsigned char *period_ptr
  ) ;
int
  set_telnet_activity_timeout_std(
    unsigned char period
  ) ;
int
  set_telnet_activity_timeout_cont(
    unsigned char period
  ) ;
/*
 * }
 */
/*
 * Object: Dummy Telnet handling
 * Dummy Telnet handles user input on TCP port 26 while
 * standard Telnet is active.
 * {
 */
int
  get_dummy_telnet_sock_addr_size(
    void
  ) ;
struct sockaddr_in
  *get_dummy_telnet_server_addr(
    void
  ) ;
struct sockaddr_in
  *get_dummy_telnet_client_addr(
    void
  ) ;
int
  get_dummy_telnet_fd(
    void
  ) ;
void
  set_dummy_telnet_fd(
    int fd
  ) ;
int
  get_dummy_telnet_socket(
    void
  ) ;
void
  set_dummy_telnet_socket(
    int socket
  ) ;
/*
 * }
 */
/*
 * OBJECT:
 * CLI error counter
 * Counts the number of syntax (or other) errors (made by
 * the user while typing in a command line and reported
 * to the user with a message usually starting with '***'
 * {
 */
unsigned
  int
    get_cli_err_counter(
      void
    ) ;
void
  inc_cli_err_counter(
    void
  ) ;
void
  clear_cli_err_counter(
    void
  ) ;
/*
 * }
 */
/*
 * OBJECT:
 * Echo - mode
 * Get/set value of CLI flag which indicates whether
 * characters received from the user are to be echoed
 * back to the sender or not.
 * {
 */
unsigned
  int
    is_echo_mode_on(
      void
    ) ;
void
  set_echo_mode_on(
    void
  ) ;
void
  set_echo_mode_off(
    void
  ) ;

void
  ui_activate_file(
    void
  );
/*
 * }
 */
/*
 * Pointer to a procedure for checking validity of
 * input to parameter value on CLI.
 * If a non-zero value is returned then input
 * value is not valid. In that case, err_msg
 * is loaded by plain text explanation of why the
 * value is not valid. Caller must make sure to supply
 * an array of at least 160 bytes.
 * First parameter:
 *   void *. Points to current_line.
 * Second parameter:
 *   unsigned long. For numeric fields - input value.
 *   For symbolic fields, numeric equivalent.
 * Third parameter:
 *   char *err_msg. Pointer to string load error
 *   text into.
 * Fourth parameter:
 *   unsigned long. Indication. Only meaningful for
 *   numeric fields. If 'true' then field has not
 *   yet been fully loaded.
 */
typedef int (*VAL_PROC_PTR)(void *,unsigned long,char *,unsigned long) ;

#ifdef  __cplusplus
}
#endif

/* } */
#endif
