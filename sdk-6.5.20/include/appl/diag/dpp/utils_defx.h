/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UTILS_DEFX_INCLUDED
/* { */
#define UTILS_DEFX_INCLUDED

#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/drv.h>
#include "ref_sys.h"
#include "bits_bytes_macros.h"
#include "dune_chips.h"

#if !DUNE_BCM
/*
 * Dune chips include file.
 */

#include "Pub/include/ref_sys.h"
#include "Pub/include/bits_bytes_macros.h"

#if (defined(LINUX) || defined(UNIX))
/* { */
  #include <sys/time.h>
  #include <sys/socket.h>
  #include <sys/types.h>
  #include <sys/signal.h>
  #include <signal.h>
  #include <unistd.h>
#elif !defined(WIN32)
/* { */
  #include <sys/time.h>
  #include <sys/socket.h>
  #include <sys/types.h>
  #include <sys/signal.h>
  #include <signal.h>
/* } */
#endif

#include <time.h>

#include "stdio.h"
#include "string.h"

#if !(defined(LINUX) || defined(UNIX))
/* { */
#include "DuneDriver/SOC_SAND_FAP10M/include/fap10m_chip_defines.h"
#include "../../../../H/drv/mem/eeprom.h"
#include "../../../../H/DuneTypeDef.h"
/*} LINUX */
#endif

#include "Pub/include/dune_chips.h"

typedef void* ESFPPC;
typedef	char *	caddr_t;

#ifdef WIN32
  /* { */
  typedef void* WIND_TCB;
  typedef int (*FUNCPTR) ();
  typedef void* SEM_ID;
  typedef int timer_t;
  /*}*/
#elif defined (__DUNE_HRP__)
  /* { */
  #include "DHRP/include/dhrp_defs.h"
  /* } */
#elif (defined(LINUX) || defined(UNIX))
  typedef int (*FUNCPTR) ();
  typedef void* SEM_ID;
  /* !DUNE_BCM changed all BOOL to uint8 typedef int BOOL; */
#endif

#if !(defined(LINUX) || defined(UNIX))
  #define D_TICKS_PER_SEC sysClkRateGet()
#else
  #define D_TICKS_PER_SEC sysconf(_SC_CLK_TCK)
#endif


#else /* !DUNE_BCM */

#define B_BIT(x)          (1<<(x))
#define MAC_ADRS_LEN 6
#define __ATTRIBUTE_PACKED__  __attribute__ ((packed))

#define FLASH_WIDTH           2

#define IMPORT
#define ERROR TRUE
#define WAIT_FOREVER -1
typedef void* SEM_ID;

#if (defined(LINUX) || defined(UNIX)) || defined(__DUNE_GTO_BCM_CPU__)
#define OK    0
#define bcopyBytes sal_memcpy
typedef unsigned char UINT8;
typedef int STATUS;
typedef void (*FUNCPTR_BCM) (void*);
typedef int (*FUNCPTR) (void);
#define FLASH_WIDTH           2
#define FLASH_DEF       unsigned short
#define FLASH_CAST      (unsigned short *)
/* from eeprom.h */
#define NV_RAM_SIZE    (16 * 1024)
#include "utils_eeprom.h"
#include "time.h"

int
  host_board_cat_number_from_nv(
    char  *board_serial_num_ptr
    ) ;
int
  host_board_description_from_nv(
    char  *board_description_ptr
    ) ;
int
  host_board_version_from_nv(
    char  *board_version_ptr
    ) ;
int
  host_board_sn_from_nv(
    unsigned short  *board_sn_ptr
    ) ;
int
  front_does_card_exist(
    void
  ) ;
int
  front_board_sn_from_nv(
    unsigned short  *board_sn_ptr
    ) ;
unsigned short
  crc16_with_ini(
    void           *buff,
    unsigned long  size,
    unsigned short init_crc
  );

#if !defined (SAND_LOW_LEVEL_SIMULATION)
#define FLASH_DEF_INF_V1 1
#endif

typedef struct
{
  
  unsigned char struct_version;
  unsigned long f_adrs;        /*Base address of FLASH*/
  unsigned char f_adrs_jump;   /*E.g. FLASH is 16 bits.*/
  unsigned char f_type;        /*FLASH type. Supported: FLASH_28F640, FLASH_28F320*/
  unsigned long f_sector_size; /*Sector Byte size*/
} FLASH_DEF_INF;

/* from usrApp.h */
#define BT_FS_VERSION_0     0
#define BT_FS_VERSION_1     1
#define FRONT_PANEL_YELLOW_LED          (1)

unsigned
  char
    get_slot_id(
      void
    ) ;
unsigned
  char
    read_epld_reg(
      unsigned int offset
    ) ;
void
  write_epld_reg(
    unsigned char byte_value,
    unsigned int offset
  );
unsigned
  char
    get_epld_board_id(
      void
    ) ;
unsigned
  char
    get_crate_id(
      void
    ) ;
unsigned
  char
    get_crate_type(
      void
    ) ;
char
  *get_crate_type_text(
    unsigned int crate_type
  ) ;
void
  lock_flash(
    void
  );
void
  unlock_flash(
    void
  );
unsigned
  int
    get_flash_device_code(
      void
    ) ;
unsigned
 long
   get_flash_sector_size(
     void
   );
unsigned int
  boot_get_fs_version(void);
void
  private_delay(
    unsigned long delay_count
  );
int
  load_indication_run_silent(
    unsigned long current_work_load
  );

STATUS
  sysFlashGet_protected_info(
    const FLASH_DEF_INF *flash_info,  /*Flash information*/
          char          *string,    /* where to copy flash memory      */
          int           strLen,     /* maximum number of bytes to copy */
          int           offset      /* byte offset into flash memory   */
  );
STATUS
  sysFlashErase_protected_info(
    const FLASH_DEF_INF *flash_info,  /*Flash information*/
          int do_poll_indication,     /* Flag. Non-Zero print progress indication*/
                                      /* in percentage.                          */
          int sector                  /* Number of sector to erase (starting from*/
                                      /* zero. If negative then erase the whole  */
                                      /* memory space. If number of sector is    */
                                      /* larger than the maximal for that flash  */
                                      /* type, last sector is erased.            */
  );
STATUS
  sysFlashWrite_protected_info(
    const FLASH_DEF_INF *flash_info,  /*Flash information*/
          FLASH_DEF *pFB,        /* string to be copied; use <value> if NULL */
          int       size,        /* size to program in bytes */
          int       offset,      /* byte offset into flash memory */
          FLASH_DEF value,       /* value to program */
          int (*poll_indication_func)(unsigned long current_work_load)    /* Function to call when polling the Flash.*/
                                      /* If NULL then call no function.          */
  );

#endif /* (defined(LINUX) || defined(UNIX)) || defined(__DUNE_GTO_BCM_CPU__) */


STATUS
  d_taskLock(
    void
  ) ;
STATUS
  d_taskUnlock(
    void
  ) ;
STATUS
  d_taskDelete(
    sal_thread_t tid /* task ID of task to delete */
    ) ;
STATUS
  d_taskSafe(
    void
  ) ;
STATUS
  d_taskUnsafe(
    void
  ) ;
int
  d_is_task_safe(
    void
  ) ;
void
  d_force_task_unsafe(
    int task_id
  ) ;
sal_thread_t
  d_taskSpawn(
    char *name,        /* name of new task (stored at pStackBase) */
    int  priority,     /* priority of new task */
    int  options,      /* task option word */
    int  stackSize,    /* size (bytes) of stack needed plus name */
    FUNCPTR_BCM entryPt,   /* entry point of new task */
    int  arg1,         /* 1st of 10 req'd task args to pass to func */
    int  arg2,
    int  arg3,
    int  arg4,
    int  arg5,
    int  arg6,
    int  arg7,
    int  arg8,
    int  arg9,
    int  arg10,
    int  private_task_id
  ) ;
STATUS
  d_taskDelay(
    int ticks
  ) ;
STATUS
  d_taskDelayMicroSec(
    const long int delay_in_microsec
  ) ;
int
  d_strnicmp(
    const char *str1,
    const char *str2,
    size_t     n
   );
int
  d_write(
    int    fd,      /* file descriptor on which to write */
    char   *buffer, /* buffer containing bytes to be written */
    int    nbytes,  /* number of bytes to write */
    int    std_out_target
    ) ;
int
  d_ioGlobalStdGet(
    int stdFd /* std input (0), output (1), or error (2) */
  ) ;
void
  d_ioGlobalStdSet(
    int stdFd, /* std input (0), output (1), or error (2) */
    int newFd  /* new underlying file descriptor */
  ) ;
void
  d_ioTaskStdSet(
    int taskId, /* task whose std fd is to be set (0 = self) */
    int stdFd,  /* std input (0), output (1), or error (2) */
    int newFd   /* new underlying file descriptor */
  ) ;
int
  d_ioctl(
    int fd,       /* file descriptor */
    int function, /* function code */
    int arg       /* arbitrary argument */
    ) ;
int
  d_timer_create(
    clockid_t         clock_id, /* clock ID (always CLOCK_REALTIME) */
    struct sigevent   *evp,     /* user event handler */
    timer_t           *pTimer   /* ptr to return value */
    ) ;
int
  d_restart_shell(
    void
  ) ;


/*  debug Dune integration */
#define D_TO_STR(x) #x

#define FLASH_28F320_SECTOR    0x20000

#define FLASH1_MEM_BASE_ADRS_8MEG 0xFF800000
#define FLASH1_MEM_BASE_ADRS_4MEG 0xFFC00000
#define BOOT_FLASH_BASE_ADRS 0xFFF00000

#define FLASH1_MEM_SIZE_4MEG      0x00400000
#define FLASH_SIZE_4MEG   FLASH1_MEM_SIZE_4MEG

#define FLASH1_MEM_SIZE_8MEG      0x00800000 
#define FLASH_SIZE_8MEG   FLASH1_MEM_SIZE_8MEG

#include <bcm/types.h>
typedef void* ESFPPC;

#ifndef __KERNEL__
extern STATUS    sysNvRamGet ();
extern STATUS    sysNvRamSet ();
#endif

#if !defined(__DUNE_GTO_BCM_CPU__)
#define SEM_FULL                        0x01
#define SEM_Q_PRIORITY                  0x01
#define SEM_DELETE_SAFE                 0x04
#define SEM_INVERSION_SAFE              0x08
#endif

extern SEM_ID    semMCreate( int opt );

extern
  sal_mutex_t
    I2c_bus_semaphore;

void
  i2c_start(
    void
  ) ;
int
  i2c_poll_tx(
    unsigned int   *err_flg,
    char           *err_msg
    ) ;
int
  i2c_load_buffer(
    unsigned char  dest_address,
    unsigned short i2c_data_len,
    unsigned char  *i2c_data,
    unsigned int   *err_flg,
    char           *err_msg
  ) ;
int
  i2c_get_buffer(
    unsigned short *i2c_data_len,
    unsigned char  *i2c_data,
    unsigned int   *err_flg,
    char           *err_msg
  ) ;

int
  take_flash_sem(
    void
  );
int
  give_back_flash_sem(
    void
  );

void load_indication_start(unsigned long total_work_load);
int  load_indication_run(unsigned long current_work_load);

int  get_prog_ver(void);


int
  ee_offset_to_block_id(
    unsigned int  offset,
    unsigned int  size,
    unsigned int  *block_id
    ) ;
int
  check_block_crc(
    unsigned int  block_id
    ) ;
int
  update_block_crc(
    unsigned int  block_id
    ) ;
int
  get_defaults(
    char *string,
    int  strLen,
    int  offset
    ) ;
int
  load_defaults(
    unsigned int  block_id
    ) ;

int
  get_run_vals(
    char *string,
    int  strLen,
    int  offset
    ) ;

/* MEMC registers */
#define BR4(base)       (CAST(VUINT32 *)((base) + 0x0120)) /* Base Reg bank 4 */
#define OR4(base)       (CAST(VUINT32 *)((base) + 0x0124)) /* Opt Reg bank 4*/
#define BR5(base)       (CAST(VUINT32 *)((base) + 0x0128)) /* Base Reg bank 5 */
#define OR5(base)       (CAST(VUINT32 *)((base) + 0x012C)) /* Opt Reg bank 5*/

/* Device Version managment { */
#define SOC_PETRA_REVISION_FLD_VAL_A0  0x0
#define SOC_PETRA_REVISION_FLD_VAL_A1  0x10
#define SOC_PETRA_REVISION_FLD_VAL_A2  0x30
#define SOC_PETRA_REVISION_FLD_VAL_A3  0x32
#define SOC_PETRA_REVISION_FLD_VAL_A4  0x832

typedef enum
{
  D_SOFT_REBOOT = 0,
  D_HARD_REBOOT = 1
} D_REBOOT_TYPE;

#endif /* !DUNE_BCM */

/**************************************************************************/
/* When the Utils module run as stand alone, some Negev/GFA/BSP dedicated */
/* Macro definition are missing.                                          */
/* Bellow are the definitions of those MACROS.                            */
/* The values should be the same values, as in the Negev/GFA/BSP system   */
/**************************************************************************/
#ifndef B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN
  #define B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN  7
#endif

#ifndef B_HOST_MAX_PARAM_LEN
  #define B_HOST_MAX_PARAM_LEN  20
#endif

/*
 * PROTOTYPES
 * {
 */
/*
 * Procedures related to DUNE chip set
 * {
 */
#if (SIMULATE_DUNE_CHIP || SIMULATE_CHIP_SIM)
/* { */
void
  inc_control_cells_cntr_1(
    unsigned long val_to_add
  ) ;
void
  inc_control_cells_cntr_2(
    unsigned long val_to_add
  ) ;
/* } */
#endif

/*
 * MACROS
 */
#define UTILS_DEFX_DEBUG 1

#if UTILS_DEFX_DEBUG
  #define UTILS_DEFX_DBG_ID "**DEBUG--> "
  #define UTILS_DEFX_DBG  \
  {                 \
    soc_sand_os_printf("\n%sFile    : %s \n%sFunction: %s line %u\n", UTILS_DEFX_DBG_ID, __FILE__, UTILS_DEFX_DBG_ID, FUNCTION_NAME(), __LINE__);\
  }
#else
  #define UTILS_DEFX_DBG
#endif

#define UTILS_STRCAT_SAFE(param_dst,param_src,param_dst_size) \
  if ((strlen(param_dst) + strlen(param_src))>= (param_dst_size))   \
{                                                 \
  soc_sand_os_printf(                                                       \
  "******************************************************* \n\r"      \
  " Error: Concatenation overflow the Destination buffer.  \n\r"      \
  " Stopping before parameter %s  ...\n\r"                            \
  "******************************************************* \n\r", param_src); \
  UTILS_DEFX_DBG; \
  goto exit;   \
}       \
    else strcat(param_dst, param_src);


void
  set_pointed_fe_default(
    void
  ) ;
void
  set_pointed_fe(
    unsigned int val
  ) ;
unsigned int
  get_pointed_fe(
    void
  ) ;
void
  set_desc_fe_reg_size(
    unsigned int size
  ) ;
unsigned
  int
    get_desc_fe_reg_size(
      void
    ) ;
int
  write_to_fe(
    unsigned long *array,
    unsigned long offset,
    unsigned long size
  );
int
  read_from_fe(
    unsigned long *array,
    unsigned long offset,
    unsigned long size
  );
int
  should_update_conf_from_nvram(
    void
  );
unsigned
  long
    get_conf_from_nvram(
      void
    );

/*
 * }
 */
void
  appLedBarInvert(
    unsigned char  value,
    unsigned char  action
  );
void
  appLedBarOff(
    unsigned char  value,
    unsigned char  action
  );
void
  appLedBarOn(
    unsigned char  value,
    unsigned char  action
  );
int
  utils_led_start_blink_led(
    unsigned int led_color,
    unsigned int nof_delay_ticks
  );
void
  utils_led_stop_blink_led(
    unsigned int led_color,
    unsigned int keep_it_on
  );


/*
 * Object: Semaphore protecting 'screen' operations
 *         (specifically 'send_XXX_to_screen())
 *
 * {
 */
int
  create_screen_semaphore(
    void
  ) ;
int
  is_screen_semaphore_owned_by_this_task(
    void
  ) ;
int
  give_screen_semaphore(
    void
  );
int
  take_screen_semaphore(
    void
  ) ;
int
  release_print_safe(
    int *safe_count,
    int *print_sem_count
  ) ;
int
  restore_print_safe(
    int safe_count,
    int print_sem_count
  ) ;
void
  start_print_services(
    void
  ) ;
void
  end_print_services(
    void
  ) ;
/*
 * }
 */
/*
 * OBJECT:
 *   Periodic display
 *     Handling of periodic display
 *     (continuous, repetitive, on the same line).
 * {
 */
void
  set_display_periodic_off(
    void
  ) ;
int
  is_display_periodic_on(
    void
  ) ;
int
  is_periodic_on(
    void
  ) ;
void
  set_periodic_on(
    void
  ) ;
void
  set_periodic_off(
    void
  ) ;
void
  elapse_periodic(
    void
  ) ;
/*
 * }
 */

typedef enum
{
  PERIODIC_WAIT_AND_PRINT_STYLE_NOT_VALID = 0,

  /*
   * Currently only one style
   */
  PERIODIC_WAIT_AND_PRINT_STYLE_DEFAULT

} PERIODIC_WAIT_AND_PRINT_STYLE;


void
  periodic_wait_and_print(
    const unsigned long nof_secs,
    const unsigned int  print_style
  );

int
  clock_step_passed(
    bcm_time_spec_t *reference_time,
    bcm_time_spec_t *step_time,
    bcm_time_spec_t *current_time
    ) ;
int
  clock_from_startup(
    unsigned long *time_from_startup,
    unsigned int  use_10_ms
    ) ;
int
  clock_get_diff(
    bcm_time_spec_t *reference_time,
    bcm_time_spec_t *diff_time,
    bcm_time_spec_t *input_time
  ) ;
void
  ms_to_timespec(
    bcm_time_spec_t *timespec_struct,
    unsigned long   input_ms
  ) ;
void
  timespec_to_ms(
    unsigned long   *ms,
    bcm_time_spec_t *input_timespec_struct
  );
#if !DUNE_BCM
void
  ms_to_timeval(
    struct timeval  *timeval_struct,
    unsigned long   input_ms
  );
void
  timeval_to_ms(
    unsigned long   *ms,
    struct timeval  *input_timeval_struct
  );
#endif
void
  ms_to_ticks(
      unsigned long   *ticks,
      unsigned long   input_ms
  );

void
  ticks_to_ms(
      unsigned long   *ms,
      unsigned long   input_ticks
  );

#if (defined(LINUX) || defined(UNIX)) || defined(__DUNE_GTO_BCM_CPU__)
/* { */
int
  set_watchdog_inject_time(
    void
  );

unsigned
  long
    get_watchdog_time(
      unsigned int use_10_ms
    );
/* } */
#endif

/*
 * OBJECT:
 *   Real Time Trace Utility
 * {
 */
void
  init_trace_fifo(
    void
  ) ;
void
  put_in_trace_fifo(
    int          trace_event_id,
    unsigned int trace_event_text_size,
    char         *trace_event_text,
    unsigned long trace_param_1,
    unsigned long trace_param_2,
    unsigned long trace_param_3,
    unsigned long trace_param_4
  ) ;
int
  get_next_from_trace_fifo(
    int            *trace_event_id_ptr,
    unsigned long  *trace_event_time_ptr,
    char           *trace_event_text_ptr,
    unsigned long  *trace_params_ptr
  ) ;
int
  get_direct_from_trace_fifo(
    unsigned int   trace_event_index,
    int            *trace_event_id_ptr,
    unsigned long  *trace_event_time_ptr,
    char           *trace_event_text_ptr,
    unsigned long  *trace_params_ptr
  ) ;
int
  is_trace_fifo_locked(
    void
  ) ;
unsigned
  int
    get_num_elements_trace_fifo(
      void
  ) ;
unsigned
  int
    get_clear_load_while_locked_trace_fifo(
      void
  ) ;
void
  lock_trace_fifo(
    void
  ) ;
void
  unlock_trace_fifo(
    void
  ) ;
/*
 * }
 */
/*
 * OBJECT:
 *   In ISR
 *     Utilities related to status of processor:
 *     Is it within an ISR or not.
 *     If 'In_isr' is 'true' then system is within
 *     ISR and some operating system calls may not
 *     be called and, if they do, will do nothing.
 * {
 */
void
  set_in_isr(
    unsigned int in_isr
  ) ;
int
  is_in_isr(
    void
  ) ;
/*
 * }
 */
/*
 * OBJECT:
 *   Event log utilities
 *     Utilities related to handling of event
 *     log in NVRAM.
 * {
 */
int
  display_event_log_block(
    void
  ) ;
/*
 * }
 */
/*
 * OBJECT:
 *   NMI utilities
 *     Utilities related to handling of NMI/IRQ0
 *     which is triggered by the watchdog timer.
 * {
 */
int
  do_watchdog_reset(
    void
  ) ;
void
  load_do_watchdog_reset(
    unsigned int val
  ) ;
int
  display_watchdog_block(
    unsigned int check_new_n_valid,
    unsigned int clear_new_n_valid,
    unsigned int store_in_nvlog
  ) ;
void
  watchdog_handler(
    ESFPPC *pEsf
  ) ;
/*
 * }
 */
/*
 * OBJECT:
 *    Utilities related to handling
 *    Software Emulation Exception.
 * {
 */
int
  display_software_exception_block(
    unsigned int check_new_n_valid,
    unsigned int clear_new_n_valid,
    unsigned int store_in_nvlog
  ) ;
void
  soft_emu_exp_handler(
    ESFPPC *pEsf
  ) ;
void
  alignment_exception_handler(
    ESFPPC *pEsf
  ) ;
void
  data_tlb_error_handler(
    ESFPPC *pEsf
  ) ;
void
  inst_tlb_error_handler(
    ESFPPC *pEsf
  ) ;
void
  set_software_emulation_exp_testing(
    const int x
  ) ;
/*
 * }
 */

/*
 * Temperature structures
 */
typedef struct{
  int          temperature_integral;
  /*0-10000*/
  int          temperature_frac;
  /*
  * time from reset.
  * 10_msec resolution.
  */
  unsigned long time_stamp;
}TEMPERATURE_INFO;

typedef struct{
  TEMPERATURE_INFO current;
  TEMPERATURE_INFO max_from_reset;
  TEMPERATURE_INFO clearable_max;
}TEMPERATURE_STATUS;


/*
 * Procedures related to Temperature object
 * {
 */
void
  set_last_thermo_reading(
    unsigned int index,
    char *temperature
  ) ;
TEMPERATURE_STATUS *
  get_thermo_reading(
    unsigned int index
  );
void
  clear_clearable_max_temperature(
    unsigned int index
  );

int
  is_temperature_valid(
    void
  ) ;
void
  set_temperature_invalid(
    void
  ) ;
void
  set_temperature_valid(
    void
  );
/*
 * }
 */
/*
 * Procedures related to Thermometer reading: DS1721
 * {
 */
int
   init_thermometer(
     void
   ) ;
int
   collect_temperature(
     char *temperature,
     unsigned char  read_addr,
     unsigned char  write_addr
   ) ;
int
  collect_all_temperatures(
    void
    );
/*
 * }
 */
/*
 * Object: Task IDs.
 * {
 */
void
  init_task_id(
    void
  ) ;
void
  set_task_id(
    int task_private_index,
    int tid
  ) ;
void
  clear_task_id(
    int task_private_index
  ) ;
int
  is_task_alive(
    int task_private_index
  ) ;
int
  get_task_id(
    int task_private_index
  ) ;
int
  get_private_task_id(
    int task_id
  ) ;
/*
 * End object
 * }
 */
/*
 * Object: Private shell file descriptor (via "/pipe/shell").
 * {
 */
void
  set_shell_file_descriptor(
    int fd
  ) ;
int
  get_shell_file_descriptor(
    void
  ) ;
/*
 * End object
 * }
 */
/*
 * Object: Private user interface file descriptor
 * (related to the pipe: "/pipe/userIf").
 * {
 */
void
  set_user_if_file_descriptor(
    int fd
  ) ;
int
  get_user_if_file_descriptor(
    void
  ) ;
/*
 * End object
 * }
 */
/*
 * Object: Startup IO file descriptor
 *
 * This is the file descriptor of the standard IO
 * with which the system gets started.
 * {
 */
void
  set_startup_io_file_descriptor(
    int fd
  ) ;
int
  get_startup_io_file_descriptor(
    void
  ) ;
/*
 * End object
 * }
 */
/*
 * OBJECT:
 * Current_ui_fd
 *    File descriptor for standard IO which is currently
 *    used in user interface task.
 *    Monitored routinely to make sure it is up to date.
 *    (It is changed by telnet)
 * {
 */
void
  set_current_ui_fd(
    int fd
  ) ;
int
  get_current_ui_fd(
    void
  ) ;
/*
 * }
 */
/*
 * Object: Block STD IO.
 * {
 */
int
  is_std_io_blocked(
    void
  ) ;
void
  set_std_io_blocked(
    int val
  ) ;
/*
 * End object
 * }
 */
/* { */
/*
 * OBJECT:
 * Ui_timer_id
 *    Timer activated routinely from user if task
 *    to make sure it is pending on the right standard
 *    file descriptor.
 * {
 */
#if !DUNE_BCM
void
  set_ui_timer_id(
    timer_t id
  ) ;
timer_t
  get_ui_timer_id(
    void
  ) ;
#endif
/*
 * }
 */
int
  d_printf(
    const char    *fmt,
    ...
  );
int
  task_create_proc(
    void
  ) ;

#if !DUNE_BCM

int
  d_write(
    int    fd,      /* file descriptor on which to write */
    char   *buffer, /* buffer containing bytes to be written */
    int    nbytes,  /* number of bytes to write */
    int    std_out_target
    ) ;
int
  d_read(
    int fd,
    char *buffer,
    size_t maxbytes
  ) ;
int
  d_open(
      const char *name,
      int flags,
      int mode
  ) ;
int
  d_ioGlobalStdGet(
    int stdFd /* std input (0), output (1), or error (2) */
  ) ;
void
  d_ioGlobalStdSet(
    int stdFd, /* std input (0), output (1), or error (2) */
    int newFd  /* new underlying file descriptor */
  ) ;
void
  d_ioTaskStdSet(
    int taskId, /* task whose std fd is to be set (0 = self) */
    int stdFd,  /* std input (0), output (1), or error (2) */
    int newFd   /* new underlying file descriptor */
  ) ;
int
  d_ioctl(
    int fd,       /* file descriptor */
    int function, /* function code */
    int arg       /* arbitrary argument */
    ) ;
/*
 */
typedef enum
{
  D_SOFT_REBOOT = 0,
  D_HARD_REBOOT = 1
} D_REBOOT_TYPE;
void
  d_reboot(
    D_REBOOT_TYPE reboot_type,
    int           start_type /* If D_SOFT_REBOOT than how the boot ROMS will reboot */
  ) ;
int
  d_restart_shell(
    void
  ) ;
STATUS
  d_taskDelete(
    sal_thread_t tid /* task ID of task to delete */
    ) ;
sal_thread_t
  d_taskSpawn(
    char *name,        /* name of new task (stored at pStackBase) */
    int  priority,     /* priority of new task */
    int  options,      /* task option word */
    int  stackSize,    /* size (bytes) of stack needed plus name */
    FUNCPTR entryPt,   /* entry point of new task */
    int  arg1,         /* 1st of 10 req'd task args to pass to func */
    int  arg2,
    int  arg3,
    int  arg4,
    int  arg5,
    int  arg6,
    int  arg7,
    int  arg8,
    int  arg9,
    int  arg10,
    int  private_task_id
  ) ;
void
  d_force_task_unsafe(
    int task_id
  ) ;
int
  d_is_task_safe(
    void
  ) ;
int
  d_is_this_task_owner_of_sem(
    SEM_ID semaphore
  ) ;
STATUS
  d_taskSafe(
    void
  ) ;
STATUS
  d_taskUnsafe(
    void
  ) ;
int
  d_intLock(
    void
  ) ;
void
  d_intUnlock(
    int lockKey
  ) ;
BOOL
 utils_d_is_interrupt_enable(
    void
  );
STATUS
  d_taskDelay(
    int ticks
  ) ;
STATUS
  d_taskDelaySec(
    const long int delay_in_sec
  ) ;
STATUS
  d_taskDelayMicroSec(
    const long int delay_in_microsec
  ) ;
int
  d_clock_gettime(
    clockid_t       clock_id, /* clock ID (always CLOCK_REALTIME) */
    bcm_time_spec_t *tp       /* where to store current time */
    ) ;
int
  d_clock(
    void
  ) ;
unsigned
  long
    d_tickGet(
      void
    ) ;
int
   d_taskIdSelf(
     void
   ) ;

char
  *d_taskName(
    sal_thread_t tid /* task ID of task to delete */
  ) ;
STATUS
   d_taskRestart(
     sal_thread_t tid /* task ID of task to restart */
     ) ;
STATUS
   d_taskResume(
     sal_thread_t tid /* task ID of task to resume */
     ) ;
STATUS
   d_taskSuspend(
     sal_thread_t tid /* task ID of task to suspend */
     ) ;
STATUS
  d_taskIdVerify (
    sal_thread_t tid
    ) ;
int
  d_socket(
    int domain,
    int type,
    int protocol
  ) ;
int
  d_bind(
    int s,
    struct sockaddr *name,
    int namelen
  ) ;
int
  d_sendto(
    int s,
    caddr_t buf,
    int bufLen,
    int flags,
    struct sockaddr *to,
    int tolen
  );
int
  d_syslog(
    int pri,
    char *message
  ) ;

unsigned long
  d_inet_addr(
    const char* cp
  );

int
  d_strnicmp(
    const char *str1,
    const char *str2,
    size_t     n
   );

int
  d_stricmp(
    const char *str1,
    const char *str2
   );

#endif /* !DUNE_BCM */
/*
 * Error identifiers {
 */
#define STARTERPROC_ERR_03                         1
#define D_TIMER_CREATE_ERR_01                      2
#define D_IOCTL_ERR_01                             3
#define D_TASKDELETE_ERR_01                        4
#define D_TASKDELETE_ERR_02                        5
#define D_TASKDELETE_ERR_03                        6
#define D_TASKDELETE_ERR_04                        7
#define STARTERPROC_ERR_01                         11
#define STARTERPROC_ERR_02                         12
#define UI_PROC_SIGNAL_ERR_01                      18
#define UI_PROC_SIGNAL_ERR_02                      19
#define UI_PROC_SIGNAL_ERR_03                      20
#define UI_TMR_NEW_N_KICK_ERR_01                   21
#define UI_TMR_NEW_N_KICK_ERR_02                   22
#define UI_TMR_NEW_N_KICK_ERR_03                   23
#define HANDLE_NEXT_CHAR_ERR_02                    25
#define HANDLE_NEXT_CHAR_ERR_04                    27
#define HANDLE_NEXT_CHAR_ERR_05                    28
#define HANDLE_NEXT_CHAR_ERR_06                    29
#define UI_PREP_PROC_ERR_03                        33
#define UI_PREP_PROC_ERR_04                        34
#define UI_PREP_PROC_ERR_05                        35
#define INIT_UI_MODULE_ERR_01                      36
#define INIT_UI_MODULE_ERR_02                      37
#define INIT_UI_MODULE_ERR_03                      38
#define INIT_UI_MODULE_ERR_04                      39
#define INIT_UI_MODULE_ERR_05                      40
#define INIT_UI_MODULE_ERR_06                      41
#define INIT_UI_MODULE_ERR_07                      42
#define INIT_UI_MODULE_ERR_08                      43
#define D_WRITE_ERR_01                             44
#define PRINT_DETAILED_HELP_ERR_01                 45
#define INIT_UI_INTERPRETER_ERR_01                 47
#define INIT_UI_INTERPRETER_ERR_02                 48
#define INIT_UI_INTERPRETER_ERR_03                 49
#define INIT_UI_INTERPRETER_ERR_04                 50
#define SYS_GETCH_ERR_01                           55
#define ASK_GET_ERR_01                             56
#define ASK_GET_ERR_02                             57
#define ASK_GET_ERR_05                             60
#define D_CLOCK_GETTIME_ERR_01                     64
#define D_RESTART_SHELL_ERR_01                     82
#define D_RESTART_SHELL_ERR_02                     83
#define D_IOGLOBALSTDGET_ERR_01                    84
#define D_CLOCK_ERR_01                             88
#define D_TASKNAME_ERR_01                          89
#define D_TASK_LOCK_ERR_01                         92
#define D_TASK_UNLOCK_ERR_01                       93
#define LINE_MODE_HANDLING_ERR_01                  94
#define EXERCISES_ERR_01                           108
#define EXERCISES_ERR_03                           110
#define EXERCISES_ERR_04                           111
#define FIELD_LEGITIMATE_TEXT_ERR_01               114
#define EXERCISES_ERR_05                           115
#define EXERCISES_ERR_06                           116
#define GET_APPLICATION_VERION_ERR_01              117
#define DOWNLOAD_APPLICATION_TO_FLASH_ERR_01       118
#define DOWNLOAD_BOOT_TO_FLASH_ERR_01              119
#define HANDLE_BACK_SPACE_ERR_01                   122
#define PROGSTART_ERR_01                           124
#define UI_PREP_PROC_ERR_06                        125
#define UI_PREP_PROC_ERR_07                        126
#define UI_PREP_PROC_ERR_08                        127
#define UI_PREP_PROC_ERR_09                        128
#define HIGH_LEVEL_LOCAL_APP_ERR_05                131
#define DUMMY_CONSOLE_ERR_01                       146
#define DUMMY_CONSOLE_ERR_02                       147
#define DUMMY_CONSOLE_ERR_03                       148
#define DUMMY_TELNET_ERR_01                        150
#define DUMMY_TELNET_ERR_02                        151
#define DUMMY_TELNET_ERR_03                        152
#define DUMMY_TELNET_ERR_04                        153
#define DUMMY_TELNET_ERR_05                        154

#define HIGH_LEVEL_LOCAL_APP_ERR_01                224
#define D_APP_ACTIVATE_GET_01                      225
#define HIGH_LEVEL_LOCAL_APP_ERR_04                235


#define CHAR_Q_INTERNAL_ERR                        315
#define GET_APPLICATION_VERION_FROM_ZIP_ERR_01     320
#define D_TASKRESTART_ERR_01                       327
#define D_TASKRESTART_ERR_02                       328
#define D_TASKRESTART_ERR_03                       329
#define D_TASKRESTART_ERR_04                       330
#define D_TASKRESUME_ERR_01                        331
#define D_TASKRESUME_ERR_02                        332
#define D_TASKRESUME_ERR_04                        334
#define D_TASKSUSPEND_ERR_01                       335
#define D_TASKSUSPEND_ERR_02                       336
#define D_TASKSUSPEND_ERR_03                       337
#define D_TASKSUSPEND_ERR_04                       338

#define SWEEP_PETRA_APP_INIT_ERR                   352

#define HIGH_LEVEL_LOCAL_APP_ERR_02                396
#define HIGH_LEVEL_LOCAL_APP_ERR_03                397
#define INIT_HIGH_LEVEL_CHECK_CRATE_ERR_01         398
#define INIT_HIGH_LEVEL_CHECK_CRATE_ERR_02         399
#define INIT_HIGH_LEVEL_CHECK_CRATE_ERR_03         400

#define DFFS_DESCRIPTOR_CRC_ERR                      410
#define DFFS_FILE_CRC_ERR                            411


#define DCL_SKT_ERR_BASE               600

enum {
  DCL_SKT_ERR_01 = DCL_SKT_ERR_BASE,
  DCL_SKT_ERR_02,
  DCL_SKT_ERR_03,
  DCL_SKT_ERR_04,
  DCL_SKT_ERR_05,
  DCL_SKT_ERR_06,
  DCL_SKT_ERR_07,
  DCL_SKT_ERR_08,
  DCL_SKT_ERR_09,

  DCL_SKT_ERR_LAST
} ;

/*
 * }
 */
/*
 * Definitions related to 'gen_err'.
 * {
 */
/*
 * Definitions related to 'severity' parameter of
 * 'gen_err'.
 */
#define SVR_MSG 0
#define SVR_WRN 1
#define SVR_ERR 2
#define SVR_FTL 3

/* Highest possible value for severity -
   Used in the syslog mechanism as a flag
   that disables remote logging. */
#define SVR_LAST 255
/*
 * Debug purposes only. See gen_err.
 */
#define SVR_DBG_ONLY BIT(8)
/*
 * Semaphore handling for RUNTIME event log file protection.
 * {
 *
 * Semaphore is taken every time RUNTIME event log is accessed and
 * released when access has ended.
 * Use mutual exclusion semaphore rather since we want the
 * 'recursive' option.
 *
 * For all utilities: Returning a non-zero value indicates error.
 *
 * To carry out direct RUNTIME event log access, use 'ignore_rtlog_sem()'
 */
unsigned
  int
    get_use_rt_log_sem(
      void
    ) ;
void
  set_use_rt_log_sem(
    unsigned int val
  ) ;
int
  init_rt_log_sem(
    void
  ) ;
int
  take_rt_log_sem(
    void
  ) ;
int
  give_back_rt_log_sem(
    void
  ) ;
/*
 * }
 */
/*
 * Definitions related to 'pcket injection' test of FAP.
 * {
 */
/*
 * Prototypes
 * {
 */
int
  line_card_packet_inject_test(
    unsigned int action,
    unsigned int num_cycles_to_do,
    unsigned int trace_print,
    unsigned int *num_cycles_done,
    unsigned int *manual_break,
    unsigned int *fail_indication,
    unsigned long *pattern,
    unsigned int  pattern_type,
    unsigned int  packet_size
  ) ;
/*
 * }
 */
/*
 * }
 */

#if !DUNE_BCM
#if !(defined(LINUX) || defined(UNIX))
#endif
#endif
/* { */

/*
 * Definitions related to 'runtime event log':
 *   Events that are temporarily stored in a RAM
 *   area and are periodically flushed into NVRAM in
 *   a background process.
 *   It is assumed, at this point, that all definitions
 *   related to event log in NVRAM are already
 *   known (from eeprom.h)
 *   Note that Runtime_event_log_table is packed.
 *   This is just to make surethat the event_log_item
 *   array, contained in it, is packed (Altjough it
 *   is so defined, we just make sure that this
 *   definition is not changed by the encapsulating
 *   structure).
 * {
 */
/*
 * Number of elements in Runtime_event_log_table.
 */

#define NUM_RUNTIME_EVENT_LOG_ELEMENTS   (NUM_EVENT_LOG_ITEMS * 2)
typedef struct
{
    /*
     * Index in array 'Runtime_event_log_table' of next item
     * to load.
     */
  unsigned int next_in_index ;
    /*
     * Number of meaningful events logged in array
     * 'Runtime_event_log_table'. Events are stored
     * cyclicly.
     * For example, if 'NUM_RUNTIME_EVENT_LOG_ELEMENTS'
     * is 20, 'next_in_index' is 5 and 'num_events_logged'
     * is 7 then the index of the first meaningful
     * entry is 18.
     */
  unsigned int num_events_logged ;
    /*
     * Index in array 'Runtime_event_log_table' of next item
     * to flush into NVRAM.
     */
  unsigned int next_flush_index ;
    /*
     * Number of meaningful events logged in array
     * 'Runtime_event_log_table' which still need to be
     * flushed into NVRAM. Events are stored
     * cyclicly, as explained above.
     */
  unsigned int num_events_to_flush ;
    /*
     * Array of flags, each corresponding to one event, which
     * indicate whether this event should go into NVRAM.
     */
  unsigned char  store_in_nv[NUM_RUNTIME_EVENT_LOG_ELEMENTS] ;
    /*
     * Array of events pushed into this log file.
     */
  EVENT_LOG_ITEM event_log_item[NUM_RUNTIME_EVENT_LOG_ELEMENTS] ;
} __ATTRIBUTE_PACKED__ RUNTIME_EVENT_LOG_TABLE  ;
/*
 * Flush RUNTIME to NVRAM log period:
 * Seconds and nanoseconds.
 * Make it 50 milliseconds.
 */
/*
 * Prototypes:
 * {
 */
unsigned
  int
    is_runtime_event_log_initialized(
    void
  ) ;
RUNTIME_EVENT_LOG_TABLE
  *get_runtime_event_log_ptr(
    void
  ) ;
int
  init_runtime_event_log(
    void
  ) ;
int
  display_runtime_log_block(
    void
  ) ;
int
  clear_runtime_log_block(
    void
  ) ;
int
  next_event_to_runtime(
    EVENT_LOG_ITEM  *event_item_ptr,
    unsigned char   store_in_nv
  ) ;
int
  next_runtime_to_nv_log(
    unsigned int *empty_ptr
  ) ;
/*
 * }
 */
/*
 * }
 */
/*
 * Definitions related to 'active events':
 *   Events that are temporarily stored and used for alarm
 *   handling and for handling of event storms.
 * {
 */
/*
 * Structure to use as basic unit of 'active_event'
 * object, which contains events which were reported
 * during the very near past. (shock absorber for
 * repeated events).
 */
typedef struct active_event_str
{
    /*
     * Full identifier of the event
     */
  int event_id ;
    /*
     * Severity of event.
     * Values
     *   SVR_MSG 0
     *   SVR_WRN 1
     *   SVR_ERR 2
     *   SVR_FTL 4
     */
  int severity ;
    /*
     * Time from startup, in units of 10 milliseconds,
     * when event was last displayed/stored.
     */
  unsigned long displayed_time ;
    /*
     * Number of times event was encountered since it
     * was last displayed.
     */
  unsigned short num_accumulated ;
    /*
     * Minimal time, in units of 10 ms, between two display/store
     * actions on this event.
     */
  unsigned short event_period ;
    /*
     * Text to display when scanning of events (for aging) finds
     * an event which has non-zero num_accumulated element and
     * which is still pending.
     */
  char event_text[80] ;
    /*
     * Flags: bit map. Specify whether this event is an
     * alarm [BIT(0) set].
     * Values:
     *   SET_ALARM       BIT(0)
     * Also conains field specifying type of alarm (which
     * may be an indication for the led to set) stored
     * in ALARM_TYPE_MASK (second LS byte)
     */
  unsigned int alarm_flags ;
    /*
     * Flag. Set to non-zero value when this entry is loaded and
     * back to zero when this entry is aged out.
     */
  unsigned char pending ;
    /*
     * Name of task at which event was detected.
     */
  char task_name[10] ;
    /*
     * Name of procedure at which event was detected.
     */
  char proc_name[16] ;
    /*
     * Error reported by OS when event was first
     * reported.
     */
  int  vx_works_error ;
    /*
     * Flag indicating whether event should be stored in NVRAM
     * log file.
     */
  unsigned int store_in_nv ;
} ACTIVE_EVENT ;
/*
 * Bit indicating the number of active event entries.
 * The number of active events is BIT(ACTIVE_EVENT_BIT)
 */
#define ACTIVE_EVENT_BIT            11
/*
 * Bit mask to use as hash to active_event_table. When
 * anded with event_id, this gives the index of the
 * corresponding entry.
 */
#define ACTIVE_EVENT_MASK           UPTO_BIT( ACTIVE_EVENT_BIT )
/*
 * Number of elements in active_event_table.
 */
#define NUM_ACTIVE_EVENT_ELEMENTS   ( ACTIVE_EVENT_MASK + 1 )
/*
 * Number of elements in active_event_table to scan
 * every pass in background task.
 * Make it a small number since task switching is
 * disabled while aging!
 */
#define NUM_ACTIVE_EVENT_TO_AGE     ( NUM_ACTIVE_EVENT_ELEMENTS / 20 )


#if (NUM_ACTIVE_EVENT_TO_AGE == 0)
  #define NUM_ACTIVE_EVENT_TO_AGE     1
#endif


/*
 * Active events aging period: Seconds and nanoseconds.
 * Make it 100 milliseconds.
 */
/*
 * Definitions related to 'load_active_entry'
 */
/*
 * Entry was loaded as required
 */
/*
 * Entry was found occupied by another event
 */
/*
 * Entry was stored with one alarm attribute and it
 * is attempted to update it with another alarm attribute.
 */
/*
 * Entry was found to have timed out (requires display)
 */
int
  load_active_entry(
          int            event_id,
          int            severity,
          unsigned short event_period,
    const char          *event_text,
          unsigned int   alarm_flags,
    const char          *task_name,
    const char          *proc_name,
          unsigned int   store_in_nv,
          int            vx_works_error,
          unsigned int   dbg_inject
  ) ;
/*
 * Alarm_flags: Bits and fields
 */
#define SET_ALARM         BIT(0)
#define RESET_ALARM       BIT(1)
#define ALARM_EVENT       (SET_ALARM | RESET_ALARM)
#define ALARM_TYPE_MASK   0x0000FF00
/*
 * Minimal time, in units of 10 milliseconds, between two
 * display/store actions of a specific event (related to gen_err).
 */
/*
 * Minimal severity, above which events are stored in
 * NV ram (related to gen_err).
 */
int
  get_store_in_nv(
    int event_id
  ) ;
void
  age_active_entries(
    int            num_entries
  ) ;
int
  print_active_entry(
    int            event_id,
    unsigned int   alarm_flag
  ) ;
int
  active_entry_to_event_log(
    int             event_id,
    EVENT_LOG_ITEM  *event_log_item
  ) ;
void
  init_repeat_period(
    void
  ) ;
void
  set_repeat_period(
    unsigned short val
  ) ;
unsigned
  short
    get_repeat_period(
      void
    ) ;
/*
 * }
 */

#if !DUNE_BCM
#endif /*} LINUX */

char
  *severity_to_text(
    int severity
  ) ;
void
  err_gen_err(
    int            event_id,
    int            severity,
    char           *event_text
  ) ;
void
  print_gen_err(
    char *text,
    int  severity
  ) ;
void
  init_gen_err(
    void
  ) ;
int
  gen_err(
        int compare,
        int reverse_logic,
        int status,
        int reference,
    const char *msg,
    const char *proc_name,
        int severity,
        int err_id,
        unsigned int store_in_nv,
        unsigned int alarm_flags,
        short min_time_between_events,
        unsigned int send_trap
  ) ;
int
  gen_err_msg(
    const char *msg,
    const char *proc_name,
          unsigned int store_in_nv,
          unsigned int alarm_flags,
          short min_time_between_events,
          unsigned int send_trap
  );

/*
 * }
 */
void
  proc_print(
    char *msg,
    char *proc_name,
    unsigned int num_pars,
    int par1,
    int par2
  ) ;
void
  set_bits(
    unsigned char *src_ptr,
    unsigned long bit_offset,
    unsigned long num_bits
    ) ;
int
  test_bits(
    unsigned char *src_ptr,
    unsigned long bit_offset,
    unsigned long num_bits
    ) ;
int
  fill_water_marks(
    unsigned char *buffer,
    unsigned long buffer_len
  ) ;
int
  get_water_size(
    unsigned char *buffer,
    unsigned long buffer_len,
    unsigned long *estimated_size
  ) ;
void
  init_utils(
    void
    ) ;
void
  init_software_exception_module(
    void
  ) ;
void
   init_watchdog_module(
        void
   ) ;
int
  first_nz_bit(
    unsigned char in_byte
  ) ;
int
  one_nz_bits(
    unsigned char in_byte
  ) ;
int
  num_nz_bits(
    unsigned char in_byte
  ) ;
int
  test_task_delay_and_resume(
    void
  );
/*
 * }
 */
#if (SIMULATE_DUNE_CHIP || SIMULATE_CHIP_SIM)
/* { */
extern
  FE_REGS
    Fe_regs[] ;
extern
  FAP10M_REGS
    Fap10m_regs[] ;
/* } */
#endif
/*
 * Object: Buff_mem
 * This object enables the user to store 'mem read' results
 * in local memory to later be retriebed. This feature enables
 * faster working with an external script agent (like proComm).
 * Storage is always in units of char (one byte).
 * {
 */
extern
  unsigned
    char
      Buff_mem[] ;
/*
 * }
 */

#ifdef  __cplusplus
}
#endif

/* } */
#endif
